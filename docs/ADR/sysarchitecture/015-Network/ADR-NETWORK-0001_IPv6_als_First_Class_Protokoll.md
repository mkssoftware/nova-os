# ADR-NETWORK-0001 – IPv6 als First-Class-Protokoll

## Status

Angenommen

## Kategorie

Network / IPv6 / Protocol Architecture / Addressing / Compatibility

## Kontext

NovaOS benötigt einen Netzwerkstack, der nicht primär auf historischen IPv4-Annahmen basiert.

IPv6 bietet einen wesentlich größeren Adressraum und ein moderneres Fundament für globale Adressierung, lokale Kommunikation, automatische Konfiguration und zukünftige Netzwerktopologien.

IPv4 bleibt auf absehbare Zeit relevant und muss unterstützt werden. Die interne Netzwerkarchitektur von NovaOS soll jedoch nicht durch IPv4-spezifische Einschränkungen bestimmt werden.

---

## Entscheidung

NovaOS behandelt **IPv6 als First-Class-Netzwerkprotokoll**.

```text
Nova Network Stack
      ↓
IP Layer
 ┌────┴────┐
IPv6      IPv4
 ↑
First-Class
```

IPv6 ist ein nativer, gleichwertig integrierter Netzwerkpfad und darf nicht als Erweiterung eines IPv4-zentrierten Stacks implementiert werden.

Neue native NovaOS-Netzwerkfunktionen sollen IPv6 von Beginn an berücksichtigen.

IPv4 bleibt als vollständig unterstützter Kompatibilitäts- und Interoperabilitätspfad erhalten.

---

## Grundprinzip

```text
IPv6 is native architecture.

IPv4 is supported compatibility,
not the architectural baseline.
```

---

## Protokollunabhängige Netzwerkarchitektur

Höhere NovaOS-Netzwerkschichten sollen grundsätzlich nicht unnötig von einer bestimmten IP-Version abhängen.

```text
Application / Capability
        ↓
Network Contract
        ↓
Transport
        ↓
IP Provider
     ┌──┴──┐
   IPv6  IPv4
```

Gemeinsame Funktionen wie Routing, Security Policies, QoS, Resource Accounting und Tracing sollen über geeignete protokollunabhängige Abstraktionen integriert werden.

---

## IPv6-native Funktionen

Der NovaOS-Netzwerkstack muss zentrale IPv6-Funktionen nativ unterstützen können.

Dazu gehören insbesondere:

- IPv6-Adressierung,
- Neighbor Discovery,
- Router Discovery,
- automatische Adresskonfiguration,
- IPv6-Routing,
- Multicast,
- ICMPv6.

Die konkreten Protokollmechanismen und RFC-Anforderungen werden in den zugehörigen NPSPECs definiert.

---

## Dual-Stack

NovaOS unterstützt IPv6 und IPv4 parallel.

```text
Network Endpoint
      ↓
Available Paths
 ├── IPv6
 └── IPv4
```

Die Existenz von IPv4 darf jedoch nicht vorausgesetzt werden, damit ein nativer NovaOS-Netzwerkdienst funktioniert.

Ebenso muss NovaOS weiterhin in IPv4-basierten Netzen interoperabel bleiben.

---

## Address Family und Endpoints

Netzwerkendpunkte dürfen nicht auf IPv4-spezifische Adressstrukturen reduziert werden.

Die interne Netzwerkarchitektur muss unterschiedliche Address Families sauber darstellen können.

```text
Network Endpoint
 ├── Address Family
 ├── Address
 ├── Scope
 ├── Transport
 └── Port / Service
```

Damit bleibt die Architektur auch für zukünftige Netzwerkprotokolle erweiterbar.

---

## Address Scope

IPv6-Adressbereiche besitzen unterschiedliche Gültigkeits- und Erreichbarkeitsbereiche.

NovaOS muss relevante Scope-Informationen explizit behandeln können.

Eine Adresse allein darf nicht automatisch als global erreichbar interpretiert werden.

Scope-Informationen können außerdem in Routing-, Security- und Location-Entscheidungen einfließen.

---

## Provider- und Pfadauswahl

Sind mehrere Netzwerkpfade verfügbar, darf NovaOS einen geeigneten Pfad anhand von Contracts und System Policies auswählen.

```text
Network Request
      ↓
Hard Constraints
      ↓
Eligible Paths
      ↓
IPv6 / IPv4 / Provider
      ↓
Selected Path
```

Security, Data Sovereignty, Connectivity und andere Hard Constraints haben Vorrang vor Performance- oder Präferenzentscheidungen.

