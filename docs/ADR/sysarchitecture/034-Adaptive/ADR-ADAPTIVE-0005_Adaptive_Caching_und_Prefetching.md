# ADR-ADAPTIVE-0005 – Adaptive Caching und Prefetching

## Status

Angenommen

## Kategorie

Adaptive Systems / Caching / Prefetching

## Kontext

NovaOS soll häufig oder wahrscheinlich benötigte Daten und Ressourcen möglichst früh verfügbar machen.

Dazu gehören beispielsweise:

```text
Objects
File Data
Metadata
Capabilities
Libraries
Application State
UI Resources
Execution Data
```

Statische Cache- und Prefetch-Regeln können reale Nutzungsmuster jedoch nur begrenzt berücksichtigen.

NovaOS soll deshalb aus tatsächlicher Nutzung und Fehlvorhersagen lernen.

## Entscheidung

NovaOS unterstützt **Adaptive Caching und Prefetching** auf Basis beobachteter Zugriffsmuster und Prediction Error.

```text
Usage History
     ↓
Prediction
     ↓
Cache / Prefetch Decision
     ↓
Actual Access
     ↓
Prediction Error
     ↓
Model Correction
```

Adaptive Entscheidungen bleiben reine Optimierungen und dürfen für die korrekte Funktion nicht erforderlich sein.

## Adaptive Caching

NovaOS darf vorhersagen, welche Daten voraussichtlich erneut benötigt werden.

```text
Object Access
     ↓
Reuse Prediction
     ↓
Cache Decision
```

Die Entscheidung kann unter anderem berücksichtigen:

```text
Access Frequency
Recency
Object Size
Reuse Probability
Load Cost
Memory Pressure
Current Workload
Execution Context
```

## Adaptive Prefetching

NovaOS darf Daten vor ihrer tatsächlichen Anforderung vorbereiten.

```text
Prediction
    ↓
Prefetch
    ↓
User / Execution Request
   ├── Match → Immediate Availability
   └── Miss  → Prediction Error
```

Prefetching darf keine Voraussetzung dafür sein, dass ein späterer normaler Zugriff funktioniert.

## Nutzerbezogene Vorhersage

Prefetching darf auf vorhergesagten Nutzeraktionen basieren.

Beispiel:

```text
Prediction:
    User likely opens Object A

        ↓

Prefetch Object A

        ↓

Actual User Decision
```

Weicht die tatsächliche Entscheidung ab, wird dies als Prediction Error behandelt.

```text
Prediction A
    ↓
User chooses B
    ↓
Deviation
    ↓
Model Correction
```

## Cache Hit und Miss

Adaptive Modelle verwenden tatsächliche Ergebnisse als Feedback.

```text
Predicted Reuse
     ↓
Cached
     ↓
Reused
     ↓
Prediction Confirmed
```

oder:

```text
Predicted Reuse
     ↓
Cached
     ↓
Not Reused
     ↓
Prediction Error
```

Auch fehlendes Prefetching kann ein Lernsignal erzeugen:

```text
Not Prefetched
      ↓
Expensive Access
      ↓
Missed Opportunity
      ↓
Model Correction
```

## Kostenmodell

Caching und Prefetching müssen ihre eigenen Kosten berücksichtigen.

Beispiele:

```text
Memory Consumption
Storage I/O
Network Traffic
Energy
CPU Usage
Transfer Cost
Cache Pollution
Eviction Cost
```

Eine hohe Zugriffswahrscheinlichkeit allein rechtfertigt nicht automatisch einen Prefetch.

## Resource Budgets

Adaptive Caching und Prefetching unterliegen Resource Budgets.

```text
Prediction
    ↓
Expected Benefit
    +
Resource Cost
    ↓
Decision
```

Bei Ressourcenknappheit dürfen spekulative Daten bevorzugt verworfen werden.

## Cache Pressure

Unter Memory- oder Storage-Druck muss NovaOS zwischen wichtigen und spekulativen Cache-Inhalten unterscheiden können.

```text
Resource Pressure
      ↓
Eviction Candidates
      ↓
Expected Reuse / Cost
      ↓
Eviction Decision
```

Adaptive Modelle dürfen Eviction unterstützen, aber keine verbindlichen Resource Limits verletzen.

## Data Locality

Caching und Prefetching dürfen Data Locality berücksichtigen.

```text
Data Location
     +
Expected Access Location
     +
Transfer Cost
     ↓
Cache Placement
```

Dadurch können Daten näher an erwartete zukünftige Ausführungen gebracht werden.

## Distributed Caching

In verteilten Umgebungen darf NovaOS Cache- oder Prefetch-Kopien auf anderen Nodes erzeugen, sofern dies zulässig ist.

```text
Object
   ↓
Predicted Remote Use
   ↓
Sovereignty + Trust Check
   ↓
Remote Prefetch
```

Data Sovereignty, Security, Privacy und Trust bleiben Hard Constraints.

## Semantische Daten

Semantic Types und Metadata dürfen für Vorhersagen verwendet werden.

Beispiele:

```text
Image
Document
Executable
Model
Configuration
Media Stream
```

NovaOS kann dadurch unterschiedliche Cache-Strategien für unterschiedliche Datenarten verwenden.

## Versionierung und Konsistenz

Ein Cache-Eintrag muss eindeutig auf die zugrunde liegende Objektversion bezogen sein.

