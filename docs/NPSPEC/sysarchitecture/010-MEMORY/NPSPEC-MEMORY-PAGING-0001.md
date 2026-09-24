# NPSPEC-MEMORY-PAGING-0001 – Nova Paging Architecture

## Status

Entwurf

## Kategorie

Memory / Paging / Virtual Memory / MMU

## Zweck

NovaOS definiert Paging als Mechanismus zur Abbildung virtueller Speicherseiten auf physische Frames.

```text
Virtual Address
      ↓
Virtual Page
      ↓
Page Tables
      ↓
Physical Frame
      ↓
Physical Memory
```

Paging bildet die technische Grundlage für virtuelle Adressräume, Prozessisolation, Speicherschutz, Shared Memory und bedarfsgesteuerte Speicherzuweisung.

## Grundprinzip

Virtueller Speicher wird in Pages und physischer Speicher in Frames unterteilt.

```text
Virtual Memory          Physical Memory

Page A ───────────────→ Frame 42
Page B ───────────────→ Frame 17
Page C ───────────────→ Frame 91
```

Dabei gilt:

```text
Virtual Page ≠ Physical Frame
VAS ≠ Page Table
Paging ≠ Swapping
```

Paging beschreibt primär die Adressübersetzung und Zugriffssteuerung.

## Page Tables

Page Tables bilden virtuelle Pages auf physische Frames oder definierte nicht-präsente Zustände ab.

Konzeptionell enthält ein Eintrag mindestens:

```text
Physical Frame
Present
Read / Write
Execute
User / Kernel
```

Optional können weitere Eigenschaften vorhanden sein:

```text
Accessed
Dirty
Global
Copy-on-Write
Cache Policy
Page Size
Architecture Flags
```

Die konkrete Bitstruktur ist architekturabhängig.

## Architekturabstraktion

NovaOS darf seine allgemeine Memory-Architektur nicht an eine bestimmte Page-Table-Struktur binden.

```text
Nova VMM
   ↓
HAL / Architecture Layer
   ↓
x86 / ARM / RISC-V / ...
   ↓
Hardware Page Tables
```

Mehrstufige Page Tables und architekturspezifische Formate bleiben Implementierungsdetails.

## Mapping

Ein Mapping verbindet einen virtuellen Bereich mit physischen Frames.

```text
Map(
    AddressSpace,
    VirtualRange,
    PhysicalFrames,
    Permissions
)
```

Der Mapping-Vorgang muss atomar gegenüber konkurrierenden Änderungen ausreichend geschützt sein.

## Unmapping

Mappings müssen kontrolliert entfernt werden können.

```text
Virtual Page
     ↓
Unmap
     ↓
Remove Page-Table Entry
     ↓
Invalidate Translation
```

Ein Unmapping darf einen physischen Frame nur dann freigeben, wenn dessen Ownership- und Referenzbedingungen dies erlauben.

```text
Unmap ≠ Free Frame
```

## Protection

Page Tables müssen Speicherzugriffe entsprechend den definierten Berechtigungen beschränken.

Mindestens:

```text
Read
Write
Execute
User / Kernel
```

NovaOS soll Write-XOR-Execute unterstützen.

```text
Writable + Executable
```

darf nur explizit autorisiert werden.

## Demand Paging

Physische Frames müssen nicht zwingend beim Reservieren eines virtuellen Bereichs bereitgestellt werden.

```text
Reserve Virtual Page
       ↓
No Physical Frame
       ↓
First Access
       ↓
Page Fault
       ↓
Allocate Frame
       ↓
Create Mapping
```

Demand Paging ermöglicht sparsamen Umgang mit physischem Speicher.

## Page Fault

Kann die MMU eine Speicheroperation nicht entsprechend dem aktuellen Mapping durchführen, entsteht ein Page Fault.

```text
Memory Access
     ↓
Translation / Permission Failure
     ↓
Page Fault
```

Der Fault Handler muss unterscheiden können zwischen:

```text
Valid Demand Fault
Copy-on-Write Fault
Permission Violation
Guard Page Access
Invalid Address
Missing Backing Data
```

Nicht jeder Page Fault ist ein Fehler.

## Copy-on-Write

Mehrere Address Spaces dürfen zunächst denselben Frame lesend verwenden.

```text
Process A ─┐
           ├→ Shared Frame
Process B ─┘
```

Bei einem Schreibzugriff:

```text
Write Fault
    ↓
Allocate New Frame
    ↓
Copy Data
    ↓
Remap Writer
```

Copy-on-Write darf nur eingesetzt werden, wenn die Ownership- und Sicherheitssemantik eindeutig definiert ist.

## Page Sizes

NovaOS muss die native Basis-Page-Größe der jeweiligen Architektur unterstützen.

