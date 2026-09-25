# NPSPEC-SCHEDULER-FAIR-0001 – Nova Fair Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Fairness / CPU Scheduling / Resource Economy

## Zweck

NovaOS definiert Fair Scheduling für normale Tasks, damit verfügbare CPU-Zeit kontrolliert zwischen konkurrierenden Ausführungseinheiten verteilt wird.

```text
Runnable Tasks
      ↓
Fair Scheduler
      ↓
Weighted CPU Distribution
      ↓
Execution
```

## Grundprinzip

Fairness bedeutet nicht, dass alle Tasks exakt gleich viel CPU-Zeit erhalten.

```text
Fairness ≠ Equal Runtime
```

Stattdessen berücksichtigt NovaOS definierte Gewichtungen, Prioritäten, Ressourcenbudgets und Ausführungsanforderungen.

## Fairness-Modell

Für normale Tasks kann eine gewichtete Verteilung verwendet werden.

```text
Task A → Weight 1
Task B → Weight 2
Task C → Weight 1
```

Langfristig soll Task B unter vergleichbaren Bedingungen ungefähr doppelt so viel CPU-Anteil erhalten wie A oder C.

Die konkrete Scheduling-Formel bleibt Implementierungsdetail.

## Scheduling-Gruppen

Fairness kann hierarchisch gelten.

```text
System
├── Session A
│   ├── Task A1
│   └── Task A2
└── Session B
    ├── Task B1
    └── Task B2
```

Dadurch kann verhindert werden, dass eine Anwendung mit vielen Tasks automatisch mehr CPU-Zeit erhält als eine Anwendung mit wenigen Tasks.

## Gewichtung

Gewichtungen können abhängig sein von:

```text
Task Priority
Process Policy
Session
Resource Budget
User Activity
Service Class
```

Gewichtungen müssen innerhalb definierter Grenzen bleiben.

## Interaktive Tasks

Interaktive Tasks dürfen kurzfristig bevorzugt werden, um geringe wahrgenommene Latenz zu erreichen.

```text
User Input
    ↓
Interactive Task
    ↓
Temporary Scheduling Preference
```

Diese Bevorzugung darf andere normale Tasks nicht dauerhaft verdrängen.

## Starvation Prevention

Ausführbare normale Tasks sollen nicht unbegrenzt von CPU-Zeit ausgeschlossen werden.

```text
Ready
  ↓
Waiting
  ↓
Eventually Scheduled
```

Mechanismen gegen Starvation müssen vorhanden sein.

## Multiprocessing

Fair Scheduling muss über mehrere CPUs hinweg funktionieren.

```text
Runnable Tasks
      ↓
Per-CPU Queues
      ↓
Load Balancing
      ↓
CPU 0 / CPU 1 / CPU N
```

Lokale Fairness darf nicht zu dauerhaft grober globaler Unfairness führen.

## Lokalität

Fairness wird mit Hardwarelokalität abgewogen.

```text
Fairness
+
Cache Locality
+
NUMA Locality
```

Eine Task soll nicht allein zur kurzfristigen Fairnessverbesserung ständig zwischen CPUs migriert werden.

## Work Stealing

Work Stealing darf zur Lastverteilung verwendet werden.

```text
Idle CPU
   ↓
Steal Eligible Task
```

Dabei müssen Fairness, Affinität und bisherige CPU-Nutzung berücksichtigt werden.

Eine Migration darf die bisherige Fairnesshistorie nicht unbegründet zurücksetzen.

## Ressourcenbudgets

CPU-Budgets aus der Resource Economy begrenzen Fair Scheduling.

```text
Fair Share
    ↓
Resource Budget
    ↓
Effective CPU Allocation
```

Fairness darf keine expliziten Ressourcenlimits umgehen.

## Realtime

Realtime-Tasks können außerhalb des normalen Fair-Scheduling-Modells behandelt werden.

```text
Realtime Constraints
        ↓
Realtime Scheduling

Normal Tasks
        ↓
Fair Scheduling
```

Realtime-Ausführung darf jedoch nicht unkontrolliert sämtliche normale Systemarbeit verdrängen.

## Deadlines

Tasks mit expliziten Deadlines können eine andere Scheduling-Policy benötigen.

Fairness darf harte Deadline-Anforderungen nicht überschreiben.

```text
Hard Constraint
      ↓
Fairness Optimization
```

## Adaptive Optimierung

NovaOS darf Fairnessparameter anhand der Systemlast anpassen.

Dabei können berücksichtigt werden:

```text
CPU Utilization
Task Behaviour
Interactive Activity
Waiting Time
Energy State
System Load
```

Adaptive Optimierung darf Hard Constraints, Ressourcenbudgets oder explizite Policies nicht überschreiben.

## Normative Anforderungen

1. NovaOS MUSS Fair Scheduling für normale konkurrierende Tasks unterstützen.
2. Fairness DARF gewichtete CPU-Anteile berücksichtigen.
3. Fairness MUSS von absolut gleicher CPU-Zeit unterschieden werden.
4. Hierarchische Scheduling-Gruppen SOLLEN unterstützt werden.
5. Viele Child Tasks DÜRFEN nicht automatisch einen unverhältnismäßigen CPU-Anteil erzeugen.
6. Ausführbare normale Tasks SOLLEN gegen unbegrenzte Starvation geschützt werden.
7. Interaktive Tasks DÜRFEN kontrolliert kurzfristig bevorzugt werden.
8. CPU-Migration DARF Fairnesshistorie nicht unbegründet zurücksetzen.
9. CPU- und NUMA-Lokalität SOLLEN bei Fairnessentscheidungen berücksichtigt werden.
10. Ressourcenbudgets MÜSSEN Fairnessentscheidungen begrenzen.
11. Hard-Realtime- und Deadline-Anforderungen MÜSSEN Vorrang vor Fairnessoptimierung haben.
12. Fairnesszustand und Scheduling-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `ADR-SCHED-0003`

## Ergebnis

```text
Runnable Normal Tasks
        ↓
Weights + Groups + Budgets
        ↓
Fair Scheduling
        ↓
Locality-Aware Distribution
        ↓
Balanced CPU Access
```

NovaOS erhält damit ein Fair-Scheduling-Modell, das CPU-Zeit nachvollziehbar verteilt, Starvation verhindert und gleichzeitig Ressourcenbudgets, Hardwarelokalität und höher priorisierte harte Anforderungen respektiert.