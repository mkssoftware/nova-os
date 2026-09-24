# NPSPEC-DATAMOVE-LOCALITY-0001 – Nova Data Locality

## Status

Entwurf

## Kategorie

DataMove / Locality / NUMA / Cache / Scheduling

## Zweck

NovaOS definiert Data Locality als systemweiten Mechanismus zur bevorzugten Verarbeitung von Daten dort, wo sie bereits effizient verfügbar sind.

```text
Data Location
     +
Execution Location
     +
Hardware Topology
     ↓
Locality Decision
```

Ziel ist die Reduktion unnötiger Datenbewegungen, Speicherzugriffe, Cache-Misses, NUMA-Transfers, Energieverbrauch und I/O-Latenzen.

## Grundprinzip

NovaOS bevorzugt grundsätzlich:

```text
Move Compute to Data
```

gegenüber:

```text
Move Data to Compute
```

wenn dies technisch und semantisch sinnvoll ist.

Dabei gilt:

```text
Identity ≠ Location
Location ≠ Ownership
Locality ≠ Mandatory Placement
Zero-Copy ≠ Good Locality
```

Data Locality ist eine Optimierungsentscheidung und darf harte Systemanforderungen nicht verletzen.

## Locality-Ebenen

NovaOS soll unterschiedliche Lokalitätsebenen berücksichtigen können.

```text
CPU Core
   ↓
CPU Cache
   ↓
CPU Package
   ↓
NUMA Node
   ↓
Local Device
   ↓
Local System
   ↓
Remote System
```

Die konkrete Topologie wird über HAL und Systemmodell bereitgestellt.

## Locality-Metadaten

Datenobjekte und Buffer dürfen Locality-Metadaten besitzen.

Beispiele:

```text
CurrentNode
PreferredNode
LastAccessNode
DeviceAffinity
ProducerLocation
ConsumerLocation
AccessFrequency
MigrationCost
```

Diese Informationen sind Hinweise für die Platzierungsentscheidung und keine Objektidentität.

## Datenplatzierung

Neue Buffer sollen nach Möglichkeit dort angelegt werden, wo ihre hauptsächliche Verarbeitung erwartet wird.

```text
Producer + Consumer
        ↓
Expected Access Pattern
        ↓
Placement Decision
        ↓
Suitable Memory Node
```

Dabei dürfen unter anderem berücksichtigt werden:

```text
CPU Locality
Device Locality
NUMA Distance
Memory Pressure
Expected Lifetime
Access Pattern
Resource Budget
```

## Execution Placement

Statt Daten zu verschieben darf NovaOS die Verarbeitung zu den Daten verlagern.

```text
Data on Node 2
      ↓
Schedule Task on Node 2
```

Dies soll insbesondere bei großen Datenmengen berücksichtigt werden.

Der Scheduler darf Locality-Informationen als Placement-Hinweis verwenden.

## Data Migration

Wenn eine dauerhafte Änderung des Zugriffsmusters erkannt wird, dürfen Daten migriert werden.

```text
Data on Node 0
      ↓
Repeated access from Node 1
      ↓
Migration beneficial?
      ↓
Move to Node 1
```

Migration muss gegen ihre eigenen Kosten abgewogen werden.

## Migration Cost

Eine Migration ist nur sinnvoll, wenn der erwartete Nutzen ihre Kosten rechtfertigt.

```text
Expected Remote Access Cost
            >
Migration Cost
```

Zu berücksichtigen sind mindestens:

```text
Data Size
Access Frequency
NUMA Distance
Copy Cost
Cache Effects
Expected Lifetime
Memory Pressure
```

## Zero-Copy und Locality

Zero-Copy verhindert Kopien, garantiert jedoch keine optimale Platzierung.

```text
Consumer Node 1
      ↓
Zero-Copy
      ↓
Buffer Node 0
```

In diesem Fall kann:

```text
Remote Zero-Copy
```

teurer sein als:

```text
Local Copy
```

NovaOS darf deshalb bewusst eine Kopie durchführen, wenn dies insgesamt wirtschaftlicher ist.

## Shared Buffer

Bei Shared Buffers können mehrere Teilnehmer unterschiedliche optimale Standorte besitzen.

```text
Producer Node 0
Consumer Node 1
Device Node 2
       ↓
Placement Decision
```

Die Entscheidung soll das erwartete Gesamtzugriffsmuster berücksichtigen.

Ein universell optimaler Standort darf nicht vorausgesetzt werden.

## Scatter/Gather

Scatter/Gather-Segmente können über mehrere NUMA-Nodes verteilt sein.

```text
Segment A → Node 0
Segment B → Node 1
Segment C → Node 0
```

NovaOS darf Segmente:

```text
Preserve
Migrate
Copy
Coalesce
```

wenn dadurch der Gesamttransfer effizienter wird.

## DMA und Device Locality

DMA-Buffer sollen nach Möglichkeit in Speicherbereichen liegen, die für das verwendete Gerät effizient erreichbar sind.

