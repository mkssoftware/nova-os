# NPSPEC-MEMORY-PMM-0001 – Nova Physical Memory Manager

## Status

Entwurf

## Kategorie

Memory / Physical Memory Management / Kernel / Resource Management

## Zweck

NovaOS definiert den Physical Memory Manager (PMM) als zentrale Instanz zur Verwaltung physischer Speicherseiten.

```text
Firmware Memory Map
        ↓
       PMM
        ↓
Physical Page Management
   ├── Free
   ├── Allocated
   ├── Reserved
   └── Unavailable
```

Der PMM verwaltet physischen Speicher unabhängig von virtuellen Adressen und stellt Seiten für Kernel, Virtual Memory Manager, Treiber und weitere Systemkomponenten bereit.

## Grundprinzip

Der PMM verwaltet physische Speicherbereiche in festen Seiteneinheiten.

```text
Physical Memory
├── Page 0
├── Page 1
├── Page 2
├── ...
└── Page N
```

Dabei gilt:

```text
Physical Memory ≠ Virtual Memory
PMM ≠ Heap Allocator
```

Der PMM verwaltet physische Seiten.

Virtuelle Adressräume und deren Mapping werden durch höhere Memory-Komponenten verwaltet.

## Initialisierung

Der PMM wird während des frühen Kernelstarts aus der vom Boot-System bereitgestellten Memory Map initialisiert.

```text
Boot Information
      ↓
Firmware Memory Map
      ↓
Validate
      ↓
Normalize
      ↓
Reserve Critical Regions
      ↓
Initialize PMM
```

Nur explizit als verwendbar klassifizierter Speicher darf dem allgemeinen Seitenpool hinzugefügt werden.

## Speicherzustände

Physische Seiten müssen mindestens folgende Zustände besitzen können:

```text
Free
Allocated
Reserved
Unavailable
```

Optional können zusätzliche Zustände verwendet werden:

```text
Firmware
Device
Persistent
Offline
Bad
```

Unbekannter Speicher darf nicht automatisch als frei betrachtet werden.

```text
Unknown ≠ Free
```

## Reservierte Bereiche

Der PMM muss kritische Speicherbereiche vor normaler Allokation schützen.

Dazu gehören beispielsweise:

```text
Kernel Image
Boot Information
Page Tables
Framebuffer
Firmware Regions
ACPI Structures
MMIO Regions
Crash Dump Memory
Reserved Hardware Memory
```

Reservierte Seiten dürfen nicht über normale PMM-Allokationen vergeben werden.

## Seitenallokation

Der PMM stellt mindestens die grundlegenden Operationen bereit:

```text
AllocatePage()
AllocatePages(count)
FreePage(page)
FreePages(range)
ReserveRange(range)
```

Eine erfolgreiche Allokation muss physische Seiten eindeutig dem Aufrufer zuordnen.

## Kontinuierlicher Speicher

Bestimmte Hardware- oder DMA-Anforderungen können physisch zusammenhängenden Speicher benötigen.

```text
AllocateContiguous(
    PageCount,
    Alignment,
    Constraints
)
```

Der PMM soll solche Anforderungen unterstützen können, darf jedoch keine kontinuierliche Verfügbarkeit garantieren.

## Alignment

Allokationen dürfen Alignment-Anforderungen besitzen.

```text
4 KiB
2 MiB
1 GiB
Device-specific Alignment
```

Die konkrete Seitengröße und unterstützte Huge-Page-Größe wird durch Architektur und HAL bestimmt.

## NUMA

Auf NUMA-Systemen muss physischer Speicher seiner Topologie zugeordnet werden können.

```text
NUMA Node 0
├── Memory

NUMA Node 1
├── Memory
```

Allokationen dürfen einen bevorzugten NUMA-Knoten angeben.

```text
Preferred Node
     ↓
Local Allocation
     ↓
Fallback if allowed
```

NUMA-Optimierung darf harte Speicheranforderungen nicht verletzen.

## Memory Zones

Der PMM darf physische Speicherbereiche nach Eigenschaften oder Hardwarebeschränkungen klassifizieren.

Beispiele:

```text
Normal Memory
DMA-reachable Memory
Low Memory
Persistent Memory
Device-specific Memory
```

Zones dürfen nicht als feste architekturübergreifende Speicheraufteilung vorausgesetzt werden.

## Allokationsstrategien

