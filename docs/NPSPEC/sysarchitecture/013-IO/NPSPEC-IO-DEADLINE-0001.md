# NPSPEC-IO-DEADLINE-0001 – Nova I/O Deadline

## Status

Angenommen

## Kategorie

I/O / Deadline / Scheduling / Realtime

## Zweck

NovaOS definiert ein einheitliches Deadline-Modell für I/O-Operationen.

```text
IORequest
   ↓
Deadline
   ↓
I/O Scheduler
   ↓
Provider
   ↓
Completion
```

Eine Deadline beschreibt den Zeitpunkt, bis zu dem eine I/O-Operation abgeschlossen sein soll.

## Grundprinzip

```text
Deadline ≠ Priority
Deadline ≠ Timeout
Deadline ≠ Completion Guarantee
```

Priorität beschreibt relative Dringlichkeit. Eine Deadline beschreibt eine zeitliche Grenze. Ein Timeout beschreibt, wann ein wartender Vorgang abgebrochen oder als fehlgeschlagen behandelt wird.

## Deadline-Modell

Ein `IORequest` darf enthalten:

```text
Deadline
├── Target Time
├── Requirement Type
└── Miss Policy
```

Die Deadline soll auf einer definierten NovaOS-Zeitbasis angegeben werden.

## Deadline-Typen

NovaOS muss mindestens unterscheiden können:

```text
Hard
Soft
```

### Hard Deadline

Das Überschreiten gilt als Verletzung einer harten Anforderung.

Die Operation darf abhängig von ihrer Policy:

```text
Reject
Cancel
Fail
Fallback
```

auslösen.

### Soft Deadline

Das Überschreiten ist zulässig, muss aber erkennbar sein.

Soft Deadlines dürfen zur Optimierung von Scheduling und QoS verwendet werden.

## Deadline Lifecycle

```text
Submit
  ↓
Queue
  ↓
Execution
  ↓
Completion
  ↓
Deadline Evaluation
```

Die Deadline muss auf den relevanten Abschlusszeitpunkt des Requests bezogen werden.

## Admission Control

Harte Deadlines sollen vor Annahme auf grundsätzliche Erfüllbarkeit geprüft werden.

```text
IORequest
   ↓
Deadline Check
   ↓
Accept / Reject / Degrade
```

NovaOS darf keine harte Deadline als garantiert behandeln, wenn Provider oder Ressourcen keine entsprechende Zusicherung ermöglichen.

## Scheduling

Der I/O-Scheduler muss Deadlines bei der Planung berücksichtigen können.

```text
Request A → Deadline 10 ms
Request B → Deadline 50 ms
Request C → keine Deadline
```

Dabei dürfen Security-, Safety- und harte Ressourcenregeln nicht verletzt werden.

## Deadline Miss

Eine überschrittene Deadline muss eindeutig erkennbar sein.

```text
DeadlineMiss
├── RequestID
├── Deadline
├── Completion Time
└── Overrun
```

Ein Deadline Miss ist nicht automatisch ein I/O-Fehler.

Die Operation kann technisch erfolgreich abgeschlossen worden sein, obwohl ihre zeitliche Anforderung verletzt wurde.

## Cancellation

Eine Deadline darf mit einer Cancellation-Policy verbunden werden.

```text
Deadline reached
      ↓
Cancel Request
      ↓
Cancelled / Too Late
```

NovaOS darf nicht annehmen, dass ein Request nach Erreichen der Deadline automatisch noch sicher abgebrochen werden kann.

## QoS

Deadlines sind Bestandteil der I/O-QoS-Planung.

```text
Priority
+
Deadline
+
Latency Target
+
Resource Budget
       ↓
I/O QoS
```

Der `ExecutionContract` darf bestimmen, ob eine Deadline hart oder weich ist.

## Provider

Provider müssen ihre Deadline-Fähigkeiten beschreiben können.

Beispiele:

```text
Deadline Aware
Best Effort
Bounded Latency
No Deadline Support
```

Fehlende Deadline-Unterstützung muss für höhere Schichten erkennbar sein.

## Zeitbasis

Deadline-Berechnungen müssen eine geeignete monotone Zeitbasis verwenden.

Änderungen der Kalenderzeit dürfen laufende relative I/O-Deadlines nicht unbeabsichtigt verändern.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Deadline
Deadline Type
Remaining Time
Completion Time
Deadline State
Overrun
Provider Capability
```

## Normative Anforderungen

1. NovaOS MUSS I/O-Requests mit Deadlines unterstützen können.
2. Deadline, Priorität und Timeout MÜSSEN getrennte Konzepte bleiben.
3. Harte und weiche Deadlines MÜSSEN unterscheidbar sein.
4. Deadline-Berechnungen SOLLEN eine monotone Zeitbasis verwenden.
5. Der I/O-Scheduler MUSS Deadlines berücksichtigen können.
6. Harte Deadlines SOLLEN vor Annahme auf grundsätzliche Erfüllbarkeit geprüft werden.
7. Nicht garantierbare harte Deadlines DÜRFEN NICHT stillschweigend als garantiert behandelt werden.
8. Ein Deadline Miss MUSS eindeutig erkennbar sein.
9. Ein Deadline Miss DARF NICHT automatisch mit einem I/O-Fehler gleichgesetzt werden.
10. Deadline-basierte Cancellation MUSS Race-sicher mit Completion koordiniert werden.
11. Provider SOLLEN ihre Deadline-Fähigkeiten offenlegen.
12. Deadline-Zustand und Überschreitung SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-IO-PRIORITY-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-IO-0007`

## Ergebnis

```text
IORequest
   ↓
Deadline Requirement
   ↓
Admission + Scheduling
   ↓
Provider
   ↓
Completion
   ↓
Met / Missed
```

NovaOS erhält damit ein einheitliches Deadline-Modell, das zeitkritische I/O-Anforderungen explizit beschreibt und sauber von Priorität, Timeout und technischem I/O-Erfolg trennt.