param([Parameter(Mandatory=$true)][string]$EfiApplication,
      [Parameter(Mandatory=$true)][string]$OutputImage)
$ErrorActionPreference='Stop'
$efi=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($EfiApplication))
$ss=512;$total=131072L;$partFirst=2048L;$partLast=$total-34;$partSectors=$partLast-$partFirst+1
$image=[byte[]]::new($total*$ss)
function W16([byte[]]$b,[long]$o,[long]$v){$b[$o]=[byte]($v-band255);$b[$o+1]=[byte](($v-shr8)-band255)}
function W32([byte[]]$b,[long]$o,[long]$v){for($i=0;$i-lt4;$i++){$b[$o+$i]=[byte](($v-shr(8*$i))-band255)}}
function W64([byte[]]$b,[long]$o,[long]$v){for($i=0;$i-lt8;$i++){$b[$o+$i]=[byte](($v-shr(8*$i))-band255)}}
function WA([byte[]]$b,[long]$o,[int]$n,[string]$s,[byte]$pad=0x20){for($i=0;$i-lt$n;$i++){$b[$o+$i]=if($i-lt$s.Length){[byte][char]$s[$i]}else{$pad}}}
function CRC32([byte[]]$b,[int]$o,[int]$n){[uint64]$crc=4294967295;for($i=0;$i-lt$n;$i++){$crc=$crc-bxor$b[$o+$i];for($j=0;$j-lt8;$j++){if($crc-band1){$crc=(($crc-shr1)-bxor3988292384)-band4294967295}else{$crc=$crc-shr1}}};return [uint32](($crc-bxor4294967295)-band4294967295)}
function Entry([byte[]]$b,[long]$o,[string]$name,[string]$ext,[byte]$attr,[long]$cluster,[long]$size){WA $b $o 8 $name;WA $b ($o+8) 3 $ext;$b[$o+11]=$attr;W16 $b ($o+20) ($cluster-shr16);W16 $b ($o+26) $cluster;W32 $b ($o+28) $size}
# Protective MBR
$image[446+4]=0xEE;W32 $image (446+8) 1;W32 $image (446+12) ($total-1);$image[510]=0x55;$image[511]=0xAA
# GPT partition entry array (128 entries x 128 bytes)
$entries=[byte[]]::new(16384);[Array]::Copy(([Guid]'C12A7328-F81F-11D2-BA4B-00A0C93EC93B').ToByteArray(),0,$entries,0,16)
[Array]::Copy(([Guid]'4E4F5641-4F53-4546-8020-4E4F56414F53').ToByteArray(),0,$entries,16,16)
W64 $entries 32 $partFirst;W64 $entries 40 $partLast
$name=[Text.Encoding]::Unicode.GetBytes('NovaOS EFI');[Array]::Copy($name,0,$entries,56,$name.Length)
$entriesCrc=CRC32 $entries 0 $entries.Length
[Array]::Copy($entries,0,$image,2*$ss,$entries.Length)
$backupEntriesLba=$total-33;[Array]::Copy($entries,0,$image,$backupEntriesLba*$ss,$entries.Length)
function GptHeader([long]$current,[long]$backup,[long]$entryLba){$h=[byte[]]::new(512);WA $h 0 8 'EFI PART' 0;W32 $h 8 0x00010000;W32 $h 12 92;W64 $h 24 $current;W64 $h 32 $backup;W64 $h 40 34;W64 $h 48 ($total-34);[Array]::Copy(([Guid]'4E4F5641-4F53-4449-534B-4E4F56414F53').ToByteArray(),0,$h,56,16);W64 $h 72 $entryLba;W32 $h 80 128;W32 $h 84 128;W32 $h 88 $entriesCrc;W32 $h 16 (CRC32 $h 0 92);return $h}
$primary=GptHeader 1 ($total-1) 2;[Array]::Copy($primary,0,$image,$ss,$ss)
$backup=GptHeader ($total-1) 1 $backupEntriesLba;[Array]::Copy($backup,0,$image,($total-1)*$ss,$ss)
# FAT32 ESP
$reserved=32;$fatCount=2;$spc=1;$spf=1
do{$clusters=[long](($partSectors-$reserved-$fatCount*$spf)/$spc);$next=[long][Math]::Ceiling((($clusters+2)*4)/$ss)
   if($next-gt$spf){$spf=$next;$grow=$true}else{$grow=$false}}while($grow)
