param(
    [Parameter(Mandatory = $true)] [string]$ElfFile,
    [Parameter(Mandatory = $true)] [string]$NkiFile
)

$ErrorActionPreference = 'Stop'
$elf = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $ElfFile))
$nki = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $NkiFile))
function U16([byte[]]$Data, [int]$Offset) { [BitConverter]::ToUInt16($Data, $Offset) }
function U32([byte[]]$Data, [int]$Offset) { [BitConverter]::ToUInt32($Data, $Offset) }

if ($elf.Length -lt 52 -or (U32 $elf 0) -ne 0x464C457F -or $elf[4] -ne 1 -or
    (U16 $elf 16) -ne 2 -or (U16 $elf 18) -ne 3) {
    throw 'ELF32-Header oder Zielarchitektur ist ungueltig.'
}

$buildId = $null
$phoff = U32 $elf 28
$phsize = U16 $elf 42
$phnum = U16 $elf 44
for ($i = 0; $i -lt $phnum; $i++) {
    $ph = $phoff + $i * $phsize
    if (($ph + 32) -gt $elf.Length) { throw 'Program Header liegt ausserhalb des ELF.' }
    if ((U32 $elf $ph) -ne 4) { continue }
    $note = U32 $elf ($ph + 4)
    $end = $note + (U32 $elf ($ph + 16))
    if (($note + 36) -le $end -and $end -le $elf.Length -and
        (U32 $elf $note) -eq 4 -and (U32 $elf ($note + 4)) -eq 20 -and
        (U32 $elf ($note + 8)) -eq 3 -and (U32 $elf ($note + 12)) -eq 0x00554E47) {
        $buildId = $elf[($note + 16)..($note + 35)]
        break
    }
}
if ($null -eq $buildId) { throw 'GNU-Build-ID fehlt im ELF.' }
if ($elf.Length -lt 188 -or (U32 $elf 152) -ne 5 -or (U32 $elf 156) -ne 16 -or
    (U32 $elf 160) -ne 0x4E4F5601 -or (U32 $elf 164) -ne 0x41564F4E -or
    (U32 $elf 172) -ne 1 -or (U32 $elf 176) -gt 0x00010000) {
    throw 'Versionierte NOVA-Anforderungsnote fehlt oder ist ungueltig.'
}

if ($nki.Length -lt 64 -or [Text.Encoding]::ASCII.GetString($nki, 0, 7) -ne 'NOVANKI' -or
    (U32 $nki 8) -ne 1 -or (U32 $nki 12) -ne 64 -or ((U32 $nki 20) -band 3) -ne 3) {
    throw 'NKI-Header oder Build-ID-Flag ist ungueltig.'
}
$nkiPayloadSize = U32 $nki 32
if ((64 + $nkiPayloadSize) -gt $nki.Length) { throw 'NKI-Payload liegt ausserhalb der Datei.' }
for ($i = 0; $i -lt 16; $i++) {
    if ($nki[44 + $i] -ne $buildId[$i]) { throw 'NKI- und ELF-Build-ID stimmen nicht ueberein.' }
}

Write-Host ('Kernelartefakte validiert: Build-ID {0}' -f ([BitConverter]::ToString($buildId).Replace('-', '')))
