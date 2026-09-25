# NPSPEC-DISTCOMM-QUEUE-0001 – Nova Distributed Queue

## Status

Angenommen

## Kategorie

Distributed Communication / Messaging / Queue

## Zweck

NovaOS definiert verteilte Queues für asynchrone, entkoppelte Arbeits- und Nachrichtenverteilung zwischen Produzenten und Konsumenten.

```text
Producer(s)
    ↓
Queue
    ↓
Consumer(s)
```

Queues ermöglichen Lastverteilung, zeitliche Entkopplung und kontrollierte Verarbeitung auch dann, wenn Producer und Consumer nicht gleichzeitig verfügbar sind.

## Grundprinzip

```text
Enqueued ≠ Delivered
Delivered ≠ Processed
Processed ≠ Committed
Queue Position ≠ Global Order
Queue Access ≠ Authorization
```

Zustellung, Verarbeitung und erfolgreiche Wirkung bleiben getrennte Zustände.

## Queue-Modell

Eine Queue besitzt mindestens:

```text
DistributedQueue
├── QueueID
├── Name
├── Message Type
├── Delivery Policy
├── State
└── Resource Budget
```

Optional:

```text
Ordering Policy
Persistence Policy
Retry Policy
Dead-Letter Queue
Security Context
ExecutionContract
Network Intent
```

Die `QueueID` bleibt unabhängig von Broker, Host, Netzwerkadresse oder physischem Speicherort.

## Queue-Eintrag

Ein Eintrag besitzt mindestens:

```text
QueueEntry
├── MessageID
├── Payload
├── Producer
├── Enqueue Time
└── State
```

Optional:

```text
Priority
Deadline
Delivery Count
Visibility Deadline
Idempotency Key
CorrelationID
Trace Context
```

## Zustandsmodell

```text
Queued
   ↓
Reserved
   ↓
Delivered
   ↓
Processing
   ↓
Acknowledged
```

Fehler dürfen zu:

```text
Queued      ← Retry
Delayed
Dead-Letter
Expired
Rejected
```

führen.

## Consumer-Modell

Mehrere Consumer dürfen aus derselben Queue arbeiten.

```text
          Queue
       ┌────┼────┐
       ↓    ↓    ↓
      C1   C2   C3
```

Ein Queue-Eintrag wird entsprechend der Delivery Policy einem Consumer zur Verarbeitung zugewiesen.

Damit eignet sich die Queue insbesondere für verteilte Worker- und Job-Systeme.

## Zustellungssemantik

NovaOS muss mindestens ausdrücken können:

```text
At-Most-Once
At-Least-Once
```

`Exactly-Once` darf nur angegeben werden, wenn der tatsächlich beobachtbare Effekt einmalige Ausführung garantiert.

```text
Exactly-Once Delivery ≠ Exactly-Once Effect
```

Idempotente Operationen sollen bevorzugt werden, wenn Wiederholungen möglich sind.

## Acknowledgement

Ein Consumer bestätigt die erfolgreiche Verarbeitung explizit.

```text
Reserve
   ↓
Deliver
   ↓
Process
   ↓
Acknowledge
```

Erfolgt keine Bestätigung innerhalb der geltenden Policy, darf der Eintrag erneut verfügbar werden.

## Visibility / Reservation

Während ein Consumer einen Eintrag verarbeitet, darf dieser temporär reserviert werden.

```text
Available
   ↓
Reserved
   ↓
Acknowledge
```

oder:

```text
Reserved
   ↓ timeout
Available
```

Dadurch kann ein abgestürzter Consumer die Queue nicht dauerhaft blockieren.

## Ordering

Queues dürfen unterschiedliche Ordering-Modelle anbieten:

```text
FIFO
Per-Key FIFO
Priority
Unordered
```

Eine globale FIFO-Garantie darf nur angegeben werden, wenn sie auch bei Replikation, Fehlern und mehreren Consumern tatsächlich eingehalten wird.

## Priorität

Queue-Einträge dürfen Prioritäten besitzen.

