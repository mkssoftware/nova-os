# ADR-REALTIME-0002 – Temporal Isolation

## Status

Angenommen

## Kategorie

Realtime / Scheduling / Resource Isolation

## Kontext

Realtime-Garantien sind nur möglich, wenn zeitkritische Workloads vor unkontrollierten Verzögerungen durch andere Workloads geschützt werden.

Mögliche Störquellen sind:

```text
CPU Contention
Interrupt Storms
Memory Pressure
I/O Contention
Lock Contention
Cache Pollution
Background Services
AI Workloads
Adaptive Tasks
Driver Activity
```

Eine hohe Scheduler-Priorität allein reicht dafür nicht aus.

## Entscheidung

NovaOS führt **Temporal Isolation** als systemweite Eigenschaft für zeitkritische Executions ein.

```text
Realtime Execution
       ↓
Temporal Isolation
       ↓
Reserved / Bounded Resources
       ↓
Predictable Execution Window
```

Eine Execution erhält damit einen zeitlich geschützten Ressourcenbereich, in dem andere Workloads ihre zugesicherten Realtime-Eigenschaften nicht unkontrolliert beeinträchtigen dürfen.

## Isolation Scope

Temporal Isolation kann mehrere Ressourcen umfassen:

```text
CPU Time
Scheduler Capacity
Memory
Interrupt Processing
I/O Bandwidth
Device Access
Network Capacity
Accelerator Time
```

Welche Ressourcen isoliert werden müssen, ergibt sich aus dem jeweiligen Execution Contract.

## CPU Isolation

Realtime-Executions können CPU-Zeit oder vollständige CPU-Kapazität reservieren.

Beispiele:

```text
Dedicated Core
Reserved Runtime
Periodic Execution Budget
Priority Protected Window
```

Best-Effort-Workloads dürfen garantierte CPU-Budgets nicht verdrängen.

## Execution Budgets

Realtime-Tasks erhalten definierbare Ausführungsbudgets.

```text
Period: 10 ms
Budget: 2 ms
Deadline: 10 ms
```

Der Scheduler überwacht die Nutzung des Budgets.

Eine Execution darf nicht unbegrenzt CPU-Zeit beanspruchen, nur weil sie als Realtime klassifiziert ist.

## Memory Isolation

Realtime-Pfade sollen vor unvorhersehbarem Memory Pressure geschützt werden.

Dafür können Ressourcen vorab reserviert werden:

```text
Physical Memory
Locked Pages
Execution Buffers
DMA Buffers
Shared Memory
```

Zeitkritische Speicherbereiche dürfen nicht durch normalen Speicherdruck verdrängt werden.

## I/O Isolation

Realtime-I/O benötigt kontrollierbare Bandbreite und Latenz.

```text
Realtime Task
     ↓
I/O Reservation
     ↓
Driver
     ↓
Device
```

Best-Effort-I/O darf garantierte Realtime-I/O-Budgets nicht unkontrolliert blockieren.

## Interrupt Isolation

Interrupt-Verarbeitung kann Realtime-Ausführungen erheblich verzögern.

NovaOS soll deshalb unterstützen:

```text
Interrupt Affinity
Interrupt Priorities
Bounded Interrupt Work
Deferred Processing
Dedicated CPU Assignment
```

Unbegrenzte Arbeit innerhalb eines Interrupt-Kontexts ist auf Realtime-kritischen Pfaden zu vermeiden.

## Lock Contention

Realtime-Ausführungen dürfen nicht unkontrolliert auf niedrig priorisierte Tasks warten.

```text
High Priority Task
       ↓
Lock
       ↓
Low Priority Task
```

Mechanismen gegen Priority Inversion müssen vorgesehen werden.

Geeignete Verfahren können beispielsweise sein:

```text
Priority Inheritance
Priority Ceiling
Lock Avoidance
Bounded Critical Sections
```

## Shared Resources

Gemeinsam verwendete Ressourcen müssen ihre zeitlichen Eigenschaften berücksichtigen.

```text
Shared Resource
   ├── Realtime Consumer
   └── Best-Effort Consumer
```

Ein Best-Effort-Consumer darf einen reservierten Realtime-Zugriff nicht unbegrenzt verzögern.

