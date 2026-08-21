# NPSPEC-COMPUTE-RUNTIME-0006 — NPU Runtime

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS NPU Runtime  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0006`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0005`  
**Kurzname:** NPU Runtime  

---

## 1. Zweck

Diese Spezifikation definiert die **NPU Runtime** der NovaOS Compute Runtime.

Die NPU Runtime stellt eine einheitliche, hardwareunabhängige Abstraktionsschicht für Neural Processing Units und vergleichbare AI-Beschleuniger bereit.

Sie kapselt insbesondere:

- NPU Device Discovery,
- NPU Capability Discovery,
- NPU Contexts,
- NPU Queues,
- AI-Workload-Submission,
- Tensor Operations,
- Tensor Memory,
- Model Execution,
- Graph Execution,
- Operator Dispatch,
- Quantization,
- Precision Selection,
- Compilation,
- Graph Optimization,
- Memory Planning,
- Synchronisation,
- Scheduling,
- CPU/GPU/NPU-Kooperation,
- Multi-NPU,
- Power Management,
- Thermal Management,
- Device Recovery,
- Profiling und Diagnose.

Die öffentliche NovaOS Compute API DARF nicht fest an einen bestimmten NPU-Hersteller, eine bestimmte AI-Beschleunigerarchitektur oder eine proprietäre Runtime gekoppelt sein.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Eine NPU wird in NovaOS als spezialisierte Compute-Einheit innerhalb derselben Compute Runtime behandelt und nicht als isolierte AI-Sonderplattform.

Anwendungen SOLLEN AI-Workloads beschreiben können, ohne die konkrete NPU-Architektur kennen zu müssen.

Die Runtime entscheidet, ob ein Workload auf:

```text
CPU
GPU
NPU
```

oder einer Kombination daraus ausgeführt wird.

---

## 3. Ziele

Die NPU Runtime MUSS folgende Ziele verfolgen:

1. herstellerunabhängige NPU-Ausführung,
2. einheitliche Tensor- und Graph-Semantik,
3. Unterstützung unterschiedlicher NPU-Architekturen,
4. Unterstützung integrierter und diskreter AI-Beschleuniger,
5. effiziente Inferenz,
6. Unterstützung zukünftiger Training-Workloads,
7. geringe Dispatch-Latenz,
8. geringe Datenkopierkosten,
9. automatische Precision-Auswahl,
10. Quantization-Unterstützung,
11. Graph-Optimierung,
12. Memory Planning,
13. CPU/GPU/NPU-Kooperation,
14. Power-Aware Scheduling,
15. Thermal-Aware Scheduling,
16. sichere Prozess- und Context-Isolation,
17. robuste Device-Lost-Behandlung,
18. Erweiterbarkeit für zukünftige AI-Hardware.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- ein vollständiges Machine-Learning-Framework,
- konkrete neuronale Netzwerkarchitekturen,
- ein bestimmtes Modellformat,
- eine bestimmte AI-Programmiersprache,
- konkrete NPU-Instruktionssätze,
- konkrete Herstellertreiber,
- vollständige Training-Frameworks.

Sie definiert die NPU-Ausführungsschicht der NovaOS Compute Runtime.

---

## 5. Architektur

Die grundlegende Architektur lautet:

```text
Application
    |
    v
Nova AI / Compute API
    |
    v
Compute Runtime
    |
    v
NPU Runtime
    |
    +-- NPU Device Manager
    |
    +-- NPU Context Manager
    |
    +-- NPU Graph Compiler
    |
    +-- NPU Operator Manager
    |
    +-- NPU Tensor Manager
    |
    +-- NPU Memory Manager
    |
    +-- NPU Queue Manager
    |
    +-- NPU Scheduler Adapter
    |
    +-- NPU Synchronization
    |
    +-- NPU Recovery
    |
    v
NPU Backend
    |
    v
NPU Driver / Hardware
```

---

## 6. NPU Device

Jede NPU MUSS als Compute Device innerhalb des allgemeinen Compute Device Models repräsentiert werden.

Mindestens folgende Eigenschaften SOLLEN verfügbar sein:

```text
DeviceID
VendorID
HardwareDeviceID
DeviceName
Architecture
ComputeCapacity
TensorUnits
MatrixUnits
SupportedPrecisions
SupportedOperators
MemoryCapacity
MemoryType
MemoryBandwidth
MaximumTensorRank
MaximumTensorSize
MaximumGraphSize
QueueCapabilities
QuantizationCapabilities
SparsityCapabilities
DynamicShapeSupport
PreemptionSupport
PowerCharacteristics
HealthState
ThermalState
```

---

## 7. NPU-Klassen

Die Runtime SOLL mindestens folgende NPU-Klassen unterscheiden können:

```text
INTEGRATED_NPU
DISCRETE_NPU
SOC_AI_ACCELERATOR
VIRTUAL_NPU
EXTERNAL_NPU
```

Optional:

```text
REMOTE_NPU
```

---

## 8. Integrated NPU

Eine integrierte NPU kann physischen Speicher mit CPU oder GPU teilen.

Die Runtime SOLL in diesem Fall:

- Zero-Copy bevorzugen,
- Shared Memory verwenden,
- unnötige Tensor-Kopien vermeiden,
- gemeinsame Memory Domains erkennen.

---

## 9. Discrete NPU

Eine diskrete NPU kann eigenen lokalen Speicher besitzen.

Die Runtime MUSS in diesem Fall Transfers zwischen:

```text
HOST RAM
NPU MEMORY
```

kontrollieren können.

---

## 10. SoC AI Accelerator

Ein SoC kann mehrere spezialisierte AI-Einheiten enthalten.

Beispiele:

```text
Tensor Engine
Matrix Engine
Neural Engine
Vision Accelerator
DSP
```

Die Runtime DARF diese als:

- einzelne Compute Devices

oder:

- Subdevices einer logischen NPU

darstellen.

---

## 11. NPU Capabilities

Eine NPU MUSS ihre Fähigkeiten explizit deklarieren.

Mindestens relevant sind:

```text
FP32
FP16
BF16

FP8

INT32
INT16
INT8
INT4

UINT8
UINT4

MATRIX
TENSOR

QUANTIZATION
DEQUANTIZATION

SPARSITY
STRUCTURED_SPARSITY

DYNAMIC_SHAPES
STATIC_SHAPES

ASYNC_EXECUTION
PREEMPTION

SHARED_MEMORY
UNIFIED_MEMORY
ZERO_COPY
```

Nicht jede NPU MUSS alle Fähigkeiten unterstützen.

---

## 12. Precision Capability

Die Runtime MUSS für jede NPU die unterstützten numerischen Datentypen abfragen können.

Beispiel:

```text
FP32 = YES
FP16 = YES
BF16 = YES
FP8  = NO
INT8 = YES
INT4 = YES
```

---

## 13. Native Precision

Die Runtime SOLL unterscheiden zwischen:

```text
SUPPORTED
NATIVE
EMULATED
```

Beispiel:

```text
FP32 = SUPPORTED / EMULATED
FP16 = SUPPORTED / NATIVE
INT8 = SUPPORTED / NATIVE
```

---

## 14. Performance Capability

Eine NPU SOLL Leistungsinformationen für unterstützte Datentypen bereitstellen können.

Beispiel:

```text
FP16Throughput
INT8Throughput
INT4Throughput
MemoryBandwidth
```

Diese Werte DÜRFEN Schätzwerte sein.

---

## 15. NPU Context

