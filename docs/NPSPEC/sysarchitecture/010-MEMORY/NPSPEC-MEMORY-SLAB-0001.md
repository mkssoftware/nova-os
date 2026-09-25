# NPSPEC-MEMORY-SLAB-0001 – Nova Slab Allocator

## Status

Angenommen

## Kategorie

Memory / Kernel Allocation / Object Cache / Slab Allocation

## Zweck

NovaOS definiert den Slab Allocator zur effizienten Allokation häufig verwendeter Kernelobjekte fester oder ähnlicher Größe.

```text
PMM
 ↓
Frame Allocator
 ↓
Slab Allocator
 ↓
Kernel Objects
```

Der Slab Allocator reduziert Allokationskosten, Fragmentierung und wiederholte Initialisierung häufig verwendeter Kernelstrukturen.

## Grundprinzip

Für einen bestimmten Objekttyp wird ein eigener Cache verwaltet.

```text
Object Cache
├── Slab
│   ├── Object
│   ├── Object
│   └── Object
├── Slab
└── Slab
```

Dabei gilt:

```text
Slab Allocator ≠ PMM
Slab Allocator ≠ Frame Allocator
```

Der Frame Allocator stellt physischen Speicher bereit.

Der Slab Allocator zerlegt diesen in wiederverwendbare Kernelobjekte.

## Object Cache

Ein Cache beschreibt einen bestimmten Objekttyp oder eine Größenklasse.

Beispiele:

```text
Task Cache
Process Cache
VFS Node Cache
Capability Cache
IPC Object Cache
Generic Size Cache
```

Ein Cache verwaltet mindestens:

```text
ObjectSize
Alignment
Slabs
FreeObjects
AllocatedObjects
```

Optional:

```text
Constructor
Destructor
NUMA Node
Statistics
Debug Flags
```

## Slab

Ein Slab besteht aus einer oder mehreren Speicherseiten und enthält mehrere Objekte desselben Caches.

```text
Slab
├── Object 0
├── Object 1
├── Object 2
├── Object 3
└── Metadata
```

Slabs können mindestens klassifiziert werden als:

```text
Empty
Partial
Full
```

Allokationen sollen bevorzugt aus bereits teilweise belegten Slabs erfolgen.

## Allokation

Der typische Ablauf lautet:

```text
Allocate Object
      ↓
Local Cache Available?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
Return Object   Find Partial Slab
                     ↓
                No Slab Available?
                     ↓
               Request Frames
                     ↓
                 Create Slab
```

Häufige Allokationen sollen ohne Zugriff auf globale Speicherstrukturen möglich sein.

## Freigabe

Bei der Freigabe wird das Objekt an seinen Cache zurückgegeben.

```text
Object
  ↓
Free
  ↓
Slab Free List
```

Wird ein Slab vollständig frei, darf er:

```text
Retained
Reused
Released to Frame Allocator
```

werden.

Die Entscheidung darf von Memory Pressure und Cache-Policy abhängen.

## Größenklassen

Neben typisierten Objekt-Caches darf NovaOS generische Größenklassen bereitstellen.

Beispiel:

```text
16 B
32 B
64 B
128 B
256 B
512 B
1 KiB
2 KiB
...
```

Eine Anforderung wird der kleinsten geeigneten Größenklasse zugeordnet.

Sehr große Allokationen dürfen direkt an einen geeigneteren Memory Allocator weitergeleitet werden.

## Alignment

Objekte müssen ihre Alignment-Anforderungen erfüllen.

```text
Object Address % Alignment = 0
```

Cache-Line-Alignment darf für häufig konkurrierend verwendete Strukturen eingesetzt werden, um False Sharing zu reduzieren.

## Konstruktoren

Caches dürfen optionale Konstruktoren besitzen.

```text
New Slab Object
      ↓
Constructor
      ↓
Initialized Object
```

Dadurch können wiederkehrende Initialisierungsschritte reduziert werden.

Sicherheitskritischer oder dynamischer Objektzustand muss dennoch bei jeder Wiederverwendung korrekt zurückgesetzt werden.

## Per-CPU Caches

NovaOS soll CPU-lokale Objekt-Caches unterstützen.

