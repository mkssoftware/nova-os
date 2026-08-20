param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [int]$MonitorPort = 45461
)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$env:TMP = [IO.Path]::GetFullPath((Join-Path $root 'build'))
$env:TEMP = $env:TMP
$logPath = [IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$lightPath = Join-Path $root 'build/uefi-theme-light.ppm'
$contrastPath = Join-Path $root 'build/uefi-theme-high-contrast.ppm'
$menuPath = Join-Path $root 'build/uefi-menu-button.ppm'
foreach ($path in @($logPath,$lightPath,$contrastPath,$menuPath)) {
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
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    while ([DateTime]::UtcNow -lt $deadline) {
        if (Test-Path -LiteralPath $logPath) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if ($content -like '*UEFI:DIAGNOSTICS-READY*') { break }
        }
        if ($process.HasExited) { throw 'QEMU wurde vor der Theme-Initialisierung beendet.' }
        Start-Sleep -Milliseconds 250
    }
    if ([DateTime]::UtcNow -ge $deadline) { throw 'Timeout beim Warten auf UEFI Themes.' }
    $client = [Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try {
        $writer = [IO.StreamWriter]::new($client.GetStream()); $writer.AutoFlush = $true
        function Send-Key([string]$key) { $writer.WriteLine("sendkey $key"); Start-Sleep -Milliseconds 650 }
        function Wait-Marker([string]$marker,[int]$timeoutSeconds=45) {
            $until = [DateTime]::UtcNow.AddSeconds($timeoutSeconds)
            while ([DateTime]::UtcNow -lt $until) {
                $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
                if ($content -like "*$marker*") { return }
                Start-Sleep -Milliseconds 150
            }
            throw "Marker $marker fehlt."
        }
        Send-Key 'down'; Send-Key 'down'; Send-Key 'ret'; Wait-Marker 'UEFI:SETTINGS'
        # SETTINGS is emitted when navigation begins. Input remains locked until
        # the final transition frame, so synchronize before opening the menu.
        Wait-Marker 'UEFI:NAV-ENTER-COMPLETE'
        Send-Key 'ret'; Wait-Marker 'UEFI:MENU-BUTTON-OPEN'
        Wait-Marker 'UEFI:MENU-BUTTON-FRAME-READY'
        $writer.WriteLine('screendump build/uefi-menu-button.ppm')
        $shotDeadline=[DateTime]::UtcNow.AddSeconds(12);$lastHash='';$stable=0
        while($stable-lt 4-and[DateTime]::UtcNow-lt$shotDeadline){
            Start-Sleep -Milliseconds 250
            if(!(Test-Path -LiteralPath $menuPath)){continue}
            $hash=(Get-FileHash -LiteralPath $menuPath -Algorithm SHA256).Hash
            if($hash-eq$lastHash){$stable++}else{$lastHash=$hash;$stable=0}
        }
        if($stable-lt 4){throw 'Offener Menu-Button-Frame wurde nicht stabil geschrieben.'}
        Send-Key 'down'; Send-Key 'ret'; Wait-Marker 'UEFI:THEME-LIGHT'; Start-Sleep -Milliseconds 1000
        $writer.WriteLine('screendump build/uefi-theme-light.ppm'); Start-Sleep -Milliseconds 800
        Send-Key 'ret'; Send-Key 'down'; Send-Key 'ret'; Wait-Marker 'UEFI:THEME-HIGH-CONTRAST'; Start-Sleep -Milliseconds 1000
        $writer.WriteLine('screendump build/uefi-theme-high-contrast.ppm'); Start-Sleep -Milliseconds 800
        Send-Key 'ret'; Send-Key 'down'; Send-Key 'down'; Send-Key 'ret'; Wait-Marker 'UEFI:THEME-DARK'
        # The fourth presentation entry opens the six-profile AQM submenu.
        Send-Key 'ret'; Send-Key 'down'; Send-Key 'down'; Send-Key 'down'; Send-Key 'ret'
        Wait-Marker 'UEFI:QUALITY-MENU-OPEN'
        # Default is Balanced (index 3); move to Safe (index 5) and persist it.
        Send-Key 'down'; Send-Key 'down'; Send-Key 'ret'; Wait-Marker 'UEFI:QUALITY-SELECTION'
        Wait-Marker 'UEFI:CONFIGURATION-COMMIT'
        Send-Key 'down'; Send-Key 'ret'; Wait-Marker 'UEFI:REDUCED-MOTION-ON'
        Wait-Marker 'UEFI:SETTINGS-SWITCH-UPDATED'
        Send-Key 'esc'; Wait-Marker 'UEFI:NAV-REDUCED-FADE'
        if (!(Test-Path -LiteralPath $lightPath) -or !(Test-Path -LiteralPath $contrastPath) -or
            !(Test-Path -LiteralPath $menuPath)) {
            throw 'QEMU hat die Theme-Referenzframes nicht erzeugt.'
        }
    } finally { $client.Dispose() }
} finally {
    if (!$process.HasExited) { Stop-Process -Id $process.Id -Force }
    $process.Dispose()
}
