# NPSPEC-TIME-COALESCING-0001 – Nova Timer Coalescing

## Status

Angenommen

## Kategorie

Time / Timer Coalescing

## Zweck

NovaOS darf zeitlich nahe Timer innerhalb ihrer zulässigen Toleranzen zu gemeinsamen Wakeups zusammenfassen.

Ziel ist die Reduzierung unnötiger Interrupts und Wakeups, ohne Deadlines oder zeitkritische Anforderungen zu verletzen.

## Grundprinzipien

```text
Coalescing ≠ Delay without Limit
Tolerance ≠ Deadline
Coalescing ≠ Loss of Timer
Hard Deadline ≠ Flexible Deadline
Energy Optimization < Timing Constraint
```

## Modell

```text
TimerCoalescing
├── TimerID
├── Deadline
├── Tolerance
├── EarliestTime
├── LatestTime
├── Priority
├── ClockDomainID
└── CoalescingState
```

## Coalescing Window

Ein Timer darf ein explizites Zeitfenster definieren:

```text
EarliestTime ≤ ExecutionWindow ≤ LatestTime
```

Beispiel:

```text
Timer A ───────┐
Timer B ─────┐ │
Timer C ────────┐
               ↓
        Common Wakeup
```

Nur Timer mit kompatiblen Zeitfenstern dürfen zusammengelegt werden.

## Toleranz

Timer können unterschiedliche Toleranzklassen besitzen:

```text
Exact
Low
Normal
Relaxed
Background
```

Die tatsächliche Toleranz wird als konkrete Zeitspanne behandelt.

`Exact` bedeutet, dass kein absichtliches Coalescing über die definierte Deadline hinaus erlaubt ist.

## Entscheidungsmodell

```text
Pending Timers
      ↓
Determine Windows
      ↓
Find Compatible Timers
      ↓
Check Constraints
      ↓
Select Common Wakeup
      ↓
Program Clock Event
```

Berücksichtigt werden mindestens:

```text
Deadline
Tolerance
Execution Contract
Priority
Realtime Requirements
Power State
Energy Policy
Clock Domain
```

## Hard Deadlines

Hard Deadlines besitzen Vorrang.

```text
Coalescing Window
      ↓
Hard Deadline reached
      ↓
STOP
```

NovaOS darf eine Hard Deadline niemals absichtlich überschreiten, um Energie zu sparen.

## Tickless Integration

Timer Coalescing ist direkt mit dem tickless Betrieb verbunden.

```text
Timer Queue
    ↓
Coalescing
    ↓
Next Effective Deadline
    ↓
One-Shot Clock Event
```

Dadurch können mehrere geplante Wakeups durch einen gemeinsamen Wakeup ersetzt werden.

## High-Resolution Timer

High-Resolution Timer dürfen ebenfalls zusammengelegt werden, sofern sie eine explizite Toleranz besitzen.

```text
High Resolution ≠ Zero Tolerance
```

Ein High-Resolution Timer ohne zulässige Verschiebung darf nicht absichtlich verzögert werden.

## Periodische Timer

Periodische Timer behalten ihre ursprüngliche Zeitbasis.

Coalescing darf keine kumulative Drift erzeugen:

```text
Deadline[n] = Start + n × Period
```

Die nächste Periode wird nicht vom tatsächlich zusammengelegten Wakeup abgeleitet.

## Energieintegration

Coalescing darf aggressiver eingesetzt werden bei:

```text
Battery Saver
Low Power Idle
Background Work
Energy Budget Pressure
Deep CPU Idle
```

Dabei bleiben harte Zeitbedingungen unverändert.

## Dynamische Anpassung

NovaOS darf Coalescing abhängig vom Systemzustand dynamisch anpassen.

```text
Performance Mode → tighter windows
Balanced Mode    → normal windows
Energy Saving    → broader allowed windows
```

Nur bereits erlaubte Toleranzen dürfen genutzt werden. Die Policy darf keine vom Owner gesetzte harte Grenze erweitern.

## Normative Anforderungen

1. NovaOS MUSS Timer Coalescing unterstützen können.
2. Jeder zusammenlegbare Timer MUSS eine zulässige Toleranz besitzen.
3. Timer DÜRFEN nur innerhalb ihrer erlaubten Zeitfenster verschoben werden.
4. Hard Deadlines DÜRFEN nicht durch Coalescing verletzt werden.
5. Timer unterschiedlicher Clock Domains DÜRFEN nicht ohne definierte Domain-Konvertierung zusammengelegt werden.
6. Periodische Timer DÜRFEN durch Coalescing keine kumulative Drift erhalten.
7. High-Resolution Timer MÜSSEN ihre Toleranz explizit angeben können.
8. Coalescing SOLL mit der tickless Timer-Infrastruktur integriert sein.
9. Energiepolitik DARF Coalescing innerhalb bestehender Grenzen beeinflussen.
10. Coalescing DARF keine zusätzliche Authority erzeugen.
11. Nicht kompatible Timer MÜSSEN getrennt geplant werden.
12. Ursprüngliche Deadline, Toleranz, effektiver Wakeup und Coalescing-Entscheidung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-HIGHRESTIMER-0001`
- `NPSPEC-TIME-TICKLESS-0001`
- `NPSPEC-POWER-POLICY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann kompatible Timer zu gemeinsamen Wakeups bündeln und dadurch Interrupts sowie Energieverbrauch reduzieren. Jede Zusammenlegung bleibt innerhalb expliziter Toleranzen, während Hard Deadlines, periodische Zeitbasen und zeitkritische Anforderungen erhalten bleiben.