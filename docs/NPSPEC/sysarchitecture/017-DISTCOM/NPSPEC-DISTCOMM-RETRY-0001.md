# NPSPEC-DISTCOMM-RETRY-0001 – Nova Distributed Retry

## Status

Angenommen

## Kategorie

Distributed Communication / Reliability / Retry

## Zweck

NovaOS definiert ein gemeinsames Retry-Modell für fehlgeschlagene oder unvollständig bestätigte verteilte Operationen.

```text
Operation
   ↓
Attempt
   ↓
Failure
   ↓
Retry Decision
 ├── Retry
 ├── Wait
 └── Fail
```

Retries dürfen Fehler tolerieren, aber keine unkontrollierte Wiederholung oder doppelte fachliche Wirkung erzeugen.

## Grundprinzip

```text
Failure ≠ Retry
Retry ≠ Safe
Timeout ≠ Operation Failed
Retry Success ≠ First Attempt Failed
Idempotent ≠ Exactly-Once
```

Vor jedem Retry muss bewertet werden, ob eine Wiederholung semantisch zulässig ist.

## Retry-Modell

Eine Retry-Operation besitzt mindestens:

```text
RetryContext
├── OperationID
├── Attempt
├── Failure
├── Retry Policy
├── State
└── Deadline
```

Optional:

```text
Idempotency Key
ExecutionContract
Resource Budget
Network Intent
Security Context
Backpressure State
Retry Budget
```

Die `OperationID` bleibt über alle Versuche derselben logischen Operation stabil.

## Zustandsmodell

```text
Initial
   ↓
Executing
   ↓
Failed
   ↓
Evaluating
 ├── Retrying
 ├── Waiting
 └── FinalFailure
```

Bei Erfolg:

```text
Executing
   ↓
Completed
```

Bei unklarem Ausführungszustand:

```text
UnknownExecutionState
```

`UnknownExecutionState` darf nicht automatisch als sicher wiederholbar behandelt werden.

## Retry Policy

Eine Retry Policy darf mindestens definieren:

```text
Maximum Attempts
Maximum Duration
Backoff
Retryable Errors
Non-Retryable Errors
Jitter
Deadline
Retry Budget
```

Unbegrenzte Retries sind nicht zulässig.

## Idempotenz

NovaOS muss zwischen idempotenten und nicht-idempotenten Operationen unterscheiden können.

```text
Read Resource
   → normalerweise retry-fähig

Create Payment
   → nicht automatisch retry-fähig
```

Nicht-idempotente Operationen dürfen nur erneut ausgeführt werden, wenn ihre Semantik dies sicher erlaubt.

## Idempotency Key

Operationen dürfen einen stabilen Idempotency Key verwenden.

```text
Logical Operation
      ↓
IdempotencyKey
      ↓
Attempt 1
Attempt 2
Attempt 3
```

Die entfernte Seite darf damit mehrere Übertragungsversuche derselben logischen Operation erkennen.

Ein Idempotency Key garantiert jedoch nur dann einmalige Wirkung, wenn der empfangende Dienst diese Semantik tatsächlich implementiert.

## Unknown Execution State

Bei Netzwerk- oder Systemfehlern kann unbekannt sein, ob eine Operation ausgeführt wurde.

```text
Request Sent
     ↓
Remote Execution?
     ↓
Connection Lost
     ↓
UnknownExecutionState
```

NovaOS darf in diesem Zustand keinen automatischen Retry durchführen, wenn dadurch ein unerwünschter doppelter Effekt entstehen könnte.

## Backoff

Retries sollen zeitlich gestaffelt werden.

Beispiel:

```text
Attempt 1
   ↓
100 ms
   ↓
Attempt 2
   ↓
200 ms
   ↓
Attempt 3
   ↓
400 ms
```

Unterstützt werden sollen mindestens:

```text
Fixed Backoff
Linear Backoff
Exponential Backoff
```

Jitter soll verwendet werden können, um synchronisierte Retry-Wellen zu vermeiden.

## Retry Budget

Retries müssen als Ressourcenverbrauch betrachtet werden.

```text
Original Requests
       +
Retry Requests
       ↓
Retry Budget
```

Retry Budgets dürfen pro:

```text
Service
Flow
Client
Namespace
Operation Type
```

definiert werden.

## Backpressure

Retries dürfen bestehende Überlastung nicht verstärken.

