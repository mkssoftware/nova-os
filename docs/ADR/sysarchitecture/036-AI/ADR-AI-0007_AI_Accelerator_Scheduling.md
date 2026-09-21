# ADR-AI-0007 – AI Accelerator Scheduling

## Status

Angenommen

## Kategorie

AI / Compute / Accelerator / Scheduling

## Kontext

AI-Workloads können auf unterschiedlichen Compute Providern ausgeführt werden:

```text
CPU
SIMD
GPU
NPU
Integrated Accelerator
Dedicated AI Accelerator
Remote Accelerator
```

Diese unterscheiden sich erheblich hinsichtlich:

```text
Performance
Latency
Memory
Energy
Supported Operations
Model Compatibility
Availability
Load
Data Transfer Cost
```

Die AI Runtime darf deshalb nicht davon ausgehen, dass GPU oder NPU automatisch die beste Ausführungsplattform ist.

NovaOS benötigt eine gemeinsame Planung für AI-Accelerator-Ressourcen.

## Entscheidung

NovaOS integriert **AI Accelerator Scheduling** in die bestehende Unified Compute Runtime und das allgemeine Execution Planning.

```text
AI Request
    ↓
Model Selection
    ↓
Compatible Providers
    ↓
Hard Constraint Filtering
    ↓
Accelerator Scheduling
    ↓
CPU / GPU / NPU / Accelerator
```

Es entsteht kein vollständig separater AI-Hardware-Scheduler.

## Provider Compatibility

Die Model Registry beschreibt, auf welchen Providern ein Modell ausgeführt werden kann.

Beispiel:

```text
Model X
 ├── CPU
 ├── GPU
 └── NPU
```

Ein Provider wird nur berücksichtigt, wenn Modell, Runtime und Hardware kompatibel sind.

## Hard Constraints

Vor jeder Optimierung werden ungeeignete Provider ausgeschlossen.

Beispiele:

```text
Required Hardware
Determinism
Memory Requirement
Deadline
Data Sovereignty
Trust
Authorization
Provider Availability
Forced Provider
```

```text
Providers
    ↓
Hard Constraint Filtering
    ↓
Valid Providers
    ↓
Scheduling
```

## Scheduling-Faktoren

Zwischen gültigen Providern darf NovaOS anhand mehrerer Faktoren entscheiden:

```text
Expected Runtime
Queue Time
Model Load Time
Transfer Time
Memory Pressure
Energy
Thermal State
Current Load
Model Residency
Deadline Risk
```

Entscheidend ist die erwartete End-to-End-Ausführung und nicht nur die reine Rechenleistung.

## Accelerator Memory

VRAM, NPU Memory und andere Accelerator-Speicher werden als systemweite Ressourcen verwaltet.

```text
Model Weights
Activations
KV Cache
Execution Buffers
Shared Models
```

Der Scheduler muss aktuelle und reservierte Speicherbelegung berücksichtigen.

## Model Residency

Bereits geladene Modelle beeinflussen die Scheduling-Entscheidung.

```text
GPU A
Model already resident

GPU B
Model must be loaded
```

GPU A kann trotz geringerer theoretischer Rechenleistung die geringere Gesamtlatenz besitzen.

Model Residency ist jedoch nur ein Optimierungsfaktor und kein Hard Constraint.

## Queue Awareness

Der Scheduler berücksichtigt aktuelle Accelerator-Warteschlangen.

```text
Fast GPU
Long Queue

Slower NPU
Idle
```

Die theoretisch schnellste Hardware ist nicht automatisch der schnellste Execution Path.

## Data Movement

Datenbewegung wird als Teil der Ausführungskosten betrachtet.

```text
Input Data
    ↓
Transfer
    ↓
Accelerator
    ↓
Compute
    ↓
Result Transfer
```

Große Transfers können den Vorteil eines Accelerators vollständig aufheben.

## Locality

Wenn Daten bereits in einem bestimmten Speicherbereich vorhanden sind, soll diese Locality berücksichtigt werden.

Beispiele:

```text
CPU Memory
GPU VRAM
Shared Memory
NPU Memory
Remote Node Memory
```

Unnötige Kopien sollen nach Möglichkeit vermieden werden.

## Multi-Accelerator Scheduling

NovaOS darf mehrere Accelerators gleichzeitig verwalten.

```text
GPU 0
GPU 1
NPU 0
NPU 1
```

