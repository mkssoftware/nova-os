# ADR-REALTIME-0004 – Deadline Guarantees

## Status

Angenommen

## Kategorie

Realtime / Deadlines / Scheduling / Execution Contracts

## Kontext

NovaOS unterstützt Workloads mit expliziten Deadlines.

Eine Deadline beschreibt den spätesten Zeitpunkt, zu dem eine Ausführung oder ein definiertes Zwischenergebnis abgeschlossen sein muss.

```text
Release Time
     ↓
Execution
     ↓
Deadline
```

Für Hard Realtime reicht es nicht aus, Deadlines lediglich als Scheduler-Priorität zu behandeln.

Eine Deadline Guarantee darf nur bestehen, wenn NovaOS den vollständigen kritischen Ausführungspfad und die dafür notwendigen Ressourcen garantieren kann.

## Entscheidung

NovaOS führt **Deadline Guarantees** als explizite, durch Admission Control abgesicherte Eigenschaft von Execution Contracts ein.

```text
Execution Contract
      ↓
Deadline Requirement
      ↓
Feasibility Analysis
      ↓
Resource Reservation
      ↓
Admission Control
      ↓
Execution
      ↓
Deadline Verification
```

Eine akzeptierte Hard-Realtime-Deadline wird zu einer verbindlichen Systemgarantie.

## Deadline Definition

Ein Execution Contract kann mindestens definieren:

```text
Release Time
Deadline
Execution Budget
Period
Maximum Jitter
Realtime Profile
Failure Policy
```

Eine relative Deadline wird gegenüber dem Release-Zeitpunkt interpretiert:

```text
Absolute Deadline
=
Release Time + Relative Deadline
```

## Deadline und Realtime Profile

Die Bedeutung einer Deadline hängt vom Realtime Profile ab.

```text
Soft Realtime
→ Deadline is a target

Firm Realtime
→ Late result may be discarded

Hard Realtime
→ Deadline is guaranteed
```

Nur Hard Realtime erzeugt eine verbindliche Deadline Guarantee.

## Guarantee Preconditions

Eine Deadline darf nur garantiert werden, wenn der gesamte kritische Pfad analysierbar ist.

Dazu gehören beispielsweise:

```text
Scheduling Latency
Execution Time
Blocking Time
Interrupt Latency
IPC
Memory Access
I/O
Device Response
Communication
```

Es gilt:

```text
Worst-Case End-to-End Latency
    ≤
Available Deadline Window
```

## Admission Control

Vor Annahme einer Hard-Realtime-Execution prüft NovaOS:

```text
Deadline
   +
Execution Budget
   +
Existing Reservations
   +
Dependency Bounds
   +
Available Capacity
        ↓
Feasible?
```

Nur wenn die Deadline unter den zugesicherten Bedingungen eingehalten werden kann, wird der Contract angenommen.

```text
Can Guarantee?
 ├── Yes → Admit
 └── No  → Reject / Renegotiate
```

## Keine Überbuchung

Garantierte Deadline-Kapazität darf nicht spekulativ überbucht werden.

```text
Guaranteed Demand
      ≤
Guaranteed Capacity
```

NovaOS darf keine zusätzlichen Hard-Realtime-Contracts akzeptieren, wenn dadurch bereits zugesicherte Deadlines gefährdet würden.

## Resource Reservation

Für eine Deadline Guarantee benötigte Ressourcen müssen reservierbar sein.

Beispiele:

```text
CPU Budget
Memory
I/O Bandwidth
Interrupt Capacity
Device Time
Network Capacity
Accelerator Time
```

Best-Effort-, AI- oder adaptive Workloads dürfen diese Reservierungen nicht verdrängen.

## Deadline Propagation

Eine End-to-End-Deadline kann auf abhängige Operationen verteilt werden.

```text
Total Deadline: 10 ms

Capability A: 2 ms
Service B:    3 ms
Driver C:     2 ms
Device:       2 ms
Reserve:      1 ms
```

Untergeordnete Execution Contracts dürfen daraus eigene Teil-Deadlines erhalten.

## Dependency Guarantees

Eine Komponente darf keine stärkere Deadline Guarantee anbieten als ihre Abhängigkeiten ermöglichen.

```text
Execution
    ↓
Capability
    ↓
Service
    ↓
Driver
    ↓
Device
```

Eine unbounded Dependency verhindert eine garantierte End-to-End-Deadline.

## Scheduling

