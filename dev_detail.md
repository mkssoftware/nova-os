# NovaOS – technische Implementierungsdetails

**Stand:** 21. September 2026
**Projekt:** `C:\recoverboot\nova-os`  
**Ergänzt:** [ENTWICKLUNGSSTAND.md](ENTWICKLUNGSSTAND.md)

Diese Datei beschreibt die bisher umgesetzten technischen Bausteine und erklärt
die wichtigsten Begriffe. Die Dokumente unter `docs/` bleiben die verbindliche
Architektur- und Produktspezifikation. Hier steht, was davon bereits im
Quellcode vorhanden ist, wie es zusammenspielt und wie weit es getestet wurde.

## 1. Statusbegriffe

In dieser Datei werden folgende Statusangaben verwendet:

- **Umgesetzt:** Der entsprechende Quellcode ist vorhanden und lässt sich
  bauen.
- **Automatisiert getestet:** Ein reproduzierbarer Test prüft den Baustein.
- **Unter QEMU getestet:** Der Pfad wurde in einer virtuellen Maschine bis zum
  angegebenen Ergebnis ausgeführt.
- **Teilweise umgesetzt:** Eine tragfähige Grundlage ist vorhanden, aber die
  vollständige Spezifikation ist noch nicht erfüllt.
- **Reserviert:** ABI-Werte oder Strukturen existieren bereits, die zugehörige
  Funktion ist aber noch nicht produktiv implementiert.

## 2. Kurze Begriffserklärungen

### DLU – Design Layout Unit

DLU ist die logische Koordinateneinheit der NovaOS-Bootoberfläche. Positionen,
Abstände und Größen werden nicht direkt als feste Pixelwerte entworfen, sondern
zunächst in DLU beschrieben und anschließend passend zur Auflösung und
Skalierung in Pixel umgerechnet. Dadurch kann dasselbe Layout beispielsweise
bei 800×600, 1280×720 und 1920×1080 sinnvoll dargestellt werden.

### NBHP – Nova Boot Handoff Protocol

NBHP ist der feste Übergabevertrag zwischen Bootloader und Kernel. Er legt fest,
wie der Bootloader dem Kernel Informationen über Firmware, Speicher, Grafik,
CPU, Sicherheit und das geladene Kernelabbild übergibt.

### BIB – Boot Information Block

Der BIB ist der konkrete, zusammenhängende Speicherblock des NBHP. Der Kernel
erhält genau einen Zeiger auf diesen Block. Der BIB besitzt einen versionierten
Header und danach TLV-Einträge.

### TLV – Type, Length, Value

Ein TLV-Eintrag besteht aus Typ, Länge und Nutzdaten. Dadurch kann der Kernel
bekannte Einträge lesen und unbekannte optionale Einträge anhand ihrer Länge
überspringen. Unbekannte erforderliche Einträge führen kontrolliert zum
Bootabbruch.

### NKI – Nova Kernel Image

NKI ist das bevorzugte NovaOS-Kernelcontainerformat. Der aktuelle NKI-Header
enthält unter anderem Architektur, Einstiegspunkt, Ladeadresse, Payloadgröße,
Kompressionsart, CRC32 und Build-ID. Das heutige NKI enthält ein ELF32-Payload.

### ELF – Executable and Linkable Format

ELF ist ein standardisiertes Format für ausführbare Dateien. Der Loader liest
nicht einfach die gesamte Datei an eine feste Position, sondern validiert die
Program Header und lädt die beschriebenen `PT_LOAD`-Segmente an ihre Zielorte.
NovaOS unterstützt im UEFI-Pfad direktes ELF32 und ELF64.

### Build-ID

Die Build-ID ist eine reproduzierbar erzeugte Kennung eines konkreten
Kernelbuilds. Sie wird aus dem Kernelinhalt abgeleitet, in einer GNU-ELF-Note
gespeichert und über NKI sowie BIB bis zum Kernel weitergereicht.

### CRC32

CRC32 ist eine Prüfsumme zur Erkennung unbeabsichtigter oder einfacher
manipulativer Änderungen. Sie ist keine kryptografische Signatur. Im aktuellen
NKI schützt sie das vollständige ELF-Payload vor unbemerkten Bitänderungen.

### UEFI

UEFI ist die moderne Firmware-Schnittstelle, über die `BOOTX64.EFI` gestartet
wird. Der NovaOS-Loader nutzt UEFI unter anderem für Dateizugriff, GOP-Grafik,
Speicherallokation, Firmwarevariablen und `ExitBootServices()`.

### GOP – Graphics Output Protocol

GOP ist die UEFI-Grafikschnittstelle. Sie liefert verfügbare Grafikmodi sowie
Framebuffer-Adresse, Auflösung, Pitch und Pixelformat. Nach dem Firmwarehandoff
zeichnet NovaOS direkt in diesen Framebuffer.

### Framebuffer

Ein Framebuffer ist ein Speicherbereich, dessen Pixelinhalt unmittelbar auf dem
Bildschirm erscheint. Der Bootmanager besitzt einen Software-Renderer, der
Flächen, Text, Icons und Bilder in diesen Speicher zeichnet.

### Dirty Region

Eine Dirty Region ist ein Bildschirmbereich, der sich geändert hat. Statt bei
jeder Mausbewegung den gesamten Bildschirm neu zu zeichnen, kann der Compositor
nur die beschädigten Bereiche aktualisieren.

### Compositor

Der Compositor setzt einzelne Oberflächen und Ebenen zum fertigen Bild
zusammen. Er berücksichtigt Reihenfolge, Sichtbarkeit, Beschädigungen,
Clipping und die Ausgabeplanung.

### Scene Graph

Der Scene Graph beschreibt die hierarchische Struktur sichtbarer UI-Elemente.
Eltern können Position, Sichtbarkeit und Transformation ihrer Kinder
beeinflussen.

### Surface und Layer

Eine Surface ist eine Zeichenoberfläche. Ein Layer bestimmt, wie mehrere
Surfaces räumlich und in der Z-Reihenfolge zusammengesetzt werden.

### W^X – Write xor Execute

W^X bedeutet, dass ein Speichersegment nicht gleichzeitig schreibbar und
ausführbar sein darf. Der Kernel- und Modullader weist entsprechende
Segmentkombinationen zurück.

### ACPI und MADT

ACPI beschreibt Hardware- und Plattforminformationen. Der RSDP ist der Einstieg
in die ACPI-Tabellen. Die MADT beschreibt unter anderem die vorhandenen CPUs und
Interruptcontroller. Der Kernel erkennt damit in QEMU derzeit vier CPUs.

### BSP und AP

Die BSP ist die Bootstrap-CPU, auf der Firmware, Bootloader und Kernel beginnen.
Weitere CPUs heißen Application Processors oder APs. NovaOS erkennt die APs,
aktiviert sie aber noch nicht vollständig für parallele Kernelarbeit.

### IDT, PIC und PIT

- Die IDT ordnet CPU-Interrupts ihren Behandlungsroutinen zu.
- Der PIC verwaltet klassische Hardwareinterrupts.
- Der PIT erzeugt im aktuellen Kernel einen regelmäßigen 100-Hz-Zeitinterrupt.

### IPC

IPC bedeutet Inter-Process Communication. Der aktuelle Kernel besitzt eine
begrenzte FIFO für Nachrichten zwischen typisierten Endpunkten und prüft dabei
Handles, Rechte, Semantic Type, Version und Wertgültigkeit.

### Capability

Eine Capability ist eine explizite, einschränkbare Berechtigung für eine
Operation oder Ressource. Besitz oder Signatur allein erzeugt keine Capability.

### Semantic Type

Ein Semantic Type beschreibt nicht nur die technische Speicherung, sondern die
Bedeutung eines Werts. Zwei Werte können beide als acht Bytes gespeichert sein
und trotzdem semantisch inkompatibel sein.

### Semantic Validation

Semantic Validation prüft, ob ein korrekt typisierter konkreter Wert auch die
inhaltlichen Regeln erfüllt. Ein Wert kann den richtigen Typ besitzen und
trotzdem ungültig sein, beispielsweise wenn ein Pflichtinhalt leer ist.

### Secure Boot, Verified Boot und Measured Boot

- Secure Boot entscheidet, ob eine Komponente starten darf.
- Verified Boot prüft Integrität und gegebenenfalls Authentizität eines
  Bootartefakts.
- Measured Boot protokolliert kryptografisch, was tatsächlich gestartet wurde.

