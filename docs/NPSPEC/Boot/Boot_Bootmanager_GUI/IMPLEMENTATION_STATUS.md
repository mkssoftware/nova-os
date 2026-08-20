# Bootmanager-GUI – Implementierungsstatus

Stand: 2026-08-20  
Geltungsbereich: UEFI-Bootmanager (`boot/bootloader/uefi` und `boot/bootloader/bootmenu`)

Dieses Dokument ist ein Implementierungs-Audit, kein Ersatz für die einzelnen
NPSPECs. „Sicher gesperrt“ bedeutet, dass die Oberfläche den Vorgang erklärt,
aber keine nicht vorhandene oder nicht verifizierte Schreib-/Netzwerkoperation
vortäuscht.

| NPSPEC | GUI-Status | Nachweis / Einschränkung |
|---|---|---|
| UI-0001 Hauptauswahl | umgesetzt | Start, Installation, Einstellungen, Diagnose, Recovery und Energieaktionen; Tastatur, Zeiger, Countdown und Tooltips integriert. |
| UI-0002 Eintragsdetails | teilweise umgesetzt | Kontextmenü öffnet einen Dialog mit Name, Version, Kernel-Format, Architektur, EFI-Partition, Bootmodus, Standardstatus und Bereitschaft. Dynamische Datenträger-Metadaten fehlen, solange keine Boot-Entry-Datenquelle existiert. |
| UI-0003 Erweiterte Startoptionen | sicher gesperrt | Dialogpfad vorhanden; es gibt noch keine verifizierte Quelle für persistierbare Kernelparameter. |
| UI-0004 Recovery-Menü | teilweise umgesetzt | Auswahl, Navigation, Integritäts- und Diagnosehinweise vorhanden. Schreibende Reparatur bleibt ohne geprüftes NovaFS-/Recovery-Backend gesperrt. |
| UI-0005 Self-Healing | sicher gesperrt | Keine geprüfte Transaktions- oder Reparatur-Engine verfügbar. |
| UI-0006 Snapshot-Auswahl | sicher gesperrt | Keine Snapshot-Auflistung und kein signiertes Restore-Backend verfügbar. |
| UI-0007 Speichertest | teilweise umgesetzt | Nicht destruktiver UI-/Speicher-Selbsttest und Diagnosepfad vorhanden; vollständiger physischer RAM-Test benötigt ein separates Testmodul. |
| UI-0008 Datenträger/Partitionen | teilweise umgesetzt | Lesende Diagnose steht bereit; keine schreibenden Partitionsoperationen ohne autorisiertes Datenträger-Backend. |
| UI-0009 Entsperren | sicher gesperrt | Sichere Passworteingabe als Dialog-Control vorhanden; Schlüsselableitung, Entschlüsselung und Secret-Provider fehlen. |
| UI-0010 Netzwerk-Boot | sicher gesperrt | Kein Netzwerk-Stack, DHCP-/PXE-/HTTP-Transport und keine Vertrauenskette im Bootmanager integriert. |
| UI-0011 Firmware-Einstellungen | umgesetzt (UEFI) | Hersteller-, Secure-Boot- und Setup-Status werden lesend angezeigt; Neustart ins Setup wird nur bei Firmware-Unterstützung angeboten. |
| UI-0012 Boot-Fortschritt | teilweise umgesetzt | Deterministischer Fortschrittsdialog und Aktivitätsanzeige vorhanden; echte Phasenwerte benötigen den Loader-/Kernel-Handoff. |
| UI-0013 Startfehler | umgesetzt | Einheitliche Fehler-/Warnungsdialoge, Recovery-Fallback und Textmodus-Fallback vorhanden. |
| UI-0014 Diagnose | umgesetzt | Firmware-, Hardware-, Boot- und Speicheransichten, Fortschrittsanzeige und NDF-Exportpuffer vorhanden. |
| UI-0015 Ausschalten/Neustarten | umgesetzt (UEFI) | Bestätigungsdialoge und UEFI-Runtime-Reset/Shutdown integriert. Nicht unterstützte Ziele werden verständlich abgewiesen. |
| UI-0016 Auto-Countdown | umgesetzt | Fünf Sekunden, sichtbarer Sekundentakt, jeder Tastendruck oder Zeiger-Ereignis bricht ab, danach startet der Standard-Eintrag. |
| UI-0017 Standardauswahl | teilweise umgesetzt | Eintrag 0 ist der sichere Standard. Persistente Auswahl erfordert einen verifizierten Konfigurationsspeicher. |
| UI-0018 Kontext-Hilfe | umgesetzt | F1-Hilfe, Suche, Tooltips, Kontextmenü und Breadcrumb-Navigation integriert. |

## Verbleibende technische Blocker

1. **Boot-Entry-Inventar:** Es fehlt ein verifizierter Dienst, der mehrere
   Einträge samt UUID, Partition, Version, Kernel und letztem Startstatus
   bereitstellt. Daher können UI-0002 und UI-0017 nicht dynamisch werden.
2. **Sichere persistente Speicherung:** Für Standardauswahl, erweiterte
   Optionen und Recovery-Schreibvorgänge fehlt ein atomarer, signierter
   Konfigurations-/NovaFS-Backendvertrag.
3. **Recovery und Snapshots:** Eine signaturgeprüfte Inventarisierung sowie
   ein transaktionales Restore-Backend fehlen. Die UI darf diese Operationen
   bis dahin nicht aktivieren.
4. **Netzwerk-Boot:** Ein abgesicherter Netzwerk-Transport mit
   Vertrauenskette ist nicht vorhanden.
5. **Entschlüsselung:** Ein Secret-Provider, KDF und freigegebener
   Entschlüsselungs-Handoff fehlen.
6. **Echte Bootfortschrittsdaten:** Loader und Kernel liefern noch keinen
   standardisierten Fortschrittskanal an die GUI.

## Letzte Laufzeitprüfung

Der QEMU-Kontexttest validiert die UEFI-Initialisierung, das Öffnen des
Kontextmenüs, den Eintragsdetails-Pfad und die Darstellung eines Dialog-Frames.
Er erwartet die Debugmarker `UEFI:CONTEXT-FRAME-READY`,
`UEFI:CONTEXT-DETAILS` und `UEFI:DIALOG-FRAME-READY`.
