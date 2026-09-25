# NPSPEC-CONCURRENCY-WORKSTEALING-0001 – Nova Work Stealing

## Status

Angenommen

## Kategorie

Concurrency / Scheduling / Task Distribution / Multiprocessing

## Zweck

NovaOS definiert Work Stealing als Mechanismus zur dynamischen Verteilung ausführbarer Tasks zwischen CPUs oder Worker-Kontexten.

```text
CPU 0 Queue        CPU 1 Queue
 Task A             Task D
 Task B             Empty
 Task C               │
   │                  ↓
   └────────────→ Steal Task
```

Ziel ist eine bessere Ressourcenauslastung ohne zentrale globale Task Queue als zwingenden Engpass.

## Grundprinzip

Jeder Worker kann bevorzugt eigene Tasks verarbeiten.

```text
Local Queue
    ↓
Execute Local Work
    ↓
Queue Empty?
    ↓
Steal Eligible Work
```

Work Stealing ist eine Scheduling-Optimierung und darf harte Ausführungsanforderungen nicht verletzen.

## Worker-Modell

Ein Worker repräsentiert einen Scheduling-Kontext, typischerweise auf:

```text
CPU
Core
Hardware Thread
Worker Thread
```

Jeder Worker kann eine lokale Queue ausführbarer Tasks besitzen.

## Lokale Queues

Neu erzeugte Tasks können zunächst lokal eingeordnet werden.

```text
Worker
  ↓
Local Task Queue
```

Lokale Verarbeitung soll Cache-Lokalität fördern und globale Synchronisation reduzieren.

Die konkrete Queue-Struktur ist Implementierungsdetail.

## Stealing

Hat ein Worker keine geeignete lokale Arbeit, darf er Tasks von einem anderen Worker übernehmen.

```text
Idle Worker
    ↓
Select Victim
    ↓
Find Stealable Task
    ↓
Transfer
    ↓
Execute
```

Nur Tasks, deren Constraints eine Migration erlauben, dürfen gestohlen werden.

## Steal Eligibility

Vor einer Übernahme müssen mindestens relevante Anforderungen geprüft werden:

```text
CPU Affinity
NUMA Constraints
Deadline
Priority
Execution Contract
Realtime Requirements
Resource Availability
Task State
```

Dabei gilt:

```text
Stealable ≠ Runnable Everywhere
```

## Lokalität

Work Stealing soll Lokalität berücksichtigen.

Bevorzugte Reihenfolge kann beispielsweise sein:

```text
Same Core Group
    ↓
Same NUMA Node
    ↓
Remote NUMA Node
```

Remote Stealing darf erfolgen, wenn der erwartete Nutzen die zusätzlichen Kosten rechtfertigt.

## NUMA

Auf NUMA-Systemen müssen Speicherlokalität und Zugriffskosten berücksichtigt werden.

```text
Task Memory @ Node 0
        ↓
Prefer CPU @ Node 0
```

Eine gleichmäßige CPU-Auslastung darf nicht automatisch wichtiger sein als Datenlokalität.

## Affinity

Tasks mit harter CPU-Affinität dürfen nur innerhalb ihres zulässigen CPU-Sets gestohlen werden.

```text
AllowedCPUs = {2,3}

CPU 0 → Steal prohibited
CPU 3 → Steal allowed
```

Soft Affinity darf bei Bedarf zugunsten besserer Auslastung verletzt werden.

## Realtime und Determinismus

Work Stealing darf für deterministische oder harte Realtime-Ausführung eingeschränkt oder deaktiviert werden.

```text
Hard Realtime
Deterministic Execution
Fixed Affinity
      ↓
Restricted Stealing
```

Deterministic Mode muss reproduzierbares Scheduling ermöglichen, wenn dies durch den Execution Contract verlangt wird.

## Überlastung

Work Stealing dient dem Ausgleich ungleich verteilter ausführbarer Arbeit.

```text
Worker A: ████████
Worker B: ██
Worker C:
Worker D: █

        ↓

Controlled Redistribution
```

Es ersetzt jedoch keine globale Ressourcen- oder Admission-Control-Policy.

## Adaptive Steuerung

NovaOS darf Stealing-Verhalten anhand aktueller Systembedingungen anpassen.

Berücksichtigt werden können:

```text
CPU Load
Queue Length
NUMA Distance
Cache Locality
Migration Cost
Energy State
Deadline Pressure
```

Adaptive Entscheidungen dürfen Hard Constraints nicht überschreiben.

## Normative Anforderungen

1. NovaOS SOLL Work Stealing für geeignete Task-Workloads unterstützen.
2. Worker SOLLEN lokale Task Queues verwenden können.
3. Ein Worker DARF Arbeit anderer Worker übernehmen, wenn seine eigene geeignete Arbeit erschöpft ist.
4. Nur migrationsfähige Tasks DÜRFEN gestohlen werden.
5. Harte CPU-Affinitäten MÜSSEN eingehalten werden.
6. NUMA- und Cache-Lokalität SOLLEN bei Steal-Entscheidungen berücksichtigt werden.
7. Execution Contracts MÜSSEN bei Task-Übernahmen eingehalten werden.
8. Hard-Realtime-Anforderungen DÜRFEN durch Work Stealing NICHT verletzt werden.
9. Deterministic Mode MUSS Work Stealing kontrollieren oder deaktivieren können.
10. Work Stealing DARF NICHT zu unkontrollierter Task-Migration führen.
11. Adaptive Optimierung DARF harte Constraints NICHT überschreiben.
12. Steal-Entscheidungen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-STRUCTURED-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `ADR-CONCURRENCY-0007`

## Ergebnis

```text
Local Task Queues
       ↓
Idle Worker
       ↓
Constraint-Aware Stealing
       ↓
Locality-Aware Redistribution
       ↓
Balanced Execution
```

NovaOS erhält damit einen skalierbaren Mechanismus zur dynamischen Task-Verteilung, der freie Rechenkapazität nutzt und gleichzeitig Affinität, NUMA-Lokalität, Determinismus und Realtime-Anforderungen respektiert.