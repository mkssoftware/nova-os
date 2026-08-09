param(
 [Parameter(Mandatory=$true)][string]$Qemu,
 [Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,
 [Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-list-controls.ppm',[int]$MonitorPort=45473
)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
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
  if(Test-Path -LiteralPath $logPath){$content=Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
   if($content-like'*UEFI:MOTION-READY*'){break}}
  if($process.HasExited){throw 'QEMU wurde vor dem List-Test beendet.'};Start-Sleep -Milliseconds 250
 }
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($key in @('end','home','pgdn','pgup','down','spc')){$writer.WriteLine("sendkey $key");Start-Sleep -Milliseconds 700}
  $deadline=[DateTime]::UtcNow.AddSeconds(35)
  while([DateTime]::UtcNow-lt$deadline){$content=Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
   if($content-like'*UEFI:LIST-HOME-END*' -and $content-like'*UEFI:LIST-PAGE-NAVIGATION*' -and
      $content-like'*UEFI:INSTALL-UNAVAILABLE*' -and $content-like'*UEFI:DIALOG-STABLE*'){break}
   Start-Sleep -Milliseconds 200}
  if([DateTime]::UtcNow-ge$deadline){throw 'List-Navigation oder Space-Aktivierung blieb unvollständig.'}
  $writer.WriteLine("screendump $Screenshot");$deadline=[DateTime]::UtcNow.AddSeconds(12);$last='';$stable=0
  while($stable-lt 4-and[DateTime]::UtcNow-lt$deadline){Start-Sleep -Milliseconds 250
   if(!(Test-Path -LiteralPath $shotPath)){continue};$hash=(Get-FileHash $shotPath -Algorithm SHA256).Hash
   if($hash-eq$last){$stable++}else{$last=$hash;$stable=0}}
  if($stable-lt 4){throw 'List-Control-Screenshot wurde nicht stabil geschrieben.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
