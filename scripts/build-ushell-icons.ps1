param(
    [string]$Root = (Get-Location).Path
)

$ErrorActionPreference = 'Stop'
$rootPath = [IO.Path]::GetFullPath($Root)
$kernel = Join-Path $rootPath 'kernel\newkernel'
$source = Join-Path $kernel 'ushell\icons_main.c'
$linker = Join-Path $kernel 'ushell\icons-link.ld'
$build = Join-Path $kernel 'ushell\build\icons-current'
$header = Join-Path $kernel 'icons_blob.h'
[IO.Directory]::CreateDirectory($build) | Out-Null

$toolRoot = 'C:\msys64\mingw32\bin'
$env:PATH = $toolRoot + ';C:\Windows\System32;C:\Windows'
$gcc = Join-Path $toolRoot 'i686-w64-mingw32-gcc.exe'
$ld = Join-Path $toolRoot 'ld.exe'
$objcopy = Join-Path $toolRoot 'objcopy.exe'
$nm = Join-Path $toolRoot 'nm.exe'
foreach ($tool in @($gcc,$ld,$objcopy,$nm)) {
    if (-not (Test-Path -LiteralPath $tool)) { throw "Build-Werkzeug fehlt: $tool" }
}

# KRITISCH: -ffunction-sections/-fdata-sections + ld --gc-sections (siehe
# Aufruf unten) sind hier PFLICHT, nicht optional - ohne sie bleibt
# svgicon.c's kompletter 8-Slot-Render-Cache (g_svgicon_cache[SVGICON_
# CACHE_SLOTS], >128 KiB) im Binary, obwohl icons_main.c nur das
# ungecachte svgicon_render() aufruft und ihn nie benutzt. Der Linker
# platziert .bss dann zwar korrekt (null Datei-Bytes), ABER
# ushell_process.c's ushell_spawn() mappt fuer JEDEN ushell-Prozess nur
# `USHELL_CODE_PAGES` (64 KiB) physischen Speicher ab der Blob-Basis -
# .bss landet bei diesem einen Prozess durch den toten Cache weit
# AUSSERHALB dieses gemappten Bereichs (gemessen: .bss allein 232128
# Byte, Gesamt-VMA-Spanne ~255 KiB) und jeder Zugriff auf seine
# statischen Variablen (g_svgicon_scene/g_svgicon_scratch) loeste einen
# echten Seitenfehler aus (Boot-Crash "EXCEPTION (kein Exception-Manager
# aktiv) - HALT" kurz nach dem ersten Scheduler-Durchlauf dieses
# Prozesses - KEIN Boot-Time-Fehler, da das Binary selbst klein genug
# aussah, nur eine zur Laufzeit garantiert fehlschlagende Pruefung).
# -ffunction-sections/-fdata-sections + --gc-sections entfernen den
# toten Cache (und jede andere unbenutzte Funktion/Variable) tatsaechlich
# aus dem Binary, statt ihn nur in .bss zu verstecken - verifiziert:
# 58048 Byte Flat-Binary, Gesamt-VMA-Spanne (Text+Rdata+Data, KEIN
# separates .bss mehr) 0xE2C0=57536 Byte, beides WEIT innerhalb des
# 64-KiB-Budgets UND vollstaendig durch den Blob abgedeckt.
$common = @('-m32','-ffreestanding','-fno-builtin','-fno-stack-protector','-fno-pic',
            '-fno-asynchronous-unwind-tables','-ffunction-sections','-fdata-sections',
            '-O2','-Wall','-Wextra','-Werror',"-I$kernel")
$objects = @(
    @{ Source=$source; Object=(Join-Path $build 'icons_main.o') },
    @{ Source=(Join-Path $kernel 'render2d.c'); Object=(Join-Path $build 'render2d.o') },
    @{ Source=(Join-Path $kernel 'font.c'); Object=(Join-Path $build 'font.o') },
    @{ Source=(Join-Path $kernel 'svgicon.c'); Object=(Join-Path $build 'svgicon.o') }
)
foreach ($item in $objects) {
    & $gcc @common '-c' $item.Source '-o' $item.Object
    if ($LASTEXITCODE -ne 0) { throw "Kompilieren fehlgeschlagen: $($item.Source)" }
}

