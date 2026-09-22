# NovaOS – aktueller Entwicklungsstand

**Stand:** 17. September 2026  
**Projektpfad:** `C:\recoverboot\nova-os`  
**Aktueller Schwerpunkt:** UEFI-Bootpfad und Kernel-Handoff

Diese Datei fasst den bisher implementierten und getesteten Stand zusammen. Die
ADRs und NPSPECs unter `docs/` bleiben die normative Quelle. Diese Übersicht ist
kein Ersatz für die Spezifikationen, sondern dokumentiert deren bisherigen
Umsetzungsstand.

## 1. Erzeugte Startmedien

Die Namen der Entwicklungsabbilder bleiben dauerhaft unverändert:

| Artefakt | Zweck | Aktueller Stand |
|---|---|---|
| `build/nova-uefi.img` | GPT/FAT32-ESP für UEFI | Aktueller Entwicklungsstand |
| `build/nova-bios.img` | Raw-Image für Legacy-BIOS | Letzter BIOS-Entwicklungsstand |
| `build/kernel.nki` | Bevorzugter Nova-Kernelcontainer | Aktueller Kernel |
| `kernel/build/kernel.elf` | Direkt ladbares ELF32 | Aktueller Kernel |
| `kernel/build/kernel.bin` | Kernel-Rohabbild | Aktueller Kernel |

Derzeit wird bewusst keine ISO bei jedem Arbeitsschritt erzeugt. Das UEFI-Image
ist das primäre Testartefakt. Konvertierungsskripte für ISO-Abbilder liegen im
Projektstamm.

## 2. Architekturentscheidungen, die bereits berücksichtigt werden

- UEFI ist der bevorzugte Bootpfad; Legacy-BIOS bleibt als separater Fallback
  erhalten.
- BIOS und UEFI übergeben dem Kernel denselben TLV-basierten NBHP/BIB-Vertrag.
- NKI ist das bevorzugte Produktionsformat.
- Gültige ELF32- und ELF64-Dateien können vom UEFI-Loader direkt geladen
  werden.
- Kernel und Bootloader bleiben über den versionierten Handoff getrennt.
- Boot-UI-Koordinaten und Layoutdefinitionen verwenden DLU als logische Einheit.
- Der Kernel erhält genau einen Zeiger auf den zusammenhängenden BIB.
- Firmwareabhängige Daten werden vor dem Kernelstart normalisiert.
- Semantic Types ergänzen primitive Repräsentationstypen und werden nicht durch
  diese ersetzt.

## 3. Bootloader und Bootmanager

### Gemeinsame Boot-UI-Grundlage

Im Bootmanager existieren inzwischen Komponenten für:

- Seiten- und Navigationsmodell
- Scene Graph, Render Queue, Layer und Surfaces
- Dirty-Region-Verwaltung und partielles Neuzeichnen
- Present Scheduler und Compositor
- Software-Renderer und Framebuffer-Backend
- DLU-basiertes Layout
- abgerundete Geometrie, Vektorgeometrie und SVG-Rendering
- Clip-Masken und 2D-Transformationen
- Bilder, Effekte und Hintergrundunschärfe
- Theme- und Darstellungsverwaltung
- Unicode-Text, Umlaute und Font-Ressourcen
- Icons, Branding und NovaOS-Logo
- Tastatur-, Maus-, Pointer- und Fokusverarbeitung
- Dialoge, Bestätigungen, Warnungen und Tooltips
- Listen, Scrollbereiche, Fortschrittsanzeigen und Recovery-Kacheln
- Diagnose-, Konfigurations-, Runtime- und Zustandsmodule
- Boot- und UI-Diagnosemarken für automatisierte QEMU-Tests

Der Bootmanager besitzt einen sichtbaren Countdown. Ohne Eingabe wird nach fünf
Sekunden der erste Eintrag gestartet. Eine Eingabe beendet die automatische
Auswahl und entfernt den Countdown-Hinweis. Noch nicht ausführbare Funktionen
können ihren Hinweis im dafür vorgesehenen Statusbereich anzeigen.

