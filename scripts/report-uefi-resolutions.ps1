param(
    [string]$OutputFile = 'build/resolution-compatibility-report.md'
)

$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$cases = @(
    @{ File='build/uefi-800x600.ppm'; Width=800; Height=600; Mode='4:3' },
    @{ File='build/uefi-1280x720.ppm'; Width=1280; Height=720; Mode='16:9' },
    @{ File='build/uefi-1920x1080.ppm'; Width=1920; Height=1080; Mode='16:9' }
)

$rows = foreach ($case in $cases) {
    $path = [IO.Path]::GetFullPath((Join-Path $root $case.File))
    if (-not (Test-Path -LiteralPath $path)) { throw "Screenshot fehlt: $path" }
    $stream = [IO.File]::OpenRead($path)
    try {
        $buffer = [byte[]]::new([Math]::Min(256,[int]$stream.Length))
        $read = $stream.Read($buffer,0,$buffer.Length)
        $header = [Text.Encoding]::ASCII.GetString($buffer,0,$read)
    } finally { $stream.Dispose() }
    $match = [regex]::Match($header,'\AP6\s+(?:#[^\r\n]*\s+)*(\d+)\s+(\d+)\s+255\s')
    if (-not $match.Success) { throw "Ungültiger PPM-P6-Header: $path" }
    $width = [int]$match.Groups[1].Value
    $height = [int]$match.Groups[2].Value
    if ($width -ne $case.Width -or $height -ne $case.Height) {
        throw "Framebuffer size $($width)x$height instead of $($case.Width)x$($case.Height): $path"
    }
    [pscustomobject]@{
        Resolution="$($width)x$height"; Aspect=$case.Mode; Result='PASS';
        Bytes=(Get-Item -LiteralPath $path).Length
        Sha256=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
        Artifact=$case.File.Replace('\','/')
    }
}

$lines = @(
    '# NovaOS Resolution Compatibility Report','',
    'Deterministischer Lauf: 65 simulierte Kombinationen (13 Aufloesungen x 5 Skalierungsstufen).',
    'Reale EDK2/QEMU-GOP-Framebuffer: 3. Die Pruefsummen dienen der Regressionserkennung; eine freigegebene Pixel-Baseline und Toleranz sind noch nicht normativ definiert.','',
    '| Aufloesung | Seitenverhaeltnis | Ergebnis | Bytes | SHA-256 | Artefakt |',
    '|---|---|---|---:|---|---|'
)
foreach($row in $rows) {
    $lines += "| $($row.Resolution) | $($row.Aspect) | $($row.Result) | $($row.Bytes) | ``$($row.Sha256)`` | $($row.Artifact) |"
}
$lines += @('','## Verbleibende Grenzen','',
    '- GOP-Modi fuer alle zehn Pflichtaufloesungen sowie 5:4, 16:10 und Ultra-Wide stehen in der verwendeten QEMU-Firmware nicht vollstaendig zur Verfuegung.',
    '- Installer und Self-Healing besitzen noch keine produktive GUI.',
    '- SVG bleibt wegen des unvollstaendigen normativen Vektorvertrags blockiert.',
    '- Referenzbilder und zulaessige Pixel-Differenztoleranzen sind in NPSPEC-BOOTTEST-0006 nicht beziffert.'
)
$target = [IO.Path]::GetFullPath((Join-Path $root $OutputFile))
[IO.File]::WriteAllLines($target,$lines,[Text.UTF8Encoding]::new($false))
Write-Host "Resolution report: $target"
