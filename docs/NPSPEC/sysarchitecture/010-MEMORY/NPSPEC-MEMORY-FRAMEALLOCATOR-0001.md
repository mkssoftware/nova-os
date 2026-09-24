# NPSPEC-MEMORY-FRAMEALLOCATOR-0001 – Nova Physical Frame Allocator

## Status

Entwurf

## Kategorie

Memory / Physical Frames / Allocation / Kernel

## Zweck

NovaOS definiert den Physical Frame Allocator als spezialisierte Komponente des Physical Memory Managers zur schnellen Vergabe und Rückgabe physischer Speicherframes.

```text
PMM
 ↓
Frame Allocator
 ├── Allocate Frame
 ├── Allocate Frames
 └── Free Frames
```

Der Frame Allocator arbeitet auf den vom PMM verwalteten nutzbaren physischen Speicherbereichen.

## Grundprinzip

Ein Frame ist die kleinste durch den Frame Allocator verwaltete physische Speichereinheit.

```text
Physical Memory
├── Frame 0
├── Frame 1
├── Frame 2
├── ...
└── Frame N
```

Dabei gilt:

```text
Frame ≠ Virtual Page
Frame Allocator ≠ Heap Allocator
```

Ein virtueller Page kann durch den Virtual Memory Manager auf einen physischen Frame abgebildet werden.

## Beziehung zum PMM

Der PMM definiert, welche physischen Speicherbereiche verwendet werden dürfen.

Der Frame Allocator verwaltet deren konkrete Vergabe.

```text
Firmware Memory Map
        ↓
       PMM
        ↓
Usable Physical Memory
        ↓
Frame Allocator
        ↓
Allocated Frames
```

Der Frame Allocator darf keine vom PMM als reserviert oder nicht verfügbar markierten Bereiche vergeben.

## Frame-Zustände

Ein verwalteter Frame besitzt mindestens einen der Zustände:

```text
Free
Allocated
Reserved
Unavailable
```

Optional können Diagnosezustände verwendet werden:

```text
Quarantined
Offline
Bad
```

Der Übergang:

```text
Free → Allocated → Free
```

muss kontrolliert erfolgen.

## Einzelallokation

Die grundlegende Operation lautet konzeptionell:

```text
AllocateFrame()
```

Ergebnis:

```text
PhysicalFrame
```

oder:

```text
AllocationFailure
```

Eine erfolgreiche Allokation muss einen eindeutig reservierten Frame liefern.

## Mehrfachallokation

Der Allocator soll mehrere Frames effizient vergeben können.

```text
AllocateFrames(count)
```

Die zurückgegebenen Frames müssen nicht zwingend physisch zusammenhängend sein.

```text
Frame 10
Frame 27
Frame 31
Frame 82
```

Physisch zusammenhängende Anforderungen müssen explizit angegeben werden.

## Kontinuierliche Frames

Für DMA, Huge Pages oder bestimmte Hardwareanforderungen muss eine zusammenhängende Allokation möglich sein.

```text
AllocateContiguous(
    Count,
    Alignment,
    Constraints
)
```

Beispiel:

```text
Frame 100
Frame 101
Frame 102
Frame 103
```

Eine solche Allokation darf fehlschlagen, obwohl insgesamt genügend freie Frames vorhanden sind.

```text
Free Memory Available
        ≠
Contiguous Memory Available
```

## Alignment

Allokationen dürfen Alignment-Anforderungen besitzen.

```text
Frame Alignment
Huge Page Alignment
DMA Alignment
Device Alignment
```

Der Allocator muss diese Anforderungen berücksichtigen können.

## Allocator-Struktur

Die interne Implementierung darf mehrere Verfahren kombinieren.

Beispiele:

```text
Bitmap
Buddy Allocator
Free Lists
Hierarchical Bitmap
Per-CPU Frame Cache
NUMA-local Pools
```

Die konkrete Datenstruktur ist nicht Bestandteil der öffentlichen Frame-Allocator-Semantik.

## Buddy Allocation

Für zusammenhängende Bereiche darf ein Buddy-System verwendet werden.

```text
Large Free Block
      ↓ split
┌───────────┬───────────┐
Buddy A     Buddy B
```

Freigegebene kompatible Buddies dürfen wieder zusammengeführt werden.

```text
Buddy A + Buddy B
        ↓
    Larger Block
```

Dies reduziert externe Fragmentierung bei kontinuierlichen Allokationen.

## Per-CPU Frame Cache

Häufige Einzelallokationen dürfen über CPU-lokale Frame-Caches beschleunigt werden.

```text
CPU
 ↓
Local Frame Cache
 ↓
NUMA / Global Allocator
```

Dadurch können Synchronisation und Cache-Line-Contention reduziert werden.

