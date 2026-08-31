# ADR-BOOT-0004 – NKI bevorzugt, ELF als Kernel-Ladeform

## Status

Angenommen

## Kategorie

Boot / Kernel Loading / Executable Format / ELF / ABI / Speicherlayout / Architekturportabilität

## Kontext

Der NovaOS-Bootloader benötigt ein definiertes natives Format, um den Kernel zuverlässig zu laden und an den Kernel Entry Point zu übergeben.

Ein Kernelimage ist mehr als ein flacher Binärblock.

Der Bootloader muss mindestens Informationen bestimmen können über:

- Zielarchitektur,
- Entry Point,
- ladbare Segmente,
- virtuelle und physische Adressanforderungen,
- Speichergrößen,
- Zugriffsattribute,
- Alignment,
- nicht initialisierte Datenbereiche.

Ein reines Flat Binary enthält solche Informationen nicht oder nur über externe implizite Konventionen.

Dies würde dazu führen, dass Bootloader und Kernelbuild sehr eng an ein fest codiertes Speicherlayout gekoppelt wären.

Beispiel:

```text
kernel.bin
    ↓
load exactly at 0x00100000
    ↓
jump to hard-coded address
```

Dieses Modell ist für frühe Prototypen einfach, skaliert jedoch schlecht für:

- 32- und 64-Bit-Kernel,
- unterschiedliche Architekturen,
- veränderbare Kernel-Layouts,
- mehrere Segmente,
- getrennte Code-/Data-Berechtigungen,
- Address Space Layout,
- Debugging und Symbolinformationen,
- zukünftige Kernelvarianten.

NovaOS benötigt deshalb ein strukturiertes natives Kernel-Ladeformat.

---

## Entscheidung

NovaOS verwendet bevorzugt das **Nova Kernel Image (NKI)**. ELF – Executable and Linkable Format – bleibt als direkt ladbare Kernel-Ladeform und als NKI-Payload unterstützt.

Der Bootloader lädt den Kernel anhand der ELF-Program-Header und nicht anhand fest codierter Dateioffsets oder eines einzigen pauschalen Ladebereichs.

Das Grundmodell lautet:

```text
Kernel ELF Image
      │
      ▼
ELF Validation
      │
      ▼
Program Headers
      │
      ▼
PT_LOAD Segments
      │
      ▼
Memory Placement
      │
      ▼
Kernel Entry Point
      │
      ▼
Kernel Handoff
```

ELF beschreibt die Ladeform des Kernels.

Die semantische Kernel-Handoff-Schnittstelle bleibt davon getrennt.

---

## Grundprinzip

Es gilt:

```text
The kernel image describes
how it must be loaded.
```

und:

```text
Do not encode the kernel memory layout
as hidden bootloader knowledge.
```

---

## Native Ladeform

NKI wird als bevorzugtes Nova-Produktionsformat verwendet; ELF bleibt eine vollwertige direkt ladbare Kernel-Ladeform.

Das bedeutet insbesondere:

```text
Bootloader
    ↓
ELF Loader
    ↓
Native NovaOS Kernel
```

Dies bedeutet nicht, dass sämtliche NovaOS-Programme zwingend ELF verwenden müssen.

Applikations-, Capability-, Modul- oder Treiberformate werden separat definiert.

---

## ELF als Ladecontainer

Für den Bootloader sind primär die ELF-Informationen relevant, die den ausführbaren Speicheraufbau definieren.

Insbesondere:

```text
ELF Header
Program Header Table
Loadable Segments
Entry Point
```

Section Headers sind für das grundlegende Kernel-Loading nicht erforderlich.

---

## Program Header statt Section Header

Der Bootloader lädt den Kernel anhand der **Program Header Table**.

Insbesondere werden Segmente vom Typ:

```text
PT_LOAD
```

verarbeitet.

Section Headers wie:

```text
.text
.data
.bss
.rodata
```

sind Linker- und Entwicklungsabstraktionen.

Sie dürfen für Debugging oder Diagnose vorhanden sein, sind aber nicht die normative Grundlage des Bootloaders.

---

## ELF Header Validation

Vor der Verwendung eines Kernelimages muss der ELF Header validiert werden.

Mindestens zu prüfen sind:

- ELF Magic,
- ELF Class,
- Endianness,
- Version,
- Machine Architecture,
- Executable Type,
- Program Header Offset,
- Program Header Size,
- Program Header Count,
- Entry Point,
- Dateigrößenbeziehungen.

Ungültige Header führen zu einem expliziten Bootfehler.

---

## ELF Magic

Das Image muss die gültige ELF-Kennung besitzen.

Konzeptionell:

```text
0x7F 'E' 'L' 'F'
```

Ein Image ohne gültige ELF-Kennung wird nicht als Kernelimage interpretiert.

---

## ELF Class

NovaOS muss die ELF-Klasse entsprechend der Zielarchitektur validieren.

Beispiele:

```text
ELFCLASS32
ELFCLASS64
```

Ein 64-Bit-Kernel darf nicht versehentlich über einen inkompatiblen 32-Bit-Laderpfad gestartet werden.

---

## Zielarchitektur

Der `e_machine`-Wert muss zur ausgewählten Kernelarchitektur passen.

Beispiele:

```text
EM_386
EM_X86_64
EM_AARCH64
EM_RISCV
```

Die tatsächlich unterstützten Architekturen werden durch jeweilige Boot- und HAL-Spezifikationen festgelegt.

---

## Endianness

Die im ELF-Header angegebene Byte-Reihenfolge muss unterstützt werden.

Ein nicht unterstütztes Endianness-Modell führt zu einem expliziten Fehler.

NovaOS darf nicht stillschweigend native Endianness voraussetzen, ohne sie zu prüfen.

---

## ELF Type

Der native Kernel muss einen von NovaOS unterstützten ELF-Typ verwenden.

Primär vorgesehen sind:

```text
ET_EXEC
```

und zukünftig optional:

