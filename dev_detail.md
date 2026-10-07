# NovaOS – technische Implementierungsdetails

**Stand:** 7. Oktober 2026
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

`Candidate -> KnownGood` ist nun als streng geprüfter Zustandsübergang
vorhanden. Kernel Entry allein reicht weiterhin nicht. Die persistente
Anbindung eines capabilitygeschützten Health Providers aus dem laufenden
Kernel beziehungsweise Userspace bleibt ein eigener Folgeschritt.

### Erkannte CPUs

Der aktuelle QEMU-Test mit `-smp 4` meldet:

```text
NOVA: ACPI MADT, erkannte CPUs (hex): 0x00000004
```

## 55. Noch offene Hauptpunkte

- produktiver kryptografischer NKI-Signaturcontainer
- Schlüssel-, Trust-Anchor- und Revocation-Verwaltung
- TPM-gestütztes Measured Boot
- autorisierte Candidate-Staging-Schnittstelle und capabilitygeschützte
  Kernel-/Userspace-Transportbrücke für Health Evidence; eindeutige
  Generationen, Health-Commit, redundante Auswahl, Bootversuchslimit und
  Known-Good-Rollback sind bereits vorhanden
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
| Storage (PCI, AHCI, GPT) | `kernel/arch/x86_64/storage32.inc` |
| NovaFS-Kernel | `kernel/arch/x86_64/novafs32.inc` |
| NovaFS-On-Disk-ABI (C) | `kernel/include/nova/novafs.h` |
| NovaFS-Host-Werkzeug | `tools/novafs/novafs.c` |
| NovaFS-QEMU-Test | `scripts/test-uefi-novafs.ps1` |
| VFS-Syscalls auf NovaFS | `kernel/arch/x86_64/vfs32.inc` |
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

## 68. Hierarchische Task-Deadlines

Tasks können nun eine absolute Deadline auf der monotonen 100-Hz-PIT-Zeitbasis
erhalten. Die ABI unterscheidet Hard-, Firm- und Soft-Deadlines sowie die
Miss-Policies `Continue`, `Cancel` und `Fail`. Deadline-Klasse, effektiver Tick,
Policy und Miss-Zustand bleiben getrennt introspektierbar.

Eine Child-Task darf das Zeitbudget ihrer Parent-Task nicht verlängern. Fordert
sie eine spätere Deadline an, klemmt der Kernel die effektive Deadline auf die
frühere Parent-Grenze. Eine geerbte Hard-Deadline bleibt hard. Hard Deadlines
mit stiller `Continue`-Policy oder bereits abgelaufenem Zeitbudget werden durch
die Bootstrap-Admission-Control abgewiesen.

IRQ0 prüft bewaffnete Deadlines begrenzt über die feste Tasktabelle. Ein Miss
wird zunächst als eigener Deadline-Zustand gezählt. `Cancel` fordert danach den
normalen kooperativen Task-Abbruch an; `Fail` setzt einen kontrollierten
terminalen Fehlerzustand. Es erfolgt kein harter Thread-Kill.

Der Selbsttest prüft Parent-Clamping, Klassenvererbung, Miss-Erkennung und die
Weiterleitung in den Cancellation Point. Das versionierte ABI steht in
`kernel/include/nova/task_deadline.h`.

## 69. Task Groups mit WaitAll und FailFast

Der Kernel bündelt mehrere Tasks jetzt in begrenzten Task Groups. Jede Gruppe
besitzt eine stabile ID, einen Prozess-Owner, genau einen Scope und eine
explizite Completion-/Failure-Policy. Die Zuordnung eines Tasks erfolgt über
eine feste Sidecar-Tabelle und verändert das Task-ABI nicht.

`WaitAll` bleibt offen, bis alle erforderlichen Tasks terminal sind. `FailFast`
wechselt beim ersten Fehler zunächst in `Failing`, fordert für verbleibende
Tasks kooperative Cancellation an und wird erst nach deren Cleanup und Drain
terminal `Failed`. Eine explizit gecancelte Gruppe folgt entsprechend
`Cancelling` zu `Cancelled`.

Task-Completion, Task-Fehler, Cancellation Points und deadlinebedingte Fehler
melden ihren Ausgang an die owning Group. Dadurch gehen Fehler nicht verloren,
und eine Gruppe wird nicht erfolgreich abgeschlossen, solange abhängige Arbeit
ungelöst ist.

Der Selbsttest prüft einen erfolgreichen WaitAll-Lauf sowie FailFast mit
Fehlerweitergabe, Cancellation des zweiten Tasks und vollständigem Drain. Das
öffentliche ABI steht in `kernel/include/nova/task_group.h`.

## 70. Begrenztes asynchrones I/O-Request-Modell

Der Kernel besitzt nun ein erstes gemeinsames, completion-basiertes I/O-Modell.
Ein `IORequest` beschreibt Owner-Prozess, owning Task und Scope, Operation,
Ziel- und Buffer-Handle, Länge, Priorität und eine stabile Request-ID. Es werden
bewusst nur Handle-Identitäten und keine rohen Userspace- oder MMIO-Adressen im
öffentlichen Datensatz geführt.

Request-Erzeugung, spätere Provider-Ausführung und Completion sind getrennte
Phasen. Eine Completion unterscheidet vollständigen Erfolg, Teilerfolg,
kontrollierten Fehler, Cancellation und Deadline Miss. Der Zustandsautomat
akzeptiert genau einen terminalen Ausgang; verspätete zweite Completions oder
Cancellation nach Abschluss werden abgewiesen.

Jeder Request gehört zu einer aktiven verwalteten Task und übernimmt deren
Scope sowie eine bewaffnete monotone Deadline. Task-Cancellation beendet noch
offene Requests kontrolliert. IRQ0 erkennt Deadline-Misses über die feste
Tabelle, bewahrt den Miss als eigenen Completion-Status und macht den Request
anschließend terminal. Es wird kein Thread pro I/O erzeugt.

Die Queue ist auf acht gleichzeitig offene Requests begrenzt. Ist sie voll,
wird die Einreichung deterministisch als Backpressure abgewiesen und gezählt,
statt unbeschränkt Kernel-Speicher zu belegen. Der Start-Selbsttest prüft
partielle Completion, Scope-/Deadline-Vererbung, volle Queue, Backpressure,
Cancellation-Propagation und die Eindeutigkeit terminaler Ergebnisse. Das
versionierte 32-Byte-API und der 64-Byte-Request-Datensatz stehen in
`kernel/include/nova/io_request.h`. Konkrete Geräteprovider folgen getrennt.

## 71. I/O-Prioritäten und zentraler I/O-Scheduler

Das I/O-Modell unterstützt nun die fünf logischen Klassen `Realtime`,
`Interactive`, `Normal`, `Background` und `Maintenance`. Fehlt eine Angabe,
wird `Normal` verwendet. Angeforderte und tatsächlich wirksame Priorität sind
getrennt im Request sichtbar. Die Bootstrap-Policy begrenzt eine nicht durch
weitere Rechte belegte Realtime-Anforderung auf `Interactive`; Priorität wird
damit nicht implizit zu einer Berechtigung.

Der neue zentrale I/O-Scheduler löst Submission- und Ausführungsreihenfolge
voneinander. Requests mit Deadline werden nach der frühesten Zeitgrenze
ausgewählt. Ohne zeitliche Vorgabe entscheiden wirksame Priorität und danach
das Einreichungsalter. Ein vom Provider wegen eigener Backpressure noch nicht
übernommener Request kann ohne Verlust seiner Identität wieder eingereiht
werden.

Nicht ausgewählte Requests sammeln begrenzte Aging-Runden. Nach vier Runden
werden niedrige Klassen schrittweise bis maximal `Interactive` angehoben. Das
verhindert Starvation, ohne stillschweigend Realtime-Rechte zu vergeben. Queue,
Einreichungszeit, Dispatch-Zeit, Aging, Dispatch- und Promotion-Zähler sind
über feste Datensätze beziehungsweise Sidecars introspektierbar.

Der Selbsttest beweist die Policy-Begrenzung, den Vorrang einer Maintenance-
Anfrage mit Deadline vor einer Realtime-Anforderung ohne Deadline, Requeue,
Completion und vollständigen Scope-Abschluss. Das ABI steht in
`kernel/include/nova/io_scheduler.h`.

## 72. I/O Quality of Service und Admission Control

I/O-Requests können nun einem eigenständigen QoS-Profil zugeordnet werden.
Das Profil hält Klasse, Latenzziel, Mindestdurchsatz, maximale Bandbreite und
ein Budget für gleichzeitig offene Requests. QoS, Request-Priorität und
Deadline bleiben getrennte Eigenschaften; die Profilklasse darf lediglich die
wirksame Scheduler-Priorität innerhalb der bestehenden Policy verschärfen.

Harte und weiche Anforderungen werden verschieden behandelt. Die aktuelle
Bootstrap-Implementierung kann ein hartes Latenzziel ab zwei PIT-Ticks prüfen.
Harte Durchsatz- oder Bandbreitengarantien werden ohne messenden Geräteprovider
bei der Admission Control abgewiesen. Weiche Wünsche dürfen angenommen werden,
werden dann aber ausdrücklich als `Degraded` samt Ursache ausgewiesen. Dadurch
täuscht der Kernel keine noch nicht belegbare Garantie vor.

`MaximumOutstanding` dient als erstes pro Profil zurechenbares Ressourcenbudget.
Ist es ausgeschöpft, wird das Attach kontrolliert gedrosselt und gezählt; eine
hohe QoS-Klasse umgeht weder die globale Queue noch dieses Budget. Bei terminaler
Completion erfasst der Kernel aktive Requests, beobachtete Latenz, übertragene
Bytes und Zielverletzungen getrennt von den angeforderten Werten.

Der Selbsttest prüft weiche Degradation, Ablehnung einer harten, nicht belegbaren
Garantie, Owner-Zuordnung, Budgetdrosselung und Completion-Accounting. Das
versionierte ABI steht in `kernel/include/nova/io_qos.h`.

## 73. I/O Completion Queue und Batch-Zustellung

Terminale I/O-Ausgänge werden nun zusätzlich zum autoritativen Request-Zustand
als eigene `IOCompletion`-Datensätze veröffentlicht. Jeder 32-Byte-Eintrag
enthält Request-ID, Owner, Status, tatsächlich übertragene Bytes, Fehlercode,
Completion-Tick, beobachtete Latenz und Deadline-/Request-Flags. Dadurch bleibt
eine Completion eindeutig einem Request zuordenbar, auch wenn dessen Tabellenslot
später wiederverwendet wird.

Die Completion Queue ist als begrenzter FIFO-Ring mit 16 Einträgen umgesetzt.
Consumer können einen oder mehrere Einträge in einem Aufruf abholen. Diese
Batch-Zustellung bildet die gemeinsame Grundlage für spätere Event-, Future-
oder `async`/`await`-Adapter, ohne einen Callback oder Thread pro I/O zu erzwingen.
Submission- und Completion-Reihenfolge sind ausdrücklich unabhängig.

Erfolg, Partial, Fehler, Cancellation und Deadline Miss werden aus allen
terminalen Request-Pfaden veröffentlicht. Erst dieser Punkt erlaubt die sichere
Wiederverwendung des zugehörigen Buffers nach dessen I/O-Semantik. Ist der Ring
voll, wächst er nicht unbeschränkt: Der Overflow wird explizit gezählt, während
der terminale Zustand weiterhin im Request-Datensatz erhalten bleibt.

Der Selbsttest prüft Batch-Dequeue, vollständige und partielle Übertragung,
Abschlussreihenfolge `B,A`, Ring-Wrap und sichtbaren Overflow beim siebzehnten
ungelesenen Ergebnis. Das ABI steht in `kernel/include/nova/io_completion.h`.

## 74. Shared Buffer und I/O-Ownership-Lease

Auf Grundlage der neu eingelesenen NPSPECs besitzt der Kernel nun eine erste
Shared-Buffer-Abstraktion mit stabiler, von virtuellen und physischen Adressen
unabhängiger `BufferId`. Jeder 64-Byte-Datensatz enthält Owner, Größe, Zustand,
Referenz- und aktive I/O-Zähler, explizite Rechte, Backing-Art,
Ressourcenverbrauch sowie Copy- und direkte Lease-Zähler.

Der Bootstrap verwendet einen festen Pool aus vier vollständig gemappten
4-KiB-Bereichen. Dadurch sind Speicherverbrauch und Lifetime begrenzt und
introspektierbar. Physische oder virtuelle Backing-Adressen werden nicht Teil
des öffentlichen ABI. Freigegebene Bereiche werden vollständig genullt, bevor
ein anderer Sicherheitskontext sie wiederverwenden kann.

Ein I/O-Request kann seinen angegebenen Buffer über eine geprüfte Lease binden.
Dabei werden Request-Owner, Buffer-Owner, Länge und benötigte Read-/Write-Rechte
validiert. Das Ownership wechselt von `CPU owned` zu `Provider owned`; eine
Freigabe ist währenddessen verboten. Erfolg, Partial, Fehler, Cancellation und
Deadline Miss führen über denselben terminalen Pfad zurück zu `CPU owned` und
lösen genau eine I/O-Referenz.

Für Provider ohne Shared-/Zero-Copy-Fähigkeit existiert ein sicherer
Copy-Fallback zwischen zwei CPU-eigenen Buffern. Er validiert beide Owner,
Rechte und Grenzen und zählt die Kopie explizit. Der Selbsttest prüft reale
Datenübertragung, verweigerte Freigabe während aktiver I/O, Rückgabe des
Ownership bei Completion, Resource Accounting und sichere endgültige Freigabe.
Das ABI steht in `kernel/include/nova/shared_buffer.h`.

DMA wird in diesem Schritt ausdrücklich noch nicht behauptet: Die NPSPECs
verbieten, normale virtuelle Adressen automatisch als Device-Adressen zu
verwenden. IOMMU-/DMA-Mapping und Scatter/Gather bauen in späteren Schritten auf
diesem kontrollierten Buffer-Lifecycle auf.

## 75. DMA-Mapping-ABI 1.0

`kernel/include/nova/dma_mapping.h` definiert einen 64 Byte großen
`nova_dma_mapping_record_t` und ein 32 Byte großes, versioniertes API. Der
Datensatz führt Mapping-, Prozess-, Device-, Buffer- und Request-ID als
getrennte semantische Identitäten und hält Richtung, Permissions, Zustand,
Länge, Device-Adresse, Domain, Flags, Pin-Anzahl, Generation und Fehlercode.

Die Bootstrap-Implementierung in `kernel/arch/x86_64/entry32.asm` besitzt vier
feste Mapping-Slots. `dma_mapping_map` validiert den aktiven I/O-Request und
seine Shared-Buffer-Lease, leitet aus der Richtung minimale Device-Rechte ab
und erhöht Mapping-, Pin- und Byte-Accounting. `dma_mapping_unmap` löst alle
Ressourcen kontrolliert. `dma_mapping_on_terminal` hängt direkt im gemeinsamen
terminalen I/O-Pfad und deckt dadurch Erfolg, Teilerfolg, Fehler, Cancellation
und Deadline Miss ab.

Die aktuelle Plattform meldet `IommuAvailable = 0`. Ihre Device-Adressen
stammen aus einer separaten, reservierten Staging-Apertur und sind ausdrücklich
als `BOUNCE | COHERENT | RESTRICTED` gekennzeichnet. Es handelt sich damit um
einen sicheren Vertrag und Lebenszyklus für den späteren HAL-Provider, nicht um
die Behauptung eines bereits programmierten DMA-Controllers. Ein künftiger
IOMMU-Provider kann Domain und IOVA ersetzen, ohne das öffentliche Mapping-ABI
oder den I/O-Request-Vertrag zu ändern.

Der Kernel-Selbsttest beweist Rechteprüfung, genau ein Mapping pro Request,
Pinning gegen vorzeitiges Release, automatische Freigabe bei Completion und
vollständig ausgeglichenes Ressourcen-Accounting. Der UEFI-End-to-End-Test
verlangt zusätzlich die Startmarkierung
`NOVA: DMA Mapping ABI 1.0, Pinning und sicherer Fallback aktiv`.

## 76. Scatter/Gather-ABI 1.0

Das neue ABI in `kernel/include/nova/scatter_gather.h` trennt den 64 Byte
großen Descriptor vom 32 Byte großen Segment. Ein Segment referenziert einen
Shared Buffer über seine stabile ID und beschreibt Offset, Länge, minimale
Permissions, logischen Stream-Offset, Flags und Generation. Es enthält bewusst
weder eine virtuelle noch eine physische oder Device-Adresse.

`scatter_gather_create` legt einen begrenzten Descriptor im Zustand `Building`
an. `scatter_gather_append` prüft Owner, Richtung und minimale Rechte:
`Gather` erfordert Read, `Scatter` Write und `Bidirectional` beide Rechte.
Offset-, Längen- und Gesamtlängenaddition erkennen Integer-Überläufe. Direkt
benachbarte Bereiche desselben Buffers mit identischen Rechten werden
zusammengeführt; der logische Datenstrom bleibt unverändert.

`scatter_gather_seal` führt eine vollständige zweite Validierung durch und
erhöht anschließend die Referenzzahl jedes tatsächlich vorhandenen Segments.
Dadurch bleibt jeder Buffer bis zum Descriptor-Abbau gültig. Ein Release ist
bei aktiven Consumern verboten und gibt sämtliche gehaltenen Referenzen
deterministisch zurück. Vier Descriptoren mit je vier Segmenten begrenzen den
frühen Speicher- und Validierungsaufwand.

Der Start-Selbsttest verwendet zwei Buffer, coalesziert zwei angrenzende
Bereiche, verwirft einen Bereich außerhalb der Buffergrenze und weist nach,
dass ein versuchtes Buffer-Release vor dem Descriptor-Abbau fehlschlägt. Der
UEFI-Test verlangt die Markierung
`NOVA: Scatter Gather ABI 1.0, Segmente und Lifetime aktiv`.

Noch nicht Teil dieses Schritts ist die Übersetzung eines versiegelten
Descriptors in mehrere IOVA-/Device-Segmente. Diese folgt als Erweiterung des
DMA-Providers und muss dessen Segmentanzahl, maximale Segmentgröße, Alignment,
Adressbreite und Boundary-Limits anwenden.

## 77. DMA-Scatter/Gather-Provider ABI 1.0

`kernel/include/nova/dma_scatter_gather.h` ergänzt einen 64 Byte großen
Mapping-Datensatz und bis zu vier 32-Byte-Device-Segmente. Das Mapping bindet
Request, SG-Descriptor, Prozess und Device zusammen und veröffentlicht die
tatsächlich wirksamen Providerlimits. Die Device-Segmente enthalten
Quellsegmentindex, Buffer-ID, Buffer-Offset, Länge, kontrollierte Device-Adresse,
Permissions und Flags.

Der Bootstrap-Provider akzeptiert höchstens zwei aktive SG-Mappings mit jeweils
vier Device-Segmenten. Seine aktuellen Grenzen sind `MaximumSegmentSize = 64`,
`Alignment = 4`, `AddressWidth = 32` und `Boundary = 4096`. Ein Segment mit
96 Byte wird beispielsweise deterministisch in 64 und 32 Byte geteilt. Die
Ausgabe bleibt in logischer Reihenfolge; ein Überschreiten der vier
Device-Segmente führt zu einer sichtbaren Validierungsablehnung.

Da der frühe Provider noch keine echte Hardware-Descriptorqueue und keine
IOMMU besitzt, stammen seine Device-Adressen aus einer getrennten Staging-
Apertur und tragen `BOUNCE | COHERENT | RESTRICTED`. CPU- oder physische
Adressen werden nicht übernommen. Mehrfaches Auftreten desselben Buffers in
verschiedenen Quellsegmenten ist als derzeitiges Providerlimit abgewiesen; die
allgemeine SG-Schicht selbst erlaubt diese Darstellung weiterhin.