Ein NPU Context MUSS in das allgemeine Compute Context Model integriert sein.

Er MUSS mindestens zugeordnet sein zu:

```text
OwnerProcess
SecurityDomain
AllowedDevices
MemoryBudget
Priority
PowerPolicy
```

---

## 16. Context Isolation

NPU Contexts unterschiedlicher Prozesse MÜSSEN voneinander isoliert sein.

Ein Prozess DARF nicht:

- fremde Tensoren lesen,
- fremde Modelle manipulieren,
- fremde NPU Queues verändern,
- fremde Graphen ausführen,
- fremden NPU Memory lesen.

---

## 17. NPU Queue

Die NPU Runtime MUSS mindestens eine Ausführungsqueue unterstützen.

Beispiel:

```text
NPU_COMPUTE_QUEUE
```

Optional:

```text
NPU_TRANSFER_QUEUE
NPU_HIGH_PRIORITY_QUEUE
NPU_BACKGROUND_QUEUE
```

---

## 18. Queue Ordering

NPU Queues SOLLEN mindestens:

```text
IN_ORDER
```

unterstützen.

Falls Hardware dies ermöglicht, DARF zusätzlich:

```text
OUT_OF_ORDER
```

unterstützt werden.

---

## 19. Asynchrone Ausführung

NPU Workloads SOLLEN asynchron eingereicht werden können.

Beispiel:

```text
Queue.Submit(GraphExecution)
```

Die Anwendung SOLL anschließend weiterarbeiten können.

---

## 20. Submission Result

Eine Submission SOLL ein Synchronisationsobjekt zurückgeben können.

Beispiele:

```text
Event
Fence
Timeline Value
Job Handle
```

---

## 21. Tensor Object

Tensoren MÜSSEN auf dem Object Model aus `NPSPEC-COMPUTE-RUNTIME-0004` aufbauen.

Ein Tensor SOLL mindestens besitzen:

```text
ElementType
Rank
Shape
Strides
Layout
Usage
MemoryDomain
Quantization
```

---

## 22. Tensor Rank

Die Runtime MUSS mehrdimensionale Tensoren unterstützen.

Beispiel:

```text
Rank = 4

Shape =
[
    Batch,
    Channels,
    Height,
    Width
]
```

---

## 23. Tensor Shape

Tensor Shapes können:

```text
STATIC
DYNAMIC
PARTIALLY_DYNAMIC
```

sein.

---

## 24. Static Shape

Bei statischen Shapes sind alle Dimensionen vor der Ausführung bekannt.

Dies SOLL bevorzugt optimiert werden.

---

## 25. Dynamic Shape

Bei dynamischen Shapes werden eine oder mehrere Dimensionen erst zur Laufzeit bestimmt.

---

## 26. Dynamic Shape Capability

Nicht jede NPU MUSS Dynamic Shapes unterstützen.

Falls eine NPU dies nicht unterstützt, DARF die Runtime:

- spezialisierte Graphvarianten erzeugen,
- Shapes begrenzen,
- den betroffenen Teil auf GPU oder CPU ausführen.

---

## 27. Tensor Layout

Die Runtime MUSS unterschiedliche Tensor Layouts beschreiben können.

Beispiele:

```text
NCHW
NHWC
CHWN
NC
CN
BLOCKED
DEVICE_NATIVE
```

---

## 28. Device Native Layout

Die Runtime DARF Tensoren intern in ein NPU-spezifisches Layout transformieren.

Die öffentliche logische Tensorsemantik MUSS erhalten bleiben.

---

## 29. Layout Conversion

Layout Conversion SOLL automatisch durchgeführt werden können.

Beispiel:

```text
NCHW
 |
 v
NHWC
```

---

## 30. Layout Conversion Elimination

Der Graph Optimizer SOLL unnötige Layout Conversions entfernen können.

---

## 31. Tensor Memory

Tensor Memory MUSS dem allgemeinen Compute Memory Model folgen.

Mögliche Memory Domains:

```text
HOST
NPU
SHARED
UNIFIED
```

---

## 32. NPU-local Memory

Eine NPU mit eigenem Speicher SOLL diesen als eigene Memory Domain darstellen.

Beispiel:

```text
NPU0_MEMORY
```

---

## 33. Tensor Residency

Die Runtime MUSS verfolgen können, wo ein Tensor aktuell resident ist.

Beispiel:

```text
Tensor A -> HOST
Tensor B -> NPU0
Tensor C -> GPU0
```

---

## 34. Tensor Migration

Tensoren DÜRFEN zwischen Compute Devices migriert werden.

Beispiel:

```text
CPU
 |
 v
NPU
 |
 v
GPU
```

---

## 35. Zero-Copy Tensor

Wenn mehrere Devices dieselbe physische Memory Domain verwenden, SOLL die Runtime Zero-Copy verwenden können.

---

## 36. Tensor Views

Mehrere Tensor Views DÜRFEN dieselbe zugrunde liegende Resource verwenden.

Beispiel:

```text
Buffer
 |
 +-- Tensor A
 |
 +-- Tensor B
```

---

## 37. Tensor Aliasing

Tensor Aliasing DARF nur erfolgen, wenn:

- Zugriffsrechte kompatibel sind,
- Lebenszeiten kompatibel sind,
- keine unzulässigen Datenabhängigkeiten entstehen.

---

## 38. Tensor Lifetime

Ein Tensor MUSS während einer laufenden NPU Operation intern gültig bleiben.

Ein öffentliches Release DARF keinen Use-After-Free verursachen.

---

## 39. Model Object

Die NPU Runtime DARF ein logisches Model Object unterstützen.

Ein Model kann enthalten:

```text
Graph
Weights
Constants
Metadata
InputDescriptions
OutputDescriptions
```

---

## 40. Model Format

Die öffentliche Runtime DARF nicht an ein einzelnes Model Format gebunden sein.

Mögliche Eingaben können über höhere Schichten stammen aus:

```text
Nova AI Model
ONNX-like Model
Framework Graph
Nova Compute Graph
Portable IR
```

---

## 41. Internal Representation

Die NPU Runtime SOLL ein internes, hardwareunabhängiges Graph- oder IR-Modell verwenden können.

Beispiel:

```text
Source Model
    |
    v
Nova Compute IR
    |
    v
Optimization
    |
    v
NPU Backend IR
    |
    v
Device Binary
```

---

## 42. Graph Object

AI-Workloads SOLLEN als Compute Graph dargestellt werden können.

Beispiel:

```text
Input
  |
  v
Conv
  |
  v
Activation
  |
  v
Pooling
  |
  v
Output
```

---

## 43. Graph Nodes

Ein Graph Node kann repräsentieren:

```text
Operator
Kernel
Memory Operation
Synchronization
Conversion
Device Transfer
```

---

## 44. Graph Edges

Graph Edges repräsentieren mindestens:

```text
Data Dependency
Execution Dependency
```

---

## 45. Graph Compilation

Ein Graph SOLL vor Ausführung kompiliert werden können.

Ablauf:

```text
Graph
 |
 v
Validation
 |
 v
Shape Analysis
 |
 v
Type Analysis
 |
 v
Optimization
 |
 v
Device Mapping
 |
 v
Memory Planning
 |
 v
Backend Compilation
 |
 v
Executable Graph
```

---

## 46. Graph Validation

Vor Compilation MUSS geprüft werden:

```text
Operators
Tensor Types
Shapes
Dependencies
Capabilities
Memory Requirements
Security Constraints
```

---

## 47. Operator Model