Diese Zustände bleiben getrennt. Eine gültige Signatur bedeutet nicht
automatisch vollständiges Vertrauen oder Ausführungsberechtigung.

## 3. Projektartefakte

### Festgelegte Dateinamen

Im Buildverzeichnis werden derzeit nur diese beiden startbaren Hauptimages
weitergeführt:

| Datei | Inhalt |
|---|---|
| `build/nova-uefi.img` | GPT-Datenträger mit FAT32-ESP, UEFI-App und Kerneldateien |
| `build/nova-bios.img` | Raw-Datenträger mit BIOS Stage 1, Stage 2 und Kernel |

Weitere wichtige Artefakte:

| Datei | Inhalt |
|---|---|
| `build/uefi/EFI/BOOT/BOOTX64.EFI` | NovaOS-UEFI-Anwendung |
| `build/uefi/edk2-x86_64.fd` | zusammengesetzte EDK2-Testfirmware |
| `build/kernel.nki` | bevorzugter NKI-Kernelcontainer |
| `build/backup.nki` | separat adressierbare Known-Good-/Backupgeneration |
| `build/recovery.nki` | separat adressierbarer UEFI-Recovery-Kernelcontainer |
| `kernel/build/kernel.elf` | direkt ladbarer ELF32-Kernel |
| `kernel/build/kernel.bin` | Kernel-Rohpayload |
| `kernel/build/kernel64-test.elf` | kleiner ELF64-Handoff-Testkernel |

## 4. Buildsystem

### Umgesetzte Prüfungen

- C-Layoutprüfung für das gemeinsame Bootprotokoll
- C-Layoutprüfung für die Kernel-ABI
- BIOS Stage 1 muss exakt 512 Byte groß sein
- BIOS Stage 1 muss die Signatur `0x55AA` besitzen
- Größenprüfung für Stage 2 und Kernel
- Erzeugung eines ELF32 mit zwei Program Headern
- GNU-Build-ID als ELF-Note
- Nova-Requirements-Note mit Loader-ABI und CPUID-Anforderungen
- Erzeugung und Validierung des NKI-Headers
- CRC32 über das vollständige NKI-Payload
- Abgleich zwischen NKI-Build-ID und ELF-Build-ID
- Erzeugung eines ELF64-Testabbilds mit denselben Metadatenregeln
- Erzeugung separater Backup- und Recovery-NKI-Container
- Erzeugung der UEFI-Anwendung
- Erzeugung einer GPT mit FAT32-EFI-Systempartition
- Erzeugung der fest benannten BIOS- und UEFI-Images

### Abhängigkeitskorrektur im Kernel

Der Kernel-Assemblerquelltext, alle Dateien unter `kernel/arch/x86_64/*.inc`
und die gemeinsamen Boot-ABI-Includes sind jetzt Make-Abhängigkeiten des
Kernelbinaries. Änderungen daran werden dadurch nicht mehr versehentlich von
einem alten `kernel.bin`, ELF oder NKI verdeckt.

## 5. UEFI-Startmedium

**Status:** umgesetzt und unter QEMU getestet.

Das Skript `scripts/build-uefi-image.ps1` erzeugt:

- Protective MBR
- primären und gesicherten GPT-Header
- EFI-Systempartition
- FAT32-Bootsektor und FAT-Ketten
- `EFI/BOOT/BOOTX64.EFI`
- optional `NOVA.NKI`
- optional `KERNEL.ELF`
- optional `KERNEL64.ELF`
- optional `BACKUP.NKI`
- optional `RECOVERY.NKI`

Das Schreiben des fertigen Images verwendet eine temporäre Datei und kurze
begrenzte Wiederholungen beim Ersetzen. Dadurch scheitert ein Build nicht mehr
sofort, wenn ein Windows-Dateiscanner das bisherige Image kurzzeitig geöffnet
hält.

Die Firmware findet dadurch den standardisierten UEFI-Fallbackpfad ohne einen
vorher angelegten NVRAM-Booteintrag.

## 6. UEFI-Anwendung

**Status:** umgesetzt und unter QEMU/EDK2 getestet.

Die Anwendung initialisiert:

- UEFI System Table und Boot Services
- GOP-Grafik
- gemeinsame Grafikabstraktionsschicht
- Boot-UI-Speicherverwaltung
- Ressourcenmanager
- Eingabesystem
- Bootmanagerseiten
- Diagnose- und Testframework
- Firmwarestatus
- Pointer-Protokolle
- Power- und Firmware-Setup-Funktionen

Die Anwendung läuft bis zur bewussten Kernelübergabe vollständig innerhalb der
UEFI-Umgebung.

## 7. UEFI-Grafik

### GOP-Modus

Der Loader fragt GOP-Modi ab, validiert Auflösung, Pitch, Pixelformat und
Framebuffergröße und wählt einen verwendbaren Modus. Die tatsächlichen
Grafikdaten werden später als Graphics-TLV an den Kernel übergeben.

### Software-Rendering

Die Oberfläche wird von NovaOS selbst gezeichnet. Die Firmware muss keine
Schriften, Icons oder Fenster rendern. Dadurch bleibt die visuelle Ausgabe über
unterschiedliche UEFI-Implementierungen hinweg kontrollierbar.

### Renderpipeline

Umgesetzt sind:

- Framebuffer-Backend
- Compositor
- Present Scheduler
- Dirty Manager
- Scene Graph
- Render Queue
- Surface Manager
- Layer Manager
- Clip Masks
- 2D-Transformationen
- abgerundete Geometrie
- Vektorgeometrie
- SVG-Renderer
- Bildrenderer
- Schatten und Glow
- Hintergrundunschärfe
- adaptive Renderqualität
- Fallbackprofile für langsamere Systeme

### Teilflächenaktualisierung

Interaktionen markieren alte und neue Bereiche als beschädigt. Dadurch können
Auswahlmarker, Tooltips und Pointer aktualisiert werden, ohne dauerhaft den
kompletten Bildschirm neu zu zeichnen.

## 8. Bootmanager-Design

### NovaOS-Branding

Umgesetzt sind:

- NovaOS-Schriftzug
- blaues Morschustier-Logo für normale Boot- und Kernelansichten
- angepasstes Fehlerbranding
- obere farbige Akzentleiste
- dunkles Grundtheme
- Light Theme
- High-Contrast Theme
- semantische Designfarben für Information, Erfolg, Warnung und Fehler

### Bootmenü

Vorhandene Hauptpunkte:

- NovaOS starten
- NovaOS installieren
- Einstellungen
- Diagnose
- Recovery
- Ausschalten

Der erste Eintrag ist standardmäßig ausgewählt. Die Auswahlfläche, Icons und
Texte werden über das gemeinsame Controls- und Designsystem gezeichnet.

### Countdown

Der Bootmanager zählt sichtbar von fünf Sekunden herunter. Ohne Eingabe startet
der erste Eintrag. Eine beliebige Eingabe beendet die automatische Auswahl und
entfernt den Countdowntext. Hinweise nicht implementierter Funktionen erscheinen
im Statusbereich anstelle des Countdowns.

### DLU-Layout

Bootmanager, Panels, Texte, Icons, Abstände und Radien werden aus logischen DLU
in Pixel überführt. Safe Display Area, Skalierungsfaktor und Zielauflösung
werden berücksichtigt.

### Rundungen und Alpha

Umgesetzt sind gerundete Panels, Leisten und Schaltflächen. Icons wie das
Ausschaltsymbol werden als geglättete Vektorformen mit Alpha-Blending
gezeichnet, statt nur aus eckigen Bitmapblöcken zu bestehen.

### Text

Der Textpfad unterstützt:

- Font-Ressourcen
- geglättete Glyphen
- UTF-8-Verarbeitung
- Unicode-Zeichen
- deutsche Groß- und Kleinschreibung
- Umlaute und `ß`
- Labels, Statusmeldungen und mehrzeilige Texte

## 9. Eingabesystem

### Tastatur

UEFI-Tastatureingaben werden in gemeinsame Nova-Ereignisse übersetzt. Umgesetzt
sind Fokusnavigation, Zeichenereignisse, Richtungstasten, Enter, Escape,
Tastaturkürzel und Wiederholungslogik.

### Maus und Pointer

Unterstützt werden:

- UEFI Simple Pointer Protocol
- UEFI Absolute Pointer Protocol
- relative und absolute Koordinaten
- Pointergeschwindigkeit
- Hit Testing
- Hoverzustände
- Klick und Doppelklick
- Pointer Capture
- kontrollierter Capture-Abbruch bei Dialog-, Recovery- und Kontextwechsel
- Geräteerkennung und Diagnoseereignisse

