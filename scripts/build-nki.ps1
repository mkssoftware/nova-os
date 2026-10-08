param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile,

    # §113: LZ4-Blockformat-Kompression (Literal-Only, kein echter Back-Reference-Kompressor)
    [switch]$Compress,

    # §114: ZSTD-Frame-Kompression (Raw-Block-Only, kein Huffman/FSE-Kompressor)
    [switch]$CompressZstd
)

$ErrorActionPreference = 'Stop'
if ($Compress -and $CompressZstd) {
    throw 'Die Schalter -Compress (LZ4) und -CompressZstd (ZSTD) schliessen sich gegenseitig aus.'
}

$headerSize = 64
$maximumPayloadSize = 262144
$entryPoint = [uint32]0x00100000

$payload = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile))
if ($payload.Length -eq 0 -or $payload.Length -gt $maximumPayloadSize) {
    throw "Kernelgröße $($payload.Length) liegt außerhalb 1..$maximumPayloadSize Bytes."
}

function Get-Crc32 {
    param([byte[]]$Data)

    [uint32]$crc = [uint32]::MaxValue
    [uint32]$polynomial = [Convert]::ToUInt32("EDB88320", 16)
    foreach ($value in $Data) {
        $crc = $crc -bxor [uint32]$value
        for ($bit = 0; $bit -lt 8; $bit++) {
            if (($crc -band 1) -ne 0) {
                $crc = ($crc -shr 1) -bxor $polynomial
            } else {
                $crc = $crc -shr 1
            }
        }
    }
    return [uint32]($crc -bxor [uint32]::MaxValue)
}

# §113: LZ4-Block-Kompressor (Literal-Only — gültiges LZ4-Blockformat, kein Back-Reference-Kompressor)
# Payload-Layout bei -Compress: [0..3] uncompressed_size LE + [4..] LZ4-Block-Daten
function Compress-Lz4Block([byte[]]$data) {
    $n = $data.Length
    $out = [Collections.Generic.List[byte]]::new()
    # Literal-Extension-Präfix: gesamte Daten als eine einzige Sequenz ohne Match
    $litNibble = [Math]::Min($n, 15)
    $out.Add([byte]($litNibble -shl 4))   # Token: lit_nibble | match_nibble=0
    if ($n -ge 15) {
        $rem = $n - 15
        while ($rem -ge 255) { $out.Add([byte]255); $rem -= 255 }
        $out.Add([byte]$rem)
    }
    foreach ($b in $data) { $out.Add($b) }
    return $out.ToArray()
}

$elfPayload = $payload   # Original-ELF für Build-ID-Extraktion behalten

# §114: ZSTD-Raw-Block-Kompressor (gültiges ZSTD-Frame-Format, kein Huffman/FSE)
# Frame-Layout: magic(4) + FHD(1=0xA0) + FCS(4 LE) + BlockHeader(3 LE) + Rohdaten
# FHD 0xA0: FCS_Flag=2 (FCS in 4 Bytes), Single_Segment=1, kein Checksum, kein Dict
function Compress-ZstdRaw([byte[]]$data) {
    $n = $data.Length
    $out = [Collections.Generic.List[byte]]::new()
    # ZSTD-Magic (0xFD2FB528 LE)
    $out.Add(0x28); $out.Add(0xB5); $out.Add(0x2F); $out.Add(0xFD)
    # Frame_Header_Descriptor: FCS_Flag=2, Single_Segment=1
    $out.Add(0xA0)
    # Frame_Content_Size (4 Bytes LE)
    $fcs = [BitConverter]::GetBytes([uint32]$n)
    foreach ($b in $fcs) { $out.Add($b) }
    # Block_Header (3 Bytes LE): Last_Block=1, Block_Type=0 (Raw), Block_Size=n
    # bh = (n << 3) | 1
    $out.Add([byte](($n -shl 3) -bor 1))
    $out.Add([byte](($n -shr 5) -band 0xFF))
    $out.Add([byte](($n -shr 13) -band 0xFF))
    # Rohdaten (unveraendert)
    foreach ($b in $data) { $out.Add($b) }
    return $out.ToArray()
}

$compressionId = [uint32]0   # NOVA_NKI_COMPRESSION_NONE
if ($Compress) {
    $lz4Block  = Compress-Lz4Block -data $payload
    $sizePrefix = [BitConverter]::GetBytes([uint32]$payload.Length)
    $compressed = [byte[]]::new(4 + $lz4Block.Length)
    [Array]::Copy($sizePrefix, 0, $compressed, 0, 4)
    [Array]::Copy($lz4Block,   0, $compressed, 4, $lz4Block.Length)
    $payload = $compressed
    $compressionId = [uint32]1   # NOVA_NKI_COMPRESSION_LZ4
}
if ($CompressZstd) {
    $zstdFrame  = Compress-ZstdRaw -data $payload
    $sizePrefix = [BitConverter]::GetBytes([uint32]$payload.Length)
    $compressed = [byte[]]::new(4 + $zstdFrame.Length)
    [Array]::Copy($sizePrefix, 0, $compressed, 0, 4)
    [Array]::Copy($zstdFrame,  0, $compressed, 4, $zstdFrame.Length)
    $payload = $compressed
    $compressionId = [uint32]2   # NOVA_NKI_COMPRESSION_ZSTD
}

$crc32 = Get-Crc32 -Data $payload
function Get-U16([byte[]]$Data, [int]$Offset) { return [BitConverter]::ToUInt16($Data, $Offset) }
function Get-U32([byte[]]$Data, [int]$Offset) { return [BitConverter]::ToUInt32($Data, $Offset) }

