param(
    [Parameter(Mandatory = $false)]
    [string]$InputImage = "build\nova-uefi.img",

    [Parameter(Mandatory = $false)]
    [string]$OutputIso = "build\nova-uefi.iso",

    [Parameter(Mandatory = $false)]
    [string]$Xorriso = "xorriso"
)

$ErrorActionPreference = "Stop"

function Fail([string]$Message) {
    Write-Host "FEHLER: $Message" -ForegroundColor Red
    exit 1
}

function Read-U32LE([System.IO.BinaryReader]$Reader) {
    return $Reader.ReadUInt32()
}

function Read-U64LE([System.IO.BinaryReader]$Reader) {
    return $Reader.ReadUInt64()
}

$inputPath = [System.IO.Path]::GetFullPath($InputImage)
$outputPath = [System.IO.Path]::GetFullPath($OutputIso)

if (!(Test-Path -LiteralPath $inputPath)) {
    Fail "IMG-Datei nicht gefunden: $inputPath"
}

try {
    $null = & $Xorriso -version 2>$null
} catch {
    Fail "xorriso wurde nicht gefunden. Installiere es z. B. unter WSL/Ubuntu mit: sudo apt install xorriso"
}

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("novaos-iso-" + [Guid]::NewGuid().ToString("N"))
$isoRoot = Join-Path $tempRoot "iso"
$efiBootImage = Join-Path $isoRoot "efiboot.img"

try {
    New-Item -ItemType Directory -Force -Path $isoRoot | Out-Null

    # GPT lesen.
    $stream = [System.IO.File]::Open(
        $inputPath,
        [System.IO.FileMode]::Open,
        [System.IO.FileAccess]::Read,
        [System.IO.FileShare]::Read
    )
    $reader = New-Object System.IO.BinaryReader($stream)

    try {
        if ($stream.Length -lt 1024) {
            Fail "IMG ist zu klein, um ein gueltiges GPT-Abbild zu sein."
        }

        # GPT Header liegt bei LBA 1, also Byte 512.
        $stream.Position = 512
        $signature = [System.Text.Encoding]::ASCII.GetString($reader.ReadBytes(8))
        if ($signature -ne "EFI PART") {
            Fail "Kein GPT-Header gefunden. Erwartet wurde 'EFI PART' bei LBA 1."
        }

        # GPT Header Felder.
        $stream.Position = 512 + 72
        $partitionEntryLba = Read-U64LE $reader
        $partitionEntryCount = Read-U32LE $reader
        $partitionEntrySize = Read-U32LE $reader

        if ($partitionEntryCount -lt 1 -or $partitionEntrySize -lt 128) {
            Fail "Ungueltige GPT-Partitionstabelle."
        }

        $espTypeGuid = [Guid]"C12A7328-F81F-11D2-BA4B-00A0C93EC93B"
        $espFound = $false
        [UInt64]$espFirstLba = 0
        [UInt64]$espLastLba = 0

        for ($i = 0; $i -lt $partitionEntryCount; $i++) {
            $entryOffset = ($partitionEntryLba * 512) + ($i * $partitionEntrySize)
            if ($entryOffset + 128 -gt $stream.Length) {
                break
            }

            $stream.Position = [Int64]$entryOffset
            $typeBytes = $reader.ReadBytes(16)

            # Leerer GPT-Eintrag?
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

            $null = $reader.ReadBytes(16)  # Unique partition GUID
            $firstLba = Read-U64LE $reader
            $lastLba = Read-U64LE $reader

            if ($typeGuid -eq $espTypeGuid) {
                $espFound = $true
                $espFirstLba = $firstLba
                $espLastLba = $lastLba
                break
            }
        }

        if (!$espFound) {
            Fail "Keine EFI System Partition (ESP) im IMG gefunden."
        }

        if ($espLastLba -lt $espFirstLba) {
            Fail "Ungueltige ESP-Grenzen im GPT."
        }

        [UInt64]$espSectorCount = ($espLastLba - $espFirstLba + 1)
        [UInt64]$espOffset = $espFirstLba * 512
        [UInt64]$espLength = $espSectorCount * 512

        if ($espOffset + $espLength -gt [UInt64]$stream.Length) {
            Fail "ESP liegt ausserhalb der IMG-Datei."
        }

        Write-Host "NovaOS UEFI IMG gefunden"
        Write-Host "  IMG:       $inputPath"
        Write-Host "  ESP Start: LBA $espFirstLba"
        Write-Host "  ESP Ende:  LBA $espLastLba"
        Write-Host "  ESP Groesse: $espLength Bytes"

        # Komplette FAT32-ESP extrahieren. Darin liegen beim aktuellen NovaOS-Build:
        # EFI/BOOT/BOOTX64.EFI sowie NOVA.NKI und/oder KERNEL.ELF.
        $stream.Position = [Int64]$espOffset

        $out = [System.IO.File]::Open(
            $efiBootImage,
            [System.IO.FileMode]::Create,
            [System.IO.FileAccess]::Write,
            [System.IO.FileShare]::None
        )

        try {
            $buffer = New-Object byte[] (1024 * 1024)
            [UInt64]$remaining = $espLength

            while ($remaining -gt 0) {
                $wanted = [int][Math]::Min([UInt64]$buffer.Length, $remaining)
                $read = $stream.Read($buffer, 0, $wanted)
                if ($read -le 0) {
                    Fail "Unerwartetes Dateiende beim Extrahieren der ESP."
                }

                $out.Write($buffer, 0, $read)
                $remaining -= [UInt64]$read
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

    # Zielordner anlegen.
    $outputDir = [System.IO.Path]::GetDirectoryName($outputPath)
    if ($outputDir) {
        New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
    }

    # UEFI-bootfaehige El-Torito-ISO erzeugen.
    #
    # -e efiboot.img     = EFI El-Torito Boot Image
    # -no-emul-boot      = keine Disketten-/HDD-Emulation
    # -V NOVAOS_UEFI     = Volume Label
    #
    # Das efiboot.img ist die originale FAT32-ESP des NovaOS-Images und
    # enthaelt dadurch exakt den Bootloader und die Kernel-Payloads des Builds.
    & $Xorriso `
        -as mkisofs `
        -iso-level 3 `
        -R `
        -V "NOVAOS_UEFI" `
        -o $outputPath `
        -eltorito-alt-boot `
        -e "efiboot.img" `
        -no-emul-boot `
        $isoRoot

    if ($LASTEXITCODE -ne 0) {
        Fail "xorriso konnte die ISO nicht erzeugen."
    }

    if (!(Test-Path -LiteralPath $outputPath)) {
        Fail "ISO wurde nicht erzeugt."
    }

    $isoSize = (Get-Item -LiteralPath $outputPath).Length

    Write-Host ""
    Write-Host "FERTIG" -ForegroundColor Green
    Write-Host "  ISO: $outputPath"
    Write-Host "  Groesse: $isoSize Bytes"
    Write-Host ""
    Write-Host "QEMU-Test, Beispiel:"
    Write-Host '  qemu-system-x86_64 -m 512M -drive if=pflash,format=raw,readonly=on,file=OVMF_CODE.fd -cdrom "' + $outputPath + '"'
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force -ErrorAction SilentlyContinue
    }
}
