# ADR-SCHED-0009 – GPU, NPU und Accelerator Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / GPU / NPU / Accelerators / Heterogeneous Compute / Resource Economy

## Kontext

NovaOS unterstützt neben CPUs zunehmend spezialisierte Recheneinheiten:

- GPUs,
- NPUs,
- DSPs,
- FPGAs,
- Media Engines,
- Crypto Engines,
- weitere zukünftige Accelerators.

Diese Ressourcen unterscheiden sich grundlegend von klassischen CPU-Kernen. Sie besitzen eigene:

- Ausführungsmodelle,
- Speicherbereiche,
- Queues,
- Parallelitätsmodelle,
- Energie- und Thermal-Eigenschaften,
- Preemption-Fähigkeiten,
- Isolationseigenschaften.

NovaOS soll Accelerator-Ausführung dennoch nicht als vollständig getrennte Welt behandeln.

Insbesondere müssen Execution Contracts, Resource Economy, Structured Concurrency, Data Sovereignty, Trust, Locality und automatische Algorithmusauswahl auch für Accelerators gelten.

---

## Entscheidung

NovaOS integriert GPUs, NPUs und andere Accelerators in eine **gemeinsame systemweite Scheduling-Architektur**, ohne sie wie CPUs zu behandeln.

```text
Execution Request
       ↓
Execution Contract
       ↓
Eligible Providers
       ├── CPU
       ├── GPU
       ├── NPU
       └── Accelerator
       ↓
Provider Selection
       ↓
Accelerator Scheduler
       ↓
Device / Queue / Execution Context
```

Der CPU-Scheduler und Accelerator-Scheduler teilen gemeinsame systemweite Contracts und Ressourcenmodelle, verwenden aber gerätespezifische Scheduling-Mechanismen.

---

## Grundprinzip

```text
One execution model.

Multiple compute providers.

Shared contracts and accounting.

Provider-specific scheduling mechanisms.
```

---

## Accelerator als Compute Provider

Ein Accelerator wird als Compute Provider mit expliziten Capabilities beschrieben.

Dazu können gehören:

```text
Supported Operations
Semantic Types
Data Formats
Memory Model
Execution Modes
Determinism Properties
Preemption Support
Realtime Properties
Performance Characteristics
Energy Characteristics
Topology
```

NovaOS darf nicht voraussetzen, dass jeder Accelerator dieselben Fähigkeiten besitzt.

---

## Provider Selection

Anwendungen sollen normalerweise eine semantische Operation anfordern, nicht ein konkretes Gerät.

Beispiel:

```text
Operation
   ↓
Execution Contract
   ↓
CPU / GPU / NPU Providers
   ↓
NovaOS Selection
```

NovaOS kann den geeigneten Provider anhand von:

- Semantik,
- Datenmenge,
- Datenort,
- Latenz,
- Deadline,
- Ressourcenbudget,
- Energie,
- Thermal State,
- Trust,
- Data Sovereignty,
- Determinismus

auswählen.

---

## Expliziter Provider

Entwickler dürfen einen Provider explizit bevorzugen oder erzwingen.

Beispiel:

```text
PreferredProvider = GPU
```

oder:

```text
RequiredProvider = NPU
```

Ein expliziter Provider darf Hard Constraints jedoch nicht umgehen.

Ist ein erzwungener Provider unzulässig oder nicht verfügbar, muss die Operation fehlschlagen beziehungsweise entsprechend ihrem Contract behandelt werden.

---

## Trennung von Provider Selection und Scheduling

Provider Selection und internes Accelerator Scheduling sind getrennte Entscheidungen.

```text
Which provider?
      ↓
Provider Selection

When / where on provider?
      ↓
Provider Scheduler
```

Dadurch kann NovaOS Provider-Policies ändern, ohne gerätespezifische Schedulermechanismen in die allgemeine Architektur zu ziehen.

---

## Accelerator Scheduling Domain

Ein Accelerator kann eine oder mehrere Scheduling Domains besitzen.

Beispiele:

```text
GPU
 ├── Compute Queue
 ├── Graphics Queue
 └── Copy Queue

NPU
 ├── Inference Queue
 └── Compute Queue
```

Die konkrete Struktur wird durch den Provider beschrieben.

---

## Resource Economy

Accelerator-Nutzung ist Bestandteil der systemweiten Resource Economy.

