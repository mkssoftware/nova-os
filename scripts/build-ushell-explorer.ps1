param(
    [string]$Root = (Get-Location).Path
)

$ErrorActionPreference = 'Stop'
$rootPath = [IO.Path]::GetFullPath($Root)
$kernel = Join-Path $rootPath 'kernel\newkernel'
$source = Join-Path $kernel 'ushell\explorer_main.c'
$linker = Join-Path $kernel 'ushell\explorer-link.ld'
$build = Join-Path $kernel 'ushell\build\explorer-current'
$header = Join-Path $kernel 'explorer_blob.h'
[IO.Directory]::CreateDirectory($build) | Out-Null

$toolRoot = 'C:\msys64\mingw32\bin'
$env:PATH = $toolRoot + ';C:\Windows\System32;C:\Windows'
$gcc = Join-Path $toolRoot 'i686-w64-mingw32-gcc.exe'
$ld = Join-Path $toolRoot 'ld.exe'
$objcopy = Join-Path $toolRoot 'objcopy.exe'
$nm = Join-Path $toolRoot 'nm.exe'
$objdump = Join-Path $toolRoot 'objdump.exe'
foreach ($tool in @($gcc,$ld,$objcopy,$nm,$objdump)) {
    if (-not (Test-Path -LiteralPath $tool)) { throw "Build-Werkzeug fehlt: $tool" }
}

$common = @('-m32','-ffreestanding','-fno-builtin','-fno-stack-protector','-fno-pic',
            '-fno-asynchronous-unwind-tables','-O2','-Wall','-Wextra','-Werror',"-I$kernel")
$objects = @(
    @{ Source=$source; Object=(Join-Path $build 'explorer_main.o') },
    @{ Source=(Join-Path $kernel 'render2d.c'); Object=(Join-Path $build 'render2d.o') },
    @{ Source=(Join-Path $kernel 'font.c'); Object=(Join-Path $build 'font.o') }
)
foreach ($item in $objects) {
    & $gcc @common '-c' $item.Source '-o' $item.Object
    if ($LASTEXITCODE -ne 0) { throw "Kompilieren fehlgeschlagen: $($item.Source)" }
}

$elf = Join-Path $build 'explorer.elf'
$bin = Join-Path $build 'explorer.bin'
& $ld '-m' 'i386pe' '-T' $linker '-o' $elf @($objects.Object)
if ($LASTEXITCODE -ne 0) { throw 'Explorer-Linken fehlgeschlagen.' }
& $objcopy '-O' 'binary' $elf $bin
if ($LASTEXITCODE -ne 0) { throw 'Explorer-Objcopy fehlgeschlagen.' }

$symbols = & $nm '-n' $elf
if ($LASTEXITCODE -ne 0) { throw 'Explorer-Symbole konnten nicht gelesen werden.' }
function Find-Symbol([string]$name) {
    $line = $symbols | Where-Object { $_ -match "\s$name$" } | Select-Object -First 1
    if (-not $line -or $line -notmatch '^([0-9A-Fa-f]+)\s') { throw "Symbol fehlt: $name" }
    return [Convert]::ToUInt32($Matches[1],16)
}
$base = 0x00500000
$entry = (Find-Symbol 'explorer_start') - $base
$roSize = (Find-Symbol '__rw_start') - $base
$bytes = [IO.File]::ReadAllBytes($bin)
if ($bytes.Length -gt 65536) { throw "Explorer-Blob ist zu gross: $($bytes.Length) Bytes" }
if (($roSize % 4096) -ne 0) { throw "EXPLORER_RO_SIZE ist nicht seitenausgerichtet: $roSize" }

# Siehe build-ushell-icons.ps1 fuer die volle Begruendung: Datei-Groesse
# allein beweist nicht, dass .bss innerhalb des gemappten Budgets bleibt.
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
    throw "Explorer-Blob's virtuelle Gesamtspanne ist zu gross: $totalSpan Bytes (Budget 65536)."
}

$sb = [Text.StringBuilder]::new()
[void]$sb.AppendLine('/* Automatisch erzeugt durch scripts/build-ushell-explorer.ps1. */')
[void]$sb.AppendLine('#ifndef NOVA_EXPLORER_BLOB_H')
[void]$sb.AppendLine('#define NOVA_EXPLORER_BLOB_H')
[void]$sb.AppendLine('#include <stdint.h>')
[void]$sb.AppendLine("#define EXPLORER_BLOB_SIZE $($bytes.Length)u")
[void]$sb.AppendLine("#define EXPLORER_ENTRY_OFFSET $($entry)u")
[void]$sb.AppendLine("#define EXPLORER_RO_SIZE $($roSize)u")
[void]$sb.AppendLine('static const uint8_t explorer_blob[EXPLORER_BLOB_SIZE] = {')
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

Write-Host "Explorer-Blob: $($bytes.Length) Bytes, Entry +$entry, RO $roSize"
Write-Host "Header: $header"