## Realtime Domains

NovaOS darf mehrere Realtime-Executions zu einer zeitlich isolierten Domain zusammenfassen.

```text
Realtime Domain
 ├── Task A
 ├── Task B
 └── Service C
```

Die Domain besitzt gemeinsame:

```text
CPU Budgets
Memory Reservations
I/O Reservations
Scheduling Constraints
```

Damit können komplette Realtime-Pipelines isoliert werden.

## End-to-End Isolation

Temporal Isolation gilt nicht nur für den unmittelbar laufenden Prozess.

Beispiel:

```text
Realtime Task
    ↓
Capability
    ↓
Service
    ↓
Driver
    ↓
Device
```

Alle zeitkritischen Bestandteile des Pfades müssen die erforderlichen Isolationseigenschaften unterstützen.

## Hard Realtime

Für Hard Realtime ist ausreichende Temporal Isolation verpflichtend.

```text
Hard Realtime
      ↓
Required Isolation
      ↓
Admission Control
```

Kann NovaOS die erforderliche Isolation nicht bereitstellen, darf der Hard-Realtime-Contract nicht akzeptiert werden.

## Firm und Soft Realtime

Firm und Soft Realtime können ebenfalls Temporal Isolation verwenden, jedoch mit schwächeren Garantien.

Beispielsweise:

```text
Soft Realtime
→ Preferential Isolation

Firm Realtime
→ Reserved Budget

Hard Realtime
→ Guaranteed Isolation
```

Die konkrete Stärke wird durch den Execution Contract definiert.

## Admission Control

Vor Annahme garantierter Realtime-Executions prüft NovaOS:

```text
Requested Resources
       ↓
Existing Reservations
       ↓
Available Capacity
       ↓
Guarantee Possible?
```

Nur tatsächlich garantierbare Ressourcen dürfen zugesichert werden.

## Overcommit

Hard-Realtime-Ressourcen dürfen nicht spekulativ überbucht werden.

```text
Guaranteed Capacity
      ≥
Accepted Hard Realtime Demand
```

Soft- und Best-Effort-Workloads dürfen ungenutzte Kapazität verwenden, müssen sie jedoch bei Bedarf freigeben.

## AI und Adaptive Workloads

AI-, Preloading-, Caching- und adaptive Optimierungsaufgaben gelten grundsätzlich als verdrängbare Workloads gegenüber garantierten Realtime-Ressourcen.

```text
Hard Realtime Reservation
          >
AI / Adaptive Optimization
```

Adaptive Systeme dürfen Temporal Isolation nicht selbstständig reduzieren.

## Resource Reclamation

NovaOS darf ungenutzte Realtime-Kapazität temporär anderen Workloads zur Verfügung stellen.

```text
Reserved Capacity
      ↓
Currently Unused
      ↓
Best-Effort Borrowing
```

Dabei gilt:

```text
Borrowed ≠ Owned
```

Die Ressource muss rechtzeitig für den garantierten Realtime-Workload zurückgewonnen werden können.

## Failure Verhalten

Kann eine zugesicherte Temporal Isolation nicht mehr eingehalten werden:

```text
Isolation Violation
       ↓
Realtime Failure
       ↓
Configured Failure Policy
```

Mögliche Reaktionen:

```text
Degrade
Failover
Safe State
Restart
Abort
Escalate
```

Eine verlorene Garantie darf nicht stillschweigend weiter als erfüllt gelten.

## Distributed Execution

Temporal Isolation über mehrere Nodes ist nur möglich, wenn alle beteiligten Systeme ausreichende Garantien liefern.

```text
Node A
  ↓
Network
  ↓
Node B
```

Nicht deterministische Netzwerkpfade dürfen nicht als garantiert isoliert dargestellt werden.

## Observability

Temporal Isolation muss messbar sein.

Relevante Werte sind:

```text
Execution ID
Realtime Domain
Reserved Budget
Consumed Budget
Scheduling Delay
Blocking Time
Interrupt Delay
I/O Delay
Deadline Miss
Isolation Violation
```

Monitoring selbst darf den Realtime-Pfad nicht unkontrolliert beeinflussen.

## Normative Anforderungen

