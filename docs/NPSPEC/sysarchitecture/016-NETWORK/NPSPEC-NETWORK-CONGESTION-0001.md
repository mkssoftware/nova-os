# NPSPEC-NETWORK-CONGESTION-0001 – Nova Network Congestion Control

## Status

Angenommen

## Kategorie

Network / Congestion Control / Traffic Management

## Zweck

NovaOS definiert eine gemeinsame Architektur zur Erkennung und kontrollierten Behandlung von Netzwerküberlastung.

```text
Network Flow
     ↓
Congestion State
     ↓
Transport / QoS / Scheduler
     ↓
Adjusted Traffic
```

Congestion Control verhindert, dass Sender oder lokale Netzwerkkomponenten einen begrenzten Netzwerkpfad dauerhaft überlasten.

## Grundprinzip

```text
Congestion ≠ Packet Loss
Congestion Control ≠ Flow Control
High Bandwidth ≠ Available Capacity
Priority ≠ Congestion Exemption
```

Flow Control schützt primär den Empfänger. Congestion Control schützt den gemeinsamen Netzwerkpfad vor Überlastung.

## Congestion State

NovaOS soll Überlastungsinformationen als gemeinsamen Zustand darstellen können.

```text
CongestionState
├── FlowID
├── PathID
├── State
├── Observed Latency
├── Loss
└── Effective Rate
```

Optional:

```text
RTT
Queue Pressure
ECN State
Bandwidth Estimate
Algorithm
Timestamp
```

## Überlastungserkennung

Congestion darf anhand mehrerer Signale erkannt werden.

```text
Packet Loss
RTT Increase
Queue Growth
ECN
ACK Behavior
Bandwidth Estimate
```

Ein einzelnes Signal darf nicht grundsätzlich als einzige Ursache für Überlastung interpretiert werden.

## Zustandsmodell

Ein Netzwerkpfad darf mindestens folgende Zustände besitzen:

```text
Normal
  ↓
Pressure
  ↓
Congested
  ↓
Severe
```

Der Zustand darf sich pro Flow, Transportverbindung oder Netzwerkpfad unterscheiden.

## Transport-Integration

Transportprotokolle dürfen eigene Congestion-Control-Algorithmen verwenden.

```text
TCP  ─┐
QUIC ─┼→ Congestion Control
UDP  ─┘
```

TCP und QUIC müssen ihre jeweiligen Congestion-Control-Mechanismen kontrolliert integrieren.

UDP-basierte Anwendungen dürfen eigene Verfahren verwenden, sofern System- und Ressourcenregeln eingehalten werden.

## Austauschbare Algorithmen

Congestion-Control-Algorithmen sollen austauschbar sein.

```text
Transport
   ↓
Congestion Interface
   ↓
Algorithm
```

Der Netzwerk-Stack darf nicht dauerhaft an einen einzelnen Algorithmus gekoppelt werden.

Die Auswahl darf von Transport, Netzwerkpfad, Policy und Execution Contract abhängen.

## Rate Adaptation

Bei erkannter Überlastung muss die Senderate angepasst werden können.

```text
Observed Network State
        ↓
Congestion Controller
        ↓
Effective Send Rate
```

Anpassungen sollen stabil erfolgen und unnötige starke Schwankungen vermeiden.

## ECN

NovaOS soll Explicit Congestion Notification unterstützen können.

```text
Network
   ↓
ECN Signal
   ↓
Transport
   ↓
Rate Adjustment
```

ECN erlaubt die Signalisierung von Überlastung ohne zwingenden Paketverlust.

ECN-Informationen müssen validiert und entsprechend der jeweiligen Protokollsemantik verarbeitet werden.

## QoS

Congestion Control und Network QoS müssen zusammenarbeiten.

```text
Flow + QoS
    ↓
Congestion State
    ↓
Effective Scheduling
```

Hohe Priorität darf Congestion Control nicht außer Kraft setzen.

QoS kann die Verteilung knapper Ressourcen beeinflussen, aber keine zusätzliche physische Netzwerkkapazität erzeugen.

## Multipath

Überlastung eines Pfades darf Multipath-Entscheidungen beeinflussen.

```text
Path A → Congested
Path B → Available
          ↓
     Path Selection
```

Ein alternativer Pfad darf nur verwendet werden, wenn Routing-, Sicherheits-, Trust- und Sovereignty-Regeln erfüllt bleiben.

## Backpressure

Lokale Überlastung muss an höhere Schichten weitergegeben werden können.

```text
Network Pressure
      ↓
Transport
      ↓
Network Flow
      ↓
Application
```

Unbegrenztes Puffern darf nicht als Ersatz für Congestion Control verwendet werden.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Packet Queues
Congestion State
Measurements
Pending Packets
Retransmissions
Per-Flow State
```

Überlastung darf nicht zu unbegrenztem Speicher- oder CPU-Verbrauch führen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
FlowID
PathID
Congestion State
Algorithm
RTT
Packet Loss
ECN State
Estimated Bandwidth
Effective Send Rate
Queue Pressure
```

Angeforderte und tatsächlich verfügbare Netzwerkleistung müssen unterscheidbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS Netzwerküberlastung kontrolliert behandeln können.
2. Congestion Control und Flow Control MÜSSEN getrennte Mechanismen bleiben.
3. TCP und QUIC MÜSSEN Congestion-Control-Mechanismen integrieren können.
4. Congestion-Control-Algorithmen SOLLEN austauschbar sein.
5. Paketverlust DARF NICHT als einzig mögliches Congestion-Signal vorausgesetzt werden.
6. NovaOS SOLL ECN unterstützen können.
7. Congestion Control MUSS mit Network QoS zusammenarbeiten können.
8. Priorität DARF Congestion Control NICHT außer Kraft setzen.
9. Überlastungszustände SOLLEN Multipath-Entscheidungen beeinflussen können.
10. Unbegrenztes Puffern DARF NICHT als Überlastungsstrategie verwendet werden.
11. Congestion-State und Messressourcen MÜSSEN begrenzbar sein.
12. Congestion-Zustand, Algorithmus und effektive Senderate SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0020`

## Ergebnis

```text
Network Measurements
        ↓
Congestion Detection
        ↓
Transport Controller
        ↓
Rate / Queue / Path Adaptation
        ↓
Stable Network Flow
```

NovaOS erhält damit eine gemeinsame Congestion-Control-Architektur, die Überlastung erkennt und Transport, QoS, Backpressure und Multipath koordiniert, ohne einzelne Algorithmen fest in den Netzwerk-Stack einzubauen.