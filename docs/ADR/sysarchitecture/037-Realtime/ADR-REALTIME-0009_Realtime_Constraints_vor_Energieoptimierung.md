# ADR-REALTIME-0009 – Realtime Constraints vor Energieoptimierung

## Status

Angenommen

## Kategorie

Realtime / Energy / Scheduling / Resource Management

## Kontext

NovaOS soll Energieverbrauch, Akkulaufzeit, Abwärme und Hardwareverschleiß systemweit optimieren.

Dafür können Mechanismen eingesetzt werden wie:

```text
DVFS
CPU Idle States
Core Parking
Device Power States
GPU/NPU Power Management
Workload Consolidation
Adaptive Scheduling
Batching
Race-to-Idle
Predictive Power Management
```

Diese Maßnahmen können jedoch die zeitlichen Eigenschaften eines Systems verändern.

Beispiele:

```text
Lower CPU Frequency
        ↓
Longer Execution Time

Deep Sleep State
        ↓
Wake-up Latency

Powered-down Device
        ↓
Initialization Delay
```

Eine Energieoptimierung darf deshalb keine zugesicherten Realtime-Eigenschaften verletzen.

## Entscheidung

NovaOS behandelt Realtime Constraints als **Hard Constraints gegenüber Energieoptimierung**.

```text
Realtime Requirements
        ↓
Hard Constraint Filtering
        ↓
Valid Power States
        ↓
Energy Optimization
```

Es gilt:

```text
Realtime Guarantee
        >
Energy Optimization
```

Energie wird nur innerhalb des verbleibenden gültigen Entscheidungsraums optimiert.

## Prioritätsprinzip

Bei einer Entscheidung zwischen:

```text
Lower Energy Consumption
```

und:

```text
Guaranteed Realtime Constraint
```

hat die Realtime-Garantie Vorrang.

Dies betrifft insbesondere:

```text
Deadline Guarantees
Bounded Latency
Temporal Isolation
Execution Budgets
Deterministic Execution
Deterministic I/O
Time Partitions
Maximum Jitter
```

## Execution Contract

Realtime- und Energieanforderungen werden gemeinsam über den Execution Contract beschrieben.

Beispiel:

```text
Realtime Profile: Hard
Deadline: 5 ms
Maximum Jitter: 100 µs
Energy Preference: Minimize
```

Die Bedeutung ist:

```text
Meet Realtime Constraints
        first
        ↓
Minimize Energy
within valid solutions
```

Nicht:

```text
Minimize Energy
and hope deadline is met
```

## Hard und Soft Energy Requirements

Energieanforderungen können selbst unterschiedliche Stärke besitzen.

```text
Hard Energy Limit
Soft Energy Preference
```

Ein Hard Energy Limit kann beispielsweise aufgrund physischer oder sicherheitsrelevanter Grenzen bestehen.

In diesem Fall muss NovaOS prüfen, ob gleichzeitig:

```text
Realtime Constraints
+
Energy Constraints
```

erfüllbar sind.

Sind zwei Hard Constraints nicht gemeinsam erfüllbar, darf NovaOS keinen davon stillschweigend abschwächen.

## DVFS

Dynamic Voltage and Frequency Scaling darf verwendet werden, wenn die verbleibende Rechenleistung ausreichend ist.

```text
Current Workload
      ↓
Realtime Bounds
      ↓
Minimum Safe Frequency
      ↓
Allowed DVFS Range
      ↓
Energy Optimization
```

Eine Frequenzreduktion darf keine garantierte Deadline oder Latency Bound gefährden.

## Frequency Floor

Realtime Domains können eine minimale erforderliche Frequenz bestimmen.

Beispiel:

```text
Required WCET at 2.0 GHz
        ↓
Minimum Frequency = 2.0 GHz
```

Der Power Manager darf für den betreffenden kritischen Zeitraum nicht unter diese Grenze gehen.

## Turbo und Boost

Eine Hard-Realtime-Garantie soll nicht ausschließlich auf kurzfristigen, thermisch nicht dauerhaft garantierbaren Boost-Zuständen beruhen.

