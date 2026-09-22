param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$Image
)

$ErrorActionPreference='Stop'
$buildRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
$tempDir=[IO.Path]::Combine($buildRoot,'nova-uefi-boot-control-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($tempDir)|Out-Null
$firmwareCopy=[IO.Path]::Combine($tempDir,'firmware.fd')
[IO.File]::Copy([IO.Path]::GetFullPath($Firmware),$firmwareCopy,$true)
$imagePath=[IO.Path]::GetFullPath($Image)
$completed=$false

function Invoke-BootControlBoot([string]$name,[string]$requiredMarker,
                                [string]$kernelMarker='UEFI:NKI-VALIDATED',
                                [string]$modeMarker='') {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $serial=[IO.Path]::Combine($tempDir,$name+'.serial.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M',
        '-drive',"if=pflash,format=raw,file=$firmwareCopy",
        '-drive',"format=raw,file=$imagePath,if=ide",'-display','none','-monitor','none',
        '-serial',"file:$serial",'-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            $debugContent=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
            $serialContent=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
            $content=$debugContent+$serialContent
            if($process.HasExited){
                $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
                throw "${name}: QEMU wurde vor der Boot-Control-Pruefung beendet. $detail"
            }
        } while($content-notlike'*NOVA_KERNEL_READY*'-and[DateTime]::UtcNow-lt$deadline)
        $markers=@($requiredMarker,'UEFI:BOOT-CONTROL-READY',$kernelMarker,
                   'UEFI:KERNEL-HANDOFF-READY','NOVA_KERNEL_READY')
        if($modeMarker){$markers+=$modeMarker}
        foreach($marker in $markers){
            if($content-notlike"*$marker*"){throw "${name}: erwartete Markierung fehlt: $marker"}
        }
    } finally {
        if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
        $process.WaitForExit()
        $process.Dispose()
    }
}

function Corrupt-BootState([bool]$allCopies) {
    $bytes=$null
    for($attempt=0;$attempt-lt20-and$null-eq$bytes;$attempt++) {
        try {$bytes=[IO.File]::ReadAllBytes($firmwareCopy)}
        catch [IO.IOException] {
            if($attempt-eq19){throw}
            Start-Sleep -Milliseconds 100
        }
    }
    $magic=[Text.Encoding]::ASCII.GetBytes('NOVABCTL')
    $records=@()
    for($offset=0;$offset-le$bytes.Length-64;$offset++) {
        $matches=$true
        for($index=0;$index-lt$magic.Length;$index++) {
            if($bytes[$offset+$index]-ne$magic[$index]){$matches=$false;break}
        }
        if(!$matches){continue}
        $version=[BitConverter]::ToUInt16($bytes,$offset+8)
        $size=[BitConverter]::ToUInt16($bytes,$offset+10)
        if($version-eq1-and$size-eq64){
            $records+=,[pscustomobject]@{Offset=$offset;Sequence=[BitConverter]::ToUInt64($bytes,$offset+12)}
        }
    }
    if($records.Count-lt2){throw "Boot-Control-Fehlerinjektion fand nur $($records.Count) Datensatz/Datensaetze."}
    if($allCopies){
        foreach($record in $records){$bytes[$record.Offset+56]=$bytes[$record.Offset+56]-bxor0xa5}
    } else {
        $newest=$records|Sort-Object Sequence -Descending|Select-Object -First 1
        $bytes[$newest.Offset+56]=$bytes[$newest.Offset+56]-bxor0x5a
    }
    [IO.File]::WriteAllBytes($firmwareCopy,$bytes)
}

try {
    Invoke-BootControlBoot 'initial' 'UEFI:BOOT-CONTROL-INITIALIZED'
    Corrupt-BootState $false
    Invoke-BootControlBoot 'restored' 'UEFI:BOOT-CONTROL-INVALID-COPY-IGNORED'
    $restored=[string](Get-Content -LiteralPath ([IO.Path]::Combine($tempDir,'restored.debug.log')) -Raw)
    if($restored-notlike'*UEFI:BOOT-CONTROL-RESTORED*'){
        throw 'Die ältere gültige Boot-Control-Kopie wurde nicht wiederhergestellt.'
    }
    Corrupt-BootState $true
    Invoke-BootControlBoot 'metadata-recovery' 'UEFI:BOOT-CONTROL-CORRUPT-RECOVERY' `
        'UEFI:RECOVERY-NKI-VALIDATED' 'UEFI:AUTOMATIC-RECOVERY-SELECTED'
    $recoverySerial=[string](Get-Content -LiteralPath ([IO.Path]::Combine($tempDir,'metadata-recovery.serial.log')) -Raw)
    if($recoverySerial-notlike'*NOVA: Recovery-Modus aus NBHP/BIB aktiv*'){
        throw 'Der durch beschädigte Boot-Metadaten ausgelöste Recovery-Modus fehlt im NBHP/BIB.'
    }
    Write-Host 'UEFI Boot-Control verwarf eine beschädigte Kopie und wechselte bei zwei beschädigten Kopien sicher zu RECOVERY.NKI'
    $completed=$true
} finally {
    $resolved=[IO.Path]::GetFullPath($tempDir)
    if($completed-and$resolved.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)-and[IO.Directory]::Exists($resolved)){
        for($attempt=0;$attempt-lt20-and[IO.Directory]::Exists($resolved);$attempt++){
            try{[IO.Directory]::Delete($resolved,$true)}
            catch [IO.IOException]{if($attempt-eq19){throw};Start-Sleep -Milliseconds 100}
        }
    } elseif(!$completed) {
        Write-Host "Boot-Control-Testartefakte bleiben zur Diagnose erhalten: $resolved"
    }
}
