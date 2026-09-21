# ADR-ADAPTIVE-0007 – Adaptive Memory Management

## Status

Angenommen

## Kategorie

Adaptive Systems / Memory Management

## Kontext

NovaOS verwaltet Speicher für unterschiedliche Workloads, Capabilities, Prozesse, Caches und Compute Provider.

Der tatsächliche Speicherbedarf kann sich dynamisch verändern.

Beispiele:

```text
Working Sets
Caches
Prefetch Data
Shared Memory
Execution Buffers
GPU / Accelerator Memory
Memory-Mapped Objects
```

Statische Regeln können nicht für jeden Workload optimale Entscheidungen treffen.

NovaOS soll deshalb beobachteten Speicherverbrauch und Prediction Error nutzen, um zukünftige Memory-Management-Entscheidungen zu verbessern.

## Entscheidung

NovaOS unterstützt **Adaptive Memory Management** als Optimierung oberhalb der verbindlichen Speicher- und Ressourcenmechanismen.

```text
Workload
   ↓
Memory Prediction
   ↓
Allocation / Placement Policy
   ↓
Execution
   ↓
Measured Memory Behavior
   ↓
Prediction Error
   ↓
Model Correction
```

Adaptive Entscheidungen dürfen ausschließlich innerhalb bestehender Hard Constraints erfolgen.

## Memory Predictions

NovaOS darf insbesondere vorhersagen:

```text
Working Set Size
Peak Memory
Allocation Rate
Lifetime
Reuse
Memory Pressure
Page Activity
Cache Value
Expected Locality
```

Diese Vorhersagen können zukünftige Speicherentscheidungen beeinflussen.

## Allocation

Vorhersagen dürfen die Vorbereitung und Verteilung von Speicher verbessern.

```text
Predicted Demand
      ↓
Resource Availability
      ↓
Allocation Strategy
```

Dabei darf vorhergesagter Bedarf nicht automatisch als tatsächlicher Verbrauch verbucht werden.

Es bleibt die Trennung:

```text
Predicted
Reserved
Allocated
Used
```

## Working Sets

NovaOS darf aktive Working Sets erkennen und deren zukünftige Nutzung vorhersagen.

```text
Observed Access
      ↓
Working Set Model
      ↓
Expected Future Access
```

Häufig verwendete Speicherbereiche können dadurch bevorzugt verfügbar gehalten werden.

## Memory Pressure

Unter Speicherdruck muss notwendiger Speicher Vorrang vor spekulativer Nutzung besitzen.

Grundsätzlich:

```text
Required Memory
      >
Reserved Memory
      >
Active Working Sets
      >
Caches
      >
Predictive Preloading
```

Die konkrete Priorisierung bleibt Policy-abhängig.

Hard Requirements dürfen dabei nicht verletzt werden.

## Adaptive Reclamation

NovaOS darf anhand erwarteter zukünftiger Nutzung entscheiden, welche Speicherbereiche bevorzugt freigegeben werden.

Beispiele:

```text
Unused Cache
Low-Reuse Pages
Expired Preloads
Reconstructable Data
Inactive Working Sets
```

Die Kosten einer späteren Wiederherstellung sollen berücksichtigt werden.

## Memory Locality

Adaptive Memory Management darf Speicherlokalität berücksichtigen.

Beispiele:

```text
NUMA Node
CPU Local Memory
GPU Memory
Shared Memory
Remote Memory
```

Planung kann versuchen, Daten nahe am erwarteten Compute Provider zu halten.

```text
Expected Compute Location
          +
Memory Location
          +
Transfer Cost
          ↓
Placement Decision
```

## Accelerator Memory

GPU-, NPU- und andere Accelerator-Speicher werden in dasselbe adaptive Modell integriert.

NovaOS darf beispielsweise zwischen:

```text
Keep in Device Memory
Move to System Memory
Evict
Reload Later
```

entscheiden.

Transferkosten müssen berücksichtigt werden.

## Caching und Preloading

Adaptive Memory Management koordiniert sich mit:

```text
Adaptive Caching
Adaptive Prefetching
Predictive Preloading
```

Spekulative Optimierungen dürfen notwendigen Speicher nicht verdrängen.

```text
Memory Pressure
      ↓
Reduce Preloading
      ↓
Evict Low-Value Cache
      ↓
Protect Required Work
```

## Prediction Error

Vorhergesagter und tatsächlicher Speicherbedarf werden verglichen.

Beispiel:

```text
Predicted Peak: 512 MiB
Actual Peak:    780 MiB
        ↓
Prediction Error
        ↓
Memory Model Correction
```

Wiederholte Abweichungen sollen zukünftige Reservations-, Placement- und Reclamation-Entscheidungen verbessern.

## Resource Budgets

Adaptive Memory Management muss Resource Budgets respektieren.

```text
Memory Budget
      ↓
Valid Memory Space
      ↓
Adaptive Optimization
```

Ein adaptives Modell darf ein Hard Memory Limit niemals selbstständig erhöhen.

## Distributed Memory

Falls entfernte Speicherressourcen verfügbar sind, dürfen sie nur verwendet werden, wenn der Execution Contract dies erlaubt.

Dabei müssen insbesondere berücksichtigt werden:

```text
Latency
Bandwidth
Trust
Security
Data Sovereignty
Failure Semantics
```

