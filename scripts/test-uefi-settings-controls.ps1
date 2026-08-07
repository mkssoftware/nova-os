param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$FatDirectory,
    [Parameter(Mandatory=$true)][string]$DebugLog,
    [string]$Screenshot='build/uefi-settings-controls.ppm',
    [int]$MonitorPort=45467
)
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
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
  if(Test-Path $logPath){$content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue;if($content-like'*UEFI:MOTION-READY*'){break}}
  if($process.HasExited){throw 'QEMU wurde vor dem Settings-Test beendet.'};Start-Sleep -Milliseconds 250
 }
 $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
 try{
  $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
  foreach($command in @('sendkey down','sendkey down','sendkey ret','sendkey down','sendkey down',
                         'sendkey ret','sendkey down','sendkey right','sendkey right',
                         'sendkey home','sendkey end')){
   $writer.WriteLine($command);Start-Sleep -Milliseconds 750
  }
  $deadline=[DateTime]::UtcNow.AddSeconds(45)
  while([DateTime]::UtcNow-lt$deadline){
   $content=Get-Content $logPath -Raw -ErrorAction SilentlyContinue
   $end=$content.LastIndexOf('UEFI:SETTINGS-SLIDER-END')
   $stable=$content.LastIndexOf('UEFI:SETTINGS-CONTROLS-STABLE')
   if($end-ge 0-and$stable-gt$end){break};Start-Sleep -Milliseconds 200
  }
  if($end-lt 0-or$stable-le$end){throw 'Checkbox und Slider erreichten keinen stabilen Endzustand.'}
  $writer.WriteLine('stop');Start-Sleep -Milliseconds 150;$writer.WriteLine("screendump $Screenshot")
  $deadline=[DateTime]::UtcNow.AddSeconds(15);$lastHash='';$same=0
  while($same-lt 4-and[DateTime]::UtcNow-lt$deadline){
   Start-Sleep -Milliseconds 300
   if(!(Test-Path $shotPath)){continue}
   $hash=(Get-FileHash $shotPath -Algorithm SHA256).Hash
   if($hash-eq$lastHash){$same++}else{$lastHash=$hash;$same=0}
  }
  if($same-lt 4){throw 'Settings-Screenshot wurde nicht stabil geschrieben.'}
 }finally{$client.Dispose()}
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id -Force};$process.Dispose()}