```text
Temporary Turbo
    ≠
Guaranteed Capacity
```

Boost kann zusätzliche Reserve erzeugen, darf aber nur dann Bestandteil einer Garantie sein, wenn die Plattform diesen Zustand für den erforderlichen Zeitraum tatsächlich garantieren kann.

## CPU Idle States

Tiefe Idle States können erhebliche Wake-up-Latenzen verursachen.

```text
CPU Sleep
    ↓
Wake-up
    ↓
Resume Execution
```

Vor Eintritt in einen Power State muss NovaOS prüfen:

```text
Wake-up Latency
      ≤
Available Realtime Slack
```

Ist dies nicht garantiert, muss ein flacherer Power State verwendet werden.

## Device Power States

Dasselbe gilt für Geräte:

```text
Active
Idle
Sleep
Deep Sleep
Off
```

Ein Gerät, das innerhalb einer garantierten Deadline benötigt werden kann, darf nur in einen Zustand wechseln, dessen Wake-up- und Initialisierungslatenz mit dem Contract vereinbar ist.

## GPU und NPU

Accelerators können eigene:

```text
Frequency States
Power States
Memory States
Clock Gating
Power Gating
```

besitzen.

Realtime-AI- oder Compute-Workloads müssen bei der Energieoptimierung einschließlich ihrer Accelerator-Wake-up- und Model-Residency-Kosten betrachtet werden.

## Time Partitioning

Power Management muss Time Partitions berücksichtigen.

```text
Partition A
Hard Realtime

Partition B
Best Effort
```

Eine Energieentscheidung aufgrund geringer Aktivität in Partition B darf die garantierte Ausführung von Partition A nicht beeinträchtigen.

## Temporal Isolation

Energieoptimierung darf reservierte Realtime-Ressourcen nicht entfernen oder unvorhersehbar verlangsamen.

Beispiele:

```text
Reserved CPU Core
Reserved Memory
Reserved I/O Capacity
Reserved Accelerator
```

Diese Ressourcen dürfen nur dann in einen energiesparenden Zustand wechseln, wenn ihre garantierte Wiederverfügbarkeit rechtzeitig sichergestellt ist.

## Slack-Based Energy Optimization

Verfügbare zeitliche Reserve kann für Energieoptimierung genutzt werden.

```text
Deadline
   -
Worst-Case Remaining Execution
   =
Available Slack
```

NovaOS kann diesen Slack beispielsweise nutzen für:

```text
Frequency Reduction
Deferred Work
Device Sleep
Workload Consolidation
```

Die Realtime-Garantie muss dabei erhalten bleiben.

## Race-to-Idle

In manchen Situationen kann es energieeffizienter sein:

```text
High Performance
      ↓
Finish Early
      ↓
Enter Idle State
```

statt:

```text
Low Frequency
      ↓
Long Execution
```

NovaOS legt deshalb keine universelle Strategie wie:

```text
Always lower frequency
```

fest.

Die Auswahl erfolgt innerhalb der gültigen Realtime Constraints.

## Thermal Constraints

Thermische Limits können selbst Hard Constraints darstellen.

```text
Realtime Requirement
+
Thermal Limit
```

Sind beide nicht gleichzeitig erfüllbar, darf NovaOS das Thermal Limit nicht überschreiten, um eine Deadline künstlich einzuhalten.

Stattdessen muss die definierte Realtime Failure Policy greifen.

## Battery Constraints

Ein niedriger Akkustand darf nicht automatisch bestehende Hard-Realtime-Garantien abschwächen.

NovaOS kann:

```text
Reject new workload
Reduce best-effort activity
Disable background tasks
Reduce AI workload
Enter degraded mode
```

bevor garantierte Realtime-Ressourcen beeinträchtigt werden.

## Best-Effort Workloads

Energieoptimierung darf Best-Effort-Workloads aggressiver beeinflussen.

Beispiele:

```text
Throttle
Delay
Batch
Migrate
Suspend
Reduce Frequency
```

sofern dadurch keine anderen Hard Constraints verletzt werden.

## AI und Adaptive Systeme

AI kann Energieverbrauch vorhersagen und Optimierungsentscheidungen vorschlagen.