### Gestaltung

Die Oberfläche wurde an die bereitgestellten NovaOS-Entwürfe angenähert:

- dunkles Nova-Farbschema
- blaue Akzentfarbe für normale Bootansichten
- rote beziehungsweise magentafarbene Akzente für Fehleransichten
- NovaOS-Schriftzug und Morschustier-Branding
- unterschiedliche Logoausrichtung für normale Ansichten und Fehlerseiten
- gerundete obere Akzentleiste, Panels, Auswahlflächen und Schaltflächen
- Bootmanager-Icons für Start, Installation, Einstellungen, Diagnose,
  Wiederherstellung und Ausschalten
- geglättete Schriftwiedergabe über die Font- und Software-Render-Pipeline
- einheitlicher Fehlerbildschirm mit verständlichem Erklärungstext,
  Fehlercode und Nova-Smiley

Die grafische Exaktheit muss bei späteren Designänderungen weiterhin anhand der
Referenzbilder und realer Screenshots geprüft werden.

## 4. UEFI-Bootpfad

Der UEFI-Pfad ist momentan der aktive Entwicklungsschwerpunkt.

### EFI-Anwendung und Startmedium

- `BOOTX64.EFI` wird als eigenständige UEFI-Anwendung gebaut.
- Das UEFI-Abbild enthält eine GPT und eine FAT32-EFI-Systempartition.
- Die Standardposition `EFI/BOOT/BOOTX64.EFI` wird verwendet.
- Das Image enthält den bevorzugten Kernel `NOVA.NKI` und zusätzlich
  `KERNEL.ELF` als direkten Fallback.
- Für QEMU wird ein zusammengesetztes EDK2/OVMF-Firmwareabbild unter
  `build/uefi/edk2-x86_64.fd` verwendet.

### Grafik und Eingabe

- GOP wird als UEFI-Grafikbackend verwendet.
- Framebuffer-Adresse, Auflösung, Pitch, Bittiefe und Pixelformat werden in den
  NBHP/BIB übernommen.
- Tastatur sowie relative und absolute Pointer-Protokolle sind angebunden.
- Die Boot-UI besitzt Software-Rendering und kann ohne Firmware-Zeichenroutinen
  weiterarbeiten.

### Kerneldateien und Formaterkennung

Der Loader arbeitet in folgender Reihenfolge:

1. `NOVA.NKI` suchen und als bevorzugtes Format validieren.
2. NKI-Header, Architektur, Flags, Größe und CRC32 prüfen.
3. Das enthaltene ELF32 zusätzlich vollständig validieren.
4. Wenn `NOVA.NKI` nicht existiert, `KERNEL.ELF` direkt laden.
5. Fehlt auch ELF32, `KERNEL64.ELF` als direkten ELF64-Kernel laden.
6. Ein vorhandenes, aber ungültiges NKI führt kontrolliert zum Fehler und wird
   nicht stillschweigend durch ELF umgangen.

Für ELF32 werden derzeit geprüft:

- ELF-Magic, Klasse, Little Endian und ELF-Version
- `ET_EXEC` und `EM_386`
- Header- und Program-Header-Größen
- Grenzen sämtlicher Program Header
- vorhandene `PT_LOAD`-Segmente
- `p_filesz <= p_memsz`
- Adressüberläufe und Dateigrenzen
- Alignment als Zweierpotenz und Adress-/Datei-Kongruenz
- Segmentüberlappungen
- W^X: gleichzeitig schreibbare und ausführbare Segmente werden abgewiesen
- BSS-Nullinitialisierung
- ausführbarer und innerhalb eines Segmentes liegender Einstiegspunkt
- GNU-Build-ID aus `PT_NOTE`
- Nova-Metadatenversion und minimale Loader-ABI
- erforderliche CPUID-Featurebits

