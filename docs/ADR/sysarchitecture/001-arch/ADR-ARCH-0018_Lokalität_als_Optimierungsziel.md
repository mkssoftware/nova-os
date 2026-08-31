# ADR-ARCH-0018 – Lokalität als Optimierungsziel

## Status

Angenommen

## Kategorie

Systemarchitektur / Performance / Datenlokalität / Scheduling / Resource Economy / Distributed Execution

## Kontext

NovaOS trennt die semantische Bedeutung einer Operation von ihrer konkreten Ausführung.

Eine Capability beschreibt primär:

```text
What should be done?
```

Provider, Algorithmus und Ausführungsort bestimmen dagegen:

```text
How?
Where?
On which hardware?
```

Dies ermöglicht die Ausführung derselben Operation beispielsweise auf:

```text
CPU
GPU
NPU
anderem NUMA-Node
anderem Prozess
anderem Gerät
Edge-Node
Remote-Node
Cloud-Provider
```

Diese Flexibilität darf jedoch nicht dazu führen, dass Daten unnötig zwischen Speicherbereichen, Geräten oder Rechnern bewegt werden.

Datenbewegung verursacht unter anderem:

- Latenz,
- Speicherbandbreite,
- Cache Misses,
- NUMA-Zugriffe,
- PCIe-Transfers,
- Energieverbrauch,
- thermische Belastung,
- Netzwerktraffic,
- Serialisierung,
- Synchronisation,
- Verschlüsselungsaufwand,
- zusätzliche Fehlerquellen,
- gegebenenfalls monetäre Kosten.

Eine lokal betrachtet schnellere Ausführung kann deshalb global schlechter sein.

Beispiel:

```text
CPU Buffer
    ↓ copy
GPU VRAM
    ↓ compute
CPU Buffer
```

kann bei kleinen Datenmengen teurer sein als:

```text
CPU Buffer
    ↓
CPU SIMD
```

obwohl der eigentliche GPU-Algorithmus schneller arbeitet.

NovaOS benötigt deshalb Lokalität als explizites Optimierungsziel.

---

## Entscheidung

NovaOS behandelt **Lokalität als systemweites Optimierungsziel**.

Execution Planner, Scheduler, Resource Economy und Pipeline Planner SOLLEN Daten- und Ausführungslokalität berücksichtigen und nach Möglichkeit:

```text
Compute near Data
```

sowie:

```text
Reuse already-local Resources
```

bevorzugen.

Lokalität ist dabei grundsätzlich ein Optimierungsziel und kein universeller Hard Constraint.

Höher priorisierte Anforderungen wie:

- Safety,
- Security,
- Data Sovereignty,
- Trust,
- Hard Realtime,
- Determinism,
- explizite harte Execution Contracts

haben Vorrang.

Das Planungsmodell lautet vereinfacht:

```text
Operation
    ↓
Required Data
    ↓
Current Data Location
    ↓
Candidate Providers
    ↓
Execution Cost
+
Transfer Cost
+
Locality
+
Execution Contract
    ↓
Execution Plan
```

Das zentrale Prinzip lautet:

```text
Move computation to data
before moving data to computation,
when practical.
```

Zusätzlich gilt:

```text
Data movement is a cost,
not an invisible implementation detail.
```

---

## Lokalitätsebenen

NovaOS kann mehrere Ebenen von Lokalität unterscheiden:

```text
Cache Locality
Core Locality
NUMA Locality
Process Locality
Memory-Domain Locality
Device Locality
Accelerator Locality
Machine Locality
Network Locality
Geographic Locality
```

Diese Ebenen besitzen unterschiedliche Kosten.

Ein Zugriff auf einen anderen CPU-Cache ist beispielsweise nicht mit einem Netzwerktransfer zu vergleichen.

Das gemeinsame Architekturmodell muss diese Unterschiede ausdrücken können.

---

## Cache- und Core-Lokalität

