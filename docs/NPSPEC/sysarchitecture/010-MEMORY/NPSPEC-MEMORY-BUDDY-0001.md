# NPSPEC-MEMORY-BUDDY-0001 – Nova Buddy Allocator

## Status

Angenommen

## Kategorie

Memory / Physical Memory / Buddy Allocation / Frame Management

## Zweck

NovaOS definiert den Buddy Allocator als Mechanismus zur effizienten Verwaltung zusammenhängender physischer Frame-Blöcke unterschiedlicher Größen.

```text
Physical Memory
      ↓
Buddy Allocator
      ↓
Power-of-Two Blocks
   ├── Split
   ├── Allocate
   ├── Free
   └── Merge
```

Der Buddy Allocator bildet eine mögliche zentrale Allokationsstrategie des Physical Frame Allocators.

## Grundprinzip

Physischer Speicher wird in Blöcke mit Größen als Zweierpotenzen organisiert.

```text
Order 0 → 1 Frame
Order 1 → 2 Frames
Order 2 → 4 Frames
Order 3 → 8 Frames
...
Order N → 2^N Frames
```

Dabei gilt:

```text
BlockSize(order) = BaseFrameSize × 2^order
```

Ein größerer Block kann rekursiv in zwei gleich große Buddy-Blöcke geteilt werden.

## Orders

Jede Blockgröße besitzt einen eigenen Order.

Beispiel bei 4-KiB-Basisframes:

```text
Order 0 →   4 KiB
Order 1 →   8 KiB
Order 2 →  16 KiB
Order 3 →  32 KiB
Order 4 →  64 KiB
...
```

Die maximal unterstützte Order darf von Speicherbereich, NUMA-Node und Architektur abhängen.

## Free Lists

Für jede Order wird konzeptionell eine Menge freier Blöcke verwaltet.

```text
Order 0 → [ ][ ][ ]
Order 1 → [ ][ ]
Order 2 → [ ]
Order 3 → [ ][ ]
```

Eine Allokation sucht zunächst einen freien Block der benötigten Order.

Ist keiner vorhanden, darf ein größerer Block geteilt werden.

## Split

Beispiel:

```text
Order 3
████████

   ↓ Split

Order 2
████  ████
```

Wird nur ein Buddy benötigt:

```text
Allocate ████
Free     ████
```

Das Teilen darf rekursiv erfolgen:

```text
Order 4
   ↓
Order 3 + Order 3
             ↓
        Order 2 + Order 2
```

## Allocation

Eine Anforderung wird auf die kleinste passende Order abgebildet.

```text
Requested Frames
      ↓
Required Order
      ↓
Free Block Available?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
Allocate      Higher Order
                  ↓
                Split
```

Die Allokation muss die Alignment-Eigenschaften der jeweiligen Order erhalten.

## Free

Bei der Freigabe wird der zugehörige Buddy bestimmt.

```text
Allocated Block
      ↓
Free
      ↓
Buddy Free?
```

Ist der Buddy ebenfalls frei und kompatibel, werden beide zusammengeführt.

## Merge

```text
Order 2
████  ████
Free  Free
   \  /
    ↓
Order 3
████████
```

Das Zusammenführen darf rekursiv bis zur maximal möglichen Order fortgesetzt werden.

## Buddy-Bestimmung

Der Buddy eines Blocks muss deterministisch aus physischer Position und Order bestimmbar sein.

Konzeptionell:

```text
BuddyAddress =
BlockAddress XOR BlockSize
```

Die konkrete Berechnung muss die verwendete physische Speicherregion und Basisadresse berücksichtigen.

## Fragmentierung

Der Buddy Allocator reduziert externe Fragmentierung durch das Zusammenführen freier Buddies.

Interne Fragmentierung bleibt möglich.

Beispiel:

```text
Request: 5 Frames
Required Block: 8 Frames
Unused: 3 Frames
```

Dabei gilt:

```text
Available Memory ≠ Suitable Buddy Block
```

Eine große Allokation kann fehlschlagen, obwohl insgesamt ausreichend freier Speicher vorhanden ist.

## Reservierte Bereiche

Reservierte oder nicht nutzbare physische Bereiche dürfen nicht Teil frei zusammenführbarer Buddy-Blöcke sein.

