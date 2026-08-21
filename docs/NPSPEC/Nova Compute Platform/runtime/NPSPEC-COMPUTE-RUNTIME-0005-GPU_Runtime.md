# NPSPEC-COMPUTE-RUNTIME-0005 — GPU Runtime

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS GPU Runtime  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0005`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Kurzname:** GPU Runtime  

---

## 1. Zweck

Diese Spezifikation definiert die **GPU Runtime** der NovaOS Compute Runtime.

Die GPU Runtime stellt eine einheitliche Abstraktionsschicht für allgemeine GPU-basierte Compute-Ausführung bereit.

Sie kapselt insbesondere:

- GPU Device Discovery,
- GPU Capabilities,
- GPU Contexts,
- GPU Queues,
- Command Submission,
- Compute Kernel Dispatch,
- GPU Memory,
- Shared Memory,
- Unified Memory,
- Transfers,
- Synchronisation,
- Scheduling,
- Multi-GPU,
- Device Recovery,
- Preemption,
- Performance Counters,
- Profiling,
- Power- und Thermal-Integration.

Die öffentliche NovaOS Compute API DARF nicht fest an einen bestimmten GPU-Hersteller oder eine bestimmte proprietäre GPU-API gekoppelt sein.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Anwendungen verwenden ein einheitliches NovaOS-Compute-Modell. Die GPU Runtime übersetzt dieses Modell auf die Fähigkeiten und Ausführungsmechanismen der jeweils vorhandenen GPU-Hardware.

Die GPU Runtime MUSS sowohl diskrete als auch integrierte GPUs abstrahieren können.

---

## 3. Ziele

Die GPU Runtime MUSS folgende Ziele verfolgen:

1. herstellerunabhängige GPU-Compute-Ausführung,
2. Unterstützung integrierter und diskreter GPUs,
3. Unterstützung mehrerer GPUs,
4. geringe Submission-Latenz,
5. effiziente GPU-Speichernutzung,
6. asynchrone Ausführung,
7. Compute/Transfer-Overlap,
8. robuste Device-Lost-Behandlung,
9. Integration mit dem Compute Scheduler,
10. Integration mit dem Compute Memory Model,
11. Integration mit dem Compute Object Model,
12. sichere Prozess- und Context-Isolation,
13. skalierbare Queue-Verwaltung,
14. Power- und Thermal-Awareness,
15. Erweiterbarkeit für zukünftige GPU-Architekturen.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- die NovaOS-Grafik-Rendering-Pipeline,
- den Desktop-Compositor,
- konkrete Shader-Sprachen,
- konkrete GPU-ISA-Formate,
- vollständige Treiberimplementierungen,
- eine bestimmte Hersteller-API.

Die GPU Runtime kann intern auf bestehende oder zukünftige Backend-Technologien abbilden.

---

## 5. Architektur

Die grundlegende Architektur lautet:

```text
Application
    |
    v
Nova Compute API
    |
    v
Compute Runtime
    |
    v
GPU Runtime
    |
    +-- GPU Device Manager
    |
    +-- GPU Queue Manager
    |
    +-- GPU Memory Manager
    |
    +-- GPU Kernel Manager
    |
    +-- GPU Synchronization
    |
    +-- GPU Scheduler Interface
    |
    +-- GPU Recovery
    |
    v
GPU Backend
    |
    v
GPU Driver / Hardware
```

---

## 6. GPU Device

Jede GPU MUSS als Compute Device gemäß dem allgemeinen Compute Device Model repräsentiert werden.

Mindestens folgende Eigenschaften SOLLEN verfügbar sein:

```text
DeviceID
VendorID
HardwareDeviceID
DeviceName
Architecture
ComputeUnits
ShaderUnits
MaximumClock
MemoryCapacity
MemoryType
MemoryBandwidth
UnifiedMemory
DedicatedMemory
QueueCapabilities
SupportedDataTypes
SupportedAtomics
PreemptionSupport
PeerAccessSupport
HealthState
ThermalState
```

---

## 7. GPU-Klassen

Die Runtime SOLL mindestens unterscheiden können:

```text
INTEGRATED_GPU
DISCRETE_GPU
VIRTUAL_GPU
EXTERNAL_GPU
```

Optional:

```text
REMOTE_GPU
```

---

## 8. Integrated GPU

Eine integrierte GPU verwendet typischerweise gemeinsam mit der CPU denselben physischen Hauptspeicher.

Die Runtime SOLL in diesem Fall:

- Zero-Copy bevorzugen können,
- unnötige Kopien vermeiden,
- Shared Memory korrekt synchronisieren.

---

## 9. Discrete GPU

Eine diskrete GPU besitzt typischerweise eigenen VRAM.

Die Runtime MUSS daher effiziente Transfers zwischen:

```text
HOST RAM
GPU VRAM
```

unterstützen.

---

## 10. External GPU

Externe GPUs SOLLEN als Hot-Plug-fähige Devices behandelt werden können.

Beispiele:

- Thunderbolt,
- PCIe Expansion,
- externe Compute-Gehäuse.

---

## 11. GPU Capabilities

Eine GPU MUSS ihre Compute-Fähigkeiten deklarieren.

Mindestens:

```text
FP16
FP32
FP64

INT8
INT16
INT32
INT64

BF16
TF32

SIMD
VECTOR

MATRIX
TENSOR

ATOMIC32
ATOMIC64

LOCAL_MEMORY
SHARED_MEMORY

ASYNC_COPY
OUT_OF_ORDER_QUEUE

