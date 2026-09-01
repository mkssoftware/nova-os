# ADR-NETWORK-0010 – NAT als Kompatibilitätsmechanismus

## Status

Angenommen

## Kategorie

Network / NAT / Compatibility / IPv4 / Address Translation

## Kontext

Network Address Translation ist insbesondere in IPv4-Netzen weit verbreitet und für die Interoperabilität mit bestehenden Heim-, Unternehmens-, Provider- und Legacy-Netzen weiterhin relevant.

NovaOS trennt jedoch mit `ADR-NETWORK-0006` Identity, Address und Route. Eine übersetzte Netzwerkadresse darf daher weder als stabile Identity noch als grundlegendes Element des nativen Netzwerkmodells betrachtet werden.

NAT soll bestehende Infrastruktur unterstützen, ohne die Architektur von `NetworkFlow`, Transport, Identity oder Security darauf aufzubauen.

---

## Entscheidung

NovaOS behandelt **NAT als expliziten Kompatibilitätsmechanismus an Netzwerkgrenzen** und nicht als fundamentales Prinzip der nativen Netzwerkarchitektur.

```text
NetworkFlow
     ↓
Routing / Policy
     ↓
NAT Compatibility
     ↓
External IPv4 Network
```

NAT darf eingesetzt werden, wenn eine Netzwerkumgebung oder ein Kompatibilitätsszenario Adressübersetzung erfordert.

Native NovaOS-Kommunikation darf NAT jedoch nicht grundsätzlich voraussetzen.

---

## Grundprinzip

```text
NAT translates addresses.

It does not define identity,
authority or trust.
```

---

## NAT und Identity

Adressübersetzung verändert Netzwerkadressen, nicht die logische Network Identity.

```text
Identity
   ↓
Internal Address
   ↓
NAT
   ↓
External Address
```

Die externe NAT-Adresse darf daher nicht automatisch als dauerhafte Identität des dahinterliegenden Systems oder Services interpretiert werden.

Mehrere Identities können dieselbe externe Adresse teilen.

---

## NAT und NetworkFlow

`NetworkFlow` bleibt die native logische Kommunikationsabstraktion.

NAT ist eine mögliche Transformation innerhalb des tatsächlich gewählten Netzwerkpfads.

```text
NetworkFlow
     ↓
Transport
     ↓
Route
     ↓
NAT
     ↓
Network
```

Die Existenz einer NAT-Grenze darf die logische Flow Identity nicht verändern.

---

## IPv4-Kompatibilität

NAT wird primär als Bestandteil der IPv4-Kompatibilitätsarchitektur betrachtet.

NovaOS muss mit bestehenden NAT-basierten IPv4-Infrastrukturen interoperabel sein.

Dazu können verschiedene Formen der Adress- und Portübersetzung gehören. Die konkreten Verfahren werden in den zugehörigen NPSPECs spezifiziert.

IPv6 soll dagegen nicht künstlich auf eine NAT-zentrierte Architektur reduziert werden.

---

## NAT ist keine Firewall

NAT darf nicht als Sicherheits- oder Autorisierungsmechanismus behandelt werden.

```text
NAT
≠ Firewall
≠ Capability
≠ Identity
≠ Trust
```

Firewall-, Capability- und Security-Entscheidungen bleiben eigenständige Funktionen.

Eine Verbindung ist nicht allein deshalb vertrauenswürdig, weil sie eine NAT-Grenze durchlaufen hat.

---

## Policy-Based Routing

NAT kann Bestandteil eines durch Policy-Based Routing ausgewählten Netzwerkpfads sein.

```text
NetworkFlow
     ↓
Eligible Routes
     ↓
Policy Selection
     ↓
Route
     ↓
Optional NAT
```

Security-, Trust- und Data-Sovereignty-Constraints werden vor der Nutzung des entsprechenden Pfads geprüft.

NAT darf diese Constraints nicht umgehen.

---

## Inbound-Kommunikation

Eingehende Kommunikation durch NAT-Grenzen benötigt eine explizite Zuordnung beziehungsweise Freigabe.

Eine solche Zuordnung erzeugt jedoch nicht automatisch eine NovaOS Communication Authority.

```text
External Network
      ↓
NAT Mapping
      ↓
Firewall / Authority
      ↓
Service
```

NAT-Mapping und Firewall-Autorisierung bleiben getrennte Entscheidungen.

---

## Multipath

Bei Multipath Networking können unterschiedliche Pfade unterschiedliche NAT-Eigenschaften besitzen.

```text
NetworkFlow
 ├── Path A → Direct
 ├── Path B → NAT
 └── Path C → Different NAT
```