Die NPU Runtime SOLL einen hardwareunabhängigen Operator-Katalog unterstützen können.

Beispiele:

```text
ADD
SUB
MUL
DIV

MATMUL
GEMM

CONVOLUTION
TRANSPOSE_CONVOLUTION

RELU
GELU
SIGMOID
TANH

SOFTMAX
NORMALIZATION

POOLING

RESHAPE
TRANSPOSE
CONCAT
SPLIT

ATTENTION
```

---

## 48. Operator Capability

Eine NPU MUSS deklarieren können, welche Operatoren sie nativ unterstützt.

---

## 49. Operator Constraints

Operator Support kann von Parametern abhängen.

Beispiel:

```text
CONVOLUTION

Supported:
Kernel = 3x3
Stride = 1 or 2
INT8

Unsupported:
Kernel = 17x17
```

---

## 50. Operator Query

Die Runtime MUSS prüfen können:

```text
IsOperatorSupported(Device, Descriptor)
```

oder eine äquivalente Funktionalität bereitstellen.

---

## 51. Unsupported Operator

Ein nicht unterstützter Operator MUSS nicht automatisch den gesamten Graph unbrauchbar machen.

---

## 52. Graph Partitioning

Die Runtime SOLL Graphen zwischen mehreren Compute Devices partitionieren können.

Beispiel:

```text
Input
  |
  v
CPU Preprocessing
  |
  v
NPU Main Graph
  |
  v
GPU Postprocessing
  |
  v
Output
```

---

## 53. Partitioning-Kosten

Der Scheduler MUSS bei Partitionierung mindestens berücksichtigen:

```text
ComputeCost
TransferCost
ConversionCost
SynchronizationCost
PowerCost
```

---

## 54. Operator Fallback

Nicht unterstützte Operatoren DÜRFEN auf:

```text
GPU
CPU
```

ausgeführt werden.

---

## 55. Fallback Policy

Ein Workload MUSS angeben können:

```text
ALLOW_FALLBACK
NO_FALLBACK
```

---

## 56. Partial Fallback

Die Runtime SOLL nur den nicht unterstützten Teil eines Graphen auf ein anderes Device verschieben können.

---

## 57. Fallback-Beispiel

```text
NPU Supported
     |
     v
Operator A
     |
     v
Operator B
     |
     v
Unsupported Operator C
     |
     v
GPU
     |
     v
Operator D
     |
     v
NPU
```

Ein solcher Wechsel SOLL nur erfolgen, wenn die Transferkosten vertretbar sind.

---

## 58. Graph Optimization

Die NPU Runtime SOLL Graph-Optimierungen durchführen können.

Mindestens:

```text
Constant Folding
Dead Node Elimination
Operator Fusion
Layout Optimization
Precision Optimization
Memory Reuse
Transfer Elimination
```

---

## 59. Operator Fusion

Mehrere Operatoren DÜRFEN zu einer NPU Operation zusammengefasst werden.

Beispiel:

```text
Convolution
   +
Bias
   +
ReLU
```

wird:

```text
FusedConvolution
```

---

## 60. Fusion Safety

Operator Fusion DARF die definierte numerische Semantik nicht außerhalb erlaubter Toleranzen verändern.

---

## 61. Constant Folding

Berechnungen mit ausschließlich konstanten Eingaben SOLLEN bereits während der Graph Compilation ausgewertet werden.

---

## 62. Dead Node Elimination

Graph Nodes ohne Einfluss auf benötigte Outputs SOLLEN entfernt werden.

---

## 63. Memory Planning

Die NPU Runtime SOLL Speicherlebenszeiten bereits während der Graph Compilation analysieren.

---

## 64. Tensor Memory Reuse

Nicht gleichzeitig benötigte temporäre Tensoren DÜRFEN denselben physischen Speicher verwenden.

Beispiel:

```text
Tensor Temp A
      |
      X Lifetime ends
      |
      v
Same Memory
      |
      v
Tensor Temp B
```

---

## 65. Static Memory Planning

Bei vollständig statischen Graphen SOLL die Runtime einen Großteil des benötigten Speichers vor der Ausführung planen können.

---

## 66. Dynamic Memory Planning

Bei dynamischen Shapes MUSS die Runtime zusätzliche dynamische Allocations unterstützen können.

---

## 67. Memory Budget

Graph Compilation MUSS das verfügbare NPU Memory Budget berücksichtigen können.

---

## 68. Precision Model

Die NPU Runtime MUSS unterschiedliche numerische Präzisionen unterstützen können.

Beispiele:

```text
FP32
FP16
BF16
FP8
INT8
INT4
```

---

## 69. Precision Selection

Die Runtime DARF automatisch eine geeignete Precision auswählen, wenn die Anwendung dies erlaubt.

---

## 70. Precision Policy

Ein Workload SOLL eine Policy angeben können.

Beispiele:

```text
EXACT
HIGH_ACCURACY
BALANCED
HIGH_PERFORMANCE
LOW_POWER
```

---

## 71. EXACT

Bei:

```text
EXACT
```

DARF die Runtime keine Precision-Reduktion durchführen, die die definierte Semantik verändert.

---

## 72. HIGH_ACCURACY

Bei `HIGH_ACCURACY` SOLL Genauigkeit gegenüber Performance priorisiert werden.

---

## 73. BALANCED

Bei `BALANCED` DARF die Runtime Precision und Performance gegeneinander optimieren.

---

## 74. HIGH_PERFORMANCE

Bei `HIGH_PERFORMANCE` DÜRFEN niedrigere Präzisionen bevorzugt werden, wenn die Anwendung dies erlaubt.

---

## 75. LOW_POWER

Bei `LOW_POWER` SOLL die Runtime energieeffiziente Precision- und Device-Kombinationen bevorzugen.

---

## 76. Quantization

Die NPU Runtime SOLL quantisierte Workloads unterstützen.

Mindestens:

```text
INT8
```

Eine vollständige Implementierung SOLL zusätzlich unterstützen können:

```text
INT4
UINT8
UINT4
FP8
```

---

## 77. Quantization Metadata

Quantisierte Tensoren MÜSSEN Quantisierungsinformationen besitzen können.

Beispiele:

```text
Scale
ZeroPoint
Axis
BlockSize
QuantizationType
```

---

## 78. Per-Tensor Quantization

Die Runtime SOLL Per-Tensor Quantization unterstützen können.

---

## 79. Per-Channel Quantization

Eine vollständige Runtime SOLL Per-Channel Quantization unterstützen können.

---

## 80. Block Quantization

Zukünftige oder spezialisierte Backends SOLLEN Block Quantization unterstützen können.

---

## 81. Quantize Operator

Die Runtime SOLL explizite Quantization unterstützen können:

```text
FP16
 |
 v
INT8
```

---

## 82. Dequantize Operator

Die Runtime SOLL unterstützen:

```text
INT8
 |
 v
FP16
```

---

## 83. Redundante Quantization

Der Optimizer SOLL unnötige Quantize/Dequantize-Sequenzen entfernen können.

---

## 84. Mixed Precision

Ein Graph DARF mehrere Präzisionen gleichzeitig verwenden.

Beispiel:

```text
Input FP16
    |
    v
INT8 Convolution
    |
    v
FP16 Normalization
    |
    v
INT8 Matrix Operation
```

---

## 85. Mixed-Precision Planning

Die Runtime SOLL Precision-Grenzen so wählen, dass unnötige Konvertierungen vermieden werden.

---

## 86. Sparsity

Die NPU Runtime SOLL Sparse Computation unterstützen können.

