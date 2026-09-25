# NPSPEC-NETWORK-STACK-0001 – Nova Network Stack

## Status

Angenommen

## Kategorie

Network / Core / Protocol Stack

## Zweck

NovaOS definiert einen modularen Netzwerk-Stack für lokale und entfernte Kommunikation.

```text
Application / Capability
        ↓
      Socket
        ↓
 Transport Protocol
        ↓
 Network Protocol
        ↓
 Network Interface
        ↓
      Driver
        ↓
     Hardware
```

Der Netzwerk-Stack stellt gemeinsame Mechanismen für Protokolle, Interfaces, Routing, Paketverarbeitung und Netzwerk-I/O bereit.

## Grundprinzip

```text
Network Access ≠ Network Authority
Interface ≠ Physical Device
Protocol ≠ Policy
Packet Received ≠ Packet Trusted
```

Netzwerkmechanismen und Sicherheits-/Routing-Policy müssen getrennt bleiben.

## Architektur

Der Stack soll modular aufgebaut sein.

```text
Application Layer
      ↓
Transport Layer
      ↓
Network Layer
      ↓
Link Layer
      ↓
Driver Layer
```

Protokollmodule dürfen unabhängig entwickelt, ersetzt und erweitert werden, sofern ihre definierten Interfaces erhalten bleiben.

## Netzwerkinterfaces

Ein Netzwerkinterface wird als logisches Systemobjekt behandelt.

```text
NetworkInterface
├── InterfaceID
├── Type
├── State
├── Addresses
├── MTU
└── Capabilities
```

Ein Interface kann physisch oder virtuell sein.

Beispiele:

```text
Ethernet
Wi-Fi
Loopback
VPN
Bridge
Virtual Interface
```

Interface-Identität darf nicht von einem konkreten Gerätenamen abhängen.

## Paketverarbeitung

Eingehende Pakete durchlaufen eine definierte Verarbeitungskette.

```text
Driver
  ↓
Packet Validation
  ↓
Link Layer
  ↓
Network Layer
  ↓
Transport Layer
  ↓
Endpoint
```

Ausgehende Pakete verwenden den umgekehrten logischen Pfad.

Ungültige Pakete müssen möglichst früh verworfen werden.

## Protokolle

Der Stack muss mehrere Protokolle parallel unterstützen können.

Mindestens vorgesehen sind:

```text
IPv4
IPv6
ICMP
TCP
UDP
```

Weitere Protokolle dürfen als Module ergänzt werden.

Ein Protokoll darf nicht voraussetzen, dass es der einzige aktive Netzwerkpfad ist.

## Routing

Routing muss als eigener Mechanismus behandelt werden.

```text
Destination
    ↓
Routing Decision
    ↓
Interface + Next Hop
```

Routing-Entscheidungen dürfen Informationen wie Ziel, Interface, Metrik, Policy und Execution Context berücksichtigen.

Routing und Zugriffsberechtigung bleiben getrennte Entscheidungen.

## Netzwerk-I/O

Netzwerkoperationen sollen in das allgemeine NovaOS-I/O-Modell integriert werden.

```text
IORequest
   ↓
Network Stack
   ↓
Protocol
   ↓
Driver
```

Asynchrone Verarbeitung, Completion, Cancellation, Priority, QoS und Backpressure sollen mit den allgemeinen I/O-Mechanismen zusammenarbeiten.

## Buffer und Zero-Copy

Pakete sollen über kontrollierte Buffer verarbeitet werden.

```text
NIC
 ↓
Packet Buffer
 ↓
Network Stack
 ↓
Application
```

Wo sinnvoll, darf Zero-Copy verwendet werden.

Zero-Copy darf Sicherheits-, Ownership- oder Lifetime-Regeln nicht umgehen.

Ein sicherer Copy-Pfad muss verfügbar bleiben.

## Parallelität

Paketverarbeitung muss auf mehreren CPUs skalierbar sein.

```text
NIC Queues
   ↓
CPU / Worker Distribution
   ↓
Protocol Processing
```

Globale Locks im kritischen Paketpfad sollen möglichst vermieden werden.

Ordering-Garantien dürfen nur dort erzeugt werden, wo das jeweilige Protokoll sie benötigt.

## Backpressure

Überlastete Netzwerkpfade müssen Backpressure unterstützen.

```text
Network Input
     ↓
Bounded Queues
     ↓
Consumer
```

Unbegrenztes Paket- oder Buffer-Wachstum darf nicht auftreten.

## Sicherheit

Netzwerkpakete sind grundsätzlich als nicht vertrauenswürdige externe Eingaben zu behandeln.

Der Stack muss mindestens berücksichtigen:

```text
Bounds Validation
Protocol Validation
Capability Checks
Resource Limits
Firewall Policy
Isolation
```

Netzwerkzugriff muss auf Prozesse, Capabilities und Sicherheitskontexte begrenzbar sein.

## Fehler und Degradation

Der Ausfall eines Interfaces oder Protokollpfades darf nicht automatisch den gesamten Netzwerk-Stack blockieren.

```text
Interface Failure
      ↓
Isolate
      ↓
Alternative Route / Degraded Mode
```

Fehler müssen möglichst auf den kleinsten betroffenen Bereich begrenzt bleiben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Interfaces
Addresses
Routes
Protocols
Connections
Packet Statistics
Queue State
Errors
Dropped Packets
Resource Usage
```

Sensitive Verbindungs- oder Paketdaten dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS einen modularen Netzwerk-Stack bereitstellen.
2. Netzwerkinterfaces MÜSSEN eindeutig und unabhängig von Gerätenamen identifizierbar sein.
3. Der Stack MUSS IPv4 und IPv6 parallel unterstützen können.
4. TCP, UDP und ICMP MÜSSEN als grundlegende Protokolle unterstützt werden können.
5. Protokollmechanismen und Netzwerk-Policy MÜSSEN getrennt bleiben.
6. Eingehende Netzwerkdaten MÜSSEN als nicht vertrauenswürdige Eingaben behandelt werden.
7. Netzwerk-I/O SOLL in das allgemeine NovaOS-I/O-Modell integriert werden.
8. Paketbuffer MÜSSEN definierte Ownership- und Lifetime-Regeln besitzen.
9. Zero-Copy DARF Sicherheits- und Speichergrenzen NICHT umgehen.
10. Netzwerkqueues MÜSSEN begrenzbar sein und Backpressure unterstützen.
11. Paketverarbeitung SOLL über mehrere CPUs skalierbar sein.
12. Netzwerkzustand und Ressourcenverbrauch SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0001`

## Ergebnis

```text
Application
    ↓
Controlled Network API
    ↓
Modular Protocol Stack
    ↓
Routing + Packet Processing
    ↓
Network Interface
    ↓
Driver / Hardware
```

NovaOS erhält damit einen modularen, skalierbaren und sicher kontrollierten Netzwerk-Stack, auf dem die weiteren Netzwerkprotokolle und Netzwerkdienste einheitlich aufbauen können.