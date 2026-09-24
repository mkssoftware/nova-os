# NPSPEC-CONCURRENCY-DEADLINE-0001 – Nova Task Deadlines

## Status

Entwurf

## Kategorie

Concurrency / Deadline / Structured Concurrency / Realtime

## Zweck

NovaOS definiert Deadlines als explizite zeitliche Anforderungen für Tasks und Task Groups.

```text
Task
  ↓
Deadline
  ↓
Scheduler / Resource Economy
  ↓
Execution
  ↓
Complete / Deadline Miss
```

## Grundprinzip

Eine Deadline beschreibt den Zeitpunkt, bis zu dem eine Operation abgeschlossen sein soll oder muss.

```text
Start Time
    ↓
Execution
    ↓
Deadline
```

Dabei gilt:

```text
Deadline ≠ Priority
```

Eine hohe Priorität kann zur Einhaltung einer Deadline beitragen, ersetzt sie aber nicht.

## Deadline-Modell

Eine Deadline beschreibt mindestens:

```text
DeadlineTime
DeadlineClass
CancellationPolicy
MissPolicy
```

NovaOS unterscheidet:

```text
Hard
Firm
Soft
```

### Hard

Eine Überschreitung ist nicht zulässig.

### Firm

Ein Ergebnis nach der Deadline besitzt keinen oder nur noch begrenzten Nutzen.

### Soft

Eine Überschreitung verschlechtert die Qualität, bleibt aber grundsätzlich verwendbar.

## Absolute und relative Deadlines

Deadlines können angegeben werden als:

```text
Absolute Deadline
```

oder:

```text
Relative Deadline
```

Beispiel:

```text
Start + 5 ms
```

Intern muss eine geeignete monotone Zeitbasis verwendet werden.

## Task-Hierarchie

Deadlines müssen mit Structured Concurrency kompatibel sein.

```text
Parent Deadline
      ↓
Child Tasks
```

Ein Child Task darf keine effektive Deadline erhalten, die eine harte Parent-Deadline unmöglich macht.

Typisch gilt:

```text
Effective Deadline =
min(Task Deadline, Parent Deadline)
```

## Task Groups

Eine Task Group kann eine gemeinsame Deadline besitzen.

```text
Task Group Deadline
        ↓
Task A
Task B
Task C
```

Die Group Policy bestimmt, welche Tasks bis zur Deadline abgeschlossen sein müssen.

## Scheduling

Der Scheduler darf Deadline-Informationen zur Ausführungsplanung verwenden.

Berücksichtigt werden können:

```text
Deadline
Estimated Runtime
Priority
CPU Availability
Resource Budget
Dependencies
```

Eine Deadline garantiert allein noch keine rechtzeitige Ausführung.

## Ressourcen

Vor Annahme harter Deadlines soll geprüft werden, ob die notwendigen Ressourcen verfügbar sind.

```text
Deadline Requirement
        +
Resource Requirement
        ↓
Feasibility Check
```

Nicht erfüllbare harte Anforderungen dürfen nicht stillschweigend akzeptiert werden.

## Deadline Miss

Wird eine Deadline überschritten:

```text
Deadline Miss
     ↓
Miss Policy
     ↓
Continue / Cancel / Fail / Degrade / Escalate
```

Die Reaktion hängt von Deadline-Klasse und Execution Contract ab.

## Cancellation

Eine Deadline darf mit Cancellation verbunden werden.

```text
Deadline Reached
      ↓
Cancellation Request
      ↓
Structured Cleanup
```

Eine Deadline darf keine unkontrollierte sofortige Terminierung verursachen.

## Realtime

Für Hard-Realtime-Ausführung müssen zusätzliche Garantien gelten.

```text
Deadline
+
Bounded Latency
+
Resource Reservation
+
Deterministic Execution
=
Realtime Guarantee
```

Normale Task-Deadlines dürfen nicht automatisch als Hard-Realtime-Garantie interpretiert werden.

## Normative Anforderungen

1. NovaOS MUSS Deadlines für Tasks und Task Groups unterstützen.
2. Deadlines MÜSSEN von Scheduling-Prioritäten getrennt behandelt werden.
3. Hard-, Firm- und Soft-Deadlines MÜSSEN unterscheidbar sein.
4. Deadline-Berechnungen MÜSSEN eine geeignete monotone Zeitbasis verwenden.
5. Parent-Deadlines MÜSSEN bei Child Tasks berücksichtigt werden.
6. Child Tasks DÜRFEN harte Parent-Deadlines nicht abschwächen.
7. Harte Deadlines SOLLEN vor Annahme auf Erfüllbarkeit geprüft werden.
8. Nicht erfüllbare Hard Requirements DÜRFEN NICHT stillschweigend akzeptiert werden.
9. Deadline Misses MÜSSEN eindeutig erkennbar sein.
10. Deadline Miss Policies MÜSSEN kontrollierte Reaktionen definieren können.
11. Deadline-basierte Cancellation MUSS das normale Cancellation-Modell verwenden.
12. Deadline-Zustand und Deadline Misses SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-STRUCTURED-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-CONCURRENCY-0005`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`

## Ergebnis

```text
Task
  ↓
Explicit Deadline
  ↓
Feasibility + Scheduling
  ↓
Execution
  ↓
Complete
   or
Deadline Miss → Defined Policy
```

NovaOS erhält damit ein explizites Deadline-Modell, das zeitliche Anforderungen in Structured Concurrency, Scheduling, Ressourcenplanung und Realtime-Ausführung integrieren kann.