```text
ET_DYN
```

für relocatable beziehungsweise positionierbare Kernelimages.

Die exakte Unterstützung wird nachgelagert spezifiziert.

---

## ET_EXEC

`ET_EXEC` ist der Baseline-Kerneltyp.

Hierbei enthält das ELF-Image bereits ein definiertes virtuelles beziehungsweise logisches Ladeadressmodell.

Der Bootloader lädt die Segmente entsprechend den validierten Program Headers.

---

## ET_DYN und PIE-Kernel

NovaOS darf zukünftig einen relocatable beziehungsweise Position Independent Kernel unterstützen.

Dies kann auf `ET_DYN` basieren.

Damit werden Konzepte wie:

```text
Kernel ASLR
```

möglich.

Ein solcher Modus ist jedoch keine Voraussetzung der ersten Implementierung.

---

## PT_LOAD

Nur explizit unterstützte Program-Header-Typen dürfen Einfluss auf den Kernel-Ladevorgang haben.

`PT_LOAD` beschreibt einen in den Kerneladressraum zu ladenden Bereich.

Beispiel:

```text
PT_LOAD
    Offset      = file offset
    FileSize    = bytes from image
    MemorySize  = bytes required in memory
    VirtualAddr = target virtual address
    Flags       = R/W/X
    Align       = alignment
```

---

## Segment Loading

Für jedes gültige `PT_LOAD`-Segment erfolgt konzeptionell:

```text
Validate Segment
      ↓
Reserve Memory
      ↓
Copy File Bytes
      ↓
Zero Remaining Memory
      ↓
Apply Mapping Attributes
```

Dabei gilt:

```text
p_memsz >= p_filesz
```

Ein Segment mit:

```text
p_filesz > p_memsz
```

ist ungültig.

---

## BSS

Ein Segment kann mehr Speicher benötigen, als im Kernelimage Dateidaten vorhanden sind.

Beispiel:

```text
p_filesz = 64 KiB
p_memsz  = 96 KiB
```

Dann gilt:

```text
64 KiB load from file
32 KiB initialize with zero
```

Dadurch werden nicht initialisierte Datenbereiche wie `.bss` korrekt erzeugt.

---

## Segmentberechtigungen

ELF-Segmentflags werden in geeignete Memory Permissions übersetzt.

Typische Flags:

```text
PF_R
PF_W
PF_X
```

Daraus können beispielsweise resultieren:

```text
Kernel Code:
    Read + Execute

Kernel ReadOnly Data:
    Read

Kernel Data:
    Read + Write
```

---

## W^X

NovaOS soll nach Möglichkeit das Prinzip:

```text
Writable XOR Executable
```

verwenden.

Speicherbereiche sollen nicht gleichzeitig beschreibbar und ausführbar sein, sofern der Kernel keine explizite Ausnahme benötigt.

Ein ELF-Segment mit ungewöhnlichen `W+X`-Rechten soll validiert, protokolliert und gegebenenfalls gemäß Security Policy abgelehnt werden können.

---

## Alignment

`p_align` muss berücksichtigt und validiert werden.

Segmente müssen so geladen beziehungsweise gemappt werden, dass die durch ELF und Architektur geforderten Alignment-Regeln erfüllt sind.

Ungültige Alignmentwerte dürfen nicht zu undefiniertem Speicherverhalten führen.

---

## Datei-Grenzen

Jeder Segmentbereich im ELF-Image muss vollständig innerhalb der tatsächlichen Imagegröße liegen.

Es gilt:

```text
p_offset + p_filesz <= file_size
```

unter Verwendung overflow-sicherer Berechnungen.

Ein manipuliertes Image darf keinen Out-of-Bounds-Zugriff im Bootloader auslösen.

---

## Integer Overflow

Alle Berechnungen für:

- Offsets,
- Größen,
- Adressen,
- Segmentenden,
- Tabellenpositionen

müssen auf Integer Overflow geprüft werden.

Beispiel:

```text
base + size
```

darf nicht ungeprüft verwendet werden.

---

## Program Header Table

Die Program Header Table muss vollständig innerhalb des Images liegen.

Zu validieren sind mindestens:

```text
e_phoff
e_phentsize
e_phnum
```

und die daraus resultierende Gesamtgröße.

---

## Begrenzung der Headeranzahl

Der Bootloader darf eine Implementierungsgrenze für die maximale Anzahl von Program Headers besitzen.

Diese Grenze muss groß genug für reale Kernelimages sein, aber vor absichtlich extremen oder korrupten Images schützen.

Ein Überschreiten führt zu einem expliziten Fehler.

---

## Entry Point

Der ELF Header enthält den Kernel Entry Point.

Der Bootloader darf nicht zu einer separat hardcodierten Kerneladresse springen.

Es gilt:

```text
KernelEntry = e_entry
```

nach erfolgreicher Validierung.

---

## Entry-Point-Validierung

Der Entry Point muss auf einen gültigen ausführbaren Kernelbereich verweisen.

Insbesondere soll geprüft werden, dass:

```text
e_entry
```

innerhalb eines geladenen ausführbaren Segments liegt.

Ein Entry Point in:

- nicht gemapptem Speicher,
- Datenbereich,
- ungültigem Bereich

ist abzulehnen.

---

## Virtuelle und physische Adresse

ELF kann unter anderem enthalten:

```text
p_vaddr
p_paddr
```

NovaOS darf nicht voraussetzen, dass beide Werte immer identisch sind.

Die konkrete Interpretation hängt vom Kernel-Adressmodell ab.

---

## Kernel Virtual Address

`p_vaddr` beschreibt die erwartete virtuelle Adresse des Segments.

Der Bootloader beziehungsweise die Kernel-Preparation-Phase muss entsprechende Page Tables aufbauen, wenn der Kernel bereits mit aktiviertem Paging gestartet wird.

---

## Physical Placement

Die tatsächliche physische Platzierung kann vom virtuellen Kerneladressraum getrennt sein.