---

## 87. Structured Sparsity

NPU-spezifische strukturierte Sparsity-Formate DÜRFEN unterstützt werden.

Beispiel:

```text
2:4 Sparsity
```

---

## 88. Sparse Capability

Sparsity MUSS explizit als Device Capability angegeben werden.

---

## 89. Sparse Fallback

Kann eine NPU ein Sparse Format nicht verarbeiten, DARF die Runtime:

- Daten dekomprimieren,
- ein anderes Device wählen,
- einen alternativen Kernel verwenden.

---

## 90. Weight Objects

Model Weights SOLLEN als immutable Resources behandelt werden können.

---

## 91. Weight Sharing

Mehrere Model Instances DÜRFEN dieselben immutable Weights verwenden.

---

## 92. Weight Caching

Häufig verwendete Weights DÜRFEN im NPU Memory gecacht werden.

---

## 93. Persistent Weights

Die Runtime SOLL häufig verwendete Model Weights optional über mehrere Inferenzaufrufe resident halten können.

---

## 94. Weight Eviction

Bei Memory Pressure DÜRFEN persistente Weights aus NPU Memory entfernt werden.

---

## 95. Model Compilation

Model- oder Graph-Compilation DARF zeitaufwendig sein.

Die Runtime SOLL Compilationsergebnisse cachen können.

---

## 96. Compilation Cache

Ein Cache Key MUSS mindestens berücksichtigen können:

```text
GraphHash
ModelHash
NPUArchitecture
BackendVersion
DriverVersion
CompilerVersion
PrecisionPolicy
OptimizationFlags
ShapeProfile
```

---

## 97. Cache Invalidation

Ein Cache-Eintrag MUSS ungültig werden können, wenn relevante Komponenten geändert wurden.

Beispiele:

```text
Driver Update
Backend Update
Compiler Update
Hardware Change
```

---

## 98. AOT Compilation

Die Runtime SOLL Ahead-of-Time Compilation unterstützen können.

---

## 99. JIT Compilation

Die Runtime SOLL Just-in-Time Compilation unterstützen können.

---

## 100. Lazy Compilation

Graphen DÜRFEN erst beim ersten tatsächlichen Einsatz kompiliert werden.

---

## 101. Background Compilation

Nicht zeitkritische Compilation DARF im Hintergrund durchgeführt werden.

---

## 102. Compilation Security

Compiler-Eingaben MÜSSEN als nicht vertrauenswürdig behandelt werden.

Ein fehlerhaftes Modell DARF keinen privilegierten Speicherzugriff ermöglichen.

---

## 103. NPU Scheduling

NPU Workloads MÜSSEN durch den Compute Scheduler gemäß `NPSPEC-COMPUTE-RUNTIME-0002` eingeplant werden können.

---

## 104. Scheduling Inputs

Mindestens folgende Werte SOLLEN berücksichtigt werden:

```text
NPU Load
QueueDepth
SupportedOperators
SupportedPrecision
DataResidency
TransferCost
GraphCompilationState
MemoryPressure
PowerState
ThermalState
Priority
Deadline
```

---

## 105. NPU Affinity

Ein Job DARF eine bevorzugte NPU angeben.

Beispiel:

```text
PreferredDevice = NPU0
```

---

## 106. Required NPU

Ein Job DARF zwingend eine bestimmte NPU verlangen.

Beispiel:

```text
RequiredDevice = NPU1
```

---

## 107. Automatic Device Selection

Ohne feste Device-Vorgabe SOLL der Scheduler automatisch entscheiden können zwischen:

```text
CPU
GPU
NPU
```

---

## 108. Device Selection Example

```text
AI Job
  |
  v
Capability Analysis
  |
  +--> CPU
  |
  +--> GPU
  |
  +--> NPU
  |
  v
Cost Model
  |
  v
Selected Device
```

---

## 109. Selection Cost Model

Das Cost Model SOLL mindestens berücksichtigen:

```text
ExecutionTime
TransferTime
QueueWaitTime
CompilationTime
PowerCost
ThermalCost
MemoryCost
```

---

## 110. Small Workloads

Sehr kleine AI-Workloads DÜRFEN auf der CPU ausgeführt werden, wenn der NPU-Dispatch-Overhead größer als der Nutzen wäre.

---

## 111. Large Workloads

Große geeignete Tensor-Workloads SOLLEN bevorzugt auf NPU oder GPU ausgeführt werden.

---

## 112. Repeated Workloads

Wiederkehrende Workloads SOLLEN von:

- persistenten Weights,
- Compilation Cache,
- vorbereiteten Graphen,
- Memory Reuse

profitieren.

---

## 113. Latency-sensitive Workloads

Interaktive AI-Aufgaben SOLLEN geringe Latenz priorisieren können.

Beispiele:

```text
Speech Recognition
UI AI
Camera Processing
Realtime Translation
```

---

## 114. Throughput Workloads

Batch- oder Hintergrundaufgaben DÜRFEN Durchsatz gegenüber Einzellatenz priorisieren.

---

## 115. Batching

Die NPU Runtime SOLL mehrere kompatible Inferenzanforderungen bündeln können.

---

## 116. Dynamic Batching

Eine vollständige Runtime DARF kurzfristig eintreffende Requests dynamisch zu einem Batch zusammenfassen.

---

## 117. Batching-Latenz

Dynamic Batching DARF definierte Latenzgrenzen nicht überschreiten.

---

## 118. Multi-NPU

Die Runtime SOLL mehrere NPUs gleichzeitig unterstützen können.

---

## 119. Multi-NPU Scheduling

Unabhängige Workloads DÜRFEN auf verschiedene NPUs verteilt werden.

Beispiel:

```text
Job A -> NPU0
Job B -> NPU1
```

---

## 120. Model Parallelism

Große Modelle DÜRFEN auf mehrere geeignete Devices verteilt werden.

---

## 121. Tensor Parallelism

Tensoroperationen DÜRFEN zwischen mehreren Devices aufgeteilt werden, wenn die Runtime dies unterstützt.

---

## 122. Pipeline Parallelism

Ein Modell DARF in mehrere Pipeline-Stufen zerlegt werden.

Beispiel:

```text
Stage 1 -> NPU0
Stage 2 -> NPU1
Stage 3 -> GPU0
```

---

## 123. Multi-Device Synchronisation

Abhängigkeiten zwischen mehreren Devices MÜSSEN korrekt synchronisiert werden.

---

## 124. Topology Awareness

Die Runtime SOLL die Verbindung zwischen Compute Devices berücksichtigen.

Beispiele:

```text
Shared Memory
On-SoC Interconnect
PCIe
Dedicated Accelerator Link
```

---

## 125. CPU/NPU Shared Memory

Bei gemeinsamer Memory Domain SOLL eine unnötige Kopie vermieden werden.

---

## 126. GPU/NPU Interop

Tensoren SOLLEN direkt zwischen GPU und NPU geteilt werden können, wenn Hardware und Memory Model dies unterstützen.

---

## 127. GPU/NPU Pipeline

Beispiel:

```text
Camera
  |
  v
GPU Preprocessing
  |
  v
NPU Inference
  |
  v
GPU Rendering
```

Unnötige Kopien SOLLEN vermieden werden.

---

## 128. Audio/NPU Pipeline

Beispiel:

```text
Audio Input
    |
    v
DSP / CPU Preprocessing
    |
    v
NPU Speech Model
    |
    v
Text Output
```

---

## 129. Power Policy

