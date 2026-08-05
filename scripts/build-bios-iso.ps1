param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile
)

$ErrorActionPreference = 'Stop'

$sectorSize = 2048
$bootCatalogLba = 20
$bootImageLba = 21
$expectedImageSize = 1440 * 1024

$inputPath = (Resolve-Path -LiteralPath $InputFile).Path
$image = [System.IO.File]::ReadAllBytes($inputPath)
if ($image.Length -ne $expectedImageSize) {
    throw "Das BIOS-Image muss exakt $expectedImageSize Bytes (1,44 MB) groß sein; gefunden: $($image.Length)."
}

$imageSectors = [int][Math]::Ceiling($image.Length / $sectorSize)
$volumeSectors = $bootImageLba + $imageSectors
$iso = [byte[]]::new($volumeSectors * $sectorSize)

function Write-Ascii {
    param([byte[]]$Buffer, [int]$Offset, [int]$Length, [string]$Text, [byte]$Padding = 0x20)
    for ($i = 0; $i -lt $Length; $i++) { $Buffer[$Offset + $i] = $Padding }
    $bytes = [Text.Encoding]::ASCII.GetBytes($Text)
    [Array]::Copy($bytes, 0, $Buffer, $Offset, [Math]::Min($bytes.Length, $Length))
}

function Write-Le16 {
    param([byte[]]$Buffer, [int]$Offset, [uint16]$Value)
    $Buffer[$Offset] = [byte]($Value -band 0xFF)
    $Buffer[$Offset + 1] = [byte](($Value -shr 8) -band 0xFF)
}

function Write-Be16 {
    param([byte[]]$Buffer, [int]$Offset, [uint16]$Value)
    $Buffer[$Offset] = [byte](($Value -shr 8) -band 0xFF)
    $Buffer[$Offset + 1] = [byte]($Value -band 0xFF)
}

function Write-Le32 {
    param([byte[]]$Buffer, [int]$Offset, [uint32]$Value)
    for ($i = 0; $i -lt 4; $i++) { $Buffer[$Offset + $i] = [byte](($Value -shr (8 * $i)) -band 0xFF) }
}

function Write-Be32 {
    param([byte[]]$Buffer, [int]$Offset, [uint32]$Value)
    for ($i = 0; $i -lt 4; $i++) { $Buffer[$Offset + $i] = [byte](($Value -shr (8 * (3 - $i))) -band 0xFF) }
}

function Write-Both16 {
    param([byte[]]$Buffer, [int]$Offset, [uint16]$Value)
    Write-Le16 $Buffer $Offset $Value
    Write-Be16 $Buffer ($Offset + 2) $Value
}

function Write-Both32 {
    param([byte[]]$Buffer, [int]$Offset, [uint32]$Value)
    Write-Le32 $Buffer $Offset $Value
    Write-Be32 $Buffer ($Offset + 4) $Value
}

# Primary Volume Descriptor. A minimal ISO9660 descriptor is sufficient because
# the boot payload is addressed through the El Torito catalog.
$pvd = 16 * $sectorSize
$iso[$pvd] = 1
Write-Ascii $iso ($pvd + 1) 5 'CD001' 0
$iso[$pvd + 6] = 1
Write-Ascii $iso ($pvd + 8) 32 'NOVAOS'
Write-Ascii $iso ($pvd + 40) 32 'NOVAOS_BIOS'
Write-Both32 $iso ($pvd + 80) $volumeSectors
Write-Both16 $iso ($pvd + 120) 1
Write-Both16 $iso ($pvd + 124) 1
Write-Both16 $iso ($pvd + 128) $sectorSize
$iso[$pvd + 881] = 1

# El Torito Boot Record Volume Descriptor.
$bootRecord = 17 * $sectorSize
$iso[$bootRecord] = 0
Write-Ascii $iso ($bootRecord + 1) 5 'CD001' 0
$iso[$bootRecord + 6] = 1
Write-Ascii $iso ($bootRecord + 7) 32 'EL TORITO SPECIFICATION' 0
Write-Le32 $iso ($bootRecord + 71) $bootCatalogLba

# Volume Descriptor Set Terminator.
$terminator = 18 * $sectorSize
$iso[$terminator] = 255
Write-Ascii $iso ($terminator + 1) 5 'CD001' 0
$iso[$terminator + 6] = 1

# El Torito validation entry (platform 0 = x86 BIOS).
$catalog = $bootCatalogLba * $sectorSize
$iso[$catalog] = 1
$iso[$catalog + 1] = 0
Write-Ascii $iso ($catalog + 4) 24 'NovaOS BIOS Boot' 0
$iso[$catalog + 30] = 0x55
$iso[$catalog + 31] = 0xAA
$sum = 0
for ($i = 0; $i -lt 32; $i += 2) {
    $sum = ($sum + $iso[$catalog + $i] + ($iso[$catalog + $i + 1] -shl 8)) -band 0xFFFF
}
$checksum = (-$sum) -band 0xFFFF
Write-Le16 $iso ($catalog + 28) $checksum

# Initial/default entry: bootable, 1.44 MB floppy emulation.
$entry = $catalog + 32
$iso[$entry] = 0x88
$iso[$entry + 1] = 0x02
Write-Le16 $iso ($entry + 2) 0
$iso[$entry + 4] = 0
Write-Le16 $iso ($entry + 6) 1
Write-Le32 $iso ($entry + 8) $bootImageLba

[Array]::Copy($image, 0, $iso, $bootImageLba * $sectorSize, $image.Length)

$outputFullPath = [IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputFile))
$outputDirectory = Split-Path -Parent $outputFullPath
if ($outputDirectory -and -not (Test-Path -LiteralPath $outputDirectory)) {
    New-Item -ItemType Directory -Path $outputDirectory | Out-Null
}
[System.IO.File]::WriteAllBytes($outputFullPath, $iso)

Write-Host "NovaOS BIOS ISO: $outputFullPath"
Write-Host "ISO-Groesse: $($iso.Length) Bytes"