```text
High
Normal
Background
```

Priorität darf jedoch keine unbegrenzte Verdrängung niedrigerer Klassen verursachen.

Fairness-Regeln müssen möglich bleiben.

## Persistenz

Queues dürfen sein:

```text
Transient
Buffered
Durable
```

Durable Queues müssen eindeutig definieren, wann ein Eintrag als persistent übernommen gilt.

Persistenz darf nicht automatisch mit erfolgreicher Verarbeitung gleichgesetzt werden.

## Retry

Fehlgeschlagene Verarbeitung darf kontrolliert wiederholt werden.

```text
Failure
   ↓
Retry Policy
├── Immediate
├── Delayed
├── Backoff
└── Dead-Letter
```

Retry-Anzahl und Retry-Zeitraum müssen begrenzt sein.

## Dead-Letter Queue

Nicht erfolgreich verarbeitbare Nachrichten dürfen in eine Dead-Letter Queue verschoben werden.

```text
Main Queue
    ↓ repeated failure
Dead-Letter Queue
```

Dadurch werden fehlerhafte Nachrichten isoliert, ohne die Hauptqueue dauerhaft zu blockieren.

## Backpressure

Queues sind keine unbegrenzten Puffer.

```text
Producer Rate
     >
Consumer Rate
     ↓
Queue Pressure
     ↓
Backpressure
```

Mögliche Reaktionen:

```text
Throttle
Wait
Reject
Drop according to Policy
```

Backpressure soll bis zum Producer propagiert werden können.

## Sicherheit

Queue-Rechte müssen mindestens getrennt werden können in:

```text
Enqueue
Consume
Acknowledge
Inspect
Administer
```

Das Wissen über `QueueID` oder Queue-Namen erzeugt keine Berechtigung.

## Verteilung

Queues dürfen über mehrere Systeme repliziert oder migriert werden.

```text
Logical QueueID
      ↓
Distributed Queue
   ┌─────┴─────┐
 Node A      Node B
```

Die logische Queue-Identität bleibt unabhängig vom aktuellen Standort.

Verteilung darf Delivery-, Ordering- und Persistenzgarantien nicht stärker darstellen als tatsächlich umgesetzt.

## Ressourcensteuerung

NovaOS muss begrenzen können:

```text
Queue Depth
Message Size
Storage
Consumers
Reservations
Retries
Bandwidth
Memory
```

Resource Budgets dürfen pro Queue, Producer, Consumer oder Sicherheitsdomäne gelten.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
QueueID
State
Queue Depth
Message Type
Consumer Count
Oldest Entry
Delivery Rate
Retry Count
Dead-Letter Count
Resource Usage
```

Payloads dürfen nur entsprechend ihrer Berechtigungen sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS verteilte Queues unterstützen können.
2. Queue-Identität MUSS unabhängig vom physischen Standort sein.
3. Queue-Einträge SOLLEN typisiert sein.
4. Zustellungssemantik MUSS explizit definiert werden.
5. Acknowledgements MÜSSEN unterstützt werden können.
6. Consumer-Ausfälle DÜRFEN Einträge NICHT dauerhaft blockieren.
7. Ordering DARF NICHT stärker garantiert werden als tatsächlich umgesetzt.
8. Retries MÜSSEN begrenzt sein.
9. Dead-Letter Queues SOLLEN unterstützt werden.
10. Queues MÜSSEN Backpressure unterstützen.
11. Queue-Ressourcen MÜSSEN begrenzt sein.
12. Queue-, Delivery- und Consumer-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0006`

## Ergebnis

```text
Producer(s)
     ↓
Typed Queue
     ↓
Buffer / Persistence
     ↓
Reservation + Delivery
     ↓
Consumer Pool
     ↓
Acknowledge / Retry / Dead-Letter
```

NovaOS erhält damit eine verteilte Queue-Infrastruktur für robuste asynchrone Arbeitsverteilung mit expliziter Zustellungssemantik, Backpressure, Wiederholungslogik, Fehlerisolation und kontrollierter Ressourcenverwaltung.