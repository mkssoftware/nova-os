param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$EfiApplication,
    [Parameter(Mandatory=$true)][string]$Nki,
    [Parameter(Mandatory=$true)][string]$Elf32,
    [Parameter(Mandatory=$true)][string]$Elf64,
    [Parameter(Mandatory=$true)][string]$ImageBuilder,
    # §113: Pfad zu build-nki.ps1 für komprimierte NKI-Testfälle (optional)
    [string]$NkiBuilder
)

$ErrorActionPreference='Stop'
$tempRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
[IO.Directory]::CreateDirectory($tempRoot)|Out-Null
$tempDir=[IO.Path]::Combine($tempRoot,'nova-uefi-kernel-validation-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($tempDir)|Out-Null
$oldTmp=$env:TMP;$oldTemp=$env:TEMP
$env:TMP=$tempDir;$env:TEMP=$tempDir

function Write-PatchedU32([string]$source,[string]$destination,[int]$offset,[uint32]$value) {
    $bytes=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($source))
    if($offset-lt0-or($offset+4)-gt$bytes.Length){throw "Patch-Offset liegt ausserhalb von $source"}
    $le=[BitConverter]::GetBytes($value)
    [Array]::Copy($le,0,$bytes,$offset,4)
    [IO.File]::WriteAllBytes($destination,$bytes)
}

function Write-TruncatedCopy([string]$source,[string]$destination,[int]$removeBytes) {
    $bytes=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($source))
    if($removeBytes-le0-or$removeBytes-ge$bytes.Length){throw "TruncateBytes $removeBytes ungueltig fuer $source"}
    $truncated=[byte[]]::new($bytes.Length-$removeBytes)
    [Array]::Copy($bytes,0,$truncated,0,$truncated.Length)
    [IO.File]::WriteAllBytes($destination,$truncated)
}

function Write-CorruptCopy([string]$source,[string]$destination,[int]$offset) {
    $bytes=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($source))
    if($offset-lt0-or$offset-ge$bytes.Length){throw "Korruptionsoffset liegt ausserhalb von $source"}
    $bytes[$offset]=$bytes[$offset]-bxor0x5A
    [IO.File]::WriteAllBytes($destination,$bytes)
}

function Invoke-SuccessCase([string]$name,[string]$image,[string[]]$requiredMarkers) {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $serial=[IO.Path]::Combine($tempDir,$name+'.serial.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M','-smp','4',
        '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
        '-drive',"format=raw,file=$image,if=ide",'-display','none','-monitor','none',
        '-serial',"file:$serial",'-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            $debugContent=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
            $serialContent=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
            $content=$debugContent+$serialContent
            if($process.HasExited){
                $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
                throw "${name}: QEMU wurde vor NOVA_KERNEL_READY beendet. $detail"
            }
        } while($content-notlike'*NOVA_KERNEL_READY*'-and[DateTime]::UtcNow-lt$deadline)
        if($content-notlike'*NOVA_KERNEL_READY*'){throw "${name}: Timeout — NOVA_KERNEL_READY nicht erreicht."}
        foreach($marker in $requiredMarkers){
            if($content-notlike"*$marker*"){throw "${name}: Pflichtmarkierung fehlt: $marker"}
        }
        if($content-like'*UEFI:KERNEL-VALIDATION-ERROR*'){throw "${name}: gueltiger Kernel wurde abgewiesen."}
        Write-Host "$name`: Kernel erfolgreich gestartet, alle Pflichtmarkierungen vorhanden"
    } finally {
        if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
        $process.WaitForExit()
        $process.Dispose()
    }
}

function Invoke-ValidationCase([string]$name,[string]$image,[string]$forbiddenMarker) {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $serial=[IO.Path]::Combine($tempDir,$name+'.serial.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M','-smp','4',
        '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
        '-drive',"format=raw,file=$image,if=ide",'-display','none','-monitor','none',
        '-serial',"file:$serial",'-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            $debugContent=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
            $serialContent=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
            $content=$debugContent+$serialContent
            if($process.HasExited){
                $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
                throw "${name}: QEMU wurde vor der erwarteten Ablehnung beendet. $detail"
            }
        } while($content-notlike'*UEFI:KERNEL-START-FAILED*'-and[DateTime]::UtcNow-lt$deadline)
        if($content-notlike'*UEFI:KERNEL-VALIDATION-ERROR*'){throw "${name}: Validierungsfehler wurde nicht gemeldet."}
        if($content-notlike'*UEFI:KERNEL-START-FAILED*'){throw "${name}: kontrollierter Startabbruch wurde nicht gemeldet."}
        if($content-like'*UEFI:KERNEL-HANDOFF-READY*'){throw "${name}: ungueltiger Kernel erreichte den Handoff."}
        if($forbiddenMarker-and$content-like"*$forbiddenMarker*"){throw "${name}: unzulaessiger Fallback wurde ausgefuehrt."}
        Write-Host "$name`: ungueltiger Kernel kontrolliert abgewiesen"
    } finally {
        if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
        $process.WaitForExit()
        $process.Dispose()
    }
}