PREEMPTION
TIMESTAMPS
PEER_ACCESS
UNIFIED_MEMORY
ZERO_COPY
```

---

## 12. Capability Query

Anwendungen und Runtime-Komponenten MÜSSEN GPU Capabilities zur Laufzeit abfragen können.

---

## 13. GPU Context

Ein GPU Context MUSS in das allgemeine Compute Context Model integriert sein.

Er MUSS mindestens zugeordnet sein zu:

```text
OwnerProcess
SecurityDomain
AllowedGPUDevices
MemoryBudget
Priority
PowerPolicy
```

---

## 14. GPU Context Isolation

GPU Contexts unterschiedlicher Prozesse MÜSSEN voneinander isoliert sein.

Ein Prozess DARF nicht:

- fremden VRAM lesen,
- fremde GPU Queues manipulieren,
- fremde GPU Kernel beeinflussen,
- fremde GPU Synchronisationsobjekte verändern.

---

## 15. GPU Queue

Die Runtime MUSS GPU Queues unterstützen.

Mindestens:

```text
COMPUTE_QUEUE
TRANSFER_QUEUE
```

Optional:

```text
HIGH_PRIORITY_COMPUTE
LOW_PRIORITY_COMPUTE
ASYNC_COMPUTE
```

---

## 16. Queue Ordering

GPU Queues SOLLEN mindestens folgende Modi unterstützen können:

```text
IN_ORDER
OUT_OF_ORDER
```

Falls Hardware `OUT_OF_ORDER` nicht nativ unterstützt, DARF das Backend den Modus emulieren oder auf geordnete Ausführung abbilden.

---

## 17. Command Submission

GPU Commands SOLLEN asynchron eingereicht werden können.

Beispiel:

```text
Queue.Submit(CommandList)
```

Der Aufruf SOLL einen Event-, Fence- oder Submission-Handle zurückgeben können.

---

## 18. Command Types

Mindestens folgende GPU Commands SOLLEN unterstützt werden:

```text
DISPATCH
COPY
FILL
BARRIER
WAIT_EVENT
SIGNAL_EVENT
MAP
UNMAP
PREFETCH
MEMORY_ADVISE
```

---

## 19. Command Lists

Die GPU Runtime SOLL wiederverwendbare Command Lists unterstützen können.

Beispiel:

```text
CommandList
 |
 +-- Copy Input
 |
 +-- Dispatch Kernel A
 |
 +-- Barrier
 |
 +-- Dispatch Kernel B
 |
 +-- Copy Output
```

---

## 20. Batch Submission

Mehrere GPU Commands SOLLEN gebündelt eingereicht werden können.

Dies reduziert:

- Syscall-Overhead,
- Runtime-Overhead,
- Queue-Overhead.

---

## 21. GPU Kernel

Ein GPU Kernel repräsentiert eine auf einer GPU ausführbare Compute-Funktion.

Ein Kernel SOLL mindestens folgende Metadaten besitzen:

```text
KernelName
KernelVersion
RequiredCapabilities
ArgumentLayout
WorkgroupRequirements
LocalMemoryRequirement
PreferredWorkgroupSize
```

---

## 22. GPU Kernel Code

Die Runtime DARF GPU Kernel aus verschiedenen Repräsentationen laden.

Beispiele:

```text
Nova Compute IR
Portable IR
Native GPU Binary
Vendor Backend Binary
```

---

## 23. GPU Compilation

Die Runtime SOLL unterstützen können:

```text
AOT Compilation
JIT Compilation
```

---

## 24. Kernel Cache

Kompilierte GPU Kernel SOLLEN gecacht werden.

Ein Cache Key MUSS mindestens berücksichtigen:

```text
KernelHash
GPUArchitecture
DriverVersion
BackendVersion
CompilerVersion
CompileOptions
```

---

## 25. Workgroup Model

Die GPU Runtime MUSS ein Workgroup-Modell unterstützen.

Beispiel:

```text
GlobalSize = 1024
LocalSize  = 64
```

---

## 26. Automatische Workgroup Size

Wenn keine Local Size angegeben wurde, SOLL die Runtime oder das Backend eine geeignete Größe wählen können.

---

## 27. Workgroup Limits

Die GPU MUSS Limits deklarieren können.

Beispiele:

```text
MaxWorkgroupSize
MaxWorkgroupSizeX
MaxWorkgroupSizeY
MaxWorkgroupSizeZ
MaxLocalMemoryPerWorkgroup
```

---

## 28. Dispatch Dimensionen

Mindestens dreidimensionale Dispatches SOLLEN unterstützt werden:

```text
X
Y
Z
```

---

## 29. GPU Memory

Die GPU Runtime MUSS GPU-Speicher gemäß `NPSPEC-COMPUTE-RUNTIME-0003` verwalten.

Mindestens:

```text
HOST
DEVICE
SHARED
UNIFIED
LOCAL
CONSTANT
```

---

## 30. Dedicated VRAM

Bei diskreten GPUs MUSS dedizierter VRAM als eigener Memory Domain modelliert werden.

Beispiel:

```text
GPU0_VRAM
```

---

## 31. GPU Memory Allocation

Compute Resources SOLLEN explizit oder automatisch im GPU Memory platziert werden können.

---

## 32. GPU Residency

Die Runtime MUSS verfolgen, welche Ressourcen auf welcher GPU resident sind.

Beispiel:

```text
Buffer A -> GPU0
Buffer B -> GPU1
```

---

## 33. Host-to-GPU Transfer

Die Runtime MUSS Transfers unterstützen:

```text
HOST -> GPU
```

---

## 34. GPU-to-Host Transfer

Die Runtime MUSS unterstützen:

```text
GPU -> HOST
```

---

## 35. GPU-to-GPU Transfer

Bei mehreren GPUs MUSS unterstützt werden können:

```text
GPU0 -> GPU1
```

---

## 36. Peer-to-Peer Transfer

Wenn Hardware dies unterstützt, SOLL direkter Peer Transfer verwendet werden.

Bevorzugt:

```text
GPU0 VRAM
    |
    v
