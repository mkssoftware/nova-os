# NPSPEC-TIME-TIMER-0001 – Nova Timer

## Status

Angenommen

## Kategorie

Time / Timer

## Zweck

NovaOS definiert eine einheitliche Timer-Infrastruktur für zeitgesteuerte Ereignisse, Timeouts, Deadlines und periodische Ausführung.

Logische Timer bleiben von konkreten Hardware-Timern und Clock Sources getrennt.

## Grundprinzipien

```text
Timer ≠ Clock Source
Timer ≠ Clock Domain
Timer ≠ Scheduler
Timer ≠ Sleep
Expiration ≠ Immediate Execution
Deadline ≠ Guaranteed Execution Time
Periodic Timer ≠ Repeated Relative Delay
```

## Modell

```text
Timer
├── TimerID
├── OwnerID
├── ClockDomainID
├── Type
├── Deadline
├── Period
├── Tolerance
├── State
└── ExpirationAction
```

## Timer-Typen

NovaOS unterstützt mindestens:

```text
One-Shot
Periodic
Deadline
High-Resolution
Coalescable
```

### One-Shot

Löst einmal nach Erreichen der definierten Deadline aus.

### Periodic

Erzeugt wiederkehrende Ereignisse mit einer definierten Periode.

Periodische Timer sollen ihre nächste Deadline aus der ursprünglichen Zeitbasis ableiten:

```text
Deadline[n] = Start + n × Period
```

Dadurch wird kumulative Drift durch wiederholtes `Now + Period` vermieden.

## Architektur

```text
Clock Domain
     ↓
Timer Core
     ↓
Timer Queue
     ↓
Clock Event Provider
     ↓
Expiration
     ↓
Scheduler / Event
```

## Deadline

Timer verwenden eine absolute Deadline innerhalb ihrer Clock Domain.

```text
Now
 +
Duration
 ↓
Absolute Deadline
```

Interne Timeouts und Deadlines sollen bevorzugt monotone Clock Domains verwenden.

## Expiration

Das Erreichen einer Deadline bedeutet:

```text
Timer Expired
      ↓
Event becomes eligible
      ↓
Scheduler / Event Processing
      ↓
Execution
```

Die tatsächliche Ausführung kann später erfolgen.

NovaOS darf daher nicht garantieren:

```text
Expiration Time = Execution Time
```

## Timer Queue

Timer dürfen effizient in gemeinsamen Datenstrukturen verwaltet werden.

Die Implementierung kann beispielsweise verwenden:

```text
Priority Queue
Timer Wheel
Hierarchical Timer Wheel
Deadline Tree
Hybrid Structure
```

Die konkrete Datenstruktur ist keine öffentliche Timer-Semantik.

## Hardware Timer

Viele logische Timer dürfen auf wenige Hardware-Timer abgebildet werden:

```text
Logical Timers
      ↓
Earliest Relevant Deadline
      ↓
Clock Event Provider
      ↓
Hardware Timer
```

Ein Hardwareinterrupt pro logischem Timer ist nicht erforderlich.

## High Resolution

Timer mit entsprechendem Execution Contract dürfen die High-Resolution-Time-Infrastruktur verwenden.

Normale Timer sollen keine unnötig hohe Auflösung erzwingen.

## Toleranz und Coalescing

Ein Timer darf eine zulässige Toleranz besitzen:

```text
Deadline
   +
Tolerance
   ↓
Coalescing Window
```

NovaOS darf kompatible Timer innerhalb dieses Fensters zusammenfassen, um Wakeups und Energieverbrauch zu reduzieren.

Hard Deadlines dürfen nicht überschritten werden.

## Suspend

Das Verhalten eines Timers ergibt sich aus seiner Clock Domain.

```text
Clock Domain pauses during suspend
→ Timer pauses

Clock Domain continues during suspend
→ Deadline continues
```

Timer selbst benötigen dadurch keine implizite Suspend-Sondersemantik.

## Cancellation

Timer müssen kontrolliert abbrechbar sein:

```text
Armed
 ↓
Cancel
 ↓
Cancelled
```

Cancellation muss Race Conditions mit gleichzeitig eintretender Expiration eindeutig behandeln.

## Lebenszyklus

```text
Created
  ↓
Armed
  ↓
Expired
  ↓
Completed

Armed
  ↓
Cancelled
```

Periodische Timer können nach einer Expiration erneut `Armed` werden.

## Normative Anforderungen

1. NovaOS MUSS logische Timer von Hardware-Timern trennen.
2. Jeder Timer MUSS einer Clock Domain zugeordnet sein.
3. One-Shot- und periodische Timer MÜSSEN unterstützt werden.
4. Timer SOLLEN absolute Deadlines innerhalb ihrer Clock Domain verwenden.
5. Expiration und tatsächliche Ausführung MÜSSEN getrennte Ereignisse bleiben.
6. Mehrere logische Timer MÜSSEN auf gemeinsamen Hardware-Timern multiplexbar sein.
7. High-Resolution Timer MÜSSEN unterstützt werden können.
8. Timer-Toleranzen und Coalescing MÜSSEN unterstützt werden können.
9. Hard Deadlines DÜRFEN durch Coalescing nicht verletzt werden.
10. Periodische Timer SOLLEN kumulative Drift vermeiden.
11. Timer MÜSSEN sicher abbrechbar sein.
12. TimerID, ClockDomainID, Deadline, Toleranz, Zustand und tatsächliche Expiration MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-HIGHRES-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine skalierbare Timer-Infrastruktur, die logische Timer unabhängig von konkreten Hardware-Timern verwaltet. Timer können unterschiedliche Clock Domains, Auflösungen und Toleranzen verwenden und energieeffizient zusammengefasst werden, während Deadlines, Expiration und tatsächliche Ausführung klar voneinander getrennt bleiben.