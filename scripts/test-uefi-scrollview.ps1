param(
 [Parameter(Mandatory=$true)][string]$Qemu,[Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,[Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-scrollview.ppm',[int]$MonitorPort=45470)
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
 do{Start-Sleep -Milliseconds 200;$content=if(Test-Path $logPath){Get-Content $logPath -Raw}else{''}
  if($process.HasExited){throw 'QEMU wurde vor dem ScrollView-Test beendet.'}}
 while($content-notlike'*UEFI:MOTION-READY*'-and[DateTime]::UtcNow-lt$deadline)
 $deadline=[DateTime]::UtcNow.AddSeconds(30)
 do{Start-Sleep -Milliseconds 150;[string]$content=if(Test-Path $logPath){Get-Content $logPath -Raw}else{''}}
 while($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'-and[DateTime]::UtcNow-lt$deadline)
 if($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'){throw 'Countdown-Frame wurde nicht bereit.'}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  $writer.WriteLine('sendkey down')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw
     $cancel=$content.LastIndexOf('UEFI:COUNTDOWN-CANCELLED');$menu=$content.LastIndexOf('UEFI:MENU-DRAWN')}
  while(($cancel-lt 0-or$menu-le$cancel)-and[DateTime]::UtcNow-lt$deadline)
  if($cancel-lt 0-or$menu-le$cancel){throw 'Countdown-Abbruch wurde nicht stabil gezeichnet.'}
  foreach($command in @('sendkey down','sendkey down')){
   $before=([regex]::Matches($content,'UEFI:MENU-DRAWN')).Count;$writer.WriteLine($command)
   $deadline=[DateTime]::UtcNow.AddSeconds(15)
   do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw
      $after=([regex]::Matches($content,'UEFI:MENU-DRAWN')).Count}
   while($after-le$before-and[DateTime]::UtcNow-lt$deadline)
   if($after-le$before){throw 'Auswahlnavigation erzeugte keinen neuen Frame.'}}
  $writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw}
  while($content-notlike'*UEFI:DIAGNOSTICS*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:DIAGNOSTICS*'){throw 'Diagnose-ScrollView wurde nicht geöffnet.'}
  $writer.WriteLine('sendkey pgdn')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw
     $key=$content.LastIndexOf('UEFI:SCROLL-KEY');$frame=$content.LastIndexOf('UEFI:MENU-DRAWN')}
  while(($key-lt 0-or$frame-le$key)-and[DateTime]::UtcNow-lt$deadline)
  if($key-lt 0-or$frame-le$key){throw 'Page Down erzeugte keinen stabilen gescrollten Frame.'}
  Start-Sleep -Milliseconds 300;$writer.WriteLine('stop');Start-Sleep -Milliseconds 150
  $writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(12);$previous=-1;$stable=0
  while($stable-lt 3-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 150
   $length=if(Test-Path $shotPath){(Get-Item $shotPath).Length}else{0}
   if($length-gt 0-and$length-eq$previous){$stable++}else{$stable=0};$previous=$length}
  if($stable-lt 3){throw 'Kein stabiler ScrollView-Screenshot erzeugt.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 150
  foreach($command in @('sendkey end','sendkey home')){
   [string]$content=Get-Content $logPath -Raw;$before=([regex]::Matches($content,'UEFI:SCROLL-KEY')).Count
   $writer.WriteLine($command);$deadline=[DateTime]::UtcNow.AddSeconds(15)
   do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw
      $after=([regex]::Matches($content,'UEFI:SCROLL-KEY')).Count}
   while($after-le$before-and[DateTime]::UtcNow-lt$deadline)
   if($after-le$before){throw "$command wurde nicht verarbeitet."}}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
