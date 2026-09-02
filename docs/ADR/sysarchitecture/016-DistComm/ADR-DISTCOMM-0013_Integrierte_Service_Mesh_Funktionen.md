# ADR-DISTCOMM-0013 – Integrierte Service-Mesh-Funktionen

## Status

Angenommen

## Kategorie

Distributed Communication / Service Mesh / Nova.Call / Security / Observability / Traffic Policy

## Kontext

Klassische verteilte Systeme ergänzen ihre Service-Kommunikation häufig nachträglich durch ein separates Service Mesh.

Solche Service-Mesh-Systeme übernehmen typischerweise Funktionen wie:

- Service-to-Service Identity,
- Mutual Authentication,
- Traffic Routing,
- Retry,
- Circuit Breaking,
- Load Balancing,
- Telemetry,
- Policy Enforcement,
- Service Discovery,
- Encryption.

Diese Funktionen werden häufig durch zusätzliche Sidecar-Proxies oder separate Netzwerkebenen umgesetzt.

NovaOS besitzt jedoch bereits native Architekturbausteine für:

- `Nova.Call`,
- Remote Capabilities,
- Service Discovery,
- Typed Schemas,
- Operation-Aware Retry,
- Circuit Breaking,
- End-to-End Deadlines,
- Sovereignty Preflight,
- NetworkFlow,
- Policy-Based Routing,
- Identity- und Capability-aware Firewall,
- Provider Health,
- Multipath,
- QoS,
- Tracing und Causation.

Ein zusätzliches unabhängiges Service-Mesh-System würde diese Funktionen teilweise duplizieren und zusätzliche Datenpfade, Proxies, Konfigurationsebenen und Fehlerquellen erzeugen.

NovaOS benötigt deshalb **Service-Mesh-Funktionen als integrierte Eigenschaften seiner Distributed-Communication-Architektur**, ohne ein verpflichtendes Sidecar-Mesh zwischen Anwendungen und Netzwerk einzuführen.

---

## Entscheidung

NovaOS integriert zentrale Service-Mesh-Funktionen direkt in:

```text
Nova.Call
    +
Service Discovery
    +
NetworkFlow
    +
Identity / Trust / Capabilities
    +
Distributed Policy
    +
Observability
```

Konzeptionell:

```text
Application
    ↓
Nova.Call
    ↓
Distributed Communication Policy
    ↓
Service Resolution
    ↓
Security / Sovereignty / Health
    ↓
Traffic Selection
    ↓
NetworkFlow
    ↓
Target Service
```

Ein separater Proxy-Hop ist für native NovaOS-Kommunikation nicht erforderlich.

---

## Grundprinzip

```text
Service mesh semantics
without mandatory sidecar proxies.
```

und:

```text
Integrate policy into the communication architecture,
not into an additional transparent middlebox.
```

---

## Native Service Identity

Services besitzen eine stabile logische Service Identity.

```text
Service Identity
      ↓
Service Instances
 ├── Instance A
 ├── Instance B
 └── Instance C
```

Die Service Identity ist unabhängig von:

- IP-Adresse,
- Port,
- Prozess-ID,
- konkreter Maschine,
- aktueller Service Instance.

Damit bleibt die logische Kommunikation auch bei:

- Skalierung,
- Failover,
- Migration,
- Neustart

stabil.

---

## Identity und Authentication

Service-to-Service-Kommunikation muss eine verifizierbare Identität der beteiligten Services unterstützen.

```text
Caller Identity
      ↓
Authentication
      ↓
Target Identity
```

Network Location darf nicht als Service Identity verwendet werden.

```text
IP Address
    ≠
Service Identity
```

Authentication und Trust werden über das allgemeine Nova.Trust- und Identity-Modell integriert.

---

## Capability-basierte Authorization

Service Identity allein erzeugt keine Authority.

```text
Authenticated Service
        ≠
Authorized Service
```

Service-to-Service-Zugriffe verwenden weiterhin Capabilities und explizite Authority.

Konzeptionell:

```text
Identity
   ↓
Trust
   ↓
Capability
   ↓
Authorized Operation
```

Damit ersetzt NovaOS klassische netzwerkzentrierte Allow-Lists durch semantische Operation Authority.

---

## Mutual Authentication

Für Remote Service Communication muss beidseitige Authentisierung unterstützt werden können.

```text
Service A
   ⇄
Authenticated Channel
   ⇄
Service B
```

