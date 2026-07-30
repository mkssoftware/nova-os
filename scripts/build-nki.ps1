param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile
)

$ErrorActionPreference = 'Stop'

$headerSize = 64
$maximumPayloadSize = 40960
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
$sha256 = [Security.Cryptography.SHA256]::Create()
try {
    $buildId = $sha256.ComputeHash($payload)[0..15]
} finally {
    $sha256.Dispose()
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
    $writer.Write([uint32]0)                  # Flags
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
