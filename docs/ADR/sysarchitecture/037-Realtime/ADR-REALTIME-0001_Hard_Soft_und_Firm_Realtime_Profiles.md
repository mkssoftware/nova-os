# ADR-REALTIME-0001 – Hard, Soft und Firm Realtime Profiles

## Status

Angenommen

## Kategorie

Realtime / Scheduling / Execution Contracts

## Kontext

NovaOS soll Workloads mit unterschiedlichen zeitlichen Anforderungen unterstützen.

Nicht jede Deadline besitzt dieselbe Bedeutung:

```text
Audio Playback
Industrial Control
GUI Rendering
Sensor Processing
Network Processing
Simulation
Background Tasks
```

Bei manchen Aufgaben ist eine verspätete Ausführung lediglich störend. Bei anderen ist das Ergebnis nach Überschreiten der Deadline wertlos oder eine Deadline-Verletzung sogar unzulässig.

Eine einzelne globale Realtime-Klasse reicht deshalb nicht aus.

## Entscheidung

NovaOS definiert drei explizite Realtime Profiles:

```text
Soft Realtime
Firm Realtime
Hard Realtime
```

Das Profil wird Bestandteil des `Nova.ExecutionContract`.

```text
Execution Contract
    ↓
Realtime Profile
    ↓
Scheduler + Resource Planning
    ↓
Execution
    ↓
Deadline Verification
```

## Soft Realtime

Bei `Soft Realtime` ist eine Deadline wichtig, darf aber gelegentlich überschritten werden.

```text
Deadline Miss
    ↓
Result remains useful
```

Typische Beispiele:

```text
Audio / Video
Interactive UI
Games
Streaming
Non-critical Visualization
```

Ziel ist eine möglichst geringe Anzahl und Dauer von Deadline Misses.

Eine absolute Garantie besteht nicht.

## Firm Realtime

Bei `Firm Realtime` ist ein Ergebnis nach Ablauf der Deadline nicht mehr sinnvoll.

```text
Execution
   ↓
Deadline Miss
   ↓
Result = Obsolete
```

Das verspätete Ergebnis kann verworfen werden.

Beispiele:

```text
Sensor Sampling
Live Analysis
Control Updates
Realtime Tracking
Time-sensitive Data Processing
```

Einzelne Deadline Misses können toleriert werden, dürfen aber nicht als erfolgreiche zeitgerechte Ausführung behandelt werden.

## Hard Realtime

Bei `Hard Realtime` ist das Einhalten der Deadline Bestandteil der Korrektheit.

```text
Correct Result
      +
Deadline Met
      =
Valid Execution
```

Ein korrektes Ergebnis nach Ablauf der Deadline gilt als Fehler.

Hard Realtime darf nur zugesichert werden, wenn NovaOS die dafür erforderlichen Ressourcen und Ausführungseigenschaften garantieren kann.

## Keine falschen Garantien

NovaOS darf einen normalen Best-Effort-Workload nicht lediglich durch höhere Priorität zu Hard Realtime erklären.

```text
High Priority
    ≠
Hard Realtime
```

Hard Realtime benötigt nachweisbare Voraussetzungen wie:

```text
Bounded Execution Time
Reserved Resources
Bounded Interrupt Latency
Bounded Scheduling Latency
Predictable Memory Behavior
Controlled Dependencies
Bounded I/O Paths
```

Sind diese Voraussetzungen nicht erfüllbar, muss der Hard-Realtime-Contract abgelehnt werden.

## Execution Contract

Ein Realtime Contract kann mindestens enthalten:

```text
Realtime Profile
Deadline
Period
Maximum Jitter
Execution Budget
Priority
Required Resources
Failure Policy
```

Beispiel:

```text
Profile: Hard
Period: 10 ms
Deadline: 10 ms
Execution Budget: 2 ms
Maximum Jitter: 100 µs
```

## Scheduling

Der Scheduler berücksichtigt das Realtime Profile bei der Ausführungsplanung.

Grundsätzlich:

```text
Hard Realtime
      ↓
Reserved / Guaranteed Resources

Firm Realtime
      ↓
Deadline-aware Scheduling

Soft Realtime
      ↓
Latency-aware Scheduling

Best Effort
      ↓
Normal Scheduling
```