Der Scheduler muss garantierte Deadlines bei seiner Planung berücksichtigen.

Geeignete Scheduling Policies können beispielsweise sein:

```text
Earliest Deadline First
Fixed Priority
Deadline Monotonic
Reserved Execution Windows
```

Die ADR legt keinen universellen Scheduling-Algorithmus fest.

Entscheidend ist, dass die gewählte Policy die akzeptierten Guarantees einhalten kann.

## Deadline Slack

NovaOS darf verbleibenden zeitlichen Spielraum explizit behandeln.

```text
Slack
=
Deadline
-
Expected Remaining Worst-Case Time
```

Slack kann zur Optimierung verwendet werden, darf aber die eigentliche Garantie nicht abschwächen.

## Periodische Workloads

Periodische Realtime-Tasks können wiederkehrende Deadlines besitzen.

```text
Period 1 → Deadline 1
Period 2 → Deadline 2
Period 3 → Deadline 3
```

Jede Instanz muss entsprechend ihrem Contract behandelt werden.

Ein früherer Deadline Miss darf nicht unbemerkt auf spätere Perioden übertragen werden.

## Deadline Miss

Eine Deadline gilt als verfehlt, wenn:

```text
Completion Time > Deadline
```

Die Behandlung hängt vom Realtime Profile ab.

```text
Soft
→ Record + Continue

Firm
→ Discard / Skip Result

Hard
→ Realtime Failure
```

## Early Detection

NovaOS soll erkennen können, wenn eine Deadline bereits vor ihrem tatsächlichen Ablauf nicht mehr erreichbar ist.

```text
Remaining Time
      <
Required Worst-Case Time
      ↓
Deadline At Risk
```

Dadurch kann frühzeitig eine definierte Reaktion ausgelöst werden.

## Deadline Failure Policy

Ein Execution Contract kann festlegen, wie auf eine gefährdete oder verfehlte Deadline reagiert wird.

Beispiele:

```text
Abort
Drop Result
Use Previous Result
Fallback
Failover
Enter Safe State
Escalate
```

Die Failure Policy darf keine Hard Safety- oder Security-Regeln verletzen.

## Dynamic System State

Ändert sich der Systemzustand während einer garantierten Execution, muss NovaOS prüfen, ob die Garantie weiterhin besteht.

Beispiele:

```text
Hardware Failure
Thermal Constraint
Device Failure
Resource Loss
Provider Failure
```

Kann die Guarantee nicht mehr eingehalten werden, muss dies explizit als Realtime Failure oder gefährdete Guarantee behandelt werden.

## Adaptive Scheduling

Adaptive Systeme dürfen Scheduling und Ressourcennutzung innerhalb vorhandener Reservierungen optimieren.

```text
Hard Deadline Guarantee
        ↓
Valid Scheduling Space
        ↓
Adaptive Optimization
```

Eine AI- oder Prediction-basierte Entscheidung darf keine zugesicherte Deadline gefährden.

## Distributed Deadlines

Bei verteilter Ausführung umfasst die Deadline den gesamten relevanten Pfad.

```text
Node A
  ↓
Network
  ↓
Node B
  ↓
Execution
  ↓
Network
  ↓
Result
```

Ohne belastbare Grenzen für Netzwerk und Remote Execution darf keine entsprechende Hard-Realtime-Deadline garantiert werden.

## Clock Requirements

Absolute Deadlines über mehrere Komponenten oder Nodes benötigen eine ausreichend definierte Zeitbasis.

NovaOS muss unterscheiden können zwischen:

```text
Monotonic Local Time
Synchronized Time
External Time Reference
```

Clock-Synchronisation und deren Unsicherheit müssen bei verteilten Deadline Guarantees berücksichtigt werden.

## Determinismus

Deadline Guarantee und Determinismus bleiben getrennte Eigenschaften.

```text
Deadline Guarantee
    ≠
Deterministic Result
```

Ein Contract kann beide Eigenschaften gleichzeitig verlangen.

## Observability

Deadline Guarantees müssen beobachtbar sein.

Mindestens:

```text
Execution ID
Realtime Profile
Release Time
Deadline
Execution Budget
Reserved Resources
Completion Time
Remaining Slack
Deadline Risk
Deadline Miss
Failure Policy
```

Die Messung selbst darf die Guarantee nicht unkontrolliert beeinflussen.

## Normative Anforderungen

