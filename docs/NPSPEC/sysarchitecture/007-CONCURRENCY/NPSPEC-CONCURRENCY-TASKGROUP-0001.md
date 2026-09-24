# NPSPEC-CONCURRENCY-TASKGROUP-0001 – Nova Task Groups

## Status

Entwurf

## Kategorie

Concurrency / Task Group / Structured Concurrency

## Zweck

NovaOS definiert Task Groups zur gemeinsamen Verwaltung mehrerer logisch zusammengehöriger Tasks innerhalb eines strukturierten Scopes.

```text
Task Group
 ├── Task A
 ├── Task B
 └── Task C
```

Eine Task Group ermöglicht gemeinsame Lifecycle-, Cancellation-, Fehler- und Ergebnisbehandlung.

## Grundprinzip

Mehrere Tasks, die gemeinsam eine Operation erfüllen, werden als kontrollierte Gruppe behandelt.

```text
Create Group
    ↓
Spawn Tasks
    ↓
Execute Concurrently
    ↓
Collect Results
    ↓
Complete Group
```

Die Gruppe darf ihren Scope nicht verlassen, solange ihre Tasks nicht abgeschlossen, abgebrochen oder kontrolliert übertragen wurden.

## Task Group

Eine Task Group beschreibt mindestens:

```text
TaskGroupID
Owner
ParentScope
Tasks
State
CancellationContext
```

Optional können enthalten sein:

```text
ExecutionContract
Deadline
ResourceBudget
ResultPolicy
FailurePolicy
```

## Zustände

Mindestens folgende Zustände werden unterstützt:

```text
Created
Running
Waiting
Completed
Cancelling
Cancelled
Failed
```

Der Gruppenzustand ergibt sich aus dem eigenen Lifecycle und den Zuständen ihrer Tasks.

## Task-Erzeugung

Neue Tasks werden explizit einer Gruppe zugeordnet.

```text
Task Group
    ↓
Spawn Task
    ↓
Register Child
    ↓
Schedule
```

Die Gruppe übernimmt die Lifecycle-Verantwortung für diese Tasks.

## Completion

Eine Task Group kann auf die Beendigung ihrer Tasks warten.

```text
Tasks Running
     ↓
Join
     ↓
All Required Tasks Complete
     ↓
Group Complete
```

Die Completion-Policy kann beispielsweise definieren:

```text
WaitAll
FirstSuccess
FirstResult
RequiredSubset
```

Nicht mehr benötigte Tasks müssen anschließend kontrolliert beendet werden.

## Ergebnisse

Task-Ergebnisse können über die Gruppe gesammelt werden.

```text
Task A ─┐
Task B ─┼→ Result Collection
Task C ─┘
```

Ergebnisreihenfolge und Fehlersemantik müssen durch die jeweilige Group Policy eindeutig definiert sein.

## Fehlerbehandlung

Ein Task-Fehler wird der Task Group gemeldet.

```text
Task Failure
     ↓
Task Group
     ↓
Continue / Cancel / Retry / Escalate
```

Eine mögliche Fail-Fast-Policy kann verbleibende Tasks abbrechen:

```text
Task B Failed
      ↓
Cancel A + C
      ↓
Group Failed
```

Fehler dürfen nicht unbemerkt verloren gehen.

## Cancellation

Eine Cancellation der Gruppe propagiert zu ihren aktiven Tasks.

```text
Cancel Group
    ↓
Cancel Children
    ↓
Wait for Cleanup
    ↓
Group Cancelled
```

Die Gruppe darf erst als vollständig abgebrochen gelten, wenn die erforderliche Cleanup-Behandlung abgeschlossen ist.

## Ressourcen

Gemeinsame Ressourcen können an die Lebensdauer der Task Group gebunden werden.

```text
Create Group
    ↓
Acquire Shared Resources
    ↓
Execute Tasks
    ↓
Resolve Tasks
    ↓
Release Resources
```

Task-spezifische Ressourcen bleiben dem jeweiligen Task zugeordnet.

## Execution Contract

Eine Task Group kann gemeinsame Ausführungsanforderungen definieren.

```text
Group Contract
     ↓
Child Tasks
```

Child Tasks dürfen zusätzliche Einschränkungen definieren, aber Hard Constraints der Gruppe nicht abschwächen.

## Parallelität

Eine Task Group beschreibt logische Nebenläufigkeit und erzwingt keine bestimmte physische Parallelität.

```text
Concurrency ≠ Parallelism
```

Der Scheduler entscheidet anhand verfügbarer Ressourcen und Constraints über die tatsächliche parallele Ausführung.

## Normative Anforderungen

1. NovaOS MUSS Task Groups für logisch zusammengehörige Tasks unterstützen.
2. Jede Task Group MUSS einen definierten Owner besitzen.
3. Tasks einer Gruppe MÜSSEN eindeutig der Gruppe zugeordnet sein.
4. Die Gruppe MUSS den Lifecycle ihrer Tasks verfolgen.
5. Eine Task Group DARF NICHT erfolgreich abgeschlossen werden, solange erforderliche Tasks ungelöst sind.
6. Cancellation MUSS zu aktiven Child Tasks propagierbar sein.
7. Task-Fehler MÜSSEN der Gruppe gemeldet werden.
8. Fehler DÜRFEN NICHT unbemerkt verloren gehen.
9. Completion- und Failure-Policies MÜSSEN eindeutig definiert sein.
10. Gruppenressourcen MÜSSEN beim Ende der Gruppe kontrolliert behandelt werden.
11. Child Tasks DÜRFEN Hard Constraints des Group Contracts NICHT abschwächen.
12. Task Groups und ihre Child-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-STRUCTURED-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-CONCURRENCY-0003`

## Ergebnis

```text
Task Group
    ↓
Concurrent Tasks
    ↓
Results / Failures / Cancellation
    ↓
Group Policy
    ↓
Controlled Completion
```

NovaOS erhält damit eine strukturierte Abstraktion zur gemeinsamen Ausführung und Verwaltung mehrerer Tasks mit klar definiertem Lifecycle, Fehlerverhalten und Ressourcenbesitz.