Beim Map wechseln die beteiligten Buffer exklusiv zu `Provider owned`; aktive
I/O-, Referenz-, Mapping- und Pin-Zähler werden gemeinsam erhöht. Der zentrale
terminale Request-Pfad ruft `dma_scatter_gather_on_terminal` vor der normalen
Buffer-Rückgabe auf und löst damit Erfolg, Teilerfolg, Fehler, Cancellation und
Deadline Miss identisch auf. Danach sinkt der Consumer-Zähler des
SG-Descriptors, sodass dieser sicher freigegeben werden kann.

Der Selbsttest erzeugt aus zwei Quellsegmenten drei Device-Segmente, prüft
64+32-Byte-Splitting, 4-KiB-Ausrichtung, blockiertes Descriptor-Release und
vollständig ausgeglichenes Pin-/Byte-Accounting. Der UEFI-Test verlangt die
Startmarkierung
`NOVA: DMA Scatter Gather ABI 1.0, Split und Providerlimits aktiv`.

## 78. IOMMU-Domain-ABI 1.0

`kernel/include/nova/iommu.h` definiert getrennte Datensätze für Domain,
Device-Binding, Mapping-Autorisierung und Fault. Die Größen sind 64, 32, 48
und 32 Byte; das zugehörige API ist 40 Byte groß. Domain-, Device-, Gruppen-,
Authorization- und externe Mapping-IDs bleiben semantisch getrennt.

`iommu_domain_create` legt einen begrenzten IOVA-Adressraum mit explizitem
Isolationsmodus und 32-Bit-Adressbreite an. Die frühe Implementierung besitzt
vier Domains, acht Device-Bindings und acht Mapping-Autorisierungen. Solange
`iommu_hardware_available = 0` gilt, ist ausschließlich der Modus `Restricted`
zulässig. `Hardware` und `Virtual` werden kontrolliert abgewiesen.

`iommu_bind_device` bindet ein Device mitsamt seiner IOMMU-Gruppe. Existiert
die Gruppe bereits in einer anderen Domain, schlägt die Operation fehl. Damit
behauptet NovaOS keine feinere Isolation, als eine spätere Plattformtopologie
tatsächlich bereitstellt. Mehrere Geräte derselben Gruppe können gemeinsam an
dieselbe Domain gebunden werden.

`iommu_authorize_mapping` prüft Owner, Device-Zuordnung, 4-KiB-Ausrichtung,
IOVA-Grenzen, Überlauf, Rechte und Überschneidung. Autorisierung und externes
DMA-Mapping bleiben getrennte Identitäten. `iommu_revoke_mapping` entfernt
aktive oder gefaultete Autorisierungen und korrigiert Domain- und globale
Zähler.

`iommu_report_fault` ordnet einen unzulässigen Zugriff einer Domain, einem
Device und – sofern vorhanden – einem externen Mapping zu. Der letzte Fault
enthält Sequenz, IOVA, Access und Fehlercode. Ein zugehöriger
Autorisierungsdatensatz bleibt als `Faulted` erhalten, bis er explizit revoked
wird. Dadurch geht die forensische Zuordnung beim Aufräumen nicht verloren.

Der Selbsttest erzeugt eine Restricted-Domain, bindet zwei Devices derselben
Gruppe, autorisiert eine IOVA-Seite, lehnt eine Überlappung ab, erfasst einen
Fault und baut Mapping, Bindings und Domain vollständig ab. Der UEFI-Test
verlangt die Markierung
`NOVA: IOMMU ABI 1.0, Domains, Gruppen und Fault-Zuordnung aktiv`.

Noch ausstehend ist ein konkreter VT-d-/AMD-Vi-/virtueller IOMMU-Provider, der
die abstrakten Autorisierungen in reale Seitentabellen programmiert. Bis dahin
bleiben die vorhandenen DMA-Pfade korrekt als Restricted-/Bounce-Pfade
gekennzeichnet.

## 79. Automatische IOMMU-Autorisierung für DMA

`dma_mapping_map` fragt jetzt über `iommu_domain_for_device` die aktive Domain
des Zielgeräts ab. Existiert eine Bindung, wird die kontrollierte Device-Seite
über `iommu_authorize_mapping` autorisiert und ihre Authorization-ID in einem
Mapping-Sidecar gespeichert. `DMA_MAPPING_DOMAIN` enthält die zugehörige
Domain-ID. Ohne Device-Binding bleibt der bestehende Restricted-Fallback
erhalten.

Der Scatter/Gather-Pfad wiederholt diese Operation für jedes tatsächlich
erzeugte Device-Segment. Ein 96-Byte-Quellsegment und ein weiteres
64-Byte-Segment ergeben im Selbsttest drei getrennte, jeweils 4 KiB große
IOVA-Autorisierungen. Der öffentliche SG-DMA-Datensatz führt nun ebenfalls die
Domain-ID; Authorization-IDs bleiben als Kernel-Sidecar verborgen.

Die Autorisierung erfolgt vollständig vor dem Ownership-Wechsel der Buffer.
Scheitert Segment zwei oder drei, läuft ein begrenzter Rollback über alle zuvor
erzeugten Authorization-IDs, setzt den unfertigen Mapping-Slot zurück und
liefert einen Fehler. Erst nach vollständigem Erfolg werden Buffer gepinnt und
der Descriptor als aktiver Consumer markiert.

`dma_mapping_release_record` und `dma_scatter_gather_release_record` widerrufen
ihre Autorisierungen vor dem Unpinning. Da alle terminalen I/O-Ausgänge über
diese Funktionen laufen, gilt dieselbe Reihenfolge für Success, Partial,
Failure, Cancellation und Deadline Miss. Die Selbsttests verlangen nach der
jeweiligen Completion `iommu_mapping_count == 0`, bevor die Restricted-Domain
gelöst wird.

Der UEFI-End-to-End-Test verlangt zusätzlich die Markierung
`NOVA: DMA IOMMU Lifecycle fuer linear und Scatter Gather aktiv`.

## 80. IOMMU-Fault-Propagation in den I/O-Request

Externe IOMMU-Mapping-IDs besitzen jetzt einen Typpräfix. `0x1.......` steht
für ein lineares DMA-Mapping und `0x2.......` für ein SG-DMA-Mapping; die
unteren 28 Bit enthalten die jeweilige stabile Mapping-ID. Damit kann
`iommu_report_fault` nach der forensischen Erfassung kontrolliert an den
zuständigen DMA-Manager weiterleiten.

`dma_mapping_fault` setzt den linearen Datensatz auf `Faulted`, speichert den
Fehlercode und löst zuerst IOMMU-Autorisierung, Mapping und Pinning. Danach ruft
es `io_request_complete` mit null übertragenen Bytes und dem Gerätefehler auf.
Der Request endet dadurch in `IO_REQUEST_STATE_FAILED` mit
`IO_COMPLETION_FAILED`.

`dma_scatter_gather_fault` führt dieselbe Sequenz für einen vollständigen
SG-Transfer aus. Auch wenn nur ein Device-Segment den Fault ausgelöst hat,
werden sämtliche Autorisierungen der Operation revoked. Alle beteiligten
Buffer kehren aus `Provider owned` nach `CPU owned` zurück, der Consumer-Zähler
des SG-Descriptors wird reduziert und erst danach entsteht die Fehler-
Completion.

Die Reihenfolge lautet damit für beide Pfade:

```text
IOMMU Fault
  → Mapping als Faulted markieren
  → alle IOVA-Autorisierungen widerrufen
  → DMA-Mapping entfernen und Buffer entpinnen
  → Ownership an CPU zurückgeben
  → IORequest als Failed abschließen
  → Fehler-Completion veröffentlichen
```

Die Selbsttests prüfen explizit `IO_REQUEST_STATE_FAILED`,
`IO_COMPLETION_FAILED`, den unveränderten Fehlercode und vollständig geleerte
IOMMU-/DMA-Zähler. Der UEFI-Test verlangt die Markierung
`NOVA: IOMMU Fault beendet DMA und IO Request kontrolliert`.

## 81. Normalisierte HAL-Hardwaretopologie

`kernel/include/nova/topology.h` definiert die HAL-Topologie-ABI 1.0. Ein
Topologiedatensatz ist 64 Byte groß und trennt `TopologyId`, Objekttyp,
Parent-ID, Zustand, Hardware-ID, NUMA-Knoten, IOMMU-Gruppe, Child-Zähler,
Eigenschaften, Flags, Generation und Änderungssequenz. Die zugehörige API ist
40 Byte groß. IDs für Topologie, Hardware, NUMA und IOMMU-Gruppen sind im
C-Header als getrennte semantische Typen benannt.

Der frühe Kernel baut daraus einen begrenzten, normalisierten Graphen:

```text
System
├── NUMA Node 0 (UMA-Fallback)
│   ├── CPU Package 0
│   │   └── Core 0
│   │       └── Hardware Thread 0
│   └── Memory Region
├── Interrupt Controller
└── Bootstrap Bus
    ├── IOMMU Group 0x10 ── Device 0xD001
    ├── IOMMU Group 0x20 ── Device 0xD002
    └── IOMMU Group 0x30 ── Device 0xD003 / 0xD004
```

Dieses Modell ist ausdrücklich als `BOOTSTRAP` gekennzeichnet. Die Geräte
`0xD001` bis `0xD004` sind Endpunkte der vorhandenen DMA-/IOMMU-Selbsttests
und keine behauptete PCI-Erkennung. Noch fehlende Firmware- und Busprovider
können später validierte ACPI-, CPU- und PCI-Daten über dieselbe ABI eintragen.
Für Systeme ohne bestätigte NUMA-Informationen stellt Knoten 0 das geforderte
einheitliche UMA-Modell bereit; unbekannte Device-Lokalität bleibt mit
`0xFFFFFFFF` ausdrücklich unbekannt.

`topology_register` akzeptiert nur bekannte Typen, einen bereits vorhandenen
Parent und eine innerhalb des Typs eindeutige Hardware-ID. Ein neuer Knoten
wird erst nach vollständiger Validierung `Online`, erhöht den Child-Zähler des
Parents und erhält eine monotone Änderungssequenz. Weil nur bestehende Knoten
Parents neuer Knoten sein können, kann beim Aufbau kein Zyklus entstehen.

`topology_transition` implementiert den kontrollierten Hotplug-Mechanismus
`Online → Quiescing → Offline`. Von `Offline` ist eine Reaktivierung nach
`Online` möglich; ein Blatt kann alternativ nach `Removed` wechseln. Während
`Quiescing` oder `Offline` liefert die Device-Abfrage keine nutzbare
IOMMU-Gruppe. Der Selbsttest führt diesen Ablauf mit Device `0xD004` aus und
prüft außerdem die gemeinsame Gruppe von `0xD003` und `0xD004`.

`iommu_bind_device` übernimmt Gruppen-IDs nicht länger ungeprüft vom Aufrufer.
Es fragt das Device im normalisierten Topologiegraphen ab und weist unbekannte
Devices sowie abweichende Gruppennummern zurück. Damit kann ein Aufrufer die
hardwarebedingte Isolationsgrenze nicht durch eine frei gewählte Zahl
verkleinern. Die HAL stellt dabei nur Topologie und Lifecycle-Mechanismen
bereit; Scheduling-, NUMA- und Ressourcenpolitik bleiben in höheren Schichten.

ABI-Layout und Assemblerbau sind automatisiert geprüft. Der UEFI-QEMU-Test
verlangt zusätzlich
`NOVA: HAL Topology ABI 1.0, Busse, Devices und IOMMU-Gruppen aktiv` und läuft
danach weiterhin bis Ring 3, interaktiver Desktop-Szene und geordnetem
`PLATFORM_OFF`.

## 82. Validierte MADT-CPUs in der HAL-Topologie

Die zuvor bereits vorhandene ACPI-Frühphase validiert RSDP, XSDT oder RSDT
und MADT, bevor Daten verwendet werden. Dabei werden Signatur, Tabellenlänge,
Prüfsumme, vollständige Lage in einem freigegebenen Firmware-Speicherbereich,
Entry-Längen und Tabellenbeziehungen geprüft. Aus MADT-Einträgen vom Typ Local
APIC und x2APIC werden nur aktivierte, eindeutige Hardware-IDs übernommen. Die
Liste bleibt auf `CPU_CAPACITY = 8` begrenzt.

`topology_initialize` übernimmt nun diese normalisierte Plattformliste statt
eines fest eingetragenen einzelnen Threads. Für jeden Eintrag entsteht ein
`CPU_THREAD`-Knoten mit APIC-ID als Hardware-ID, stabilem logischem Index und
dem Flag `FIRMWARE_VALIDATED`. Im QEMU-Lauf mit `-smp 4` enthält der Graph
dadurch vier Hardware-Threads und meldet:

`NOVA: HAL Topology, normalisierte CPU-Threads (hex): 0x00000004`

Die MADT beschreibt keine Package-/Core-Zuordnung. Dieser Abschnitt übernimmt
daher ausschließlich Thread-Identitäten; die nachfolgende CPUID-
Normalisierung ergänzt Package und Core aus einer dafür vorgesehenen
Architekturquelle. Die NUMA-Lokalität bleibt ohne validierte SRAT unbekannt
und trägt kein `LOCALITY_KNOWN`-Flag.

Fehlt eine gültige MADT, bleibt genau der BSP als kontrollierter Fallback im
Graphen. Seine APIC-Hardware-ID wird auch ohne RSDP direkt über CPUID Blatt 1
ermittelt. Der Knoten trägt `FALLBACK` statt `FIRMWARE_VALIDATED`; fehlende
Firmwaredaten erscheinen somit nicht als erfolgreich validierte Topologie.

Der Selbsttest gleicht für jeden Hardware-Thread APIC-ID, logischen Index und
Quellenstatus mit der normalisierten Plattformliste ab. Der automatisierte
UEFI-Test verlangt bei vier emulierten CPUs exakt vier Topologieknoten und
erreicht danach weiterhin Desktop, Eingabeverarbeitung und den geordneten
Shutdown.

## 83. CPUID-Package-/Core-/SMT-Hierarchie

Der x86-Topologieprovider bevorzugt CPUID-Blatt `0x1F` und verwendet
`0x0B` als standardisierten Fallback. Er läuft die Subleaves begrenzt ab,
übernimmt nur bekannte Leveltypen und validiert alle Shiftwerte. Eine
Core-Ebene muss vorhanden sein und ihr Shift darf nicht kleiner als der
SMT-Shift sein. Unvollständige oder widersprüchliche Angaben aktivieren keine
scheinbar genaue Hierarchie.

Aus der validierten Bitaufteilung werden für jede MADT-APIC-ID drei Werte
gebildet:

```text
PackageKey = ApicId >> CoreShift
CoreKey    = ApicId >> ThreadShift
ThreadId   = ApicId & ((1 << ThreadShift) - 1)
```

Package- und Core-Keys sind innerhalb des laufenden Systems eindeutig.
Existierende Package-/Core-Knoten werden wiederverwendet, sodass SMT-Threads
desselben Kerns denselben Parent besitzen. Neue Knoten tragen
`ARCH_VALIDATED`; ein Thread trägt dieses Flag zusätzlich zu seinem getrennten
MADT-Quellenstatus. Der Graph bildet damit
`NUMA → Package → Core → HardwareThread` ab, ohne aus einer logischen
CPU-Nummer eine Hardwarebeziehung abzuleiten.

Der Selbsttest prüft die vollständige Parent-Kette jedes Threads, die Anzahl
aller Knoten und das getrennte Package-/Core-/Thread-Accounting. Ist weder
CPUID `0x1F` noch `0x0B` verwendbar, bleiben die Threads direkt unter dem
unbekannten NUMA-Fallback; dieser Zustand wird nicht als validierte
Package-Hierarchie ausgegeben.

Die Topologiekapazität wurde von 32 auf 48 Knoten erweitert. Damit passen zum
aktuellen CPU-Limit von acht Hardware-Threads auch im ungünstigsten Fall je
acht Package- und Core-Knoten neben die vorhandenen Plattform- und
I/O-Knoten. Der UEFI-Test verlangt den zusätzlichen Laufzeitmarker
`NOVA: HAL CPUID Hierarchy, Packages/Core/Threads (hex): 0x` und erreicht
weiterhin Ring 3 und `PLATFORM_OFF`.

## 84. CPU Manager als HAL-Topologie-Consumer

`kernel/include/nova/cpu.h` definiert nun die CPU-ABI 1.0. Der öffentliche
Datensatz ist 112 Byte groß und besitzt getrennte semantische Felder für
CPU-ID, Hardware-ID, Package, Die, Cluster, Core, Hardware-Thread und
NUMA-Knoten. Hinzu kommen Zustand, Kapazität, Features, LLC-ID,
Topologieknoten-ID und Topologiegeneration. Das zugehörige API bleibt 48 Byte
groß und ist durch statische ABI-Prüfungen abgesichert.

Damit wurde ein Fehler des bisherigen internen 96-Byte-Layouts beseitigt:
Package, Die, Cluster, Core, Thread und NUMA benötigen sechs getrennte Felder;
zuvor waren dafür nur fünf Slots vorhanden. Der neue Datensatz enthält ein
eigenes `CoreId` und kann die in `NPSPEC-KERNEL-0026` definierte Topologie ohne
Zusammenlegen semantisch verschiedener IDs darstellen.

`cpu_import_hal_topology` ist jetzt die einzige Topologiequelle des CPU
Managers. Für BSP und alle firmwareerkannten APs sucht der Manager den
normalisierten Hardware-Thread über dessen APIC-Hardware-ID. Anschließend
übernimmt er:

```text
HardwareThread.TopologyId  → CPU.TopologyNodeId
HardwareThread.Generation  → CPU.TopologyGeneration
HardwareThread.ThreadId    → CPU.ThreadId
HardwareThread.Parent      → Core.HardwareId → CPU.CoreId
Core.Parent                → Package.HardwareId → CPU.PackageId
```

Der CPU Manager interpretiert dabei weder ACPI-Einträge noch CPUID-
Topologieblätter erneut. Firmware- und Architekturdetails bleiben hinter dem
HAL-Graphen gekapselt. Die CPU Manager API liefert dadurch dieselbe
normalisierte Sicht wie IOMMU und spätere Scheduler-Consumer.

Nicht vorhandene Informationen werden mit `0xFFFFFFFF` ausgegeben. Das gilt
derzeit für Die, Cluster, LLC und – bis zur SRAT-Implementierung – NUMA. Ein
NUMA-Wert wird nur übernommen, wenn der Topologieknoten ausdrücklich
`LOCALITY_KNOWN` trägt. Der UMA-Fallback wird somit nicht fälschlich als
bestätigte lokale Speicherbeziehung dargestellt.

Der CPU-Selbsttest gleicht jeden Datensatz mit der MADT-Hardware-ID und dem
zugehörigen HAL-Knoten ab. Er prüft Topology-ID, Generation, Thread-ID sowie
bekannte oder unbekannte Package-/Core-Felder. Der UEFI-Test verlangt
zusätzlich die Markierung
`NOVA: CPU Manager bezieht Package, Core und Thread aus HAL Topology` und
läuft danach weiterhin bis Desktop und geordnetem Shutdown.

## 85. ACPI-SRAT und NUMA-Knoten im Hardwaregraphen

Die frühe ACPI-Phase sucht nun zusätzlich nach der System Resource Affinity
Table (SRAT). Wie bei der MADT werden Root-Pointer, RSDT/XSDT, Signatur,
Tabellenlänge, vollständige Lage im Firmware-Speicherbereich und Prüfsumme vor
dem Parsen validiert. SRAT-Einträge besitzen zusätzlich eine strikt begrenzte
Längen- und Endprüfung.

