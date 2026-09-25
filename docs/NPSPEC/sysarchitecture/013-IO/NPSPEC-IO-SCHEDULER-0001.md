# NPSPEC-IO-SCHEDULER-0001 – Nova I/O Scheduler

## Status

Angenommen

## Kategorie

I/O / Scheduling / Resource Management

## Zweck

NovaOS definiert einen I/O-Scheduler zur kontrollierten Planung konkurrierender I/O-Requests.

```text
IORequests
    ↓
I/O Scheduler
    ↓
Provider / Device
    ↓
Completion
```

Der Scheduler koordiniert Priorität, Fairness, Deadlines und Ressourcenverbrauch, ohne Consumer an konkrete Geräte oder Provider zu koppeln.

## Grundprinzip

```text
Submission Order ≠ Execution Order
Priority ≠ Unlimited Preference
I/O Scheduling ≠ CPU Scheduling
```

Die Ausführungsreihenfolge darf anhand der Eigenschaften von Request, Provider und Systemzustand optimiert werden.

## Scheduling Context

Für Entscheidungen dürfen insbesondere verwendet werden:

```text
Priority
Deadline
Request Type
Target
Owner
ExecutionContract
Queue State
Provider State
Resource Pressure
```

Security- und Data-Sovereignty-Regeln haben Vorrang vor Optimierungen.

## Prioritätsklassen

NovaOS soll mindestens logische I/O-Klassen unterscheiden können:

```text
Realtime
Interactive
Normal
Background
Maintenance
```

Die konkrete Abbildung auf Hardware-Queues oder Provider bleibt implementationsabhängig.

## Fairness

Ein einzelner Consumer darf gemeinsame I/O-Ressourcen nicht unbegrenzt dominieren.

```text
Process A ─┐
Process B ─┼→ Fair Scheduling → Provider
Process C ─┘
```

Fairness darf zugunsten expliziter Realtime- oder Deadline-Anforderungen kontrolliert eingeschränkt werden.

## Deadlines

Requests dürfen eine Deadline besitzen.

```text
Request
├── Priority
└── Deadline
      ↓
I/O Scheduler
```

Der Scheduler soll Requests mit zeitlichen Anforderungen bevorzugen können, sofern dadurch keine höheren Systemanforderungen verletzt werden.

Eine nicht einhaltbare Deadline muss erkennbar sein.

## Provider Awareness

Der Scheduler darf Eigenschaften des Providers berücksichtigen.

Beispiele:

```text
Queue Depth
Parallelism
Latency
Throughput
Device Topology
Sequential Access Cost
```

Gerätespezifische Optimierungen sollen möglichst im zuständigen Provider verbleiben.

## Request Merging

Kompatible Requests dürfen zusammengeführt werden, wenn ihre Semantik erhalten bleibt.

```text
Read A
Read B
Read C
  ↓
Merged Request
```

Requests mit inkompatiblen Security-, Ordering-, Deadline- oder Transaction-Anforderungen dürfen nicht zusammengeführt werden.

## Reordering

Requests dürfen umgeordnet werden, wenn keine explizite Reihenfolge verlangt wird.

```text
Submit:  A B C
Execute: B C A
```

Ordering-Anforderungen müssen durch Request, Provider oder Execution Contract explizit angegeben werden können.

## Backpressure

Der Scheduler muss begrenzte Queue-Kapazitäten unterstützen.

```text
Requests
   ↓
Queue Limit
   ↓
Backpressure
```

Bei Ressourcenknappheit dürfen Submission verlangsamt, Requests zurückgewiesen oder niedrig priorisierte Arbeit verzögert werden.

## Cancellation

Noch nicht ausgeführte Requests sollen effizient aus Scheduler-Queues entfernt werden können.

Bereits an einen Provider übergebene Requests müssen dessen Cancellation-Modell verwenden.

## Adaptive Optimierung

NovaOS darf Scheduling-Strategien anhand beobachteter Eigenschaften anpassen.

Beispiele:

```text
Latency
Queue Depth
Throughput
Access Pattern
Energy State
```

Adaptive Optimierung darf harte Anforderungen des `ExecutionContract` nicht verändern.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Queue
RequestID
Priority
Deadline
Wait Time
Provider
Queue Depth
Scheduling Class
```

## Normative Anforderungen

1. NovaOS MUSS konkurrierende I/O-Requests kontrolliert planen können.
2. Submission-Reihenfolge DARF NICHT automatisch Ausführungsreihenfolge bedeuten.
3. Der Scheduler MUSS Prioritäten und Deadlines berücksichtigen können.
4. Security- und Sovereignty-Regeln DÜRFEN durch Scheduling NICHT umgangen werden.
5. Gemeinsame I/O-Ressourcen SOLLEN Fairness zwischen Consumern unterstützen.
6. Realtime- und Deadline-Anforderungen MÜSSEN gegenüber normalen Optimierungen priorisierbar sein.
7. Provider-Eigenschaften SOLLEN bei Scheduling-Entscheidungen berücksichtigt werden können.
8. Requests DÜRFEN nur bei erhaltener Semantik zusammengeführt oder umgeordnet werden.
9. Explizite Ordering-Anforderungen MÜSSEN erhalten bleiben.
10. I/O-Queues MÜSSEN begrenzbar sein und Backpressure unterstützen.
11. Adaptive Optimierung DARF harte Execution-Contract-Anforderungen NICHT verletzen.
12. Scheduling-Zustand und Queue-Auslastung SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IO-0004`

## Ergebnis

```text
IORequests
    ↓
Priority / Deadline / Fairness
    ↓
I/O Scheduler
    ↓
Provider Queues
    ↓
Execution
```

NovaOS erhält damit eine zentrale I/O-Scheduling-Abstraktion, die konkurrierende Requests effizient plant und dabei Fairness, Realtime-Anforderungen, Ressourcenlimits und Provider-Eigenschaften berücksichtigt.