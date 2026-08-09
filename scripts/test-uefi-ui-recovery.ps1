param(
 [Parameter(Mandatory=$true)][string]$Qemu,
 [Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,
 [Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-ui-recovery.ppm',
 [string]$MemoryScreenshot='build/uefi-memory-self-test.ppm',
 [string]$RuntimeScreenshot='build/uefi-runtime-lifecycle.ppm',
 [int]$MonitorPort=45475
)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog));$shot=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
$memoryShot=[IO.Path]::GetFullPath((Join-Path $root $MemoryScreenshot))
$runtimeShot=[IO.Path]::GetFullPath((Join-Path $root $RuntimeScreenshot))
foreach($path in @($log,$shot,$memoryShot,$runtimeShot)){if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$DebugLog",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{
 $deadline=[DateTime]::UtcNow.AddSeconds(60)
 do{Start-Sleep -Milliseconds 250;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}
    if($process.HasExited){throw 'QEMU wurde vor dem Recovery-Test beendet.'}}
 while($content-notlike'*UEFI:MOTION-READY*'-and[DateTime]::UtcNow-lt$deadline)
 if($content-notlike'*UEFI:RECOVERY-MANAGER-READY*'){throw 'Recovery Manager wurde nicht initialisiert.'}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  $writer.WriteLine('sendkey home')
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content $log -Raw -ErrorAction SilentlyContinue}
  while($content-notlike'*UEFI:COUNTDOWN-CANCELLED*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:COUNTDOWN-CANCELLED*'){throw 'Countdown wurde vor der Testnavigation nicht beendet.'}
  foreach($command in @('sendkey down','sendkey down','sendkey down')){
   $writer.WriteLine($command);Start-Sleep -Milliseconds 850}
  $writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content $log -Raw -ErrorAction SilentlyContinue}
  while($content-notlike'*UEFI:NAV-ENTER-COMPLETE*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:NAV-ENTER-COMPLETE*'){throw 'Diagnose-Navigation wurde nicht abgeschlossen.'}
  $writer.WriteLine('sendkey f6')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content $log -Raw -ErrorAction SilentlyContinue}
  while($content-notlike'*UEFI:RUNTIME-LIFECYCLE-SELF-TEST-FRAME*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:RUNTIME-LIFECYCLE-SELF-TEST*'){throw 'Runtime-Lifecycle-Selbsttest wurde nicht erreicht.'}
  if($content-notlike'*UEFI:RUNTIME-LIFECYCLE-SELF-TEST-FRAME*'){throw 'Runtime-Lifecycle-Statusframe wurde nicht vollstaendig gezeichnet.'}
  if($content-notlike'*UEFI:STATE-MODEL-SELF-TEST*'){throw 'Hierarchischer State-Model-Selbsttest wurde nicht erreicht.'}
  if($content-notlike'*UEFI:STATE-MODEL-SELF-TEST-FRAME*'){throw 'State-Model-Statusframe wurde nicht vollstaendig gezeichnet.'}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $RuntimeScreenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  while(!(Test-Path $runtimeShot)-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 250}
  if(!(Test-Path $runtimeShot)){throw 'Runtime-Lifecycle-Screenshot fehlt.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 250
  $writer.WriteLine('sendkey f8')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content $log -Raw -ErrorAction SilentlyContinue}
  while($content-notlike'*UEFI:MEMORY-SELF-TEST-FRAME*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:MEMORY-SELF-TEST*'){throw 'Memory-Selbsttest wurde nicht erreicht.'}
  if($content-notlike'*UEFI:MEMORY-SELF-TEST-FRAME*'){throw 'Memory-Statusframe wurde nicht vollständig gezeichnet.'}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $MemoryScreenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  while(!(Test-Path $memoryShot)-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 250}
  if(!(Test-Path $memoryShot)){throw 'Memory-Screenshot fehlt.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 250;$writer.WriteLine('sendkey f10')
  $deadline=[DateTime]::UtcNow.AddSeconds(30)
  do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content $log -Raw -ErrorAction SilentlyContinue}
  while($content-notlike'*UEFI:RECOVERY-FRAME-READY*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:RECOVERY-SAFE-MODE*'){throw 'Safe-Mode-Recovery wurde nicht erreicht.'}
  if($content-notlike'*UEFI:RECOVERY-FRAME-READY*'){throw 'Recovery-Statusframe wurde nicht vollständig gezeichnet.'}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  while(!(Test-Path $shot)-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 250}
  if(!(Test-Path $shot)){throw 'Recovery-Screenshot fehlt.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 250;$writer.WriteLine('sendkey f9')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 250;[string]$content=Get-Content $log -Raw -ErrorAction SilentlyContinue}
  while($content-notlike'*UEFI:TEXT-CONTINUE*'-and[DateTime]::UtcNow-lt$deadline)
  if($content-notlike'*UEFI:TEXT-FALLBACK-SELF-TEST*' -or
     $content-notlike'*UEFI:TEXT-FALLBACK*' -or
     $content-notlike'*UEFI:TEXT-CONTINUE*'){
      throw 'Fataler Textfallback setzte den Start nicht deterministisch fort.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