Accounting muss mindestens auf verursachende:

```text
Execution Domain
      ↓
Execution Scope
      ↓
Task / Operation
```

zurückführbar sein.

Relevante Ressourcen können umfassen:

- Compute Time,
- Device Memory,
- Memory Bandwidth,
- Queue Capacity,
- Transfer Bandwidth,
- Energy.

---

## Reservations und Budgets

Execution Contracts können Accelerator-Ressourcen reservieren oder begrenzen.

Dies ist insbesondere relevant für:

- Realtime,
- interaktive Grafik,
- Media Processing,
- latenzkritische AI-Inferenz.

Ein Workload darf durch das Erzeugen vieler Queues oder Commands seine Resource Budgets nicht umgehen.

---

## Structured Concurrency

Accelerator Work bleibt Bestandteil der Structured-Concurrency-Hierarchie.

```text
Execution Scope
      ↓
Task
      ↓
Accelerator Operation
```

Eine Accelerator Operation besitzt weiterhin:

- Owner,
- Cancellation Context,
- Deadline,
- Resource Attribution,
- Execution Contract.

Das Auslagern auf einen Accelerator erzeugt keine ownerlose Hintergrundarbeit.

---

## Asynchrone Ausführung

Accelerator-Ausführung ist grundsätzlich asynchron integrierbar.

```text
Submit
  ↓
Execute on Accelerator
  ↓
Completion
  ↓
Resume Awaiting Task
```

Async/Await soll Accelerator Completion verwenden können, ohne einen CPU-Thread blockieren zu müssen.

---

## Cancellation

Cancellation einer Accelerator Operation kann abhängig vom Gerät unterschiedlich umgesetzt werden.

Mögliche Fälle sind:

```text
Preempt
Cancel queued work
Stop at execution boundary
Logical cancellation
Wait for physical completion
```

NovaOS muss zwischen logischer und physischer Cancellation unterscheiden.

---

## Preemption

Nicht jeder Accelerator unterstützt dieselbe Preemption-Granularität.

Provider müssen ihre Fähigkeiten explizit beschreiben.

Der Scheduler darf keine feingranulare Preemption-Garantie annehmen, wenn die Hardware sie nicht bereitstellt.

---

## Deadlines

Accelerator Operations können die systemweite Deadline Propagation verwenden.

Der Provider Scheduler muss Deadlines berücksichtigen können, sofern seine Fähigkeiten dies erlauben.

Eine Deadline allein erzeugt keine Realtime-Garantie.

---

## Realtime

Accelerator Realtime Scheduling ist nur zulässig, wenn der Provider ausreichend vorhersehbare Eigenschaften bereitstellt.

Hard-Realtime-Contracts können insbesondere erfordern:

- reservierte Queue Capacity,
- reservierte Compute Capacity,
- kontrollierte Memory Resources,
- begrenzte Preemption-Latenz,
- vorhersehbare Transfers.

Nicht beweisbare Eigenschaften dürfen nicht als Hard-Realtime-Garantie behandelt werden.

---

## Memory Locality

Accelerator Scheduling muss den Ort der Daten berücksichtigen.

```text
Data Location
      +
Compute Provider
      +
Transfer Cost
      ↓
Placement Decision
```

Ein theoretisch schnellerer Accelerator kann ungeeignet sein, wenn die notwendigen Datentransfers den Vorteil übersteigen.

---

## Zero-Copy

Wo Hardware und Isolation dies erlauben, soll Accelerator-Ausführung Zero-Copy beziehungsweise Shared-Memory-Mechanismen verwenden können.

Dazu gehören beispielsweise:

- Shared Virtual Memory,
- DMA Mapping,
- Unified Memory,
- Buffer Ownership Transfer.

Zero-Copy bleibt den Security-, Isolation- und Data-Sovereignty-Regeln untergeordnet.

---

## Topology Awareness

Accelerators werden in den Hardware System Graph integriert.

Der Scheduler kann dadurch Beziehungen berücksichtigen zwischen:

```text
CPU
 ↕
NUMA Node
 ↕
Memory
 ↕
PCIe / Fabric
 ↕
Accelerator
```

Provider Selection und Scheduling sollen Daten- und Device-Locality berücksichtigen können.

---

## GPU

