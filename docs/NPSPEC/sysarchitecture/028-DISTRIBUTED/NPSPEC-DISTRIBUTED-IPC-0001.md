# NPSPEC-DISTRIBUTED-IPC-0001 – Nova Distributed IPC

## Status

Angenommen

## Kategorie

Distributed / IPC / Communication

## Zweck

NovaOS erweitert das lokale IPC-Modell auf Kommunikation über System- und Knotengrenzen hinweg.

Lokale und entfernte Kommunikation sollen dasselbe semantische Nachrichten-, Capability- und Execution-Modell verwenden, ohne Netzwerkadressen oder Transportprotokolle zu logischen Identitäten zu machen.

```text
Sender
  ↓
Semantic IPC
  ↓
Endpoint Resolution
  ↓
Local / Remote Transport
  ↓
Receiver
```

## Grundprinzipien

```text
IPC Identity ≠ Network Address
Endpoint ≠ Location
Connection ≠ Authority
Transport Security ≠ Authorization
Remote IPC ≠ Trusted IPC
Message Delivery ≠ Message Processing
Timeout ≠ Operation Failure
Location Transparency ≠ Authority Transparency
```

## Distributed IPC Model

Eine verteilte IPC-Verbindung wird logisch beschrieben durch:

```text
DistributedIPCChannel
├── ChannelID
├── Source Identity
├── Target Identity
├── InterfaceID
├── Security Context
└── State
```

Optional:

```text
ExecutionID
TraceID
TransactionID
Deadline
Schema Version
Transport
Location
Trust Requirements
Sovereignty Constraints
QoS
Backpressure Policy
```

## Einheitliches IPC-Modell

Aufrufer sollen grundsätzlich dieselbe semantische Operation verwenden können:

```text
Semantic Message
      ↓
IPC Resolution
      ↓
Local?
├── Yes → Local IPC
└── No  → Distributed IPC
```

Die Semantik einer Operation darf nicht vom verwendeten Transport abhängen.

## Endpoint Resolution

Kommunikation erfolgt über stabile Identitäten.

```text
Target Identity
      ↓
Resolution
      ↓
Current Endpoint
```

Endpoints können sich ändern, ohne dass sich die logische Identität ändert.

```text
Identity ≠ IP Address
Identity ≠ Port
Identity ≠ DNS Name
```

## Semantic Messages

Distributed IPC verwendet das Nova Semantic IPC Model.

```text
SemanticMessage
├── MessageTypeID
├── OperationID
├── Schema Version
└── Semantic Types
```

Optional:

```text
ObjectID
ExecutionContractID
TransactionID
TraceID
Deadline
Capability References
```

## Serialization

Semantik und Serialisierung bleiben getrennt.

```text
Semantic Message
      ↓
Serialization
      ↓
Transport
      ↓
Deserialization
      ↓
Semantic Message
```

```text
Schema ≠ Serialization
```

Unbekannte erforderliche Felder oder inkompatible Schema-Versionen dürfen nicht stillschweigend ignoriert werden.

## Transport Independence

Distributed IPC kann unterschiedliche Transporte verwenden.

Beispiele:

```text
TCP
QUIC
Shared Transport
Message Bus
Specialized Interconnect
```

Der Transport ist Implementierungsdetail und darf nicht Teil der logischen IPC-Identität werden.

## Capability Transfer

Capabilities dürfen nur explizit über Systemgrenzen übertragen oder delegiert werden.

```text
Local Capability
      ↓
Delegation Policy
      ↓
Attenuation
      ↓
Remote Capability
```

Remote Capabilities sollen:

```text
Minimal
Scoped
Revocable
Time-Bound
Task-Bound
```

sein.

```text
Remote Connection ≠ Capability
```

## Security

Vor Nachrichtenverarbeitung müssen relevante Eigenschaften geprüft werden:

```text
Peer Identity
Authentication
Message Integrity
Capabilities
Security Policy
```

Transportverschlüsselung allein erzeugt keine Authority.

## Trust

Remote Teilnehmer können Trust Requirements unterliegen.

```text
Peer Identity
Attestation
Software Provenance
Trust State
Revocation State
```

```text
Authenticated ≠ Trusted
Unknown Trust ≠ Trusted
```

## Sovereignty

Endpoint Resolution und Transportauswahl müssen Sovereignty Constraints berücksichtigen.

```text
Message
   ↓
Allowed Communication Domains
   ↓
Eligible Route / Endpoint
```

Auch Relay-, Proxy- oder Zwischensysteme dürfen relevante Hard Sovereignty Constraints nicht umgehen.

## Deadlines

Deadlines werden über IPC-Grenzen propagiert.

```text
Execution Deadline
      ↓
IPC Budget
      ↓
Remote Processing Budget
      ↓
Response Budget
```

Ein Empfänger darf eine bereits abgelaufene Deadline erkennen und entsprechend der Contract Policy reagieren.

## Cancellation

Cancellation muss über verteilte IPC propagierbar sein.

