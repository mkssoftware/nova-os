# ADR-AI-0003 – Model Scheduling und Caching

## Status

Angenommen

## Kategorie

AI / Runtime / Scheduling / Caching

## Kontext

AI-Modelle können erhebliche Ressourcen benötigen:

```text
RAM
VRAM
NPU Memory
Storage I/O
Load Time
Initialization Time
```

Ein Modell für jede Anfrage neu zu laden wäre ineffizient. Gleichzeitig können nicht alle verfügbaren Modelle dauerhaft im Speicher gehalten werden.

NovaOS benötigt daher eine gemeinsame Strategie für:

```text
Model Loading
Model Residency
Model Scheduling
Model Sharing
Model Caching
Model Eviction
Predictive Preloading
```

## Entscheidung

Die `Nova.AI Runtime` verwaltet Model Scheduling und Model Caching systemweit.

```text
AI Requests
     ↓
Model Selection
     ↓
Model Scheduler
     ↓
Resident?
 ├── Yes → Execute
 └── No  → Load / Prepare
              ↓
           Execute
```

Modelle werden abhängig von Bedarf, Ressourcen, Contracts und erwarteter Wiederverwendung geladen, gehalten oder entfernt.

## Model State

Ein Modell kann sich in folgenden Zuständen befinden:

```text
Registered
Available
Loading
Resident
Executing
Idle
Evicting
Unavailable
```

`Registered` bedeutet nicht, dass das Modell bereits im Arbeitsspeicher oder Accelerator Memory vorhanden ist.

## Model Scheduling

Der Model Scheduler koordiniert konkurrierende AI Workloads.

Berücksichtigt werden insbesondere:

```text
Execution Contract
Priority
Deadline
Latency Requirement
Resource Budget
Model Load Cost
Current Residency
Provider Availability
Expected Runtime
Expected Reuse
Energy Cost
```

Der Scheduler ergänzt das allgemeine NovaOS Scheduling und ersetzt es nicht.

## Shared Model Residency

Kompatible AI-Ausführungen dürfen dieselbe geladene Modellinstanz verwenden.

```text
Execution A ─┐
Execution B ─┼→ Resident Model
Execution C ─┘
```

Dabei bleiben Execution Context, Permissions und Input-Daten voneinander isoliert.

## Model Cache

Geladene, aktuell nicht verwendete Modelle dürfen als Cache im Speicher verbleiben.

```text
Execute
   ↓
Idle Model
   ↓
Keep Resident?
 ├── Yes → Model Cache
 └── No  → Unload
```

Der Model Cache ist eine Optimierung und kein persistenter Zustand.

## Cache Levels

Modelle dürfen auf unterschiedlichen Ebenen gecacht werden.

```text
Storage
   ↓
RAM
   ↓
VRAM / NPU Memory
   ↓
Active Execution
```

Je näher ein Modell an der Ausführung gehalten wird, desto geringer ist typischerweise die Startlatenz, aber desto höher sind die belegten Ressourcen.

## Cache Identity

Cache-Einträge müssen eindeutig an die konkrete Modellversion gebunden sein.

Mindestens:

```text
Model ID
Model Version
Variant
Runtime Adapter Version
Provider Compatibility
```

Eine gecachte alte Modellversion darf nicht stillschweigend als neue Version verwendet werden.

## Eviction

Bei Ressourcenknappheit darf NovaOS nicht benötigte Modelle entfernen.

Mögliche Faktoren sind:

```text
Last Use
Reuse Probability
Model Size
Reload Cost
Current Pressure
Priority
Load Time
Energy Cost
Provider Demand
```

Grundsätzlich gilt:

```text
Required Execution
      >
Reserved Resources
      >
Active Models
      >
Idle Model Cache
      >
Predictive Preloading
```

## Predictive Model Preloading

NovaOS darf Modelle vorausschauend laden.

```text
Context
   ↓
Predicted AI Request
   ↓
Model Candidate
   ↓
Resource Check
   ↓
Preload
```

