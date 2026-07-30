# ADR-BOOT-0004: ELF als Kernel-Format

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1014
* **Titel:** ELF als Kernel-Format
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** NovaOS Bootloader, Kernel Loader und Kernel Build System
* **Verantwortlich:** Nova Platform Architecture
* **Ersetzt:** Keine
* **Ersetzt durch:** Keine

## Referenzen

* ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* NPSPEC-BOOTFS-0001
* NPSPEC-KERNELLOADER-0001
* NPSPEC-BOOTHANDOFF-0001
* NPSPEC-KERNELENTRY-0001
* NPSPEC-BOOTERROR-0001
* System V Application Binary Interface
* ELF-64 Object File Format
* x86-64 System V ABI

---

## 1. Kontext

NovaOS benötigt ein standardisiertes Format für die ausführbare Kerneldatei.

Der Bootloader muss den Kernel unabhängig von der verwendeten Firmware-Umgebung laden können. Dies betrifft insbesondere:

* BIOS-Systeme
* UEFI-Systeme
* physische Hardware
* virtuelle Maschinen
* Entwicklungs- und Debug-Umgebungen
* zukünftige Prozessorarchitekturen

Das Kernel-Format muss ausreichend Informationen enthalten, damit der Kernel Loader:

* die Datei validieren,
* die Zielarchitektur erkennen,
* die zu ladenden Segmente bestimmen,
* Speicherbereiche reservieren,
* Segmente an die vorgesehenen Adressen kopieren,
* nicht initialisierte Speicherbereiche anlegen,
* den Kernel-Einstiegspunkt ermitteln,
* Schutzattribute vorbereiten,
* und die Kontrolle sicher an den Kernel übergeben kann.

Ein einfaches flaches Binärformat enthält diese Informationen nicht. Bei einem Flat Binary müsste ein großer Teil der Speicheranordnung außerhalb der Kerneldatei fest im Bootloader hinterlegt werden.

Dies würde eine starke Kopplung zwischen Bootloader, Linker-Skript und Kernelimplementierung erzeugen.

NovaOS benötigt stattdessen ein Format, das:

* standardisiert,
* dokumentiert,
* erweiterbar,
* werkzeugfreundlich,
* architekturübergreifend,
* statisch analysierbar,
* und für Bare-Metal-Systeme geeignet ist.

---

## 2. Problemstellung

Es muss entschieden werden, welches Dateiformat der NovaOS-Kernel verwendet.

Die Entscheidung betrifft insbesondere:

* die Kerneldatei auf dem Bootmedium,
* den Kernel Loader,
* das Linker-Skript,
* das Build-System,
* Debugging-Werkzeuge,
* Symbolinformationen,
* zukünftiges dynamisches Laden,
* Kernelmodule,
* Recovery-Werkzeuge,
* Signatur- und Integritätsprüfungen,
* sowie die langfristige ABI- und Toolchain-Kompatibilität.

Das Format muss für die erste x86-Implementierung geeignet sein, darf die Architektur jedoch nicht dauerhaft auf x86 oder eine bestimmte Speicheranordnung beschränken.

---

## 3. Entscheidung

NovaOS verwendet **ELF – Executable and Linkable Format** als verbindliches Kernel-Dateiformat.

Für den initialen 32-Bit-Kernel wird verwendet:

```text
ELF32
```

Für spätere 64-Bit-Kernel wird verwendet:

```text
ELF64
```

Die Kerneldatei muss als ausführbares ELF-Image erzeugt werden.

Der Bootloader lädt den Kernel anhand seiner **Program Header Table**.

Die Section Header Table ist für den normalen Bootvorgang nicht erforderlich.

Der Kernel Loader darf daher nicht voraussetzen, dass Section Header vorhanden oder vollständig sind.

---

## 4. Verbindliche Festlegung

Der NovaOS-Kernel muss mindestens folgende ELF-Eigenschaften erfüllen:

```text
ELF Class:

ELF32 oder ELF64

Data Encoding:

Little Endian für x86 und x86-64

Object Type:

ET_EXEC oder ausdrücklich unterstütztes ET_DYN

Machine:

passend zur Zielarchitektur

Entry Point:

gültige Kernel-Einsprungadresse

Program Header Table:

vorhanden und gültig

Loadable Segments:

mindestens ein PT_LOAD-Segment
```

Für die erste Kernelgeneration gilt verbindlich:

```text
Class: ELF32

Data: Little Endian

Type: ET_EXEC

Machine: EM_386
```

Für die spätere x86-64-Variante gilt:

```text
Class: ELF64

Data: Little Endian

Type: ET_EXEC oder ET_DYN

Machine: EM_X86_64
```

---

## 5. Entscheidungsgründe

ELF wird gewählt, weil es die Anforderungen der NovaOS-Bootarchitektur am besten erfüllt.

Wesentliche Gründe sind:

* standardisierte Headerstruktur
* explizite Architekturkennung
* definierter Einstiegspunkt
* segmentbasierte Speicherbeschreibung
* Trennung von Datei- und Speichergröße
* Unterstützung nicht initialisierter Speicherbereiche
* Unterstützung von Zugriffsattributen
* gute Toolchain-Unterstützung
* gute Debugging-Unterstützung
* Erweiterbarkeit
* Eignung für statische und positionsunabhängige Images
* Unterstützung mehrerer Prozessorarchitekturen
* Eignung für Kernelmodule und Systemkomponenten

