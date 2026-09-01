# ADR-NETWORK-0012 – Service Discovery

## Status

Angenommen

## Kategorie

Network / Service Discovery / Identity / NetworkFlow / Location Transparency

## Kontext

NovaOS trennt gemäß `ADR-NETWORK-0006` Identity, Address und Route. Anwendungen sollen daher Dienste nicht dauerhaft über konkrete IP-Adressen, Ports oder Netzwerkpfade identifizieren müssen.

Zusätzlich ermöglicht `NetworkFlow` die Kommunikation anhand logischer Ziele und semantischer Anforderungen.

Dafür benötigt NovaOS eine native Service-Discovery-Architektur, die verfügbare Dienste anhand stabiler Service Identities und deklarierter Eigenschaften auffindbar macht, ohne Discovery mit Authority, Trust oder Routing gleichzusetzen.

---

## Entscheidung

NovaOS führt **Service Discovery als native, providerbasierte Systemfunktion** ein.

```text
Service Requirement
        ↓
Service Discovery
        ↓
Candidate Services
        ↓
Policy / Authority / Trust
        ↓
Eligible Service
        ↓
NetworkFlow
```

Discovery beschreibt, welche Services verfügbar beziehungsweise auffindbar sind.

Sie entscheidet nicht automatisch, ob ein Service verwendet werden darf oder über welchen Netzwerkpfad er erreicht wird.

---

## Grundprinzip

```text
Discovery finds services.

Authority permits their use.

Routing finds the path.
```

---

## Service Identity

Ein Service besitzt eine logische Service Identity, die unabhängig von seiner aktuellen Netzwerkadresse sein kann.

```text
Service Identity
    ├── Instance A
    ├── Instance B
    └── Instance C
```

Eine Service Identity darf mehrere konkrete Service Instances repräsentieren.

Eine Instance kann wiederum über mehrere Adressen oder Netzwerkpfade erreichbar sein.

---

## Service Description

Discovery kann neben der Service Identity deklarative Eigenschaften eines Dienstes bereitstellen.

Dazu können insbesondere gehören:

- Service Type,
- unterstützte Contracts,
- Semantic Types,
- Version,
- Provider-Eigenschaften,
- verfügbare Capabilities,
- Location-Informationen,
- Trust-relevante Informationen.

Die genaue Struktur wird in den zugehörigen NPSPECs definiert.

---

## Providerbasierte Discovery

NovaOS bindet unterschiedliche Discovery-Mechanismen über gemeinsame Discovery Provider ein.

```text
Service Discovery
   ├── Local Provider
   ├── Network Provider
   ├── Enterprise Provider
   └── Future Provider
```

Dadurch ist die native Service-Abstraktion nicht an einen einzelnen Discovery-Mechanismus oder ein bestimmtes Netzwerkprotokoll gebunden.

---

## Discovery und DNS

DNS kann Bestandteil der Service Discovery sein, ist aber nicht mit ihr gleichzusetzen.

```text
Service Discovery
        ↓
Candidate Service
        ↓
DNS / Other Resolution
        ↓
Addresses
```

DNS löst Namen und Service-Daten auf. Service Discovery bestimmt dagegen geeignete logische Dienste und deren Instanzen.

Beide Funktionen dürfen zusammenarbeiten, bleiben jedoch architektonisch getrennt.

---

## Discovery ist keine Authority

Das Auffinden eines Services erzeugt keine Berechtigung, diesen Service zu verwenden.

```text
Discovered
    ≠
Authorized
```

Nach Discovery müssen weiterhin die erforderlichen Capability-, Security- und Trust-Prüfungen erfolgen.

Die Kenntnis einer Service Identity oder Service Instance darf keine implizite Communication Authority erzeugen.

---

## Service Selection

Wenn mehrere geeignete Service Instances verfügbar sind, kann NovaOS eine zulässige Instance auswählen.

```text
Candidate Services
       ↓
Hard Constraints
       ↓
Eligible Services
       ↓
Soft Preferences
       ↓
Selected Service
```

Hard Constraints können beispielsweise Security, Trust, Data Sovereignty, Contract Compatibility oder erforderliche Capabilities umfassen.

Soft Policies dürfen anschließend unter anderem Locality, Latenz, Last oder Ressourcenverbrauch optimieren.

---

## Location Transparency

Service Discovery unterstützt die Location Transparency von NovaOS.

Eine Anwendung kann einen Dienst anhand seiner logischen Identität beziehungsweise seines Contracts anfordern, ohne dessen aktuellen physischen Standort kennen zu müssen.

```text
Service Requirement
       ↓
Discovery
       ↓
Local / Remote / Device Provider
```

Die tatsächliche Location sowie deren Kosten und Fehlersemantik dürfen dabei nicht verborgen werden.

---

## Dynamische Service Instances

Service Instances können erscheinen, verschwinden oder ihren Standort verändern.

Discovery muss daher mit dynamischen Systemzuständen umgehen können.

