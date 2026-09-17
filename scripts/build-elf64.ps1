param(
    [Parameter(Mandatory = $true)] [string]$InputFile,
    [Parameter(Mandatory = $true)] [string]$OutputFile
)

$ErrorActionPreference = 'Stop'
$loadAddress = [uint64]0x00200000
$payloadOffset = [uint64]0x1000
$payload = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile))
$sha256 = [Security.Cryptography.SHA256]::Create()
try { $buildId = $sha256.ComputeHash($payload)[0..19] } finally { $sha256.Dispose() }
$noteOffset = [uint64]176
$noteSize = [uint64]72
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
    $writer.Write([uint16]2)
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

    $writer.Write([uint32]4)                    # PT_NOTE
    $writer.Write([uint32]4)                    # PF_R
    $writer.Write($noteOffset)
    $writer.Write([uint64]0)
    $writer.Write([uint64]0)
    $writer.Write($noteSize)
    $writer.Write($noteSize)
    $writer.Write([uint64]4)

    $writer.Write([uint32]4)                    # namesz: GNU\0
    $writer.Write([uint32]20)                   # 160-Bit Build-ID
    $writer.Write([uint32]3)                    # NT_GNU_BUILD_ID
    $writer.Write([byte[]](0x47,0x4E,0x55,0x00))
    $writer.Write([byte[]]$buildId)

    $writer.Write([uint32]5)                    # namesz: NOVA\0
    $writer.Write([uint32]16)
    $writer.Write([uint32]0x4E4F5601)           # NT_NOVA_REQUIREMENTS
    $writer.Write([byte[]](0x4E,0x4F,0x56,0x41,0x00,0x00,0x00,0x00))
    $writer.Write([uint32]1)                    # Metadatenversion
    $writer.Write([uint32]0x00010000)           # minimale Loader-ABI 1.0
    $writer.Write([uint32]1)                    # CPUID.1:EDX FPU
    $writer.Write([uint32]0)
    $writer.Write([byte[]]::new([int]($payloadOffset - $stream.Position)))
    $writer.Write($payload)
} finally {
    $writer.Dispose()
    $stream.Dispose()
}
Write-Host ("ELF64-Testkernel: {0} Bytes, Entry 0x{1:X}" -f ($payloadOffset + $payload.Length), $loadAddress)