Konzeptionell:

```text
Kernel Virtual Address
        │
        ▼
Page Table Mapping
        │
        ▼
Physical Boot Allocation
```

Damit bleibt das physische Bootlayout flexibel.

---

## p_paddr

`p_paddr` darf nur gemäß einer expliziten NovaOS-ELF-Konvention verwendet werden.

NovaOS darf nicht ungeprüft annehmen, dass `p_paddr` immer eine zwingende physische Ladeadresse definiert.

Die konkrete Semantik wird in `NPSPEC-BOOT-KERNEL-ELF-0001` festgelegt.

---

## Höherer Kerneladressraum

Ein 64-Bit-NovaOS-Kernel darf in einem höheren virtuellen Adressbereich ausgeführt werden.

Beispiel:

```text
Physical:
    0x00200000

Virtual:
    higher-half kernel address
```

ELF muss dieses Modell ausdrücken können.

Der Bootloader richtet das notwendige Mapping vor dem Kernel Handoff ein.

---

## Kein festes 1-MiB-Kernelmodell

NovaOS darf nicht architektonisch voraussetzen:

```text
kernel always starts at 1 MiB
```

Eine solche Adresse darf für einen frühen Prototyp verwendet werden, ist aber keine langfristige Kernel-Ladeinvariante.

Das ELF-Layout und der Kernel Memory Contract sind maßgeblich.

---

## Segmentüberlappung

ELF-Segmente dürfen nicht auf eine Weise überlappen, die zu widersprüchlichen oder unsicheren Mappings führt.

Der Loader muss Segmentüberlappungen erkennen.

Kompatible, bewusst vom Toolchainmodell erzeugte Fälle dürfen nur unterstützt werden, wenn sie explizit definiert sind.

---

## Bootloader-Speicher

Kernel-Segmente dürfen nicht unkontrolliert überschreiben:

- aktive Bootloaderbereiche,
- Boot Stack,
- BootInfo,
- Page Tables,
- geladene Module,
- Firmwaredaten,
- Framebuffer,
- reservierte Memory Regions.

Vor dem Laden muss ein Memory Placement Plan validiert werden.

---

## Memory Map Integration

Kernel Loading muss die normalisierte Boot Memory Map berücksichtigen.

Es darf nur Speicher verwendet werden, der für diesen Zweck reserviert werden kann.

Unbekannter oder firmware-reservierter Speicher darf nicht überschrieben werden.

---

## Kernel Memory Reservation

Geladene Kernelbereiche werden im Boot Resource Manager als Kernelressourcen markiert.

Beispiel:

```text
BootResource {
    type = KernelSegment
    state = Reserved
    owner = Kernel
}
```

Diese Bereiche dürfen durch spätere Bootallokationen nicht erneut verwendet werden.

---

## Kernel Stack

Der Kernel Stack ist nicht zwingend Bestandteil eines `PT_LOAD`-Segments.

Er kann durch die Kernel Preparation separat erzeugt und reserviert werden.

Seine Adresse und Größe folgen dem Kernel Handoff Contract.

---

## Initiale Page Tables

ELF definiert nicht allein das vollständige Pagingmodell.

Die Bootarchitektur verbindet daher:

```text
ELF Segment Layout
        +
Architecture Memory Rules
        +
Kernel Memory Contract
        ↓
Initial Page Tables
```

---

## Relocations

Der Baseline-Loader für `ET_EXEC` benötigt keine allgemeinen dynamischen ELF-Relocations.

Falls relocatable Kernelimages unterstützt werden, müssen nur explizit definierte Relocation-Typen verarbeitet werden.

Der Bootloader soll kein vollständiger allgemeiner dynamischer Linker werden.

---

## Kein allgemeiner Dynamic Linker

Der Kernel ELF Loader ist kein Userspace ELF Loader und kein allgemeiner Runtime Linker.

Nicht erforderlich sind standardmäßig:

- Shared Library Resolution,
- `DT_NEEDED`,
- allgemeines Symbol Binding,
- ELF Interpreter,
- Userspace Loader Semantics.

Die Bootimplementierung bleibt auf den nativen Kernelstart fokussiert.

---

## Interpreter Segment

Ein nativer NovaOS-Kernel darf nicht auf einen externen ELF Interpreter angewiesen sein.

`PT_INTERP` ist im Baseline-Kernelmodell nicht erforderlich.

Wenn ein Kernelimage einen nicht unterstützten Interpreter verlangt, wird es abgelehnt.

---

## Dynamic Section

Eine Dynamic Section darf nur verwendet werden, wenn ein später spezifiziertes relocatable Kernelmodell dies erfordert.

Ihre bloße Existenz darf nicht dazu führen, dass der Bootloader Linux-artige Userspace-Dynamic-Linker-Semantik implementiert.

---

## Symboltabellen

ELF-Symboltabellen sind für das Laden des Kernels nicht erforderlich.

Sie können optional für:

- Debugging,
- Panic Symbolization,
- Bootdiagnose,
- Development Builds

bereitgestellt werden.

---

## Debug-Informationen

Debug Sections dürfen im Kernelimage vorhanden sein oder in separaten Debugartefakten gespeichert werden.

Der Bootloader muss sie nicht in den aktiven Kerneladressraum laden.

---

## Stripped Kernel

Ein produktives Kernelimage darf nicht benötigte Section-, Symbol- und Debuginformationen entfernen, solange die Program Header für korrektes Loading vollständig erhalten bleiben.

---

## Build-ID

Kernelimages sollen eine stabile Build-Identität besitzen können.

Beispiel:

```text
KernelBuildId
```

Diese kann für:

- Crash Reports,
- Symbol Resolution,
- Known-Good Selection,
- Updateverwaltung,
- Architecture Introspection

verwendet werden.

Die konkrete Ablage kann über ELF Notes oder NovaOS-Metadaten erfolgen.

---

## ELF Notes

