# ADR-NETWORK-0004 – TCP, UDP und QUIC als Transportprovider

## Status

Angenommen

## Kategorie

Network / Transport / TCP / UDP / QUIC / Provider Architecture

## Kontext

`ADR-NETWORK-0003` definiert Netzwerktransporte in NovaOS als Provider hinter semantischen Flow Requirements.

Damit darf die Netzwerkarchitektur nicht fest mit einem einzelnen Transportprotokoll verbunden sein. Gleichzeitig benötigt NovaOS konkrete, interoperable Transportprovider für bestehende und moderne Netzwerke.

TCP und UDP bilden weiterhin wesentliche Grundlagen heutiger Netzwerkkommunikation. QUIC ergänzt diese um einen modernen, verbindungsorientierten und multiplexfähigen Transport über UDP.

NovaOS soll diese Protokolle als unterschiedliche Implementierungen innerhalb derselben Transportarchitektur behandeln.

---

## Entscheidung

NovaOS integriert **TCP, UDP und QUIC als native Transportprovider** hinter der semantischen Flow-Abstraktion.

```text
Semantic Flow Requirements
          ↓
Transport Resolution
          ↓
   ┌──────┼──────┐
  TCP    UDP    QUIC
```

Jeder Provider deklariert seine unterstützten Eigenschaften und kann ausgewählt werden, wenn er die Anforderungen des jeweiligen Flows erfüllt.

Weitere Transportprovider können später ergänzt werden.

---

## Grundprinzip

```text
Transport protocols are providers,
not application architecture.
```

---

## Gemeinsamer Transport-Contract

TCP, UDP und QUIC implementieren einen gemeinsamen NovaOS Transport Provider Contract.

Dieser beschreibt die systemweit relevanten Eigenschaften eines Providers, ohne die protokollspezifischen Unterschiede zu verbergen.

```text
Transport Provider
 ├── Supported Semantics
 ├── Capabilities
 ├── Constraints
 ├── Resource Characteristics
 └── Protocol-specific Extensions
```

Protokollspezifische Funktionen bleiben über geeignete Erweiterungen erreichbar.

---

## TCP Provider

Der TCP Provider stellt insbesondere einen zuverlässigen, geordneten Byte-Stream bereit.

Er eignet sich für Flows, deren Anforderungen mit dieser Semantik kompatibel sind.

TCP bleibt ein vollständig unterstützter Standardtransport und wird sowohl über IPv6 als auch IPv4 integriert.

---

## UDP Provider

Der UDP Provider stellt datagrammorientierte Kommunikation mit minimaler Transportsemantik bereit.

Er eignet sich insbesondere für Anwendungen, die:

- Datagrammgrenzen benötigen,
- Verlust selbst behandeln,
- geringe Transportkomplexität bevorzugen,
- eigene Transportsemantik implementieren.

NovaOS darf UDP nicht künstlich mit TCP-artiger Zuverlässigkeitssemantik versehen.

---

## QUIC Provider

QUIC wird als eigener Transportprovider behandelt und nicht lediglich als gewöhnliche anwendungsspezifische UDP-Nutzung.

Der Provider kann insbesondere moderne Transportfunktionen wie:

- sichere Verbindungsherstellung,
- multiplexfähige Streams,
- verbindungsbezogene Transportsemantik,
- Connection Migration

bereitstellen.

QUIC verwendet UDP als darunterliegenden Netzwerktransport, besitzt innerhalb der NovaOS-Transportarchitektur jedoch eine eigene semantische Provider-Identität.

---

## Provider Selection

Die Auswahl erfolgt anhand der Flow Requirements.

```text
Flow Requirements
      ↓
Hard Constraints
      ↓
Eligible Providers
      ↓
Soft Preferences
      ↓
TCP / UDP / QUIC
```

Ein Provider darf nur ausgewählt werden, wenn seine deklarierte Semantik die verbindlichen Anforderungen des Flows erfüllt.

NovaOS darf anschließend zwischen mehreren geeigneten Providern optimieren.

---

## Explizite Auswahl

Anwendungen dürfen einen Provider bevorzugen oder explizit anfordern.

```text
Automatic
→ Select suitable provider

Preferred QUIC
→ Prefer QUIC when suitable

Required TCP
→ TCP required
```

Eine explizite Auswahl darf keine übergeordneten Security-, Data-Sovereignty- oder anderen Hard Constraints verletzen.

---

## IPv6 und IPv4

Transportprovider werden grundsätzlich unabhängig von einer einzelnen IP-Version modelliert.

```text
TCP  ─┬→ IPv6
      └→ IPv4

UDP  ─┬→ IPv6
      └→ IPv4

QUIC ─┬→ IPv6
      └→ IPv4
```

Damit bleibt die Transportarchitektur mit dem IPv6-First-Class- und IPv4-Kompatibilitätsmodell konsistent.

