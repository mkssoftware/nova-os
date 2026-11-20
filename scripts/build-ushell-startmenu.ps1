param([string]$Root = (Get-Location).Path)

$ErrorActionPreference = 'Stop'
$rootPath = [IO.Path]::GetFullPath($Root)
$kernel = Join-Path $rootPath 'kernel\newkernel'
$source = Join-Path $kernel 'ushell\startmenu_main.c'
$linker = Join-Path $kernel 'ushell\startmenu-link.ld'
$build = Join-Path $kernel 'ushell\build\startmenu-current'
$header = Join-Path $kernel 'startmenu_blob.h'
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

$common = @('-m32','-ffreestanding','-fno-builtin','-fno-stack-protector','-fno-pic',
            '-fno-asynchronous-unwind-tables','-O2','-Wall','-Wextra','-Werror',"-I$kernel")
$objects = @(
    @{ Source=$source; Object=(Join-Path $build 'startmenu_main.o') },
    @{ Source=(Join-Path $kernel 'render2d.c'); Object=(Join-Path $build 'render2d.o') },
    @{ Source=(Join-Path $kernel 'font.c'); Object=(Join-Path $build 'font.o') }
)
foreach ($item in $objects) {
    & $gcc @common '-c' $item.Source '-o' $item.Object
    if ($LASTEXITCODE -ne 0) { throw "Kompilieren fehlgeschlagen: $($item.Source)" }
}

$elf = Join-Path $build 'startmenu.elf'
$bin = Join-Path $build 'startmenu.bin'
& $ld '-m' 'i386pe' '-T' $linker '-o' $elf @($objects.Object)
if ($LASTEXITCODE -ne 0) { throw 'Startmenue-Linken fehlgeschlagen.' }
& $objcopy '-O' 'binary' $elf $bin
if ($LASTEXITCODE -ne 0) { throw 'Startmenue-Objcopy fehlgeschlagen.' }

$symbols = & $nm '-n' $elf
if ($LASTEXITCODE -ne 0) { throw 'Startmenue-Symbole konnten nicht gelesen werden.' }
function Find-Symbol([string]$name) {
    $line = $symbols | Where-Object { $_ -match "\s$name$" } | Select-Object -First 1
    if (-not $line -or $line -notmatch '^([0-9A-Fa-f]+)\s') { throw "Symbol fehlt: $name" }
    return [Convert]::ToUInt32($Matches[1],16)
}
$base = 0x00500000
$entry = (Find-Symbol 'startmenu_start') - $base
$roSize = (Find-Symbol '__rw_start') - $base
$bytes = [IO.File]::ReadAllBytes($bin)
if ($bytes.Length -gt 65536) { throw "Startmenue-Blob ist zu gross: $($bytes.Length) Bytes" }
if (($roSize % 4096) -ne 0) { throw "STARTMENU_RO_SIZE ist nicht seitenausgerichtet: $roSize" }

$sb = [Text.StringBuilder]::new()
[void]$sb.AppendLine('/* Automatisch erzeugt durch scripts/build-ushell-startmenu.ps1. */')
[void]$sb.AppendLine('#ifndef NOVA_STARTMENU_BLOB_H')
[void]$sb.AppendLine('#define NOVA_STARTMENU_BLOB_H')
[void]$sb.AppendLine('#include <stdint.h>')
[void]$sb.AppendLine("#define STARTMENU_BLOB_SIZE $($bytes.Length)u")
[void]$sb.AppendLine("#define STARTMENU_ENTRY_OFFSET $($entry)u")
[void]$sb.AppendLine("#define STARTMENU_RO_SIZE $($roSize)u")
[void]$sb.AppendLine('static const uint8_t startmenu_blob[STARTMENU_BLOB_SIZE] = {')
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

Write-Host "Startmenue-Blob: $($bytes.Length) Bytes, Entry +$entry, RO $roSize"
Write-Host "Header: $header"