---

## 6. Segmentorientiertes Laden

Der Kernel Loader muss den Kernel anhand der Program Header laden.

Relevant sind insbesondere Einträge vom Typ:

```text
PT_LOAD
```

Für jedes ladbare Segment werden mindestens folgende Felder ausgewertet:

```text
p_offset

p_vaddr

p_paddr

p_filesz

p_memsz

p_flags

p_align
```

Die grundlegende Ladeoperation lautet:

```text
Kerneldatei[p_offset : p_offset + p_filesz]

↓

Zielspeicher[p_paddr oder abgeleitete Ladeadresse]
```

Falls gilt:

```text
p_memsz > p_filesz
```

muss der verbleibende Speicherbereich mit Null initialisiert werden.

Dies betrifft insbesondere:

```text
.bss
```

---

## 7. Program Header als Ladequelle

Die Program Header Table ist die alleinige normative Quelle für das Laden des Kernel-Images.

Der Bootloader darf nicht anhand von Section-Namen wie:

```text
.text

.rodata

.data

.bss
```

laden.

Section Header sind primär für:

* Linker
* Debugger
* Analysewerkzeuge
* Symbolverarbeitung

bestimmt.

Ein gültiger NovaOS-Kernel muss auch dann ladbar bleiben, wenn nicht benötigte Section Header aus der veröffentlichten Kerneldatei entfernt wurden.

---

## 8. Physische und virtuelle Adressen

ELF stellt für Segmente sowohl virtuelle als auch physische Adressinformationen bereit.

Relevant sind:

```text
p_vaddr

p_paddr
```

Der Kernel Loader muss gemäß der jeweiligen Architektur und Ladepolicy entscheiden, welche Adresse für den tatsächlichen Ladevorgang verwendet wird.

Für den initialen Kernel gilt:

* Der Kernel wird an die im Linker-Skript definierte physische Ladeadresse geladen.
* Virtuelle und physische Adresse dürfen in der frühen Entwicklungsphase identisch sein.
* Eine spätere Higher-Half-Kernel-Architektur darf unterschiedliche Werte verwenden.

Beispiel:

```text
Physische Ladeadresse:

0x00100000

Virtuelle Kerneladresse:

0xC0100000
```

Der Bootloader muss solche unterschiedlichen Adressmodelle künftig unterstützen können.

---

## 9. Einstiegspunkt

Der Kernel-Einstiegspunkt wird ausschließlich aus dem ELF-Header gelesen.

Relevant ist:

```text
e_entry
```

Der Bootloader darf die Einsprungadresse nicht dauerhaft fest im Quellcode hinterlegen.

Vor dem Sprung zum Kernel muss geprüft werden, dass:

* die Adresse innerhalb eines gültigen ausführbaren Segments liegt,
* das Segment erfolgreich geladen wurde,
* die Adresse korrekt ausgerichtet ist,
* die Adresse im aktuellen Prozessormodus erreichbar ist.

Nach erfolgreichem Boot-Handoff erfolgt der Kontrolltransfer an:

```text
e_entry
```

---

## 10. Speicherattribute

Die ELF-Segmentflags werden ausgewertet.

Unterstützt werden:

```text
PF_R

PF_W

PF_X
```

Die Flags beschreiben:

* lesbare Segmente
* beschreibbare Segmente
* ausführbare Segmente

In frühen Bootphasen dürfen die Attribute zunächst nur validiert und protokolliert werden.

Sobald Paging und Speicherschutz aktiv sind, müssen die ELF-Flags in geeignete Seitentabellenattribute übersetzt werden.

Beispiel:

```text
PF_R | PF_X

↓

Read + Execute
```

```text
PF_R | PF_W

↓

Read + Write
```

Writable-and-Executable-Speicherbereiche sollen vermieden werden.

---

## 11. Ausrichtung

Der Kernel Loader muss die durch:

```text
p_align
```

definierte Segmentausrichtung beachten.

Zulässige Werte sind:

```text
0

1

Zweierpotenz
```

Falls `p_align` größer als 1 ist, muss gelten:

```text
p_vaddr mod p_align = p_offset mod p_align
```

Ungültige Ausrichtungsangaben führen zum Abbruch des Kernel-Ladevorgangs.

---

## 12. ELF-Validierung

Vor dem Laden muss der Kernel Loader mindestens folgende Prüfungen durchführen:

### 12.1 Magic Number

Die ersten vier Bytes müssen lauten:

```text
0x7F 0x45 0x4C 0x46
```

Textdarstellung:

```text
0x7F E L F
```

### 12.2 ELF-Klasse

Die ELF-Klasse muss zur Loader-Implementierung passen.

Beispiele:

```text
ELFCLASS32

ELFCLASS64
```

### 12.3 Byte-Reihenfolge

Die Byte-Reihenfolge muss zur Zielarchitektur passen.

Für x86 gilt:

```text
ELFDATA2LSB
```

### 12.4 ELF-Version

Die ELF-Version muss unterstützt werden.

