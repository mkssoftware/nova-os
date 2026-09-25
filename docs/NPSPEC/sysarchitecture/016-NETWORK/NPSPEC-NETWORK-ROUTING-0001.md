# NPSPEC-NETWORK-ROUTING-0001 – Nova Network Routing

## Status

Angenommen

## Kategorie

Network / Routing / Path Selection

## Zweck

NovaOS definiert ein gemeinsames Routing-Modell für die Auswahl von Netzwerkpfaden über IPv4, IPv6 und zukünftige Netzwerkprotokolle.

```text
Destination
    ↓
Routing Decision
    ↓
Route
    ↓
Interface + Next Hop
```

Routing bestimmt den geeigneten Netzwerkpfad, erzeugt jedoch keine Zugriffsberechtigung.

## Grundprinzip

```text
Route ≠ Permission
Destination ≠ Route
Interface ≠ Route
Route Identity ≠ Physical Path
```

Routing bleibt von Firewall, Capability-System und Transportprotokollen getrennt.

## Route-Modell

Eine Route besitzt mindestens:

```text
NetworkRoute
├── RouteID
├── Destination
├── Next Hop
├── Interface
├── Metric
└── State
```

Optional:

```text
Source Constraint
Flow Constraint
Policy
QoS
Security Context
Lifetime
Provider
```

Eine `RouteID` identifiziert die Route logisch und stellt keine Berechtigung dar.

## Routing Table

NovaOS muss mehrere Routen gleichzeitig verwalten können.

```text
Routing Table
├── Local Routes
├── Network Routes
├── Host Routes
└── Default Routes
```

IPv4 und IPv6 dürfen gemeinsame Routing-Mechanismen verwenden, behalten jedoch ihre protokollspezifischen Adress- und Präfixregeln.

## Route Selection

Die Auswahl einer Route darf mindestens berücksichtigen:

```text
Destination
Prefix
Metric
Interface State
Source Address
Flow Policy
QoS
Security Policy
```

Grundsätzlich wird die spezifischste passende Route bevorzugt.

Weitere Policy-Entscheidungen dürfen anschließend die Auswahl beeinflussen.

## Next Hop

Eine Route darf direkt oder über einen Next Hop führen.

```text
Destination
   ↓
Route
   ├── Direct → Interface
   └── Gateway → Next Hop → Interface
```

Die Auflösung des Next Hop auf eine Link-Layer-Adresse erfolgt durch den jeweiligen Netzwerkmechanismus, beispielsweise ARP oder IPv6 Neighbor Discovery.

## Multipath Routing

NovaOS soll mehrere geeignete Pfade zu einem Ziel verwalten können.

```text
Destination
   ↓
Route Set
 ├── Path A
 ├── Path B
 └── Path C
```

Die Auswahl darf anhand von Metrik, QoS, Flow-Eigenschaften, Verfügbarkeit und Policy erfolgen.

Multipath-Routing darf keine Transportsemantik verletzen.

## Flow-Integration

Routing soll mit dem Nova Network Flow Model zusammenarbeiten.

```text
NetworkFlow
     ↓
Routing Policy
     ↓
Selected Path
```

Ein bestehender Flow darf einen anderen Pfad erhalten, sofern Transportprotokoll und Sicherheitsregeln dies zulassen.

## Dynamische Änderungen

Routing muss auf Änderungen der Netzwerktopologie reagieren können.

```text
Interface Down
     ↓
Route Invalid
     ↓
Recalculate
     ↓
Alternative Route
```

Mögliche Ereignisse sind:

```text
Interface Added
Interface Removed
Address Changed
Gateway Changed
Route Added
Route Removed
Path Failed
```

Veraltete Routen dürfen nicht unbegrenzt weiterverwendet werden.

## Policy Routing

NovaOS darf Routingentscheidungen zusätzlich anhand von Policy treffen.

Beispiele:

```text
Application
Security Context
Traffic Class
Network Flow
QoS
Data Sovereignty
Interface Type
```

Policy Routing darf bestehende Sicherheitsregeln nicht umgehen.

## Routing Cache

Routingentscheidungen dürfen gecacht werden.

```text
Destination / Flow
        ↓
Routing Cache
        ↓
Selected Route
```

Cache-Einträge müssen invalidiert werden, wenn ihre zugrunde liegende Route oder Policy ungültig wird.

## Sicherheit

Routinginformationen sind sicherheitsrelevanter Systemzustand.

Änderungen an Routingtabellen müssen autorisiert sein.

Insbesondere müssen kontrollierbar sein:

```text
Add Route
Remove Route
Modify Route
Change Gateway
Change Metric
Change Policy
```

Extern gelernte Routinginformationen dürfen nicht automatisch als vertrauenswürdig gelten.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Routes
Routing Cache
Policy Rules
Multipath State
Dynamic Route State
```

Manipulierte Netzwerkereignisse dürfen kein unbegrenztes Wachstum der Routingstrukturen verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RouteID
Destination
Next Hop
Interface
Metric
State
Source
Policy
Usage
Last Change
```

Routingentscheidungen sollen für Diagnosezwecke nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Routing-Modell bereitstellen.
2. IPv4 und IPv6 MÜSSEN in die Routing-Infrastruktur integriert werden.
3. Routing und Netzwerkberechtigung MÜSSEN getrennt bleiben.
4. Routen MÜSSEN eindeutig identifizierbar sein.
5. Die Routenauswahl MUSS Präfix und Ziel berücksichtigen.
6. Interface-Zustände MÜSSEN bei Routingentscheidungen berücksichtigt werden.
7. NovaOS SOLL mehrere mögliche Netzwerkpfade verwalten können.
8. Routing SOLL mit dem Network Flow Model zusammenarbeiten.
9. Änderungen der Netzwerktopologie MÜSSEN Routingzustände aktualisieren können.
10. Routing-Cache-Einträge MÜSSEN bei ungültigen Grundlagen invalidierbar sein.
11. Änderungen an Routingtabellen MÜSSEN autorisiert werden.
12. Routingzustand und Routingentscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-NETWORK-0008`

## Ergebnis

```text
Destination / Flow
        ↓
Routing + Policy
        ↓
Route Selection
        ↓
Interface + Next Hop
        ↓
Network Path
```

NovaOS erhält damit eine gemeinsame, protokollunabhängige Routing-Infrastruktur, die IPv4, IPv6, Flow-Steuerung, Multipath und dynamische Netzwerkänderungen kontrolliert zusammenführt.