Scheduler und Runtime dürfen berücksichtigen, auf welchem Core ein Task zuvor ausgeführt wurde und welche Daten wahrscheinlich noch in lokalen Caches vorhanden sind.

Wo sinnvoll, kann NovaOS deshalb:

```text
Task
  ↓
previous CPU/Core
```

bevorzugen.

Diese Affinität darf jedoch nicht:

- Fairness verletzen,
- Realtime-Anforderungen gefährden,
- Überlastung erzeugen,
- höhere Execution Contracts verletzen.

---

## NUMA-Lokalität

Auf NUMA-Systemen sollen Tasks bevorzugt auf Nodes ausgeführt werden, auf denen ihre dominanten Daten liegen.

Bevorzugt:

```text
Task
  │
  ▼
NUMA Node 0
  │
  ▼
Local Memory
```

gegenüber:

```text
Task
  │
  ▼
NUMA Node 1
  │
  ▼
Remote NUMA Access
  │
  ▼
NUMA Node 0 Memory
```

sofern andere Anforderungen dies zulassen.

NovaOS darf dazu sowohl:

```text
Move Compute to Memory
```

als auch:

```text
Move Memory to Compute
```

verwenden.

Die günstigere Gesamtstrategie entscheidet.

---

## Memory-Domain-Lokalität

NovaOS unterscheidet unterschiedliche Speicherbereiche, beispielsweise:

```text
CPU RAM
NUMA Memory
Shared Memory
Pinned Memory
GPU VRAM
NPU Memory
Device Memory
Persistent Memory
Remote Memory
```

Die physische Repräsentation und Location eines Datenobjekts müssen deshalb bei der Planung berücksichtigt werden können.

---

## Accelerator-Lokalität

Wenn Daten bereits auf einem Accelerator vorhanden sind, soll NovaOS nach Möglichkeit weitere kompatible Verarbeitung dort durchführen.

Beispiel:

```text
GPU Decode
    ↓
GPU Resize
    ↓
GPU Filter
    ↓
GPU Encode
```

kann besser sein als:

```text
GPU Decode
    ↓
CPU Resize
    ↓
GPU Filter
    ↓
CPU Encode
```

wenn die zweite Variante mehrere große Transfers erzeugt.

Die Auswahl darf deshalb nicht ausschließlich anhand der isolierten Geschwindigkeit einzelner Pipeline-Stages erfolgen.

---

## Pipeline-Lokalität

Diese ADR erweitert das einheitliche Daten- und Objekt-Pipelining aus `ADR-ARCH-0009`.

Bei einer Pipeline:

```text
A → B → C → D
```

soll NovaOS nicht nur für jeden Node einzeln den vermeintlich schnellsten Provider auswählen.

Stattdessen soll auch die Gesamtkette betrachtet werden.

Beispiel:

```text
A = CPU
B = GPU
C = CPU
D = GPU
```

kann trotz schneller Einzelprovider schlechter sein als:

```text
A = GPU
B = GPU
C = GPU
D = GPU
```

wenn dadurch große Datenbewegungen vermieden werden.

---

## Data Gravity

Große Datenmengen erzeugen eine stärkere Bindung an ihren aktuellen Speicherort als kleine Datenmengen.

Konzeptionell:

```text
Data Gravity
    ∝
Data Size
×
Transfer Cost
×
Reuse Probability
```

Dies ist kein zwingend linearer mathematischer Zusammenhang, sondern ein Planungsprinzip.

Beispiel:

```text
2 KiB Metadata
```

kann problemlos zu einem Provider bewegt werden.

Ein:

```text
40 GiB Dataset
```

soll dagegen einen wesentlich stärkeren Einfluss auf die Provider- und Location-Auswahl besitzen.

---

## Compute-to-Data

Bei großen oder häufig wiederverwendeten Datenmengen soll bevorzugt geprüft werden, ob Compute zum Datenort gebracht werden kann.

```text
Data
 │
 ├── Provider A
 ├── Provider B
 └── Provider C
```

