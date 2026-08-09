param([Parameter(Mandatory=$true)][string]$Qemu,[Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,[Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-recovery-tiles.ppm',[int]$MonitorPort=45471)
$ErrorActionPreference='Stop';$root=(Get-Location).Path;$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog));$shot=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach($p in @($log,$shot)){if(Test-Path $p){Remove-Item $p -Force}}
$args=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",'-drive',"format=raw,file=fat:rw:$FatDirectory",
 '-display','none','-serial','none','-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$DebugLog",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process $Qemu -ArgumentList $args -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{$deadline=[DateTime]::UtcNow.AddSeconds(60)
 do{Start-Sleep -Milliseconds 200;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}}
 while($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'-and!$process.HasExited-and[DateTime]::UtcNow-lt$deadline)
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($command in @('sendkey down','sendkey down','sendkey down','sendkey down','sendkey ret')){
   $before=([regex]::Matches($content,'UEFI:MENU-DRAWN')).Count;$writer.WriteLine($command);$deadline=[DateTime]::UtcNow.AddSeconds(20)
   do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw;$after=([regex]::Matches($content,'UEFI:MENU-DRAWN')).Count}
   while($after-le$before-and[DateTime]::UtcNow-lt$deadline)
   if($after-le$before){throw "Kein stabiler Frame nach $command."}}
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw}
  while(($content-notlike'*UEFI:RECOVERY*'-or$content-notlike'*UEFI:NAV-ENTER-COMPLETE*')-and[DateTime]::UtcNow-lt$deadline)
  foreach($command in @('sendkey right','sendkey down')){
   [string]$content=Get-Content $log -Raw;$before=([regex]::Matches($content,'UEFI:TILE-NAVIGATION')).Count
   $writer.WriteLine($command);$deadline=[DateTime]::UtcNow.AddSeconds(20)
   do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw
      $after=([regex]::Matches($content,'UEFI:TILE-NAVIGATION')).Count;$key=$content.LastIndexOf('UEFI:TILE-NAVIGATION');$frame=$content.LastIndexOf('UEFI:MENU-DRAWN')}
   while(($after-le$before-or$frame-le$key)-and[DateTime]::UtcNow-lt$deadline)
   if($after-le$before-or$frame-le$key){throw "$command erzeugte keinen stabilen Tile-Frame."}}
  [string]$content=Get-Content $log -Raw
  if(([regex]::Matches($content,'UEFI:TILE-NAVIGATION')).Count-lt2){throw 'Zweidimensionale Tile-Navigation fehlgeschlagen.'}
  $key=$content.LastIndexOf('UEFI:TILE-NAVIGATION');$frame=$content.LastIndexOf('UEFI:MENU-DRAWN')
  if($frame-le$key){Start-Sleep -Milliseconds 1000}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(12);$previous=-1;$stable=0
  while($stable-lt3-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 150;$length=if(Test-Path $shot){(Get-Item $shot).Length}else{0}
   if($length-gt0-and$length-eq$previous){$stable++}else{$stable=0};$previous=$length}
  if($stable-lt3){throw 'Kein stabiler Tile-Screenshot.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 150;$writer.WriteLine('sendkey spc')
  $deadline=[DateTime]::UtcNow.AddSeconds(25)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw}
  while($content-notlike'*UEFI:DIALOG-STABLE*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:DIALOG-STABLE*'){throw 'Space aktivierte das Recovery-Tile nicht.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