```text
Free | Free | Reserved | Free
```

Ein Merge darf niemals über eine solche Grenze hinweg erfolgen.

## NUMA

Auf NUMA-Systemen soll jeder Node eigene Buddy-Strukturen besitzen können.

```text
NUMA Node 0
└── Buddy Allocator

NUMA Node 1
└── Buddy Allocator
```

Lokale Allokationen sollen bevorzugt werden.

Ein Fallback auf andere Nodes darf nur entsprechend der Allokations-Policy erfolgen.

## Memory Zones

Unterschiedliche Speicherklassen dürfen getrennte Buddy-Pools verwenden.

```text
Node
├── Normal
├── DMA
└── Special
```

Blöcke unterschiedlicher Speicherklassen dürfen nicht unkontrolliert miteinander zusammengeführt werden.

## Concurrency

Buddy-Operationen müssen SMP-sicher sein.

```text
CPU 0 ─┐
CPU 1 ─┼→ Buddy Allocator
CPU 2 ─┘
```

Synchronisation darf pro Node, Zone oder Order partitioniert werden, um globale Contention zu reduzieren.

Kleine häufige Allokationen dürfen über Per-CPU-Caches abgefangen werden.

## Metadaten

Für verwaltete Blöcke müssen mindestens Informationen über deren Zustand und Order bestimmbar sein.

```text
Physical Address
Order
State
Node / Zone
```

Metadaten dürfen nicht zu einer unkontrollierten Rekursion in den Frame Allocator führen.

## Hotplug

Neu hinzugefügter physischer Speicher darf in geeignete Buddy-Strukturen integriert werden.

Beim Entfernen von Speicher müssen betroffene Blöcke zunächst aus der Allokation genommen werden.

```text
Memory Offline
     ↓
Mark Unavailable
     ↓
Drain Allocations
     ↓
Remove Buddy Region
```

## Fehlererkennung

Der Allocator soll insbesondere erkennen können:

```text
Double Free
Invalid Order
Misaligned Block
Overlapping Blocks
Merge with Reserved Memory
Corrupted Free List
```

Debug-Modi dürfen zusätzliche Integritätsprüfungen durchführen.

## Introspection

Der Buddy Allocator soll mindestens bereitstellen können:

```text
Free Blocks per Order
Free Frames
Largest Available Order
Fragmentation
Split Count
Merge Count
Allocation Failures
Node / Zone Statistics
```

## Normative Anforderungen

1. NovaOS SOLL Buddy Allocation für physisch zusammenhängende Frame-Blöcke unterstützen.
2. Blockgrößen MÜSSEN als Zweierpotenzen der Basisframegröße organisiert werden.
3. Eine Allokation MUSS die kleinste geeignete verfügbare Order verwenden können.
4. Größere freie Blöcke MÜSSEN bei Bedarf rekursiv teilbar sein.
5. Freigegebene kompatible Buddies SOLLEN automatisch zusammengeführt werden.
6. Ein Merge DARF reservierte, nicht verfügbare oder inkompatible Speicherbereiche NICHT überschreiten.
7. NUMA-Nodes und unterschiedliche Memory Zones SOLLEN getrennte Buddy-Strukturen verwenden können.
8. Buddy-Operationen MÜSSEN SMP-sicher sein.
9. Double-Free, ungültige Orders und fehlerhafte Blockausrichtungen MÜSSEN verhindert oder erkannt werden.
10. Memory Hotplug DARF die Konsistenz der Buddy-Strukturen NICHT verletzen.
11. Die Implementierung SOLL globale Synchronisationspunkte minimieren.
12. Orders, Fragmentierung, Splits, Merges und Allokationsfehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-MEMORY-0003`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Physical Frame Pool
        ↓
   Buddy Orders
        ↓
┌──── Split ────┐
│               ↓
│          Allocate Block
│               ↓
│             Free
│               ↓
└──── Merge ← Buddy Free
```

NovaOS erhält damit einen skalierbaren Mechanismus zur Verwaltung physisch zusammenhängender Speicherblöcke, der Split und Merge nutzt, NUMA- und Zone-Strukturen unterstützt und die Grundlage für effiziente Frame-, DMA- und Huge-Page-Allokationen bildet.