# NPSPEC-NETWORK-TCP-0001 – Nova TCP

## Status

Angenommen

## Kategorie

Network / TCP / Transport Layer

## Zweck

NovaOS definiert TCP als zuverlässiges, verbindungsorientiertes Transportprotokoll innerhalb des Nova Network Stack.

```text
Application
    ↓
TCP Connection
    ↓
IPv4 / IPv6
    ↓
Network Interface
```

TCP stellt geordnete Byte-Streams, Flusskontrolle, Überlastungssteuerung und zuverlässige Übertragung bereit.

## Grundprinzip

```text
Connection ≠ Permission
Sent ≠ Acknowledged
Acknowledged ≠ Processed by Application
TCP Reliability ≠ Application Transaction
```

TCP garantiert Transportsemantik, aber keine erfolgreiche Verarbeitung durch die Anwendung.

## TCP Connection

Eine Verbindung wird als eigenständiges Netzwerkobjekt behandelt.

```text
TCPConnection
├── ConnectionID
├── Local Endpoint
├── Remote Endpoint
├── State
├── Send State
└── Receive State
```

Optional:

```text
QoS
Security Context
ExecutionContract
Resource Budget
```

## Zustandsmodell

TCP muss die standardmäßigen Verbindungszustände abbilden können.

```text
CLOSED
  ↓
LISTEN
  ↓
SYN-SENT / SYN-RECEIVED
  ↓
ESTABLISHED
  ↓
FIN-WAIT / CLOSE-WAIT
  ↓
TIME-WAIT
  ↓
CLOSED
```

Zustandsübergänge müssen durch gültige TCP-Ereignisse gesteuert werden.

## Verbindungsaufbau

Der Aufbau erfolgt über den TCP-Handshake.

```text
Client              Server

SYN        →
           ←        SYN + ACK
ACK        →
```

Eine Verbindung darf erst nach erfolgreichem Handshake als `ESTABLISHED` gelten.

## Datenübertragung

TCP stellt Anwendungen einen geordneten Byte-Stream bereit.

```text
Application Data
      ↓
TCP Segments
      ↓
Network
      ↓
Reassembly
      ↓
Ordered Byte Stream
```

TCP darf keine Nachrichten- oder Record-Grenzen der Anwendung voraussetzen.

## Zuverlässigkeit

TCP muss mindestens unterstützen:

```text
Sequence Numbers
Acknowledgements
Retransmission
Duplicate Detection
Ordered Delivery
```

Verlorene Segmente müssen anhand definierter TCP-Mechanismen erneut übertragen werden können.

## Flow Control

TCP muss verhindern, dass ein Sender den Empfangspuffer des Kommunikationspartners unkontrolliert überlastet.

```text
Receiver Window
      ↓
Sender Limit
```

Flow Control und Congestion Control müssen getrennt behandelt werden.

## Congestion Control

TCP muss Netzwerküberlastung berücksichtigen.

```text
Send
 ↓
Observe ACK / Loss / Delay
 ↓
Adjust Sending Rate
```

Der konkrete Congestion-Control-Algorithmus soll austauschbar sein.

Der TCP-Kern darf nicht dauerhaft an einen einzelnen Algorithmus gekoppelt werden.

## Retransmission

Ausstehende Segmente müssen zeitlich überwacht werden.

```text
Segment Sent
     ↓
ACK?
 ├── Yes → Complete
 └── No  → Retransmission
```

Retransmission-Timer müssen begrenzt und gegen fehlerhafte oder extreme Netzwerkbedingungen robust sein.

## Buffer

TCP verwendet begrenzte Send- und Receive-Buffer.

```text
Application
    ↓
Send Buffer
    ↓
TCP
    ↓
Receive Buffer
    ↓
Application
```

Buffer dürfen nicht unbegrenzt wachsen.

Backpressure muss an die Anwendung weitergegeben werden können.

## Asynchrones I/O

TCP soll vollständig in das NovaOS-I/O-Modell integriert werden.

```text
IORequest
   ↓
TCP
   ↓
Pending
   ↓
Completion
```

Connect, Accept, Send und Receive müssen asynchron ausführbar sein.

Cancellation und Deadlines müssen unterstützt werden können.

## Zero-Copy

TCP darf Zero-Copy für Send- und Receive-Pfade verwenden.

```text
Application Buffer
       ↓
TCP / Network Stack
       ↓
NIC
```

Buffer-Lifetime und Ownership müssen dabei eindeutig bleiben.

Ein Copy-Fallback muss verfügbar sein.

## IPv4 und IPv6

TCP muss unabhängig von der verwendeten Network-Layer-Version arbeiten.

```text
        TCP
       ↙   ↘
    IPv4   IPv6
```

Gemeinsame TCP-Logik soll nicht unnötig zwischen IPv4 und IPv6 dupliziert werden.

## Sicherheit

TCP-Verbindungen müssen den NovaOS-Capability- und Netzwerk-Policies unterliegen.

Mindestens kontrollierbar sein müssen:

```text
Bind
Listen
Connect
Accept
Send
Receive
```

Eingehende TCP-Segmente sind als nicht vertrauenswürdige Netzwerkdaten zu behandeln.

## Ressourcensteuerung

TCP muss Ressourcen begrenzen können für:

```text
Connections
Listening Sockets
Pending Connections
Send Buffers
Receive Buffers
Retransmission State
TIME-WAIT State
```

Externe Systeme dürfen keinen unbegrenzten TCP-Zustand erzeugen können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ConnectionID
Local Endpoint
Remote Endpoint
State
Bytes Sent
Bytes Received
Retransmissions
Buffer Usage
RTT
Errors
```

Payload-Inhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS TCP über IPv4 und IPv6 unterstützen.
2. TCP MUSS zuverlässige und geordnete Byte-Streams bereitstellen.
3. TCP-Verbindungen MÜSSEN einen eindeutig bestimmbaren Zustand besitzen.
4. TCP MUSS Sequence Numbers, Acknowledgements und Retransmission unterstützen.
5. TCP MUSS Flow Control unterstützen.
6. TCP MUSS Congestion Control unterstützen.
7. Congestion-Control-Algorithmen SOLLEN austauschbar sein.
8. Send- und Receive-Buffer MÜSSEN begrenzbar sein.
9. TCP MUSS Backpressure an höhere Schichten weitergeben können.
10. Connect, Accept, Send und Receive SOLLEN asynchron ausführbar sein.
11. TCP-Ressourcen MÜSSEN gegen unkontrollierten Verbrauch geschützt werden.
12. TCP-Verbindungszustände und Statistiken SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0004`

## Ergebnis

```text
Application
    ↓
Reliable Byte Stream
    ↓
TCP
    ↓
IPv4 / IPv6
    ↓
Nova Network Stack
```

NovaOS erhält damit eine zuverlässige, asynchrone und ressourcenkontrollierte TCP-Implementierung, die IPv4 und IPv6 gemeinsam nutzt und vollständig in das NovaOS-I/O- und Netzwerkmodell integriert ist.