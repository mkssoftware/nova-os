# NPSPEC-NETWORK-IPV4-0001 – Nova IPv4

## Status

Angenommen

## Kategorie

Network / IPv4 / Network Layer

## Zweck

NovaOS definiert IPv4 als unterstütztes Network-Layer-Protokoll innerhalb des modularen Nova Network Stack.

```text
Transport Protocol
       ↓
      IPv4
       ↓
Network Interface
       ↓
Driver / Hardware
```

IPv4 bleibt für Kompatibilität mit bestehenden Netzwerken vollständig unterstützt, ohne IPv6 oder andere Netzwerkprotokolle strukturell zu benachteiligen.

## Grundprinzip

```text
IPv4 Address ≠ Interface Identity
Route ≠ Permission
Packet Received ≠ Packet Trusted
IPv4 ≠ Preferred Protocol by Definition
```

IPv4 wird als austauschbares Protokollmodul des allgemeinen Netzwerk-Stacks behandelt.

## IPv4-Modul

Das IPv4-Modul übernimmt mindestens:

```text
Header Validation
Address Handling
Routing Integration
Fragment Handling
Protocol Dispatch
Packet Generation
```

TCP, UDP, ICMP und weitere Transport- oder Kontrollprotokolle werden anhand des IPv4-Protokollfeldes weitergeleitet.

## Adressierung

NovaOS muss IPv4-Adressen unabhängig von der dauerhaften Identität eines Netzwerkinterfaces behandeln.

```text
NetworkInterface
├── InterfaceID
└── IPv4 Addresses
```

Ein Interface darf mehrere IPv4-Adressen besitzen.

Adresskonfiguration darf statisch oder durch Netzwerkdienste wie DHCP erfolgen.

## Paketverarbeitung

Eingehende IPv4-Pakete durchlaufen mindestens:

```text
Packet
  ↓
Header Validation
  ↓
Destination Check
  ↓
Fragment Handling
  ↓
Protocol Dispatch
```

Ungültige Header, Längen oder nicht unterstützte Strukturen müssen kontrolliert verworfen werden.

## Ausgehende Pakete

Ausgehende IPv4-Pakete werden aus Transportdaten und Routinginformationen erzeugt.

```text
Transport Payload
      ↓
IPv4 Header
      ↓
Route Selection
      ↓
Network Interface
```

Die Wahl des Interfaces erfolgt über das allgemeine Routing-System und nicht direkt durch das IPv4-Modul.

## Routing

IPv4 muss mit der gemeinsamen NovaOS-Routing-Infrastruktur arbeiten.

```text
Destination IPv4
      ↓
Routing Table
      ↓
Next Hop
      ↓
Interface
```

Unterstützt werden müssen mindestens:

```text
Network Route
Host Route
Default Route
```

Routing-Entscheidungen erzeugen keine Zugriffsberechtigung.

## Fragmentierung

IPv4-Fragmentierung und Reassembly müssen kontrolliert behandelt werden.

```text
Fragments
   ↓
Validation
   ↓
Bounded Reassembly
   ↓
Packet
```

Reassembly muss durch Zeit-, Speicher- und Fragmentlimits begrenzt werden.

Überlappende, ungültige oder missbräuchliche Fragmente müssen sicher behandelt werden.

## MTU

Das IPv4-Modul muss die MTU des verwendeten Netzwerkpfades berücksichtigen.

```text
Packet Size
    ↓
MTU Check
   ├→ Fits
   └→ Fragment / Error
```

Das `Don't Fragment`-Flag muss berücksichtigt werden.

## Checksummen

IPv4-Headerprüfsummen müssen beim Empfang validiert und beim Versand korrekt erzeugt werden.

Hardware-Offloading darf verwendet werden, sofern dessen Ergebnis eindeutig in den Paketstatus einfließt.

## Broadcast und Multicast

IPv4 muss relevante Adresstypen unterscheiden können:

```text
Unicast
Broadcast
Multicast
```

Broadcast- und Multicast-Verarbeitung muss durch Netzwerk- und Sicherheitsrichtlinien begrenzbar sein.

## ARP-Integration

IPv4 darf für lokale Link-Layer-Auflösung ARP verwenden.

```text
IPv4 Next Hop
     ↓
ARP Resolution
     ↓
Link Address
```

ARP ist dabei ein separater Mechanismus und nicht Teil der IPv4-Identität.

## Sicherheit

IPv4-Pakete sind grundsätzlich nicht vertrauenswürdig.

Validiert werden müssen insbesondere:

```text
Header Length
Total Length
Checksum
Fragment Information
Source Address
Destination Address
Protocol
```

Malformed Packets dürfen keine Speicher- oder Ressourcenverletzungen verursachen.

## Ressourcensteuerung

IPv4 muss Ressourcen begrenzen können für:

```text
Packet Queues
Fragment Reassembly
Routing State
Address State
Pending Resolution
```

Externe Netzwerkdaten dürfen keinen unbegrenzten Speicherverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
IPv4 Addresses
Routes
Packet Counters
Fragment Statistics
Dropped Packets
Protocol Statistics
Errors
```

## Normative Anforderungen

1. NovaOS MUSS IPv4 innerhalb des Nova Network Stack unterstützen.
2. IPv4 MUSS als eigenständiges Protokollmodul implementierbar sein.
3. IPv4-Adressen DÜRFEN NICHT als dauerhafte Interface-Identität verwendet werden.
4. Ein Interface MUSS mehrere IPv4-Adressen unterstützen können.
5. Eingehende IPv4-Header MÜSSEN vor weiterer Verarbeitung validiert werden.
6. IPv4 MUSS mit der gemeinsamen Routing-Infrastruktur arbeiten.
7. Fragment-Reassembly MUSS durch Ressourcen- und Zeitlimits begrenzt werden.
8. Das `Don't Fragment`-Flag MUSS berücksichtigt werden.
9. IPv4-Headerprüfsummen MÜSSEN korrekt validiert oder durch verifiziertes Hardware-Offloading behandelt werden.
10. Broadcast- und Multicast-Verarbeitung MUSS durch Policy begrenzbar sein.
11. Fehlerhafte IPv4-Pakete DÜRFEN NICHT zu unkontrolliertem Ressourcenverbrauch führen.
12. IPv4-Zustand und Statistiken SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-BACKPRESSURE-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0002`

## Ergebnis

```text
TCP / UDP / ICMP
       ↓
      IPv4
       ↓
Routing + Validation
       ↓
Network Interface
       ↓
Link Layer
```

NovaOS erhält damit eine klar abgegrenzte IPv4-Implementierung, die bestehende IPv4-Netzwerke vollständig unterstützt und gleichzeitig in die gemeinsame Routing-, Sicherheits-, Ressourcen- und I/O-Architektur von NovaOS integriert bleibt.