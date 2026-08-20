param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$FatDirectory,
    [Parameter(Mandatory=$true)][string]$DebugLog,
    [string]$Screenshot='build/uefi-confirmation.ppm',
    [int]$MonitorPort=45479
)
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$shot=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach($path in @($log,$shot)){if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$log",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{
 $deadline=[DateTime]::UtcNow.AddSeconds(60)
 do{Start-Sleep -Milliseconds 200;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}
    if($process.HasExited){throw 'QEMU wurde vor dem Confirmation-Test beendet.'}}
 while($content -notlike '*UEFI:MOTION-READY*' -and [DateTime]::UtcNow -lt $deadline)
 if($content -notlike '*UEFI:MOTION-READY*'){throw 'Bootmanager wurde nicht bereit.'}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  1..5|ForEach-Object{$writer.WriteLine('sendkey down');Start-Sleep -Milliseconds 130}
  $writer.WriteLine('sendkey ret');Start-Sleep -Milliseconds 900
  $writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw}
  while(($content -notlike '*UEFI:CONFIRMATION-DIALOG-FRAME-READY*' -or
         $content -notlike '*UEFI:CONFIRMATION-SAFE-DEFAULT*' -or
         $content -notlike '*UEFI:MODAL-BACKDROP-FRAME*' -or
         $content -notlike '*UEFI:DIALOG-STABLE*') -and [DateTime]::UtcNow -lt $deadline)
  foreach($marker in @('UEFI:CONFIRMATION-DIALOG-FRAME-READY','UEFI:CONFIRMATION-SAFE-DEFAULT','UEFI:MODAL-BACKDROP-FRAME','UEFI:DIALOG-STABLE')){
   if($content -notlike "*$marker*"){throw "Confirmation-Nachweis fehlt: $marker"}}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(10);$previous=-1
  do{Start-Sleep -Milliseconds 200;$length=if(Test-Path $shot){(Get-Item $shot).Length}else{0}
     $stable=$length -gt 0 -and $length -eq $previous;$previous=$length}
  while(!$stable -and [DateTime]::UtcNow -lt $deadline)
  if(!(Test-Path $shot)){throw 'Kein Confirmation-Screenshot erzeugt.'}
  $hashDeadline=[DateTime]::UtcNow.AddSeconds(12);$lastHash='';$stableHashes=0
  while($stableHashes -lt 4 -and [DateTime]::UtcNow -lt $hashDeadline){
   Start-Sleep -Milliseconds 300
   $hash=(Get-FileHash -LiteralPath $shot -Algorithm SHA256).Hash
   if($hash -eq $lastHash){$stableHashes++}else{$lastHash=$hash;$stableHashes=0}
  }
  if($stableHashes -lt 4){throw 'Confirmation-Screenshot wurde nicht stabil fertiggeschrieben.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 150;$writer.WriteLine('sendkey esc')
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $log -Raw}
  while($content -notlike '*UEFI:DIALOG-RESULT-CANCEL*' -and [DateTime]::UtcNow -lt $deadline)
  if($content -notlike '*UEFI:DIALOG-RESULT-CANCEL*'){throw 'Escape lieferte kein sicheres Cancel-Ergebnis.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