GPU Scheduling muss unterschiedliche Workload-Arten unterstützen können, insbesondere:

- Graphics,
- Compute,
- Media,
- General-Purpose Parallel Work.

NovaOS darf nicht voraussetzen, dass Graphics und Compute vollständig getrennte physische Ressourcen besitzen.

Interaktive Darstellung muss vor unkontrollierter Compute-Sättigung geschützt werden können.

---

## NPU

NPUs werden als spezialisierte Compute Provider behandelt.

Sie können insbesondere für:

- Inference,
- Matrixoperationen,
- Tensoroperationen,
- andere unterstützte AI-Workloads

verwendet werden.

Eine NPU ist keine Voraussetzung für NovaOS-Korrektheit oder grundlegende Systemfunktionen.

---

## Weitere Accelerators

Die Architektur darf nicht auf CPU, GPU und NPU beschränkt sein.

Neue Provider sollen über dieselben grundlegenden Contracts integrierbar sein.

Beispiele:

```text
DSP
FPGA
Video Engine
Crypto Accelerator
Scientific Accelerator
Future Compute Provider
```

---

## Algorithmusauswahl

Provider- und Algorithmusauswahl können gemeinsam erfolgen.

Beispielsweise:

```text
Semantic Operation
      ↓
Compatible Algorithms
      ↓
Compatible Providers
      ↓
Cost Evaluation
      ↓
Algorithm + Provider
```

NovaOS kann damit beispielsweise unterschiedliche Implementierungen für CPU, GPU oder NPU auswählen.

Explizite Algorithmus-Overrides bleiben möglich.

---

## Data Sovereignty

Accelerator Scheduling muss Data-Sovereignty-Constraints berücksichtigen.

Dies ist insbesondere relevant für:

- diskrete Geräte,
- gemeinsam genutzte Accelerators,
- virtuelle Accelerators,
- entfernte Compute Provider.

Daten dürfen nicht allein aus Performancegründen in eine unzulässige Execution Location übertragen werden.

---

## Trust und Isolation

Accelerator Provider unterliegen Nova.Trust und dem Capability-Modell.

Execution Domains erhalten nur explizit autorisierten Zugriff.

DMA- und IOMMU-Isolation müssen mit Accelerator Scheduling koordiniert werden.

Ein vertrauenswürdiger Accelerator erzeugt nicht automatisch zusätzliche Authority.

---

## Multi-Tenant Isolation

Mehrere Execution Domains dürfen Accelerators gemeinsam verwenden, wenn der Provider ausreichende Isolation bereitstellt.

Fehlt eine sichere Isolation, muss NovaOS:

- exklusive Zuweisung,
- stärkere Isolation,
- einen anderen Provider,
- oder Ablehnung

verwenden.

---

## Energy Awareness

Provider Selection darf Energieeffizienz berücksichtigen.

Eine NPU kann beispielsweise für bestimmte Operationen energieeffizienter sein als CPU oder GPU.

Energie bleibt ein Soft Objective, sofern der Execution Contract nichts Stärkeres verlangt.

---

## Thermal Awareness

Accelerator Scheduling muss thermische Einschränkungen berücksichtigen können.

Thermal Pressure kann:

- Compute Capacity reduzieren,
- Provider Selection verändern,
- Parallelität reduzieren,
- optionale Arbeit degradieren.

Thermal Safety besitzt Vorrang vor Performance- und Realtime-Garantien.

---

## Graceful Degradation

Ist der bevorzugte Accelerator nicht verfügbar, kann NovaOS auf einen anderen kompatiblen Provider ausweichen.

```text
Preferred NPU
      ↓ unavailable
GPU
      ↓ unavailable
CPU
```

Ein solcher Fallback ist nur zulässig, wenn:

- Semantik,
- Deadline,
- Determinismus,
- Trust,
- Data Sovereignty,
- Resource Contract

weiterhin erfüllt werden.

Required Provider dürfen nicht still ersetzt werden.

---

## Deterministic Mode

Accelerator-Ausführung muss ihre Determinism Properties offenlegen.

Ein Deterministic Execution Contract darf nur Provider und Algorithmen verwenden, die die verlangte Determinismusklasse erfüllen.

NovaOS darf nicht voraussetzen, dass parallele GPU- oder NPU-Ausführung automatisch deterministisch ist.

---

## Hotplug und Device Failure

