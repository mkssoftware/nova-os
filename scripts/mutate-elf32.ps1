param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputFile,

    [Parameter(Mandatory = $true)]
    [ValidateSet('Overlap', 'Bounds', 'WritableExecutable', 'MalformedNote', 'UnsupportedCpu')]
    [string]$Mode
)

$bytes = [System.IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile))
if ($bytes.Length -lt 116) {
    throw 'Das ELF32-Image ist zu klein fuer zwei Programmheader.'
}

function Set-U16([byte[]]$Data, [int]$Offset, [uint16]$Value) {
    [BitConverter]::GetBytes($Value).CopyTo($Data, $Offset)
}

function Set-U32([byte[]]$Data, [int]$Offset, [uint32]$Value) {
    [BitConverter]::GetBytes($Value).CopyTo($Data, $Offset)
}

switch ($Mode) {
    'Overlap' {
        Set-U16 $bytes 44 2
        [Array]::Copy($bytes, 52, $bytes, 84, 32)
    }
    'Bounds' {
        Set-U32 $bytes 68 0x7FFFFFFF
        Set-U32 $bytes 72 0x7FFFFFFF
    }
    'WritableExecutable' {
        Set-U32 $bytes 76 7
    }
    'MalformedNote' {
        # Zweiter Program Header ist PT_NOTE; descsz liegt bei Note-Offset + 4.
        Set-U32 $bytes 120 0x7FFFFFFF
    }
    'UnsupportedCpu' {
        # NOVA-Deskriptor: reserviertes CPUID.1:EDX-Bit 10 erzwingen.
        Set-U32 $bytes 180 0x00000400
    }
}

$outputDirectory = Split-Path -Parent $OutputFile
if ($outputDirectory) {
    [System.IO.Directory]::CreateDirectory($outputDirectory) | Out-Null
}
[System.IO.File]::WriteAllBytes($OutputFile, $bytes)
Write-Host "ELF32-Testmutation erstellt: $Mode"
