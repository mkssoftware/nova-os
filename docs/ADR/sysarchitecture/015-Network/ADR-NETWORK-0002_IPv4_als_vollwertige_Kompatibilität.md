# ADR-NETWORK-0002 – IPv4 als vollwertige Kompatibilität

## Status

Angenommen

## Kategorie

Network / IPv4 / Compatibility / Dual-Stack / Interoperability

## Kontext

`ADR-NETWORK-0001` legt IPv6 als First-Class-Protokoll und architektonischen Ausgangspunkt des NovaOS-Netzwerkstacks fest.

IPv4 bleibt gleichzeitig ein wesentlicher Bestandteil bestehender lokaler Netze, Unternehmensinfrastrukturen, eingebetteter Geräte, Internetdienste und Legacy-Systeme.

NovaOS darf IPv4 daher nicht lediglich über eine unvollständige Übergangsschicht unterstützen. Anwendungen und Dienste müssen zuverlässig mit IPv4-Systemen kommunizieren können, ohne dass IPv4 dadurch wieder die interne Netzwerkarchitektur bestimmt.

---

## Entscheidung

NovaOS unterstützt **IPv4 als vollwertigen Kompatibilitäts- und Interoperabilitätspfad** innerhalb seiner protokollunabhängigen Netzwerkarchitektur.

```text
Nova Network Architecture
          ↓
Protocol-independent Core
      ┌───────┴───────┐
    IPv6             IPv4
 First-Class     Full Compatibility
```

IPv4 erhält vollständige native Netzwerkfunktionalität, bleibt architektonisch jedoch ein IP Provider und nicht das Fundament höherer Netzwerkschichten.

---

## Grundprinzip

```text
IPv6 defines the future architecture.

IPv4 remains fully interoperable.
```

---

## Vollwertige IPv4-Unterstützung

NovaOS muss für IPv4 die für reguläre Netzwerkkommunikation erforderlichen Funktionen bereitstellen können.

Dazu gehören insbesondere:

- IPv4-Adressierung,
- Subnetze,
- Routing,
- Broadcast,
- Multicast,
- ICMPv4,
- automatische und statische Konfiguration.

Die konkreten Protokollmechanismen werden in den zugehörigen NPSPECs definiert.

---

## Gemeinsame Netzwerkarchitektur

IPv4 verwendet soweit sinnvoll dieselben übergeordneten NovaOS-Abstraktionen wie IPv6.

```text
Application
     ↓
Network Contract
     ↓
Transport
     ↓
IP Abstraction
  ┌─────┴─────┐
IPv6         IPv4
```

Security, Resource Accounting, QoS, Tracing und Capability Authority sollen dadurch nicht für jede IP-Version separat erfunden werden.

---

## IPv4-only-Netze

NovaOS muss in bestehenden IPv4-only-Netzen funktionsfähig bleiben.

```text
NovaOS
   ↓
IPv4
   ↓
Legacy / Existing Network
```

IPv6-Verfügbarkeit darf keine Voraussetzung für grundlegende IPv4-Kommunikation sein.

Dies gilt insbesondere für Legacy-Infrastrukturen und Geräte, die ausschließlich IPv4 unterstützen.

---

## Dual-Stack

Sind IPv4 und IPv6 verfügbar, können beide Protokolle parallel verwendet werden.

Die Auswahl eines konkreten Pfads erfolgt anhand der verfügbaren Netzwerkpfade, Contracts und System Policies.

```text
Network Request
      ↓
Eligible Paths
   ┌────┴────┐
 IPv6      IPv4
```

IPv4 darf nicht künstlich bevorzugt werden, nur weil es historisch verbreiteter ist.

---

## IPv4-spezifische Semantik

IPv4-spezifische Eigenschaften dürfen innerhalb des IPv4 Providers unterstützt werden, ohne sie zu allgemeinen Eigenschaften des gesamten NovaOS-Netzwerkmodells zu machen.

Dies betrifft beispielsweise:

- Broadcast,
- begrenzten Adressraum,
- IPv4-spezifische Konfiguration,
- NAT-bezogene Kompatibilität.

Höhere Systemschichten sollen keine IPv4-spezifischen Annahmen voraussetzen, sofern dies nicht ausdrücklich erforderlich ist.

---

## NAT und private Adressräume

NovaOS muss mit bestehenden IPv4-Netzen einschließlich privater Adressräume und NAT-Infrastrukturen interoperabel sein.

NAT wird jedoch nicht als grundlegende Eigenschaft des nativen NovaOS-Netzwerkmodells betrachtet.

```text
Native Network Model
        ↓
IPv4 Compatibility
        ↓
NAT / Existing Infrastructure
```

