# ADR-REALTIME-0006 – Deterministic I/O

## Status

Angenommen

## Kategorie

Realtime / I/O / Determinism / Drivers

## Kontext

Eine deterministische Realtime-Ausführung kann nur so vorhersehbar sein wie ihre I/O-Pfade.

Selbst wenn CPU-Scheduling, Speicher und Execution Time begrenzt sind, können unkontrollierte I/O-Latenzen eine Realtime-Garantie zerstören.

Typische Ursachen sind:

```text
Device Queueing
Driver Blocking
DMA Contention
Bus Contention
Interrupt Delays
Storage Latency
Network Variability
Device Internal Caching
Power State Transitions
Firmware Behavior
Retries
Error Recovery
```

Für Hard Realtime reicht es deshalb nicht aus, nur den ausführenden Task deterministisch zu planen.

Der vollständige Pfad:

```text
Task
 ↓
I/O API
 ↓
Driver
 ↓
Bus
 ↓
Device
 ↓
Completion
```

muss ausreichend bekannte und begrenzte Eigenschaften besitzen.

## Entscheidung

NovaOS führt **Deterministic I/O** als explizite Eigenschaft von Realtime-I/O-Pfaden ein.

```text
Realtime Execution
       ↓
I/O Requirement
       ↓
Deterministic I/O Path
       ↓
Bounded Queueing
Bounded Driver Work
Bounded Device Response
       ↓
Completion
```

Ein I/O-Pfad darf nur als deterministisch gelten, wenn die für den jeweiligen Execution Contract erforderlichen zeitlichen und funktionalen Eigenschaften garantiert werden können.

## I/O Determinism Profile

I/O-Anforderungen können unterscheiden zwischen:

```text
None
Temporal Determinism
Functional Determinism
Temporal + Functional
```

`Temporal Determinism` betrifft die zeitliche Begrenzbarkeit.

`Functional Determinism` betrifft reproduzierbare I/O-Semantik unter definierten Bedingungen.

## Temporal Deterministic I/O

Für Temporal Determinism muss eine obere Grenze für den relevanten I/O-Pfad bekannt sein.

```text
I/O Request
    ↓
Queueing
    ↓
Dispatch
    ↓
Device Processing
    ↓
Completion

Total I/O Latency ≤ Bound
```

Die Grenze muss alle relevanten Komponenten berücksichtigen.

## Functional Deterministic I/O

Functional Determinism bedeutet, dass definierte Eingaben und definierter Gerätezustand eine definierte beobachtbare I/O-Semantik erzeugen.

Beispielsweise müssen relevante Eigenschaften festgelegt sein:

```text
Operation
Target
Ordering
Atomicity
Device State
Driver Version
Configuration
```

Externe physische Eingaben wie Sensorwerte sind dadurch nicht automatisch reproduzierbar.

Ihre Erfassung und Reihenfolge müssen gegebenenfalls separat aufgezeichnet werden.

## I/O Contracts

Treiber und I/O Provider müssen ihre relevanten Eigenschaften maschinenlesbar beschreiben können.

Beispiele:

```text
Maximum Request Latency
Maximum Queue Depth
Maximum Service Time
Ordering Guarantees
Atomicity
Retry Policy
Timeout Bound
DMA Requirements
Interrupt Behavior
Failure Semantics
```

Der Execution Planner kann diese Informationen für Realtime-Contracts verwenden.

## Bounded Queueing

Realtime-I/O darf nicht unbegrenzt hinter normalen I/O-Requests warten.

```text
Best-Effort Queue
      ≠
Guaranteed Realtime Queue
```

NovaOS muss Mechanismen für begrenzbares Queueing unterstützen.

Beispiele:

```text
Reserved Queue Slots
Priority Queues
Dedicated Queues
Bandwidth Reservation
Time-Sliced Device Access
```

## I/O Reservation

Zeitkritische Workloads können I/O-Ressourcen reservieren.

```text
Realtime Domain
      ↓
I/O Reservation
      ↓
Driver / Device
```

Reservierbar können beispielsweise sein:

```text
Bandwidth
IOPS
Queue Slots
Device Time
DMA Capacity
Bus Capacity
```

