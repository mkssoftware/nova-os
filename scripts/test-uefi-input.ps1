param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [string]$Screenshot = 'build/uefi-help.ppm',
    [int]$MonitorPort = 45458
)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$env:TMP = [IO.Path]::GetFullPath((Join-Path $root 'build'))
$env:TEMP = $env:TMP
$logPath = [IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$screenshotPath = [IO.Path]::GetFullPath((Join-Path $root $Screenshot))
if (Test-Path -LiteralPath $logPath) { Remove-Item -LiteralPath $logPath -Force }
if (Test-Path -LiteralPath $screenshotPath) { Remove-Item -LiteralPath $screenshotPath -Force }
$arguments = @(
    '-machine','q35',
    '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
    '-drive',"format=raw,file=fat:rw:$FatDirectory",
    '-display','none','-serial','none',
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
            if ($content -like '*UEFI:MOTION-READY*') { break }
        }
        if ($process.HasExited) { throw 'QEMU wurde vor der Input-Initialisierung beendet.' }
        Start-Sleep -Milliseconds 250
    }
    if ([DateTime]::UtcNow -ge $deadline) { throw 'Timeout beim Warten auf UEFI:MOTION-READY.' }
    $client = [Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try {
        $writer = [IO.StreamWriter]::new($client.GetStream())
        $writer.AutoFlush = $true
        $commands = @(
            'sendkey down','sendkey ret','sendkey ret',
            'sendkey down','sendkey ret','sendkey esc',
            'sendkey down','sendkey ret','sendkey esc',
            'sendkey down','sendkey ret','sendkey esc',
            'sendkey down','sendkey ret',
            'sendkey down','sendkey down','sendkey ret','sendkey esc',
            'sendkey esc','sendkey f1'
        )
        foreach ($command in $commands) {
            $writer.WriteLine($command)
            Start-Sleep -Milliseconds 500
        }
        $deadline = [DateTime]::UtcNow.AddSeconds(60)
        while ([DateTime]::UtcNow -lt $deadline) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if ($content -like '*UEFI:INSTALL-UNAVAILABLE*' -and
                $content -like '*UEFI:SETTINGS*' -and
                $content -like '*UEFI:DIAGNOSTICS-VIEW*' -and
                $content -like '*UEFI:RECOVERY-VIEW*' -and
                $content -like '*UEFI:POWER-VIEW*' -and
                $content -like '*UEFI:POWER-TARGET-UNAVAILABLE*' -and
                $content -like '*UEFI:DIALOG-OPEN*' -and
                $content -like '*UEFI:NAV-RESTORED*' -and
                $content -like '*UEFI:NAV-SLIDE-LEFT*' -and
                $content -like '*UEFI:NAV-SLIDE-RIGHT*' -and
                $content -like '*UEFI:NAV-RECOVERY-FADE*' -and
                $content -like '*UEFI:NAV-TRANSFORMED-FRAME*' -and
                $content -like '*UEFI:HELP-VIEW*') {
                Start-Sleep -Milliseconds 1500
                $writer.WriteLine("screendump $Screenshot")
                $shotDeadline = [DateTime]::UtcNow.AddSeconds(5)
                while ([DateTime]::UtcNow -lt $shotDeadline) {
                    if (Test-Path -LiteralPath $screenshotPath) { return }
                    Start-Sleep -Milliseconds 100
                }
                throw 'QEMU hat die visuelle UEFI-Referenz nicht erzeugt.'
            }
            Start-Sleep -Milliseconds 200
        }
        throw 'Nicht alle erwarteten UEFI-Unteransichten wurden erreicht.'
    } finally { $client.Dispose() }
} finally {
    if (!$process.HasExited) { Stop-Process -Id $process.Id -Force }
    $process.Dispose()
}
