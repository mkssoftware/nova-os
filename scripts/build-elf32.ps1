param(
    [Parameter(Mandatory = $true)] [string]$InputFile,
    [Parameter(Mandatory = $true)] [string]$OutputFile,
    # BUGFIX (2026-10-30): $InputFile ist die per `objcopy -O binary` erzeugte
    # flache Binary - OHNE .bss (SHT_NOBITS hat keinen Dateiinhalt, objcopy
    # laesst die Sektion komplett weg). Ohne $BssSize wurden p_filesz UND
    # p_memsz bisher IDENTISCH auf die Dateigroesse gesetzt, wodurch der
    # Bootloader (kernel_loader.c, load_elf32) `image_size = highest-lowest`
    # (aus p_memsz) um die GESAMTE .bss-Groesse UNTERSCHAETZTE - pmm.c's
    # `pmm_reserved_end` lag dadurch WEIT innerhalb des echten, noch lebenden
    # .bss-Bereichs des Kernels, und der PMM vergab Frames, die tatsaechlich
    # noch von Kernel-eigenen statischen Variablen belegt waren (reale, durch
    # Boot-Crash gefundene Korruption, siehe project_c_kernel_migration_
    # status.md "Echter Boot-Bug" zum 010-MEMORY SWAP/COMPRESSION/DEDUP-
    # Batch). $BssSize (Bytes) wird auf p_memsz AUFGESCHLAGEN, p_filesz bleibt
    # unveraendert die reale Dateigroesse - der Bootloader zero-fillt den
    # Differenzbereich bereits korrekt (bytes_zero auf ph->memsz, bytes_copy
    # nur ph->filesz). 0 = alter Default (nur fuer Abwaertskompatibilitaet,
    # ergibt wieder das alte, fehlerhafte Verhalten - IMMER die echte
    # .bss-Groesse des gelinkten .exe uebergeben, z.B. per
    # `objdump -h newkernel.exe` Sektion .bss "Size"-Spalte).
    [uint32]$BssSize = 0
)

$ErrorActionPreference = 'Stop'
$loadAddress = [uint32]0x00100000
$payloadOffset = [uint32]0x00001000
$payload = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile))
$totalMemSize = [uint32]($payload.Length + $BssSize)
$sha256 = [Security.Cryptography.SHA256]::Create()
try {
    $buildId = $sha256.ComputeHash($payload)[0..19]
} finally {
    $sha256.Dispose()
}
$noteOffset = [uint32]116
$noteSize = [uint32]72
$maximumPayloadSize = [uint32]16777216
$maximumElfSize = [uint32]($payloadOffset + $maximumPayloadSize)
if ($payload.Length -eq 0 -or ($payloadOffset + $totalMemSize) -gt $maximumElfSize) {
    throw "ELF32-Image (inkl. .bss) ueberschreitet das 16-MiB-Kernelpayload-Limit."
}

$outputDirectory = Split-Path -Parent $OutputFile
if ($outputDirectory -and -not (Test-Path -LiteralPath $outputDirectory)) {
    New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
}

$stream = [IO.File]::Open($OutputFile, [IO.FileMode]::Create, [IO.FileAccess]::Write)
$writer = [IO.BinaryWriter]::new($stream)
try {
    $writer.Write([byte[]](0x7F, 0x45, 0x4C, 0x46)) # ELF magic
    $writer.Write([byte]1)                          # ELFCLASS32
    $writer.Write([byte]1)                          # ELFDATA2LSB
    $writer.Write([byte]1)                          # EV_CURRENT
    $writer.Write([byte]0)                          # System V ABI
    $writer.Write([byte[]]::new(8))
    $writer.Write([uint16]2)                        # ET_EXEC
    $writer.Write([uint16]3)                        # EM_386
    $writer.Write([uint32]1)                        # EV_CURRENT
    $writer.Write($loadAddress)                     # e_entry
    $writer.Write([uint32]52)                       # e_phoff
    $writer.Write([uint32]0)                        # e_shoff
    $writer.Write([uint32]0)                        # e_flags
    $writer.Write([uint16]52)                       # e_ehsize
    $writer.Write([uint16]32)                       # e_phentsize
    $writer.Write([uint16]2)                        # e_phnum
    $writer.Write([uint16]0)                        # e_shentsize
    $writer.Write([uint16]0)                        # e_shnum
    $writer.Write([uint16]0)                        # e_shstrndx

    $writer.Write([uint32]1)                        # PT_LOAD
    $writer.Write($payloadOffset)
    $writer.Write($loadAddress)                     # p_vaddr
    $writer.Write($loadAddress)                     # p_paddr
    $writer.Write([uint32]$payload.Length)          # p_filesz
    $writer.Write($totalMemSize)                    # p_memsz (inkl. .bss, siehe $BssSize)
    $writer.Write([uint32]5)                        # PF_R | PF_X
    $writer.Write([uint32]0x1000)                   # p_align

    $writer.Write([uint32]4)                        # PT_NOTE
    $writer.Write($noteOffset)
    $writer.Write([uint32]0)                        # p_vaddr
    $writer.Write([uint32]0)                        # p_paddr
    $writer.Write($noteSize)                        # p_filesz
    $writer.Write($noteSize)                        # p_memsz
    $writer.Write([uint32]4)                        # PF_R
    $writer.Write([uint32]4)                        # p_align

    $writer.Write([uint32]4)                        # namesz: "GNU\0"
    $writer.Write([uint32]20)                       # descsz: 160-Bit Build-ID
    $writer.Write([uint32]3)                        # NT_GNU_BUILD_ID
    $writer.Write([byte[]](0x47, 0x4E, 0x55, 0x00))
    $writer.Write([byte[]]$buildId)

    $writer.Write([uint32]5)                        # namesz: "NOVA\0"
    $writer.Write([uint32]16)                       # descsz
    $writer.Write([uint32]0x4E4F5601)               # NT_NOVA_REQUIREMENTS
    $writer.Write([byte[]](0x4E, 0x4F, 0x56, 0x41, 0x00, 0x00, 0x00, 0x00))
    $writer.Write([uint32]1)                        # Metadatenversion
    $writer.Write([uint32]0x00010000)               # minimale Loader-ABI 1.0
    $writer.Write([uint32]1)                        # CPUID.1:EDX FPU erforderlich
    $writer.Write([uint32]0)                        # keine CPUID.1:ECX-Pflichtbits

    $writer.Write([byte[]]::new([int]($payloadOffset - $stream.Position)))
    $writer.Write($payload)
} finally {
    $writer.Dispose()
    $stream.Dispose()
}

$buildIdText = [BitConverter]::ToString($buildId).Replace('-', '')
Write-Host ("ELF32: {0} Bytes (p_filesz), p_memsz=0x{1:X8} (inkl. {2} Bytes .bss), PT_LOAD 0x{3:X8}, Entry 0x{4:X8}, Build-ID {5}" -f ($payloadOffset + $payload.Length), $totalMemSize, $BssSize, $loadAddress, $loadAddress, $buildIdText)
