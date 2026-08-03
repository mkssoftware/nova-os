param(
    [Parameter(Mandatory = $true)] [string]$InputFile,
    [Parameter(Mandatory = $true)] [string]$OutputFile
)

$ErrorActionPreference = 'Stop'
$loadAddress = [uint64]0x00200000
$payloadOffset = [uint64]0x1000
$payload = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile))
if ($payload.Length -eq 0 -or ($payloadOffset + $payload.Length) -gt 40960) {
    throw 'ELF64-Testimage liegt ausserhalb des Loaderlimits.'
}
$directory = Split-Path -Parent $OutputFile
if ($directory) { [IO.Directory]::CreateDirectory($directory) | Out-Null }
$stream = [IO.File]::Open($OutputFile, [IO.FileMode]::Create, [IO.FileAccess]::Write)
$writer = [IO.BinaryWriter]::new($stream)
try {
    $writer.Write([byte[]](0x7F,0x45,0x4C,0x46,2,1,1,0))
    $writer.Write([byte[]]::new(8))
    $writer.Write([uint16]2)
    $writer.Write([uint16]62)
    $writer.Write([uint32]1)
    $writer.Write($loadAddress)
    $writer.Write([uint64]64)
    $writer.Write([uint64]0)
    $writer.Write([uint32]0)
    $writer.Write([uint16]64)
    $writer.Write([uint16]56)
    $writer.Write([uint16]1)
    $writer.Write([uint16]0)
    $writer.Write([uint16]0)
    $writer.Write([uint16]0)
    $writer.Write([uint32]1)
    $writer.Write([uint32]5)
    $writer.Write($payloadOffset)
    $writer.Write($loadAddress)
    $writer.Write($loadAddress)
    $writer.Write([uint64]$payload.Length)
    $writer.Write([uint64]$payload.Length)
    $writer.Write([uint64]0x1000)
    $writer.Write([byte[]]::new([int]($payloadOffset - $stream.Position)))
    $writer.Write($payload)
} finally {
    $writer.Dispose()
    $stream.Dispose()
}
Write-Host ("ELF64-Testkernel: {0} Bytes, Entry 0x{1:X}" -f ($payloadOffset + $payload.Length), $loadAddress)
