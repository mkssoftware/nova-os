param(
    [string]$InputImage,
    [string]$OutputIso,
    [string]$WslDistro
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
    if ([string]::IsNullOrWhiteSpace($Value)) { $Value = $DefaultRelative }
    if ([IO.Path]::IsPathRooted($Value)) { return [IO.Path]::GetFullPath($Value) }
    return [IO.Path]::GetFullPath([IO.Path]::Combine($RepoRoot,$Value))
}

function To-WslPath([string]$WindowsPath) {
    $full=[IO.Path]::GetFullPath($WindowsPath)
    if($full -match '^([A-Za-z]):\\(.*)$'){
        $drive=$matches[1].ToLowerInvariant()
        $rest=$matches[2].Replace('\','/')
        return "/mnt/$drive/$rest"
    }
    Fail "Nicht unterstuetzter Windows-Pfad: $full"
}

function Invoke-WslBash([string]$Command) {
    & wsl.exe -- bash -lc $Command
    if ($LASTEXITCODE -ne 0) { Fail "WSL-Befehl fehlgeschlagen: $Command" }
}

function U16([byte[]]$b,[int]$o) {
    return [BitConverter]::ToUInt16($b,$o)
}
function U32([byte[]]$b,[int]$o) {
    return [BitConverter]::ToUInt32($b,$o)
}
function W16([byte[]]$b,[int]$o,[int]$v) {
    $b[$o]=[byte]($v-band255)
    $b[$o+1]=[byte](($v-shr8)-band255)
}
function W32([byte[]]$b,[int]$o,[uint32]$v) {
    for($i=0;$i-lt4;$i++){ $b[$o+$i]=[byte](($v-shr(8*$i))-band255) }
}
function PutAscii([byte[]]$b,[int]$o,[int]$n,[string]$s,[byte]$pad=0x20) {
    for($i=0;$i-lt$n;$i++){ $b[$o+$i]=if($i-lt$s.Length){[byte][char]$s[$i]}else{$pad} }
}

function Get-EspInfo([string]$ImagePath) {
    $fs=[IO.File]::OpenRead($ImagePath)
    $br=New-Object IO.BinaryReader($fs)
    try {
        $fs.Position=512
        if([Text.Encoding]::ASCII.GetString($br.ReadBytes(8)) -ne "EFI PART"){ Fail "Kein GPT-Image." }

        $fs.Position=512+72
        $entryLba=$br.ReadUInt64()
        $entryCount=$br.ReadUInt32()
        $entrySize=$br.ReadUInt32()
        $espGuid=[Guid]"C12A7328-F81F-11D2-BA4B-00A0C93EC93B"

        for($i=0;$i-lt$entryCount;$i++){
            $fs.Position=[int64](($entryLba*512)+($i*$entrySize))
            $type=$br.ReadBytes(16)
            $all0=$true
            foreach($x in $type){ if($x-ne0){$all0=$false;break} }
            if($all0){continue}

            $g=New-Object Guid (,$type)
            $null=$br.ReadBytes(16)
            $first=$br.ReadUInt64()
            $last=$br.ReadUInt64()

            if($g-eq$espGuid){ return @{First=$first;Last=$last} }
        }
        Fail "Keine EFI System Partition gefunden."
    }
    finally {
        $br.Dispose()
        $fs.Dispose()
    }
}

function Read-Fat32File {
    param(
        [string]$ImagePath,
        [uint64]$EspFirst,
        [string[]]$PathParts
    )

    $fs=[IO.File]::OpenRead($ImagePath)
    $br=New-Object IO.BinaryReader($fs)

    try {
        $boot=[byte[]]::new(512)
        $fs.Position=[int64]($EspFirst*512)
        if($fs.Read($boot,0,512)-ne512){Fail "ESP-Bootsektor nicht lesbar."}

        $bps=U16 $boot 11
        $spc=[int]$boot[13]
        $reserved=U16 $boot 14
        $fats=[int]$boot[16]
        $spf=U32 $boot 36
        $rootCluster=U32 $boot 44

        Write-Host "FAT32 BPB:   Bytes/Sektor=$bps, Sektoren/Cluster=$spc, Reserved=$reserved, FATs=$fats, SPF=$spf, RootCluster=$rootCluster"

        if($bps-ne512){
            $hex = ($boot[0..63] | ForEach-Object { $_.ToString("X2") }) -join " "
            Fail "Nicht unterstuetzte Sektorgroesse ($bps). Erste 64 Bytes des ESP-Bootsektors: $hex"
        }

        [uint64]$fatLba=$EspFirst+$reserved
        [uint64]$dataLba=$fatLba+($fats*$spf)

        function NextCluster([uint32]$c) {
            $fs.Position=[int64](($fatLba*512)+($c*4))
            return ($br.ReadUInt32() -band 0x0FFFFFFF)
        }

        function ReadCluster([uint32]$c) {
            $buf=[byte[]]::new($spc*512)
            [uint64]$lba=$dataLba+(($c-2)*$spc)
            $fs.Position=[int64]($lba*512)
            $n=$fs.Read($buf,0,$buf.Length)
            if($n-ne$buf.Length){Fail "Cluster $c nicht lesbar."}
            return $buf
        }

        function FindEntry([uint32]$dirCluster,[string]$part) {
            $want=$part.ToUpperInvariant()
            $c=$dirCluster

            while($c-ge2 -and $c-lt0x0FFFFFF8){
                $buf=ReadCluster $c

                for($o=0;$o-lt$buf.Length;$o+=32){
                    if($buf[$o]-eq0){return $null}
                    if($buf[$o]-eq0xE5 -or $buf[$o+11]-eq0x0F){continue}

                    $name=[Text.Encoding]::ASCII.GetString($buf,$o,8).TrimEnd()
                    $ext=[Text.Encoding]::ASCII.GetString($buf,$o+8,3).TrimEnd()
                    $full=if($ext){"$name.$ext"}else{$name}

                    if($full.ToUpperInvariant()-eq$want){
                        $hi=U16 $buf ($o+20)
                        $lo=U16 $buf ($o+26)
                        [uint32]$cl=(($hi-shl16)-bor$lo)
                        [uint32]$size=U32 $buf ($o+28)
                        return @{Cluster=$cl;Size=$size;Attr=$buf[$o+11]}
                    }
                }

                $c=NextCluster $c
            }
            return $null
        }

        [uint32]$cluster=$rootCluster

        for($i=0;$i-lt$PathParts.Count;$i++){
            $e=FindEntry $cluster $PathParts[$i]
            if(!$e){return $null}

            if($i-lt$PathParts.Count-1){
                if(($e.Attr-band0x10)-eq0){Fail "$($PathParts[$i]) ist kein Verzeichnis."}
                $cluster=$e.Cluster
            }
            else {
                if(($e.Attr-band0x10)-ne0){Fail "$($PathParts[$i]) ist kein Datei-Eintrag."}

                $data=New-Object byte[] $e.Size
                $remaining=$e.Size
                $dst=0
                $c=$e.Cluster

                while($remaining-gt0){
                    $buf=ReadCluster $c
                    $take=[Math]::Min($remaining,$buf.Length)
                    [Array]::Copy($buf,0,$data,$dst,$take)
                    $dst+=$take
                    $remaining-=$take
                    if($remaining-gt0){$c=NextCluster $c}
                }

                return $data
            }
        }

        return $null
    }
    finally {
        $br.Dispose()
        $fs.Dispose()
    }
}

function Make83([string]$Name) {
    $p=$Name.ToUpperInvariant().Split('.',2)
    $n=$p[0]
    $e=if($p.Count-gt1){$p[1]}else{""}
    if($n.Length-gt8 -or $e.Length-gt3){Fail "Nicht 8.3-kompatibel: $Name"}
    return @($n,$e)
}

function WriteDirEntry([byte[]]$img,[int]$off,[string]$name,[byte]$attr,[int]$cluster,[int]$size) {
    $p=Make83 $name
    PutAscii $img $off 8 $p[0]
    PutAscii $img ($off+8) 3 $p[1]
    $img[$off+11]=$attr
    W16 $img ($off+26) $cluster
    W32 $img ($off+28) ([uint32]$size)
}

function New-Fat16BootImage {
    param(
        [byte[]]$BootX64,
        [byte[]]$NovaNki,
        [byte[]]$KernelElf,
        [byte[]]$StartupNsh,
        [string]$OutputPath
    )

    $payloadBytes=$BootX64.Length
    if($NovaNki){$payloadBytes+=$NovaNki.Length}
    if($KernelElf){$payloadBytes+=$KernelElf.Length}
    if($StartupNsh){$payloadBytes+=$StartupNsh.Length}

    # Kleinstmoegliches Image waehlen, aber bewusst unter 65535 * 512 Byte,
    # damit El Torito eine normale Load-Size schreiben kann.
    if($payloadBytes -lt 7MB){$imageMiB=8}
    elseif($payloadBytes -lt 15MB){$imageMiB=16}
    elseif($payloadBytes -lt 23MB){$imageMiB=24}
    elseif($payloadBytes -lt 29MB){$imageMiB=30}
    else { Fail "Payload ist zu gross fuer ein kompatibles El-Torito-Bootimage (<32 MiB)." }

    $bps=512
    $totalSectors=[int](($imageMiB*1MB)/$bps)
    $spc=1
    $reserved=1
    $fats=2
    $rootEntries=512
    $rootSectors=[int][Math]::Floor(($rootEntries*32 + $bps-1)/[double]$bps)

    # FAT16 sectors iterativ bestimmen.
    $spf=1
    do {
        $dataSectors=$totalSectors-$reserved-($fats*$spf)-$rootSectors
        $clusters=[int][Math]::Floor($dataSectors/[double]$spc)
        $next=[int][Math]::Ceiling((($clusters+2)*2)/$bps)
        $changed=$next-ne$spf
        $spf=$next
    } while($changed)

    $dataStart=$reserved+($fats*$spf)+$rootSectors
    $clusterBytes=$spc*$bps
    $img=[byte[]]::new($totalSectors*$bps)

    # FAT16 BPB
    $img[0]=0xEB;$img[1]=0x3C;$img[2]=0x90
    PutAscii $img 3 8 "NOVAUEFI"
    W16 $img 11 $bps
    $img[13]=$spc
    W16 $img 14 $reserved
    $img[16]=$fats
    W16 $img 17 $rootEntries

    if($totalSectors-lt65536){
        W16 $img 19 $totalSectors
        W32 $img 32 0
    } else {
        W16 $img 19 0
        W32 $img 32 $totalSectors
    }

    $img[21]=0xF8
    W16 $img 22 $spf
    W16 $img 24 63
    W16 $img 26 255
    W32 $img 28 0

    $img[36]=0x80
    $img[38]=0x29
    W32 $img 39 0x4E4F5641
    PutAscii $img 43 11 "NOVAOS UEFI"
    PutAscii $img 54 8 "FAT16"
    $img[510]=0x55;$img[511]=0xAA

    $fat=[byte[]]::new($spf*$bps)
    W16 $fat 0 0xFFF8
    W16 $fat 2 0xFFFF

    $script:nextCluster=2

    function Alloc([int]$bytes) {
        $count=[Math]::Max(1,[int][Math]::Ceiling($bytes/[double]$clusterBytes))
        $first=$script:nextCluster

        for($i=0;$i-lt$count;$i++){
            $c=$first+$i
            $v=if($i-eq$count-1){0xFFFF}else{$c+1}
            W16 $fat ($c*2) $v
        }

        $script:nextCluster=$first+$count
        return $first
    }

    $efiCluster=Alloc $clusterBytes
    $bootDirCluster=Alloc $clusterBytes
    $bootFileCluster=Alloc $BootX64.Length

    $nkiCluster=0
    $elfCluster=0
    $startupCluster=0

    if($NovaNki){$nkiCluster=Alloc $NovaNki.Length}
    if($KernelElf){$elfCluster=Alloc $KernelElf.Length}
    if($StartupNsh){$startupCluster=Alloc $StartupNsh.Length}

    $maxClusters=[int](($totalSectors-$dataStart)/$spc)
    if(($script:nextCluster-2)-gt$maxClusters){Fail "FAT16-Image ist zu klein."}

    for($i=0;$i-lt$fats;$i++){
        [Array]::Copy($fat,0,$img,($reserved+$i*$spf)*$bps,$fat.Length)
    }

    $rootOff=($reserved+$fats*$spf)*$bps
    WriteDirEntry $img $rootOff "EFI" 0x10 $efiCluster 0

    $ri=1
    if($NovaNki){
        WriteDirEntry $img ($rootOff+$ri*32) "NOVA.NKI" 0x20 $nkiCluster $NovaNki.Length
        $ri++
    }
    if($KernelElf){
        WriteDirEntry $img ($rootOff+$ri*32) "KERNEL.ELF" 0x20 $elfCluster $KernelElf.Length
        $ri++
    }
    if($StartupNsh){
        WriteDirEntry $img ($rootOff+$ri*32) "STARTUP.NSH" 0x20 $startupCluster $StartupNsh.Length
        $ri++
    }

    # PowerShell functions do not form lexical closures.  Keep these values
    # under unique script-scope names so ClusterOff cannot accidentally pick
    # up $dataStart from the earlier FAT32 reader (which is one sector larger).
    $script:fat16DataStart=$dataStart
    $script:fat16SectorsPerCluster=$spc
    $script:fat16BytesPerSector=$bps
    function ClusterOff([int]$c){
        return ($script:fat16DataStart+(($c-2)*$script:fat16SectorsPerCluster))*$script:fat16BytesPerSector
    }

    $efiOff=ClusterOff $efiCluster
    WriteDirEntry $img $efiOff "." 0x10 $efiCluster 0
    WriteDirEntry $img ($efiOff+32) ".." 0x10 0 0
    WriteDirEntry $img ($efiOff+64) "BOOT" 0x10 $bootDirCluster 0

    $bootOff=ClusterOff $bootDirCluster
    WriteDirEntry $img $bootOff "." 0x10 $bootDirCluster 0
    WriteDirEntry $img ($bootOff+32) ".." 0x10 $efiCluster 0
    WriteDirEntry $img ($bootOff+64) "BOOTX64.EFI" 0x20 $bootFileCluster $BootX64.Length

    function CopyFile([byte[]]$data,[int]$first){
        $remaining=$data.Length
        $src=0
        $c=$first

        while($remaining-gt0){
            $take=[Math]::Min($remaining,$clusterBytes)
            [Array]::Copy($data,$src,$img,(ClusterOff $c),$take)
            $src+=$take
            $remaining-=$take
            $c++
        }
    }

    CopyFile $BootX64 $bootFileCluster
    if($NovaNki){CopyFile $NovaNki $nkiCluster}
    if($KernelElf){CopyFile $KernelElf $elfCluster}
    if($StartupNsh){CopyFile $StartupNsh $startupCluster}

    [IO.File]::WriteAllBytes($OutputPath,$img)
    return $img.Length
}

# ---------------- MAIN ----------------

$inputPath=Resolve-RepoPath $InputImage "build\nova-uefi.img"
$outputPath=Resolve-RepoPath $OutputIso "build\nova-uefi.iso"

if(!(Test-Path -LiteralPath $inputPath)){Fail "IMG nicht gefunden: $inputPath"}

& wsl.exe -- bash -lc "command -v xorriso >/dev/null 2>&1"
if($LASTEXITCODE-ne0){Fail "xorriso ist in WSL nicht verfuegbar."}

Write-Host "NovaOS Repo: $RepoRoot"
Write-Host "IMG:         $inputPath"
Write-Host "ISO:         $outputPath"

$esp=Get-EspInfo $inputPath
$bootx64=Read-Fat32File $inputPath $esp.First @("EFI","BOOT","BOOTX64.EFI")
if(!$bootx64){Fail "BOOTX64.EFI nicht gefunden."}

$nki=Read-Fat32File $inputPath $esp.First @("NOVA.NKI")
$elf=Read-Fat32File $inputPath $esp.First @("KERNEL.ELF")

Write-Host "BOOTX64.EFI: $($bootx64.Length) Bytes"
if($nki){Write-Host "NOVA.NKI:    $($nki.Length) Bytes"}
if($elf){Write-Host "KERNEL.ELF:  $($elf.Length) Bytes"}

$tempName="novaos-iso-"+[Guid]::NewGuid().ToString("N")
$tempWin=Join-Path ([IO.Path]::GetTempPath()) $tempName
New-Item -ItemType Directory -Force -Path $tempWin|Out-Null

$efiBootWin=Join-Path $tempWin "efiboot.img"

try {
    $startup=[Text.Encoding]::ASCII.GetBytes("fs0:\EFI\BOOT\BOOTX64.EFI`r`n")
    $bootSize=New-Fat16BootImage -BootX64 $bootx64 -NovaNki $nki -KernelElf $elf -StartupNsh $startup -OutputPath $efiBootWin
    Write-Host "EFI Boot Image: $bootSize Bytes"

    if($bootSize -ge (65535*512)){
        Fail "EFI-Bootimage ist weiterhin zu gross fuer normale El-Torito-Load-Size."
    }

    $outputDir=[IO.Path]::GetDirectoryName($outputPath)
    if($outputDir){New-Item -ItemType Directory -Force -Path $outputDir|Out-Null}
    if(Test-Path -LiteralPath $outputPath){Remove-Item -LiteralPath $outputPath -Force}

    $efiBootWsl=To-WslPath $efiBootWin
    $outputWsl=To-WslPath $outputPath
    $tempIso="/tmp/$tempName"

    Invoke-WslBash "rm -rf '$tempIso'; mkdir -p '$tempIso'"
    Invoke-WslBash "cp '$efiBootWsl' '$tempIso/efiboot.img'"

    # Keep the removable-media fallback path in the ISO9660 tree as well as
    # inside the El-Torito FAT image.  Some EDK2/VirtualBox revisions resolve
    # Boot#### against the optical filesystem rather than its emulated ESP.
    Invoke-WslBash "mkdir -p '$tempIso/EFI/BOOT'"
    $bootX64Win=Join-Path $tempWin "BOOTX64.EFI"
    [IO.File]::WriteAllBytes($bootX64Win,$bootx64)
    $bootX64Wsl=To-WslPath $bootX64Win
    Invoke-WslBash "cp '$bootX64Wsl' '$tempIso/EFI/BOOT/BOOTX64.EFI'"

    if($nki){
        $nkiWin=Join-Path $tempWin "NOVA.NKI"
        [IO.File]::WriteAllBytes($nkiWin,$nki)
        $nkiWsl=To-WslPath $nkiWin
        Invoke-WslBash "cp '$nkiWsl' '$tempIso/NOVA.NKI'"
    }
    if($elf){
        $elfWin=Join-Path $tempWin "KERNEL.ELF"
        [IO.File]::WriteAllBytes($elfWin,$elf)
        $elfWsl=To-WslPath $elfWin
        Invoke-WslBash "cp '$elfWsl' '$tempIso/KERNEL.ELF'"
    }

    Write-Host ""
    Write-Host "Erzeuge kompakte UEFI-El-Torito-ISO..."

    # Publish the ESP twice: as the UEFI El-Torito image used by optical
    # firmware and as an appended GPT EFI partition.  The latter provides a
    # second standards-based discovery path for VirtualBox/EDK2 revisions
    # whose removable-CD Boot#### entry does not resolve the El-Torito child.
    $cmd="xorriso -as mkisofs -iso-level 3 -R -J -V NOVAOS_UEFI -o '$outputWsl' -append_partition 2 0xef '$efiBootWsl' -appended_part_as_gpt -e --interval:appended_partition_2:all:: -no-emul-boot '$tempIso'"
    Invoke-WslBash $cmd

    if(!(Test-Path -LiteralPath $outputPath)){Fail "ISO wurde nicht erzeugt."}

    Write-Host ""
    Write-Host "Validiere El-Torito..."

    $old=$ErrorActionPreference
    $ErrorActionPreference="Continue"
    $report=& wsl.exe -- bash -lc "xorriso -indev '$outputWsl' -report_el_torito plain 2>&1"
    $code=$LASTEXITCODE
    $ErrorActionPreference=$old

    if($code-ne0){Fail "ISO-Validierung fehlgeschlagen."}

    $txt=$report -join "`n"
    Write-Host $txt

    if($txt -notmatch "UEFI"){Fail "Kein UEFI-El-Torito-Eintrag gefunden."}
    if($txt -match "Ldsiz\s+0\s"){Fail "El-Torito Load Size ist weiterhin 0."}

    Write-Host ""
    Write-Host "FERTIG" -ForegroundColor Green
    Write-Host "ISO:       $outputPath"
    Write-Host "Groesse:   $((Get-Item $outputPath).Length) Bytes"
    Write-Host "Bootimage: $bootSize Bytes"
}
finally {
    Remove-Item -LiteralPath $tempWin -Recurse -Force -ErrorAction SilentlyContinue
    if($tempIso){ & wsl.exe -- bash -lc "rm -rf '$tempIso'" 2>$null | Out-Null }
}