```text
CPU 0 → Local Objects
CPU 1 → Local Objects
CPU 2 → Local Objects
```

Fast Path:

```text
Allocate
   ↓
Per-CPU Cache
   ↓
Object
```

Dadurch werden globale Locks und Cache-Line-Contention reduziert.

Lokale Caches dürfen bei Memory Pressure geleert werden.

## NUMA

Slabs sollen einem NUMA-Node zugeordnet werden können.

```text
NUMA Node
   ↓
Slab Cache
   ↓
Local Objects
```

Lokale Speicherallokation soll bevorzugt werden.

Remote Allocation darf entsprechend der Ressourcen- und Fallback-Policy erfolgen.

## Concurrency

Slab-Caches müssen SMP-sicher sein.

Der Fast Path soll möglichst über:

```text
Per-CPU State
Atomic Operations
Local Free Lists
```

arbeiten.

Globale Synchronisation soll auf notwendige Slow-Path-Operationen beschränkt werden.

## Memory Pressure

Bei Speicherdruck darf der Slab Allocator ungenutzte Slabs zurückgeben.

```text
Memory Pressure
      ↓
Find Empty Slabs
      ↓
Release Frames
      ↓
Frame Allocator
```

Objekte in aktiver Verwendung dürfen dabei nicht beeinflusst werden.

## Sicherheit

Wiederverwendete Objekte dürfen keine vertraulichen Daten vorheriger Sicherheitskontexte offenlegen.

Je nach Cache können erforderlich sein:

```text
Zeroing
State Reset
Poisoning
Validation
```

Freigegebene Objekte dürfen nicht weiter verwendet werden.

```text
Free → Use
```

ist ein Use-After-Free-Fehler.

## Debugging

Debug-Modi dürfen zusätzliche Schutzmechanismen aktivieren.

Beispiele:

```text
Object Poisoning
Red Zones
Canaries
Double-Free Detection
Use-After-Free Detection
Allocation Tracking
```

Diese Mechanismen dürfen im Produktionsbetrieb abhängig von Kosten und Sicherheitsanforderungen reduziert werden.

## Introspection

Der Slab Allocator soll mindestens bereitstellen können:

```text
Cache Name
Object Size
Allocated Objects
Free Objects
Slab Count
Empty / Partial / Full Slabs
Memory Usage
Per-CPU Cache Usage
Allocation Failures
```

## Normative Anforderungen

1. NovaOS MUSS einen Kernel-Allocator für kleine und häufig verwendete Objekte bereitstellen.
2. Der Slab Allocator MUSS Speicher über den Physical Memory Manager beziehungsweise Frame Allocator beziehen.
3. Objekte gleicher Art oder Größenklasse SOLLEN in eigenen Caches organisiert werden können.
4. Slabs MÜSSEN mindestens als frei, teilweise belegt oder vollständig belegt bestimmbar sein.
5. Häufige Allokationen SOLLEN über einen effizienten Per-CPU-Fast-Path möglich sein.
6. Slab-Allokation und -Freigabe MÜSSEN SMP-sicher sein.
7. NUMA-Lokalität SOLL bei der Erzeugung und Nutzung von Slabs berücksichtigt werden.
8. Vollständig freie Slabs MÜSSEN bei Memory Pressure zurückgegeben werden können.
9. Double-Free und beschädigte Cache-Zustände MÜSSEN verhindert oder erkannt werden.
10. Wiederverwendete Objekte DÜRFEN keine unzulässigen Daten vorheriger Besitzer offenlegen.
11. Alignment-Anforderungen der Objekttypen MÜSSEN eingehalten werden.
12. Cache-, Slab- und Allokationszustände SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-BUDDY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-MEMORY-0004`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Physical Frames
      ↓
Slab Cache
      ↓
┌─────┼─────────┐
↓     ↓         ↓
Empty Partial  Full
      ↓
Reusable Kernel Objects
      ↓
Tasks / VFS / IPC / Capabilities / Kernel
```

NovaOS erhält damit einen schnellen, SMP- und NUMA-fähigen Kernel-Objektallocator, der physische Frames effizient in wiederverwendbare Objekt-Caches überführt und Allokationskosten sowie Fragmentierung reduziert.