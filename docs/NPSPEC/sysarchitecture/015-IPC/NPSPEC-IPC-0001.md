# NPSPEC-IPC-0001 – Nova Inter-Process Communication

## Status

Entwurf

## Kategorie

IPC / Core / Communication

## Zweck

NovaOS definiert ein einheitliches IPC-Modell für kontrollierte Kommunikation zwischen Prozessen, Diensten, Treibern und Systemkomponenten.

```text
Process A
    ↓
IPC
    ↓
Process B
```

IPC soll Kommunikation ermöglichen, ohne Speicherisolation, Capability-Grenzen oder Prozessidentitäten aufzuheben.

## Grundprinzip

```text
Communication ≠ Shared Authority
Endpoint Knowledge ≠ Permission
IPC ≠ Shared Address Space
```

Kommunikation erfolgt über explizite Endpoints und autorisierte Zugriffsrechte.

## IPC Endpoint

Ein Kommunikationsziel wird durch einen stabilen Endpoint repräsentiert.

```text
IPCEndpoint
├── EndpointID
├── Owner
├── Interface
├── Security Context
└── State
```

Optional:

```text
Queue Limits
QoS
Deadline Support
Location
```

Die Endpoint-Identität soll unabhängig von der aktuellen physischen Ausführungslocation bleiben.

## Kommunikationsmodelle

NovaOS soll mehrere IPC-Formen auf einem gemeinsamen Grundmodell unterstützen können.

```text
Message
Request / Response
Stream
Event
Shared Buffer
```

Subsysteme dürfen spezialisierte Interfaces darauf aufbauen.

## Nachrichten

Eine IPC-Nachricht besteht logisch aus:

```text
Message
├── MessageID
├── Type
├── Payload
├── Sender
└── Metadata
```

Optional:

```text
CorrelationID
Deadline
Priority
Capabilities
Shared Buffer References
```

Nachrichten müssen eindeutig validierbar sein.

## Request / Response

Synchrones und asynchrones Request/Response muss möglich sein.

```text
Client
  ↓ Request
Service
  ↓ Response
Client
```

Asynchrone Requests dürfen mit Futures, Completion-Mechanismen oder Events verbunden werden.

## Capability Transfer

Capabilities dürfen kontrolliert über IPC übertragen werden.

```text
Process A
   ↓
Restricted Capability
   ↓ IPC
Process B
```

Eine Übertragung darf keine Rechte erzeugen, die der Sender nicht delegieren darf.

## Shared Buffer

Größere Datenmengen sollen nicht zwingend in IPC-Nachrichten kopiert werden.

```text
Process A
    ↓
Shared Buffer
    ↓
Process B
```

Shared Buffers müssen explizit autorisiert werden und definierte Ownership-, Lifetime- und Zugriffsregeln besitzen.

## Zero-Copy

IPC soll Zero-Copy-Datenübertragung unterstützen können.

```text
Message
   ↓
Buffer Reference
   ↓
Mapped / Shared Buffer
```

Zero-Copy ist eine Optimierung und darf nicht Voraussetzung für korrekte IPC sein.

Ein sicherer Copy-Fallback muss möglich bleiben.

## Asynchronität

IPC darf unabhängig vom Lebenszyklus des aufrufenden Threads verarbeitet werden.

```text
Submit
  ↓
Queue
  ↓
Receiver
  ↓
Completion / Response
```

Blockierende und nicht blockierende APIs dürfen auf dasselbe IPC-Grundmodell abgebildet werden.

## Backpressure

IPC-Queues müssen begrenzbar sein.

```text
Sender
   ↓
Bounded Queue
   ↓
Receiver
```

Bei Überlastung müssen definierte Reaktionen möglich sein:

```text
Wait
Reject
Throttle
Drop
```

`Drop` darf nur verwendet werden, wenn die jeweilige IPC-Semantik dies ausdrücklich erlaubt.

## Sicherheit

Vor Kommunikation müssen mindestens folgende Aspekte prüfbar sein:

```text
Sender Identity
Endpoint Capability
Operation
Security Context
Delegated Capabilities
```

Ein Endpoint darf nicht allein durch Kenntnis seiner ID zugänglich werden.

## Prozess-Lifecycle

IPC muss Prozessende und Endpoint-Verlust kontrolliert behandeln.

```text
Process B Terminates
       ↓
Endpoint Closed
       ↓
Pending IPC → Failure / Cancellation
```

Wartende Prozesse dürfen nicht unbegrenzt auf verschwundene Endpoints warten.

## Location Transparency

IPC soll lokale und zukünftig entfernte Endpoints über ein gemeinsames logisches Modell adressieren können.

```text
EndpointID
   ↓
Resolution
   ├→ Local
   └→ Remote
```

Location Transparency darf Security-, Trust- oder Data-Sovereignty-Regeln nicht umgehen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
EndpointID
Owner
Interface
State
Queue Depth
Pending Messages
Transferred Bytes
Errors
Latency
```

Nicht autorisierte Nachrichteninhalte dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches IPC-Grundmodell bereitstellen.
2. IPC-Endpunkte MÜSSEN eindeutig identifizierbar sein.
3. Endpoint-Identität und Zugriffsberechtigung MÜSSEN getrennt bleiben.
4. IPC MUSS Prozess- und Speicherisolation erhalten.
5. Nachrichten MÜSSEN eindeutig validierbar sein.
6. Synchrones und asynchrones Request/Response MUSS abbildbar sein.
7. Capability-Übertragung MUSS explizit autorisiert und begrenzbar sein.
8. Shared Buffers MÜSSEN definierte Ownership- und Lifetime-Regeln besitzen.
9. IPC SOLL Zero-Copy unterstützen, MUSS aber einen sicheren Copy-Pfad ermöglichen.
10. IPC-Queues MÜSSEN begrenzbar sein und Backpressure unterstützen.
11. Endpoint-Verlust MUSS für wartende Kommunikationspartner erkennbar sein.
12. IPC-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-CONCURRENCY-ASYNC-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-IPC-0001`

## Ergebnis

```text
Process / Service
       ↓
Authorized Endpoint
       ↓
Message / Request / Shared Buffer
       ↓
Nova IPC
       ↓
Process / Service
```

NovaOS erhält damit ein einheitliches IPC-Modell für sichere, asynchrone und effiziente Kommunikation, ohne Prozessisolation oder Capability-Grenzen aufzugeben.