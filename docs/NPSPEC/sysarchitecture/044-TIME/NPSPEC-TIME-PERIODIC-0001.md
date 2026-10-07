# NPSPEC-TIME-PERIODIC-0001 – Nova Periodic Time

## Status

Angenommen

## Kategorie

Time / Periodic Execution

## Zweck

NovaOS definiert periodische Zeitereignisse als wiederkehrende Deadlines auf Basis eines festen Startpunkts und einer definierten Periode.

Periodische Ausführung darf nicht durch wiederholtes relatives Warten implementiert werden, da dadurch kumulative Drift entstehen kann.

## Grundprinzipien

```text
Periodic ≠ Global System Tick
Period ≠ Deadline
Period ≠ Execution Duration
Missed Period ≠ Automatic Immediate Replay
Actual Execution Time ≠ Next Time Base
```

## Modell

```text
PeriodicSchedule
├── PeriodicID
├── OwnerID
├── ClockDomainID
├── StartTime
├── Period
├── Phase
├── Tolerance
├── MissPolicy
└── State
```

## Deadline-Berechnung

Periodische Deadlines werden aus einer stabilen Zeitbasis berechnet:

```text
Deadline[n] = StartTime + Phase + n × Period
```

Die tatsächliche Ausführungszeit einer Periode darf nicht automatisch Grundlage der nächsten Deadline werden.

Nicht empfohlen:

```text
Execute
  ↓
Wait Period
  ↓
Execute
```

Bevorzugt:

```text
Fixed Time Base
      ↓
Deadline 1
Deadline 2
Deadline 3
Deadline 4
```

## Driftvermeidung

Wird eine Ausführung verspätet gestartet:

```text
Expected:  10 ── 20 ── 30 ── 40
Actual:    10 ───22 ─────31
```

bleibt die nächste Deadline weiterhin:

```text
40
```

und wird nicht automatisch auf `41` verschoben.

## Missed Periods

Werden eine oder mehrere Perioden verpasst, muss eine definierte MissPolicy bestimmen, wie fortgefahren wird.

Unterstützte Strategien können sein:

```text
Skip
CatchUp
Coalesce
ReportOnly
Cancel
```

`CatchUp` darf nicht unkontrolliert zu einer Ausführungsflut führen.

## Clock Domain

Jeder periodische Zeitplan gehört zu genau einer Clock Domain.

Für laufzeitbezogene Perioden soll eine monotone Domain verwendet werden.

Wall-Clock-basierte periodische Ereignisse müssen explizit als solche definiert werden.

## Timer-Integration

```text
Periodic Schedule
       ↓
Next Deadline
       ↓
Timer Core
       ↓
Clock Event
       ↓
Expiration
       ↓
Next Deadline
```

Ein globaler periodischer System-Tick ist dafür nicht erforderlich.

## Coalescing

Periodische Ereignisse dürfen innerhalb ihrer expliziten Toleranz zusammengelegt werden.

Coalescing darf die ursprüngliche Periodenbasis nicht verändern.

```text
Effective Wakeup ≠ New Period Origin
```

## Suspend

Das Verhalten während Suspend ergibt sich aus der verwendeten Clock Domain und der MissPolicy.

Nach Resume muss NovaOS bestimmen können, welche Perioden während Suspend verstrichen sind.

## Cancellation

Periodische Zeitpläne müssen explizit abbrechbar sein.

```text
Created
   ↓
Active
   ↓
Cancelled / Completed / Failed
```

Nach Cancellation dürfen keine weiteren Deadlines erzeugt werden.

## Normative Anforderungen

1. NovaOS MUSS periodische Zeitereignisse ohne globalen System-Tick unterstützen.
2. Periodische Ereignisse MÜSSEN einer Clock Domain zugeordnet sein.
3. Deadlines SOLLEN aus einer festen Zeitbasis berechnet werden.
4. Tatsächliche Ausführungszeiten DÜRFEN nicht automatisch die nächste Periodenbasis bestimmen.
5. Periodische Ausführung MUSS kumulative Drift vermeiden können.
6. Verpasste Perioden MÜSSEN über eine definierte MissPolicy behandelt werden.
7. `CatchUp` MUSS begrenzbar sein.
8. Coalescing DARF die ursprüngliche Periodenbasis nicht verändern.
9. Suspend-Verhalten MUSS aus Clock Domain und MissPolicy eindeutig bestimmbar sein.
10. Periodische Zeitpläne MÜSSEN sicher abbrechbar sein.
11. High-Resolution-Perioden MÜSSEN bei geeigneter Hardware unterstützt werden können.
12. PeriodicID, Startzeit, Periode, nächste Deadline, Misses und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-HIGHRESTIMER-0001`
- `NPSPEC-TIME-TICKLESS-0001`
- `NPSPEC-TIME-COALESCING-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt periodische Ereignisse als Folge absolut berechneter Deadlines statt als wiederholte relative Wartezeiten. Dadurch bleiben Perioden langfristig stabil, kumulative Drift wird vermieden und verpasste Perioden können kontrolliert behandelt werden, ohne einen globalen periodischen System-Tick zu benötigen.