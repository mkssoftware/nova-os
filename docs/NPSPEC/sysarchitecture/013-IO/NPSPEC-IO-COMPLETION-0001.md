# NPSPEC-IO-COMPLETION-0001 – Nova I/O Completion

## Status

Angenommen

## Kategorie

I/O / Completion / Asynchronous I/O

## Zweck

NovaOS definiert ein einheitliches Completion-Modell für den Abschluss asynchroner I/O-Operationen.

```text
IORequest
   ↓
Processing
   ↓
Completion
   ↓
Consumer
```

Completion übermittelt den endgültigen oder teilweisen Ausgang eines zuvor eingereichten I/O-Requests.

## Grundprinzip

```text
Submitted ≠ Completed
Completed ≠ Successful
Completion ≠ Callback
```

Completion ist ein I/O-Ereignis. Callback, Future, Event oder Completion Queue sind lediglich mögliche Zustellmechanismen.

## Completion Record

Jeder abgeschlossene Request muss einen eindeutigen Completion-Eintrag erzeugen können.

```text
IOCompletion
├── RequestID
├── Status
├── Result
└── BytesTransferred
```

Optional:

```text
Error
Timestamp
Device Status
Provider Data
```

## Status

Mindestens folgende Ergebnisse müssen unterscheidbar sein:

```text
Success
Partial
Failed
Cancelled
Timeout
DeadlineMiss
Unavailable
```

Providerspezifische Fehler dürfen zusätzlich enthalten sein.

## Request-Zuordnung

Jede Completion muss eindeutig ihrem ursprünglichen `IORequest` zugeordnet werden können.

```text
RequestID
   ↓
IORequest ↔ Completion
```

Eine Completion darf nicht versehentlich einem wiederverwendeten oder fremden Request zugeordnet werden.

## Completion Queue

NovaOS soll Completion Queues als effizienten Standardmechanismus unterstützen.

```text
Device / Driver
      ↓
Completion Queue
      ↓
Consumer
```

Eine Queue darf mehrere abgeschlossene Requests gesammelt bereitstellen.

Dies reduziert Kontextwechsel und unterstützt hohe I/O-Raten.

## Batch Completion

Mehrere Completions dürfen gemeinsam verarbeitet werden.

```text
Completion Queue
├── Request A
├── Request B
├── Request C
└── Request D
```

Batch-Verarbeitung soll insbesondere für Storage, Netzwerk und High-Throughput-I/O verfügbar sein.

## Buffer-Freigabe

Eine Completion definiert den Punkt, ab dem der zugehörige Buffer entsprechend der I/O-Semantik wieder freigegeben oder wiederverwendet werden darf.

```text
I/O Running
    ↓
Completion
    ↓
Release / Reuse Buffer
```

Bei DMA muss zusätzlich die erforderliche Synchronisation abgeschlossen sein.

## Partial Completion

Eine Operation darf nur teilweise abgeschlossen werden.

```text
Requested: 4096 Bytes
Transferred: 2048 Bytes
Status: Partial
```

Der Consumer muss erkennen können, welcher Teil tatsächlich übertragen wurde.

## Cancellation

Cancellation und Completion müssen miteinander koordiniert werden.

```text
Cancel Request
      ↓
Race
 ┌────┴────┐
Cancelled Completed
```

Es darf genau ein eindeutig bestimmbarer finaler Zustand entstehen.

Eine bereits abgeschlossene Operation darf nicht nachträglich als erfolgreich abgebrochen dargestellt werden.

## Ordering

NovaOS darf unterschiedliche Completion-Reihenfolgen unterstützen.

```text
Submit:   A B C
Complete: B A C
```

Submission-Reihenfolge darf nicht automatisch Completion-Reihenfolge bedeuten.

Falls Ordering erforderlich ist, muss dies explizit durch Request, Provider oder Execution Contract definiert werden.

## Notification

Completion Queues dürfen unterschiedliche Benachrichtigungsmechanismen verwenden:

```text
Interrupt
Event
Polling
Hybrid Polling
Scheduler Wakeup
```

Der Mechanismus darf abhängig von Latenz-, Energie- und Throughput-Anforderungen gewählt werden.

## Realtime

Realtime-I/O muss Completion-Latenzen begrenzen können.

```text
Submit
  ↓
I/O
  ↓
Completion
  ↓
Deadline Check
```

Deadline Misses müssen explizit sichtbar sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Completion Status
BytesTransferred
Latency
Error
Queue
Timestamp
Deadline State
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches I/O-Completion-Modell bereitstellen.
2. Jede Completion MUSS eindeutig einem `IORequest` zugeordnet werden können.
3. Completion und Erfolg MÜSSEN getrennte Konzepte bleiben.
4. Erfolg, Partial, Fehler, Cancellation und Timeout MÜSSEN unterscheidbar sein.
5. Die tatsächlich übertragene Datenmenge MUSS meldbar sein.
6. NovaOS SOLL Completion Queues unterstützen.
7. Completion Queues SOLLEN Batch-Verarbeitung unterstützen können.
8. Buffer DÜRFEN NICHT vor ihrem definierten sicheren Completion-Punkt wiederverwendet werden.
9. Cancellation und normale Completion MÜSSEN Race-sicher koordiniert werden.
10. Submission-Reihenfolge DARF NICHT automatisch Completion-Reihenfolge voraussetzen.
11. Realtime-I/O MUSS Deadline Misses eindeutig erkennen können.
12. Completion-Zustand, Fehler und Latenz SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DATAMOVE-RINGBUFFER-0001`
- `NPSPEC-CONCURRENCY-ASYNC-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-IO-0002`

## Ergebnis

```text
IORequest
   ↓
I/O Provider
   ↓
Completion Record
   ↓
Completion Queue
   ↓
Consumer
```

NovaOS erhält damit ein einheitliches Completion-Modell, das asynchrone I/O-Operationen effizient, eindeutig und Race-sicher abschließt.