# NPSPEC-RESOURCE-NETWORK-0001 – Nova Network Resource Model

## Status

Angenommen

## Kategorie

Resource / Network / Resource Management

## Zweck

NovaOS definiert Netzwerkkommunikation als explizite, budgetierbare, messbare und kontrollierbare Systemressource.

```text
Network Demand
      ↓
Capability + Budget + Policy
      ↓
Network Resource Resolution
      ↓
QoS + Routing
      ↓
Transport
      ↓
Accounting
```

Das Modell verbindet Netzwerkzugriff mit Resource Economy, QoS, Data Sovereignty, Security und Execution Contracts.

## Grundprinzipien

```text
Network Access ≠ Unlimited Network Usage
Network Capability ≠ Network Budget
Connectivity ≠ Authority
Bandwidth ≠ Guaranteed Throughput
Address ≠ Identity
Route ≠ Authority
Interface Access ≠ Internet Access
Encrypted Connection ≠ Trusted Connection
Reservation ≠ Consumption
```

## Network Resource

Eine Netzwerkressource wird semantisch beschrieben durch:

```text
NetworkResource
├── ResourceID
├── ResourceTypeID
├── State
├── Capacity
└── Network Domain
```

Optional:

```text
Interface
Namespace
Bandwidth
Latency
MTU
Address Families
Transport Support
Cost
Trust Level
Sovereignty Domain
Metering
Energy Properties
```

## Network Requirement

Workloads können ihren Netzwerkbedarf deklarieren.

```text
NetworkRequirement
├── Connectivity Requirement
├── Traffic Class
└── Network Budget
```

Optional:

```text
Minimum Bandwidth
Maximum Latency
Deadline
Destination Constraints
Protocol Requirements
Trust Requirements
Sovereignty Requirements
Encryption Requirements
Multipath Policy
Locality
```

## Network Budget

Resource Economy kann Netzwerkbudgets definieren.

```text
NetworkBudget
├── Byte Limit
├── Packet Limit
├── Time Window
└── Enforcement Policy
```

Optional:

```text
Bandwidth Limit
Connection Limit
Burst Limit
Reservation
Cost Limit
```

Budgets können hierarchisch gelten:

```text
System
 ↓
Application
 ↓
Process
 ↓
Task
```

## Bandbreite

Bandbreite muss als begrenzte Ressource behandelt werden können.

```text
Available Bandwidth
        ↓
Reservations
        ↓
QoS
        ↓
Shared Capacity
```

Priorität darf Bandbreitenlimits nicht automatisch aufheben.

## Verbindungen

Verbindungen stellen ebenfalls Ressourcen dar.

Accounting kann berücksichtigen:

```text
Active Connections
Connection Rate
Connection Duration
Streams
Sessions
```

Eine Anwendung darf nicht allein aufgrund vorhandener Netzwerkautorität unbegrenzt Verbindungen erzeugen.

## QoS

Netzwerkverkehr kann Serviceklassen erhalten.

```text
Realtime
Interactive
Normal
Background
Bulk
```

QoS kann Anforderungen definieren für:

```text
Latency
Bandwidth
Jitter
Packet Loss
Priority
```

QoS ist keine Garantie ohne entsprechende Ressourcenreservierung.

## Routing

Routing und Resource Economy bleiben getrennte Aufgaben.

```text
Network Requirement
        ↓
Eligible Paths
        ↓
Routing Policy
        ↓
Selected Path
```

Die Auswahl kann berücksichtigen:

```text
Latency
Bandwidth
Congestion
Cost
Trust
Sovereignty
Energy
Availability
```

## Multipath

NovaOS kann mehrere Netzwerkpfade gleichzeitig verwenden.

```text
Connection
├── Wi-Fi
├── Ethernet
└── Cellular
```

Resource Accounting muss Nutzung pro Pfad und aggregiert erfassen können.

## Data Sovereignty

Netzwerkressourcen müssen Sovereignty Constraints berücksichtigen können.

