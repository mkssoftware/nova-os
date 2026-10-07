param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$Image,
    [Parameter(Mandatory=$true)][string]$NovaFsTool
)

# End-to-End-Test des persistenten NovaFS-Systemvolumes (NPSPEC-NOVAFS-ONDISK-0001).
# Jeder Fall arbeitet auf einer temporaeren Kopie des UEFI-Images; das Volume im
# Original-Image bleibt unveraendert.

$ErrorActionPreference='Stop'
$buildRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
$runDir=[IO.Path]::Combine($buildRoot,'nova-uefi-novafs-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($runDir)|Out-Null
$tool=[IO.Path]::GetFullPath($NovaFsTool)
$baseImage=[IO.Path]::GetFullPath($Image)
$hidden=-not($PSVersionTable.PSEdition-eq'Core'-and-not$IsWindows)

function Invoke-NovaFs([string[]]$Arguments,[switch]$AllowFailure){
    # Windows PowerShell 5.1 wertet stderr nativer Programme sonst als Fehler.
    $previous=$ErrorActionPreference;$ErrorActionPreference='Continue'
    try{$output=& $tool @Arguments 2>&1}finally{$ErrorActionPreference=$previous}
    $text=[string](($output|ForEach-Object{"$_"}) -join "`n")
    if(-not$AllowFailure-and$LASTEXITCODE-ne0){throw "novafs $($Arguments -join ' ') fehlgeschlagen: $text"}
    return $text
}

# Lage der NovaFS-Partition aus dem GPT des Basis-Images bestimmen.
$probe=[byte[]]::new(512+16384)
$stream=[IO.File]::OpenRead($baseImage)
try{$null=$stream.Seek(512,'Begin');$null=$stream.Read($probe,0,$probe.Length)}finally{$stream.Dispose()}
if([Text.Encoding]::ASCII.GetString($probe,0,8)-ne'EFI PART'){throw 'Basis-Image ohne GPT'}
$typeGuid=[Guid]'4E4F5641-4653-5359-5354-454D30303031'
$partFirst=0L;$partLast=0L
for($i=0;$i-lt4;$i++){
    $o=512+$i*128
    if([Guid]::new([byte[]]$probe[$o..($o+15)])-eq$typeGuid){
        $partFirst=[BitConverter]::ToInt64($probe,$o+32);$partLast=[BitConverter]::ToInt64($probe,$o+40)
    }
}
if($partFirst-eq0){throw 'Basis-Image enthaelt keine NovaFS-Partition (make uefi-image)'}
$partBytes=($partLast-$partFirst+1)*512
$sizeMib=[int]($partBytes/1MB)

function New-ScenarioImage([string]$Name,[int]$Prefill,[string]$Mode){
    $dir=[IO.Path]::Combine($runDir,$Name);[IO.Directory]::CreateDirectory($dir)|Out-Null
    $disk=[IO.Path]::Combine($dir,'disk.img');$part=[IO.Path]::Combine($dir,'part.img')
    [IO.File]::Copy($baseImage,$disk,$true)
    $null=Invoke-NovaFs @('mkfs',$part,'--size-mib',"$sizeMib",'--label','NovaOS Test')
    if($Prefill-gt0){
        $one=[IO.Path]::Combine($dir,'one.bin');[IO.File]::WriteAllBytes($one,[byte[]]@(0x78))
        for($i=1;$i-le$Prefill;$i++){$null=Invoke-NovaFs @('put',$part,$one,('/Benutzer/f-{0:D3}' -f $i))}
    }
    $bytes=[IO.File]::ReadAllBytes($part)
    if($Mode-eq'empty'){[Array]::Clear($bytes,0,$bytes.Length)}
    if($Mode-eq'corrupt-primary'){$bytes[4096+200]=$bytes[4096+200]-bxor0xFF}
    $out=[IO.File]::OpenWrite($disk)
    try{$null=$out.Seek($partFirst*512,'Begin');$out.Write($bytes,0,$bytes.Length)}finally{$out.Dispose()}
    if($Mode-eq'dirty'){$null=Invoke-NovaFs @('mark-dirty',$disk,'--gpt')}
    return $disk
}

function Invoke-Boot([string]$Disk,[string]$Tag){
    $dir=[IO.Path]::GetDirectoryName($Disk)
    $serial=[IO.Path]::Combine($dir,"serial-$Tag.log");$stderr=[IO.Path]::Combine($dir,"stderr-$Tag.log")
    $firmwareCopy=[IO.Path]::Combine($dir,"firmware-$Tag.fd")
    [IO.File]::Copy([IO.Path]::GetFullPath($Firmware),$firmwareCopy,$true)
    $arguments=@('-machine','q35','-m','256M','-smp','2',
        '-drive',"if=pflash,format=raw,file=$firmwareCopy",
        '-drive',"format=raw,file=$Disk,if=ide",
        '-display','none','-monitor','none','-serial',"file:$serial",'-no-reboot','-no-shutdown')
    if($hidden){$process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError $stderr}
    else{$process=Start-Process $Qemu -ArgumentList $arguments -PassThru -RedirectStandardError $stderr}
    try{
        $deadline=[DateTime]::UtcNow.AddSeconds(120);$text=''
        do{
            Start-Sleep -Milliseconds 250
            $text=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
            if($process.HasExited){throw "QEMU wurde vorzeitig beendet ($Tag)"}
        }while($text-notlike'*NOVA_KERNEL_READY*'-and$text-notlike'*NOVA PANIC REPORT*'-and[DateTime]::UtcNow-lt$deadline)
        Start-Sleep -Milliseconds 300
        $text=[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)
    } finally {
        if(-not$process.HasExited){Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue}
        $process.WaitForExit()
    }
    if($text-notlike'*NOVA_KERNEL_READY*'){throw "Kernel erreichte NOVA_KERNEL_READY nicht ($Tag)"}
    return $text
}

function Assert-Contains([string]$Text,[string[]]$Markers,[string]$Tag){
    foreach($m in $Markers){if($Text-notlike"*$m*"){throw "Marker fehlt ($Tag): $m"}}
}
function Assert-Missing([string]$Text,[string[]]$Markers,[string]$Tag){
    foreach($m in $Markers){if($Text-like"*$m*"){throw "Unerwarteter Marker ($Tag): $m"}}
}
function Assert-Fsck([string]$Disk,[string]$Tag){
    $result=Invoke-NovaFs @('fsck',$Disk,'--gpt') -AllowFailure
    if($LASTEXITCODE-ne0-or$result-notlike'*fsck: OK*'){throw "fsck fehlgeschlagen ($Tag): $result"}
    Write-Host "  $Tag $result"
}
function Assert-BootCount([string]$Disk,[int]$Expected,[string]$Tag){
    $value=(Invoke-NovaFs @('cat',$Disk,'--gpt','/System/Diagnose/novafs-bootcount')).Trim()
    $want='NOVAFS-BOOTCOUNT {0:X8}' -f $Expected
    if($value-ne$want){throw "Bootzaehler ($Tag): '$value' statt '$want'"}
}

$mounted=@('NOVA: NovaFS 1.0 Systemvolume gemountet','NOVA: NovaFS Root-Layout konsistent mit Semantic-Core-ObjectIDs',
    'NOVA: NovaFS ist persistentes SystemRoot unter /')
$writable=$mounted+@('NOVA: NovaFS Lese-/Schreibtest mit Extents, Teilbloecken und Blockgrenze bereit',
    'NOVA: Boot Health SystemRoot bereit, wartet auf Trust')
$failures=@('Selbsttest fehlgeschlagen','Root-Layout inkonsistent','Mount fehlgeschlagen','Root-Registrierung fehlgeschlagen')

try {
    Write-Host 'NovaFS: frisches Volume, zwei Starts'
    $disk=New-ScenarioImage 'fresh' 0 ''
    for($boot=1;$boot-le2;$boot++){
        $text=Invoke-Boot $disk "boot$boot"
        Assert-Contains $text ($writable+@(('NOVA: NovaFS persistenter Bootzaehler 0x{0:X8}' -f $boot))) "fresh/$boot"
        Assert-Missing $text $failures "fresh/$boot"
        Assert-Fsck $disk "fresh/$boot"
        Assert-BootCount $disk $boot "fresh/$boot"
    }
    $pattern=Invoke-NovaFs @('ls',$disk,'--gpt','/System/Diagnose')
    if($pattern-notmatch'12388\s+\d+\s+novafs-muster\.bin'){throw "Musterdatei hat falsche Groesse: $pattern"}

    # Vorbelegungen, bei denen der Kernel gezielt Wurzel- und Blatt-Splits
    # aller drei Baeume ausfuehren muss (ermittelt mit "novafs tree").
    foreach($case in @(@{N=7;T='Directory-Wurzel'},@{N=13;T='Directory-Blatt'},@{N=17;T='Object-Wurzel'},
                       @{N=30;T='Object-Blatt'},@{N=54;T='Extent-Wurzel'},@{N=82;T='Extent-Blatt'})){
        $name="split-$($case.N)"
        Write-Host "NovaFS: Split $($case.T) (Vorbelegung $($case.N))"
        $disk=New-ScenarioImage $name $case.N ''
        $text=Invoke-Boot $disk 'boot1'
        Assert-Contains $text ($writable+@('NOVA: NovaFS persistenter Bootzaehler 0x00000001')) $name
        Assert-Missing $text $failures $name
        Assert-Fsck $disk $name
        Assert-BootCount $disk 1 $name
        $first=Invoke-NovaFs @('cat',$disk,'--gpt','/Benutzer/f-001')
        if($first-ne'x'){throw "Vorbelegte Datei beschaedigt ($name)"}
    }

    Write-Host 'NovaFS: beschaedigter primaerer Superblock'
    $disk=New-ScenarioImage 'backup' 0 'corrupt-primary'
    $text=Invoke-Boot $disk 'boot1'
    Assert-Contains $text ($writable+@('NOVA: NovaFS Backup-Superblock verwendet','NOVA: NovaFS persistenter Bootzaehler 0x00000001')) 'backup'
    Assert-Fsck $disk 'backup'
    $info=Invoke-NovaFs @('info',$disk,'--gpt')
    if($info-like'*Backup verwendet*'){throw 'Primaerer Superblock wurde nicht repariert'}

    Write-Host 'NovaFS: unsauberes Volume (DIRTY)'
    $disk=New-ScenarioImage 'dirty' 0 'dirty'
    $text=Invoke-Boot $disk 'boot1'
    Assert-Contains $text ($mounted+@('NOVA: NovaFS Volume nicht sauber (DIRTY), nur Read-only gemountet',
        'NOVA: NovaFS Read-only, Schreibtest uebersprungen','NOVA: Boot Health SystemRoot nur Read-only verfuegbar')) 'dirty'
    Assert-Missing $text @('Bootzaehler','SystemRoot bereit, wartet auf Trust') 'dirty'
    $info=Invoke-NovaFs @('info',$disk,'--gpt')
    if($info-notlike'*DIRTY*'){throw 'DIRTY-Volume wurde vom Kernel veraendert'}

    Write-Host 'NovaFS: unformatierte Partition'
    $disk=New-ScenarioImage 'empty' 0 'empty'
    $text=Invoke-Boot $disk 'boot1'
    Assert-Contains $text @('NOVA: NovaFS Mount fehlgeschlagen','NOVA: Boot Health wartet auf persistentes SystemRoot') 'empty'
    Assert-Missing $text @('Systemvolume gemountet') 'empty'

    Write-Host 'QEMU UEFI NovaFS-Test erfolgreich'
} finally {
    if(Test-Path -LiteralPath $runDir){Remove-Item -LiteralPath $runDir -Recurse -Force -ErrorAction SilentlyContinue}
}
