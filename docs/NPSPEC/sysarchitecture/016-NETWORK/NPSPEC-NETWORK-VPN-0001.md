# NPSPEC-NETWORK-VPN-0001 – Nova Virtual Private Network

## Status

Angenommen

## Kategorie

Network / VPN / Secure Tunneling

## Zweck

NovaOS definiert VPN-Verbindungen als kontrollierte virtuelle Netzwerkpfade für geschützte Kommunikation über nicht vertrauenswürdige Netzwerke.

```text
Application / Namespace
        ↓
VPN Interface
        ↓
Encrypted Tunnel
        ↓
Physical Network
```

VPNs werden als reguläre virtuelle Netzwerkinterfaces in Routing, Firewall, Multipath und Network Namespaces integriert.

## Grundprinzip

```text
VPN ≠ Trust
Encryption ≠ Authorization
Tunnel ≠ Network Permission
VPN Connected ≠ All Traffic Protected
```

Ein VPN schützt den definierten Netzwerkpfad. Welche Daten diesen Pfad verwenden, wird durch Routing und Policy bestimmt.

## VPN-Modell

Eine VPN-Verbindung besitzt mindestens:

```text
VPNConnection
├── VPNID
├── Provider
├── Virtual Interface
├── Remote Endpoint
├── Security State
└── State
```

Optional:

```text
Namespace
Routes
DNS Context
Security Context
ExecutionContract
Resource Budget
```

Die `VPNID` bleibt unabhängig von IP-Adresse, Interface-Name oder konkretem Netzwerkpfad.

## Lifecycle

Eine VPN-Verbindung besitzt einen kontrollierten Lebenszyklus.

```text
Created
   ↓
Connecting
   ↓
Authenticating
   ↓
Active
   ↓
Reconnecting
   ↓
Closed
```

Eine Verbindung darf erst als `Active` gelten, wenn die erforderlichen Authentifizierungs- und Sicherheitsprüfungen erfolgreich abgeschlossen wurden.

## VPN-Protokolle

Die Architektur darf unterschiedliche VPN-Protokolle über gemeinsame Interfaces integrieren.

```text
Nova VPN
├── WireGuard Provider
├── IPsec Provider
├── OpenVPN Provider
└── Future Provider
```

Der Netzwerk-Stack darf nicht dauerhaft an ein einzelnes VPN-Protokoll gekoppelt sein.

Protokollspezifische Logik bleibt im jeweiligen Provider.

## Virtuelles Interface

Eine aktive VPN-Verbindung stellt dem Netzwerk-Stack ein virtuelles Interface bereit.

```text
VPN Provider
     ↓
Virtual Interface
     ↓
Nova Network Stack
```

Das VPN-Interface nimmt wie andere Interfaces an Routing, Firewall, QoS und Multipath teil.

## Routing

VPN-Verkehr wird über die gemeinsame Routing-Infrastruktur gesteuert.

```text
Network Flow
     ↓
Routing Policy
   ↙           ↘
VPN          Normal Path
```

Unterstützt werden müssen können:

```text
Full Tunnel
Split Tunnel
Per-Namespace Routing
Per-Flow Routing
```

Eine Anwendung darf ihre VPN-Vorgaben nicht ohne entsprechende Berechtigung umgehen.

## Network Namespaces

VPN-Verbindungen dürfen an Network Namespaces gebunden werden.

```text
Application
    ↓
Namespace
    ↓
VPN Interface
    ↓
Encrypted Tunnel
```

Dadurch kann beispielsweise nur eine bestimmte Anwendung oder isolierte Umgebung über ein VPN kommunizieren.

## DNS

VPNs dürfen eigene DNS-Konfigurationen bereitstellen.

```text
VPN
 ↓
DNS Context
 ↓
Nova Resolver
```

DNS-Anfragen müssen entsprechend der Routing- und Privacy-Policy über den vorgesehenen Resolver und Netzwerkpfad geführt werden können.

Split-DNS muss abbildbar sein.

## Kryptografie

VPN-Provider müssen kryptografische Schlüssel über die NovaOS-Sicherheitsinfrastruktur verwalten können.

```text
Credentials / Keys
        ↓
Protected Security Context
        ↓
VPN Provider
```

Schlüsselmaterial darf nicht unnötig an Anwendungen oder andere Netzwerkkomponenten offengelegt werden.

## Multipath und Netzwerkwechsel

Ein VPN darf seinen zugrunde liegenden Netzwerkpfad wechseln, sofern das verwendete Protokoll dies unterstützt.

```text
VPN Tunnel
    ↓
Wi-Fi → Ethernet
```

Ein Pfadwechsel darf Sicherheits- oder Sovereignty-Anforderungen nicht abschwächen.

Der VPN-Zustand muss nach einem Wechsel erneut validiert werden können.

## Kill Switch

NovaOS soll eine Policy bereitstellen können, die Verkehr blockiert, wenn ein vorgeschriebener VPN-Pfad nicht verfügbar ist.

```text
VPN Required
     ↓
VPN Available?
 ├── Yes → Allow
 └── No  → Block
```

Ein Fallback auf einen ungeschützten Netzwerkpfad darf bei aktiver entsprechender Policy nicht erfolgen.

## Firewall

VPN-Interfaces unterliegen der normalen Firewall-Infrastruktur.

Firewall-Regeln dürfen unterscheiden zwischen:

```text
Pre-Tunnel Traffic
VPN Interface Traffic
Post-Tunnel Traffic
```

VPN-Tunnel dürfen bestehende Firewall- oder Capability-Regeln nicht umgehen.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
VPN Connections
Packet Buffers
Encryption State
Handshake State
Queues
Bandwidth
Reconnect Attempts
```

Fehlerhafte VPN-Verbindungen dürfen keine unbegrenzten Reconnect- oder Ressourcenzyklen erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
VPNID
Provider
State
Virtual Interface
Remote Endpoint
Security State
Current Path
Routes
DNS Context
Bytes Sent
Bytes Received
Last Error
```

Private Schlüssel und andere geheime Authentifizierungsdaten dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS VPNs als virtuelle Netzwerkinterfaces integrieren können.
2. Die VPN-Architektur MUSS mehrere VPN-Protokolle unterstützen können.
3. VPN-Verbindungen MÜSSEN Routing und Firewall des gemeinsamen Network Stack verwenden.
4. VPN-Verbindungen MÜSSEN Network Namespaces zugeordnet werden können.
5. Full-Tunnel- und Split-Tunnel-Konfigurationen MÜSSEN abbildbar sein.
6. VPNs SOLLEN eigene DNS-Kontexte bereitstellen können.
7. Kryptografisches Schlüsselmaterial MUSS geschützt gespeichert und verarbeitet werden.
8. VPN-Tunnel DÜRFEN Capability- oder Firewall-Regeln NICHT umgehen.
9. NovaOS SOLL einen VPN-Kill-Switch unterstützen.
10. Netzwerkpfadwechsel DÜRFEN Sicherheits- und Sovereignty-Regeln NICHT abschwächen.
11. VPN-Ressourcen und Reconnect-Versuche MÜSSEN begrenzbar sein.
12. VPN-, Routing-, Security- und Pfadzustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-VIRTUAL-0001`
- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-NETWORK-0018`

## Ergebnis

```text
Application / Namespace
        ↓
Routing + VPN Policy
        ↓
Virtual VPN Interface
        ↓
Encrypted Tunnel
        ↓
Controlled Network Path
```

NovaOS erhält damit eine protokollunabhängige VPN-Infrastruktur, die verschlüsselte Netzwerkpfade vollständig in Routing, Firewall, DNS, Multipath und Network Namespaces integriert.