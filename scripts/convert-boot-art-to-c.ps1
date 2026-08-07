param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
$ErrorActionPreference = 'Stop'
$text = Get-Content -LiteralPath $InputFile -Raw -Encoding UTF8
$match = [regex]::Match($text, '(?ms)^nova_logo_bitmap:\s*(.*?)(?=^nova_error_mark_bitmap:)')
if (-not $match.Success) { throw 'NovaOS-Logoressource nicht gefunden.' }
$values = [regex]::Matches($match.Groups[1].Value, '0x[0-9A-Fa-f]{2}') |
    ForEach-Object { $_.Value.ToLowerInvariant() }
if ($values.Count -ne 6000) { throw "NovaOS-Logo besitzt $($values.Count) statt 6000 Bytes." }
$lines = @()
for ($i = 0; $i -lt $values.Count; $i += 16) {
    $last = [Math]::Min($i + 15, $values.Count - 1)
    $lines += '    ' + (($values[$i..$last]) -join ', ') + ','
}
$directory = Split-Path -Parent $OutputFile
if ($directory) { New-Item -ItemType Directory -Force -Path $directory | Out-Null }
$header = "#ifndef NOVA_BOOT_ART_DATA_H`n#define NOVA_BOOT_ART_DATA_H`n#include <stdint.h>`n" +
          "#define NOVA_LOGO_WIDTH 160u`n#define NOVA_LOGO_HEIGHT 150u`n" +
          "static const uint8_t nova_logo_data[6000] = {`n$($lines -join "`n")`n};`n#endif`n"
[IO.File]::WriteAllText($OutputFile, $header, [Text.UTF8Encoding]::new($false))