Dadurch kann die wahrgenommene Startlatenz reduziert werden.

Predictive Preloading bleibt vollständig optional.

Eine falsche Vorhersage darf die normale Systemfunktion nicht beeinträchtigen.

## Prediction Error

Vorhersagen werden mit der tatsächlichen Nutzung verglichen.

```text
Predicted Model
      ↓
Preloaded
      ↓
Actual Request
      ↓
Used / Not Used
      ↓
Prediction Error
```

Diese Daten dürfen zukünftige Preloading- und Cache-Entscheidungen verbessern.

## Resource Pressure

Bei Speicher- oder Accelerator-Druck werden spekulative Ressourcen zuerst reduziert.

```text
Resource Pressure
      ↓
Cancel Predictive Loads
      ↓
Evict Idle Models
      ↓
Reduce Cache
      ↓
Replan AI Workloads
```

Aktive oder vertraglich reservierte Ausführungen besitzen höhere Priorität.

## Accelerator Memory

VRAM und NPU Memory werden als systemweite Ressourcen behandelt.

AI-Modelle dürfen Accelerator Memory nicht dauerhaft exklusiv beanspruchen, sofern dies nicht ausdrücklich reserviert wurde.

Die AI Runtime koordiniert dies mit der Unified Compute Runtime.

## Model Migration

Wenn technisch unterstützt, darf eine AI-Ausführung oder Modellinstanz zwischen Providern neu geplant werden.

Beispiel:

```text
GPU Pressure
    ↓
Alternative Provider
    ↓
Contract Check
    ↓
CPU / NPU / Other GPU
```

Migration oder Provider-Wechsel dürfen den Execution Contract nicht verletzen.

## Quantized Variants

Bei Ressourcenknappheit darf NovaOS eine kleinere Modellvariante wählen, wenn diese als kompatibler Kandidat registriert ist.

```text
FP16 Model
    ↓
Insufficient Resources
    ↓
INT8 Variant
```

Dies ist nur zulässig, wenn Qualitäts-, Determinismus- und andere Hard Requirements weiterhin erfüllt werden.

Eine Quantisierung darf nicht stillschweigend als identische Ausführung behandelt werden.

## Multi-Model Workloads

AI-Pipelines können mehrere Modelle benötigen.

```text
Speech Model
     ↓
Language Model
     ↓
Embedding Model
```

Der Scheduler darf Modelle gemeinsam planen, um unnötige Lade- und Entladevorgänge zu vermeiden.

## Fairness

Ein großer AI-Workload darf kleinere oder interaktive AI-Anfragen nicht unbegrenzt blockieren.

Scheduling muss bestehende:

```text
Priority
Fairness
Deadline
Resource Budget
Interactive Latency
```

berücksichtigen.

## Deterministic Mode

Deterministic Mode darf durch Cache-Zustände keine semantisch unterschiedlichen Ergebnisse erzeugen.

Für reproduzierbare Ausführung können insbesondere fixiert werden:

```text
Model ID
Model Version
Variant
Provider
Runtime Adapter
Execution Parameters
```

Ob ein Modell bereits gecacht war, darf die fachliche Bedeutung der Ausführung nicht verändern.

## Security und Isolation

Shared Model Caching darf keine Daten zwischen unterschiedlichen Executions übertragen.

Insbesondere dürfen nicht unbeabsichtigt geteilt werden:

```text
Prompt State
User Context
KV Cache
Embeddings
Temporary Inputs
Generated Outputs
Secrets
```

Solche zustandsbehafteten Daten benötigen explizite Ownership- und Isolation-Regeln.

## Observability

Model Scheduling und Caching müssen beobachtbar sein.

Relevante Informationen sind:

```text
Model ID
Model Version
Load Time
Queue Time
Execution Time
Cache Hit
Cache Miss
Eviction
Preload Hit
Preload Miss
Memory Usage
Provider
Decision ID
```

Damit können Scheduling- und Cache-Strategien anhand realer Messwerte verbessert werden.

## Normative Anforderungen