GPU1 VRAM
```

anstatt:

```text
GPU0 VRAM
    |
    v
HOST RAM
    |
    v
GPU1 VRAM
```

---

## 37. Peer Access Capability

Direkter Zugriff auf fremden GPU Memory MUSS explizit als Capability angegeben werden.

---

## 38. GPU Memory Mapping

GPU Memory SOLL host-seitig gemappt werden können, wenn das Backend dies unterstützt.

---

## 39. Staging Buffer

Falls GPU VRAM nicht direkt mapbar ist, DARF die Runtime automatisch Staging Memory verwenden.

---

## 40. Unified Memory

Die GPU Runtime SOLL Unified Memory unterstützen.

Sie MUSS unterscheiden zwischen:

```text
Shared Address Space
Shared Physical Memory
Hardware Coherency
Software Coherency
```

---

## 41. Zero-Copy

Bei integrierten GPUs SOLL Zero-Copy bevorzugt werden können.

Zero-Copy DARF jedoch nicht automatisch angenommen werden.

---

## 42. Memory Pressure

Die GPU Runtime MUSS GPU Memory Pressure melden können.

Mindestens:

```text
LOW
MODERATE
HIGH
CRITICAL
```

---

## 43. Eviction

Bei Memory Pressure DARF die Runtime GPU Resources in Host Memory verschieben.

---

## 44. Oversubscription

Die GPU Runtime DARF VRAM überbuchen, wenn Paging oder sichere Migration unterstützt wird.

---

## 45. GPU Page Fault

GPU Page Faults SOLLEN unterstützt werden können, sofern Hardware dies erlaubt.

---

## 46. GPU Fault Handling

Ein gültiger GPU Page Fault kann folgende Sequenz auslösen:

```text
Fault
 |
 v
Pause Workload
 |
 v
Resolve Page
 |
 v
Update Mapping
 |
 v
Resume
```

---

## 47. Synchronisation

Die GPU Runtime MUSS mindestens unterstützen:

```text
Event
Fence
Barrier
```

---

## 48. GPU Fence

Ein GPU Fence SOLL einen monotonen Wert verwenden können.

Beispiel:

```text
FenceValue = 42
```

---

## 49. Timeline Synchronization

Eine vollständige Runtime SOLL Timeline-basierte GPU-Synchronisation unterstützen können.

---

## 50. GPU Barrier

Barriers MÜSSEN Ausführungs- und Speicherabhängigkeiten korrekt abbilden können.

---

## 51. Memory Barrier

Mindestens folgende Übergänge SOLLEN unterstützt werden können:

```text
HOST_TO_GPU
GPU_TO_HOST
GPU_TO_GPU
QUEUE_TO_QUEUE
```

---

## 52. Compute/Transfer Overlap

Die Runtime SOLL GPU Compute und Transfers parallel ausführen können.

Beispiel:

```text
Compute Queue:  [Kernel A][Kernel B]

Transfer Queue: [Upload B][Download A]
```

---

## 53. Async Copy Engines

Falls eine GPU separate Copy Engines besitzt, SOLL die Runtime diese verwenden können.

---

## 54. GPU Scheduling

GPU Workloads MÜSSEN über den Compute Scheduler gemäß `NPSPEC-COMPUTE-RUNTIME-0002` eingeplant werden können.

---

## 55. GPU Scheduling Inputs

Mindestens folgende Werte SOLLEN berücksichtigt werden:

```text
GPU Load
QueueDepth
MemoryPressure
DataResidency
TransferCost
ThermalState
PowerState
Priority
Deadline
```

---

## 56. GPU Load

Die Runtime MUSS aktuelle GPU-Auslastung erfassen können.

---

## 57. Queue Depth

Der Scheduler SOLL die aktuelle Queue-Tiefe berücksichtigen.

---

## 58. GPU Affinity

Jobs DÜRFEN an eine bevorzugte GPU gebunden werden.

Beispiel:

```text
PreferredDevice = GPU0
```

---

## 59. Required GPU

Ein Job DARF zwingend eine bestimmte GPU verlangen.

Beispiel:

```text
RequiredDevice = GPU1
```

Ist diese GPU nicht verfügbar, MUSS der Job fehlschlagen.

---

## 60. Multi-GPU Scheduling

Die Runtime SOLL mehrere GPUs parallel verwenden können.

---

## 61. Multi-GPU Workload Splitting

Teilbare Workloads DÜRFEN auf mehrere GPUs verteilt werden.

Beispiel:

```text
Workload
 |
 +--> GPU0
 |
 +--> GPU1
