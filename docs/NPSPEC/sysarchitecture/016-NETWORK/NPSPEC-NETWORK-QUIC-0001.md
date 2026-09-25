# NPSPEC-NETWORK-QUIC-0001 – Nova QUIC

## Status

Angenommen

## Kategorie

Network / QUIC / Transport Layer

## Zweck

NovaOS definiert QUIC als sicheren, verbindungsorientierten Transport über UDP mit integrierter Verschlüsselung und mehreren unabhängigen Streams.

```text
Application
    ↓
QUIC Connection
    ↓
UDP
    ↓
IPv4 / IPv6
```

QUIC ergänzt TCP um einen modernen Transportpfad für niedrige Verbindungsaufbauzeiten, Stream-Multiplexing und Connection Migration.

## Grundprinzip

```text
QUIC ≠ TCP over UDP
Connection ≠ Network Path
Stream ≠ Connection
Encryption ≠ Authorization
```

Eine QUIC-Verbindung besitzt eine logische Identität, die nicht dauerhaft an eine einzelne IP-Adresse oder Netzwerkroute gebunden sein muss.

## QUIC Connection

Eine Verbindung wird als eigenständiges Netzwerkobjekt behandelt.

```text
QUICConnection
├── ConnectionID
├── Local Endpoint
├── Remote Endpoint
├── State
├── Security State
└── Streams
```

Optional:

```text
QoS
ExecutionContract
Resource Budget
Path State
```

## Verbindungsaufbau

QUIC kombiniert Transport- und kryptografischen Verbindungsaufbau.

```text
Client
  ↓
QUIC + TLS Handshake
  ↓
Server
  ↓
Secure Connection
```

Eine Verbindung darf erst als sicher etabliert gelten, wenn die erforderlichen kryptografischen Prüfungen erfolgreich abgeschlossen wurden.

## Streams

Eine QUIC-Verbindung darf mehrere unabhängige Streams enthalten.

```text
QUIC Connection
├── Stream 1
├── Stream 2
├── Stream 3
└── Stream N
```

Streams können unabhängig übertragen und verarbeitet werden.

Paketverlust in einem Stream soll andere unabhängige Streams nicht unnötig blockieren.

## Stream-Typen

QUIC muss mindestens unterstützen können:

```text
Bidirectional Stream
Unidirectional Stream
```

Jeder Stream besitzt einen eigenen logischen Zustand und eigene Flow-Control-Grenzen.

## Zuverlässigkeit

QUIC muss zuverlässige Übertragung innerhalb seiner Streams unterstützen.

```text
Send
 ↓
Packet Number
 ↓
ACK / Loss Detection
 ↓
Retransmission
```

Transportzustand und Paketverlust müssen unabhängig von UDP behandelt werden.

## Flow Control

Flow Control muss sowohl auf Connection- als auch auf Stream-Ebene möglich sein.

```text
Connection Limit
      ↓
Stream Limits
```

Ein einzelner Stream darf nicht automatisch sämtliche Ressourcen einer Verbindung verbrauchen.

## Congestion Control

QUIC muss Congestion Control unterstützen.

```text
ACK / Loss / RTT
      ↓
Congestion Controller
      ↓
Sending Rate
```

Der konkrete Congestion-Control-Algorithmus soll austauschbar sein.

## Verschlüsselung

QUIC verwendet TLS-basierte kryptografische Absicherung.

```text
QUIC
 ↓
TLS Security Context
 ↓
Encrypted Transport
```

Schlüsselmaterial muss durch die NovaOS-Sicherheitsinfrastruktur geschützt werden.

Transportverschlüsselung ersetzt keine Capability- oder Anwendungsautorisierung.

## Connection IDs

QUIC-Verbindungen dürfen durch Connection IDs unabhängig vom aktuellen Netzwerkpfad identifiziert werden.

```text
ConnectionID
     ↓
QUIC Connection
     ↓
Current Network Path
```

Connection IDs dürfen nicht als Sicherheitsberechtigung interpretiert werden.

## Connection Migration

Eine bestehende Verbindung darf auf einen neuen Netzwerkpfad wechseln.

```text
Wi-Fi
  ↓
QUIC Connection
  ↓
Ethernet
```

Ein neuer Pfad muss vor uneingeschränkter Nutzung validiert werden.

Migration darf bestehende Security- oder Sovereignty-Regeln nicht umgehen.

## Path Management

Eine QUIC-Verbindung soll Netzwerkpfade explizit verwalten können.

```text
Connection
├── Path A
├── Path B
└── Active Path
```

Pfadstatus, RTT, Erreichbarkeit und Validierungszustand sollen getrennt erfassbar sein.

## 0-RTT

QUIC darf 0-RTT-Verbindungswiederaufnahme unterstützen.

```text
Known Server
    ↓
0-RTT Data
```

0-RTT-Daten besitzen Replay-Risiken.

Operationen dürfen daher nur dann über 0-RTT ausgeführt werden, wenn ihre Semantik eine mögliche Wiederholung sicher erlaubt.

## Asynchrones I/O

QUIC muss in das NovaOS-I/O-Modell integrierbar sein.

```text
IORequest
   ↓
QUIC Stream
   ↓
Completion
```

Connect, Accept, Stream Open, Send und Receive sollen asynchron ausführbar sein.

## Backpressure

Connection- und Stream-Queues müssen begrenzbar sein.

```text
Application
    ↓
Bounded Stream Buffer
    ↓
QUIC
```

Flow Control und NovaOS-Backpressure müssen zusammenarbeiten können.

## Ressourcensteuerung

QUIC muss Ressourcen begrenzen können für:

```text
Connections
Streams
Packet Buffers
Crypto State
Pending Handshakes
Retransmissions
Connection IDs
Path State
```

Externe Systeme dürfen keinen unbegrenzten QUIC-Zustand erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ConnectionID
Connection State
Stream Count
Security State
Active Path
RTT
Packet Loss
Bytes Sent
Bytes Received
Congestion State
```

Schlüsselmaterial und geschützte Payloads dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS SOLL QUIC über UDP unterstützen.
2. QUIC MUSS über IPv4 und IPv6 funktionieren können.
3. QUIC MUSS mehrere unabhängige Streams pro Verbindung unterstützen.
4. Flow Control MUSS auf Connection- und Stream-Ebene möglich sein.
5. QUIC MUSS Congestion Control unterstützen.
6. Congestion-Control-Algorithmen SOLLEN austauschbar sein.
7. QUIC-Verbindungen MÜSSEN kryptografisch abgesichert werden.
8. Connection IDs DÜRFEN NICHT als Zugriffsberechtigung interpretiert werden.
9. Connection Migration MUSS neue Netzwerkpfade kontrolliert validieren.
10. 0-RTT DARF nur für Operationen verwendet werden, deren Semantik das Replay-Risiko berücksichtigt.
11. QUIC-Ressourcen MÜSSEN begrenzbar sein und Backpressure unterstützen.
12. Connection-, Stream-, Path- und Transportzustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-NETWORK-0006`

## Ergebnis

```text
Application
    ↓
QUIC Streams
    ↓
Secure QUIC Connection
    ↓
UDP
    ↓
IPv4 / IPv6
```

NovaOS erhält damit einen sicheren, multiplexfähigen und pfadunabhängigen Transportmechanismus, der moderne Netzwerkkommunikation über UDP ermöglicht und sich vollständig in die gemeinsame Netzwerk- und I/O-Architektur integriert.