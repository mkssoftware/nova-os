# NPSPEC-NETWORK-INTROSPECTION-0001 – Nova Network Introspection

## Status

Angenommen

## Kategorie

Network / Introspection / Observability

## Zweck

NovaOS definiert eine gemeinsame Introspection-Infrastruktur zur strukturierten Beobachtung des gesamten Netzwerkzustands.

```text
Network Stack
     ↓
Structured State
     ↓
Network Introspection
     ↓
Diagnostics / UI / Security / Automation
```

Ziel ist nicht nur die Anzeige von Statistiken, sondern die nachvollziehbare Darstellung von Netzwerkobjekten, Beziehungen, Entscheidungen und Fehlerzuständen.

## Grundprinzip

```text
Observable ≠ Mutable
Visible ≠ Authorized
Metric ≠ Diagnosis
Network State ≠ Network Permission
```

Introspection gewährt keine Kontrolle über beobachtete Netzwerkressourcen.

## Objektmodell

Netzwerkobjekte sollen über stabile IDs referenziert werden.

```text
NetworkObject
├── ObjectID
├── Type
├── State
├── Relationships
└── Metrics
```

Beispiele:

```text
InterfaceID
FlowID
ConnectionID
PathID
RouteID
NamespaceID
VPNID
IntentID
MigrationID
```

IDs bleiben möglichst unabhängig von Namen, Adressen und physischem Standort.

## Beobachtbare Bereiche

Network Introspection soll mindestens folgende Bereiche integrieren können:

```text
Interfaces
Addresses
Routes
Connections
Flows
DNS
Firewall
NAT
Namespaces
Virtual Networks
VPN
QoS
Congestion
Multipath
Migration
Sovereignty
Network Intent
```

Die Daten werden über ein gemeinsames Modell zugänglich gemacht, ohne die jeweiligen Subsysteme zu einer monolithischen Netzwerkkomponente zusammenzuführen.

## Beziehungen

Neben Einzelobjekten müssen Beziehungen sichtbar sein.

```text
Process
   ↓
Namespace
   ↓
Flow
   ↓
Connection
   ↓
Route
   ↓
Path
   ↓
Interface
```

Dadurch kann nachvollzogen werden, wie eine Netzwerkoperation tatsächlich ausgeführt wird.

## Entscheidungsgründe

NovaOS soll nicht nur das Ergebnis, sondern soweit möglich auch den Entscheidungsgrund darstellen.

```text
Intent
  ↓
Candidate Paths
  ↓
Policy Evaluation
  ↓
Rejected Paths
  ↓
Selected Path
```

Beispielsweise:

```text
Path A → rejected: Sovereignty
Path B → rejected: Firewall
Path C → selected: valid + low latency
```

Damit können automatische Netzwerkentscheidungen nachvollzogen werden.

## Live-Zustand

Introspection soll sowohl aktuellen Zustand als auch relevante Zustandsänderungen bereitstellen.

```text
Snapshot
   +
Event Stream
```

Beispiele:

```text
Interface Up/Down
Address Changed
Route Changed
Flow Created
VPN Lost
Congestion Detected
Migration Started
Migration Completed
Policy Rejected
```

Polling soll nicht die einzige Möglichkeit zur Beobachtung sein.

## Metriken

Netzwerkobjekte dürfen strukturierte Metriken bereitstellen.

```text
Bytes Sent / Received
Packets Sent / Received
Latency
RTT
Jitter
Packet Loss
Bandwidth
Queue Pressure
Retransmissions
Dropped Packets
```

Metriken müssen ihrem jeweiligen Objekt und Kontext eindeutig zugeordnet werden können.

## Fehlerdiagnose

Fehler sollen strukturierte Ursacheninformationen liefern.

```text
Operation Failed
      ↓
Reason
├── DNS Failure
├── No Route
├── Firewall Denied
├── Sovereignty Violation
├── VPN Required
├── Timeout
└── Resource Limit
```

Ein generisches `Network Error` soll vermieden werden, wenn eine genauere Ursache bekannt ist.

## Sicherheit

Introspection muss Capability- und Security-Kontexte berücksichtigen.

```text
Observer
   ↓
Authorization
   ↓
Filtered Network View
```

Ein Prozess darf nur Netzwerkzustände sehen, für die er eine entsprechende Beobachtungsberechtigung besitzt.

Sensible Daten wie:

```text
Remote Endpoints
DNS Queries
Traffic Metadata
VPN Information
Security Policies
Credentials
Keys
```

müssen geschützt oder ausgeblendet werden.

Geheime Schlüssel dürfen niemals über normale Network Introspection ausgegeben werden.

## Network Namespaces

Introspection muss Namespace-Grenzen respektieren.

```text
Namespace A → View A
Namespace B → View B
```

Systemdiagnosewerkzeuge dürfen bei entsprechender Berechtigung mehrere Namespaces beobachten.

## Historie

NovaOS darf eine begrenzte Historie wichtiger Netzwerkereignisse führen.

```text
Timestamp
ObjectID
Event
Old State
New State
Reason
```

Die Historie muss ressourcenbegrenzt sein und darf keine unbegrenzte Verkehrsaufzeichnung darstellen.

## Performance

Introspection darf den normalen Netzwerkpfad nicht wesentlich beeinträchtigen.

Hot-Path-Metriken sollen möglichst über kostengünstige Counter, strukturierte Events oder aggregierte Daten erfasst werden.

Detailliertes Tracing darf explizit aktivierbar sein.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Metrics
Event Buffers
History
Subscribers
Tracing
Diagnostic State
```

Bei Überlastung dürfen diagnostische Detaildaten kontrolliert reduziert werden, ohne die Netzwerkfunktion selbst zu gefährden.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Network-Introspection-Infrastruktur bereitstellen.
2. Netzwerkobjekte SOLLEN über stabile IDs referenzierbar sein.
3. Introspection MUSS Beziehungen zwischen Netzwerkobjekten darstellen können.
4. Beobachtung DARF keine implizite Änderungsberechtigung erzeugen.
5. Introspection MUSS Capability- und Namespace-Grenzen respektieren.
6. Sensible Netzwerkdaten MÜSSEN entsprechend ihrer Berechtigungen gefiltert werden.
7. Kryptografische Geheimnisse DÜRFEN NICHT über normale Introspection ausgegeben werden.
8. NovaOS SOLL strukturierte Netzwerkereignisse bereitstellen.
9. Fehler SOLLEN möglichst mit konkreten Ursachen statt generischen Fehlercodes dargestellt werden.
10. Automatische Netzwerkentscheidungen SOLLEN nachvollziehbare Entscheidungsgründe bereitstellen.
11. Introspection-Ressourcen MÜSSEN begrenzt sein.
12. Introspection DARF den normalen Netzwerkbetrieb NICHT wesentlich beeinträchtigen.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-VPN-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-CONGESTION-0001`
- `NPSPEC-NETWORK-MIGRATION-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0026`

## Ergebnis

```text
Network Objects
      ↓
Structured State + Events
      ↓
Network Introspection
      ↓
Observe → Explain → Diagnose
```

NovaOS erhält damit eine einheitliche Beobachtungsschicht, durch die Netzwerkzustände, Ressourcen, Beziehungen und automatische Entscheidungen transparent nachvollzogen werden können, ohne Sicherheits- oder Isolationsgrenzen aufzuheben.