```

---

## 62. Split Requirements

Ein Job MUSS explizit teilbar sein oder durch die Runtime als sicher teilbar erkannt werden.

---

## 63. Multi-GPU Synchronisation

Abhängigkeiten zwischen mehreren GPUs MÜSSEN korrekt synchronisiert werden.

---

## 64. Multi-GPU Topology

Die Runtime SOLL GPU-Topologie berücksichtigen.

Beispiele:

```text
PCIe
NVLink-like Interconnect
Shared Root Complex
Shared Memory Domain
```

---

## 65. Topology-aware Scheduling

Der Scheduler SOLL GPUs mit günstiger Verbindung bevorzugen können.

---

## 66. GPU Preemption

Wenn eine GPU Preemption unterstützt, SOLL die Runtime sie verwenden können.

---

## 67. Preemption Capability

Die GPU MUSS die Granularität deklarieren können.

Beispiele:

```text
QUEUE
DISPATCH
WORKGROUP
WAVE
INSTRUCTION
```

---

## 68. Non-preemptive GPU

Bei GPUs ohne geeignete Preemption SOLL die Runtime sehr große Dispatches vermeiden oder aufteilen können.

---

## 69. GPU Time Slicing

Mehrere Compute Contexts DÜRFEN über Time Slicing dieselbe GPU verwenden.

---

## 70. Fairness

Eine Anwendung DARF die GPU nicht dauerhaft monopolieren.

---

## 71. Foreground Priority

Interaktive GPU-Compute-Aufgaben SOLLEN gegenüber Background Compute bevorzugt werden können.

---

## 72. Graphics Coordination

Wenn dieselbe GPU für Rendering und Compute verwendet wird, MUSS die Compute Runtime mit dem Grafik-Scheduler kooperieren.

---

## 73. Desktop Responsiveness

Normale Compute Workloads DÜRFEN die Desktop-Darstellung nicht dauerhaft blockieren.

---

## 74. Graphics Reserve

NovaOS DARF einen Teil der GPU-Kapazität für Grafik und Compositor reservieren.

---

## 75. Async Compute

GPUs mit separaten Async-Compute-Einheiten SOLLEN diese nutzen können.

---

## 76. Concurrent Kernels

Mehrere unabhängige GPU Kernel DÜRFEN gleichzeitig ausgeführt werden, wenn Hardware und Backend dies erlauben.

---

## 77. Occupancy

Die GPU Runtime SOLL Hardwareinformationen zur möglichen Kernel Occupancy berücksichtigen können.

---

## 78. Local Memory Usage

Hoher Local-Memory-Verbrauch kann die Parallelität reduzieren.

Die Runtime SOLL diese Information berücksichtigen können.

---

## 79. Register Pressure

Backend und Compiler DÜRFEN Register Pressure für Scheduling und Kernel-Optimierung berücksichtigen.

---

## 80. Power Policy

GPU Compute MUSS in das NovaOS-Power-Modell integriert sein.

Mindestens:

```text
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 81. Performance Mode

Im Performance-Modus DARF die Runtime:

- leistungsfähigere GPU bevorzugen,
- höhere Performance States verwenden,
- mehr parallele Queues aktivieren.

---

## 82. Efficiency Mode

Im Efficiency-Modus DARF eine integrierte GPU gegenüber einer diskreten GPU bevorzugt werden, wenn die Performance ausreichend ist.

---

## 83. Battery Saver

Im Battery-Saver-Modus SOLL diskrete GPU Compute-Ausführung reduziert oder vermieden werden können.

---

## 84. GPU Power State

Die Runtime SOLL den aktuellen Power State berücksichtigen.

Beispiele:

```text
ACTIVE
IDLE
LOW_POWER
SUSPENDED
```

---

## 85. Wake Cost

Das Aktivieren einer schlafenden diskreten GPU kann zusätzliche Latenz und Energie verursachen.

Der Scheduler SOLL diesen Wake Cost berücksichtigen.

---

## 86. Thermal State

Die GPU Runtime MUSS thermische Zustände berücksichtigen können.

Mindestens:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

---

## 87. Thermal Throttling

Eine gedrosselte GPU DARF nicht anhand ihrer nominellen Spitzenleistung bewertet werden.

---

## 88. Thermal Migration

Bei mehreren GPUs DARF der Scheduler neue Workloads auf eine kühlere GPU verschieben.

---

## 89. GPU Health State

Die GPU Runtime SOLL mindestens folgende Health States modellieren:

```text
HEALTHY
DEGRADED
UNSTABLE
FAILED
RESETTING
LOST
```

---

## 90. Device Lost

Ein GPU Device Loss MUSS kontrolliert behandelt werden.

---

## 91. Device-Lost-Ablauf

Ein typischer Ablauf lautet:

```text
GPU Error
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

## 92. GPU Reset

Ein GPU Reset DARF bei folgenden Situationen ausgelöst werden:

- Hardware Hang,
- Watchdog Timeout,
- Treiberfehler,
- Memory Fault,
- Security Violation.

---

## 93. Reset Isolation

Ein GPU Reset SOLL den Betrieb anderer Compute Devices nicht unnötig beeinträchtigen.

---

## 94. Retry-Safe Jobs

Jobs mit:

```text
RETRY_SAFE
```

DÜRFEN nach erfolgreichem GPU Reset erneut geplant werden.

---

## 95. Non-Retry-Safe Jobs

Jobs ohne `RETRY_SAFE` DÜRFEN nicht automatisch erneut ausgeführt werden.

---

## 96. Watchdog

Die GPU Runtime SOLL einen Watchdog für lang laufende oder blockierende GPU Workloads besitzen.

---

## 97. Timeout

Ein GPU Job DARF ein Timeout besitzen.

Beispiel:

```text
Timeout = 2000 ms
```

---

## 98. Long-running Kernels

Lang laufende GPU Kernel SOLLEN in kleinere Dispatches aufgeteilt werden können, wenn dies möglich ist.

---

## 99. Security

GPU Compute MUSS dem allgemeinen NovaOS-Sicherheitsmodell folgen.

---

## 100. GPU Memory Isolation

Fremde GPU Memory Regions DÜRFEN nicht lesbar oder beschreibbar sein.

---

## 101. GPU Context Switching

Bei Context-Wechsel MUSS verhindert werden, dass Daten anderer Contexts sichtbar bleiben.

---

## 102. GPU Memory Sanitization

GPU Memory MUSS vor Cross-Context-Reuse bereinigt werden, wenn Hardware dies nicht bereits garantiert.

---

## 103. Kernel Validation

GPU Kernel MÜSSEN vor Ausführung validiert werden.

Mindestens:

```text
Format
Architecture
Capabilities
Resource Requirements
Argument Layout
Security Policy
```

---

## 104. JIT Security

JIT-generierter GPU Code MUSS den Sicherheitsrichtlinien der Compute Runtime unterliegen.

---

## 105. Shader/Kernel Isolation

Ein fehlerhafter Compute Kernel DARF nicht unkontrolliert auf Host- oder fremden Device-Speicher zugreifen.

---

## 106. Privileged GPU Features

Bestimmte Funktionen DÜRFEN privilegiert sein.

Beispiele:

```text
Performance Counters
Exclusive Queue
Device Reset
Protected Memory
Low-Level Hardware Access
```

---

## 107. Performance Counter Security

GPU Performance Counter DÜRFEN eingeschränkt werden, wenn sie Side-Channel-Informationen über fremde Prozesse liefern können.

---

## 108. Profiling

Die GPU Runtime SOLL Profiling unterstützen.

Mindestens:

```text
QueueWaitTime
DispatchTime
ExecutionTime
TransferTime
MemoryBandwidth
GPUUtilization
```

---

## 109. Hardware Timestamps

Wenn unterstützt, SOLLEN GPU Hardware Timestamps verwendet werden können.

---

## 110. Timestamp Domain

GPU Timestamp-Domänen SOLLEN mit Host-Timestamps korrelierbar sein.

---

## 111. GPU Trace Events

Mindestens folgende Trace Events SOLLEN verfügbar sein:

```text
GPU_DEVICE_REGISTER
GPU_CONTEXT_CREATE
GPU_QUEUE_CREATE
GPU_MEMORY_ALLOC
GPU_MEMORY_FREE
GPU_KERNEL_COMPILE
GPU_DISPATCH_SUBMIT
GPU_DISPATCH_START
GPU_DISPATCH_END
GPU_DEVICE_LOST
GPU_DEVICE_RESET
```

---

## 112. GPU Metrics

Pro GPU SOLLEN mindestens verfügbar sein:

```text
ComputeUtilization
MemoryUtilization
MemoryUsed
MemoryFree
QueueDepth
Temperature
PowerState
HealthState
```

---

## 113. Transfer Metrics

Mindestens:

```text
HostToGPUBytes
GPUToHostBytes
GPUToGPUBytes
TransferBandwidth
TransferLatency
```

---

## 114. Compilation Metrics

Optional:

```text
KernelCompilationTime
PipelineCreationTime
CacheHitRate
```

---

## 115. GPU Backend

Die GPU Runtime MUSS über ein standardisiertes Backend Interface verfügen.

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
compile_kernel()
dispatch()
copy()
signal()
wait()
reset_device()
shutdown()
```