### 12.5 Dateityp

Der Dateityp muss unterstützt werden.

Zunächst:

```text
ET_EXEC
```

Optional später:

```text
ET_DYN
```

### 12.6 Zielarchitektur

Das Machine-Feld muss zur Zielarchitektur passen.

Beispiele:

```text
EM_386

EM_X86_64

EM_AARCH64

EM_RISCV
```

### 12.7 Headergrößen

Folgende Werte müssen plausibel sein:

```text
e_ehsize

e_phentsize

e_phnum
```

### 12.8 Dateigrenzen

Alle referenzierten Header und Segmente müssen vollständig innerhalb der Kerneldatei liegen.

### 12.9 Segmentgrößen

Für jedes ladbare Segment muss gelten:

```text
p_filesz <= p_memsz
```

### 12.10 Speichergrenzen

Kein Segment darf:

* reservierten Firmware-Speicher überschreiben,
* den Bootloader überschreiben,
* Boot-Handoff-Daten überschreiben,
* Seitentabellen überschreiben,
* andere Kernel-Segmente unerlaubt überlappen,
* außerhalb des adressierbaren physischen Speichers liegen.

---

## 13. Unterstützte Program-Header-Typen

Der Loader muss mindestens folgenden Typ unterstützen:

```text
PT_LOAD
```

Folgende Typen dürfen erkannt, aber zunächst ignoriert werden:

```text
PT_NULL

PT_NOTE

PT_PHDR

PT_GNU_STACK

PT_GNU_RELRO
```

Folgende Typen benötigen eine ausdrückliche spätere Erweiterung:

```text
PT_DYNAMIC

PT_INTERP

PT_TLS
```

Ein unbekannter Program-Header-Typ darf ignoriert werden, sofern er nicht als zwingend erforderlich markiert oder für die korrekte Ausführung notwendig ist.

---

## 14. ET_EXEC

Für die erste Kernelgeneration wird:

```text
ET_EXEC
```

verwendet.

Eigenschaften:

* feste virtuelle Adressen
* einfache Loader-Implementierung
* keine Laufzeitrelokation erforderlich
* gut für den frühen Bootloader geeignet
* deterministische Speicheranordnung

Dies reduziert die Komplexität des initialen Kernel Loaders.

---

## 15. ET_DYN und positionsunabhängiger Kernel

Eine spätere Version des Kernel Loaders darf:

```text
ET_DYN
```

unterstützen.

Dies ermöglicht:

* Position Independent Executables
* Kernel Address Space Layout Randomization
* flexible Ladeadressen
* bessere Speicherplatzierung
* konfliktfreie Recovery- und Testumgebungen

ET_DYN ist erst zulässig, wenn der Loader:

* eine Ladebasis bestimmen,
* Relokationen auswerten,
* Adressüberläufe erkennen,
* und alle erforderlichen Relocation-Typen sicher verarbeiten kann.

Bis dahin bleibt ET_EXEC verbindlicher Standard.

---

## 16. Relokationen

Der initiale Kernel muss ohne Bootloader-seitige Relokationen ladbar sein.

Der erste Loader muss daher keine Relocation Sections auswerten.

Spätere Loader dürfen Relokationen unterstützen.

Dabei müssen mindestens berücksichtigt werden:

* Zielarchitektur
* Relocation-Typ
* Symbolauflösung
* Addenden
* Ladebasis
* Speichergrenzen
* Überläufe

Nicht unterstützte Relokationen führen zu einem kontrollierten Ladefehler.

---

## 17. Symbole

Symboltabellen sind für den normalen Bootvorgang optional.

Relevante Sections können sein:

```text
.symtab

.strtab

.dynsym

.dynstr
```

Der veröffentlichte Kernel darf:

* vollständig mit Symbolen,
* teilweise mit Symbolen,
* oder vollständig stripped

vorliegen.

Für Debug- und Entwicklungs-Builds sollen Symbolinformationen erhalten bleiben.

Für Release-Builds können externe Debug-Symboldateien erzeugt werden.

---

## 18. Debugging

ELF ermöglicht die Verwendung etablierter Werkzeuge.

Beispiele:

```text
readelf

objdump

nm

gdb

llvm-readobj

llvm-objdump
```

Dadurch können unter anderem geprüft werden:

* ELF-Header
* Program Header
* Sections
* Symbole
* Disassembly
* Einstiegspunkt
* Speicheradressen
* Relokationen

ELF reduziert damit die Entwicklung eigener Analysewerkzeuge.

---

## 19. Linker-Skript

Die Speicheranordnung des Kernels wird durch ein eigenes Linker-Skript definiert.

Das Linker-Skript muss mindestens festlegen:

* Einstiegspunkt
* physische Ladeadresse
* virtuelle Kerneladresse
* Segmentanordnung
* Ausrichtung
* `.text`
* `.rodata`
* `.data`
* `.bss`

Beispiel:

```ld
ENTRY(kernel_entry)

SECTIONS
{
    . = 1M;

    .text ALIGN(4096) :
    {
        *(.text.boot)
        *(.text*)
    }

    .rodata ALIGN(4096) :
    {
        *(.rodata*)
    }

    .data ALIGN(4096) :
    {
        *(.data*)
    }

    .bss ALIGN(4096) :
    {
        *(COMMON)
        *(.bss*)
    }
}
```

