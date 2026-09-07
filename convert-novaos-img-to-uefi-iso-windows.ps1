param(
    [string]$InputImage,
    [string]$OutputIso
)

$ErrorActionPreference = "Stop"

function Fail([string]$Message) {
    Write-Host "FEHLER: $Message" -ForegroundColor Red
    exit 1
}

$RepoRoot = $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($RepoRoot)) {
    $RepoRoot = (Get-Location).Path
}

function Resolve-RepoPath([string]$Value,[string]$DefaultRelative) {
    if ([string]::IsNullOrWhiteSpace($Value)) {
        $Value = $DefaultRelative
    }

    if ([IO.Path]::IsPathRooted($Value)) {
        return [IO.Path]::GetFullPath($Value)
    }

    return [IO.Path]::GetFullPath([IO.Path]::Combine($RepoRoot,$Value))
}

function To-WslPath([string]$WindowsPath) {
    if ([string]::IsNullOrWhiteSpace($WindowsPath)) {
        Fail "Leerer Windows-Pfad kann nicht nach WSL konvertiert werden."
    }

    $full = [IO.Path]::GetFullPath($WindowsPath)

    # Standard-Windows-Laufwerk, z. B.
    # C:\recoverboot\nova-os -> /mnt/c/recoverboot/nova-os
    if ($full -match '^([A-Za-z]):\\(.*)$') {
        $drive = $matches[1].ToLowerInvariant()
        $rest = $matches[2].Replace('\','/')
        return "/mnt/$drive/$rest"
    }

    # Laufwerkswurzel, z. B. C:\
    if ($full -match '^([A-Za-z]):\\?$') {
        $drive = $matches[1].ToLowerInvariant()
        return "/mnt/$drive"
    }

    Fail "Nicht unterstuetzter Windows-Pfad fuer WSL: $full"
}

function Invoke-WslBash([string]$Command) {
    & wsl.exe -- bash -lc $Command
    if ($LASTEXITCODE -ne 0) {
        Fail "WSL-Befehl fehlgeschlagen: $Command"
    }
}

$inputPath  = Resolve-RepoPath $InputImage "build\nova-uefi.img"
$outputPath = Resolve-RepoPath $OutputIso "build\nova-uefi.iso"

if (!(Test-Path -LiteralPath $inputPath)) {
    Fail "IMG-Datei nicht gefunden: $inputPath"
}

if (!(Get-Command wsl.exe -ErrorAction SilentlyContinue)) {
    Fail "WSL wurde nicht gefunden."
}

Write-Host "NovaOS Repo: $RepoRoot"
Write-Host "IMG:         $inputPath"
Write-Host "ISO:         $outputPath"

# Genau der Aufruf, der auf deinem System funktioniert.
& wsl.exe -- bash -lc "command -v xorriso >/dev/null 2>&1"
if ($LASTEXITCODE -ne 0) {
    Fail "xorriso ist in der Standard-WSL-Distribution nicht verfuegbar."
}

$oldEap = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$xorrisoVersion = & wsl.exe -- bash -lc "xorriso -version 2>&1 | head -n 1"
$xorrisoExit = $LASTEXITCODE
$ErrorActionPreference = $oldEap

if ($xorrisoExit -ne 0) {
    Fail "xorriso konnte nicht ausgefuehrt werden."
}

Write-Host "xorriso:     $($xorrisoVersion -join ' ')"

# GPT/ESP lesen
$stream = [IO.File]::OpenRead($inputPath)
$reader = New-Object IO.BinaryReader($stream)

$tempWin = $null

try {
    $stream.Position = 512
    $sig = [Text.Encoding]::ASCII.GetString($reader.ReadBytes(8))
    if ($sig -ne "EFI PART") {
        Fail "Kein GPT-Header gefunden."
    }

    $stream.Position = 512 + 72
    [UInt64]$entryLba = $reader.ReadUInt64()
    [UInt32]$entryCount = $reader.ReadUInt32()
    [UInt32]$entrySize = $reader.ReadUInt32()

    $espGuid = [Guid]"C12A7328-F81F-11D2-BA4B-00A0C93EC93B"

    $found = $false
    [UInt64]$first = 0
    [UInt64]$last = 0

    for ($i = 0; $i -lt $entryCount; $i++) {
        [UInt64]$off = ($entryLba * 512) + ([UInt64]$i * $entrySize)

        if ($off + 128 -gt [UInt64]$stream.Length) {
            break
        }

        $stream.Position = [Int64]$off
        $typeBytes = $reader.ReadBytes(16)

        $allZero = $true
        foreach ($b in $typeBytes) {
            if ($b -ne 0) {
                $allZero = $false
                break
            }
        }

        if ($allZero) {
            continue
        }

        $typeGuid = New-Object Guid (,$typeBytes)

        $null = $reader.ReadBytes(16)
        [UInt64]$firstLba = $reader.ReadUInt64()
        [UInt64]$lastLba = $reader.ReadUInt64()

        if ($typeGuid -eq $espGuid) {
            $found = $true
            $first = $firstLba
            $last = $lastLba
            break
        }
    }

    if (!$found) {
        Fail "Keine EFI System Partition im IMG gefunden."
    }

    if ($last -lt $first) {
        Fail "Ungueltige ESP-Grenzen."
    }

    [UInt64]$espOffset = $first * 512
    [UInt64]$espLength = ($last - $first + 1) * 512

    Write-Host "ESP Start:   LBA $first"
    Write-Host "ESP Ende:    LBA $last"
    Write-Host "ESP Groesse: $espLength Bytes"

    $tempName = "novaos-esp-" + [Guid]::NewGuid().ToString("N") + ".img"
    $tempWin = Join-Path ([IO.Path]::GetTempPath()) $tempName

    $out = [IO.File]::Create($tempWin)

    try {
        $stream.Position = [Int64]$espOffset

        $buffer = New-Object byte[] (1024 * 1024)
        [UInt64]$remaining = $espLength

        while ($remaining -gt 0) {
            $want = [int][Math]::Min([UInt64]$buffer.Length, $remaining)
            $n = $stream.Read($buffer, 0, $want)

            if ($n -le 0) {
                Fail "Unerwartetes Dateiende beim Extrahieren der ESP."
            }

            $out.Write($buffer, 0, $n)
            $remaining -= [UInt64]$n
        }
    }
    finally {
        $out.Dispose()
    }
}
finally {
    $reader.Dispose()
    $stream.Dispose()
}

$outputDir = [IO.Path]::GetDirectoryName($outputPath)
if ($outputDir) {
    New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
}

if (Test-Path -LiteralPath $outputPath) {
    Remove-Item -LiteralPath $outputPath -Force
}

$tempEspWsl = To-WslPath $tempWin
$outputWsl = To-WslPath $outputPath

$tempIsoDir = "/tmp/novaos-iso-" + [Guid]::NewGuid().ToString("N")
$efiImageWsl = "$tempIsoDir/efiboot.img"

try {
    Invoke-WslBash "rm -rf '$tempIsoDir'; mkdir -p '$tempIsoDir'"
    Invoke-WslBash "cp '$tempEspWsl' '$efiImageWsl'"

    $buildCmd = "xorriso -as mkisofs -iso-level 3 -R -J -V NOVAOS_UEFI -o '$outputWsl' -eltorito-alt-boot -e efiboot.img -no-emul-boot '$tempIsoDir'"

    Write-Host ""
    Write-Host "Erzeuge UEFI-ISO..."
    Invoke-WslBash $buildCmd

    if (!(Test-Path -LiteralPath $outputPath)) {
        Fail "ISO wurde nicht erzeugt."
    }

    Write-Host ""
    Write-Host "Validiere EFI-El-Torito-Eintrag..."

    $oldEap = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $report = & wsl.exe -- bash -lc "xorriso -indev '$outputWsl' -report_el_torito plain 2>&1"
    $reportExit = $LASTEXITCODE
    $ErrorActionPreference = $oldEap

    if ($reportExit -ne 0) {
        Fail "xorriso konnte die ISO nicht validieren."
    }

    $reportText = ($report -join "`n")

    Write-Host $reportText

    if ($reportText -notmatch "(EFI|0xEF|efiboot)") {
        Fail "Kein EFI-El-Torito-Bootentry erkannt."
    }

    Write-Host ""
    Write-Host "FERTIG" -ForegroundColor Green
    Write-Host "ISO:         $outputPath"
    Write-Host "Groesse:     $((Get-Item -LiteralPath $outputPath).Length) Bytes"
    Write-Host "Bootmodus:   UEFI / El Torito"
}
finally {
    if ($tempWin -and (Test-Path -LiteralPath $tempWin)) {
        Remove-Item -LiteralPath $tempWin -Force -ErrorAction SilentlyContinue
    }

    & wsl.exe -- bash -lc "rm -rf '$tempIsoDir'" 2>$null | Out-Null
}
