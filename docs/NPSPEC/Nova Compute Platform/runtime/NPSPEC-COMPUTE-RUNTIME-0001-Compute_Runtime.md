# NPSPEC-COMPUTE-RUNTIME-0001 — Compute Runtime

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS Compute Runtime  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0001`

## 1. Zweck

Diese Spezifikation definiert die grundlegende **Compute Runtime** von NovaOS.

Die Compute Runtime stellt eine einheitliche Laufzeitumgebung für allgemeine und beschleunigte Berechnungen bereit. Sie abstrahiert die konkrete Ausführungshardware gegenüber Anwendungen, Systemdiensten, Bibliotheken und Nova-Fähigkeiten.

Die Runtime bildet die gemeinsame Ausführungsschicht für:

- CPU-basierte Berechnungen,
- SIMD- und Vektorberechnungen,
- Multi-Core-Verarbeitung,
- GPU-Compute,
- integrierte GPUs,
- Neural Processing Units,
- DSPs,
- spezialisierte AI-Beschleuniger,
- andere allgemeine Compute-Beschleuniger,
- zukünftige heterogene Recheneinheiten.

Die Compute Runtime DARF nicht von einem bestimmten Hardwarehersteller oder einer bestimmten proprietären Compute-API abhängig sein.

Der zentrale Grundsatz lautet:

> Eine Anwendung beschreibt die auszuführende Berechnung und deren Anforderungen. Die Compute Runtime bestimmt anhand von Fähigkeiten, Datenlokalität, aktueller Auslastung, Energiebedarf, thermischem Zustand und Systemrichtlinien die geeignete Recheneinheit.

---

## 2. Ziele

Die Compute Runtime MUSS folgende Ziele verfolgen:

1. Hardwareunabhängige Compute-Ausführung.
2. Einheitliche Abstraktion unterschiedlicher Recheneinheiten.
3. Automatische Auswahl geeigneter Compute Devices.
4. Explizite Gerätewahl für spezialisierte Anwendungen.
5. Asynchrone Ausführung.
6. Parallele Ausführung auf mehreren Devices.
7. Geringer Runtime-Overhead.
8. Sichere Isolation zwischen Prozessen.
9. Energie- und temperaturbewusstes Scheduling.
10. Kontrollierte Fehlerbehandlung.
11. Device-Fallback.
12. Device-Recovery.
13. Erweiterbarkeit für zukünftige Hardware.
14. Einheitliches Diagnose- und Profiling-Modell.
15. Integration mit dem NovaOS-Prozess-, Speicher-, Sicherheits- und Power-Modell.

---

## 3. Nicht-Ziele

Die Compute Runtime ist nicht:

- der allgemeine NovaOS-Prozess-Scheduler,
- ein vollständiger Grafikrenderer,
- ein Machine-Learning-Framework,
- eine Programmiersprache,
- ein vollständiges Compiler-Framework,
- ein Gerätetreiber,
- ein Video- oder Audioframework,
- ein verteiltes Cluster-System.

Diese Systeme DÜRFEN jedoch auf der Compute Runtime aufbauen.

---

## 4. Architektur

Die grundlegende Architektur lautet:

```text
Application / Nova Capability
            |
            v
+-----------------------------+
| Nova Compute API            |
+-----------------------------+
            |
            v
+-----------------------------+
| Nova Compute Runtime        |
|                             |
| - Device Discovery          |
| - Context Management        |
| - Scheduler                 |
| - Queue Management          |
| - Memory Management         |
| - Synchronization           |
| - Policy Engine             |
| - Recovery                  |
| - Diagnostics               |
+-----------------------------+
            |
            v
+-----------------------------+
| Compute Backend Interface   |
+-----------------------------+
      |       |       |       |
      v       v       v       v
     CPU     GPU     NPU     DSP
      |       |       |       |
      v       v       v       v
   Drivers / HAL / Hardware
```

---

## 5. Grundbegriffe

### 5.1 Compute Runtime

Die systemweite Laufzeitumgebung zur Verwaltung und Ausführung von Compute-Aufgaben.

### 5.2 Compute Device

Eine physische oder virtuelle Recheneinheit, die Compute Workloads ausführen kann.

### 5.3 Compute Context

Eine logische Runtime-Umgebung, welche Ressourcen, Geräte, Queues und Sicherheitsgrenzen einer Anwendung zusammenfasst.

### 5.4 Compute Queue

Eine Warteschlange für Compute Commands.

### 5.5 Compute Job

Eine logisch zusammengehörige Compute-Aufgabe.

### 5.6 Compute Command

Eine einzelne auszuführende Operation.

Beispiele:

- Kernel Dispatch,
- Speichertransfer,
- Speicherinitialisierung,
- Barrier,
- Synchronisation.

### 5.7 Compute Kernel

Eine auf einem Compute Device ausführbare Berechnungsfunktion.

Der Begriff `Compute Kernel` ist nicht mit dem NovaOS-Systemkernel gleichzusetzen.

### 5.8 Compute Resource

Ein Speicher- oder Datenobjekt, das einer Compute-Ausführung zur Verfügung gestellt wird.

### 5.9 Compute Event

Ein Objekt zur Synchronisation und Statusverfolgung asynchroner Compute-Operationen.

### 5.10 Fence

Ein monoton fortschreitendes Synchronisationsobjekt.

---

## 6. Compute Device Model

Jedes erkannte Compute Device MUSS durch ein Runtime-Objekt repräsentiert werden.

Ein Device SOLL mindestens folgende Informationen besitzen:

```text
DeviceID
DeviceClass
VendorID
HardwareDeviceID
DeviceName
Architecture
DriverName
DriverVersion
BackendName
BackendVersion
ComputeUnits
MemoryCapacity
MemoryType
UnifiedMemory
DedicatedMemory
SupportedFeatures
PowerClass
ThermalState
Availability
HealthState
```

---

## 7. Device-Klassen

Die Runtime MUSS mindestens folgende Device-Klassen modellieren können:

```text
CPU
GPU
NPU
DSP
FPGA
ACCELERATOR
VIRTUAL
REMOTE
UNKNOWN
```

`REMOTE` ist für zukünftige Remote-Compute-Erweiterungen reserviert.

Eine Implementierung dieser Funktion ist für Version 1 der Compute Runtime nicht verpflichtend.

---

## 8. Device-Identifikation

Jedes Compute Device MUSS innerhalb des laufenden Systems eindeutig identifizierbar sein.

Eine Device-ID DARF nicht ausschließlich aus der Reihenfolge der Geräteerkennung bestehen.

Folgendes ist daher nicht als dauerhafte Identifikation ausreichend:

```text
GPU0
GPU1
GPU2
```

Die Runtime SOLL stabile Hardwareinformationen verwenden.

Beispiele:

```text
compute://pci/0000:01:00.0
```

oder:

```text
5c88f604-74d0-44ca-96eb-b74baad78001
```

Die interne Darstellung der Device-ID ist implementierungsabhängig.

---

## 9. Device Capabilities

Jedes Device MUSS seine unterstützten Fähigkeiten deklarieren.

Mögliche Capabilities sind unter anderem:

```text
FP16
FP32
FP64

BF16
TF32

INT8
INT16
INT32
INT64

SIMD
VECTOR

MATRIX
TENSOR

ATOMIC32
ATOMIC64

LOCAL_MEMORY
SHARED_MEMORY
UNIFIED_MEMORY
ZERO_COPY

ASYNC_COPY
OUT_OF_ORDER_QUEUE

