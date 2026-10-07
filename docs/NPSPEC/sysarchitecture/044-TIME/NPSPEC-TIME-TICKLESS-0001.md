# NPSPEC-TIME-TICKLESS-0001 – Nova Tickless Timekeeping

## Status

Angenommen

## Kategorie

Time / Tickless

## Zweck

NovaOS verwendet eine tickless Zeit- und Timer-Architektur. Periodische Timer-Interrupts dürfen nicht erforderlich sein, wenn keine zeitabhängige Arbeit ansteht.

Statt eines festen System-Ticks wird das nächste tatsächlich relevante Zeitereignis programmiert.

## Grundprinzipien

```text
Tickless ≠ Timerless
Tickless ≠ Interruptless
Tickless ≠ No Scheduling
System Time ≠ Periodic Tick Counter
Idle ≠ Periodic Wakeup
```

## Architektur

```text
Clock Source
     ↓
Timekeeping Core
     ↓
Timer / Scheduler Deadlines
     ↓
Next Relevant Deadline
     ↓
Clock Event Provider
     ↓
One-Shot Hardware Timer
```

Die aktuelle Zeit wird aus der Clock Source bestimmt und nicht durch das Zählen periodischer Interrupts erzeugt.

## Deadline-basierter Betrieb

NovaOS ermittelt das nächste relevante Zeitereignis aus:

```text
Timers
Scheduler Deadlines
Realtime Deadlines
Timeouts
Wake Leases
Maintenance Events
Execution Contracts
```

Anschließend wird nur die früheste notwendige Deadline programmiert.

```text
D1 ───────────────┐
D2 ──────────┐    │
D3 ───────────────────┐
           ↓
     Earliest Deadline
           ↓
     Hardware Timer
```

## Idle

Wenn keine Deadline unmittelbar ansteht, darf NovaOS den Prozessor oder das System in geeignete Idle-Zustände versetzen.

```text
No Pending Work
      ↓
Next Deadline
      ↓
Select Idle State
      ↓
Sleep
      ↓
Timer Event
      ↓
Resume Execution
```

Dadurch werden unnötige Wakeups vermieden.

## Scheduler

Der Scheduler darf keine feste Tick-Frequenz für grundlegende Zeitmessung oder Scheduling-Korrektheit voraussetzen.

Zeitscheiben und Scheduling-Ereignisse werden als Deadlines programmiert.

```text
Task Runtime
     ↓
Scheduling Deadline
     ↓
Timer Event
     ↓
Reschedule
```

## Timer-Coalescing

Kompatible Timer dürfen innerhalb ihrer zulässigen Toleranzen zusammengelegt werden.

```text
Timer A ─┐
Timer B ─┼─→ Combined Wakeup
Timer C ─┘
```

Hard Deadlines dürfen dabei nicht verletzt werden.

## High-Resolution Timer

High-Resolution Timer bleiben mit tickless Betrieb kompatibel.

Die nächste präzise Deadline darf direkt auf einen geeigneten Clock Event Provider programmiert werden.

## Periodische Aufgaben

Periodische Aufgaben benötigen keinen periodischen globalen System-Tick.

Ihre nächste Deadline wird individuell berechnet:

```text
Deadline[n] = Start + n × Period
```

## Energieintegration

Tickless Betrieb unterstützt:

```text
CPU Idle
Deep Idle
Device Runtime Power Management
Low Power Idle
Energy Budgets
Thermal Management
```

Je länger die nächste Deadline entfernt ist, desto tiefere Energiesparzustände können grundsätzlich gewählt werden.

## Fallback

Falls Hardware nur eingeschränkte Timermechanismen unterstützt, darf NovaOS einen kompatiblen periodischen oder gröberen Clock Event Provider verwenden.

```text
Preferred:
Tickless One-Shot

Fallback:
Periodic Hardware Tick
```

Der Fallback darf die logische Timer-Semantik nicht verändern.

## Normative Anforderungen

1. NovaOS SOLL im Normalbetrieb tickless arbeiten.
2. Systemzeit DARF nicht vom Zählen periodischer Timer-Interrupts abhängig sein.
3. Timer und Scheduler SOLLEN deadline-basiert arbeiten.
4. Die nächste relevante Deadline SOLL direkt programmierbar sein.
5. Der Scheduler DARF keinen festen System-Tick für seine Korrektheit benötigen.
6. Periodische Aufgaben MÜSSEN ohne globalen periodischen Tick funktionieren.
7. Timer-Coalescing MUSS mit tickless Betrieb kombinierbar sein.
8. Hard Deadlines DÜRFEN durch Coalescing nicht verletzt werden.
9. High-Resolution Timer MÜSSEN im tickless Betrieb unterstützt werden.
10. Idle-Zustände SOLLEN bis zur nächsten relevanten Deadline nutzbar sein.
11. Hardware ohne geeigneten One-Shot-Timer MUSS einen sicheren Fallback verwenden können.
12. Nächste Deadline, programmierter Clock Event und tatsächlicher Wakeup MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-HIGHRESTIMER-0001`
- `NPSPEC-POWER-CPUIDLE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS benötigt keinen permanenten periodischen System-Tick. Timer, Scheduler und zeitabhängige Systemfunktionen arbeiten mit konkreten Deadlines, sodass Prozessor und Plattform zwischen relevanten Ereignissen möglichst lange in geeigneten Idle-Zuständen verbleiben können.