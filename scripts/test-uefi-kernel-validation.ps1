param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$EfiApplication,
    [Parameter(Mandatory=$true)][string]$Nki,
    [Parameter(Mandatory=$true)][string]$Elf32,
    [Parameter(Mandatory=$true)][string]$Elf64,
    [Parameter(Mandatory=$true)][string]$ImageBuilder
)

$ErrorActionPreference='Stop'
$tempRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
[IO.Directory]::CreateDirectory($tempRoot)|Out-Null
$tempDir=[IO.Path]::Combine($tempRoot,'nova-uefi-kernel-validation-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($tempDir)|Out-Null
$oldTmp=$env:TMP;$oldTemp=$env:TEMP
$env:TMP=$tempDir;$env:TEMP=$tempDir

function Write-CorruptCopy([string]$source,[string]$destination,[int]$offset) {
    $bytes=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($source))
    if($offset-lt0-or$offset-ge$bytes.Length){throw "Korruptionsoffset liegt ausserhalb von $source"}
    $bytes[$offset]=$bytes[$offset]-bxor0x5A
    [IO.File]::WriteAllBytes($destination,$bytes)
}

function Invoke-ValidationCase([string]$name,[string]$image,[string]$forbiddenMarker) {
    $debug=[IO.Path]::Combine($tempDir,$name+'.debug.log')
    $stderr=[IO.Path]::Combine($tempDir,$name+'.stderr.log')
    $arguments=@('-machine','q35','-m','256M','-smp','4',
        '-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
        '-drive',"format=raw,file=$image,if=ide",'-display','none','-monitor','none',
        '-serial','none','-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
        '-no-reboot','-no-shutdown')
    $process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
        do {
            Start-Sleep -Milliseconds 250
            if(Test-Path -LiteralPath $debug){$content=Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue}
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

    $elf32Image=[IO.Path]::Combine($tempDir,'bad-elf32.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelElf $badElf32 -OutputImage $elf32Image | Out-Null
    Invoke-ValidationCase 'bad-elf32' $elf32Image ''

    $elf64Image=[IO.Path]::Combine($tempDir,'bad-elf64.img')
    & $ImageBuilder -EfiApplication $EfiApplication -KernelElf64 $badElf64 -OutputImage $elf64Image | Out-Null
    Invoke-ValidationCase 'bad-elf64' $elf64Image ''
} finally {
    $env:TMP=$oldTmp;$env:TEMP=$oldTemp
    $resolved=[IO.Path]::GetFullPath($tempDir)
    if($resolved.StartsWith($tempRoot,[StringComparison]::OrdinalIgnoreCase)-and[IO.Directory]::Exists($resolved)){
        [IO.Directory]::Delete($resolved,$true)
    }
}