NovaOS darf ELF Notes verwenden, um zusätzliche Kernel-Metadaten bereitzustellen.

Beispiele:

```text
NovaABI version
Kernel build ID
Minimum boot protocol
Required architecture features
Kernel metadata version
```

Solche Notes ergänzen ELF, ersetzen aber nicht das versionierte BootInfo-Protokoll.

---

## NovaOS Kernel Metadata

NovaOS darf einen eigenen ELF-Note-Namespace definieren.

Konzeptionell:

```text
NOVA
```

mit typisierten Metadateneinträgen.

Die konkrete Struktur wird separat spezifiziert.

---

## Boot Protocol Version

Ein Kernelimage darf deklarieren, welche Boot-Handoff-Version es benötigt.

Beispiel:

```text
RequiredBootProtocol = 4
```

Der Bootloader prüft vor dem Laden beziehungsweise Handoff die Kompatibilität.

---

## Keine versteckte Versionsannahme

Bootloader und Kernel dürfen nicht nur deshalb als kompatibel gelten, weil das ELF-Format syntaktisch gültig ist.

Zusätzlich muss die NovaOS Boot Protocol Compatibility geprüft werden.

---

## CPU Feature Requirements

Ein Kernelimage darf minimale CPU- oder Architekturfeatures deklarieren.

Beispiel:

```text
Requires:
    x86_64
    NX
```

oder später:

```text
Requires:
    ARMv8.x
```

Der Bootloader beziehungsweise Platform Preparation Path darf inkompatible Images früh ablehnen.

---

## Baseline und optimierte Kernel

NovaOS kann mehrere Kernelimages für unterschiedliche Architekturprofile unterstützen.

Beispiel:

```text
kernel-x86_64-baseline.elf
kernel-x86_64-v3.elf
```

Die Auswahl erfolgt durch Boot Policy und nicht durch Veränderung der ELF-Grundsemantik.

---

## Sicherheitsprüfung vor Parsing

Kryptografische Verifikation und strukturelles Parsing sind unterschiedliche Schritte.

Je nach Trust-Modell kann das Image zunächst als Bytefolge authentifiziert und anschließend strukturell validiert werden.

Mindestens gilt:

```text
Untrusted input
    ↓
Defensive parser
```

Auch signierte Dateien müssen strukturell validiert werden.

---

## Signierte ELF-Dateien

Die Signatur darf:

- das gesamte ELF-Image,
- einen manifestierten Hash,
- oder eine definierte Boot-Artefaktstruktur

authentifizieren.

Die konkrete Verified-Boot-Struktur wird separat spezifiziert.

---

## Integrität

Alle Bytes, die die Ausführung oder den Speicheraufbau des Kernels beeinflussen, müssen von der Integritätsprüfung erfasst werden.

Ein Signaturmodell darf beispielsweise nicht Program Headers ungeschützt lassen.

---

## TOCTOU

Zwischen erfolgreicher Verifikation und dem Laden des Kernels darf das Image nicht unbemerkt ausgetauscht werden.

Geeignete Mechanismen sind beispielsweise:

```text
Verify loaded buffer
```

oder:

```text
Read immutable artifact
    ↓
Verify
    ↓
Load from same verified representation
```

---

## Boot Storage und ELF

Der ELF Loader arbeitet auf einem abstrakten Boot File beziehungsweise Image Reader.

Er darf nicht direkt von:

```text
BIOS sectors
UEFI file protocols
FAT internals
```

abhängig sein.

Das Modell lautet:

```text
Boot Storage
    ↓
Boot File
    ↓
ELF Loader
```

---

## Streaming

Sehr große Kernelimages dürfen zukünftig segmentweise geladen werden.

Die Baseline-Implementierung darf jedoch das gesamte Kernelimage in einen validierten Buffer laden, sofern Resource Budget und Plattform dies erlauben.

---

## Kompression

ELF selbst wird als native logische Kernel-Ladeform beibehalten.

Ein Kernelartefakt darf zusätzlich komprimiert gespeichert werden.

Beispiel:

```text
Compressed Kernel Artifact
        ↓
Decompression
        ↓
ELF Image
        ↓
ELF Loader
```

Kompression ist eine Transport-/Storage-Eigenschaft und nicht Teil der ELF-Semantik.

---

## Verschlüsselung

Analog darf ein Kernelartefakt verschlüsselt gespeichert werden.

Konzeptionell:

```text
Encrypted Artifact
        ↓
Authenticate / Decrypt
        ↓
Verified ELF
        ↓
Load
```

Auch hier bleibt NKI das bevorzugte Produktionsformat; ein enthaltenes ELF-Payload wird nach der Entschlüsselung gemäß ELF-Ladevertrag verarbeitet.

---

## Kernelmodule

Diese ADR definiert primär das Format des Kernels.

Initiale Kernelmodule dürfen separat geladen und über `BootInfo` übergeben werden.

Sie müssen nicht zwangsläufig dieselbe ELF-Konvention wie der Kernel verwenden.

---

## Init Modules

Beispiel:

```text
Kernel ELF
Initial Ramdisk
System Manifest
Recovery Metadata
```

werden als getrennte Bootartefakte behandelt.

Der ELF Loader darf diese nicht fälschlich als Kernelsegmente interpretieren.

---

## BIOS und UEFI

Beide Firmwarepfade müssen denselben ELF Kernel Loader verwenden können.

Das Modell lautet:

```text
BIOS ──┐
       ├── Shared ELF Kernel Loader
UEFI ──┘
```

Firmwaretyp darf keinen Unterschied im nativen Kerneldateiformat verursachen.

---

## Architekturabhängige ELF-Details

ELF ist architekturübergreifend, besitzt jedoch architekturspezifische:

- Machine IDs,
- Relocation Types,
- ABI-Details,
- Pointer Widths.

Diese werden hinter Architekturprofilen des Loaders gekapselt.

---

## ELF Loader Core

Konzeptionell:

```text
ELF Loader Core
      │
      ├── ELF32 parser
      ├── ELF64 parser
      └── Architecture validation
```

Es soll möglichst viel gemeinsame defensive Parsinglogik verwendet werden.

---

## 32-Bit-Kernel

NovaOS kann für historische oder spezielle Plattformen einen ELF32-Kernel unterstützen.

Beispiel:

```text
ELF32
EM_386
```

Dies ist vom ELF64-Pfad klar getrennt.

---

## 64-Bit-Kernel

Für moderne x86_64-Plattformen wird ELF64 verwendet.

Beispiel:

```text
ELF64
EM_X86_64
```

Der Bootloader muss vor Handoff Long Mode und das definierte Memory Mapping herstellen.

---

## ARM64 und RISC-V

Die Wahl von ELF erleichtert zukünftige Architekturports.

Beispiele:

```text
ELF64 + EM_AARCH64
ELF64 + EM_RISCV
```

Dabei bleibt die höhere Bootlogik weitgehend gleich.

---

## BootInfo bleibt getrennt

ELF beschreibt:

```text
how to load the kernel image
```

`BootInfo` beschreibt:

```text
what environment the kernel receives
```

Beide Contracts dürfen nicht miteinander vermischt werden.

---

## Kernel ABI bleibt getrennt

ELF ist ein Binärcontainer und Ladeformat.

Es definiert nicht automatisch:

- NovaABI,
- Syscall ABI,
- Kernel Internal ABI,
- Capability ABI.

Diese werden separat spezifiziert.

---

## Linker Script

Der Kernelbuild verwendet ein kontrolliertes Linker Script oder äquivalentes Toolchainmodell, um das gewünschte ELF-Layout zu erzeugen.

Dieses definiert unter anderem:

- Segmentstruktur,
- virtuelle Basis,
- Alignment,
- Kernel Entry Symbol,
- read-only und writable Bereiche.

---

## Segmentorientiertes Linkerlayout

Das Linkerlayout soll die späteren Memory Permissions sinnvoll abbilden.

Beispiel:

```text
RX segment
    kernel code

R segment
    read-only data

RW segment
    writable data + bss
```

Dies erleichtert frühe W^X-Durchsetzung.

---

## Keine unnötige 1:1-Section-Segment-Abbildung

Nicht jede ELF Section benötigt ein eigenes `PT_LOAD`-Segment.

Der Kernelbuild soll sinnvolle Lade- und Protection-Gruppen bilden.

---

## Page Alignment

Kernelsegmente sollen soweit sinnvoll auf Page Boundaries ausgerichtet werden.

Dies ermöglicht:

- saubere Page Permissions,
- einfachere Mappings,
- weniger Mischseiten zwischen Code und Daten.

---

## Huge Pages

Der Kernel darf später Teile seines statischen Mappings mit Large/Huge Pages abbilden, wenn:

- Alignment,
- Segmentgrenzen,
- Permissions

dies erlauben.

ELF soll diese Optimierung nicht verhindern.

---

## Kernel ASLR

Ein zukünftiger relocatable Kernel darf zufällig innerhalb zulässiger Adressbereiche platziert werden.

Dabei müssen:

- Relocations,
- Alignment,
- Address Space Constraints,
- Deterministic Mode

berücksichtigt werden.

---

## Deterministic Mode

Bei deterministischem Boot darf eine Kernelbasis fixiert werden.

Beispiel:

```text
KernelRelocation = Fixed
```

Damit kann Kernel ASLR für reproduzierbare Tests deaktiviert werden.

---

## Reproduzierbare Builds

ELF-Kernelimages sollen reproduzierbare Builds unterstützen.

Nicht deterministische Metadaten wie unkontrollierte Zeitstempel sollen vermieden oder kontrolliert werden.

Build-ID und Signatur dürfen reproduzierbar beziehungsweise eindeutig nachvollziehbar erzeugt werden.

---

## Known-Good

Known-Good-Kernelgenerationen können anhand ihrer:

- Build ID,
- Hash,
- Signatur,
- Version

identifiziert werden.

Das ELF-Format unterstützt die technische Zuordnung, die Policy bleibt Teil des Bootsystems.

---

## Architecture Introspection

Nach dem Start sollen relevante Kernelimageinformationen introspektierbar sein.

Beispiel:

```text
Kernel Image:
    ELF64

Machine:
    x86_64

Build ID:
    6F91...

Entry:
    0x...

Boot Protocol:
    4
```

Sicherheitskritische Adressen dürfen für unprivilegierte Prozesse gefiltert werden.

---

## Boot Logging

Der Bootloader soll relevante ELF-Ladeschritte protokollieren können.

Beispiel:

```text
ELF64 detected
Architecture x86_64 validated
4 PT_LOAD segments found
Kernel image verified
Segments mapped
Entry point validated
```

Bei Fehlern muss der betroffene Validierungsschritt erkennbar sein.

---

## Fehlerklassen

Der ELF Loader soll typisierte Fehler liefern.

Beispiele:

```text
InvalidMagic
UnsupportedClass
UnsupportedEndianness
UnsupportedMachine
UnsupportedType
InvalidProgramHeaderTable
InvalidSegment
SegmentOutOfBounds
SegmentOverlap
AddressOverflow
MemoryConflict
EntryPointInvalid
UnsupportedRelocation
BootProtocolMismatch
```

---

## Fail-Safe Defaults

Ein unbekannter oder nicht verstandener ELF-Zustand wird nicht optimistisch akzeptiert.

Es gilt:

```text
Unknown ELF feature
    ↓
Reject unless explicitly supported
```

Der Bootloader soll keine Heuristik verwenden, um beschädigte Kernelimages „irgendwie“ zu starten.

---

## Parser-Sicherheit

Der ELF Parser ist Teil der Boot Security Boundary.

Er muss daher besonders defensiv implementiert werden.

Er soll insbesondere vermeiden:

