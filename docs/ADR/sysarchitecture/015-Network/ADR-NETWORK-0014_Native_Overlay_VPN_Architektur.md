# ADR-NETWORK-0014 – Native Overlay-/VPN-Architektur

## Status

Angenommen

## Kategorie

Network / VPN / Overlay Network / Security / Virtual Networks

## Kontext

NovaOS unterstützt mit `ADR-NETWORK-0013` virtuelle Netzwerke und Network Namespaces als native Netzwerkabstraktionen.

VPNs und Overlay-Netzwerke sollen darauf nicht als vollständig separate Netzwerkarchitektur aufgesetzt werden. Sie stellen vielmehr zusätzliche logische Netzwerkpfade bereit, die über bestehende physische oder virtuelle Netzwerke transportiert werden.

Traditionelle VPN-Lösungen werden häufig als nachträglich hinzugefügte virtuelle Interfaces behandelt. NovaOS soll Overlay- und VPN-Funktionalität dagegen direkt mit `NetworkFlow`, Identity, Capabilities, Routing, Trust und Data Sovereignty verbinden.

---

## Entscheidung

NovaOS führt eine **native Overlay-/VPN-Architektur** als Bestandteil seines allgemeinen Netzwerkmodells ein.

```text
NetworkFlow
     ↓
Policy / Routing
     ↓
Overlay / VPN Network
     ↓
Underlying NetworkFlow
     ↓
Physical / Virtual Network
```

Ein Overlay beziehungsweise VPN bildet ein logisches Netzwerk über einem oder mehreren darunterliegenden Netzwerkpfaden.

Es verwendet dieselben grundlegenden Identity-, Routing-, Security-, Resource- und Introspection-Prinzipien wie andere NovaOS-Netzwerke.

---

## Grundprinzip

```text
Overlay networks are networks over networks,
not a separate networking universe.
```

---

## Overlay Network

Ein Overlay Network besitzt eine eigene logische Netzwerktopologie und kann unabhängig von der darunterliegenden physischen Topologie aufgebaut werden.

```text
Overlay Node A
      ╲
       Overlay Network
      ╱
Overlay Node B

        ↓

Underlying Physical Networks
```

Die darunterliegenden Verbindungen dienen als Transportpfade für das Overlay.

Overlay Identity, Underlay Address und Underlay Route bleiben getrennte Konzepte.

---

## VPN als gesichertes Overlay

Ein VPN wird als gesicherte Form eines Overlay Networks behandelt.

```text
NetworkFlow
     ↓
VPN Policy
     ↓
Secure Overlay
     ↓
Encrypted / Authenticated Transport
     ↓
Underlay Network
```

Verschlüsselung und Authentisierung schützen die Overlay-Kommunikation, ersetzen jedoch nicht das allgemeine Capability-, Firewall- oder Trust-Modell von NovaOS.

---

## Identity und Trust

Overlay-Teilnehmer werden über logische Identities und geeignete Trust-Beziehungen identifiziert.

Eine Underlay-IP-Adresse darf nicht automatisch als Identität eines Overlay-Teilnehmers gelten.

```text
Overlay Identity
      ≠
Underlay Address
```

Nova.Trust kann zur Prüfung von Identität, Provenance und Vertrauensbeziehungen eingesetzt werden.

---

## NetworkFlow Integration

Anwendungen verwenden weiterhin `NetworkFlow`.

Ein Flow kann durch Routing Policy einem Overlay beziehungsweise VPN zugewiesen werden.

```text
NetworkFlow
     ↓
Policy
     ↓
Overlay Route
     ↓
Overlay Provider
     ↓
Underlay NetworkFlow
```

Damit benötigen Anwendungen keine grundsätzlich andere Kommunikationsabstraktion für VPN-Verbindungen.

---

## Policy-Based Routing

Overlay- und VPN-Pfade werden in Policy-Based Routing integriert.

Policies können beispielsweise festlegen:

```text
Corporate Services
→ Corporate VPN

Sensitive Data
→ Trusted Overlay Required

Public Traffic
→ Direct Network Allowed
```

Die Verwendung eines bestimmten Overlays kann ein Hard Constraint oder eine Soft Preference sein.

---

## Network Namespaces

Overlay Networks können mit Network Namespaces verbunden werden.

```text
Network Namespace
       ↓
Virtual Interface
       ↓
Overlay / VPN
       ↓
Remote Network
```

Dadurch können einzelne Execution Domains oder Gruppen gezielt einem Overlay zugeordnet werden, ohne das gesamte Systemnetzwerk umzuleiten.

---

## Multipath

Ein Overlay darf mehrere Underlay-Pfade verwenden, sofern der Overlay Provider dies unterstützt.

```text
Overlay Flow
   ├── Ethernet
   ├── WLAN
   └── Mobile
```

Damit können Resilienz, Mobility oder Bandbreitennutzung verbessert werden.

Alle verwendeten Underlay-Pfade müssen weiterhin die geltenden Hard Constraints erfüllen.

---

## Data Sovereignty

Overlay-Netzwerke können zur Durchsetzung von Data-Sovereignty-Anforderungen verwendet werden.

Beispielsweise kann ein Contract verlangen:

```text
Data
 ↓
Trusted Overlay Required
 ↓
Permitted Overlay Providers
 ↓
Permitted Underlay Paths
```

Ein verschlüsseltes VPN macht einen ansonsten unzulässigen Netzwerkpfad jedoch nicht automatisch zulässig.

Sovereignty Policies gelten sowohl für Overlay als auch Underlay.

---

## Resource Economy

Overlay- und VPN-Kommunikation verursacht zusätzliche Ressourcen.

Dazu gehören insbesondere:

- Verschlüsselungs- und Authentisierungsaufwand,
- Buffer,
- Bandbreite,
- Tunnel- und Sessionzustand,
- zusätzliche Header und Datenübertragung.

Diese Ressourcen müssen dem verursachenden Resource Context zugerechnet werden.

---

## Providerarchitektur

Konkrete Overlay- und VPN-Technologien werden als Provider integriert.

```text
Overlay Contract
      ↓
Provider
 ├── VPN Provider
 ├── Enterprise Overlay Provider
 └── Future Overlay Provider
```

Dadurch kann NovaOS neue Tunnel-, VPN- und Overlay-Technologien integrieren, ohne `NetworkFlow` oder das allgemeine Netzwerkmodell zu verändern.

Die konkreten Protokolle und kryptografischen Verfahren werden in NPSPECs definiert.

---

## Fail-Safe Defaults

Wenn ein `NetworkFlow` ein bestimmtes Overlay oder VPN zwingend voraussetzt, darf NovaOS bei dessen Ausfall nicht stillschweigend auf einen direkten Netzwerkpfad wechseln.

```text
VPN Required
     ↓
VPN unavailable
     ↓
Explicit Failure
```

Ein direkter Fallback ist nur zulässig, wenn der geltende Contract dies ausdrücklich erlaubt.

---

## Introspection

Overlay-Kommunikation muss kontrolliert introspektierbar sein.

NovaOS soll die logische Beziehung darstellen können:

```text
NetworkFlow
     ↓
Overlay / VPN
     ↓
Overlay Peer
     ↓
Underlay Flow
     ↓
Physical Interface
```

Sicherheitskritische Schlüssel oder andere vertrauliche Informationen dürfen dadurch nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Overlay- und VPN-Netzwerke als native Netzwerkfunktion unterstützen können.
2. Overlay Networks MÜSSEN auf der allgemeinen NovaOS-Netzwerkarchitektur aufbauen.
3. VPNs SOLLEN als gesicherte Overlay Networks modelliert werden.
4. Overlay Identity und Underlay Address MÜSSEN getrennte Konzepte bleiben.
5. Anwendungen SOLLEN Overlay-Kommunikation über `NetworkFlow` verwenden können.
6. Overlay- und VPN-Pfade MÜSSEN in Policy-Based Routing integrierbar sein.
7. Die Nutzung eines Overlays MUSS als Hard Constraint festgelegt werden können.
8. Overlay Networks MÜSSEN mit Network Namespaces kombinierbar sein.
9. Overlay Provider MÜSSEN grundsätzlich mehrere Underlay-Pfade unterstützen können, sofern ihre Technologie dies erlaubt.
10. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN für Overlay und Underlay gelten.
11. VPN-Verschlüsselung DARF Capability- oder Firewall-Autorisierung NICHT ersetzen.
12. Overlay- und VPN-Ressourcen MÜSSEN der Resource Economy zugerechnet werden.
13. Unterschiedliche Overlay- und VPN-Technologien MÜSSEN über Provider integrierbar sein.
14. Bei einem verpflichtenden VPN DARF NICHT stillschweigend auf einen ungeschützten direkten Pfad zurückgefallen werden.
15. Overlay-, VPN- und Underlay-Beziehungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- VPN und Overlay Networking werden Teil der nativen Netzwerkarchitektur,
- Anwendungen benötigen keine separate VPN-Kommunikationslogik,
- einzelne Execution Domains können gezielt Overlays verwenden,
- Policy-Based Routing und Data Sovereignty können VPN-Nutzung erzwingen,
- Multipath und Mobility lassen sich mit Overlays kombinieren,
- zukünftige Overlay-Technologien können als Provider ergänzt werden.

### Negative Konsequenzen

- Overlay und Underlay erzeugen zusätzliche Routing- und Diagnoseebenen,
- Verschlüsselung und Encapsulation verursachen Ressourcen- und Bandbreitenkosten,
- Lifecycle und Policy von Overlay- und Underlay-Verbindungen müssen koordiniert werden.

---

## Verworfene Alternativen

### VPN ausschließlich als externe Anwendung

Verworfen.

Dadurch wären Routing, Identity, Capabilities, Resource Economy und Data Sovereignty nur eingeschränkt systemweit integrierbar.

### VPN als globales virtuelles Interface für das gesamte System

Verworfen.

Unterschiedliche Execution Domains und `NetworkFlow`-Instanzen können unterschiedliche Overlay-Anforderungen besitzen.

### VPN als Ersatz für Firewall und Capability Security

Verworfen.

Ein gesicherter Transportkanal definiert nicht automatisch, welche Kommunikation autorisiert ist.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-NETWORK-0013_Virtual_Networks_und_Network_Namespaces`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-OVERLAY-0001`
- `NPSPEC-NETWORK-VPN-0001`
- `NPSPEC-NETWORK-OVERLAY-PROVIDER-0001`
- `NPSPEC-NETWORK-OVERLAY-IDENTITY-0001`
- `NPSPEC-NETWORK-OVERLAY-ROUTING-0001`
- `NPSPEC-NETWORK-OVERLAY-SECURITY-0001`
- `NPSPEC-NETWORK-OVERLAY-TEST-0001`

---

## Ergebnis

NovaOS integriert VPNs und Overlay-Netzwerke als native, policygesteuerte Netzwerkebene:

```text
Application
     ↓
NetworkFlow
     ↓
Routing / Security Policy
     ↓
Overlay / VPN
     ↓
Underlay NetworkFlow
     ↓
Physical / Virtual Network
```

Damit werden sichere und virtuelle Netzwerke nicht als nachträglicher Sondermechanismus behandelt, sondern direkt mit NetworkFlow, Identity, Routing, Capabilities, Data Sovereignty und Resource Economy verbunden.