TIMESTAMPS
PREEMPTION
FAULT_RECOVERY
```

Die Capability-Liste MUSS erweiterbar sein.

Unbekannte zukünftige Capabilities DÜRFEN von älteren Runtime-Versionen ignoriert werden.

---

## 10. Feature Query

Anwendungen MÜSSEN Device Capabilities zur Laufzeit abfragen können.

Beispiel:

```text
Supports(FP16)
Supports(MATRIX)
Supports(UNIFIED_MEMORY)
```

Anwendungen SOLLEN nach Fähigkeiten und nicht nach konkreten Hardwaremodellen fragen.

Bevorzugt:

```text
Supports(MatrixMultiplyFP16)
```

Nicht bevorzugt:

```text
Device == VendorModel123
```

---

## 11. Compute Context

Eine Anwendung MUSS einen Compute Context erzeugen können.

Ein Context SOLL mindestens enthalten:

```text
ContextID
OwnerProcess
AllowedDevices
MemoryBudget
Priority
SecurityDomain
PowerPolicy
SchedulingPolicy
```

Der Context bildet die primäre Isolationseinheit der Compute Runtime.

---

## 12. Context-Isolation

Compute Contexts unterschiedlicher Prozesse MÜSSEN voneinander isoliert sein.

Ein Prozess DARF nicht:

- fremde Compute Buffer lesen,
- fremde Compute Buffer verändern,
- fremde Queues manipulieren,
- fremde Events verändern,
- fremde Fences signalisieren,
- fremde Device-Speicheradressen verwenden,
- fremde Kernel-Argumente lesen.

Hardware ohne ausreichende Isolation MUSS durch Treiber oder Runtime entsprechend virtualisiert oder eingeschränkt werden.

---

## 13. Compute Queue

Compute Commands werden über Compute Queues eingereicht.

Eine Queue SOLL mindestens folgende Eigenschaften besitzen:

```text
QueueID
ContextID
DeviceID
Priority
OrderingMode
Status
```

---

## 14. Queue Ordering

Die Runtime MUSS mindestens folgende Queue-Modi definieren:

```text
IN_ORDER
OUT_OF_ORDER
```

### 14.1 IN_ORDER

Commands werden logisch entsprechend ihrer Einreichungsreihenfolge ausgeführt.

### 14.2 OUT_OF_ORDER

Die Runtime DARF Commands umordnen, wenn sämtliche Daten- und Synchronisationsabhängigkeiten eingehalten werden.

Ein Backend ohne native Out-of-Order-Unterstützung DARF diesen Modus intern auf eine geordnete Queue abbilden.

---

## 15. Compute Commands

Mindestens folgende Command-Typen SOLLEN unterstützt werden:

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

## 16. Compute Job

Ein Compute Job beschreibt eine logisch zusammengehörige Berechnung.

Ein Job kann mehrere Commands enthalten.

Beispiel:

```text
Compute Job
    |
    +-- Upload Input
    |
    +-- Dispatch Kernel A
    |
    +-- Barrier
    |
    +-- Dispatch Kernel B
    |
    +-- Download Result
```

Jobs SOLLEN vollständig asynchron eingereicht werden können.

---

## 17. Job-Zustände

Ein Compute Job MUSS mindestens folgende Zustände besitzen können:

```text
CREATED
QUEUED
WAITING
RUNNING
COMPLETED
FAILED
CANCEL_REQUESTED
CANCELLED
DEVICE_LOST
```

---

## 18. Compute Kernel

Ein Compute Kernel repräsentiert eine auf einem Compute Device ausführbare Berechnungsfunktion.

Ein Kernel SOLL Metadaten besitzen.

Beispiel:

```text
KernelName
KernelVersion
TargetArchitecture
RequiredFeatures
ArgumentLayout
LocalMemoryRequirement
PreferredWorkgroupSize
```

---

## 19. Portable Compute Representation

NovaOS SOLL langfristig ein hardwareunabhängiges Compute-Zwischenformat bereitstellen.

Prinzip:

```text
Source
   |
   v
Nova Compute IR
   |
   +----------------------+
   |          |           |
   v          v           v
 CPU        GPU          NPU
Backend    Backend      Backend
   |          |           |
   v          v           v
Native     Device       Device
Code       Code         Code
```

Das konkrete Compute IR MUSS durch eine separate Spezifikation definiert werden.

---

## 20. Native Compute Code

Die Runtime DARF zusätzlich bereits kompilierten Device-Code laden.

Beispiele:

```text
x86-64
ARM64
SPIR-V
GPU ISA
NPU ISA
```

Vor der Ausführung MUSS die Runtime prüfen, ob der Code mit dem Zielgerät kompatibel ist.

---

## 21. Compute Compilation

Die Runtime SOLL sowohl:

```text
Ahead-of-Time Compilation
```

als auch:

```text
Just-in-Time Compilation
```

unterstützen können.

JIT-Kompilierung MUSS durch Systemrichtlinien deaktivierbar sein.

---

## 22. Compilation Cache

Bereits kompilierter Device-Code SOLL gecacht werden können.

Ein Cache-Key MUSS mindestens berücksichtigen:

```text
KernelHash
RuntimeVersion
BackendVersion
DriverVersion
DeviceArchitecture
CompilerVersion
CompileOptions
```

Veraltete oder inkompatible Cache-Einträge MÜSSEN verworfen werden.

---

## 23. Dispatch-Modell

Compute Workloads SOLLEN in ein allgemeines dreidimensionales Dispatch-Modell abgebildet werden können.

Dimensionen:

```text
X
Y
Z
```

Beispiele:

```text
Dispatch(1024, 1, 1)
```

```text
Dispatch(1920, 1080, 1)
```

---

## 24. Workgroups

Backends mit Workgroup-Konzept MÜSSEN lokale Gruppen unterstützen können.

Beispiel:

```text
GlobalSize = 1024
LocalSize  = 64
```

Wenn keine Local Size angegeben wurde, SOLL die Runtime oder das Backend eine geeignete Größe auswählen können.

---

## 25. Speicherklassen

Die Runtime MUSS unterschiedliche Speicherklassen modellieren können.

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

## 26. Host Memory

`HOST` bezeichnet regulären CPU-adressierbaren Arbeitsspeicher.

Host Memory MUSS nicht automatisch von jedem Compute Device direkt adressierbar sein.

---

## 27. Device Memory

`DEVICE` bezeichnet Speicher, der primär einem Compute Device zugeordnet ist.

Beispiel:

```text
dedizierter GPU-VRAM
```

---

## 28. Shared Memory

`SHARED` bezeichnet Speicher, der von mehreren Compute-Komponenten gemeinsam verwendet werden kann.

---

## 29. Unified Memory

Die Runtime MUSS Hardware erkennen können, bei der CPU und Beschleuniger denselben oder einen kohärenten Adressraum verwenden.

Bei Unified Memory SOLLEN unnötige Speichertransfers vermieden werden.

---

## 30. Local Memory

`LOCAL` bezeichnet besonders schnellen, Compute-Unit-nahen Speicher.

Beispiele:

- GPU Shared Memory,
- Scratchpad Memory,
- NPU Local SRAM.

Die Lebensdauer kann auf einen Dispatch oder eine Workgroup beschränkt sein.

---

## 31. Constant Memory

Read-only-Daten SOLLEN als Constant Memory markiert werden können.

Backends DÜRFEN dafür optimierte Hardwarepfade verwenden.

---

## 32. Compute Buffer

Ein Compute Buffer MUSS mindestens folgende Informationen besitzen:

```text
BufferID
Size
Alignment
MemoryClass
UsageFlags
OwnerContext
DeviceVisibility
MappingState
```

---

## 33. Buffer Usage Flags

Mindestens folgende Usage Flags SOLLEN definiert werden:

```text
READ
WRITE
READ_WRITE

