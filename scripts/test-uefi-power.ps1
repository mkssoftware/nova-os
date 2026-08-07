param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [int]$MonitorPort = 45460
)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$env:TMP = [IO.Path]::GetFullPath((Join-Path $root 'build'))
$env:TEMP = $env:TMP
$logPath = [IO.Path]::GetFullPath((Join-Path $root $DebugLog))
if (Test-Path -LiteralPath $logPath) { Remove-Item -LiteralPath $logPath -Force }
$arguments = @(
    '-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
    '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
    '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",
    '-debugcon',"file:$DebugLog",'-global','isa-debugcon.iobase=0xe9',
    '-no-reboot','-no-shutdown'
)
$process = Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try {
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    while ([DateTime]::UtcNow -lt $deadline) {
        if (Test-Path -LiteralPath $logPath) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if ($content -like '*UEFI:DIAGNOSTICS-READY*') { break }
        }
        if ($process.HasExited) { throw 'QEMU wurde vor der Power-Initialisierung beendet.' }
        Start-Sleep -Milliseconds 250
    }
    if ([DateTime]::UtcNow -ge $deadline) { throw 'Timeout beim Warten auf UEFI Power.' }
    $client = [Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try {
        $writer = [IO.StreamWriter]::new($client.GetStream())
        $writer.AutoFlush = $true
        foreach ($command in @('sendkey down','sendkey down','sendkey down','sendkey down','sendkey down',
                               'sendkey ret','sendkey ret','sendkey down','sendkey ret','sendkey ret')) {
            $writer.WriteLine($command)
            Start-Sleep -Milliseconds 600
        }
        $deadline = [DateTime]::UtcNow.AddSeconds(15)
        while ([DateTime]::UtcNow -lt $deadline) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if ($content -like '*UEFI:POWER-CONFIRM*' -and
                $content -like '*UEFI:DIALOG-DESTRUCTIVE-ARMED*' -and
                $content -like '*UEFI:DIALOG-RESULT-YES*' -and
                $content -like '*UEFI:POWER-SHUTDOWN*') { return }
            Start-Sleep -Milliseconds 200
        }
        throw 'Die bestätigte UEFI-Shutdown-Aktion wurde nicht ausgeführt.'
    } finally { $client.Dispose() }
} finally {
    if (!$process.HasExited) { Stop-Process -Id $process.Id -Force }
    $process.Dispose()
}