Das Realtime Profile ersetzt jedoch keine konkrete Scheduling Policy.

## Resource Reservation

Hard-Realtime-Ausführungen müssen notwendige Ressourcen vorab reservieren können.

Beispiele:

```text
CPU Time
Memory
Interrupt Capacity
I/O Bandwidth
Device Access
Network Capacity
```

Spekulative oder adaptive Workloads dürfen diese Reservierungen nicht beeinträchtigen.

## Memory

Hard Realtime darf nicht von unvorhersehbaren Speicheroperationen abhängig sein.

Während zeitkritischer Phasen sollen insbesondere vermeidbar sein:

```text
Unbounded Allocation
Paging
Swap
Memory Reclamation
Unexpected Copying
```

Benötigter Speicher soll vorher reserviert oder garantiert verfügbar sein.

## I/O und Geräte

Ein Hard-Realtime-Contract ist nur möglich, wenn der gesamte kritische Pfad die erforderlichen zeitlichen Garantien unterstützt.

```text
Task
 ↓
Scheduler
 ↓
Driver
 ↓
Device
 ↓
Result
```

Ein nicht deterministisch reagierendes Gerät kann keine garantierte Hard-Realtime-Ausführung ermöglichen.

## Abhängigkeiten

Realtime-Garantien gelten für den gesamten kritischen Dependency Path.

```text
Realtime Task
    ↓
Capability A
    ↓
Service B
    ↓
Driver C
```

Besitzt eine notwendige Abhängigkeit keine ausreichende zeitliche Garantie, darf NovaOS keine stärkere End-to-End-Garantie behaupten.

## Deadline Miss

Deadline Misses werden entsprechend dem Profil behandelt.

```text
Soft
→ Continue + Record Miss

Firm
→ Result may be discarded

Hard
→ Realtime Failure
```

Die konkrete Recovery- oder Safe-State-Reaktion wird durch den Execution Contract bestimmt.

## Overload

Bei Ressourcenüberlastung haben garantierte Realtime-Reservierungen Vorrang vor:

```text
Background Tasks
Predictive Preloading
Adaptive Optimization
AI Workloads
Cache Population
Best-Effort Compute
```

NovaOS darf nicht versuchen, mehr Hard-Realtime-Workloads zu akzeptieren, als garantiert bedient werden können.

```text
Admission Control
      ↓
Can Guarantee?
 ├── Yes → Admit
 └── No  → Reject
```

## Adaptive Systeme

Adaptive Scheduling darf Realtime-Ausführungen optimieren, aber keine Hard-Realtime-Garantien gefährden.

```text
Hard Constraints
      ↓
Valid Scheduling Space
      ↓
Adaptive Optimization
```

Vorhersagen können bei Soft und Firm Realtime stärker verwendet werden.

Hard Realtime darf nicht von einer probabilistischen Vorhersage abhängen, wenn dadurch die Garantie selbst unsicher wird.

## AI Workloads

AI-Ausführungen sind standardmäßig keine Hard-Realtime-Ausführungen.

Ein AI-Workload darf nur Hard Realtime verwenden, wenn Modell, Runtime, Provider und kompletter Execution Path entsprechende Grenzen garantieren können.

```text
AI Prediction
    ≠
Realtime Guarantee
```

## Distributed Execution

Remote Execution erhöht Unsicherheit durch:

```text
Network Latency
Queueing
Remote Scheduling
Failure Domains
Clock Differences
```

Hard Realtime über verteilte Systeme darf deshalb nur angeboten werden, wenn der gesamte Pfad entsprechende Garantien besitzt.

Andernfalls muss ein schwächeres Profil verwendet oder die Ausführung abgelehnt werden.

## Determinismus

Realtime und Determinismus sind getrennte Eigenschaften.

```text
Deterministic
    ≠
Realtime

Realtime
    ≠
Deterministic
```

Ein Execution Contract kann beide Anforderungen gleichzeitig verlangen.

## Observability

Realtime-Ausführungen müssen messbar sein.

Relevante Werte sind:

```text
Execution ID
Realtime Profile
Deadline
Start Time
Completion Time
Execution Time
Jitter
Deadline Miss
Reserved Resources
Scheduling Delay
```

