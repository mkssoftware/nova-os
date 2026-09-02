param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile
)

$ErrorActionPreference = 'Stop'

$headerSize = 64
$maximumPayloadSize = 65536
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

$crc32 = Get-Crc32 -Data $payload
function Get-U16([byte[]]$Data, [int]$Offset) { return [BitConverter]::ToUInt16($Data, $Offset) }
function Get-U32([byte[]]$Data, [int]$Offset) { return [BitConverter]::ToUInt32($Data, $Offset) }

$buildId = $null
if ($payload.Length -ge 52 -and (Get-U32 $payload 0) -eq 0x464C457F) {
    $phoff = Get-U32 $payload 28
    $phentsize = Get-U16 $payload 42
    $phnum = Get-U16 $payload 44
    for ($index = 0; $index -lt $phnum; $index++) {
        $ph = $phoff + ($index * $phentsize)
        if (($ph + 32) -gt $payload.Length) { break }
        if ((Get-U32 $payload $ph) -ne 4) { continue }
        $note = Get-U32 $payload ($ph + 4)
        $noteEnd = $note + (Get-U32 $payload ($ph + 16))
        while (($note + 12) -le $noteEnd -and $noteEnd -le $payload.Length) {
            $nameSize = Get-U32 $payload $note
            $descSize = Get-U32 $payload ($note + 4)
            $noteType = Get-U32 $payload ($note + 8)
            $namePadded = ($nameSize + 3) -band -4
            $descPadded = ($descSize + 3) -band -4
            $next = $note + 12 + $namePadded + $descPadded
            if ($next -gt $noteEnd) { break }
            if ($noteType -eq 3 -and $nameSize -eq 4 -and $descSize -ge 16 -and
                [Text.Encoding]::ASCII.GetString($payload, $note + 12, 3) -eq 'GNU') {
                $start = $note + 12 + $namePadded
                $buildId = $payload[$start..($start + 15)]
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

$stream = [IO.File]::Open($OutputFile, [IO.FileMode]::Create, [IO.FileAccess]::Write)
$writer = [IO.BinaryWriter]::new($stream)
try {
    $writer.Write([Text.Encoding]::ASCII.GetBytes("NOVANKI"))
    $writer.Write([byte]0)
    $writer.Write([uint32]1)                  # Formatversion
    $writer.Write([uint32]$headerSize)
    $writer.Write([uint32]1)                  # x86-32
    $writer.Write([uint32]3)                  # Build-ID und Nova-Metadaten verbindlich
    $writer.Write($entryPoint)
    $writer.Write($entryPoint)
    $writer.Write([uint32]$payload.Length)
    $writer.Write([uint32]0)                  # keine Kompression
    $writer.Write($crc32)
    $writer.Write([byte[]]$buildId)
    $writer.Write([uint32]0)                  # reserviert
    $writer.Write($payload)
} finally {
    $writer.Dispose()
    $stream.Dispose()
}

Write-Host ("NKI v1: {0} Bytes Payload, CRC32 {1:X8}" -f $payload.Length, $crc32)
