param(
    [Parameter(Mandatory = $true)][string]$VariablesFile,
    [Parameter(Mandatory = $true)][string]$CodeFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)

$ErrorActionPreference = 'Stop'
$outputDirectory = Split-Path -Parent $OutputFile
if ($outputDirectory) {
    New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null
}

$output = [System.IO.File]::Create($OutputFile)
try {
    foreach ($inputPath in @($VariablesFile, $CodeFile)) {
        $input = [System.IO.File]::OpenRead($inputPath)
        try { $input.CopyTo($output) } finally { $input.Dispose() }
    }
} finally {
    $output.Dispose()
}

if ((Get-Item -LiteralPath $OutputFile).Length -ne 4194304) {
    throw 'Das zusammengesetzte EDK2-Abbild muss exakt 4 MiB gross sein.'
}