statt:

```text
Data
 │
 ▼
Large Transfer
 │
 ▼
Remote Compute
```

Dies gilt insbesondere für:

- große Dateien,
- Datenbanken,
- Medien,
- wissenschaftliche Datensätze,
- Tensoren,
- KI-Modelle,
- GPU-Ressourcen,
- verteilte Datenbestände.

---

## Data-to-Compute

Compute-to-Data ist keine absolute Regel.

Bei kleinen Datenmengen kann:

```text
Move Data to Faster Compute
```

die bessere Lösung sein.

NovaOS entscheidet deshalb anhand der Gesamtkosten.

---

## Transferkosten

Datenbewegung soll als explizite Kostenklasse modellierbar sein.

Konzeptionell:

```text
TransferCost {
    DataSize
    Latency
    Bandwidth
    SerializationCost
    SynchronizationCost
    EnergyCost
    MonetaryCost
    FailureRisk
}
```

Nicht jede Plattform muss jede Dimension exakt bestimmen können.

Fehlende Werte können durch Schätzungen oder konservative Defaults ersetzt werden.

---

## Integration mit Zero-Copy

Diese ADR ergänzt `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`.

Zero-Copy reduziert Datenbewegung, während Lokalitätsoptimierung versucht, Datenbewegung bereits durch die Wahl geeigneter Ausführungsorte zu vermeiden.

Beide Prinzipien ergänzen sich:

```text
Locality
    ↓
Avoid Movement

Zero-Copy
    ↓
Avoid Copies
```

Ein logischer Transfer muss dabei nicht zwingend einen physischen Copy bedeuten.

---

## Semantic Types und Representation

Semantic Type, physische Representation und Location bleiben getrennte Eigenschaften.

Beispiel:

```text
Semantic Type:
    Image<RGBA8>

Representation:
    GPU Texture

Location:
    GPU0.VRAM
```

Eine andere Instanz desselben semantischen Typs kann gleichzeitig vorliegen als:

```text
Semantic Type:
    Image<RGBA8>

Representation:
    Linear Buffer

Location:
    NUMA0.RAM
```

Der Planner darf diese Informationen zur Optimierung verwenden.

---

## Location Transparency

Diese ADR ergänzt `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`.

Location Transparency bedeutet nicht:

```text
Location does not matter.
```

sondern:

```text
Location does not define semantics.
```

Die physische Location bleibt für:

- Performance,
- Kosten,
- Fehler,
- Security,
- Sovereignty,
- Ressourcenplanung

relevant.

---

## Resource Economy

Lokalität wird in die systemweite Ressourcenökonomie integriert.

Ein Provider mit mehr freier Rechenleistung ist nicht automatisch besser, wenn seine Nutzung hohe Datenbewegung erzeugt.

Die Planung darf deshalb beispielsweise abwägen:

```text
Local CPU
    slower compute
    zero transfer
```

gegen:

```text
Remote GPU
    faster compute
    expensive transfer
```

---

## Execution Contracts

`Nova.ExecutionContract` kann Lokalitätsanforderungen und -präferenzen enthalten.

Beispiel:

```text
ExecutionContract {
    location = LocalPreferred
}
```

oder als Hard Constraint:

```text
ExecutionContract {
    location = LocalOnly
}
```

Dabei muss zwischen:

```text
Required
```

und:

```text
Preferred
```

unterschieden werden.

---

## Data Sovereignty

Data Sovereignty besitzt Vorrang vor reiner Lokalitätsoptimierung.

Wenn Daten beispielsweise:

```text
LocalOnly
```

markiert sind, darf NovaOS sie nicht zu einem entfernten Provider übertragen, nur weil dieser schneller wäre.

Lokalität kann dadurch vom Optimierungsziel zum Hard Constraint werden.

---

## Trust und Security

