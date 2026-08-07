param(
 [Parameter(Mandatory=$true)][string]$Qemu,
 [Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,
 [Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-firmware.ppm',[int]$MonitorPort=45468)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$logPath=[IO.Path]::GetFullPath((Join-Path $root $DebugLog));$shotPath=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach($path in @($logPath,$shotPath)){if(Test-Path $path){Remove-Item $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$DebugLog",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{
 $deadline=[DateTime]::UtcNow.AddSeconds(60)
 while([DateTime]::UtcNow-lt$deadline){
  if(Test-Path $logPath){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue;if($content-like'*UEFI:MOTION-READY*'){break}}
  if($process.HasExited){throw 'QEMU wurde vor dem Firmwaretest beendet.'};Start-Sleep -Milliseconds 250}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{
  $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($command in @('sendkey down','sendkey down','sendkey ret','sendkey down','sendkey down',
                         'sendkey down','sendkey down','sendkey ret')){
   $writer.WriteLine($command);Start-Sleep -Milliseconds 750}
  $deadline=[DateTime]::UtcNow.AddSeconds(40)
  while([DateTime]::UtcNow-lt$deadline){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   if($content-like'*UEFI:FIRMWARE-VIEW-STABLE*'){break};Start-Sleep -Milliseconds 200}
  if($content-notlike'*UEFI:FIRMWARE-VIEW-STABLE*'){throw 'Firmwarestatusseite wurde nicht stabil gezeichnet.'}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(15);$last='';$same=0
  while($same-lt 4-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 300
   if(!(Test-Path $shotPath)){continue};$hash=(Get-FileHash $shotPath -Algorithm SHA256).Hash
   if($hash-eq$last){$same++}else{$last=$hash;$same=0}}
  if($same-lt 4){throw 'Firmware-Screenshot wurde nicht stabil geschrieben.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 200
  foreach($command in @('sendkey down','sendkey down','sendkey down','sendkey down','sendkey ret')){
   $writer.WriteLine($command);Start-Sleep -Milliseconds 750}
  $deadline=[DateTime]::UtcNow.AddSeconds(30)
  while([DateTime]::UtcNow-lt$deadline){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   if($content-like'*UEFI:FIRMWARE-SETUP-SUPPORTED*'){
    if($content-like'*UEFI:FIRMWARE-SETUP-CONFIRM*'){break}
   }elseif($content-like'*UEFI:FIRMWARE-SETUP-UNAVAILABLE*' -and $content-like'*UEFI:DIALOG-OPEN*'){break}
   Start-Sleep -Milliseconds 200}
  if($content-like'*UEFI:FIRMWARE-SETUP-SUPPORTED*'){
   if($content-notlike'*UEFI:FIRMWARE-SETUP-CONFIRM*'){throw 'Setup-Unterstützung führte nicht zur Bestätigung.'}
   foreach($command in @('sendkey down','sendkey ret','sendkey ret')){$writer.WriteLine($command);Start-Sleep -Milliseconds 850}
   $deadline=[DateTime]::UtcNow.AddSeconds(30)
   while([DateTime]::UtcNow-lt$deadline){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
    if($content-like'*UEFI:FIRMWARE-SETUP-REQUESTED*'){return};Start-Sleep -Milliseconds 200}
   throw 'Der bestätigte BootToFwUi-Neustart wurde nicht angefordert.'
  }
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
