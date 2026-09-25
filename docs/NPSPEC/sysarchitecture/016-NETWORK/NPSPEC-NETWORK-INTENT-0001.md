# NPSPEC-NETWORK-INTENT-0001 – Nova Network Intent

## Status

Angenommen

## Kategorie

Network / Intent / Policy

## Zweck

NovaOS definiert Network Intent als deklarative Beschreibung dessen, was eine Netzwerkoperation erreichen soll, ohne dass Anwendungen konkrete Interfaces, Routen oder Netzwerkpfade auswählen müssen.

```text
Application Intent
       ↓
Network Policy Engine
       ↓
Routing / QoS / Sovereignty / Security
       ↓
Network Path
```

Die Anwendung beschreibt Anforderungen. NovaOS bestimmt die passende technische Umsetzung.

## Grundprinzip

```text
Intent ≠ Route
Intent ≠ Interface
Intent ≠ Permission
Intent ≠ Implementation
```

Ein Intent beschreibt das gewünschte Ergebnis, nicht den konkreten Netzwerkmechanismus.

## Intent-Modell

Ein Network Intent besitzt mindestens:

```text
NetworkIntent
├── IntentID
├── Owner
├── Destination
├── Requirements
└── State
```

Optional:

```text
Security Requirements
Trust Requirements
Sovereignty Requirements
QoS Requirements
Latency
Bandwidth
Cost
Preferred Network
Required VPN
ExecutionContract
Resource Budget
```

Die `IntentID` bleibt unabhängig von Route, Interface oder Verbindung.

## Beispiel

Eine Anwendung darf beispielsweise anfordern:

```text
Destination: service.example
Security: Encrypted
Latency: Low
Sovereignty: EU
Network: Any Trusted
VPN: Preferred
```

NovaOS kann daraus einen geeigneten Pfad bestimmen:

```text
Intent
  ↓
Policy Evaluation
  ↓
Wi-Fi + VPN + IPv6
```

Die Anwendung muss dafür weder Interface noch Gateway oder konkrete Route kennen.

## Hard und Soft Requirements

Network Intent unterscheidet verbindliche Anforderungen und Präferenzen.

```text
Hard Requirements
├── Security
├── Sovereignty
└── Required Network Properties

Soft Requirements
├── Low Latency
├── High Bandwidth
├── Low Cost
└── Energy Efficiency
```

Hard Requirements dürfen nicht stillschweigend verletzt werden.

Soft Requirements dürfen bei Bedarf kontrolliert degradiert werden.

## Constraint-Priorität

Die NovaOS-Constraint-Priorität gilt auch für Network Intent:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine Performance-Optimierung darf keine höher priorisierte Anforderung verletzen.

## Intent Resolution

NovaOS löst einen Intent in einen ausführbaren Netzwerkplan auf.

```text
Intent
  ↓
Validate
  ↓
Resolve Destination
  ↓
Evaluate Policies
  ↓
Find Candidate Paths
  ↓
Select Path
  ↓
Establish Flow
```

Die Auflösung darf Routing, DNS, Multipath, VPN, QoS und Firewall berücksichtigen.

## Routing und Multipath

Anwendungen sollen normalerweise keinen konkreten Netzwerkpfad erzwingen müssen.

```text
Network Intent
      ↓
Candidate Paths
      ↓
Constraint Filtering
      ↓
Optimization
      ↓
Selected Path
```

Ein erzwungener Pfad darf nur verwendet werden, wenn er alle verbindlichen Anforderungen erfüllt.

## Network Migration

Ein Intent bleibt während eines Pfadwechsels bestehen.

```text
Intent
  ↓
Current Path Failed
  ↓
Re-Evaluation
  ↓
Alternative Valid Path
```

Dadurch kann NovaOS einen Flow migrieren, ohne dass die Anwendung ihre ursprünglichen Anforderungen neu formulieren muss.

## VPN

Ein Intent darf VPN-Nutzung definieren als:

```text
Required
Preferred
Forbidden
Unspecified
```

`Required` darf keinen automatischen Fallback auf einen ungeschützten Pfad erlauben.

## Network Namespace

Network Intents werden innerhalb ihres Network Namespace ausgewertet.

```text
Process
   ↓
Namespace
   ↓
Network Intent
   ↓
Available Network Resources
```

Ein Intent darf Namespace-Grenzen nicht automatisch überschreiten.

## Execution Contract

Network Intent ergänzt `Nova.ExecutionContract`.

```text
ExecutionContract
       ↓
Network Intent
       ↓
Network Execution Plan
```

Der Execution Contract beschreibt die übergeordneten Ausführungsanforderungen; Network Intent übersetzt relevante Anforderungen in das Netzwerkmodell.

## Dynamische Neubewertung

Ein Intent darf bei Änderungen neu ausgewertet werden.

Auslöser können sein:

```text
Interface Change
Route Change
Congestion
VPN Failure
Network Migration
Security Change
Policy Change
```

Eine Neubewertung darf einen besseren zulässigen Pfad auswählen, ohne die ursprünglichen Hard Requirements zu verändern.

## Sicherheit

Network Intent erzeugt keine zusätzlichen Berechtigungen.

```text
Requested Intent
      ↓
Capability / Policy Check
      ↓
Allowed Intent
```

Eine Anwendung darf über Intent keine Interfaces, VPNs, Namespaces oder Netzwerkpfade verwenden, für die sie keine Berechtigung besitzt.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Active Intents
Intent Evaluation
Candidate Paths
Policy State
Re-Evaluations
Pending Operations
```

Häufige Änderungen dürfen keine unbegrenzten Neuberechnungen erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
IntentID
Owner
Requirements
Hard Constraints
Soft Preferences
Selected Path
Rejected Paths
Effective QoS
Sovereignty State
Resolution State
Decision Reason
```

Damit muss nachvollziehbar sein, wie NovaOS einen Intent umgesetzt hat.

## Normative Anforderungen

1. NovaOS MUSS deklarative Network Intents unterstützen können.
2. Intent und konkrete Netzwerkrouten MÜSSEN getrennt bleiben.
3. Network Intent DARF keine Netzwerkberechtigung erzeugen.
4. Hard und Soft Requirements MÜSSEN unterscheidbar sein.
5. Hard Requirements DÜRFEN NICHT stillschweigend verletzt werden.
6. NovaOS MUSS Intents gegen verfügbare Netzwerkpfade auflösen können.
7. Routing, Multipath, QoS, VPN und Sovereignty SOLLEN bei der Auflösung berücksichtigt werden.
8. Network Migration SOLL den bestehenden Intent erhalten.
9. Intents MÜSSEN innerhalb ihres Network Namespace ausgewertet werden.
10. Netzwerkänderungen SOLLEN eine kontrollierte Neubewertung auslösen können.
11. Intent-Auflösung MUSS ressourcenbegrenzt sein.
12. Entscheidungen und abgelehnte Pfade SOLLEN introspektierbar sein.

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
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0025`

## Ergebnis

```text
Application
     ↓
"What do I need?"
     ↓
Network Intent
     ↓
Constraints + Policies
     ↓
Nova Network Planning
     ↓
"How do we achieve it?"
     ↓
Best Valid Network Path
```

NovaOS erhält damit eine deklarative Netzwerkschicht, bei der Anwendungen ihre Anforderungen beschreiben, während das Betriebssystem selbstständig den zulässigen technischen Netzwerkpfad auswählt und bei Änderungen neu bewertet.