NPU Compute MUSS in das NovaOS-Power-Modell integriert werden.

Mindestens:

```text
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 130. NPU Energy Advantage

Die Runtime SOLL berücksichtigen, dass eine NPU für bestimmte AI-Workloads erheblich energieeffizienter als CPU oder GPU sein kann.

---

## 131. Performance Mode

Im Performance-Modus DARF die Runtime:

- höhere NPU Performance States verwenden,
- größere Batches verwenden,
- mehr parallele Execution Units aktivieren.

---

## 132. Balanced Mode

Im Balanced-Modus SOLL ein Kompromiss zwischen:

```text
Latency
Throughput
Power
Thermals
```

gewählt werden.

---

## 133. Efficiency Mode

Im Efficiency-Modus SOLL die NPU bevorzugt werden, wenn sie für den Workload energieeffizienter ist.

---

## 134. Battery Saver

Im Battery-Saver-Modus SOLL die Runtime:

- energieeffiziente Precision verwenden,
- Background-AI drosseln,
- unnötige Device Wakeups vermeiden.

---

## 135. NPU Power State

Die Runtime SOLL mindestens folgende Power States berücksichtigen können:

```text
ACTIVE
IDLE
LOW_POWER
SUSPENDED
```

---

## 136. Wake Cost

Das Aktivieren einer schlafenden NPU besitzt einen Wake Cost.

Der Scheduler SOLL diesen berücksichtigen.

---

## 137. Thermal State

Die NPU Runtime MUSS thermische Zustände berücksichtigen können.

Mindestens:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

---

## 138. Thermal Throttling

Eine gedrosselte NPU DARF nicht anhand ihrer nominellen Spitzenleistung bewertet werden.

---

## 139. Thermal Migration

Bei mehreren geeigneten Devices DARF ein Workload auf ein thermisch günstigeres Device verschoben werden.

---

## 140. NPU Health State

Mindestens folgende Health States SOLLEN unterstützt werden:

```text
HEALTHY
DEGRADED
UNSTABLE
RESETTING
FAILED
LOST
```

---

## 141. Device Lost

Ein NPU Device Loss MUSS kontrolliert behandelt werden.

---

## 142. Device-Lost-Ablauf

Ein typischer Ablauf lautet:

```text
NPU Error
   |
   v
Stop New Submissions
   |
   v
Mark Running Jobs
   |
   v
Drain / Abort Queues
   |
   v
Reset Device
   |
   v
Reinitialize Backend
   |
   v
Restore Device
```

---

## 143. NPU Reset

Ein NPU Reset DARF ausgelöst werden bei:

- Hardware Hang,
- Watchdog Timeout,
- Memory Fault,
- Backend Failure,
- Security Violation.

---

## 144. Retry-Safe Jobs

Jobs mit:

```text
RETRY_SAFE
```

DÜRFEN nach erfolgreichem Reset erneut eingeplant werden.

---

## 145. Non-Retry-Safe Jobs

Jobs ohne `RETRY_SAFE` DÜRFEN nicht automatisch erneut ausgeführt werden.

---

## 146. Model Recovery

Nach einem Device Reset SOLL die Runtime regenerierbare:

```text
Compiled Graphs
Weight Mappings
Temporary Buffers
Queues
```

neu erzeugen können.

---

## 147. Persistent Model State

Nicht rekonstruierbarer Model State MUSS nach Device Loss als verloren gemeldet werden.

---

## 148. Watchdog

Die NPU Runtime SOLL einen Watchdog für blockierende oder fehlerhafte Workloads unterstützen.

---

## 149. Timeout

NPU Jobs DÜRFEN ein Timeout besitzen.

Beispiel:

```text
Timeout = 1000 ms
```

---

## 150. Preemption

Falls Hardware Preemption unterstützt, SOLL die Runtime diese nutzen können.

---

## 151. Preemption Capability

Eine NPU MUSS deklarieren können, ob und auf welcher Ebene Preemption möglich ist.

Beispiele:

```text
GRAPH
OPERATOR
TILE
INSTRUCTION
```

---

## 152. Non-preemptive NPU

Bei nicht preemptierbaren NPUs SOLL die Runtime große Workloads möglichst in kleinere Ausführungseinheiten zerlegen können.

---

## 153. Fairness

Ein einzelner Prozess DARF die NPU nicht dauerhaft monopolieren.

---

## 154. Foreground Priority

Interaktive AI-Aufgaben SOLLEN gegenüber Background AI bevorzugt werden können.

---

## 155. Background AI

Nicht zeitkritische AI-Aufgaben DÜRFEN:

- verzögert,
- gebündelt,
- gedrosselt

werden.

---

## 156. Security

NPU Compute MUSS dem allgemeinen NovaOS-Sicherheitsmodell folgen.

---

## 157. NPU Memory Isolation

Ein Prozess DARF keine Tensor- oder Weight-Daten eines anderen Prozesses lesen.

---

## 158. Model Isolation

Ein Prozess DARF keine fremden Model Objects oder Graph Objects verwenden, sofern diese nicht explizit geteilt wurden.

---

## 159. Memory Sanitization

NPU Memory MUSS vor Wiederverwendung durch eine andere Security Domain bereinigt werden, sofern Hardware dies nicht garantiert.

---

## 160. Graph Validation

Graphen MÜSSEN vor Ausführung validiert werden.

Mindestens:

```text
Operator Validity
Tensor Bounds
Shape Consistency
Type Consistency
Memory Access
Capability Requirements
```

---

## 161. Backend Binary Validation

NPU-native Binaries MÜSSEN vor Ausführung validiert oder aus vertrauenswürdigen Runtime-Komponenten erzeugt werden.

---

## 162. Protected Models

Die Runtime DARF geschützte Model Resources unterstützen.

Beispiel:

```text
PROTECTED_MODEL
```

---

## 163. Protected Weights

Sicherheitskritische oder lizenzgeschützte Weights DÜRFEN in geschützten Memory Domains gehalten werden.

---

## 164. Export Policy

Geschützte Model- oder Tensor-Resources DÜRFEN nicht ohne entsprechende Berechtigung exportiert werden.

---

## 165. Performance Counters

Performance Counter können sensible Informationen über fremde Workloads offenlegen.

Der Zugriff MUSS daher kontrollierbar sein.

---

## 166. Profiling

Die NPU Runtime SOLL Profiling unterstützen.

Mindestens:

```text
QueueWaitTime
CompilationTime
ExecutionTime
OperatorTime
TransferTime
MemoryUsage
NPUUtilization
```

---

## 167. Operator Profiling

Eine vollständige Runtime SOLL Ausführungszeiten einzelner Operatoren messen können.

---

## 168. Graph Profiling

Graph Profiling SOLL mindestens ermöglichen:

```text
Critical Path
Slow Operators
Transfer Bottlenecks
Memory Peaks
Fallback Nodes
```

---

## 169. Hardware Timestamps

Wenn verfügbar, SOLLEN NPU Hardware Timestamps verwendet werden.

---

## 170. NPU Trace Events

Mindestens folgende Trace Events SOLLEN verfügbar sein:

```text
NPU_DEVICE_REGISTER
NPU_CONTEXT_CREATE
NPU_QUEUE_CREATE

NPU_GRAPH_COMPILE_START
NPU_GRAPH_COMPILE_END

NPU_JOB_SUBMIT
NPU_JOB_START
NPU_JOB_END

NPU_MEMORY_ALLOC
NPU_MEMORY_FREE

