param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$Image
)

$ErrorActionPreference='Stop'
$buildRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
$runDir=[IO.Path]::Combine($buildRoot,'nova-uefi-display-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($runDir)|Out-Null
$serial=[IO.Path]::Combine($runDir,'serial.log')
$debug=[IO.Path]::Combine($runDir,'debug.log')
$stderr=[IO.Path]::Combine($runDir,'stderr.log')
$firmwareCopy=[IO.Path]::Combine($runDir,'firmware.fd')
[IO.File]::Copy([IO.Path]::GetFullPath($Firmware),$firmwareCopy,$true)
$completed=$false
$listener=[Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback,0)
$listener.Start()
$qmpPort=([Net.IPEndPoint]$listener.LocalEndpoint).Port
$listener.Stop()

function Send-QmpKey([int]$Port,[string]$Key) {
    $client=[Net.Sockets.TcpClient]::new()
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(5)
        do {
            try {$client.Connect('127.0.0.1',$Port)}
            catch [Net.Sockets.SocketException] {
                if([DateTime]::UtcNow-ge$deadline){throw}
                Start-Sleep -Milliseconds 100
            }
        } while(-not$client.Connected)
        $reader=[IO.StreamReader]::new($client.GetStream())
        $writer=[IO.StreamWriter]::new($client.GetStream())
        $writer.AutoFlush=$true
        $null=$reader.ReadLine()
        $writer.WriteLine('{"execute":"qmp_capabilities"}')
        $null=$reader.ReadLine()
        $command=@{execute='human-monitor-command';arguments=@{'command-line'="sendkey $Key"}} | ConvertTo-Json -Compress
        $writer.WriteLine($command)
        $null=$reader.ReadLine()
        $writer.Dispose()
        $reader.Dispose()
    } finally {
        $client.Dispose()
    }
}

$arguments=@('-machine','q35','-m','256M','-smp','4',
    '-drive',"if=pflash,format=raw,file=$firmwareCopy",
    '-drive',"format=raw,file=$([IO.Path]::GetFullPath($Image)),if=ide",
    '-display','none','-monitor','none','-qmp',"tcp:127.0.0.1:$qmpPort,server=on,wait=off",'-serial',"file:$serial",
    '-debugcon',"file:$debug",'-global','isa-debugcon.iobase=0xe9',
    '-no-reboot','-no-shutdown')
$process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru `
    -RedirectStandardError $stderr
try {
    $deadline=[DateTime]::UtcNow.AddSeconds(90)
    $content=''
    do {
        Start-Sleep -Milliseconds 250
        $serialText=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
        $debugText=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
        $content=$debugText+$serialText
        if($process.HasExited){
            $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
            throw "QEMU wurde vor der Display-Server-Pruefung beendet. $detail"
        }
    } while(($content-notlike'*NOVA_KERNEL_READY*'-or$content-notlike'*NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert*')-and[DateTime]::UtcNow-lt$deadline)

    foreach($marker in @(
        'UEFI:KERNEL-HANDOFF-READY',
        'UEFI:BOOTSPLASH-READY',
        'NOVA: Display Server ABI 1.0, Firmware-Framebuffer uebernommen',
        'NOVA: Userspace Display.QueryPrimary ohne MMIO-Adresse erfolgreich',
        'NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert',
        'NOVA_KERNEL_READY')) {
        if($content-notlike"*$marker*"){throw "Erwartete Markierung fehlt: $marker"}
    }
    $initialPresentCount=([regex]::Matches($content,'NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert')).Count
    $initialInputCount=([regex]::Matches($content,'NOVA: Input-Router-Ereignis an Ring-3-System-UI zugestellt')).Count
    Send-QmpKey -Port $qmpPort -Key 'tab'
    do {
        Start-Sleep -Milliseconds 100
        $serialText=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
        $debugText=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
        $content=$debugText+$serialText
        $presentCount=([regex]::Matches($content,'NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert')).Count
        $inputCount=([regex]::Matches($content,'NOVA: Input-Router-Ereignis an Ring-3-System-UI zugestellt')).Count
    } while(($inputCount-le$initialInputCount-or$presentCount-le$initialPresentCount)-and[DateTime]::UtcNow-lt$deadline)
    if($inputCount-le$initialInputCount){throw 'Tab wurde nicht als geschuetztes Input-Router-Ereignis zugestellt'}
    if($presentCount-le$initialPresentCount){throw 'Ring-3-System-UI hat nach Tab keine neue Fokusszene praesentiert'}

    $initialPresentCount=$presentCount
    $initialInputCount=$inputCount
    Send-QmpKey -Port $qmpPort -Key 'esc'
    do {
        Start-Sleep -Milliseconds 100
        $serialText=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
        $debugText=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
        $content=$debugText+$serialText
        $presentCount=([regex]::Matches($content,'NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert')).Count
        $inputCount=([regex]::Matches($content,'NOVA: Input-Router-Ereignis an Ring-3-System-UI zugestellt')).Count
    } while(($inputCount-le$initialInputCount-or$presentCount-le$initialPresentCount)-and[DateTime]::UtcNow-lt$deadline)
    if($inputCount-le$initialInputCount){throw 'Escape wurde nicht als geschuetztes Input-Router-Ereignis zugestellt'}
    if($presentCount-le$initialPresentCount){throw 'Ring-3-System-UI hat nach Escape keine neue Szene praesentiert'}

    Send-QmpKey -Port $qmpPort -Key 'esc'
    do {
        Start-Sleep -Milliseconds 100
        $serialText=if(Test-Path -LiteralPath $serial){[string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)}else{''}
        $debugText=if(Test-Path -LiteralPath $debug){[string](Get-Content -LiteralPath $debug -Raw -ErrorAction SilentlyContinue)}else{''}
        $content=$debugText+$serialText
    } while($content-notlike'*NOVA: Power Shutdown PLATFORM_OFF*'-and[DateTime]::UtcNow-lt$deadline)
    if($content-notlike'*NOVA: Power Shutdown PLATFORM_OFF*'){
        throw 'Escape bei geschlossenem Startmenue hat keinen geordneten Shutdown ausgeloest'
    }
    Write-Host 'UEFI Display Server: Tab setzt Fokus; Escape schliesst Startmenue und startet danach den geordneten Shutdown'
    $completed=$true
} finally {
    if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
    $process.WaitForExit()
    $process.Dispose()
    $resolved=[IO.Path]::GetFullPath($runDir)
    if($completed-and$resolved.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)-and[IO.Directory]::Exists($resolved)){
        for($attempt=0;$attempt-lt20-and[IO.Directory]::Exists($resolved);$attempt++){
            try{[IO.Directory]::Delete($resolved,$true)}
            catch [IO.IOException]{if($attempt-eq19){throw};Start-Sleep -Milliseconds 100}
        }
    } elseif(!$completed) {
        Write-Host "Display-Testartefakte bleiben zur Diagnose erhalten: $resolved"
    }
}
