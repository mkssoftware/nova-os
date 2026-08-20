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
try{$deadline=[DateTime]::UtcNow.AddSeconds(90)
 do{Start-Sleep -Milliseconds 250;[string]$content=if(Test-Path $log){Get-Content $log -Raw}else{''}
    if($process.HasExited){$detail=if(Test-Path $stderr){Get-Content $stderr -Raw}else{''};throw "QEMU beendete den UEFI-IMG-Boot vorzeitig. $detail"}}
 while($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'-and[DateTime]::UtcNow-lt$deadline)
 if($content-notlike'*UEFI:NOVA-ENTRY*'){throw 'Das UEFI-IMG startete BOOTX64.EFI nicht.'}
 if($content-notlike'*UEFI:DIAGNOSTICS-FRAMEWORK-READY*'){throw 'Das strukturierte Diagnoseframework wurde nicht validiert.'}
 if($content-notlike'*UEFI:RENDERING-STATISTICS-READY*'){throw 'Die Rendering-Statistiken wurden nicht validiert.'}
 if($content-notlike'*UEFI:INPUT-EVENT-TRACING-READY*'){throw 'Das passive Input Event Tracing wurde nicht validiert.'}
 if($content-notlike'*UEFI:ANIMATION-DIAGNOSTICS-READY*'){throw 'Die passive Animationsdiagnose wurde nicht validiert.'}
 if($content-notlike'*UEFI:RESOURCE-LOADING-DIAGNOSTICS-READY*'){throw 'Die passive Ressourcenladediagnose wurde nicht validiert.'}
 if($content-notlike'*UEFI:PERFORMANCE-REGRESSION-READY*'){throw 'Die Performance-Regressionspruefung wurde nicht validiert.'}
 if($content-notlike'*UEFI:RESOURCE-CORRUPTION-TESTS-READY*'){throw 'Die isolierten Ressourcenkorruptionstests wurden nicht validiert.'}
 if($content-notlike'*UEFI:FALLBACK-MODE-TESTS-READY*'){throw 'Die gemeinsamen Fallbackmodustests wurden nicht validiert.'}
 if($content-notlike'*UEFI:RESOLUTION-COMPATIBILITY-READY*'){throw 'Die Resolution-Kompatibilitaetsmatrix wurde nicht validiert.'}
 if($content-notlike'*UEFI:RENDER-REFERENCE-TESTS-READY*'){throw 'Die isolierten Rendering-Referenztests wurden nicht validiert.'}
 if($content-notlike'*UEFI:BOOT-TEST-ARCHITECTURE-READY*'){throw 'Die zentrale Boot-UI-Testarchitektur wurde nicht validiert.'}
 if($content-notlike'*UEFI:PASSWORD-FIELD-READY*'){throw 'Das sichere Password Field wurde nicht validiert.'}
 if($content-notlike'*UEFI:CONTROL-EVENT-ROUTING-READY*'){throw 'Das hierarchische Control-Ereignissystem wurde nicht validiert.'}
 if($content-notlike'*UEFI:DOUBLE-CLICK-DETECTION-READY*'){throw 'Die Doppelklickerkennung wurde nicht validiert.'}
 if($content-notlike'*UEFI:POINTER-SPEED-READY*'){throw 'Die konfigurierbare Pointergeschwindigkeit wurde nicht validiert.'}
 if($content-notlike'*UEFI:POINTER-CAPTURE-READY*'){throw 'Der Pointer-Capture-Lebenszyklus wurde nicht validiert.'}
 if($content-notlike'*UEFI:POINTER-CAPTURE-CANCEL-EVENT-READY*'){throw 'Capture-Cancel-Routing und Diagnosedauer wurden nicht validiert.'}
 if($content-notlike'*UEFI:DIALOG-CAPTURE-CANCEL-READY*'){throw 'Dialogwechsel beendet Pointer Capture nicht korrekt.'}
 if($content-notlike'*UEFI:RECOVERY-CAPTURE-CANCEL-READY*'){throw 'Recovery beendet Pointer Capture nicht korrekt.'}
 if($content-notlike'*UEFI:DIALOG-TESTS-READY*'){throw 'Die isolierte Dialogtestsuite wurde nicht validiert.'}
 if($content-notlike'*UEFI:NAVIGATION-TESTS-READY*'){throw 'Die isolierte Navigationstestsuite wurde nicht validiert.'}
 if($content-notlike'*UEFI:CONTROL-INTERACTION-TESTS-READY*'){throw 'Die isolierte Control-Interaction-Suite wurde nicht validiert.'}
 if($content-notlike'*UEFI:BASE-SURFACE-CACHE-HIT*'-or$content-notlike'*UEFI:PARTIAL-INTERACTION-FRAME*'){throw 'Die optimierte Teilflächen-GUI wurde nicht ausgeführt.'}
 if($content-notlike'*UEFI:COUNTDOWN-FRAME-READY*'){throw 'Die aktuelle Bootmanager-GUI wurde aus dem UEFI-IMG nicht vollständig gezeichnet.'}
}finally{if(!$process.HasExited){Stop-Process $process.Id -Force};$process.Dispose()}