Transportprovider müssen mit Änderungen der beobachtbaren Adressierung entsprechend ihrer eigenen Fähigkeiten umgehen.

NAT darf Multipath- oder Mobility-Garantien nicht vortäuschen, die der verwendete Transport nicht unterstützt.

---

## Resource Economy

NAT-bezogene Ressourcen müssen kontrollierbar und zurechenbar sein.

Dazu gehören insbesondere:

- Translation State,
- Mapping Capacity,
- Connection Tracking,
- Buffer- und Verarbeitungskosten.

NAT darf nicht als unbegrenzte, implizite Netzwerkressource behandelt werden.

---

## Introspection

NovaOS muss NAT kontrolliert introspektierbar machen.

Für einen Netzwerkpfad soll erkennbar sein können:

```text
NetworkFlow
     ↓
Internal Address
     ↓
NAT Translation
     ↓
External Address
```

Dabei dürfen sicherheitskritische Mapping-Informationen nur autorisierten Komponenten zugänglich sein.

---

## Normative Anforderungen

1. NovaOS MUSS mit bestehenden NAT-basierten Netzwerken interoperabel sein.
2. NAT MUSS als Kompatibilitätsmechanismus und DARF NICHT als fundamentales natives Netzwerkprinzip behandelt werden.
3. Native NovaOS-Netzwerkkommunikation DARF NAT NICHT grundsätzlich voraussetzen.
4. NAT DARF Network Identity und Network Address NICHT gleichsetzen.
5. Eine übersetzte Adresse DARF NICHT automatisch als vertrauenswürdige Identity gelten.
6. `NetworkFlow` MUSS unabhängig von NAT-Mappings modelliert bleiben.
7. NAT MUSS mit der IPv4-Kompatibilitätsarchitektur integrierbar sein.
8. NAT DARF NICHT als Ersatz für Firewall, Capability Enforcement oder Nova.Trust verwendet werden.
9. NAT-Mapping und Communication Authority MÜSSEN getrennte Konzepte bleiben.
10. NAT MUSS mit Policy-Based Routing kombinierbar sein.
11. NAT DARF Security-, Trust- oder Data-Sovereignty-Constraints NICHT umgehen.
12. Multipath-Pfade MÜSSEN unterschiedliche NAT-Eigenschaften besitzen können.
13. NAT-bezogene Ressourcen MÜSSEN der Resource Economy zurechenbar sein.
14. Aktive NAT-Nutzung und relevante Übersetzungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- vollständige Interoperabilität mit bestehenden NAT-Infrastrukturen,
- keine architektonische Abhängigkeit von NAT,
- Identity bleibt unabhängig von übersetzten Adressen,
- Firewall und Capability Security bleiben klar getrennt,
- IPv6 kann ohne historische IPv4-NAT-Annahmen gestaltet werden.

### Negative Konsequenzen

- NAT-State und Translation müssen weiterhin implementiert und verwaltet werden,
- eingehende Verbindungen und Multipath werden komplexer,
- Legacy-Netze können weiterhin NAT-spezifische Sonderbehandlung benötigen.

---

## Verworfene Alternativen

### NAT als grundlegendes NovaOS-Netzwerkmodell

Verworfen.

Dies würde die native Architektur unnötig an Einschränkungen des IPv4-Adressraums koppeln.

### NAT als Firewall verwenden

Verworfen.

Adressübersetzung stellt keine ausreichende Security- oder Authority-Grenze dar.

### NAT vollständig ablehnen

Verworfen.

Dies würde die Interoperabilität mit einem erheblichen Teil bestehender IPv4-Infrastruktur einschränken.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0001_IPv6_als_First_Class_Protokoll`
- `ADR-NETWORK-0002_IPv4_als_vollwertige_Kompatibilität`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-NAT-0001`
- `NPSPEC-NETWORK-NAT-IPV4-0001`
- `NPSPEC-NETWORK-NAT-MAPPING-0001`
- `NPSPEC-NETWORK-NAT-RESOURCE-0001`
- `NPSPEC-NETWORK-NAT-INTROSPECTION-0001`
- `NPSPEC-NETWORK-NAT-TEST-0001`

---

## Ergebnis

NovaOS unterstützt NAT dort, wo bestehende Netzwerke es erfordern, ohne seine native Netzwerkarchitektur darauf aufzubauen:

```text
Native NovaOS Network
        ↓
    NetworkFlow
        ↓
 Routing / Security
        ↓
   Optional NAT
        ↓
 Legacy / IPv4 Network
```

NAT bleibt damit ein kontrollierter Interoperabilitätsmechanismus, während Identity, Authority, Security und die logische Kommunikation unabhängig von Adressübersetzungen modelliert werden.