```text
Object ID
   +
Version ID
   ↓
Cache Entry
```

Veraltete Daten dürfen nicht aufgrund adaptiver Optimierung als aktuelle Version ausgegeben werden.

Korrektheit und Konsistenz besitzen Vorrang vor Cache Performance.

## Security und Privacy

Adaptive Caches dürfen bestehende Sicherheitsgrenzen nicht umgehen.

Insbesondere darf Prefetching nicht dazu führen, dass Daten an einen Ort gelangen, an dem sie laut Contract nicht vorhanden sein dürfen.

```text
Prediction
    ↓
Security / Privacy / Sovereignty
    ↓
Allowed Cache Locations
```

## Explizite Nutzerentscheidung

Explizite Nutzerentscheidungen besitzen Vorrang vor adaptiven Vorhersagen.

Beispiel:

```text
Prediction:
    Object A likely needed

User:
    Opens Object B

        ↓

User Decision wins
        ↓
Prediction Error
        ↓
Model Correction
```

Das System darf den Nutzer nicht zugunsten seiner Prefetch-Vorhersage beeinflussen.

## Deterministic Mode

Adaptive Cache-Zustände dürfen deterministische Ausführungen nicht unkontrolliert beeinflussen.

Wenn Cache-Verhalten für eine deterministische Ausführung relevant ist, muss der Zustand:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

Caching darf das Ergebnis einer korrekt definierten Operation nicht verändern.

## Observability

Adaptive Caching und Prefetching sollen messbar sein.

Relevante Informationen können sein:

```text
Cache Hit
Cache Miss
Prefetch Hit
Prefetch Miss
Eviction
Prefetch Cost
Avoided Latency
Wasted Transfer
Prediction Error
```

Diese Daten können zur weiteren Modellverbesserung verwendet werden.

## Cold Start

Ohne ausreichende historische Daten verwendet NovaOS:

```text
Static Cache Policies
Locality Heuristics
Declared Metadata
Safe Defaults
```

Adaptive Modelle sind keine Voraussetzung für funktionierendes Caching.

## Normative Anforderungen

1. NovaOS SOLL Adaptive Caching und Prefetching unterstützen.
2. Adaptive Cache- und Prefetch-Entscheidungen SOLLEN Prediction Error als Lernsignal verwenden.
3. Vorhersagen MÜSSEN mit tatsächlichen Zugriffen korrelierbar sein können.
4. Cache Hits, Cache Misses, Prefetch Hits und Prefetch Misses SOLLEN als Feedback nutzbar sein.
5. Prefetching MUSS eine optionale Optimierung bleiben.
6. Fehlvorhersagen DÜRFEN die korrekte Funktion nicht beeinträchtigen.
7. Cache- und Prefetch-Kosten MÜSSEN bei Entscheidungen berücksichtigt werden können.
8. Resource Budgets DÜRFEN durch spekulatives Prefetching nicht verletzt werden.
9. Spekulative Cache-Inhalte SOLLEN bei Ressourcenknappheit bevorzugt freigebbar sein.
10. Data Locality DARF für Cache Placement verwendet werden.
11. Distributed Caching MUSS Security-, Privacy-, Trust- und Data-Sovereignty-Constraints einhalten.
12. Cache-Einträge MÜSSEN eindeutig einer gültigen Objektversion zugeordnet werden können.
13. Adaptive Optimierung DARF Konsistenz und Korrektheit nicht abschwächen.
14. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Vorhersagen besitzen.
15. Deterministic Mode DARF durch adaptive Cache-Zustände nicht unkontrolliert beeinflusst werden.
16. Adaptive Caching und Prefetching SOLLEN über Observability messbar sein.
17. Ohne adaptive Modelle MUSS korrektes Caching weiterhin möglich sein.

## Konsequenzen

### Positive Konsequenzen

- häufig benötigte Daten können schneller verfügbar sein,
- Prefetching verbessert sich anhand realer Nutzung,
- unnötige Prefetches werden durch Prediction Error reduziert,
- Cache Placement kann Workload und Locality berücksichtigen,
- das System kann sich dynamisch an Nutzungsmuster anpassen.

### Negative Konsequenzen

- Fehlvorhersagen verbrauchen Ressourcen,
- adaptive Modelle benötigen zusätzliche Zustandsinformationen,
- verteiltes Prefetching erhöht die Anforderungen an Sovereignty und Konsistenz.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0002_Prediction_User_Decision_Deviation_Model_Correction`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0005_Data_Sovereignty_im_ExecutionContract`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-CACHING-0001`
- `NPSPEC-ADAPTIVE-PREFETCHING-0001`
- `NPSPEC-ADAPTIVE-CACHE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-CACHE-EVICTION-0001`
- `NPSPEC-ADAPTIVE-CACHE-FEEDBACK-0001`

## Ergebnis

NovaOS behandelt Caching und Prefetching als lernende, aber vollständig optionale Optimierung:

```text
Observe
   ↓
Predict
   ↓
Cache / Prefetch
   ↓
Actual Access
   ↓
Hit / Miss
   ↓
Prediction Error
   ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf versuchen,
Daten bereitzuhalten,
bevor sie benötigt werden.

Trifft die Vorhersage nicht zu,
lernt das System daraus.

Korrektheit, Nutzerentscheidung
und Hard Constraints haben dabei
immer Vorrang vor Geschwindigkeit.
```