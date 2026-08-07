param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [Parameter(Mandatory = $true)][string]$Screenshot,
    [int]$MonitorPort = 45462
)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$env:TMP = [IO.Path]::GetFullPath((Join-Path $root 'build'))
$env:TEMP = $env:TMP
$logPath = [IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$shotPath = [IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach ($path in @($logPath,$shotPath)) {
    if (Test-Path -LiteralPath $path) { Remove-Item -LiteralPath $path -Force }
}
$arguments = @(
    '-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
    '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
    '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",
    '-debugcon',"file:$DebugLog",'-global','isa-debugcon.iobase=0xe9',
    '-no-reboot','-no-shutdown'
)
$process = Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try {
    $deadline = [DateTime]::UtcNow.AddSeconds(70)
    while ([DateTime]::UtcNow -lt $deadline) {
        if (Test-Path -LiteralPath $logPath) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if ($content -like '*UEFI:GOP-PREFERRED-READY*' -and
                $content -like '*UEFI:LAYOUT-READY*' -and
                $content -like '*UEFI:COUNTDOWN-FRAME-READY*') { break }
            if ($content -like '*UEFI:GOP-PREFERRED-UNAVAILABLE*') {
                throw 'Der angeforderte GOP-Modus ist in QEMU nicht verfügbar.'
            }
        }
        if ($process.HasExited) { throw 'QEMU wurde vor dem Resolution-Frame beendet.' }
        Start-Sleep -Milliseconds 250
    }
    if ([DateTime]::UtcNow -ge $deadline) { throw 'Timeout beim Warten auf das responsive UEFI-Layout.' }
    $client = [Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try {
        $writer = [IO.StreamWriter]::new($client.GetStream()); $writer.AutoFlush = $true
        # Pause immediately after the completely composed frame so the
        # screenshot cannot catch the following redraw half-finished.
        Start-Sleep -Milliseconds 300
        $writer.WriteLine('stop')
        Start-Sleep -Milliseconds 100
        $writer.WriteLine("screendump $Screenshot")
        $deadline = [DateTime]::UtcNow.AddSeconds(8)
        while ([DateTime]::UtcNow -lt $deadline) {
            if (Test-Path -LiteralPath $shotPath) { return }
            Start-Sleep -Milliseconds 100
        }
        throw 'QEMU hat keinen Resolution-Framebuffer erzeugt.'
    } finally { $client.Dispose() }
} finally {
    if (!$process.HasExited) { Stop-Process -Id $process.Id -Force }
    $process.Dispose()
}