---

## 116. Backend Versioning

Das GPU Backend ABI MUSS versioniert sein.

Beispiel:

```text
NovaGPUBackendABI 1.0
```

---

## 117. Capability Negotiation

Runtime und GPU Backend MÜSSEN Features explizit aushandeln.

---

## 118. Backend Independence

Die öffentliche Nova Compute API DARF nicht davon abhängen, ob das Backend intern verwendet:

```text
Vulkan
OpenCL
CUDA
ROCm
DirectCompute
Metal
Native Driver Interface
```

---

## 119. Vendor Backend

Hersteller DÜRFEN optimierte GPU Backends bereitstellen.

Diese MÜSSEN die Sicherheits- und ABI-Regeln der Nova GPU Runtime einhalten.

---

## 120. Generic Backend

NovaOS SOLL mindestens einen generischen GPU Backend-Pfad unterstützen können, sofern die Plattform dies ermöglicht.

---

## 121. Software Fallback

Wenn keine GPU verfügbar ist, MUSS die allgemeine Compute Runtime auf CPU Compute zurückfallen können, sofern der Job dies erlaubt.

---

## 122. GPU Fallback

Ein automatischer Fallback kann lauten:

```text
GPU0
 |
 v
GPU1
 |
 v
CPU SIMD
 |
 v
CPU Scalar
```

---

## 123. Fallback Policy

Jobs MÜSSEN angeben können:

```text
ALLOW_FALLBACK
NO_FALLBACK
```

---

## 124. Graphics Interop

GPU Compute Resources SOLLEN direkt mit Grafikressourcen geteilt werden können.

Beispiel:

```text
Graphics Texture
      |
      v
Compute Image
```

---

## 125. Zero-Copy Graphics Interop

Eine unnötige Kopie zwischen Grafik- und Compute-Ressource SOLL vermieden werden.

---

## 126. Synchronisation mit Graphics

Compute und Grafik MÜSSEN gemeinsame Synchronisationsmechanismen verwenden können.

Beispiel:

```text
Render Fence
    |
    v
Compute Wait
```

---

## 127. Video Interop

GPU Compute SOLL mit Video Decode und Encode zusammenarbeiten können.

Beispiel:

```text
Video Decode
     |
     v
GPU Compute Filter
     |
     v
Video Encode
```

---

## 128. Camera Interop

Kamera-Frames SOLLEN direkt in GPU Compute Pipelines verwendet werden können.

---

## 129. AI Interop

GPU Tensor Resources SOLLEN ohne unnötige Kopien mit höheren AI-Runtimes geteilt werden können.

---

## 130. Tensor Acceleration

GPUs mit Tensor- oder Matrix-Einheiten SOLLEN diese als Capabilities deklarieren.

---

## 131. Sparse Resources

Eine vollständige GPU Runtime SOLL Sparse Resources unterstützen können.

---

## 132. Sparse Residency

Nur tatsächlich benötigte Teile großer GPU Resources MÜSSEN resident sein.

---

## 133. GPU Virtual Memory

GPUs mit virtuellem Speicher SOLLEN sicher in das Compute Memory Model integriert werden.

---

## 134. Device Page Tables

Device Page Tables sind Backend-Details.

Die Runtime MUSS jedoch sichere Isolation verlangen.

---

## 135. Shared Virtual Memory

Falls unterstützt, SOLL Shared Virtual Memory verwendet werden können.

---

## 136. Huge Pages

Große GPU Allocations DÜRFEN größere Pages verwenden.

---

## 137. Memory Pools