NPU_DEVICE_LOST
NPU_DEVICE_RESET
```

---

## 171. NPU Metrics

Pro NPU SOLLEN mindestens folgende Werte verfügbar sein:

```text
Utilization
QueueDepth
MemoryUsed
MemoryFree
Temperature
PowerState
HealthState
```

---

## 172. AI Metrics

Optional:

```text
InferenceCount
AverageInferenceLatency
BatchSize
OperatorFallbackCount
GraphCacheHitRate
WeightCacheHitRate
```

---

## 173. Privacy

Profiling und Telemetrie DÜRFEN keine unnötigen:

- Modellinhalte,
- Tensorinhalte,
- Nutzerdaten

offenlegen.

---

## 174. NPU Backend

Die NPU Runtime MUSS ein standardisiertes Backend Interface definieren.

Konzeptionell:

```text
probe()
initialize()
enumerate_devices()

create_context()
destroy_context()

create_queue()
destroy_queue()

allocate_memory()
free_memory()

query_operator_support()

compile_graph()
destroy_graph()

submit_graph()

signal()
wait()

reset_device()

shutdown()
```

---

## 175. Backend Versioning

Das NPU Backend ABI MUSS versioniert sein.

Beispiel:

```text
NovaNPUBackendABI 1.0
```

---

## 176. Capability Negotiation

Runtime und NPU Backend MÜSSEN ihre unterstützten Funktionen explizit aushandeln.

---

## 177. Backend Independence

Die öffentliche Nova Compute API DARF nicht von einer bestimmten Hersteller-Runtime abhängen.

---

## 178. Vendor Backend

Hardwarehersteller DÜRFEN optimierte NPU Backends bereitstellen.

Diese MÜSSEN die NovaOS-Regeln für:

```text
ABI
Security
Isolation
Memory
Scheduling
Recovery
```

einhalten.

---

## 179. Generic Backend

NovaOS SOLL generische Backend-Pfade unterstützen können, sofern eine geeignete standardisierte Schnittstelle vorhanden ist.

---

## 180. Software Fallback

Wenn keine geeignete NPU vorhanden ist, DARF die Compute Runtime auf:

```text
GPU
CPU SIMD
CPU Scalar
```

zurückfallen.

---

## 181. Fallback-Reihenfolge

Eine mögliche Reihenfolge lautet:

```text
NPU
 |
 v
GPU
 |
 v
CPU SIMD
 |
 v
CPU Scalar
```

Die tatsächliche Reihenfolge MUSS durch das Cost Model bestimmt werden können.

---

## 182. Kein zwingender NPU-Vorrang

Die Existenz einer NPU bedeutet nicht, dass jeder AI-Workload auf ihr ausgeführt werden muss.

---

## 183. Cost-Based Selection

Die Runtime SOLL das Device mit den geringsten erwarteten Gesamtkosten wählen.

Beispiel:

```text
TotalCost =
    QueueWait
  + Transfer
  + Compilation
  + Execution
  + Synchronization
  + PowerPenalty
  + ThermalPenalty
```

---

## 184. AI Framework Integration

Höhere NovaOS AI Frameworks SOLLEN die NPU Runtime über die gemeinsame Compute Runtime verwenden.

---

## 185. Framework Independence

Die NPU Runtime DARF nicht an ein einzelnes Framework wie:

```text
PyTorch
TensorFlow
ONNX Runtime
```

gebunden sein.

Framework Adapter DÜRFEN darüber aufgebaut werden.

---

## 186. Nova AI Integration

Nova AI SOLL Tensoren und Graphen möglichst direkt an die Compute Runtime übergeben können.

---

## 187. Model Preparation

Ein typischer Ablauf lautet:

```text
Model
  |
  v
Framework Import
  |
  v
Nova Compute Graph
  |
  v
Optimization
  |
  v
Device Partitioning
  |
  v
NPU Compilation
```

---

## 188. Inference Execution

Ein typischer Inferenzablauf lautet:

```text
Input Tensor
     |
     v
Prepare / Map
     |
     v
NPU Queue
     |
     v
Executable Graph
     |
     v
NPU Execution
     |
     v
Output Tensor
```

---

## 189. Streaming Inference

Die NPU Runtime SOLL Streaming-Workloads unterstützen können.

Beispiele:

```text
Audio
Video
Camera
Sensor Data
Token Streams
```

---

## 190. Stateful Inference

Modelle DÜRFEN persistenten Zustand besitzen.

Beispiele:

```text
KV Cache
Recurrent State
Streaming State
```

---

## 191. Stateful Object Lifetime

Persistenter Model State MUSS als explizite Runtime Resource verwaltet werden.

---

## 192. KV Cache

Die Runtime SOLL für transformerbasierte Modelle effiziente Cache-Ressourcen unterstützen können.

---

## 193. KV-Cache Placement

KV Cache DARF abhängig von Hardware und Memory Pressure liegen in:

```text
NPU Memory
GPU Memory
Shared Memory
Host Memory
```

---

## 194. KV-Cache Migration

KV Cache DARF zwischen Memory Domains migriert werden, sofern die Semantik erhalten bleibt.

---

## 195. Large Models

Modelle, die größer als der verfügbare NPU Memory sind, SOLLEN unterstützt werden können, sofern Backend und Runtime geeignete Strategien besitzen.

---

## 196. Model Streaming

Große Weight-Blöcke DÜRFEN bei Bedarf in den NPU Memory gestreamt werden.

---

## 197. Layer-wise Execution

Große Modelle DÜRFEN schichtweise ausgeführt werden.

Beispiel:

```text
Load Layer 1
Execute
Evict

Load Layer 2
Execute
Evict
```

---

## 198. Memory Pressure

Die NPU Runtime MUSS Memory Pressure erkennen können.

Mindestens:

```text
LOW
MODERATE
HIGH
CRITICAL
```

---

## 199. Memory Pressure Response

Bei hohem Memory Pressure DARF die Runtime:

- Weight Caches verkleinern,
- temporäre Tensoren freigeben,
- Daten in Host Memory verschieben,
- andere Devices verwenden,
- neue Workloads verzögern.

---

## 200. Suspend

Vor System-Suspend MUSS die NPU Runtime aktive Workloads behandeln.

Mögliche Aktionen:

```text
COMPLETE
PAUSE
CHECKPOINT
CANCEL
```

---

## 201. Resume

Nach Resume MUSS die Runtime:

- Device State prüfen,
- Backend neu initialisieren,
- Graph Executables validieren,
- Memory Residency prüfen,
- verlorene Resources erkennen.

---

## 202. Hibernate

Regenerierbare NPU Backend Objects MÜSSEN nicht zwingend vollständig im Hibernate-Image gespeichert werden.

---

## 203. Hot-Plug

Externe NPUs SOLLEN dynamisch registriert werden können.

---

## 204. Hot-Unplug

Ein geplantes Removal SOLL über folgende Zustände erfolgen können:

```text
AVAILABLE
   |
   v
QUIESCING
   |
   v
OFFLINE
   |
   v
REMOVED
```

---

## 205. Testing — Device Enumeration

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
Integrated NPU
Discrete NPU
Multiple NPUs
Virtual NPU
External NPU
No NPU
```

---

## 206. Testing — Capability Detection

Zu testen sind:

```text
FP32
FP16
BF16
FP8
INT8
INT4
Quantization
Sparsity
Dynamic Shapes
Preemption
```

---

## 207. Testing — Tensor

Zu testen sind:

