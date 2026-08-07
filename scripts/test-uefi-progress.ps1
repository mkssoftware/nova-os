param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [string]$Screenshot = 'build/uefi-progress.ppm',
    [int]$MonitorPort = 45464
)
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build')); $env:TEMP=$env:TMP
$logPath=[IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$shotPath=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach($path in @($logPath,$shotPath)){if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$DebugLog",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{
 $deadline=[DateTime]::UtcNow.AddSeconds(60)
 while([DateTime]::UtcNow-lt$deadline){
  if(Test-Path $logPath){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue;if($content-like'*UEFI:MOTION-READY*'){break}}
  if($process.HasExited){throw 'QEMU wurde vor dem Progresstest beendet.'};Start-Sleep -Milliseconds 250
 }
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{
  $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($command in @('sendkey down','sendkey down','sendkey down','sendkey ret',
                         'sendkey down','sendkey down','sendkey down','sendkey ret')){
   $writer.WriteLine($command);Start-Sleep -Milliseconds 500
  }
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  while([DateTime]::UtcNow-lt$deadline){
   $content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   $completeIndex=$content.LastIndexOf('UEFI:PROGRESS-COMPLETE')
   $frameIndex=$content.LastIndexOf('UEFI:DIALOG-FRAME-READY')
   if($completeIndex-ge 0 -and $frameIndex-gt$completeIndex){break};Start-Sleep -Milliseconds 200
  }
  if($completeIndex-lt 0 -or $frameIndex-le$completeIndex){throw 'Progressdialog wurde nicht vollständig gezeichnet.'}
  Start-Sleep -Milliseconds 300;$writer.WriteLine('stop');Start-Sleep -Milliseconds 100
  $writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(8)
  while([DateTime]::UtcNow-lt$deadline){if(Test-Path $shotPath){break};Start-Sleep -Milliseconds 100}
  if(!(Test-Path $shotPath)){throw 'Kein Progress-Screenshot erzeugt.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 100;$writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(8)
  while([DateTime]::UtcNow-lt$deadline){
   $content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   if($content-like'*UEFI:DIALOG-RESULT-OK*'){return};Start-Sleep -Milliseconds 100
  }
  throw 'Der abgeschlossene Progressdialog ließ sich nicht schließen.'
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
