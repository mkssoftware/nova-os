# NPSPEC-TIME-ARCH-0001 – Nova Time Architecture

## Status

Angenommen

## Kategorie

Time / Architecture

## Zweck

NovaOS definiert eine zentrale Zeitarchitektur für Zeitmessung, Zeitplanung, Zeitstempel und zeitabhängige Systemfunktionen.

Die Architektur trennt monotone Zeit, reale Kalenderzeit und Hardware-Zeitquellen voneinander, damit Zeitkorrekturen die interne Systemausführung nicht beschädigen.

## Grundprinzipien

```text
Monotonic Time ≠ Wall Clock
Wall Clock ≠ RTC
Timestamp ≠ Duration
Clock Source ≠ Timer
Timer ≠ Scheduler
Time Zone ≠ System Time
Clock Adjustment ≠ Monotonic Time Jump
```

## Architektur

```text
Hardware Clock Sources
        ↓
Timekeeping Core
   ┌────┴─────┐
   ↓          ↓
Monotonic   Wall Clock
   ↓          ↓
Timers     Calendar Time
   ↓
Scheduler / Deadlines
```

## Zeitdomänen

NovaOS unterscheidet mindestens:

```text
Monotonic Time
Boot Time
Wall Clock
UTC Time
Process / Task Time
```

### Monotonic Time

Läuft monoton vorwärts und wird für interne Zeitmessung verwendet:

```text
Timeouts
Deadlines
Durations
Scheduling
Performance Measurement
```

Änderungen der realen Uhrzeit dürfen monotone Zeit nicht rückwärts springen lassen.

### Wall Clock

Repräsentiert die reale Systemzeit und darf durch Synchronisation oder Benutzerkonfiguration korrigiert werden.

```text
Monotonic Time
      +
Wall Clock Offset
      ↓
System Wall Clock
```

## Hardware-Zeitquellen

NovaOS darf unterschiedliche Clock Sources verwenden:

```text
TSC
HPET
ACPI PM Timer
PIT
RTC
Architecture-Specific Counter
Paravirtualized Clock
Registered Clock Provider
```

Keine konkrete Hardware-Zeitquelle ist Voraussetzung der allgemeinen Time Architecture.

## Clock Source

```text
ClockSource
├── ClockSourceID
├── Frequency
├── Resolution
├── Precision
├── Stability
├── Monotonic
├── SynchronizationState
└── State
```

NovaOS darf mehrere Zeitquellen vergleichen und die geeignetste Quelle auswählen.

## Zeitkorrektur

Wall-Clock-Korrekturen können erfolgen durch:

```text
Step
Slew
Synchronization
Administrative Change
```

Interne Deadlines und Timeouts dürfen dadurch nicht unbeabsichtigt verlängert, verkürzt oder rückwärts verschoben werden.

## Timer

Timer verwenden geeignete monotone Zeitbasen:

```text
Clock Source
     ↓
Timekeeping
     ↓
Timer
     ↓
Event / Scheduler
```

Timer dürfen:

```text
One-Shot
Periodic
Deadline-Based
High Resolution
Coalesced
```

sein.

## Auflösung und Präzision

NovaOS unterscheidet:

```text
Resolution
Precision
Accuracy
Stability
```

Eine hohe nominelle Auflösung bedeutet nicht automatisch hohe Genauigkeit.

## Suspend und Resume

Zeitmessung muss System-Suspend berücksichtigen.

NovaOS muss unterscheiden können, ob eine Zeitdomäne Suspend-Zeit einschließt oder ausschließt.

```text
Monotonic Active Time
Boot Time Including Suspend
Wall Clock
```

Nach Resume müssen relevante Zeitquellen validiert und gegebenenfalls neu synchronisiert werden.

## Virtualisierung

Virtuelle oder emulierte Systeme dürfen eigene Time Provider verwenden.

Zeit darf dabei skaliert, pausiert oder von einer Host-Zeitquelle abgeleitet sein, sofern die jeweilige Zeitdomäne dies explizit beschreibt.

## Fehlerverhalten

Instabile oder fehlerhafte Clock Sources müssen erkannt werden können.

```text
Clock Failure
    ↓
Mark Degraded
    ↓
Select Alternative Source
    ↓
Recalibrate
```

Eine fehlerhafte Quelle darf nicht unkontrolliert die gesamte Systemzeit beschädigen.

## Normative Anforderungen

1. NovaOS MUSS monotone Zeit und Wall Clock getrennt behandeln.
2. Interne Deadlines SOLLEN monotone Zeit verwenden.
3. Wall-Clock-Korrekturen DÜRFEN monotone Zeit nicht rückwärts verändern.
4. Hardware-Zeitquellen MÜSSEN hinter einer gemeinsamen Abstraktion liegen.
5. Mehrere Clock Sources MÜSSEN unterstützt werden können.
6. Clock Sources MÜSSEN hinsichtlich Stabilität und Verfügbarkeit bewertbar sein.
7. Timer MÜSSEN von konkreten Hardware-Timern abstrahiert sein.
8. Resolution, Precision, Accuracy und Stability MÜSSEN getrennt behandelbar sein.
9. Suspend-Zeit MUSS je nach Zeitdomäne ein- oder ausgeschlossen werden können.
10. Fehlerhafte Clock Sources MÜSSEN ersetzt oder deaktiviert werden können.
11. Virtuelle und paravirtualisierte Zeitquellen MÜSSEN unterstützt werden können.
12. Zeitdomäne, aktive Clock Source, Auflösung, Korrekturen und Synchronisationszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine hardwareunabhängige Zeitarchitektur, die monotone Systemzeit, reale Kalenderzeit, Hardware-Zeitquellen und Timer klar voneinander trennt. Dadurch bleiben Deadlines, Timeouts und Scheduling stabil, selbst wenn die reale Uhrzeit korrigiert wird, Hardware-Zeitquellen wechseln oder das System Suspend- und Resume-Zyklen durchläuft.