1. NovaOS MUSS Deadlines über Execution Contracts ausdrücken können.
2. Hard-Realtime-Deadlines MÜSSEN als verbindliche Systemgarantien behandelbar sein.
3. Deadline Guarantees MÜSSEN einem Admission Control unterliegen.
4. Eine Deadline DARF nur garantiert werden, wenn der relevante End-to-End-Pfad ausreichende Bounds besitzt.
5. Garantierte Realtime-Kapazität DARF nicht spekulativ überbucht werden.
6. Für Deadline Guarantees notwendige Ressourcen MÜSSEN reservierbar sein.
7. Best-Effort-, AI- und adaptive Workloads DÜRFEN garantierte Ressourcen nicht verdrängen.
8. End-to-End-Deadlines MÜSSEN auf abhängige Operationen aufteilbar sein.
9. Eine Komponente DARF keine stärkere Deadline Guarantee anbieten als ihre Abhängigkeiten ermöglichen.
10. Der Scheduler MUSS akzeptierte Deadline Guarantees berücksichtigen.
11. NovaOS DARF keinen bestimmten universellen Realtime-Scheduling-Algorithmus voraussetzen.
12. Periodische Executions MÜSSEN ihre Deadlines pro Instanz behandeln können.
13. Deadline Misses MÜSSEN entsprechend dem Realtime Profile behandelt werden.
14. Ein Hard-Realtime-Deadline-Miss MUSS als Realtime Failure gelten.
15. Nicht mehr erreichbare Deadlines SOLLEN möglichst vor Ablauf erkannt werden.
16. Execution Contracts SOLLEN eine Deadline Failure Policy definieren können.
17. Verlust einer bestehenden Deadline Guarantee MUSS explizit signalisiert werden.
18. Adaptive Optimierung DARF zugesicherte Deadlines nicht gefährden.
19. AI-Vorhersagen DÜRFEN eine erforderliche Deadline Guarantee nicht ersetzen.
20. Distributed Deadline Guarantees MÜSSEN Netzwerk- und Remote-Latenzen berücksichtigen.
21. Verteilte absolute Deadlines MÜSSEN die verwendete Zeitbasis und deren Unsicherheit berücksichtigen.
22. Deadline Guarantees und Determinism MÜSSEN als getrennte Eigenschaften behandelt werden.
23. Deadline Guarantees, Risiken und Misses MÜSSEN beobachtbar sein.
24. Kann eine geforderte Deadline nicht garantiert werden, MUSS der Contract abgelehnt oder explizit neu ausgehandelt werden.

## Konsequenzen

### Positive Konsequenzen

- Hard-Realtime-Deadlines werden zu echten Systemgarantien statt Prioritätswünschen,
- nicht erfüllbare Contracts werden bereits vor der Ausführung erkannt,
- End-to-End-Deadlines können auf Subsysteme verteilt werden,
- Ressourcenreservierung und Scheduling werden gemeinsam geplant,
- gefährdete Deadlines können frühzeitig erkannt werden.

### Negative Konsequenzen

- Admission Control benötigt Worst-Case-Informationen über den Execution Path,
- Ressourcen müssen teilweise exklusiv reserviert werden,
- verteilte Deadline Guarantees benötigen kontrollierte Netzwerk- und Zeiteigenschaften.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
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

- `NPSPEC-REALTIME-DEADLINE-GUARANTEE-0001`
- `NPSPEC-REALTIME-DEADLINE-ADMISSION-0001`
- `NPSPEC-REALTIME-DEADLINE-PROPAGATION-0001`
- `NPSPEC-REALTIME-DEADLINE-SLACK-0001`
- `NPSPEC-REALTIME-DEADLINE-RISK-0001`
- `NPSPEC-REALTIME-DEADLINE-FAILURE-0001`

## Ergebnis

NovaOS behandelt eine Hard-Realtime-Deadline als zugesicherte Systemeigenschaft:

```text
Deadline Request
      ↓
Bounded Execution Path
      ↓
Admission Control
      ↓
Resource Reservation
      ↓
Scheduling
      ↓
Execution
      ↓
Deadline Verification
```

Der zentrale Grundsatz lautet:

```text
Eine Deadline ist erst dann
eine Garantie,
wenn NovaOS vor der Ausführung
nachweisen kann,
dass genügend Zeit und Ressourcen
dafür verfügbar sind.

Kann NovaOS das nicht garantieren,
wird der Contract nicht angenommen.

Eine unerfüllbare Garantie
wird nicht versprochen.
```