```text
Device
  ↓
Topology
  ↓
Nearest Suitable Memory
```

IOMMU- und DMA-Anforderungen haben dabei Vorrang vor reinen Locality-Optimierungen.

## Cache Locality

NovaOS soll Cache-Lokalität berücksichtigen können.

Ein Task, der kürzlich Daten auf einer CPU verarbeitet hat, kann von erneuter Ausführung auf derselben oder einer cache-nahen CPU profitieren.

```text
Task
 ↓
CPU Cache contains working set
 ↓
Prefer same CPU
```

Scheduler- und DataMove-Entscheidungen sollen sich hierbei koordinieren können.

## Streaming

Bei kontinuierlichen Datenströmen soll eine stabile Verarbeitungspipeline bevorzugt werden.

```text
Device
  ↓
Buffer
  ↓
Worker
  ↓
Consumer
```

Häufige Migrationen zwischen NUMA-Nodes sollen vermieden werden.

## Adaptive Locality

NovaOS darf tatsächliche Zugriffsmuster beobachten.

```text
Expected Locality
       ↓
Actual Access Pattern
       ↓
Prediction Error
       ↓
Policy Adjustment
```

Wiederholte Remote-Zugriffe können zur Anpassung von:

```text
Task Placement
Buffer Placement
Migration Policy
Prefetch Policy
```

führen.

Adaptive Entscheidungen bleiben Soft Optimizations.

## Hard Constraints

Locality darf niemals höhere Anforderungen überschreiben.

Priorität:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Realtime / Hard Constraints
  ↓
Explicit User Decision
  ↓
Locality Optimization
```

Ein lokal günstiger Speicherbereich darf nicht verwendet werden, wenn Security- oder Hardwareanforderungen dies verbieten.

## Realtime

Realtime-Komponenten dürfen stabile Datenplatzierung verlangen.

```text
Realtime Buffer
      ↓
Fixed / Controlled Placement
```

Unvorhersehbare automatische Migrationen müssen für solche Daten deaktivierbar sein.

## Memory Pressure

Locality darf nicht dazu führen, dass ein ausgelasteter NUMA-Node künstlich überfüllt wird.

```text
Preferred Node
      ↓
Insufficient Memory
      ↓
Policy Decision
```

Abhängig vom Execution Contract darf NovaOS:

```text
Use Remote Memory
Migrate Compute
Reclaim Local Memory
Fail Allocation
```

## Resource Economy

Locality-Entscheidungen sollen nicht nur Latenz betrachten.

NovaOS darf ein Kostenmodell verwenden:

```text
Cost =
Transfer
+ Latency
+ CPU
+ Cache
+ Energy
+ Memory Pressure
+ Migration
```

Hard Constraints werden vor diesem Kostenmodell ausgewertet.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Buffer Location
NUMA Placement
Producer Location
Consumer Location
Device Locality
Remote Accesses
Migration Count
Migration Cost
Locality Decisions
Locality Fallbacks
```

Entscheidungen sollen über die systemweite Introspection nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS Daten- und Ausführungsort logisch voneinander trennen.
2. Datenidentität DARF NICHT von ihrer physischen oder virtuellen Position abhängen.
3. DataMove-Komponenten SOLLEN Hardware- und NUMA-Topologie berücksichtigen.
4. NovaOS SOLL lokale Speicherzugriffe gegenüber unnötigen Remote-Zugriffen bevorzugen.
5. NovaOS SOLL Compute zu Daten verlagern können, wenn dies günstiger als Datenmigration ist.
6. Datenmigration DARF nur unter Erhaltung von Ownership, Permissions und Datenintegrität erfolgen.
7. Zero-Copy DARF NICHT automatisch als optimale Locality-Strategie betrachtet werden.
8. Eine gezielte Kopie DARF verwendet werden, wenn sie wirtschaftlicher als dauerhafter Remote-Zugriff ist.
9. DMA- und Hardwareanforderungen MÜSSEN gegenüber reinen Locality-Optimierungen eingehalten werden.
10. Hard-Realtime-Komponenten MÜSSEN unvorhersehbare automatische Datenmigration verhindern können.
11. Locality-Optimierungen DÜRFEN Safety-, Security-, Trust- oder andere Hard Constraints NICHT überschreiben.
12. Placement-, Migration- und Locality-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DATAMOVE-RINGBUFFER-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-SCHEDULER-CACHE-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-DATAMOVE-0006`

## Ergebnis

```text
Data + Compute + Device
          ↓
      Topology
          ↓
   Locality Analysis
          ↓
┌─────────┼─────────┐
↓         ↓         ↓
Keep    Migrate   Move Compute
Data     Data      to Data
└─────────┼─────────┘
          ↓
Efficient Data Path
```

NovaOS erhält damit eine systemweite Data-Locality-Strategie, die Speicherplatzierung, Scheduling und Datenbewegung gemeinsam optimiert und unnötige Transfers vermeidet, ohne Sicherheit, Realtime-Anforderungen oder explizite Systemregeln zu verletzen.