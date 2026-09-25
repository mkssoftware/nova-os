# NPSPEC-DISTCOMM-HTTP-0001 – Nova HTTP

## Status

Angenommen

## Kategorie

Distributed Communication / HTTP / Application Protocol

## Zweck

NovaOS definiert eine gemeinsame HTTP-Infrastruktur für Systemdienste, Capabilities und Anwendungen.

```text
Application / Capability
        ↓
Nova HTTP
        ↓
Transport
   ┌────┴────┐
  TCP       QUIC
```

HTTP wird als Protokollschicht oberhalb der Transportinfrastruktur behandelt und nicht direkt an einen bestimmten Transport gebunden.

## Grundprinzip

```text
HTTP ≠ Transport
URL ≠ Identity
HTTP Success ≠ Operation Success
Encryption ≠ Authorization
Connection ≠ Request
```

HTTP-Kommunikation muss die Sicherheits-, Ressourcen- und Netzwerkregeln von NovaOS respektieren.

## HTTP-Modell

Eine HTTP-Operation wird als strukturierter Request modelliert.

```text
HTTPRequest
├── RequestID
├── Method
├── Target
├── Headers
├── Body
└── State
```

Optional:

```text
Security Context
ExecutionContract
Network Intent
Deadline
Resource Budget
Cancellation
```

Die Antwort wird separat modelliert:

```text
HTTPResponse
├── Status
├── Headers
├── Body
└── Metadata
```

## HTTP-Versionen

NovaOS soll mindestens folgende Protokollgenerationen unterstützen können:

```text
HTTP/1.1
HTTP/2
HTTP/3
```

Die öffentliche HTTP-Schnittstelle soll möglichst unabhängig von der verwendeten HTTP-Version bleiben.

Die konkrete Version darf anhand von:

```text
Server Capability
Transport
Policy
Security
Performance
Network Conditions
```

ausgewählt werden.

## Transport

HTTP verwendet die gemeinsame NovaOS-Transportinfrastruktur.

```text
HTTP/1.1 → TCP
HTTP/2   → TCP
HTTP/3   → QUIC
```

Transportdetails dürfen Anwendungen nicht unnötig offengelegt werden.

Ein Wechsel der Transportimplementierung darf die logische HTTP-Operation nicht verändern.

## Asynchrones Modell

HTTP-Operationen müssen asynchron ausführbar sein.

```text
Request
   ↓
Resolve
   ↓
Connect
   ↓
Send
   ↓
Receive
   ↓
Completion
```

Cancellation und Deadlines müssen in den gesamten Request-Lifecycle propagiert werden können.

## Streaming

Request- und Response-Bodies dürfen als Streams verarbeitet werden.

```text
Producer
   ↓
HTTP Body Stream
   ↓
Transport
```

Große Inhalte sollen nicht vollständig im Speicher vorgehalten werden müssen.

Backpressure muss zwischen Anwendung, HTTP-Schicht und Transport propagiert werden können.

## Multiplexing

HTTP/2 und HTTP/3 dürfen mehrere logische Requests über eine gemeinsame Verbindung übertragen.

```text
Connection
├── Request A
├── Request B
└── Request C
```

Request-Lifecycle und Fehlerzustände müssen trotzdem getrennt bleiben.

## Verbindungspooling

NovaOS darf bestehende HTTP-Verbindungen wiederverwenden.

```text
Origin
  ↓
Connection Pool
  ↓
Requests
```

Pooling muss Sicherheitskontext, Isolation, Zielidentität und Network Namespace berücksichtigen.

Verbindungen unterschiedlicher inkompatibler Sicherheitsdomänen dürfen nicht unkontrolliert gemeinsam verwendet werden.

## Sicherheit

HTTPS muss über die gemeinsame TLS-Infrastruktur integrierbar sein.

```text
HTTP
 ↓
TLS
 ↓
TCP
```

beziehungsweise:

```text
HTTP/3
 ↓
QUIC + TLS
```

Zertifikatsprüfung, Serveridentität und Trust Policy dürfen nicht von einzelnen Anwendungen beliebig umgangen werden, sofern Systempolicy dies untersagt.

HTTP-Header und Bodies gelten grundsätzlich als externe, nicht vertrauenswürdige Daten.

## Network Intent

HTTP-Requests dürfen einen Network Intent besitzen.

```text
HTTP Request
     ↓
Network Intent
     ↓
Routing / VPN / Sovereignty / QoS
```

Dadurch kann eine Anwendung Anforderungen beschreiben, ohne konkrete Interfaces oder Netzwerkpfade auszuwählen.

## Redirects

HTTP-Redirects müssen kontrolliert verarbeitet werden.

```text
Original Target
      ↓
Redirect
      ↓
New Target
      ↓
Policy Revalidation
```

Ein Redirect darf Sicherheits-, Sovereignty- oder Berechtigungsregeln nicht umgehen.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Concurrent Requests
Connections
Streams
Header Size
Body Size
Buffers
Connection Pools
Redirects
Retries
```

Serverantworten dürfen keinen unbegrenzten Speicher- oder Ressourcenverbrauch verursachen.

Retries müssen begrenzt sein und die Semantik der HTTP-Methode berücksichtigen.

## Zero-Copy

HTTP darf die NovaOS-Zero-Copy-Infrastruktur für große Bodies verwenden.

```text
Storage / Application Buffer
          ↓
HTTP
          ↓
Transport
```

Zero-Copy darf Ownership, Verschlüsselung oder Speicherisolation nicht verletzen.

Ein sicherer Copy-Fallback muss verfügbar bleiben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Method
Target
HTTP Version
State
Transport
Duration
Bytes Sent
Bytes Received
Status
Failure Reason
```

Credentials, Tokens, Cookies und andere Geheimnisse dürfen nicht ungefiltert über Introspection ausgegeben werden.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame HTTP-Infrastruktur bereitstellen können.
2. HTTP MUSS von der konkreten Transportimplementierung getrennt bleiben.
3. HTTP/1.1, HTTP/2 und HTTP/3 SOLLEN unterstützt werden können.
4. HTTP-Operationen MÜSSEN asynchron ausführbar sein.
5. Cancellation und Deadlines MÜSSEN propagiert werden können.
6. Request- und Response-Bodies MÜSSEN streambar sein.
7. Streaming MUSS Backpressure unterstützen können.
8. Verbindungspooling MUSS Sicherheits- und Namespace-Grenzen respektieren.
9. Redirects MÜSSEN gegen geltende Policies neu geprüft werden.
10. HTTP DARF Network-, Firewall- oder Sovereignty-Regeln NICHT umgehen.
11. HTTP-Ressourcen, Retries und Redirects MÜSSEN begrenzt sein.
12. HTTP-Zustände und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-ZEROCOPY-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0001`

## Ergebnis

```text
Application / Capability
        ↓
HTTP Request / Stream
        ↓
Nova HTTP
        ↓
HTTP/1.1 / HTTP/2 / HTTP/3
        ↓
TCP / QUIC
        ↓
Nova Network
```

NovaOS erhält damit eine gemeinsame, transportunabhängige HTTP-Infrastruktur für Anwendungen und Systemdienste, die moderne HTTP-Versionen, Streaming, Backpressure, Network Intent und die Sicherheits- und Ressourcenmodelle von NovaOS integriert.