### Fokus

Der Fokusmanager kennt Bereiche, Richtungssuche, Wiederherstellung und
hierarchisches Event-Routing. Dialoge besitzen einen eigenen Fokusbereich.

## 10. Bootmanager-Seiten und Controls

Vorhandene UI-Bausteine umfassen:

- Buttons
- Labels
- Icons
- Bildcontrols
- Listen
- Scroll Views
- Cards und Tiles
- Badges
- Textfelder
- Passwortfelder
- Switches
- Slider beziehungsweise einstellbare Werte
- Fortschrittsanzeigen
- Dialoge
- Bestätigungen
- Warnungen
- Tooltips
- Breadcrumbs
- Kontextansichten

Vorhandene Seiten beziehungsweise Ansichten umfassen Bootmenü, Einstellungen,
Diagnose, Recovery, Firmwareinformationen, Hilfe und mehrere isolierte
Testansichten.

## 11. Boot-UI-Ressourcen

### Ressourcenmanager

Ressourcen werden über stabile IDs registriert und vor der Nutzung validiert.
Es existieren Registry-, Versionierungs-, Cache- und Preload-Grundlagen.

### Fonts, Icons und Bilder

- Fontressourcen werden in generierte C-Daten überführt.
- Icons besitzen semantische IDs und Vektorpfade.
- NovaOS-Branding wird als gemeinsame Ressource eingebunden.
- Der Bildrenderer prüft PNG-Signatur, Chunkgrenzen und Prüfsummen.
- SVG wird über die Vektorpipeline verarbeitet.

### Ressourcenintegrität

Umgesetzt sind Größen-, Versions-, Format- und Prüfsummenprüfungen. Die
Korruptionstests erzeugen manipulierte Kopien, ohne die Produktivressource zu
verändern.

## 12. Boot-UI-Diagnose

Die UEFI-Anwendung schreibt strukturierte Diagnosemarken über den Debug-Port.
Sie decken unter anderem ab:

- Firmwareentry
- GOP und Framebuffer
- Layout und Skalierung
- Ressourcen
- Eingabe
- Pointer
- Dialoge
- Navigation
- Compositor
- Teilflächenaktualisierung
- Countdown
- Recovery
- Performancebudget
- Kernelvalidierung
- NBHP/BIB-Erzeugung
- `ExitBootServices()`
- Kernelhandoff

Damit können Tests ohne Bildschirmerkennung feststellen, welche Bootphase
erreicht wurde.

## 13. UEFI-Firmwarestatus

Die Firmwareintegration liest:

- Firmwarehersteller
- Firmwarerevision
- UEFI-Variable `SecureBoot`
- UEFI-Variable `SetupMode`
- `OsIndicationsSupported`

Wenn unterstützt, kann NovaOS einen kontrollierten Neustart in das
Firmware-Setup anfordern.

Der Secure-Boot-Zustand wird unterschieden als:

- `Unknown`
- `Disabled`
- `Enabled`
- `SetupMode`

Ein nicht lesbarer Zustand wird nicht fälschlich als „ausgeschaltet“ gemeldet.

## 14. UEFI-Kernellader

### Auswahlreihenfolge

Der Loader sucht in dieser Reihenfolge:

1. `NOVA.NKI`
2. `KERNEL.ELF`
3. `KERNEL64.ELF`

Kann der gewählte Hauptkernel nicht gelesen oder validiert werden, wird ein
separates `BACKUP.NKI` und danach `RECOVERY.NKI` gesucht. Ein beschädigtes NKI
führt weiterhin niemals zu einem stillen Wechsel auf das normale ELF. Backup
und Recovery sind ausdrückliche, erneut vollständig validierte NKI-Pfade.

Die umgesetzte Reihenfolge lautet:

```text
PRIMARY -> BACKUP -> RECOVERY -> kontrollierter Abbruch
```

Der Recovery-Kernel kann außerdem manuell über die erste Kachel der
Recovery-Seite oder im UEFI-Textfallback mit `R` gestartet werden.

NKI bleibt damit das bevorzugte Produktionsformat. Ein vorhandenes, aber
ungültiges NKI wird nicht durch einen unsichereren stillen Fallback umgangen.

### NKI-Validierung

Geprüft werden:

- Magic `NOVANKI\0`
- NKI-Version
- Headergröße
- Zielarchitektur
- Pflichtflags
- reservierte Felder
- Kompressionsmodus
- Payloadgrenzen
- CRC32
- enthaltenes ELF
- Übereinstimmung von NKI- und ELF-Build-ID
- Einstiegspunkt und Ladeadresse

### ELF32-Validierung

Geprüft werden:

- ELF-Magic, Klasse, Endianness und Version
- `ET_EXEC`
- `EM_386`
- ELF- und Program-Header-Größe
- Anzahl und Grenzen der Program Header
- mindestens ein `PT_LOAD`
- Datei- und Speichergrößen
- BSS-Nullinitialisierung
- Adressüberläufe
- Segmentüberlappungen
- Alignment
- W^X
- ausführbarer Einstiegspunkt
- GNU-Build-ID
- Nova-Requirements-Note
- minimale Loader-ABI
- erforderliche CPUID-Bits

### ELF64-Validierung

ELF64 verwendet dieselben Sicherheitsregeln, ergänzt um:

- `ELFCLASS64`
- `EM_X86_64`
- 64-Bit-Program-Header
- 64-Bit-Datei- und Speichergrenzen
- gegenwärtig vollständiges Ladelayout unterhalb von 4 GiB, da BIB v1 für
  Kerneladressen 32-Bit-Felder verwendet

### Kernelstack

Zunächst wird die bevorzugte feste Stackregion reserviert. Ist sie belegt, sucht
der Loader eine alternative Region unterhalb der unterstützten Adressgrenze und
meldet `UEFI:KERNEL-STACK-RELOCATED`.

## 15. UEFI Memory Map

### Dynamische Größenbestimmung

Der Loader fragt die tatsächlich benötigte Größe bei der Firmware ab und
reserviert einen Puffer mit zusätzlichem Spielraum. `EFI_BUFFER_TOO_SMALL` wird
durch kontrollierte Vergrößerung behandelt.

### Normalisierung

UEFI-Speicherdeskriptoren werden in gemeinsame 24-Byte-Nova-Speichereinträge
übersetzt. Konventioneller Speicher wird als verfügbar markiert, alle anderen
Bereiche zunächst als reserviert.

### Sicherheitsprüfungen

- Deskriptorgröße
- Mapgröße
- Seitenzahl
- Multiplikationsüberlauf
- Adressüberlauf
- maximale Eintragszahl
- kein stilles Abschneiden

## 16. `ExitBootServices()`

`ExitBootServices()` beendet die UEFI-Bootdienste. Danach darf der Loader keine
Boot-Service-Funktionen mehr aufrufen.

Der NovaOS-Pfad:

1. liest eine frische Memory Map,
2. baut daraus den BIB,
3. ruft `ExitBootServices()` mit dem zugehörigen Map-Key auf,
4. liest bei einem Fehler eine neue Map,
5. baut den BIB neu,
6. versucht die Übergabe höchstens dreimal.

Zwischen finalem `GetMemoryMap()` und `ExitBootServices()` wird keine weitere
UEFI-Speicherallokation ausgeführt.

## 17. UEFI-Kernelübergang

### 32-Bit-Kernel

Der normale Entwicklungskernel ist derzeit x86-32. Ein eigenes Trampolin
wechselt aus der UEFI-x64-Umgebung kontrolliert in den erwarteten
Protected-Mode-Zustand, setzt Stack, Bootmagic und BIB-Zeiger und springt zum
Kernelentry.

### 64-Bit-Kernel

Für ELF64 bleibt der Loader im Long Mode. Ein separates 64-Bit-Trampolin setzt
Stack, Bootmagic und BIB-Zeiger und springt ohne Rückkehr zum 64-Bit-Entry.

Der Testkernel bestätigt dies mit:

```text
NOVA_ELF64_LONG_MODE_READY
```

## 18. NBHP/BIB-Header

Der Header enthält:

- Magic `NBHPBIB\0`
- Major-, Minor- und Patchversion
- Headergröße
- Gesamtgröße
- CRC32 über den vollständigen BIB
- Zielarchitektur
- globale Flags