GPU Memory Pools SOLLEN unterstützt werden können.

Beispiel:

```text
DEVICE_POOL
UPLOAD_POOL
DOWNLOAD_POOL
TEMPORARY_POOL
```

---

## 138. Suballocation

Große VRAM Blocks DÜRFEN intern in kleinere Allocations aufgeteilt werden.

---

## 139. Defragmentation

GPU Memory DARF defragmentiert werden, wenn Ressourcen sicher migrierbar sind.

---

## 140. Transient Resources

Kurzlebige GPU Resources SOLLEN über effiziente Transient Pools verwaltet werden können.

---

## 141. Resource Aliasing

Nicht gleichzeitig verwendete temporäre GPU Resources DÜRFEN denselben physischen Speicher verwenden.

---

## 142. Alias Safety

Aliasing DARF nur erfolgen, wenn Ressourcenlebenszeiten sich nicht überlappen.

---

## 143. Compute Graph Integration

GPU Workloads SOLLEN in Compute Graphs eingebunden werden können.

---

## 144. Graph Optimization

Die Runtime DARF GPU-spezifische Graph-Optimierungen durchführen.

Beispiele:

```text
Kernel Fusion
Transfer Elimination
Resource Reuse
Queue Parallelism
```

---

## 145. Pipeline Precompilation

GPU Pipelines DÜRFEN vorkompiliert und gecacht werden.

---

## 146. Persistent Kernel

GPUs DÜRFEN Persistent Kernels unterstützen.

Dies MUSS als Capability angegeben werden.

---

## 147. Cooperative Kernels

Cooperative oder global synchronisierende Kernels DÜRFEN als optionale Capability unterstützt werden.

---

## 148. Atomics

Die GPU Runtime SOLL atomare Operationen unterstützen können.

Mindestens optional:

```text
ATOMIC32
ATOMIC64
```

---

## 149. Atomic Scope

Atomics SOLLEN Scope-Information besitzen können.

Beispiele:

```text
WORKGROUP
DEVICE
SYSTEM
```

---

## 150. Cross-GPU Atomics

Cross-GPU Atomics DÜRFEN nur angeboten werden, wenn Hardware und Interconnect dies tatsächlich unterstützen.

---

## 151. Memory Coherency

Die GPU Runtime MUSS unterscheiden können zwischen:

```text
COHERENT
NON_COHERENT
PARTIALLY_COHERENT
```

---

## 152. Cache Maintenance

Hardware-spezifische Cache Flushes und Invalidations SOLLEN durch Runtime oder Backend gekapselt werden.

---

## 153. Resource State

GPU Resources DÜRFEN logische Nutzungszustände besitzen.

Beispiele:

```text
GPU_READ
GPU_WRITE
TRANSFER_SOURCE
TRANSFER_DESTINATION
GRAPHICS_READ
GRAPHICS_WRITE
```

---

## 154. Resource State Transition

Die Runtime MUSS notwendige State Transitions durchführen können.

Beispiel:

```text
GRAPHICS_WRITE
      |
      v
GPU_READ
```

---

## 155. Automatic Transitions

Normale Anwendungen SOLLEN automatische Resource Transitions verwenden können.

---

## 156. Explicit Transitions

Performancekritische Anwendungen DÜRFEN Resource Transitions explizit steuern.

---

## 157. Fast Submission Path

Für bekannte GPU Queues und vorbereitete Pipelines SOLL ein Fast Path existieren.

Beispiel:

```text
Known Queue
+
Known Kernel
+
Resident Resources
=
Fast Dispatch
```

---

## 158. Slow Path

Komplexe Fälle DÜRFEN einen Slow Path verwenden.

Beispiele:

- Device Migration,
- GPU Reset,
- neue Kernel Compilation,
- Memory Eviction,
- Multi-GPU Planning.

---

## 159. Thread Safety

Die öffentliche GPU Runtime MUSS Thread-safe nutzbar sein.

Mehrere Threads DÜRFEN gleichzeitig:

- Queues erzeugen,
- Jobs einreichen,
- Buffers erzeugen,
- Kernel verwenden.

---

## 160. Concurrent Submission

Mehrere Threads DÜRFEN gleichzeitig auf unterschiedliche GPU Queues submitten.

---

## 161. Same-Queue Submission

Für Submission auf dieselbe Queue MUSS eine definierte Thread-Safety-Semantik existieren.

---

## 162. Process Exit

Beim Prozessende MUSS die GPU Runtime sämtliche zugehörigen:

```text
Contexts
Queues
Resources
Kernels
Jobs
Events
Fences
```

kontrolliert bereinigen.

---

## 163. Crash Cleanup

Ein abgestürzter Prozess DARF keine dauerhaft belegten GPU Resources hinterlassen.

---

## 164. Suspend

Vor System-Suspend MUSS die GPU Runtime aktive Compute Workloads behandeln.

Mögliche Aktionen:

```text
COMPLETE
PAUSE
CHECKPOINT
CANCEL
```

---

## 165. Resume

Nach Resume MUSS die GPU Runtime:

- Device State prüfen,
- Memory Residency validieren,
- Queues rekonstruieren,
- verlorene Resources erkennen.

---

## 166. Hibernate

GPU Resources MÜSSEN nicht zwingend vollständig in den Hibernate-Stand geschrieben werden.

Regenerierbare Ressourcen DÜRFEN nach Resume neu erzeugt werden.

---

## 167. Hot-Plug

Neue GPUs SOLLEN nach erfolgreicher Initialisierung dynamisch registriert werden können.

---

## 168. Hot-Unplug

Vor geplantem GPU Removal SOLL folgender Ablauf möglich sein:

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

## 169. Testing — Device Enumeration

Mindestens:

```text
Integrated GPU
Discrete GPU
Multiple GPUs
External GPU
Virtual GPU
No GPU
```