NKI- und ELF-Build-ID müssen zusammenpassen. Der BIB kennzeichnet außerdem, ob
der geladene ELF-Kernel aus einem NKI-Container oder aus einer direkten
ELF-Datei stammt.

Für direktes ELF64 werden zusätzlich ELF-Klasse 64, `EM_X86_64`, 64-Bit-
Program-Header und ein vollständig unterhalb der aktuellen 4-GiB-BIB-Grenze
liegendes Ladelayout geprüft. Der Loader validiert auch hier GNU-Build-ID,
Nova-Requirements, W^X, Alignment, Segmentüberlappungen und Einstiegspunkt. Der
UEFI-x64-Loader bleibt nach `ExitBootServices()` im Long Mode und übergibt über
ein separates, nicht zurückkehrendes 64-Bit-Trampolin.

### Speicherkarte und `ExitBootServices()`

Die UEFI-Speicherübergabe wurde gehärtet:

- Die Firmware bestimmt zunächst die benötigte Größe der Memory Map.
- Der Puffer wird dynamisch mit zusätzlichem Deskriptor-Spielraum angelegt.
- Ein erneutes `EFI_BUFFER_TOO_SMALL` führt zu kontrollierter Vergrößerung.
- Deskriptorgröße, Längen und Adressüberläufe werden validiert.
- Die normalisierte Speicherkarte umfasst maximal 256 Einträge.
- Eine Überschreitung wird als Fehler gemeldet und nicht still gekürzt.
- Vor jedem `ExitBootServices()`-Versuch wird eine frische Memory Map gelesen.
- Der vollständige NBHP/BIB wird aus genau dieser aktuellen Karte neu erzeugt.
- Nach einem ungültigen Map-Key erfolgen maximal drei kontrollierte Versuche.
- Zwischen finalem `GetMemoryMap()` und `ExitBootServices()` findet keine
  weitere UEFI-Speicherallokation statt.

Vorhandene Diagnosemarken sind unter anderem:

```text
UEFI:NKI-VALIDATED
UEFI:ELF32-DIRECT-VALIDATED
UEFI:ELF64-DIRECT-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-RETRY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
```

### NBHP/BIB-Inhalt des UEFI-Loaders

Der aktuelle BIB ist versioniert, zusammenhängend, TLV-basiert und enthält:

- Firmwaretyp UEFI
- normalisierte Speicherkarte
- optionale GOP-/Framebufferdaten
- Kernel-Ladeadresse, Größe und Einstiegspunkt
- Security-Status
- CPUID-Hersteller und Featurebits
- frühen Entropie-Seed
- System-/Bootversuchsinformationen
- validierten ACPI-RSDP, wenn vorhanden
- Kernel-Build-ID und Kernelformat

Der Security-TLV verwendet keine festen Platzhalter mehr. Er unterscheidet:

- unbekannten Prüfzustand,
- strukturell validiertes direktes ELF,
- integritätsgeprüftes NKI mit CRC32 und passender innerer ELF-Build-ID,
- signaturgeprüft und policyautorisiert als reservierte höhere Zustände.

Da das aktuelle NKI-Format noch keinen kryptografischen Signaturcontainer
enthält, wird ausdrücklich **nicht** behauptet, der Kernel sei signaturgeprüft.
Die Diagnose meldet stattdessen `UEFI:KERNEL-SIGNATURE-NOT-PRESENT`.

Der Firmwarezustand wird unmittelbar vor der Kernelprüfung erneut aus den
UEFI-Variablen gelesen und als `Unknown`, `Disabled`, `Enabled` oder
`SetupMode` in den BIB übertragen. Zusätzliche Security-Flags unterscheiden
bekannten Firmwarezustand, gültige ELF-Build-ID, gültige NKI-CRC32 und eine
zukünftig vorhandene Signatur. UEFI Secure Boot bleibt damit zusätzliche
Evidenz und wird nicht mit NovaOS-Trust oder Ausführungsautorität gleichgesetzt.

Unbekannte optionale TLVs können vom Kernel anhand ihrer Länge übersprungen
werden. Unbekannte als erforderlich markierte TLVs führen zum kontrollierten
Bootabbruch.