```text
1D Tensor
2D Tensor
3D Tensor
4D Tensor
High-Rank Tensor
Static Shape
Dynamic Shape
Tensor View
Tensor Aliasing
```

---

## 208. Testing — Layout

Mindestens:

```text
NCHW
NHWC
Device Native
Layout Conversion
Redundant Conversion Elimination
```

---

## 209. Testing — Graph Compilation

Zu testen sind:

```text
Simple Graph
Large Graph
Dynamic Graph
Invalid Graph
Unsupported Operator
Mixed Device Graph
```

---

## 210. Testing — Operators

Mindestens:

```text
MATMUL
CONVOLUTION
RELU
GELU
SOFTMAX
NORMALIZATION
RESHAPE
TRANSPOSE
```

---

## 211. Testing — Operator Fusion

Zu testen sind:

```text
Conv + Bias
Conv + Bias + ReLU
MatMul + Add
Unsupported Fusion
Numerical Correctness
```

---

## 212. Testing — Quantization

Mindestens:

```text
FP32 -> INT8
FP16 -> INT8
INT8 -> FP16
Per-Tensor
Per-Channel
Mixed Precision
```

---

## 213. Testing — Fallback

Zu testen sind:

```text
NPU -> GPU
NPU -> CPU
Partial Graph Fallback
No Fallback
Fallback with Transfer Cost
```

---

## 214. Testing — Memory

Zu testen sind:

```text
NPU-local Memory
Shared Memory
Unified Memory
Zero-Copy
Tensor Migration
Memory Pressure
Weight Eviction
```

---

## 215. Testing — Multi-Device

Mindestens:

```text
CPU + NPU
GPU + NPU
NPU0 + NPU1
CPU + GPU + NPU
```

---

## 216. Testing — Scheduling

Zu testen sind:

```text
Idle NPU
Busy NPU
Queue Saturation
Small Workload
Large Workload
Latency-Sensitive Workload
Background Workload
```

---

## 217. Testing — Power

Zu testen sind:

```text
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 218. Testing — Thermal

Zu testen sind:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

---

## 219. Testing — Device Lost

Zu testen sind:

```text
Device Lost before Submit
Device Lost during Graph Execution
Device Reset
Queue Failure
Tensor Loss
Weight Loss
Retry-Safe Job
Fallback after Failure
```

---

## 220. Testing — Security

Zu testen sind:

```text
Cross-Process Tensor Access
Cross-Context Tensor Access
Invalid Graph
Out-of-Bounds Tensor
Unauthorized Model Export
Performance Counter Access
```

---

## 221. Testing — Stateful Models

Zu testen sind:

```text
Recurrent State
Streaming State
KV Cache
State Migration
Device Loss with State
```

---

## 222. Stress Tests

Stress Tests SOLLEN mindestens umfassen:

```text
Thousands of Graphs
Millions of Inferences
Continuous Tensor Allocation
Continuous Tensor Release
Multiple Processes
Multiple Models
Multiple NPUs
Device Reset under Load
```

---

## 223. Fuzzing

Folgende Eingaben SOLLTEN fuzz-getestet werden:

```text
Graph Descriptors
Tensor Descriptors
Shape Metadata
Operator Descriptors
Quantization Metadata
Backend IR
Import / Export Descriptors
```

---

## 224. Fault Injection

Mindestens folgende Fehler SOLLEN injizierbar sein:

```text
DEVICE_LOST
DEVICE_RESET
OUT_OF_MEMORY
GRAPH_COMPILE_FAILED
OPERATOR_FAILED
TRANSFER_FAILED
QUEUE_FAILED
INVALID_MODEL
```

---

## 225. Performance Tests

Mindestens folgende Werte SOLLEN gemessen werden:

```text
GraphCompilationLatency
InferenceLatency
BatchThroughput
QueueSubmissionLatency
TensorTransferLatency
TensorTransferBandwidth
MemoryAllocationLatency
SynchronizationLatency
```

---

## 226. Energy Tests

Da Energieeffizienz ein zentraler NPU-Anwendungsfall ist, SOLLEN mindestens verglichen werden:

```text
CPU Energy per Inference
GPU Energy per Inference
NPU Energy per Inference
```

---

## 227. Regression Tests

Änderungen an Runtime, Compiler oder Backend SOLLEN mindestens hinsichtlich folgender Werte verglichen werden:

```text
Accuracy
Latency
Throughput
MemoryUsage
TransferVolume
PowerUsage
ThermalBehavior
FallbackRate
CompilationTime
```

---

## 228. Numerical Validation

Optimierungen und Precision-Änderungen MÜSSEN gegen definierte numerische Toleranzen geprüft werden.

---

## 229. Accuracy Regression

Eine Runtime-Optimierung DARF nicht unbemerkt die Modellgenauigkeit außerhalb definierter Grenzen verschlechtern.

---

## 230. Minimalimplementierung

Eine minimale konforme NPU Runtime MUSS mindestens unterstützen:

```text
Single NPU Device
NPU Context
NPU Queue
Tensor Objects
Static Shapes
FP16 or INT8
Graph Compilation
Graph Execution
Host-to-NPU Transfer
NPU-to-Host Transfer
Event or Fence
Basic Device Lost Handling
```

---

## 231. Erweiterte Implementierung

Eine vollständige NPU Runtime SOLL zusätzlich unterstützen:

```text
Multiple NPUs
Dynamic Shapes
Mixed Precision
INT8
INT4
FP8
Quantization
Per-Channel Quantization
Sparsity
Operator Fusion
Graph Partitioning
CPU/GPU/NPU Cooperation
Zero-Copy
Unified Memory
Persistent Weights
Dynamic Batching
Stateful Models
KV Cache
Large Model Streaming
Preemption
Power-Aware Scheduling
Thermal-Aware Scheduling
Device Recovery
Profiling
```

---

## 232. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0006` MUSS:

1. mindestens eine NPU als Compute Device abstrahieren können.
2. NPU Capabilities abfragbar machen.
3. NPU Contexts unterstützen.
4. NPU Queue Submission unterstützen.
5. Tensor Objects unterstützen.
6. Tensor Shapes und Datentypen validieren.
7. mindestens statische Graphen ausführen können.
8. Graphen vor Ausführung validieren.
9. NPU Memory verwalten.
10. Host-to-NPU Transfers unterstützen, sofern getrennte Memory Domains existieren.
11. NPU-to-Host Transfers unterstützen, sofern getrennte Memory Domains existieren.
12. Synchronisationsobjekte bereitstellen.
13. Context-Isolation gewährleisten.
14. Process-Isolation gewährleisten.
15. NPU Memory Isolation gewährleisten.
16. Device Lost kontrolliert behandeln.
17. Jobs mit dem Compute Scheduler integrieren.
18. Tensor Memory mit dem Compute Memory Model integrieren.
19. NPU Objects mit dem Compute Object Model integrieren.
20. unsupported Operators definiert behandeln.
21. einen definierten Fallback-Pfad ermöglichen, sofern der Job Fallback erlaubt.
22. Resource Limits unterstützen.
23. Diagnoseinformationen bereitstellen.
24. zukünftige NPU Backends erweiterbar halten.

---

## 233. Empfohlene interne Komponenten

Eine vollständige NPU Runtime SOLL logisch folgende Komponenten besitzen können:

```text
NovaNPURuntime
 |
 +-- NPUDeviceManager
 |
 +-- NPUContextManager
 |
 +-- NPUQueueManager
 |
 +-- NPUTensorManager
 |
 +-- NPUGraphManager
 |
 +-- NPUGraphCompiler
 |
 +-- NPUOperatorManager
 |
 +-- NPUMemoryManager
 |
 +-- NPUQuantizationManager
 |
 +-- NPUSchedulerAdapter
 |
 +-- NPUSynchronizationManager
 |
 +-- NPUPowerManager
 |
 +-- NPUThermalManager
 |
 +-- NPURecoveryManager
 |
 +-- NPUDiagnostics
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Modul- oder Klassenstruktur vor.

---

## 234. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

`NPSPEC-COMPUTE-RUNTIME-0001` definiert die allgemeine NovaOS Compute Runtime.

Die NPU Runtime implementiert die NPU-spezifische Ausführung innerhalb dieses Modells.

```text
Compute Runtime
      |
      v
NPU Runtime
      |
      v
NPU Backend
      |
      v
NPU Hardware
```

---

## 235. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler entscheidet, ob ein geeigneter Workload auf:

```text
CPU
GPU
NPU
```

ausgeführt werden soll.

```text
Compute Job
    |
    v
Scheduler
    |
    +--> CPU Runtime
    |
    +--> GPU Runtime
    |
    +--> NPU Runtime
```

Die NPU Runtime übernimmt anschließend die gerätespezifische Ausführung.

---

## 236. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Tensor Memory und NPU Memory MÜSSEN auf dem allgemeinen Compute Memory Model aufbauen.

```text
Tensor
   |
   v
Logical Resource
   |
   v
Compute Memory Model
   |
   v
NPU Memory Manager
   |
   v
NPU Memory
```

---

## 237. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Alle NPU Runtime Objects MÜSSEN die Regeln des Compute Object Models einhalten.

Beispiele:

```text
NPU Device
NPU Context
NPU Queue
Tensor
Model
Graph
Executable Graph
Event
Fence
```

---

## 238. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime und NPU Runtime sind gleichrangige spezialisierte Backends der allgemeinen Compute Runtime.

```text
                 Compute Runtime
                       |
          +------------+------------+
          |                         |
          v                         v
     GPU Runtime                NPU Runtime
          |                         |
          v                         v
         GPU                       NPU
```

Workloads DÜRFEN zwischen beiden Runtimes aufgeteilt werden.

---

## 239. CPU/GPU/NPU-Kooperation

Das langfristige Ziel ist eine gemeinsame heterogene Compute-Ausführung.

```text
                    Compute Graph
                         |
                         v
                      Scheduler
                         |
          +--------------+--------------+
          |              |              |
          v              v              v
         CPU            GPU            NPU
          |              |              |
          +--------------+--------------+
                         |
                         v
                    Shared Result
```

Die Anwendung SOLL nicht für jede Hardwareklasse vollständig getrennte Ausführungspfade implementieren müssen.

---

## 240. Empfohlene Folge-Spezifikationen

Auf dieser Spezifikation SOLLEN weitere Detail-Spezifikationen aufbauen können.

Empfohlen:

```text
NPSPEC-COMPUTE-NPU-DEVICE-0001
NPU Device Model

NPSPEC-COMPUTE-NPU-CONTEXT-0001
NPU Context

NPSPEC-COMPUTE-NPU-QUEUE-0001
NPU Queue

NPSPEC-COMPUTE-NPU-TENSOR-0001
NPU Tensor Model

NPSPEC-COMPUTE-NPU-MEMORY-0001
NPU Memory

NPSPEC-COMPUTE-NPU-GRAPH-0001
NPU Graph Model

NPSPEC-COMPUTE-NPU-OPERATOR-0001
NPU Operator Model

NPSPEC-COMPUTE-NPU-COMPILER-0001
NPU Graph Compiler

NPSPEC-COMPUTE-NPU-QUANT-0001
NPU Quantization

NPSPEC-COMPUTE-NPU-SPARSE-0001
NPU Sparsity

NPSPEC-COMPUTE-NPU-SYNC-0001
NPU Synchronization

NPSPEC-COMPUTE-NPU-MULTI-0001
Multi-NPU Runtime

NPSPEC-COMPUTE-NPU-INTEROP-0001
CPU/GPU/NPU Interoperability

NPSPEC-COMPUTE-NPU-POWER-0001
NPU Power Management

NPSPEC-COMPUTE-NPU-THERMAL-0001
NPU Thermal Management

NPSPEC-COMPUTE-NPU-RECOVERY-0001
NPU Recovery

NPSPEC-COMPUTE-NPU-PROFILE-0001
NPU Profiling

NPSPEC-COMPUTE-NPU-SECURITY-0001
NPU Security

NPSPEC-COMPUTE-NPU-TEST-0001
NPU Runtime Conformance Tests
```

---

## 241. Zusammenfassung

Die NovaOS NPU Runtime bildet die hardwareunabhängige Ausführungsschicht für Neural Processing Units und vergleichbare AI-Beschleuniger.

Die grundlegende Architektur lautet:

```text
Application
    |
    v
Nova AI / Compute API
    |
    v
Compute Runtime
    |
    v
NPU Runtime
    |
    +-- Device Management
    |
    +-- Tensor Management
    |
    +-- Graph Compilation
    |
    +-- Operator Mapping
    |
    +-- Quantization
    |
    +-- Memory Planning
    |
    +-- Queue Management
    |
    +-- Scheduling
    |
    +-- Power / Thermal
    |
    +-- Recovery
    |
    v
NPU Backend
    |
    v
NPU Hardware
```

Die Runtime abstrahiert unterschiedliche NPU-Architekturen und stellt sie als normale Compute Devices bereit.

Ein typischer AI-Ausführungspfad lautet:

```text
Model
  |
  v
Compute Graph
  |
  v
Graph Validation
  |
  v
Graph Optimization
  |
  v
Device Selection
  |
  v
NPU Compilation
  |
  v
Memory Planning
  |
  v
Queue Submission
  |
  v
NPU Execution
  |
  v
Output Tensor
```

Die NPU Runtime berücksichtigt dabei nicht nur reine Rechenleistung, sondern auch:

```text
Operator Support
Precision
Quantization
Tensor Layout
Memory Residency
Transfer Cost
Queue Load
Power
Thermals
Security
Recovery
```

Wenn ein Graph nicht vollständig auf einer NPU ausgeführt werden kann, DARF NovaOS ihn zwischen mehreren Compute Devices partitionieren:

```text
Input
  |
  v
CPU
  |
  v
GPU
  |
  v
NPU
  |
  v
GPU
  |
  v
Output
```

Die tatsächliche Verteilung wird nicht allein anhand theoretischer Rechenleistung gewählt. Die Runtime berücksichtigt die Gesamtkosten aus:

```text
Execution
+
Transfer
+
Synchronization
+
Compilation
+
Memory
+
Power
+
Thermals
```

Damit entsteht langfristig eine gemeinsame heterogene Compute-Plattform:

```text
                 Nova Compute Runtime
                         |
        +----------------+----------------+
        |                |                |
        v                v                v
       CPU              GPU              NPU
        |                |                |
        +----------------+----------------+
                         |
                         v
                Unified Compute Model
```

Das grundlegende Architekturprinzip lautet:

> Eine NPU ist in NovaOS kein isolierter AI-Sonderbeschleuniger, sondern ein vollständig integriertes Compute Device. Tensoren, Graphen und AI-Workloads werden über dieselbe Compute-Infrastruktur verwaltet wie andere Rechenaufgaben, sodass NovaOS CPU, GPU und NPU gemeinsam, dynamisch und hardwareunabhängig einsetzen kann.