function Invoke-RecoveryCase([string]$name,[string]$image) {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $serial=[IO.Path]::Combine($tempDir,$name+'.serial.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M','-smp','4',
        '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
        '-drive',"format=raw,file=$image,if=ide",'-display','none','-monitor','none',
        '-serial',"file:$serial",'-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            $debugContent=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
            $serialContent=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
            $content=$debugContent+$serialContent
            if($process.HasExited){
                $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
                throw "${name}: QEMU wurde vor dem Recovery-Handoff beendet. $detail"
            }
        } while($content-notlike'*NOVA_KERNEL_READY*'-and[DateTime]::UtcNow-lt$deadline)
        foreach($marker in @('UEFI:PRIMARY-KERNEL-VALIDATION-ERROR','UEFI:AUTOMATIC-RECOVERY-SELECTED',
                             'UEFI:RECOVERY-NKI-VALIDATED','UEFI:KERNEL-HANDOFF-READY',
                             'NOVA: Recovery-Modus aus NBHP/BIB aktiv','NOVA_KERNEL_READY')){
            if($content-notlike"*$marker*"){throw "${name}: erwartete Markierung fehlt: $marker"}
        }
        if($content-like'*UEFI:ELF32-DIRECT-VALIDATED*'){
            throw "${name}: beschädigtes Haupt-NKI wurde unzulässig durch das normale ELF ersetzt."
        }
        Write-Host "$name`: beschädigter Hauptkernel kontrolliert über RECOVERY.NKI gestartet"
    } finally {
        if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
        $process.WaitForExit()
        $process.Dispose()
    }
}

function Invoke-BackupCase([string]$name,[string]$image) {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $serial=[IO.Path]::Combine($tempDir,$name+'.serial.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M','-smp','4',
        '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
        '-drive',"format=raw,file=$image,if=ide",'-display','none','-monitor','none',
        '-serial',"file:$serial",'-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            $debugContent=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
            $serialContent=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
            $content=$debugContent+$serialContent
            if($process.HasExited){
                $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
                throw "${name}: QEMU wurde vor dem Backup-Handoff beendet. $detail"
            }
        } while($content-notlike'*NOVA_KERNEL_READY*'-and[DateTime]::UtcNow-lt$deadline)
        foreach($marker in @('UEFI:PRIMARY-KERNEL-VALIDATION-ERROR','UEFI:AUTOMATIC-BACKUP-SELECTED',
                             'UEFI:BACKUP-NKI-VALIDATED','UEFI:KERNEL-HANDOFF-READY',
                             'NOVA: Backup-Kernelgeneration aus NBHP/BIB aktiv','NOVA_KERNEL_READY')){
            if($content-notlike"*$marker*"){throw "${name}: erwartete Markierung fehlt: $marker"}
        }
        foreach($forbidden in @('UEFI:AUTOMATIC-RECOVERY-SELECTED','UEFI:ELF32-DIRECT-VALIDATED')){
            if($content-like"*$forbidden*"){throw "${name}: unzulässiger Pfad wurde ausgeführt: $forbidden"}
        }
        Write-Host "$name`: beschädigter Hauptkernel kontrolliert über BACKUP.NKI gestartet"
    } finally {
        if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
        $process.WaitForExit()
        $process.Dispose()
    }
}