- unchecked pointer arithmetic,
- unchecked casts,
- unbounded loops,
- trusted file offsets,
- unchecked allocation sizes.

---

## Parser-Komplexität

NovaOS implementiert nur den für den Kernelstart benötigten ELF-Subset.

Dies reduziert:

- Angriffsfläche,
- Codegröße,
- Testaufwand,
- unerwartete Semantik.

---

## Kein Linux-Kernel-Bootprotokoll

Die Verwendung von ELF bedeutet nicht, dass NovaOS das Linux Boot Protocol übernimmt.

NovaOS definiert ein eigenes:

```text
NovaOS Boot Protocol
```

mit eigener `BootInfo`-Struktur und eigener Kernel Entry Convention.

---

## Kein Multiboot-Zwang

NovaOS darf optional kompatible Loader oder Development Paths für Multiboot-artige Protokolle unterstützen.

Die native Bootarchitektur ist jedoch nicht davon abhängig.

ELF + NovaOS Boot Protocol bilden die native Grundlage.

---

## Externe Bootloader

Zukünftig kann NovaOS auch von Drittanbieter-Bootloadern geladen werden, wenn diese:

- das unterstützte ELF-Modell,
- die NovaOS Kernel Handoff Convention,
- BootInfo,
- Trust Requirements

korrekt implementieren.

Die native NovaOS-Implementierung bleibt Referenz.

---

## Bootloader-Tests

Der ELF Loader muss mit synthetischen und realen Images getestet werden.

Zu testen sind unter anderem:

```text
valid minimal ELF
multiple PT_LOAD segments
BSS
invalid magic
truncated header
truncated program table
overflowed offsets
invalid alignment
overlapping segments
entry outside executable segment
unsupported architecture
```

---

## Fuzzing

Der ELF Parser soll für automatisiertes Fuzzing geeignet sein.

Da ELF-Images vollständig untrusted Input darstellen können, ist parserzentriertes Fuzzing ausdrücklich vorgesehen.

---

## Cross-Toolchain-Verifikation

Kernelbuilds sollen mit geeigneten Tools validiert werden können.

Beispielsweise:

```text
readelf
objdump
custom NovaOS verifier
```

Der NovaOS-Buildprozess soll zusätzlich eigene statische Validierungen des erzeugten Kernel-ELF durchführen.

---

## Build-Time Validation

Ein fehlerhaftes Kernelimage soll möglichst bereits beim Build erkannt werden.

Der Buildprozess prüft beispielsweise:

- erwartete ELF-Klasse,
- Architektur,
- Entry Point,
- Segmentberechtigungen,
- Alignment,
- unerwünschte W+X-Segmente,
- Boot Protocol Metadata.

Runtime-Validierung im Bootloader bleibt trotzdem verpflichtend.

---

## Evolvierbarkeit

Der Kernel-ELF-Contract muss versionierbar erweitert werden können.

Neue NovaOS-spezifische Metadaten sollen bevorzugt über explizit versionierte Strukturen oder ELF Notes ergänzt werden.

Bestehende grundlegende ELF-Semantik soll dabei nicht verändert werden.

---

## KI-Unabhängigkeit

Das Parsen, Validieren und Laden von ELF muss vollständig ohne KI funktionieren.

KI darf optional:

- Buildfehler analysieren,
- Bootlogs auswerten,
- Performanceprobleme erklären.

KI darf niemals entscheiden, ein strukturell ungültiges Kernelimage trotzdem auszuführen.

---

## Normative Anforderungen