Zusätzlich dürfen größere Page-Größen verwendet werden.

Beispiel:

```text
4 KiB
2 MiB
1 GiB
```

Die tatsächlich verfügbaren Größen müssen über die Architekturabstraktion erkannt werden.

## Huge Pages

Große Pages können reduzieren:

```text
Page-Table Memory
TLB Pressure
Translation Overhead
```

Sie können jedoch erhöhen:

```text
Internal Fragmentation
Allocation Constraints
Migration Cost
```

Huge Pages sind daher eine Optimierung und keine universelle Vorgabe.

## TLB

Die Translation Lookaside Buffer (TLB) speichert Adressübersetzungen in der CPU.

```text
Virtual Address
      ↓
TLB Hit
      ↓
Physical Address
```

Nach relevanten Page-Table-Änderungen müssen veraltete Translationen invalidiert werden.

## TLB Shootdown

Auf SMP-Systemen können mehrere CPUs Übersetzungen desselben Address Spaces zwischenspeichern.

```text
CPU 0 changes Mapping
        ↓
Other CPUs may hold old TLB entry
        ↓
TLB Shootdown
```

NovaOS muss betroffene CPUs gezielt invalidieren können.

Globale Shootdowns sollen vermieden werden, wenn eine kleinere Zielmenge ausreicht.

## Address-Space Identifier

Unterstützt die Hardware Address-Space-Identifier oder vergleichbare Mechanismen, sollen diese verwendet werden können.

```text
TLB Entry
├── Virtual Address
└── AddressSpace ID
```

Dadurch können unnötige vollständige TLB-Invalidierungen bei Context Switches reduziert werden.

## NUMA

Demand Paging darf die physische Frame-Auswahl mit NUMA-Policies verbinden.

```text
Page Fault
   ↓
Allocation Policy
   ↓
Preferred NUMA Node
   ↓
Physical Frame
```

Die virtuelle Adresse selbst bestimmt keine NUMA-Lokalität.

## Shared Memory

Ein physischer Frame darf kontrolliert in mehrere Address Spaces eingebunden werden.

```text
VAS A ─┐
       ├→ Physical Frame
VAS B ─┘
```

Berechtigungen dürfen pro Mapping unterschiedlich sein.

## Memory-Mapped Objects

Paging darf auch nicht-anonyme Backing Objects abbilden.

Beispiele:

```text
Files
Shared Memory Objects
Device Memory
Persistent Memory
Executable Images
```

Der VMM muss das Backing Object unabhängig von der Hardware-Page-Table-Struktur modellieren.

## Concurrency

Page-Table-Änderungen müssen SMP-sicher erfolgen.

Konkurrierende Operationen wie:

```text
Map
Unmap
Protect
Fault Handling
Migration
```

dürfen keine inkonsistenten oder teilweise sichtbaren Mapping-Zustände erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Mapped Pages
Page-Table Memory Usage
Page Fault Count
Demand Faults
Protection Faults
Copy-on-Write Faults
Huge Page Usage
TLB Shootdowns
Mapping Statistics
```

## Normative Anforderungen

1. NovaOS MUSS hardwaregestütztes Paging über eine architekturunabhängige Abstraktion unterstützen.
2. Virtuelle Pages und physische Frames MÜSSEN logisch getrennt behandelt werden.
3. Page Tables MÜSSEN Zugriffsrechte für Mappings abbilden können.
4. Die allgemeine Paging-API DARF NICHT von einem spezifischen Hardware-Page-Table-Format abhängen.
5. Map-, Unmap- und Protection-Operationen MÜSSEN SMP-sicher sein.
6. Unmapping DARF physische Frames NICHT freigeben, solange weitere gültige Referenzen bestehen.
7. Demand Paging SOLL unterstützt werden.
8. Page Faults MÜSSEN nach Ursache klassifizierbar sein.
9. Copy-on-Write SOLL als kontrollierter Mapping-Mechanismus unterstützt werden.
10. TLB-Invalidierungen MÜSSEN nach Mapping-Änderungen korrekt durchgeführt werden.
11. TLB Shootdowns SOLLEN auf die tatsächlich betroffenen CPUs begrenzt werden.
12. Paging-, Fault- und Translation-Zustände SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `ADR-MEMORY-0011`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Virtual Address Space
        ↓
Virtual Pages
        ↓
Page Tables
        ↓
MMU + TLB
        ↓
Physical Frames
        ↓
Physical Memory
```

NovaOS erhält damit eine architekturunabhängige Paging-Grundlage für virtuelle Speicherabbildung, Prozessisolation, Demand Paging, Copy-on-Write, Shared Memory und effiziente hardwaregestützte Adressübersetzung.