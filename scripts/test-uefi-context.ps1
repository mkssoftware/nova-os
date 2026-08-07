param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [string]$Screenshot = 'build/uefi-context.ppm',
    [int]$MonitorPort = 45467
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
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    do {
        Start-Sleep -Milliseconds 200
        [string]$content = if(Test-Path -LiteralPath $logPath){Get-Content -LiteralPath $logPath -Raw}else{''}
        if($process.HasExited){throw 'QEMU wurde vor dem Context-Menu-Test beendet.'}
    } while($content -notlike '*UEFI:MOTION-READY*' -and [DateTime]::UtcNow -lt $deadline)
    if($content -notlike '*UEFI:MOTION-READY*'){throw 'UEFI-Bootmanager wurde nicht rechtzeitig bereit.'}
    $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try {
        $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
        $writer.WriteLine('sendkey f2')
        $deadline=[DateTime]::UtcNow.AddSeconds(10)
        do {
            Start-Sleep -Milliseconds 150
            [string]$content=Get-Content -LiteralPath $logPath -Raw
        } while($content -notlike '*UEFI:CONTEXT-FRAME-READY*' -and [DateTime]::UtcNow -lt $deadline)
        if($content -notlike '*UEFI:CONTEXT-FRAME-READY*'){throw 'Context Menu wurde nicht gezeichnet.'}
        Start-Sleep -Milliseconds 500
        $writer.WriteLine('stop');Start-Sleep -Milliseconds 100
        $writer.WriteLine("screendump $Screenshot")
        $deadline=[DateTime]::UtcNow.AddSeconds(8)
        $previousLength=-1
        do {
            Start-Sleep -Milliseconds 150
            $length=if(Test-Path -LiteralPath $shotPath){(Get-Item -LiteralPath $shotPath).Length}else{0}
            $stable=$length -gt 0 -and $length -eq $previousLength
            $previousLength=$length
        } while(!$stable -and [DateTime]::UtcNow -lt $deadline)
        if(!(Test-Path -LiteralPath $shotPath)){throw 'Kein Context-Menu-Screenshot erzeugt.'}
        $writer.WriteLine('cont');Start-Sleep -Milliseconds 100
        $writer.WriteLine('sendkey down');Start-Sleep -Milliseconds 200
        $writer.WriteLine('sendkey ret')
        $deadline=[DateTime]::UtcNow.AddSeconds(10)
        do {
            Start-Sleep -Milliseconds 150
            [string]$content=Get-Content -LiteralPath $logPath -Raw
        } while(($content -notlike '*UEFI:CONTEXT-ADVANCED*' -or
                 $content -notlike '*UEFI:DIALOG-FRAME-READY*') -and
                [DateTime]::UtcNow -lt $deadline)
        if($content -notlike '*UEFI:CONTEXT-ADVANCED*' -or $content -notlike '*UEFI:DIALOG-FRAME-READY*'){
            throw 'Context-Menu-Aktion hat den sicheren Hinweisdialog nicht geöffnet.'
        }
    } finally {$client.Dispose()}
} finally {
    if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
    $process.Dispose()
}