Die konkrete kryptografische Umsetzung wird in NPSPECs definiert.

Mutual Authentication ersetzt jedoch weder Capabilities noch Data-Sovereignty-Prüfungen.

---

## Service Discovery Integration

Service Mesh und Service Discovery sind keine getrennten Kontrollsysteme.

```text
Service Requirement
       ↓
Discovery
       ↓
Candidate Instances
       ↓
Policy Filtering
       ↓
Eligible Instances
```

Discovery liefert Kandidaten.

Die Distributed-Communication-Policy entscheidet, welche Kandidaten tatsächlich verwendet werden dürfen.

---

## Traffic Policy

NovaOS muss semantische Traffic Policies für Service-Kommunikation unterstützen.

Solche Policies können unter anderem berücksichtigen:

- Service Identity,
- Operation Type,
- Capability Context,
- Trust,
- Data Sovereignty,
- Deadline,
- QoS,
- Provider Health,
- Locality,
- Resource Budget.

Konzeptionell:

```text
Candidates
    ↓
Hard Constraints
    ↓
Eligible Candidates
    ↓
Soft Optimization
    ↓
Selected Instance / Path
```

---

## Load Balancing

Sind mehrere zulässige Service Instances verfügbar, kann NovaOS die Last verteilen.

```text
Service
 ├── Instance A
 ├── Instance B
 └── Instance C
```

Load-Balancing-Entscheidungen können berücksichtigen:

- Health,
- Load,
- Latency,
- Locality,
- Resource Cost,
- Deadline,
- Sovereignty.

Load Balancing ist Policy und darf Hard Constraints nicht umgehen.

---

## Health-Aware Selection

Service Instances können unterschiedliche Zustände besitzen.

```text
Instance A → Healthy
Instance B → Degraded
Instance C → Unhealthy
```

Health Information wird in die Candidate Selection integriert.

Ein technisch erreichbarer Service muss nicht automatisch ausgewählt werden.

---

## Retry und Circuit Breaking

Retry und Circuit Breaking sind integrierte Eigenschaften von `Nova.Call` und werden nicht durch einen zusätzlichen Proxy implementiert.

```text
Nova.Call
   ↓
Failure
   ↓
Operation-Aware Retry
   ↓
Circuit Evaluation
```

Dadurch bleiben:

- Operation Identity,
- Idempotency,
- Deduplication,
- Deadlines

für Retry-Entscheidungen verfügbar.

Ein generischer Proxy müsste diese Semantik ansonsten rekonstruieren oder ignorieren.

---

## Timeout und Deadline

Service-Mesh-Kommunikation verwendet die End-to-End-Deadline des Distributed Calls.

Ein Proxy-artiger lokaler Timeout darf die globale Deadline nicht zurücksetzen.

```text
Original Deadline
      ↓
Service A
      ↓
Service B
      ↓
Service C
```

Die verbleibende Deadline wird entlang der gesamten Causation Chain propagiert.

---

## Traffic Splitting

NovaOS kann Traffic kontrolliert auf mehrere kompatible Service-Versionen oder Instances verteilen.

Beispiele:

```text
Service v1 → 90 %
Service v2 → 10 %
```

oder:

```text
Stable Instance
Canary Instance
```

Traffic Splitting ist nur zulässig, wenn:

- Contracts kompatibel sind,
- Hard Constraints erfüllt werden,
- Capability- und Trust-Regeln eingehalten werden.

Die konkrete Policy- und Rollout-Semantik wird in NPSPECs definiert.

---

## Service-Versionen

Die logische Service Identity darf mehrere kompatible Implementierungs- oder Schema-Versionen besitzen.

```text
Service Identity
 ├── Implementation v1
 └── Implementation v2
```

Typed Schemas und Schema Negotiation bestimmen, ob eine konkrete Service Instance mit dem Call Contract kompatibel ist.

Softwareversionsnummern allein bestimmen die Kompatibilität nicht.

---

## Locality-Aware Selection

NovaOS darf lokale Service Instances bevorzugen, wenn diese alle Hard Constraints erfüllen.

Konzeptionell:

```text
Same Execution Domain
      ↓
Same System
      ↓
Same Local Network
      ↓
Remote Region
```

Locality ist ein Optimierungsziel, keine Authority-Regel.

---

## Zero-Serialization Fast Path

