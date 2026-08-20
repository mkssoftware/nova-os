param(
    [Parameter(Mandatory = $true)][string]$Qemu,
    [Parameter(Mandatory = $true)][string]$Firmware,
    [Parameter(Mandatory = $true)][string]$FatDirectory,
    [Parameter(Mandatory = $true)][string]$DebugLog,
    [string]$Screenshot = 'build/uefi-dialog.ppm',
    [int]$MonitorPort = 45463,
    [switch]$OpenOnly
)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$env:TMP = [IO.Path]::GetFullPath((Join-Path $root 'build'))
$env:TEMP = $env:TMP
$logPath = [IO.Path]::GetFullPath((Join-Path $root $DebugLog))
$shotPath = [IO.Path]::GetFullPath((Join-Path $root $Screenshot))
foreach ($path in @($logPath,$shotPath)) {
    if (Test-Path -LiteralPath $path) { Remove-Item -LiteralPath $path -Force }
}
$arguments = @(
    '-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
    '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
    '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",
    '-debugcon',"file:$logPath",'-global','isa-debugcon.iobase=0xe9',
    '-no-reboot','-no-shutdown'
)
$process = Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru
try {
    $lastContent=''
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    while ([DateTime]::UtcNow -lt $deadline) {
        if (Test-Path -LiteralPath $logPath) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if($content){$lastContent=$content}
            if ($content -like '*UEFI:MOTION-READY*') { break }
        }
        if ($process.HasExited) { throw 'QEMU wurde vor dem Dialogtest beendet.' }
        Start-Sleep -Milliseconds 250
    }
    $client = [Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try {
        $writer = [IO.StreamWriter]::new($client.GetStream()); $writer.AutoFlush = $true
        foreach ($command in @('sendkey down','sendkey ret')) {
            $writer.WriteLine($command); Start-Sleep -Milliseconds 700
        }
        $deadline = [DateTime]::UtcNow.AddSeconds(60)
        while ([DateTime]::UtcNow -lt $deadline) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if($content){$lastContent=$content}
            if ($content -like '*UEFI:DIALOG-FRAME-READY*' -and
                $content -like '*UEFI:DIALOG-ENTER-COMPLETE*' -and
                $content -like '*UEFI:DIALOG-STABLE*') { break }
            Start-Sleep -Milliseconds 200
        }
        if ($content -notlike '*UEFI:DIALOG-FRAME-READY*' -or
            $content -notlike '*UEFI:DIALOG-ENTER-COMPLETE*' -or
            $content -notlike '*UEFI:DIALOG-STABLE*') {
            throw 'Der modale Dialogframe wurde nicht fertig.'
        }
        Start-Sleep -Milliseconds 300
        $writer.WriteLine('stop'); Start-Sleep -Milliseconds 100
        $writer.WriteLine("screendump $Screenshot")
        $deadline = [DateTime]::UtcNow.AddSeconds(8)
        $previousLength=-1
        do {
            Start-Sleep -Milliseconds 150
            $length=if(Test-Path -LiteralPath $shotPath){(Get-Item -LiteralPath $shotPath).Length}else{0}
            $stable=$length -gt 0 -and $length -eq $previousLength
            $previousLength=$length
        } while(!$stable -and [DateTime]::UtcNow -lt $deadline)
        if (!(Test-Path -LiteralPath $shotPath)) { throw 'Kein Dialog-Screenshot erzeugt.' }
        # QEMU legt die PPM-Datei bereits mit der Endgroesse an und schreibt die
        # Pixel danach weiter. Erst mehrere identische Inhaltspruefsummen sind ein
        # belastbares Fertigsignal.
        $hashDeadline=[DateTime]::UtcNow.AddSeconds(12)
        $lastHash='';$stableHashes=0
        while($stableHashes -lt 4 -and [DateTime]::UtcNow -lt $hashDeadline){
            Start-Sleep -Milliseconds 300
            $hash=(Get-FileHash -LiteralPath $shotPath -Algorithm SHA256).Hash
            if($hash -eq $lastHash){$stableHashes++}else{$lastHash=$hash;$stableHashes=0}
        }
        if($stableHashes -lt 4){throw 'Der Dialog-Screenshot wurde nicht stabil fertiggeschrieben.'}
        if($OpenOnly){
            $content=Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if(!$content){$content=$lastContent}
            $frames=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-FRAME')).Count
            $hits=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-CACHE-HIT')).Count
            $misses=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-CACHE-MISS')).Count
            $lifecycleSkips=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-LIFECYCLE-SKIP')).Count
            $contrastSkips=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-HIGH-CONTRAST-SKIP')).Count
            $geometryFailures=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-GEOMETRY-FAILED')).Count
            $renderFailures=([regex]::Matches($content,'UEFI:BACKGROUND-BLUR-FAILED')).Count
            $blocks=([regex]::Matches($content,'UEFI:DIALOG-BLUR-BLOCK')).Count
            $dialogFrames=([regex]::Matches($content,'UEFI:DIALOG-FRAME-READY')).Count
            $transparentFallbacks=([regex]::Matches($content,'UEFI:GLASS-DIALOG-TRANSPARENT-FALLBACK')).Count
            $opaqueFallbacks=([regex]::Matches($content,'UEFI:GLASS-DIALOG-OPAQUE-FALLBACK')).Count
            Write-Host "Dialog open-only: dialog-frames=$dialogFrames blocks=$blocks blur frames=$frames hits=$hits misses=$misses lifecycle-skips=$lifecycleSkips contrast-skips=$contrastSkips geometry-failures=$geometryFailures render-failures=$renderFailures"
            $dialogMarker=$content.LastIndexOf('UEFI:DIALOG-FRAME-READY')
            if($dialogMarker -ge 0){
                $markerStart=[Math]::Max(0,$dialogMarker-900)
                Write-Host $content.Substring($markerStart,$dialogMarker-$markerStart+25)
            }
            if($frames -lt 1 -and $transparentFallbacks -lt 1 -and $opaqueFallbacks -lt 1){
                throw 'Der stabile Dialog hat weder Glass-Blur noch einen definierten Materialfallback ausgefuehrt.'
            }
            return
        }
        $writer.WriteLine('cont'); Start-Sleep -Milliseconds 750
        $writer.WriteLine('sendkey esc')
        $deadline = [DateTime]::UtcNow.AddSeconds(60)
        while ([DateTime]::UtcNow -lt $deadline) {
            $content = Get-Content -LiteralPath $logPath -Raw -ErrorAction SilentlyContinue
            if($content){$lastContent=$content}
            if ($content -like '*UEFI:DIALOG-EXIT-COMPLETE*' -and
                $content -like '*UEFI:DIALOG-RESULT-CANCEL*' -and
                $content -like '*UEFI:PAGE-RESTORED*') { return }
            Start-Sleep -Milliseconds 100
        }
        $blurHits=([regex]::Matches($lastContent,'UEFI:BACKGROUND-BLUR-CACHE-HIT')).Count
        $blurMisses=([regex]::Matches($lastContent,'UEFI:BACKGROUND-BLUR-CACHE-MISS')).Count
        $blurFailures=([regex]::Matches($lastContent,'UEFI:BACKGROUND-BLUR-FAILED')).Count
        $blurFrames=([regex]::Matches($lastContent,'UEFI:BACKGROUND-BLUR-FRAME')).Count
        throw "Escape hat den modalen Dialog nicht abgebrochen. Blur frames=$blurFrames cache hits=$blurHits misses=$blurMisses failures=$blurFailures"
    } finally { $client.Dispose() }
} finally {
    if (!$process.HasExited) { Stop-Process -Id $process.Id -Force }
    $process.Dispose()
}
