# NPSPEC-NETWORK-UDP-0001 – Nova UDP

## Status

Angenommen

## Kategorie

Network / UDP / Transport Layer

## Zweck

NovaOS definiert UDP als verbindungsloses, nachrichtenorientiertes Transportprotokoll innerhalb des Nova Network Stack.

```text
Application
    ↓
UDP Datagram
    ↓
IPv4 / IPv6
    ↓
Network Interface
```

UDP ermöglicht geringe Protokollkomplexität und niedrige Latenz für Anwendungen, die keine durch TCP bereitgestellte Verbindungs-, Reihenfolge- oder Wiederholungslogik benötigen.

## Grundprinzip

```text
Sent ≠ Delivered
Delivered ≠ Processed
UDP ≠ Reliable
UDP Endpoint ≠ Permission
```

UDP garantiert weder Zustellung noch Reihenfolge noch Einmaligkeit eines Datagramms.

## UDP Endpoint

Ein UDP-Endpoint wird als Netzwerkobjekt behandelt.

```text
UDPEndpoint
├── EndpointID
├── Local Address
├── Local Port
├── State
└── Security Context
```

Optional:

```text
Remote Address
Remote Port
QoS
Resource Budget
```

Ein UDP-Endpoint darf mit einem festen Kommunikationspartner verbunden werden, ohne dadurch die grundlegende verbindungslose UDP-Semantik zu verändern.

## Datagramm-Modell

UDP erhält Nachrichtengrenzen.

```text
Application Message
       ↓
UDP Datagram
       ↓
Network
       ↓
UDP Datagram
       ↓
Application
```

Ein Datagramm wird als einzelne logische Einheit behandelt.

UDP darf nicht wie ein Byte-Stream interpretiert werden.

## Versand

Beim Versand werden mindestens benötigt:

```text
Source Port
Destination Address
Destination Port
Payload
```

Der Netzwerk-Stack übernimmt anschließend Routing und IPv4-/IPv6-Verarbeitung.

```text
UDP
 ↓
IP Routing
 ↓
Interface
```

Erfolgreiches lokales Senden bedeutet nicht, dass das Datagramm den Empfänger erreicht hat.

## Empfang

Eingehende Datagramme werden anhand ihrer Zielinformationen einem passenden UDP-Endpoint zugeordnet.

```text
IP Packet
   ↓
UDP Validation
   ↓
Endpoint Lookup
   ↓
Receive Queue
```

Existiert kein gültiger Empfänger, muss das Datagramm kontrolliert verworfen oder entsprechend der IP-/ICMP-Semantik behandelt werden.

## Checksummen

UDP-Prüfsummen müssen entsprechend der verwendeten IP-Version behandelt werden.

Hardware-Offloading darf verwendet werden, sofern der Stack den tatsächlichen Validierungszustand eindeutig kennt.

Fehlerhafte Datagramme dürfen nicht an Anwendungen weitergereicht werden.

## IPv4 und IPv6

UDP muss über IPv4 und IPv6 funktionieren.

```text
        UDP
       ↙   ↘
    IPv4   IPv6
```

Die gemeinsame UDP-Logik soll nicht unnötig zwischen beiden Protokollversionen dupliziert werden.

## Asynchrones I/O

Send und Receive sollen in das allgemeine NovaOS-I/O-Modell integriert werden.

```text
IORequest
   ↓
UDP
   ↓
Completion
```

Unterstützt werden sollen insbesondere:

```text
Send
SendTo
Receive
ReceiveFrom
```

Cancellation und Deadlines müssen auf wartende Operationen anwendbar sein.

## Backpressure

Receive- und Send-Queues müssen begrenzt sein.

```text
Incoming Datagrams
        ↓
Bounded Queue
        ↓
Application
```

Kann eine Anwendung Datagramme nicht schnell genug verarbeiten, darf die Queue nicht unbegrenzt wachsen.

Die Drop-Policy muss eindeutig definiert und introspektierbar sein.

## Zero-Copy

UDP darf Zero-Copy-Pfade verwenden.

```text
Application Buffer
       ↓
UDP / IP
       ↓
NIC
```

Buffer-Ownership und Lifetime müssen dabei erhalten bleiben.

Ein sicherer Copy-Pfad muss verfügbar sein.

## Broadcast und Multicast

UDP darf IPv4-Broadcast und IP-Multicast verwenden.

```text
UDP Sender
    ↓
Broadcast / Multicast
    ↓
Multiple Receivers
```

Die Nutzung muss durch Netzwerk- und Capability-Policy kontrollierbar sein.

## Sicherheit

Mindestens folgende Operationen müssen autorisierbar sein:

```text
Bind
Send
Receive
Broadcast
Multicast Join
```

Eingehende UDP-Datagramme sind grundsätzlich als nicht vertrauenswürdige externe Daten zu behandeln.

## Ressourcensteuerung

UDP muss Ressourcen begrenzen können für:

```text
Endpoints
Send Queues
Receive Queues
Datagram Size
Buffered Bytes
Multicast Memberships
```

Hohe Datagrammraten dürfen keinen unbegrenzten Speicher- oder CPU-Verbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
EndpointID
Local Address
Local Port
Remote Peer
Datagrams Sent
Datagrams Received
Dropped Datagrams
Queue Usage
Checksum Errors
```

Payload-Inhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS UDP über IPv4 und IPv6 unterstützen.
2. UDP MUSS Datagrammgrenzen erhalten.
3. UDP DARF Zustellung, Reihenfolge oder Einmaligkeit NICHT garantieren.
4. UDP-Endpunkte MÜSSEN eindeutig identifizierbar sein.
5. Eingehende Datagramme MÜSSEN vor Übergabe an Anwendungen validiert werden.
6. UDP-Prüfsummen MÜSSEN entsprechend der jeweiligen IP-Spezifikation behandelt werden.
7. Send- und Receive-Operationen SOLLEN asynchron ausführbar sein.
8. UDP-Queues MÜSSEN begrenzbar sein.
9. Überlastung DARF NICHT zu unbegrenztem Queue-Wachstum führen.
10. Broadcast- und Multicast-Nutzung MUSS durch Policy begrenzbar sein.
11. Zero-Copy DARF Ownership- oder Sicherheitsregeln NICHT umgehen.
12. UDP-Zustand, Queue-Nutzung und Statistiken SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0005`

## Ergebnis

```text
Application Datagram
        ↓
       UDP
        ↓
IPv4 / IPv6
        ↓
Nova Network Stack
```

NovaOS erhält damit eine schlanke, asynchrone und ressourcenkontrollierte UDP-Implementierung für verbindungslose Datagrammkommunikation über IPv4 und IPv6.