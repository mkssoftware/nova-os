# NPSPEC-MEMORY-POOL-0001 – Nova Memory Pools

## Status

Entwurf

## Kategorie

Memory / Memory Pool / Deterministic Allocation / Resource Management

## Zweck

NovaOS definiert Memory Pools als vorreservierte Speicherbereiche zur schnellen und kontrollierten Allokation von Objekten oder Speicherblöcken.

```text
Reserved Memory
      ↓
Memory Pool
├── Block
├── Block
├── Block
└── Block
```

Memory Pools dienen insbesondere für vorhersehbare Allokationszeiten, begrenzte Ressourcen und kritische Systempfade.

## Grundprinzip

Ein Memory Pool besitzt eine definierte Menge von Speicherressourcen.

```text
Create Pool
    ↓
Reserve Capacity
    ↓
Allocate / Free
    ↓
Reuse Blocks
```

Dabei gilt:

```text
Memory Pool ≠ General Heap
Memory Pool ≠ Slab Cache
```

Ein Pool besitzt eine explizite Kapazität und einen definierten Verwendungszweck.

## Pool-Modell

Ein Memory Pool verwaltet mindestens:

```text
PoolID
Capacity
BlockSize / AllocationPolicy
Used
Available
State
```

Optional:

```text
Owner
NUMA Node
Resource Budget
Alignment
Maximum Allocation
Security Domain
```

## Pool-Typen

NovaOS darf unterschiedliche Pool-Modelle unterstützen.

```text
Fixed-Size Pool
Variable-Size Pool
Object Pool
Per-CPU Pool
NUMA-local Pool
Emergency Pool
Realtime Pool
```

Die konkrete Implementierung hängt vom Einsatzzweck ab.

## Fixed-Size Pool

Ein Fixed-Size Pool besteht aus gleich großen Blöcken.

```text
Pool
├── 256 B
├── 256 B
├── 256 B
└── 256 B
```

Dies ermöglicht sehr schnelle Allokation und reduziert Fragmentierung.

Typische Operationen:

```text
Allocate()
Free(block)
```

## Variable-Size Pool

Ein Pool darf unterschiedliche Allokationsgrößen unterstützen.

```text
Pool
├── Allocation A
├── Free Region
├── Allocation B
└── Free Region
```

Dabei müssen Fragmentierung und Allokationskosten berücksichtigt werden.

Für stark deterministische Pfade sollen Fixed-Size Pools bevorzugt werden.

## Reservierung

Pool-Speicher wird kontrolliert aus einem darunterliegenden Memory-System bezogen.

```text
PMM / Frame Allocator
        ↓
Memory Pool
        ↓
Dedicated Capacity
```

Ein Pool darf seinen Speicher vollständig beim Erstellen reservieren oder entsprechend einer definierten Policy kontrolliert erweitern.

## Harte Kapazitätsgrenze

Pools dürfen eine feste Obergrenze besitzen.

```text
Capacity = 1024 Blocks
Used     = 1024 Blocks

Next Allocation
      ↓
Pool Exhausted
```

Eine erschöpfte Pool-Kapazität muss als expliziter Zustand behandelt werden.

Der Pool darf nicht unkontrolliert auf andere Speicherquellen ausweichen.

## Fallback

Fallback-Verhalten muss explizit definiert sein.

Mögliche Policies:

```text
No Fallback
Fallback to Parent Pool
Fallback to General Allocator
Wait for Resource
Fail Allocation
```

Für Realtime- oder sicherheitskritische Pools darf ein impliziter Fallback nicht erfolgen.

## Ownership

Ein Pool muss einen klar definierten Besitzer oder Lebenszyklus besitzen.

```text
Subsystem
   ↓ owns
Memory Pool
   ↓
Allocations
```

Beim Entfernen eines Pools müssen alle verbleibenden Allokationen behandelt werden.

Ein Pool darf nicht zerstört werden, solange aktive Objekte unkontrolliert auf seinen Speicher verweisen.

## Resource Budgets

Memory Pools dürfen direkt mit Nova Resource Budgets verbunden werden.

```text
ExecutionContract
      ↓
Memory Budget
      ↓
Memory Pool
```

Damit kann ein Subsystem nur die ihm explizit zugewiesene Speichermenge verwenden.

## Realtime

Realtime-Pools sollen Speicher bereits vor dem zeitkritischen Pfad reservieren.

