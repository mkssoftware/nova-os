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

function Invoke-BootControlBoot([string]$name,[string]$requiredMarker) {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M',
        '-drive',"if=pflash,format=raw,file=$firmwareCopy",
        '-drive',"format=raw,file=$imagePath,if=ide",'-display','none','-monitor','none',
        '-serial','none','-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            if(Test-Path -LiteralPath $debug){$content=Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue}
            if($process.HasExited){
                $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
                throw "${name}: QEMU wurde vor der Boot-Control-Pruefung beendet. $detail"
            }
        } while($content-notlike'*NOVA_KERNEL_READY*'-and[DateTime]::UtcNow-lt$deadline)
        foreach($marker in @($requiredMarker,'UEFI:BOOT-CONTROL-READY','UEFI:NKI-VALIDATED',
                             'UEFI:KERNEL-HANDOFF-READY','NOVA_KERNEL_READY')){
            if($content-notlike"*$marker*"){throw "${name}: erwartete Markierung fehlt: $marker"}
        }
    } finally {
        if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
        $process.Dispose()
    }
}

try {
    Invoke-BootControlBoot 'initial' 'UEFI:BOOT-CONTROL-INITIALIZED'
    Invoke-BootControlBoot 'restored' 'UEFI:BOOT-CONTROL-RESTORED'
    Write-Host 'UEFI Boot-Control wurde redundant gespeichert, wiederhergestellt und erreichte bei beiden Starts NOVA_KERNEL_READY'
} finally {
    $resolved=[IO.Path]::GetFullPath($tempDir)
    if($resolved.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)-and[IO.Directory]::Exists($resolved)){
        [IO.Directory]::Delete($resolved,$true)
    }
}