Das konkrete Linker-Skript wird durch die Kernelarchitektur festgelegt und ist nicht vollständig Bestandteil dieses ADR.

---

## 20. Kerneldateiname

Der empfohlene Dateiname lautet:

```text
nova-kernel.elf
```

Architekturspezifische Varianten dürfen verwendet werden:

```text
nova-kernel-x86.elf

nova-kernel-x86_64.elf

nova-kernel-aarch64.elf

nova-kernel-riscv64.elf
```

Der Dateiname ist nicht Teil der Kernelidentität.

Die tatsächliche Kerneldatei wird durch Bootkonfiguration oder BootFS-Metadaten bestimmt.

---

## 21. BootFS-Integration

Die Kerneldatei wird im Nova Boot File System als typisiertes Bootartefakt geführt.

Beispiel:

```yaml
artifact:
  id: nova.kernel.primary
  type: kernel
  format: elf
  architecture: x86
  path: /boot/nova-kernel-x86.elf
  version: 0.1.0
```

Der Bootloader muss das deklarierte Format prüfen.

Eine Datei mit:

```text
format: elf
```

muss einen gültigen ELF-Header besitzen.

---

## 22. Boot-Handoff

ELF beschreibt ausschließlich das Kernel-Image.

ELF ersetzt nicht das Nova Boot-Handoff-Protokoll.

Das Boot-Handoff-Protokoll überträgt zusätzliche Informationen wie:

* Speicherkarte
* Framebuffer
* ACPI
* SMBIOS
* Bootlaufwerk
* Kernelargumente
* Module
* Sicherheitsstatus
* Firmwaretyp
* Bootfehlerhistorie
* Recoveryinformationen

Der Kernel-Einstiegspunkt erhält diese Informationen gemäß:

```text
ADR-BOOT-1012
```

und:

```text
NPSPEC-BOOTHANDOFF-0001
```

---

## 23. Kernelmodule

ELF soll später ebenfalls als Standardformat für Kernelmodule verwendet werden.

Kernelmodule können als:

```text
ET_REL
```

vorliegen.

Ein Modullader muss dann unter anderem unterstützen:

* Section Loading
* Symbol Resolution
* Relocations
* Dependency Resolution
* Signaturprüfung
* Versionsprüfung

Die Verwendung von ELF für Kernelmodule wird durch eine separate Spezifikation festgelegt.

---

## 24. Integritätsprüfung

Vor dem Parsen des ELF-Inhalts muss die Integrität der vollständigen Kerneldatei geprüft werden, sofern ein Hash vorhanden ist.

Unterstützte Mechanismen können sein:

* SHA-256
* SHA-512
* Merkle-Tree
* digitale Signatur
* TPM-basierte Messung

Die ELF-Validierung ersetzt keine kryptografische Integritätsprüfung.

Ein formal korrektes ELF-Image kann dennoch manipuliert sein.

---

## 25. Signaturprüfung

Der Kernel darf als vollständiges ELF-Artefakt signiert werden.

Die Signatur wird nicht zwingend direkt in standardfremde ELF-Felder eingebettet.

Bevorzugte Varianten sind:

```text
separate Signaturdatei

BootFS-Metadaten

signiertes Boot-Manifest

ELF Note Segment
```

Die konkrete Signaturarchitektur wird in einer separaten Security-Spezifikation definiert.

---

## 26. ELF Notes

ELF Notes dürfen für Nova-spezifische Metadaten verwendet werden.

Mögliche Inhalte:

* NovaOS-Kernelversion
* Build-ID
* ABI-Version
* Boot-Handoff-Version
* erforderliche CPU-Features
* erforderliche Loader-Version
* Sicherheitsinformationen
* Debug-Build-Kennung

Nova-spezifische Notes müssen einen eindeutig definierten Namespace verwenden.

Beispiel:

```text
NOVA
```

Der Loader darf unbekannte Notes ignorieren, sofern sie nicht als zwingend gekennzeichnet sind.

---

## 27. Erforderliche CPU-Eigenschaften

Der Kernel darf erforderliche CPU-Eigenschaften über Boot-Metadaten oder ELF Notes deklarieren.

Beispiele:

```text
SSE2

PAE

NX

Long Mode

AVX

Virtualization
```

Der Bootloader muss zwingende Anforderungen vor dem Kernelstart prüfen.

Fehlende zwingende CPU-Eigenschaften führen zu einem kontrollierten Bootfehler.

---

## 28. Architekturabhängigkeit

ELF wird architekturübergreifend verwendet.

Architekturspezifisch bleiben:

* ELF-Klasse
* Machine-ID
* Endianness
* Relocation-Typen
* Aufrufkonvention
* Registerzustand
* Seitentabellen
* Einstiegspunktprotokoll

Die Verwendung von ELF vereinheitlicht das Dateiformat, ersetzt jedoch keine architekturspezifische Bootlogik.

---

## 29. BIOS- und UEFI-Kompatibilität

Sowohl der BIOS- als auch der UEFI-Bootpfad müssen denselben ELF-Kernel laden können.

