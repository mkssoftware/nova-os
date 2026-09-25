# NPSPEC-NETWORK-SLAAC-0001 – Nova IPv6 Stateless Address Autoconfiguration

## Status

Angenommen

## Kategorie

Network / IPv6 / Configuration

## Zweck

NovaOS definiert SLAAC als Mechanismus zur automatischen IPv6-Adresskonfiguration ohne verpflichtenden zentralen Adressserver.

```text
Network Interface
       ↓
Router Advertisement
       ↓
SLAAC
       ↓
IPv6 Address
```

SLAAC wird in die gemeinsame IPv6-, Routing- und Netzwerk-Konfigurationsarchitektur integriert.

## Grundprinzip

```text
SLAAC Address ≠ Interface Identity
Router Advertisement ≠ Trust
Configured Address ≠ Permission
Prefix ≠ Network Authority
```

Über SLAAC erhaltene Informationen sind externe Netzwerkdaten und müssen entsprechend validiert werden.

## SLAAC-Modell

Die Konfiguration wird einem konkreten Netzwerkinterface zugeordnet.

```text
SLAACContext
├── InterfaceID
├── Prefixes
├── Addresses
├── Router Information
└── State
```

Optional:

```text
Network Context
Security Context
Address Policy
Privacy Policy
Lifetime
```

Mehrere SLAAC-Kontexte dürfen gleichzeitig aktiv sein.

## Router Discovery

SLAAC verwendet IPv6 Neighbor Discovery und ICMPv6 Router Advertisements.

```text
Interface
   ↓
Router Solicitation
   ↓
Router Advertisement
   ↓
Validation
```

Router Advertisements dürfen Informationen liefern wie:

```text
Network Prefix
Prefix Length
Valid Lifetime
Preferred Lifetime
Default Router
MTU
Configuration Flags
```

Diese Informationen dürfen nicht ungeprüft übernommen werden.

## Adressbildung

Aus einem geeigneten IPv6-Präfix erzeugt NovaOS eine Interface-Adresse.

```text
IPv6 Prefix
     +
Interface Identifier
     ↓
IPv6 Address
```

Die Bildung des Interface Identifiers soll austauschbar sein.

NovaOS darf stabile, zufällige oder temporäre Identifier entsprechend der geltenden Privacy Policy verwenden.

## Address States

Eine SLAAC-Adresse besitzt einen definierten Zustand.

```text
Tentative
   ↓
Preferred
   ↓
Deprecated
   ↓
Invalid
```

Eine `Tentative` Adresse darf nicht wie eine vollständig aktive Adresse verwendet werden.

## Duplicate Address Detection

Neue SLAAC-Adressen müssen auf mögliche Adresskonflikte geprüft werden.

```text
Generated Address
       ↓
DAD
   ┌───┴───┐
Unique   Duplicate
  ↓          ↓
Use       Reject
```

Ein erkannter Konflikt muss kontrolliert behandelt werden.

## Lifetimes

Router Advertisements dürfen Gültigkeitszeiten für Präfixe und Adressen vorgeben.

```text
Preferred Lifetime
       ↓
Deprecated

Valid Lifetime
       ↓
Invalid
```

Abgelaufene Adressen dürfen nicht unbegrenzt weiter als gültige Quelladressen verwendet werden.

## Privacy Addresses

NovaOS soll temporäre IPv6-Adressen unterstützen können.

```text
Stable Address
      +
Temporary Addresses
```

Privacy-Adressen dürfen regelmäßig erneuert werden, ohne die logische Identität des Netzwerkinterfaces zu verändern.

Die Verwendung muss durch System- oder Benutzerpolicy steuerbar sein.

## DHCPv6-Koexistenz

SLAAC und DHCPv6 dürfen gleichzeitig verwendet werden.

```text
Router Advertisement
       ↓
Configuration Policy
    ↙           ↘
SLAAC          DHCPv6
```

Router-Advertisement-Flags dürfen bei der Wahl zusätzlicher Konfigurationsmechanismen berücksichtigt werden.

SLAAC darf DHCPv6 nicht grundsätzlich ausschließen.

## Routing

Über Router Advertisements gelernte Router- und Präfixinformationen werden an die Routing-Infrastruktur übergeben.

```text
Router Advertisement
       ↓
Validated Route Data
       ↓
Routing
```

Gelernte Routinginformationen erzeugen keine Netzwerkberechtigung.

## DNS-Konfiguration

SLAAC selbst ist primär für IPv6-Adresskonfiguration zuständig.

Zusätzliche IPv6-Konfigurationsmechanismen dürfen DNS-Resolverinformationen bereitstellen.

```text
IPv6 Configuration
      ↓
DNS Configuration
      ↓
Nova Resolver
```

DNS-Konfiguration bleibt vom eigentlichen SLAAC-Adressmodell getrennt.

## Netzwerkwechsel

SLAAC muss dynamische Netzwerkänderungen unterstützen.

```text
Interface Up
     ↓
Discover
     ↓
Configure
     ↓
Network Change
     ↓
Revalidate
```

Präfixe und Router eines vorherigen Netzwerks dürfen nicht unkontrolliert auf ein neues Netzwerk übertragen werden.

## Sicherheit

ICMPv6 Router Advertisements sind grundsätzlich als nicht vertrauenswürdig zu behandeln.

Validiert werden müssen insbesondere:

```text
Source
Message Structure
Prefix Information
Option Lengths
Lifetimes
MTU
Router Information
```

SLAAC darf keine Firewall-, Capability- oder Sicherheitsrichtlinien verändern.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Known Routers
Prefixes
Generated Addresses
Temporary Addresses
Pending DAD
Router Advertisement State
```

Manipulierte Router Advertisements dürfen keinen unbegrenzten Systemzustand erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
InterfaceID
SLAAC State
IPv6 Addresses
Address State
Prefixes
Default Routers
Preferred Lifetime
Valid Lifetime
DAD State
```

## Normative Anforderungen

1. NovaOS MUSS SLAAC für IPv6 unterstützen.
2. SLAAC MUSS mit IPv6 Neighbor Discovery zusammenarbeiten.
3. Router Advertisements MÜSSEN als nicht vertrauenswürdige Netzwerkdaten behandelt werden.
4. Über SLAAC erzeugte Adressen DÜRFEN NICHT als Interface-Identität verwendet werden.
5. Neue SLAAC-Adressen MÜSSEN Duplicate Address Detection unterstützen.
6. Adressen MÜSSEN Preferred- und Valid-Lifetimes berücksichtigen.
7. NovaOS SOLL temporäre Privacy-Adressen unterstützen.
8. SLAAC MUSS mit DHCPv6 koexistieren können.
9. Gelernte Routerinformationen MÜSSEN kontrolliert an das Routing-System übergeben werden.
10. Netzwerkwechsel MÜSSEN eine Neubewertung der SLAAC-Konfiguration ermöglichen.
11. SLAAC-Zustände MÜSSEN gegen unkontrollierten Ressourcenverbrauch geschützt werden.
12. Adress-, Präfix-, Router- und DAD-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-DHCP-0001`
- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0014`

## Ergebnis

```text
IPv6 Network
     ↓
Router Discovery
     ↓
SLAAC
     ↓
Validated IPv6 Address
     ↓
Routing + Network Policy
```

NovaOS erhält damit eine native und kontrollierte IPv6-Autokonfiguration, die SLAAC, Address Lifetimes, Duplicate Address Detection und Privacy-Adressen mit der gemeinsamen NovaOS-Netzwerkarchitektur verbindet.