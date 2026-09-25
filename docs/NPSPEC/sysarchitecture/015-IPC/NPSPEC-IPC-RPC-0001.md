# NPSPEC-IPC-RPC-0001 – Nova Remote Procedure Call

## Status

Angenommen

## Kategorie

IPC / RPC / Request-Response

## Zweck

NovaOS definiert RPC als typisierten Request/Response-Mechanismus für den Aufruf von Operationen über IPC.

```text
Client
  ↓ Request
RPC Interface
  ↓
Service
  ↓ Response
Client
```

RPC soll lokale und zukünftig entfernte Services über dasselbe logische Interface ansprechbar machen, ohne Location, Sicherheit oder Fehlersemantik zu verbergen.

## Grundprinzip

```text
RPC ≠ Local Function Call
Request Sent ≠ Request Executed
Response Received ≠ Operation Successful
Remote ≠ Trusted
```

RPC muss Fehler, Abbruch, Timeout und Service-Ausfall als normale Zustände behandeln.

## RPC Interface

Ein RPC-Service stellt ein typisiertes Interface bereit.

```text
RPCInterface
├── InterfaceID
├── Version
└── Operations
```

Eine Operation definiert mindestens:

```text
OperationID
Request Type
Response Type
Error Type
```

Optional:

```text
Deadline
Cancellation
Execution Contract
Idempotency
```

## RPC Call

Ein Aufruf besitzt eine eindeutige Identität.

```text
RPCCall
├── CallID
├── Interface
├── Operation
├── Request
└── State
```

Optional:

```text
Deadline
Priority
Security Context
ExecutionContract
```

`CallID` dient der Zuordnung von Request, Response, Cancellation und Fehlern.

## Lifecycle

```text
Created
   ↓
Submitted
   ↓
Pending
   ↓
Executing
   ↓
Completed
```

Alternative Endzustände:

```text
Failed
Cancelled
Timeout
Unavailable
```

Ein unbekannter Ausführungszustand muss darstellbar sein.

## Synchron und Asynchron

RPC muss sowohl synchron als auch asynchron nutzbar sein.

```text
Synchronous:
Call → Wait → Response

Asynchronous:
Call → Future / Completion → Response
```

Die synchrone API soll auf dem asynchronen Grundmodell aufbauen können.

## Typed RPC

Requests und Responses müssen mit dem definierten Interface kompatibel sein.

```text
Request<T>
    ↓
Operation
    ↓
Response<R>
```

Schema- und Interface-Versionen müssen vor Verarbeitung validierbar sein.

## Capability-Modell

Ein RPC-Aufruf benötigt eine passende IPC-Capability.

```text
Caller
  ↓
Capability<Service.Operation>
  ↓
RPC Call
```

Der Besitz einer Service- oder Endpoint-ID darf keine Aufrufberechtigung erzeugen.

Capabilities müssen einzelne Interfaces oder Operationen begrenzen können.

## Deadlines und Timeouts

RPC muss Deadlines und Timeouts unterscheiden.

```text
Deadline = gewünschter spätester Abschlusszeitpunkt
Timeout  = maximale lokale Wartezeit
```

Das Ende der lokalen Wartezeit bedeutet nicht automatisch, dass die entfernte Operation nicht ausgeführt wurde.

## Cancellation

RPC-Aufrufe müssen nach Möglichkeit abbrechbar sein.

```text
Client
  ↓ Cancel
RPC Runtime
  ↓
Service
```

Cancellation ist eine Anfrage und darf nicht automatisch als erfolgreiche Beendigung interpretiert werden.

Der tatsächliche Endzustand muss ermittelbar sein.

## Idempotenz

Operationen sollen deklarieren können, ob wiederholte Ausführung sicher ist.

```text
Idempotent
Non-Idempotent
Unknown
```

NovaOS darf einen RPC-Aufruf nicht automatisch wiederholen, wenn dadurch eine nicht-idempotente Operation mehrfach ausgeführt werden könnte.

`Unknown` muss wie nicht nachgewiesene Idempotenz behandelt werden.

## Fehler

RPC muss mindestens unterscheiden können:

```text
Transport Failure
Service Unavailable
Permission Denied
Invalid Request
Remote Error
Timeout
Cancellation
Deadline Miss
Protocol Error
```

Transportfehler und fachliche Servicefehler dürfen nicht vermischt werden.

## Zero-Copy

Große RPC-Payloads dürfen Shared Buffers oder Shared Memory verwenden.

```text
RPC Request
    ↓
Buffer Reference
    ↓
Shared Memory
```

Dabei gelten weiterhin die normalen Capability-, Ownership- und Lifetime-Regeln.

## Location Transparency

RPC soll lokale und entfernte Services über dasselbe logische Interface unterstützen können.

```text
ServiceID
   ↓
Resolution
  ├→ Local IPC
  └→ Remote Transport
```

Location Transparency darf Unterschiede bei Latenz, Ausfallwahrscheinlichkeit, Trust und Data Sovereignty nicht verbergen.

## Ressourcensteuerung

RPC muss begrenzbar sein hinsichtlich:

```text
Concurrent Calls
Queue Depth
Request Size
Response Size
Execution Time
Memory
Bandwidth
```

Überlastete Services müssen Backpressure oder kontrollierte Ablehnung verwenden können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
CallID
Interface
Operation
Caller
Service
State
Latency
Deadline
Error
```

Payload-Inhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS typisierte RPC-Kommunikation unterstützen können.
2. RPC-Aufrufe MÜSSEN eindeutig identifizierbar sein.
3. Request-, Response- und Fehlertypen MÜSSEN durch das RPC-Interface definierbar sein.
4. RPC MUSS synchrone und asynchrone Nutzung ermöglichen.
5. RPC-Aufrufe MÜSSEN durch passende Capabilities autorisiert werden.
6. Service- oder Endpoint-Kenntnis DARF NICHT automatisch Aufrufrechte gewähren.
7. Timeout, Cancellation und tatsächlicher Ausführungszustand MÜSSEN getrennt behandelt werden.
8. RPC-Operationen SOLLEN ihre Idempotenz deklarieren können.
9. Nicht nachgewiesen idempotente Operationen DÜRFEN NICHT automatisch wiederholt werden.
10. Transport- und Servicefehler MÜSSEN unterscheidbar sein.
11. Location Transparency DARF Security-, Trust- oder Sovereignty-Regeln NICHT umgehen.
12. RPC-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IPC-CHANNEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-IPC-0009`

## Ergebnis

```text
Client
  ↓
Typed RPC Request
  ↓
Capability Check
  ↓
Local / Remote Service
  ↓
Typed Response
  ↓
Client
```

NovaOS erhält damit ein einheitliches RPC-Modell für typisierte Request/Response-Kommunikation mit expliziter Autorisierung, Fehlersemantik, Cancellation und Location Transparency.