Befinden sich Caller und Service lokal und erfüllen beide die Voraussetzungen, kann der Zero-Serialization Local Fast Path verwendet werden.

```text
Service Mesh Policy
       ↓
Local Target Selected
       ↓
Zero-Serialization Fast Path
```

Eine klassische Proxy-Architektur würde diesen Optimierungspfad häufig verhindern oder erschweren.

---

## Encryption

Remote Service Communication muss entsprechend dem Security Contract geschützte Transportkanäle verwenden können.

Encryption wird in die normale Distributed-Communication- und Network-Provider-Architektur integriert.

```text
Nova.Call
   ↓
Security Contract
   ↓
Protected NetworkFlow
```

Verschlüsselung ersetzt weder Identity noch Authority.

---

## Data Sovereignty

Service-Mesh-Routing muss Data Sovereignty als Hard Constraint behandeln.

Beispielsweise darf Traffic Splitting nicht dazu führen, dass ein Teil der Requests an eine unzulässige Region gesendet wird.

```text
100 Candidate Instances
        ↓
Sovereignty Filter
        ↓
Permitted Candidates
        ↓
Load Balancing
```

Der Sovereignty Preflight erfolgt vor Remote Execution und relevanter Datenübertragung.

---

## Bounded Queues und Backpressure

Service-Mesh-Funktionen dürfen Überlastung nicht durch unbegrenzte Proxy- oder Retry-Queues verstecken.

Die integrierte Architektur verwendet:

- bounded Queues,
- Backpressure,
- Resource Budgets.

```text
Service overloaded
       ↓
Backpressure
       ↓
Upstream Services
```

Überlastung soll entlang der tatsächlichen Causation Chain sichtbar werden.

---

## QoS

Service Communication kann semantische QoS Requirements besitzen.

Beispiele:

```text
Interactive Query
Realtime Command
Background Synchronization
Telemetry Stream
```

QoS wird zusammen mit NetworkFlow und Resource Economy behandelt.

Service Mesh erzeugt keine separate konkurrierende Prioritätsarchitektur.

---

## Event und Stream Integration

Service-Mesh-Funktionen gelten nicht ausschließlich für Request/Response-RPC.

Sie müssen auch:

- Commands,
- Queries,
- Events,
- Streams

unterstützen.

Streams behalten insbesondere ihre:

- Backpressure,
- Lifecycle,
- QoS,
- Resource-Budget-

Semantik.

---

## Traffic Mirroring

NovaOS kann kontrolliertes Traffic Mirroring für autorisierte Diagnose-, Test- oder Analysezwecke unterstützen.

```text
Primary Call
     ↓
Primary Service

     └── Optional Mirror
             ↓
       Diagnostic Target
```

Mirroring ist ein zusätzlicher Information Flow und benötigt deshalb eigene:

- Authority,
- Data-Sovereignty-Prüfung,
- Trust-Prüfung,
- Resource-Budgetierung.

Es darf niemals allein durch eine Performance- oder Debug-Policy aktiviert werden, wenn Daten dadurch unzulässig offengelegt würden.

---

## Fault Injection

Kontrollierte Fault Injection kann für Tests und Resilience Engineering unterstützt werden.

Beispiele:

- künstliche Latenz,
- kontrollierter Fehler,
- simulierte Unverfügbarkeit.

Fault Injection darf nur innerhalb ausdrücklich autorisierter Test- oder Diagnosekontexte erfolgen.

Sie darf nicht unkontrolliert die semantischen Garantien produktiver Calls verändern.

---

## Policy Enforcement

Service-Mesh-Policies werden an den tatsächlichen Kommunikationsmechanismen durchgesetzt.

Konzeptionell:

```text
Call Contract
      ↓
Policy Resolution
      ↓
Enforcement
      ↓
Call / Flow Setup
```

Policies dürfen nicht lediglich beobachtende Metadaten sein.

Hard Constraints müssen vor Einrichtung des nicht zulässigen Kommunikationspfads greifen.

---

## Control Plane und Data Plane

NovaOS unterscheidet logisch zwischen:

```text
Control Plane
    =
Policy, Discovery, Health, Planning

Data Plane
    =
Actual Call and Data Transfer
```

Beide sind jedoch Teil derselben Architektur.

Der Data Plane darf nicht für jeden Call von der synchronen Verfügbarkeit eines zentralen Control-Plane-Dienstes abhängig sein.

