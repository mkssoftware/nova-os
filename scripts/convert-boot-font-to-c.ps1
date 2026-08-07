param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)

$ErrorActionPreference = 'Stop'
$text = Get-Content -LiteralPath $InputFile -Raw -Encoding UTF8
function Read-HexValues([string]$section, [string]$until) {
    $start = $text.IndexOf($section)
    if ($start -lt 0) { throw "Fontabschnitt fehlt: $section" }
    $end = if ($until) { $text.IndexOf($until, $start + $section.Length) } else { $text.Length }
    if ($end -lt 0) { throw "Fontabschnittsende fehlt: $until" }
    return [regex]::Matches($text.Substring($start, $end - $start), '0x[0-9A-Fa-f]{2}') |
        ForEach-Object { $_.Value.ToLowerInvariant() }
}

$characters = Read-HexValues 'bm_font_characters:' 'bm_font_widths:'
$widthSectionStart = $text.IndexOf('bm_font_widths:')
$widthSectionEnd = $text.IndexOf('bm_font_bitmap:', $widthSectionStart)
$widthSection = $text.Substring($widthSectionStart, $widthSectionEnd - $widthSectionStart)
$widths = [regex]::Matches($widthSection, '(?m)^\s*db\s+([^\r\n]+)') | ForEach-Object {
    [regex]::Matches($_.Groups[1].Value, '\d+') | ForEach-Object {
        '0x{0:x2}' -f [int]$_.Value
    }
}
$bitmap = Read-HexValues 'bm_font_bitmap:' ''
if ($characters.Count -ne $widths.Count -or $bitmap.Count -ne $widths.Count * 70) {
    throw 'Fontdaten besitzen unerwartete Abmessungen.'
}

function Format-Array([string[]]$values) {
    $lines = @()
    for ($i = 0; $i -lt $values.Count; $i += 16) {
        $last = [Math]::Min($i + 15, $values.Count - 1)
        $lines += '    ' + (($values[$i..$last]) -join ', ') + ','
    }
    return $lines -join "`n"
}

$directory = Split-Path -Parent $OutputFile
if ($directory) { New-Item -ItemType Directory -Force -Path $directory | Out-Null }
$header = @"
#ifndef NOVA_BOOT_FONT_DATA_H
#define NOVA_BOOT_FONT_DATA_H
#include <stdint.h>
#define NOVA_FONT_WIDTH 14u
#define NOVA_FONT_HEIGHT 20u
#define NOVA_FONT_BYTES_PER_GLYPH 70u
#define NOVA_FONT_GLYPH_COUNT $($widths.Count)u
static const uint8_t nova_font_characters[] = {
$(Format-Array $characters)
};
static const uint8_t nova_font_widths[] = {
$(Format-Array $widths)
};
static const uint8_t nova_font_bitmap[] = {
$(Format-Array $bitmap)
};
#endif
"@
[System.IO.File]::WriteAllText($OutputFile, $header, [System.Text.UTF8Encoding]::new($false))
