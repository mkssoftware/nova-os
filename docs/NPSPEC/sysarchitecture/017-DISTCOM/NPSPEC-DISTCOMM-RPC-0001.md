# NPSPEC-DISTCOMM-RPC-0001 – Nova Distributed Remote Procedure Call

## Status

Angenommen

## Kategorie

Distributed Communication / RPC / Service Communication

## Zweck

NovaOS definiert eine gemeinsame RPC-Infrastruktur für typisierte Aufrufe zwischen Diensten über Prozess-, System- und Netzwerkgrenzen hinweg.

```text
Caller
  ↓
RPC Request
  ↓
Transport
  ↓
Remote Service
  ↓
RPC Response
```

Distributed RPC baut auf dem NovaOS-IPC-Modell auf, erweitert dieses jedoch um Netzwerk-, Sicherheits-, Fehler- und Verteilungssemantik.

## Grundprinzip

```text
RPC ≠ Local Function Call
Remote Success ≠ Local Success
Reachable ≠ Authorized
Retry ≠ Safe
Transport Connection ≠ Service Identity
```

Verteilte Aufrufe müssen Netzwerkfehler, Teilfehler und unbekannte Ausführungszustände explizit behandeln.

## RPC-Modell

Ein verteilter RPC-Aufruf besitzt mindestens:

```text
DistributedRPC
├── CallID
├── ServiceID
├── MethodID
├── Request
├── State
└── Security Context
```

Optional:

```text
Deadline
Cancellation
ExecutionContract
Network Intent
Resource Budget
Trace Context
Idempotency Key
```

`ServiceID` und `MethodID` dürfen nicht an IP-Adresse, Port oder physischen Standort gebunden sein.

## Aufrufmodell

```text
Created
   ↓
Resolving
   ↓
Connecting
   ↓
Sending
   ↓
Executing
   ↓
Receiving
   ↓
Completed
```

Fehler dürfen zu:

```text
Failed
Cancelled
DeadlineExceeded
Unknown
```

führen.

`Unknown` bedeutet, dass nicht sicher festgestellt werden kann, ob die entfernte Operation ausgeführt wurde.

## Typisierte Schnittstellen

Distributed RPC soll die typisierte NovaOS-IPC-Infrastruktur verwenden.

```text
Service Interface
├── Method
│   ├── Input Type
│   └── Output Type
└── Version
```

Schemas müssen versionierbar sein.

Unbekannte optionale Felder sollen kompatibel behandelt werden können.

Unbekannte zwingend benötigte Semantik muss kontrolliert abgelehnt werden.

## Service Identity

Die logische Identität eines Dienstes bleibt unabhängig von seinem Netzwerkstandort.

```text
ServiceID
   ↓
Service Resolution
   ↓
Endpoint(s)
```

Dadurch können Dienste verschoben, repliziert oder über alternative Netzwerkpfade erreicht werden.

## Transport

RPC darf unterschiedliche Transportmechanismen verwenden.

```text
Nova RPC
├── HTTP
├── HTTP/2
├── HTTP/3
├── QUIC
└── Future Transports
```

Die RPC-Semantik darf nicht unnötig an einen einzelnen Transport gekoppelt sein.

## Call-Typen

NovaOS soll mindestens unterstützen können:

```text
Unary
Client Streaming
Server Streaming
Bidirectional Streaming
```

Streaming muss Backpressure unterstützen.

## Deadline und Cancellation

Deadlines und Cancellation müssen über die Kommunikationskette propagiert werden können.

```text
Caller
  ↓
RPC
  ↓
Transport
  ↓
Remote Execution
```

Cancellation bedeutet nicht automatisch, dass eine bereits gestartete entfernte Operation rückgängig gemacht wurde.

## Fehlersemantik

Fehler müssen strukturiert unterscheidbar sein.

```text
TransportError
AuthenticationError
AuthorizationError
ServiceUnavailable
DeadlineExceeded
Cancelled
RemoteError
ProtocolError
UnknownExecutionState
```

Transportfehler und fachliche Fehler dürfen nicht vermischt werden.

## Retry

Retries dürfen nur kontrolliert erfolgen.

```text
Call Failed
    ↓
Retry Safe?
 ├── Yes → Retry Policy
 └── No  → Report
```

NovaOS darf insbesondere berücksichtigen:

```text
Idempotency
Execution State
Deadline
Retry Budget
Network State
```

Nicht-idempotente Operationen dürfen nicht blind wiederholt werden.

## Sicherheit

RPC-Aufrufe müssen den NovaOS-Sicherheitskontext transportieren oder sicher auf einen entfernten Sicherheitskontext abbilden können.

```text
Caller Identity
      ↓
Authentication
      ↓
Remote Authorization
      ↓
Service Method
```

Transportverschlüsselung ersetzt keine Autorisierung.

Credentials und Capabilities dürfen nur entsprechend ihrer Delegationsregeln übertragen werden.

## Network Intent

RPC darf Network Intent verwenden.

```text
RPC Call
   ↓
Network Intent
   ↓
Routing / VPN / QoS / Sovereignty
```

Die Service-Semantik bleibt dadurch unabhängig vom konkreten Netzwerkpfad.

## Location Transparency

Ein Aufrufer soll dieselbe logische Serviceschnittstelle verwenden können, unabhängig davon, ob ein Dienst:

```text
Local Process
Local System Service
Remote Machine
Cluster Node
```

ist.

Location Transparency darf jedoch keine Security Transparency erzeugen.

Ein entfernter Aufruf darf stärkere Sicherheits- und Fehlersemantik benötigen als ein lokaler IPC-Aufruf.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Concurrent Calls
Pending Requests
Streams
Message Size
Buffers
Retries
Connections
Remote Execution Budget
```

Backpressure muss bis zum Aufrufer propagiert werden können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
CallID
ServiceID
MethodID
State
Endpoint
Transport
Duration
Retry Count
Deadline
Result State
Failure Reason
```

Credentials, Tokens und vertrauliche Payloads dürfen nicht ungefiltert ausgegeben werden.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Distributed-RPC-Infrastruktur bereitstellen können.
2. RPC-Schnittstellen SOLLEN typisiert und versionierbar sein.
3. Service-Identität MUSS unabhängig vom Netzwerkstandort sein können.
4. Distributed RPC DARF NICHT wie ein garantiert erfolgreicher lokaler Funktionsaufruf behandelt werden.
5. Deadlines und Cancellation MÜSSEN propagierbar sein.
6. `UnknownExecutionState` MUSS darstellbar sein.
7. Nicht-idempotente Operationen DÜRFEN NICHT blind wiederholt werden.
8. Streaming MUSS Backpressure unterstützen können.
9. Transportverschlüsselung DARF NICHT als Autorisierung interpretiert werden.
10. RPC MUSS Network Intent und Sovereignty-Regeln berücksichtigen können.
11. RPC-Ressourcen und Retries MÜSSEN begrenzt sein.
12. Aufrufzustände und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-RPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-HTTP-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0003`

## Ergebnis

```text
Typed Service Interface
          ↓
Distributed RPC
          ↓
Security + Execution Contract
          ↓
Network Intent
          ↓
HTTP / QUIC / Future Transport
          ↓
Remote Service
```

NovaOS erhält damit eine typisierte, transportunabhängige RPC-Infrastruktur, die lokale und entfernte Servicekommunikation konzeptionell vereinheitlicht, ohne die besonderen Fehler-, Sicherheits- und Ausführungssemantiken verteilter Systeme zu verbergen.