Unterstützt werden derzeit:

- Processor Local APIC/SAPIC Affinity, Typ 0,
- Memory Affinity, Typ 1,
- Processor Local x2APIC Affinity, Typ 2.

Nur aktivierte Einträge werden übernommen. Jede SRAT-CPU muss bereits in der
validierten MADT-Liste vorhanden sein. Eine doppelte CPU-Zuordnung zu
verschiedenen Proximity-Domains, überlappende Speicherbereiche, Null-Längen,
Adressüberläufe oder überschrittene Kapazitäten machen die Tabelle ungültig.
Identische doppelte CPU-Zuordnungen werden deterministisch zusammengeführt.

Die Bootstrapdarstellung ist auf acht CPU- und acht Speicher-Affinitäten
begrenzt. Der aktuelle 32-Bit-Kernel übernimmt nur Speicherbereiche, deren
Basis und Länge vollständig in 32 Bit darstellbar sind. Höhere Bereiche werden
gezählt und kontrolliert übersprungen; sie werden weder abgeschnitten noch als
lokaler Speicher ausgegeben.

Aus jeder verwendeten Proximity-Domain erzeugt die HAL genau einen stabilen
`NUMA_NODE`. CPU-Threads erhalten Domain und `LOCALITY_KNOWN` nur, wenn eine
validierte SRAT-Zuordnung existiert. Speicher-Affinitäten werden als getrennte
`MEMORY_REGION`-Knoten mit physischer Basis, Länge und SRAT-Flags unter ihrem
NUMA-Knoten eingetragen. Fehlt die SRAT, bleibt ein expliziter unbekannter
Fallback-Knoten erhalten.

Package-Knoten hängen am System-Root und nicht an einem einzelnen NUMA-Knoten.
Das verhindert die falsche Annahme `Package == NUMA Domain`, insbesondere bei
Sub-NUMA-Clustering. Die Thread-Datensätze tragen ihre Lokalität unabhängig
von der Package-/Core-Hierarchie. Der CPU Manager übernimmt diese bestätigte
Domain über sein `NumaNodeId`-Feld; ohne `LOCALITY_KNOWN` bleibt es
`0xFFFFFFFF`.

`scripts/test-uefi-numa.ps1` startet QEMU mit zwei Sockets, zwei NUMA-Knoten,
vier CPUs und zwei getrennten 128-MiB-Memory-Backends. OVMF beschreibt dabei
vier CPU- und drei Memory-Affinitäten, weil der niedrige Speicherbereich
separat modelliert wird. Der Test verlangt exakt
`0x00000004/0x00000003`, den CPU-Manager-Import und `NOVA_KERNEL_READY`.
Zusätzlich bleibt der normale UEFI-Test ohne erzwungene NUMA-Konfiguration bis
Desktop, Eingabe und Shutdown erfolgreich.

Noch nicht umgesetzt sind ACPI SLIT für relative Distanzen, 64-Bit-
Speicherregionen und eine systemweite NUMA-Placement-Policy. Die SRAT-Daten
sind jetzt jedoch validiert und als gemeinsame Topologiequelle verfügbar.

## 86. NUMA-bewusster physischer Bootstrap-Speichermanager

Die PMM-ABI wurde kompatibel von 1.0 auf 1.1 erweitert. Ihr bisheriger
32-Byte-Präfix besitzt unveränderte Offsets; vier angehängte Felder vergrößern
die Struktur auf 48 Byte. Neue Capability-Bits kennzeichnen getaggte Frames,
bevorzugte und strikte NUMA-Allokation.

Jede beim Start übernommene physische 4-KiB-Seite erhält in einem parallelen
Metadatenfeld ihre validierte SRAT-Proximity-Domain. Deckt keine bestätigte
Speicher-Affinität die vollständige Seite ab, wird ausdrücklich
`NOVA_PMM_NUMA_UNKNOWN` (`0xFFFFFFFF`) gespeichert. Auch freigegebene Seiten
werden über den normalisierten frühen SRAT-Provider erneut korrekt getaggt.
Der PMM interpretiert dabei keine rohen ACPI-Strukturen; der spätere
Hardwaregraph wird aus derselben bereits validierten Quelle erzeugt.

Die ABI 1.1 stellt drei zusätzliche Operationen bereit:

- `AllocPreferredEntry` versucht zunächst den verlangten Node und fällt bei
  fehlendem lokalen Speicher kontrolliert auf eine beliebige Seite zurück.
- `AllocStrictEntry` liefert nur eine Seite des verlangten Nodes und andernfalls
  null.
- `NodeForPageEntry` ermittelt die validierte Lokalität einer physischen Seite.

Jede erfolgreiche Allokation liefert in `EDX` die tatsächliche Domain. Die
strikte Suche entfernt einen Treffer per Swap-with-last, sodass Frame- und
Node-Array kompakt und synchron bleiben. Der Startselbsttest prüft reguläre
Entnahme/Rückgabe, strikte lokale Auswahl, Ablehnung einer nicht vorhandenen
Domain, Preferred-Fallback und die vollständige Wiederherstellung des freien
Seitenzählers.

Der PMM bleibt bewusst ein 32-Bit-Bootstrap-Allocator mit maximal 1024
verwalteten Frames. Er bietet nun den Mechanismus für lokale Anforderungen,
legt aber noch keine globale Placement-, Migration- oder Distanzpolitik fest.
ABI-Prüfung, normaler UEFI-Desktoptest und der Zwei-Knoten-NUMA-Test laufen mit
dem Marker `NOVA: PMM NUMA ABI 1.1, Preferred und Strict Allocation aktiv`
erfolgreich durch.

## 87. Persistenter Boot-Health-Zustandsautomat

Die inzwischen angenommenen Boot-Health-, A/B- und Rollback-NPSPECs wurden in
den vorhandenen 64-Byte-Boot-Control-Datensatz integriert, ohne dessen Größe
oder CRC-/Redundanzmodell zu ändern. Die früher reservierten acht Bytes tragen
nun getrennte logische Generationen für Slot A und B. Beim Candidate-Staging
wird monoton eine neue Generation vergeben. Vorhandene Datensätze aus dem
älteren Layout werden beim UEFI-Start erkannt, mit Generationen versehen und
über den bestehenden Write-and-read-back-Pfad crash-konsistent migriert.

Der Health-Aggregator unterscheidet `Unknown`, `Pending`, `Healthy`,
`Degraded`, `Failed` und `TimedOut`. Er verarbeitet die semantischen
Milestones `BootloaderStarted`, `KernelEntered`, `KernelInitialized`,
`SystemRootReady`, `CriticalServicesReady`, `Operational` und
`HealthConfirmed`. Milestone-Masken dürfen keine Lücken enthalten und nur
monoton wachsen.

Ein Candidate-Commit ist nur möglich, wenn:

- Slot, logische Generation und Bootversuch exakt zum laufenden Candidate
  gehören,
- die aktive Policy alle Required-Milestones als erreicht bewertet,
- kein blockierender Milestone fehlgeschlagen ist,
- die Bootartefakte als vertrauenswürdig bestätigt wurden,
- der Aufrufer bereits durch die Capability-Grenze autorisiert wurde.

`Degraded` darf nur committen, wenn die aktive Policy dies ausdrücklich
zulässt. `Failed` und `TimedOut` bleiben diagnostizierbar, committen den Slot
nicht und erlauben entsprechend dem Versuchslimit einen weiteren Boot oder den
Rollback. Evidence einer anderen Generation oder eines früheren Attempts,
unautorisierte Meldungen, unbekannter Zustand, nur `KernelEntered` und fehlende
Trust-Bestätigung werden ohne Zustandsänderung abgewiesen.

Der NBHP/BIB-System-TLV übergibt nun die tatsächliche logische Slot-Generation
statt des bisherigen konstanten Nullwerts. Der isolierte C-Test deckt positive
und negative Health-Pfade sowie Generationswechsel ab. Der UEFI-QEMU-Test
bestätigt zusätzlich redundante Variablen, beschädigte neueste Kopie,
vollständig beschädigte Metadaten und Recovery über drei Starts. Der normale
Desktop-/Shutdown-Pfad bleibt erfolgreich.

Während dieser Prüfung wurde außerdem ein HAL-Randfehler behoben: Der
Fallback-NUMA-Provider hatte `EBX` mit dem Unknown-Sentinel überschrieben und
diesen Wert auf Ein-CPU-Systemen als Package-Key weiterverwendet. Die
Package-Bildung verwendet jetzt immer die gespeicherte APIC-ID. Ein zusätzlicher
serieller Initialisierungsmarker und eine Fehlerstufe erleichtern künftige
CPU-Manager-Diagnosen.

Die danach ergänzte capabilitygeschützte Transportbrücke ist in Abschnitt 89
beschrieben. Der Zustandsautomat nimmt weiterhin ausschließlich bereits
autorisierte und vollständig validierte Evidence entgegen.

## 88. Kernel-seitige Boot-Health-Autorität

Mit `kernel/include/nova/boot_health.h` steht nun eine explizite ABI 1.0 für
Provider-Reports, den internen Health-Datensatz, exportierbare Evidence und die
zugehörige API-Tabelle bereit. Statische Assertions sichern die Größen 32, 64,
32 und 40 Byte. Reservierte Felder halten die Strukturen erweiterbar, ohne die
bestehenden Offsets zu verschieben.

Zwei neue Security-Capabilities trennen die Rollen:

- `NOVA_CAP_BOOT_HEALTH_REPORT` erlaubt das Einreichen eines Reports,
- `NOVA_CAP_BOOT_HEALTH_COMMIT` erlaubt den Export der aggregierten Evidence.

Nur PID 1 besitzt diese Rechte. Die bestehende Ring-3-System-UI erhält sie
nicht. Jeder Report muss ABI-Größe und -Version, exakt die 64-Bit-
Systemgeneration, den Bootversuch, einen einzelnen bekannten Provider und eine
für das jeweilige Milestone erlaubte Providerrolle enthalten. Terminale
Fehlerzustände nehmen keine weiteren Reports an. Abgewiesene und autorisierte
Meldungen werden getrennt gezählt.

Der Aggregator verlangt folgende Provider:

- `KernelInitialized`: Kernel Core und Memory,
- `SystemRootReady`: persistentes SystemRoot,
- `CriticalServicesReady`: Trust, Capability und IPC,
- `Operational`: Session.

Milestones schreiten ausschließlich lückenlos fort. Erst nach allen Required-
Providern entstehen `Healthy`, `HealthConfirmed` und eine exportierbare
Evidence. `Degraded` erfüllt in der aktuellen strikten Policy keinen Required-
Provider; `Failed` hält zusätzlich das fehlgeschlagene Milestone fest. Der
Trust-Wert wird getrennt aus dem verifizierten Bootkontext übernommen und kann
nicht durch einen gewöhnlichen Providerreport erfunden werden.

Der Selbsttest kopiert Record und Provider-Masken, prüft eine unautorisierte
PID, unvollständige Aggregation, die vollständige Milestone-Kette und den
capabilitygeschützten Evidence-Export. Danach stellt er den Live-Zustand exakt
wieder her. Im realen Boot meldet der Kernel seine Core-/Memory-Bereitschaft
sowie Capability und IPC. Das Bootstrap-RAMFS gilt absichtlich nicht als
persistentes SystemRoot; ein Trust-Provider wird ebenfalls noch nicht
behauptet. Die serielle Diagnose zeigt diesen wartenden Zustand ausdrücklich.

`scripts/test-uefi-display-server.ps1` verlangt die neuen Health-Marker und
prüft anschließend weiterhin Desktop, Eingabe und `PLATFORM_OFF`. Zusätzlich
erfolgreich sind ABI-Check, der dreistufige persistente Boot-Control-Test und
der Zwei-Knoten-NUMA-Test. Der anschließend implementierte Laufzeittransport
ist in Abschnitt 89 beschrieben.

Beim Abgleich der Identitätsbindung wurde außerdem korrigiert, dass der interne
Kernel-Kontext zuvor nur die unteren 32 Bit der als `uint64_t` spezifizierten
BIB-Systemgeneration kopierte. Ein zusätzliches internes High-Dword bewahrt
die vorhandenen Kontextoffsets und bindet Reports sowie Evidence nun an alle
64 Bit der Generation.

## 89. UEFI-Boot-Health-Runtime-Transport

Der öffentliche NBHP/BIB-Vertrag besitzt jetzt den optionalen TLV-Typ 15
`FIRMWARE_RUNTIME`. Seine 32-Byte-Struktur beschreibt einen x64-UEFI-Provider,
die Capability `PERSIST_BOOT_HEALTH`, eine Kontext- und Einsprungadresse sowie
die maximale Wire-Größe. Unbekannte oder nicht verfügbare Provider bleiben
durch den optionalen TLV abwärtskompatibel.

Der UEFI-Loader reserviert einen dauerhaft erreichbaren 192-Byte-Kontext. Er
enthält die gesicherte Firmware-Seitentabelle, `SetVariable`, Variablenname und
GUID, einen privaten Evidence-Puffer sowie eine minimale temporäre GDT. Alle
für den IA32-Kernel sichtbaren Adressen werden vor Veröffentlichung auf
32-Bit-Darstellbarkeit geprüft. Der Provider selbst wechselt von Protected
Mode in Long Mode, ruft `SetVariable` nach Microsoft-x64-ABI auf und kehrt mit
dem EFI-Status wieder in den 32-Bit-Aufrufer zurück.

Da der Kernel zu diesem Zeitpunkt bereits eigene Seitentabellen und eine
eigene GDT verwendet, sichert der Aufrufpfad CR0, CR3, CR4, EFER, GDTR und
EFLAGS. Er deaktiviert sein Paging für den physischen Provider, lässt diesen
kurz in den Firmware-Runtime-Kontext wechseln und stellt danach Kernel-Paging,
GDT, Codesegment und Interruptzustand wieder her. Dadurch läuft der normale
Kernelpfad nach dem Firmwareaufruf ohne Sonderzustand weiter.

Die Inbox `NovaBootHealth` verwendet einen festen 64-Byte-Wire-Datensatz mit
Magic, Version, Slot, 64-Bit-Generation, Bootversuch, Milestones, Fehlerpunkt,
Health-Status, Trust-Bit, Sequenznummer und CRC32. Der Kernel schreibt nur für
einen echten Candidate mit `BootAttempt > 0`; ein gewöhnlicher Known-Good-Boot
führt keinen NVRAM-Schreibzugriff aus.

Beim nächsten UEFI-Start wird die Inbox vor der Slotwahl konsumiert. Der
Bootloader trennt Transport-, CRC-, Feld- und Zustandsfehler in eindeutige
Diagnosemarker. Slot, Generation, Attempt, Sequenz, monotone Milestones,
Policy, Trust und Commit-Regeln werden erneut geprüft. Erst wenn der redundante
Boot-Control-Datensatz erfolgreich geschrieben und rückgelesen wurde, wird die
Inbox gelöscht. Bei einem Persistenzfehler bleibt sie zur Wiederholung
erhalten; Replay oder fremde/stale Evidence verändert den Zustand nicht.

Der Hosttest prüft Wire-Validierung, Capability, fremde Generationen,
CRC-Fehler, Commit und Replay. Der QEMU-End-to-End-Test staged zusätzlich Slot
B als Candidate, beobachtet den Kernel-Checkpoint in der persistenten Inbox
und verlangt beim Folgestart den Marker
`UEFI:BOOT-HEALTH-EVIDENCE-UPDATED`. Danach laufen weiterhin die redundanten
Korruptions- und Recovery-Szenarien. Der normale Displaytest verlangt außerdem
`UEFI:FIRMWARE-RUNTIME-BRIDGE-READY` und bestätigt, dass Desktop, Eingabe und
geordneter Shutdown unverändert funktionieren.

Ein finaler Candidate-Commit wird derzeit bewusst nicht erzeugt: Dafür fehlen
noch ein echtes persistentes SystemRoot und ein Trust-Provider. Sobald diese
Provider `HealthConfirmed` liefern, kann dieselbe vollständig implementierte
Brücke die finale Evidence ohne weiteres ABI-Redesign persistieren.

## 90. Syscall Discovery, Global State und Transaction ABI

`kernel/include/nova/syscall.h` wurde um feste V1-Strukturen für
Syscall-Feature-Discovery, API-Discovery, semantische API-Deskriptoren und
Operationsergebnisse ergänzt. Damit kann Userspace künftig verfügbare Features,
Providerkandidaten und semantische Verträge abfragen, ohne daraus automatisch
Authority abzuleiten.

`kernel/include/nova/state.h` definiert stabile State-IDs, State-Versionen,
Records und Transitions. State-ID, Version, Generation, Validity und Owner sind
getrennt, damit Unknown-, Stale- und Conflict-Zustände nicht mit gültigem
aktuellen Zustand verwechselt werden.

`kernel/include/nova/transaction.h` definiert stabile Transaction-IDs,
Transaction-Records und Entscheidungen. Der Datensatz trennt Active, Prepared,
Committed und Completed, sodass Commit und Verification nicht vermischt werden.

Der Assemblerkern bindet diese Grundlage über
`kernel/arch/x86_64/state32.inc` ein. Beim Boot werden State- und Transaction-
Bootstrap-Selbsttests nach den Semantic Types und vor dem Service Manager
ausgeführt. Der UEFI-Smoke-Test bestätigt die neuen Marker:

```text
NOVA: Global State ABI 1.0, Versionen und Unknown-State-Pruefung bereit
NOVA: Transaction ABI 1.0, Begin-Prepare-Commit-Verify bereit
```

Geprüft wurden `make abi-check`, `make uefi-image`,
`make test-uefi-display-server` und ein zusätzlicher UEFI-Smoke-Lauf bis
`NOVA_KERNEL_READY`.

## 91. Neuer Boot-Splash und Boot-Konsole nach `newBoot`

Die NPSPECs unter `docs/NPSPEC/Boot/newBoot` definieren den normalen
Boot-Splash als Standardansicht, die Boot-Konsole als Diagnoseansicht und eine
zustandserhaltende Umschaltung zwischen beiden Ansichten. Als visuelle
Referenz dienen `bootSplash.png` und `BootKonsole.png`.

Die Referenzbilder wurden in ein dynamisches Boot-Rendering überführt:

- `boot/image/bootbackground.png` enthält nur den gemeinsamen Space-/Earth-
  Hintergrund für Splash und Boot-Konsole.
- `boot/image/bootlogo.png` enthält das neue Logo für den normalen Splash.
  Es wird als `LOGO.NBS` eingebettet, proportional in seine Zielbox eingepasst
  und mit dem Hintergrund im Mischmodus Screen/Bildschirm verrechnet. Schwarze
  Pixel verändern den Hintergrund dadurch nicht sichtbar.
- Fortschrittsbalken, Console-Panel und Logzeilen werden vom UEFI-Loader live
  darüber gezeichnet.

`scripts/build-bootsplash.ps1` wandelt den Hintergrund in das Nova Boot Splash
Format `NBS1` um. Das Format ist ein kleiner Header plus RGB888-Pixeln und
CRC32-Prüfsumme. Die Skalierung wurde auf Aspect-Fill geändert: Das Bild füllt
die Zielauflösung vollständig, bleibt proportional und wird bei abweichendem
Seitenverhältnis zentriert beschnitten. Das verhindert die früher sichtbare
Kompression beziehungsweise schwarze Balken.

`scripts/build-uefi-image.ps1` kann nun `-BootBackground` und `-BootLogo`
aufnehmen. Das UEFI-Image enthält dadurch `BACKGRND.NBS` und `LOGO.NBS`. Der
Makefile-Target `uefi-image` erzeugt `build/bootbackground.nbs` sowie
`build/bootlogo.nbs` automatisch und packt beide Ressourcen in
`build/nova-uefi.img`.

