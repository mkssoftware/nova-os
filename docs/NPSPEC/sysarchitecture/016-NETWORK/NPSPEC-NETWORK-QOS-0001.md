# NPSPEC-NETWORK-QOS-0001 – Nova Network Quality of Service

## Status

Angenommen

## Kategorie

Network / QoS / Traffic Management

## Zweck

NovaOS definiert ein gemeinsames Quality-of-Service-Modell zur Steuerung von Latenz, Bandbreite, Priorität und Ressourcenverbrauch von Netzwerkverkehr.

```text
Network Flow
     ↓
QoS Policy
     ↓
Scheduling / Shaping
     ↓
Network Path
```

QoS beschreibt Anforderungen und Ressourcenregeln, garantiert jedoch keine physisch nicht verfügbaren Netzwerkressourcen.

## Grundprinzip

```text
QoS ≠ Permission
Priority ≠ Guaranteed Bandwidth
Deadline ≠ Priority
High Priority ≠ Unlimited Resources
```

Sicherheits-, Routing- und Ressourcenregeln besitzen weiterhin Vorrang vor QoS-Optimierungen.

## QoS-Modell

Ein Netzwerkflow darf einen QoS-Kontext besitzen.

```text
NetworkQoS
├── Priority Class
├── Latency Target
├── Bandwidth Target
├── Bandwidth Limit
└── Resource Budget
```

Optional:

```text
Deadline
Loss Tolerance
Jitter Target
Traffic Class
ExecutionContract
```

QoS-Eigenschaften können aus Anwendung, Service, Network Namespace oder Systempolicy stammen.

## Traffic Classes

NovaOS soll mindestens folgende logische Klassen unterstützen:

```text
Realtime
Interactive
Normal
Background
Maintenance
```

Diese Klassen stellen keine festen Bandbreitenwerte dar.

Die konkrete Umsetzung hängt von Interface, Netzwerkpfad und verfügbarer Kapazität ab.

## Priorität

Priorität beeinflusst die bevorzugte Verarbeitung bei Ressourcenkonflikten.

```text
Network Traffic
      ↓
Priority Classification
      ↓
Queue Scheduling
```

Hohe Priorität darf niedrigere Klassen nicht unbegrenzt verdrängen.

Fairness und Ressourcenlimits müssen erhalten bleiben.

## Latenz

Flows dürfen Latenzanforderungen definieren.

```text
Flow
├── Target Latency
└── Maximum Latency
```

NovaOS darf Scheduling, Queue-Auswahl und Multipath-Entscheidungen verwenden, um Latenzanforderungen möglichst einzuhalten.

Eine Latenzanforderung ist keine Garantie, sofern der Netzwerkpfad diese nicht erfüllen kann.

## Bandbreite

QoS muss Bandbreitenanforderungen und -grenzen ausdrücken können.

```text
Minimum Target
Preferred Bandwidth
Maximum Limit
```

Bandbreitenlimits dürfen durch Traffic Shaping umgesetzt werden.

Nicht verfügbare Bandbreite darf nicht künstlich als garantiert dargestellt werden.

## Traffic Shaping

NovaOS darf Datenraten kontrolliert begrenzen.

```text
Network Flow
     ↓
Traffic Shaper
     ↓
Bounded Rate
```

Shaping kann pro Flow, Prozess, Namespace, Interface oder Sicherheitskontext angewendet werden.

## Queue Management

Netzwerkinterfaces dürfen mehrere QoS-Queues besitzen.

```text
Traffic
  ↓
Classification
  ↓
┌─────────────┐
│ Realtime    │
│ Interactive │
│ Normal      │
│ Background  │
└─────────────┘
       ↓
Interface
```

Queues müssen begrenzt sein und Backpressure unterstützen.

## Flow-Integration

QoS wird primär auf Network Flows angewendet.

```text
NetworkFlow
     ↓
QoS Context
     ↓
Network Scheduler
```

Mehrere Verbindungen oder Datagramme dürfen dadurch einer gemeinsamen Ressourcenpolicy zugeordnet werden.

## Routing und Multipath

QoS darf die Auswahl eines Netzwerkpfades beeinflussen.

```text
Flow + QoS
    ↓
Multipath Selection
 ├→ Low Latency Path
 └→ High Bandwidth Path
```

Sicherheits-, Trust- und Sovereignty-Anforderungen dürfen durch eine QoS-Optimierung nicht abgeschwächt werden.

## Transport-Integration

Transportprotokolle dürfen QoS-Informationen berücksichtigen.

```text
TCP
UDP
QUIC
 ↓
QoS Context
```

Congestion Control darf jedoch nicht durch QoS-Vorgaben umgangen werden.

QoS erzeugt keine Berechtigung, das Netzwerk aggressiver als zulässig zu verwenden.

## Execution Contracts

Netzwerk-QoS soll mit `Nova.ExecutionContract` zusammenarbeiten.

```text
ExecutionContract
       ↓
Network Requirements
       ↓
NetworkQoS
```

Dadurch können Latenz-, Deadline- und Ressourcenanforderungen einer Operation bis in den Netzwerkpfad weitergegeben werden.

## Degradation

Kann eine QoS-Anforderung nicht erfüllt werden, muss dies kontrolliert behandelt werden.

```text
QoS Requirement
      ↓
Available Resources?
 ├── Yes → Execute
 └── No  → Degrade / Reject / Report
```

Hard Requirements dürfen nicht stillschweigend als erfüllt behandelt werden.

Soft Requirements dürfen kontrolliert degradiert werden.

## Ressourcensteuerung

QoS muss Ressourcen begrenzen können für:

```text
Bandwidth
Queue Memory
Packet Buffers
Flow State
Scheduling State
CPU Processing
```

QoS darf keine unbegrenzten Ressourcenreservierungen erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
FlowID
Traffic Class
Requested QoS
Effective QoS
Bandwidth
Latency
Jitter
Packet Loss
Queue State
Dropped Packets
```

Angeforderte und tatsächlich erreichte QoS müssen unterscheidbar sein.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Netzwerk-QoS-Modell bereitstellen.
2. QoS und Netzwerkberechtigung MÜSSEN getrennt bleiben.
3. NovaOS SOLL logische Traffic Classes unterstützen.
4. Priorität DARF NICHT unbegrenzte Ressourcen erzeugen.
5. Netzwerkflows MÜSSEN QoS-Eigenschaften tragen können.
6. Bandbreitenlimits MÜSSEN durchsetzbar sein können.
7. Netzwerkqueues MÜSSEN begrenzt sein und Backpressure unterstützen.
8. QoS SOLL Routing- und Multipath-Entscheidungen beeinflussen können.
9. QoS DARF Sicherheits-, Trust- oder Sovereignty-Regeln NICHT umgehen.
10. Hard Requirements DÜRFEN NICHT stillschweigend als erfüllt behandelt werden.
11. Angeforderte und tatsächlich erreichte QoS MÜSSEN unterscheidbar sein.
12. QoS-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0019`

## Ergebnis

```text
Application Requirement
        ↓
ExecutionContract
        ↓
Network Flow + QoS
        ↓
Scheduling + Routing + Multipath
        ↓
Controlled Network Resources
```

NovaOS erhält damit ein einheitliches Netzwerk-QoS-Modell, das Latenz, Bandbreite, Priorität und Ressourcenverbrauch steuert, ohne Sicherheitsregeln oder physische Netzwerkgrenzen zu umgehen.