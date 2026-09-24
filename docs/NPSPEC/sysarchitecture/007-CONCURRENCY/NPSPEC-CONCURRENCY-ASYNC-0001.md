# NPSPEC-CONCURRENCY-ASYNC-0001 – Nova Asynchronous Execution

## Status

Entwurf

## Kategorie

Concurrency / Async / Task / Non-Blocking Execution

## Zweck

NovaOS definiert ein natives Modell für asynchrone Operationen, bei denen Tasks auf Ergebnisse warten können, ohne einen CPU-Ausführungskontext unnötig zu blockieren.

```text
Task
  ↓
Async Operation
  ↓
Suspend
  ↓
Event / Completion
  ↓
Resume
```

## Grundprinzip

Asynchrones Warten blockiert nicht automatisch einen Thread oder CPU-Kern.

```text
Waiting for Result ≠ Blocking CPU
```

Während eine Operation wartet, kann der Scheduler andere ausführbare Arbeit ausführen.

## Async Operation

Eine asynchrone Operation besitzt mindestens:

```text
OperationID
Owner
State
Result
Error
CancellationContext
```

Optional können gelten:

```text
Deadline
ExecutionContract
ResourceBudget
```

## Zustände

Mindestens folgende Zustände werden unterstützt:

```text
Created
Pending
Completed
Cancelled
Failed
```

Ein terminaler Zustand darf nur einmal erreicht werden.

## Ablauf

```text
Start Async Operation
        ↓
Register Completion
        ↓
Suspend Task
        ↓
Perform Operation
        ↓
Completion Event
        ↓
Mark Result
        ↓
Wake Task
        ↓
Resume
```

Die Wiederaufnahme erfolgt über den normalen Scheduler.

## Await

Tasks können auf eine asynchrone Operation warten.

```text
Result = await Operation
```

`await` beschreibt dabei semantisch:

```text
Check Completion
      ↓
Not Complete
      ↓
Suspend Task
      ↓
Resume on Completion
```

Die konkrete Sprachsyntax ist nicht Bestandteil dieser Spezifikation.

## Completion

Eine Operation kann abgeschlossen werden durch:

```text
I/O Completion
IPC Response
Timer
Task Result
Device Event
Network Event
Explicit Signal
```

Completion muss eindeutig einer wartenden Operation zugeordnet werden können.

## Cancellation

Asynchrone Operationen müssen in das NovaOS-Cancellation-Modell integrierbar sein.

```text
Cancellation
     ↓
Async Operation
     ↓
Abort if possible
     ↓
Resolve State
```

Ist eine zugrunde liegende Operation nicht abbrechbar, muss ihr späteres Ergebnis kontrolliert behandelt werden.

## Deadlines

Async Operations können Deadlines besitzen.

```text
Async Operation
      ↓
Deadline
      ↓
Complete
   or
Cancellation / Miss Policy
```

Deadline und Cancellation bleiben getrennte Konzepte.

## Ressourcen

Während eines asynchronen Waits dürfen unnötige CPU-Ressourcen nicht dauerhaft reserviert bleiben.

Andere notwendige Ressourcen können bis zur Completion gebunden bleiben.

Besitz und Lebensdauer müssen eindeutig definiert sein.

## Structured Concurrency

Async Operations gehören zum Scope des erzeugenden Tasks.

```text
Task Scope
   ↓
Async Operation
```

Der Scope darf nicht beendet werden, solange abhängige Operationen unkontrolliert aktiv sind.

Bei Scope-Cancellation müssen offene Async Operations aufgelöst werden.

## Fehler

Fehler werden als Teil des Operationsergebnisses behandelt.

```text
Completion
├── Result
├── Cancelled
└── Error
```

Fehler dürfen nicht verloren gehen oder ausschließlich über globale Fehlerzustände gemeldet werden.

## Normative Anforderungen

1. NovaOS MUSS asynchrone Operationen unterstützen.
2. Asynchrones Warten SOLL keinen CPU-Ausführungskontext unnötig blockieren.
3. Jede Async Operation MUSS eindeutig identifizierbar sein.
4. Completion MUSS eindeutig einer Operation zugeordnet werden.
5. Eine Operation DARF nur einen terminalen Zustand erreichen.
6. Wartende Tasks MÜSSEN nach Completion kontrolliert wieder ausführbar werden.
7. Async Operations MÜSSEN in Structured Concurrency integrierbar sein.
8. Cancellation MUSS auf offene Async Operations propagierbar sein.
9. Nicht abbrechbare Operationen MÜSSEN nach Cancellation kontrolliert aufgelöst werden.
10. Deadlines MÜSSEN mit Async Operations kombinierbar sein.
11. Ressourcenbesitz MUSS während asynchroner Ausführung eindeutig bleiben.
12. Zustand, Fehler und Completion SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-STRUCTURED-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-CONCURRENCY-0006`

## Ergebnis

```text
Task
  ↓
Start Async Work
  ↓
Suspend without CPU Blocking
  ↓
Completion
  ↓
Scheduler
  ↓
Resume Task
```

NovaOS erhält damit ein natives asynchrones Ausführungsmodell, das I/O, IPC und andere wartende Operationen effizient mit Tasks, Structured Concurrency, Cancellation und Deadlines verbindet.