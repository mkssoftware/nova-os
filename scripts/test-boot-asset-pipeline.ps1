param([string]$Assets='assets',[string]$BuildDirectory='build/asset-pipeline-test')
$ErrorActionPreference='Stop'
$first=Join-Path $BuildDirectory 'first.json';$second=Join-Path $BuildDirectory 'second.json'
& "$PSScriptRoot/validate-boot-assets.ps1" -InputDirectory $Assets -OutputReport $first -Profile Release
& "$PSScriptRoot/validate-boot-assets.ps1" -InputDirectory $Assets -OutputReport $second -Profile Release
$hashA=(Get-FileHash -Algorithm SHA256 -LiteralPath $first).Hash
$hashB=(Get-FileHash -Algorithm SHA256 -LiteralPath $second).Hash
if($hashA -ne $hashB){throw 'Identische Eingaben erzeugen unterschiedliche Reports.'}
$bad=Join-Path $BuildDirectory 'invalid';[IO.Directory]::CreateDirectory($bad)|Out-Null
[IO.File]::WriteAllBytes((Join-Path $bad 'broken.png'),[byte[]](1,2,3,4))
$failed=$false
try{& "$PSScriptRoot/validate-boot-assets.ps1" -InputDirectory $bad -OutputReport (Join-Path $bad 'bad.json')}catch{$failed=$true}
if(!$failed){throw 'Beschaedigtes PNG wurde akzeptiert.'}
Write-Host "ASSET-PIPELINE-TEST-OK: $hashA"
