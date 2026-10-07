# NPSPEC-POWER-CPUIDLE-0001 – Nova CPU Idle Management

## Status

Angenommen

## Kategorie

Power / CPU Idle

## Zweck

NovaOS definiert die energieeffiziente Behandlung inaktiver CPU-Kerne.

Wenn ein CPU-Kern keine ausführbare Arbeit besitzt, darf er kontrolliert in einen geeigneten Idle-Zustand wechseln. Die Auswahl berücksichtigt Energieeinsparung, erwartete Idle-Dauer, Wake-Latenz, Scheduler-Anforderungen und Hardwaregrenzen.

## Grundprinzipien

```text
Idle ≠ Suspend
Idle ≠ Offline
Idle State ≠ CPU Frequency
Deepest State ≠ Best State
Requested State ≠ Reached State
Idle Optimization ≠ Deadline Violation
```

## Architektur

```text
Scheduler
   ↓
CPU Idle Manager
   ↓
Idle Governor
   ↓
Platform / HAL
   ↓
CPU Idle States
```

Der Scheduler erkennt fehlende ausführbare Arbeit.

Der CPU Idle Manager entscheidet, welcher verfügbare Zustand geeignet ist.

## Idle State

```text
CPUIdleState
├── StateID
├── TargetCPU
├── ExitLatency
├── MinimumResidency
├── PowerCost
├── WakeSources
├── Constraints
└── State
```

Die konkrete Hardwareimplementierung darf architekturabhängig sein.

## Zustandsauswahl

```text
No Runnable Work
      ↓
Expected Idle Duration
      +
Exit Latency
      +
Next Deadline
      +
Power Policy
      +
Thermal State
      ↓
Idle State Selection
```

Ein tieferer Zustand darf nur gewählt werden, wenn seine erwartete Energieeinsparung die zusätzlichen Eintritts- und Aufwachkosten rechtfertigt.

## Scheduler-Integration

Der Scheduler muss dem Idle Manager mindestens relevante zeitliche Informationen bereitstellen können:

```text
Next Timer
Deadline
Runnable Work
CPU Affinity
Realtime Constraints
```

Echtzeit- und Deadline-Anforderungen begrenzen die zulässige Wake-Latenz.

## Idle-Zustände

NovaOS verwendet abstrakte Zustände:

```text
Running
Shallow Idle
Medium Idle
Deep Idle
```

Diese werden auf verfügbare Hardwarezustände abgebildet.

Auf ACPI-Systemen können darunter beispielsweise C-States liegen.

NovaOS darf jedoch nicht von ACPI oder einer bestimmten CPU-Architektur abhängig sein.

## Per-CPU und gekoppelte Zustände

Idle-Zustände dürfen gelten für:

```text
Logical CPU
Core
CPU Cluster
Package
SoC Domain
```

Gekoppelte Zustände dürfen nur aktiviert werden, wenn die Anforderungen aller betroffenen Verarbeitungseinheiten erfüllt sind.

## Wakeup

Ein Idle-Zustand muss definierte Wake-Mechanismen besitzen.

```text
Interrupt
Timer
IPI
Device Event
Platform Event
```

Nach dem Aufwachen wird der tatsächlich erreichte Zustand verlassen und die normale Scheduler-Ausführung fortgesetzt.

## Adaptive Auswahl

NovaOS darf vergangene Idle-Zeiten zur Prognose zukünftiger Idle-Intervalle verwenden.

```text
History
   +
Current Scheduler State
   ↓
Idle Prediction
```

Fehlprognosen müssen korrigierbar sein und dürfen keine Hard Constraints verletzen.

## Fehlerverhalten

Ein fehlerhafter oder nicht verfügbarer Idle-Zustand muss deaktivierbar sein.

Fallback:

```text
Selected State unavailable
        ↓
Shallower Valid State
        ↓
Safe Idle
```

## Normative Anforderungen

1. NovaOS MUSS CPU-Idle-Zustände hardwareunabhängig abstrahieren.
2. Idle und CPU-Frequenzsteuerung MÜSSEN getrennte Mechanismen bleiben.
3. Idle-State-Auswahl MUSS Wake-Latenz und erwartete Idle-Dauer berücksichtigen können.
4. Scheduler-Deadlines MÜSSEN bei der Auswahl berücksichtigt werden.
5. Hard Realtime Constraints DÜRFEN durch Idle-Optimierung nicht verletzt werden.
6. Mehrere Idle-Zustände MÜSSEN unterstützt werden können.
7. Per-CPU- und gekoppelte Idle-Zustände MÜSSEN darstellbar sein.
8. NovaOS DARF nicht von ACPI-spezifischen C-States abhängig sein.
9. Angeforderter und tatsächlich erreichter Zustand MÜSSEN unterscheidbar sein.
10. Adaptive Idle-Prognosen DÜRFEN verwendet werden.
11. Fehlerhafte Zustände MÜSSEN sicher deaktiviert und durch gültige Fallbacks ersetzt werden können.
12. Idle-State, Residency, Wake-Latenz und Auswahlentscheidung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-ACPI-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS kann inaktive CPU-Kerne automatisch in geeignete Energiesparzustände versetzen. Die Auswahl erfolgt dynamisch anhand von Idle-Dauer, Wake-Latenz, Scheduler-Anforderungen, Power Policy und Hardwaremöglichkeiten, ohne Echtzeitbedingungen oder Plattformgrenzen zu verletzen.