Lokale Caches dürfen Frames jedoch nicht dauerhaft dem globalen Ressourcenmanagement entziehen.

## NUMA

Frames müssen ihrer physischen NUMA-Zugehörigkeit zugeordnet werden können.

```text
NUMA Node 0
└── Frame Pool

NUMA Node 1
└── Frame Pool
```

Allokationen dürfen angeben:

```text
PreferredNode
AllowedNodes
LocalityPreference
```

Kann lokal kein geeigneter Frame bereitgestellt werden, darf entsprechend der Allokations-Policy auf andere Nodes ausgewichen werden.

## Constraints

Eine Frame-Allokation darf zusätzliche Anforderungen besitzen.

Beispiele:

```text
Physical Address Range
NUMA Node
DMA Reachability
Alignment
Contiguous
Page Size
Memory Type
```

Harte Constraints dürfen bei der Allokation nicht verletzt werden.

## Freigabe

Frames werden über eine kontrollierte Operation zurückgegeben.

```text
FreeFrame(frame)
```

oder:

```text
FreeFrames(frames)
```

Vor der Rückgabe muss sichergestellt sein, dass keine aktiven Mappings, DMA-Operationen oder sonstigen legitimen Nutzer mehr auf den Frame zugreifen.

## Double-Free

Ein bereits freier Frame darf nicht erneut freigegeben werden.

```text
Allocated → Free
```

ist gültig.

```text
Free → Free
```

ist ein Fehler.

Debug- und Verifikationsmodi sollen ungültige Frame-Zustandsübergänge erkennen.

## Zeroing

Der Frame Allocator darf Frames vor der Wiederverwendung nullen.

```text
Free Frame
    ↓
Zero
    ↓
Allocate
```

Alternativ darf Zeroing kontrolliert durch den PMM oder einen Hintergrundmechanismus erfolgen.

Ein neuer Sicherheitskontext darf keine vertraulichen Daten des vorherigen Besitzers erhalten.

## Fragmentierung

Der Frame Allocator soll Fragmentierung beobachten können.

```text
Free Frames: 10000
Largest Contiguous Block: 128
```

Beide Werte beschreiben unterschiedliche Eigenschaften des verfügbaren Speichers.

Fragmentierungsinformationen dürfen für Huge Pages, DMA und Ressourcenplanung verwendet werden.

## Concurrency

Frame-Allokationen müssen SMP-sicher sein.

```text
CPU 0 ─┐
CPU 1 ─┼→ Frame Allocator
CPU 2 ─┤
CPU 3 ─┘
```

Kein Frame darf gleichzeitig mehreren unabhängigen Allokationen zugeteilt werden.

Globale Synchronisationspunkte sollen möglichst reduziert werden.

## Introspection

Der Frame Allocator soll mindestens bereitstellen können:

```text
Free Frame Count
Allocated Frame Count
Frames per NUMA Node
Largest Free Block
Fragmentation
Allocation Failures
Contiguous Allocation Failures
Per-CPU Cache Usage
```

## Normative Anforderungen

1. NovaOS MUSS einen definierten Physical Frame Allocator bereitstellen.
2. Der Frame Allocator DARF nur vom PMM freigegebenen physischen Speicher verwalten.
3. Ein Frame DARF gleichzeitig nur einer gültigen Allokation zugeordnet sein.
4. Einzel- und Mehrfachallokationen MÜSSEN unterstützt werden.
5. Zusammenhängende und ausgerichtete Frame-Allokationen SOLLEN unterstützt werden.
6. Harte Allokations-Constraints DÜRFEN NICHT verletzt werden.
7. NUMA-Zugehörigkeit MUSS auf NUMA-Systemen berücksichtigt werden können.
8. Frame-Allokation und -Freigabe MÜSSEN SMP-sicher sein.
9. Double-Free und ungültige Zustandsübergänge MÜSSEN verhindert oder erkannt werden.
10. Frames DÜRFEN nicht freigegeben werden, solange legitime Zugriffe bestehen.
11. Speicherinhalte vorheriger Sicherheitsdomänen DÜRFEN nicht unkontrolliert weitergegeben werden.
12. Frame-Nutzung, Fragmentierung und Allokationsfehler SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `ADR-MEMORY-0002`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
PMM Usable Memory
        ↓
Physical Frame Allocator
   ┌────┼─────────┐
   ↓    ↓         ↓
Single Batch   Contiguous
Frame  Frames    Frames
   ↓    ↓         ↓
Kernel / VMM / DMA / Drivers
```

NovaOS erhält damit einen schnellen, SMP- und NUMA-fähigen Frame Allocator, der die konkrete Vergabe physischer Speicherframes übernimmt und als fundamentale Grundlage für Virtual Memory, Page Tables, DMA und weitere Kernel-Speicherdienste dient.