UPLOAD
DOWNLOAD

STORAGE
CONSTANT

TEMPORARY
PERSISTENT

ZERO_COPY_PREFERRED
```

---

## 34. Memory Mapping

Host-seitiger Zugriff auf Compute Memory MUSS kontrolliert erfolgen.

Beispiel:

```text
Map(buffer)

...

Unmap(buffer)
```

Die Runtime MUSS notwendige Synchronisations- und Cache-Operationen durchführen.

---

## 35. Zero-Copy

Wenn die Hardware Zero-Copy unterstützt, SOLL die Runtime unnötige Speicherübertragungen vermeiden.

Zero-Copy DARF niemals Sicherheits- oder Isolationseigenschaften umgehen.

---

## 36. Memory Residency

Die Runtime DARF Ressourcen automatisch zwischen Speicherbereichen verschieben.

Beispiele:

```text
RAM -> VRAM

VRAM -> RAM

RAM -> NPU Memory
```

Die Anwendung SOLL nicht zwingend wissen müssen, in welchem physischen Speicher sich eine Ressource aktuell befindet.

---

## 37. Speicherbudgets

Ein Compute Context SOLL ein Ressourcenbudget besitzen.

Mögliche Limits:

```text
HostMemoryLimit
DeviceMemoryLimit
PinnedMemoryLimit
SharedMemoryLimit
```

Eine Anwendung DARF nicht unbegrenzt Compute Memory reservieren.

---

## 38. Memory Oversubscription

Wenn das Backend dies unterstützt, DARF Device Memory überbucht werden.

Mögliche Mechanismen:

- Paging,
- Eviction,
- Migration,
- Kompression,
- temporäre Auslagerung,
- Job-Drosselung.

---

## 39. Synchronisation

Die Compute Runtime MUSS explizite Synchronisationsmechanismen bereitstellen.

Mindestens:

```text
Event
Fence
Barrier
```

---

## 40. Event

Ein Event SOLL mindestens folgende Zustände besitzen:

```text
UNSIGNALED
SIGNALED
FAILED
```

Events können für Abhängigkeiten zwischen Compute Commands verwendet werden.

---

## 41. Fence

Ein Fence verwendet einen monoton steigenden Wert.

Beispiel:

```text
FenceValue = 42
```

Eine Anwendung kann beispielsweise warten auf:

```text
FenceValue >= 42
```

---

## 42. Barrier

Barriers definieren Ausführungs- oder Speicherabhängigkeiten.

Mindestens folgende Barrier-Arten SOLLEN darstellbar sein:

```text
EXECUTION
MEMORY
DEVICE
QUEUE
```

---

## 43. Asynchrone Ausführung

Compute APIs SOLLEN standardmäßig asynchron arbeiten.

Ein Aufruf wie:

```text
submit()
```

SOLL einen Handle, ein Event oder einen vergleichbaren Status zurückgeben.

Warten erfolgt separat:

```text
wait(event)
```

---

## 44. Device Discovery

Die Runtime MUSS verfügbare Compute Devices automatisch erkennen.

Device Discovery MUSS mindestens erfolgen:

- beim Start der Runtime,
- nach relevanten Hardwareänderungen,
- nach Treiberänderungen,
- nach einem Device Reset.

---

## 45. Hot-Plug

Hot-Plug-fähige Compute Devices SOLLEN unterstützt werden können.

Beispiele:

- externe GPUs,
- Thunderbolt-Beschleuniger,
- USB-basierte AI-Beschleuniger.

Wird ein aktives Device entfernt, MUSS ein definierter `DEVICE_LOST`-Zustand erzeugt werden.

---

## 46. Device Groups

Mehrere Devices DÜRFEN zu einer logischen Compute Group zusammengefasst werden.

Beispiel:

```text
ComputeGroup
    |
    +-- CPU0
    +-- GPU0
    +-- NPU0
```

Oder:

```text
ComputeGroup
    |
    +-- GPU0
    +-- GPU1
```

---

## 47. Automatische Geräteauswahl

Die Runtime MUSS automatische Device Selection unterstützen.

Dabei SOLLEN mindestens folgende Faktoren berücksichtigt werden:

```text
WorkloadType
RequiredFeatures
EstimatedRuntime
DataLocation
TransferCost
CurrentLoad
QueueDepth
MemoryAvailability
PowerPolicy
ThermalState
ApplicationPriority
UserPolicy
```

---

## 48. Device Selection Score

Eine Implementierung DARF intern einen Device Score verwenden.

Beispiel:

```text
Score =
      PerformanceScore
    - TransferCost
    - QueuePenalty
    - ThermalPenalty
    - PowerPenalty
```

Die konkrete Formel ist implementierungsabhängig und DARF nicht Teil des öffentlichen ABI sein.

---

## 49. Workload Hints

Anwendungen SOLLEN Hinweise zur Charakteristik eines Workloads geben können.

Beispiele:

```text
LATENCY_CRITICAL
THROUGHPUT
ENERGY_EFFICIENT

BACKGROUND
INTERACTIVE
BATCH

REALTIME_SOFT

AI_INFERENCE
AI_TRAINING

MEDIA
SCIENTIFIC
```

Ein Workload Hint ist keine zwingende Hardwarezuordnung.

---

## 50. Preferred Device

Eine Anwendung DARF ein bevorzugtes Device angeben.

Beispiel:

```text
PreferredDevice = GPU
```

Die Runtime DARF davon abweichen, wenn:

- das Gerät nicht verfügbar ist,
- notwendige Features fehlen,
- Sicherheitsrichtlinien dies verlangen,
- nicht genügend Ressourcen vorhanden sind.

---

## 51. Required Device

Eine Anwendung DARF ein bestimmtes Device erzwingen.

Beispiel:

```text
RequiredDevice = GPU0
```

Ist dieses Device nicht verfügbar oder ungeeignet, MUSS der Job fehlschlagen.

Die Runtime DARF nicht stillschweigend ein anderes Device auswählen.

---

## 52. Compute Fallback

Automatisch geplante Workloads SOLLEN alternative Ausführungspfade besitzen können.

Beispiel:

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

Die tatsächliche Reihenfolge ist abhängig vom Workload.

---

## 53. CPU Backend

Ein CPU Compute Backend MUSS Bestandteil jeder vollständigen NovaOS-Compute-Runtime sein.

Dadurch bleibt die Compute API auf Systemen ohne spezielle Beschleuniger verwendbar.

---

## 54. CPU-Ausführungsmodi

Das CPU Backend SOLL mindestens unterstützen:

```text
Scalar
SIMD
Multi-Core
Task Parallelism
Data Parallelism
```

---

## 55. CPU Feature Detection

Das CPU Backend MUSS verfügbare Instruktionssatzerweiterungen erkennen.

Beispiele:

```text
SSE2
SSE4
AVX
AVX2
AVX-512
FMA
AES

