param(
    [Parameter(Mandatory=$true)][string]$InputFile,
    [Parameter(Mandatory=$true)][string]$OutputFile,
    [ValidateRange(64,1920)][int]$Width=1280,
    [ValidateRange(64,1080)][int]$Height=720
)

$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing

function Write-U32([byte[]]$Buffer,[int]$Offset,[uint32]$Value) {
    for($index=0;$index-lt4;$index++){
        $Buffer[$Offset+$index]=[byte](($Value-shr(8*$index))-band0xff)
    }
}

function Get-Crc32([byte[]]$Buffer,[int]$Offset,[int]$Length) {
    [uint64]$crc=4294967295
    for($index=0;$index-lt$Length;$index++){
        $crc=$crc-bxor$Buffer[$Offset+$index]
        for($bit=0;$bit-lt8;$bit++){
            if($crc-band1){$crc=(($crc-shr1)-bxor3988292384)-band4294967295}
            else{$crc=$crc-shr1}
        }
    }
    return [uint32](($crc-bxor4294967295)-band4294967295)
}

$inputPath=[IO.Path]::GetFullPath($InputFile)
if(!(Test-Path -LiteralPath $inputPath)){throw "Bootsplash fehlt: $inputPath"}
$outputPath=[IO.Path]::GetFullPath($OutputFile)
[IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($outputPath))|Out-Null

$source=[Drawing.Bitmap]::new($inputPath)
$scaled=[Drawing.Bitmap]::new($Width,$Height,[Drawing.Imaging.PixelFormat]::Format24bppRgb)
try {
    $graphics=[Drawing.Graphics]::FromImage($scaled)
    try {
        $graphics.Clear([Drawing.Color]::Black)
        $graphics.CompositingMode=[Drawing.Drawing2D.CompositingMode]::SourceCopy
        $graphics.CompositingQuality=[Drawing.Drawing2D.CompositingQuality]::HighQuality
        $graphics.InterpolationMode=[Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $graphics.PixelOffsetMode=[Drawing.Drawing2D.PixelOffsetMode]::HighQuality
        $graphics.SmoothingMode=[Drawing.Drawing2D.SmoothingMode]::HighQuality
        $graphics.DrawImage($source,[Drawing.Rectangle]::new(0,0,$Width,$Height),
            0,0,$source.Width,$source.Height,[Drawing.GraphicsUnit]::Pixel)
    } finally {$graphics.Dispose()}

    $rectangle=[Drawing.Rectangle]::new(0,0,$Width,$Height)
    $data=$scaled.LockBits($rectangle,[Drawing.Imaging.ImageLockMode]::ReadOnly,
        [Drawing.Imaging.PixelFormat]::Format24bppRgb)
    try {
        $stride=[Math]::Abs($data.Stride)
        $rgb=[byte[]]::new($stride*$Height)
        [Runtime.InteropServices.Marshal]::Copy($data.Scan0,$rgb,0,$rgb.Length)
    } finally {$scaled.UnlockBits($data)}

    $headerSize=32
    $payloadSize=$Width*$Height*3
    $result=[byte[]]::new($headerSize+$payloadSize)
    $result[0]=[byte][char]'N';$result[1]=[byte][char]'B'
    $result[2]=[byte][char]'S';$result[3]=[byte][char]'1'
    Write-U32 $result 4 $headerSize
    Write-U32 $result 8 $Width
    Write-U32 $result 12 $Height
    Write-U32 $result 16 ($Width*3)
    Write-U32 $result 20 2             # verlustfreies RGB888
    Write-U32 $result 24 $payloadSize
    for($y=0;$y-lt$Height;$y++){
        $sourceY=if($data.Stride-lt0){$Height-1-$y}else{$y}
        for($x=0;$x-lt$Width;$x++){
            $sourceOffset=$sourceY*$stride+$x*3
            $blue=$rgb[$sourceOffset]
            $green=$rgb[$sourceOffset+1]
            $red=$rgb[$sourceOffset+2]
            $target=$headerSize+($y*$Width+$x)*3
            $result[$target]=$red
            $result[$target+1]=$green
            $result[$target+2]=$blue
        }
    }
    Write-U32 $result 28 (Get-Crc32 $result $headerSize $payloadSize)
    $temporary=$outputPath+'.tmp-'+[Guid]::NewGuid().ToString('N')
    [IO.File]::WriteAllBytes($temporary,$result)
    try {[IO.File]::Copy($temporary,$outputPath,$true)}
    finally {if(Test-Path -LiteralPath $temporary){[IO.File]::Delete($temporary)}}
    Write-Host "BOOTSPLASH: $outputPath ($Width x $Height, RGB888, $($result.Length) Bytes)"
} finally {
    $scaled.Dispose()
    $source.Dispose()
}