Der BIB wird vor jedem `ExitBootServices()`-Versuch neu erzeugt, damit seine
Speicherkarte exakt zum verwendeten UEFI-Map-Key passt.

## 19. NBHP/BIB-TLVs

### Firmware-TLV

Kennzeichnet den Firmwarepfad als UEFI. Der Kernel muss dadurch keine direkten
UEFI-Funktionen verwenden.

### Memory-TLV

Enthält Adresse, Anzahl und Größe der normalisierten Speicherkarteneinträge.

### Graphics-TLV

Enthält Framebufferadresse, Pitch, Breite, Höhe, Bittiefe und Pixelformat.

### Kernel-TLV

Enthält Ladeadresse, Imagegröße und Einstiegspunkt des geladenen Kernels.

### Security-TLV

Enthält getrennte Felder für:

- Kernelprüfzustand
- Firmware-Secure-Boot-Zustand
- Entropiequalität
- Security-Evidenzflags

Direktes ELF wird als strukturell validiert markiert. NKI wird nach erfolgreicher
CRC32- und Build-ID-Prüfung als integritätsgeprüft markiert. Ohne echten
Signaturcontainer wird niemals „Signatur geprüft“ behauptet.

### Boot-Options-TLV

Enthält den Bootmodus, Modusflags, die ausgewählte Generation und die
Fallbackstufe. Der aktuelle UEFI-Pfad unterscheidet Primärstart, automatisches
Rollback auf Backup, automatisches Recovery und manuell gewähltes Recovery.
Der Kernel validiert Modus und Generation, kopiert beide in seinen internen
Kontext und meldet Backup- oder Recoverybetrieb sichtbar im seriellen
Startprotokoll.

### CPU-TLV

Enthält CPU-Herstellerkennung, höchsten CPUID-Basisleaf und wichtige
Featurebits.

### Entropy-TLV

Enthält einen frühen Seed aus Zeitstempelzähler und zusätzlicher Mischung. Das
ist eine frühe Bootentropie-Grundlage, noch kein vollständiger kryptografischer
Random-Service.

### System-TLV

Enthält Systemgeneration, Bootversuch und Flags für spätere Lifecycle- und
Rollbackfunktionen.

### ACPI-TLV

Enthält die validierte physische Adresse des RSDP, wenn eine gültige ACPI-Tabelle
gefunden wurde.

### Kernel-Identity-TLV

Enthält die 20-Byte-Build-ID, ELF32-/ELF64-Format und die Information, ob das
ELF aus einem NKI-Container oder direkt geladen wurde.

## 20. UEFI-Kernelvertrauen

Die aktuelle Implementierung trennt:

- strukturelle ELF-Validierung,
- NKI-Integritätsprüfung,
- Firmware Secure Boot,
- kryptografische Signaturprüfung,
- spätere Trust- und Policyentscheidung.

Aktuell umgesetzt:

- ELF-Strukturprüfung
- ELF-Build-ID
- NKI-CRC32
- Abgleich von innerer und äußerer Build-ID
- Abfrage des Firmware-Secure-Boot-Zustands
- Übertragung dieser Evidenz im Security-TLV

Noch nicht umgesetzt:

- normativer NKI-Signaturcontainer
- Public-Key- und Trust-Anchor-Verwaltung
- Signaturprüfung
- Revocation
- vollständige Bootpolicy
- TPM-basiertes Measured Boot

## 21. Automatisierte UEFI-Kernelladertests

Der Befehl

```powershell
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make test-uefi-kernel-validation'
```

prüft:

- beschädigtes NKI bei gleichzeitig vorhandenem gültigem ELF32
- ungültiges direktes ELF32
- ungültiges direktes ELF64
- kontrollierten Validierungsfehler
- ausbleibenden Kernelhandoff
- keinen stillen Sicherheitsfallback
- erfolgreichen automatischen Wechsel von einem beschädigten Haupt-NKI auf
  `BACKUP.NKI`, wobei ein gleichzeitig vorhandenes Recovery-NKI nicht vorzeitig
  ausgewählt werden darf
- erfolgreichen automatischen Wechsel von einem beschädigten Haupt-NKI auf
  `RECOVERY.NKI`
- Backup-/Recovery-Kennung im NBHP/BIB und deren Kernel-seitige Auswertung

Die Testimages werden in einem eindeutig benannten temporären Unterordner von
`build/` erstellt und anschließend vollständig entfernt.

## 22. BIOS-Bootpfad

**Status:** umgesetzt, derzeit nicht der aktive Entwicklungsschwerpunkt.

Vorhanden sind:

- Stage 1 mit 512 Byte und Bootsignatur
- Stage 2
- Protected-Mode-Übergang
- E820-Speichererkennung
- VBE-Grafik
- kontrollierter Textmodus-Fallback
- Bootmanager
- NKI- und ELF-Ladegrundlage
- NBHP/BIB
- Kernelhandoff
- Fehlerpfad für beschädigte Kerneldateien

Das BIOS-Image wurde während der jüngsten reinen UEFI-Arbeiten nicht neu gebaut.

## 23. Kernelentry und Bootvalidierung

Der x86-32-Kernel prüft:

- Bootmagic
- BIB-Adresse
- BIB-Magic
- BIB-Version
- Header- und Gesamtgröße
- BIB-Prüfsumme
- Zielarchitektur
- TLV-Grenzen und Alignment
- erforderliche TLVs
- Speicherkarte
- Kerneladresse und Entry
- Kernel-Build-ID
- optionale Grafikdaten
- ACPI-Zeiger

Unbekannte optionale TLVs werden übersprungen. Unbekannte Required-TLVs werden
abgelehnt.

## 24. Kernel-Bootphasen

Der Kernel protokolliert nummerierte Phasen und die jeweils letzte erfolgreiche
Phase. Dadurch kann ein Panic-Bericht erkennen, in welchem Abschnitt der Start
abgebrochen wurde.

Umgesetzt sind Phasen für:

- früher Entry
- NBHP/BIB
- Speichermanager
- Kernobjekte
- Interrupts und Zeit
- Kernelservices
- Scheduler
- Geräte
- Dateisystem und Netzwerk
- Power
- Userspace

## 25. Logging, Panic und Crash Dump

### Logging

Der Kernel besitzt einen strukturierten Ringpuffer und einen reservierten
Fallbackpuffer. Records werden atomar veröffentlicht, damit ein Leser keinen
halb geschriebenen Eintrag erhält.

### Panic Reporter

Ein Panic erzeugt einen klaren Fehlerpfad, serielle Diagnose und die grafische
NovaOS-Fehleransicht. Die Fehleransicht verwendet verständliche Texte,
Fehlercode, Nova-Farben, Logo und Smiley.

### Crash Dump

Ein reservierter Minimal-Dump-Bereich steht bereits während des frühen Boots
bereit. Er ist unabhängig von später initialisierten Dateisystemdiensten.

## 26. Physical Memory Manager

Der PMM verwaltet physische Seiten auf Grundlage der normalisierten
Bootspeicherkarte. Reservierte Bereiche wie Kernel, BIB, Loaderdaten und
Framebuffer werden nicht als frei behandelt. Ein Seitentest prüft grundlegende
Allokation und Freigabe.

## 27. Heap

Der Kernelheap besitzt eine initiale Allokationsgrundlage und einen
Schreib-/Lesetest. Er wird nach dem PMM initialisiert und steht höheren
Kernelmanagern zur Verfügung.

## 28. Object Manager

Kernelressourcen erhalten typisierte Objekteinträge mit:

- Objekttyp
- Handle beziehungsweise Generation
- Status
- Besitzer-/Nutzdaten
- Semantic-Type-Sidecar

Freigabe invalidiert das Objekt und entfernt auch seine semantischen
Zusatzinformationen.

## 29. Handle Manager

Handles trennen Nutzerreferenzen von Kernelobjekten. Bei der Auflösung werden
geprüft:

- Prozessbesitz
- erwarteter Objekttyp
- Generation
- benötigte Rechte
- Gültigkeitszustand

Ein geschlossenes Handle kann nicht weiterverwendet werden.

## 30. Component Manager

Der Component Manager stellt eine frühe Grundlage für registrierbare
Kernelkomponenten, Lebenszyklus und Abhängigkeiten bereit.

## 31. Paging

Der Paging-Code besitzt eine initiale Seitentabellen- und Speichertestgrundlage.
Kernel- und Userspacebereiche werden getrennt. Der Kernel meldet die erfolgreiche
Initialisierung vor dem Aktivieren höherer Dienste.

