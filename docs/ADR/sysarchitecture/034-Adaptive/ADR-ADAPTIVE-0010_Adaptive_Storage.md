# ADR-ADAPTIVE-0010 – Adaptive Storage

## Status

Angenommen

## Kategorie

Adaptive Systems / Storage

## Kontext

NovaOS verwaltet Daten über unterschiedliche Storage Provider, Geräte, Speicherklassen und verteilte Standorte.

Dabei unterscheiden sich Workloads hinsichtlich:

```text
Access Frequency
Latency
Throughput
Object Size
Read / Write Pattern
Locality
Retention
Availability
Energy
```

Statische Storage-Policies können diese Unterschiede nur begrenzt berücksichtigen.

NovaOS soll deshalb Storage-Verhalten beobachten, zukünftige Zugriffe vorhersagen und aus Prediction Error lernen.

## Entscheidung

NovaOS unterstützt **Adaptive Storage** als Optimierung innerhalb bestehender Storage-, Security- und Execution-Contracts.

```text
Storage State
     ↓
Prediction
     ↓
Storage Decision
     ↓
Actual Access
     ↓
Measured Result
     ↓
Prediction Error
     ↓
Model Correction
```

Adaptive Storage darf Korrektheit, Persistenz, Konsistenz oder andere Hard Requirements niemals abschwächen.

## Storage Predictions

NovaOS darf insbesondere vorhersagen:

```text
Access Frequency
Next Access
Read / Write Ratio
Expected Lifetime
Required Throughput
Expected Latency
Data Locality
Storage Pressure
Reuse Probability
```

Diese Vorhersagen dürfen zukünftige Storage-Entscheidungen beeinflussen.

## Adaptive Placement

Objekte dürfen abhängig von erwarteter Nutzung auf geeigneten Storage Providern platziert werden.

```text
Object Characteristics
        +
Access Prediction
        +
Storage Providers
        ↓
Placement Decision
```

Mögliche Ziele sind:

```text
Lower Latency
Higher Throughput
Better Locality
Lower Energy
Reduced Transfer Cost
Balanced Storage Load
```

## Storage Tiers

NovaOS darf unterschiedliche Storage-Klassen adaptiv verwenden.

Beispiel:

```text
Fast Local Storage
        ↓
Normal Storage
        ↓
Capacity Storage
        ↓
Archive
```

Die konkrete physische Technologie ist nicht Teil der logischen Storage-Semantik.

## Adaptive Migration

Ändert sich das Zugriffsmuster, darf NovaOS Objekte zwischen zulässigen Storage Providern verschieben.

```text
Observed Usage
      ↓
Changed Access Pattern
      ↓
Migration Evaluation
      ↓
New Placement
```

Eine Migration darf die stabile Object Identity nicht verändern.

```text
Object ID
    =
Before Migration
    =
After Migration
```

## Hot und Cold Data

NovaOS darf Zugriffsmuster verwenden, um Daten dynamisch zu klassifizieren.

Beispiele:

```text
Hot
Warm
Cold
Archive
```

Diese Klassifikation ist eine Optimierungsinformation und darf nicht mit Security-, Privacy- oder Retention-Klassifikation verwechselt werden.

## Prediction Error

Vorhergesagtes und tatsächliches Storage-Verhalten werden verglichen.

Beispiel:

```text
Predicted:
    Object rarely accessed

Actual:
    Object frequently accessed

        ↓

Prediction Error
        ↓
Storage Model Correction
```

Wiederholte Abweichungen sollen zukünftige Placement- und Migration-Entscheidungen verbessern.

## Data Locality

Adaptive Storage wird mit Execution Planning koordiniert.

```text
Expected Execution Location
          +
Object Location
          +
Transfer Cost
          ↓
Storage / Execution Planning
```

NovaOS darf Daten näher an erwartete Compute-Ressourcen bringen oder Compute näher an Daten planen.

## Replication

Adaptive Storage darf Replication Placement optimieren.

```text
Access Pattern
     +
Failure Domains
     +
Locality
     ↓
Replica Placement
```