Ein lokal vorhandener Provider darf nicht allein aufgrund seiner Lokalität bevorzugt werden, wenn er notwendige Trust- oder Security-Anforderungen nicht erfüllt.

Beispiel:

```text
Local Provider
Trust = Unverified
```

gegen:

```text
Remote Provider
Trust = Verified
```

Der Execution Contract entscheidet, welche Kandidaten überhaupt zulässig sind.

Erst danach erfolgt die Optimierung.

---

## Structured Concurrency

Execution Scopes können Locality Hints oder Affinitäten besitzen.

Child Tasks können Lokalitätsinformationen vom Parent Scope erben, sofern dies sinnvoll ist.

Beispiel:

```text
ImagePipelineScope
    │
    ├── Decode
    ├── Resize
    ├── Filter
    └── Encode
```

kann bevorzugt auf demselben Device oder innerhalb derselben Memory Domain ausgeführt werden.

---

## Deterministic Mode

Adaptive Lokalitätsentscheidungen dürfen Deterministic Mode nicht verletzen.

Wenn reproduzierbare Ausführung verlangt wird, können relevante Entscheidungen fixiert werden:

```text
Provider
Device
NUMA Node
Location
Data Placement
```

oder über einen reproduzierbaren Plan bestimmt werden.

---

## Hot Replacement

Bei Hot Replacement soll NovaOS berücksichtigen, ob ein Providerwechsel große State- oder Datenmigrationen erzeugt.

Beispiel:

```text
Provider Generation 41
GPU0
```

zu:

```text
Provider Generation 42
GPU1
```

kann funktional kompatibel sein, aber erhebliche Migrationskosten verursachen.

Diese Kosten sollen in die Replacement-Entscheidung einfließen.

---

## Composability

Diese ADR ergänzt `ADR-ARCH-0017_Composability_und_Orthogonalität`.

Lokalität bleibt orthogonal zur Capability-Semantik.

Nicht:

```text
LocalImageResize
RemoteImageResize
GPUImageResize
```

sondern:

```text
Image.Resize
```

mit separat bestimmter:

```text
Provider
Algorithm
Location
Representation
```

---

## Graceful Degradation

Bei Ressourcenknappheit kann NovaOS eine lokalere, aber weniger leistungsfähige Alternative bevorzugen.

Beispiel:

```text
Preferred:
Remote AI Provider
```

wird bei Netzwerkproblemen:

```text
Fallback:
Local Classical Algorithm
```

sofern der Degradation Contract dies erlaubt.

---

## Fail-Safe Defaults

Kann die Sicherheit oder Zulässigkeit eines Datenstandorts nicht zuverlässig bestimmt werden, darf NovaOS Daten nicht allein aufgrund einer vermuteten Performanceverbesserung dorthin verschieben.

```text
Unknown Location Policy
        ↓
Do Not Move Sensitive Data
```

---

## Architecture Introspection

Lokalitätsentscheidungen müssen introspektierbar sein.

Architecture Introspection soll beispielsweise darstellen können:

```text
Object:
    Image#481

Semantic Type:
    Image<RGBA8>

Current Location:
    GPU0.VRAM

Consumer:
    Image.Filter

Selected Provider:
    GPU0.Provider

Reason:
    Data already resident on GPU0
```

Bei Datenbewegung sollen relevante Transfers sichtbar sein können.

---

## Decision Trace

Automatische Lokalitätsentscheidungen sollen strukturierte Gründe besitzen.

Beispiel:

```text
Selected:
    GPU0

Reasons:
    Input already resident on GPU0
    Output consumed by GPU encoder
    No additional transfer required

Rejected:
    GPU1

Reason:
    512 MiB transfer required
```

---

## Adaptive Optimierung

NovaOS darf Transfer- und Ausführungskosten lernen.

Beispielsweise:

```text
Predicted Transfer Cost
        ↓
Actual Transfer Cost
        ↓
Prediction Error
        ↓
Cost Model Update
```

Dadurch kann der Planner reale Hardwareeigenschaften berücksichtigen.