## 32. Interrupts und Timer

Umgesetzt sind:

- IDT
- Ausnahme- und Interruptstubs
- PIC-Grundkonfiguration
- PIT mit 100 Hz
- Timer-Selbsttest
- Aktivierung von Interrupts erst nach erfolgreicher Initialisierung

## 33. IPC

### Paketformat

Das IPC-Paket besitzt:

- ABI-Version
- Endpoint-Handle
- Semantic-Type-Handle
- Korrelation beziehungsweise Nachrichtendaten
- Semantic-Type-Version
- Payloadlänge
- Inline-Payload

### Sendeweg

Vor Queue-Mutation werden geprüft:

1. Userspace-Pointer und vollständiger Speicherbereich,
2. Paketgröße und ABI,
3. reservierte Felder,
4. Handletyp und Senderecht,
5. Endpoint-Semantic-Contract,
6. Type Handle und Version,
7. Semantic Validation,
8. Queuekapazität.

### Empfangsweg

Der Empfang prüft Receive-Recht, Endpointtyp, gespeicherten Validierungszustand,
Typ und Version, bevor Daten in den Userspace kopiert werden.

## 34. Service Manager

Services besitzen:

- Serviceobjekt
- Providerreferenz
- Semantic Input Type und Version
- Semantic Output Type und Version

Provideridentität und semantischer Contract bleiben getrennt. Ein Provider mit
technisch gleicher, aber semantisch anderer Repräsentation wird abgelehnt.

## 35. Process Manager

Vorhanden sind Prozessobjekte, Prozessidentitäten, Self-Abfragen und sichere
Self-Handles. Prozesse bilden die Grundlage für Userspace-Isolation und
Handlebesitz.

## 36. Thread Manager und Scheduler

Der Thread Manager verwaltet Threadobjekte und Zustände. Der Scheduler besitzt
eine erste lauffähige Umschaltgrundlage und zwei aktive Testthreads.

## 37. CPU Manager und SMP-Grundlage

Umgesetzt sind:

- BSP-Erkennung
- CPU-Topologie aus ACPI/MADT
- Per-CPU-Daten
- BSP-Barriere
- lokaler TLB-Pfad
- SMP-bezogene ABI-Grundlage

Noch offen ist die vollständige Aktivierung und parallele Planung der APs.

## 38. Module Loader

Der Modulpfad prüft:

- ABI
- Segmentgrenzen
- Trustzustand
- W^X
- Lade- und Aktivierungszustand

Eine vollständige produktive Modul- und Treiberlandschaft existiert noch nicht.

## 39. Device Manager

Der Device Manager besitzt Kernelobjekte und Registrierungsgrundlagen für
Bootgeräte und spätere Treiberbindung.

## 40. VFS

Umgesetzt sind:

- VFS-Grund-ABI
- Filesystemobjekt
- Mount Namespace
- Mountobjekt
- VFS-Node
- Bootstrap-Root
- Öffnen des Root-Handles
- Lookup des Pfads `/`

Persistente NovaFS-Dateien und vollständige allgemeine Dateisystemoperationen
sind noch nicht umgesetzt.

## 41. Netzwerk

Die Kernelgrundlage umfasst:

- IPv4
- IPv6-Objektmodell
- UDP
- ICMP-Grundlage
- TCP-Stream
- Socketobjekte
- Bind
- Connect
- Listen
- Accept
- begrenzte nicht blockierende Warteschlangen
- Loopbackprüfungen
- Längen- und Prüfsummenvalidierung
- Capability-Prüfung für privilegierte Raw Sockets

Es handelt sich derzeit hauptsächlich um eine Kernel- und Loopback-Grundlage,
nicht um einen vollständigen Hardware-Netzwerkstack.

## 42. Power Manager

Umgesetzt sind:

- Systemstatusabfrage
- zeitlich begrenzte Wake Locks
- Freigabe von Wake Locks
- Leistungsprofil
- capabilitygeschützte Zustandsanforderung
- kontrollierter Shutdown-Pfad
- Escape-Auslösung aus der Kernelansicht

Unter QEMU kann der Powerpfad den virtuellen Rechner geordnet beenden.

## 43. Userspace und System Calls

Der Kernel besitzt:

- x86-32-Ring-3-Grundlage
- TSS
- getrennte Code- und Stackseiten
- `int 0x80`-System-Call-ABI
- Pointer- und Bereichsvalidierung
- Copy-from-User und Copy-to-User
- strukturierte Argument- und Ergebnisblöcke
- eigene Fehlercodes für ABI, Größe, Rechte, Pointer, Typ und Validierung
- Shared Service Page

## 44. Userspace-Selbsttests

Der Bootstrap-Userspace prüft:

- Prozess- und Threadidentität
- sichere Self-Handles
- Ablehnung einer Typverwechslung
- IPC-Roundtrip
- falschen Semantic Type
- falsche Semantic-Type-Version
- semantisch ungültigen Wert
- VFS-Root und `/`-Lookup
- Handlefreigabe
- Powerabfragen und Capability-Ablehnung
- UDP-Loopback
- TCP-Stream
- TCP-Listener und Accept
- Raw-Socket-Ablehnung
- Loggingabfrage
- Shared Service Page

## 45. Semantic-Type-Registry

### Identität

Ein Semantic Type besitzt:

- namespacefähigen Namen
- internierte Handle-ID
- Version
- primitive Repräsentation
- Validator-ID

### Aktuelle Typen

```text
nova.kernel.ipc.inline-data
nova.kernel.ipc.diagnostic
```

Beide verwenden `Bytes8`. Trotzdem sind sie nicht automatisch kompatibel.

### Registry-Lebenszyklus

Registrierung ist nur während der frühen BSP-Control-Plane erlaubt. Danach wird
die Registry versiegelt. Userspace kann keine neuen Kerneltypen registrieren
oder vorhandene Bedeutungen verändern.

## 46. Semantic Compatibility

Die Kompatibilitätsfunktion unterscheidet:

- `Unknown`
- `Exact`
- `Subtype`
- `Trait`
- `Convertible`
- `Incompatible`

`Subtype` und `Trait` sind reserviert, bis die zugehörige Spezifikation
vollständig umgesetzt wird.

Versionen gehören zur Prüfung. Derselbe Type Handle mit falscher Version gilt
nicht als exakter Contract.

## 47. Semantic Conversion

Eine Conversion-Registrierung enthält:

- Source Type
- Source Version
- Target Type
- Target Version
- Conversion-Capability-ID
- Lossless- oder Lossy-Klasse

Der Kernel kann damit einen zulässigen Pfad erkennen. Er führt die Konvertierung
nicht stillschweigend aus. Eine Lossy Conversion muss ausdrücklich bekannt und
erlaubt sein.

## 48. Typed Resources

Objekte besitzen getrennte Sidecarfelder für Semantic Type, Version und
Validierungsstatus. Die Ressourcen-ID wird dadurch nicht zum Type Handle.

Ein vorhandener Primary Type kann nicht still ersetzt werden. Beim Freigeben des
Objekts werden die Sidecarinformationen ebenfalls entfernt.

Mehrere kompatible Semantic Types pro Ressource sind noch nicht vollständig
umgesetzt.

## 49. Typed IPC

Senderclaim, Endpointcontract und technische Repräsentation werden getrennt
geprüft. Gleiche Bytegröße reicht nicht. Eine Nachricht mit dem
`diagnostic`-Typ kann nicht an einen `inline-data`-Endpoint gesendet werden,
obwohl beide acht Bytes verwenden.

## 50. Typed Capabilities und Services

Ein Service deklariert Input- und Outputtyp. Eine spätere automatische
Komposition darf nur bei exakter Kompatibilität oder über eine ausdrücklich
registrierte Conversion Capability erfolgen.

## 51. Semantic Validation

### Ablauf

```text
Type Check
    ↓
Type Validation Rule
    ↓
Capability Contract Rule
    ↓
Execution
```

### Aktuelle Regeln

- Inline-Daten müssen nicht leer sein.
- Diagnosewerte müssen einen Diagnosecode enthalten.
- Ein Capability Contract kann zusätzlich die maximale Länge begrenzen.

### Strukturierter Fehler

Bei Ablehnung werden gespeichert:

- Fehlerklasse
- verletzte Regel
- erwarteter Wert
- tatsächlicher Wert
- Validierungszustand

Die Validierung verändert den Nutzwert nicht.

## 52. Grafischer Kernelstatus