```text
ExecutionContract
      ↓
Allowed Regions / Domains
      ↓
Network Path Selection
```

Ein technisch erreichbares Ziel ist nicht automatisch ein zulässiges Ziel.

```text
Reachable ≠ Permitted
```

## Capability Security

Netzwerkzugriff benötigt explizite Capabilities.

```text
Network Capability
        +
Network Budget
        ↓
Permitted Communication
```

Capabilities können beschränkt werden auf:

```text
Destination
Protocol
Port / Service
Network Namespace
Direction
Purpose
Duration
Traffic Volume
```

## Network Namespaces

Workloads können isolierte Netzwerkumgebungen besitzen.

```text
Application
    ↓
Network Namespace
    ↓
Allowed Network Resources
```

Namespace-Mitgliedschaft allein erzeugt keine zusätzliche Netzwerkautorität.

## Congestion

Netzwerküberlastung muss erkennbar und kontrolliert behandelbar sein.

```text
Normal
 ↓
Congested
 ↓
Severely Congested
```

Mögliche Reaktionen:

```text
Backpressure
Rate Limiting
Traffic Shaping
Alternative Route
Multipath
Reduced Quality
Controlled Failure
```

## ExecutionContract

Ein ExecutionContract kann Netzwerkbedingungen definieren.

```text
ExecutionContract
├── Network Budget
├── Latency
├── Bandwidth
├── Deadline
├── Trust
├── Sovereignty
├── Security
└── Location Constraints
```

Hard Requirements dürfen durch Routing oder Optimierung nicht abgeschwächt werden.

## Network Accounting

Mindestens folgende Werte sollen messbar sein:

```text
Bytes Sent
Bytes Received
Packets Sent
Packets Received
Connections
Bandwidth
Connection Duration
Retransmissions
```

Die Zuordnung kann erfolgen nach:

```text
Application
Process
Task
Service
Agent
ExecutionContract
Accounting Domain
```

## Adaptive Optimierung

NovaOS darf Netzwerkpfade dynamisch optimieren.

```text
Observe
 ↓
Measure
 ↓
Predict
 ↓
Select Path
 ↓
Measure Result
```

Dabei können Latenz, Bandbreite, Kosten und Energie berücksichtigt werden.

Security-, Trust-, Sovereignty- und Hard Constraints haben Vorrang.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Available Network Resources
Current Usage
Bandwidth
Latency
Connections
Reservations
Budgets
QoS State
Congestion State
Active Paths
Sovereignty Constraints
Accounting
```

## Normative Anforderungen

1. NovaOS MUSS Netzwerkkommunikation als explizite Systemressource behandeln.
2. Netzwerkverbrauch MUSS durch Resource Accounting messbar sein.
3. Network Budgets MÜSSEN hierarchisch begrenzbar sein.
4. Network Capability und Network Budget MÜSSEN getrennte Konzepte bleiben.
5. Bandbreite, Verbindungen und Traffic-Mengen MÜSSEN kontrollierbar sein.
6. QoS und Routing MÜSSEN Resource Requirements berücksichtigen können.
7. Multipath-Nutzung MUSS getrennt und aggregiert abrechenbar sein.
8. Sovereignty- und Trust-Anforderungen MÜSSEN die Auswahl von Netzwerkpfaden einschränken können.
9. Adaptive Netzwerkoptimierung DARF Hard Requirements oder Sicherheitsregeln NICHT überschreiben.
10. Netzwerkressourcen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-CONGESTION-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-INTROSPECTION-0001`
- `ADR-ARCH-0036`

## Ergebnis

```text
Network Capacity
       +
Communication Demand
       ↓
Capability + Budget + Policy
       ↓
QoS + Routing + Sovereignty
       ↓
Controlled Communication
       ↓
Accounting + Feedback
```

NovaOS erhält damit ein einheitliches Network Resource Model, das Netzwerkkommunikation systemweit budgetiert, abrechnet und kontrolliert und gleichzeitig QoS, Routing, Multipath, Security, Trust und Data Sovereignty in die Ressourcenverwaltung integriert.