1. NovaOS MUSS Model Scheduling systemweit koordinieren können.
2. Model Scheduling MUSS Execution Contracts berücksichtigen.
3. Geladene Modelle DÜRFEN zwischen kompatiblen Executions geteilt werden.
4. Shared Models DÜRFEN Execution Isolation nicht aufheben.
5. Idle Models DÜRFEN als Cache resident bleiben.
6. Model Cache MUSS unter Ressourcenknappheit freigebbar sein.
7. Cache-Einträge MÜSSEN an Model ID, Version und relevante Variante gebunden sein.
8. Predictive Model Preloading DARF verwendet werden.
9. Predictive Preloading MUSS gegenüber erforderlichen Workloads nachrangig sein.
10. Prediction Error SOLL zur Verbesserung von Preloading und Caching verwendet werden.
11. Aktive und reservierte Ressourcen MÜSSEN gegenüber spekulativem Caching priorisiert werden.
12. Accelerator Memory MUSS in die systemweite Resource Economy integriert sein.
13. Provider-Wechsel MÜSSEN den Execution Contract weiterhin erfüllen.
14. Alternative Modellvarianten DÜRFEN nur bei erfüllten Hard Requirements verwendet werden.
15. Multi-Model Workloads SOLLEN gemeinsam planbar sein.
16. Scheduling MUSS Priority, Fairness und Deadline berücksichtigen können.
17. Cache-Zustände DÜRFEN Determinism Requirements nicht verletzen.
18. Zustandsbehaftete AI-Daten MÜSSEN explizit isoliert werden.
19. Model Scheduling und Caching MÜSSEN mit Resource Tracing und Decision Tracing korrelierbar sein.
20. Der Ausfall des Model Cache DARF die grundsätzliche AI-Ausführung nicht verhindern, sofern das benötigte Modell anderweitig geladen werden kann.

## Konsequenzen

### Positive Konsequenzen

- häufig verwendete Modelle starten deutlich schneller,
- RAM, VRAM und NPU Memory werden systemweit koordiniert,
- mehrere Anwendungen können Modelle effizient gemeinsam nutzen,
- Predictive Preloading kann wahrgenommene AI-Latenzen reduzieren,
- Ressourcenknappheit kann durch kontrollierte Eviction behandelt werden,
- Model Scheduling wird Teil der allgemeinen NovaOS-Ressourcenökonomie.

### Negative Konsequenzen

- Residency und Eviction erhöhen die Runtime-Komplexität,
- Shared Models benötigen strikte Isolation zustandsbehafteter Daten,
- falsches Preloading kann Ressourcen und Energie verschwenden.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0002_Model_Registry`
- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0005_Adaptive_Caching_und_Prefetching`
- `ADR-ADAPTIVE-0006_Predictive_Preloading`
- `ADR-ADAPTIVE-0007_Adaptive_Memory_Management`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AI-MODEL-SCHEDULING-0001`
- `NPSPEC-AI-MODEL-CACHE-0001`
- `NPSPEC-AI-MODEL-RESIDENCY-0001`
- `NPSPEC-AI-MODEL-EVICTION-0001`
- `NPSPEC-AI-MODEL-PRELOADING-0001`
- `NPSPEC-AI-MODEL-ISOLATION-0001`

## Ergebnis

NovaOS behandelt AI-Modelle als gemeinsam verwaltete Systemressourcen:

```text
AI Requests
     ↓
Model Scheduler
     ↓
Registry + Contracts + Resources
     ↓
Resident Model / Load / Preload
     ↓
CPU / GPU / NPU
     ↓
Measure
     ↓
Cache / Evict / Adapt
```

Der zentrale Grundsatz lautet:

```text
Modelle werden nicht
pro Anwendung isoliert verwaltet.

NovaOS entscheidet systemweit,
welche Modelle geladen,
geteilt, vorgeladen oder
aus dem Speicher entfernt werden.

Caching verbessert die Leistung,
bleibt aber immer nachrangig
gegenüber tatsächlichen
Ausführungs- und Ressourcenanforderungen.
```