```text
Workload Prediction
      ↓
Energy Prediction
      ↓
Optimization Proposal
```

Die tatsächliche Entscheidung bleibt jedoch durch Realtime Constraints begrenzt.

```text
AI Proposal
      ↓
Realtime Validation
      ↓
Execute / Reject
```

KI darf eine Realtime-Garantie nicht zugunsten erwarteter Energieeinsparungen abschwächen.

## Predictive Power Management

NovaOS darf zukünftige Aktivität vorhersagen.

Beispiel:

```text
Predicted Idle Window
      ↓
Select Power State
```

Die Tiefe des Power States darf jedoch nicht ausschließlich von der Vorhersage abhängen, wenn dadurch eine Hard-Realtime-Garantie gefährdet werden könnte.

```text
Prediction Error
    ≠
Permission to miss deadline
```

## Energy Model

NovaOS kann Energieentscheidungen anhand eines Kostenmodells treffen.

Beispielsweise:

```text
Energy Cost
Wake-up Cost
Transition Cost
Execution Time
Thermal Impact
Remaining Slack
```

Optimiert wird nur über Kandidaten, die alle Hard Constraints erfüllen.

## Admission Control

Neue Realtime-Contracts müssen auch unter zulässigen Energie- und Thermalzuständen erfüllbar sein.

```text
Realtime Request
       ↓
Hardware Capacity
       +
Power Constraints
       +
Thermal Constraints
       ↓
Admission Control
```

NovaOS darf keine Garantie annehmen, die nur unter dauerhaft nicht garantierbaren Betriebszuständen erreichbar wäre.

## Power-State Transitions

Power-State-Wechsel besitzen selbst Kosten.

```text
State A
  ↓
Transition Time
  ↓
State B
```

Bei Realtime-Workloads müssen berücksichtigt werden:

```text
Transition Latency
Energy Transition Cost
Temporary Unavailability
Performance Ramp-up
```

## Distributed Execution

Eine energiebedingte Migration auf einen anderen Node darf nur erfolgen, wenn:

```text
Latency
Deadline
Sovereignty
Trust
Determinism
```

weiterhin erfüllt werden.

```text
Energy Saving
    ≠
Migration Authorization
```

## Degradation

Kann ein System aufgrund von:

```text
Battery
Thermal State
Hardware Degradation
Power Limit
```

eine bestehende Realtime-Garantie nicht mehr aufrechterhalten, muss dies explizit behandelt werden.

Mögliche Reaktionen:

```text
Failover
Safe State
Reduced Function
Reject New Work
Abort
Escalate
```

Die Garantie darf nicht stillschweigend in Best Effort umgewandelt werden.

## Observability

NovaOS muss nachvollziehen können, wie Energieentscheidungen Realtime-Executions beeinflussen.

Relevante Informationen:

```text
Execution ID
Realtime Profile
Deadline
Available Slack
Power State
Frequency
Power Transition
Wake-up Latency
Energy Estimate
Thermal State
Optimization Decision
Rejected Energy Optimization
Constraint Violation
```

## Normative Anforderungen