```text
Service Overloaded
      ↓
Failure
      ↓
Backpressure
      ↓
Delay / Reject Retry
```

Backpressure muss Vorrang vor aggressiver Wiederholung erhalten können.

## Deadline

Retries müssen die ursprüngliche Deadline berücksichtigen.

```text
Remaining Time
      ↓
Retry Cost
      ↓
Retry useful?
```

Ist eine erfolgreiche Fertigstellung innerhalb der verbleibenden Deadline nicht mehr realistisch, soll kein weiterer Versuch gestartet werden.

## Netzwerkänderungen

Ein Retry darf einen anderen gültigen Netzwerkpfad verwenden.

```text
Attempt 1
   ↓ Path A failed
Retry
   ↓
Path B
```

Dabei müssen weiterhin gelten:

```text
Network Intent
Security Policy
Sovereignty
Trust Requirements
```

Ein Retry darf diese Anforderungen nicht abschwächen.

## Retry bei RPC

Distributed RPC muss unterscheiden zwischen:

```text
Request Not Sent
Request Partially Sent
Request Sent
Response Lost
Remote Failure
Unknown Execution
```

Diese Zustände können unterschiedliche Retry-Regeln erfordern.

## Retry bei Messaging

Message Bus, Pub/Sub und Queues dürfen Nachrichten erneut zustellen.

```text
Deliver
   ↓
No Acknowledgement
   ↓
Retry
```

Delivery Count muss dabei nachvollziehbar bleiben.

Wiederholte Fehler dürfen zu:

```text
Delay
Dead-Letter
Quarantine
Final Failure
```

führen.

## Retry Storm Protection

NovaOS muss Retry-Stürme verhindern können.

Mechanismen dürfen umfassen:

```text
Backoff
Jitter
Retry Budget
Rate Limit
Circuit Breaking
Backpressure
Maximum Attempts
```

Der Ausfall eines Dienstes darf nicht durch massenhafte Wiederholungen weiter verstärkt werden.

## Sicherheit

Ein Retry muss weiterhin sämtliche Sicherheitsprüfungen erfüllen.

```text
Previous Authorization ≠ Permanent Authorization
```

Capabilities, Credentials, Trust State und Policies dürfen zwischen zwei Versuchen erneut validiert werden, wenn dies erforderlich ist.

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
Retry Attempts
Retry Duration
Retry Queue
Retry Memory
Retry Bandwidth
Concurrent Retries
Retry Storage
```

Retries müssen dem Resource Budget der verursachenden Operation zurechenbar sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
OperationID
Attempt
Maximum Attempts
Last Failure
Next Retry
Backoff
Retry Budget
Idempotency State
Execution State
Final Result
```

Damit muss nachvollziehbar sein, warum ein Retry ausgeführt, verzögert oder verweigert wurde.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Distributed-Retry-Modell bereitstellen.
2. Retries MÜSSEN durch explizite Policies kontrolliert werden.
3. Unbegrenzte Retries DÜRFEN NICHT stattfinden.
4. Nicht-idempotente Operationen DÜRFEN NICHT blind wiederholt werden.
5. `UnknownExecutionState` MUSS explizit darstellbar sein.
6. Idempotency Keys SOLLEN unterstützt werden.
7. Backoff und Jitter SOLLEN unterstützt werden.
8. Retries MÜSSEN Backpressure berücksichtigen.
9. Retries MÜSSEN Deadline und Resource Budget berücksichtigen.
10. Retries DÜRFEN Security-, Trust-, Network-Intent- oder Sovereignty-Regeln NICHT umgehen.
11. Retry-Stürme MÜSSEN begrenzt werden können.
12. Retry-Entscheidungen und Ursachen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-0001`
- `NPSPEC-NETWORK-MIGRATION-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-DISTCOMM-0012`

## Ergebnis

```text
Failure
   ↓
Execution State
   +
Idempotency
   +
Deadline
   +
Backpressure
   +
Retry Budget
   ↓
Retry Decision
 ├── Retry
 ├── Delay
 └── Final Failure
```

NovaOS erhält damit ein kontrolliertes Retry-Modell, das temporäre Fehler tolerieren kann, ohne unbekannte Ausführungszustände, Überlastung oder nicht-idempotente Operationen durch unkontrollierte Wiederholungen zu verschärfen.