Nach erfolgreichem Start zeigt der Kernel eine NovaOS-Statusansicht mit Logo,
Titel, geladenen Komponenten und Logausgabe. Escape fordert über den Powerpfad
ein geordnetes Herunterfahren an.

## 53. Kernel-Panic-Anzeige

Der Panic-Screen verwendet das gemeinsame NovaOS-Fehlerdesign:

- dunkler Hintergrund
- Fehlerakzent
- NovaOS-Branding
- verständlicher Fehlertext
- Fehlercode
- kleiner geglätteter Nova-Smiley
- technische Diagnose über die serielle Ausgabe

Ein Testpfad erlaubt das gezielte Auslösen der Panic-Anzeige.

## 54. Zuletzt bestätigte Startpfade

### UEFI mit NKI

```text
UEFI:NKI-VALIDATED
UEFI:KERNEL-INTEGRITY-VERIFIED
UEFI:KERNEL-SIGNATURE-NOT-PRESENT
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_KERNEL_READY
```

### UEFI mit direktem ELF32

```text
UEFI:ELF32-DIRECT-VALIDATED
UEFI:KERNEL-STRUCTURE-VALIDATED
UEFI:KERNEL-HANDOFF-READY
NOVA_KERNEL_READY
```

### UEFI mit direktem ELF64

```text
UEFI:ELF64-DIRECT-VALIDATED
UEFI:KERNEL-STRUCTURE-VALIDATED
UEFI:KERNEL-HANDOFF-READY
NOVA_ELF64_LONG_MODE_READY
```

### Persistenter UEFI-Boot-Control-Zustand

Der UEFI-Bootloader verwaltet einen kompakten 64-Byte-Boot-State. Darin stehen
Active-, Candidate- und Known-Good-Slot, Versuchszähler, policyfähiges Limit,
letztes Ergebnis und letzter grober Health-Milestone. `CRC32` erkennt
Beschädigungen; eine überlaufsicher verglichene Sequenznummer bestimmt die
neueste gültige Kopie.

Der Zustand liegt redundant in `NovaBootState0` und `NovaBootState1`. Änderungen
werden abwechselnd geschrieben und sofort zurückgelesen. So bleibt beim
abgebrochenen Schreiben die vorherige Kopie erhalten. Sind beide Kopien nicht
lesbar oder Variablenschreibzugriffe nicht möglich, startet der Loader mit
einem konservativen flüchtigen Standardzustand.

Der Zustandsautomat kann:

- einen vollständig vorbereiteten Slot als Candidate vormerken,
- Candidate-Bootversuche zählen,
- nach dem policydefinierten Limit deterministisch Known-Good auswählen,
- einen eindeutig beschädigten Candidate sofort deaktivieren,
- bei zwei vorhandenen, aber ungültigen Boot-State-Kopien Fail-Safe direkt den
  Recovery-Kernel wählen, ohne die forensisch relevanten Kopien zu überschreiben,
- den gewählten Slot und Versuchszähler in das NBHP/BIB-Boot-Options-TLV
  übernehmen.

`make uefi-boot-control-state-check` prüft beide Slotrichtungen, alle Limits von
1 bis 16, deterministische Wiederholung, unzulässige Übergänge, CRC-Fehler und
den Sequenzüberlauf. `make test-uefi-boot-control` startet QEMU zunächst mit
einer beschreibbaren Firmwarekopie, beschädigt danach die neueste persistente
Kopie und startet erneut. Der zweite Lauf muss `INVALID-COPY-IGNORED` melden,
die ältere Kopie wiederherstellen und erneut `NOVA_KERNEL_READY` erreichen.
Danach beschädigt der Test beide Kopien. Der dritte Start muss
`BOOT-CONTROL-CORRUPT-RECOVERY`, `AUTOMATIC-RECOVERY-SELECTED` und
`RECOVERY-NKI-VALIDATED` melden; der Kernel bestätigt den Recovery-Modus aus
NBHP/BIB und erreicht wiederum `NOVA_KERNEL_READY`.

Bewusst fehlt noch `Candidate -> KnownGood`: Die ADR verbietet einen Commit
allein aufgrund von Kernel Entry. Erst ein künftig spezifizierter,
capabilitygeschützter Health Provider darf die erforderlichen Milestones
bestätigen und den Commit auslösen.

### Erkannte CPUs

Der aktuelle QEMU-Test mit `-smp 4` meldet:

```text
NOVA: ACPI MADT, erkannte CPUs (hex): 0x00000004
```

## 55. Noch offene Hauptpunkte

- produktiver kryptografischer NKI-Signaturcontainer
- Schlüssel-, Trust-Anchor- und Revocation-Verwaltung
- TPM-gestütztes Measured Boot
- autorisierte Candidate-Staging-Schnittstelle, eindeutige Generationen und
  capabilitygeschützter Health-Commit; persistente redundante Auswahl,
  Bootversuchslimit und Known-Good-Rollback sind bereits vorhanden
- echte Prozess-/Stromunterbrechung an jedem UEFI-Schreibzeitpunkt; CRC-Korruption
  der neuesten Kopie mit erfolgreichem Rückfall ist bereits in QEMU geprüft
- Kernelkompression mit LZ4, ZSTD und GZIP
- vollständige AP-Aktivierung und echter SMP-Scheduler
- persistentes NovaFS
- vollständige Hardwaretreiber
- mehrere Semantic Types pro Ressource
- Semantic Relationships, Discovery und Execution
- tatsächliche Ausführung von Conversion Capabilities
- physische UEFI-Hardwaretests
- aktuelle VirtualBox-End-to-End-Prüfung
- pixelgenauer Vergleich aller Seiten mit den Referenzbildern

## 56. Wichtige Quellpfade

| Thema | Pfad |
|---|---|
| Normative Dokumente | `docs/` |
| Entwicklungsübersicht | `ENTWICKLUNGSSTAND.md` |
| UEFI-Hauptprogramm | `boot/bootloader/uefi/main.c` |
| UEFI-Kernellader | `boot/bootloader/uefi/kernel_loader.c` |
| UEFI-Kerneltrampoline | `boot/bootloader/uefi/kernel_transition.S` |
| UEFI-Firmwarestatus | `boot/bootloader/uefi/firmware.c` |
| Bootmanager | `boot/bootloader/bootmenu/` |
| Bootprotokoll C | `boot/include/nova_boot_protocol.h` |
| Bootprotokoll Assembly | `boot/include/nova_boot_protocol.inc` |
| Kernel | `kernel/arch/x86_64/entry32.asm` |
| Semantic Types | `kernel/arch/x86_64/semantic32.inc` |
| UEFI-Imagebuilder | `scripts/build-uefi-image.ps1` |
| ELF32-Builder | `scripts/build-elf32.ps1` |
| ELF64-Builder | `scripts/build-elf64.ps1` |
| NKI-Builder | `scripts/build-nki.ps1` |
| UEFI-Kernelnegativtest | `scripts/test-uefi-kernel-validation.ps1` |
| Hauptbuildsystem | `Makefile` |

## 57. Pflegehinweis

Neue technische Funktionen sollen hier nach ihrer Umsetzung ergänzt werden.
Dabei muss klar bleiben, ob ein Punkt nur spezifiziert, bereits implementiert,
automatisiert getestet oder lediglich als zukünftiger ABI-Wert reserviert ist.

## 58. UI-Architekturkern aus `001-UI`

Die neue Bibliothek `ui/src/runtime.c` setzt die gemeinsamen Invarianten aller
26 angenommenen UI-NPSPECs in einem betriebssystemunabhängigen C17-Kern um.
`ui/include/nova/ui/runtime.h` ist der öffentliche Vertrag.

### Modell und Einheiten

`DLU` steht für *Device-independent Layout Unit*. Positionen, Größen, Abstände
und Rundungsradien bleiben dadurch von physischen Pixeln und einer konkreten
Displayauflösung unabhängig. Der Display Server löst DLU später mit dem
jeweiligen Skalierungsfaktor in physische Ausgabe auf.

Retained Nodes besitzen stabile, stark typisierte IDs und getrennte Desired-
und Actual-Werte. Reconciliation übernimmt nur geänderte Eigenschaften,
erzeugt Damage und bewahrt die Identität unveränderter Nodes. Ein erfolgreicher
Scene-Publish prüft Parent-Beziehungen und Zyklen und erhöht erst danach die
sichtbare Generation.

### Semantik und Accessibility