Bereits validierte und ausreichend aktuelle lokale Policy- und Discovery-Zustände müssen einen robusten Betrieb ermöglichen.

---

## Kein verpflichtender Sidecar

Native NovaOS-Anwendungen benötigen keinen separaten Sidecar-Proxy pro Service.

```text
Traditional:

App → Sidecar → Network → Sidecar → App


NovaOS:

App → Nova.Call / NetworkFlow → App
```

Dies reduziert:

- zusätzliche Context Switches,
- Serialisierung,
- Kopien,
- Speicherverbrauch,
- zusätzliche Fehlerdomänen.

---

## Legacy Service Mesh

NovaOS darf klassische Proxy- oder Sidecar-basierte Service-Mesh-Technologien aus Kompatibilitätsgründen unterstützen.

Sie bleiben jedoch Kompatibilitätsmechanismen und bilden nicht die native NovaOS-Architektur.

```text
Native Nova Service
    → Integrated Mesh Functions

Legacy Application
    → Optional Proxy Compatibility
```

---

## Causation und Distributed Tracing

Tracing ist Bestandteil des Distributed Call Contexts.

Konzeptionell:

```text
User Operation
   ↓
Call A
   ↓
Call B
   ↓
Query C
   ↓
Stream D
```

Causation muss über Service-, Prozess-, Host- und Netzwerkgrenzen hinweg erhalten bleiben.

Ein separater Proxy darf dabei nicht zum alleinigen Besitzer des Trace Contexts werden.

---

## Metrics und Observability

Die integrierte Service-Mesh-Architektur muss relevante Kommunikationsmetriken bereitstellen können.

Beispiele:

- Call Rate,
- Error Rate,
- Latency,
- Retry Count,
- Circuit State,
- Queue Pressure,
- Instance Health,
- Traffic Distribution,
- Resource Usage.

Observability muss auf logischen Services und Operationen basieren können, nicht nur auf IP-Adressen und Ports.

---

## Architecture Introspection

NovaOS soll den resultierenden Kommunikationsplan darstellen können.

Beispielsweise:

```text
Nova.Call
 ├── Caller: Service A
 ├── Target: Service B
 ├── Operation: Query
 ├── Candidates: 4
 ├── Eligible: 2
 ├── Selected: Instance B2
 ├── Route: Path 3
 ├── Retry Policy: Safe
 ├── Circuit: Closed
 ├── Sovereignty: Allowed
 └── Data Path: Network
```

Damit werden Service-Mesh-Entscheidungen systemweit nachvollziehbar.

---

## Adaptive Steuerung

NovaOS darf Soft Policies adaptiv optimieren.

Beobachtungen können insbesondere betreffen:

- Service Latency,
- Failure Probability,
- Queue Pressure,
- Load,
- Locality,
- Prediction Error.

Adaptive Modelle können daraus beispielsweise Candidate Ordering oder Load Balancing verbessern.

Sie dürfen jedoch keine:

- Capability Requirements,
- Security Constraints,
- Sovereignty Policies,
- Hard Deadlines

abschwächen.

---

## Deterministic Mode

Im Deterministic Mode müssen relevante Service-Mesh-Entscheidungen reproduzierbar kontrollierbar sein.

Dazu können gehören:

- Instance Selection,
- Traffic Splitting,
- Retry Order,
- Failover Order,
- Provider Selection.

Adaptive Entscheidungen können dafür eingeschränkt oder deaktiviert werden.

---

## Resource Economy

Integrierte Service-Mesh-Funktionen unterliegen der systemweiten Resource Economy.

Dies betrifft unter anderem:

- Discovery State,
- Health Monitoring,
- Retry Capacity,
- Queues,
- Telemetry,
- Encryption,
- Traffic Mirroring.

Keine Mesh-Funktion erhält implizit unbegrenzte Ressourcen.

---

## Fail-Safe Verhalten

Kann eine für den Call erforderliche Hard Policy nicht zuverlässig ausgewertet werden, darf der Call nicht aufgrund einer optimistischen Annahme zugelassen werden.

```text
Policy Known Allowed → Continue
Policy Known Denied  → Reject
Policy Unknown       → Deny / Resolve
```

Soft-Policy-Informationen wie aktuelle Load-Werte dürfen dagegen entsprechend definierter Fallback-Regeln degradieren.

---

## Normative Anforderungen

