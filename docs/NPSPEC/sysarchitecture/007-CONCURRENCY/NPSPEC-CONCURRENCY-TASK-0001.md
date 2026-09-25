# NPSPEC-CONCURRENCY-TASK-0001 – Nova Task Model

## Status

Angenommen

## Kategorie

Concurrency / Task / Execution / Kernel

## Zweck

NovaOS definiert Tasks als strukturierte, verwaltbare Ausführungseinheiten für nebenläufige Arbeit innerhalb eines Prozesses oder Systemkontexts.

```text
Process / Service
      ↓
Task Scope
      ↓
Task
      ↓
Execution
```

## Grundprinzip

Ein Task besitzt immer einen definierten Besitzer und Lebenszyklus.

```text
Owner
  ↓
Task
  ├── Execution
  ├── Resources
  ├── Cancellation
  └── Result
```

Unkontrollierte, besitzerlose Hintergrundausführung soll vermieden werden.

## Task-Modell

Ein Task beschreibt mindestens:

```text
TaskID
ParentID
Owner
State
ExecutionContext
CancellationContext
Result
```

Optional können enthalten sein:

```text
ExecutionContract
Priority
Deadline
ResourceBudget
Affinity
```

## Task-Zustände

Mindestens folgende Zustände werden unterstützt:

```text
Created
Ready
Running
Waiting
Suspended
Completed
Cancelled
Failed
```

`Completed`, `Cancelled` und `Failed` sind terminale Zustände.

## Task-Hierarchie

Tasks können untergeordnete Tasks erzeugen.

```text
Parent Task
├── Child A
├── Child B
└── Child C
```

Ein Parent darf seinen Scope grundsätzlich erst verlassen, wenn seine Child Tasks:

```text
Completed
Cancelled
```

oder kontrolliert an einen neuen Besitzer übertragen wurden.

## Erzeugung

Ein Task wird innerhalb eines definierten Scopes erzeugt.

```text
Create Task
    ↓
Assign Owner
    ↓
Validate Contract
    ↓
Allocate Resources
    ↓
Ready
    ↓
Schedule
```

Ein Task ohne gültigen Owner darf nicht normal gestartet werden.

## Ausführung

Der Scheduler entscheidet, wann und auf welcher geeigneten CPU ein ausführbarer Task ausgeführt wird.

```text
Ready Task
    ↓
Scheduler
    ↓
Running
```

Task-Modell und Scheduling-Policy bleiben logisch getrennt.

## Warten

Ein Task kann auf Ereignisse, Ressourcen oder andere Tasks warten.

```text
Running
   ↓
Waiting
   ↓
Ready
```

Warten soll den ausführenden CPU-Kontext nicht unnötig blockieren.

## Cancellation

Cancellation ist ein regulärer Bestandteil des Task-Lebenszyklus.

```text
Parent Cancellation
        ↓
Child Tasks
        ↓
Cancellation Propagation
```

Cancellation muss kontrolliert verarbeitet werden und darf Ressourcen nicht inkonsistent zurücklassen.

## Fehler

Fehler eines Child Tasks werden innerhalb der Task-Hierarchie behandelt.

```text
Child Failure
     ↓
Parent Scope
     ↓
Handle / Cancel / Escalate
```

Fehler dürfen nicht unbemerkt aus strukturierten Task Scopes verschwinden.

## Ressourcen

Task-spezifische Ressourcen werden an den Task-Lebenszyklus gebunden.

```text
Task Start
   ↓
Acquire Resources
   ↓
Execute
   ↓
Release Resources
```

Beim terminalen Zustand müssen taskgebundene Ressourcen freigegeben oder explizit übertragen werden.

## Detached Tasks

Detached Tasks sind nur zulässig, wenn ein neuer expliziter Besitzer festgelegt wird.

Beispiele:

```text
System Service
Session Manager
Job Manager
Supervisor
```

```text
Detach ≠ Ownerless
```

## Execution Contract

Tasks dürfen durch einen `Nova.ExecutionContract` Anforderungen definieren.

Beispiele:

```text
Deadline
Resource Budget
Determinism
Execution Location
Trust
Preferred Provider
```

Hard Requirements müssen vor und während der Ausführung eingehalten werden.

## Normative Anforderungen

1. NovaOS MUSS Tasks als verwaltbare Ausführungseinheiten unterstützen.
2. Jeder Task MUSS eine eindeutige `TaskID` besitzen.
3. Jeder aktive Task MUSS einen definierten Besitzer besitzen.
4. Tasks SOLLEN hierarchisch strukturiert werden.
5. Parent Scopes DÜRFEN Child Tasks nicht unkontrolliert zurücklassen.
6. Cancellation MUSS durch Task-Hierarchien propagierbar sein.
7. Task-Fehler MÜSSEN kontrolliert behandelt oder eskaliert werden.
8. Taskgebundene Ressourcen MÜSSEN beim terminalen Zustand behandelt werden.
9. Detached Tasks DÜRFEN nur mit explizitem neuen Besitzer existieren.
10. Task-Modell und Scheduling-Policy MÜSSEN getrennt bleiben.
11. Execution Contracts MÜSSEN bei der Task-Ausführung berücksichtigt werden.
12. Task-Zustände und Beziehungen SOLLEN introspektierbar sein.

## Abhängigkeiten

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
Structured Task Scope
  ↓
Tasks
  ↓
Schedule / Wait / Cancel
  ↓
Result + Cleanup
```

NovaOS erhält damit ein strukturiertes Task-Modell mit eindeutigem Besitz, kontrollierter Cancellation, Fehlerweitergabe und klar definiertem Lebenszyklus.