param(
    [Parameter(Mandatory=$true)][string]$InputDirectory,
    [Parameter(Mandatory=$true)][string]$OutputReport,
    [ValidateSet('Debug','Development','Release')][string]$Profile='Development',
    [uint16]$VersionMajor=1,[uint16]$VersionMinor=0,
    [uint16]$VersionPatch=0,[uint16]$VersionBuild=0
)

$ErrorActionPreference='Stop'
$root=(Resolve-Path -LiteralPath $InputDirectory).Path
$supported=@{
    '.png'='Image';'.svg'='SvgIcon';'.ttf'='Font';'.otf'='Font';
    '.theme'='Theme';'.lang'='Localization';'.anim'='Animation';
    '.cur'='Cursor';'.bin'='Binary'
}

function Get-Fnv64([string]$Text) {
    $value=[Numerics.BigInteger]::Parse('1469598103934665603')
    $prime=[Numerics.BigInteger]::Parse('1099511628211')
    $mask=[Numerics.BigInteger]::Parse('18446744073709551615')
    foreach($byte in [Text.Encoding]::UTF8.GetBytes($Text)) {
        $value=(($value -bxor [Numerics.BigInteger]$byte)*$prime) -band $mask
    }
    return ('{0:x16}' -f [uint64]$value)
}

function Get-Sha256([byte[]]$Bytes) {
    $sha=[Security.Cryptography.SHA256]::Create()
    try{return ([BitConverter]::ToString($sha.ComputeHash($Bytes))).Replace('-','').ToLowerInvariant()}
    finally{$sha.Dispose()}
}

function Assert-Png([byte[]]$Bytes,[string]$Path) {
    [byte[]]$signature=137,80,78,71,13,10,26,10
    if($Bytes.Length -lt 33){throw "PNG zu klein: $Path"}
    for($i=0;$i -lt 8;$i++){if($Bytes[$i] -ne $signature[$i]){throw "PNG-Signatur ungueltig: $Path"}}
    if([Text.Encoding]::ASCII.GetString($Bytes,12,4) -ne 'IHDR'){throw "PNG ohne IHDR: $Path"}
    $width=([uint32]$Bytes[16]*16777216)+([uint32]$Bytes[17]*65536)+
           ([uint32]$Bytes[18]*256)+$Bytes[19]
    $height=([uint32]$Bytes[20]*16777216)+([uint32]$Bytes[21]*65536)+
            ([uint32]$Bytes[22]*256)+$Bytes[23]
    if($width -le 0 -or $height -le 0){throw "PNG-Dimensionen ungueltig: $Path"}
}

function Assert-Utf8([byte[]]$Bytes,[string]$Path) {
    $strict=[Text.UTF8Encoding]::new($false,$true)
    try{[void]$strict.GetString($Bytes)}catch{throw "Ungueltiges UTF-8: $Path"}
}

function Assert-Svg([byte[]]$Bytes,[string]$Path) {
    Assert-Utf8 $Bytes $Path
    $settings=[Xml.XmlReaderSettings]::new();$settings.DtdProcessing='Prohibit';$settings.XmlResolver=$null
    $stream=[IO.MemoryStream]::new($Bytes,$false)
    try{
        $reader=[Xml.XmlReader]::Create($stream,$settings);$rootName=$null
        while($reader.Read()){if($reader.NodeType -eq [Xml.XmlNodeType]::Element){$rootName=$reader.LocalName;break}}
        $reader.Dispose();if($rootName -ne 'svg'){throw "SVG-Wurzelelement fehlt: $Path"}
    } finally {$stream.Dispose()}
}

if($VersionMajor -eq 0){throw 'Major-Version 0 ist ungueltig.'}
$files=Get-ChildItem -LiteralPath $root -Recurse -File | ForEach-Object {
    $relative=$_.FullName.Substring($root.Length).TrimStart('\','/').Replace('\','/').ToLowerInvariant()
    [pscustomobject]@{File=$_;Relative=$relative}
} | Sort-Object Relative
if(!$files){throw "Keine Assets in $root gefunden."}

$ids=@{};$records=[Collections.Generic.List[object]]::new()
foreach($item in $files){
    $extension=[IO.Path]::GetExtension($item.Relative).ToLowerInvariant()
    if(!$supported.ContainsKey($extension)){throw "Nicht unterstuetztes Assetformat: $($item.Relative)"}
    [byte[]]$bytes=[IO.File]::ReadAllBytes($item.File.FullName)
    if(!$bytes.Length){throw "Leeres Asset: $($item.Relative)"}
    switch($extension){
        '.png' {Assert-Png $bytes $item.Relative}
        '.svg' {Assert-Svg $bytes $item.Relative}
        {$_ -in '.theme','.lang','.anim'} {Assert-Utf8 $bytes $item.Relative}
        '.cur' {if($bytes.Length -lt 6 -or $bytes[2] -ne 2){throw "CUR-Header ungueltig: $($item.Relative)"}}
        '.ttf' {if($bytes.Length -lt 12){throw "TTF-Header ungueltig: $($item.Relative)"}}
        '.otf' {if($bytes.Length -lt 12 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'OTTO'){throw "OTF-Header ungueltig: $($item.Relative)"}}
    }
    $uri="boot://assets/$($item.Relative)";$id=Get-Fnv64 $uri
    if($ids.ContainsKey($id)){throw "Doppelte Resource-ID: $uri und $($ids[$id])"};$ids[$id]=$uri
    $hash=Get-Sha256 $bytes
    $records.Add([ordered]@{path=$item.Relative;uri=$uri;resource_id=$id;
        type=$supported[$extension];size=[uint64]$bytes.Length;sha256=$hash;
        version="$VersionMajor.$VersionMinor.$VersionPatch.$VersionBuild"})
}

$report=[ordered]@{format='NovaOS Asset Validation Report';format_version=1;
    profile=$Profile;package_version="$VersionMajor.$VersionMinor.$VersionPatch.$VersionBuild";
    resource_count=$records.Count;resources=$records}
$json=$report|ConvertTo-Json -Depth 6 -Compress
$outputFull=[IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputReport))
[IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($outputFull))|Out-Null
[IO.File]::WriteAllText($outputFull,$json+"`n",[Text.UTF8Encoding]::new($false))
$reportHash=Get-Sha256 ([IO.File]::ReadAllBytes($outputFull))
Write-Host "ASSET-VALIDATION-OK: $($records.Count) Ressourcen"
Write-Host "REPORT-SHA256: $reportHash"