Im UEFI-Kernel-Loader wurde das Zeichnen von NBS-Bildern verallgemeinert. Die
Funktion rendert den Hintergrund bildschirmfüllend mit bilinearer Abtastung.
Das Logo wird separat skaliert, bilinear abgetastet, seitenverhältnistreu in
die Zielbox eingepasst und per Screen-Blending über den Hintergrund gelegt.
Darüber hinaus zeichnen kleine Primitive Alpha-Rechtecke, Rahmen und
Fortschrittsbalken. Der Standardpfad zeichnet zuerst den Splash und meldet:

```text
UEFI:BOOTSPLASH-READY
UEFI:BOOT-VIEW-SPLASH-READY
```

Während des Splash-Fensters schaltet `F3` auf die Boot-Konsole und erzeugt:

```text
UEFI:BOOT-VIEW-SWITCH-F3
UEFI:BOOT-CONSOLE-READY
```

In der Boot-Konsole bringt `ESC` die Ansicht zurück zum Splash:

```text
UEFI:BOOT-VIEW-SWITCH-ESC
UEFI:BOOT-VIEW-SPLASH-READY
```

Wenn der Kernel nicht geladen oder validiert werden kann, wird keine separate
stille Fehleransicht verwendet; der Loader zeichnet den gemeinsamen Hintergrund
und darauf die dynamische Boot-Konsole mit ERROR-Eintrag. Danach meldet er
`UEFI:BOOT-CONSOLE-ERROR-VIEW`. Damit ist der sichtbare Fehlerpfad an die neue
Boot-Konsole gekoppelt.

Geprüft wurden der vollständige UEFI-Image-Build mit `BACKGRND.NBS` und
`LOGO.NBS` sowie `make test-uefi-display-server`. Der Test bestätigt weiterhin
`UEFI:BOOTSPLASH-READY`, den Kernel-Handoff, die Ring-3-System-UI und den
geordneten Shutdown.

## 94. Semantic Core Lookup und Kernel-Object-Handles

Der frühe Kernel besitzt jetzt eine kleine, statische Semantic-Core-
Implementierung. Sie ist bewusst begrenzt, damit sie bereits im frühen
Bootpfad deterministisch funktioniert.

### ObjectID

Eine ObjectID ist die stabile Identität eines Kernel- oder Systemobjekts. Sie
ist nicht dasselbe wie ein Benutzer-Handle.

### NamespaceID

Eine NamespaceID beschreibt einen benannten Bereich wie `/`, `System`,
`Benutzer`, `Apps`, `Volumes` oder `Boot`. Namespaces geben später vor, wo ein
Objekt sichtbar ist.

### ObjectID-Projektion

Eine Projektion ordnet eine ObjectID einem sichtbaren Namespace-Eintrag zu.
Dadurch kann derselbe stabile Objektkern kontrolliert in einem Namensraum
auftauchen, ohne seine Identität zu verlieren.

### Kernel-Object-Handle

Ein Kernel-Object-Handle ist eine temporäre Referenz auf eine ObjectID mit
Rechten und Generation. Der Kernel kann ein Objekt öffnen, über das Handle
wiederfinden und das Handle schließen. Geschlossene Handles werden als inaktiv
markiert und können wiederverwendet werden.

### Selbsttests im Kernelstart

Der Kernel prüft beim Start:

- Registry-Lookup für ObjectID und CapabilityID
- Namespace-Lookup für `System`
- Projektion von ObjectID nach Namespace
- Ablehnung unbekannter ObjectIDs
- Öffnen, Lookup, Schließen und ungültiges Lookup eines geschlossenen Handles

Bei Erfolg erscheinen im Bootlog:

```text
NOVA: Capability Authority Rechtepruefung bereit
NOVA: Capability Lifecycle Lookup und Revoke bereit
NOVA: ObjectID Registry Lookup bereit
NOVA: Namespace Lookup bereit
NOVA: Namespace Pfadauflösung bereit
NOVA: Namespace Introspection bereit
NOVA: Namespace Enumeration bereit
NOVA: ObjectID Projection Map bereit
NOVA: Projection Introspection bereit
NOVA: Filesystem Object Registry bereit
NOVA: Filesystem Object Enumeration bereit
NOVA: Filesystem Object Projection Konsistenz bereit
NOVA: Filesystem Object Pfadauflösung bereit
NOVA: Kernel Object Handle ABI bereit
NOVA: Handle Object-Registry Bindung bereit
NOVA: Handle Rechtevalidierung gegen Capabilities bereit
NOVA: Namespace Pfad zu Handle bereit
```

### Capability Authority

Die Capability Authority ist die erste Kernel-interne Rechteentscheidung vor
dem Erzeugen eines Object-Handles. Sie enthält im frühen Boot feste Einträge
für System-, Apps- und Boot-Objekte. Ein Handle wird nur geöffnet, wenn eine
aktive Capability die angeforderten Rechte vollständig abdeckt.

Der Selbsttest prüft dadurch zwei Fälle:

- Lesen auf dem Systemobjekt ist erlaubt.
- Schreiben auf demselben Objekt wird abgewiesen, weil dafür keine Capability
  existiert.

### Capability Lifecycle

Capabilities können jetzt über ihre CapabilityID gefunden und gezielt widerrufen
werden. Ein widerrufener Eintrag bleibt als Datensatz vorhanden, verliert aber
sein Active-Flag. Lookup und Rechteprüfung ignorieren ihn danach.

Der Kernel-Selbsttest nutzt eine frühe Volumes-Test-Capability:

1. Capability per ID finden.
2. Leserecht auf das Volumes-Objekt bestätigen.
3. Capability widerrufen.
4. Nachweisen, dass Lookup und Rechteprüfung sie nicht mehr akzeptieren.

### Handle-Rechtevalidierung

Ein geöffnetes Handle wird nicht mehr nur beim Erzeugen geprüft. Der Kernel kann
jetzt ein Handle für eine konkrete Operation erneut validieren:

1. Handle muss aktiv sein.
2. Handle muss die angeforderten Rechte enthalten.
3. Für die referenzierte ObjectID muss weiterhin eine aktive Capability mit
   diesen Rechten existieren.

Dadurch entzieht ein Capability-Revoke auch bereits geöffneten Handles ihre
Nutzbarkeit. Der Selbsttest öffnet ein Volumes-Handle, validiert es, widerruft
die Capability und erwartet danach, dass dieselbe Handle-Validierung fehlschlägt.

### Namespace-Pfadauflösung

Der frühe Namespace-Core kann jetzt absolute Bootstrap-Pfade auflösen. In dieser
ersten Stufe sind bewusst nur Root und ein Segment unter Root erlaubt:

- `/`
- `/System`
- `/Benutzer`
- `/Apps`
- `/Volumes`
- `/Boot`

Der Resolver gibt den Namespace-Datensatz zurück. Unbekannte Pfade werden
abgelehnt. Das reicht für die nächsten Service- und Userspace-Anbindungen, ohne
schon ein vollständiges VFS vorzutäuschen.

### Namespace-Pfad zu Object-Handle

`semantic_core_handle_open_by_path` verbindet den Namespace-Core mit dem
Handle- und Capability-Pfad:

1. absoluten Pfad auflösen,
2. ObjectID aus dem Namespace-Eintrag übernehmen,
3. Capability-Rechte prüfen,
4. Kernel-Object-Handle erzeugen.

Der Kernel-Selbsttest öffnet `/System` mit Leserecht, validiert das erzeugte
Handle und schließt es wieder.

### Namespace-Introspection

`semantic_core_namespace_count_children` zählt direkte Kinder eines Namespace-
Knotens. Optional kann die Zählung auf Einträge mit bestimmten Flags begrenzt
werden, zum Beispiel nur benutzersichtbare oder nur System-Namespace-Einträge.

Der Kernel-Selbsttest prüft am Root-Namespace:

- 5 direkte Kinder insgesamt,
- 3 benutzersichtbare Kinder,
- 2 System-Kinder.

### Namespace-Enumeration

`semantic_core_namespace_child_at` gibt ein direktes Kind eines Namespace nach
Ordinal zurück. Wie bei der Zählung kann optional nach Flags gefiltert werden.
Damit kann ein Dienst später schrittweise Listen aufbauen, ohne die internen
Namespace-Tabellen direkt zu kennen.

Der Kernel-Selbsttest prüft:

- erstes benutzersichtbares Root-Kind,
- drittes benutzersichtbares Root-Kind,
- Out-of-range-Ablehnung,
- letztes ungefiltertes Root-Kind.

### Projection-Introspection

Die ObjectID-Projektionen können jetzt pro Namespace gezählt und nach Ordinal
abgerufen werden. Das erlaubt späteren Diensten, die sichtbaren Objektprojektionen
eines Namespace zu inspizieren, ohne die interne Projection-Tabelle direkt zu
kennen.

Der Kernel-Selbsttest prüft:

- Anzahl der Projektionen in `System`,
- Anzahl der Projektionen in Root,
- erste Projektion in `Apps`,
- Out-of-range-Ablehnung in `Apps`.

### Filesystem Object Registry

Die Bootstrap-Namespace-Einträge werden jetzt zusätzlich als echte
Filesystem-Objekte geführt. Jeder Datensatz enthält:

- ObjectID,
- Namespace-Slot,
- stabilen Namen,
- Flags wie `Stable`, `System`, `UserVisible` und `Namespace`,
- semantische Zuordnung zum passenden NamespaceID-Datensatz.

Der Kernel-Selbsttest prüft den Lookup von `Apps`, die Flagzählung für System-
und benutzersichtbare Objekte sowie die Ablehnung einer unbekannten ObjectID.

### Filesystem Object Enumeration

`semantic_core_object_at_by_flags` liest ein Objekt nach Ordinal innerhalb eines
Flag-Filters. Dadurch können spätere Dienste sichtbare oder systeminterne
Objektlisten aufbauen, ohne die Registry-Tabelle direkt zu kennen.

Der Kernel-Selbsttest prüft:

- erstes benutzersichtbares Objekt,
- drittes benutzersichtbares Objekt,
- drittes Systemobjekt,
- Out-of-range-Ablehnung.

### Object-Projection-Konsistenz

`semantic_core_object_validate_projection` verbindet die Object Registry mit der
Projection Map. Die Prüfung stellt sicher, dass:

- die ObjectID als Objekt existiert,
- eine Projection für diese ObjectID existiert,
- der Namespace-Slot der Projection zum Object-Datensatz passt,
- das semantische Ziel der Projection zum Object-Datensatz passt.

Der Kernel-Selbsttest bestätigt die gültige `Apps`-Projektion und lehnt eine
unbekannte ObjectID ab.

### Handle-Bindung an Object Registry

`semantic_core_handle_open_by_object` prüft jetzt vor der Capability-Prüfung,
ob die ObjectID in Object Registry und Projection Map konsistent bekannt ist.
Ein Handle kann dadurch nicht mehr allein durch eine zufällig passende
Projection oder Capability entstehen.

Der Kernel-Selbsttest lehnt einen Handle-Open-Versuch für eine unbekannte
ObjectID ab.

### Filesystem Object Pfadauflösung

`semantic_core_object_resolve_path` löst einen absoluten Namespace-Pfad zu
einem Filesystem-Object-Datensatz auf:

1. Pfad über den Namespace-Core auflösen,
2. ObjectID aus dem Namespace-Datensatz übernehmen,
3. Object-/Projection-Konsistenz prüfen,
4. Object-Datensatz zurückgeben.

Der Kernel-Selbsttest prüft `/System`, `/Apps` und einen fehlenden Pfad.

## 95. Storage-Bootstrap: PCI, AHCI, Block-Devices und GPT

`kernel/arch/x86_64/storage32.inc` stellt dem Kernel erstmals echten
Datenträgerzugriff bereit (NPSPEC-STORAGE-DEVICE-0001,
NPSPEC-STORAGE-DISCOVERY-0001). Der Aufruf erfolgt in der Bootphase
Device Discovery direkt nach dem Device Manager.

### PCI und Controller

- PCI-Konfigurationszugriff über Mechanismus #1 (`0xCF8`/`0xCFC`), Scan aller
  256 Busse mit Multifunction-Erkennung.
- Der erste Controller mit Klasse `01/06/01` (AHCI 1.0) wird verwendet.
- BAR5 (ABAR) muss ein 32-Bit-erreichbares Memory-BAR sein; ein 64-Bit-BAR mit
  gesetztem High-DWORD wird abgewiesen.
- Memory Space und Bus Master werden aktiviert, INTx abgeschaltet.
- Die ABAR-Seiten werden identisch und ungecacht (`PCD|PWT`) abgebildet.
- BIOS/OS-Handoff (falls `CAP2.BOH`), HBA-Reset, danach AHCI-Modus ohne
  Interrupts.

### Ports und Datenträger

- Alle implementierten Ports erhalten `SUD|POD`; ein gemeinsames
  100-ms-Linkfenster endet vorzeitig, sobald alle Ports `DET=3` melden.
- Pro Port liegen Kommandoliste (1 KiB), Received FIS (256 Byte) und
  Kommandotabelle in einer genullten PMM-Seite unterhalb von 8 MiB.
- Nur Ports mit ATA-Signatur `0x00000101` werden verwendet.
- `IDENTIFY DEVICE` verlangt 48-Bit-LBA und 512-Byte-Sektoren.
- Die stabile **DeviceID** ist `CRC32C(Seriennummer ‖ Modell)` und damit
  unabhängig von Port, Bus oder Mountpoint.
- Controller (`DEVICE_CLASS 4`) und Datenträger (`DEVICE_CLASS 5`) werden im
  Device Manager registriert. Discovery erzeugt keine Authority.

### Block-I/O

`storage_read`, `storage_write` und `storage_flush` arbeiten synchron im
Polling-Betrieb mit genau einem Kommando in Slot 0:

- `READ DMA EXT` / `WRITE DMA EXT` mit 1–8 Sektoren in eine
  identitätsabgebildete Seite,
- `FLUSH CACHE EXT` als Ordnungspunkt für NovaFS,
- Bereichsprüfung gegen die Kapazität vor jeder Übertragung,
- Timeout über die 100-Hz-Kernelzeit plus Spin-Grenze,
- Task-File-Fehler (`PxIS.TFES`, `ERR`, `BSY`, `DRQ`) gelten nie als Erfolg;
  der Port wird danach kontrolliert neu gestartet.

Der Bootstrap-Treiber nutzt noch nicht die DMA-Mapping-/IOMMU-Schicht; diese
Anbindung folgt mit dem HAL-Storage-Provider.

### GPT

`storage_find_novafs_partition` prüft GPT-Signatur, Header-CRC32, Eintragsgröße,
Eintrags-CRC32 (fortlaufend über alle Einträge) und Partitionsgrenzen. Gesucht
wird die NovaFS-Typ-GUID `4E4F5641-4653-5359-5354-454D30303031`. Die Typ-GUID
kennzeichnet nur den Inhalt; Identität ist die VolumeID im Superblock.

### Prüfungen

- CRC32C- und CRC32-Prüfwerte für `123456789` beim Start,
- DMA-Lesetest von LBA 0 und Ablehnung eines Zugriffs jenseits der Kapazität,
- ohne AHCI-Controller (z. B. i440fx) meldet der Kernel
  `kein AHCI-Controller gefunden` und bootet mit dem Bootstrap-RAMFS weiter,
- mit zwei Datenträgern wird das Systemvolume auch auf Port 1 gefunden.

## 96. NovaFS 1.0 Phase 1 im Kernel

`kernel/arch/x86_64/novafs32.inc` implementiert NovaFS Phase 1 gemäß
NPSPEC-NOVAFS-0001 §60 und dem neuen Byte-Layout
`docs/NPSPEC/NPSPEC-NOVAFS-ONDISK-0001.md`.

### On-Disk-Format

- 4096-Byte-Blöcke, Little Endian, CRC32C für alle Metadaten,
- primärer Superblock in Block 1, Backup im letzten Block,
- Free-Space-Bitmap ab Block 2 mit eigener CRC32C im Superblock,
- Object Tree (152-Byte-Items = `novafs_object_record_t`), Directory Tree
  (288-Byte-Items, Schlüssel `(parent_id, CRC32C(name))`) und Extent Tree
  (72-Byte-Items, Schlüssel `(object_id, logical_offset)`),
- Copy-on-Write-fähige Knotenstruktur nach §12; Phase 1 schreibt an Ort und
  Stelle und erzeugt höchstens Baumhöhe 2.

Die C-Strukturen mit statischen Offset-Prüfungen liegen in
`kernel/include/nova/novafs.h` und sind Teil von `make abi-check`.

### Mount

1. NovaFS-Partition per GPT suchen (alle Datenträger),
2. Primär- und Backup-Superblock vollständig validieren, höchste Generation
   wählen,
3. unbekannte `incompat_flags` → kein Mount; unbekannte
   `readonly_compat_flags` oder `state ≠ CLEAN` → nur Read-only,
4. Bitmap laden und gegen `bitmap_crc32c` prüfen,
5. alle Baumwurzeln laden und Root-Objekt 1 prüfen,
6. Read-Write: `mount_count` in einer eigenen Generation fortschreiben.

Fehler führen fail-closed zu keinem Mount; das Bootstrap-RAMFS bleibt Root.

### Operationen

| Funktion | Bedeutung |
|---|---|
| `novafs_resolve_path` | absoluter Pfad → ObjectID und Typ |
| `novafs_lookup` | Name in einem Verzeichnis (Hash + Namensvergleich) |
| `novafs_create` | Datei/Verzeichnis anlegen, ObjectID nie wiederverwendet |
| `novafs_read` | Lesen mit Löchern als Nullen, gekürzt auf die Dateigröße |
| `novafs_write` | Schreiben mit Read-Modify-Write für Teilblöcke, Extent-Verlängerung |
| `novafs_change_begin/commit` | Phase-1-Schreibprotokoll |

Das Schreibprotokoll lautet: Superblock `DIRTY` → Daten → Baumknoten → Bitmap
→ `FLUSH` → Generation + 1 und `CLEAN` → primärer Superblock → `FLUSH` →
Backup-Superblock → `FLUSH`. Bricht ein Vorgang ab, bleibt `DIRTY` sichtbar
und der nächste Start mountet nur Read-only.

Beim Einfügen wird ein volles Blatt hälftig geteilt; eine Blattwurzel wird
dabei zur inneren Wurzel. Zusätzlich angeforderte Blöcke werden bei Fehlern
vor dem Schreiben wieder freigegeben.

### Systemintegration

- Die stabilen Wurzel-Namespaces `/System`, `/Benutzer`, `/Apps`, `/Volumes`
  und `/Boot` müssen die ObjectIDs 2–6 tragen, also dieselben wie
  `OBJI:2` … `OBJI:6` im Semantic Core.
- Das Volume wird mit der VolumeID aus der `filesystem_uuid` als gemountetes
  NovaFS-Volume (`NOVA_VOLUME_FS_NOVAFS`) am Root-Namespace registriert.
- Der VFS-Zustand wechselt auf `VFS_FLAG_NOVAFS_ROOT` (bei Read-only
  zusätzlich `VFS_FLAG_READ_ONLY`).
- Boot Health erhält `SystemRootReady` vom Provider `SystemRoot`; ein
  Read-only-Mount meldet `Degraded`. Der Gesamtzustand bleibt bis zum
  Trust-Provider korrekt `Pending`.

### Selbsttest bei jedem Read-Write-Start

- `/System/Diagnose/novafs-bootcount` wird gelesen, erhöht, geschrieben und
  zurückgelesen (persistent über Neustarts),