Rolle, Action, Zielobjekt und erforderliche Capability sind unabhängig von der
visuellen Repräsentation gespeichert. Der Accessibility Tree wird daraus als
eigene Datenstruktur erzeugt. Er besitzt eine eigene Generation und eigenen
Fokus, unterstützt virtuelle Ausschnitte großer Inhalte und führt Actions erst
nach erneuter Capability-Prüfung aus. Geschützte Nodes geben nicht ihren
sensiblen Namen in den Accessibility Tree weiter.

### Rendering und Presentation

Damage-Regionen werden begrenzt gespeichert und überlappend zusammengeführt;
bei unbekanntem oder zu komplexem Damage steht ein vollständiges Redraw bereit.
Surfaces speichern Buffer Age und ihren Owner. Der Compositor wählt kontrolliert
zwischen Direct Scanout, austauschbarer GPU-Composition und Software-Fallback.
Der Frame Scheduler bündelt Änderungen, begrenzt die Queue auf zwei Frames,
liefert Backpressure und priorisiert Input. VRR-Minimum, -Maximum und aktueller
Zustand werden pro Display verwaltet; Fixed Refresh bleibt Fallback.

### Window-, Input- und Capability-Grenzen

Window, Surface, Display und Owner verwenden nicht austauschbare ID-Typen.
Display-Hot-Unplug verschiebt Fenster auf einen verbleibenden Display oder
suspendiert sie ohne Display. Input wird nach Session, Secure-Input-Zustand,
Fokus, Scene-Z-Order und Capture geroutet. Touch behält vom Beginn bis Ende ein
stabiles Ziel; Owner-Ausfall widerruft Capture und isoliert seine Fenster.

Capability Discovery und Authorization sind getrennte Zustände. Semantische
Contributions für Startmenü, Ribbon und Dashboard werden von der Runtime
validiert, bei äquivalenten Aktionen dedupliziert und bei Providerausfall
isoliert entfernt. Sichtbarkeit allein erteilt keine Ausführungsberechtigung.

### Nachweis und verbleibende Provider

`make ui-architecture-runtime-check` kompiliert und testet diese Verträge mit
striktem Warning-as-Error-Modus. Reale GPU-/Displaytreiber, Shared-Buffer-
Transporte, Persistenz, Suche, Privacy und ausführende Capability Provider sind
bewusst außerhalb dieser Runtimegrenze und müssen als nachfolgende
Systemdienste angebunden werden.

## 59. Display Server und erste Ring-3-System-UI

Der Kernel stellt mit Service-ID 12 eine Display-ABI bereit. Der
System-UI-Prozess fragt über `Display.QueryPrimary` kontrollierte Metadaten ab
und übergibt anschließend eine `NovaSystemSceneV1`. Diese Struktur enthält
keine Pointer, Bufferadressen oder ausführbare Daten, sondern nur Generation,
Szenenflags, Theme, Workspace, Fokus und semantische Color-Tokens.

Die Framebufferadresse aus dem NBHP/BIB bleibt ausschließlich im Kernel. Vor
einer Scene-Presentation prüft der Dispatcher:

- `SECURITY_CAP_DISPLAY_SYSTEM_UI`,
- ABI-Version und exakte Strukturgröße,
- streng steigende Scene-Generation,
- ausschließlich bekannte Scene-Flags,
- gültige Theme-, Workspace- und Tokenbereiche,
- vollständig nullgesetzte reservierte Felder.

Die erste Scene aktiviert Desktop, Startmenü, Ribbon und Taskleiste. Bei 1.000
oder mehr horizontalen Pixeln zeigt die Taskleiste getrennte App-, Status-,
Uhr- und Benachrichtigungsbereiche; darunter wechselt sie auf einen kompakten
Statusbereich. Das Startmenü reserviert den unteren Raum für die schwebende
Taskleiste. Providerbeiträge für die Taskleiste verwenden dieselbe dynamische
Capability- und Fehlerisolation wie Ribbon und Dashboard.

`scripts/test-uefi-display-server.ps1` bootet das stabile GPT/FAT32-Image mit
einer privaten Firmwarekopie und verlangt die Display-Server-, Query-, Scene-
und Kernel-Ready-Marker. Der reale QEMU-Frame wurde zusätzlich als
`build/nova-desktop.png` visuell geprüft.

## 60. Geschützter Input Router für die System-UI

Die Display-ABI besitzt nun die nichtblockierende Operation
`Display.PollInput`. Sie liefert eine pointerfreie, 32 Byte große
`NovaSystemInputEventV1` mit Aktion, ursprünglichem Scan-Code, monotonem Tick
und semantischem Zielelement. Zugriff erhält weiterhin ausschließlich der
Prozess mit `SECURITY_CAP_DISPLAY_SYSTEM_UI`.

Der Kernel verarbeitet sowohl PS/2-Scan-Code-Set 1 als auch Set 2 einschließlich
Extended- und Break-Präfixen. Er gibt keine Controllerports an Ring 3 frei,
sondern reduziert die Eingabe auf vier Aktionen: Startmenü umschalten,
Startmenü schließen, Fokus weiterschalten und fokussiertes Element aktivieren.
Eine feste Ein-Ereignis-Warteschlange begrenzt Speicher und Eingabefluten;
Überlauf wird gezählt und nicht dynamisch alloziert.

Der Bootstrap-System-UI-Prozess pollt diese Events, verändert seine bestehende
deklarative `NovaSystemSceneV1`, erhöht die Generation und reicht die Szene
erneut ein. Dadurch bleibt der Kernel für Validierung und Darstellung
zuständig, während Sichtbarkeit und Fokus aus dem Ring-3-Zustand kommen. Das
Suchfeld und die sechs Startmenü-Kacheln zeichnen den aktuellen Fokus sichtbar
mit dem Nova-Akzenttoken.

Der QEMU-Test sendet über QMP reale virtuelle Tastendrücke. Er weist zunächst
nach, dass `Tab` den Fokus über den vollständigen Inputpfad verschiebt und eine
neue Szene präsentiert. Das erste `Esc` schließt danach das Startmenü. Ein
zweites `Esc` wird bei
geschlossenem Startmenü nicht an die UI delegiert, sondern durchläuft den
geordneten Power-Manager-Pfad bis `NOVA: Power Shutdown PLATFORM_OFF`.

## 61. UEFI-Bootsplash und ruhiger Kernelstart

Die Quelldatei `boot/image/bootsplash.png` wird von
`scripts/build-bootsplash.ps1` mit hochwertigem Downsampling nach 1280×720
konvertiert. Das interne NBS1-Format besitzt einen 32-Byte-Header mit Breite,
Höhe, Stride, verlustfreiem RGB888-Format, Payloadgröße und CRC32. Dadurch muss weder ein
PNG-Decoder noch ein mehrere Megabyte großer RGBA-Puffer in den begrenzten
Kernelcontainer aufgenommen werden.

`scripts/build-uefi-image.ps1` legt die Ressource als `SPLASH.NBS` in die
FAT32-ESP. Der UEFI-Kernellader liest sie vor dem letzten Memory-Map-Snapshot,
prüft Header, Grenzen, Format, exakte Dateigröße und CRC32 und rendert sie bei
abweichenden GOP-Auflösungen bilinear gefiltert sowie seitenverhältnistreu mit
schwarzen Letterbox-Flächen direkt in den aktiven
GOP-Framebuffer. Danach meldet er `UEFI:BOOTSPLASH-READY` und übergibt denselben
Framebuffer über NBHP/BIB an den Kernel.

`kernel_operational_prepare` zeichnet beim erfolgreichen Grafikstart keine
Logkonsole mehr. Der UEFI-Splash bleibt damit während der Kernelinitialisierung
sichtbar und wird erst durch die erste validierte Ring-3-Systemszene ersetzt.
Die Funktionen für Kernel-Log- und Fehlerdarstellung bleiben vorhanden und
können von Fehler- und Diagnosepfaden weiterhin verwendet werden.

## 62. NovaOS Aurora-Designsystem und Desktop-Shell

`ui/include/nova/ui/design.h` definiert die gemeinsame visuelle Sprache der
neuen Referenzen. Dazu gehören semantische Farben für Desktop, Acrylic,
verschiedene Surface-Ebenen, Text, Borders, Blau, Violett, Cyan, Magenta und
Statuszustände. Radien, Abstandsgrundmaß, Control-/Taskleistenhöhe,
Transparenz, Blur, Schatten und Motion-Zeiten sind ebenfalls zentrale Tokens.

