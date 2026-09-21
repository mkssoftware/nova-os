# ADR-REALTIME-0008 – Time Partitioning

## Status

Angenommen

## Kategorie

Realtime / Scheduling / Temporal Isolation / Resource Management

## Kontext

Temporal Isolation schützt Realtime-Workloads vor unkontrollierter Konkurrenz um Ausführungszeit.

Für besonders kritische Systeme reicht eine reine Priorisierung oder dynamische Ressourcenreservierung jedoch nicht immer aus.

Mehrere Workload-Klassen können auf demselben System ausgeführt werden:

```text
Hard Realtime
Safety Critical
System Services
Interactive Workloads
AI
Background Tasks
```

Ohne feste zeitliche Grenzen kann ein fehlerhafter, überlasteter oder kompromittierter Workload andere Workloads zeitlich beeinflussen.

NovaOS benötigt deshalb einen Mechanismus, mit dem CPU-Zeit und andere zeitkritische Ressourcen in explizite, voneinander isolierte Zeitbereiche aufgeteilt werden können.

## Entscheidung

NovaOS unterstützt **Time Partitioning** als systemweiten Mechanismus zur zeitlichen Isolation von Execution Domains.

```text
Physical Resource
       ↓
Time Partition Scheduler
       ↓
┌────────────┬────────────┬────────────┐
│ Partition A│ Partition B│ Partition C│
│ Realtime   │ System     │ Best Effort│
└────────────┴────────────┴────────────┘
```

Eine Time Partition erhält einen definierten Anteil oder ein definiertes Zeitfenster einer Ressource.

Workloads innerhalb einer Partition dürfen die reservierte Zeit anderer Partitionen nicht unkontrolliert verbrauchen.

## Time Partition

Eine Time Partition beschreibt mindestens:

```text
Partition ID
Resource
Period
Time Budget
Scheduling Policy
Criticality
Overrun Policy
```

Beispiel:

```text
Partition: Control
Period:    10 ms
Budget:     2 ms
Criticality: Hard Realtime
```

Damit stehen der Partition innerhalb jeder 10-ms-Periode bis zu 2 ms garantierte CPU-Zeit zur Verfügung.

## Partition Schedule

Time Partitions können nach einem wiederkehrenden Zeitplan ausgeführt werden.

```text
0 ms                         10 ms
│                              │
├──── A ────┬── B ──┬──── C ──┤
│           │       │          │
0           4       6         10
```

Beispielsweise:

```text
A = Hard Realtime
B = System Services
C = Best Effort
```

Der konkrete Scheduling-Algorithmus wird nicht durch diese ADR festgelegt.

## Static und Dynamic Partitioning

NovaOS unterstützt konzeptionell:

```text
Static Time Partitioning
```

und:

```text
Dynamic Time Partitioning
```

### Static

Zeitfenster sind vorab festgelegt.

```text
A → 0–2 ms
B → 2–5 ms
C → 5–10 ms
```

Dies bietet hohe Vorhersagbarkeit.

### Dynamic

Budgets sind fest definiert, ihre genaue Platzierung kann innerhalb zulässiger Grenzen variieren.

```text
Partition A:
2 ms innerhalb jeder 10-ms-Periode
```

Dies ermöglicht höhere Flexibilität.

Hard-Realtime-Contracts bestimmen, welche Variante zulässig ist.

## Temporal Firewall

Time Partitioning wirkt als **Temporal Firewall** zwischen Execution Domains.

```text
Partition A Overrun
        │
        X
        │
Partition B Budget
```

Ein Workload darf durch übermäßige CPU-Nutzung nicht automatisch die reservierte Zeit einer anderen Partition konsumieren.

## Budget Enforcement

Jede Partition besitzt ein Zeitbudget.

```text
Budget Available
      ↓
Execution
      ↓
Budget Exhausted
      ↓
Suspend / Throttle / Failure Policy
```

Nach Verbrauch des Budgets darf die Partition nicht unkontrolliert weiterlaufen.

## Overrun

Überschreitet eine Partition ihr Budget:

```text
Consumed Time > Partition Budget
```

entsteht ein:

```text
Time Partition Overrun
```

Die Reaktion wird durch die Partition Policy bestimmt.

Beispiele:

```text
Suspend
Throttle
Defer
Abort
Safe State
Escalate
```

Ein Overrun darf nicht stillschweigend zulasten anderer garantierter Partitionen gehen.

## Realtime Domains

Eine Realtime Domain kann einer Time Partition zugeordnet werden.

