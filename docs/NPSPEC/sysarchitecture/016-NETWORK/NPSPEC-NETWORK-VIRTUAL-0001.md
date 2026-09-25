# NPSPEC-NETWORK-VIRTUAL-0001 – Nova Virtual Networking

## Status

Angenommen

## Kategorie

Network / Virtualization / Virtual Network

## Zweck

NovaOS definiert virtuelle Netzwerkressourcen als gleichwertige Teilnehmer des gemeinsamen Netzwerk-Stacks.

```text
Process / Service
       ↓
Virtual Interface
       ↓
Virtual Network
       ↓
Physical / Virtual Path
```

Virtuelle Netzwerke ermöglichen isolierte Kommunikation, Bridges, VPNs, Network Namespaces und softwaredefinierte Netzwerkpfade ohne Abhängigkeit von physischer Hardware.

## Grundprinzip

```text
Virtual Interface ≠ Physical Interface
Virtual Network ≠ Network Namespace
Connectivity ≠ Permission
Virtualization ≠ Security Bypass
```

Virtuelle und physische Netzwerkressourcen verwenden möglichst dieselben Netzwerkmechanismen.

## Virtual Interface

Ein virtuelles Interface wird wie ein logisches Netzwerkinterface behandelt.

```text
VirtualInterface
├── InterfaceID
├── Type
├── State
├── Addresses
├── MTU
└── Capabilities
```

Optional:

```text
Peer
Namespace
Parent Interface
Security Context
Resource Budget
```

Die `InterfaceID` bleibt unabhängig von Interface-Name, Netzwerkadresse oder physischem Netzwerkpfad.

## Interface-Typen

NovaOS soll unterschiedliche virtuelle Interface-Typen unterstützen können.

```text
Loopback
Point-to-Point
Virtual Ethernet
Bridge Interface
Tunnel Interface
VPN Interface
Virtual NIC
```

Weitere Typen dürfen über definierte Interfaces ergänzt werden.

## Virtuelle Links

Zwei Netzwerkendpunkte dürfen über einen virtuellen Link verbunden werden.

```text
Interface A
     ↓
Virtual Link
     ↓
Interface B
```

Ein virtueller Link darf vollständig innerhalb des Systems existieren und benötigt keine physische Netzwerkkarte.

Virtuelle Links müssen Bandbreite, Queue-Größe und weitere Ressourcen begrenzen können.

## Virtual Ethernet

Virtuelle Ethernet-Verbindungen dürfen paarweise erzeugt werden.

```text
vNIC A ←────→ vNIC B
```

Die beiden Endpunkte dürfen unterschiedlichen Network Namespaces zugeordnet werden.

Dadurch können isolierte Netzwerkumgebungen kontrolliert miteinander verbunden werden.

## Bridge

Mehrere Interfaces dürfen über eine virtuelle Bridge verbunden werden.

```text
       Bridge
     ┌───┼───┐
     ↓   ↓   ↓
   vNIC vNIC NIC
```

Die Bridge arbeitet auf Link-Layer-Ebene und darf physische sowie virtuelle Interfaces kombinieren.

Forwarding-Zustände müssen ressourcenbegrenzt sein.

## Tunnel

Virtuelle Interfaces dürfen Netzwerkverkehr über andere Netzwerkpfade kapseln.

```text
Virtual Interface
       ↓
Encapsulation
       ↓
Network Flow
       ↓
Physical Network
```

Tunnelmechanismus und transportierter Netzwerkverkehr müssen logisch getrennt bleiben.

## Namespace-Integration

Virtuelle Interfaces bilden den primären Mechanismus zur Verbindung isolierter Network Namespaces.

```text
Namespace A
     ↓
Virtual Interface
     ↓
Virtual Link
     ↓
Virtual Interface
     ↓
Namespace B
```

Eine solche Verbindung muss explizit erstellt und autorisiert werden.

## Routing

Virtuelle Interfaces nehmen regulär am Routing teil.

```text
Network Flow
     ↓
Routing
     ↓
Virtual Interface
```

Routing darf physische und virtuelle Interfaces nach denselben grundlegenden Regeln behandeln.