1. NovaOS MUSS zentrale Service-Mesh-Funktionen nativ in die Distributed-Communication-Architektur integrieren.
2. Native Service-Mesh-Funktionen DÜRFEN keinen verpflichtenden Sidecar-Proxy benötigen.
3. Services MÜSSEN stabile logische Service Identities besitzen können.
4. Service Identity MUSS von IP-Adresse, Port und konkreter Service Instance getrennt sein.
5. Service-to-Service Authentication MUSS unterstützt werden können.
6. Authentication DARF NICHT mit Authorization gleichgesetzt werden.
7. Service-to-Service Authorization MUSS mit dem Capability-Modell integrierbar sein.
8. Service Discovery MUSS mit Policy Filtering und Instance Selection integriert werden.
9. Load Balancing MUSS Hard Constraints vor Soft Optimization berücksichtigen.
10. Provider- und Instance-Health MÜSSEN in die Service-Auswahl einfließen können.
11. Retry und Circuit Breaking MÜSSEN die semantischen `Nova.Call`-Contracts verwenden können.
12. End-to-End Deadlines DÜRFEN durch Service-Mesh-Komponenten NICHT zurückgesetzt werden.
13. Traffic Splitting MUSS mit Schema-, Trust-, Capability- und Sovereignty-Constraints kompatibel sein.
14. Service-Versionen MÜSSEN anhand ihrer Contracts und Schemas auf Kompatibilität geprüft werden können.
15. Locality DARF als Optimierungsziel verwendet werden.
16. Lokale Service-Kommunikation MUSS weiterhin Zero-Serialization- und Zero-Copy-Fast-Paths verwenden können.
17. Data Sovereignty MUSS vor Load Balancing, Traffic Splitting und Soft Routing Optimization geprüft werden.
18. Service-Mesh-Funktionen MÜSSEN Bounded Queues und Backpressure respektieren.
19. Service-Mesh-QoS MUSS mit der systemweiten QoS- und Resource-Economy-Architektur integriert werden.
20. Commands, Queries, Events und Streams MÜSSEN durch die integrierte Mesh-Architektur unterstützt werden können.
21. Traffic Mirroring MUSS als eigener autorisierter Information Flow behandelt werden.
22. Fault Injection MUSS auf ausdrücklich autorisierte Kontexte begrenzt sein.
23. Hard Policies MÜSSEN am tatsächlichen Kommunikationspfad durchsetzbar sein.
24. Der Data Plane DARF NICHT für jeden Call zwingend von einem zentral verfügbaren Control Plane abhängig sein.
25. Klassische Sidecar- oder Proxy-Meshes DÜRFEN als Kompatibilitätsmechanismus unterstützt werden.
26. Causation und Tracing MÜSSEN über Servicegrenzen erhalten bleiben können.
27. Service-Mesh-Entscheidungen und relevante Metriken MÜSSEN kontrolliert introspektierbar sein.
28. Adaptive Mesh Policies DÜRFEN Hard Constraints NICHT verändern.
29. Deterministic Mode MUSS dynamische Service-Mesh-Entscheidungen kontrollieren können.
30. Service-Mesh-Funktionen MÜSSEN der systemweiten Resource Economy unterliegen.
31. Unbekannte Hard-Policy-Zustände DÜRFEN NICHT automatisch als erlaubt behandelt werden.

---

## Konsequenzen

### Positive Konsequenzen

- kein zusätzlicher verpflichtender Proxy-Hop für native NovaOS-Services,
- geringere Serialisierungs-, Kopier- und Context-Switch-Kosten,
- Service Mesh kann Operation Semantics direkt verwenden,
- Capabilities ersetzen grobe ausschließlich netzwerkbasierte Authorization,
- Deadlines und Causation bleiben end-to-end erhalten,
- Zero-Serialization Local Fast Paths bleiben möglich,
- Data Sovereignty wird vor Traffic Selection berücksichtigt,
- weniger duplizierte Control- und Data-Plane-Architektur,
- einheitliche Introspection über Netzwerk und Distributed Calls.

### Negative Konsequenzen

- NovaOS selbst muss Funktionen bereitstellen, die klassische Plattformen häufig externen Mesh-Produkten überlassen,
- Policy-, Discovery- und Health-Systeme werden zentrale Bestandteile der Distributed Runtime,
- Interoperabilität mit klassischen Service-Mesh-Technologien benötigt Kompatibilitätsschichten,
- integrierte Policy Enforcement benötigt klare Grenzen zwischen Runtime, Network Stack und Trust-System.