NEON
SVE
```

Eine Instruktion DARF nur verwendet werden, wenn deren Unterstützung bestätigt wurde.

---

## 56. Hybrid CPUs

Die Runtime SOLL CPUs mit unterschiedlichen Kernklassen unterstützen.

Beispiele:

```text
Performance Core
Efficiency Core
```

Compute Workloads DÜRFEN entsprechend ihrer Eigenschaften auf unterschiedliche Kernklassen verteilt werden.

---

## 57. GPU Backend

GPU Compute MUSS über austauschbare Backends implementierbar sein.

Die öffentliche Nova Compute API DARF nicht fest an folgende APIs gebunden sein:

- CUDA,
- OpenCL,
- Vulkan Compute,
- DirectCompute,
- Metal,
- ROCm.

Ein Backend DARF intern eine oder mehrere dieser Technologien verwenden.

---

## 58. NPU Backend

Neural Processing Units SOLLEN als eigenständige Compute Device Class unterstützt werden.

Eine NPU DARF nicht ausschließlich als Sonderfall einer GPU behandelt werden.

---

## 59. NPU Capabilities

NPU-spezifische Einschränkungen MÜSSEN deklarierbar sein.

Beispiele:

```text
SupportedDataTypes
SupportedTensorRanks
SupportedOperators
MaximumTensorSize
LocalMemory
StaticShapeOnly
DynamicShapeSupport
```

---

## 60. DSP Backend

DSPs SOLLEN als eigenständige Compute Device Class unterstützt werden können.

Typische Einsatzbereiche sind:

- Audioverarbeitung,
- Signalverarbeitung,
- Sensorverarbeitung,
- Kommunikation,
- Filterung.

---

## 61. Multi-Device Execution

Ein Compute Job DARF mehrere Compute Devices verwenden.

Beispiel:

```text
Input
 |
 v
CPU Preprocessing
 |
 v
GPU Matrix Processing
 |
 v
NPU Inference
 |
 v
CPU Postprocessing
```

Die Runtime MUSS sämtliche Datenabhängigkeiten korrekt verwalten.

---

## 62. Parallel Device Execution

Unabhängige Workloads DÜRFEN parallel auf mehreren Devices ausgeführt werden.

Beispiel:

```text
              +--> GPU0
              |
Input --------+
              |
              +--> GPU1
```

---

## 63. Device-to-Device Transfer

Wenn Hardware einen direkten Datentransfer ermöglicht, SOLL die Runtime diesen verwenden können.

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
System RAM
    |
    v
GPU1 VRAM
```

sofern dies sicher und technisch möglich ist.

---

## 64. Compute Scheduling

Die Compute Runtime MUSS Compute Jobs auf verfügbare Devices verteilen können.

Scheduling MUSS mindestens berücksichtigen:

- Priorität,
- Gerätezustand,
- Queue Load,
- verfügbare Ressourcen,
- Datenlokalität,
- Power Policy,
- thermische Situation.

---

## 65. Prioritätsklassen

Mindestens folgende Prioritäten SOLLEN vorhanden sein:

```text
LOW
NORMAL
HIGH
CRITICAL
```

`CRITICAL` SOLL ausschließlich privilegierten Systemkomponenten zur Verfügung stehen.

---

## 66. Scheduling Fairness

Eine normale Anwendung DARF ein gemeinsam genutztes Compute Device nicht dauerhaft monopolieren.

Der Scheduler MUSS Fairness zwischen Contexts ermöglichen.

---

## 67. Preemption

Wenn ein Device Preemption unterstützt, SOLL die Runtime diese Fähigkeit verwenden können.

Die Unterstützung MUSS über eine Capability erkennbar sein.

---

## 68. Nicht-preemptive Devices

Bei Geräten ohne Preemption SOLL die Runtime:

- sehr große Dispatches erkennen,
- Queue-Fairness berücksichtigen,
- kleinere Dispatches bevorzugen können,
- lang andauernde Blockaden minimieren.

---

## 69. Interaktive Compute Workloads

Interaktive Workloads SOLLEN gegenüber Hintergrundworkloads bevorzugt werden können.

Beispiele:

- UI-Effekte,
- Echtzeitvorschau,
- Sprachverarbeitung,
- interaktive AI,
- Kameraeffekte.

---

## 70. Background Compute

Hintergrundberechnungen SOLLEN automatisch gedrosselt werden können.

Beispiele:

- Indexierung,
- Training,
- Cache-Erzeugung,
- Analyse,
- Vorberechnung.

---

## 71. Power Policy

Ein Compute Context SOLL eine Power Policy besitzen können.

Mindestens:

```text
SYSTEM_DEFAULT
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 72. Energieadaptive Ausführung

Bei energieeffizienten Policies DARF die Runtime ein langsameres, aber sparsameres Device bevorzugen.

Beispiel:

```text
AI Inference
    |
    +-- GPU
    |
    +-- NPU
```

Bei vergleichbarer Laufzeit SOLL die NPU bevorzugt werden können, wenn sie deutlich energieeffizienter arbeitet.

---

## 73. Thermal State

Die Runtime SOLL mindestens folgende thermischen Zustände berücksichtigen können:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

Bei thermischen Einschränkungen DARF ein Workload auf andere Devices verschoben werden.

---

## 74. Aktuelle Systemlast

Die automatische Gerätewahl MUSS nicht immer das theoretisch schnellste Device auswählen.

Beispiel:

Eine GPU kann theoretisch schneller als eine CPU sein.

Wenn:

```text
GPU Load = 100 %
```

und:

```text
CPU Load = 10 %
```

kann die CPU für einen kleinen Workload die bessere Wahl darstellen.

---

## 75. Transferkosten

Der Scheduler SOLL Speichertransferkosten berücksichtigen.

Beispiel:

```text
GPU Compute Time = 1 ms
Transfer Time    = 10 ms
Total            = 11 ms
```

CPU:

```text
CPU Compute Time = 3 ms
Transfer Time    = 0 ms
Total            = 3 ms
```

In diesem Fall SOLL die CPU bevorzugt werden können.

---

## 76. Small Workload Optimization

Sehr kleine Workloads SOLLEN nicht automatisch auf einen Beschleuniger verschoben werden.

Der Runtime-Overhead und die Transferkosten MÜSSEN berücksichtigt werden.

---

## 77. Runtime Policy Engine

Die Compute Runtime SOLL eine zentrale Policy Engine besitzen.

Diese kann unter anderem Entscheidungen treffen über:

```text
Device Selection
Scheduling
Power
Thermal Management
Memory Residency
Migration
Fallback
Priorities
```

---

## 78. Lernende Optimierung

NovaOS DARF historische Laufzeitdaten verwenden, um zukünftige Scheduling-Entscheidungen zu verbessern.

Beispiel:

```text
KernelHash
+
InputSize
+
DeviceID
+
RuntimeConfiguration
=
ExpectedExecutionTime
```

Diese Informationen können für zukünftige Device Selection verwendet werden.

Eine lernende Optimierung DARF jedoch keine Sicherheits-, Berechtigungs- oder Determinismusregeln umgehen.

---

## 79. Deterministische Ausführung

Eine Anwendung MUSS deterministische Ausführung anfordern können.

Beispiel:

```text
DETERMINISTIC
```

Bei aktiviertem Determinismus DÜRFEN Optimierungen, welche die geforderte Reproduzierbarkeit verletzen, nicht verwendet werden.

---

## 80. Numerische Unterschiede

Unterschiedliche Compute Devices können bei Gleitkommaberechnungen geringfügig unterschiedliche Ergebnisse erzeugen.

Die Runtime DARF daher bei automatischem Device Switching keine Bitidentität garantieren, sofern diese nicht ausdrücklich Bestandteil des verwendeten Compute-Modells ist.

---

## 81. Batch Submission

Mehrere Commands oder Jobs SOLLEN gesammelt eingereicht werden können.

Dies reduziert Runtime- und Syscall-Overhead.

---

## 82. Command Lists

Die Runtime DARF wiederverwendbare Command Lists unterstützen.

Beispiel:

```text
CommandList
    |
    +-- Upload
    +-- Dispatch A
    +-- Dispatch B
    +-- Download