---

## 170. Testing — Capability Detection

Zu testen sind:

```text
FP16
FP32
FP64
Matrix
Tensor
Atomics
Peer Access
Unified Memory
Preemption
```

---

## 171. Testing — Queue Management

Zu testen sind:

```text
Create Queue
Destroy Queue
Multiple Queues
Compute Queue
Transfer Queue
High Priority Queue
Invalid Queue
```

---

## 172. Testing — Kernel Dispatch

Mindestens:

```text
Small Dispatch
Large Dispatch
1D Dispatch
2D Dispatch
3D Dispatch
Invalid Workgroup
Unsupported Capability
```

---

## 173. Testing — Memory

Zu testen sind:

```text
GPU Allocation
Host Allocation
Unified Memory
Zero-Copy
Mapping
Unmapping
Memory Pressure
Eviction
```

---

## 174. Testing — Transfers

Mindestens:

```text
Host -> GPU
GPU -> Host
GPU0 -> GPU1
Peer Transfer
Fallback via Host
Async Transfer
```

---

## 175. Testing — Synchronisation

Zu testen sind:

```text
Event
Fence
Timeline
Barrier
Cross-Queue
Cross-GPU
Graphics Interop
```

---

## 176. Testing — Multi-GPU

Mindestens:

```text
GPU0 + GPU1

Load Distribution

Data Residency

Peer Copy

Job Splitting

Device Loss
```

---

## 177. Testing — Preemption

Zu testen sind:

```text
Background -> Interactive
Resume after Preemption
Repeated Preemption
GPU without Preemption
```

---

## 178. Testing — Device Lost

Zu testen sind:

```text
GPU Lost before Submit
GPU Lost during Dispatch
GPU Reset
Queue Failure
Resource Lost
Retry-Safe Job
Fallback
```

---

## 179. Testing — Memory Fault

Mindestens:

```text
Out-of-Bounds
Invalid Mapping
Page Fault
Missing Residency
Device Memory Fault
```

---

## 180. Testing — Security

Zu testen sind:

```text
Cross-Context VRAM Access
Cross-Process Handle
Kernel Memory Violation
Performance Counter Access
Unauthorized Export
```

---

## 181. Testing — Graphics Coordination

GPU Compute unter gleichzeitigem Desktop-Rendering MUSS getestet werden.

Der Desktop DARF unter normaler Compute Last nicht dauerhaft unbenutzbar werden.

---

## 182. Testing — Power

Zu testen sind:

```text
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 183. Testing — Thermal

Zu testen sind:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

---

## 184. Stress Tests

Stress Tests SOLLEN umfassen:

```text
Thousands of Queues
Millions of Dispatches
Continuous Allocation
Continuous Free
Continuous Transfers
Multiple Processes
Multiple GPUs
GPU Reset under Load
```

---

## 185. Fuzzing

Folgende GPU Runtime Inputs SOLLTEN fuzz-getestet werden:

```text
Kernel Metadata
Queue Descriptors
Memory Descriptors
Dispatch Descriptors
Barrier Descriptors
Import / Export Descriptors
```

---

## 186. Fault Injection

Mindestens folgende Fehler SOLLEN injizierbar sein:

```text
DEVICE_LOST
DEVICE_RESET
OUT_OF_MEMORY
KERNEL_COMPILE_FAILED
DISPATCH_FAILED
TRANSFER_FAILED
PAGE_FAULT_FAILED
QUEUE_FAILED
```

---

## 187. Performance Tests

Mindestens folgende Werte SOLLEN gemessen werden:

```text
QueueCreationLatency
DispatchLatency
KernelLaunchLatency
TransferLatency
TransferBandwidth
MemoryAllocationLatency
SynchronizationLatency
```

---

## 188. Regression Tests

Änderungen an GPU Runtime oder Backend SOLLEN mindestens hinsichtlich folgender Werte verglichen werden:

```text
Latency
Throughput
MemoryUsage
TransferVolume
GPUUtilization
PowerUsage
ThermalBehavior
DeviceResetRate
```

---

## 189. Minimalimplementierung

Eine minimale konforme GPU Runtime MUSS mindestens unterstützen:

```text
Single GPU Device
GPU Context
Compute Queue
GPU Buffer
Host-to-GPU Transfer
GPU-to-Host Transfer
Kernel Dispatch
Event
Fence
Basic Device Lost Handling
```

---

## 190. Erweiterte Implementierung

Eine vollständige GPU Runtime SOLL zusätzlich unterstützen:

```text
Multiple GPUs
Transfer Queues
Async Compute
Unified Memory
Zero-Copy
Peer-to-Peer
GPU Virtual Memory
Sparse Resources
Timeline Synchronization
Preemption
Power-Aware Scheduling
Thermal-Aware Scheduling
Graphics Interop
Video Interop
AI Interop
Device Recovery
Profiling
```

---

## 191. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0005` MUSS:

1. mindestens eine GPU als Compute Device abstrahieren können.
2. GPU Capabilities abfragbar machen.
3. GPU Contexts unterstützen.
4. GPU Queue Submission unterstützen.
5. GPU Compute Kernel dispatchen können.
6. GPU Buffers verwalten.
7. Host-to-GPU Transfers unterstützen.
8. GPU-to-Host Transfers unterstützen.
9. Synchronisationsobjekte bereitstellen.
10. Context-Isolation gewährleisten.
11. GPU Memory Isolation gewährleisten.
12. Device Lost kontrolliert behandeln.
13. Jobs mit dem Compute Scheduler integrieren.
14. GPU Memory mit dem Compute Memory Model integrieren.
15. GPU Objects mit dem Compute Object Model integrieren.
16. Thread-safe Submission ermöglichen.
17. Resource Limits unterstützen.
18. GPU Errors definiert melden.
19. Performance- und Diagnoseinformationen bereitstellen.
20. zukünftige GPU Backends erweiterbar halten.