Workloads können abhängig von Modell und Execution Contract verteilt werden.

Eine Aufteilung eines einzelnen Modells über mehrere Accelerators erfolgt nur, wenn Runtime und Modell dies unterstützen.

## Concurrent AI Workloads

Mehrere AI-Ausführungen können gleichzeitig Accelerator-Ressourcen anfordern.

Der Scheduler berücksichtigt:

```text
Priority
Fairness
Deadline
Resource Reservation
Interactive Workload
Background Workload
```

Ein großer Hintergrund-Workload darf interaktive AI-Funktionen nicht unbegrenzt blockieren.

## Resource Reservation

Execution Contracts dürfen Accelerator-Ressourcen reservieren.

Beispiele:

```text
GPU Memory
NPU Memory
Compute Time
Execution Slot
Deadline Capacity
```

Reservierte Ressourcen besitzen Vorrang vor spekulativem Preloading oder Model Caching.

## Preemption

Wenn Hardware und Runtime dies sicher unterstützen, dürfen AI-Workloads unterbrochen oder verdrängt werden.

```text
Background AI
     ↓
High Priority Request
     ↓
Pause / Preempt
     ↓
Execute Priority Workload
```

Preemption ist keine universelle Voraussetzung und muss providerabhängig beschrieben werden.

## Thermal und Energy Awareness

Accelerator Scheduling berücksichtigt Energie- und Thermalzustand.

```text
High Thermal Pressure
       ↓
Reduce Accelerator Load
       ↓
Alternative Provider
```

Safety Limits sind Hard Constraints.

Energieeffizienz bleibt innerhalb dieser Grenzen ein Optimierungsziel.

## Shared Models

Model Sharing wird in die Scheduling-Entscheidung integriert.

```text
Shared Model resident on GPU
          ↓
New Compatible Request
          ↓
Reuse Existing Instance
```

Dies kann Ladezeit und Speicherverbrauch reduzieren.

Private Execution States bleiben weiterhin isoliert.

## Dynamic Replanning

Ändert sich der Systemzustand, darf NovaOS neu planen.

Beispiele:

```text
Accelerator Failure
Memory Pressure
Thermal Limit
Deadline Risk
Provider Unavailable
Higher Priority Workload
```

```text
Current Plan
    ↓
State Change
    ↓
Reevaluate
    ↓
Continue / Replan
```

Ein Provider-Wechsel muss weiterhin alle Hard Constraints erfüllen.

## Remote Accelerators

Remote Accelerators werden als Compute Provider behandelt.

Zusätzlich müssen berücksichtigt werden:

```text
Network Latency
Transfer Cost
Remote Queue
Trust
Data Sovereignty
Failure Domain
```

Remote Scheduling darf keine LocalOnly- oder NoRemote-Anforderung umgehen.

## Adaptive Scheduling

NovaOS darf aus realen Ausführungen lernen.

```text
Predicted Runtime
Predicted Queue
Predicted Energy
Predicted Transfer
       ↓
Provider Selection
       ↓
Actual Result
       ↓
Prediction Error
       ↓
Model Correction
```

Dadurch können hardware- und workload-spezifische Entscheidungen verbessert werden.

## Deterministic Mode

Determinism Requirements können die Provider-Auswahl einschränken.

Falls eine deterministische Ausführung verlangt wird, dürfen nur Provider und Runtime-Modi verwendet werden, welche die erforderlichen Eigenschaften erfüllen.

```text
Required Determinism
        ↓
Compatible Providers Only
```

Adaptive Scheduling darf diese Auswahl nicht verändern.

## Failure Handling

Fällt ein Accelerator aus, darf NovaOS alternative Provider prüfen.

```text
GPU Failure
    ↓
Replan
    ↓
NPU / CPU / Other GPU
```

Ist kein contract-konformer Provider verfügbar:

```text
Execution → Unavailable
```

Hard Requirements dürfen für einen Fallback nicht stillschweigend abgeschwächt werden.

## Observability

AI Accelerator Scheduling muss nachvollziehbar sein.

Relevante Informationen sind:

```text
Execution ID
Model ID
Provider Candidates
Rejected Providers
Selected Provider
Queue Time
Load Time
Transfer Time
Execution Time
Memory Usage
Energy
Decision ID
Replanning
```

Diese Informationen fließen in Resource Tracing, Decision Tracing und adaptive Modelle ein.

## Normative Anforderungen

