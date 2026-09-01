# ADR-NETWORK-0013 – Virtual Networks und Network Namespaces

## Status

Angenommen

## Kategorie

Network / Virtualization / Network Namespaces / Isolation / Virtual Networks

## Kontext

NovaOS verwendet Execution Domains als grundlegende Isolations- und Ressourcendomänen. Netzwerkzugriff darf diese Isolation nicht durch einen einzigen globalen Netzwerkzustand unterlaufen.

Gleichzeitig benötigen Anwendungen, Container-artige Umgebungen, Kompatibilitätsdomänen, Entwicklungsumgebungen und Systemdienste teilweise eigene logische Netzwerkumgebungen.

NovaOS benötigt deshalb virtuelle Netzwerke und Network Namespaces, ohne dafür vollständige virtuelle Maschinen oder einen Hypervisor vorauszusetzen.

---

## Entscheidung

NovaOS unterstützt **Virtual Networks und Network Namespaces als native Netzwerkabstraktionen**.

Ein Network Namespace stellt einer Execution Domain beziehungsweise einer Gruppe autorisierter Domains eine kontrollierte Sicht auf Netzwerkressourcen bereit.

```text
Physical Network
       ↓
NovaOS Network Architecture
       ↓
Virtual Networks
   ┌──────┼──────┐
  VN A   VN B   VN C
   ↓      ↓      ↓
Namespace Namespace Namespace
```

Physische und virtuelle Netzwerkressourcen verwenden soweit möglich dieselben übergeordneten `NetworkFlow`-, Security-, Routing- und Resource-Contracts.

---

## Grundprinzip

```text
Network isolation without
requiring machine virtualization.
```

---

## Network Namespace

Ein Network Namespace definiert einen isolierten logischen Netzwerkraum.

Er kann insbesondere eigene Sichten auf folgende Ressourcen besitzen:

- Interfaces,
- Addresses,
- Routes,
- Firewall Policies,
- Service Discovery,
- DNS Resolution Contexts,
- Network Identities.

Ein Namespace darf nur Ressourcen sehen oder verwenden, für die ihm entsprechende Authority zugewiesen wurde.

---

## Virtual Network

Ein Virtual Network verbindet virtuelle oder autorisierte physische Netzwerkendpunkte.

```text
Execution Domain A
       ↓
Virtual Interface
       ↓
Virtual Network
       ↓
Virtual Interface
       ↓
Execution Domain B
```

Virtuelle Netzwerke können vollständig intern bleiben oder über kontrollierte Gateways mit anderen virtuellen beziehungsweise physischen Netzwerken verbunden werden.

---

## Verhältnis zu Execution Domains

Network Namespaces sind mit Execution Domains integrierbar, aber nicht identisch mit ihnen.

```text
Execution Domain
      ↓
Network Namespace
```

Mehrere Execution Domains dürfen kontrolliert denselben Namespace verwenden.

Ebenso darf eine Execution Domain bei entsprechender Authority mit mehreren Netzwerkcontexts interagieren.

Die Zuordnung muss explizit sein.

---

## NetworkFlow Integration

`NetworkFlow` bleibt auch innerhalb virtueller Netzwerke die native Kommunikationsabstraktion.

```text
NetworkFlow
     ↓
Network Namespace
     ↓
Routing / Policy
     ↓
Virtual or Physical Path
```

Anwendungen müssen daher kein separates Kommunikationsmodell verwenden, nur weil ein Netzwerkpfad virtualisiert ist.

---

## Isolation

Network Namespaces bilden eine explizite Sicherheits- und Sichtbarkeitsgrenze.

Ein Namespace darf nicht automatisch:

- Interfaces anderer Namespaces verwenden,
- deren Traffic beobachten,
- deren Routingzustand verändern,
- deren Services erreichen,
- deren Network Identities übernehmen.

Namespace-übergreifende Kommunikation benötigt eine explizit autorisierte Verbindung.

---

## Routing und Firewall

Jeder Namespace kann einen eigenen Routing- und Policy-Kontext besitzen.

```text
Namespace
   ├── Routes
   ├── Routing Policies
   ├── Firewall Policies
   └── Resolution Context
```

Systemweite Security-, Trust- und Data-Sovereignty-Regeln bleiben dabei übergeordnet gültig.

Ein Namespace darf keine übergeordneten Hard Constraints abschwächen.

---

## Virtuelle Interfaces

Virtuelle Interfaces werden als reguläre Netzwerkressourcen in das NovaOS Device- und Network-Modell integriert.

Sie können beispielsweise:

- zwei Namespaces verbinden,
- einen Namespace mit einem Virtual Network verbinden,
- als Gateway zu einem physischen Netzwerk dienen.

Die konkrete Interface- und Switching-Implementation wird in NPSPECs definiert.

---

## Resource Economy

Virtuelle Netzwerke sind keine kostenlosen logischen Ressourcen.

Netzwerkressourcen müssen weiterhin dem verursachenden Resource Context zugerechnet werden.

Dazu gehören insbesondere:

- Bandbreite,
- Buffer,
- Queues,
- virtuelle Interfaces,
- Routingzustand,
- Connection State,
- CPU-Aufwand.

Isolation darf Resource Budgets nicht umgehen.

---