---

## Verworfene Alternativen

### Verpflichtendes Sidecar-Service-Mesh

Verworfen.

Ein zusätzlicher Proxy-Hop würde NovaOS-eigene Typed Calls, Semantic Types, Capabilities, Zero-Copy und Zero-Serialization unnötig unterbrechen beziehungsweise duplizieren.

### Service Mesh ausschließlich im Netzwerkstack

Verworfen.

Der Netzwerkstack besitzt nicht ausreichend semantisches Wissen über Commands, Queries, Idempotency, Capabilities oder Operation Contracts.

### Service Mesh ausschließlich als Benutzerraum-Daemon

Verworfen.

Ein zentraler Daemon allein könnte nicht effizient alle notwendigen Call-, Security-, Backpressure- und Data-Path-Eigenschaften durchsetzen.

### Keine Service-Mesh-Funktionen

Verworfen.

Verteilte NovaOS-Systeme benötigen Service Discovery, Health-Aware Selection, Security, Traffic Policies, Retry, Circuit Breaking und Observability unabhängig davon, ob diese Funktionen als separates Produkt bezeichnet werden.

### Service Identity anhand von IP und Port

Verworfen.

Adressen sind Location Information und keine stabile logische Service Identity.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`
- `ADR-DISTCOMM-0005_Typed_Schemas_und_versionierte_Serialisierung`
- `ADR-DISTCOMM-0006_Zero_Serialization_Local_Fast_Path`
- `ADR-DISTCOMM-0007_Bounded_Queues_und_Backpressure`
- `ADR-DISTCOMM-0008_Idempotency_und_Deduplication_für_Exactly_Once_Effects`
- `ADR-DISTCOMM-0009_Operation_Aware_Retry_und_Circuit_Breaking`
- `ADR-DISTCOMM-0010_End_To_End_Deadline_Propagation`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`
- `ADR-DISTCOMM-0012_Command_Query_Event_Stream_als_Operationstypen`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`
- `ADR-NETWORK-0022_Health_Aware_Network_Provider_Selection`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-SERVICE-MESH-0001`
- `NPSPEC-DISTCOMM-SERVICE-IDENTITY-0001`
- `NPSPEC-DISTCOMM-SERVICE-AUTHENTICATION-0001`
- `NPSPEC-DISTCOMM-SERVICE-AUTHORIZATION-0001`
- `NPSPEC-DISTCOMM-SERVICE-TRAFFIC-POLICY-0001`
- `NPSPEC-DISTCOMM-SERVICE-LOAD-BALANCING-0001`
- `NPSPEC-DISTCOMM-SERVICE-TRAFFIC-SPLITTING-0001`
- `NPSPEC-DISTCOMM-SERVICE-MESH-OBSERVABILITY-0001`
- `NPSPEC-DISTCOMM-SERVICE-MESH-CONTROL-PLANE-0001`
- `NPSPEC-DISTCOMM-SERVICE-MESH-COMPATIBILITY-0001`
- `NPSPEC-DISTCOMM-SERVICE-MESH-TEST-0001`

---

## Ergebnis

NovaOS integriert Service-Mesh-Funktionen direkt in seine native Distributed-Communication-Architektur:

```text
                         Application
                             ↓
                          Nova.Call
                             ↓
                       Call Contract
                             ↓
                  Distributed Policy Layer
                             ↓
       ┌─────────────────────┼─────────────────────┐
       ↓                     ↓                     ↓
   Identity              Capabilities          Sovereignty
       ↓                     ↓                     ↓
       └─────────────────────┼─────────────────────┘
                             ↓
                     Service Discovery
                             ↓
                     Candidate Services
                             ↓
              Health / Deadline / Locality
                             ↓
                    Instance Selection
                             ↓
                       NetworkFlow
                             ↓
                      Target Service
```

Damit erhält NovaOS die wesentlichen Eigenschaften moderner Service-Mesh-Systeme, ohne native Anwendungen durch verpflichtende Sidecar-Proxies oder einen zusätzlichen unabhängigen Kommunikationsstack führen zu müssen.

Service Mesh wird dadurch nicht als nachträglich aufgesetzte Infrastruktur behandelt, sondern als natürliche systemweite Funktion von `Nova.Call`, NetworkFlow, Nova.Trust, Capabilities, Resource Economy und Architecture Introspection.