1. NovaOS MUSS Realtime Constraints gegenüber Energieoptimierung priorisieren.
2. Energieoptimierung DARF nur innerhalb des durch Hard Constraints erlaubten Entscheidungsraums erfolgen.
3. Hard-Realtime-Deadlines DÜRFEN nicht zugunsten einer Energieeinsparung abgeschwächt werden.
4. Bounded Latency DARF durch Power Management nicht verletzt werden.
5. Temporal Isolation DARF durch Energieoptimierung nicht aufgehoben werden.
6. Time Partition Guarantees DÜRFEN durch Power Management nicht beeinträchtigt werden.
7. DVFS DARF nur verwendet werden, wenn verbleibende Realtime-Garantien eingehalten werden können.
8. Realtime Domains MÜSSEN bei Bedarf eine minimale Performance- oder Frequenzgrenze erzwingen können.
9. Nicht garantierbare Turbo- oder Boost-Zustände DÜRFEN nicht alleinige Grundlage einer Hard-Realtime-Garantie sein.
10. Wake-up-Latenzen von CPU- und Device-Power-States MÜSSEN bei Realtime-Contracts berücksichtigt werden.
11. Reservierte Realtime-Ressourcen DÜRFEN nur in Power States wechseln, aus denen sie rechtzeitig wieder verfügbar sind.
12. Slack DARF für Energieoptimierung verwendet werden, solange die Realtime-Garantie erhalten bleibt.
13. NovaOS DARF keine universelle Energieoptimierungsstrategie wie dauerhaftes Downclocking voraussetzen.
14. Thermal Safety Constraints DÜRFEN nicht zur Einhaltung einer Realtime-Deadline verletzt werden.
15. Niedriger Batteriestand DARF bestehende Hard-Realtime-Contracts nicht stillschweigend abschwächen.
16. Best-Effort-Workloads SOLLEN vor garantierten Realtime-Workloads gedrosselt oder verschoben werden.
17. AI-basierte Energieentscheidungen MÜSSEN gegen Realtime Constraints validiert werden.
18. Predictive Power Management DARF Hard-Realtime-Garantien nicht von einer probabilistischen Vorhersage abhängig machen.
19. Power-State-Transition-Latenzen MÜSSEN Bestandteil relevanter Realtime-Analysen sein.
20. Admission Control MUSS verfügbare und garantierbare Power- und Thermalzustände berücksichtigen.
21. Energiebedingte Distributed Migration DARF bestehende Hard Constraints nicht verletzen.
22. Verlust einer Realtime-Garantie aufgrund von Power- oder Thermalzuständen MUSS explizit signalisiert werden.
23. Eine bestehende Hard-Realtime-Garantie DARF nicht automatisch zu Soft oder Best Effort degradiert werden.
24. Energieentscheidungen und ihre Realtime-Auswirkungen MÜSSEN beobachtbar sein.

## Konsequenzen

### Positive Konsequenzen

- Energieoptimierung bleibt mit echten Realtime-Garantien vereinbar,
- DVFS und Sleep States können sicher innerhalb verfügbarer Slack-Zeit genutzt werden,
- AI-basierte Power-Optimierung kann keine Hard-Realtime-Regeln überschreiben,
- thermische und elektrische Grenzen bleiben explizite Hard Constraints,
- Realtime-Workloads werden nicht unerwartet durch aggressive Energiesparmechanismen verlangsamt.

### Negative Konsequenzen

- Realtime-Workloads können den minimal erreichbaren Energieverbrauch erhöhen,
- tiefe Power States sind während kritischer Zeiträume teilweise nicht nutzbar,
- Energieplanung benötigt Informationen über Wake-up- und Transition-Latenzen,
- Admission Control muss Power- und Thermalzustände berücksichtigen.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-REALTIME-0005_Deterministic_Execution`
- `ADR-REALTIME-0006_Deterministic_IO`
- `ADR-REALTIME-0008_Time_Partitioning`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-ADAPTIVE-0008_Adaptive_Power_Management`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`
- `ADR-AI-0013_KI_darf_harte_Systemregeln_nicht_überschreiben`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-ENERGY-CONSTRAINT-0001`
- `NPSPEC-REALTIME-DVFS-0001`
- `NPSPEC-REALTIME-POWER-STATE-0001`
- `NPSPEC-REALTIME-SLACK-ENERGY-0001`
- `NPSPEC-REALTIME-THERMAL-CONSTRAINT-0001`
- `NPSPEC-REALTIME-POWER-DEGRADATION-0001`

## Ergebnis

NovaOS optimiert Energie erst nach Prüfung der zeitlichen Garantien:

```text
Realtime Constraints
        ↓
Safety / Thermal Constraints
        ↓
Valid Hardware States
        ↓
Available Slack
        ↓
Energy Optimization
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf Energie sparen,
solange die zugesicherten
Realtime-Eigenschaften
erhalten bleiben.

Energiesparen ist eine Optimierung.

Eine akzeptierte
Hard-Realtime-Garantie
ist ein Vertrag.

Die Optimierung muss sich
dem Vertrag unterordnen –
nicht umgekehrt.
```