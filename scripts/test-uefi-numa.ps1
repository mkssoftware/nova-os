param(
    [Parameter(Mandatory=$true)][string]$Qemu,
    [Parameter(Mandatory=$true)][string]$Firmware,
    [Parameter(Mandatory=$true)][string]$Image
)

$ErrorActionPreference='Stop'
$buildRoot=[IO.Path]::GetFullPath((Join-Path (Get-Location).Path 'build'))
$runDir=[IO.Path]::Combine($buildRoot,'nova-uefi-numa-'+[Guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($runDir)|Out-Null
$serial=[IO.Path]::Combine($runDir,'serial.log')
$stderr=[IO.Path]::Combine($runDir,'stderr.log')
$firmwareCopy=[IO.Path]::Combine($runDir,'firmware.fd')
[IO.File]::Copy([IO.Path]::GetFullPath($Firmware),$firmwareCopy,$true)
$completed=$false

$arguments=@(
    '-machine','q35',
    '-m','256M',
    '-smp','4,sockets=2,cores=2,threads=1',
    '-object','memory-backend-ram,id=mem0,size=128M',
    '-object','memory-backend-ram,id=mem1,size=128M',
    '-numa','node,nodeid=0,cpus=0-1,memdev=mem0',
    '-numa','node,nodeid=1,cpus=2-3,memdev=mem1',
    '-drive',"if=pflash,format=raw,file=$firmwareCopy",
    '-drive',"format=raw,file=$([IO.Path]::GetFullPath($Image)),if=ide",
    '-display','none','-monitor','none','-serial',"file:$serial",
    '-no-reboot','-no-shutdown'
)

$process=Start-Process $Qemu -ArgumentList $arguments -WindowStyle Hidden -PassThru `
    -RedirectStandardError $stderr
try {
    $deadline=[DateTime]::UtcNow.AddSeconds(100)
    $content=''
    do {
        Start-Sleep -Milliseconds 250
        $content=if(Test-Path -LiteralPath $serial){
            [string](Get-Content -LiteralPath $serial -Raw -ErrorAction SilentlyContinue)
        }else{''}
        if($process.HasExited){
            $detail=if(Test-Path -LiteralPath $stderr){Get-Content -LiteralPath $stderr -Raw}else{''}
            throw "QEMU wurde vor der NUMA-Pruefung beendet. $detail"
        }
    } while($content-notlike'*NOVA_KERNEL_READY*'-and[DateTime]::UtcNow-lt$deadline)

    $marker='NOVA: ACPI SRAT validiert, CPU/Memory-Affinitaeten (hex): 0x00000004/0x00000003'
    if($content-notlike"*$marker*"){
        throw "Erwartete NUMA-Markierung fehlt: $marker"
    }
    if($content-notlike'*NOVA: CPU Manager bezieht Package, Core und Thread aus HAL Topology*'){
        throw 'CPU Manager erreichte den HAL-Topologieimport nicht'
    }
    if($content-notlike'*NOVA_KERNEL_READY*'){
        throw 'NUMA-Test erreichte NOVA_KERNEL_READY nicht'
    }
    Write-Host 'UEFI NUMA: SRAT mit 4 CPU- und 3 Memory-Affinitäten validiert'
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
        Write-Host "NUMA-Testartefakte bleiben zur Diagnose erhalten: $resolved"
    }
}