### Kernelübergang

- Der normale NovaOS-Entwicklungskernel wird derzeit als x86-32-Kernel geladen.
- Direkte ELF64-Testkernel werden als x86-64 geladen und im Long Mode gestartet.
- Der UEFI-x64-Loader wechselt über einen eigenen Trampolinpfad in den vom
  Kernel erwarteten CPU-Zustand.
- Der Einstiegspunkt, der einzelne BIB-Zeiger und ein reservierter Kernelstack
  werden übergeben.
- Ist die bevorzugte feste Stackadresse nicht verfügbar, wird ein geeigneter
  Stack unterhalb der unterstützten Adressgrenze dynamisch reserviert.
- Nach erfolgreichem `ExitBootServices()` werden keine Boot Services mehr
  verwendet.

## 5. Legacy-BIOS-Pfad

Der vorhandene BIOS-Stand umfasst:

- 512-Byte Stage 1 mit Bootsignatur
- Stage 2 und Protected-Mode-Übergang
- VBE-Grafik mit kontrolliertem Textmodus-Fallback
- Bootmanager und Kernelstart
- NKI-Validierung und ELF-basierte Kernelladung
- TLV-basierten NBHP/BIB-Handoff
- Fehleransicht bei beschädigtem oder unvollständigem Kernel
- QEMU-Boot bis `NOVA_KERNEL_READY`

Seit der Festlegung des aktuellen Arbeitsschwerpunkts wurde das BIOS-Image nicht
weiter verändert oder erneut gebaut.

## 6. Kernel

Der aktuelle Kernel startet über BIOS und UEFI; die neuesten Arbeiten wurden
nur noch über UEFI getestet.

### Früher Kernelstart

- Kernel Entry und strukturierte Bootphasen
- Validierung von NBHP/BIB-Header, Version, Größe und Prüfsumme
- TLV-Parser mit Required-/Optional-Semantik
- Übernahme der Kernel-Build-ID
- ACPI-RSDP-Prüfung und MADT-Auswertung
- Erkennung der vorhandenen CPUs
- Framebuffer- und Kernelkontextübernahme

### Implementierte Kernelgrundlagen

Die Startdiagnose meldet aktuell funktionsfähige Grundlagen für:

- Logging mit Ring- und Reservepuffer
- Panic Reporter und reservierten Crash-Dump-Pfad
- Physical Memory Manager und Seitentest
- Heap und Schreibtest
- Object Manager
- Component Manager
- Paging und Speichertest
- IDT, PIC und PIT mit 100 Hz
- IPC mit begrenzter FIFO
- Service Manager
- Process Manager
- Security-/Capability-Grundlage
- CPU Manager mit BSP-Topologie und Per-CPU-Daten
- Module Loader mit Trust-, ABI- und W^X-Prüfungen
- Thread Manager
- Scheduler mit zwei Testthreads
- SMP-Grundlage, BSP-Barriere und lokaler TLB-Pfad
- Device Manager
- VFS, Mount-Namespace und Bootstrap-Root
- Netzwerkgrundlage für IPv4, IPv6, UDP, ICMP und TCP
- Power Manager und Shutdown-Pfad
- x86-32-Userspace, TSS und System-Call-ABI
- Shared Service Page

### Userspace-Selbsttests

Der Bootstrap-Userspace prüft unter anderem:

- `Process.QuerySelf` und `Process.OpenSelf`
- `Thread.QuerySelf` und `Thread.OpenSelf`
- typ- und rechtegeprüfte Handles
- atomaren IPC-Inline-Roundtrip
- VFS-Root und Lookup von `/`
- Schließen und Ungültigwerden alter Handles
- Power-Abfragen, Wake-Locks und Profile
- Ablehnung eines Shutdowns ohne Capability
- IPv4-/IPv6-Socketobjekte
- UDP-Loopback und begrenzte Warteschlangen
- TCP-Stream, Listener und Accept
- Capability-Prüfung für Raw Sockets
- Logging-Abfragen und atomar veröffentlichte Records
- Shared Service Page

