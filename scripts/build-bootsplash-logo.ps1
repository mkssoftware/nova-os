# NovaOS - konvertiert assets/logoNew.png in einen fest einkompilierten
# C-Pixel-Blob fuer den Boot-Splash (kernel/newkernel/bootsplash.c).
#
# Analog zum bestehenden Icon-Blob-Ansatz (siehe icons_blob.h/
# ushell_process.c): das Logo wird frueh gezeigt, lange bevor NovaFS
# gemountet ist (anders als wallpaper.c's Laufzeit-Ladepfad aus NovaFS,
# siehe wallpaper.h) - daher Kompilierzeit-Einbettung statt Laufzeit-Datei.
#
# Das Quellbild hat Transparenz (Stern-Logo auf durchsichtigem Grund); da
# der Splash-Hintergrund IMMER eine feste Volltonfarbe ist (siehe
# BOOTSPLASH_BG_RGB in bootsplash.c), wird hier bereits beim Konvertieren
# mit genau dieser Farbe alpha-ueberblendet ("gebacken") - der Kernel
# braucht dadurch KEIN eigenes Alpha-Blending fuer das Logo (render2d.c
# bietet ohnehin nur 1:1-Blit ohne Blending, siehe render2d.h).
param(
    [string]$Source = "assets/logoNew.png",
    [string]$OutputHeader = "kernel/newkernel/bootsplash_logo.h",
    [int]$TargetWidth = 200,
    [int]$BackgroundR = 0,
    [int]$BackgroundG = 0,
    [int]$BackgroundB = 0
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$resolvedSource = Resolve-Path -LiteralPath $Source
$src = [System.Drawing.Bitmap]::FromFile($resolvedSource)
$targetHeight = [int][math]::Round($TargetWidth * $src.Height / $src.Width)

$canvas = New-Object System.Drawing.Bitmap($TargetWidth, $targetHeight, `
    [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($canvas)
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
$g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
$g.Clear([System.Drawing.Color]::FromArgb(255, $BackgroundR, $BackgroundG, $BackgroundB))
$g.DrawImage($src, 0, 0, $TargetWidth, $targetHeight)
$g.Dispose()
$src.Dispose()

$pixelValues = [System.Collections.Generic.List[string]]::new()
for ($y = 0; $y -lt $targetHeight; $y++) {
    for ($x = 0; $x -lt $TargetWidth; $x++) {
        $pixel = $canvas.GetPixel($x, $y)
        $packed = ([uint32]$pixel.R -shl 16) -bor ([uint32]$pixel.G -shl 8) -bor [uint32]$pixel.B
        $pixelValues.Add("0x{0:X6}u" -f $packed)
    }
}
$canvas.Dispose()

$lines = [System.Collections.Generic.List[string]]::new()
$lines.Add("/* NovaOS - Boot-Splash-Logo-Pixeldaten. GENERIERT von")
$lines.Add("   scripts/build-bootsplash-logo.ps1 aus assets/logoNew.png - NICHT von")
$lines.Add("   Hand bearbeiten, stattdessen das Skript erneut mit dem gewuenschten")
$lines.Add("   Quellbild laufen lassen. Bereits mit dem Splash-Hintergrund")
$bgHex = "0x{0:X2}{1:X2}{2:X2}" -f $BackgroundR, $BackgroundG, $BackgroundB
$lines.Add("   ($bgHex) alpha-vorgeblendet - siehe Skriptkopf. */")
$lines.Add("#ifndef NOVA_BOOTSPLASH_LOGO_H")
$lines.Add("#define NOVA_BOOTSPLASH_LOGO_H")
$lines.Add("")
$lines.Add("#include <stdint.h>")
$lines.Add("")
$lines.Add("#define BOOTSPLASH_LOGO_WIDTH  $($TargetWidth)u")
$lines.Add("#define BOOTSPLASH_LOGO_HEIGHT $($targetHeight)u")
$lines.Add("")
$lines.Add("static const uint32_t bootsplash_logo_pixels[BOOTSPLASH_LOGO_WIDTH * BOOTSPLASH_LOGO_HEIGHT] = {")
for ($offset = 0; $offset -lt $pixelValues.Count; $offset += 8) {
    $count = [math]::Min(8, $pixelValues.Count - $offset)
    $slice = $pixelValues.GetRange($offset, $count)
    $lines.Add("    " + ($slice -join ",") + ",")
}
$lines.Add("};")
$lines.Add("")
$lines.Add("#endif")

$outputDirectory = Split-Path -Parent $OutputHeader
if ($outputDirectory) {
    New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null
}
[System.IO.File]::WriteAllLines((Join-Path (Get-Location) $OutputHeader), $lines)
Write-Output "Geschrieben: $OutputHeader ($TargetWidth x $targetHeight, $($pixelValues.Count) Pixel)"
