# NPSPEC-SCHEDULER-QOS-0001 – Nova Quality of Service Scheduling

## Status

Entwurf

## Kategorie

Scheduler / QoS / Resource Economy / Execution Policy

## Zweck

NovaOS definiert Quality-of-Service-Scheduling zur kontrollierten Priorisierung unterschiedlicher Workloads anhand ihrer funktionalen und qualitativen Anforderungen.

```text
Tasks
  ↓
QoS Requirements
  ↓
Scheduler
  ↓
Resource Allocation
  ↓
Expected Service Quality
```

QoS ergänzt klassische Prioritäten um Anforderungen wie Latenz, Durchsatz, Responsivität, Ressourcenbudgets und Degradationsverhalten.

## Grundprinzip

QoS beschreibt gewünschte Serviceeigenschaften.

```text
Execution Contract
      +
QoS Requirements
      ↓
Resource Allocation
      ↓
Scheduling Behaviour
```

Dabei gilt:

```text
QoS ≠ Priority
```

Eine QoS-Klasse darf mehrere Scheduling- und Ressourcenparameter beeinflussen.

## QoS-Profil

Ein Task, eine Task Group oder ein Service kann ein QoS-Profil besitzen.

Beispielhafte Eigenschaften:

```text
Latency Target
Throughput Target
Responsiveness
CPU Budget
Memory Budget
I/O Budget
Energy Preference
Deadline
Degradation Policy
```

Nicht benötigte Eigenschaften können entfallen.

## QoS-Klassen

NovaOS darf standardisierte QoS-Klassen bereitstellen.

Beispiel:

```text
Critical
Interactive
Foreground
Service
Background
Maintenance
```

Diese Klassen sind semantische Profile und keine festen CPU-Prioritäten.

Anwendungen dürfen konkrete Anforderungen bevorzugt über Execution Contracts ausdrücken.

## Interaktive Workloads

Interaktive Tasks sollen geringe wahrgenommene Latenz erhalten.

```text
User Input
    ↓
Interactive Work
    ↓
Low Latency Scheduling
```

Kurzfristige Bevorzugung darf erfolgen, solange andere Hard Requirements nicht verletzt werden.

## Hintergrundarbeit

Nicht zeitkritische Arbeit darf mit geringerer Scheduling-Dringlichkeit ausgeführt werden.

Beispiele:

```text
Indexing
Cleanup
Prefetching
Background Analysis
Maintenance
```

Solche Tasks dürfen freie Ressourcen nutzen, sollen aber aktive Benutzerarbeit nicht unnötig beeinträchtigen.

## Ressourcensteuerung

QoS kann mehrere Ressourcen gleichzeitig betreffen.

```text
CPU
Memory
Storage I/O
Network
Accelerators
Energy
```

Der Scheduler koordiniert CPU-Entscheidungen mit der systemweiten Resource Economy.

## Hierarchische QoS

QoS kann hierarchisch gelten.

```text
Session
  ↓
Application
  ↓
Task Group
  ↓
Task
```

Child-Anforderungen dürfen die Hard Limits ihrer übergeordneten Ressourcendomäne nicht umgehen.

## Admission Control

QoS-Anforderungen mit garantierten Ressourcen können eine Admission-Control-Prüfung erfordern.

```text
Requested QoS
      ↓
Available Capacity
      ↓
Admission Control
      ↓
Accept / Degrade / Reject
```

NovaOS darf keine Garantie zusagen, wenn die erforderlichen Ressourcen nicht verfügbar sind.

## Degradation

Kann die gewünschte QoS nicht vollständig bereitgestellt werden, muss ein definiertes Verhalten möglich sein.

```text
QoS Target Miss
      ↓
Degradation Policy
      ↓
Reduce Quality
Delay Work
Reduce Parallelism
Fallback
Reject
```

Degradation darf keine Hard Requirements verletzen.

## Fairness

QoS und Fair Scheduling müssen zusammenarbeiten.

```text
QoS
 +
Fairness
 ↓
Effective Scheduling
```

Eine hohe QoS-Klasse darf normale Tasks nicht unbegrenzt verdrängen, sofern keine expliziten Hard Requirements dies verlangen.

## Realtime und Deadlines

Realtime- und Hard-Deadline-Anforderungen sind stärker als normale QoS-Präferenzen.

```text
Safety / Security
      ↓
Hard Realtime / Hard Deadline
      ↓
Hard Resource Constraints
      ↓
QoS
      ↓
Soft Optimization
```

QoS darf keine Realtime-Garantie vortäuschen.

## Energie und Thermal

QoS kann mit Energie- und Thermal-Zielen kombiniert werden.

```text
Interactive
→ Prefer Responsiveness

Background
→ Prefer Efficiency
```

Thermische Sicherheitsgrenzen bleiben unabhängig von QoS verbindlich.

## Adaptive QoS

NovaOS darf Scheduling innerhalb zulässiger QoS-Grenzen dynamisch optimieren.

Berücksichtigt werden können:

```text
Observed Latency
CPU Load
Queue Pressure
User Activity
Energy State
Thermal State
Resource Contention
```

Explizite Nutzerentscheidungen und Hard Constraints haben Vorrang.

## Normative Anforderungen

1. NovaOS MUSS QoS-Anforderungen unabhängig von einfachen Task-Prioritäten modellieren können.
2. QoS MUSS mit Execution Contracts kombinierbar sein.
3. QoS DARF CPU-, Speicher-, I/O-, Netzwerk- und Accelerator-Ressourcen berücksichtigen.
4. Hard Requirements MÜSSEN Vorrang vor QoS-Optimierungen haben.
5. Garantierte QoS-Anforderungen MÜSSEN bei Bedarf Admission Control verwenden.
6. Nicht verfügbare Ressourcen DÜRFEN NICHT als garantiert zugesagt werden.
7. Interaktive Workloads DÜRFEN kontrolliert bevorzugt werden.
8. Hintergrundarbeit SOLL aktive interaktive Arbeit möglichst wenig beeinträchtigen.
9. Hierarchische Ressourcenlimits MÜSSEN durch Child-QoS eingehalten werden.
10. QoS-Degradation MUSS kontrolliert und explizit erfolgen.
11. Adaptive QoS-Optimierung DARF Hard Constraints NICHT überschreiben.
12. QoS-Zustand, Zielverletzungen und Scheduling-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-FAIR-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-SCHEDULER-ENERGY-0001`
- `NPSPEC-SCHEDULER-THERMAL-0001`
- `NPSPEC-SCHEDULER-ACCELERATOR-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SCHED-0012`

## Ergebnis

```text
Task / Service
      ↓
Execution Contract + QoS
      ↓
Hard Constraint Filter
      ↓
Resource Availability
      ↓
QoS-Aware Scheduling
      ↓
Execute / Degrade / Reject
```

NovaOS erhält damit ein QoS-Scheduling-Modell, das unterschiedliche Qualitätsanforderungen systemweit ausdrückbar macht und CPU-, I/O-, Energie- und andere Ressourcen kontrolliert auf interaktive, zeitkritische und Hintergrund-Workloads verteilt.