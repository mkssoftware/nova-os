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
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-0001'; Status = 'Teilweise integriert'; Evidence = 'resources.c: zentrale URI/ID-Registry, fester Pool, CRC32-Prüfung, Lazy Load, Cache und Referenzzählung; Font, Icons und gemeinsames NovaOS-Logo sind registriert; BAP-/Dateisystemloader bleibt blockiert' },
    @{ Pattern = 'NPSPEC-BOOTTEXT-0003'; Status = 'Teilweise integriert'; Evidence = 'unicode.c/text.c: validierender UTF-8-Decoder bis U+10FFFF, Overlong-/Surrogat-Abweisung, U+FFFD-Missing-Glyph, UTF-8-Umlaute, Block-/Fehlertests und Diagnose; internationale Glyphatlanten fehlen' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0009'; Status = 'Teilweise integriert'; Evidence = 'icons.c/ui.c: alle 15 semantischen O(1)-Tokens besitzen zentral registrierte, geprüfte 2-Bit-AA-Ressourcen; Unterseiten verwenden eigene Tokenzuordnungen, DLU-Skalierung, Theme-Tint, Cache-Diagnose und ausschließlich einen Fallback für ungültige externe Tokens; allgemeiner SVG-Renderer und BIOS-Parität der neun erweiterten Symbole fehlen' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0002'; Status = 'Teilweise integriert'; Evidence = 'theme.c: validierte statische O(1)-Token-Tabellen für Farben, Abstände, Radien, Transparenz, Material und Motion-Overrides; Typography-/Shadow-Registry noch nicht vollständig' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0003'; Status = 'Teilweise integriert'; Evidence = 'theme.c/ui.c: sämtliche UEFI-Farben über semantische Rollen, deterministischer Kontrasttest, Focus/Selected/Disabled/Statusfarben und Accessibility-Overrides; BIOS nutzt weiterhin eigene äquivalente Tokenquelle' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0012'; Status = 'Teilweise integriert'; Evidence = 'UEFI und BIOS: Dark Theme; UEFI aus zentral validierter Token-Tabelle mit dynamischen Controls, Icons, Branding, Fokus und Materialien; allgemeine externe Theme-Resource fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0013'; Status = 'Teilweise integriert'; Evidence = 'theme.c: vollständiger statischer Light-Token-Satz; QEMU-Wechsel und visueller Framebuffer-Nachweis; BIOS-Light-Theme fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0014'; Status = 'Teilweise integriert'; Evidence = 'theme.c: High Contrast mit opaken Flächen, Rahmen, gelbem Fokus, weißen Texten, Reduced Motion und Materialfallback; QEMU-Wechsel und visueller Nachweis; Screenreader fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-0015'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI verwenden dieselbe generierte 2-Bit-AA-NovaOS-Bildmarke, Nova-Blau und Wortmarke; UEFI Bootmanager spiegelt den Kopf spezifikationsgemäß nach rechts; vollständige visuelle BIOS/UEFI-Parität aller Themes fehlt' },
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-0011'; Status = 'Teilweise integriert'; Evidence = 'Dark-, Light- und High-Contrast-Deskriptoren sind als geprüfte Theme-Ressourcen zentral registriert; externer BAP-Loader bleibt durch fehlendes Wire-Format blockiert' },
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
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0001'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI: Control-basierte Hauptauswahl, Icons, sichtbarer Fokus, Tastatur/Maus, Statuszeile und sicherer Standardstart; Multi-Boot-Discovery, Touch und vollständiges Informationspanel fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0002'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_DETAILS, F2, Eigenschaften-/Statusansicht und Start-/Erweitert-Aktionen; Motion und Touch fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0003'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BM_VIEW_ADVANCED, F3, fokussierbare Startmodusliste, Recovery-/Fallback-Aktionen; Kernelparametereditor fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0004'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI: navigierbares Recovery-Menü, sichere Nicht-verfügbar-Zustände und Rücknavigation; geprüfte schreibende Recovery-Module fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0005'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Self-Healing-Systemstatus und sichere Sperre schreibender Reparaturen; NovaFS-Reparaturmodul fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0006'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Snapshot-Ansicht mit leerem, erklärtem Zustand; NovaFS-Snapshot-Katalog fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0007'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Speichertestansicht, AA55/55AA-Schnelltest, Ergebnis und Fortschritt; vollständiger physischer RAM-Test fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0008'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: read-only BIOS-Datenträgerstatus; GPT/MBR-, SMART- und Schreibmodule fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0009'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Verschlüsselungsstatus und sicher deaktivierte Authentifizierung ohne verschlüsseltes Volume; Crypto-/TPM-Backend fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0010'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Netzwerk-Boot-Status und erklärter No-Adapter/No-Server-Zustand; PXE-UNDI-/DHCP-Backend fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0011'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: BIOS-Firmware- und Secure-Boot-Status; standardisierter Setup-Neustart ist nur unter UEFI möglich' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0014'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI: read-only Diagnoseansicht mit Übersicht, Hardware-, Boot-, Speicher- und Ereignisbereichen; vollständige Hardwareprovider und Berichtsexport fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0015'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI: Energieauswahl mit expliziter zweiter Bestätigung; UEFI Shutdown/Restart über Runtime ResetSystem, sichere Sperre nicht verfügbarer Recovery-/Firmware-/Netzwerkziele; Touch fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0016'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI: sichtbarer, sekündlich aktualisierter Fünf-Sekunden-Countdown, Abbruch durch Eingabe und sicherer Standardstart; persistente Konfiguration fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-0018'; Status = 'Teilweise integriert'; Evidence = 'BIOS und UEFI: F1-Hilfe, lokale Bedienhinweise, ESC-Rücknavigation und Fokuswiederherstellung; vollständiger lokalisierter Help-Index fehlt' },
    @{ Pattern = 'NPSPEC-BOOTINPUT-'; Status = 'Teilweise integriert'; Evidence = 'BIOS: PS/2-Tastatur/Maus; input.c: gemeinsame O(1)-Eventqueue, Pointer Capture/Hit-Test, Fokus, Shortcuts und Repeat; UEFI: Tastatur und Simple Pointer mit Cursor, Auswahl, Klickaktion und Countdown-Abbruch; QEMU weist Pointer-Protokoll und Tastaturnavigation nach, headless HMP-Mausinjektion und Touch fehlen' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-0001'; Status = 'Teilweise integriert'; Evidence = 'layout.c/ui.c: heapfreie Measure-/Arrange-/Finalize-Laufzeit, Invalidierung und ausschließlich berechnete Bounds; allgemeiner Containerbaum und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-0006'; Status = 'Teilweise integriert'; Evidence = 'layout.c: Compact/Standard/Wide-Breakpoints, 3%-Safe-Area und adaptive Brand-/Panel-Anordnung; QEMU-Nachweis in drei echten GOP-Modi, Touch-/Rotationstest fehlt' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-0007'; Status = 'Teilweise integriert'; Evidence = 'layout.c: deterministische Ganzzahlskalierung von 1000 bis 2000 sowie Accessibility-Untergrenze; Firmware-DPI ist mangels physischer Displaymetrik sicher auf 96 DPI gesetzt' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-0008'; Status = 'Teilweise integriert'; Evidence = 'layout.c: zentrale nova_dlu_to_pixels-Rundung; Panel, Abstände, Radien, Icons und Textskalierung werden daraus abgeleitet; BIOS nutzt eigene kompatible DLU-Tokens' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-0009'; Status = 'Teilweise integriert'; Evidence = 'layout.c: validierter Min/Preferred/Max-Constraint-Resolver mit deterministischem Fehlerzähler; vollständige verschachtelte Container-Priorisierung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-0010'; Status = 'Teilweise integriert'; Evidence = 'layout.c/text.c: Safe-Area, skalierte Textmessung, Ellipsis und Layoutklassen; Hosttests decken 640x480 bis 4K ab, UEFI-QEMU 800x600/1280x720/1920x1080' },
    @{ Pattern = 'NPSPEC-BOOTLAYOUT-'; Status = 'Teilweise integriert'; Evidence = 'layout.c/ui.c: DLU-basierte responsive UEFI-Layoutlaufzeit mit Safe Area und Constraints; BIOS-/UEFI-Parität ist noch offen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-PROGRESS-0001'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c/motion.c: bestimmter und unbestimmter Progress, 0..1000-Range, Status-/Prozentdarstellung, Theme/High Contrast und QEMU-Abschlussflow; eigenständiges BIOS-Control fehlt' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-PASSWORD-0001'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: UTF-8-Secret-Puffer, Maskierung, Längengrenze, codepointweises Löschen und volatile Speicherbereinigung; Cursor/Selection/Paste und Authentifizierungsbackend fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-SWITCH-0001'; Status = 'Teilweise integriert'; Evidence = 'controls.c/ui.c: typisierter Off/On-/Locked-/Busy-Zustand, rundes Track-/Knob-Rendering, Theme/High Contrast, Accessibility und echter Reduced-Motion-Switch in den UEFI-Einstellungen; Motion und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-SLIDER-0001'; Status = 'Teilweise integriert'; Evidence = 'controls.c/ui.c/main.c: produktiver Tooltip-Delay-Slider 250..1500 ms mit 250-ms-Schritten, Clamp, Wertlabel, Links/Rechts/Home/End, Enter/Space, Pointer-Drag, Theme, Accessibility und QEMU-Ablauftest; Touch/Controller, vertikale Variante und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-CHECKBOX-0001'; Status = 'Teilweise integriert'; Evidence = 'controls.c/ui.c/main.c: produktive unabhängige Tooltip-Checkbox mit Checked/Unchecked/Indeterminate-Grundmodell, Enter/Space/Pointer, Theme, Accessibility, exakt einer Änderung je Aktivierung und QEMU-Nachweis; Touch/Controller und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-STATUSBADGE-0001'; Status = 'Teilweise integriert'; Evidence = 'controls.c/ui.c: thematische, abgerundete Status-Badges zeigen Theme, Tastatur- und Sprachzustand in den UEFI-Einstellungen; vollständige Statusvarianten und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-CONTEXTMENU-0001'; Status = 'Teilweise integriert'; Evidence = 'ui.c/main.c: exklusives heapfreies Context Menu am Ziel-Control, Safe-Area-Korrektur, Fokusfang, F2/Rechtsklick, Pfeile, Enter, Escape, Outside-Click und sichere Detail-/Optionsaktionen; Untermenüs, Touch und BIOS-Anbindung fehlen; QEMU-Frame und Aktion nachgewiesen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-TEXTFIELD-0001'; Status = 'Teilweise integriert'; Evidence = 'controls.c: fester UTF-8-Puffer, Caret/Selection, codepointweises Insert/Backspace/Delete/Move, Maximalgröße, Standard-/Zahlen-/Hex-/Passwort-/Dateiname-/Suchfilter, Readonly/Locked/Busy, Placeholder, Maskierung, Caret/Selection/Error-Renderer und Host-Grenztests; virtuelle Tastatur, Pointer-Selection und produktiver UEFI-Editor fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-TOOLTIP-0001'; Status = 'Teilweise integriert'; Evidence = 'ui.c/main.c: genau ein zentraler Tooltip mit produktiv konfigurierbarer Aktivierung und 250..1500-ms-Verzögerung, Safe-Area-Positionierung, Theme/High Contrast, kein Fokus-/Input-Capture und sofortigem Ausblenden bei Eingabe, Dialog oder Context Menu; Motion/Touch und BIOS-Anbindung fehlen; QEMU-Frames visuell geprüft' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-BREADCRUMB-0001'; Status = 'Teilweise integriert'; Evidence = 'ui.c/navigation.c: Unterseiten zeigen den aus dem aktiven Navigationszustand abgeleiteten Pfad Start/Chevron/Seite, semantisches Control, Themefarben, DLU-Layout und QEMU-Frame; anklickbare Elternknoten, tiefere Hierarchien und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-0001'; Status = 'Teilweise integriert'; Evidence = 'controls.c/ui.c: heapfreier 128er Pool, Typen, Parent/Child, Lifecycle, Bounds, Dirty-State, Fokus, Hit-Test, exakt-einmal Aktionen, Styles, Accessibility, Werte und typisierte Renderer; Ereignisbubbling und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-0002'; Status = 'Teilweise integriert'; Evidence = 'controls.c: Haupt-, Sichtbarkeits-, Fokus-, Hover-, Pressed-, Selected-, Busy-, Error-, Checked-, Readonly- und Locked-Zustände mit validierten Übergängen und Diagnostik; Zustandsmotion und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTCONTROL-'; Status = 'Teilweise integriert'; Evidence = 'controls.c/ui.c und boot2.asm: gemeinsames typisiertes Controlmodell, datengetriebene Menüeinträge, Fokus, Icons/Text, Hit-Testing und sichere Fallbackdarstellung; spezialisierte Controls werden schrittweise angebunden' },
    @{ Pattern = 'NPSPEC-BOOTTEXT-'; Status = 'Teilweise integriert'; Evidence = 'boot-font-aa.inc; boot2.asm: bm_draw_text, bm_draw_text_wrapped, bm_measure_text' },
    @{ Pattern = 'NPSPEC-BOOTDESIGN-'; Status = 'Teilweise integriert'; Evidence = 'boot-ui-tokens.inc; boot2.asm: Theme-Tokens und Hochkontrastmodus' },
    @{ Pattern = 'NPSPEC-BOOTMANAGER-UI-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: vollständiger BIOS-GUI-Pfad; uefi/main.c und bootmenu/ui.c: getesteter GOP-Menü- und Countdown-Grundpfad' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0001'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: fester Dialogpool und -stack, vollständige Lifecycle-Reihenfolge, Typen, modale Root, Ergebnis und Ressourcenfreiheit; nichtmodale UEFI-Nutzung und BIOS-Parität fehlen' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0002'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: genau ein aktiver modaler Dialog, Backdrop, Eingabesperre, Safe-Area-Zentrierung, Fokusfang und Escape-Abbruch; Touch fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0003'; Status = 'Teilweise integriert'; Evidence = 'ui.c/compositor.c: abgerundete thematische Dialogfläche, Alpha-Backdrop sowie Glass/Acrylic-Fallback-Infrastruktur; echter Blur-Cache fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0004'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/main.c: Confirmation-Typ, fokussierbare Aktionen, eindeutiges Resultat und sichere Abbruchaktion; BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0005'; Status = 'Teilweise integriert'; Evidence = 'ui.c/main.c: thematisch markierter Warning-Dialog für blockierte Recovery-, Firmware- und Neustartziele mit verständlichem Text' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0006'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: Error-Typ, Fehlerfarbtoken und Ergebnisweg; fataler Nova-Fehlerbildschirm existiert separat, Retry-/Detailpfad fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0007'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c/main.c: sichtbarer bestimmter/unbestimmter Progress, Prozent, Statuswechsel, Abschluss und Close; Werte werden begrenzt, QEMU-Frame nachgewiesen; Task-/Restzeitbackend und BIOS-Parität fehlen' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0008'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: statischer 128-Byte-Secret-Puffer, UTF-8-Eingabe, Maskierung, codepointweises Backspace, Boundsprüfung und volatile Nullung beim Close; kein Crypto-/TPM-Backend, daher sicher nicht im normalen Flow angefordert' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0009'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: zyklischer interner Fokus, modale Pointer-Hit-Tests, gespeicherte Vorfokus-ID und Blockade des Hintergrunds; Screenreader/Touch fehlen' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0010'; Status = 'Teilweise integriert'; Evidence = 'motion.c/main.c/ui.c: UEFI Enter/Exit mit 180-ms-Fade und 95-auf-100-Prozent-Scale, 150-ms-Reduced-Motion-Fade ohne Scale, finalem Fokus, sicherem Abbruch und stabil synchronisiertem QEMU-Frame; BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0011'; Status = 'Teilweise integriert'; Evidence = 'dialog.c: genau ein typisiertes Resultat, O(1)-Close, Cancel-Regel und oberster-Dialog-Priorität; asynchroner Callback fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-0012'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/main.c: zerstörerische Aktion erfordert nach Fokuswahl eine zweite explizite Bestätigung; QEMU weist Dialog-Arming und echtes ResetSystem-Shutdown nach' },
    @{ Pattern = 'NPSPEC-BOOTNAV-0001'; Status = 'Teilweise integriert'; Evidence = 'navigation.c/page.c/main.c: zentraler heapfreier Zustandscontroller, Navigation Tree, Stack, Fokus, Dialog-/Recovery-Priorität und Transitionen; alternative Eingabegeräte und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTNAV-0002'; Status = 'Teilweise integriert'; Evidence = 'navigation.c: spezifikationsgemäßer statischer 64er O(1)-Stack mit Push/Pop/Replace/Reset, Root-Schutz, vollständigem State und Diagnose; Dialogstack separat, Recovery als typisierter Zweig; BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTNAV-0003'; Status = 'Teilweise integriert'; Evidence = 'page.c/ui.c: feste 16-Page-/64-View-Pools, eindeutige Root-Bäume, strikte Lifecycle, Sichtbarkeit, Layoutboxen, Fokus/Accessibility, iterative Events, Zyklus-/ID-Prüfung und modale Dialog-Pages; QEMU-Aktivierung/-Restore belegt, BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTNAV-0004'; Status = 'Teilweise integriert'; Evidence = 'navigation.c/main.c: Dialog vor Page, O(1)-Back, Root-Schutz und Wiederherstellung von Seite/Auswahl/Fokus/Scroll/Kontext; Recovery-Unterstack ist noch nicht separat typisiert' },
    @{ Pattern = 'NPSPEC-BOOTNAV-0005'; Status = 'Teilweise integriert'; Evidence = 'navigation.c/main.c/ui.c: sichtbarer 32-DLU Slide+Fade für Push/Pop, spiegelbildliche Richtung, Cross-Fade für Replace, ruhiger Recovery-Fade, Fokus-Sperre und Reduced-Motion-Fade; QEMU weist transformierte Frames/Richtungen nach, BIOS-Parität fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIALOG-'; Status = 'Teilweise integriert'; Evidence = 'dialog.c/ui.c: gemeinsame heapfreie UEFI-Dialoglaufzeit; spezialisierte Backends und BIOS-Parität fehlen' },
    @{ Pattern = 'NPSPEC-BOOTNAV-'; Status = 'Teilweise integriert'; Evidence = 'navigation.c/main.c: UEFI-Navigation-Stack mit Zustandswiederherstellung; allgemeiner View-Tree und BIOS-Parität fehlen' },
    @{ Pattern = 'NPSPEC-BOOTRENDER-'; Status = 'Teilweise integriert'; Evidence = 'boot2.asm: Framebuffer-Primitiven, Alpha-Text, Rundungen und Clipping' },
    @{ Pattern = 'NPSPEC-BOOTCOMPOSITOR-'; Status = 'Teilweise integriert'; Evidence = 'compositor.c: feste Surface-/Layer-Pools, stabile Z-Sortierung, Alpha, Overlay/Modal, Damage Merge, Glass/Acrylic und Safe-Fallback; BIOS-Anbindung und echter Blur fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMOTION-0007'; Status = 'Teilweise integriert'; Evidence = 'motion.c/navigation.c: zentrale Property-Timeline erzeugt gruppierte Offset-/Opacity-Transitionen, sperrt Fokus, unterstützt Cancel und Reduced Motion; Materialgruppen und BIOS-Anbindung fehlen' },
    @{ Pattern = 'NPSPEC-BOOTMOTION-0009'; Status = 'Teilweise integriert'; Evidence = 'main.c/ui.c/navigation.c: alle UEFI-Menüwechsel laufen über Exit/atomaren Stackwechsel/Enter; Push links, Pop rechts, Recovery/Replace Fade, deterministische Software-Keyframes und QEMU-Transformnachweis; BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTMOTION-'; Status = 'Teilweise integriert'; Evidence = 'motion.c: globale Millisekunden-Timeline, O(n)-Pool-Scheduler, Properties, Easing/Spring-Fallback, Transition/Dialog/Navigation/Fokus/Progress, Unterbrechung, Reduced Motion und Budget-Degradation; BIOS-Anbindung fehlt' },
    @{ Pattern = 'NPSPEC-BOOTRESOURCE-'; Status = 'Teilweise integriert'; Evidence = 'Statisch validierte Font-, Icon- und Logo-Ressourcen; BAP-Loader fehlt' },
    @{ Pattern = 'NPSPEC-BOOTDIAG-'; Status = 'Teilweise integriert'; Evidence = 'diagnostics.c: strukturierter 256er Ringpuffer, Schweregrade, Rendering-/Input-/Motion-/Resource-Snapshots, Frame-/Speicher-/Qualitätsmetriken und QEMU-Initialisierungsnachweis; persistenter Export fehlt' },
    @{ Pattern = 'NPSPEC-BOOTTEST-'; Status = 'Teilweise integriert'; Evidence = 'Makefile: BIOS-, Maus-, Theme-, UI-, Recovery-, Plattform-, UEFI-Autostart-, UEFI-Eventqueue/Navigation-, ELF- und Fehlertests' },
    @{ Pattern = 'NPSPEC-BOOTPERF-'; Status = 'Teilweise integriert'; Evidence = 'diagnostics.c: 16,67/33,33-ms-Framebudget, Teilzeiten, 32-MiB-Low-End-Speicherbudget, Peak/Cache/Pool, adaptive und feste Qualitätsprofile sowie Safe-Fallback; hardwarebasierte GPU/High-End-Erkennung fehlt' }
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
