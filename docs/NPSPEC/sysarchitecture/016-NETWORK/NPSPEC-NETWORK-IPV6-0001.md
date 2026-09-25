# NPSPEC-NETWORK-IPV6-0001 – Nova IPv6

## Status

Angenommen

## Kategorie

Network / IPv6 / Network Layer

## Zweck

NovaOS definiert IPv6 als natives Network-Layer-Protokoll innerhalb des Nova Network Stack.

```text
Transport Protocol
       ↓
      IPv6
       ↓
Network Interface
       ↓
Driver / Hardware
```

IPv6 wird parallel zu IPv4 unterstützt und darf unabhängig davon betrieben werden.

## Grundprinzip

```text
IPv6 Address ≠ Interface Identity
Route ≠ Permission
Packet Received ≠ Packet Trusted
IPv6 ≠ IPv4 with Larger Addresses
```

IPv6 wird als eigenständiges Protokollmodul mit eigener Adressierung, Neighbor Discovery und Paketsemantik behandelt.

## IPv6-Modul

Das IPv6-Modul übernimmt mindestens:

```text
Header Validation
Address Handling
Extension Headers
Routing Integration
Fragment Handling
Protocol Dispatch
Packet Generation
```

Transportprotokolle wie TCP, UDP und ICMPv6 werden über das `Next Header`-Feld zugeordnet.

## Adressierung

Ein Netzwerkinterface darf mehrere IPv6-Adressen gleichzeitig besitzen.

```text
NetworkInterface
├── InterfaceID
├── Link-Local Address
├── Global Addresses
└── Temporary Addresses
```

NovaOS muss mindestens unterscheiden können:

```text
Unicast
Multicast
Link-Local
Global
Unique Local
```

Die IPv6-Adresse darf nicht als dauerhafte Identität des Interfaces verwendet werden.

## Adresskonfiguration

IPv6-Adressen dürfen durch verschiedene Mechanismen konfiguriert werden:

```text
Static Configuration
SLAAC
DHCPv6
Link-Local Configuration
```

Mehrere Konfigurationsmechanismen dürfen gleichzeitig verwendet werden.

## Paketverarbeitung

Eingehende IPv6-Pakete durchlaufen mindestens:

```text
Packet
  ↓
Header Validation
  ↓
Extension Header Processing
  ↓
Destination Check
  ↓
Fragment Handling
  ↓
Protocol Dispatch
```

Ungültige oder nicht unterstützte Strukturen müssen kontrolliert verworfen werden.

## Extension Headers

IPv6 Extension Headers müssen als kontrollierte Verarbeitungskette behandelt werden.

```text
IPv6 Header
    ↓
Extension Header
    ↓
Extension Header
    ↓
Transport Protocol
```

Anzahl und Verarbeitungsaufwand müssen begrenzbar sein, damit manipulierte Header-Ketten keinen unbegrenzten Ressourcenverbrauch verursachen.

## Routing

IPv6 verwendet die gemeinsame NovaOS-Routing-Infrastruktur.

```text
Destination IPv6
      ↓
Routing Table
      ↓
Next Hop
      ↓
Interface
```

Routing und Zugriffsberechtigung bleiben getrennte Entscheidungen.

## Neighbor Discovery

IPv6 muss Neighbor Discovery über ICMPv6 integrieren.

```text
IPv6 Next Hop
     ↓
Neighbor Discovery
     ↓
Link Address
```

Unterstützt werden müssen insbesondere:

```text
Neighbor Solicitation
Neighbor Advertisement
Router Solicitation
Router Advertisement
```

Neighbor-Informationen müssen zeitlich begrenzt und aktualisierbar sein.

## Fragmentierung

IPv6-Router fragmentieren Pakete nicht.

```text
Sender
  ↓
Fragment Extension Header
  ↓
Network
  ↓
Receiver Reassembly
```

Fragmentierung erfolgt ausschließlich durch den Sender.

Reassembly muss durch Speicher-, Fragment- und Zeitlimits begrenzt werden.

## Path MTU

IPv6 muss Path MTU Discovery unterstützen können.

```text
Packet
  ↓
Path MTU
  ↓
Suitable Packet Size
```

ICMPv6 `Packet Too Big` muss für die Anpassung des Übertragungspfades verarbeitet werden können.

## Multicast

IPv6 verwendet Multicast für verschiedene Kernfunktionen.

```text
Node
  ↓
Multicast Group
```

Multicast-Mitgliedschaften müssen explizit verwaltet und durch Sicherheits- und Ressourcenrichtlinien begrenzbar sein.

## Dual Stack

IPv4 und IPv6 müssen parallel betrieben werden können.

```text
Application
    ↓
Network API
   ↙       ↘
IPv4      IPv6
```

Die Auswahl des Protokolls soll anhand von Ziel, Routing, Policy und verfügbaren Netzwerkpfaden erfolgen können.

## Sicherheit

IPv6-Pakete sind grundsätzlich nicht vertrauenswürdig.

Validiert werden müssen insbesondere:

```text
Payload Length
Next Header
Extension Headers
Source Address
Destination Address
Fragment Information
ICMPv6 Messages
```

Neighbor Discovery und Router Advertisements dürfen nicht automatisch als vertrauenswürdig gelten.

## Ressourcensteuerung

IPv6 muss Ressourcen begrenzen können für:

```text
Packet Queues
Neighbor Cache
Fragment Reassembly
Extension Header Processing
Address State
Routing State
```

Externe Netzwerkdaten dürfen keinen unbegrenzten Ressourcenverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
IPv6 Addresses
Address State
Routes
Neighbors
Multicast Memberships
Fragment Statistics
Packet Counters
Dropped Packets
Errors
```

## Normative Anforderungen

1. NovaOS MUSS IPv6 innerhalb des Nova Network Stack unterstützen.
2. IPv6 MUSS parallel zu IPv4 betrieben werden können.
3. Ein Interface MUSS mehrere IPv6-Adressen unterstützen können.
4. IPv6-Adressen DÜRFEN NICHT als dauerhafte Interface-Identität verwendet werden.
5. Link-Local-Adressierung MUSS unterstützt werden.
6. SLAAC SOLL unterstützt werden.
7. Neighbor Discovery MUSS über ICMPv6 unterstützt werden.
8. Extension Headers MÜSSEN kontrolliert und ressourcenbegrenzt verarbeitet werden.
9. IPv6-Router DÜRFEN Pakete NICHT fragmentieren.
10. Fragment-Reassembly MUSS durch Zeit- und Ressourcenlimits begrenzt werden.
11. IPv6 MUSS in die gemeinsame NovaOS-Routing-Infrastruktur integriert werden.
12. IPv6-Zustand, Neighbor State und Statistiken SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0003`

## Ergebnis

```text
TCP / UDP / ICMPv6
        ↓
       IPv6
        ↓
Routing + Neighbor Discovery
        ↓
Network Interface
        ↓
Link Layer
```

NovaOS erhält damit eine native IPv6-Implementierung mit moderner Adressierung, Neighbor Discovery, kontrollierter Extension-Header-Verarbeitung und vollständiger Integration in die gemeinsame Netzwerkarchitektur.