1. NovaOS MUSS AI Accelerator Scheduling in die Unified Compute Runtime integrieren.
2. AI-Hardware DARF nicht durch einen vollständig unabhängigen Scheduler verwaltet werden.
3. Model- und Provider-Kompatibilität MUSS vor der Planung geprüft werden.
4. Hard Constraints MÜSSEN vor Performance-Optimierung ausgewertet werden.
5. Scheduling MUSS End-to-End-Kosten statt ausschließlich Compute Performance berücksichtigen.
6. Accelerator Memory MUSS als systemweite Ressource verwaltet werden.
7. Model Residency SOLL bei der Provider-Auswahl berücksichtigt werden.
8. Queue State SOLL bei der Provider-Auswahl berücksichtigt werden.
9. Datenbewegung MUSS als Teil der Ausführungskosten berücksichtigt werden.
10. Data Locality SOLL unnötige Transfers reduzieren.
11. Mehrere Accelerators MÜSSEN gemeinsam planbar sein.
12. Scheduling MUSS Priority, Fairness und Deadlines berücksichtigen können.
13. Reservierte Ressourcen MÜSSEN Vorrang vor spekulativem AI-Caching und Preloading besitzen.
14. Preemption DARF nur verwendet werden, wenn Provider und Runtime sie sicher unterstützen.
15. Thermal Safety Limits MÜSSEN als Hard Constraints behandelt werden.
16. Energy Awareness SOLL bei der Accelerator-Auswahl berücksichtigt werden.
17. Shared Model Residency SOLL in Scheduling-Entscheidungen einfließen.
18. Änderungen des Provider-Zustands MÜSSEN Replanning ermöglichen.
19. Remote Accelerators MÜSSEN Trust-, Sovereignty- und Netzwerkconstraints berücksichtigen.
20. Adaptive Scheduling DARF ausschließlich innerhalb des gültigen Provider-Raums erfolgen.
21. Determinism Requirements MÜSSEN die Provider-Auswahl einschränken können.
22. Accelerator-Fallbacks DÜRFEN Hard Requirements nicht abschwächen.
23. Accelerator Scheduling MUSS mit Resource Tracing und Decision Tracing korrelierbar sein.

## Konsequenzen

### Positive Konsequenzen

- CPU, GPU, NPU und andere Accelerators werden einheitlich geplant,
- NovaOS kann reale End-to-End-Latenz statt theoretischer Hardwareleistung optimieren,
- VRAM und NPU Memory werden systemweit koordiniert,
- Model Residency und Sharing reduzieren Lade- und Transferkosten,
- mehrere AI-Workloads können fair auf verfügbare Hardware verteilt werden,
- adaptive Modelle können Scheduling anhand realer Hardwaremessungen verbessern.

### Negative Konsequenzen

- Scheduling benötigt aktuelle Provider-, Queue- und Speicherinformationen,
- unterschiedliche Accelerator-Runtimes benötigen geeignete Adapter,
- Multi-Accelerator- und Preemption-Unterstützung bleibt hardwareabhängig.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0002_Model_Registry`
- `ADR-AI-0003_Model_Scheduling_und_Caching`
- `ADR-AI-0004_Model_Sharing`
- `ADR-AI-0005_On_Device_AI_bevorzugen_wo_sinnvoll`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_Compute_Provider`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AI-ACCELERATOR-SCHEDULING-0001`
- `NPSPEC-AI-ACCELERATOR-MEMORY-0001`
- `NPSPEC-AI-ACCELERATOR-RESERVATION-0001`
- `NPSPEC-AI-ACCELERATOR-PREEMPTION-0001`
- `NPSPEC-AI-MULTI-ACCELERATOR-0001`
- `NPSPEC-AI-ACCELERATOR-REPLANNING-0001`

## Ergebnis

AI Accelerators werden nicht als Sonderhardware außerhalb der NovaOS-Ressourcenverwaltung behandelt:

```text
AI Execution
     ↓
Model Requirements
     ↓
Execution Contract
     ↓
Valid Compute Providers
     ↓
Accelerator Scheduling
     ↓
CPU / GPU / NPU / Remote
     ↓
Measure
     ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
NovaOS wählt nicht einfach
die schnellste GPU.

Es wählt den Provider,
der die konkrete AI-Ausführung
unter Berücksichtigung von
Ladezeit, Queue, Datenbewegung,
Speicher, Energie und Constraints
am sinnvollsten ausführen kann.
```