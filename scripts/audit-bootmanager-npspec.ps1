param(
    [string]$DocsRoot = 'docs/NPSPEC/Boot',
    [string]$OutputFile = 'build/bootmanager-npspec-coverage.md'
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $DocsRoot).Path
$files = Get-ChildItem -LiteralPath $root -Recurse -File -Filter '*.md' | Sort-Object FullName

$evidenceRules = @(
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0002'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_DETAILS, F2, Eigenschaften-/Statusansicht und Start-/Erweitert-Aktionen; Motion und Touch fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0003'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_ADVANCED, F3, fokussierbare Startmodusliste, Recovery-/Fallback-Aktionen; Kernelparametereditor fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0004'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: navigierbares Recovery-Menü mit Self-Healing, Snapshots, Speichertest und Datenträgerdiagnose; schreibende Recovery-Module fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0005'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Self-Healing-Systemstatus und sichere Sperre schreibender Reparaturen; NovaFS-Reparaturmodul fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0006'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Snapshot-Ansicht mit leerem, erklärtem Zustand; NovaFS-Snapshot-Katalog fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0007'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Speichertestansicht, AA55/55AA-Schnelltest, Ergebnis und Fortschritt; vollständiger physischer RAM-Test fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0008'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: read-only BIOS-Datenträgerstatus; GPT/MBR-, SMART- und Schreibmodule fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0015'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_POWER mit Herunterfahren, Neustart, Recovery/Firmware-Fallback und Abbrechen; Touch fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0018'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: globale F1-Hilfe, Tastatur-/Maus-/Shortcut-Hinweise und ESC-Navigation; kontextspezifische Karten fehlen' },
    @{ Pattern = 'NPSPEC-BOOTINPUT-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: bm_wait_input_event, bm_mouse_initialize, bm_mouse_consume_byte, bm_mouse_hit_test' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-'; Status = 'Teilweise integriert'; Evidence = 'boot-ui-tokens.inc; boot2.asm: DLU-Tokens, Safe Area, Wrapped Text' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: datengetriebene Menüeinträge, Fokusmarker, Icon-/Text-Control, Hit-Testing' },
    @{ Pattern = 'NPSPEC-BOOTTEXT-'; Status = 'Teilweise integriert'; Evidence = 'boot-font-aa.inc; boot2.asm: bm_draw_text, bm_draw_text_wrapped, bm_measure_text' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-'; Status = 'Teilweise integriert'; Evidence = 'boot-ui-tokens.inc; boot2.asm: Theme-Tokens und Hochkontrastmodus' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Hauptmenü, Countdown, Einstellungen, Diagnose, Recovery, Fehler und Shutdown' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: gemeinsame Seitenbasis und einheitliche ESC-/Backspace-Navigation' },
    @{ Pattern = 'NPSPEC-BOOTNAV-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: bm_view-Zustandsmodell und bm_redraw_current_view' },
    @{ Pattern = 'NPSPEC-BOOTRENDER-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Framebuffer-Primitiven, Alpha-Text, Rundungen und Clipping' },
    @{ Pattern = 'NPSPEC-BOOTCOMPOSITOR-'; Status = 'Offen'; Evidence = 'Noch kein separater Surface-/Layer-Compositor' },
    @{ Pattern = 'NPSPEC-BOOTMOTION-'; Status = 'Offen'; Evidence = 'Countdown ist zeitbasiert; allgemeiner Scheduler fehlt' },
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-'; Status = 'Teilweise integriert'; Evidence = 'Statisch validierte Font-, Icon- und Logo-Ressourcen; BAP-Loader fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIAG-'; Status = 'Teilweise integriert'; Evidence = 'Debug-Port-Ereignisse und Diagnoseansicht; Statistikmodell fehlt' },
    @{ Pattern = 'NPSPEC-BOOTTEST-'; Status = 'Teilweise integriert'; Evidence = 'Makefile: test, test-mouse, test-theme, test-ui-flows sowie ELF-/Fehlertests' },
    @{ Pattern = 'NPSPEC-BOOTPERF-'; Status = 'Teilweise integriert'; Evidence = 'Feste Speichergrößen und keine Laufzeitallokation; vollständige Budgets fehlen' }
)

$rows = foreach ($file in $files) {
    $raw = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8
    $relative = $file.FullName.Substring($root.Length + 1).Replace('\', '/')
    $id = [regex]::Match($file.BaseName, 'NPSPEC-[A-Z0-9-]+').Value.TrimEnd('-')
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