```text
Time Partition
      ↓
Realtime Domain
 ├── Task A
 ├── Task B
 └── Service C
```

Die Tasks innerhalb der Partition können anschließend durch einen lokalen Scheduler verwaltet werden.

Damit entsteht eine hierarchische Scheduling-Struktur:

```text
Global Partition Scheduler
          ↓
Time Partition
          ↓
Local Scheduler
          ↓
Tasks
```

## Hierarchical Scheduling

NovaOS behandelt Partition Scheduling und Task Scheduling als getrennte Ebenen.

```text
Level 1:
Which partition may execute?

Level 2:
Which task inside the partition executes?
```

Eine hohe Task-Priorität innerhalb einer Partition erzeugt keine Authority über die Zeit einer anderen Partition.

## Hard Realtime

Hard-Realtime-Workloads können dedizierte Time Partitions erhalten.

```text
Hard Realtime Contract
        ↓
Admission Control
        ↓
Reserved Time Partition
        ↓
Realtime Domain
```

Die Partition wird nur akzeptiert, wenn das System das benötigte Budget garantieren kann.

## Mixed Criticality

Time Partitioning ermöglicht die gemeinsame Nutzung eines Systems durch Workloads unterschiedlicher Kritikalität.

```text
Critical Control
       │
       │ isolated
       │
User Interface
       │
       │ isolated
       │
AI / Background
```

Ein Fehler oder eine Überlastung eines weniger kritischen Workloads darf die zugesicherte CPU-Zeit eines höher kritischen Workloads nicht unkontrolliert reduzieren.

## Partition Criticality

Partitionen können eine Criticality-Klasse besitzen.

Beispiele:

```text
Safety Critical
Hard Realtime
System Critical
Interactive
Best Effort
Background
```

Criticality ist jedoch nicht automatisch identisch mit Scheduler-Priorität.

Sie beeinflusst insbesondere:

```text
Admission
Isolation
Failure Policy
Resource Reservation
Recovery
```

## Idle Time

Eine Partition kann ihre reservierte Zeit nicht vollständig verwenden.

```text
Reserved Budget
      ↓
Unused
```

NovaOS darf diese Zeit kontrolliert anderen Partitionen zur Verfügung stellen.

```text
Unused Partition Time
        ↓
Temporary Borrowing
```

Dabei gilt:

```text
Borrowed Time ≠ Guaranteed Time
```

Die ursprüngliche Partition muss ihre Garantie weiterhin erhalten.

## Slack Reclamation

Ungenutzte Zeit kann durch einen kontrollierten Slack-Reclamation-Mechanismus verwendet werden.

```text
Partition A finishes early
          ↓
Available Slack
          ↓
Eligible Partition B
```

Slack Reclamation darf keine zukünftigen garantierten Zeitfenster verändern.

## Best-Effort Nutzung

Nicht reservierte oder sicher freigegebene Zeit kann von Best-Effort-Workloads verwendet werden.

```text
Guaranteed Partitions
        ↓
Remaining Capacity
        ↓
Best Effort
```

Dadurch muss Time Partitioning nicht zwangsläufig zu dauerhaft ungenutzter CPU-Kapazität führen.

## Multi-Core

Time Partitioning kann mit räumlicher CPU-Isolation kombiniert werden.

```text
CPU 0 → Realtime Partition A
CPU 1 → Realtime Partition B
CPU 2 → System
CPU 3 → Best Effort
```

oder:

```text
CPU 0:
A → B → C

CPU 1:
D → E
```

NovaOS darf je nach Hardware und Execution Contract eine geeignete Kombination aus:

```text
Spatial Isolation
+
Temporal Isolation
```

verwenden.

## Shared Resources

CPU-Time-Partitioning allein garantiert noch keine vollständige Isolation.

Gemeinsam genutzte Ressourcen können weiterhin Interferenzen verursachen:

```text
Memory Bus
Caches
DMA
I/O
Interrupts
GPU
NPU
Devices
```

Eine End-to-End-Realtime-Garantie muss diese Ressourcen zusätzlich berücksichtigen.

## Interrupts

Interrupt-Verarbeitung muss mit Time Partitioning kompatibel sein.

Ein unkontrollierter Interrupt Storm darf nicht beliebig Zeit aus einer garantierten Partition konsumieren.

NovaOS muss relevante Interrupt-Kosten:

```text
Attribute
Bound
Reserve
or
Isolate
```

können.

## Kernel Work

Auch Kernel-Ausführung verbraucht Zeit.