Accelerators können:

- hinzugefügt,
- entfernt,
- zurückgesetzt,
- fehlerhaft

werden.

Provider Scheduler müssen mit dem Device Lifecycle integriert sein.

Bei Verlust eines Accelerators werden betroffene Operations entsprechend ihrem Contract:

```text
Fail
Retry
Replan
Fallback
Restart
```

behandelt.

Nicht-idempotente Arbeit darf nicht blind erneut ausgeführt werden.

---

## Introspection

Accelerator Scheduling muss systemweit introspektierbar sein.

Mindestens relevant sind:

```text
Execution Domain
Operation
Provider
Device
Queue
Resource Usage
Deadline
Data Location
Provider Selection Reason
Fallback State
```

Damit müssen insbesondere unerwartete CPU/GPU/NPU-Auswahl und Ressourcenengpässe nachvollziehbar sein.

---

## Adaptive Optimierung

NovaOS darf lernen, welcher Provider für bestimmte Workloads geeignet ist.

```text
Predicted:
    GPU fastest

Actual:
    transfer overhead too high

Prediction Error
      ↓
Provider Model Correction
```

Adaptive Modelle dürfen:

- Performance,
- Energie,
- Transferkosten,
- Queueing-Latenz

optimieren.

Sie dürfen Hard Constraints nicht verändern.

---

## Fail-Safe Defaults

Ist unklar, ob ein Provider:

- eine erforderliche Operation unterstützt,
- ausreichende Isolation besitzt,
- einen Determinismus-Contract erfüllt,
- Data-Sovereignty-Regeln einhalten kann,

gilt er für diesen Contract als nicht geeignet.

Unbekannte Provider-Eigenschaften dürfen nicht optimistisch als Garantie interpretiert werden.

---

## Normative Anforderungen

1. NovaOS MUSS GPU-, NPU- und weitere Accelerator-Ressourcen in eine gemeinsame systemweite Scheduling-Architektur integrieren können.
2. Accelerators MÜSSEN als Compute Provider mit expliziten Capabilities beschrieben werden.
3. NovaOS DARF Accelerators NICHT wie homogene CPU-Kerne behandeln.
4. Provider Selection und providerinternes Scheduling MÜSSEN getrennte Entscheidungen bleiben.
5. Anwendungen MÜSSEN semantische Operationen ohne konkrete Providerwahl anfordern können.
6. NovaOS MUSS geeignete Provider automatisch auswählen können.
7. Preferred und Required Provider MÜSSEN unterscheidbar sein.
8. Required Provider DÜRFEN NICHT still durch andere Provider ersetzt werden.
9. Hard Execution Constraints MÜSSEN Vorrang vor Provider-Optimierung besitzen.
10. Accelerator-Nutzung MUSS der systemweiten Resource Economy zurechenbar sein.
11. Accelerator Budgets und Reservations MÜSSEN ausdrückbar sein.
12. Accelerator Operations MÜSSEN in Structured Concurrency integrierbar sein.
13. Asynchrone Accelerator Completion MUSS ohne dauerhaft blockierenden CPU-Thread integrierbar sein.
14. Logische und physische Cancellation MÜSSEN unterscheidbar sein.
15. Provider MÜSSEN ihre Preemption-Fähigkeiten beschreiben können.
16. Deadline Propagation MUSS auf Accelerator Operations anwendbar sein.
17. Hard-Realtime-Garantien DÜRFEN nur bei ausreichend kontrollierbaren Provider-Eigenschaften angeboten werden.
18. Provider Selection MUSS Datenort und Transferkosten berücksichtigen können.
19. Zero-Copy- und Shared-Memory-Mechanismen MÜSSEN bei geeigneter Hardware integrierbar sein.
20. Accelerators MÜSSEN in den Hardware System Graph integrierbar sein.
21. GPU Compute DARF interaktive Graphics Workloads NICHT unkontrolliert verdrängen.
22. Die Architektur DARF NICHT auf CPU, GPU und NPU als einzige Compute Provider beschränkt sein.
23. Algorithmus- und Provider-Auswahl MÜSSEN gemeinsam optimierbar sein.
24. Accelerator Scheduling MUSS Data-Sovereignty-, Trust- und Capability-Constraints einhalten.
25. DMA- und IOMMU-Isolation MÜSSEN mit Accelerator-Nutzung integrierbar sein.
26. Energy- und Thermal-Aware Policies MÜSSEN Accelerator Scheduling beeinflussen können.
27. Provider-Fallback MUSS explizit contractabhängig sein.
28. Deterministic Mode MUSS ungeeignete Accelerator Provider beziehungsweise Ausführungsmodi ausschließen können.
29. Accelerator Hotplug, Reset und Failure MÜSSEN kontrolliert behandelbar und Scheduling-Entscheidungen introspektierbar sein.
30. Adaptive Provider Selection DARF Hard Constraints NICHT verändern und Systemkorrektheit MUSS ohne KI gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches Scheduling-Modell über CPU, GPU, NPU und zukünftige Accelerators,
- automatische Providerwahl anhand tatsächlicher Anforderungen,
- gemeinsame Resource Economy und Structured Concurrency,
- bessere Daten- und Device-Locality,
- systemweite Energie-, Thermal-, Trust- und Sovereignty-Integration,
- Erweiterbarkeit für zukünftige Compute Hardware.

