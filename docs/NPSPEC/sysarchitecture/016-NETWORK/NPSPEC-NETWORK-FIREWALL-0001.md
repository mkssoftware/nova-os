# NPSPEC-NETWORK-FIREWALL-0001 – Nova Network Firewall

## Status

Angenommen

## Kategorie

Network / Security / Firewall

## Zweck

NovaOS definiert eine integrierte Firewall zur kontrollierten Filterung von ein- und ausgehendem Netzwerkverkehr.

```text
Network Traffic
      ↓
Firewall Policy
      ↓
Allow / Reject / Drop
      ↓
Network Stack
```

Die Firewall arbeitet mit Netzwerkflows, Sicherheitskontexten und Capabilities zusammen und bleibt von Routing und Transportmechanismen getrennt.

## Grundprinzip

```text
Reachable ≠ Allowed
Route ≠ Permission
Open Port ≠ Authorization
Packet Accepted ≠ Application Trusted
```

Netzwerkerreichbarkeit darf niemals automatisch Netzwerkberechtigung bedeuten.

## Firewall-Modell

Eine Firewall-Regel besitzt mindestens:

```text
FirewallRule
├── RuleID
├── Direction
├── Match
├── Action
├── Priority
└── State
```

Optional:

```text
Interface
Protocol
Source
Destination
Port
Flow
Application
Security Context
Capability
QoS
Lifetime
```

Regelidentität und Berechtigung bleiben getrennt.

## Richtungen

Die Firewall muss mindestens unterscheiden:

```text
Inbound
Outbound
Forwarded
```

Regeln dürfen zusätzlich an bestimmte Interfaces, Netzwerkpfade oder Sicherheitsdomains gebunden werden.

## Aktionen

Mindestens folgende Aktionen müssen möglich sein:

```text
Allow
Drop
Reject
```

Erweiterbar sind beispielsweise:

```text
Log
Rate Limit
Redirect
Quarantine
```

`Drop` verwirft Verkehr ohne aktive Antwort.

`Reject` darf eine geeignete Fehlerantwort erzeugen.

## Stateful Filtering

Die Firewall soll verbindungs- und flowbezogenen Zustand berücksichtigen können.

```text
Packet
  ↓
Flow / Connection State
  ↓
Firewall Decision
```

Beispiele:

```text
New
Established
Related
Invalid
```

State Tracking muss ressourcenbegrenzt sein.

## Flow-Integration

Die Firewall arbeitet mit dem Nova Network Flow Model zusammen.

```text
NetworkFlow
     ↓
Firewall Policy
     ↓
Allowed / Denied
```

Regeln dürfen dadurch nicht nur anhand von IP-Adressen und Ports, sondern auch anhand logischer Systeminformationen entscheiden.

## Prozess- und Capability-Bezug

NovaOS darf Firewall-Regeln an Anwendungen, Services und Sicherheitskontexte binden.

```text
Process / Service
       ↓
Security Context
       ↓
Network Capability
       ↓
Firewall
```

Dadurch können beispielsweise unterschiedliche Programme unterschiedliche Netzwerkrechte besitzen.

Eine Capability ersetzt jedoch nicht automatisch eine Firewall-Entscheidung.

## Routing-Integration

Firewall und Routing bleiben getrennte Mechanismen.

```text
Firewall Policy
      ↕
Network Flow
      ↕
Routing
```

Eine gültige Route darf eine Firewall-Regel nicht umgehen.

Ein Routing- oder Multipath-Wechsel muss weiterhin die geltenden Firewall-Regeln erfüllen.

## Protokolle

Die Firewall muss mindestens mit folgenden Protokollen arbeiten können:

```text
IPv4
IPv6
TCP
UDP
ICMP / ICMPv6
QUIC Traffic
```

Die Architektur muss für zukünftige Protokolle erweiterbar bleiben.

## Default Policy

NovaOS muss definierte Standardrichtlinien unterstützen.

Beispielsweise:

```text
Inbound  → Deny unless allowed
Outbound → Policy controlled
Forward  → Deny unless allowed
```

Die konkrete System- oder Benutzerpolicy darf darauf aufbauen.

Unbekannter Verkehr darf nicht automatisch privilegiert werden.

## Regelpriorität

Wenn mehrere Regeln zutreffen, muss die Entscheidung deterministisch sein.

```text
Traffic
   ↓
Matching Rules
   ↓
Defined Priority
   ↓
Decision
```

Die Reihenfolge und Konfliktauflösung müssen eindeutig definiert und introspektierbar sein.

## Rate Limiting

Die Firewall darf Netzwerkverkehr begrenzen.

```text
Flow
 ↓
Rate Limit
 ↓
Controlled Traffic
```

Dies darf zum Schutz gegen Ressourcenmissbrauch, Flooding oder fehlerhafte Anwendungen verwendet werden.

Rate Limits müssen begrenzt und nachvollziehbar sein.

## Dynamische Regeln

Firewall-Regeln dürfen zur Laufzeit hinzugefügt, geändert oder entfernt werden.

```text
Policy Update
     ↓
Validate
     ↓
Atomic Activation
```

Regeländerungen sollen möglichst atomar aktiviert werden, damit kein undefinierter Zwischenzustand entsteht.

## Sicherheit

Änderungen der Firewall-Konfiguration müssen autorisiert werden.

Mindestens kontrollierbar sind:

```text
Add Rule
Modify Rule
Remove Rule
Disable Firewall
Change Default Policy
Inspect Sensitive State
```

Normale Anwendungen dürfen ihre Netzwerkrechte nicht durch eigene Firewall-Regeln erweitern.

## Ressourcensteuerung

Die Firewall muss Ressourcen begrenzen können für:

```text
Rules
Connection State
Flow State
Rate Limit State
Logs
Pending Decisions
```

Manipulierter Netzwerkverkehr darf keinen unbegrenzten Firewall-Zustand erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RuleID
Rule State
Matched Traffic
Action
Flow
Application
Interface
Packet Counters
Drop Counters
State Table Usage
```

Sensible Netzwerk- oder Prozessinformationen dürfen nur mit entsprechender Berechtigung sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS eine integrierte Netzwerk-Firewall bereitstellen.
2. Firewall und Routing MÜSSEN getrennte Mechanismen bleiben.
3. Die Firewall MUSS eingehenden, ausgehenden und weitergeleiteten Verkehr unterscheiden können.
4. Die Aktionen `Allow`, `Drop` und `Reject` MÜSSEN unterstützt werden.
5. Stateful Filtering SOLL unterstützt werden.
6. Firewall-Regeln MÜSSEN mit Network Flows verknüpfbar sein.
7. Regeln SOLLEN Anwendungen und Sicherheitskontexte berücksichtigen können.
8. Routing- und Multipath-Wechsel DÜRFEN Firewall-Regeln NICHT umgehen.
9. Konflikte zwischen Regeln MÜSSEN deterministisch aufgelöst werden.
10. Firewall-Zustände und Tabellen MÜSSEN ressourcenbegrenzt sein.
11. Änderungen an Firewall-Regeln MÜSSEN autorisiert werden.
12. Firewall-Entscheidungen und relevante Statistiken SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `ADR-NETWORK-0010`

## Ergebnis

```text
Application / Network
        ↓
Network Flow
        ↓
Firewall Policy
        ↓
Allow / Reject / Drop
        ↓
Routing + Transport
```

NovaOS erhält damit eine systemweit integrierte, flow- und capability-fähige Firewall, die Netzwerkzugriffe kontrolliert, ohne Routing-, Transport- oder Anwendungslogik miteinander zu vermischen.