IPv6 darf bevorzugt werden, wenn mehrere technisch geeignete Pfade vorhanden sind, ohne IPv4 künstlich unbrauchbar zu machen.

---

## Location Transparency

IPv6 integriert sich in die Location-Transparency-Architektur von NovaOS.

Anwendungen sollen möglichst mit logischen Endpoints und Services arbeiten können, ohne unnötig an konkrete IP-Adressen gekoppelt zu sein.

Die tatsächliche Netzwerk-Location, Latenz, Erreichbarkeit und Fehlergrenze bleibt dennoch sichtbar.

---

## Security und Data Sovereignty

IPv6 erhält keine Sonderstellung gegenüber den allgemeinen NovaOS-Sicherheitsregeln.

Netzwerkkommunikation unterliegt weiterhin:

- Security Policies,
- Capability Authority,
- Nova.Trust,
- Data Sovereignty,
- Resource Budgets.

IPv6-Konnektivität allein erzeugt keine Authority zur Kommunikation.

---

## Normative Anforderungen

1. NovaOS MUSS IPv6 als natives First-Class-Protokoll unterstützen.
2. IPv6 DARF NICHT lediglich als Erweiterung eines IPv4-zentrierten Netzwerkstacks implementiert werden.
3. NovaOS MUSS weiterhin IPv4 unterstützen.
4. Native Netzwerkfunktionen SOLLEN IPv6 von Beginn an berücksichtigen.
5. Höhere Netzwerkschichten SOLLEN soweit sinnvoll unabhängig von der konkreten IP-Version bleiben.
6. Netzwerkendpunkte MÜSSEN unterschiedliche Address Families darstellen können.
7. IPv6-Adress-Scope MUSS explizit berücksichtigt werden können.
8. NovaOS MUSS IPv6-only-Betrieb grundsätzlich unterstützen können.
9. IPv4 DARF NICHT als zwingende Voraussetzung nativer NovaOS-Netzwerkdienste gelten.
10. IPv6 und IPv4 MÜSSEN parallel nutzbar sein können.
11. Provider- und Pfadauswahl MUSS Hard Constraints vor Optimierungszielen berücksichtigen.
12. Netzwerkkommunikation MUSS dem Capability-, Security- und Data-Sovereignty-Modell unterliegen.
13. Die Netzwerkarchitektur DARF zukünftige Address Families nicht unnötig ausschließen.
14. Verwendete Address Family, Provider und Netzwerkpfad MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- zukunftsfähige Netzwerkarchitektur,
- vollständiger IPv6-only-Betrieb wird möglich,
- keine langfristige Bindung an IPv4-spezifische Annahmen,
- IPv4-Kompatibilität bleibt erhalten,
- bessere Grundlage für moderne und zukünftige Netzwerktopologien,
- Address Families bleiben erweiterbar.

### Negative Konsequenzen

- IPv4 und IPv6 müssen parallel getestet und gepflegt werden,
- Dual-Stack erhöht vorübergehend die Komplexität,
- IPv6-spezifische Discovery-, Scope- und Konfigurationsmechanismen benötigen eigene Implementierungen.

---

## Verworfene Alternativen

### IPv4 als primäres Protokoll und IPv6 als Erweiterung

Verworfen.

Dies würde historische IPv4-Einschränkungen dauerhaft in die interne NovaOS-Netzwerkarchitektur übernehmen.

### Ausschließlich IPv6

Verworfen.

IPv4 bleibt für reale Interoperabilität und Legacy-Netze weiterhin erforderlich.

### IPv4 und IPv6 als vollständig getrennte Netzwerkarchitekturen

Verworfen.

Gemeinsame Transport-, Security-, Resource- und Observability-Funktionen sollen nicht unnötig dupliziert werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-IPV6-ADDRESSING-0001`
- `NPSPEC-NETWORK-IPV6-NDP-0001`
- `NPSPEC-NETWORK-IPV6-AUTOCONF-0001`
- `NPSPEC-NETWORK-DUALSTACK-0001`
- `NPSPEC-NETWORK-IPV6-TEST-0001`

---

## Ergebnis

NovaOS baut seine IP-Netzwerkarchitektur nicht auf IPv4 als historischem Fundament auf:

```text
        Nova Network Architecture
                  ↓
        Protocol-independent Core
             ┌────┴────┐
           IPv6       IPv4
       First-Class   Supported
```

IPv6 wird damit zum nativen Ausgangspunkt für die zukünftige Netzwerkarchitektur, während IPv4 als vollständig integrierter Interoperabilitätspfad erhalten bleibt.