Die vorgeschriebene Replikationszahl, Konsistenz und Failure-Domain-Unabhängigkeit bleiben Hard Constraints.

Adaptive Optimierung darf sie nicht reduzieren.

## Distributed Storage

Bei Distributed Storage können adaptive Modelle berücksichtigen:

```text
Node Load
Network Latency
Storage Latency
Bandwidth
Failure Domains
Replica Location
Data Locality
```

Eine bessere Performance rechtfertigt niemals eine Verletzung von Sovereignty oder Trust Requirements.

## Storage Pressure

Bei knappem Speicher darf Adaptive Storage geeignete Maßnahmen priorisieren.

Beispiele:

```text
Evict Reconstructable Cache
Remove Expired Temporary Data
Move Cold Data
Reduce Replication only if Contract permits
Reject New Allocation
```

Persistente Nutzerdaten dürfen nicht aufgrund einer adaptiven Vorhersage gelöscht werden.

## Caching und Prefetching

Adaptive Storage koordiniert sich mit:

```text
Adaptive Caching
Adaptive Prefetching
Predictive Preloading
```

Dabei bleibt die Trennung bestehen:

```text
Persistent Object
      ≠
Cache Copy
      ≠
Prefetched Copy
```

Der Verlust einer Cache- oder Prefetch-Kopie darf nicht mit dem Verlust des persistenten Objekts gleichgesetzt werden.

## Versionierung und Konsistenz

Adaptive Placement- oder Migration-Entscheidungen dürfen die Objektsemantik nicht verändern.

Insbesondere müssen erhalten bleiben:

```text
Object Identity
Version Identity
Integrity
Consistency Contract
Durability Contract
```

Ein schnellerer Storage Provider darf nicht verwendet werden, wenn er den geforderten Storage Contract nicht erfüllen kann.

## Data Sovereignty

Storage Placement unterliegt Data Sovereignty als Hard Constraint.

```text
Candidate Storage Locations
          ↓
Sovereignty Filtering
          ↓
Trust / Authorization
          ↓
Valid Locations
          ↓
Adaptive Optimization
```

Adaptive Storage darf keine Daten an unzulässige Orte verschieben.

## Energy

Storage Placement darf Energieverbrauch berücksichtigen.

Beispiele:

```text
Device Energy
Transfer Energy
Idle Cost
Wake-up Cost
Replication Cost
```

Energy Optimization bleibt innerhalb der gültigen Storage Contracts.

## Nutzerentscheidung

Explizite Nutzerentscheidungen besitzen Vorrang vor adaptiven Storage-Präferenzen, soweit keine Hard Constraints verletzt werden.

Beispiele:

```text
Keep Local
Do Not Archive
Pin to Device
Disable Automatic Migration
```

Adaptive Modelle dürfen solche Entscheidungen nicht selbstständig überschreiben.

## Deterministic Mode

Adaptive Storage darf deterministische Ausführungen nicht unkontrolliert beeinflussen.

Wenn Storage Placement für Reproduzierbarkeit relevant ist, muss der adaptive Zustand:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

Die logische Objektsemantik darf unabhängig vom physischen Placement nicht verändert werden.

## Observability

Adaptive Storage muss mit Observability korrelierbar sein.

Relevante Informationen können sein:

```text
Predicted Access
Actual Access
Storage Provider
Placement Decision
Migration
Transfer Cost
Latency
Throughput
Storage Pressure
Prediction Error
```

## Explainability

NovaOS soll relevante Storage-Entscheidungen erklären können.

Beispiele:

```text
Warum wurde dieses Objekt verschoben?

Warum liegt dieses Objekt auf diesem Provider?

Warum wurde eine zusätzliche Replica erzeugt?

Warum wurde eine Migration verworfen?

Warum wurde ein Objekt nicht archiviert?
```

## Cold Start

Ohne ausreichende historische Daten verwendet NovaOS:

```text
Storage Contracts
Declared Object Properties
Semantic Metadata
Provider Capabilities
Current Measurements
Safe Defaults
```