1. NovaOS MUSS Temporal Isolation für Realtime-Executions unterstützen können.
2. Temporal Isolation MUSS CPU, Memory, I/O und weitere relevante Ressourcen umfassen können.
3. Hard Realtime MUSS ausreichende Temporal Isolation voraussetzen.
4. Garantierte CPU-Budgets DÜRFEN nicht durch Best-Effort-Workloads verdrängt werden.
5. Realtime-Executions MÜSSEN begrenzbare Execution Budgets besitzen können.
6. Realtime-Speicher MUSS vor unkontrolliertem Memory Pressure schützbar sein.
7. Realtime-I/O MUSS reservierbare Bandbreite oder zeitliche Garantien unterstützen können.
8. Interrupt-Verarbeitung DARF Realtime-Pfade nicht unbegrenzt blockieren.
9. Priority Inversion MUSS auf zeitkritischen Pfaden begrenzt werden.
10. Kritische Sections auf Realtime-Pfaden MÜSSEN zeitlich begrenzbar sein.
11. Shared Resources DÜRFEN garantierte Realtime-Ausführungen nicht unbegrenzt blockieren.
12. Realtime Domains MÜSSEN mehrere zusammengehörige Executions isolieren können.
13. Temporal Isolation MUSS entlang des gesamten kritischen Execution Paths berücksichtigt werden.
14. Garantierte Realtime-Ressourcen MÜSSEN einem Admission Control unterliegen.
15. Hard-Realtime-Ressourcen DÜRFEN nicht spekulativ überbucht werden.
16. AI- und adaptive Workloads DÜRFEN garantierte Realtime-Ressourcen nicht beeinträchtigen.
17. Ungenutzte reservierte Ressourcen DÜRFEN kontrolliert durch Best-Effort-Workloads verwendet werden.
18. Geliehene Ressourcen MÜSSEN rechtzeitig zurückgewonnen werden können.
19. Verlust einer zugesicherten Isolation MUSS als explizites Ereignis behandelt werden.
20. Temporal-Isolation-Verletzungen MÜSSEN beobachtbar und auditierbar sein.
21. NovaOS DARF keine Temporal-Isolation-Garantie behaupten, die der vollständige kritische Pfad nicht erfüllen kann.

## Konsequenzen

### Positive Konsequenzen

- Realtime-Workloads werden vor normalen Systemlasten geschützt,
- Hard-Realtime-Garantien erhalten eine technisch belastbare Grundlage,
- CPU, Memory und I/O können gemeinsam als zeitliche Ressourcen geplant werden,
- Priority Inversion und unkontrollierte Blockierungen werden begrenzt,
- ungenutzte Realtime-Ressourcen können trotzdem effizient verwendet werden.

### Negative Konsequenzen

- Ressourcenreservierungen können die Auslastung reduzieren,
- Scheduler, Memory Manager, I/O und Treiber benötigen Realtime-Unterstützung,
- End-to-End-Isolation erhöht die Komplexität der Ressourcenplanung.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
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

- `NPSPEC-REALTIME-TEMPORAL-ISOLATION-0001`
- `NPSPEC-REALTIME-CPU-RESERVATION-0001`
- `NPSPEC-REALTIME-MEMORY-ISOLATION-0001`
- `NPSPEC-REALTIME-IO-ISOLATION-0001`
- `NPSPEC-REALTIME-INTERRUPT-ISOLATION-0001`
- `NPSPEC-REALTIME-DOMAIN-0001`

## Ergebnis

Temporal Isolation schützt zeitkritische Ausführungen vor unkontrollierter Konkurrenz:

```text
Realtime Execution
        ↓
Reserved CPU
Reserved Memory
Reserved I/O
Bounded Interrupts
Bounded Blocking
        ↓
Predictable Execution
```

Der zentrale Grundsatz lautet:

```text
Eine Deadline kann nur garantiert werden,
wenn auch die dafür benötigte Zeit
und die benötigten Ressourcen
geschützt werden.

Realtime bedeutet deshalb nicht nur
"zuerst ausführen",

sondern:

"andere Workloads dürfen die
zugesicherte Ausführungszeit
nicht unkontrolliert wegnehmen."
```