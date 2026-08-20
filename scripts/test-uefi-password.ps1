param([Parameter(Mandatory=$true)][string]$Qemu,
      [Parameter(Mandatory=$true)][string]$Firmware,
      [Parameter(Mandatory=$true)][string]$FatDirectory,
      [Parameter(Mandatory=$true)][string]$DebugLog,
      [string]$Screenshot='build/uefi-password.ppm',[int]$MonitorPort=45479)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog));$shot=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach($path in @($log,$shot)){if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$DebugLog",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{$deadline=[DateTime]::UtcNow.AddSeconds(70)
 do{Start-Sleep -Milliseconds 200;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}
    if($process.HasExited){throw 'QEMU wurde vor dem Password-Test beendet.'}}
 while($content -notlike '*UEFI:MOTION-READY*' -and [DateTime]::UtcNow -lt $deadline)
 if($content -notlike '*UEFI:MOTION-READY*'){throw 'Bootmanager wurde nicht rechtzeitig bereit.'}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{$writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($key in @('down','down','down','ret','f12','n','o','v','a','7')){
   $writer.WriteLine("sendkey $key");Start-Sleep -Milliseconds 800}
  $deadline=[DateTime]::UtcNow.AddSeconds(25)
  do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content $log -Raw}
  while(($content -notlike '*UEFI:PASSWORD-FIELD-DIALOG-OPEN*' -or
         ([regex]::Matches($content,'UEFI:PASSWORD-FIELD-INPUT')).Count -lt 5 -or
         $content -notlike '*UEFI:PASSWORD-FIELD-FRAME-READY*') -and [DateTime]::UtcNow -lt $deadline)
  if($content -notlike '*UEFI:PASSWORD-FIELD-FRAME-READY*' -or
     ([regex]::Matches($content,'UEFI:PASSWORD-FIELD-INPUT')).Count -lt 5){
      throw 'Password Field wurde nicht vollstaendig eingegeben und gezeichnet.'}
  Start-Sleep -Milliseconds 500
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $shot")
  $deadline=[DateTime]::UtcNow.AddSeconds(10)
  while(!(Test-Path $shot) -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 150}
  if(!(Test-Path $shot)){throw 'Kein Password-Field-Screenshot erzeugt.'}
  $hashDeadline=[DateTime]::UtcNow.AddSeconds(12);$lastHash='';$stableHashes=0
  while($stableHashes -lt 4 -and [DateTime]::UtcNow -lt $hashDeadline){
   Start-Sleep -Milliseconds 300
   $hash=(Get-FileHash -LiteralPath $shot -Algorithm SHA256).Hash
   if($hash -eq $lastHash){$stableHashes++}else{$lastHash=$hash;$stableHashes=0}}
  if($stableHashes -lt 4){throw 'Der Password-Field-Screenshot wurde nicht stabil fertiggeschrieben.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