Die stabile Service Identity darf dabei unabhängig vom Lifecycle einzelner Instances bestehen bleiben.

Hot Replacement und Live Evolution können dadurch neue Service Instances veröffentlichen, ohne Anwendungen dauerhaft an die vorherige Instance zu koppeln.

---

## Data Sovereignty

Discovery und Service Selection unterliegen Data-Sovereignty-Policies.

Ein technisch verfügbarer Service darf ausgeschlossen werden, wenn seine:

- Location,
- Trust Domain,
- administrative Domain,
- Datenverarbeitungseigenschaften

die geltenden Hard Constraints verletzen.

Discovery darf solche Einschränkungen nicht umgehen.

---

## Introspection

NovaOS muss Service Discovery kontrolliert introspektierbar machen.

```text
Service Requirement
       ↓
Discovery Provider
       ↓
Candidates
       ↓
Policy Filtering
       ↓
Selected Instance
```

Dabei müssen sicherheits- oder datenschutzrelevante Informationen entsprechend ihrer Authority geschützt bleiben.

---

## Normative Anforderungen

1. NovaOS MUSS Service Discovery als native Systemfunktion bereitstellen.
2. Services MÜSSEN über logische Service Identities beschreibbar sein.
3. Service Identity DARF NICHT grundsätzlich an eine konkrete IP-Adresse oder Route gebunden sein.
4. Eine Service Identity MUSS mehrere Service Instances repräsentieren können.
5. Service Discovery MUSS unterschiedliche Discovery Provider unterstützen können.
6. Die native Discovery-Abstraktion DARF NICHT von einem einzelnen Discovery-Protokoll abhängig sein.
7. DNS und Service Discovery MÜSSEN getrennte, aber kombinierbare Funktionen bleiben.
8. Das Auffinden eines Services DARF keine implizite Communication Authority erzeugen.
9. Capability-, Security- und Trust-Prüfungen MÜSSEN unabhängig von Discovery erhalten bleiben.
10. Service Selection MUSS Hard Constraints vor Optimierungszielen berücksichtigen.
11. Data-Sovereignty-Policies MÜSSEN Discovery und Service Selection begrenzen können.
12. Service Discovery MUSS lokale und entfernte Services abbilden können.
13. Dynamisches Erscheinen, Entfernen und Ersetzen von Service Instances MUSS unterstützt werden können.
14. Discovery Provider, Kandidaten und relevante Auswahlentscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen müssen Dienste nicht dauerhaft an Adressen koppeln,
- lokale und entfernte Services können über ein gemeinsames Modell gefunden werden,
- mehrere Service Instances können transparent verwaltet werden,
- Location Transparency wird unterstützt,
- Security, Trust und Data Sovereignty bleiben von Discovery getrennte Kontrollmechanismen,
- zukünftige Discovery-Technologien können als Provider ergänzt werden.

### Negative Konsequenzen

- Discovery benötigt zusätzliche Identity- und Lifecycle-Verwaltung,
- mehrere Discovery Provider können widersprüchliche oder veraltete Informationen liefern,
- Service Selection benötigt klar definierte Policy- und Constraint-Regeln.

---

## Verworfene Alternativen

### Service Discovery ausschließlich über DNS

Verworfen.

DNS ist ein wichtiger Resolution-Mechanismus, bildet jedoch nicht das vollständige native Service-, Contract- und Identity-Modell von NovaOS ab.

### Direkte Verwendung von IP-Adresse und Port

Verworfen.

Dies koppelt Services unnötig an ihren aktuellen Netzwerkstandort.

### Discovery-Ergebnis automatisch autorisieren

Verworfen.

Auffindbarkeit und Communication Authority sind unterschiedliche Sicherheitskonzepte.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-NETWORK-0011_DNS_mit_Security_Privacy_und_Policy`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-SERVICE-DISCOVERY-0001`
- `NPSPEC-NETWORK-SERVICE-IDENTITY-0001`
- `NPSPEC-NETWORK-SERVICE-INSTANCE-0001`
- `NPSPEC-NETWORK-DISCOVERY-PROVIDER-0001`
- `NPSPEC-NETWORK-SERVICE-SELECTION-0001`
- `NPSPEC-NETWORK-SERVICE-DISCOVERY-SECURITY-0001`
- `NPSPEC-NETWORK-SERVICE-DISCOVERY-TEST-0001`

---

## Ergebnis

NovaOS behandelt Service Discovery als eigenständige Ebene zwischen logischer Service-Anforderung und konkreter Netzwerkkommunikation:

```text
Service Requirement
        ↓
Service Discovery
        ↓
Service Identity / Instances
        ↓
Authority + Policy
        ↓
Address / Route Resolution
        ↓
NetworkFlow
```

Damit können Anwendungen Dienste anhand ihrer Identität und Eigenschaften finden, während Addressing, Routing, Trust und Communication Authority weiterhin klar getrennte Verantwortlichkeiten bleiben.