### Negative Konsequenzen

- Provider Selection wird zu einer zusätzlichen Scheduling-Ebene,
- unterschiedliche Accelerator-Preemption- und Memory-Modelle erhöhen die Komplexität,
- Realtime-Garantien sind nicht auf jedem Accelerator möglich,
- Kostenmodelle müssen Compute- und Transferkosten gemeinsam berücksichtigen.

---

## Verworfene Alternativen

### Accelerators vollständig außerhalb des System-Schedulers

Verworfen.

Resource Accounting, Deadlines, Structured Concurrency und systemweite Policies würden dadurch fragmentiert.

### GPU, NPU und Accelerators wie zusätzliche CPUs behandeln

Verworfen.

Ihre Execution-, Memory-, Queue- und Preemption-Modelle unterscheiden sich grundlegend.

### Anwendungen wählen immer selbst das konkrete Gerät

Verworfen.

Dies verhindert systemweite Optimierung und erschwert Portabilität auf zukünftige Hardware.

### NovaOS wählt immer automatisch ohne Override

Verworfen.

Entwickler müssen für spezialisierte Anforderungen einen Provider bevorzugen oder erzwingen können.

### Immer den theoretisch schnellsten Accelerator wählen

Verworfen.

Transferkosten, Queueing, Energie, Thermal State, Trust und Data Sovereignty können einen anderen Provider sinnvoller machen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0005_Cache_Aware_Scheduling`
- `ADR-SCHED-0006_Energy_Aware_Scheduling`
- `ADR-SCHED-0007_Thermal_Aware_Scheduling`
- `ADR-SCHED-0008_Heterogeneous_CPU_Scheduling`
- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0005_Async_Await_als_native_Abstraktion`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-ACCELERATOR-0001`
- `NPSPEC-SCHED-PROVIDER-0001`
- `NPSPEC-SCHED-GPU-0001`
- `NPSPEC-SCHED-NPU-0001`
- `NPSPEC-SCHED-ACCELERATOR-QUEUE-0001`
- `NPSPEC-SCHED-ACCELERATOR-MEMORY-0001`
- `NPSPEC-SCHED-ACCELERATOR-RESOURCE-0001`
- `NPSPEC-SCHED-ACCELERATOR-REALTIME-0001`
- `NPSPEC-SCHED-ACCELERATOR-ISOLATION-0001`
- `NPSPEC-SCHED-ACCELERATOR-TEST-0001`

---

## Ergebnis

NovaOS erweitert Scheduling von einem reinen CPU-Konzept zu einer systemweiten Compute-Architektur:

```text
Semantic Operation
        ↓
Execution Contract
        ↓
Compatible Algorithms
        ↓
Compatible Providers
        │
   ┌────┼────┬─────────┐
   ↓    ↓    ↓         ↓
  CPU  GPU  NPU   Accelerator
        │
        ↓
Provider-specific Scheduling
        ↓
Execution
```

Damit bleibt die Ausführung für Anwendungen semantisch einheitlich, während NovaOS unterschiedliche Hardwaremechanismen gezielt nutzen kann.

Die zentrale Architekturregel lautet:

```text
Choose by capability,
not by device name.

Schedule by contract,
not by hardware habit.

Move compute only when
the total cost makes sense.
```