### SMP-Stand

ACPI/MADT erkennt in den aktuellen QEMU-Tests vier CPUs. Aktiv läuft bislang nur
die Bootstrap-CPU. Die AP-Aktivierung und echte Ausführung auf mehreren CPUs ist
noch nicht abgeschlossen.

## 7. Semantic Types

Auf Grundlage der Semantic-ADRs wurde ein erster Kernelkern für semantische
Typisierung umgesetzt.

### Registry und Typidentität

- namespacefähige stabile Typnamen
- internierte Type Handles für schnelle Kernelpfade
- explizite Typversionen
- getrennte primitive Repräsentation und semantische Identität
- Boot-Control-Plane für Registrierung
- versiegelte Registry vor Userspace und Schedulerbetrieb
- Ablehnung nachträglicher oder mehrdeutiger Registrierung

Aktuell registrierte Demonstrationstypen:

```text
nova.kernel.ipc.inline-data
nova.kernel.ipc.diagnostic
```

Beide verwenden technisch `Bytes8`, gelten dadurch aber nicht automatisch als
semantisch gleich.

### Kompatibilität und Konvertierung

Die Kompatibilitätsprüfung unterscheidet:

- `Unknown`
- `Exact`
- `Subtype` (für spätere Spezifikation reserviert)
- `Trait` (für spätere Spezifikation reserviert)
- `Convertible`
- `Incompatible`

Konvertierungen werden explizit mit Source Type, Source Version, Target Type,
Target Version, Capability-ID und Verlustklasse registriert. Lossless und Lossy
werden unterschieden. Die Registry beschreibt nur zulässige Pfade; sie führt
keine stillschweigende Konvertierung aus.

### Typed Resources, IPC und Capabilities

- Object-Handles besitzen getrennte Semantic-Type-Sidecars.
- Typinformation ersetzt weder Objektidentität noch Rechteprüfung.
- IPC-Pakete tragen Type Handle und Version.
- Endpoint-Contract, User-Claim, Handle-Rechte und Payloadprüfung bleiben
  getrennte Schritte.
- Falscher Typ und falsche Version werden vor Queue-Mutation abgewiesen.
- Service Contracts besitzen deklarierte Input- und Outputtypen.
- Gleiche primitive Repräsentation reicht nicht für Providerkompatibilität.
- Eine registrierte Conversion Capability bleibt explizit erforderlich.

### Semantic Validation

Typkompatibilität und Wertgültigkeit werden getrennt behandelt:

1. Type Handle und Version prüfen.
2. typspezifische Validation Rule ausführen.
3. zusätzliche Capability-Contract-Regeln anwenden.
4. erst danach die Operation ausführen.

Implementiert sind:

- deklarierte Validator-ID pro Typ
- `NONEMPTY`-Regel für Inline-Daten
- Regel für einen vorhandenen Diagnosecode
- zusätzliches Längenmaximum aus dem Capability Contract
- Validierungszustände `Declared`, `Verified` und `Invalid`
- eigener System-Call-Status für Validierungsfehler
- strukturierter Fehler mit Regel, Erwartungswert und Istwert
- Garantie, dass Validierung den geprüften Wert nicht verändert

Ein Userspace-Test sendet absichtlich einen korrekt typisierten und korrekt
versionierten, aber leeren IPC-Wert. Dieser wird als Validierungsfehler getrennt
von einem Typfehler abgewiesen.

## 8. Buildsystem und Artefaktprüfung

Das Buildsystem prüft beziehungsweise erzeugt unter anderem:

- C-Layoutchecks für Bootprotokoll und Kernel-ABI
- exakte Größe und Signatur der BIOS Stage 1
- Größenlimit des BIOS Stage 2
- Kernelgrößenlimit
- ELF32-Container mit GNU-Build-ID und Nova-Requirements-Note
- NKI mit CRC32 und Build-ID
- separate `BACKUP.NKI`- und `RECOVERY.NKI`-Container im UEFI-Startmedium
- Übereinstimmung der NKI- und ELF-Build-ID
- GPT/FAT32-UEFI-Abbild
- EFI-Anwendung und zusammengesetzte EDK2-Firmware

Das Root- und Kernel-Makefile berücksichtigt jetzt den Kernel-Assemblerquelltext,
alle `arch/x86_64/*.inc`-Dateien und die gemeinsamen ABI-Includes als echte
Abhängigkeiten. Änderungen daran lösen dadurch sicher einen Neuaufbau von
`kernel.bin`, ELF, NKI, Recovery-NKI und UEFI-Image aus.

Wichtige Befehle:

```powershell
# Nur UEFI-Anwendung und aktuelles UEFI-Image bauen
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make uefi-image'

# Kernel bauen
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make kernel'

# BIOS-Image bauen – derzeit nicht der Arbeitsschwerpunkt
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make image'
```

## 9. Zuletzt erfolgreich geprüfter UEFI-Start

Der aktuelle UEFI-Stand wurde unter QEMU mit Q35, EDK2, 256 MiB RAM und vier
virtuellen CPUs geprüft. Der bevorzugte NKI-Pfad meldete:

```text
UEFI:NKI-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_KERNEL_READY
```

Zusätzlich wurde ein temporäres UEFI-Testabbild ohne NKI erzeugt. Der direkte
ELF32-Fallback meldete:

```text
UEFI:ELF32-DIRECT-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_KERNEL_READY
```

Ein weiteres temporäres Abbild enthielt ausschließlich `KERNEL64.ELF`. Der
direkte ELF64-Pfad meldete:

```text
UEFI:ELF64-DIRECT-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_ELF64_LONG_MODE_READY
```

Die UEFI-Kernellader-Fehlerpfade besitzen außerdem einen automatisierten
Negativtest (`make test-uefi-kernel-validation`). Er erzeugt ausschließlich
temporäre Abbilder und prüft:

- beschädigtes NKI bei gleichzeitig vorhandenem gültigem ELF32,
- ungültiges direktes ELF32,
- ungültiges direktes ELF64,
- kontrollierte Meldung von `UEFI:KERNEL-VALIDATION-ERROR`,
- ausbleibenden `UEFI:KERNEL-HANDOFF-READY`-Marker,
- keinen stillschweigenden ELF-Fallback bei vorhandenem, aber ungültigem NKI,
- Vorrang von `BACKUP.NKI` vor einem gleichzeitig vorhandenen Recovery-NKI,
- automatischen Wechsel auf ein separat validiertes `RECOVERY.NKI`, wenn der
  Hauptkernel beschädigt ist,
- Übertragung und Kernel-seitige Bestätigung des Recovery-Modus im
  NBHP/BIB-Boot-Options-TLV.

Die drei isolierten Negativfälle werden vor dem Kernel-Handoff abgewiesen. Ein
vierter Test startet bei beschädigtem Haupt-NKI vorrangig das unabhängig
adressierte Backup-NKI. Ein fünfter Test entfernt die Backupgeneration und
bestätigt den Wechsel auf Recovery bis `NOVA_KERNEL_READY`. Die temporären
Images und Logs werden anschließend automatisch entfernt.

Das temporäre Testabbild wurde danach entfernt. Im Buildordner bleiben die
festgelegten Image-Namen erhalten.

### Persistenter UEFI-Boot-Control-Zustand

Der UEFI-Pfad besitzt jetzt einen ersten persistenten A/B-Boot-Control-Kern:

- zwei abwechselnd beschriebene UEFI-Variablen (`NovaBootState0` und
  `NovaBootState1`),
- 64-Byte-Datensätze mit Formatversion, Sequenznummer und CRC32,
- Auswahl der neuesten noch gültigen Kopie beim Start,
- logischer Active-, Candidate- und Known-Good-Slot,
- policyfähiges Versuchslimit im Datensatz statt einer fest verdrahteten
  Entscheidung im Auswahlalgorithmus,
