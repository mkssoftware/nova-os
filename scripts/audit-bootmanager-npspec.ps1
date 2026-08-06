param(
    [string]$DocsRoot = 'docs/NPSPEC/Boot',
    [string]$OutputFile = 'build/bootmanager-npspec-coverage.md'
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $DocsRoot).Path
$files = Get-ChildItem -LiteralPath $root -Recurse -File -Filter '*.md' | Sort-Object FullName

$evidenceRules = @(
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-0002'; Status = 'Blockiert'; Evidence = 'BAP beschreibt Inhalte semantisch, definiert aber kein normatives Wire-Format: Magic-Wert, Headergröße, Feldoffsets/Endianness und Gesamtprüfsummenverfahren fehlen' },
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-0003'; Status = 'Blockiert'; Evidence = 'Resource-Index-API ist beschrieben, aber binäre Packung, Hash-/ID-Ableitung, Tabellenheader und Kollisionsverfahren sind nicht normativ festgelegt' },
    @{ Pattern = 'NPSPEC-KERNELLOADER-0001'; Status = 'Integriert'; Evidence = 'boot2.asm: NKI-bevorzugte Erkennung sowie validiertes direktes ELF32-/ELF64-Laden; QEMU-Positiv- und Negativtests' },
    @{ Pattern = 'NPSPEC-KERNELENTRY-0001'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm und kernel: definierter NBHP/BIB-Registerübergang und frühe Kernelvalidierung; UEFI-Kernelübergang fehlt' },
    @{ Pattern = 'NPSPEC-BOOT-0001-N'; Status = 'Teilweise integriert'; Evidence = 'BIOS Stage 1/2, versioniertes NBHP/BIB, NKI/ELF-Fallback, Fehlerpfade und erster UEFI-Plattformpfad' },
    @{ Pattern = 'NPSPEC-BOOTERROR-0001'; Status = 'Teilweise integriert'; Evidence = 'Einheitlicher Nova-Fehlerbildschirm für Boot- und Kernel-Fehler; vollständige Recovery-Backends fehlen' },
    @{ Pattern = 'NPSPEC-BOOTFS-0001'; Status = 'Teilweise integriert'; Evidence = 'Sektorbasierter read-only Kernelzugriff vorhanden; Dateisystemtreiber-Registry und NovaFS fehlen' },
    @{ Pattern = 'NPSPEC-BOOTGRAPHICS-0001'; Status = 'Teilweise integriert'; Evidence = 'BIOS-Framebuffer-Renderer und UEFI-GOP-Grundpfad; gemeinsame plattformneutrale Runtime fehlt' },
    @{ Pattern = 'NPSPEC-BOOTHANDOFF-0001'; Status = 'Teilweise integriert'; Evidence = 'TLV-basiertes NBHP/BIB wird im BIOS-Pfad aufgebaut und vom Kernel validiert; UEFI-Handoff fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-0001'; Status = 'Teilweise integriert'; Evidence = 'BIOS Bootmanager-Runtime, Entries, Auswahl und sichere Fallbacks; persistente Entry-Quellen fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMEMORY-0001'; Status = 'Teilweise integriert'; Evidence = 'Statische Speicherbereiche, Grenzen und lokaler Schreibtest; allgemeine Memory-Map-/Pool-Runtime fehlt' },
    @{ Pattern = 'NPSPEC-BOOTPLATFORM-0001'; Status = 'Teilweise integriert'; Evidence = 'BIOS- und getesteter UEFI-GOP-Einstieg vorhanden; einheitliche Plattform-vtable fehlt' },
    @{ Pattern = 'NPSPEC-BOOTSTAGE1-0001'; Status = 'Integriert'; Evidence = 'boot1.asm: deterministischer 512-Byte-Stage-1-Lader mit Signatur, Fehlerpfad und fester Stage-2-Grenze' },
    @{ Pattern = 'NPSPEC-BOOTSTAGE2-0001'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Loader, GUI, Diagnose und Handoff; modulare Ressourcenauslagerung ist durch BAP-Wire-Format blockiert' },
    @{ Pattern = 'NPSPEC-BOOTUI-'; Status = 'Teilweise integriert'; Evidence = 'BIOS-Runtime mit DLU, Zuständen, Eingabe, Fehlerpfaden und Tokens sowie UEFI-GOP-Grundpfad; gemeinsame Runtime/Pools fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0002'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_DETAILS, F2, Eigenschaften-/Statusansicht und Start-/Erweitert-Aktionen; Motion und Touch fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0003'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_ADVANCED, F3, fokussierbare Startmodusliste, Recovery-/Fallback-Aktionen; Kernelparametereditor fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0004'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: navigierbares Recovery-Menü mit Self-Healing, Snapshots, Speichertest und Datenträgerdiagnose; schreibende Recovery-Module fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0005'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Self-Healing-Systemstatus und sichere Sperre schreibender Reparaturen; NovaFS-Reparaturmodul fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0006'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Snapshot-Ansicht mit leerem, erklärtem Zustand; NovaFS-Snapshot-Katalog fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0007'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Speichertestansicht, AA55/55AA-Schnelltest, Ergebnis und Fortschritt; vollständiger physischer RAM-Test fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0008'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: read-only BIOS-Datenträgerstatus; GPT/MBR-, SMART- und Schreibmodule fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0009'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Verschlüsselungsstatus und sicher deaktivierte Authentifizierung ohne verschlüsseltes Volume; Crypto-/TPM-Backend fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0010'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Netzwerk-Boot-Status und erklärter No-Adapter/No-Server-Zustand; PXE-UNDI-/DHCP-Backend fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0011'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BIOS-Firmware- und Secure-Boot-Status; standardisierter Setup-Neustart ist nur unter UEFI möglich' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0015'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_POWER mit Herunterfahren, Neustart, Recovery/Firmware-Fallback und Abbrechen; Touch fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0018'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: globale F1-Hilfe, Tastatur-/Maus-/Shortcut-Hinweise und ESC-Navigation; kontextspezifische Karten fehlen' },
    @{ Pattern = 'NPSPEC-BOOTINPUT-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: bm_wait_input_event, bm_mouse_initialize, bm_mouse_consume_byte, bm_mouse_hit_test' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-'; Status = 'Teilweise integriert'; Evidence = 'boot-ui-tokens.inc; boot2.asm: DLU-Tokens, Safe Area, Wrapped Text' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: datengetriebene Menüeinträge, Fokusmarker, Icon-/Text-Control, Hit-Testing' },
    @{ Pattern = 'NPSPEC-BOOTTEXT-'; Status = 'Teilweise integriert'; Evidence = 'boot-font-aa.inc; boot2.asm: bm_draw_text, bm_draw_text_wrapped, bm_measure_text' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-'; Status = 'Teilweise integriert'; Evidence = 'boot-ui-tokens.inc; boot2.asm: Theme-Tokens und Hochkontrastmodus' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: vollständiger BIOS-GUI-Pfad; uefi/main.c und bootmenu/ui.c: getesteter GOP-Menü- und Countdown-Grundpfad' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: gemeinsame Seitenbasis und einheitliche ESC-/Backspace-Navigation' },
    @{ Pattern = 'NPSPEC-BOOTNAV-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: bm_view-Zustandsmodell und bm_redraw_current_view' },
    @{ Pattern = 'NPSPEC-BOOTRENDER-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Framebuffer-Primitiven, Alpha-Text, Rundungen und Clipping' },
    @{ Pattern = 'NPSPEC-BOOTCOMPOSITOR-'; Status = 'Teilweise integriert'; Evidence = 'compositor.c: feste Surface-/Layer-Pools, stabile Z-Sortierung, Alpha, Overlay/Modal, Damage Merge, Glass/Acrylic und Safe-Fallback; BIOS-Anbindung und echter Blur fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMOTION-'; Status = 'Teilweise integriert'; Evidence = 'motion.c: globale Millisekunden-Timeline, O(n)-Pool-Scheduler, Properties, Easing/Spring-Fallback, Transition/Dialog/Navigation/Fokus/Progress, Unterbrechung, Reduced Motion und Budget-Degradation; BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-'; Status = 'Teilweise integriert'; Evidence = 'Statisch validierte Font-, Icon- und Logo-Ressourcen; BAP-Loader fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIAG-'; Status = 'Teilweise integriert'; Evidence = 'Debug-Port-Ereignisse und Diagnoseansicht; Statistikmodell fehlt' },
    @{ Pattern = 'NPSPEC-BOOTTEST-'; Status = 'Teilweise integriert'; Evidence = 'Makefile: BIOS-, Maus-, Theme-, UI-, Recovery-, Plattform-, UEFI-, ELF- und Fehlertests' },
    @{ Pattern = 'NPSPEC-BOOTPERF-'; Status = 'Teilweise integriert'; Evidence = 'Feste Speichergrößen und keine Laufzeitallokation; vollständige Budgets fehlen' }
)

$rows = foreach ($file in $files) {
    $raw = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8
    $relative = $file.FullName.Substring($root.Length + 1).Replace('\', '/')
    $id = [regex]::Match($file.BaseName, 'NPSPEC-[A-Z0-9-]+').Value.TrimEnd('-')
    if ([string]::IsNullOrWhiteSpace($id)) { continue }
    if ([string]::IsNullOrWhiteSpace($raw)) {
        [pscustomobject]@{ Id=$id; File=$relative; Status='Blockiert'; Evidence='Spezifikationsdatei ist leer' }
        continue
    }
    $rule = $evidenceRules | Where-Object { $id -like "$($_.Pattern)*" } | Select-Object -First 1
    if ($rule) {
        [pscustomobject]@{ Id=$id; File=$relative; Status=$rule.Status; Evidence=$rule.Evidence }
    } else {
        [pscustomobject]@{ Id=$id; File=$relative; Status='Prüfung offen'; Evidence='Noch keine explizite Implementierungszuordnung' }
    }
}

$lines = @(
    '# NovaOS Bootmanager NPSPEC-Abdeckung',
    '',
    "Erzeugt: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')",
    '',
    "Dokumente: $($rows.Count)",
    '',
    '| NPSPEC | Status | Implementierungsnachweis | Datei |',
    '|---|---|---|---|'
)
foreach ($row in $rows) {
    $lines += "| $($row.Id) | $($row.Status) | $($row.Evidence) | $($row.File) |"
}

$target = [IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputFile))
$directory = Split-Path -Parent $target
if (-not (Test-Path -LiteralPath $directory)) { New-Item -ItemType Directory -Path $directory | Out-Null }
[IO.File]::WriteAllLines($target, $lines, [Text.UTF8Encoding]::new($false))

$rows | Group-Object Status | Sort-Object Name | ForEach-Object {
    Write-Host ("{0}: {1}" -f $_.Name, $_.Count)
}
Write-Host "Abdeckungsmatrix: $target"
