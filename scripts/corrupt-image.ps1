param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile,

    [Parameter(Mandatory = $true)]
    [int64]$Offset
)

$ErrorActionPreference = 'Stop'
$source = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile))
if ($Offset -lt 0 -or $Offset -ge $source.LongLength) {
    throw "Offset $Offset liegt außerhalb des Images."
}

$source[$Offset] = $source[$Offset] -bxor 0x5A
[IO.File]::WriteAllBytes($OutputFile, $source)