Systemarbeit, die durch eine Partition verursacht wird, soll soweit möglich dieser Partition zugerechnet werden.

```text
Partition Task
     ↓
Syscall
     ↓
Kernel Work
     ↓
Partition Accounting
```

Andernfalls könnten Workloads ihre Budgets indirekt über Kernel-Arbeit umgehen.

## IPC

Kommunikation zwischen Partitionen muss die zeitlichen Grenzen beider Seiten respektieren.

```text
Partition A
    ↓
IPC
    ↓
Partition B
```

Partition A darf Partition B nicht dazu zwingen, außerhalb ihres zulässigen Zeitbudgets Arbeit auszuführen.

Asynchrone Kommunikation wird bevorzugt, wenn dadurch die Isolation verbessert wird.

## AI und Agenten

AI- und Agent-Workloads erhalten keine Sonderrechte gegenüber Time Partitions.

```text
AI Workload
Agent Runtime
Predictive Preloading
Adaptive Optimization
        ↓
Partition Constraints
```

Sie dürfen garantierte Partitionen nicht verdrängen.

## Adaptive Scheduling

Adaptive Scheduling darf freie Kapazität optimieren.

Es darf jedoch keine garantierten Partition Bounds verändern, sofern dies nicht über einen autorisierten Contract-Änderungsprozess erfolgt.

```text
Hard Partition Constraints
          ↓
Available Scheduling Space
          ↓
Adaptive Optimization
```

## Partition Reconfiguration

Time Partitions können grundsätzlich geändert werden.

Eine Änderung von:

```text
Period
Budget
CPU Assignment
Criticality
Scheduling Policy
```

muss jedoch wie eine Änderung einer Ressourcen-Garantie behandelt werden.

Bestehende Hard-Realtime-Contracts dürfen dadurch nicht ungültig werden.

## Admission Control

Neue garantierte Partitionen unterliegen Admission Control.

```text
Requested Partition
        ↓
Existing Partitions
        ↓
Required Capacity
        ↓
Scheduling Feasibility
        ↓
Admit / Reject
```

Es gilt:

```text
Sum of Guaranteed Demand
    ≤
Guaranteable Capacity
```

einschließlich notwendiger System- und Safety-Reserven.

## Failure Isolation

Ein Fehler innerhalb einer Partition soll zeitlich auf diese Partition begrenzt bleiben.

Beispiele:

```text
Infinite Loop
CPU Exhaustion
Runaway Agent
Faulty Service
Unexpected Load
```

Ein solcher Fehler darf nicht automatisch sämtliche CPU-Zeit des Systems konsumieren.

## Safe State

Bei kritischen Partitionen kann ein Overrun einen definierten Safe-State-Pfad auslösen.

```text
Critical Partition
       ↓
Budget Violation
       ↓
Safety Policy
       ↓
Safe State / Failover
```

Der Safe-State-Pfad selbst muss die erforderlichen Ressourcen besitzen.

## Deterministic Execution

Time Partitioning unterstützt Temporal Determinism.

```text
Defined Time Window
+
Bounded Interference
+
Bounded Execution
=
Predictable Temporal Behavior
```

Functional Determinism wird dadurch jedoch nicht automatisch erzeugt.

## Record/Replay

Partition Scheduling kann Bestandteil eines Realtime Record sein.

Aufgezeichnet werden können:

```text
Partition ID
Activation
Budget
Consumed Time
Preemption
Overrun
Slack Reclamation
```

Damit lassen sich zeitliche Fehler später besser analysieren.

## Observability

NovaOS muss Time Partitions beobachten können.

Mindestens:

```text
Partition ID
Execution IDs
Period
Configured Budget
Consumed Budget
Remaining Budget
Activation Time
Overrun
Borrowed Time
Reclaimed Slack
CPU Assignment
Deadline Impact
```

Observability darf die zeitliche Isolation selbst nicht unkontrolliert beeinflussen.

## Normative Anforderungen