```

Eine Command List kann einmal erstellt und mehrfach ausgeführt werden.

---

## 83. Compute Graph

Die Runtime SOLL langfristig Compute Graphs unterstützen können.

Ein Compute Graph beschreibt:

```text
Nodes
Dependencies
Resources
ExecutionConstraints
```

Beispiel:

```text
             +--> Kernel B --+
             |               |
Kernel A ----+               +--> Kernel D
             |               |
             +--> Kernel C --+
```

---

## 84. Graph Optimization

Die Runtime DARF einen Compute Graph optimieren.

Mögliche Optimierungen:

- Kernel Fusion,
- parallele Ausführung,
- Transfer Elimination,
- Memory Reuse,
- Device Placement,
- Command Batching.

Die definierte Semantik DARF dadurch nicht verändert werden.

---

## 85. Cancellation

Noch nicht abgeschlossene Compute Jobs SOLLEN abgebrochen werden können.

Die Runtime MUSS unterscheiden zwischen:

```text
CANCEL_REQUESTED
```

und:

```text
CANCELLED
```

Ein Device kann einen laufenden Dispatch möglicherweise nicht unmittelbar abbrechen.

---

## 86. Timeout

Compute Jobs SOLLEN optional ein Timeout definieren können.

Beispiel:

```text
Timeout = 500 ms
```

Bei Überschreitung MUSS eine definierte Fehlerbehandlung erfolgen.

---

## 87. Watchdog

Compute Devices, die durch fehlerhaften Code dauerhaft blockiert werden könnten, SOLLEN durch einen Watchdog überwacht werden.

---

## 88. Fehlerklassen

Die Runtime MUSS mindestens folgende Fehlerklassen definieren:

```text
INVALID_ARGUMENT
INVALID_CONTEXT
INVALID_DEVICE
INVALID_HANDLE

UNSUPPORTED_FEATURE

OUT_OF_MEMORY
OUT_OF_RESOURCES

COMPILATION_FAILED
DISPATCH_FAILED
TRANSFER_FAILED

TIMEOUT
DEVICE_BUSY
DEVICE_LOST
DEVICE_RESET

ACCESS_DENIED
RESOURCE_CORRUPTED

INTERNAL_ERROR
```

---

## 89. Device Lost

Wird ein Compute Device unerwartet nicht mehr verfügbar, MUSS die Runtime:

1. laufende Jobs kennzeichnen,
2. betroffene Contexts informieren,
3. Queues stoppen oder invalidieren,
4. Ressourcenstatus aktualisieren,
5. das Backend informieren,
6. optional Recovery ausführen.

---

## 90. Device Reset

Ein Device Reset DARF unter anderem bei folgenden Situationen erfolgen:

- Hardware-Hang,
- Treiberfehler,
- Watchdog Timeout,
- Speicherfehler,
- Sicherheitsverletzung.

Ein Device Reset DARF das gesamte Betriebssystem nicht unnötig zum Absturz bringen.

---

## 91. Device Recovery

Wenn möglich, SOLL Recovery folgendem Ablauf folgen:

```text
Device Error
     |
     v
Stop Queues
     |
     v
Cancel / Fail Jobs
     |
     v
Reset Device
     |
     v
Reinitialize Backend
     |
     v
Register Device
     |
     v
Device Available
```

---

## 92. Retry-Sicherheit

Jobs können als:

```text
RETRY_SAFE
```

markiert werden.

Nur Jobs, die sicher wiederholt werden können, SOLLEN nach einem Device Failure automatisch neu gestartet werden.

---

## 93. Compute Security Model

Compute Devices MÜSSEN Teil des regulären NovaOS-Sicherheitsmodells sein.

Ein Compute Device DARF nicht als unkontrollierter Speicher- oder Ausführungsbereich betrachtet werden.

---

## 94. Speicherbereinigung

Compute Memory MUSS vor der Wiederverwendung durch einen anderen Security Context bereinigt werden, wenn andernfalls Datenreste auslesbar wären.

---

## 95. Kernel Validation

Externer oder portabler Compute Code MUSS vor seiner Ausführung validiert werden.

Mindestens zu prüfen sind:

```text
Format
Version
Architecture
RequiredCapabilities
KernelMetadata
ResourceRequirements
SecurityPolicy
```

---

## 96. JIT-Sicherheit

JIT-generierter Code MUSS dem NovaOS-Speicherschutzmodell folgen.

Speicherseiten SOLLEN nicht gleichzeitig:

```text
Writable
+
Executable
```

sein.

Das W^X-Prinzip SOLL eingehalten werden.

---

## 97. Ressourcenlimits

Die Runtime MUSS Limits setzen können für:

```text
QueueCount
BufferCount
KernelCount
EventCount

DeviceMemory
PinnedMemory
SharedMemory

OutstandingJobs

CompilationTime
ExecutionTime
```

---

## 98. Prozessbeendigung

Wird ein Prozess beendet, MUSS die Runtime sämtliche ihm gehörenden Compute-Ressourcen bereinigen.

Dazu gehören:

```text
Contexts
Queues
Buffers
Events
Fences
Jobs
KernelObjects
Mappings
```

---

## 99. Prozessabsturz

Ein abgestürzter Prozess DARF keine dauerhaft belegten Compute-Ressourcen hinterlassen.

Cleanup MUSS durch Runtime und Betriebssystem gewährleistet werden.

---

## 100. Backend-Modell

Die Runtime DARF unterschiedliche hardware- oder herstellerspezifische Backends verwenden.

Ein Backend MUSS eine standardisierte Schnittstelle bereitstellen.

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

synchronize()

reset_device()

shutdown()
```

---

## 101. Backend ABI

Das Compute Backend ABI MUSS versioniert sein.

Beispiel:

```text
NovaComputeBackendABI 1.0
```

Runtime und Backend MÜSSEN vor der Aktivierung ihre Kompatibilität prüfen.

---

## 102. Capability Negotiation

Runtime und Backend MÜSSEN unterstützte Funktionen explizit aushandeln.

Eine neuere Runtime DARF nicht voraussetzen, dass ein älteres Backend neue Funktionen implementiert.

---

## 103. Backend Isolation

Komplexe optionale Backend-Komponenten SOLLEN, soweit technisch sinnvoll, außerhalb des Kernels ausgeführt werden.

Ein fehlerhaftes Backend SOLL nicht den gesamten Betriebssystemkernel zum Absturz bringen können.

---

## 104. Software Backend

NovaOS SOLL ein Software-Fallback-Backend bereitstellen.

Das Software Backend kann langsam sein, MUSS aber grundlegende Compute-Funktionalität bereitstellen.

---

## 105. User-Mode Runtime

Die öffentliche Compute API SOLL bevorzugt über eine User-Mode Runtime Library bereitgestellt werden.

Eine mögliche Architektur lautet:

```text
Application
     |
     v
Nova Compute User Library
     |
     v
Nova Compute Service
     |
     v
Kernel Interface
     |
     v
Backend / Driver
```

Nicht jede Operation MUSS dabei einen Prozesswechsel verursachen.

Performancekritische Pfade DÜRFEN optimiert werden.

---

## 106. Thread Safety

Die öffentliche Compute Runtime MUSS Thread-safe sein.

Mehrere Threads desselben Prozesses MÜSSEN gleichzeitig:

- Ressourcen erzeugen,
- Commands einreichen,
- Jobs starten,
- Events abfragen

können.

---

## 107. Handle-Modell

Öffentliche Runtime-Objekte SOLLEN durch opaque Handles repräsentiert werden.

Beispiele:

```text
NovaComputeContextHandle
NovaComputeDeviceHandle
NovaComputeQueueHandle
NovaComputeBufferHandle
NovaComputeKernelHandle
NovaComputeEventHandle
NovaComputeFenceHandle
```

Interne Runtime-Strukturen DÜRFEN von Anwendungen nicht direkt verändert werden.

---

## 108. Resource Lifetime

Für Runtime-Objekte MUSS ein klar definiertes Lebenszyklusmodell existieren.

Beispiel:

```text
create
  |
  v
use
  |
  v
release
  |
  v
destroy
```

Alternativ kann ein Referenzzählungs- oder Ownership-Modell verwendet werden.

---

## 109. NUMA

Auf NUMA-Systemen SOLL die Runtime Speicherlokalität berücksichtigen.

CPU Compute Workloads SOLLEN nach Möglichkeit auf dem NUMA Node ausgeführt werden, auf dem sich ihre Daten befinden.

---

## 110. Device Topology

Die Runtime SOLL die physische Beziehung zwischen Compute Devices kennen können.

Beispiele:

```text
NUMA Node

CPU Socket

PCIe Root

Shared Cache

Memory Domain

Interconnect
```

---

## 111. Device Locality

Bei mehreren Beschleunigern SOLL Topologie bei Scheduling und Speichertransfers berücksichtigt werden.

Beispiel:

```text
CPU Socket 0
      |
      v
PCIe Root 0
      |
      v
GPU0
```

Ein Workload auf Socket 0 kann GPU0 gegenüber einer topologisch weiter entfernten GPU bevorzugen.

---

## 112. Graphics Coordination

Wenn Grafik und Compute dasselbe GPU Device verwenden, MUSS eine Koordination mit dem Grafik-Scheduler möglich sein.

Compute Workloads DÜRFEN den interaktiven Desktop nicht dauerhaft blockieren.

---

## 113. System Compute Reserve

NovaOS DARF Compute-Kapazität für kritische Systemaufgaben reservieren.

Normale Anwendungen DÜRFEN diese Kapazität nicht vollständig belegen.

---

## 114. Quality of Service

Die Compute Runtime SOLL unterschiedliche QoS-Klassen unterstützen.

Beispiele:

```text
SYSTEM_CRITICAL
INTERACTIVE
FOREGROUND
NORMAL
BACKGROUND
MAINTENANCE
```

---

## 115. AI-Integration

NovaOS-AI-Komponenten SOLLEN die Compute Runtime verwenden können.

Beispiel:

```text
Nova.AI
   |
   v
Nova Compute Runtime
   |
   +--> CPU
   +--> GPU
   +--> NPU
```

Spezialisierte AI-Abstraktionen DÜRFEN oberhalb der Compute Runtime implementiert werden.

---

## 116. Nova.Math-Integration

Mathematische Systembibliotheken SOLLEN die Compute Runtime optional für Hardwarebeschleunigung verwenden können.

Beispiel:

```text
Nova.Math.MatrixMultiply()
```

kann intern abhängig von Hardware und Workload auf:

```text
CPU Scalar
CPU SIMD
GPU
NPU
```

abgebildet werden.

---

## 117. Transparent Acceleration

Eine höherliegende NovaOS-Bibliothek DARF Compute Hardware transparent verwenden, wenn:

- die Semantik gleich bleibt,
- Sicherheitsregeln eingehalten werden,
- Fehler kontrolliert behandelt werden.

---

## 118. Explicit Compute

Spezialisierte Anwendungen MÜSSEN zusätzlich direkten Zugriff auf die Nova Compute API erhalten können.

Damit können sie:

- Devices abfragen,
- Queues erzeugen,
- Speicher explizit verwalten,
- Kernels einreichen,
- Synchronisation kontrollieren.

---

## 119. Resource Sharing

Compute Resources SOLLEN kontrolliert mit anderen Subsystemen geteilt werden können.

Beispiele:

```text
Graphics <-> Compute

Video <-> Compute

AI <-> Compute

Camera <-> Compute
```

---

## 120. Shared Handles

Geteilte Ressourcen MÜSSEN über explizite und validierte Handles freigegeben werden.

Physische Speicheradressen DÜRFEN nicht als öffentliche Resource Handles verwendet werden.

---

## 121. Resource Import und Export

Die Runtime SOLL Ressourcen importieren und exportieren können.

Beispiel:

```text
GraphicsTexture
      |
      v
ComputeResource
```

Dabei SOLL eine unnötige Speicherduplikation vermieden werden.

---

## 122. Cache Coherency

Die Runtime MUSS notwendige Cache-Kohärenz zwischen CPU und Compute Devices gewährleisten.

Anwendungen SOLLEN keine hardwarespezifischen Cache Flushes durchführen müssen.

---

## 123. Diagnostics

Die Compute Runtime MUSS Diagnoseinformationen bereitstellen.

Mindestens:

```text
DetectedDevices

DeviceState

DriverVersion
BackendVersion

MemoryUsage

QueueUsage

RunningJobs
CompletedJobs
FailedJobs

DeviceResets

CompilationErrors
```

---

## 124. Performance Counters

Wenn Hardware dies unterstützt, SOLL die Runtime Performance Counter bereitstellen.

Beispiele:

```text
ExecutionTime
QueueWaitTime
TransferTime

MemoryBandwidth
ComputeUtilization

CacheMisses

PowerUsage
Temperature
```

---

## 125. Profiling

Entwicklerwerkzeuge SOLLEN Compute Jobs profilieren können.

Profiling MUSS kontrolliert aktivierbar sein.

Profiling DARF Sicherheitsgrenzen zwischen Prozessen nicht umgehen.

---

## 126. Timestamping

Compute Devices mit Hardware-Timestamps SOLLEN diese über eine normalisierte Runtime-Schnittstelle bereitstellen können.

---

## 127. Tracing

Die Compute Runtime SOLL in das NovaOS-Tracing-System integriert werden.

Mögliche Trace Events:

```text
COMPUTE_DEVICE_REGISTER

COMPUTE_CONTEXT_CREATE
COMPUTE_CONTEXT_DESTROY

COMPUTE_QUEUE_CREATE
COMPUTE_QUEUE_DESTROY

COMPUTE_JOB_SUBMIT
COMPUTE_JOB_START
COMPUTE_JOB_END

COMPUTE_MEMORY_ALLOC
COMPUTE_MEMORY_FREE

COMPUTE_DEVICE_LOST
COMPUTE_DEVICE_RESET
```

---

## 128. Debug Validation

Debug-Konfigurationen SOLLEN zusätzliche Validierung ermöglichen.

Beispiele:

- Buffer Bounds Checks,
- Handle Validation,
- Queue Validation,
- Dependency Validation,
- Resource Lifetime Checks,
- Kernel Argument Validation.

---

## 129. Release-Konfiguration

Performanceintensive Debug-Prüfungen DÜRFEN in Release Builds reduziert werden.

Sicherheitskritische Prüfungen DÜRFEN nicht deaktiviert werden.

---

## 130. Side-Channel-Schutz

Die Runtime SOLL bekannte Informationslecks zwischen Security Contexts minimieren.

Mögliche Maßnahmen:

- Speicherbereinigung,
- Queue-Isolation,
- eingeschränkte Performance Counter,
- Context Reset,
- kontrolliertes Timestamping.

