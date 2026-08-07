param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$FatDirectory,
    [Parameter(Mandatory=$true)][string]$DebugLog,
    [string]$Screenshot='build/uefi-tooltip.ppm',
    [string]$BreadcrumbScreenshot='build/uefi-breadcrumb.ppm',
    [int]$MonitorPort=45468
)
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$logPath=[IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$shotPath=[IO.Path]::GetFullPath((Join-Path $root $Screenshot))
$breadcrumbPath=[IO.Path]::GetFullPath((Join-Path $root $BreadcrumbScreenshot))
foreach($path in @($logPath,$shotPath,$breadcrumbPath)){if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path -Force}}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$DebugLog",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try{
 $deadline=[DateTime]::UtcNow.AddSeconds(60)
 do{Start-Sleep -Milliseconds 200;[string]$content=if(Test-Path $logPath){Get-Content $logPath -Raw}else{''}
    if($process.HasExited){throw 'QEMU wurde vor dem Tooltip-Test beendet.'}}
 while($content -notlike '*UEFI:MOTION-READY*' -and [DateTime]::UtcNow -lt $deadline)
 if($content -notlike '*UEFI:MOTION-READY*'){throw 'UEFI-Bootmanager wurde nicht rechtzeitig bereit.'}
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{
  $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  $writer.WriteLine('sendkey down')
  $deadline=[DateTime]::UtcNow.AddSeconds(10)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw}
  while($content -notlike '*UEFI:TOOLTIP-FRAME-READY*' -and [DateTime]::UtcNow -lt $deadline)
  if($content -notlike '*UEFI:TOOLTIP-FRAME-READY*'){throw 'Verzögerter Fokus-Tooltip wurde nicht sichtbar.'}
  Start-Sleep -Milliseconds 300;$writer.WriteLine('stop');Start-Sleep -Milliseconds 100
  $writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(8);$previous=-1
  do{Start-Sleep -Milliseconds 150;$length=if(Test-Path $shotPath){(Get-Item $shotPath).Length}else{0};
     $stable=$length -gt 0 -and $length -eq $previous;$previous=$length}
  while(!$stable -and [DateTime]::UtcNow -lt $deadline)
  if(!(Test-Path $shotPath)){throw 'Kein Tooltip-Screenshot erzeugt.'}
  $writer.WriteLine('cont');Start-Sleep -Milliseconds 100
  $writer.WriteLine('sendkey down');Start-Sleep -Milliseconds 150
  $writer.WriteLine('sendkey ret')
  $deadline=[DateTime]::UtcNow.AddSeconds(15)
  do{Start-Sleep -Milliseconds 150;[string]$content=Get-Content $logPath -Raw}
  while(($content -notlike '*UEFI:SETTINGS*' -or $content -notlike '*UEFI:BREADCRUMB-FRAME-READY*' -or
         $content -notlike '*UEFI:NAV-ENTER-COMPLETE*') -and
        [DateTime]::UtcNow -lt $deadline)
  if($content -notlike '*UEFI:BREADCRUMB-FRAME-READY*'){throw 'Breadcrumb wurde auf der Unterseite nicht gezeichnet.'}
  Start-Sleep -Milliseconds 300;$writer.WriteLine('stop');Start-Sleep -Milliseconds 100
  $writer.WriteLine("screendump $BreadcrumbScreenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(8)
  while(!(Test-Path $breadcrumbPath) -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 100}
  if(!(Test-Path $breadcrumbPath)){throw 'Kein Breadcrumb-Screenshot erzeugt.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