1. NovaOS MUSS NKI als bevorzugtes Produktionsformat verwenden und ELF direkt laden können.
2. BIOS- und UEFI-Bootpfad MÜSSEN dasselbe native ELF-Kernelmodell verwenden können.
3. Der Bootloader MUSS ELF vor dem Laden strukturell validieren.
4. Die ELF-Magic MUSS geprüft werden.
5. ELF Class MUSS geprüft werden.
6. Endianness MUSS geprüft werden.
7. Machine Architecture MUSS geprüft werden.
8. ELF Type MUSS geprüft werden.
9. Program Header Offset, Größe und Anzahl MÜSSEN validiert werden.
10. Die Program Header Table MUSS vollständig innerhalb des Images liegen.
11. Größen- und Adressberechnungen MÜSSEN overflow-sicher erfolgen.
12. `PT_LOAD` MUSS als grundlegender ladbarer Segmenttyp unterstützt werden.
13. Nicht unterstützte kritische Program-Header-Semantik MUSS explizit abgelehnt werden.
14. Für jedes `PT_LOAD`-Segment MUSS `p_memsz >= p_filesz` gelten.
15. Segmentdateibereiche MÜSSEN vollständig innerhalb des Kernelimages liegen.
16. Nicht initialisierte Segmentanteile MÜSSEN mit Null initialisiert werden.
17. Segmentalignment MUSS validiert und eingehalten werden.
18. Segmentberechtigungen MÜSSEN aus ELF-Flags ableitbar sein.
19. NovaOS SOLL W^X für statische Kernelbereiche durchsetzen.
20. Ungewöhnliche W+X-Segmente SOLLEN durch Security Policy ablehnbar sein.
21. Segmentüberlappungen MÜSSEN validiert werden.
22. Kernel-Segmente DÜRFEN Bootloader- oder Firmware-Ressourcen NICHT unkontrolliert überschreiben.
23. Kernel Loading MUSS die Boot Memory Map berücksichtigen.
24. Geladene Kernelbereiche MÜSSEN als reservierte Kernelressourcen markiert werden.
25. Der Kernel Entry Point MUSS aus dem ELF Header gelesen werden.
26. Der Entry Point MUSS validiert werden.
27. Der Entry Point SOLL innerhalb eines ausführbaren geladenen Segments liegen.
28. Der Bootloader DARF NICHT dauerhaft auf einen hardcodierten Kernel Entry Point angewiesen sein.
29. NovaOS DARF NICHT dauerhaft ein festes physisches Kernel-Ladeziel als Architekturvoraussetzung verwenden.
30. Virtuelle und physische Kerneladressen MÜSSEN getrennt modellierbar sein.
31. `p_vaddr` MUSS entsprechend dem Kernel Memory Contract berücksichtigt werden.
32. Die Verwendung von `p_paddr` MUSS explizit durch die NovaOS-ELF-Konvention definiert werden.
33. Höhere virtuelle Kerneladressen MÜSSEN unterstützt werden können.
34. Die Kernel Preparation MUSS notwendige initiale Page Tables auf Basis des ELF-Layouts aufbauen können.
35. ELF DARF NICHT allein das vollständige Pagingmodell definieren.
36. `ET_EXEC` MUSS als Baseline-Kerneltyp unterstützt werden.
37. `ET_DYN` DARF zukünftig für relocatable Kernel unterstützt werden.
38. Der Baseline-Loader MUSS kein allgemeiner dynamischer ELF-Linker sein.
39. Shared-Library-Auflösung DARF NICHT Voraussetzung des nativen Kernelstarts sein.
40. `PT_INTERP` DARF im Baseline-Kernelmodell nicht erforderlich sein.
41. Allgemeine Userspace-ELF-Semantik DARF NICHT ungeprüft auf den Kernelloader übertragen werden.
42. Symboltabellen DÜRFEN optional für Diagnose verwendet werden.
43. Debuginformationen DÜRFEN aus produktiven Kernelimages entfernt werden.
44. Kernelimages SOLLEN eine Build Identity besitzen können.
45. NovaOS DARF ELF Notes für versionierte Kernelmetadaten verwenden.
46. Kernelmetadaten DÜRFEN Boot Protocol Requirements deklarieren.
47. Bootloader und Kernel MÜSSEN ihre Boot Protocol Compatibility prüfen können.
48. Ein syntaktisch gültiges ELF DARF NICHT allein als bootkompatibel gelten.
49. Kernelimages DÜRFEN minimale Architekturfeatures deklarieren.
50. Inkompatible Kernelimages MÜSSEN vor dem Handoff abgelehnt werden.
51. ELF-Parsing MUSS auch bei signierten Artefakten defensiv erfolgen.
52. Verified Boot und ELF-Strukturvalidierung MÜSSEN getrennte Prüfungen bleiben.
53. Alle ausführungsrelevanten ELF-Bestandteile MÜSSEN durch das Integritätsmodell geschützt werden.
54. Verifikation und Loading DÜRFEN keinen unsicheren TOCTOU-Pfad erzeugen.
55. Der ELF Loader MUSS über abstrahierten Boot Storage arbeiten.
56. Der ELF Loader DARF NICHT direkt von BIOS- oder UEFI-Dateisystemmechanismen abhängen.
57. Komprimierte Kernelartefakte DÜRFEN verwendet werden, müssen vor dem ELF-Loading in ein gültiges ELF überführt werden.
58. Verschlüsselte Kernelartefakte DÜRFEN verwendet werden, müssen vor dem ELF-Loading authentifiziert und entschlüsselt werden.
59. ELF MUSS von BootInfo semantisch getrennt bleiben.
60. ELF DARF NICHT NovaABI oder Syscall ABI definieren.
61. Der Kernelbuild MUSS ein kontrolliertes Linkerlayout erzeugen.
62. Kernelsegmente SOLLEN nach Schutzklasse sinnvoll gruppiert werden.
63. Kernelsegmente SOLLEN soweit sinnvoll Page-aligned sein.
64. Der ELF-Vertrag SOLL zukünftiges Kernel ASLR nicht verhindern.
65. Deterministic Mode MUSS eine reproduzierbare Kernelplatzierung erlauben können.
66. Kernelbuilds SOLLEN reproduzierbar erzeugt werden können.
67. Known-Good-Systeme MÜSSEN Kernelimages eindeutig identifizieren können.
68. Relevante ELF-Kernelmetadaten SOLLEN introspektierbar sein.
69. Sicherheitskritische Kerneladressen MÜSSEN für unprivilegierte Introspection filterbar sein.
70. ELF-Ladefehler MÜSSEN typisiert behandelbar sein.
71. Unbekannte kritische ELF-Features DÜRFEN NICHT optimistisch akzeptiert werden.
72. Der ELF Parser MUSS als sicherheitskritische Komponente behandelt werden.
73. Der Loader SOLL nur das für den Kernelstart notwendige ELF-Subset implementieren.
74. Die Verwendung von ELF DARF NovaOS NICHT an das Linux Boot Protocol binden.
75. Die native Bootarchitektur DARF NICHT von Multiboot abhängen.
76. Drittanbieter-Bootloader DÜRFEN den NovaOS-Kernel laden, wenn sie ELF- und NovaOS-Handoff-Contracts erfüllen.
77. Der ELF Loader MUSS automatisiert testbar sein.
78. Der ELF Parser SOLL fuzzbar sein.
79. Kernel-ELF-Images SOLLEN bereits beim Build statisch validiert werden.
80. Runtime-Validierung im Bootloader MUSS trotz Build-Time Validation erhalten bleiben.
81. Der NovaOS-ELF-Contract MUSS evolvierbar sein.
82. ELF Loading und Validation MÜSSEN vollständig ohne KI funktionieren.
83. KI DARF niemals ein ungültiges Kernelimage zur Ausführung freigeben.
84. NovaOS MUSS das Kernellayout aus einem expliziten strukturierten Image ableiten statt aus versteckten Bootloaderannahmen.

---

## Konsequenzen

### Positive Konsequenzen

- Kernel- und Bootloader-Speicherlayout werden entkoppelt.
- mehrere ladbare Kernelbereiche sind sauber darstellbar.
- Code-, Read-Only- und Data-Segmente können getrennte Berechtigungen erhalten.
- `.bss` wird standardisiert dargestellt.
- höhere Kerneladressen und flexible physische Platzierung werden möglich.
- ELF unterstützt mehrere CPU-Architekturen.
- vorhandene Toolchains können genutzt werden.
- Debugging und Symbolinformationen lassen sich sauber ergänzen.
- zukünftiges Kernel ASLR bleibt möglich.
- BIOS und UEFI können exakt dasselbe Kernelimage laden.
- Flat-Binary-spezifische Sonderlogik wird vermieden.