---

## 131. Performance-Counter-Sicherheit

Performance Counter DÜRFEN für normale Anwendungen eingeschränkt werden, wenn darüber Informationen über fremde Prozesse gewonnen werden könnten.

---

## 132. Virtual Compute Devices

Ein physisches Compute Device DARF in mehrere logische Devices unterteilt werden.

Beispiel:

```text
Physical GPU
     |
     +-- Virtual Device A
     |
     +-- Virtual Device B
```

Virtuelle Devices MÜSSEN dieselbe grundlegende Device-Abstraktion verwenden können.

---

## 133. Sandbox Integration

NovaOS-Sandboxen SOLLEN Compute-Zugriff separat kontrollieren können.

Mögliche Berechtigungen:

```text
Compute.CPU

Compute.GPU

Compute.NPU

Compute.HighPerformance

Compute.Persistent
```

---

## 134. Berechtigungsmodell

Normale CPU-Berechnung benötigt grundsätzlich keine spezielle Compute-Berechtigung.

Zugriff auf:

- spezielle Beschleuniger,
- privilegierte Queues,
- direkte Hardwarefunktionen,
- Performance Counter,
- persistente Compute Jobs

DARF zusätzliche Berechtigungen erfordern.

---

## 135. Safe Mode

Im NovaOS Safe Mode DARF die Compute Runtime ausschließlich sichere CPU- oder Software-Backends verwenden.

Herstellerspezifische GPU-, NPU- oder andere Beschleuniger-Backends DÜRFEN deaktiviert werden.

---

## 136. Recovery Mode

NovaDOS und andere Recovery-Umgebungen MÜSSEN nicht die vollständige Compute Runtime implementieren.

Eine reduzierte CPU-basierte Compute-Schicht ist ausreichend.

---

## 137. Boot-Verhalten

Das Fehlen optionaler Compute Devices DARF den NovaOS-Systemstart nicht verhindern.

Das CPU Backend SOLL nach erfolgreicher Initialisierung der grundlegenden Kernel- und Prozessinfrastruktur verfügbar gemacht werden.

---

## 138. ABI-Stabilität

Das öffentliche Nova Compute ABI MUSS versioniert sein.

Anwendungen SOLLEN innerhalb derselben Major-Version binär kompatibel bleiben.

Neue optionale Funktionen SOLLEN über Capability Detection ergänzt werden.

---

## 139. Graceful Degradation

Fehlende Compute Features SOLLEN kontrolliert behandelt werden.

Beispiel:

```text
NPU unavailable
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

sofern die Anwendung Fallback zulässt.

---

## 140. Zukunftssicherheit

Die Architektur MUSS neue Compute-Technologien aufnehmen können.

Mögliche zukünftige Klassen:

- neuromorphe Prozessoren,
- optische Compute Units,
- ReRAM-basierte Matrixbeschleuniger,
- Quantenbeschleuniger,
- externe Compute Nodes,
- neue spezialisierte AI-Prozessoren.

Eine neue Device-Klasse SOLL keine Änderung bestehender Anwendungen erfordern, wenn diese ausschließlich generische Compute Capabilities verwenden.

---

## 141. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens bereitstellen:

```text
CPU Device

Compute Context

In-Order Queue

Compute Buffer

Kernel Dispatch

Memory Copy

Event

Fence

Basic Scheduling

Error Handling

Resource Cleanup

Context Isolation
```

---

## 142. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
GPU

NPU

DSP

Multi-Device Scheduling

Unified Memory

Zero-Copy

Portable Compute IR

JIT Compilation

Compute Graph

Automatic Device Selection

Power-Aware Scheduling

Thermal-Aware Scheduling

Profiling

Device Recovery
```

---

## 143. Beispiel — einfache Ausführung

Konzeptioneller Ablauf:

```text
context = Compute.CreateContext()

device = context.SelectDevice(
    preference = AUTO
)

queue = context.CreateQueue(device)

input = context.CreateBuffer(...)

output = context.CreateBuffer(...)

kernel = context.LoadKernel(
    "matrix_multiply"
)

queue.Copy(
    inputData,
    input
)

event = queue.Dispatch(
    kernel,
    globalSize = ...
)

queue.Wait(event)

queue.Copy(
    output,
    result
)

queue.Submit()
```

Die konkrete Programmierschnittstelle wird separat spezifiziert.

---

## 144. Beispiel — automatische AI-Ausführung

Vorhandene Devices:

```text
CPU
GPU
NPU
```

Messwerte:

```text
NPU

Performance = HIGH
Energy      = LOW
Queue       = FREE
```

```text
GPU

Performance = VERY_HIGH
Energy      = HIGH
Queue       = BUSY
```

```text
CPU

Performance = LOW
Energy      = MEDIUM
Queue       = FREE
```

Bei:

```text
PowerPolicy = BALANCED
```

kann die Runtime die NPU auswählen.

---

## 145. Beispiel — kleiner Workload

GPU:

```text
Transfer = 0.4 ms
Dispatch = 0.1 ms
Compute  = 0.1 ms

Total = 0.6 ms
```

CPU SIMD:

```text
Compute = 0.2 ms

Total = 0.2 ms
```

Die Runtime SOLL die CPU auswählen können.

---

## 146. Beispiel — Device Failure

```text
Job Running
    |
    v
GPU Timeout
    |
    v
Watchdog
    |
    v
DEVICE_LOST
    |
    v
Is RETRY_SAFE?
    |
 +--+--+
 |     |
YES    NO
 |     |
 v     v
Reset  FAILED
 |
 v
Retry
```

---

## 147. Performance-Anforderungen

Performancekritische Runtime-Pfade SOLLEN:

- unnötige Speicherallokationen vermeiden,
- Lock Contention minimieren,
- Batch Submission verwenden können,
- Kernel Caching unterstützen,
- Compilation Caching unterstützen,
- unnötige Prozesswechsel vermeiden,
- unnötige Speichertransfers vermeiden.

---

## 148. Robustheitsanforderungen

Fehlerhafte Compute Workloads DÜRFEN nicht:

- Kernel-Speicher überschreiben,
- fremden Prozessspeicher lesen,
- fremden Prozessspeicher verändern,
- fremde Compute Contexts manipulieren,
- dauerhaft Systemressourcen blockieren,
- nicht betroffene Devices beschädigen,
- das gesamte Betriebssystem unnötig zum Absturz bringen.

---

## 149. Device Enumeration Tests

Folgende Fälle MÜSSEN getestet werden:

- CPU-only-System,
- CPU + GPU,
- CPU + GPU + NPU,
- mehrere GPUs,
- Device ohne optionale Features,
- fehlerhaftes Device,
- Device Hot-Plug,
- Device Removal.

---

## 150. Context Tests

Zu testen sind mindestens:

```text
Create Context
Destroy Context
Invalid Handle
Cross-Process Access
Memory Budget
Allowed Device Restrictions
```

---

## 151. Queue Tests

Zu testen sind:

```text
IN_ORDER

OUT_OF_ORDER

Queue Priority

Multiple Queues

Parallel Submission

Queue Destruction

Queue under Device Loss
```

---

## 152. Memory Tests

Zu testen sind:

- Allocation,
- Free,
- Mapping,
- Unmapping,
- Host-to-Device Copy,
- Device-to-Host Copy,
- Device-to-Device Copy,
- Zero-Copy,
- Unified Memory,
- Out-of-Memory,
- ungültige Buffer Handles,
- Buffer Bounds.

---

## 153. Dispatch Tests

Zu testen sind:

