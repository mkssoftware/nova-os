# NPSPEC-NETWORK-DHCP-0001 – Nova Dynamic Host Configuration Protocol

## Status

Angenommen

## Kategorie

Network / DHCP / Configuration

## Zweck

NovaOS definiert eine gemeinsame DHCP-Infrastruktur zur automatischen Konfiguration von Netzwerkinterfaces.

```text
Network Interface
       ↓
DHCP Client
       ↓
DHCP Server
       ↓
Network Configuration
```

DHCP liefert dynamische Netzwerkkonfiguration, bestimmt jedoch weder Interface-Identität noch Netzwerkberechtigung.

## Grundprinzip

```text
DHCP Configuration ≠ Trust
Assigned Address ≠ Interface Identity
DHCP Server ≠ Trusted Authority
Lease ≠ Permanent Configuration
```

Von DHCP empfangene Daten müssen als externe, nicht vertrauenswürdige Netzwerkinformationen behandelt werden.

## DHCP Client

Der DHCP-Client arbeitet pro Netzwerkinterface bzw. Netzwerkkontext.

```text
DHCPClient
├── ClientID
├── InterfaceID
├── Protocol Version
├── State
└── Lease
```

Optional:

```text
Security Context
Network Context
Policy
Timeout
Retry State
```

Die DHCP-Client-Identität darf nicht mit der dauerhaften Identität des Netzwerkinterfaces gleichgesetzt werden.

## DHCPv4

NovaOS muss DHCPv4 unterstützen können.

Der grundlegende Ablauf ist:

```text
Client                Server

DHCPDISCOVER   →
               ←      DHCPOFFER
DHCPREQUEST    →
               ←      DHCPACK
```

Nach erfolgreicher Konfiguration können insbesondere übernommen werden:

```text
IPv4 Address
Subnet Mask
Default Gateway
DNS Resolver
Lease Lifetime
```

Empfangene Werte müssen vor Aktivierung validiert werden.

## DHCPv6

NovaOS soll DHCPv6 unterstützen.

```text
IPv6 Interface
      ↓
DHCPv6
      ↓
IPv6 Configuration
```

DHCPv6 muss mit IPv6 Neighbor Discovery und SLAAC koexistieren können.

Die IPv6-Konfiguration darf nicht voraussetzen, dass ausschließlich DHCPv6 verwendet wird.

## Lease-Modell

Eine DHCP-Konfiguration besitzt eine begrenzte Gültigkeit.

```text
Lease
├── Configuration
├── Start Time
├── Renewal Time
├── Rebinding Time
└── Expiration
```

Der Client muss Leases erneuern können.

Abgelaufene Konfiguration darf nicht unbegrenzt als gültig behandelt werden.

## Konfigurationsübergabe

DHCP darf Netzwerkparameter nicht unkontrolliert direkt in globale Systemzustände schreiben.

```text
DHCP Response
     ↓
Validation
     ↓
Network Configuration
     ↓
Routing / DNS / Interface
```

Änderungen sollen über die jeweiligen NovaOS-Netzwerkmechanismen aktiviert werden.

## Routing-Integration

Von DHCP gelernte Gateways und Routen werden an die gemeinsame Routing-Infrastruktur übergeben.

```text
DHCP Gateway
     ↓
Route Validation
     ↓
Routing Table
```

Eine über DHCP gelernte Route erzeugt keine Netzwerkberechtigung.

## DNS-Integration

DHCP darf DNS-Resolver bereitstellen.

```text
DHCP
 ↓
Resolver Configuration
 ↓
Nova DNS Resolver
```

Die Resolver-Konfiguration muss dem zugehörigen Netzwerkinterface oder Netzwerkkontext zugeordnet bleiben können.

Dadurch können beispielsweise VPN-, WLAN- und Ethernet-Netze unterschiedliche Resolver verwenden.

## Netzwerkwechsel

DHCP muss dynamische Interface-Zustände unterstützen.

```text
Interface Up
     ↓
Acquire Lease
     ↓
Configured
     ↓
Interface Down
     ↓
Suspend / Release
```

Bei Netzwerkwechseln dürfen alte Konfigurationen nicht unkontrolliert auf neue Netzwerke übertragen werden.

## Multipath

Mehrere Interfaces dürfen gleichzeitig eigene DHCP-Konfigurationen besitzen.

```text
Ethernet → Lease A
Wi-Fi   → Lease B
VPN     → Configuration C
```

Routing und Multipath entscheiden anschließend, welcher Netzwerkpfad verwendet wird.

## Sicherheit

DHCP-Antworten sind grundsätzlich nicht vertrauenswürdig.

Validiert werden müssen insbesondere:

```text
Message Structure
Transaction Association
Address Information
Option Lengths
Option Types
Lease Values
Server Information
```

Ein DHCP-Server darf keine Capability- oder Firewall-Regeln verändern.

## Fehlerbehandlung

Fehlerhafte oder ausbleibende DHCP-Konfiguration darf den gesamten Netzwerk-Stack nicht blockieren.

```text
DHCP Failure
    ↓
Retry / Alternative Configuration
```

Mögliche Alternativen dürfen sein:

```text
Static Configuration
IPv6 SLAAC
Link-Local Configuration
Alternative Interface
```

Retries müssen begrenzt und mit Backoff durchgeführt werden.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Pending Requests
DHCP Responses
Stored Leases
Retry State
Option Data
```

Manipulierte DHCP-Nachrichten dürfen keinen unbegrenzten Speicher- oder CPU-Verbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
InterfaceID
DHCP State
Assigned Addresses
Gateway
DNS Resolvers
Lease Lifetime
Renewal State
DHCP Server
Last Error
```

Sensible Netzwerkdetails dürfen nur entsprechend der geltenden Berechtigungen sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS DHCPv4 unterstützen können.
2. NovaOS SOLL DHCPv6 unterstützen.
3. DHCP-Konfiguration DARF NICHT als dauerhafte Interface-Identität verwendet werden.
4. DHCP-Antworten MÜSSEN als nicht vertrauenswürdige Netzwerkdaten behandelt werden.
5. Empfangene Konfiguration MUSS vor Aktivierung validiert werden.
6. DHCP-Leases MÜSSEN zeitlich begrenzt behandelt werden.
7. DHCP MUSS mit der gemeinsamen Routing-Infrastruktur zusammenarbeiten.
8. DHCP MUSS Resolver-Konfiguration an die DNS-Infrastruktur übergeben können.
9. DHCPv6 MUSS mit SLAAC und Neighbor Discovery koexistieren können.
10. Mehrere Interfaces MÜSSEN unabhängige Netzwerkkonfigurationen besitzen können.
11. DHCP-Fehler DÜRFEN NICHT den gesamten Netzwerk-Stack blockieren.
12. DHCP-Zustand, Lease und Konfiguration SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0013`

## Ergebnis

```text
Network Interface
       ↓
DHCPv4 / DHCPv6
       ↓
Validated Configuration
       ↓
Address + Routing + DNS
       ↓
Nova Network Stack
```

NovaOS erhält damit eine zentrale und kontrollierte DHCP-Infrastruktur für dynamische IPv4- und IPv6-Konfiguration, ohne DHCP mit Identität, Vertrauen oder Netzwerkberechtigung gleichzusetzen.