Remote Memory ist keine transparente Erweiterung lokalen Speichers ohne Kosten und Fehlergrenzen.

## Deterministic Mode

Adaptive Speicherentscheidungen dürfen deterministische Ausführungen nicht semantisch verändern.

Wenn Memory Placement oder adaptive Zustände für Reproduzierbarkeit relevant sind, müssen sie:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

## Resource Tracing

Adaptive Memory Management wird mit Resource Tracing verbunden.

```text
Prediction
    ↓
Memory Decision
    ↓
Allocation
    ↓
Actual Usage
    ↓
Resource Trace
    ↓
Prediction Error
```

Dadurch können reale Speicherprofile zukünftige Entscheidungen verbessern.

## Explainability

NovaOS soll relevante adaptive Speicherentscheidungen erklären können.

Beispiele:

```text
Warum wurde Speicher reserviert?

Warum wurde dieser Cache verworfen?

Warum wurde ein Working Set bevorzugt?

Warum wurde Speicher auf einen anderen NUMA Node verschoben?

Warum wurde Predictive Preloading reduziert?
```

## Failure und Degradation

Kann Speicherbedarf nicht erfüllt werden, gelten die normalen Resource- und Resilience-Regeln.

Adaptive Memory Management darf insbesondere keine gefährlichen Annahmen treffen wie:

```text
Prediction says memory will probably be enough
        ↓
Ignore Hard Limit
```

Bei Unsicherheit gilt für garantierte Anforderungen:

```text
Unknown ≠ Satisfied
```

## Cold Start

Ohne ausreichende historische Daten verwendet NovaOS:

```text
Static Memory Policies
Declared Requirements
Hardware Topology
Resource Budgets
Safe Defaults
```

Adaptive Modelle sind keine Voraussetzung für korrektes Memory Management.

## Normative Anforderungen

1. NovaOS SOLL Adaptive Memory Management unterstützen.
2. Adaptive Speicherentscheidungen SOLLEN Prediction Error als Lernsignal verwenden.
3. Vorhergesagter und tatsächlicher Speicherverbrauch MÜSSEN unterscheidbar bleiben.
4. Predicted, Reserved, Allocated und Used Memory MÜSSEN getrennt behandelbar sein.
5. Working Sets DÜRFEN adaptiv erkannt und bewertet werden.
6. Memory Pressure MUSS notwendige Arbeit gegenüber spekulativer Nutzung priorisieren können.
7. Adaptive Reclamation DARF erwartete Wiederverwendung und Wiederherstellungskosten berücksichtigen.
8. Memory Locality und Hardwaretopologie DÜRFEN in Placement Decisions einfließen.
9. Accelerator Memory MUSS in die Speicherplanung integrierbar sein.
10. Caching und Predictive Preloading MÜSSEN unter Memory Pressure begrenzbar sein.
11. Hard Memory Budgets DÜRFEN durch adaptive Modelle nicht abgeschwächt werden.
12. Distributed Memory MUSS Latency-, Trust-, Security- und Sovereignty-Constraints berücksichtigen.
13. Deterministic Mode DARF durch adaptive Speicherzustände nicht unkontrolliert beeinflusst werden.
14. Adaptive Memory Decisions MÜSSEN mit Resource Tracing korrelierbar sein.
15. Relevante Memory Decisions SOLLEN erklärbar sein.
16. `Unknown` DARF garantierte Speicheranforderungen nicht erfüllen.
17. Ohne adaptive Modelle MUSS korrektes Memory Management weiterhin möglich sein.

## Konsequenzen

### Positive Konsequenzen

- Speicherplanung kann sich an reale Workloads anpassen,
- Working Sets können effizienter behandelt werden,
- Cache und Preloading reagieren besser auf Memory Pressure,
- NUMA- und Accelerator-Speicher können workloadabhängig optimiert werden,
- wiederholte Fehlprognosen verbessern zukünftige Speicherentscheidungen.

### Negative Konsequenzen

- zusätzliche Modelle und Laufzeitmessungen erzeugen Overhead,
- falsche Vorhersagen können kurzfristig ungünstige Placement- oder Reclamation-Entscheidungen verursachen,
- historische Speicherprofile können nach Workload- oder Hardwareänderungen veralten.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-ADAPTIVE-0005_Adaptive_Caching_und_Prefetching`
- `ADR-ADAPTIVE-0006_Predictive_Preloading`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0007_Explainable_Resource_Allocation`
- `ADR-RESILIENCE-0015_Resource_Exhaustion_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-MEMORY-0001`
- `NPSPEC-ADAPTIVE-MEMORY-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-MEMORY-PLACEMENT-0001`
- `NPSPEC-ADAPTIVE-MEMORY-RECLAMATION-0001`
- `NPSPEC-ADAPTIVE-MEMORY-FEEDBACK-0001`

## Ergebnis

NovaOS erweitert Memory Management um einen kontrollierten adaptiven Feedback-Zyklus:

```text
Observe Memory Behavior
        ↓
Predict Demand
        ↓
Allocate / Place / Reclaim
        ↓
Measure Actual Usage
        ↓
Prediction Error
        ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf aus realem
Speicherverhalten lernen
und zukünftige Entscheidungen
darauf optimieren.

Adaptation entscheidet jedoch nur,
wie vorhandener Speicher
besser genutzt wird.

Sie darf niemals
Speichergrenzen, Korrektheit
oder Hard Constraints aufheben.
```