Adaptive Modelle dürfen jedoch keine Hard Constraints umgehen.

---

## Lokalitäts-Hints

Anwendungen und Komponenten dürfen Locality Hints angeben.

Beispiel:

```text
PreferSameDevice
PreferSameNUMANode
PreferLocalMachine
PreferDataLocation
```

Hints sind grundsätzlich Preferences.

Hard Constraints müssen explizit als solche gekennzeichnet sein.

---

## Keine unnötige Pinning-Architektur

Lokalitätsoptimierung darf nicht dazu führen, dass Tasks oder Daten grundsätzlich dauerhaft an Hardware gebunden werden.

Zu starke Affinität kann:

- Load Balancing verhindern,
- Hot Replacement erschweren,
- Ressourcen verschwenden,
- Fehlerdomänen vergrößern.

NovaOS muss Lokalität deshalb gegen Flexibilität abwägen.

---

## Normative Anforderungen

1. NovaOS MUSS Lokalität als systemweites Optimierungsziel unterstützen.
2. Lokalität DARF die semantische Identität einer Capability NICHT bestimmen.
3. Execution Planning SOLL aktuelle Datenlokation berücksichtigen.
4. Execution Planning SOLL Datenbewegungskosten berücksichtigen.
5. NovaOS SOLL Compute-to-Data bevorzugen, wenn dies insgesamt günstiger ist.
6. NovaOS DARF Data-to-Compute verwenden, wenn dies insgesamt günstiger ist.
7. Lokalität MUSS gegenüber höheren Hard Constraints nachrangig sein.
8. Data Sovereignty MUSS Lokalitätsoptimierung überstimmen können.
9. Security MUSS Lokalitätsoptimierung überstimmen können.
10. Trust Requirements MÜSSEN Lokalitätsoptimierung überstimmen können.
11. Hard Realtime Requirements MÜSSEN Lokalitätsoptimierung überstimmen können.
12. Deterministic Mode MUSS relevante Lokalitätsentscheidungen kontrollieren können.
13. Cache-Lokalität DARF berücksichtigt werden.
14. Core-Lokalität DARF berücksichtigt werden.
15. NUMA-Lokalität SOLL auf NUMA-Systemen berücksichtigt werden.
16. Memory-Domain-Lokalität SOLL berücksichtigt werden.
17. Accelerator-Lokalität SOLL berücksichtigt werden.
18. Machine- und Network-Lokalität SOLLEN bei verteilter Ausführung berücksichtigt werden.
19. Pipeline Planning SOLL End-to-End-Datenbewegung statt ausschließlich lokaler Providerkosten betrachten.
20. Data Gravity SOLL bei großen Datenobjekten berücksichtigt werden.
21. Transferkosten SOLLEN als explizite Planungsgröße modellierbar sein.
22. Zero-Copy und Lokalitätsoptimierung SOLLEN gemeinsam genutzt werden können.
23. Semantic Type, Representation und Location MÜSSEN unterscheidbar bleiben.
24. Provider und Location MÜSSEN unterscheidbar bleiben.
25. Location Transparency DARF physische Kosten und Fehler NICHT verbergen.
26. Resource Economy SOLL Datenbewegung in ihre Entscheidungen einbeziehen.
27. Execution Contracts MÜSSEN Locality Preferences ausdrücken können.
28. Execution Contracts MÜSSEN harte Location Constraints ausdrücken können.
29. Locality Hints MÜSSEN von Hard Constraints unterscheidbar sein.
30. Structured Concurrency SOLL Locality Hints hierarchisch propagieren können.
31. Hot Replacement SOLL Daten- und State-Migrationskosten berücksichtigen.
32. Graceful Degradation DARF lokalere Fallbacks verwenden.
33. Fail-Safe Defaults MÜSSEN bei unbekannter Datenzulässigkeit gelten.
34. Architecture Introspection SOLL aktuelle Daten- und Execution Locations darstellen können.
35. Architecture Introspection SOLL relevante Datenbewegungen darstellen können.
36. Automatische Lokalitätsentscheidungen SOLLEN über Decision Traces erklärbar sein.
37. Adaptive Cost Models DÜRFEN reale Transferkosten lernen.
38. Adaptive Optimierung DARF Hard Constraints NICHT verändern.
39. Lokalitätsoptimierung DARF keine unnötige dauerhafte Hardwarebindung erzwingen.
40. NovaOS MUSS Lokalität als Kosten- und Optimierungsdimension behandeln, nicht als versteckte Implementierungsannahme.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Datenbewegung,
- bessere Cache- und NUMA-Nutzung,
- effizientere GPU-/NPU-Pipelines,
- geringere Speicherbandbreitenbelastung,
- bessere Energieeffizienz,
- geringere Netzwerkbelastung,
- bessere End-to-End-Performance,
- bessere Grundlage für Zero-Copy,
- effizientere verteilte Verarbeitung,
- bessere Skalierung großer Daten- und KI-Workloads.

