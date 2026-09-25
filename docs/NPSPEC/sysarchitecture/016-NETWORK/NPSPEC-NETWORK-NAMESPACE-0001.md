# NPSPEC-NETWORK-NAMESPACE-0001 – Nova Network Namespace

## Status

Angenommen

## Kategorie

Network / Isolation / Namespace

## Zweck

NovaOS definiert Network Namespaces als isolierte logische Sichten auf Netzwerkressourcen und Netzwerkzustände.

```text
System Network
      ↓
Network Namespace
      ↓
Process / Service
```

Namespaces ermöglichen getrennte Interfaces, Adressen, Routen, Firewall-Regeln und Netzwerkzustände, ohne mehrere vollständige Netzwerk-Stacks vorauszusetzen.

## Grundprinzip

```text
Namespace ≠ Network Stack
Namespace ≠ Security Permission
Visibility ≠ Authority
Isolation ≠ Physical Separation
```

Ein Namespace definiert eine Sicht und Isolation von Netzwerkressourcen. Berechtigungen werden weiterhin durch Capabilities und Security Policy bestimmt.

## Namespace-Modell

Ein Network Namespace besitzt mindestens:

```text
NetworkNamespace
├── NamespaceID
├── Owner
├── Interfaces
├── Routing Context
├── Firewall Context
└── State
```

Optional:

```text
DNS Context
Security Context
Resource Budget
Parent Namespace
Network Policy
```

Die `NamespaceID` ist unabhängig von Namen, Prozessen und konkreten Interfaces.

## Isolierter Netzwerkzustand

Ein Namespace darf eigenen Netzwerkzustand besitzen.

```text
Namespace
├── Interfaces
├── Addresses
├── Routes
├── Flows
├── Firewall State
└── Resolver Configuration
```

Änderungen innerhalb eines Namespace dürfen andere Namespaces nicht automatisch beeinflussen.

## Prozesse und Services

Prozesse und Services können einem Network Namespace zugeordnet werden.

```text
Process
   ↓
Network Namespace
   ↓
Network API
```

Ein Prozess darf nur Netzwerkressourcen sehen oder verwenden, die in seinem Kontext sichtbar und autorisiert sind.

Ein Prozesswechsel zwischen Namespaces muss explizit autorisiert werden.

## Interfaces

Physische und virtuelle Interfaces dürfen Namespaces zugeordnet werden.

```text
Physical NIC
     ↓
Host Namespace
     ↓
Virtual Interface
     ↓
Isolated Namespace
```

Ein physisches Interface darf über kontrollierte virtuelle Mechanismen mehreren Netzwerkumgebungen dienen.

Die Zuordnung erzeugt keine zusätzlichen Hardwareberechtigungen.

## Routing

Jeder Namespace darf einen eigenen Routing-Kontext besitzen.

```text
Namespace A → Routing Table A
Namespace B → Routing Table B
```

Identische Zieladressen dürfen dadurch innerhalb unterschiedlicher Namespaces unterschiedlich geroutet werden.

Routing außerhalb des Namespace darf nur über explizit konfigurierte Übergänge erfolgen.

## Firewall

Firewall-Regeln müssen Namespace-spezifisch anwendbar sein.

```text
Namespace
    ↓
Firewall Policy
    ↓
Network Path
```

Globale Sicherheitsregeln dürfen zusätzliche Einschränkungen durchsetzen.

Ein Namespace darf globale Firewall- oder Sicherheitsregeln nicht abschwächen.

## DNS

Namespaces dürfen eigene Resolver-Konfigurationen besitzen.

```text
Namespace A → Resolver A
Namespace B → Resolver B
```

Dies ermöglicht beispielsweise getrennte DNS-Kontexte für:

```text
System
Applications
Containers
VPN
Development
Sandbox
```

DNS-Caches dürfen getrennt oder kontrolliert gemeinsam genutzt werden.

## Namespace-Verbindungen

Kommunikation zwischen Namespaces muss explizit hergestellt werden.

```text
Namespace A
     ↓
Virtual Link
     ↓
Namespace B
```

Mögliche Mechanismen sind:

```text
Virtual Interface
Bridge
Router
Controlled Gateway
```

Eine gemeinsame physische Netzwerkkarte erzeugt keine automatische Verbindung zwischen Namespaces.

## VPN und virtuelle Netzwerke

VPN- oder virtuelle Netzwerkpfade dürfen vollständig innerhalb eines Namespace existieren.

```text
Application
    ↓
Namespace
    ↓
VPN Interface
    ↓
Encrypted Network
```

Dadurch kann Netzwerkverkehr bestimmter Anwendungen isoliert über definierte Netzwerkpfade geführt werden.

## Lifecycle

Namespaces besitzen einen kontrollierten Lebenszyklus.

```text
Created
   ↓
Configured
   ↓
Active
   ↓
Stopping
   ↓
Destroyed
```

Beim Entfernen müssen zugehörige Flows, Routen und temporäre Netzwerkressourcen kontrolliert beendet oder übertragen werden.

## Ressourcensteuerung

NovaOS muss Ressourcen pro Namespace begrenzen können.

```text
Interfaces
Routes
Flows
Sockets
Packet Buffers
Firewall State
DNS Cache
Bandwidth
```

Ein Namespace darf nicht unkontrolliert globale Netzwerkressourcen verbrauchen.

## Sicherheit

Mindestens folgende Operationen müssen autorisiert werden:

```text
Create Namespace
Destroy Namespace
Attach Interface
Move Interface
Modify Routing
Modify Firewall
Create Namespace Link
Assign Process
```

Namespace-Isolation darf nicht als Ersatz für Capability- und Prozessisolation betrachtet werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
NamespaceID
Owner
State
Processes
Interfaces
Addresses
Routes
Flows
Firewall Context
DNS Context
Resource Usage
```

Ein Namespace darf ohne entsprechende Berechtigung keine sensiblen Netzwerkzustände anderer Namespaces einsehen.

## Normative Anforderungen

1. NovaOS MUSS isolierte Network Namespaces unterstützen können.
2. Jeder Network Namespace MUSS eine stabile `NamespaceID` besitzen.
3. Namespace-Sichtbarkeit und Netzwerkberechtigung MÜSSEN getrennt bleiben.
4. Namespaces MÜSSEN eigene Routing-Kontexte besitzen können.
5. Namespaces MÜSSEN eigene Firewall-Kontexte besitzen können.
6. Namespaces SOLLEN eigene DNS-Konfigurationen besitzen können.
7. Prozesse und Services MÜSSEN einem Network Namespace zugeordnet werden können.
8. Kommunikation zwischen Namespaces MUSS explizit konfiguriert werden.
9. Ein Namespace DARF globale Sicherheitsregeln NICHT abschwächen.
10. Namespace-Ressourcen MÜSSEN begrenzbar sein.
11. Das Entfernen eines Namespace MUSS dessen Netzwerkressourcen kontrolliert bereinigen.
12. Namespace-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0016`

## Ergebnis

```text
Physical / Virtual Network
          ↓
Network Namespaces
    ┌─────┼─────┐
    ↓     ↓     ↓
System   App   VPN
    ↓     ↓     ↓
Isolated Network Views
```

NovaOS erhält damit eine einheitliche Netzwerkisolation, über die Prozesse, Anwendungen, VPNs und Systemdienste getrennte Netzwerksichten nutzen können, ohne dafür voneinander unabhängige vollständige Netzwerk-Stacks zu benötigen.