`ui/include/nova/ui/shell.h` und `ui/src/shell.c` bilden den heapfreien
Shell-Zustand. Das responsive Layout reserviert Branding, globale
Befehlsleiste, Systemstatus, Arbeitsbereich, schwebende Taskleiste und das
dreispaltige Startmenü. Explorer, Nova Sheet und Fähigkeiten Studio besitzen
stabile Fensteridentitäten und Zustände für Aktivieren, Verschieben,
Minimieren, Maximieren, Wiederherstellen und Schließen. Taskleistenaktivierung
stellt minimierte Apps wieder her.

Der Startmenüzustand unterstützt Nova-Orb-Toggle, Outside-Click, Escape und eine
begrenzte 190-ms-Transition. `Ctrl+K` öffnet und fokussiert die globale Command
Palette. `tests/ui_shell_runtime.c` validiert das Designsystem, die Shell-
Geometrie bei 1920×1080 und 1280×720 sowie alle genannten Interaktionen mit
`-Wall -Wextra -Werror`.

## 63. Produktive Aurora-Desktop-Szene

Der Kernel-Display-Provider rendert die neue Shell jetzt aus der validierten
Ring-3-Systemszene. Der sichtbare Pfad enthält Navy-/Aurora-Hintergrund,
Branding, globale Befehlsleiste, Systemstatus, einen runden Nova-Orb, die
Acrylic-Taskleiste und das dreispaltige Startmenü. Alle Elemente entstehen aus
Framebuffer-Primitiven und Text; kein Referenzbild wird als UI-Hintergrund
verwendet.

Das erste gemeinsame NovaWindow stellt den Explorer mit Navigation,
Breadcrumb, Suche, Toolbar, Sidebar, Ordnerkarten, Dateiliste und Statuszeile
dar. Zusätzlich existieren produktive Arbeitsbereiche für Nova Sheet mit
Ribbon, Budgettabelle und Fähigkeitenpanel sowie für Fähigkeiten Studio mit
Navigation, Katalog, Node-Canvas und Inspector.

Die pointerfreie Systemszene verwendet das validierte Workspace-Feld für die
aktive Anwendung. Im Startmenü schaltet `Tab` den sichtbaren Fokus weiter und
`Enter` öffnet Explorer, Nova Sheet oder Fähigkeiten Studio. Ein reiner
Fokuswechsel bestätigt weiterhin eine neue Szenengeneration, vermeidet aber
einen identischen vollständigen Software-Frame. Sichtbarkeits- und
Workspacewechsel werden vollständig neu gezeichnet.

Das aktuelle `build/nova-uefi.img` wurde neu erzeugt. Architektur-, Shell- und
UEFI-Displaytests laufen erfolgreich; der QEMU-Test bestätigt außerdem das
Schließen des Startmenüs und den geordneten Shutdown bis `PLATFORM_OFF`.

## 64. Semantische Anwendungsnavigation

Der PS/2-Eingabepfad normalisiert die vier Pfeiltasten aus Scan-Code-Set 1 und
2 zu richtungsbezogenen System-UI-Aktionen. Ring 3 führt getrennte,
begrenzte Fokusbereiche für Startmenü, Explorer-Dateien, Sheet-Zeilen und
Studio-Nodes. Beim Öffnen einer Anwendung wird ein gültiger Startfokus gesetzt;
beim erneuten Öffnen des Startmenüs kehrt der Fokus kontrolliert zur Suche
zurück.

Fokusänderungen zeichnen nicht mehr den gesamten Desktop. Der Display-Provider
rekonstruiert nur das geöffnete Startmenü oder das aktive NovaWindow.
Explorer-Zeilen erhalten eine Auswahlfläche, Nova Sheet verschiebt den
Zellrahmen über die Budgetzeilen und Fähigkeiten Studio markiert den gewählten
Node. Sichtbarkeits- oder Workspacewechsel bleiben vollständige Frames.

Da der gewachsene interaktive Ring-3-Dienst die ursprüngliche 4-KiB-Codeseite
überschreitet, besitzt er jetzt zwei einzeln allozierte und gemappte
Codeseiten. Beide Seiten sind user-lesbar und ausführbar, aber nicht
beschreibbar; die feste Obergrenze von 8 KiB wird beim Assemblieren geprüft.
Der QEMU-Displaytest sendet zusätzlich eine reale Pfeil-rechts-Eingabe über QMP
und verlangt sowohl die Zustellung als auch eine neue Scene-Presentation.

## 65. Strukturierte Nebenläufigkeit im Kernel

Der Kernel besitzt jetzt einen begrenzten Task-Scope-Manager nach den
angenommenen Process- und Concurrency-NPSPECs. Ein Task Scope gruppiert
zusammengehörige nebenläufige Arbeit unter einem expliziten Besitzer und einem
optionalen Parent-Scope.

Der initiale Kernelprozess erhält einen dauerhaften Root-Scope. Die vorhandenen
Kernelthreads referenzieren diesen Scope in ihrem ABI-Datensatz. Child-Scopes
erhöhen den Child-Zähler ihres Parents; ein Parent kann deshalb nicht beendet
werden, solange noch ein Kind aktiv ist.

Cancellation wird deterministisch durch die feste Scope-Tabelle an alle
Nachfahren weitergegeben. Jeder betroffene Scope speichert den
Cancellation-Grund. Ein Scope kann erst geschlossen werden, wenn seine Kinder
aufgelöst wurden; beim Abschluss wird die Parent-Beziehung atomar aktualisiert.
Die feste Kapazität verhindert unbegrenzte Kernelallokationen.

`kernel/include/nova/task_scope.h` beschreibt das versionierte, 32 Byte große
Record- und API-Layout. Der Kernel-Selbsttest prüft Hierarchie, verweigerten
vorzeitigen Parent-Abschluss, Cancellation-Propagation und geordneten Abschluss.
Der UEFI-Displaytest verlangt den Marker des erfolgreichen Selbsttests, bevor er
die Ring-3-Oberfläche und den Shutdownpfad prüft.

## 66. Verwaltete Kernel-Tasks

Auf den Task Scopes baut nun ein eigenständiger Task Manager auf. Jeder Task
besitzt eine stabile Task-ID, einen Prozess-Owner, genau einen owning Scope,
optional einen Parent-Task sowie getrennte Felder für Zustand, Resultat und
Cancellation-Grund.

Der aktuelle Lifecycle unterscheidet `Created`, `Ready`, `Running`, `Waiting`,
`CancellationRequested`, `Completed`, `Cancelled` und `Failed`. Abgeschlossene,
abgebrochene und fehlgeschlagene Zustände sind terminal. Scheduling-Policy und
Task-Modell bleiben getrennt.

Cancellation beendet einen Task nicht hart. Eine Anforderung propagiert entlang
der Task-Hierarchie und wird erst an einem expliziten Cancellation Point durch
`task_checkpoint` in den terminalen Zustand überführt. Bis zu diesem Cleanup
bleibt der Task im Active-Task-Zähler seines Scopes sichtbar und verhindert
dessen vorzeitigen Abschluss.

Der Selbsttest prüft Parent-/Child-Tasks, hierarchische Cancellation,
kooperative Checkpoints, verweigerten Scope-Abschluss mit aktiver Arbeit sowie
einen normalen Completion-Pfad mit erhaltenem Ergebnis. Das öffentliche
32-Byte-ABI steht in `kernel/include/nova/task.h`.

## 67. Task-/Thread-/Scheduler-Integration

Die drei vorhandenen Kernelthreads werden beim Registrieren nun zusätzlich als
verwaltete Tasks im dauerhaften Kernel-Root-Scope angelegt. Eine begrenzte
Sidecar-Tabelle hält die Task-ID pro Scheduler-Slot, ohne das bestehende
Thread-ABI inkompatibel zu vergrößern.

Bei jedem Timer-Tick setzt der Round-Robin-Scheduler den zuvor laufenden Task
zurück auf `Ready` und den ausgewählten Task auf `Running`. Terminale oder zur
Cancellation vorgemerkte Zustände werden dabei nicht überschrieben. Taskmodell
und Scheduling-Policy bleiben getrennt, sind aber über stabile IDs miteinander
verbunden.

Der Thread-Manager-Selbsttest validiert Owner, Root-Scope-Zuordnung und die
vollständige Taskbelegung aller drei Scheduler-Slots. Der UEFI-End-to-End-Test
verlangt nun auch den erfolgreichen Thread-Manager-Marker.