---

## 192. Empfohlene interne Komponenten

Eine vollständige GPU Runtime SOLL logisch folgende Komponenten besitzen können:

```text
NovaGPURuntime
 |
 +-- GPUDeviceManager
 |
 +-- GPUContextManager
 |
 +-- GPUQueueManager
 |
 +-- GPUCommandManager
 |
 +-- GPUKernelManager
 |
 +-- GPUMemoryManager
 |
 +-- GPUSynchronizationManager
 |
 +-- GPUSchedulerAdapter
 |
 +-- GPUPowerManager
 |
 +-- GPUThermalManager
 |
 +-- GPURecoveryManager
 |
 +-- GPUDiagnostics
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei- oder Klassenstruktur vor.

---

## 193. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

`NPSPEC-COMPUTE-RUNTIME-0001` definiert die allgemeine Compute Runtime.

Die GPU Runtime implementiert GPU-spezifische Ausführung innerhalb dieses Modells.

```text
Compute Runtime
      |
      v
GPU Runtime
      |
      v
GPU Backend
      |
      v
GPU Hardware
```

---

## 194. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler entscheidet unter anderem, ob ein Workload auf einer GPU ausgeführt werden soll.

```text
Compute Job
    |
    v
Scheduler
    |
    +--> CPU
    |
    +--> GPU
    |
    +--> NPU
```

Wird eine GPU gewählt, übernimmt die GPU Runtime die gerätespezifische Ausführung.

---

## 195. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das GPU Memory Management basiert auf dem allgemeinen Compute Memory Model.

```text
Logical Resource
      |
      v
Compute Memory Model
      |
      v
GPU Memory Manager
      |
      v
VRAM / Shared Memory
```

---

## 196. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Alle GPU-spezifischen Runtime Objects MÜSSEN die Regeln des Compute Object Models einhalten.

Beispiele:

```text
GPU Device Object
GPU Queue Object
GPU Buffer Object
GPU Kernel Object
GPU Event Object
```

---

## 197. Empfohlene Folge-Spezifikationen

Auf dieser Spezifikation SOLLEN weitere Detail-Spezifikationen aufbauen können.

Empfohlen:

```text
NPSPEC-COMPUTE-GPU-DEVICE-0001
GPU Device Model

NPSPEC-COMPUTE-GPU-CONTEXT-0001
GPU Context

NPSPEC-COMPUTE-GPU-QUEUE-0001
GPU Queues

NPSPEC-COMPUTE-GPU-COMMAND-0001
GPU Command Submission

NPSPEC-COMPUTE-GPU-KERNEL-0001
GPU Kernel Model

NPSPEC-COMPUTE-GPU-MEMORY-0001
GPU Memory

NPSPEC-COMPUTE-GPU-SYNC-0001
GPU Synchronization

NPSPEC-COMPUTE-GPU-MULTI-0001
Multi-GPU Runtime

NPSPEC-COMPUTE-GPU-PEER-0001
GPU Peer Access

NPSPEC-COMPUTE-GPU-PREEMPT-0001
GPU Preemption

NPSPEC-COMPUTE-GPU-POWER-0001
GPU Power Management

NPSPEC-COMPUTE-GPU-THERMAL-0001
GPU Thermal Management

NPSPEC-COMPUTE-GPU-RECOVERY-0001
GPU Recovery

NPSPEC-COMPUTE-GPU-PROFILE-0001
GPU Profiling

NPSPEC-COMPUTE-GPU-INTEROP-0001
GPU Resource Interoperability

NPSPEC-COMPUTE-GPU-SECURITY-0001
GPU Security

NPSPEC-COMPUTE-GPU-TEST-0001
GPU Runtime Conformance Tests
```

---

## 198. Zusammenfassung

Die NovaOS GPU Runtime bildet die hardwareunabhängige Ausführungsschicht für GPU-basierte Compute Workloads.

Die grundlegende Architektur lautet:

```text
Application
    |
    v
Nova Compute API
    |
    v
Compute Runtime
    |
    v
GPU Runtime
    |
    +-- Device Management
    |
    +-- Queue Management
    |
    +-- Kernel Management
    |
    +-- Memory Management
    |
    +-- Synchronization
    |
    +-- Scheduling Integration
    |
    +-- Power / Thermal
    |
    +-- Recovery
    |
    v
GPU Backend
    |
    v
GPU Hardware
```

Die GPU Runtime abstrahiert dabei sowohl:

```text
Integrated GPU
Discrete GPU
External GPU
Virtual GPU
Multi-GPU
```

und stellt ein einheitliches Compute-Modell bereit.

Ein typischer GPU Compute Ablauf lautet:

```text
Create Context
      |
      v
Select GPU
      |
      v
Create Queue
      |
      v
Create / Import Resources
      |
      v
Prepare Kernel
      |
      v
Submit Dispatch
      |
      v
GPU Execution
      |
      v
Signal Fence
      |
      v
Consume Result
```

Die GPU Runtime muss dabei nicht nur reine Rechenleistung berücksichtigen.

Sie ist ebenso verantwortlich für:

```text
Memory Residency
Transfer Cost
Synchronization
Queue Load
Graphics Coordination
Power
Thermals
Security
Recovery
```

Dadurch kann NovaOS GPU Compute als allgemeine Systemfähigkeit bereitstellen, ohne Anwendungen dauerhaft an eine einzelne Hersteller-API oder GPU-Generation zu binden.

Das grundlegende Architekturprinzip lautet:

> Die GPU ist in NovaOS kein isolierter Spezialbeschleuniger mit eigener Parallelwelt, sondern ein vollständig integriertes Compute Device innerhalb einer gemeinsamen, hardwareunabhängigen Compute Runtime.