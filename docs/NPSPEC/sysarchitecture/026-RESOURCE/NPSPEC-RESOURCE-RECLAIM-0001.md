# NPSPEC-RESOURCE-RECLAIM-0001 – Nova Resource Reclaim

## Status

Angenommen

## Kategorie

Resource / Reclaim / Resource Management

## Zweck

NovaOS definiert ein systemweites Verfahren zur kontrollierten Rückgewinnung nicht mehr benötigter, ungenutzter oder unter Ressourcenknappheit entbehrlicher Ressourcen.

```text
Resource Pressure
      ↓
Reclaim Candidates
      ↓
Policy + Constraints
      ↓
Reclaim
      ↓
Recovered Capacity
```

Reclaim soll Ressourcenknappheit möglichst beheben, bevor kritische Maßnahmen wie harte Drosselung oder OOM erforderlich werden.

## Grundprinzipien

```text
Reclaim ≠ Delete
Reclaim ≠ Revocation
Reclaim ≠ Arbitrary Eviction
Unused ≠ Reclaimable
Allocated ≠ Permanently Required
Reserved ≠ Automatically Reclaimable
Hard Guarantee ≠ Reclaimable Capacity
Reclaim ≠ Permission to Lose Data
```

## Reclaimable Resource

Ressourcen können ihren Reclaim-Status deklarieren:

```text
NonReclaimable
Reclaimable
ConditionallyReclaimable
Regeneratable
Disposable
```

Optional werden beschrieben:

```text
Reclaim Cost
Recovery Cost
Priority
Dependencies
Dirty State
Last Usage
Owner
Accounting Domain
```

## Reclaim-Kandidaten

Typische Kandidaten sind:

```text
Caches
Unused Memory
Compressed Data
Temporary Buffers
Idle Connections
Idle Device Contexts
Unused GPU/NPU Memory
Expired Reservations
Regeneratable Objects
Background Resources
```

Persistente oder sicherheitskritische Daten dürfen nicht allein aufgrund von Resource Pressure entfernt werden.

## Reclaim-Ablauf

```text
Pressure Detected
      ↓
Discover Candidates
      ↓
Filter Protected Resources
      ↓
Rank Candidates
      ↓
Reclaim
      ↓
Measure Result
      ↓
Re-Evaluate Pressure
```

## Auswahl

Die Auswahl kann berücksichtigen:

```text
Reclaim Cost
Recovery Cost
Last Usage
Resource Size
Priority
Deadline
Guarantee
Reservation
Criticality
Dependency
Expected Future Use
```

Die Auswahlstrategie ist Policy und bleibt vom Reclaim-Mechanismus getrennt.

## Reclaim-Stufen

NovaOS kann schrittweise vorgehen:

```text
Normal Cleanup
      ↓
Cache Reclaim
      ↓
Idle Resource Reclaim
      ↓
Compression / Swap
      ↓
Soft Reservation Reclaim
      ↓
Graceful Degradation
      ↓
Critical Recovery
```

Teure oder disruptive Maßnahmen sollen erst eingesetzt werden, wenn günstigere Maßnahmen nicht ausreichen.

## Reservations

Hard Reservations dürfen durch normalen Reclaim nicht verletzt werden.

Soft Reservations können abhängig von ihrer Policy reduziert oder zurückgewonnen werden.

```text
Hard Reservation → Protected
Soft Reservation → Policy Dependent
Expired Reservation → Reclaimable
```

## Guarantees

Ressourcen, die eine aktive Hard Guarantee absichern, sind grundsätzlich geschützt.

```text
Reclaim Candidate
      ↓
Guarantee Check
      ↓
Protected / Reclaimable
```

Eine Guarantee Violation darf nicht stillschweigend durch Reclaim verursacht werden.

## Memory Reclaim

Das Resource-Reclaim-Modell orchestriert bestehende Memory-Mechanismen:

```text
Cache Eviction
Compression
Swap
Deduplication
Page Reclaim
```

