# Baut das NEF-1.0-Host-Werkzeug (tools/nef/nef_main.c) mit dem normalen,
# gehosteten MinGW-gcc - KEIN freistehender Kernel-Build, dieses Programm
# laeuft auf dem Windows-Entwicklungssystem, nicht in NovaOS selbst. Es
# verlinkt direkt gegen kernel/newkernel/nef.c (reines, portables C ohne
# Kernel-Abhaengigkeiten), damit Host-Werkzeug und Kernel exakt dieselbe
# Formatlogik verwenden.
param(
    [string]$Root = (Get-Location).Path
)

$ErrorActionPreference = 'Stop'
$rootPath = [IO.Path]::GetFullPath($Root)
$kernel = Join-Path $rootPath 'kernel\newkernel'
$toolDir = Join-Path $rootPath 'tools\nef'

$toolRoot = 'C:\msys64\mingw32\bin'
$env:PATH = $toolRoot + ';C:\Windows\System32;C:\Windows'
$gcc = Join-Path $toolRoot 'gcc.exe'
if (-not (Test-Path -LiteralPath $gcc)) { throw "Build-Werkzeug fehlt: $gcc" }

$out = Join-Path $toolDir 'nef.exe'
& $gcc '-std=c99' '-Wall' '-Wextra' '-I' $kernel `
    (Join-Path $toolDir 'nef_main.c') (Join-Path $kernel 'nef.c') `
    '-o' $out
if ($LASTEXITCODE -ne 0) { throw 'nef-Werkzeug-Build fehlgeschlagen.' }

Write-Host "nef-Werkzeug gebaut: $out"