1. NovaOS MUSS Time Partitioning für zeitkritische Execution Domains unterstützen können.
2. Eine Time Partition MUSS mindestens Period, Budget und Scheduling Policy definieren können.
3. Partition Budgets MÜSSEN durch den Scheduler durchsetzbar sein.
4. Eine Partition DARF garantierte Zeit einer anderen Partition nicht unkontrolliert konsumieren.
5. Budget Overruns MÜSSEN erkannt werden.
6. Overruns MÜSSEN einer expliziten Policy unterliegen.
7. Realtime Domains MÜSSEN Time Partitions zugeordnet werden können.
8. NovaOS MUSS hierarchisches Scheduling aus Partition- und Task-Scheduling unterstützen können.
9. Task-Prioritäten innerhalb einer Partition DÜRFEN Partition Boundaries nicht umgehen.
10. Hard-Realtime-Partitionen MÜSSEN einem Admission Control unterliegen.
11. Garantierte Time Partitions DÜRFEN nicht spekulativ überbucht werden.
12. Mixed-Criticality-Workloads MÜSSEN zeitlich isolierbar sein.
13. Ungenutzte Partition-Zeit DARF kontrolliert zurückgewonnen werden.
14. Borrowed Time DARF nicht als garantierte Zeit behandelt werden.
15. Slack Reclamation DARF bestehende Hard-Realtime-Garantien nicht verletzen.
16. Time Partitioning MUSS mit Multi-Core- und CPU-Isolation kombinierbar sein.
17. Shared-Resource-Interferenz MUSS bei End-to-End-Garantien zusätzlich berücksichtigt werden.
18. Interrupt-Verarbeitung DARF garantierte Partitionen nicht unbounded beeinträchtigen.
19. Durch eine Partition verursachte Kernel-Arbeit SOLL soweit möglich dieser Partition zugerechnet werden.
20. IPC DARF Partition Boundaries nicht indirekt umgehen.
21. AI-, Agent- und adaptive Workloads DÜRFEN garantierte Partitionen nicht verdrängen.
22. Adaptive Scheduling DARF Hard Partition Constraints nicht eigenständig verändern.
23. Änderungen garantierter Partitionen MÜSSEN gegen bestehende Contracts validiert werden.
24. Fehlerhafte Workloads SOLLEN zeitlich auf ihre Partition begrenzt werden.
25. Kritische Partitionen MÜSSEN definierte Overrun- und Safe-State-Policies unterstützen können.
26. Time Partitioning DARF nicht automatisch mit Functional Determinism gleichgesetzt werden.
27. Partition Scheduling MUSS für Record/Replay erfassbar sein können.
28. Partition Budget, Verbrauch und Overruns MÜSSEN beobachtbar sein.

## Konsequenzen

### Positive Konsequenzen

- Realtime-Domains erhalten klar begrenzte CPU-Zeit,
- fehlerhafte oder überlastete Workloads können andere Partitionen zeitlich nicht beliebig verdrängen,
- Mixed-Criticality-Systeme werden besser beherrschbar,
- Hard-Realtime- und Best-Effort-Workloads können kontrolliert auf derselben Hardware koexistieren,
- ungenutzte Zeit kann durch Slack Reclamation weiterhin effizient genutzt werden,
- Time Partitioning ergänzt Temporal Isolation um eine explizite zeitliche Firewall.

### Negative Konsequenzen

- Partition Scheduling erhöht die Scheduler-Komplexität,
- statische Reservierungen können Ressourcen zeitweise ungenutzt lassen,
- Shared Hardware Resources benötigen zusätzliche Isolation,
- Kernel-, Interrupt- und IPC-Zeit müssen korrekt zugeordnet werden.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-REALTIME-0005_Deterministic_Execution`
- `ADR-REALTIME-0006_Deterministic_IO`
- `ADR-REALTIME-0007_Record_Replay`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AI-0013_KI_darf_harte_Systemregeln_nicht_überschreiben`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-TIME-PARTITION-0001`
- `NPSPEC-REALTIME-PARTITION-SCHEDULER-0001`
- `NPSPEC-REALTIME-PARTITION-BUDGET-0001`
- `NPSPEC-REALTIME-PARTITION-OVERRUN-0001`
- `NPSPEC-REALTIME-SLACK-RECLAMATION-0001`
- `NPSPEC-REALTIME-MIXED-CRITICALITY-0001`

## Ergebnis

Time Partitioning ergänzt die räumliche Ressourcenisolation um eine zeitliche Grenze:

```text
CPU Time
   ↓
┌───────────────────────────────┐
│ Partition A │ B │ Partition C │
│ Guaranteed  │   │ Best Effort │
└───────────────────────────────┘
       ↓
Independent Scheduling Domains
```

Der zentrale Grundsatz lautet:

```text
Nicht nur Speicher und Rechte
besitzen Grenzen.

Auch Ausführungszeit
ist eine isolierbare Ressource.

Eine Partition darf ihre eigene
Zeit verbrauchen –

aber nicht die garantierte Zeit
einer anderen Partition.
```