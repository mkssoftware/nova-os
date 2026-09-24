# NPSPEC-CONCURRENCY-STRUCTURED-0001 – Nova Structured Concurrency

## Status

Entwurf

## Kategorie

Concurrency / Structured Concurrency / Task Lifecycle

## Zweck

NovaOS definiert Structured Concurrency als Standardmodell für hierarchisch organisierte nebenläufige Arbeit.

```text
Parent Task
    ↓
Task Scope
 ├── Child A
 ├── Child B
 └── Child C
```

Lebensdauer, Fehler und Cancellation von Child Tasks bleiben an einen klar definierten Scope gebunden.

## Grundprinzip

Nebenläufige Arbeit darf nicht unkontrolliert den Scope überleben, der sie erzeugt hat.

```text
Parent Scope
    ↓
Create Children
    ↓
Execute
    ↓
Join / Cancel / Transfer
    ↓
Leave Scope
```

Dabei gilt:

```text
Child Lifetime ⊆ Parent Scope
```

Eine Ausnahme ist nur durch explizite Übertragung an einen neuen Besitzer zulässig.

## Task Scope

Ein Task Scope beschreibt mindestens:

```text
ScopeID
Owner
ParentScope
Children
CancellationContext
State
```

Scopes bilden eine Hierarchie:

```text
Root Scope
 ├── Scope A
 │    ├── Task A1
 │    └── Task A2
 └── Scope B
      └── Task B1
```

## Scope-Ende

Ein Scope darf grundsätzlich erst beendet werden, wenn alle zugehörigen Child Tasks:

```text
Completed
Cancelled
Failed and handled
Transferred
```

sind.

Unbeaufsichtigte Tasks dürfen nicht zurückbleiben.

## Cancellation

Cancellation propagiert kontrolliert durch die Hierarchie.

```text
Parent Cancel
     ↓
Child A
Child B
Child C
```

Child Tasks müssen Gelegenheit erhalten, definierte Cleanup-Operationen auszuführen.

Cancellation ist ein regulärer Lifecycle-Vorgang und kein unkontrollierter Prozessabbruch.

## Fehlerpropagation

Fehler eines Child Tasks werden an den zuständigen Scope gemeldet.

```text
Child Failure
     ↓
Parent Scope
     ↓
Handle
Cancel Siblings
Retry
Escalate
```

Die konkrete Reaktion wird durch Scope- oder Execution-Policy bestimmt.

Fehler dürfen nicht unbemerkt verloren gehen.

## Ressourcen

Ressourcen können an einen Scope gebunden werden.

```text
Enter Scope
    ↓
Acquire Resources
    ↓
Execute Tasks
    ↓
Resolve Tasks
    ↓
Release Resources
```

Dadurch wird sichergestellt, dass Ressourcen nicht länger als ihr definierter Ausführungskontext bestehen bleiben.

## Detached Execution

Unabhängige Hintergrundarbeit ist nur durch explizite Besitzübertragung zulässig.

```text
Task Scope
    ↓
Transfer Ownership
    ↓
System Service / Supervisor / Job Manager
```

Dabei gilt:

```text
Detached ≠ Ownerless
```

## Deadlines

Ein Scope kann gemeinsame Zeitgrenzen definieren.

```text
Scope Deadline
      ↓
Child Tasks
```

Child Tasks dürfen strengere eigene Deadlines besitzen.

Eine Child Deadline darf die harte Deadline ihres Parent Scopes nicht ungültig machen.

## Execution Contracts

Execution Contracts können auf Scope- oder Task-Ebene gelten.

```text
Parent Contract
      ↓
Child Contract
```

Child Tasks dürfen Hard Constraints des Parent Scopes nicht abschwächen.

Zusätzliche Einschränkungen sind zulässig.

## Prozessgrenzen

Structured Concurrency ist nicht auf Threads innerhalb eines Prozesses beschränkt.

Das gleiche Lifecycle-Prinzip kann für:

```text
Local Tasks
Worker Processes
Remote Tasks
Distributed Execution
```

verwendet werden, sofern Besitz, Cancellation und Ergebnis eindeutig bleiben.

## Normative Anforderungen

1. NovaOS MUSS Structured Concurrency als natives Ausführungsmodell unterstützen.
2. Nebenläufige Tasks SOLLEN innerhalb expliziter Task Scopes erzeugt werden.
3. Jeder aktive Task MUSS einen definierten Owner besitzen.
4. Ein Scope DARF NICHT beendet werden, solange abhängige Child Tasks unkontrolliert aktiv sind.
5. Cancellation MUSS hierarchisch propagierbar sein.
6. Child-Fehler MÜSSEN dem zuständigen Scope gemeldet werden.
7. Fehler DÜRFEN NICHT unbemerkt verloren gehen.
8. Scopegebundene Ressourcen MÜSSEN beim Scope-Ende kontrolliert behandelt werden.
9. Detached Execution MUSS einen neuen expliziten Owner erhalten.
10. Child Contracts DÜRFEN Hard Constraints des Parent Scopes NICHT abschwächen.
11. Deadlines und Cancellation MÜSSEN über Scope-Grenzen kontrolliert propagierbar sein.
12. Task- und Scope-Hierarchien SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-PROCESS-MODEL-0001`
- `ADR-CONCURRENCY-0001`
- `ADR-CONCURRENCY-0002`

## Ergebnis

```text
Owner
  ↓
Structured Scope
  ↓
Child Tasks
  ↓
Completion / Cancellation / Failure
  ↓
Controlled Scope Exit
```

NovaOS erhält damit ein hierarchisches Nebenläufigkeitsmodell, bei dem Lebensdauer, Fehler, Cancellation und Ressourcen jeder Ausführung eindeutig kontrolliert bleiben.