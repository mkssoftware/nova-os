param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile,

    # §113: LZ4-Blockformat-Kompression (Literal-Only, kein echter Back-Reference-Kompressor)
    [switch]$Compress,

    # §114: ZSTD-Frame-Kompression (Raw-Block-Only, kein Huffman/FSE-Kompressor)
    [switch]$CompressZstd,

    # §115: GZIP-Frame-Kompression (DEFLATE Stored-Blocks, kein eigentlicher Kompressor)
    [switch]$CompressGzip,

    # §116: GZIP-Frame-Kompression mit echtem DEFLATE-Kompressor (.NET GZipStream, erzeugt BTYPE=10)
    [switch]$CompressGzipReal,

    # §117: HMAC-SHA-256-Signatur (Phase-2 Dev-Key) statt CRC-XOR-MAC (Phase-1)
    [switch]$SignHmacSha256
)

$ErrorActionPreference = 'Stop'
$compressionSwitches = @($Compress.IsPresent, $CompressZstd.IsPresent, $CompressGzip.IsPresent, $CompressGzipReal.IsPresent) | Where-Object { $_ }
if ($compressionSwitches.Count -gt 1) {
    throw 'Die Schalter -Compress (LZ4), -CompressZstd (ZSTD), -CompressGzip (GZIP) und -CompressGzipReal schliessen sich gegenseitig aus.'
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

# §115: GZIP-Kompressor (DEFLATE Stored-Blocks — gueltige RFC-1952-Frame, kein eigentlicher Kompressor)
# Frame-Layout: GZIP-Header(10) + DEFLATE-Stored-Bloecke + GZIP-Footer(CRC32+ISIZE)
# DEFLATE Stored-Block-Layout: Byte=(BFINAL|0x00), LEN(2 LE), NLEN=~LEN(2 LE), Daten
function Compress-GzipStored([byte[]]$data) {
    $n = $data.Length
    $out = [Collections.Generic.List[byte]]::new()
    # GZIP-Header (10 Bytes): Magic(2) + CM=8(1) + FLG=0(1) + MTIME=0(4) + XFL=0(1) + OS=0xFF(1)
    $out.Add(0x1F); $out.Add(0x8B)    # Magic
    $out.Add(0x08)                     # CM = DEFLATE
    $out.Add(0x00)                     # FLG = 0 (keine optionalen Felder)
    $out.Add(0x00); $out.Add(0x00); $out.Add(0x00); $out.Add(0x00)  # MTIME = 0
    $out.Add(0x00)                     # XFL = 0
    $out.Add(0xFF)                     # OS = 0xFF (unbekannt)
    # DEFLATE Stored-Bloecke (max 65535 Bytes pro Block)
    $offset = 0
    do {
        $chunk = [Math]::Min(65535, $n - $offset)
        $isLast = ($offset + $chunk -ge $n)
        $out.Add([byte](if ($isLast) { 1 } else { 0 }))  # BFINAL | BTYPE=00
        $out.Add([byte]($chunk -band 0xFF))               # LEN lo
        $out.Add([byte](($chunk -shr 8) -band 0xFF))      # LEN hi
        $nlenVal = $chunk -bxor 0xFFFF                    # NLEN = ~LEN
        $out.Add([byte]($nlenVal -band 0xFF))             # NLEN lo
        $out.Add([byte](($nlenVal -shr 8) -band 0xFF))    # NLEN hi
        for ($i = 0; $i -lt $chunk; $i++) { $out.Add($data[$offset + $i]) }
        $offset += $chunk
    } while ($offset -lt $n)
    # GZIP-Footer: CRC32 (LE) + ISIZE = unkomprimierte Groesse (LE)
    $crcBytes = [BitConverter]::GetBytes((Get-Crc32 -Data $data))
    $isizeBytes = [BitConverter]::GetBytes([uint32]$n)
    foreach ($b in $crcBytes)  { $out.Add($b) }
    foreach ($b in $isizeBytes) { $out.Add($b) }
    return $out.ToArray()
}

# §116: GZIP-Kompressor mit echtem DEFLATE (.NET GZipStream — erzeugt BTYPE=10 Dynamic-Huffman-Bloecke)
function Compress-GzipReal([byte[]]$data) {
    $ms = [IO.MemoryStream]::new()
    $gz = [IO.Compression.GZipStream]::new($ms, [IO.Compression.CompressionLevel]::Optimal)
    $gz.Write($data, 0, $data.Length)
    $gz.Dispose()
    return $ms.ToArray()
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
if ($CompressGzip) {
    $gzipFrame  = Compress-GzipStored -data $payload
    $sizePrefix = [BitConverter]::GetBytes([uint32]$payload.Length)
    $compressed = [byte[]]::new(4 + $gzipFrame.Length)
    [Array]::Copy($sizePrefix, 0, $compressed, 0, 4)
    [Array]::Copy($gzipFrame,  0, $compressed, 4, $gzipFrame.Length)
    $payload = $compressed
    $compressionId = [uint32]3   # NOVA_NKI_COMPRESSION_GZIP
}
if ($CompressGzipReal) {
    $gzipFrame  = Compress-GzipReal -data $payload
    $sizePrefix = [BitConverter]::GetBytes([uint32]$payload.Length)
    $compressed = [byte[]]::new(4 + $gzipFrame.Length)
    [Array]::Copy($sizePrefix, 0, $compressed, 0, 4)
    [Array]::Copy($gzipFrame,  0, $compressed, 4, $gzipFrame.Length)
    $payload = $compressed
    $compressionId = [uint32]3   # NOVA_NKI_COMPRESSION_GZIP
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

# §117: HMAC-SHA-256 Dev-Key (32 Bytes, identisch mit g_hmac_dev_key in kernel_loader.c)
$hmacDevKey = [byte[]](
    0x4E, 0x6F, 0x76, 0x61, 0x4F, 0x53, 0x44, 0x65,
    0x76, 0x4B, 0x65, 0x79, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62,
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A
)
if ($SignHmacSha256) {
    # NKI-Header-Bytes exakt so aufbauen wie der Schreibblock (64 Bytes)
    $hdrMs = [IO.MemoryStream]::new()
    $hdrW  = [IO.BinaryWriter]::new($hdrMs)
    $hdrW.Write([Text.Encoding]::ASCII.GetBytes("NOVANKI"))
    $hdrW.Write([byte]0)
    $hdrW.Write([uint32]2)
    $hdrW.Write([uint32]$headerSize)
    $hdrW.Write([uint32]1)
    $hdrW.Write([uint32]3)
    $hdrW.Write($entryPoint)
    $hdrW.Write($entryPoint)
    $hdrW.Write([uint32]$payload.Length)
    $hdrW.Write($compressionId)
    $hdrW.Write($crc32)
    $hdrW.Write([byte[]]$buildId)
    $hdrW.Write([uint32]64)    # sig_size = 64
    $hdrW.Flush()
    $headerBytes = $hdrMs.ToArray()
    $hdrW.Dispose(); $hdrMs.Dispose()
    # HMAC-SHA-256 über (Header[64] + Payload[image_size])
    $hmacMsg = [byte[]]::new($headerBytes.Length + $payload.Length)
    [Array]::Copy($headerBytes, 0, $hmacMsg, 0, $headerBytes.Length)
    [Array]::Copy($payload,     0, $hmacMsg, $headerBytes.Length, $payload.Length)
    $hmacObj    = [Security.Cryptography.HMACSHA256]::new($hmacDevKey)
    $hmacResult = $hmacObj.ComputeHash($hmacMsg)
    $hmacObj.Dispose()
    # DevSign-Felder auf Phase-2 überschreiben
    $devsignScheme = [byte]2               # NOVA_NKI_SCHEME_HMACSHA256
    $devsignKeyId  = [uint32]0x48534832    # NOVA_NKI_HMACSHA256_KEY_ID "HSH2"
    $sigData       = $hmacResult           # 32 Bytes HMAC-SHA-256
}

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
                 elseif ($CompressGzip) { "GZIP-Stored-komprimiert, unkomprimiert=$($elfPayload.Length)" } `
                 elseif ($CompressGzipReal) { "GZIP-DEFLATE-komprimiert, unkomprimiert=$($elfPayload.Length)" } `
                 else { "unkomprimiert" }
$signLabel = if ($SignHmacSha256) { "scheme=2 HMAC-SHA-256" } else { "scheme=1 CRC-XOR dev_mac=$($devMac.ToString('X8'))" }
Write-Host ("NKI v2: {0} Bytes Payload ({1}), CRC32 {2:X8}, DevSign {3}" -f $payload.Length, $compressLabel, $crc32, $signLabel)