Die Virtualität eines Interfaces erzeugt keine bevorzugte Route.

## Multipath

Virtuelle Netzwerkpfade dürfen Bestandteil des Multipath-Modells sein.

```text
NetworkFlow
   ↓
Multipath
 ├→ Ethernet
 ├→ Wi-Fi
 └→ Virtual / VPN
```

QoS-, Sicherheits- und Sovereignty-Anforderungen gelten unabhängig davon, ob ein Pfad physisch oder virtuell ist.

## Firewall

Virtuelle Interfaces und Netzwerke unterliegen der normalen Firewall-Infrastruktur.

```text
Virtual Traffic
      ↓
Firewall Policy
      ↓
Routing / Forwarding
```

Virtuelle Verbindungen dürfen Firewall- oder Capability-Regeln nicht umgehen.

## Datenpfad

Virtuelle Netzwerkpfade sollen unnötige Datenkopien vermeiden.

```text
Sender Buffer
     ↓
Virtual Network
     ↓
Receiver Buffer
```

Shared Buffer und Zero-Copy dürfen verwendet werden, wenn Ownership, Isolation und Lifetime eindeutig bleiben.

Ein sicherer Copy-Fallback muss verfügbar sein.

## Lifecycle

Virtuelle Netzwerkressourcen besitzen einen kontrollierten Lebenszyklus.

```text
Created
   ↓
Configured
   ↓
Active
   ↓
Stopping
   ↓
Destroyed
```

Beim Entfernen müssen zugehörige Routen, Flows und Netzwerkzustände kontrolliert bereinigt werden.

## Sicherheit

Mindestens folgende Operationen müssen autorisiert werden:

```text
Create Virtual Interface
Destroy Virtual Interface
Connect Interfaces
Create Bridge
Attach Interface
Create Tunnel
Move Interface to Namespace
```

Normale Anwendungen dürfen keine virtuellen Netzwerkpfade erzeugen, um bestehende Netzwerkberechtigungen zu erweitern.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Virtual Interfaces
Virtual Links
Bridges
Forwarding State
Packet Queues
Buffers
Bandwidth
Tunnel State
```

Virtuelle Netzwerke dürfen keinen unbegrenzten Speicher-, CPU- oder Netzwerkverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
InterfaceID
Type
State
Peer
Namespace
Addresses
Routes
Flows
Queue Usage
Traffic Statistics
Resource Usage
```

## Normative Anforderungen

1. NovaOS MUSS virtuelle Netzwerkinterfaces unterstützen können.
2. Virtuelle Interfaces MÜSSEN in das gemeinsame Network-Interface-Modell integriert sein.
3. Virtuelle und physische Interfaces SOLLEN möglichst dieselben Netzwerk-APIs verwenden.
4. Virtuelle Links MÜSSEN explizit erstellt und autorisiert werden.
5. Virtuelle Interfaces MÜSSEN Network Namespaces zugeordnet werden können.
6. NovaOS SOLL virtuelle Bridges unterstützen.
7. Tunnel- und VPN-Interfaces MÜSSEN integrierbar sein.
8. Virtuelle Interfaces MÜSSEN an Routing und Multipath teilnehmen können.
9. Virtuelle Netzwerkpfade DÜRFEN Firewall- oder Capability-Regeln NICHT umgehen.
10. Zero-Copy DARF Isolation und Ownership NICHT verletzen.
11. Virtuelle Netzwerkressourcen MÜSSEN ressourcenbegrenzt sein.
12. Virtuelle Interfaces, Links und Ressourcen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0017`

## Ergebnis

```text
Physical + Virtual Interfaces
           ↓
     Nova Network Stack
           ↓
Routing + Firewall + Multipath
           ↓
Virtual Networks / Namespaces
```

NovaOS erhält damit eine einheitliche virtuelle Netzwerkinfrastruktur, in der virtuelle Interfaces, Links, Bridges und Tunnel wie reguläre Netzwerkressourcen behandelt werden, ohne Isolation, Sicherheit oder Ressourcensteuerung zu umgehen.