```text
Parent Task Cancel
      ↓
IPC Cancellation
      ↓
Remote Task Cancel
```

Cancellation bestätigt jedoch nicht automatisch, dass entfernte Seiteneffekte verhindert wurden.

## Backpressure

Distributed IPC muss Überlastung kontrollieren.

```text
Producer
   ↓
Queue
   ↓
Consumer
```

Mögliche Mechanismen:

```text
Bounded Queues
Flow Control
Rate Limiting
Credits
Batching
Priority
Controlled Dropping
```

Unbegrenzte Queues sind zu vermeiden.

## Delivery Semantics

Die Delivery-Semantik muss explizit sein.

Beispiele:

```text
AtMostOnce
AtLeastOnce
ApplicationDefined
```

Exactly-once darf nur behauptet werden, wenn die gesamte Verarbeitungskette diese Eigenschaft tatsächlich gewährleistet.

## Retry

```text
Retry ≠ Safe
```

Vor einem Retry müssen berücksichtigt werden:

```text
Operation Semantics
Idempotency
Previous Execution State
Deadline
Transaction State
```

## Unknown Execution State

Bei Kommunikationsverlust kann gelten:

```text
Request Sent
      ↓
Connection Lost
      ↓
UnknownExecutionState
```

NovaOS darf daraus nicht automatisch ableiten:

```text
Operation Failed
```

Der entfernte Teilnehmer könnte die Operation bereits ausgeführt haben.

## Reconnection

Nach Wiederherstellung einer Verbindung kann NovaOS:

```text
Reauthenticate
Revalidate Capabilities
Revalidate Trust
Resolve Endpoint
Synchronize State
Resume
Retry
Fail
```

Alte Verbindungen oder Credentials dürfen nicht automatisch weiterverwendet werden, wenn ihre Gültigkeit nicht mehr bestätigt ist.

## Zero-Copy

Innerhalb eines Knotens kann Zero-Copy verwendet werden.

Über Knotengrenzen kann NovaOS Datenpfade optimieren durch:

```text
Scatter/Gather
DMA
Pinned Buffers
Transport Offload
Direct Device Transfer
```

Die logische IPC-Semantik bleibt davon unabhängig.

## Distributed Execution

Distributed IPC bildet den Kommunikationspfad zwischen verteilten Tasks.

```text
Task A
  ↓
Distributed IPC
  ↓
Task B
```

Dabei werden relevante Kontexte propagiert:

```text
ExecutionID
TraceID
Deadline
Cancellation
Security Context
```

## Observability

Nachrichten können über Systemgrenzen hinweg korreliert werden.

```text
TraceID
├── Local IPC
├── Network Transport
├── Remote IPC
└── Remote Execution
```

Trace-Informationen erzeugen keine Authority.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ChannelID
Source Identity
Target Identity
Endpoint
Transport
Schema Version
Security State
Trust State
Sovereignty State
Deadline
Backpressure State
Delivery State
Retry State
TraceID
```

Sensible Netzwerk- und Security-Informationen dürfen nur entsprechend ihrer Policy offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS lokale und verteilte IPC über ein gemeinsames semantisches Kommunikationsmodell unterstützen.
2. IPC-Identitäten MÜSSEN unabhängig von Netzwerkadressen und Transporten bleiben.
3. Endpoint Resolution DARF keine Authority erzeugen.
4. Distributed IPC MUSS Semantic Types und versionierte Schemas unterstützen.
5. Serialization und Message Semantics MÜSSEN getrennte Konzepte bleiben.
6. Capability Transfer über Systemgrenzen MUSS explizit, kontrolliert und attenuierbar sein.
7. Transportverschlüsselung DARF NICHT mit Authorization oder Trust gleichgesetzt werden.
8. Trust- und Sovereignty-Anforderungen MÜSSEN bei Remote IPC berücksichtigt werden.
9. Deadlines und Cancellation MÜSSEN über IPC-Grenzen propagierbar sein.
10. Distributed IPC MUSS Backpressure unterstützen.
11. Delivery Semantics MÜSSEN explizit definiert sein.
12. Retries DÜRFEN nur unter Berücksichtigung von Idempotency und Execution State erfolgen.
13. Kommunikationsverlust DARF NICHT automatisch als bestätigtes Operation Failure interpretiert werden.
14. Trace- und IPC-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-SEMANTIC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-SERIALIZATION-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `NPSPEC-DISTCOMM-TRACE-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0062`

## Ergebnis

```text
Semantic IPC
      ↓
Identity-Based Resolution
      ↓
Capability + Security Validation
      ↓
Trust + Sovereignty Validation
      ↓
Local / Remote Transport
      ↓
Backpressure + Deadline Control
      ↓
Remote Semantic Processing
```

NovaOS erhält damit ein einheitliches Distributed-IPC-Modell, bei dem Kommunikation unabhängig von konkreten Netzwerkadressen und Transporten bleibt und gleichzeitig Capabilities, Security, Trust, Sovereignty, Deadlines, Backpressure und Fehlerzustände über Systemgrenzen hinweg kontrolliert werden.