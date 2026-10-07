# NPSPEC-TIME-DEADLINE-0001 – Nova Deadline

## Status

Angenommen

## Kategorie

Time / Deadline

## Zweck

NovaOS definiert Deadlines als absolute Zeitpunkte innerhalb einer eindeutig bestimmten Clock Domain.

Deadlines bilden die gemeinsame Grundlage für Timer, Scheduling, Timeouts, Echtzeitaufgaben, Execution Contracts und zeitgebundene Systemoperationen.

## Grundprinzipien

```text
Deadline ≠ Duration
Deadline ≠ Timeout
Deadline ≠ Timer
Deadline ≠ Execution Time
Deadline ≠ Wall Clock Time
Reached Deadline ≠ Immediate Execution
```

## Modell

```text
Deadline
├── DeadlineID
├── ClockDomainID
├── TargetTime
├── Class
├── Tolerance
├── OwnerID
├── ExecutionContractID
└── State
```

## Absolute Darstellung

Intern sollen Deadlines bevorzugt als absolute Zeitpunkte dargestellt werden:

```text
Deadline = ClockDomain.Now + Duration
```

Danach bleibt die Deadline unabhängig davon, wann sie erneut ausgewertet wird.

Dies verhindert Fehler durch wiederholte relative Berechnungen.

## Deadline-Klassen

NovaOS unterscheidet mindestens:

```text
Hard
Firm
Soft
Advisory
```

### Hard

Eine Verletzung ist für die Operation nicht akzeptabel.

### Firm

Ein Ergebnis nach Ablauf der Deadline besitzt keinen oder stark reduzierten Nutzen.

### Soft

Eine Überschreitung ist zulässig, soll jedoch vermieden werden.

### Advisory

Die Deadline dient primär als Optimierungs- oder Planungshinweis.

## Clock Domain

Jede Deadline gehört genau zu einer Clock Domain.

```text
Deadline
   ↓
ClockDomainID
   ↓
Comparable Time
```

Deadlines verschiedener Clock Domains dürfen nicht ohne definierte Konvertierung direkt verglichen werden.

Für interne Laufzeit-Deadlines sollen monotone Domains verwendet werden.

## Deadline-Verarbeitung

```text
Deadline Created
      ↓
Registered
      ↓
Pending
      ↓
Reached
      ↓
Eligible
      ↓
Executed / Missed / Cancelled
```

Das Erreichen der Deadline bedeutet nicht, dass die zugehörige Arbeit exakt zu diesem Zeitpunkt ausgeführt wird.

## Deadline Miss

NovaOS muss erkennen können:

```text
ActualTime > Deadline
```

Dabei sollen mindestens erfasst werden:

```text
Deadline
Actual Execution Time
Lateness
Cause
Owner
Resource Context
```

Ein Deadline Miss darf abhängig von Klasse und Execution Contract unterschiedliche Reaktionen auslösen.

## Scheduling

Der Scheduler darf Deadlines für zeitkritische Aufgaben berücksichtigen:

```text
Runnable Tasks
      ↓
Deadline Evaluation
      ↓
Scheduling Decision
```

Eine Deadline allein garantiert jedoch keine Ressourcen oder Ausführungszeit.

Ressourcenanforderungen werden über Execution Contracts und Scheduling-Policy behandelt.

## Timer

Timer dürfen Deadlines verwenden, um ihren Auslösezeitpunkt festzulegen:

```text
Deadline
   ↓
Timer Queue
   ↓
Clock Event
   ↓
Expiration
```

Timer und Deadline bleiben getrennte Konzepte.

## Coalescing

Nur Deadlines mit expliziter Toleranz dürfen zeitlich verschoben werden.

Hard Deadlines ohne Toleranz dürfen nicht zugunsten von Timer-Coalescing oder Energieoptimierung verschoben werden.

## Cancellation

Noch nicht erreichte Deadlines müssen kontrolliert zurückziehbar sein, wenn die zugehörige Operation abgebrochen wird.

Verwaiste Deadlines dürfen keine unnötigen Wakeups verursachen.

## Normative Anforderungen

1. NovaOS MUSS Deadlines als eigenständiges Zeitkonzept behandeln.
2. Jede Deadline MUSS einer Clock Domain zugeordnet sein.
3. Interne Laufzeit-Deadlines SOLLEN monotone Zeit verwenden.
4. Deadlines SOLLEN intern absolut dargestellt werden.
5. Hard, Firm, Soft und Advisory Deadlines MÜSSEN unterscheidbar sein.
6. Deadline und tatsächliche Ausführungszeit MÜSSEN getrennt bleiben.
7. Deadline Misses MÜSSEN erkennbar sein.
8. Deadlines unterschiedlicher Clock Domains DÜRFEN nicht implizit verglichen werden.
9. Timer und Scheduler MÜSSEN dieselbe Deadline-Semantik verwenden können.
10. Coalescing DARF nur innerhalb expliziter Toleranzen erfolgen.
11. Abgebrochene Operationen MÜSSEN zugehörige Deadlines freigeben können.
12. DeadlineID, ClockDomainID, Zielzeit, Klasse, Toleranz, Zustand und Miss-Informationen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-HIGHRESTIMER-0001`
- `NPSPEC-TIME-TICKLESS-0001`
- `NPSPEC-TIME-COALESCING-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine einheitliche Deadline-Semantik für Timer, Scheduler, Echtzeitaufgaben und Execution Contracts. Absolute Deadlines sind eindeutig an Clock Domains gebunden, können nach Kritikalität klassifiziert und überwacht werden und bleiben klar von Dauer, Timer-Expiration und tatsächlicher Ausführung getrennt.