$elf = Join-Path $build 'icons.elf'
$bin = Join-Path $build 'icons.bin'
& $ld '-m' 'i386pe' '--gc-sections' '-T' $linker '-o' $elf @($objects.Object)
if ($LASTEXITCODE -ne 0) { throw 'Icons-Linken fehlgeschlagen.' }
& $objcopy '-O' 'binary' $elf $bin
if ($LASTEXITCODE -ne 0) { throw 'Icons-Objcopy fehlgeschlagen.' }

$symbols = & $nm '-n' $elf
if ($LASTEXITCODE -ne 0) { throw 'Icons-Symbole konnten nicht gelesen werden.' }
function Find-Symbol([string]$name) {
    $line = $symbols | Where-Object { $_ -match "\s$name$" } | Select-Object -First 1
    if (-not $line -or $line -notmatch '^([0-9A-Fa-f]+)\s') { throw "Symbol fehlt: $name" }
    return [Convert]::ToUInt32($Matches[1],16)
}
$base = 0x00500000
$entry = (Find-Symbol 'icons_start') - $base
$roSize = (Find-Symbol '__rw_start') - $base
$bytes = [IO.File]::ReadAllBytes($bin)
if ($bytes.Length -gt 65536) { throw "Icons-Blob ist zu gross: $($bytes.Length) Bytes" }
if (($roSize % 4096) -ne 0) { throw "ICONS_RO_SIZE ist nicht seitenausgerichtet: $roSize" }

# ZUSAETZLICH zur reinen Datei-Groesse (die ein verbliebenes .bss NICHT
# mitzaehlt, siehe Kommentar oben) die tatsaechliche VIRTUELLE Gesamt-
# spanne aller Sektionen pruefen - genau DIESE Pruefung haette den
# urspruenglichen Bug (Datei klein, aber .bss weit ausserhalb des
# gemappten Bereichs) sofort erkannt, statt ihn erst beim echten Boot-
# Test als Seitenfehler sichtbar zu machen.
$objdump = Join-Path $toolRoot 'objdump.exe'
$sections = & $objdump '-h' $elf
$maxEnd = 0
foreach ($line in $sections) {
    if ($line -match '^\s*\d+\s+(\S+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s') {
        $size = [Convert]::ToUInt32($Matches[2],16)
        $vma = [Convert]::ToUInt32($Matches[3],16)
        if ($size -gt 0) {
            $end = $vma + $size
            if ($end -gt $maxEnd) { $maxEnd = $end }
        }
    }
}
$totalSpan = $maxEnd - $base
if ($totalSpan -gt 65536) {
    throw "Icons-Blob's virtuelle Gesamtspanne (Text+Rdata+Data+Bss) ist zu gross: $totalSpan Bytes - ushell_spawn() mappt nur USHELL_CODE_PAGES*4096=65536 Byte, alles darueber ist zur Laufzeit ein garantierter Seitenfehler."
}

$sb = [Text.StringBuilder]::new()
[void]$sb.AppendLine('/* Automatisch erzeugt durch scripts/build-ushell-icons.ps1. */')
[void]$sb.AppendLine('#ifndef NOVA_ICONS_BLOB_H')
[void]$sb.AppendLine('#define NOVA_ICONS_BLOB_H')
[void]$sb.AppendLine('#include <stdint.h>')
[void]$sb.AppendLine("#define ICONS_BLOB_SIZE $($bytes.Length)u")
[void]$sb.AppendLine("#define ICONS_ENTRY_OFFSET $($entry)u")
[void]$sb.AppendLine("#define ICONS_RO_SIZE $($roSize)u")
[void]$sb.AppendLine('static const uint8_t icons_blob[ICONS_BLOB_SIZE] = {')
for ($i=0; $i -lt $bytes.Length; $i += 16) {
    $end = [Math]::Min($i + 16, $bytes.Length)
    $chunk = for ($j=$i; $j -lt $end; ++$j) { '0x{0:X2}u' -f $bytes[$j] }
    [void]$sb.Append('    ')
    [void]$sb.Append(($chunk -join ','))
    [void]$sb.AppendLine(',')
}
[void]$sb.AppendLine('};')
[void]$sb.AppendLine('#endif')
[IO.File]::WriteAllText($header,$sb.ToString(),[Text.UTF8Encoding]::new($false))

Write-Host "Icons-Blob: $($bytes.Length) Bytes, Entry +$entry, RO $roSize"
Write-Host "Header: $header"
