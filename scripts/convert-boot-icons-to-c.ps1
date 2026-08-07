param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
$ErrorActionPreference = 'Stop'
$text = Get-Content -LiteralPath $InputFile -Raw -Encoding UTF8
$blocks = [regex]::Matches($text, '(?ms)^(bm_icon_[a-z0-9_]+):\s*(.*?)(?=^bm_icon_|\z)')
if ($blocks.Count -lt 6) { throw 'Zu wenige Iconressourcen.' }
$definitions = @()
foreach ($block in $blocks) {
    $name = $block.Groups[1].Value
    $values = [regex]::Matches($block.Groups[2].Value, '0x[0-9A-Fa-f]{2}') |
        ForEach-Object { $_.Value.ToLowerInvariant() }
    if ($values.Count -ne 100) { throw "Icon $name besitzt nicht 100 Bytes." }
    $lines = @()
    for ($i = 0; $i -lt $values.Count; $i += 16) {
        $last = [Math]::Min($i + 15, $values.Count - 1)
        $lines += '    ' + (($values[$i..$last]) -join ', ') + ','
    }
    $definitions += "static const uint8_t ${name}[100] = {`n$($lines -join "`n")`n};"
}
$directory = Split-Path -Parent $OutputFile
if ($directory) { New-Item -ItemType Directory -Force -Path $directory | Out-Null }
$header = "#ifndef NOVA_BOOT_ICON_DATA_H`n#define NOVA_BOOT_ICON_DATA_H`n#include <stdint.h>`n" +
          ($definitions -join "`n") + "`n#endif`n"
[System.IO.File]::WriteAllText($OutputFile, $header, [System.Text.UTF8Encoding]::new($false))