Best-Effort-I/O darf garantierte Reservierungen nicht verdrängen.

## Driver Requirements

Ein Realtime-fähiger Treiber muss kritische Operationen zeitlich begrenzen können.

Zu vermeiden sind:

```text
Unbounded Polling
Infinite Waits
Unbounded Retry Loops
Unbounded Lock Waiting
Dynamic Allocation in Critical Path
Unknown Firmware Waits
```

Jeder relevante Wait muss:

```text
Bounded
Timed
or
Explicitly Non-Realtime
```

sein.

## Device Requirements

Ein Treiber kann keine stärkere Garantie anbieten als das zugrunde liegende Gerät.

```text
Driver Guarantee
    ≤
Device Guarantee
```

Besitzt ein Gerät keine bekannte obere Antwortzeit, darf der entsprechende Pfad nicht als Hard-Realtime-deterministisch behandelt werden.

## DMA

DMA kann CPU-Last reduzieren, erzeugt aber gemeinsame Ressourcennutzung.

NovaOS muss bei deterministischem I/O berücksichtigen:

```text
DMA Channels
DMA Buffers
Memory Bandwidth
IOMMU Translation
Bus Arbitration
Completion Interrupts
```

Realtime-DMA-Ressourcen müssen bei Bedarf reservierbar sein.

## Interrupts

I/O-Completion über Interrupts muss mit den Realtime-Anforderungen kompatibel sein.

```text
Device
   ↓
Interrupt
   ↓
Handler
   ↓
Completion
```

Interrupt-Latenz und Handler-Ausführungszeit müssen ausreichend begrenzbar sein.

Nichtkritische Verarbeitung soll aus dem zeitkritischen Interrupt-Pfad ausgelagert werden.

## Polling

Für bestimmte Hard-Realtime-Pfade kann kontrolliertes Polling geeigneter sein als Interrupt-basierte Verarbeitung.

NovaOS legt jedoch keinen universellen Mechanismus fest.

```text
Interrupt
or
Bounded Polling
```

wird anhand des Execution Contracts und der Hardwareeigenschaften gewählt.

Unbounded Busy Waiting ist nicht zulässig.

## Storage

Persistenter Storage besitzt häufig komplexe interne Zustände:

```text
Controller Cache
Flash Translation Layer
Garbage Collection
Wear Leveling
Firmware
Error Recovery
```

Daher darf normale SSD-, NVMe-, USB- oder andere Storage-Latenz nicht automatisch als Hard-Realtime-deterministisch betrachtet werden.

Hard-Realtime-Storage benötigt explizit nachweisbare Bounds oder eine geeignete Architektur, die den kritischen Pfad davon entkoppelt.

## Network I/O

Normale Netzwerke besitzen typischerweise keine harte obere Latenzgarantie.

```text
Local Execution
   ↓
Network
   ↓
Remote Endpoint
```

Deterministic Network I/O darf nur angenommen werden, wenn der verwendete Netzwerkpfad entsprechende Eigenschaften bereitstellt.

Andernfalls kann der Pfad höchstens mit einem schwächeren Realtime Profile verwendet werden.

## Device Sharing

Ein gemeinsam verwendetes Gerät darf einen Realtime-Consumer nicht unkontrolliert durch andere Consumer blockieren.

```text
Device
 ├── Hard Realtime
 ├── Soft Realtime
 └── Best Effort
```

Die I/O-Scheduling-Policy muss die zugesicherten Garantien erhalten.

## Priority Inversion

I/O kann Priority Inversion verursachen.

Beispiel:

```text
Low Priority Request
       ↓
Device Busy
       ↓
High Priority Realtime Request
```

NovaOS muss solche Situationen durch geeignete Mechanismen begrenzen.

Mögliche Verfahren:

```text
Priority-aware Queues
Preemption where supported
Request Segmentation
Reservation
Bounded Non-preemptive Sections
```

## Non-Preemptive Devices

Viele Geräte können eine gestartete Operation nicht abbrechen oder unterbrechen.

Für solche Geräte muss die maximale nicht-preemptierbare Operationsdauer in die Realtime-Analyse eingehen.

```text
Maximum Blocking
=
Maximum Non-Preemptive Operation
```

