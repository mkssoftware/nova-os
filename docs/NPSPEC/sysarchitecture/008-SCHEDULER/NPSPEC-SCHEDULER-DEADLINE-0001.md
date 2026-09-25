# NPSPEC-SCHEDULER-DEADLINE-0001 – Nova Deadline Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Deadline / Realtime / Execution

## Zweck

NovaOS definiert Deadline Scheduling für Tasks mit expliziten zeitlichen Anforderungen.

```text
Runnable Tasks
      ↓
Deadline Evaluation
      ↓
Scheduler
      ↓
CPU Allocation
      ↓
Completion / Deadline Miss
```

Ziel ist, zeitkritische Arbeit anhand ihrer tatsächlichen zeitlichen Anforderungen statt ausschließlich anhand statischer Prioritäten zu planen.

## Grundprinzip

Tasks mit Deadline werden anhand ihrer verbleibenden Zeit und Ausführungsanforderungen bewertet.

```text
Current Time
     +
Deadline
     +
Required Runtime
     ↓
Scheduling Decision
```

Dabei gilt:

```text
Deadline ≠ Priority
```

Eine hohe Priorität ersetzt keine explizite Deadline.

## Deadline-Information

Der Scheduler berücksichtigt mindestens:

```text
TaskID
Deadline
DeadlineClass
EstimatedRuntime
RemainingRuntime
ExecutionContract
ResourceRequirements
```

Deadline-Klassen werden gemäß dem Concurrency-Modell unterschieden:

```text
Hard
Firm
Soft
```

## Scheduling

Deadline-fähige Tasks werden nur berücksichtigt, wenn sie ausführbar sind.

```text
Ready Tasks
    ↓
Hard Constraint Filter
    ↓
Deadline Evaluation
    ↓
Select Task
```

Eine mögliche Policy ist die bevorzugte Ausführung der Task mit der nächsten relevanten Deadline.

Die konkrete Scheduling-Policy bleibt austauschbar.

## Feasibility

Vor Annahme harter Deadline-Anforderungen soll NovaOS prüfen, ob deren Einhaltung realistisch garantiert werden kann.

```text
Deadline
+
Runtime Estimate
+
Available CPU
+
Resource Reservations
      ↓
Feasibility Check
```

Nicht erfüllbare harte Deadlines dürfen nicht stillschweigend akzeptiert werden.

## Runtime-Schätzung

Deadline Scheduling kann Schätzungen der benötigten Ausführungszeit verwenden.

```text
Estimated Runtime
Remaining Runtime
Observed Runtime
```

Schätzungen dürfen durch Laufzeitmessungen verbessert werden.

Bei Hard-Realtime dürfen unsichere adaptive Schätzungen keine garantierten Worst-Case-Grenzen ersetzen.

## Preemption

Ein Task mit dringenderer Deadline darf einen aktuell laufenden Task verdrängen, sofern dies zulässig ist.

```text
Running Task
     ↓
Earlier Deadline Ready
     ↓
Preempt
     ↓
Deadline Task
```

Preemption-Kosten müssen berücksichtigt werden.

## Ressourcenreservierung

Hard-Deadline-Tasks können Ressourcenreservierungen benötigen.

```text
CPU Time
Memory
I/O
Device Access
Bandwidth
```

Eine CPU-Zusage allein genügt nicht, wenn andere benötigte Ressourcen die Deadline verhindern können.

## Multiprocessing

Auf mehreren CPUs muss Deadline Scheduling geeignete Ziel-CPUs bestimmen.

```text
Deadline Task
      ↓
Affinity + Capacity + Locality
      ↓
Target CPU
```

Unnötige Migrationen sollen vermieden werden.

Harte CPU-Affinitäten müssen eingehalten werden.

## Deadline Miss

Wird eine Deadline nicht eingehalten:

```text
Deadline Miss
     ↓
Miss Policy
     ↓
Cancel / Fail / Continue / Degrade / Escalate
```

Die Reaktion richtet sich nach Deadline-Klasse und Execution Contract.

Ein Deadline Miss muss explizit erkennbar sein.

## Fairness

Deadline Scheduling darf normale Fairness temporär überstimmen, wenn dies zur Einhaltung zulässiger zeitlicher Anforderungen notwendig ist.

```text
Hard Deadline
     ↓
Fairness
```

Normale Tasks dürfen dadurch jedoch nicht unbegrenzt verdrängt werden.

## Determinismus

Deterministische Ausführung muss mit Deadline Scheduling kombinierbar sein.

Wenn ein Execution Contract beides fordert, muss die verwendete Scheduling-Policy beide Hard Requirements erfüllen.

## Normative Anforderungen

1. NovaOS MUSS Deadline Scheduling für Tasks mit zeitlichen Anforderungen unterstützen.
2. Deadline und Scheduling-Priorität MÜSSEN getrennte Konzepte bleiben.
3. Hard-, Firm- und Soft-Deadlines MÜSSEN unterscheidbar sein.
4. Der Scheduler MUSS nur ausführbare Tasks für Deadline Scheduling berücksichtigen.
5. Harte Constraints MÜSSEN vor Deadline-Optimierungen geprüft werden.
6. Hard Deadlines SOLLEN vor Annahme auf Erfüllbarkeit geprüft werden.
7. Nicht erfüllbare Hard Deadlines DÜRFEN NICHT stillschweigend garantiert werden.
8. Ressourcenreservierungen MÜSSEN bei garantierten Deadlines berücksichtigt werden.
9. Harte CPU-Affinitäten MÜSSEN eingehalten werden.
10. Deadline Misses MÜSSEN eindeutig erkannt und behandelt werden.
11. Hard-Realtime-Garantien DÜRFEN NICHT allein auf adaptiven Laufzeitschätzungen beruhen.
12. Deadline-Scheduling-Entscheidungen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-FAIR-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `ADR-SCHED-0004`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`

## Ergebnis

```text
Deadline Task
      ↓
Feasibility + Constraints
      ↓
Deadline Scheduling
      ↓
Resource-Aware Execution
      ↓
Complete
   or
Defined Deadline Miss
```

NovaOS erhält damit ein Deadline-bewusstes Scheduling-Modell, das zeitkritische Tasks kontrolliert plant und dabei Ressourcen, Affinität, Determinismus und Realtime-Anforderungen berücksichtigt.