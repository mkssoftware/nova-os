# NPSPEC-EXECUTION-DEADLINE-0001 – Nova Execution Deadline

## Status

Angenommen

## Kategorie

Execution / Deadline / Temporal Execution

## Zweck

NovaOS definiert Deadlines als expliziten Bestandteil des Execution Contracts.

Eine Execution Deadline legt fest, bis zu welchem Zeitpunkt eine Operation oder ein definierter Teil ihrer Ausführung abgeschlossen sein muss.

```text
ExecutionContract
      ↓
Deadline Requirement
      ↓
Feasibility + Planning
      ↓
Execution
      ↓
Deadline Verification
```

## Grundprinzipien

```text
Deadline ≠ Timeout
Deadline ≠ Latency
Deadline ≠ Priority
Deadline ≠ Resource Budget
Deadline ≠ Guarantee

High Priority ≠ Deadline Guarantee
Accepted Execution ≠ Deadline Guaranteed
Deadline Miss ≠ Automatic Retry
```

## Deadline Requirement

Ein Execution Contract kann enthalten:

```text
ExecutionDeadline
├── Deadline
├── Deadline Class
└── Clock Domain
```

Optional:

```text
Start Time
Relative Deadline
Execution Budget
Latency Requirement
Maximum Jitter
Miss Policy
Fallback Policy
Criticality
```

## Deadline-Klassen

NovaOS unterscheidet:

```text
Hard
Firm
Soft
Preferred
```

### Hard

Das Ergebnis muss innerhalb der Deadline vorliegen.

Eine Überschreitung gilt als Contract Violation.

### Firm

Ein verspätetes Ergebnis besitzt keinen oder stark reduzierten Nutzen.

### Soft

Eine Überschreitung ist zulässig, verschlechtert jedoch die Dienstqualität.

### Preferred

Die Deadline dient als Optimierungsziel.

## Absolute und relative Deadline

NovaOS unterstützt:

```text
Absolute Deadline:
Complete before T

Relative Deadline:
Complete within Δt
```

Relative Deadlines werden beim definierten Startpunkt auf eine eindeutige Zeitbasis bezogen.

## Deadline Propagation

Deadlines müssen entlang abhängiger Ausführungsschritte propagiert werden.

```text
Execution
   ↓
Stage A
   ↓
Stage B
   ↓
Stage C
```

Dabei gilt:

```text
Remaining Time =
Deadline
-
Current Time
```

Eine Child Operation darf nicht erneut das ursprüngliche vollständige Zeitfenster erhalten.

## Deadline Budgeting

Das verbleibende Zeitfenster kann auf Teiloperationen verteilt werden.

```text
End-to-End Deadline
├── Scheduling
├── Compute
├── IPC
├── I/O
└── Network
```

Diese Teilbudgets dienen der Planung und dürfen die End-to-End-Deadline nicht verändern.

## Latency Integration

Deadline und Latenz bleiben getrennte Anforderungen.

```text
Deadline:
Bis wann muss die Operation fertig sein?

Latency:
Wie lange darf die Operation dauern?
```

Beide können gleichzeitig Bestandteil des Execution Contracts sein.

## Resource Budget Integration

Eine Deadline hebt Resource Budgets nicht auf.

```text
Deadline
    +
Resource Budget
    ↓
Execution Planning
```

NovaOS darf eine Deadline nicht durch unbegrenzte Ressourcennutzung erzwingen.

## Admission Control

Hard- und Firm-Deadlines können vor Ausführungsbeginn auf Erfüllbarkeit geprüft werden.

```text
Remaining Time
      +
Expected Work
      +
Available Resources
      +
Reservations
      ↓
Admission Decision
```

Eine bereits offensichtlich nicht erfüllbare Hard Deadline soll nicht als normal ausführbar angenommen werden.

## Reservation

Deadline-kritische Ausführungen können Ressourcenreservierungen anfordern.

```text
Deadline
   ↓
Required Capacity
   ↓
Reservation
```

Eine Reservation allein stellt jedoch noch keine Deadline Guarantee dar.

## Guarantee

Eine verbindliche Deadline Guarantee erfordert eine gesonderte Zusicherung.

```text
Deadline Requirement
        ↓
Admission
        ↓
Reservation
        ↓
Feasibility Verification
        ↓
Guarantee
```

```text
Deadline Requirement ≠ Deadline Guarantee
```

## Scheduling

Der Scheduler kann berücksichtigen:

```text
Deadline
Remaining Time
Execution Budget
Criticality
Reservations
Guarantees
Resource Contention
```

Priorität kann daraus abgeleitet werden, bleibt jedoch ein separates Konzept.

## Runtime Monitoring

Während der Ausführung kann NovaOS den Deadline-Zustand überwachen.

```text
Safe
 ↓
AtRisk
 ↓
Critical
 ↓
Missed
```

Dabei können Vorhersagen über den erwarteten Abschlusszeitpunkt verwendet werden.

## Replanning

Wird eine Deadline gefährdet, kann NovaOS innerhalb des Contracts reagieren.

```text
AtRisk
  ↓
Replan
```

Mögliche Maßnahmen:

```text
Change Provider
Improve Locality
Reserve Additional Capacity
Reduce Queueing
Reduce Optional Work
Use Allowed Degradation
Migrate Execution
```

Hard Constraints dürfen dabei nicht abgeschwächt werden.

## Deadline Miss

Eine Überschreitung muss explizit erkannt werden.

```text
Deadline Reached
      ↓
Operation Incomplete
      ↓
DeadlineMiss
```

Der Execution Contract bestimmt die Reaktion:

```text
Continue
Cancel
Fallback
Degrade
Fail
Safety Action
```

## Cancellation

Ein Deadline Miss kann kontrollierte Cancellation auslösen.

```text
DeadlineMiss
     ↓
Cancellation
     ↓
Structured Concurrency
     ↓
Child Tasks
```

Dadurch kann die gesamte zugehörige Ausführung kontrolliert beendet werden.

## Transactions

Eine Deadline-Verletzung darf keine undefinierten Teilzustände erzeugen.

```text
DeadlineMiss
     ↓
Abort / Rollback / Compensate
```

Transaktionale Operationen müssen ihre definierte Fehlersemantik einhalten.

## Distributed Execution

Bei Remote Execution müssen berücksichtigt werden:

```text
Clock Uncertainty
Serialization
Network Latency
Remote Queue
Remote Execution
Return Transport
```

Eine Deadline muss über Prozess- und Rechnergrenzen hinweg eindeutig interpretierbar bleiben.

## Adaptive Prediction

NovaOS kann historische Ausführungen verwenden:

```text
Predicted Completion
        ↓
Execution
        ↓
Actual Completion
        ↓
Prediction Error
        ↓
Model Adjustment
```

Vorhersagen dürfen jedoch niemals als Garantie behandelt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Deadline
Deadline Class
Clock Domain
Remaining Time
Execution Budget
Current State
Predicted Completion
Reservation
Guarantee
Miss Policy
Deadline Violations
```

## Normative Anforderungen

1. NovaOS MUSS Deadlines als Bestandteil von Execution Contracts unterstützen.
2. Deadline, Timeout, Latency, Priority und Resource Budget MÜSSEN getrennte Konzepte bleiben.
3. Hard-, Firm-, Soft- und Preferred-Deadlines MÜSSEN unterscheidbar sein.
4. Deadlines MÜSSEN entlang abhängiger Ausführungen propagierbar sein.
5. Child Operations DÜRFEN verbrauchte Zeit NICHT erneut als verfügbares Deadline-Budget erhalten.
6. Hard- und Firm-Deadlines SOLLEN Admission Control unterstützen.
7. Deadline Requirements DÜRFEN NICHT automatisch als Guarantees behandelt werden.
8. Deadline-Optimierung DARF Resource Budgets und Hard Constraints NICHT umgehen.
9. Deadline Misses MÜSSEN explizit erkennbar sein.
10. Deadline-basierte Cancellation MUSS mit Structured Concurrency integrierbar sein.
11. Transaktionale Ausführungen MÜSSEN bei Deadline Misses kontrollierte Zustände erhalten.
12. Deadline-Zustand und Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `ADR-ARCH-0052`

## Ergebnis

```text
ExecutionContract
      ↓
Deadline Requirement
      ↓
Admission + Resource Planning
      ↓
Reservation / Guarantee
      ↓
Deadline-Aware Execution
      ↓
Runtime Monitoring
      ↓
Completed / DeadlineMiss
```

NovaOS erhält damit ein durchgängiges Execution-Deadline-Modell, das zeitliche Anforderungen von der ursprünglichen Operation über Ressourcenplanung und Scheduling bis zur tatsächlichen Ausführung propagiert, ohne Deadline, Latenz, Priorität, Budget oder Garantie miteinander zu vermischen.