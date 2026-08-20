param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [string]$DebugLog = 'build/qemu-uefi-pointer-qmp-debug.log',
    [int]$QmpPort = 45465
)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$logPath = [IO.Path]::GetFullPath((Join-Path $root $DebugLog))
if (Test-Path -LiteralPath $logPath) { Remove-Item -LiteralPath $logPath -Force }
$args = @(
    '-machine','q35','-m','256M',
    '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
    '-drive',"format=raw,file=fat:rw:$FatDirectory",
    '-device','qemu-xhci','-device','usb-mouse',
    '-display','none','-serial','none',
    '-qmp',"tcp:127.0.0.1:$QmpPort,server=on,wait=off",
    '-debugcon',"file:$logPath",'-global','isa-debugcon.iobase=0xe9',
    '-no-reboot','-no-shutdown'
)
$process = Start-Process -FilePath $Qemu -ArgumentList $args -WorkingDirectory $root -WindowStyle Hidden -PassThru
try {
    $until = [DateTime]::UtcNow.AddSeconds(90)
    while ([DateTime]::UtcNow -lt $until) {
        $content = if(Test-Path -LiteralPath $logPath) { Get-Content -LiteralPath $logPath -Raw } else { '' }
        if($content -like '*UEFI:RUNTIME-RUNNING*') { break }
        if($process.HasExited) { throw 'QEMU wurde vor der UEFI-Runtime beendet.' }
        Start-Sleep -Milliseconds 100
    }
    if($content -notlike '*UEFI:RUNTIME-RUNNING*') { throw 'Timeout beim UEFI-Start.' }
    $client = [Net.Sockets.TcpClient]::new('127.0.0.1',$QmpPort)
    try {
        $stream = $client.GetStream(); $reader = [IO.StreamReader]::new($stream)
        $writer = [IO.StreamWriter]::new($stream)
        $writer.AutoFlush = $true
        $greeting=$reader.ReadLine()
        if($greeting -notlike '*"QMP"*') { throw "Unerwartete QMP-Begrüßung: $greeting" }
        $writer.WriteLine('{"execute":"qmp_capabilities"}')
        $capabilities=$reader.ReadLine()
        if($capabilities -notlike '*"return"*') { throw "QMP-Capabilities abgelehnt: $capabilities" }
        $writer.WriteLine('{"execute":"query-mice"}')
        $mice=$reader.ReadLine()
        Write-Output "QMP-MICE: $mice"
        $writer.WriteLine('{"execute":"input-send-event","arguments":{"events":[{"type":"rel","data":{"axis":"x","value":80}},{"type":"rel","data":{"axis":"y","value":40}}]}}')
        $motion=$reader.ReadLine()
        if($motion -notlike '*"return"*') { throw "QMP-PS/2-Bewegung abgelehnt: $motion" }
        Start-Sleep -Milliseconds 500
        $writer.WriteLine('{"execute":"input-send-event","arguments":{"events":[{"type":"btn","data":{"button":"left","down":true}}]}}')
        $down=$reader.ReadLine()
        if($down -notlike '*"return"*') { throw "QMP-Maustaste abgelehnt: $down" }
        Start-Sleep -Milliseconds 100
        $writer.WriteLine('{"execute":"input-send-event","arguments":{"events":[{"type":"btn","data":{"button":"left","down":false}}]}}')
        $up=$reader.ReadLine()
        if($up -notlike '*"return"*') { throw "QMP-Maustaste abgelehnt: $up" }
    } finally { $client.Dispose() }
    $until = [DateTime]::UtcNow.AddSeconds(5)
    while([DateTime]::UtcNow -lt $until) {
        $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
        if($content -like '*UEFI:RELATIVE-POINTER-EVENT*') { break }
        Start-Sleep -Milliseconds 100
    }
    if($content -notlike '*UEFI:RELATIVE-POINTER-EVENT*') { throw 'QMP-PS/2-Ereignis wurde von UEFI nicht empfangen.' }
} finally {
    if(!$process.HasExited) { Stop-Process -Id $process.Id -Force }
    $process.Dispose()
}
