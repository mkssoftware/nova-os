# NPSPEC-NETWORK-MULTIPATH-0001 – Nova Network Multipath

## Status

Angenommen

## Kategorie

Network / Routing / Multipath

## Zweck

NovaOS definiert Multipath Networking als Fähigkeit, mehrere Netzwerkpfade gleichzeitig zu erkennen, zu bewerten und kontrolliert für Netzwerkverkehr zu verwenden.

```text
Network Flow
     ↓
Multipath Selection
  ┌──┼──┐
  ↓  ↓  ↓
Path A B C
```

Multipath ermöglicht Redundanz, Lastverteilung, Failover und die Nutzung mehrerer Netzwerkinterfaces.

## Grundprinzip

```text
Multiple Paths ≠ Multiple Connections
Path ≠ Interface
Best Path ≠ Fastest Path
Failover ≠ Permission Bypass
```

Die Wahl eines Netzwerkpfades muss Routing-, Sicherheits-, QoS- und Ressourcenanforderungen berücksichtigen.

## Path-Modell

Ein Netzwerkpfad besitzt mindestens:

```text
NetworkPath
├── PathID
├── Source
├── Destination
├── Interface
├── Next Hop
└── State
```

Optional:

```text
Latency
Bandwidth
Packet Loss
Cost
QoS
Security Properties
Trust Level
```

Die `PathID` identifiziert einen logischen Pfad und stellt keine Zugriffsberechtigung dar.

## Path States

Ein Pfad kann mindestens folgende Zustände besitzen:

```text
Unknown
   ↓
Validating
   ↓
Available
   ↓
Degraded
   ↓
Unavailable
```

`Unknown` darf nicht automatisch als `Available` behandelt werden.

## Path Discovery

Mögliche Netzwerkpfade entstehen aus:

```text
Routing Tables
Network Interfaces
Gateways
Protocol State
Network Configuration
```

Beispiele:

```text
Ethernet
Wi-Fi
VPN
Mobile Network
Multiple Gateways
```

Mehrere Pfade dürfen über dasselbe Interface existieren.

## Path Validation

Ein Pfad muss vor sicherheitskritischer oder dauerhafter Nutzung validierbar sein.

```text
Candidate Path
      ↓
Validation
      ↓
Available / Rejected
```

Die Validierung darf Erreichbarkeit, Interface-Zustand, Routing, Security Policy und Protokollanforderungen berücksichtigen.

## Path Selection

Die Auswahl darf mehrere Eigenschaften berücksichtigen:

```text
Latency
Bandwidth
Packet Loss
Stability
QoS
Cost
Security
Trust
Resource Pressure
```

Die niedrigste Latenz allein darf nicht automatisch den bevorzugten Pfad bestimmen.

## Flow-Integration

Multipath arbeitet mit dem Nova Network Flow Model zusammen.

```text
NetworkFlow
     ↓
Path Selection
     ↓
NetworkPath
```

Unterschiedliche Flows dürfen unterschiedliche Pfade verwenden.

```text
Flow A → Ethernet
Flow B → Wi-Fi
Flow C → VPN
```

## Failover

Fällt ein aktiver Pfad aus, darf NovaOS einen alternativen gültigen Pfad auswählen.

```text
Path A
  ↓ failure
Path B
  ↓
Continue
```

Ein Failover darf Sicherheits-, Sovereignty- oder Capability-Regeln nicht umgehen.

Kann kein zulässiger Ersatzpfad gefunden werden, muss die Netzwerkoperation fehlschlagen oder kontrolliert warten.

## Load Distribution

Mehrere verfügbare Pfade dürfen gleichzeitig genutzt werden.

```text
Traffic
  ↓
Multipath Policy
 ├→ Path A
 ├→ Path B
 └→ Path C
```

Lastverteilung darf Transport-Ordering und Protokollsemantik nicht verletzen.

## Transport-Integration

Transportprotokolle besitzen unterschiedliche Multipath-Fähigkeiten.

```text
TCP
UDP
QUIC
 ↓
Multipath Layer
```

Ein Transport darf Pfadwechsel oder parallele Pfade nur verwenden, wenn seine Semantik dies unterstützt.

QUIC Connection Migration kann direkt mit dem Multipath-Modell zusammenarbeiten.

## Path Monitoring

Aktive Pfade sollen kontinuierlich beobachtbar sein.

```text
Path
├── RTT
├── Loss
├── Throughput
├── Availability
└── Resource Pressure
```

Messwerte dürfen zur Anpassung zukünftiger Pfadauswahl verwendet werden.

Kurzzeitige Schwankungen sollen nicht unnötig häufige Pfadwechsel verursachen.

## Sicherheit

Jeder verwendete Pfad muss die geltenden Sicherheitsanforderungen erfüllen.

```text
Candidate Path
      ↓
Security / Trust Policy
      ↓
Allowed / Rejected
```

Ein neuer oder schnellerer Pfad darf keine bestehenden Sicherheits- oder Datenhoheitsanforderungen abschwächen.

## Ressourcensteuerung

Multipath muss Ressourcen begrenzen können für:

```text
Known Paths
Path Probes
Path State
Measurements
Failover Attempts
Routing Cache
```

Path Discovery und Monitoring dürfen keinen unbegrenzten Netzwerk- oder CPU-Verbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
PathID
Interface
Next Hop
State
Latency
Bandwidth
Packet Loss
Active Flows
Selection Reason
Last Change
```

Die Ursache einer Pfadauswahl soll für Diagnosezwecke nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS mehrere Netzwerkpfade zu einem Ziel verwalten können.
2. Netzwerkpfade MÜSSEN eindeutig identifizierbar sein.
3. PathID und Netzwerkberechtigung MÜSSEN getrennt bleiben.
4. Unbekannte Pfade DÜRFEN NICHT automatisch als verfügbar gelten.
5. Pfadauswahl MUSS Routing- und Sicherheitsanforderungen berücksichtigen.
6. Pfadauswahl SOLL QoS, Latenz, Verlust und Ressourcen berücksichtigen können.
7. Unterschiedliche Network Flows DÜRFEN unterschiedliche Pfade verwenden.
8. Failover DARF Sicherheits- oder Sovereignty-Regeln NICHT umgehen.
9. Lastverteilung DARF Transport- oder Ordering-Semantik NICHT verletzen.
10. Transportprotokolle MÜSSEN Multipath nur entsprechend ihrer unterstützten Semantik verwenden.
11. Path Discovery und Monitoring MÜSSEN ressourcenbegrenzt sein.
12. Pfadzustand und Auswahlentscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-NETWORK-0009`

## Ergebnis

```text
Network Flow
     ↓
Routing
     ↓
Multipath Selection
  ┌──┼──┐
  ↓  ↓  ↓
Path A B C
     ↓
Controlled Network Transport
```

NovaOS erhält damit eine einheitliche Multipath-Infrastruktur, die mehrere Netzwerkpfade für Failover, Lastverteilung und adaptive Pfadauswahl nutzen kann, ohne Transport-, Sicherheits- oder Ressourcenregeln zu umgehen.