- `/System/Diagnose/novafs-muster.bin` (3 × 4096 + 100 Byte) wird mit einem
  vom Zähler abhängigen Muster überschrieben, danach folgt ein 16-Byte-
  Schreibzugriff über eine Blockgrenze; alles wird zurückgelesen und
  verglichen.

```text
NOVA: NovaFS 1.0 Systemvolume gemountet, Generation 0x... VolumeID 0x...
NOVA: NovaFS Root-Layout konsistent mit Semantic-Core-ObjectIDs
NOVA: NovaFS persistenter Bootzaehler 0x...
NOVA: NovaFS Lese-/Schreibtest mit Extents, Teilbloecken und Blockgrenze bereit
NOVA: NovaFS ist persistentes SystemRoot unter /
NOVA: Boot Health SystemRoot bereit, wartet auf Trust
```

### Host-Werkzeug und Build

`tools/novafs/novafs.c` ist eine unabhängige Referenzimplementierung desselben
Formats: `mkfs`, `info`, `ls`, `cat`, `mkdir`, `put`, `fsck`, `tree` und die
Testhilfe `mark-dirty`. `fsck` prüft alle Prüfsummen, Sortierung und
Schlüsselgrenzen der Bäume, Verzeichnis-/Objekt-Referenzen, Extent-
Überlappungen und die exakte Übereinstimmung von Bitmap und tatsächlicher
Belegung.

`build-uefi-image.ps1 -NovaFsImage` legt hinter der unveränderten ESP eine
zweite GPT-Partition „NovaOS System“ an. Eine bereits beschriebene Partition
im bestehenden `build/nova-uefi.img` wird übernommen, damit Kernel-Neubauten
keine Daten verwerfen; `-ResetNovaFs` bzw. `make novafs-reset` formatiert neu.

| Ziel | Wirkung |
|---|---|
| `make novafs-tool` | Host-Werkzeug `build/novafs.exe` |
| `make novafs-image` | Vorlage `build/novafs-system.img` (nur falls fehlend) |
| `make uefi-image` | UEFI-Image inkl. Systemvolume (bestehendes bleibt erhalten) |
| `make novafs-reset` | Systemvolume neu formatieren |
| `make novafs-fsck` | `info` + `fsck` des Volumes in `build/nova-uefi.img` |
| `make novafs-check` | Host-Selbsttest des Werkzeugs |
| `make test-uefi-novafs` | QEMU-End-to-End-Test |

### Nachweis

`make test-uefi-novafs` arbeitet auf temporären Kopien und prüft in 12 Starts:

- frisches Volume über zwei Starts (Bootzähler 1 → 2, Musterdatei 12388 Byte),
- vom Kernel ausgeführte Wurzel- und Blatt-Splits aller drei Bäume
  (Vorbelegungen 7, 13, 17, 30, 54 und 82 Dateien) mit anschließendem
  Host-`fsck`,
- beschädigten primären Superblock: Backup wird verwendet und der Primär-
  Superblock beim nächsten Commit repariert,
- `DIRTY`-Volume: Read-only-Mount, kein Schreibzugriff, Boot Health degradiert,
- unformatierte Partition: kein Mount, Boot läuft mit Bootstrap-RAMFS weiter.

Der bestehende Displaytest, der NUMA-Test, der Boot-Control-Test und der
UEFI-Kernelnegativtest bleiben erfolgreich.

### Grenzen der Phase 1

- Baumhöhe ≤ 2, Kernel-Bitmap ≤ 4 Blöcke (Volume ≤ 512 MiB), Dateien < 4 GiB,
- keine Nutzdatenprüfsummen, keine Zeitstempel, kein Löschen im Kernel,
- keine Copy-on-Write-Transaktionen und kein Transaction Log (Phase 2),
- kein feingranulares Locking (Syscalls laufen exklusiv, siehe Abschnitt 97),
- Schutzrichtlinie aller Dateien ist das Profil `Unprotected` (Policy-ID 0).

## 97. VFS-Syscalls auf NovaFS (VFS ABI 1.1)

`kernel/arch/x86_64/vfs32.inc` macht NovaFS für Ring 3 nutzbar. Die Syscalls
folgen dem bestehenden `int 0x80`-Vertrag (Service 6 = VFS, versionierte
Argumentstrukturen mit expliziter Größe) und sind in
`kernel/include/nova/syscall.h` als C-ABI mit statischen Größenprüfungen
beschrieben.

| Op | Name | Argumente | Recht am Handle | Capability |
|---:|---|---|---|---|
| 1 | `OpenRoot` | unverändert | – | `SERVICE` |
| 2 | `Lookup` | `NovaVfsLookupArgumentsV1` (Pfad absolut oder relativ, 1–255 Byte, Flag `WRITE`) | `QUERY` am Startverzeichnis | `FS_READ` (+ `FS_WRITE` bei Flag) |
| 3 | `Read` | `NovaVfsIoArgumentsV1` (≤ 4096 Byte) | `READ` | `FS_READ` |
| 4 | `Write` | `NovaVfsIoArgumentsV1` (≤ 4096 Byte) | `WRITE` | `FS_WRITE` |
| 5 | `Create` | `NovaVfsCreateArgumentsV1` (Datei/Verzeichnis) | `WRITE` am Verzeichnis | `FS_WRITE` |
| 6 | `ReadDirectory` | `NovaVfsDirectoryArgumentsV1` → `NovaVfsDirectoryEntryV1` (288 Byte) | `READ` | `FS_READ` |
| 7 | `Query` | `NovaVfsObjectInfoV1` (ObjectID, Typ, Größe, Generation, Rechte) | `QUERY` | – |

### Sicherheitsmodell

```text
Path -> Resolve -> ObjectID -> Capability + Policy -> Handle(Rechte) -> Operation
```

- Pfad- oder ObjectID-Kenntnis allein erzeugt keinen Zugriff; jede Operation
  verlangt ein prozesslokales, generationsgeschütztes Handle mit dem nötigen
  Recht (`HANDLE_RIGHT_READ 0x40`, `HANDLE_RIGHT_WRITE 0x80`) und die
  Prozess-Capability (`SECURITY_CAP_FS_READ`, `SECURITY_CAP_FS_WRITE`).
- Ein geschlossenes Handle ist durch die Generation sofort ungültig.
- Schreib-Authority für Systembereiche ist getrennt
  (`SECURITY_CAP_FS_SYSTEM_WRITE`, NPSPEC-POLICY-SYSTEMWRITE-0001 Nr. 1, 4, 6).
  Phase-1-Policy: ohne diese Authority ist Schreiben nur unterhalb von
  `/Benutzer` erlaubt; `/`, `/System`, `/Boot`, `/Apps`, `/Volumes` und
  `/Solutions` bleiben geschützt. Die Zugehörigkeit wird über die stabile
  Parent-Kette der ObjectIDs bestimmt, nicht über den Pfad.
- Capability, Mountzustand und Policy werden bei jedem `Write` und `Create`
  erneut geprüft, nicht nur beim Öffnen (Revalidierung vor Commit).
- Der System-UI-Prozess erhält `FS_READ | FS_WRITE`, aber keine
  System-Write-Authority.

### Konsistenz

- Syscalls laufen über ein Interrupt-Gate und damit exklusiv; `nfs_busy`
  verhindert zusätzlich Wiedereintritt.
- Jede schreibende Operation bildet eine eigene NovaFS-Änderung
  (DIRTY → Änderung → CLEAN). Scheitert sie, bevor etwas geschrieben wurde,
  wird sauber abgeschlossen. Scheitert sie nach einer Teiländerung, bleibt das
  Volume `DIRTY` und wird bis zum Neustart read-only (kein Rollback in
  Phase 1).
- Der Kernel kopiert Benutzerdaten erst nach vollständiger Bereichsprüfung in
  eine eigene Bounce-Seite.

Das Shared Service Page meldet `NOVA_SYSCALL_FEATURE_FILESYSTEM (0x100)` bei
gemountetem Systemvolume und zusätzlich `..._FILESYSTEM_WRITABLE (0x200)`,
wenn es beschreibbar ist.

### Ring-3-Nachweis

Das Bootstrap-Programm prüft bei jedem Start aus Ring 3:

1. `/System/Diagnose/novafs-bootcount` lesend öffnen, `Query` (Typ, 27 Byte,
   kein Schreibrecht) und `Read` des Inhalts,
2. `Write` ohne Schreibrecht → `ACCESS_DENIED`, Zugriff über das geschlossene
   Handle → `ACCESS_DENIED`,
3. Schreib-Lookup auf eine Systemdatei → Policy-Ablehnung,
4. `/Benutzer` schreibbar öffnen, `Willkommen.txt` relativ suchen oder anlegen,
   doppelte Anlage → `ALREADY_EXISTS`,
5. Text schreiben, zurücklesen und vergleichen,
6. `/Benutzer` per `ReadDirectory` durchlaufen und die Datei mit korrekter
   Größe finden, alle Handles schließen.

Ist das Volume read-only (z. B. `DIRTY`), läuft nur der Leseteil; der Desktop
startet in jedem Fall. `make test-uefi-novafs` prüft die Datei anschließend vom
Host aus (`/Benutzer/Matthias/Dokumente/Willkommen.txt` = „Willkommen bei NovaOS.“; siehe Abschnitt 98).

```text
NOVA: Userspace VFS.Lookup auf NovaFS erfolgreich
NOVA: Userspace VFS.Query erfolgreich
NOVA: Userspace VFS.Read erfolgreich
NOVA: VFS Schreibzugriff ausserhalb /Benutzer ohne System-Write-Authority abgewiesen
NOVA: Userspace VFS.Create erfolgreich
NOVA: Userspace VFS.Write erfolgreich
NOVA: Userspace VFS.ReadDirectory erfolgreich
```

### Grenzen

- Benutzerpuffer liegen im Bootstrap-Prozess auf seiner einzigen Stackseite;
  ein allgemeiner Userspace-Speicher folgt mit dem Prozessmodell,
- höchstens 16 Handles systemweit (bestehende Handle-Tabelle),
- kein Ändern von Rechten (Delete/Rename siehe Abschnitt 99),
- Policy ist eine feste Phase-1-Regel; deklarative Policies und
  objektbezogene Capabilities aus dem Semantic Core folgen.

## 98. Explorer-Ansicht aus NovaFS (Display-Operation 4)

### Benutzerprofil im Image

`novafs mkfs` legt standardmäßig `/Benutzer/Matthias` mit den Unterordnern
`Desktop`, `Dokumente`, `Downloads`, `Bilder`, `Musik` und `Videos` an.
`--user NAME` wählt den Namen, `--no-user` lässt das Profil weg (genutzt von
den Split-Szenarien in `test-uefi-novafs.ps1`, damit die Vorbelegung exakt
bleibt). Im Makefile steuert `NOVAFS_USER` den Namen.

### Ablauf in Ring 3

1. `ufs_select_home` öffnet `/Benutzer`, nimmt den ersten Unterordner und
   öffnet relativ `<name>/Dokumente`. Breadcrumb `Benutzer  /  <name>  /
   Dokumente`, Highlight-Index 3. Ohne Profil fällt es auf `/Benutzer` zurück.
2. Ist das Volume schreibbar, wird `Willkommen.txt` (24 Bytes) dort angelegt
   bzw. gefunden, geschrieben, zurückgelesen und per `ReadDirectory` gesucht.
3. `ufs_build_view` liest alle Einträge, übernimmt höchstens 4 Ordner und
   4 Dateien und trägt die Gesamtzahlen ein.
4. `Display.SubmitExplorerView` (Service 12, Operation 4) mit 480 Bytes.
   `-8 SERVICE` (kein Display) wird toleriert.

### ABI `NovaExplorerViewV1` (480 Bytes, `syscall.h`)

| Offset | Feld | Regel |
|---|---|---|
| 0 | Size | 480 |
| 4 | Version | 1 |
| 8 | Generation (u64) | > 0 |
| 16 | EntryCount | ≤ 8 |
| 20 | TotalEntries | = TotalDirectories + TotalFiles |
| 24 | Flags | Bits 0–2: Schnellzugriff-Zeile 1..7 |
| 28 | PathLength | ≤ 56 |
| 32 | TotalDirectories | |
| 36 | TotalFiles | |
| 40 | Path[56] | Breadcrumb-Text |
| 96 | Entries[8] × 48 | Type, Size, NameLength (1..32), Reserved, Name[32] |

Der Kernel (`explorer32.inc`) kopiert die Ansicht, prüft alle Felder und
ersetzt Bytes außerhalb 0x20..0x7E durch `-`, weil die Bootschrift nur einen
ASCII-Teilsatz plus Umlaute enthält.

### Darstellung

- Ordner erscheinen als Kacheln (wie bisher), Dateien als Zeilen mit den
  Spalten Name / Typ / Größe / Geändert (45/65/85 % der Breite),
- Typ nach Endung: `.txt` Textdatei, `.bin` Binärdatei, `.png` PNG-Bild,
  `.md` Markdown, sonst Datei; Größe in B/KB/MB; Datum vorerst `-`,
- Fußzeile `N Elemente  D Ordner  F Dateien  NovaFS: X MB frei`,
- leere Liste: „Keine Dateien in diesem Ordner“.

```text
NOVA: Explorer zeigt NovaFS-Verzeichnis aus Ring 3, Eintraege 0x...
```

### Grenzen

- Navigation siehe Abschnitt 99,
- keine Zeitstempel in der Anzeige,
- Namen außerhalb ASCII werden als `-` dargestellt.

## 99. VFS.Delete, VFS.Rename und Explorer-Navigation

### NovaFS-Kernel (`novafs32.inc`)

- `novafs_cursor_remove` entfernt das Item an der Cursorposition und schreibt
  das Blatt. Blätter werden nicht zusammengelegt; leere Blätter bleiben
  bestehen, die inneren Schlüssel bleiben gültige Untergrenzen
  (NPSPEC-NOVAFS-ONDISK-0001 §5, §7a).
- `novafs_delete` (EAX Eltern, ESI/ECX Name): ObjectID < 256 → `PROTECTED`,
  Verzeichnis mit Einträgen → `NOT_EMPTY`. Für jedes Extent wird erst das Item
  entfernt (Knoten geschrieben), danach werden die Blöcke in der Bitmap
  freigegeben. Scheitert das Schreiben eines Knotens, bleibt die Bitmap
  unverändert, sodass ein sauberer Abschluss nie freie, aber referenzierte
  Blöcke erzeugt. Danach folgen Objekt-Item, Verzeichniseintrag und
  `object_count − 1`.
- `novafs_rename` (Parameter in `nfs_mv_*`): Zielname nach §5 prüfen,
  Zyklusschutz über die `parent_id`-Kette (≤ 64 Ebenen), vorhandenes Ziel →
  `EXISTS` (gleiches Objekt → nichts zu tun), neuen Eintrag einfügen, alten
  entfernen, bei anderem Elternverzeichnis `parent_id` aktualisieren.
- `novafs_name_check` ist aus `novafs_create` herausgelöst.

### VFS-Operationen (ABI 1.2)

| Op | Struktur | Felder |
|---:|---|---|
| 8 Delete | `NovaVfsDeleteArgumentsV1` (32 B) | +8 DirectoryHandle (WRITE), +12 NameAddress, +16 NameLength, +20 Flags = 0, +24/+28 reserviert |
| 9 Rename | `NovaVfsRenameArgumentsV1` (40 B) | +8 SourceDirectoryHandle, +12/+16 Quellname, +20 TargetDirectoryHandle, +24/+28 Zielname, +32 Flags = 0, +36 reserviert |

Status: `NOT_FOUND`, `EXISTS`, `ACCESS_DENIED` (fehlendes Recht, Policy oder
stabiler Namespace), `VALIDATION_FAILED` (ungültiger Zielname, Zyklus),
`DIRECTORY_NOT_EMPTY` (−25, neu), `READ_ONLY`, `IO_ERROR`. Beide Operationen
prüfen Capability und `/Benutzer`-Policy für Quelle und Ziel bei jedem Aufruf.
Offene Handles auf gelöschte Objekte bleiben gültige Handles, jede Operation
darauf liefert `NOT_FOUND`.

```text
NOVA: Userspace VFS.Rename erfolgreich
NOVA: Userspace VFS.Delete erfolgreich
```

### Host-Werkzeug

`novafs rm <image> <pfad>` und `novafs mv <image> <alt> <neu>` folgen derselben
Reihenfolge. `fsck` prüft zusätzlich, dass `parent_id` jedes Objekts zu seinem
Verzeichniseintrag passt. `scripts/test-novafs-tool.sh` benennt um, verschiebt,
prüft die Fehlerfälle, löscht 120 Dateien bis auf leere Blätter, kontrolliert
die freigegebenen Blöcke (147 für eine 600-KB-Datei) und befüllt den Baum neu.

### Explorer-Navigation

Fokus-Elemente im Explorer:

| Element | Bedeutung | Maus-Trefferbereich (relativ zum Fenster) |
|---:|---|---|
| 20–23 | Dateizeile | y 324 + n·24 |
| 24–27 | Ordnerkarte | x 202 + n·142 (Breite 132), y 178–233 |
| 28 | Zurück | x 16–109, y 56–93 |
| 30–36 | Schnellzugriff | x 12–163, y 160 + k·24 |

- Neue Eingabeaktion `NOVA_SYSTEM_INPUT_NAVIGATE_BACK` (10) für Backspace
  (Set 1 0x0E, Set 2 0x66).
- Ring 3 hält `UFS_H_DIR` offen und den absoluten Pfad in `UFS_CWD`. Ein
  Ordnerwechsel baut den Kandidatenpfad, öffnet ihn lesend relativ zum
  Root-Handle und ersetzt erst bei Erfolg Handle und Pfad; sonst bleibt die
  bisherige Ansicht. Nach oben endet bei `/Benutzer`.
- Breadcrumb und Schnellzugriff-Markierung entstehen aus `UFS_CWD`
  (zu lange Pfade: `..` plus Ende des Pfads).
- Die Explorer-Meldung enthält jetzt den Pfad:
  `NOVA: Explorer zeigt NovaFS-Verzeichnis aus Ring 3, Eintraege 0x…, Pfad Benutzer  /  Matthias  /  Dokumente`.
- `test-uefi-display-server.ps1` fährt Backspace → Enter → Backspace und
  prüft die Pfade.
- Die unsichtbaren Pfeile `<`/`>` (fehlen in der Bootschrift) sind durch
  „Zurück“ ersetzt; damit überlappt die Navigation den Breadcrumb nicht mehr.

### Grenzen

- Bootstrap-Code belegt jetzt etwa 7,4 von 8 KiB der beiden Codeseiten.
- Ein offenes Handle verhindert das Löschen nicht (keine Referenzzählung auf
  NovaFS-Objekten).
- Kein Zusammenlegen leerer Blätter; die Baumhöhe sinkt nicht.

## 100. Stable Root Namespace `/Solutions`

Die neue `NPSPEC-NOVAFS-ONDISK-0001` konkretisiert das NovaFS-Rootlayout für
Phase 1 und ergänzt `/Solutions` als stabilen Rootbereich:

```text
7  /Solutions   Verzeichnis, NAMESPACE
```

### Semantic-Core

- `NOVA_NAMESPACE_SOLUTIONS` ist der siebte stabile Namespace-Slot.
- `semantic_core_initialize` legt NamespaceID 7, ObjectID 7 und die primäre
  Projection an.
- ObjectID 7 ist `STABLE | USER_VISIBLE | NAMESPACE`.
- Eine Capability `CAPI:5` gibt `READ | EXECUTE` auf `/Solutions`.
- Der Selbsttest prüft:
  - Gesamtzahlen: 7 Objects, 7 Namespaces, 7 Projections, 5 Capabilities,
  - `/Solutions` über Namespace- und Object-Pfadauflösung,
  - Root-Kind-Aufzählung und Projection-Introspection,
  - Execute-Authority für ObjectID 7.

