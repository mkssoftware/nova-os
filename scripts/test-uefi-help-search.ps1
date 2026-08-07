param(
 [Parameter(Mandatory=$true)][string]$Qemu,[Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,[Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-help-search.ppm',[int]$MonitorPort=45469)
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
 while([DateTime]::UtcNow-lt$deadline){if(Test-Path $logPath){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
  if($content-like'*UEFI:MOTION-READY*'){break}};if($process.HasExited){throw 'QEMU wurde vor dem Hilfetest beendet.'};Start-Sleep -Milliseconds 250}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{
  $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($command in @('sendkey f1','sendkey ret','sendkey d','sendkey i','sendkey a','sendkey g','sendkey g',
                         'sendkey backspace','sendkey home','sendkey delete','sendkey d','sendkey end','sendkey ret')){
   $writer.WriteLine($command);Start-Sleep -Milliseconds 700}
  $deadline=[DateTime]::UtcNow.AddSeconds(45)
  while([DateTime]::UtcNow-lt$deadline){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   $complete=$content.LastIndexOf('UEFI:HELP-SEARCH-COMPLETE');$stable=$content.LastIndexOf('UEFI:HELP-SEARCH-STABLE')
   if($complete-ge 0-and$stable-gt$complete){break};Start-Sleep -Milliseconds 200}
  if($complete-lt 0-or$stable-le$complete){throw 'Hilfesuche erreichte keinen stabilen Ergebniszustand.'}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(15);$last='';$same=0
  while($same-lt 4-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 300
   if(!(Test-Path $shotPath)){continue};$hash=(Get-FileHash $shotPath -Algorithm SHA256).Hash
   if($hash-eq$last){$same++}else{$last=$hash;$same=0}}
  if($same-lt 4){throw 'Hilfesuch-Screenshot wurde nicht stabil geschrieben.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 150;$writer.WriteLine('sendkey down');Start-Sleep -Milliseconds 600
  $writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(35)
  while([DateTime]::UtcNow-lt$deadline){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   if($content-like'*UEFI:HELP-RESULT-OPEN*' -and $content-like'*UEFI:DIALOG-STABLE*'){return};Start-Sleep -Milliseconds 200}
  throw 'Der gefilterte Hilfeeintrag wurde nicht geöffnet.'
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