Die interne PMM-Implementierung darf unterschiedliche Verfahren kombinieren.

Beispiele:

```text
Bitmap
Free Lists
Buddy Allocator
Per-CPU Page Cache
NUMA-local Pools
```

Die konkrete Implementierung ist kein Bestandteil der externen PMM-Semantik.

## Per-CPU Cache

Häufige Einzel-Seiten-Allokationen dürfen über lokale Caches beschleunigt werden.

```text
CPU
 ↓
Local Page Cache
 ↓
Global / NUMA Pool
```

Dadurch sollen globale Synchronisation und Cache-Contention reduziert werden.

Lokale Caches bleiben Bestandteil des globalen Ressourcenmodells.

## Speicherfreigabe

Eine physische Seite darf nur freigegeben werden, wenn sie nicht mehr verwendet wird.

```text
Allocated
   ↓
References Removed
   ↓
Mappings Removed
   ↓
Free
```

Double-Free und die Freigabe reservierter Seiten müssen erkannt oder verhindert werden.

## Zeroing

Speicher, der zwischen unterschiedlichen Sicherheitsdomänen übertragen wird, muss vor unzulässiger Informationsweitergabe geschützt werden.

```text
Previous Owner
     ↓
Physical Page
     ↓
Zero / Sanitize
     ↓
New Owner
```

Zeroing darf beim Free, Allocate oder über kontrollierte Hintergrundverarbeitung erfolgen.

## Memory Pressure

Der PMM muss den verfügbaren physischen Speicher beobachten können.

```text
Free Pages
    ↓
Low Threshold
    ↓
Memory Pressure
```

Memory Pressure muss höheren Speicherverwaltungs- und Ressourcenmechanismen signalisiert werden können.

Der PMM selbst entscheidet nicht über komplexe Prozess- oder Cache-Reclamation-Policies.

## Hotplug

Physischer Speicher darf dynamisch hinzugefügt oder entfernt werden, sofern Plattform und Hardware dies unterstützen.

```text
Memory Online
     ↓
Validate
     ↓
Add to PMM

Memory Offline
     ↓
Migrate / Drain
     ↓
Remove from PMM
```

Speicher darf erst entfernt werden, wenn keine nicht migrierbaren aktiven Belegungen verbleiben.

## Introspection

Der PMM soll mindestens folgende Informationen bereitstellen können:

```text
Total Physical Memory
Free Memory
Allocated Memory
Reserved Memory
Unavailable Memory
Memory per NUMA Node
Allocation Failures
Fragmentation Information
```

Diese Daten müssen für Diagnose und Ressourcenplanung verfügbar sein.

## Normative Anforderungen

1. NovaOS MUSS einen zentral definierten Physical Memory Manager besitzen.
2. Der PMM MUSS aus einer validierten Boot-Memory-Map initialisiert werden.
3. Unbekannte Speicherbereiche DÜRFEN NICHT automatisch als frei behandelt werden.
4. Kernel-, Firmware-, MMIO- und andere reservierte Bereiche MÜSSEN vor normaler Allokation geschützt werden.
5. Der PMM MUSS einzelne und mehrere physische Seiten allozieren und freigeben können.
6. Physisch zusammenhängende und ausgerichtete Allokationen SOLLEN unterstützt werden.
7. NUMA-Zugehörigkeit MUSS auf NUMA-Systemen abbildbar sein.
8. Sicherheitsdomänen DÜRFEN keine Speicherinhalte vorheriger Besitzer unkontrolliert erhalten.
9. Double-Free und ungültige Freigaben MÜSSEN verhindert oder erkannt werden.
10. Memory Pressure MUSS an höhere Ressourcenmechanismen meldbar sein.
11. Memory Hotplug SOLL unterstützt werden, sofern die Plattform dies ermöglicht.
12. PMM-Zustand und Speichernutzung SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-MEMORY-0001`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Firmware / Boot Memory Map
          ↓
     Validation
          ↓
Physical Memory Manager
   ┌──────┼──────┐
   ↓      ↓      ↓
 Free   Used   Reserved
   ↓
Page Allocation
   ↓
Kernel / VMM / Drivers
```

NovaOS erhält damit eine architekturunabhängige Grundlage für die sichere und effiziente Verwaltung des gesamten physischen Speichers, auf der virtuelle Speicherverwaltung, Kernel-Allokatoren und weitere Memory-Subsysteme aufbauen können.