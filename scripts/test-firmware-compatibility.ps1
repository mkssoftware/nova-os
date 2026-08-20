param(
    [Parameter(Mandatory=$true)][string]$BiosDebugLog,
    [Parameter(Mandatory=$true)][string]$BiosSerialLog,
    [Parameter(Mandatory=$true)][string]$UefiDebugLog,
    [Parameter(Mandatory=$true)][string]$OutputReport
)
$ErrorActionPreference='Stop'
foreach($path in @($BiosDebugLog,$BiosSerialLog,$UefiDebugLog)){
    if(!(Test-Path -LiteralPath $path)){throw "Firmware-Testprotokoll fehlt: $path"}
}
$biosDebug=Get-Content -LiteralPath $BiosDebugLog -Raw
$biosSerial=Get-Content -LiteralPath $BiosSerialLog -Raw
$uefiDebug=Get-Content -LiteralPath $UefiDebugLog -Raw
$checks=@(
    [pscustomobject]@{Capability='Firmware entry';Bios=$biosDebug.Contains('BIOS:');Uefi=$uefiDebug.Contains('UEFI:NOVA-ENTRY');Parity=$true;Evidence='SeaBIOS / EDK2 OVMF'},
    [pscustomobject]@{Capability='Graphics framebuffer';Bios=$biosDebug.Contains('BIOS:VBE-BACKEND-READY');Uefi=$uefiDebug.Contains('UEFI:GOP-DESCRIPTOR-READY');Parity=$true;Evidence='VBE / GOP'},
    [pscustomobject]@{Capability='Graphics fallback';Bios=$biosDebug.Contains('BIOS:VBE-MODE-RANK-');Uefi=$uefiDebug.Contains('UEFI:SOFTWARE-RENDERER-READY');Parity=$true;Evidence='VBE mode ranking / software renderer'},
    [pscustomobject]@{Capability='Boot manager UI';Bios=$biosDebug.Contains('BM:START');Uefi=$uefiDebug.Contains('UEFI:COUNTDOWN-FRAME-READY');Parity=$false;Evidence='separate BIOS and UEFI UI pipelines'},
    [pscustomobject]@{Capability='Resources and integrity';Bios=$biosDebug.Contains('BM:START');Uefi=$uefiDebug.Contains('UEFI:RESOURCE-INTEGRITY-READY');Parity=$false;Evidence='UEFI resource manager has no BIOS-C parity'},
    [pscustomobject]@{Capability='Input';Bios=$biosDebug.Contains('BM:START');Uefi=$uefiDebug.Contains('UEFI:INPUT-EVENT-TRACING-READY');Parity=$false;Evidence='BIOS keyboard versus UEFI keyboard/pointer'},
    [pscustomobject]@{Capability='Recovery';Bios=$biosDebug.Contains('BM:SELF-HEALING');Uefi=$uefiDebug.Contains('UEFI:FALLBACK-MODE-TESTS-READY');Parity=$false;Evidence='independent recovery implementations'},
    [pscustomobject]@{Capability='Kernel handoff';Bios=$biosSerial.Contains('NOVA_KERNEL_READY');Uefi=$uefiDebug.Contains('UEFI:KERNEL-HANDOFF-READY');Parity=$false;Evidence='UEFI productive kernel handoff not implemented'},
    [pscustomobject]@{Capability='Secure Boot';Bios=$false;Uefi=$uefiDebug.Contains('UEFI:SECURE-BOOT-READY');Parity=$false;Evidence='optional and not implemented'},
    [pscustomobject]@{Capability='ExitBootServices';Bios=$true;Uefi=$uefiDebug.Contains('UEFI:EXIT-BOOT-SERVICES-READY');Parity=$false;Evidence='not applicable to BIOS; UEFI handoff missing'}
)
$lines=[System.Collections.Generic.List[string]]::new()
$lines.Add('# NovaOS BIOS/UEFI Compatibility Report')
$lines.Add('')
$lines.Add('| Capability | BIOS | UEFI | Identical contract | Evidence |')
$lines.Add('|---|---:|---:|---:|---|')
foreach($check in $checks){
    $lines.Add("| $($check.Capability) | $($check.Bios) | $($check.Uefi) | $($check.Parity -and $check.Bios -and $check.Uefi) | $($check.Evidence) |")
}
$lines.Add('')
$lines.Add('Automated: true')
$lines.Add('Deterministic: true')
$lines.Add('Virtual platforms: SeaBIOS, EDK2/OVMF')
$lines.Add('Physical hardware validated: false')
$lines.Add('Full BIOS/UEFI behavioral parity: false')
$directory=Split-Path -Parent $OutputReport
if($directory){New-Item -ItemType Directory -Force -Path $directory|Out-Null}
[IO.File]::WriteAllLines((Join-Path (Get-Location) $OutputReport),$lines,[Text.UTF8Encoding]::new($false))
if(!($checks[0].Bios -and $checks[0].Uefi -and $checks[1].Bios -and $checks[1].Uefi)){
    throw 'Die gemeinsamen Firmware-Grundvertraege wurden nicht nachgewiesen.'
}
Write-Host "FIRMWARE-COMPATIBILITY-REPORT: $OutputReport"
