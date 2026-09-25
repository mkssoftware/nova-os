# NPSPEC-IO-ASYNC-0001 – Nova Asynchronous I/O

## Status

Angenommen

## Kategorie

I/O / Asynchronous I/O / Concurrency

## Zweck

NovaOS definiert ein einheitliches Modell für asynchrone I/O-Operationen.

```text
Submit I/O
    ↓
Continue Execution
    ↓
I/O Processing
    ↓
Completion
```

Ein Task muss während einer laufenden I/O-Operation nicht blockieren, sofern die Operation asynchron ausgeführt werden kann.

## Grundprinzip

```text
Submit ≠ Complete
Async I/O ≠ New Thread per Operation
Completion ≠ Success
```

Asynchronität beschreibt die Trennung zwischen Einreichen und Abschluss einer I/O-Operation.

## I/O Request

Eine asynchrone Operation wird durch einen `IORequest` repräsentiert.

```text
IORequest
├── RequestID
├── Operation
├── Target
├── Buffer
├── State
└── Completion
```

Optional:

```text
Deadline
Priority
Cancellation
ExecutionContract
Security Context
```

## Zustände

Ein Request besitzt einen definierten Lifecycle.

```text
Created
   ↓
Submitted
   ↓
Pending
   ↓
Running
   ↓
Completed / Failed / Cancelled
```

`Submitted` oder `Running` darf nicht als erfolgreicher Abschluss interpretiert werden.

## Completion

Der Abschluss einer Operation muss eindeutig signalisiert werden können.

```text
Completion
├── Result
├── BytesTransferred
├── Error
└── Status
```

NovaOS darf verschiedene Completion-Mechanismen bereitstellen:

```text
Future / Promise
Completion Queue
Event
Callback
Await
```

Diese müssen auf ein gemeinsames I/O-Modell abbildbar sein.

## Buffer Lifecycle

Ein verwendeter Buffer muss während der gesamten relevanten I/O-Operation gültig bleiben.

```text
Submit
  ↓
Buffer Owned / Pinned / Referenced
  ↓
I/O
  ↓
Completion
  ↓
Release
```

Ein Buffer darf nicht vorzeitig freigegeben oder anderweitig unsicher wiederverwendet werden.

## DataMove

Async I/O soll mit den NovaOS-DataMove-Mechanismen zusammenarbeiten.

```text
IORequest
   ↓
Shared Buffer / Scatter-Gather
   ↓
DMA / Driver
   ↓
Completion
```

Zero-Copy soll verwendet werden können, wenn der gesamte I/O-Pfad dies unterstützt.

## Cancellation

Ausstehende Operationen müssen kontrolliert abbrechbar sein, sofern der Provider dies unterstützt.

```text
Pending
   ↓
Cancel Request
   ↓
Cancelled / Too Late / Failed
```

Eine Cancellation-Anforderung bedeutet nicht automatisch, dass die Operation nicht bereits teilweise ausgeführt wurde.

## Deadlines

I/O-Requests dürfen Deadlines besitzen.

```text
Submit
  ↓
Deadline
  ↓
Complete / Deadline Miss
```

Ein Deadline Miss muss erkennbar sein und darf nicht mit erfolgreicher Cancellation gleichgesetzt werden.

## Fehlerbehandlung

Asynchrone Fehler müssen dem ursprünglichen Request eindeutig zugeordnet werden.

Beispiele:

```text
Timeout
Unavailable
PermissionDenied
IOError
Cancelled
PartialTransfer
DeviceRemoved
```

Teilweise abgeschlossene Operationen müssen die tatsächlich übertragene Datenmenge melden können.

## Backpressure

NovaOS muss verhindern können, dass unbegrenzt neue I/O-Requests erzeugt werden.

```text
Producer
   ↓
Submission Queue
   ↓
Capacity Limit
   ↓
Backpressure
```

Queue-Größen und Ressourcenverbrauch müssen begrenzbar sein.

## Structured Concurrency

Asynchrone I/O-Operationen sollen einem Owner beziehungsweise Task-Scope zugeordnet werden.

```text
Task Scope
   ├── IORequest A
   ├── IORequest B
   └── IORequest C
```

Beim Beenden eines Scopes müssen noch laufende Requests kontrolliert abgeschlossen, übertragen oder abgebrochen werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Operation
Target
State
Owner
Transferred Bytes
Deadline
Error
Latency
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Modell für asynchrone I/O-Operationen bereitstellen.
2. Submission und Completion MÜSSEN logisch getrennt sein.
3. Async I/O DARF NICHT einen eigenen Thread pro Operation voraussetzen.
4. Jeder Request MUSS einen eindeutig bestimmbaren Lifecycle besitzen.
5. Verwendete Buffer MÜSSEN bis zum sicheren Abschluss der Operation gültig bleiben.
6. Completion MUSS Erfolg, Fehler und teilweise Übertragung unterscheiden können.
7. Cancellation MUSS einen eindeutig erkennbaren Ausgang besitzen.
8. Deadline Miss und Cancellation MÜSSEN getrennte Zustände bleiben.
9. Async I/O SOLL Zero-Copy, Scatter/Gather und DMA unterstützen können.
10. Submission Queues MÜSSEN begrenzbar sein und Backpressure unterstützen.
11. I/O-Requests SOLLEN in Structured-Concurrency-Scopes integrierbar sein.
12. Request-Zustand, Fehler und Laufzeitinformationen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-CONCURRENCY-ASYNC-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-IO-0001`

## Ergebnis

```text
Task
 ↓
IORequest
 ↓
Async I/O Pipeline
 ↓
Device / Provider
 ↓
Completion
 ↓
Task
```

NovaOS erhält damit ein einheitliches, nicht blockierendes I/O-Modell, das Completion, Cancellation, Deadlines, DataMove und Structured Concurrency miteinander verbindet.