param([Parameter(Mandatory=$true)][string]$BootImage,[Parameter(Mandatory=$true)][string]$OutputFile)
$ErrorActionPreference='Stop'; $s=2048; $img=[IO.File]::ReadAllBytes((Resolve-Path $BootImage))
if($img.Length-ne1474560){throw 'Bootimage muss 1,44 MiB gross sein.'}
$pl=19;$pb=20;$root=21;$cat=22;$boot=23;$total=$boot+[int][Math]::Ceiling($img.Length/$s);$iso=[byte[]]::new($total*$s)
function A($b,$o,$n,$v){$x=[Text.Encoding]::ASCII.GetBytes($v);[Array]::Copy($x,0,$b,$o,[Math]::Min($n,$x.Length))}
function L16($b,$o,$v){$b[$o]=$v-band255;$b[$o+1]=($v-shr8)-band255}
function B16($b,$o,$v){$b[$o]=($v-shr8)-band255;$b[$o+1]=$v-band255}
function L32($b,$o,$v){0..3|%{$b[$o+$_]=($v-shr(8*$_))-band255}}
function B32($b,$o,$v){0..3|%{$b[$o+$_]=($v-shr(8*(3-$_)))-band255}}
function D16($b,$o,$v){L16 $b $o $v;B16 $b ($o+2) $v}
function D32($b,$o,$v){L32 $b $o $v;B32 $b ($o+4) $v}
function DR($b,$o,$e,$z,$f,[byte[]]$n){$l=33+$n.Length;if($l%2){$l++};$b[$o]=[byte]$l;L32 $b ($o+2) $e;B32 $b ($o+6) $e;L32 $b ($o+10) $z;B32 $b ($o+14) $z;$d=Get-Date;$b[$o+18]=[byte]($d.Year-1900);$b[$o+19]=[byte]$d.Month;$b[$o+20]=[byte]$d.Day;$b[$o+21]=[byte]$d.Hour;$b[$o+22]=[byte]$d.Minute;$b[$o+23]=[byte]$d.Second;$b[$o+25]=$f;D16 $b ($o+28) 1;$b[$o+32]=[byte]$n.Length;[Array]::Copy($n,0,$b,$o+33,$n.Length);return $l}
$p=16*$s;$iso[$p]=1;A $iso ($p+1) 5 'CD001';$iso[$p+6]=1;A $iso ($p+8) 32 'NOVAOS';A $iso ($p+40) 32 'NOVAOS_BOOT';D32 $iso ($p+80) $total;D16 $iso ($p+120) 1;D16 $iso ($p+124) 1;D16 $iso ($p+128) $s;D32 $iso ($p+132) 10;L32 $iso ($p+140) $pl;B32 $iso ($p+148) $pb;[void](DR $iso ($p+156) $root $s 2 ([byte[]](0)))
$p=17*$s;$iso[$p]=0;A $iso ($p+1) 5 'CD001';$iso[$p+6]=1;A $iso ($p+7) 32 'EL TORITO SPECIFICATION';L32 $iso ($p+71) $cat
$p=18*$s;$iso[$p]=255;A $iso ($p+1) 5 'CD001';$iso[$p+6]=1
$p=$pl*$s;$iso[$p]=1;L32 $iso ($p+2) $root;L16 $iso ($p+6) 1;$p=$pb*$s;$iso[$p]=1;B32 $iso ($p+2) $root;B16 $iso ($p+6) 1
$p=$root*$s;$n=DR $iso $p $root $s 2 ([byte[]](0));$n+=DR $iso ($p+$n) $root $s 2 ([byte[]](1));[void](DR $iso ($p+$n) $boot $img.Length 0 ([Text.Encoding]::ASCII.GetBytes('NOVA-BIOS.IMG;1')))
$p=$cat*$s;$iso[$p]=1;A $iso ($p+4) 24 'NOVAOS BOOT';$iso[$p+30]=0x55;$iso[$p+31]=0xAA;$sum=0;for($i=0;$i-lt32;$i+=2){$sum=($sum+$iso[$p+$i]+($iso[$p+$i+1]-shl8))-band0xFFFF};L16 $iso ($p+28) ((-$sum)-band0xFFFF);$iso[$p+32]=0x88;$iso[$p+33]=2;L16 $iso ($p+38) 1;L32 $iso ($p+40) $boot
[Array]::Copy($img,0,$iso,$boot*$s,$img.Length);$out=[IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputFile));[IO.File]::WriteAllBytes($out,$iso);Write-Host "ISO: $out ($($iso.Length) Bytes)"
