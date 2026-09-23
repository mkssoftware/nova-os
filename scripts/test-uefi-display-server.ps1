param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$Image
)

$ErrorActionPreference='Stop'
$buildRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
$runDir=[IO.Path]::Combine($buildRoot,'nova-uefi-display-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($runDir)|Out-Null
$serial=[IO.Path]::Combine($runDir,'serial.log')
$debug=[IO.Path]::Combine($runDir,'debug.log')
$stderr=[IO.Path]::Combine($runDir,'stderr.log')
$firmwareCopy=[IO.Path]::Combine($runDir,'firmware.fd')
[IO.File]::Copy([IO.Path]::GetFullPath($Firmware),$firmwareCopy,$true)
$completed=$false

$arguments=@('-machine','q35','-m','256M','-smp','4',
    '-drive',"if=pflash,format=raw,file=$firmwareCopy",
    '-drive',"format=raw,file=$([IO.Path]::GetFullPath($Image)),if=ide",
    '-display','none','-monitor','none','-serial',"file:$serial",
    '-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
    '-no-reboot','-no-shutdown')
$process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru `
    -RedirectStandardError $stderr
try {
    $deadline=[DateTime]::UtcNow.AddSeconds(90)
    $content=''
    do {
        Start-Sleep -Milliseconds 250
        $serialText=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
        $debugText=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
        $content=$debugText+$serialText
        if($process.HasExited){
            $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
            throw "QEMU wurde vor der Display-Server-Pruefung beendet. $detail"
        }
    } while($content-notlike'*NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert*'-and[DateTime]::UtcNow-lt$deadline)

    foreach($marker in @(
        'UEFI:KERNEL-HANDOFF-READY',
        'NOVA: Display Server ABI 1.0, Firmware-Framebuffer uebernommen',
        'NOVA: Userspace Display.QueryPrimary ohne MMIO-Adresse erfolgreich',
        'NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert',
        'NOVA_KERNEL_READY')) {
        if($content-notlike"*$marker*"){throw "Erwartete Markierung fehlt: $marker"}
    }
    Write-Host 'UEFI Display Server: Ring-3-Szene mit Desktop, Startmenue, Ribbon und Taskleiste praesentiert'
    $completed=$true
} finally {
    if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
    $process.WaitForExit()
    $process.Dispose()
    $resolved=[IO.Path]::GetFullPath($runDir)
    if($completed-and$resolved.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)-and[IO.Directory]::Exists($resolved)){
        for($attempt=0;$attempt-lt20-and[IO.Directory]::Exists($resolved);$attempt++){
            try{[IO.Directory]::Delete($resolved,$true)}
            catch [IO.IOException]{if($attempt-eq19){throw};Start-Sleep -Milliseconds 100}
        }
    } elseif(!$completed) {
        Write-Host "Display-Testartefakte bleiben zur Diagnose erhalten: $resolved"
    }
}