### Negative Konsequenzen

- der Bootloader benötigt einen defensiven ELF Parser,
- ELF ist komplexer als ein Flat Binary,
- fehlerhafte oder manipulierte Header erzeugen zusätzliche Sicherheitsrisiken,
- ELF-Varianten und Architekturdetails müssen klar begrenzt werden,
- relocatable Kernel benötigen später zusätzliche Relocation-Unterstützung.

---

## Verworfene Alternativen

### Flat Binary als dauerhaftes natives Kernelimage

Verworfen.

Es besitzt keine ausreichende strukturierte Beschreibung von Segmenten, Entry Point und Memory Permissions.

### PE/COFF als natives NovaOS-Kernelformat

Verworfen.

PE/COFF ist für den nativen NovaOS-Kernel nicht erforderlich und würde stärker an Windows-/UEFI-geprägte Toolingmodelle koppeln.

UEFI-spezifische Loaderkomponenten dürfen selbstverständlich PE/COFF verwenden, wenn die UEFI-Plattform dies verlangt.

Das Kernelimage selbst bleibt ELF.

### Unterschiedliche Kerneldateiformate für BIOS und UEFI

Verworfen.

Firmwaretyp und Kernelimageformat sind voneinander unabhängig.

### Eigenes vollständig neues Kernel-Executable-Format

Verworfen.

Dies würde zusätzlichen Compiler-, Linker-, Debugger- und Toolingaufwand erzeugen, ohne aktuell einen ausreichenden architektonischen Vorteil zu bieten.

### ELF Sections als Ladegrundlage

Verworfen.

Executable Loading basiert auf Program Headers.

Sections dienen primär Linker-, Debugging- und Entwicklungszwecken.

### Vollständigen Userspace-ELF-Linker in den Bootloader integrieren

Verworfen.

Der Kernelloader soll klein, deterministisch und sicher bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-KERNEL-ELF-0001`
- `NPSPEC-BOOT-ELF-HEADER-0001`
- `NPSPEC-BOOT-ELF-PROGRAMHEADER-0001`
- `NPSPEC-BOOT-ELF-SEGMENT-0001`
- `NPSPEC-BOOT-ELF-VALIDATION-0001`
- `NPSPEC-BOOT-ELF-ENTRY-0001`
- `NPSPEC-BOOT-ELF-MEMORY-0001`
- `NPSPEC-BOOT-ELF-PERMISSION-0001`
- `NPSPEC-BOOT-ELF-METADATA-0001`
- `NPSPEC-BOOT-ELF-NOTE-0001`
- `NPSPEC-BOOT-ELF32-0001`
- `NPSPEC-BOOT-ELF64-0001`
- `NPSPEC-BOOT-KERNEL-LINKER-0001`
- `NPSPEC-BOOT-KERNEL-RELOCATION-0001`
- `NPSPEC-BOOT-KERNEL-ASLR-0001`
- `NPSPEC-BOOT-KERNEL-BUILDID-0001`
- `NPSPEC-BOOT-KERNEL-IMAGE-VERIFY-0001`
- `NPSPEC-BOOT-ELF-TEST-0001`
- `NPSPEC-BOOT-ELF-FUZZ-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- gültiger minimaler ELF32-Kernel,
- gültiger minimaler ELF64-Kernel,
- x86-ELF,
- x86_64-ELF,
- mehrere `PT_LOAD`-Segmente,
- getrennte RX-, R- und RW-Segmente,
- `.bss` über `p_memsz > p_filesz`,
- Page Alignment,
- höherer virtueller Kerneladressraum,
- getrennte physische und virtuelle Platzierung,
- ungültige ELF-Magic,
- falsche ELF Class,
- falsches Endianness,
- falsche Machine Architecture,
- ungültiger ELF Type,
- abgeschnittener ELF Header,
- abgeschnittene Program Header Table,
- Program Header außerhalb des Images,
- Segment außerhalb des Images,
- Integer Overflow bei Offsets,
- `p_filesz > p_memsz`,
- ungültiges Alignment,
- unerlaubte Segmentüberlappung,
- Konflikt mit Bootloader-Speicher,
- Konflikt mit reserviertem Firmware-Speicher,
- Entry Point außerhalb aller Segmente,
- Entry Point in nicht ausführbarem Segment,
- W+X-Segment gemäß Security Policy,
- Boot-Protocol-Inkompatibilität,
- unbekanntes kritisches NovaOS-Metadatum,
- beschädigtes signiertes Image,
- Known-Good-Kernelimage,
- ELF-Loading über BIOS,
- dasselbe ELF-Image über UEFI,
- Fuzzing mit zufälligen und mutierten ELF-Headern.

---

## Ergebnis

NovaOS verwendet NKI als bevorzugtes Produktionsformat; ELF bleibt die strukturierte direkt ladbare Beschreibung des Kernelimages beziehungsweise des NKI-Payloads.

Der Bootpfad lautet:

```text
Kernel Artifact
      ↓
Trust / Integrity Validation
      ↓
ELF Structural Validation
      ↓
Program Headers
      ↓
PT_LOAD Segments
      ↓
Memory Mapping
      ↓
Entry Point Validation
      ↓
Kernel Handoff
```

ELF definiert dabei:

```text
what must be loaded
where it belongs
how large it is
and how it may be accessed
```

während `BootInfo` weiterhin definiert:

```text
what environment
the kernel receives
```

Die zentrale Architekturregel lautet:

```text
ELF describes the kernel image.

NovaOS defines the boot protocol.

Do not hide kernel layout
inside bootloader assumptions.
```
