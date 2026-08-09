param([Parameter(Mandatory=$true)][string]$Qemu,
      [Parameter(Mandatory=$true)][string]$Firmware,
      [Parameter(Mandatory=$true)][string]$Image,
      [Parameter(Mandatory=$true)][string]$DebugLog)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$processPath=$env:Path
[Environment]::SetEnvironmentVariable('PATH',$null,'Process')
[Environment]::SetEnvironmentVariable('Path',$processPath,'Process')
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog));if(Test-Path $log){Remove-Item $log -Force}
$firmwarePath=[IO.Path]::GetFullPath((Join-Path $root $Firmware));$imagePath=[IO.Path]::GetFullPath((Join-Path $root $Image))
$stderr=[IO.Path]::GetFullPath((Join-Path $root 'build/qemu-uefi-image-stderr.log'));if(Test-Path $stderr){Remove-Item $stderr -Force}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$firmwarePath",
 '-drive',"format=raw,file=$imagePath",'-display','none','-serial','none','-debugcon',"file:$log",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
try{$deadline=[DateTime]::UtcNow.AddSeconds(60)
 do{Start-Sleep -Milliseconds 250;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}
    if($process.HasExited){$detail=if(Test-Path $stderr){Get-Content $stderr -Raw}else{''};throw "QEMU beendete den UEFI-IMG-Boot vorzeitig. $detail"}}
 while($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'-and[DateTime]::UtcNow-lt$deadline)
 if($content-notlike'*UEFI:NOVA-ENTRY*'){throw 'Das UEFI-IMG startete BOOTX64.EFI nicht.'}
 if($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'){throw 'Die aktuelle Bootmanager-GUI wurde aus dem UEFI-IMG nicht vollständig gezeichnet.'}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