### Negative Konsequenzen

- Planner und Scheduler werden komplexer,
- Datenlokation muss systemweit modelliert werden,
- Transferkosten müssen gemessen oder geschätzt werden,
- Locality Hints können mit Load Balancing kollidieren,
- Hot Replacement kann durch große Datenmigrationen teurer werden,
- dynamische Hardwarezustände können Replanning erforderlich machen.

---

## Verworfene Alternativen

### Lokalität vollständig ignorieren

Verworfen.

Dies würde insbesondere bei NUMA-, GPU-, NPU- und verteilten Systemen erhebliche unnötige Kosten erzeugen.

### Immer Compute zu Data verschieben

Verworfen.

Bei kleinen Datenmengen oder wesentlich leistungsfähigeren entfernten Ressourcen kann Data-to-Compute günstiger sein.

### Immer Data zu schnellstem Provider verschieben

Verworfen.

Die Transferkosten können den Geschwindigkeitsgewinn vollständig aufheben.

### Dauerhaftes Hardware-Pinning

Verworfen.

Es würde Load Balancing, Resilience und Hot Replacement unnötig einschränken.

### Lokalität als Teil der Capability-Identität

Verworfen.

```text
Image.Resize
```

bleibt dieselbe semantische Capability unabhängig davon, ob sie auf CPU, GPU oder Remote ausgeführt wird.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0017_Composability_und_Orthogonalität`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`

---

## Zugehörige NPSPECs

- `NPSPEC-LOCALITY-MODEL-0001`
- `NPSPEC-LOCALITY-COST-0001`
- `NPSPEC-LOCALITY-HINT-0001`
- `NPSPEC-LOCALITY-PLANNER-0001`
- `NPSPEC-LOCALITY-CACHE-0001`
- `NPSPEC-LOCALITY-NUMA-0001`
- `NPSPEC-LOCALITY-MEMORY-0001`
- `NPSPEC-LOCALITY-DEVICE-0001`
- `NPSPEC-LOCALITY-ACCELERATOR-0001`
- `NPSPEC-LOCALITY-NETWORK-0001`
- `NPSPEC-LOCALITY-DATAGRAVITY-0001`
- `NPSPEC-LOCALITY-TRANSFER-0001`
- `NPSPEC-LOCALITY-PIPELINE-0001`
- `NPSPEC-LOCALITY-INTROSPECTION-0001`
- `NPSPEC-LOCALITY-ADAPTIVE-0001`

---

## Architekturregel

Die Architekturregel dieser ADR lautet:

```text
Keep data where it is useful.

Move computation toward data
when practical.

Move data only when the total
execution plan justifies the cost.
```

Damit wird Lokalität in NovaOS nicht zu einer festen Bindung, sondern zu einer systemweit sichtbaren und optimierbaren Eigenschaft.

```text
Semantics remain location-independent.

Performance does not.
```