param(
    [Parameter(Mandatory = $true)][string]$InputImage,
    [Parameter(Mandatory = $true)][string]$OutputFile
)

$ErrorActionPreference = 'Stop'
$isoSector = 2048
$diskSector = 512
$espFirstLba = 2048
$gptTailSectors = 34
$catalogLba = 20
$bootImageLba = 21

function Write-Ascii([byte[]]$buffer,[int]$offset,[int]$length,[string]$text,[byte]$padding=0x20) {
    for($i=0;$i-lt$length;$i++){$buffer[$offset+$i]=$padding}
    $bytes=[Text.Encoding]::ASCII.GetBytes($text)
    [Array]::Copy($bytes,0,$buffer,$offset,[Math]::Min($bytes.Length,$length))
}
function Write-Le16([byte[]]$buffer,[int]$offset,[uint16]$value) {
    $buffer[$offset]=[byte]($value-band 0xff);$buffer[$offset+1]=[byte](($value-shr 8)-band 0xff)
}
function Write-Le32([byte[]]$buffer,[int]$offset,[uint32]$value) {
    for($i=0;$i-lt4;$i++){$buffer[$offset+$i]=[byte](($value-shr(8*$i))-band 0xff)}
}
function Write-Be16([byte[]]$buffer,[int]$offset,[uint16]$value) {
    $buffer[$offset]=[byte](($value-shr 8)-band 0xff);$buffer[$offset+1]=[byte]($value-band 0xff)
}
function Write-Be32([byte[]]$buffer,[int]$offset,[uint32]$value) {
    for($i=0;$i-lt4;$i++){$buffer[$offset+$i]=[byte](($value-shr(8*(3-$i)))-band 0xff)}
}
function Write-Both16([byte[]]$buffer,[int]$offset,[uint16]$value) {
    Write-Le16 $buffer $offset $value;Write-Be16 $buffer ($offset+2) $value
}
function Write-Both32([byte[]]$buffer,[int]$offset,[uint32]$value) {
    Write-Le32 $buffer $offset $value;Write-Be32 $buffer ($offset+4) $value
}

$inputFullPath=(Resolve-Path -LiteralPath $InputImage).Path
$disk=[IO.File]::ReadAllBytes($inputFullPath)
$espOffset=$espFirstLba*$diskSector
$espLength=$disk.Length-$espOffset-($gptTailSectors*$diskSector)
if($espLength -le 0 -or ($espLength % $diskSector) -ne 0) {
    throw 'Das Eingabeabbild enthält keine gültige GPT-ESP.'
}

# A UEFI El-Torito image is a FAT ESP, not the outer GPT disk.  The ESP uses
# LBA zero inside the ISO, so its FAT BPB must not retain the GPT offset.
$esp=[byte[]]::new($espLength)
[Array]::Copy($disk,$espOffset,$esp,0,$espLength)
for($i=0;$i-lt4;$i++){$esp[28+$i]=0}

$bootImageSectors=[int][Math]::Ceiling($esp.Length/$isoSector)
$volumeSectors=$bootImageLba+$bootImageSectors
$iso=[byte[]]::new($volumeSectors*$isoSector)

$pvd=16*$isoSector
$iso[$pvd]=1;Write-Ascii $iso ($pvd+1) 5 'CD001' 0;$iso[$pvd+6]=1
Write-Ascii $iso ($pvd+8) 32 'NOVAOS';Write-Ascii $iso ($pvd+40) 32 'NOVAOS_UEFI'
Write-Both32 $iso ($pvd+80) $volumeSectors;Write-Both16 $iso ($pvd+120) 1
Write-Both16 $iso ($pvd+124) 1;Write-Both16 $iso ($pvd+128) $isoSector;$iso[$pvd+881]=1

$bootRecord=17*$isoSector
$iso[$bootRecord]=0;Write-Ascii $iso ($bootRecord+1) 5 'CD001' 0;$iso[$bootRecord+6]=1
Write-Ascii $iso ($bootRecord+7) 32 'EL TORITO SPECIFICATION' 0;Write-Le32 $iso ($bootRecord+71) $catalogLba
$terminator=18*$isoSector
$iso[$terminator]=255;Write-Ascii $iso ($terminator+1) 5 'CD001' 0;$iso[$terminator+6]=1

$catalog=$catalogLba*$isoSector
$iso[$catalog]=1;$iso[$catalog+1]=0xef # EFI platform identifier
Write-Ascii $iso ($catalog+4) 24 'NovaOS UEFI Boot' 0;$iso[$catalog+30]=0x55;$iso[$catalog+31]=0xaa
$sum=0;for($i=0;$i-lt32;$i+=2){$sum=($sum+$iso[$catalog+$i]+($iso[$catalog+$i+1]-shl8))-band0xffff}
Write-Le16 $iso ($catalog+28) ((-$sum)-band0xffff)

# Bootable EFI, no-emulation image.  Sector count is intentionally zero as
# specified for EFI El-Torito images larger than the 16-bit legacy field.
$entry=$catalog+32
$iso[$entry]=0x88;$iso[$entry+1]=0;Write-Le16 $iso ($entry+2) 0;$iso[$entry+4]=0
Write-Le16 $iso ($entry+6) 0;Write-Le32 $iso ($entry+8) $bootImageLba
[Array]::Copy($esp,0,$iso,$bootImageLba*$isoSector,$esp.Length)

$outputFullPath=[IO.Path]::GetFullPath($OutputFile)
[IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($outputFullPath))|Out-Null
[IO.File]::WriteAllBytes($outputFullPath,$iso)
Write-Host "NovaOS UEFI ISO: $outputFullPath"
Write-Host "ISO-Groesse: $($iso.Length) Bytes"
