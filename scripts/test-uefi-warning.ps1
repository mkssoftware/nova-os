param(
 [Parameter(Mandatory=$true)][string]$Qemu,
 [Parameter(Mandatory=$true)][string]$Firmware,
 [Parameter(Mandatory=$true)][string]$FatDirectory,
 [Parameter(Mandatory=$true)][string]$DebugLog,
 [string]$Screenshot='build/uefi-warning.ppm',[int]$MonitorPort=45480)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog));$shot=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach($path in @($log,$shot)){if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$log",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{$deadline=[DateTime]::UtcNow.AddSeconds(60)
 do{Start-Sleep -Milliseconds 200;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}
    if($process.HasExited){throw 'QEMU wurde vor dem Warning-Test beendet.'}}
 while($content -notlike '*UEFI:MOTION-READY*' -and [DateTime]::UtcNow -lt $deadline)
 if($content -notlike '*UEFI:MOTION-READY*'){throw 'Bootmanager wurde nicht bereit.'}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  1..4|ForEach-Object{$writer.WriteLine('sendkey down');Start-Sleep -Milliseconds 130}
  $writer.WriteLine('sendkey ret');Start-Sleep -Milliseconds 900;$writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw}
  while(($content -notlike '*UEFI:WARNING-DIALOG-OPEN*' -or
         $content -notlike '*UEFI:WARNING-DIALOG-FRAME-READY*' -or
         $content -notlike '*UEFI:MODAL-BACKDROP-FRAME*' -or
         $content -notlike '*UEFI:DIALOG-STABLE*') -and [DateTime]::UtcNow -lt $deadline)
  foreach($marker in @('UEFI:WARNING-DIALOG-OPEN','UEFI:WARNING-DIALOG-FRAME-READY',
    'UEFI:MODAL-BACKDROP-FRAME','UEFI:DIALOG-STABLE')){
   if($content -notlike "*$marker*"){throw "Warning-Nachweis fehlt: $marker"}}
  Start-Sleep -Milliseconds 600
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 200;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(10);$lastHash='';$stable=0
  while($stable -lt 4 -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 300
   if(Test-Path $shot){$stream=[IO.File]::OpenRead($shot)
    try{$sha=[Security.Cryptography.SHA256]::Create()
     try{$hash=[BitConverter]::ToString($sha.ComputeHash($stream))}finally{$sha.Dispose()}}
    finally{$stream.Dispose()}
    if($hash -eq $lastHash){$stable++}else{$lastHash=$hash;$stable=0}}}
  if($stable -lt 4){throw 'Warning-Screenshot wurde nicht stabil fertiggeschrieben.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 150;$writer.WriteLine('sendkey esc')
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw}
  while($content -notlike '*UEFI:DIALOG-RESULT-CANCEL*' -and [DateTime]::UtcNow -lt $deadline)
  if($content -notlike '*UEFI:DIALOG-RESULT-CANCEL*'){throw 'Warning lieferte kein Cancel-Ergebnis.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