$buildId = $null
if ($elfPayload.Length -ge 52 -and (Get-U32 $elfPayload 0) -eq 0x464C457F) {
    $phoff = Get-U32 $elfPayload 28
    $phentsize = Get-U16 $elfPayload 42
    $phnum = Get-U16 $elfPayload 44
    for ($index = 0; $index -lt $phnum; $index++) {
        $ph = $phoff + ($index * $phentsize)
        if (($ph + 32) -gt $elfPayload.Length) { break }
        if ((Get-U32 $elfPayload $ph) -ne 4) { continue }
        $note = Get-U32 $elfPayload ($ph + 4)
        $noteEnd = $note + (Get-U32 $elfPayload ($ph + 16))
        while (($note + 12) -le $noteEnd -and $noteEnd -le $elfPayload.Length) {
            $nameSize = Get-U32 $elfPayload $note
            $descSize = Get-U32 $elfPayload ($note + 4)
            $noteType = Get-U32 $elfPayload ($note + 8)
            $namePadded = ($nameSize + 3) -band -4
            $descPadded = ($descSize + 3) -band -4
            $next = $note + 12 + $namePadded + $descPadded
            if ($next -gt $noteEnd) { break }
            if ($noteType -eq 3 -and $nameSize -eq 4 -and $descSize -ge 16 -and
                [Text.Encoding]::ASCII.GetString($elfPayload, $note + 12, 3) -eq 'GNU') {
                $start = $note + 12 + $namePadded
                $buildId = $elfPayload[$start..($start + 15)]
                break
            }
            $note = $next
        }
        if ($null -ne $buildId) { break }
    }
}
if ($null -eq $buildId) {
    throw 'Dem ELF-Payload fehlt eine gueltige GNU-Build-ID.'
}

$outputDirectory = Split-Path -Parent $OutputFile
if ($outputDirectory -and -not (Test-Path -LiteralPath $outputDirectory)) {
    New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
}

# §108 NKI v2: DevSign-Block (64 Bytes) nach Payload anfügen
# dev_mac = payload_crc32 XOR "NOVD" (0x4E4F5644) — Phase-1-Platzhalter, kein echter Krypto
$sigSize      = [uint32]64
$devsignMagic = [byte[]][Text.Encoding]::ASCII.GetBytes("NKTS")
$devsignScheme      = [byte]1         # NOVA_NKI_SCHEME_DEVSIGN
$devsignFlags       = [byte]0
$devsignReserved    = [byte[]]@(0, 0)
$devsignKeyId       = [uint32]0x44455601   # NOVA_NKI_DEVSIGN_KEY_ID  "DEV\x01"
$devsignRevGen      = [uint32]0            # Revocation-Generation 0 = aktuell
$devMac             = [uint32]($crc32 -bxor [uint32]0x4E4F5644)
# sig_data: dev_mac[4] dann 28 Null-Bytes
$sigData = [byte[]]::new(32)
$devMacBytes = [BitConverter]::GetBytes($devMac)
[Array]::Copy($devMacBytes, 0, $sigData, 0, 4)
# build_id (16 Bytes) — Spiegel des NKI-Header-Feldes
$sigBuildId = [byte[]]$buildId

$stream = [IO.File]::Open($OutputFile, [IO.FileMode]::Create, [IO.FileAccess]::Write)
$writer = [IO.BinaryWriter]::new($stream)
try {
    # NKI v2 Header (64 Bytes)
    $writer.Write([Text.Encoding]::ASCII.GetBytes("NOVANKI"))
    $writer.Write([byte]0)
    $writer.Write([uint32]2)                  # Formatversion 2
    $writer.Write([uint32]$headerSize)
    $writer.Write([uint32]1)                  # x86-32
    $writer.Write([uint32]3)                  # Build-ID und Nova-Metadaten verbindlich
    $writer.Write($entryPoint)
    $writer.Write($entryPoint)
    $writer.Write([uint32]$payload.Length)
    $writer.Write($compressionId)             # §113: 0=keine Kompression, 1=LZ4
    $writer.Write($crc32)
    $writer.Write([byte[]]$buildId)
    $writer.Write($sigSize)                   # v2: sig_size = 64 (DevSign-Block)
    # ELF-Payload
    $writer.Write($payload)
    # DevSign-Block (64 Bytes) — NKTS-Signaturcontainer
    $writer.Write($devsignMagic)              # "NKTS" [4]
    $writer.Write($devsignScheme)             # scheme=1 [1]
    $writer.Write($devsignFlags)              # sig_flags=0 [1]
    $writer.Write($devsignReserved)           # reserved[2]
    $writer.Write($devsignKeyId)              # key_id "DEV\x01" [4]
    $writer.Write($devsignRevGen)             # revocation_gen=0 [4]
    $writer.Write($sigData)                   # sig_data[32]: dev_mac[4] + zeros[28]
    $writer.Write($sigBuildId)                # build_id[16]
} finally {
    $writer.Dispose()
    $stream.Dispose()
}

$compressLabel = if ($Compress) { "LZ4-komprimiert, unkomprimiert=$($elfPayload.Length)" } `
                 elseif ($CompressZstd) { "ZSTD-komprimiert, unkomprimiert=$($elfPayload.Length)" } `
                 else { "unkomprimiert" }
Write-Host ("NKI v2: {0} Bytes Payload ({1}), CRC32 {2:X8}, DevSign dev_mac={3:X8}" -f $payload.Length, $compressLabel, $crc32, $devMac)