---

## Resource Economy und QoS

Transportprovider integrieren sich in die systemweite Resource Economy.

Relevante Ressourcen wie:

- Buffer,
- Bandbreite,
- Connections,
- Queues,
- CPU-Zeit

müssen dem verursachenden Resource Context zurechenbar sein.

QoS-, Deadline- und Priority-Anforderungen können bei Provider- und Pfadauswahl berücksichtigt werden, sofern der jeweilige Transport entsprechende Eigenschaften unterstützen kann.

---

## Protokollspezifische APIs

Die semantische Flow-Abstraktion ersetzt nicht den direkten Zugriff auf Transportprotokolle.

Low-Level- und Kompatibilitätsanwendungen dürfen explizite TCP-, UDP- oder QUIC-Schnittstellen verwenden.

```text
Semantic Flow API
        ↓ preferred

Explicit Transport API
        ↓ available
```

Beide Pfade sollen auf denselben nativen Transportprovidern aufbauen.

---

## Normative Anforderungen

1. NovaOS MUSS TCP, UDP und QUIC als native Transportprovider unterstützen.
2. Die Provider MÜSSEN in die gemeinsame Transportabstraktion integriert werden.
3. Jeder Transportprovider MUSS seine unterstützten semantischen Eigenschaften deklarieren können.
4. Provider-Auswahl MUSS auf den Flow Requirements basieren können.
5. Hard Constraints MÜSSEN vor Transportoptimierungen ausgewertet werden.
6. TCP MUSS als zuverlässiger geordneter Byte-Stream verfügbar sein.
7. UDP MUSS seine datagrammorientierte Semantik erhalten.
8. QUIC MUSS als eigenständiger semantischer Transportprovider modelliert werden.
9. Die Nutzung von UDP durch QUIC DARF QUIC NICHT auf die Semantik eines normalen UDP-Flows reduzieren.
10. TCP, UDP und QUIC MÜSSEN grundsätzlich mit IPv6 und IPv4 verwendbar sein.
11. Anwendungen MÜSSEN einen konkreten Transportprovider explizit auswählen können.
12. Explizite Provider-Auswahl DARF übergeordnete Hard Constraints NICHT verletzen.
13. Transportprovider MÜSSEN in Resource Economy, Security und Tracing integrierbar sein.
14. Weitere Transportprovider MÜSSEN ohne grundlegende Änderung der Transportabstraktion ergänzt werden können.
15. Ausgewählter Provider und relevante Provider-Eigenschaften MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- TCP, UDP und QUIC besitzen eine gemeinsame architektonische Integration,
- Anwendungen können semantische Anforderungen statt konkrete Protokolle formulieren,
- moderne und klassische Netzwerkprotokolle bleiben vollständig nutzbar,
- zukünftige Transportprotokolle können ergänzt werden,
- explizite Low-Level-Kontrolle bleibt möglich.

### Negative Konsequenzen

- Provider müssen ihre Fähigkeiten präzise deklarieren,
- automatische Provider-Auswahl benötigt geeignete Policies,
- unterschiedliche Transportsemantiken dürfen trotz gemeinsamer Abstraktion nicht verwischt werden.

---

## Verworfene Alternativen

### TCP als universeller Standardtransport

Verworfen.

Nicht alle Kommunikationsmuster benötigen oder profitieren von TCP-Semantik.

### UDP nur als interne Basis für höhere Protokolle

Verworfen.

Datagrammorientierte Kommunikation ist selbst eine relevante native Transportsemantik.

### QUIC ausschließlich als Anwendungsbibliothek

Verworfen.

QUIC soll als systemweit nutzbarer Transportprovider in Flow Selection, Resource Economy und Introspection integrierbar sein.

### Transportprotokolle hinter vollständig identischer Semantik verstecken

Verworfen.

Die gemeinsame Abstraktion darf relevante Unterschiede zwischen TCP, UDP und QUIC nicht verschleiern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0001_IPv6_als_First_Class_Protokoll`
- `ADR-NETWORK-0002_IPv4_als_vollwertige_Kompatibilität`
- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-TRANSPORT-PROVIDER-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-NETWORK-TRANSPORT-SELECTION-0001`
- `NPSPEC-NETWORK-TRANSPORT-TEST-0001`

---

## Ergebnis

TCP, UDP und QUIC werden in NovaOS nicht als voneinander isolierte Netzwerkarchitekturen behandelt, sondern als Provider einer gemeinsamen semantischen Transportabstraktion:

```text
       Semantic Flow
             ↓
    Transport Requirements
             ↓
      Provider Selection
       /      |      \
     TCP     UDP     QUIC
```

Damit kann NovaOS für einen Flow einen geeigneten Transport auswählen, während Anwendungen bei Bedarf weiterhin einen konkreten Provider explizit festlegen können.