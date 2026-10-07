# NPSPEC-TIME-HIGHRESTIMER-0001 – Nova High-Resolution Timer

## Status

Angenommen

## Kategorie

Time / High-Resolution Timer

## Zweck

NovaOS definiert High-Resolution Timer für zeitkritische Operationen, deren benötigte Auflösung über normale Systemtimer hinausgeht.

High-Resolution Timer verwenden die hochauflösende monotone Zeitbasis und bleiben von konkreten Hardware-Timern abstrahiert.

## Grundprinzipien

```text
High-Resolution Timer ≠ Hardware Timer
High-Resolution Timer ≠ Busy Waiting
High Resolution ≠ Exact Execution
Expiration ≠ Immediate Execution
Precision ≠ Accuracy
Requested Deadline ≠ Guaranteed Deadline
```

## Modell

```text
HighResTimer
├── TimerID
├── OwnerID
├── ClockDomainID
├── Deadline
├── Period
├── Tolerance
├── RequestedResolution
├── EffectiveResolution
├── State
└── ExpirationAction
```

## Architektur

```text
High-Resolution Clock
        ↓
High-Resolution Timer Core
        ↓
Deadline Queue
        ↓
Clock Event Provider
        ↓
Hardware Timer
        ↓
Expiration
```

Mehrere High-Resolution Timer dürfen auf einem gemeinsamen Hardware-Timer multiplexiert werden.

## Verwendung

High-Resolution Timer sind insbesondere vorgesehen für:

```text
Realtime Workloads
Media Synchronization
Audio Processing
Precise Device Timing
Fine-Grained Deadlines
Latency-Sensitive Operations
Profiling
```

Normale Aufgaben sollen Standard-Timer verwenden, wenn deren Auflösung ausreichend ist.

## Deadline

Ein High-Resolution Timer verwendet eine absolute Deadline innerhalb einer geeigneten monotonen Clock Domain.

```text
Monotonic Time
      +
Duration
      ↓
High-Resolution Deadline
```

Wall-Clock-Korrekturen dürfen die Deadline nicht beeinflussen.

## Auflösung

NovaOS unterscheidet:

```text
Requested Resolution
Effective Resolution
Hardware Resolution
Observed Accuracy
```

Eine angeforderte Nanosekundenauflösung bedeutet nicht, dass Hardware oder Scheduler eine entsprechende Genauigkeit garantieren.

## Expiration

```text
Deadline reached
      ↓
Timer Expiration
      ↓
Task/Event becomes eligible
      ↓
Scheduler
      ↓
Execution
```

Die Differenz zwischen Deadline und tatsächlicher Ausführung muss messbar sein können.

## Periodische Timer

Periodische High-Resolution Timer sollen ihre Deadlines aus einer festen Basis ableiten:

```text
Deadline[n] = Start + n × Period
```

Dadurch wird kumulative Drift vermieden.

## Toleranz

Auch High-Resolution Timer dürfen eine explizite Toleranz besitzen.

```text
Deadline ± Allowed Tolerance
```

Nur Timer ohne zulässige Verschiebung müssen mit maximal verfügbarer zeitlicher Präzision geplant werden.

## Energie

High-Resolution Timer können tiefe CPU- und System-Idle-Zustände verhindern.

NovaOS darf deshalb unnötig präzise Timer begrenzen, zusammenfassen oder auf gröbere Timer abbilden, sofern deren Execution Contract dies erlaubt.

```text
Precision Requirement
        ↕
Energy Cost
```

Hard Deadlines besitzen Vorrang vor Energieoptimierung.

## Cancellation

High-Resolution Timer müssen sicher abbrechbar sein.

Cancellation und gleichzeitig eintretende Expiration müssen race-sicher behandelt werden.

## Fallback

Ist keine geeignete High-Resolution-Hardware verfügbar:

```text
High-Resolution Request
        ↓
Best Available Timer
        ↓
Effective Resolution Reported
```

NovaOS darf keine höhere Genauigkeit vortäuschen als tatsächlich verfügbar ist.

## Normative Anforderungen

1. NovaOS MUSS High-Resolution Timer unterstützen können.
2. High-Resolution Timer MÜSSEN von Hardware-Timern abstrahiert bleiben.
3. High-Resolution Timer SOLLEN monotone Clock Domains verwenden.
4. Wall-Clock-Korrekturen DÜRFEN deren Deadlines nicht beeinflussen.
5. Requested und Effective Resolution MÜSSEN unterscheidbar sein.
6. Deadline, Expiration und tatsächliche Ausführung MÜSSEN getrennte Zeitpunkte bleiben.
7. Mehrere Timer MÜSSEN auf gemeinsamen Hardware-Timern multiplexbar sein.
8. Periodische Timer SOLLEN kumulative Drift vermeiden.
9. Explizite Timer-Toleranzen MÜSSEN unterstützt werden.
10. Energieoptimierung DARF Hard Deadlines nicht verletzen.
11. Fehlende Hardwareauflösung MUSS durch einen sicheren Fallback behandelbar sein.
12. TimerID, Deadline, Auflösung, Expiration, Ausführungsverzögerung und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-HIGHRES-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS stellt High-Resolution Timer für zeitkritische Aufgaben bereit, ohne logische Timer an konkrete Hardware zu koppeln. Auflösung, tatsächliche Genauigkeit und Ausführungszeit bleiben klar unterscheidbar, während Echtzeitanforderungen und Energieeffizienz kontrolliert gegeneinander abgewogen werden können.