### NovaFS

`novafs_layout_table` enthält jetzt zusätzlich:

```text
Solutions -> ObjectID 7
```

`novafs_check_root_layout` lehnt ein NovaFS-Systemvolume ab, wenn `/Solutions`
fehlt oder eine andere ObjectID besitzt. Damit bleibt das On-Disk-Layout
konsistent mit dem Kernel-Semantic-Core.

### Userspace-VFS

Der Ring-3-VFS-Test öffnet das Root-Verzeichnis `/`, liest dessen Einträge mit
`VFS.ReadDirectory` und sucht explizit nach `Solutions` mit dem Typ
`NOVAFS_TYPE_DIRECTORY`. Fehlt dieser Eintrag, schlägt der Bootstrap-Test fehl.
Damit ist `/Solutions` nicht nur ein interner Kernel- und NovaFS-Begriff,
sondern durch die spätere Userspace-Schicht sichtbar.

### Bootausgabe

```text
NOVA: Namespace Core bereit: / System Benutzer Apps Volumes Boot Solutions
```

### Artefakte

- Kernel Build-ID: `AA18D312460BC36F9C29DD530979316949681F16`
- NKI CRC32: `B3A36CCE`

## 101. NovaFS Phase 2: Transaction Log und Crash Recovery

Phase 1 (Abschnitt 99) schreibt in-place: bricht eine Änderung ab, bleibt
`state = DIRTY` sichtbar, und ohne weitere Vorsorge könnten die Blöcke, auf
die der (weiterhin DIRTY, aber auf die *alten* Baumwurzeln zeigende)
Superblock verweist, bereits teilweise überschrieben sein. Bislang blieb das
Volume dann für den Rest des Boots read-only. Phase 2
(NPSPEC-NOVAFS-ONDISK-0001 §9) schließt diese Lücke mit einem
Undo-Journal, sodass eine Änderungsoperation atomar wird: entweder
vollständig sichtbar oder vollständig unsichtbar.

### Journal-Region

Direkt nach der Free-Space-Bitmap liegt eine feste Transaction-Log-Region
von `1 + NOVAFS_JOURNAL_SLOTS` (Phase 2: 17) Blöcken: ein Header-Block
(Magic `NJRN`, Version, `generation`, `entry_count`, CRC32C, bis zu 16
betroffene Blocknummern) plus 16 Daten-Slots (je ein vollständiges
4096-Byte-Pre-Image). Ihre Position steht im zuvor für Phase 2 reservierten
Superblock-Feld `transaction_log_block` (Offset 112); die Größe ist ein
fester Kernel-Konstantenwert, nicht pro Volume konfigurierbar. `mkfs`
reserviert und initialisiert die Region mit einem leeren, aber gültigen
Journal; `fsck` behandelt sie wie die Bitmap als reservierte Metadaten.

### Kernel: Capture-beim-Schreiben, Undo-beim-Wiederherstellen

`novafs_write_block_logged` ersetzt `novafs_write_block` an allen
Stellen, die Teil einer laufenden Änderung sind (`novafs_node_store`,
die Bitmap-Schreibschleife in `novafs_change_commit`, der Datenblock in
`novafs_write`, sowie der DIRTY- und der abschließende CLEAN-Schreibzugriff
des primären Superblocks selbst in `novafs_change_begin`/`_commit`). Vor
dem eigentlichen Schreibzugriff sichert `novafs_journal_capture` (sofern
eine Änderung läuft, `nfs_txn_open = 1`, und der Block in dieser
Transaktion noch nicht gesichert wurde) dessen bisherigen Inhalt in einen
freien Journal-Daten-Slot und aktualisiert den Header – inklusive Flush,
bevor der eigentliche Schreibzugriff erfolgt. Der allererste journalisierte
Schreibzugriff jeder Transaktion ist damit immer der DIRTY-Schreibzugriff
des Superblocks selbst: das Journal sichert sich also immer mindestens
seinen eigenen Zustand vor der Transaktion. Der Backup-Superblock wird
bewusst nicht journalisiert (siehe §9-Begründung in der Spezifikation).

`novafs_journal_undo` liest den Header, prüft Magic/Version/CRC32C und dass
`generation` exakt zur `generation` des gelesenen (weiterhin DIRTY)
Superblocks passt, und schreibt dann alle Pre-Images zurück (in
beliebiger Reihenfolge, da jeder Slot einen unabhängigen Block
beschreibt). Zwei Aufrufer nutzen das:

- **`novafs_mount`**: liest `state = DIRTY`, versucht die
  Wiederherstellung, liest danach den (jetzt wieder CLEAN) primären
  Superblock neu ein und mountet normal weiter. Schlägt die
  Wiederherstellung fehl (kein/ungültiges Journal – etwa ein Volume aus
  der Zeit vor Phase 2), bleibt es beim Phase-1-Verhalten: Read-only,
  `state = DIRTY` sichtbar.
- **`novafs_change_abort`** (aufgerufen von `vfs_change_end`, wenn eine
  VFS-Operation selbst fehlschlägt, ohne dass der Kernel neu startet):
  rollt die Teiländerung sofort zurück, lädt zusätzlich die im Speicher
  gehaltene Bitmap-Kopie neu von der (jetzt wiederhergestellten) Platte,
  und das Volume bleibt beschreibbar – statt wie zuvor für den Rest des
  Boots read-only zu werden (`message_vfs_volume_poisoned`). Nur wenn die
  Wiederherstellung selbst scheitert, greift weiterhin dieser alte
  Rückfall.

### Host-Werkzeug

`novafs.c` kennt dieselbe Journal-Region (Layout, Validierung,
`fsck`-Reservierung). `volume_begin` journalisiert vor dem Setzen von
`state = DIRTY` das Pre-Image des primären Superblocks (ein Eintrag,
Blocknummer 1); `volume_commit` leert das Journal nach dem CLEAN-Schreib-
zugriff, bevor (unjournalisiert) der Backup-Superblock geschrieben wird.
Damit erzeugt `novafs mark-dirty` ein Abbild, das der Kernel per Journal
tatsächlich reparieren kann – getestet über ein echtes QEMU-Boot-Szenario:
Volume vor `mark-dirty` beschreiben, danach booten, Journal-Wieder-
herstellung beobachten, Schreibtest/Bootzähler laufen weiter, `fsck`
danach weiterhin fehlerfrei.

### Grenzen

- das Host-Werkzeug journalisiert nur den einen Schreibzugriff in
  `volume_begin` (keine Knoten/Bitmap); für `mark-dirty` als Testhilfe
  reicht das, ein echtes, mitten in einer Baumänderung abgebrochenes
  Schreiben von Knoten simuliert nur der Kernel selbst (z. B. über
  `novafs_change_abort`, wenn eine VFS-Operation mitten in einer Änderung
  fehlschlägt),
- höchstens 16 gesicherte Blöcke pro Transaktion (`NOVAFS_JOURNAL_SLOTS`);
  eine Transaktion, die mehr Blöcke als das träfe, würde beim Versuch,
  einen 17. Block zu sichern, mit `NOVAFS_ERR_FULL` abgebrochen, bevor der
  ungesicherte Schreibzugriff erfolgt (in der aktuellen Phase-1-Baumhöhe
  ≤ 2 wird dieses Limit nicht erreicht),
- die Wiederherstellung rollt eine abgebrochene Transaktion immer
  vollständig zurück (kein Redo); eine Transaktion, die kurz vor dem
  letzten Schritt (CLEAN-Schreibzugriff) abbricht, wird dadurch verworfen,
  statt als abgeschlossen zu gelten – ein bewusster, konservativer
  Kompromiss zugunsten der Einfachheit,
- kein Schutz gegen gleichzeitige Änderungen (weiterhin Single-Threaded-
  Bootpfad, siehe §8).

## 102. Explorer: Entf löscht den fokussierten Eintrag

Bislang existierten `ufs_delete`/`ufs_rename` nur als von Ring 3 aus
aufrufbare VFS-Wrapper (getestet im Ring-3-Selbsttest beim Boot), ohne an
eine Explorer-Bedienhandlung angebunden zu sein. Dieser Abschnitt verdrahtet
die Entf-Taste als erste solche Aktion.

### Tastaturpfad (Kernel, Ring 0 – keine Codegrenze)

Die Entf-Taste ist auf PC-Tastaturen eine erweiterte Taste (Scancode Set 1
`0xE0 0x53`, Set 2 `0xE0 0x71`). Der bestehende Tastatur-Treiber
(`keyboard_scancode_translate` o. ä., im erweiterten Zweig neben den
Pfeiltasten) erkennt sie jetzt zusätzlich und reicht eine neue semantische
Aktion `SYSTEM_INPUT_DELETE equ 11` über `input_router_enqueue` an das
Ring-3-System-UI weiter – exakt derselbe Mechanismus wie für Tab, Enter,
Backspace und die Pfeiltasten. Da dieser Übersetzungscode im Kernel (Ring 0)
liegt, unterliegt er nicht der Ring-3-Codegrenze.

### Dispatch und Löschen (Ring 3, `userspace_program_start`..`_end`)

Im zentralen Eingabe-Dispatch des Ring-3-System-UI (`SYSTEM_INPUT_*`-
Vergleichskette) wird `SYSTEM_INPUT_DELETE` auf einen neuen `.delete_entry`-
Zweig geleitet. Dieser ist nur im Explorer-Arbeitsbereich (Workspace 0)
aktiv und wertet den aktuellen Fokuswert aus:

- Fokus 20..23 (Dateizeile 0..3) → `NOVAFS_TYPE_FILE`, Index = Fokus − 20
- Fokus 24..27 (Ordnerkarte 0..3) → `NOVAFS_TYPE_DIRECTORY`, Index = Fokus − 24
- außerhalb 20..27 → keine Wirkung (Szene wird unverändert erneut
  präsentiert)

Eine neue, gemeinsam genutzte Routine `ufs_find_nth` (EAX = NovaFS-Typ,
EBX = Index) sucht den n-ten Verzeichniseintrag dieses Typs im aktuellen
Arbeitsverzeichnis (`UFS_H_DIR`, bereits offen, nur lesend) und liefert ihn
in `UFS_ENTRY` zurück – dasselbe Scan-Muster (Index hochzählen, Typ
vergleichen, Nth herunterzählen) wie das bereits vorhandene
`ufs_enter_child` für Ordnerkarten, jetzt aber einmal implementiert statt
in jeder Aktion neu. `ufs_delete_nth` ruft `ufs_find_nth` auf und löscht
dann den gefundenen Namen. Eine Besonderheit dabei: Die Explorer-Navigation
(`ufs_open_path`, aufgerufen von `ufs_enter_child`/`ufs_enter_quick`/
`ufs_enter_parent`) öffnet Verzeichnisse stets mit `EDI = 0`, also ohne
Schreibrecht – ausreichend zum Anzeigen, aber `VFS.Delete` verlangt ein
Handle mit `HANDLE_RIGHT_WRITE` (`vfs_check_write` in `vfs_op_delete`).
`ufs_delete_nth` öffnet deshalb den aktuellen Pfad (`UFS_CWD`/
`UFS_CWD_LEN`) zusätzlich kurz mit `VFS_LOOKUP_FLAG_WRITE` (Handle in
`UFS_H_NEW` zwischengespeichert – zu diesem Zeitpunkt im Programmlauf
frei, da nur während des einmaligen Boot-Selbsttests benutzt) und löscht
den von `ufs_find_nth` gefundenen Namen (`UFS_ENTRY + 32`/`+ 20`) über
dieses neue Handle – ohne das Verzeichnis dafür ein zweites Mal zu
durchsuchen, da der Name bereits bekannt ist und `VFS.Delete` ihn über das
Verzeichnis-Handle plus Namen auflöst, nicht über einen bestimmten
Scan-Index. Das Schreibrecht-Handle wird anschließend wieder geschlossen,
unabhängig vom Ergebnis. Schlägt schon das Öffnen mit Schreibrecht fehl
(z. B. außerhalb `/Benutzer` ohne `SECURITY_CAP_FS_SYSTEM_WRITE`), wird
dieser Fehler unverändert zurückgegeben.

Nach einem erfolgreichen Löschen ruft `.delete_entry` `ufs_present` auf,
das die Ansicht unverändert aus `UFS_H_DIR`/`UFS_CWD` neu aufbaut (gleicher
Verzeichnis-Lesehandle wie zuvor, jetzt mit einem Eintrag weniger) und an
den Display Server übergibt. Schlägt das Löschen fehl (z. B.
`NOVAFS_ERR_NOT_EMPTY` bei einem nicht-leeren Ordner, oder kein Eintrag an
dieser Fokusposition), bleibt die zuletzt übergebene Ansicht unverändert
bestehen – es gibt keine Fehleranzeige im UI, nur das Fehlen einer neuen
`Explorer zeigt...`-Protokollzeile.

### Ring-3-Codebudget

Der anfängliche Userspace-Prozess ist hart auf zwei 4-KiB-Seiten begrenzt
(`%error`-Prüfung zwischen `userspace_program_start`/`_end`; Code- und
Stack-Seite liegen ohne Lücke aneinander, siehe `userspace_initialize`).
Vor dieser Änderung waren 453 von 8192 Byte frei. Die erste Fassung von
`ufs_delete_nth` (mit einem zweiten, überflüssigen Verzeichnis-Scan unter
dem Schreibrecht-Handle, siehe Abschnitt 103 zur Begründung des späteren
Refactorings) belegte davon rund 280 Byte, sodass 173 Byte frei blieben.
Nach dem Refactoring auf das gemeinsame `ufs_find_nth` (Abschnitt 103)
sind es **13 von 8192 Byte** – siehe dort für die genaue Aufschlüsselung
und was dieses Restbudget für weitere Erweiterungen bedeutet.

### Test

`test-uefi-display-server.ps1` erzeugt einen deterministischen
Ausgangszustand für den Fokus, ohne das aktuelle Verzeichnis zu verändern:
Startmenü öffnen (setzt Fokus fest auf 2), einmal Tab (Fokus 3, Eintrag
"Explorer"), Enter (öffnet den Explorer-Arbeitsbereich mit Fokus fest auf
20, Startmenü schließt). Von dort vier weitere Tab verschieben den Fokus
auf 24 (erste Ordnerkarte). Entf wird gesendet, und das Skript prüft
anhand der zuletzt protokollierten `Explorer zeigt NovaFS-Verzeichnis...`-
Zeile, dass die Eintragszahl um genau eins gesunken ist und der Pfad
unverändert geblieben ist. Nebenbei behoben: `Start-Process -WindowStyle
Hidden` wird von PowerShell Core auf Linux nicht unterstützt und ließ das
Testskript bisher dort grundsätzlich fehlschlagen (`$IsWindows`-Weiche
ergänzt); QEMU läuft ohnehin mit `-display none`, sodass kein Fenster
entsteht.

