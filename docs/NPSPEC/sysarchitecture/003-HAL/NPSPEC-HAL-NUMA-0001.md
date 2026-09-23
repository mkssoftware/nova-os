# NPSPEC-HAL-NUMA-0001 – Nova NUMA Abstraction

## Status

Entwurf

## Kategorie

HAL / NUMA / Memory / CPU Topology

## Zweck

NovaOS stellt Non-Uniform Memory Access (NUMA) über ein einheitliches Modell bereit, damit Scheduler, Speicherverwaltung und Resource Economy die physische Nähe zwischen CPUs, Speicher und Geräten berücksichtigen können.

```text
CPU / Device
     ↓
NUMA Topology
     ↓
Local / Remote Memory
```

## Grundprinzip

Speicherzugriffskosten können abhängig vom physischen Standort unterschiedlich sein.

```text
CPU Node 0 → Memory Node 0 = Local
CPU Node 0 → Memory Node 1 = Remote
```

Die HAL beschreibt diese Beziehungen, legt jedoch keine Scheduling- oder Speicherpolitik fest.

## NUMA-Modell

Ein NUMA-Knoten beschreibt mindestens:

```text
NodeID
CPUs
Memory Regions
Distances
State
```

Optional können weitere Beziehungen wie Geräte oder Accelerator-Nähe abgebildet werden.

## Distanzmodell

NovaOS muss relative Zugriffskosten zwischen NUMA-Knoten darstellen können.

```text
Node 0 ──10── Node 0
Node 0 ──20── Node 1
Node 0 ──30── Node 2
```

Die Werte beschreiben relative Kosten und müssen nicht direkt einer Zeitangabe entsprechen.

## Speicherzuordnung

Die Speicherverwaltung kann NUMA-Informationen für lokale Speicherzuweisung verwenden.

```text
Task
 ↓
Execution CPU
 ↓
Preferred NUMA Node
 ↓
Memory Allocation
```

Falls lokaler Speicher nicht verfügbar ist, kann Speicher eines anderen Knotens verwendet werden.

## Scheduling

Der Scheduler kann NUMA-Topologie berücksichtigen, um unnötige Remote-Zugriffe zu reduzieren.

```text
Task + Memory Location
        ↓
Scheduler
        ↓
Suitable CPU / Node
```

Die Entscheidung verbleibt beim Scheduler und nicht bei der HAL.

## Geräte

Geräte können einem NUMA-Knoten oder einer Nähebeziehung zugeordnet sein.

```text
Device
  ↓
NUMA Node
  ↓
CPU / Memory
```

Dies ermöglicht lokalitätsbewusste I/O- und DMA-Entscheidungen.

## Systeme ohne NUMA

Auf UMA-Systemen wird ein vereinfachtes Modell verwendet:

```text
NUMA Node 0
 ├── All CPUs
 └── All Memory
```

Höhere Systemschichten müssen dadurch keine getrennten NUMA- und UMA-Codepfade benötigen.

## Dynamische Änderungen

Änderungen durch:

```text
CPU Hotplug
Memory Hotplug
Virtualization
Platform Reconfiguration
```

müssen kontrolliert in das NUMA-Modell übernommen werden.

## Normative Anforderungen

1. NovaOS MUSS NUMA-Topologien strukturiert darstellen können.
2. NUMA-Knoten MÜSSEN eindeutig identifizierbar sein.
3. CPUs und Speicherregionen MÜSSEN NUMA-Knoten zugeordnet werden können.
4. Relative Distanzen zwischen NUMA-Knoten MÜSSEN darstellbar sein.
5. Scheduler und Speicherverwaltung SOLLEN NUMA-Informationen verwenden können.
6. Geräte SOLLEN NUMA-Lokalität deklarieren können.
7. Die HAL DARF keine NUMA-Scheduling- oder Speicherpolitik festlegen.
8. Systeme ohne NUMA MÜSSEN über ein einheitliches Ein-Knoten-Modell darstellbar sein.
9. Dynamische Topologieänderungen MÜSSEN kontrolliert aktualisierbar sein.
10. Fehlende NUMA-Informationen DÜRFEN NICHT als bekannte optimale Lokalität behandelt werden.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-HAL-0005`

## Ergebnis

```text
Hardware NUMA Topology
         ↓
     HAL NUMA Model
         ↓
Scheduler / Memory / I/O
         ↓
Locality-aware Decisions
```

NovaOS kann damit die physische Nähe von CPU, Speicher und Geräten berücksichtigen, ohne NUMA-spezifische Politik in der HAL zu verankern.