- sofortige Candidate-Deaktivierung bei einem eindeutig ungültigen Artefakt,
- automatischer Wechsel zur Known-Good-Generation nach Erreichen des
  Versuchslimits,
- erzwungener automatischer Recovery-Start, wenn vorhandene Bootmetadaten in
  beiden Kopien ungültig sind; die beschädigten Datensätze werden dabei nicht
  still mit einem normalen Standardzustand überschrieben,
- sicherer flüchtiger Betrieb, falls UEFI-Variablen nicht geschrieben werden
  können.

Die Initialisierung schreibt beide redundanten Kopien und liest jede Änderung
nach dem Schreiben zur Validierung zurück. Der isolierte Zustandsautomat prüft
beide Slotrichtungen, alle Versuchslimits von 1 bis 16, deterministische
Auswahl, Limit-Rollback, Artefaktfehler, abgelehnte Übergänge, CRC-Erkennung und
den Überlauf der Sequenznummer. Ein zusätzlicher QEMU-Test startet mit einer
beschreibbaren Firmwarekopie, beschädigt danach gezielt den neuesten
Boot-State-Datensatz und startet erneut. Der zweite Start verwirft die defekte
Kopie, stellt die ältere gültige Kopie wieder her und erreicht wie der erste den
vollständigen primären NKI-Handoff bis `NOVA_KERNEL_READY`. Vor einem dritten
Start werden beide Kopien beschädigt. Dieser Lauf muss `RECOVERY.NKI` auswählen,
den automatischen Recovery-Modus über NBHP/BIB an den Kernel übertragen und
ebenfalls `NOVA_KERNEL_READY` erreichen.

Noch nicht umgesetzt ist der sicherheitskritische Health-Commit: Kernel Entry
allein markiert einen Candidate ausdrücklich nicht als Known-Good. Dafür fehlen
im aktuellen Dokumentbestand noch die in den ADRs referenzierten detaillierten
Boot-State-, Attempt-, Known-Good- und Health-Provider-NPSPECs.

## 10. Noch offene oder nur teilweise umgesetzte Punkte

Die folgenden Bereiche sind noch nicht vollständig abgeschlossen:

- LZ4-, ZSTD- und GZIP-Dekompression für Kernelabbilder
- kryptografischer Kernelsignaturcontainer, Schlüssel-/Revocation-Policy und
  vollständige NovaOS-Trustentscheidung; der UEFI-Secure-Boot- und
  Integritätszustand wird bereits getrennt in den BIB übertragen
- autorisierte Candidate-Staging-Schnittstelle, eindeutige Generationen und
  capabilitygeschützter Health-Commit; der persistente Boot-Control-Kern mit
  redundanter Speicherung, Candidate, Known-Good, Versuchslimit und Rollback
  ist vorhanden, alle drei Container verwenden derzeit aber noch dasselbe
  Entwicklungskernelpayload
- echte Prozess-/Stromunterbrechung an jedem einzelnen UEFI-Schreibzeitpunkt;
  die CRC-beschädigte neueste Kopie und der Rückfall auf die ältere Kopie sind
  bereits in QEMU geprüft
- vollständige AP-Aktivierung und echter SMP-Betrieb
- vollständige Semantic Relationships, Subtypes und Traits
- mehrere kompatible Semantic Types pro Ressource
- Typed Files und persistente Semantic Metadata
- Semantic Discovery und Semantic Execution
- tatsächliche Ausführung registrierter Conversion Capabilities
- vollständiges NovaFS und persistenter Userspace
- Tests auf realer UEFI-Hardware
- erneute End-to-End-Prüfung des aktuellen Images in VirtualBox
- pixelgenauer visueller Vergleich aller Bootmanagerseiten mit sämtlichen
  Referenzbildern und Zielauflösungen

## 11. Empfohlene nächste Schritte