## Retries

Automatische Retries dürfen keine unbounded Latenz erzeugen.

```text
Request
  ↓
Failure
  ↓
Retry
  ↓
Retry
  ↓
...
```

Realtime-I/O benötigt:

```text
Retry Budget
Timeout
Maximum Attempts
Failure Policy
```

## Error Recovery

Komplexe Recovery-Prozesse dürfen nicht unbemerkt innerhalb eines garantierten Realtime-Pfades ausgeführt werden.

Wenn Recovery die zugesicherte Deadline gefährdet:

```text
I/O Failure
     ↓
Realtime Failure Policy
```

statt unbegrenzter Wiederherstellungsversuche.

## Power Management

Geräte können unterschiedliche Power States besitzen.

```text
Active
Idle
Sleep
Deep Sleep
```

Wake-up-Latenzen müssen bei Realtime-I/O berücksichtigt werden.

Ein Gerät darf nicht in einen Power State versetzt werden, aus dem es seine zugesicherten Realtime-Anforderungen nicht rechtzeitig erfüllen kann.

## Cache Verhalten

Device- und Controller-Caches dürfen Garantien nicht verfälschen.

Beispielsweise:

```text
Write Accepted
    ≠
Data Persisted
```

Der Contract muss unterscheiden können zwischen:

```text
Submitted
Accepted
Transferred
Completed
Persisted
```

Die relevante Completion-Semantik muss explizit sein.

## Ordering

Realtime-I/O kann definierte Ordering-Anforderungen besitzen.

```text
Operation A
    ↓
Operation B
```

Wenn Reihenfolge Teil der Korrektheit ist, muss der I/O-Pfad diese Reihenfolge garantieren oder explizite Synchronisationsmechanismen verwenden.

## Temporal Isolation

Deterministic I/O baut auf Temporal Isolation auf.

```text
I/O Reservation
+
Bounded Queueing
+
Bounded Device Access
=
Temporal I/O Isolation
```

Ohne Isolation können konkurrierende I/O-Workloads die Latenzgarantie zerstören.

## Admission Control

Vor Annahme eines garantierten I/O-Contracts prüft NovaOS:

```text
Requested I/O Bound
        ↓
Driver Properties
        ↓
Device Properties
        ↓
Existing Reservations
        ↓
Bus / DMA Capacity
        ↓
Guarantee Possible?
```

Ist die Garantie nicht nachweisbar:

```text
Reject
```

oder:

```text
Renegotiate
```

## Unknown Device Behavior

Es gilt:

```text
Unknown Device Bound
    ≠
Deterministic I/O
```

Unbekannte Firmware-, Controller- oder Device-Latenzen dürfen nicht durch Schätzungen zu Hard-Realtime-Garantien werden.

## Adaptive I/O

Adaptive Systeme dürfen:

```text
Queue Placement
Prefetching
Caching
Power States
Request Batching
```

optimieren.

Dabei gilt:

```text
Hard I/O Constraints
       ↓
Valid Optimization Space
       ↓
Adaptive Optimization
```

Adaptive Entscheidungen dürfen garantierte I/O-Bounds nicht verletzen.

## Failure Verhalten

Wird eine zugesicherte I/O-Grenze verletzt:

```text
Observed I/O Latency
        >
Guaranteed I/O Bound
```

entsteht eine:

```text
I/O Determinism Violation
```

Bei Hard Realtime wird diese als Realtime Failure behandelt.

## Observability

Deterministic I/O muss beobachtbar sein.

Relevante Informationen:

```text
Execution ID
I/O Request ID
Device ID
Driver Version
Operation
Queue Delay
Service Time
Completion Time
Retry Count
Timeout
Reserved Resources
Latency Bound
Bound Violation
```

Messung darf den kritischen I/O-Pfad selbst nicht unkontrolliert beeinflussen.

## Normative Anforderungen

