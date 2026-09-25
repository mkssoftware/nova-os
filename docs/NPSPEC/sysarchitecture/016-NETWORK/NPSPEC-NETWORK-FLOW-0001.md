# NPSPEC-NETWORK-FLOW-0001 – Nova Network Flow Model

## Status

Angenommen

## Kategorie

Network / Flow / Traffic Management

## Zweck

NovaOS definiert Network Flows als logische Beschreibung zusammengehöriger Netzwerkkommunikation.

```text
Packets
   ↓
Flow Classification
   ↓
Network Flow
   ↓
Policy / QoS / Routing
```

Flows ermöglichen es, Netzwerkverkehr unabhängig von einzelnen Paketen zu verwalten, zu priorisieren, zu überwachen und Ressourcen zuzuweisen.

## Grundprinzip

```text
Flow ≠ Connection
Flow ≠ Socket
Flow ≠ Permission
Flow Identity ≠ Network Path
```

Ein Flow beschreibt zusammengehörigen Verkehr, ohne zwingend eine verbindungsorientierte Transportverbindung vorauszusetzen.

## Flow-Modell

Ein Flow besitzt mindestens:

```text
NetworkFlow
├── FlowID
├── Source
├── Destination
├── Protocol
├── Direction
└── State
```

Optional:

```text
Priority
QoS
Deadline
Security Context
ExecutionContract
Resource Budget
Path
```

Die `FlowID` dient der logischen Identifikation und stellt keine Zugriffsberechtigung dar.

## Klassifikation

Pakete müssen einem Flow anhand definierter Merkmale zugeordnet werden können.

```text
Packet
  ↓
Classification
  ↓
FlowID
```

Kriterien dürfen unter anderem sein:

```text
Source Address
Destination Address
Source Port
Destination Port
Protocol
Interface
Security Context
```

Die Klassifikation soll erweiterbar bleiben.

## Flow Lifecycle

Ein Flow kann folgende Zustände besitzen:

```text
Created
   ↓
Active
   ↓
Idle
   ↓
Closed
```

Verbindungslose Protokolle dürfen Flows dynamisch anhand beobachteter Kommunikation erzeugen.

Idle-Flows müssen nach definierter Zeit oder Policy entfernt werden können.

## Transportprotokolle

Flows müssen unabhängig vom konkreten Transportprotokoll funktionieren.

```text
TCP  ─┐
UDP  ─┼→ Network Flow
QUIC ─┘
```

Ein TCP-Flow kann einer Verbindung entsprechen, muss jedoch nicht mit deren interner Identität gleichgesetzt werden.

Eine QUIC-Verbindung kann mehrere logische Streams besitzen, während der darunterliegende Netzwerkverkehr eigene Flow-Zuordnungen verwendet.

## QoS und Priorität

Flows dürfen QoS-Eigenschaften erhalten.

```text
Flow
├── Priority
├── Latency Target
├── Bandwidth
├── Deadline
└── Resource Budget
```

QoS beschreibt Anforderungen und Grenzen, erzeugt jedoch keine unbegrenzten Ressourcen.

## Routing

Flows dürfen Routing-Entscheidungen beeinflussen.

```text
Flow
  ↓
Routing Policy
  ↓
Network Path
```

Ein Flow darf auf einen anderen Netzwerkpfad wechseln, sofern Protokoll und Policy dies erlauben.

```text
FlowID
   ↓
Path A → Path B
```

Die logische Flow-Identität soll dadurch möglichst erhalten bleiben.

## Traffic Control

NovaOS darf Netzwerkverkehr pro Flow steuern.

Mögliche Mechanismen:

```text
Prioritize
Throttle
Shape
Queue
Drop
Redirect
```

Die konkrete Maßnahme muss mit Protokoll-, QoS- und Sicherheitsanforderungen vereinbar sein.

## Backpressure

Überlastung muss auf Flow-Ebene berücksichtigt werden können.

```text
Flow
 ↓
Queue Pressure
 ↓
Throttle / Backpressure
```

Ein einzelner Flow darf nicht automatisch sämtliche Netzwerkressourcen beanspruchen.

## Sicherheit

Flows müssen einem Sicherheitskontext zugeordnet werden können.

```text
Process
   ↓
Security Context
   ↓
Network Flow
```

Firewall- und Netzwerk-Policies dürfen Regeln anhand von Flow-Eigenschaften anwenden.

Eine FlowID darf keine Capability oder Netzwerkberechtigung ersetzen.

## Ressourcensteuerung

NovaOS muss Ressourcen pro Flow begrenzen können.

```text
Bandwidth
Queue Memory
Packet Buffers
Connection State
CPU Processing
```

Ressourcenlimits dürfen zusätzlich pro Prozess, Service, Sicherheitsdomain oder Interface gelten.

## Accounting

Netzwerknutzung soll pro Flow messbar sein.

```text
FlowID
├── Bytes Sent
├── Bytes Received
├── Packets
├── Drops
└── Lifetime
```

Diese Informationen können für Ressourcenökonomie, Diagnose und adaptive Optimierung verwendet werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
FlowID
Source
Destination
Protocol
State
Security Context
QoS
Current Path
Bytes
Packets
Drops
Latency
```

Payload-Inhalte dürfen dabei nicht ohne entsprechende Berechtigung offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Netzwerkverkehr als logische Flows beschreiben können.
2. Jeder Flow MUSS eindeutig identifizierbar sein.
3. FlowID und Netzwerkberechtigung MÜSSEN getrennt bleiben.
4. Flow-Klassifikation MUSS mehrere Netzwerk- und Sicherheitsmerkmale berücksichtigen können.
5. Flows MÜSSEN unabhängig von TCP, UDP oder QUIC verwendbar sein.
6. Flows SOLLEN QoS-, Prioritäts- und Ressourceninformationen tragen können.
7. QoS DARF NICHT unbegrenzte Netzwerkressourcen erzeugen.
8. Routing DARF Netzwerkpfade ändern, ohne zwingend die logische Flow-Identität zu verändern.
9. Traffic Control MUSS pro Flow möglich sein können.
10. Ein einzelner Flow DARF NICHT unkontrolliert sämtliche Netzwerkressourcen beanspruchen.
11. Netzwerkverbrauch SOLL pro Flow abrechenbar sein.
12. Flow-Zustand, Ressourcenverbrauch und Netzwerkpfad SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0007`

## Ergebnis

```text
Network Traffic
      ↓
Flow Classification
      ↓
NetworkFlow
      ↓
QoS + Policy + Routing
      ↓
Controlled Network Path
```

NovaOS erhält damit ein protokollunabhängiges Flow-Modell, über das Netzwerkverkehr einheitlich klassifiziert, gesteuert, priorisiert, abgerechnet und auf unterschiedliche Netzwerkpfade verteilt werden kann.