Die konkrete Speichermechanik bleibt Aufgabe des Memory-Subsystems.

## CPU, I/O und Netzwerk

Reclaim kann auch nicht-speicherbasierte Ressourcen betreffen.

Beispiele:

```text
Release Idle CPU Reservation
Close Idle Connection
Reduce Queue Capacity
Release I/O Reservation
Release Device Context
```

## GPU und NPU

Accelerator-Ressourcen können zurückgewonnen werden durch:

```text
Unused Buffer Release
Model Eviction
Context Release
Queue Reduction
Cache Eviction
```

Ein später benötigtes Modell kann bei Bedarf erneut geladen werden.

## Reclaim und Degradation

Falls normale Rückgewinnung nicht ausreicht:

```text
Reclaim
   ↓
Still Insufficient
   ↓
Graceful Degradation
```

Beispiele:

```text
Reduce Cache
Reduce Resolution
Reduce Parallelism
Suspend Background Work
Use Alternative Provider
```

Hard Requirements dürfen dabei nicht verletzt werden.

## Structured Concurrency

Task-Lebenszyklen sollen automatische Ressourcenfreigabe ermöglichen.

```text
Task Ends
   ↓
Owned Resources
   ↓
Release / Reclaim
```

Dadurch werden verwaiste Ressourcen reduziert.

## Accounting

Resource Accounting unterscheidet:

```text
Allocated
Reclaimable
Reclaimed
Released
Recovered Capacity
Reclaim Cost
```

Dadurch kann NovaOS die Wirksamkeit von Reclaim-Strategien bewerten.

## Sicherheit

Reclaim darf keine geschützten Informationen offenlegen.

Wiederverwendete Ressourcen müssen gegebenenfalls bereinigt werden:

```text
Memory
Buffers
Device Memory
Temporary Storage
Shared Resources
```

```text
Reclaimed ≠ Safe to Reassign
```

Erst nach erforderlicher Bereinigung darf eine Ressource einer anderen Security Domain zugewiesen werden.

## Adaptive Reclaim

NovaOS darf vergangene Nutzung berücksichtigen.

```text
Candidate
   ↓
Reclaim
   ↓
Resource Needed Again?
   ↓
Prediction Error
   ↓
Policy Adjustment
```

Adaptive Entscheidungen dürfen Reservations, Guarantees oder Hard Constraints nicht überschreiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Reclaimable Capacity
Candidates
Protected Resources
Reclaim State
Recovered Capacity
Reclaim Cost
Pressure State
Accounting Domain
Reclaim History
```

## Normative Anforderungen

1. NovaOS MUSS systemweiten Resource Reclaim unterstützen.
2. Ressourcen MÜSSEN ihren Reclaim-Status explizit darstellen können.
3. Hard Reservations und Hard Guarantees DÜRFEN durch normalen Reclaim NICHT verletzt werden.
4. Reclaim DARF keine implizite Datenlöschung bedeuten.
5. NovaOS SOLL kostengünstige Reclaim-Maßnahmen vor disruptiven Maßnahmen bevorzugen.
6. Reclaim MUSS mehrere Ressourcenklassen unterstützen können.
7. Wiederverwendete Ressourcen MÜSSEN vor Domain-Wechsel sicher bereinigt werden.
8. Reclaim MUSS mit Resource Accounting und Pressure Management integrierbar sein.
9. Adaptive Reclaim-Strategien DÜRFEN Hard Constraints NICHT überschreiben.
10. Reclaim Decisions MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0047`

## Ergebnis

```text
Resource Pressure
      ↓
Reclaim Discovery
      ↓
Protection + Cost Evaluation
      ↓
Controlled Reclaim
      ↓
Recovered Capacity
      ↓
Pressure Re-Evaluation
```

NovaOS erhält damit einen gemeinsamen Resource-Reclaim-Mechanismus, der entbehrliche Ressourcen kontrolliert zurückgewinnt, bestehende Garantien schützt und Ressourcenknappheit möglichst früh behebt, bevor disruptive Maßnahmen notwendig werden.