Messung darf Hard-Realtime-Pfade selbst nicht unkontrolliert beeinträchtigen.

## Normative Anforderungen

1. NovaOS MUSS `Soft`, `Firm` und `Hard Realtime` unterscheiden.
2. Das Realtime Profile MUSS Bestandteil eines Execution Contracts sein können.
3. Soft-Realtime-Deadline-Misses DÜRFEN toleriert werden.
4. Firm-Realtime-Ergebnisse DÜRFEN nach einem Deadline Miss als wertlos verworfen werden.
5. Bei Hard Realtime MUSS die Deadline Bestandteil der Ausführungskorrektheit sein.
6. Hohe Priorität DARF nicht mit Hard Realtime gleichgesetzt werden.
7. Hard Realtime DARF nur zugesichert werden, wenn die notwendigen Garantien tatsächlich verfügbar sind.
8. Hard-Realtime-Workloads MÜSSEN vor ihrer Annahme einem Admission Control unterliegen.
9. Notwendige Ressourcen MÜSSEN für Hard Realtime reservierbar sein.
10. Spekulative Workloads DÜRFEN Hard-Realtime-Reservierungen nicht beeinträchtigen.
11. Der gesamte kritische Dependency Path MUSS die erforderliche Realtime-Garantie erfüllen.
12. Deadline Misses MÜSSEN entsprechend dem Realtime Profile behandelt werden.
13. Hard-Realtime-Deadline-Misses MÜSSEN als Realtime Failure behandelt werden.
14. Adaptive Optimierung DARF Hard-Realtime-Garantien nicht verletzen.
15. Probabilistische Vorhersagen DÜRFEN keine erforderliche Hard-Realtime-Garantie ersetzen.
16. AI-Workloads DÜRFEN nicht automatisch als Hard Realtime behandelt werden.
17. Distributed Hard Realtime DARF nur bei garantierbarem End-to-End-Pfad zugesichert werden.
18. Realtime und Determinism MÜSSEN als getrennte Contract-Eigenschaften behandelt werden.
19. Realtime-Ausführungen MÜSSEN hinsichtlich Deadline, Jitter und Execution Time beobachtbar sein.
20. Kann eine geforderte Hard-Realtime-Garantie nicht erfüllt werden, MUSS der Contract abgelehnt oder explizit neu ausgehandelt werden.

## Konsequenzen

### Positive Konsequenzen

- zeitkritische Workloads erhalten klar definierte Garantien,
- Hard Realtime wird nicht mit bloßer hoher Priorität verwechselt,
- Ressourcen können gezielt für kritische Aufgaben reserviert werden,
- Soft- und Firm-Realtime-Workloads können flexibler optimiert werden,
- Realtime-Anforderungen integrieren sich in die bestehende Execution-Contract-Architektur.

### Negative Konsequenzen

- Hard Realtime benötigt Admission Control und Ressourcenreservierung,
- Treiber und Hardware müssen ihre zeitlichen Eigenschaften beschreiben können,
- bestimmte dynamische Systemfunktionen müssen auf Hard-Realtime-Pfaden eingeschränkt werden.

## Abhängigkeiten

- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AI-0013_KI_darf_harte_Systemregeln_nicht_überschreiben`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-HARD-0001`
- `NPSPEC-REALTIME-FIRM-0001`
- `NPSPEC-REALTIME-SOFT-0001`
- `NPSPEC-REALTIME-ADMISSION-CONTROL-0001`
- `NPSPEC-REALTIME-DEADLINE-MISS-0001`

## Ergebnis

NovaOS unterscheidet zeitliche Anforderungen nach ihrer tatsächlichen Bedeutung:

```text
Soft Realtime
Deadline wichtig,
Miss tolerierbar.

Firm Realtime
Deadline wichtig,
verspätetes Ergebnis wertlos.

Hard Realtime
Deadline ist Teil
der Korrektheit.
```

Der zentrale Grundsatz lautet:

```text
NovaOS verspricht keine
Realtime-Garantie,
die es nicht technisch
durchsetzen kann.

Hard Realtime ist
eine garantierte Eigenschaft –

keine Prioritätsstufe.
```