## Data Sovereignty und Trust

Virtual Networks und Namespaces können zur technischen Durchsetzung von Trust- und Data-Sovereignty-Grenzen verwendet werden.

Sie ersetzen diese Policies jedoch nicht.

```text
Namespace Isolation
        +
Security / Trust Policy
        +
Data Sovereignty
        ↓
Permitted Communication
```

Eine virtuelle Verbindung ist daher nicht allein aufgrund ihrer Existenz autorisiert.

---

## Lifecycle

Network Namespaces und Virtual Networks besitzen einen kontrollierten Lifecycle.

Beim Entfernen eines Namespace müssen seine:

- virtuellen Interfaces,
- Routen,
- temporären Netzwerkzustände,
- zugehörigen Ressourcen

kontrolliert freigegeben beziehungsweise übertragen werden.

Persistente oder gemeinsam genutzte Netzwerkobjekte folgen ihren eigenen Ownership- und Lifetime-Regeln.

---

## Introspection

NovaOS muss virtuelle Netzwerkstrukturen kontrolliert introspektierbar machen.

```text
Execution Domain
      ↓
Network Namespace
      ↓
Virtual Network
      ↓
Interfaces
      ↓
Routes / NetworkFlows
```

Dadurch können Isolation, Routing, Ressourcenverbrauch und Kommunikationsbeziehungen nachvollzogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS Network Namespaces als native Netzwerkisolationsabstraktion unterstützen.
2. NovaOS MUSS virtuelle Netzwerke ohne zwingende vollständige Maschinenvirtualisierung bereitstellen können.
3. Network Namespaces MÜSSEN eigene Netzwerkressourcensichten besitzen können.
4. Network Namespace und Execution Domain MÜSSEN getrennte, kombinierbare Konzepte bleiben.
5. Die Zuordnung zwischen Execution Domains und Network Namespaces MUSS explizit kontrolliert werden.
6. `NetworkFlow` MUSS innerhalb virtueller und physischer Netzwerke dasselbe grundlegende Kommunikationsmodell verwenden können.
7. Namespace-übergreifende Kommunikation MUSS explizit autorisiert werden.
8. Network Namespaces MÜSSEN eigene Routing-, Firewall- und Resolution-Kontexte besitzen können.
9. Namespace-lokale Policies DÜRFEN übergeordnete Security-, Trust- oder Data-Sovereignty-Constraints NICHT abschwächen.
10. Virtuelle Interfaces MÜSSEN in das allgemeine NovaOS-Netzwerkmodell integrierbar sein.
11. Virtuelle Netzwerke MÜSSEN mit physischen Netzwerken über kontrollierte Verbindungen kombinierbar sein.
12. Ressourcen virtueller Netzwerke MÜSSEN der systemweiten Resource Economy zugerechnet werden.
13. Der Lifecycle eines Network Namespace MUSS kontrollierte Ressourcenfreigabe gewährleisten.
14. Namespaces, virtuelle Netzwerke und ihre relevanten Beziehungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Netzwerkisolation ohne vollständige virtuelle Maschinen,
- saubere Netzwerkumgebungen für Execution Domains und Kompatibilitätsdomänen,
- gemeinsame Architektur für virtuelle und physische Kommunikation,
- flexible virtuelle Topologien,
- Integration mit Firewall, Routing, Service Discovery und Resource Economy.

### Negative Konsequenzen

- zusätzliche Lifecycle- und Ressourcenverwaltung,
- Namespace-übergreifende Kommunikation benötigt explizite Vermittlung,
- virtuelle Netzwerktopologien erhöhen die Routing- und Diagnosekomplexität.

---

## Verworfene Alternativen

### Ein globaler Netzwerknamespace für das gesamte System

Verworfen.

Dies würde Isolation, Least Privilege und unterschiedliche Netzwerk-Policies unnötig einschränken.

### Netzwerkisolation ausschließlich über virtuelle Maschinen

Verworfen.

Viele Isolationsszenarien benötigen keine vollständige Hardwarevirtualisierung.

### Eigenes Netzwerkmodell für virtuelle Netzwerke

Verworfen.

Virtuelle und physische Netzwerkpfade sollen dieselben übergeordneten NovaOS-Contracts verwenden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-NETWORK-0011_DNS_mit_Security_Privacy_und_Policy`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-VIRTUAL-NETWORK-0001`
- `NPSPEC-NETWORK-VIRTUAL-INTERFACE-0001`
- `NPSPEC-NETWORK-NAMESPACE-ISOLATION-0001`
- `NPSPEC-NETWORK-NAMESPACE-ROUTING-0001`
- `NPSPEC-NETWORK-NAMESPACE-RESOURCE-0001`
- `NPSPEC-NETWORK-NAMESPACE-TEST-0001`

---

## Ergebnis

NovaOS integriert Netzwerkvirtualisierung direkt in seine native Netzwerkarchitektur:

```text
Execution Domains
       ↓
Network Namespaces
       ↓
Virtual Networks
       ↓
NetworkFlow
       ↓
Virtual / Physical Paths
```

Damit können isolierte und frei komponierbare Netzwerkumgebungen aufgebaut werden, ohne dafür ein separates Netzwerkmodell oder vollständige Maschinenvirtualisierung vorauszusetzen.