$fatStart=$partFirst+$reserved;$dataStart=$fatStart+$fatCount*$spf
$boot=$partFirst*$ss;$image[$boot]=0xEB;$image[$boot+1]=0x58;$image[$boot+2]=0x90;WA $image ($boot+3) 8 'NOVAUEF'
W16 $image ($boot+11) $ss;$image[$boot+13]=$spc;W16 $image ($boot+14) $reserved;$image[$boot+16]=$fatCount
W16 $image ($boot+17) 0;W16 $image ($boot+19) 0;$image[$boot+21]=0xF8;W16 $image ($boot+22) 0
W16 $image ($boot+24) 63;W16 $image ($boot+26) 255;W32 $image ($boot+28) $partFirst;W32 $image ($boot+32) $partSectors
W32 $image ($boot+36) $spf;W32 $image ($boot+44) 2;W16 $image ($boot+48) 1;W16 $image ($boot+50) 6
$image[$boot+64]=0x80;$image[$boot+66]=0x29;W32 $image ($boot+67) 0x4E4F5641;WA $image ($boot+71) 11 'NOVAOS UEFI';WA $image ($boot+82) 8 'FAT32'
$image[$boot+510]=0x55;$image[$boot+511]=0xAA;[Array]::Copy($image,$boot,$image,($partFirst+6)*$ss,$ss)
$fs=($partFirst+1)*$ss;W32 $image $fs 0x41615252;W32 $image ($fs+484) 0x61417272;W32 $image ($fs+488) 0xffffffff;W32 $image ($fs+492) 0xffffffff;W32 $image ($fs+508) 0xaa550000
$fat=[byte[]]::new($spf*$ss);function F([long]$c,[long]$v){W32 $fat ($c*4) $v}
F 0 0x0ffffff8;F 1 0x0fffffff;F 2 0x0fffffff;F 3 0x0fffffff;F 4 0x0fffffff
$first=5;$count=[long][Math]::Ceiling($efi.Length/$ss);$last=$first+$count-1
for($c=$first;$c-le$last;$c++){F $c $(if($c-eq$last){0x0fffffff}else{$c+1})}
for($i=0;$i-lt$fatCount;$i++){[Array]::Copy($fat,0,$image,($fatStart+$i*$spf)*$ss,$fat.Length)}
$root=$dataStart*$ss;$efiDir=($dataStart+1)*$ss;$bootDir=($dataStart+2)*$ss
Entry $image $root 'EFI' '' 0x10 3 0
Entry $image $efiDir '.' '' 0x10 3 0;Entry $image ($efiDir+32) '..' '' 0x10 2 0;Entry $image ($efiDir+64) 'BOOT' '' 0x10 4 0
Entry $image $bootDir '.' '' 0x10 4 0;Entry $image ($bootDir+32) '..' '' 0x10 3 0;Entry $image ($bootDir+64) 'BOOTX64' 'EFI' 0x20 $first $efi.Length
$fileOffset=($dataStart+($first-2))*$ss;[Array]::Copy($efi,0,$image,$fileOffset,$efi.Length)
$out=[IO.Path]::GetFullPath($OutputImage);[IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($out))|Out-Null;[IO.File]::WriteAllBytes($out,$image)
Write-Host "UEFI-IMG: $out";Write-Host "Groesse: $($image.Length) Bytes";Write-Host "ESP: FAT32 / GPT";Write-Host "BOOTX64.EFI: $($efi.Length) Bytes"