- gültiger Kernel,
- ungültiger Kernel,
- fehlende Capability,
- ungültige Argumente,
- kleine Dispatches,
- große Dispatches,
- parallele Dispatches,
- Dispatch Cancellation.

---

## 154. Synchronisationstests

Zu testen sind:

```text
Events
Fences
Barriers
Cross-Queue Dependencies
Cross-Device Dependencies
Race Conditions
```

---

## 155. Fallback Tests

Mindestens folgende Übergänge SOLLEN getestet werden:

```text
NPU -> GPU

GPU -> CPU

CPU SIMD -> CPU Scalar
```

---

## 156. Device-Lost Tests

Zu testen sind:

```text
Device Timeout

Device Removal

Driver Failure

Device Reset

Running Job Failure

Queue Failure

Resource Cleanup

Retry-Safe Job Recovery
```

---

## 157. Security Tests

Mindestens:

- Cross-Process Buffer Access,
- gefälschte Handles,
- fremde Queue Handles,
- Speicherreste,
- ungültiger Device-Code,
- Resource Exhaustion,
- ungültige Kernel-Metadaten,
- unzulässige Performance Counter.

---

## 158. Stress Tests

Die Runtime MUSS unter hoher Last getestet werden.

Testfälle SOLLEN unter anderem beinhalten:

```text
1000 Queues

100000 Jobs

mehrere Prozesse

ständige Buffer-Allokation

ständige Buffer-Freigabe

Process Exit während Dispatch

Device Reset unter Last
```

Die konkreten Grenzwerte sind plattformabhängig.

---

## 159. Conformance Tests

Compute Backends MÜSSEN gegen eine gemeinsame NovaOS Compute Conformance Test Suite geprüft werden.

Ein Backend DARF nur dann als vollständig kompatibel gekennzeichnet werden, wenn sämtliche verpflichtenden Tests bestanden wurden.

---

## 160. Fuzzing

Folgende Eingaben SOLLTEN fuzz-getestet werden:

- Compute IR,
- Kernel Metadata,
- Backend Metadata,
- Resource Descriptor,
- Command Descriptor,
- Compilation Input.

---

## 161. Fault Injection

Testumgebungen SOLLEN Fehler künstlich injizieren können.

Mindestens:

```text
DEVICE_LOST

DEVICE_RESET

OUT_OF_MEMORY

TIMEOUT

COMPILATION_FAILED

TRANSFER_FAILED

DISPATCH_FAILED
```

---

## 162. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0001` MUSS:

1. Compute Devices abstrahieren.
2. mindestens ein CPU Compute Device bereitstellen.
3. Compute Contexts unterstützen.
4. Context-Isolation gewährleisten.
5. Compute Queues bereitstellen.
6. Compute Jobs verwalten.
7. Compute Buffer verwalten.
8. Compute Kernels ausführen können.
9. asynchrone Dispatches ermöglichen.
10. Synchronisationsobjekte bereitstellen.
11. Device Capabilities abfragbar machen.
12. Fehlerzustände definiert behandeln.
13. Ressourcen bei Prozessende freigeben.
14. Device-Ausfälle kontrolliert behandeln.
15. Ressourcenlimits unterstützen.
16. Backend-Versionierung besitzen.
17. Thread-safe verwendbar sein.
18. Diagnoseinformationen bereitstellen.
19. Sicherheitsgrenzen zwischen Prozessen erzwingen.
20. optionale Beschleuniger ohne Abhängigkeit des Systemstarts behandeln.
21. Capability-basierte Hardwareerkennung verwenden.
22. zukünftige Device-Klassen erweiterbar halten.

---

## 163. Abhängigkeiten

Die Compute Runtime baut insbesondere auf folgenden NovaOS-Subsystemen auf:

```text
Kernel

Process Management

Thread Management

Scheduler

Physical Memory Manager

Virtual Memory Manager

IPC

Driver Model

HAL

Security

Power Management

Diagnostics

Tracing
```

---

## 164. Empfohlene Folge-Spezifikationen

Auf `NPSPEC-COMPUTE-RUNTIME-0001` SOLLEN spezialisierte NPSPECs aufbauen.

Empfohlen werden:

```text
NPSPEC-COMPUTE-DEVICE-0001
Compute Device Model

NPSPEC-COMPUTE-CONTEXT-0001
Compute Context

NPSPEC-COMPUTE-QUEUE-0001
Compute Queue

NPSPEC-COMPUTE-JOB-0001
Compute Job Model

NPSPEC-COMPUTE-KERNEL-0001
Compute Kernel

NPSPEC-COMPUTE-MEMORY-0001
Compute Memory Model

NPSPEC-COMPUTE-SYNC-0001
Compute Synchronization

NPSPEC-COMPUTE-SCHED-0001
Compute Scheduler

NPSPEC-COMPUTE-DEVICESEL-0001
Automatic Device Selection

NPSPEC-COMPUTE-CPU-0001
CPU Compute Backend

NPSPEC-COMPUTE-GPU-0001
GPU Compute Backend

NPSPEC-COMPUTE-NPU-0001
NPU Compute Backend

NPSPEC-COMPUTE-DSP-0001
DSP Compute Backend

NPSPEC-COMPUTE-IR-0001
Portable Compute Intermediate Representation

NPSPEC-COMPUTE-COMPILER-0001
Compute Compiler

NPSPEC-COMPUTE-GRAPH-0001
Compute Graph Runtime

NPSPEC-COMPUTE-PROFILE-0001
Compute Profiling

NPSPEC-COMPUTE-POWER-0001
Power-Aware Compute

NPSPEC-COMPUTE-RECOVERY-0001
Compute Device Recovery

NPSPEC-COMPUTE-SECURITY-0001
Compute Security Model

NPSPEC-COMPUTE-TEST-0001
Compute Runtime Conformance Tests
```

---

## 165. Zusammenfassung

Die NovaOS Compute Runtime stellt eine gemeinsame hardwareunabhängige Ausführungsschicht für allgemeine und spezialisierte Rechenoperationen bereit.

Die Architektur lautet:

```text
Application
    |
    v
Nova Compute API
    |
    v
Compute Runtime
    |
    +-- Device Discovery
    |
    +-- Scheduler
    |
    +-- Memory Manager
    |
    +-- Synchronization
    |
    +-- Policy Engine
    |
    +-- Recovery
    |
    +-- Diagnostics
    |
    v
Compute Backends
    |
    +-- CPU
    |
    +-- GPU
    |
    +-- NPU
    |
    +-- DSP
    |
    +-- Accelerator
```

Anwendungen SOLLEN primär die gewünschte Berechnung und ihre Anforderungen beschreiben.

Die Runtime entscheidet daraufhin anhand von:

```text
Capabilities
Performance
Data Locality
Transfer Cost
Current Load
Memory Availability
Energy Consumption
Thermal State
Application Priority
System Policy
```

welches Compute Device verwendet wird.

Dadurch erhält NovaOS eine einheitliche, heterogene und langfristig erweiterbare Compute-Infrastruktur, auf der insbesondere folgende Komponenten aufbauen können:

- `Nova.Math`,
- `Nova.AI`,
- `Nova.NN`,
- `Nova.Vision`,
- `Nova.Audio`,
- wissenschaftliche Berechnungen,
- Multimediafunktionen,
- adaptive Nova-Fähigkeiten,
- zukünftige Hardwarebeschleuniger.

Das grundlegende Architekturprinzip lautet:

> Compute ist in NovaOS eine systemweite Fähigkeit und keine herstellerspezifische Sonderlösung.