```text
Initialization
     ↓
Preallocate Pool
     ↓
Realtime Execution
     ↓
Constant / Bounded Allocation
```

Dadurch werden unvorhersehbare globale Speicherallokationen während zeitkritischer Operationen vermieden.

Für Hard-Realtime müssen Allokations- und Freigabezeiten analysierbar und begrenzt sein.

## Emergency Pools

NovaOS darf reservierte Emergency Pools besitzen.

```text
Normal Memory Exhausted
        ↓
Critical Operation
        ↓
Emergency Pool
```

Sie dürfen beispielsweise für Recovery, Logging, Crash Handling oder kritische Kerneloperationen verwendet werden.

Emergency Memory darf nicht für normale Allokationen verbraucht werden.

## Per-CPU Pools

Häufig verwendete Pools dürfen CPU-lokal organisiert werden.

```text
CPU 0 → Pool 0
CPU 1 → Pool 1
CPU 2 → Pool 2
```

Dadurch können Synchronisationskosten und Cache-Contention reduziert werden.

Eine kontrollierte Rebalancing- oder Rückgabestrategie darf vorgesehen werden.

## NUMA

Pools dürfen einem NUMA-Node zugeordnet werden.

```text
NUMA Node 0
└── Local Pool

NUMA Node 1
└── Local Pool
```

NUMA-Lokalität soll bei der Reservierung berücksichtigt werden.

Ein Fallback auf entfernte Nodes darf nur entsprechend der Pool-Policy erfolgen.

## Sicherheit

Speicher aus Pools muss dieselben Sicherheitsregeln wie andere Speicherallokationen einhalten.

Bei Wiederverwendung zwischen Sicherheitskontexten kann erforderlich sein:

```text
Zero
Reset
Validate
```

Freigegebene Pool-Objekte dürfen nicht weiter verwendet werden.

## Concurrency

Pools müssen entsprechend ihres Einsatzbereichs synchronisiert werden.

Mögliche Strategien:

```text
Per-CPU Pools
Atomic Free Lists
Lock-Free Structures
Mutex-protected Pools
```

Die Synchronisationsstrategie darf entsprechend den Anforderungen des Pools gewählt werden.

## Introspection

Memory Pools sollen mindestens folgende Informationen bereitstellen können:

```text
PoolID
Owner
Capacity
Used
Available
Peak Usage
Allocation Failures
Fallback Count
NUMA Location
Pool Type
```

Pool-Erschöpfung muss diagnostizierbar sein.

## Normative Anforderungen

1. NovaOS MUSS explizit begrenzte Memory Pools unterstützen können.
2. Jeder Pool MUSS eine definierte Kapazität und einen definierten Lebenszyklus besitzen.
3. Eine Pool-Allokation DARF die konfigurierte harte Kapazitätsgrenze NICHT unkontrolliert überschreiten.
4. Fallback-Verhalten MUSS explizit definiert sein.
5. Kritische Pools DÜRFEN keinen impliziten Fallback auf den allgemeinen Heap durchführen.
6. Fixed-Size Pools SOLLEN für häufige deterministische Allokationen unterstützt werden.
7. Realtime-Pools SOLLEN Speicher vor Eintritt in zeitkritische Ausführung reservieren können.
8. Emergency Pools DÜRFEN NICHT durch normale Allokationen erschöpft werden.
9. NUMA- und CPU-Lokalität SOLLEN unterstützt werden.
10. Pool-Allokationen MÜSSEN entsprechend ihrer Concurrency-Anforderungen synchronisiert sein.
11. Speicher vorheriger Sicherheitskontexte DARF nicht unkontrolliert weitergegeben werden.
12. Kapazität, Nutzung, Peak Usage und Allokationsfehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-SLAB-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-MEMORY-0005`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Physical / Kernel Memory
          ↓
     Memory Pool
   ┌──────┼───────┐
   ↓      ↓       ↓
Fixed   Realtime Emergency
   ↓      ↓       ↓
Bounded Resource Allocation
          ↓
Predictable Memory Usage
```

NovaOS erhält damit einen explizit begrenzten Speichermechanismus für deterministische, ressourcenisolierte und kritische Allokationen, der Memory Budgets, Realtime-Anforderungen, NUMA-Lokalität und kontrollierte Fehlerbehandlung unterstützt.