Die Firmwareumgebung darf keinen Einfluss auf das Kerneldateiformat haben.

```text
BIOS Loader
        \
         → nova-kernel.elf
        /
UEFI Loader
```

Beide Loader müssen dieselben:

* ELF-Validierungsregeln,
* Segmentregeln,
* Speicherregeln,
* und Boot-Handoff-Regeln

anwenden.

Dadurch bleibt der Kernel von der Firmwareumgebung entkoppelt.

---

## 30. Fehlerbehandlung

Der Kernel Loader muss ELF-Fehler eindeutig klassifizieren.

Mindestens folgende Fehlercodes sind vorzusehen:

```text
BOOT_ELF_INVALID_MAGIC

BOOT_ELF_UNSUPPORTED_CLASS

BOOT_ELF_UNSUPPORTED_ENDIANNESS

BOOT_ELF_UNSUPPORTED_VERSION

BOOT_ELF_UNSUPPORTED_TYPE

BOOT_ELF_WRONG_ARCHITECTURE

BOOT_ELF_INVALID_HEADER_SIZE

BOOT_ELF_INVALID_PROGRAM_HEADER

BOOT_ELF_NO_LOADABLE_SEGMENT

BOOT_ELF_INVALID_ENTRY_POINT

BOOT_ELF_SEGMENT_OUT_OF_FILE

BOOT_ELF_SEGMENT_OUT_OF_MEMORY

BOOT_ELF_SEGMENT_OVERLAP

BOOT_ELF_INVALID_ALIGNMENT

BOOT_ELF_FILE_SIZE_EXCEEDS_MEMORY_SIZE

BOOT_ELF_UNSUPPORTED_RELOCATION

BOOT_ELF_INTEGRITY_FAILED

BOOT_ELF_SIGNATURE_FAILED
```

Fehler müssen an das Bootfehler-System weitergereicht werden.

---

## 31. Diagnostik

Bei einem ELF-Ladefehler sollen mindestens folgende Informationen verfügbar sein:

```text
Fehlercode

Kernelpfad

Dateigröße

ELF-Klasse

Machine-ID

Program-Header-Index

Segmentadresse

Segmentgröße

Einstiegspunkt
```

Sensible Sicherheitsinformationen dürfen in produktiven Fehlermeldungen reduziert werden.

---

## 32. Sicherheitsanforderungen

Der ELF-Parser ist Teil der vertrauenswürdigen Bootkette.

Er muss daher besonders defensiv implementiert werden.

Verbindliche Anforderungen:

* keine ungeprüfte Zeigerarithmetik
* keine Integer-Überläufe
* keine Dateigrenzüberschreitungen
* keine Speichergrenzüberschreitungen
* keine ungeprüften Multiplikationen von Headeranzahl und Headergröße
* keine Überlappung mit Bootloaderdaten
* keine Ausführung vor vollständiger Validierung
* keine Verwendung nicht validierter Adressen

Alle Größen- und Offsetberechnungen müssen auf Überlauf geprüft werden.

---

## 33. Parser-Komplexität

Der Bootloader soll nur den für das Laden erforderlichen ELF-Teil implementieren.

Zwingend erforderlich sind:

* ELF Header
* Program Header Table
* PT_LOAD-Segmente
* Einstiegspunkt

Nicht zwingend erforderlich sind:

* vollständige Section-Auswertung
* Symbolauflösung
* dynamisches Linking
* Debug-Informationen
* Relokationen bei ET_EXEC

Dadurch bleibt der frühe Loader klein und überprüfbar.

---

## 34. Build-Anforderungen

Das Kernel-Build-System muss:

* eine gültige ELF-Datei erzeugen,
* das korrekte Linker-Skript verwenden,
* die Zielarchitektur korrekt setzen,
* einen gültigen Einstiegspunkt definieren,
* mindestens ein PT_LOAD-Segment erzeugen,
* die Segmente korrekt ausrichten,
* und die resultierende Datei automatisch validieren.

Empfohlene Prüfkommandos:

```bash
readelf -h nova-kernel.elf
readelf -l nova-kernel.elf
objdump -h nova-kernel.elf
nm nova-kernel.elf
```

---

## 35. Automatische Build-Validierung

Der Build muss fehlschlagen, wenn:

* das Ergebnis kein ELF-Image ist,
* die falsche Architektur enthalten ist,
* kein Einstiegspunkt vorhanden ist,
* kein PT_LOAD-Segment vorhanden ist,
* Segmente ungültig überlappen,
* die Ladeadresse außerhalb des erlaubten Bereichs liegt,
* oder die Kerneldatei die konfigurierte Maximalgröße überschreitet.

---

## 36. Testanforderungen

Der Kernel Loader muss mindestens mit folgenden Testfällen geprüft werden:

### 36.1 Gültige Kerneldatei

Ein korrektes ELF-Image wird erfolgreich geladen und gestartet.

### 36.2 Falsche Magic Number

Das Image wird abgelehnt.

### 36.3 Falsche Architektur

Ein x86-64-Kernel wird von einem reinen x86-Loader abgelehnt.

### 36.4 Abgeschnittene Datei

Header oder Segmente außerhalb der Datei werden erkannt.