1. NovaOS MUSS Deterministic I/O als explizite Realtime-Eigenschaft unterstützen.
2. Temporal und Functional I/O Determinism MÜSSEN getrennt ausdrückbar sein.
3. Realtime-I/O-Garantien MÜSSEN den vollständigen relevanten I/O-Pfad berücksichtigen.
4. Treiber MÜSSEN relevante Realtime-Eigenschaften maschinenlesbar beschreiben können.
5. Ein Treiber DARF keine stärkere Garantie anbieten als das zugrunde liegende Gerät.
6. Hard-Realtime-I/O MUSS bounded Queueing besitzen.
7. I/O-Ressourcen MÜSSEN für garantierte Workloads reservierbar sein.
8. Best-Effort-I/O DARF garantierte I/O-Reservierungen nicht verdrängen.
9. Unbounded Polling DARF nicht Bestandteil eines garantierten Realtime-Pfades sein.
10. Unbounded Retry Loops DÜRFEN nicht Bestandteil eines garantierten Realtime-Pfades sein.
11. Realtime-I/O MUSS definierte Timeouts und Retry Budgets unterstützen.
12. Interrupt-Latenz MUSS bei interruptbasiertem Realtime-I/O berücksichtigt werden.
13. DMA- und Bus-Contention MÜSSEN bei entsprechenden Garantien berücksichtigt werden.
14. Nicht-preemptierbare Geräteoperationen MÜSSEN als maximale Blocking Time berücksichtigt werden.
15. Device Power State Transitions DÜRFEN zugesicherte Realtime-Garantien nicht verletzen.
16. Completion- und Persistence-Semantik MÜSSEN explizit unterscheidbar sein.
17. Erforderliches I/O Ordering MUSS explizit garantierbar sein.
18. Hard-Realtime-I/O MUSS ausreichende Temporal Isolation besitzen.
19. Garantierte I/O-Contracts MÜSSEN einem Admission Control unterliegen.
20. `Unknown Device Behavior` DARF nicht als deterministische Garantie behandelt werden.
21. Adaptive I/O-Optimierung DARF Hard-Realtime-Garantien nicht abschwächen.
22. Verlust einer zugesicherten I/O-Garantie MUSS explizit signalisiert werden.
23. Hard-Realtime-I/O-Verletzungen MÜSSEN als Realtime Failure behandelt werden.
24. I/O-Latenzen und Bound Violations MÜSSEN beobachtbar sein.

## Konsequenzen

### Positive Konsequenzen

- Realtime-Garantien umfassen auch tatsächliche Gerätezugriffe,
- Treiber werden Teil des expliziten Realtime-Vertragsmodells,
- Queueing, DMA, Interrupts und Device Sharing werden systematisch berücksichtigt,
- unbounded Hardware-Waits und Retry-Schleifen werden auf kritischen Pfaden ausgeschlossen,
- I/O-Ressourcen können gemeinsam mit CPU und Memory geplant werden.

### Negative Konsequenzen

- viele normale Geräte können keine Hard-Realtime-Garantien anbieten,
- Treiber benötigen zusätzliche Realtime-Metadaten,
- Geräteinterne Firmware kann starke Garantien verhindern,
- I/O-Admission-Control und Ressourcenreservierung erhöhen die Komplexität.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-REALTIME-0005_Deterministic_Execution`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-IO-0001`
- `ADR-DRIVER-0001`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-DETERMINISTIC-IO-0001`
- `NPSPEC-REALTIME-IO-CONTRACT-0001`
- `NPSPEC-REALTIME-IO-QUEUE-0001`
- `NPSPEC-REALTIME-IO-RESERVATION-0001`
- `NPSPEC-REALTIME-IO-TIMEOUT-0001`
- `NPSPEC-REALTIME-IO-VIOLATION-0001`

## Ergebnis

NovaOS betrachtet I/O als Teil der Realtime-Garantie:

```text
Realtime Task
      ↓
I/O Contract
      ↓
Bounded Queue
      ↓
Bounded Driver
      ↓
Bounded Device
      ↓
Defined Completion
```

Der zentrale Grundsatz lautet:

```text
Eine deterministische CPU-Ausführung
reicht nicht aus,
wenn der Task anschließend
unbegrenzt auf Hardware wartet.

Realtime endet deshalb nicht
am Treibereinstieg.

Die Garantie muss bis zu dem Punkt
reichen, an dem die benötigte
I/O-Operation tatsächlich
definiert abgeschlossen ist.
```