Adaptive Modelle sind keine Voraussetzung für korrektes Storage Management.

## Normative Anforderungen

1. NovaOS SOLL Adaptive Storage unterstützen.
2. Adaptive Storage Decisions SOLLEN Prediction Error als Lernsignal verwenden.
3. Vorhergesagte und tatsächliche Zugriffsmuster MÜSSEN korrelierbar sein.
4. Storage Placement DARF adaptiv optimiert werden.
5. Adaptive Migration DARF stabile Object Identity nicht verändern.
6. Hot-, Warm- und Cold-Klassifikation DARF als Optimierungsinformation verwendet werden.
7. Adaptive Storage MUSS Data Locality berücksichtigen können.
8. Replication Placement DARF adaptiv optimiert werden.
9. Replication-, Durability- und Consistency-Contracts DÜRFEN nicht adaptiv abgeschwächt werden.
10. Persistent Objects DÜRFEN nicht aufgrund spekulativer Vorhersagen gelöscht werden.
11. Persistent Storage, Cache und Prefetch Copies MÜSSEN semantisch unterscheidbar bleiben.
12. Object Identity, Version Identity und Integrity MÜSSEN bei Migration erhalten bleiben.
13. Data Sovereignty, Trust und Authorization MÜSSEN Vorrang vor Storage-Optimierung besitzen.
14. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Storage-Präferenzen besitzen.
15. Deterministic Mode DARF durch adaptive Storage-Zustände nicht unkontrolliert beeinflusst werden.
16. Adaptive Storage Decisions MÜSSEN mit Observability und Decision Tracing korrelierbar sein.
17. Relevante Placement- und Migration-Entscheidungen SOLLEN erklärbar sein.
18. Ohne adaptive Modelle MUSS korrektes Storage Management weiterhin möglich sein.

## Konsequenzen

### Positive Konsequenzen

- Storage Placement passt sich realen Zugriffsmustern an,
- häufig benötigte Daten können näher an Compute und Nutzer gebracht werden,
- selten benötigte Daten können ressourcenschonender platziert werden,
- Replica Placement kann Last, Locality und Failure Domains berücksichtigen,
- wiederholte Fehlprognosen verbessern zukünftige Storage-Entscheidungen.

### Negative Konsequenzen

- Migration und Replication erzeugen zusätzlichen I/O- und Netzwerkverkehr,
- falsche Vorhersagen können unnötige Datenbewegungen verursachen,
- adaptive Storage-Modelle benötigen zusätzlichen Zustand und Observability.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0005_Adaptive_Caching_und_Prefetching`
- `ADR-ADAPTIVE-0006_Predictive_Preloading`
- `ADR-ADAPTIVE-0008_Adaptive_Power_Management`
- `ADR-ADAPTIVE-0009_Adaptive_Networking`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-DISTRIBUTED-0002_Distributed_Storage`
- `ADR-DISTRIBUTED-0006_Failure_Domain_Aware_Placement`
- `ADR-DISTRIBUTED-0007_Replication_und_Consistency_Profiles`
- `ADR-EXECUTION-0005_Data_Sovereignty_im_ExecutionContract`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-STORAGE-0001`
- `NPSPEC-ADAPTIVE-STORAGE-PLACEMENT-0001`
- `NPSPEC-ADAPTIVE-STORAGE-MIGRATION-0001`
- `NPSPEC-ADAPTIVE-STORAGE-ACCESS-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-STORAGE-FEEDBACK-0001`

## Ergebnis

NovaOS erweitert Storage Management um einen kontrollierten adaptiven Feedback-Zyklus:

```text
Observe Access
      ↓
Predict Usage
      ↓
Placement / Migration
      ↓
Actual Access
      ↓
Measure
      ↓
Prediction Error
      ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf lernen,
wo Daten sinnvollerweise liegen.

Das physische Placement
darf sich an reale Nutzung anpassen.

Identität, Korrektheit,
Persistenz, Konsistenz,
Sovereignty und Contracts
bleiben davon unberührt.
```