Für die weitere Arbeit am derzeit priorisierten UEFI-Pfad bietet sich diese
Reihenfolge an:

1. normativen Kernel-Signaturcontainer sowie Schlüssel- und Revocation-Policy
   spezifizieren beziehungsweise implementieren,
2. die fehlenden Boot-Control-/Health-NPSPECs ergänzen und darauf die
   autorisierte Candidate-Aktivierung sowie den Health-Commit aufbauen,
3. VirtualBox-UEFI mit dem aktuellen GPT/FAT32-Image erneut validieren,
4. danach die nächsten Kernel- und Semantic-Type-Abschnitte umsetzen.

## 12. Wichtige Quellbereiche

| Bereich | Pfad |
|---|---|
| Spezifikationen | `docs/` |
| BIOS-Bootloader | `boot/bootloader/` |
| UEFI-Anwendung | `boot/bootloader/uefi/` |
| Bootmanager | `boot/bootloader/bootmenu/` |
| Gemeinsames Bootprotokoll | `boot/include/` |
| Kernel Entry und Subsysteme | `kernel/arch/x86_64/entry32.asm` |
| Semantic-Type-Kern | `kernel/arch/x86_64/semantic32.inc` |
| Build- und Testskripte | `scripts/` und `tests/` |
| Erzeugte Artefakte | `build/` und `kernel/build/` |

## 13. Pflege dieser Datei

Diese Datei soll nach größeren abgeschlossenen Arbeitsschritten aktualisiert
werden. Neue Einträge müssen klar unterscheiden zwischen:

- **spezifiziert:** in ADR/NPSPEC beschrieben,
- **implementiert:** im Quellcode vorhanden,
- **automatisiert getestet:** durch einen reproduzierbaren Test bestätigt,
- **manuell geprüft:** visuell oder in einer VM kontrolliert,
- **offen:** noch nicht oder nur teilweise umgesetzt.

## 14. Native UI-Systemarchitektur

Der vollständige Dokumentbestand und insbesondere die 25 angenommenen
Spezifikationen in `docs/NPSPEC/sysarchitecture/001-UI` wurden am 22. September
2026 neu eingelesen. Unter `ui/` existiert nun ein eigenständiger, in C17
kompilierbarer UI-Architekturkern. Er implementiert die gemeinsamen Verträge
für Retained Mode, deklaratives Reconciliation, Scene Graph, einen logisch
getrennten Accessibility Tree, Semantic UI, Theme-/Color-Tokens, Damage
Tracking, Frame Scheduling, VRR, Display-, Surface-, Window- und Input-Routing
sowie capabilitybasierte Startmenü-, Ribbon- und Dashboard-Contributions.

Alle Layoutgrößen werden als DLU geführt. Identitäten verschiedener Domänen
sind als unterschiedliche C-Typen modelliert, damit beispielsweise eine
`WindowID` nicht versehentlich als `SurfaceID` verwendet wird. Feste Kapazitäten
und begrenzte Frame Queues vermeiden unbeschränkten Speicherverbrauch.

`make ui-architecture-runtime-check` baut die Runtime mit
`-Wall -Wextra -Werror` und prüft die zentralen positiven und negativen Pfade.
Der Test umfasst unter anderem Discovery ohne Autorisierung, capabilitygeprüfte
Accessibility-Actions, Session-Isolation, sicheren Capture-Abbau bei
Owner-Ausfall, fehlerhafte Surface-Damage-Daten, Direct Scanout,
GPU-/Software-Fallback, Provider-Ausfall, Display-Hot-Unplug und VRR.

Noch nicht als Hardwareintegration vorhanden sind reale GPU- und Displaytreiber,
prozessübergreifende Shared Buffer, persistente Desktop-/Startmenüdaten,
Suchindex, Privacy-Dienst und ausführender Capability Broker. Die Runtime stellt
hierfür die kontrollierten Providergrenzen bereit; bis zur Treiberanbindung
bleibt der Softwarepfad der definierte funktionale Fallback.