try {
    $badNki=[IO.Path]::Combine($tempDir,'NOVA.NKI')
    $badElf32=[IO.Path]::Combine($tempDir,'KERNEL.ELF')
    $badElf64=[IO.Path]::Combine($tempDir,'KERNEL64.ELF')
    Write-CorruptCopy $Nki $badNki 64
    Write-CorruptCopy $Elf32 $badElf32 18
    Write-CorruptCopy $Elf64 $badElf64 18

    $nkiImage=[IO.Path]::Combine($tempDir,'bad-nki.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $badNki -KernelElf $Elf32 -OutputImage $nkiImage | Out-Null
    Invoke-ValidationCase 'bad-nki' $nkiImage 'UEFI:ELF32-DIRECT-VALIDATED'

    $backupImage=[IO.Path]::Combine($tempDir,'bad-primary-with-backup.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $badNki -KernelElf $Elf32 `
        -BackupKernelImage $Nki -RecoveryKernelImage $Nki -OutputImage $backupImage | Out-Null
    Invoke-BackupCase 'bad-primary-with-backup' $backupImage

    $recoveryImage=[IO.Path]::Combine($tempDir,'bad-primary-with-recovery.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $badNki -KernelElf $Elf32 `
        -RecoveryKernelImage $Nki -OutputImage $recoveryImage | Out-Null
    Invoke-RecoveryCase 'bad-primary-with-recovery' $recoveryImage

    $elf32Image=[IO.Path]::Combine($tempDir,'bad-elf32.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelElf $badElf32 -OutputImage $elf32Image | Out-Null
    Invoke-ValidationCase 'bad-elf32' $elf32Image ''

    $elf64Image=[IO.Path]::Combine($tempDir,'bad-elf64.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelElf64 $badElf64 -OutputImage $elf64Image | Out-Null
    Invoke-ValidationCase 'bad-elf64' $elf64Image ''

    # §110/§111 NKI v2 DevSign-Testfälle

    # Positivfall: gueltiges NKI v2 — DevSign muss verifiziert sein
    $validNkiV2Image=[IO.Path]::Combine($tempDir,'nki-v2-devsign-valid.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $Nki -KernelElf $Elf32 -OutputImage $validNkiV2Image | Out-Null
    Invoke-SuccessCase 'nki-v2-devsign-valid' $validNkiV2Image @(
        'UEFI:KERNEL-DEVSIGN-VERIFIED',
        'UEFI:KERNEL-HANDOFF-READY',
        'NOVA_KERNEL_READY'
    )

    # NKI v2 mit sig_size=32 (weder 0 noch 64) — strukturell ungueltig
    # NKI-Header-Layout: magic[8] version[4] header_size[4] arch[4] flags[4]
    # entry[4] load[4] image_size[4] compression[4] crc32[4] build_id[16] sig_size[4]
    # => sig_size liegt bei Byte-Offset 60
    $badSigSizeNki=[IO.Path]::Combine($tempDir,'nki-v2-invalid-sig-size.nki')
    Write-PatchedU32 $Nki $badSigSizeNki 60 32
    $badSigSizeImage=[IO.Path]::Combine($tempDir,'bad-nki-v2-invalid-sig-size.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $badSigSizeNki -KernelElf $Elf32 -OutputImage $badSigSizeImage | Out-Null
    Invoke-ValidationCase 'bad-nki-v2-invalid-sig-size' $badSigSizeImage 'UEFI:ELF32-DIRECT-VALIDATED'

    # NKI v2 um 1 Byte abgeschnitten — DevSign-Block unvollstaendig
    $truncatedNki=[IO.Path]::Combine($tempDir,'nki-v2-truncated.nki')
    Write-TruncatedCopy $Nki $truncatedNki 1
    $truncatedImage=[IO.Path]::Combine($tempDir,'bad-nki-v2-truncated.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $truncatedNki -KernelElf $Elf32 -OutputImage $truncatedImage | Out-Null
    Invoke-ValidationCase 'bad-nki-v2-truncated' $truncatedImage 'UEFI:ELF32-DIRECT-VALIDATED'

    # §111 Revocation-Policy: DevSign-Block mit revocation_gen=1 muss hart abgewiesen werden
    # NKI-Header: image_size liegt bei Offset 32 (4 Bytes, little-endian)
    # DevSign-Block beginnt bei Offset 64+image_size; revocation_gen bei +12 im Block
    $nkiBytes=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($Nki))
    $nkiImageSize=[BitConverter]::ToUInt32($nkiBytes,32)
    $revocOffset=64+[int]$nkiImageSize+12
    $revokedNki=[IO.Path]::Combine($tempDir,'nki-v2-devsign-revoked.nki')
    Write-PatchedU32 $Nki $revokedNki $revocOffset 1
    $revokedImage=[IO.Path]::Combine($tempDir,'bad-nki-v2-devsign-revoked.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $revokedNki -KernelElf $Elf32 -OutputImage $revokedImage | Out-Null
    Invoke-ValidationCase 'bad-nki-v2-devsign-revoked' $revokedImage 'UEFI:ELF32-DIRECT-VALIDATED'

    # §113 LZ4-Kompression: Testfälle (nur wenn NkiBuilder angegeben)
    if($NkiBuilder){
        # Positivfall: LZ4-komprimiertes NKI v2 muss erfolgreich dekomprimiert und geladen werden
        $lz4Nki=[IO.Path]::Combine($tempDir,'nki-v2-lz4.nki')
        & $NkiBuilder -InputFile $Elf32 -OutputFile $lz4Nki -Compress | Out-Null
        $lz4Image=[IO.Path]::Combine($tempDir,'nki-v2-lz4.img')
        & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $lz4Nki -KernelElf $Elf32 -OutputImage $lz4Image | Out-Null
        Invoke-SuccessCase 'nki-v2-lz4-valid' $lz4Image @(
            'UEFI:KERNEL-LZ4-DECOMPRESSED',
            'UEFI:KERNEL-DEVSIGN-VERIFIED',
            'UEFI:KERNEL-HANDOFF-READY',
            'NOVA_KERNEL_READY'
        )

        # Negativfall: NKI v2 mit compression=3 (unbekannt) muss abgelehnt werden
        # NKI-Header: compression liegt bei Offset 36; Wert 3 ist nach §113/§114 nicht definiert
        $badCompNki=[IO.Path]::Combine($tempDir,'nki-v2-bad-compression.nki')
        Write-PatchedU32 $lz4Nki $badCompNki 36 3
        $badCompImage=[IO.Path]::Combine($tempDir,'bad-nki-v2-compression.img')
        & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $badCompNki -KernelElf $Elf32 -OutputImage $badCompImage | Out-Null
        Invoke-ValidationCase 'bad-nki-v2-unknown-compression' $badCompImage 'UEFI:ELF32-DIRECT-VALIDATED'

        # §114 ZSTD-Kompression: Testfälle
        # Positivfall: ZSTD-komprimiertes NKI v2 muss erfolgreich dekomprimiert und geladen werden
        $zstdNki=[IO.Path]::Combine($tempDir,'nki-v2-zstd.nki')
        & $NkiBuilder -InputFile $Elf32 -OutputFile $zstdNki -CompressZstd | Out-Null
        $zstdImage=[IO.Path]::Combine($tempDir,'nki-v2-zstd.img')
        & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $zstdNki -KernelElf $Elf32 -OutputImage $zstdImage | Out-Null
        Invoke-SuccessCase 'nki-v2-zstd-valid' $zstdImage @(
            'UEFI:KERNEL-ZSTD-DECOMPRESSED',
            'UEFI:KERNEL-DEVSIGN-VERIFIED',
            'UEFI:KERNEL-HANDOFF-READY',
            'NOVA_KERNEL_READY'
        )

        # Negativfall: ZSTD-komprimiertes NKI v2 mit falschem compression-Feld (1=LZ4)
        # LZ4-Decompressor versucht ZSTD-Frame als LZ4-Block zu parsen und schlaegt fehl
        $zstdAsLz4Nki=[IO.Path]::Combine($tempDir,'nki-v2-zstd-as-lz4.nki')
        Write-PatchedU32 $zstdNki $zstdAsLz4Nki 36 1
        $zstdAsLz4Image=[IO.Path]::Combine($tempDir,'bad-nki-v2-zstd-as-lz4.img')
        & $ImageBuilder -EfiApplication $EfiApplication -KernelImage $zstdAsLz4Nki -KernelElf $Elf32 -OutputImage $zstdAsLz4Image | Out-Null
        Invoke-ValidationCase 'bad-nki-v2-zstd-as-lz4' $zstdAsLz4Image 'UEFI:ELF32-DIRECT-VALIDATED'
    }
} finally {
    $env:TMP=$oldTmp;$env:TEMP=$oldTemp
    $resolved=[IO.Path]::GetFullPath($tempDir)
    if($resolved.StartsWith($tempRoot,[StringComparison]::OrdinalIgnoreCase)-and[IO.Directory]::Exists($resolved)){
        for($attempt=0;$attempt-lt20-and[IO.Directory]::Exists($resolved);$attempt++){
            try{[IO.Directory]::Delete($resolved,$true)}
            catch [IO.IOException]{if($attempt-eq19){throw};Start-Sleep -Milliseconds 100}
        }
    }
}