Anwendungen sollen daher nicht grundsätzlich voraussetzen, dass ihre sichtbare lokale IPv4-Adresse ihre globale Netzwerkidentität darstellt.

---

## IPv4 und Location Transparency

Auch IPv4 wird in die Location-Transparency-Architektur integriert.

Logische Services und Endpoints sollen nicht unnötig an konkrete IPv4-Adressen gebunden werden.

IPv4-Adresse und Netzwerkpfad bleiben Eigenschaften der tatsächlichen Kommunikation und können sich ändern, ohne die logische Service Identity verändern zu müssen.

---

## Security

IPv4-Kommunikation unterliegt denselben grundlegenden NovaOS-Sicherheitsprinzipien wie IPv6.

```text
Network Request
      ↓
Capability / Security Policy
      ↓
Allowed Network Paths
      ↓
IPv4 Provider
```

IPv4-Kompatibilität darf keine schwächeren Authority- oder Isolation-Regeln erzeugen.

---

## Normative Anforderungen

1. NovaOS MUSS IPv4 als vollwertigen Kompatibilitäts- und Interoperabilitätspfad unterstützen.
2. NovaOS MUSS grundsätzlich in IPv4-only-Netzen funktionsfähig sein.
3. IPv4 DARF NICHT zum architektonischen Fundament höherer NovaOS-Netzwerkschichten werden.
4. IPv4 und IPv6 SOLLEN gemeinsame protokollunabhängige Netzwerkabstraktionen verwenden.
5. IPv4-spezifische Eigenschaften SOLLEN innerhalb des IPv4 Providers gekapselt werden.
6. Höhere Netzwerkschichten DÜRFEN IPv4-spezifische Semantik NICHT unnötig voraussetzen.
7. NovaOS MUSS IPv4-Adressierung und Routing nativ unterstützen.
8. IPv4 MUSS parallel zu IPv6 betrieben werden können.
9. IPv6 DARF NICHT Voraussetzung für grundlegende IPv4-Kommunikation sein.
10. NovaOS MUSS mit privaten IPv4-Adressräumen und bestehenden NAT-Infrastrukturen interoperabel sein.
11. NAT DARF NICHT als grundlegende Eigenschaft des allgemeinen NovaOS-Netzwerkmodells behandelt werden.
12. IPv4 MUSS in das Capability-, Security- und Resource-Modell integriert sein.
13. IPv4 MUSS mit Location Transparency und logischen Service Identities kombinierbar sein.
14. Verwendete IP-Version und Netzwerkpfade MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- vollständige Interoperabilität mit bestehenden IPv4-Netzen,
- Unterstützung älterer Geräte und Infrastrukturen,
- IPv4-only-Betrieb bleibt möglich,
- gemeinsame Architektur für IPv4 und IPv6,
- keine dauerhafte Bindung des Netzwerkstacks an IPv4-Annahmen.

### Negative Konsequenzen

- Dual-Stack erhöht Implementierungs- und Testaufwand,
- IPv4-spezifische Legacy-Semantik muss weiterhin unterstützt werden,
- NAT und unterschiedliche Adressierungsmodelle erhöhen die Kompatibilitätskomplexität.

---

## Verworfene Alternativen

### IPv4 nur über eine externe Compatibility Layer

Verworfen.

IPv4 ist weiterhin zu verbreitet, um nur eingeschränkt oder indirekt unterstützt zu werden.

### IPv4 als primäre interne Netzwerkarchitektur

Verworfen.

Dies würde die zukünftige NovaOS-Netzwerkarchitektur dauerhaft an historische IPv4-Einschränkungen koppeln.

### IPv4 vollständig entfernen

Verworfen.

Dadurch würde NovaOS einen erheblichen Teil bestehender Netzwerkgeräte und Infrastrukturen nicht direkt unterstützen können.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0001_IPv6_als_First_Class_Protokoll`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV4-ADDRESSING-0001`
- `NPSPEC-NETWORK-IPV4-ROUTING-0001`
- `NPSPEC-NETWORK-IPV4-CONFIG-0001`
- `NPSPEC-NETWORK-DUALSTACK-0001`
- `NPSPEC-NETWORK-IPV4-TEST-0001`

---

## Ergebnis

NovaOS behandelt IPv4 als vollständig unterstützten Netzwerkpfad, ohne seine zukünftige Architektur darauf aufzubauen:

```text
       Nova Network Architecture
                 ↓
       Protocol-independent Core
            ┌────┴────┐
          IPv6       IPv4
      First-Class   Full
                   Compatibility
```

Damit bleibt NovaOS vollständig mit bestehenden IPv4-Infrastrukturen interoperabel, während IPv6 die langfristige architektonische Ausrichtung bestimmt.