### 36.5 Ungültige Segmentgröße

`p_filesz > p_memsz` wird abgelehnt.

### 36.6 Überlappende Segmente

Unzulässige Überlappungen werden erkannt.

### 36.7 Ungültiger Einstiegspunkt

Ein Einstiegspunkt außerhalb eines ausführbaren Segments wird abgelehnt.

### 36.8 BSS-Initialisierung

Der Bereich zwischen `p_filesz` und `p_memsz` wird vollständig genullt.

### 36.9 Fehlende Program Header

Das Image wird abgelehnt.

### 36.10 Unbekannte optionale Header

Das Image bleibt ladbar, sofern die unbekannten Informationen nicht zwingend erforderlich sind.

### 36.11 Beschädigte Signatur

Der Bootvorgang wird bei aktivierter Signaturprüfung abgebrochen.

### 36.12 Grenzwerttests

Maximale Offsets, Größen und Headeranzahlen werden auf Integer-Überläufe geprüft.

---

## 37. Fuzzing

Der ELF-Parser soll mit automatisch erzeugten und mutierten ELF-Dateien getestet werden.

Fuzzing-Ziele sind insbesondere:

* Headerparser
* Program-Header-Auswertung
* Offsetberechnung
* Größenberechnung
* Segmentüberlappung
* Einstiegspunktprüfung
* Note-Parser
* Relocation-Parser bei späterer Unterstützung

Der Parser darf bei ungültigen Eingaben weder:

* abstürzen,
* hängen bleiben,
* Speicher außerhalb erlaubter Bereiche verändern,
* noch unvalidierten Code ausführen.

---

## 38. Performance

Die ELF-Verarbeitung erfolgt einmalig während des Bootvorgangs.

Daher besitzt Sicherheit Vorrang vor maximaler Parsergeschwindigkeit.

Dennoch soll der Loader:

* sequenzielles Lesen unterstützen,
* unnötige Dateikopien vermeiden,
* Segmente direkt an die Zieladresse laden,
* und bei geeigneten Dateisystemen größere Blocktransfers verwenden.

---

## 39. Speicherbedarf des Loaders

Der Loader muss nicht zwingend die vollständige Kerneldatei in einen temporären Speicher laden.

Er darf:

1. den ELF-Header lesen,
2. die Program Header lesen,
3. Segmente direkt aus dem Bootmedium an ihre Zieladressen übertragen.

Dadurch kann auch auf speicherarmen Systemen ein größerer Kernel geladen werden.

---

## 40. Alternativen

### 40.1 Flat Binary

Ein Flat Binary enthält ausschließlich rohe Maschinencodes und Daten.

Vorteile:

* sehr einfacher Loader
* minimale Dateigröße
* geringer Parseraufwand

Nachteile:

* kein standardisierter Header
* kein eingebetteter Einstiegspunkt
* keine Segmentinformationen
* keine Architekturkennung
* keine Speicherattribute
* keine BSS-Beschreibung
* starke Kopplung an feste Ladeadressen
* schlechte Debugging-Unterstützung
* schlechte Erweiterbarkeit

**Entscheidung:** Abgelehnt.

Flat Binaries dürfen weiterhin für frühe Bootstufen verwendet werden, jedoch nicht als primäres Kernel-Format.

---

### 40.2 PE/COFF

PE/COFF wird insbesondere von Windows und UEFI verwendet.

Vorteile:

* gute UEFI-Integration
* etablierte Toolchains
* Unterstützung von Relokationen
* Signaturmechanismen im Windows-Umfeld

Nachteile:

* stärkere Bindung an Windows- und UEFI-Ökosysteme
* weniger natürlich für Unix-artige und Bare-Metal-Kernel
* BIOS-Pfad benötigt weiterhin eigenen Parser
* für NovaOS keine funktionalen Vorteile gegenüber ELF
* weniger passend für die geplante Kernel- und Modularchitektur

**Entscheidung:** Als primäres Kernel-Format abgelehnt.

Der UEFI-Bootloader selbst kann weiterhin als PE/COFF-Anwendung vorliegen, da dies von UEFI verlangt wird. Der von ihm geladene NovaOS-Kernel bleibt ELF.

---

### 40.3 Mach-O

Mach-O wird primär im Apple-Ökosystem verwendet.

Vorteile:

* segmentbasiert
* gute Debugging- und Toolchain-Unterstützung im Apple-Umfeld

Nachteile:

* geringe Relevanz außerhalb des Apple-Ökosystems
* schlechtere allgemeine Toolchain-Verfügbarkeit
* keine Vorteile für NovaOS

**Entscheidung:** Abgelehnt.

---

### 40.4 Eigenes Kernel-Format

NovaOS könnte ein eigenes Dateiformat definieren.

Vorteile:

* vollständige Kontrolle
* exakt auf NovaOS zugeschnitten
* beliebige Metadaten integrierbar

Nachteile:

* hoher Entwicklungs- und Testaufwand
* eigene Linker- oder Konvertierungswerkzeuge erforderlich
* schlechtere Debugger-Unterstützung
* zusätzliche Sicherheitsrisiken
* langfristige Wartungskosten
* unnötige Abweichung von etablierten Standards

