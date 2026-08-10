param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$FatDirectory,
    [Parameter(Mandatory=$true)][string]$DebugLog,
    [int]$MonitorPort=45469
)
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$env:TMP=[IO.Path]::GetFullPath((Join-Path $root 'build'));$env:TEMP=$env:TMP
$log=[IO.Path]::GetFullPath((Join-Path $root $DebugLog))
if(Test-Path -LiteralPath $log){Remove-Item -LiteralPath $log -Force}
$arguments=@('-machine','q35','-drive',"if=pflash,format=raw,snapshot=on,file=$Firmware",
 '-drive',"format=raw,file=fat:rw:$FatDirectory",'-display','none','-serial','none',
 '-monitor',"tcp:127.0.0.1:$MonitorPort,server=on,wait=off",'-debugcon',"file:$log",
 '-global','isa-debugcon.iobase=0xe9','-no-reboot','-no-shutdown')
$process=Start-Process -FilePath $Qemu -ArgumentList $arguments -WorkingDirectory $root `
 -WindowStyle Hidden -PassThru
try{
    $deadline=[DateTime]::UtcNow.AddSeconds(90);$content=''
    do{Start-Sleep -Milliseconds 250
       if(Test-Path -LiteralPath $log){$content=Get-Content -LiteralPath $log -Raw}
       if($process.HasExited){throw 'QEMU wurde vor dem Software-Renderer-Test beendet.'}
    }while($content-notlike'*UEFI:MOTION-READY*'-and[DateTime]::UtcNow-lt$deadline)
    if($content-notlike'*UEFI:MOTION-READY*'){throw 'Bootmanager wurde nicht rechtzeitig bereit.'}
    $client=[Net.Sockets.TcpClient]::new('127.0.0.1',$MonitorPort)
    try{
        $writer=[IO.StreamWriter]::new($client.GetStream());$writer.AutoFlush=$true
        $writer.WriteLine('sendkey down')
        $deadline=[DateTime]::UtcNow.AddSeconds(30)
        do{Start-Sleep -Milliseconds 200;$content=Get-Content -LiteralPath $log -Raw
        }while($content-notlike'*UEFI:COUNTDOWN-CANCELLED*'-and
                [DateTime]::UtcNow-lt$deadline)
        if($content-notlike'*UEFI:COUNTDOWN-CANCELLED*'){throw 'Countdown wurde nicht abgebrochen.'}
        Start-Sleep -Seconds 8
        foreach($key in @('home','down','down','down','ret')){
            [string]$current=Get-Content -LiteralPath $log -Raw
            $before=([regex]::Matches($current,'UEFI:MENU-DRAWN')).Count
            $writer.WriteLine("sendkey $key")
            $keyDeadline=[DateTime]::UtcNow.AddSeconds(25)
            do{Start-Sleep -Milliseconds 200;[string]$content=Get-Content -LiteralPath $log -Raw
               $after=([regex]::Matches($content,'UEFI:MENU-DRAWN')).Count
            }while($after-le$before-and[DateTime]::UtcNow-lt$keyDeadline)
            if($after-le$before){throw "Taste $key erzeugte keinen neuen Menüframe."}
        }
        $deadline=[DateTime]::UtcNow.AddSeconds(90)
        do{Start-Sleep -Milliseconds 250;$content=Get-Content -LiteralPath $log -Raw
        }while(($content-notlike'*UEFI:DIAGNOSTICS-VIEW*'-or
                 $content-notlike'*UEFI:NAV-ENTER-COMPLETE*')-and
                [DateTime]::UtcNow-lt$deadline)
        if($content-notlike'*UEFI:DIAGNOSTICS-VIEW*'){throw 'Diagnoseseite wurde nicht geöffnet.'}
        $writer.WriteLine('sendkey f11')
        $deadline=[DateTime]::UtcNow.AddSeconds(60)
        do{Start-Sleep -Milliseconds 200;$content=Get-Content -LiteralPath $log -Raw
        }while($content-notlike'*UEFI:SOFTWARE-RENDERER-RECOVERED*'-and
                [DateTime]::UtcNow-lt$deadline)
        if($content-notlike'*UEFI:SOFTWARE-RENDERER-RESET*'-or
           $content-notlike'*UEFI:SOFTWARE-RENDERER-RECOVERED*'){
            throw 'Software-Renderer wurde nach injiziertem Fehler nicht wiederhergestellt.'
        }
    }finally{$client.Dispose()}
}finally{
    if(!$process.HasExited){Stop-Process -Id $process.Id -Force}
    $process.Dispose()
}