Beim Entwickeln dieses Tests zeigte sich ein zweiter, subtilerer Fehler
im eigenen Testaufbau: `build-uefi-image.ps1` übernimmt ohne
`-ResetNovaFs` eine im Ziel-Image bereits vorhandene NovaFS-Partition
unverändert (`"bestehende Partition uebernommen"`), statt sie durch das
frisch per `-NovaFsImage` übergebene Abbild zu ersetzen. Da
`build/nova-uefi.img` zwischen Testläufen auf der Festplatte liegen
bleibt, mutierten wiederholte Testläufe so stillschweigend dasselbe
persistente Volume weiter – eine Annahme des Tests ("die erste
Ordnerkarte in Matthias ist ein leerer Ordner") traf nach einem
vorherigen erfolgreichen Löschlauf (der genau diesen leeren Ordner
entfernt hatte) beim nächsten Lauf nicht mehr zu, und der nächste
Ordner an dieser Position war nicht mehr leer (`NOVAFS_ERR_NOT_EMPTY`).
Für reproduzierbare, von vorherigen Läufen unabhängige Testläufe muss
`build-uefi-image.ps1` daher mit `-ResetNovaFs` aufgerufen werden.

### Offen

- Umbenennen (F2) aus dem Explorer heraus: `ufs_rename` existiert bereits
  als Wrapper, eine analoge `.rename_entry`-Verdrahtung bräuchte zusätzlich
  eine Texteingabemöglichkeit (es gibt noch keine) und passt bei nur noch
  13 freien Byte (Abschnitt 103) ohnehin nicht mehr ohne eine strukturelle
  Erweiterung des Codebudgets.
- Löschen über die Schnellzugriff-Einträge (Fokus 30..36) ist nicht
  verdrahtet; diese sind über Tastatur ohnehin nicht fokussierbar (nur per
  Maus über `explorer_hit_test`).

## 103. Explorer: Enter öffnet eine Datei (einfache Inhaltsvorschau)

Enter/Aktivieren auf einer fokussierten Dateizeile (Fokus 20..23) tat
bisher nichts: Der zentrale `.activate`-Dispatch prüfte nur
`cmp eax, 24 / jb .activate_menu` – ein Fokus unter 24 im
Explorer-Arbeitsbereich fiel also ungenutzt in die für den Startmenü-
Fokus (2..10) gedachte `.activate_menu`-Vergleichskette, wo er garantiert
auf keinen der Fälle `eax == 3/6/9` passt und folgenlos verpufft.

### Gemeinsame Suche: `ufs_find_nth`

Bevor dieses Feature dazukam, hatte `ufs_delete_nth` seinen eigenen
Verzeichnis-Scan (Index hochzählen, Typ vergleichen, Nth herunterzählen)
inline – und brauchte für die neue Dateivorschau eine zweite, fast
identische Kopie desselben Scans. Beide wurden daher in eine gemeinsame
Routine `ufs_find_nth` (EAX = NovaFS-Typ, EBX = Index -> EAX = Status,
Treffer in `UFS_ENTRY`) ausgelagert. Dabei fiel zusätzlich auf, dass die
ursprüngliche `ufs_delete_nth` den gefundenen Eintrag ein zweites Mal
suchte – diesmal unter dem frisch mit Schreibrecht geöffneten Handle
(`UFS_H_NEW`) – obwohl das gar nicht nötig ist: `VFS.Delete` identifiziert
den zu löschenden Eintrag über Verzeichnis-Handle plus Name, nicht über
einen bestimmten Scan-Index, sodass der bereits über `UFS_H_DIR` (lesend)
gefundene Name direkt auf dem neuen Schreibrecht-Handle gelöscht werden
kann. Dieses Refactoring sparte selbst Platz, noch bevor die neue
Dateivorschau überhaupt etwas hinzufügte – ohne diese beiden
Einsparungen hätte das Codebudget (siehe unten) das neue Feature gar
nicht mehr aufgenommen.

### `ufs_preview_nth`

Eine neue Routine `ufs_preview_nth` (EBX = Index der n-ten Datei im
aktuellen Verzeichnis) ruft `ufs_find_nth` mit `NOVAFS_TYPE_FILE` auf,
öffnet den gefundenen Namen readonly über `ufs_lookup` (ohne
`VFS_LOOKUP_FLAG_WRITE` – `VFS.Read` verlangt nur `HANDLE_RIGHT_READ`,
das ein Lookup auch ohne Schreibrecht-Flag immer mitvergibt, siehe
`vfs_op_lookup`/`.resolve` in `vfs32.inc`), liest bis zu
`EXPLORER_PATH_MAX` (56) Byte direkt in `UFS_VIEW + 40` – also genau das
Feld, das sonst den Breadcrumb-Pfad trägt – setzt `UFS_VIEW + 28`
(PathLength) auf die tatsächlich gelesene Byteanzahl und ruft
`SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW` direkt erneut auf. Bewusst wird
dabei *nicht* `ufs_present`/`ufs_build_view` aufgerufen, da das den
Breadcrumb wieder aus `UFS_CWD` neu aufbauen und die Vorschau sofort
überschreiben würde. Nicht darstellbare Bytes im Dateiinhalt werden vom
Kernel beim Entgegennehmen der Ansicht automatisch wie jeder andere
Pfadtext behandelt (`explorer_sanitize`, Ersetzung durch `-`), es gibt
also keinen Absturzpfad für binäre oder mehrzeilige Inhalte – nur
unleserliche Darstellung. Das Dateihandle wird danach wieder geschlossen.
Es gibt noch kein eigenes Anzeigeelement für Dateiinhalte; die Vorschau
missbraucht bewusst das vorhandene Pfadfeld, weil für ein neues
UI-Element (siehe Ring-3-Codebudget unten) kein Platz mehr ist. Navigiert
man danach weiter, baut `ufs_present` den Breadcrumb normal neu auf –
die Vorschau ist rein transient und hinterlässt keinen Zustand.

### Ring-3-Codebudget

Nach `SYSTEM_INPUT_DELETE`-Dispatch, dem `ufs_find_nth`-Refactoring, dem
schlankeren `ufs_delete_nth` und dem neuen `ufs_preview_nth` plus
`.open_file`-Dispatch sind nur noch **13 von 8192 Byte** des
Ring-3-Codebudgets frei (`%error`-Prüfung zwischen
`userspace_program_start`/`_end` schlägt erst beim Überschreiten fehl –
bei diesem Stand kompiliert es gerade noch). Jede weitere Erweiterung,
insbesondere Umbenennen (F2), das zusätzlich eine Texteingabemöglichkeit
bräuchte, passt ohne eine strukturelle Änderung (zusätzliche Codeseite,
verschobene `USER_STACK_ADDRESS`-Konstanten und alle davon abhängigen
`UFS_*`-Offsets) nicht mehr in dieses Budget.

### Test

`test-uefi-display-server.ps1` nutzt denselben Startmenü-Trick wie beim
Entf-Test (Fokus fest auf 20), diesmal direkt nach dem Boot-Selbsttest
und vor jeder Explorer-Navigation, damit das Arbeitsverzeichnis
unverändert `Matthias/Dokumente` bleibt (die einzige Datei dort ist die
vom Selbsttest angelegte `Willkommen.txt`). Ein weiteres Enter löst
`ufs_preview_nth` aus; das Skript prüft, dass die nächste protokollierte
`Explorer zeigt NovaFS-Verzeichnis...`-Zeile den Dateiinhalt
("Willkommen bei NovaOS.") statt des Pfads enthält.

### Offen

- Es gibt keine echte Dateiinhalts-Anzeige (eigenes Fenster/Textfeld,
  Scrollen, mehr als 56 Byte, binäre Dateien lesbar machen); das ist
  ebenfalls erst nach einer strukturellen Erweiterung des
  Ring-3-Codebudgets sinnvoll umsetzbar.
- Umbenennen (F2) bleibt offen (siehe Abschnitt 102) – bei 13 freien Byte
  endgültig nicht mehr ohne Strukturänderung machbar (siehe Abschnitt 104,
  das diese Strukturänderung durchführt).

## 104. Ring-3-Codebudget: dritte Codeseite für das System-UI-Programm

Bei nur noch 13 von 8192 freien Byte (Abschnitt 103) passt keine weitere
Erweiterung – insbesondere Umbenennen (F2) mit der dafür nötigen
Texteingabemöglichkeit – mehr in das bisherige Zwei-Seiten-Codebudget.
Diese Änderung erweitert es auf drei Seiten (12 KiB) und behebt zwei
Fehler, die dabei sichtbar wurden.

### Layoutänderung

`USER_STACK_ADDRESS` wandert von `0x00403000` auf `0x00404000`, womit
zwischen `USER_CODE_ADDRESS` (`0x00400000`, unverändert) und der
Stack-Seite drei volle 4-KiB-Codeseiten liegen statt bisher zwei. Alle
`UFS_*`-Scratch-Variablen sind als `USER_STACK_ADDRESS - N` definiert und
verschieben sich dadurch automatisch korrekt mit; keiner dieser Offsets
musste einzeln angepasst werden. `userspace_initialize` erhält einen
dritten Alloziere-Nullen-Kopiere-Mappe-Block (dritte physische Seite,
`userspace_code_page_3`), der – wie die zweite Seite schon zuvor – pro
Seite eine volle `PMM_PAGE_SIZE` unbedingt kopiert statt einer von der
tatsächlichen Programmlänge abhängigen Länge. Das ist bewusst so: NASMs
`%if`-Präprozessor kann (anders als gewöhnliche Operanden, die über
mehrere Pässe hinweg vorwärts auflösen) nicht auf `userspace_program_end`
vorwärtsverweisen, eine längenabhängige `%if`-Kopiergröße scheidet also
aus, und Byte hinter `userspace_program_end` innerhalb einer Seite galten
als harmlos, weil sie laut `%error`-Budgetprüfung nie ausgeführt werden.
Der `%error`-Schwellenwert selbst steigt von `PMM_PAGE_SIZE * 2` auf
`PMM_PAGE_SIZE * 3`, und der IPC-Paket-Seitenpatch (Sprung zwischen
`userspace_code_page`/`_2`/`_3` je nach Lage von `userspace_ipc_packet`)
wächst von einer zweiseitigen auf eine dreiseitige `%if`/`%elif`/`%else`-
Fallunterscheidung.

### Fehler 1: `SHARED_SERVICE_ADDRESS` kollidierte mit der neuen Stackseite

`SHARED_SERVICE_ADDRESS` war bislang hart auf `0x00403000` codiert – ein
Wert, der zufällig mit dem *alten* `USER_STACK_ADDRESS` übereinstimmte
und dadurch direkt oberhalb der alten (einseitigen) Stackseite lag, ohne
sie zu überlappen. Beim Verschieben von `USER_STACK_ADDRESS` auf
`0x00404000` blieb dieser hartcodierte Wert unverändert und lag damit
plötzlich *innerhalb* der neuen Stackseite (`0x00403000`-`0x00403FFF`):
`shared_service_page_initialize` überschrieb die beschreibbare PTE der
Stackseite mit einer nicht beschreibbaren Abbildung auf eine andere
physische Seite. Ergebnis: ein sofortiger Seitenfehler
(`NOVA PANIC: CPU-Ausnahme Vektor 0x0000000E bei Adresse 0x00403FC0`),
sobald der Userspace-Prozess seinen eigenen Stack beschrieb. Behoben
durch eine selbstnachführende Definition statt eines Literals:
`SHARED_SERVICE_ADDRESS equ USER_STACK_ADDRESS`. `USER_ADDRESS_MIN`/
`USER_ADDRESS_MAX` (für `syscall_validate_user_range`) brauchten keine
Anpassung – sie sind bereits relativ zu `USER_CODE_ADDRESS`/
`USER_STACK_ADDRESS` definiert und schließen die Shared-Service-Seite
weiterhin korrekt als ungültiges Nutzerpuffer-Ziel aus.

### Fehler 2: zu kurz deklarierte `userspace_system_scene`-Struktur

Nach der Behebung von Fehler 1 bootete der Kernel zwar ohne Absturz,
aber `test-uefi-display-server.ps1` schlug fehl: Die Markierung
„Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene
praesentiert" erschien nie. Eine gezielte Diagnoseausgabe im
`SYSCALL_DISPLAY_SUBMIT_SCENE`-Handler (temporär, wieder entfernt) zeigte,
dass das letzte der fünf vom Kernel geprüften Reserviert-Felder
(`syscall_display_scene + 60`, erwartet `0`) einen zufälligen Wert
(`0x1B74C985`) enthielt, worauf `.bad_reserved` die Szene ablehnte, ohne
dass irgendeine Fehlermeldung protokolliert wird – der Prozess lief
danach einfach regulär zu Ende (`SYSCALL_CORE_EXIT`), ohne die Desktop-
Szene je präsentiert zu haben. Ursache: `userspace_system_scene` deklarierte
nur **60 Byte** Nutzlast (`times 5 dd 0` als Füllfelder), während sowohl
`SYSTEM_SCENE_SIZE` als auch der Ring-0-Prüfcode 64 Byte (5 Reserviert-
Dwords bei Offset 44..60) erwarten. Mit der *alten*, längenabhängigen
Seitenkopie (Abschnitt „Layoutänderung") endete die kopierte Seite exakt
bei `userspace_program_end`, und alles danach blieb beim vorherigen
Nullen der Seite auf `0` stehen – der fehlende 64. Byte las deshalb
zufällig immer `0` und der Fehler blieb unsichtbar. Die neue, unbedingte
Vollseitenkopie kopiert dagegen die tatsächlichen (nicht genullten) Byte
des Kernel-Abbilds hinter `userspace_program_end`, wodurch dieser
vorher verdeckte Strukturfehler sichtbar wurde. Behoben durch
`times 6 dd 0` (6 statt 5 Reserve-Dwords), womit die Struktur die vollen
64 Byte erreicht.

### Test

Nach beiden Fixes liefen sowohl `test-uefi-display-server.ps1` (alle drei
Szenarien: Enter öffnet Datei, Entf löscht Eintrag, Tab/Pfeiltasten-
Navigation mit Shutdown) als auch die vollständige
`test-uefi-novafs.ps1`-Regressionssuite (frisches Volume, alle
Split-Fälle, beschädigter Superblock, DIRTY-Journal-Reparatur,
DIRTY-ohne-Journal-Rückfall, unformatierte Partition) fehlerfrei durch,
jeweils auf einem mit `-ResetNovaFs` frisch aufgesetzten Image.

### Ergebnis

Das Ring-3-Codebudget liegt jetzt bei **4099 von 12288 Byte frei**
(eine volle zusätzliche Seite abzüglich der schon vorher fehlenden 3
Byte). Das reicht für Umbenennen (F2) inklusive einer einfachen
Texteingabemöglichkeit.

### Offen

- Umbenennen (F2) selbst ist noch nicht implementiert – nur das dafür
  nötige Codebudget steht jetzt bereit (siehe Abschnitt 105, das dies
  umsetzt).

## 105. Explorer: Umbenennen (F2) mit einfacher Texteingabe

Baut auf dem in Abschnitt 104 freigemachten Codebudget auf und
implementiert sowohl F2/Umbenennen selbst als auch die dafür nötige,
bisher nicht existierende Texteingabemöglichkeit.

### Texteingabe im Kernel (Ring 0): `keyboard_ascii_table`

Bisher kannte der Tastatur-Interrupt-Handler (`input_router_handle_scancode`)
nur eine feste Liste semantischer Aktionen (Pfeiltasten, Enter, Escape,
Tab, Backspace, Entf, die Windows/Nova-Taste); jede andere Taste fiel in
`xor eax, eax / ret` und wurde stillschweigend ignoriert. Für Umbenennen
reicht das nicht – ein Dateiname braucht echte Buchstaben. Eine neue
256-Byte-Tabelle `keyboard_ascii_table` übersetzt PS/2-Set-1-Make-Codes
in Kleinbuchstaben (US-QWERTY: Ziffern, Buchstaben, Leertaste, `,./-=`;
alles andere bleibt `0` = kein druckbares Zeichen). Im `.plain:`-Zweig des
Handlers wird nach den bestehenden Spezialtasten-Vergleichen zuerst F2
abgefragt (Set 1 `0x3C`, Set 2 `0x06`) und erst danach die Tabelle
konsultiert; nicht zugeordnete Codes (einschließlich aller Set-2-Tasten,
für die es keinen eigenen Tabelleneintrag gibt) bleiben wie zuvor
wirkungslos. Ein Treffer wird als neue semantische Aktion
`SYSTEM_INPUT_TEXT_CHAR` mit dem übersetzten ASCII-Byte im
Scancode-Feld des Eingabeereignisses an Ring 3 weitergereicht – die
Übersetzung geschieht bewusst in Ring 0 (unbegrenztes Codebudget), damit
Ring 3 keine eigene Tabelle braucht. `SYSTEM_INPUT_RENAME_KEY` (F2
selbst) ist eine zweite neue Aktion. Beide Erweiterungen sind rein
additiv zum bestehenden `SYSTEM_INPUT_*`-Schema und betreffen nur Ring 0,
nicht das knappe Ring-3-Codebudget.

Bewusst nicht gelöst: Umschalt-Großschreibung, Sonderzeichen über
Tottasten, Set-2-Tastaturen für Text (nur Set 1 ist abgedeckt) – für
Dateinamen in diesem Entwicklungsstand ausreichend.

### Wiederverwendung bestehender Tasten statt Konflikten

Statt neue Tastenbelegungen für "Zeichen löschen" oder "abbrechen"
einzuführen, interpretiert der zentrale Ring-3-Dispatch zwei ohnehin
vorhandene Aktionen kontextabhängig um, solange `UFS_RENAME_ACTIVE`
gesetzt ist:
- `SYSTEM_INPUT_NAVIGATE_BACK` (Backspace) bedeutet normalerweise
  "Explorer: einen Ordner nach oben". Während der Umbenennung löscht es
  statt dessen das letzte Zeichen des Editierpuffers
  (`.navigate_back` prüft `UFS_RENAME_ACTIVE` als erstes und springt bei
  gesetztem Flag zu `.rename_backspace`, statt `ufs_enter_parent`
  aufzurufen).
- `SYSTEM_INPUT_ACTIVATE` (Enter) bedeutet normalerweise "Startmenü-
  Eintrag aktivieren" bzw. "Datei öffnen"/"Ordner betreten". Während der
  Umbenennung bestätigt es statt dessen die Eingabe (`.activate` prüft
  `UFS_RENAME_ACTIVE` ebenfalls zuerst und springt zu `.rename_commit`).

Escape wird bewusst **nicht** zum Abbrechen verwendet: Im Kernel löst
Escape außerhalb des Startmenüs einen sofortigen, ungeordneten
Shutdown-Pfad aus (`input_router_handle_scancode` kennt keinen
Ring-3-Zustand und wüsste nicht, dass gerade umbenannt wird). Abbrechen
geschieht daher über ein zweites F2 (`.rename_key` prüft
`UFS_RENAME_ACTIVE`; ist es bereits gesetzt, springt es zu
`.rename_cancel`, das den Puffer verwirft und über `ufs_present` die
normale Ansicht wiederherstellt, ohne `ufs_rename` aufzurufen).

### Ring-3-Zustand und -Ablauf

Sieben neue, selbstnachführende `UFS_RENAME_*`-Scratch-Variablen (nach
dem gleichen `USER_STACK_ADDRESS - N`-Schema wie alle `UFS_*`-Variablen)
halten den Editierzustand: `ACTIVE` (Flag), `TYPE`/`INDEX` (welcher
Eintrag), `LEN`/`OLDLEN` (aktuelle/urspüngliche Namenslänge) sowie die
beiden `EXPLORER_NAME_MAX` (32) Byte großen Puffer `BUF` (neuer Name,
wird live editiert) und `OLDNAME` (Schnappschuss des alten Namens, für
den späteren `ufs_rename`-Aufruf).

- **F2 (`.rename_key`):** nur im Explorer-Arbeitsbereich (nicht
  Startmenü) und nur auf einer Datei-/Ordnerzeile (Fokus 20..27) aktiv.
  Ruft `ufs_find_nth` auf dem fokussierten Eintrag auf, kopiert dessen
  Namen sowohl nach `OLDNAME` (unveränderliche Kopie) als auch nach
  `BUF` (Startzustand des Editierpuffers, damit man einen Namen auch nur
  teilweise ändern kann statt ihn komplett neu eintippen zu müssen),
  setzt `ACTIVE=1` und zeichnet den Puffer sofort.
- **Zeichen tippen (`.text_char`):** nur bei `ACTIVE=1` und solange
  `LEN < EXPLORER_NAME_MAX`; hängt das vom Kernel übersetzte Zeichen an
  `BUF` an und zeichnet neu.
- **Backspace (`.rename_backspace`):** nur bei `LEN > 0`; verkürzt `LEN`
  um 1 und zeichnet neu.
- **Enter (`.rename_commit`):** öffnet `UFS_CWD` kurz mit Schreibrecht
  (wie bei Löschen/Entf), ruft `ufs_rename` mit `OLDNAME`/`OLDLEN` als
  Quelle und `BUF`/`LEN` als Ziel auf (dasselbe Verzeichnis als Quelle
  und Ziel, da nur innerhalb des aktuellen Ordners umbenannt wird),
  schließt das Handle wieder, setzt `ACTIVE=0` und baut die Ansicht über
  `ufs_present` neu auf NovaFS auf – unabhängig davon, ob `ufs_rename`
  erfolgreich war (ein Fehler lässt den alten Namen unverändert bestehen,
  es gibt aber keine Fehleranzeige).
- **Zweites F2 (`.rename_cancel`):** setzt `ACTIVE=0` und stellt über
  `ufs_present` die normale Ansicht wieder her, ohne `ufs_rename`
  aufzurufen.

Die Live-Anzeige des Editierpuffers (`ufs_rename_redraw`) funktioniert
exakt wie die Dateivorschau in Abschnitt 103: Sie schreibt `BUF`/`LEN`
direkt in `UFS_VIEW + 40`/`+ 28` (das Breadcrumb-Feld) und ruft
`SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW` direkt auf, ohne über
`ufs_present`/`ufs_build_view` zu gehen (das würde den Breadcrumb aus
`UFS_CWD` überschreiben). Es gibt also weiterhin kein eigenes
Eingabefeld-UI-Element – der Editierzustand wird, wie schon die
Dateivorschau, im vorhandenen Pfadfeld dargestellt.

### Ring-3-Codebudget

Nach F2/Text-Dispatch, den sieben neuen `UFS_RENAME_*`-Variablen und
`ufs_rename_redraw` sind **3611 von 12288 Byte** frei (vorher 4099) –
die dritte Codeseite aus Abschnitt 104 hatte also reichlich Platz für
dieses Feature gelassen.

### Test

`test-uefi-display-server.ps1` ergänzt ein neues Szenario direkt nach
dem Dateivorschau-Test (Fokus weiterhin fest auf 20, Verzeichnis
weiterhin `Matthias/Dokumente`, Datei `Willkommen.txt`, 14 Zeichen): F2,
14x Backspace (Puffer leeren), dann `h`,`a`,`l`,`l`,`o` eintippen. Nach
jedem Tastendruck wird anhand der protokollierten
`Explorer zeigt NovaFS-Verzeichnis...`-Zeilen geprüft, dass eine neue
Ansicht mit dem erwarteten Zwischenstand im Pfadfeld erscheint (zuletzt
`Pfad hallo`). Ein abschließendes Enter bestätigt die Umbenennung; das
Skript prüft nur, dass danach wieder eine Ansicht mit dem normalen
Breadcrumb (`Dokumente`) erscheint, nicht aber den Namen auf der
Platte. Die tatsächliche Umbenennung wurde zusätzlich manuell mit dem
Host-Werkzeug (`novafs ls <image> --gpt /Benutzer/Matthias/Dokumente`)
gegen das von diesem Testlauf erzeugte Image geprüft: Der Eintrag heißt
danach `hallo` statt `Willkommen.txt`, bei unveränderter Inode (24) und
Dateigröße (266 Byte) – also eine echte Umbenennung, kein
Löschen-und-Neuanlegen. Anschließend liefen sowohl die restlichen
Display-Server-Szenarien (Entf, Navigation mit Shutdown) als auch die
vollständige `test-uefi-novafs.ps1`-Regressionssuite fehlerfrei durch,
jeweils auf einem mit `-ResetNovaFs` frisch aufgesetzten Image.

### Offen

- Kein eigenes Eingabefeld-UI-Element (wie bei der Dateivorschau wird
  das Pfadfeld zweckentfremdet).
- Keine Umschalt-Großschreibung, keine Sonderzeichen über Tottasten,
  keine Set-2-Texteingabe (siehe oben).
- Escape bricht die Umbenennung nicht ab (löst statt dessen weiterhin
  den Shutdown-Pfad aus, sofern das Startmenü geschlossen ist) – man
  muss ein zweites F2 drücken.
- Wechselt man während der Umbenennung über die Windows/Nova-Taste ins
  Startmenü (`SYSTEM_INPUT_TOGGLE_START` wird nicht abgefangen), bleibt
  `UFS_RENAME_ACTIVE` gesetzt; kehrt man in den Explorer zurück, wird
  die Umbenennung dort fortgesetzt. Funktional unbedenklich, aber nicht
  gezielt getestet.
- Keine automatisierte, auf der Platte verifizierte Prüfung der
  Umbenennung innerhalb von `test-uefi-display-server.ps1` selbst (nur
  manuell mit dem Host-Werkzeug nachvollzogen); eine solche Prüfung
  bräuchte entweder ein `-NovaFsTool`-Parameter für dieses Skript oder
  einen Abgleich über eine erneute Dateivorschau.

## 106. NovaFS: Zeitstempel (created/modified/accessed/changed_time)

### Ausgangslage

`NPSPEC-NOVAFS-ONDISK-0001` §4 definiert im Object-Tree-Item vier u64-Felder
bei den Offsets 40/48/56/64 (`created_time`, `modified_time`,
`accessed_time`, `changed_time`), die §8 als Phase-1-Grenze explizit nennt:
„keine Nutzdatenprüfsummen, keine Zeitstempel". Weder der Kernel
(`novafs32.inc`) noch das Host-Werkzeug (`tools/novafs/novafs.c`) hatten
dafür bislang Offset-Konstanten; die Felder lagen als ungenutzte Nullbytes
innerhalb des bereits CRC32C-geschützten 152-Byte-Items, weil der
Objekt-Erzeugungscode den gesamten Puffer vorab mit `rep stosd` nullt und
die Felder nie explizit beschrieb. Eine Suche im Kernel ergab außerdem,
dass es überhaupt keine Uhr gab: keine CMOS-RTC-Portzugriffe (0x70/0x71),
kein PIT-/APIC-Tick-Zähler, keine 64-Bit-Arithmetik-Hilfsroutinen für eine
Epoch-Umrechnung.

### Entwurfsentscheidungen

- **Zeitquelle:** Die batteriegepufferte CMOS-Echtzeituhr, gelesen über die
  klassischen Ports 0x70 (Index)/0x71 (Daten). Das ist nach dem
  UEFI-Kernel-Handoff (`ExitBootServices` ist zu diesem Zeitpunkt bereits
  erfolgt) ebenso sicher direkt ansprechbar wie die bereits verwendeten
  Tastatur-Controller-Ports 0x60/0x64. Keine Zeitzonenkorrektur: Die RTC
  wird als UTC behandelt, wie in den meisten PC-Firmwares üblich.
- **Format:** Sekunden seit der Unix-Epoche (1970-01-01T00:00:00 UTC), als
  echter 64-Bit-Wert (EDX:EAX) berechnet – nicht nur ein 32-Bit-Wert mit
  angenommener Nullerweiterung. Damit ist die Umrechnung auch für
  Jahrhundert-Werte jenseits von 2038/2106 arithmetisch korrekt, auch wenn
  das Jahrhundert-Register praktisch auf 19–21 beschränkt bleibt (siehe
  unten).
- **Kein Kernel-seitiges 64-Bit-Divisionsproblem:** Die Tage-seit-1970-
  Berechnung läuft als einfache Schleife über die Jahre (höchstens rund
  230 Iterationen bei Jahrhundert 21/Jahr 99) statt über eine
  divisionsbasierte Kalenderformel (Håward-Hinnant-„days_from_civil" o.ä.);
  das ist in Assembler leichter nachvollziehbar und korrekt zu
  verifizieren als eine Formel mit mehreren vorzeichenbehafteten
  Ganzzahldivisionen, und die Laufzeit (keine Hunderte Mikrosekunden) ist
  für einen Schreibvorgang irrelevant.
- **Welche Operationen welche Felder berühren** (POSIX-nahe Semantik):
  - **Erzeugen** (`novafs_create`, `create_object`): `created`,
    `modified`, `accessed` und `changed` werden alle auf denselben
    Zeitpunkt gesetzt.
  - **Inhalt schreiben** (`novafs_write`, `file_write`): `modified`,
    `accessed` und `changed` werden aktualisiert, `created` bleibt.
  - **Umbenennen/Verschieben** (`novafs_rename`, `object_rename`): nur
    `changed` wird aktualisiert (reine Metadatenänderung, der Inhalt
    ändert sich nicht). Das gilt bewusst auch für einen reinen
    Namenswechsel innerhalb desselben Verzeichnisses – der bestehende
    Code aktualisierte `NFS_O_PARENT`/rief `novafs_object_store` bisher
    nur auf, wenn sich das Elternverzeichnis änderte, und ließ einen
    reinen Namenswechsel am Objekt-Item komplett unberührt. Das wurde im
    Zuge dieser Änderung korrigiert (Kernel und Host-Werkzeug): Das
    Objekt wird jetzt in jedem erfolgreichen Rename-Fall einmal
    nachgeladen, `changed` gesetzt und zurückgeschrieben.
  - **Lesen:** `accessed_time` wird bewusst **nicht** bei jedem Lesezugriff
    aktualisiert (kein „echtes" atime-Verhalten). Das würde bedeuten, dass
    jedes `VFS.Read`/jede Dateivorschau im Explorer eine journalisierte
    Schreibtransaktion auf einem sonst nur lesend geöffneten Volume
    auslöst – ein unverhältnismäßiger Preis für ein Feld, das in der
    Praxis kaum ausgewertet wird. `accessed_time` verhält sich damit wie
    `modified_time` plus den Erzeugungszeitpunkt.

### Kernel-Implementierung (`novafs32.inc`)

- Neue Offset-Konstanten `NFS_O_CREATED`/`NFS_O_MODIFIED`/
  `NFS_O_ACCESSED`/`NFS_O_CHANGED` (40/48/56/64) neben den bestehenden
  `NFS_O_*`-Konstanten.
- Neuer Abschnitt „Echtzeituhr (CMOS RTC) und Unix-Epoch" vor
  `novafs_create`:
  - `novafs_cmos_read` (AL=Registerindex → EAX=Rohwert) und
    `novafs_bcd_to_bin` (AL=BCD-Byte → EAX=Binärwert) als kleine
    Hilfsroutinen.
  - `novafs_rtc_sample` liest Status Register B (0x0B) sowie Sekunden,
    Minuten, Stunden, Tag, Monat, Jahr und Jahrhundert (Register 0x32,
    nicht auf jeder Firmware belegt) roh aus, wandelt bei Bedarf von BCD
    nach Binär (Stunden-PM-Bit wird dabei um die BCD-Wandlung
    herumgeführt), löst 12-Stunden-AM/PM-Kodierung in echte
    24-Stunden-Werte auf (inklusive der Sonderfälle 12 AM → 0 und
    12 PM → 12) und bildet aus dem Jahrhundert-Register (plausibel nur
    19–21, sonst wird 20 angenommen) das volle vierstellige Jahr.
  - `novafs_rtc_is_leap` (ECX=Jahr → EAX=1/0) nach der Standardregel
    (durch 4, aber nicht durch 100, außer durch 400).
  - `novafs_now_epoch` liest zweimal hintereinander (nach Warten auf
    „kein Registerupdate läuft", Status-Register-A-Bit 7) und verwirft
    die Lesung, falls sich zwischen beiden Lesungen etwas geändert hat
    (Schutz gegen einen während des Lesens laufenden
    RTC-Registerupdate-Übergang). Danach: Tage seit 1970-01-01 (Jahre
    einzeln aufsummiert plus Tabelle `novafs_days_before_month` für den
    Monat plus Schalttagskorrektur plus Tag im Monat), anschließend
    `Tage*86400 + Stunden*3600 + Minuten*60 + Sekunden` als volle
    64-Bit-Summe (EBX:ESI akkumuliert über drei `mul`-Schritte mit
    `adc`-Übertrag). Rückgabe in EDX:EAX.
  - Keine NMI-Maskierung während des CMOS-Zugriffs (Port 0x70 Bit 7
    würde das NMI deaktivieren) – bewusste Vereinfachung, siehe „Offen".
- Aufrufstellen: `novafs_create` setzt nach dem Belegen von
  `NFS_O_LINKS` alle vier Felder über einen Aufruf von
  `novafs_now_epoch`. `novafs_write` (`.metadata`-Zweig, vor
  `novafs_object_store`) aktualisiert `modified`/`accessed`/`changed`.
  `novafs_rename` lädt das verschobene/umbenannte Objekt jetzt
  bedingungslos (vorher nur bei Elternwechsel), setzt `changed`, und
  aktualisiert `NFS_O_PARENT` nur noch bedingt – `novafs_object_store`
  wird in jedem Erfolgsfall genau einmal aufgerufen.
- Neue Skalar-Variablen `nfs_rtc_regb/sec/min/hour/day/month/year/
  century`, ein 6-Dword-Vergleichspuffer `nfs_rtc_check` für die
  Doppellesung und `nfs_rtc_days` als Zwischenergebnis, neben den
  übrigen `nfs_*`-Arbeitsvariablen.
- Das 152-Byte-CRC32C-Siegel (`novafs_seal`/`novafs_verify` über
  `NFS_O_ITEM`) deckte die vier Felder schon vorher ab, da sie innerhalb
  der Item-Größe lagen; an der Prüfsummenbildung ändert sich nichts.

### Host-Werkzeug (`tools/novafs/novafs.c`)

- Neue Makros `O_CREATED`/`O_MODIFIED`/`O_ACCESSED`/`O_CHANGED` (40/48/56/
  64).
- `create_object()` setzt alle vier Felder über ein gemeinsames
  `time(NULL)`.
- Das direkt (nicht über `create_object()`) angelegte Root-Objekt
  (ObjectID 1) in `cmd_mkfs()` bekommt dieselben vier Felder zum
  Formatierungszeitpunkt.
- `file_write()` (Befehl `put`) aktualisiert `modified`/`accessed`/
  `changed` zusätzlich zu `size`/`allocated`.
- `object_rename()` (Befehl `mv`) wurde wie die Kernel-Seite korrigiert:
  Das Objekt wird jetzt immer (nicht nur bei Verzeichniswechsel)
  nachgeladen, `changed` gesetzt und zurückgeschrieben; `parent_id`
  wird weiterhin nur bei tatsächlichem Verzeichniswechsel geändert.
- Neuer Befehl `novafs stat <image> <pfad>` zeigt ObjectID, Typ, Größe,
  Rechte und alle vier Zeitstempel (als `%Y-%m-%d %H:%M:%S (UTC)`, `-`
  bei 0) eines Objekts. Bewusst ein **neuer** Befehl statt einer
  Erweiterung der bestehenden `ls`-Ausgabe: Ein erster Versuch, die
  Zeitstempelspalte direkt in `cmd_ls` einzufügen, brach die feste
  Spaltenregex in `test-uefi-novafs.ps1` (Zeile 143,
  `'12388\s+\d+\s+novafs-muster\.bin'`), die auf das bisherige,
  stabile `ls`-Format angewiesen ist. Ein separater `stat`-Befehl
  erweitert die Diagnosemöglichkeiten, ohne bestehende, textbasiert
  parsende Tests zu gefährden.

### Test

- Mit dem neu gebauten Host-Werkzeug wurde `build/novafs-system.img`
  komplett neu per `mkfs` erzeugt (damit auch die mkfs-Zeitstempel aus
  dem neuen Code stammen), anschließend über `build.sh -ResetNovaFs` neu
  in `build/nova-uefi.img` eingebettet.
- `test-uefi-novafs.ps1` (vollständige Regressionssuite: frisches Volume,
  alle sechs Baum-Split-Szenarien, beschädigter primärer Superblock,
  DIRTY mit und ohne gültiges Journal, unformatierte Partition) lief
  fehlerfrei durch.
- `test-uefi-display-server.ps1` (alle Szenarien inklusive des
  F2-Umbenennen-Tests aus Abschnitt 105) lief ebenfalls fehlerfrei durch
  – ein wichtiger Beleg, dass die neuen CMOS-Portzugriffe das bestehende
  Verhalten nicht stören.
- Manuelle Verifikation mit `novafs stat` am von diesem Testlauf
  erzeugten Image: Die im Test umbenannte Datei (`hallo`, ehemals
  `Willkommen.txt`) zeigt `created`/`modified`/`accessed` auf denselben
  Zeitpunkt (Objekterzeugung während des `VFS.Create`/`VFS.Write`-Tests)
  und ein rund vier Sekunden späteres `changed` (der F2-Umbenennen-Test
  lief kurz danach) – exakt die erwartete Reihenfolge, und die
  gelesenen Uhrzeiten stimmten sichtbar mit der tatsächlichen
  Wanduhrzeit des QEMU-Laufs überein (die Kernel-RTC lieferte also
  nicht nur intern konsistente, sondern tatsächlich reale Zeitwerte).
  Zusätzlich wurde das Host-Werkzeug isoliert geprüft (`mkdir`, `put`,
  `stat`, zwei Sekunden warten, `mv`, `stat`): `created`/`modified`/
  `accessed` blieben nach dem `mv` unverändert, nur `changed` sprang um
  die gewartete Zeit vor – `novafs fsck` akzeptierte das Ergebnis ohne
  Beanstandung.

### Offen

- Keine Zeitzonenkorrektur; die RTC wird ungeprüft als UTC behandelt.
  Läuft eine reale Firmware mit lokaler Zeit in der RTC, wären die
  gespeicherten Zeitstempel entsprechend verschoben.
- Keine NMI-Maskierung während des CMOS-Zugriffs (bewusste
  Vereinfachung, siehe oben).
- `accessed_time` wird nicht bei jedem Lesezugriff aktualisiert (siehe
  Entwurfsentscheidungen) – kein vollständiges atime-Verhalten.
- Das Jahrhundert-Register (CMOS 0x32) ist nicht auf jeder Firmware/in
  jeder QEMU-Konfiguration sinnvoll belegt; außerhalb des plausiblen
  Bereichs 19–21 wird pauschal 20 angenommen. Für den praktisch
  relevanten Zeitraum (hier: 2026) unproblematisch.
- Nutzdatenprüfsummen (die andere in `NPSPEC-NOVAFS-ONDISK-0001` §8
  genannte Phase-1-Grenze) bleiben weiterhin offen.
- Kein automatisierter Test prüft die Zeitstempel innerhalb der
  PowerShell-Testskripte selbst (nur manuell mit `novafs stat`
  nachvollzogen, siehe oben); eine Automatisierung bräuchte entweder
  einen `-NovaFsTool`-Parameter für `test-uefi-display-server.ps1` oder
  eine Zeitfenster-Toleranzprüfung in `test-uefi-novafs.ps1`.

## 107. Time Core ABI 1.0

Die neuen TIME-NPSPECs verlangen vor allem, dass monotone Zeit, Wall Clock,
Clock Sources und Clock Domains nicht vermischt werden. Der Kernel hatte
bereits `timer_ticks` aus PIT/IRQ0 und darauf aufbauende Deadline-Logik; jetzt
gibt es dafür eine explizite Time-Core-Schicht.

### Clock Source

`time_clock_sources` enthält zunächst eine Quelle:

```text
ClockSourceID 1
Provider      PIT
Frequenz      100 Hz
Auflösung     10 ms
Flags         REGISTERED | VALIDATED | ACTIVE | MONOTONIC | STABLE
```

Wichtig: Die PIT-Quelle wird nur als monotone Bootstrap-Zeitbasis verwendet.
Sie ist keine Wall Clock und keine Civil Time.

### Clock Domains

`time_clock_domains` enthält zwei getrennte Domains:

```text
1  Monotonic Kernel Time  Quelle PIT, 100 Hz, aktiv
2  Wall Clock             keine Quelle, Zustand Unknown
```

Damit ist die neue NPSPEC-Regel abgebildet, dass `Unknown` nicht als `0`,
`Valid` oder `Trusted` interpretiert werden darf. Die Wall Clock existiert als
Konzept, wird aber ohne RTC-/Sync-Provider nicht als gültige Systemzeit
veröffentlicht.

### API und Tests

Die interne `time_core_api` stellt bereit:

- `time_clock_source_lookup`
- `time_clock_domain_lookup`
- `time_clock_domain_compatible`
- `time_monotonic_now`

Der Boot-Selftest prüft:

- PIT-Source ist registriert, validiert, aktiv und monoton.
- Monotonic-Domain zeigt auf die PIT-Source und läuft mit 100 Hz.
- Wall-Clock-Domain bleibt getrennt und `Unknown`.
- Monotone Zeit läuft nicht rückwärts.
- Unterschiedliche Clock Domains werden nicht implizit kompatibel gemacht.

### Deadline-Anbindung

`TASK_DEADLINE_RECORD_SIZE` ist von 16 auf 32 Byte gewachsen. Neben absolutem
Ziel-Tick, Klasse, Policy und Zustand enthält ein Deadline-Record jetzt:

```text
ClockDomainID   aktuell immer Monotonic Kernel Time (1)
Tolerance       erlaubtes Coalescing-Fenster in monotonic ticks
EffectiveTick   absoluter Ziel-Tick plus zulässige Toleranz
MissTick        tatsächlicher Tick, an dem ein Miss erkannt wurde
```

Damit sind Deadline und tatsächliche Ausführung getrennt sichtbar. IO-Requests
erben nicht mehr blind den rohen Ziel-Tick, sondern den effektiven Wakeup-Tick.
Der Selftest prüft, dass Child-Deadlines an Parent-Deadlines geklemmt bleiben,
dass die monotone ClockDomainID erhalten bleibt und dass ein erkannter Miss den
tatsächlichen `MissTick` setzt.

### Coalescing

`task_deadline_set_tolerant` ergänzt die bisherige `task_deadline_set`-API um
eine explizite Toleranz. Die Bootstrap-Regel ist absichtlich konservativ:

- Hard Deadlines dürfen keine Toleranz besitzen und werden mit Toleranz
  abgewiesen.
- Von einem Hard-Parent geerbte Deadlines werden ebenfalls hart und verlieren
  ihre Toleranz.
- Firm, Soft und Advisory Deadlines dürfen einen `EffectiveTick` innerhalb des
  Fensters `AbsoluteTick + Tolerance` erhalten.
- `EffectiveTick` ist der Zeitpunkt, den Polling und IO-Vererbung verwenden;
  `AbsoluteTick` bleibt als ursprüngliche Anforderung erhalten.

Damit ist die erste Grundlage aus `NPSPEC-TIME-COALESCING-0001` umgesetzt,
ohne schon einen globalen Tickless-Planer oder echte Hardware-One-Shot-Timer
vorauszusetzen.

Bootausgabe:

```text
NOVA: Time Core ABI 1.0, Clock Source, Domains und Monotonic Introspection bereit
NOVA: Task Deadline ABI 1.0, ClockDomain, Toleranz und Miss-Introspection aktiv
```

### Artefakte

- Kernel Build-ID: `A7EE8DA708561CA55B3855AA79DC1AC702101540`
- NKI CRC32: `BAB3803F`
- IMG SHA256: `0D1B9EF5199291A04BC4882CA46FF1B5366D6903B3E39993377D6115A9C18C3E`