**Entscheidung:** Abgelehnt.

Nova-spezifische Informationen werden über Boot-Manifeste, ELF Notes oder das Boot-Handoff-Protokoll ergänzt.

---

### 40.5 Multiboot-kompatibles ELF

Ein ELF-Kernel könnte ausschließlich gemäß Multiboot oder Multiboot2 aufgebaut werden.

Vorteile:

* etablierte Bootloader-Unterstützung
* einfache Nutzung mit GRUB
* vorhandene Testmöglichkeiten

Nachteile:

* Abhängigkeit von einem extern definierten Bootprotokoll
* Einschränkung der eigenen Nova-Handoff-Architektur
* nicht erforderlich für den eigenen Bootloader

**Entscheidung:** Nicht als primäres Bootprotokoll übernommen.

Eine optionale Multiboot-Kompatibilität für Entwicklung und Tests bleibt möglich.

---

## 41. Konsequenzen

### 41.1 Positive Konsequenzen

* Der Kernel Loader kann Segmente standardisiert laden.
* Kernel und Bootloader bleiben weniger stark gekoppelt.
* Der Einstiegspunkt ist Teil des Kernelartefakts.
* BSS-Bereiche können korrekt initialisiert werden.
* Debugging mit etablierten Werkzeugen ist möglich.
* Das Format unterstützt mehrere Architekturen.
* Eine spätere Higher-Half-Architektur ist möglich.
* Positionsunabhängige Kernel können später ergänzt werden.
* Kernelmodule können dasselbe Grundformat verwenden.
* Sicherheits- und Build-Prüfungen lassen sich automatisieren.
* BIOS und UEFI können denselben Kernel verwenden.

### 41.2 Negative Konsequenzen

* Der Bootloader benötigt einen ELF-Parser.
* Die Loader-Implementierung ist komplexer als bei Flat Binaries.
* Alle Offset- und Größenangaben müssen sicher validiert werden.
* Unterschiedliche ELF-Klassen benötigen architekturspezifische Behandlung.
* ET_DYN und Relokationen erhöhen später die Komplexität.
* Fehlerhafte oder unvollständige ELF-Validierung kann sicherheitskritisch sein.

### 41.3 Neutrale Konsequenzen

* Der UEFI-Bootloader bleibt selbst eine PE/COFF-Datei.
* Frühe Bootstufen können weiterhin Flat Binaries verwenden.
* ELF definiert nicht das Nova Boot-Handoff-Protokoll.
* ELF definiert nicht die interne Kernel-ABI.

---

## 42. Implementierungsstrategie

Die Einführung erfolgt stufenweise.

### Phase 1: Minimaler ELF32-Loader

Unterstützt:

```text
ELF32

Little Endian

ET_EXEC

EM_386

PT_LOAD
```

Keine Unterstützung für:

```text
Relokationen

Dynamic Linking

TLS

Interpreter

ELF64
```

### Phase 2: Erweiterte Validierung

Ergänzt:

* umfassende Bereichsprüfungen
* Segmentflags
* Build-ID
* ELF Notes
* Integritätsprüfung
* Signaturprüfung

### Phase 3: ELF64

Ergänzt:

```text
ELF64

EM_X86_64

64-Bit-Adressen

64-Bit-Größen
```

### Phase 4: Positionsunabhängiger Kernel

Ergänzt:

```text
ET_DYN

Relokationen

zufällige Ladebasis

KASLR
```

### Phase 5: Weitere Architekturen

Mögliche Zielarchitekturen:

```text
AArch64

RISC-V 64
```

---

## 43. Empfohlene Loader-Schnittstelle

Eine mögliche interne Schnittstelle lautet:

```c
typedef enum
{
    NOVA_ELF_OK = 0,
    NOVA_ELF_INVALID_MAGIC,
    NOVA_ELF_UNSUPPORTED_CLASS,
    NOVA_ELF_UNSUPPORTED_ENDIANNESS,
    NOVA_ELF_UNSUPPORTED_TYPE,
    NOVA_ELF_WRONG_MACHINE,
    NOVA_ELF_INVALID_HEADER,
    NOVA_ELF_INVALID_SEGMENT,
    NOVA_ELF_OUT_OF_MEMORY,
    NOVA_ELF_INVALID_ENTRY
} nova_elf_result_t;
```

```c
typedef struct
{
    uintptr_t entry_point;
    uintptr_t physical_start;
    uintptr_t physical_end;
    uint32_t loaded_segment_count;
} nova_loaded_kernel_t;
```

```c
nova_elf_result_t nova_elf_load_kernel(
    const void* file_data,
    size_t file_size,
    nova_loaded_kernel_t* result
);
```

Die konkrete API wird in NPSPEC-KERNELLOADER-0001 festgelegt.

---

## 44. Beispielhafter Ladealgorithmus

```text
1. Kerneldatei öffnen.

2. ELF-Header lesen.

3. Magic Number validieren.

4. Klasse, Endianness, Typ und Architektur validieren.

5. Program Header Table validieren.

6. Alle PT_LOAD-Segmente vorab prüfen.

7. Speicherbereiche auf Kollisionen prüfen.

8. Jedes PT_LOAD-Segment an die Zieladresse kopieren.

9. Bereich zwischen p_filesz und p_memsz nullen.

10. Instruction Cache synchronisieren, falls erforderlich.

11. Einstiegspunkt validieren.

12. Boot-Handoff-Struktur finalisieren.

13. Firmwaredienste beenden, sofern erforderlich.

14. Prozessorzustand vorbereiten.

15. Zu e_entry springen.
```

Die Validierung aller Segmente muss vor dem ersten irreversiblen Kontrolltransfer abgeschlossen sein.

---

## 45. Beispiel eines erwarteten ELF-Layouts

```text
ELF Header

Program Header Table

PT_LOAD: Kernel Text
    Read
    Execute

PT_LOAD: Read-Only Data
    Read

PT_LOAD: Kernel Data
    Read
    Write

PT_LOAD: BSS
    Read
    Write
    p_memsz > p_filesz
```

Ein mögliches Speicherlayout:

```text
0x00100000  Kernel Text

0x00120000  Read-Only Data

0x00130000  Initialized Data

0x00140000  BSS

0x00180000  Kernel End
```

Die konkreten Adressen werden durch das Linker-Skript bestimmt.

---

## 46. Kompatibilitätsregeln

Ein Kernel-Image ist mit einem Loader kompatibel, wenn:

* ELF-Klasse unterstützt wird,
* Zielarchitektur übereinstimmt,
* Endianness unterstützt wird,
* Dateityp unterstützt wird,
* alle zwingenden ELF-Features unterstützt werden,
* die Kernel-Handoff-Version unterstützt wird,
* und alle erforderlichen CPU-Eigenschaften vorhanden sind.

Ein neuer Kernel darf keine Loader-Funktion voraussetzen, ohne dies über Metadaten oder Versionsanforderungen zu deklarieren.

---

## 47. Versionsstrategie

Das ELF-Format selbst wird nicht durch NovaOS neu versioniert.

Nova-spezifische Erweiterungen erhalten eigene Versionsfelder.

Beispiele:

```text
Nova Kernel ABI Version

Nova Boot-Handoff Version

Nova ELF Note Version

Required Loader Version
```

Diese Versionen dürfen nicht mit der ELF-Standardversion verwechselt werden.

---

## 48. Rückwärtskompatibilität

Ein neuer Loader soll ältere gültige NovaOS-ELF-Kernel weiterhin laden können.

Ein älterer Loader darf einen neueren Kernel kontrolliert ablehnen, wenn dieser nicht unterstützte zwingende Funktionen benötigt.

Die Ablehnung muss einen eindeutigen Fehler liefern.

Beispiel:

```text
BOOT_KERNEL_REQUIRES_NEWER_LOADER
```

---

## 49. Dokumentationsanforderungen

Jeder veröffentlichte Kernel-Build soll dokumentieren:

* ELF-Klasse
* Architektur
* Dateityp
* Einstiegspunkt
* physische Ladeadresse
* virtuelle Basisadresse
* Boot-Handoff-Version
* erforderliche Loader-Version
* erforderliche CPU-Features
* Build-ID
* Integritätswert

---

## 50. Konformitätskriterien

Eine Implementierung erfüllt dieses ADR, wenn:

* der NovaOS-Kernel als ELF-Datei erzeugt wird,
* der Bootloader die ELF-Header validiert,
* der Loader ausschließlich PT_LOAD-Segmente lädt,
* `p_filesz` und `p_memsz` korrekt behandelt werden,
* BSS-Speicher genullt wird,
* der Einstiegspunkt aus `e_entry` gelesen wird,
* die Zielarchitektur geprüft wird,
* alle Datei- und Speichergrenzen validiert werden,
* BIOS und UEFI denselben ELF-Kernel laden können,
* und der Loader bei ungültigen Dateien kontrolliert abbricht.

---

## 51. Entscheidungsergebnis

ELF wird als verbindliches Kernel-Format für NovaOS angenommen.

Für die erste Implementierung gilt:

```text
ELF32

Little Endian

ET_EXEC

EM_386

PT_LOAD-basiertes Laden
```

Spätere Erweiterungen dürfen umfassen:

```text
ELF64

ET_DYN

Relokationen

KASLR

weitere Prozessorarchitekturen
```

Die Erweiterungen müssen rückwärtskompatibel und durch separate Spezifikationen geregelt sein.

---

## 52. Zusammenfassung

NovaOS verwendet ELF als standardisiertes Format für Kernel-Images.

ELF stellt alle für den Bootloader erforderlichen Informationen bereit:

* Architektur
* Einstiegspunkt
* Segmente
* Speichergrößen
* Ladeadressen
* Ausrichtung
* Zugriffsattribute

Die Entscheidung reduziert die Kopplung zwischen Kernel und Bootloader, verbessert Debugging und Toolchain-Kompatibilität und schafft eine stabile Grundlage für spätere Erweiterungen wie ELF64, Higher-Half-Kernel, Kernelmodule, positionsunabhängige Kernel und zusätzliche Prozessorarchitekturen.

Flat Binaries bleiben auf frühe Bootstufen beschränkt. Der eigentliche NovaOS-Kernel wird ausschließlich als validiertes ELF-Image geladen.
