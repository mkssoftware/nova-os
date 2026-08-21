# NPSPEC-COMPUTE-RUNTIME-0002 — Scheduler

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS Compute Runtime Scheduler  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0002`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Kurzname:** Compute Scheduler  

---

## 1. Zweck

Diese Spezifikation definiert den **Compute Scheduler** der NovaOS Compute Runtime.

Der Compute Scheduler ist für die Planung, Priorisierung, Platzierung und Koordination von Compute Workloads auf den verfügbaren Recheneinheiten eines Systems verantwortlich.

Er verwaltet insbesondere die Ausführung auf:

- CPU-Kernen,
- CPU-SIMD-Einheiten,
- integrierten GPUs,
- dedizierten GPUs,
- NPUs,
- DSPs,
- FPGA-basierten Beschleunigern,
- virtuellen Compute Devices,
- zukünftigen spezialisierten Compute Accelerators.

Der Compute Scheduler ist vom allgemeinen Kernel-Thread-Scheduler zu unterscheiden.

Der Kernel-Scheduler entscheidet primär:

> Welcher Thread läuft wann auf welcher CPU?

Der Compute Scheduler entscheidet primär:

> Welcher Compute Workload wird wann, mit welcher Priorität und auf welchem Compute Device ausgeführt?

---

## 2. Designprinzip

Das zentrale Scheduling-Prinzip lautet:

> Die Compute Runtime soll nicht einfach das theoretisch schnellste Gerät auswählen, sondern die für den konkreten Workload unter den aktuellen Systembedingungen insgesamt günstigste Ausführungsstrategie.

Dabei MÜSSEN neben reiner Rechenleistung insbesondere berücksichtigt werden:

- aktuelle Geräteauslastung,
- Queue-Länge,
- Datenlokalität,
- Speicherverfügbarkeit,
- Transferkosten,
- Energieverbrauch,
- thermischer Zustand,
- Workload-Priorität,
- Latenzanforderungen,
- Durchsatzanforderungen,
- Sicherheitsrichtlinien,
- Benutzer- und Systemrichtlinien,
- Device Capabilities.

---

## 3. Ziele

Der Compute Scheduler MUSS folgende Ziele verfolgen:

1. geringe Latenz,
2. hoher Gesamtdurchsatz,
3. faire Ressourcenverteilung,
4. effiziente Hardwareauslastung,
5. geringe unnötige Datenbewegung,
6. energieeffiziente Ausführung,
7. thermisch stabile Ausführung,
8. Unterstützung heterogener Hardware,
9. Unterstützung mehrerer Devices,
10. kontrollierte Priorisierung,
11. Vermeidung von Starvation,
12. kontrolliertes Fallback,
13. Device-Failure-Recovery,
14. adaptive Optimierung,
15. geringe Scheduling-Kosten.

---

## 4. Nicht-Ziele

Der Compute Scheduler ersetzt NICHT:

- den CPU-Thread-Scheduler des Kernels,
- den Interrupt-Scheduler,
- den allgemeinen Prozess-Scheduler,
- den Grafik-Compositor,
- einen GPU-Hardware-Scheduler,
- einen Device-Treiber.

Er koordiniert diese Komponenten, soweit dies für Compute Workloads notwendig ist.

---

## 5. Architektur

Die grundlegende Architektur lautet:

```text
Application / Capability
          |
          v
+---------------------------+
| Nova Compute API          |
+---------------------------+
          |
          v
+---------------------------+
| Compute Runtime           |
+---------------------------+
          |
          v
+---------------------------+
| Compute Scheduler         |
|                           |
| - Admission Control       |
| - Dependency Resolver     |
| - Priority Manager        |
| - Device Selector         |
| - Queue Manager           |
| - Load Balancer           |
| - Cost Model              |
| - Power Policy            |
| - Thermal Policy          |
| - Fairness Manager        |
| - Recovery Manager        |
+---------------------------+
          |
          v
+---------------------------+
| Compute Backends          |
+---------------------------+
   |       |       |       |
   v       v       v       v
  CPU     GPU     NPU     DSP
```

---

## 6. Scheduling-Einheiten

Der Scheduler MUSS mindestens folgende Einheiten unterscheiden:

```text
Compute Context
Compute Job
Compute Command
Compute Dispatch
Compute Queue
Compute Device
```

---

## 7. Compute Context

Ein Compute Context bildet die Scheduling- und Sicherheitsdomäne einer Anwendung.

Der Scheduler MUSS mindestens berücksichtigen:

```text
ContextID
OwnerProcess
Priority
QoSClass
AllowedDevices
MemoryBudget
PowerPolicy
SchedulingPolicy
SecurityDomain
```

---

## 8. Compute Job

Ein Compute Job ist eine logisch zusammengehörige Berechnung.

Ein Job kann mehrere Commands enthalten.

Beispiel:

```text
Job
 |
 +-- Copy Input
 |
 +-- Dispatch A
 |
 +-- Barrier
 |
 +-- Dispatch B
 |
 +-- Copy Output
```

Der Scheduler DARF Commands eines Jobs getrennt planen, sofern die definierten Abhängigkeiten eingehalten werden.

---

## 9. Scheduling-Zustände

Ein schedulbarer Workload MUSS mindestens folgende Zustände besitzen können:

```text
CREATED
ADMITTED
BLOCKED
READY
QUEUED
DISPATCHED
RUNNING
PREEMPTED
COMPLETED
FAILED
CANCEL_REQUESTED
CANCELLED
DEVICE_LOST
```

---

## 10. Zustandsmodell

Ein typischer Ablauf lautet:

```text
CREATED
   |
   v
ADMITTED
   |
   v
READY
   |
   v
QUEUED
   |
   v
DISPATCHED
   |
   v
RUNNING
   |
   +---------> PREEMPTED
   |               |
   |               v
   |             READY
   |
   +---------> COMPLETED
   |
   +---------> FAILED
```

Ein Workload mit noch nicht erfüllten Abhängigkeiten befindet sich in:

```text
BLOCKED
```

---

## 11. Admission Control

Vor Aufnahme eines Jobs MUSS der Scheduler prüfen, ob dieser grundsätzlich ausführbar ist.

Zu prüfen sind mindestens:

```text
ValidContext
ValidKernel
RequiredCapabilities
AllowedDevices
MemoryRequirements
ResourceLimits
SecurityPolicy
QueueLimits
```

---

## 12. Ablehnung

Ein Job MUSS abgelehnt werden können, wenn:

- kein kompatibles Device existiert,
- Sicherheitsrichtlinien verletzt werden,
- Ressourcenlimits überschritten werden,
- erforderliche Capabilities fehlen,
- der Context ungültig ist,
- das Compute Backend nicht verfügbar ist.

Eine Ablehnung MUSS einen definierten Fehlercode erzeugen.

---

## 13. Dependency Resolution

Der Scheduler MUSS Abhängigkeiten zwischen Commands und Jobs berücksichtigen.

Beispiel:

```text
A
|
v
B
|
v
C
```

`B` DARF nicht vor Abschluss der notwendigen Operationen von `A` ausgeführt werden.

---

## 14. Parallele Abhängigkeiten

Unabhängige Workloads DÜRFEN parallel ausgeführt werden.

Beispiel:

```text
        +--> B --+
        |       |
A ------+       +--> D
        |       |
        +--> C --+
```

Nach Abschluss von `A` dürfen `B` und `C` parallel laufen.

`D` darf erst ausgeführt werden, wenn seine erforderlichen Abhängigkeiten erfüllt sind.

---

## 15. Dependency Graph

Der Scheduler SOLL interne Abhängigkeiten als gerichteten azyklischen Graphen darstellen können.

```text
Directed Acyclic Graph
DAG
```

Zyklische Abhängigkeiten MÜSSEN erkannt und abgelehnt oder durch eine höhere Runtime-Schicht verhindert werden.

---

## 16. Ready Queue

Workloads, deren Abhängigkeiten erfüllt sind, MÜSSEN in eine oder mehrere Ready Queues aufgenommen werden können.

Eine Implementierung DARF getrennte Ready Queues verwenden für:

```text
CPU
GPU
NPU
DSP
LATENCY_CRITICAL
INTERACTIVE
BACKGROUND
```

Die konkrete interne Struktur ist implementierungsabhängig.

---

## 17. Scheduling Policies

Der Scheduler MUSS mehrere Scheduling Policies unterstützen können.

Mindestens:

```text
SYSTEM_DEFAULT
LATENCY
THROUGHPUT
BALANCED
ENERGY_EFFICIENT
DETERMINISTIC
```

---

## 18. SYSTEM_DEFAULT

`SYSTEM_DEFAULT` verwendet die aktuelle globale NovaOS-Compute-Policy.

Die Runtime entscheidet automatisch zwischen:

- Performance,
- Latenz,
- Energieverbrauch,
- thermischer Belastung,
- Fairness.

---

## 19. LATENCY

`LATENCY` priorisiert geringe Antwortzeiten.

Geeignet für:

- interaktive Anwendungen,
- Echtzeitvorschau,
- Sprachverarbeitung,
- UI-nahe AI,
- Kameraeffekte.

Der Scheduler DARF dafür weniger effiziente Geräte verwenden, wenn dadurch die Gesamtlatenz reduziert wird.

---

## 20. THROUGHPUT

`THROUGHPUT` optimiert den Gesamtdurchsatz.

Geeignet für:

- Batch Processing,
- Rendering,
- wissenschaftliche Berechnungen,
- große Datenverarbeitung,
- AI Training.

---

## 21. BALANCED

`BALANCED` versucht einen Kompromiss zwischen:

```text
Performance
Latency
Energy
Thermals
Fairness
```

zu erreichen.

Dies SOLL die Standardpolicy für normale Anwendungen sein.

---

## 22. ENERGY_EFFICIENT

`ENERGY_EFFICIENT` bevorzugt Recheneinheiten mit geringerem Energieverbrauch.

Beispiel:

```text
AI Inference

NPU = 8 ms / 2 W
GPU = 5 ms / 20 W
CPU = 25 ms / 8 W
```

Wenn keine strenge Latenzanforderung besteht, SOLL die NPU bevorzugt werden können.

---

## 23. DETERMINISTIC

`DETERMINISTIC` priorisiert reproduzierbares Scheduling.

Automatische Migration oder Device-Wechsel DÜRFEN eingeschränkt werden, wenn diese die geforderte Reproduzierbarkeit beeinflussen.

---

## 24. QoS-Klassen

Der Scheduler SOLL mindestens folgende Quality-of-Service-Klassen kennen:

```text
SYSTEM_CRITICAL
REALTIME_SOFT
INTERACTIVE
FOREGROUND
NORMAL
BACKGROUND
MAINTENANCE
```

---

## 25. SYSTEM_CRITICAL

`SYSTEM_CRITICAL` ist ausschließlich für privilegierte NovaOS-Komponenten vorgesehen.

Beispiele:

- sicherheitskritische Systemanalyse,
- kritische Echtzeit-Systemfunktionen,
- Recovery-nahe Compute-Aufgaben.

Normale Anwendungen DÜRFEN diese Klasse nicht anfordern.

---

## 26. REALTIME_SOFT

`REALTIME_SOFT` beschreibt Workloads mit starken Latenzanforderungen, jedoch ohne harte Echtzeitgarantie.

Der Scheduler SOLL Deadline-Verletzungen minimieren.

---

## 27. INTERACTIVE

`INTERACTIVE` ist für Workloads vorgesehen, deren Ergebnis unmittelbar durch den Benutzer wahrgenommen wird.

Beispiele:

```text
UI AI
Image Preview
Camera Filter
Speech Processing
```

---

## 28. FOREGROUND

`FOREGROUND` bezeichnet Compute Workloads der aktuell aktiven Anwendung.

Diese SOLLEN gegenüber normalen Hintergrundjobs bevorzugt werden können.

---

## 29. NORMAL

`NORMAL` ist die Standard-QoS-Klasse normaler Compute Workloads.

---

## 30. BACKGROUND

`BACKGROUND` bezeichnet nicht zeitkritische Berechnungen.

Beispiele:

- Indexierung,
- Cache-Aufbau,
- Analyse,
- Hintergrund-AI.

Diese Workloads DÜRFEN gedrosselt werden.

---

## 31. MAINTENANCE

`MAINTENANCE` ist für besonders niedrig priorisierte Wartungsarbeiten vorgesehen.

Beispiele:

```text
Cache Optimization
Model Precompilation
Background Verification
Data Reorganization
```

---

## 32. Prioritäten

Innerhalb einer QoS-Klasse SOLLEN zusätzliche Prioritäten unterstützt werden.

Mindestens:

```text
LOW
NORMAL
HIGH
CRITICAL
```

`CRITICAL` MUSS privilegiert sein.

---

## 33. Effektive Priorität

Der Scheduler DARF eine interne effektive Priorität berechnen.

Beispiel:

```text
EffectivePriority =
    BasePriority
  + QoSWeight
  + AgingBoost
  + DeadlineBoost
  + ForegroundBoost
  - ResourcePenalty
```

Die konkrete Formel ist nicht Bestandteil des öffentlichen ABI.

---

## 34. Priority Aging

Um Starvation zu verhindern, SOLL die Priorität wartender Jobs schrittweise erhöht werden können.

Beispiel:

```text
Waiting Time
    |
    v
Aging Boost
    |
    v
Higher Effective Priority
```

---

## 35. Starvation Prevention

Ein dauerhaft ausführbarer normaler Job DARF nicht unbegrenzt blockiert werden, nur weil ständig höher priorisierte normale Jobs eintreffen.

Ausnahmen DÜRFEN für privilegierte Echtzeit- oder System-Workloads gelten.

---

## 36. Fairness

Der Scheduler MUSS Fairness zwischen Compute Contexts gewährleisten können.

Ein einzelner Prozess DARF ein gemeinsam genutztes Compute Device nicht unbegrenzt monopolisieren.

---

## 37. Fair-Share-Modell

Die Implementierung DARF ein gewichtetes Fair-Share-Modell verwenden.

Beispiel:

```text
Context A Weight = 100
Context B Weight = 100
Context C Weight = 50
```

Die konkrete Scheduling-Strategie bleibt implementierungsabhängig.

---

## 38. Device Selection

Für jeden automatisch geplanten Workload MUSS ein geeignetes Device ausgewählt werden.

Der Scheduler MUSS zunächst alle inkompatiblen Devices ausschließen.

---

## 39. Capability Filter

Ein Device DARF nur ausgewählt werden, wenn alle zwingend erforderlichen Capabilities vorhanden sind.

Beispiel:

```text
Required:

FP16
MATRIX
LOCAL_MEMORY
```

Ein Device ohne `MATRIX` MUSS ausgeschlossen werden.

---

## 40. Allowed Device Filter

Der Compute Context kann die zulässigen Devices beschränken.

Beispiel:

```text
AllowedDevices:

CPU
GPU
```

Eine vorhandene NPU DARF in diesem Context nicht verwendet werden.

---

## 41. Device Health Filter

Devices mit folgenden Zuständen SOLLEN nicht für neue Jobs verwendet werden:

```text
FAILED
RESETTING
OFFLINE
REMOVED
UNHEALTHY
```

---

## 42. Device Availability

Der Scheduler MUSS den aktuellen Verfügbarkeitszustand eines Devices berücksichtigen.

Mögliche Zustände:

```text
AVAILABLE
BUSY
SATURATED
THROTTLED
UNAVAILABLE
```

---

## 43. Device Cost Model

Nach der Filterung SOLL der Scheduler die verbleibenden Devices bewerten.

Ein abstraktes Kostenmodell kann lauten:

```text
TotalCost =
      ExecutionCost
    + TransferCost
    + QueueCost
    + MemoryCost
    + PowerCost
    + ThermalCost
    + MigrationCost
```

---

## 44. Performance Score

Der Scheduler SOLL die erwartete Ausführungsleistung berücksichtigen.

Diese kann bestimmt werden durch:

- statische Hardwareinformationen,
- Benchmarkwerte,
- historische Laufzeitdaten,
- Kernel-spezifische Messungen,
- aktuelle Taktrate,
- aktuelle Auslastung.

---

## 45. Queue Cost

Ein schnelles Device kann aufgrund einer langen Queue langsamer reagieren als ein weniger leistungsfähiges freies Device.

Beispiel:

```text
GPU:

Execution = 2 ms
QueueWait = 20 ms
Total     = 22 ms
```

```text
CPU:

Execution = 8 ms
QueueWait = 0 ms
Total     = 8 ms
```

Der Scheduler SOLL in diesem Fall die CPU auswählen können.

---

## 46. Transfer Cost

Der Scheduler MUSS die Kosten notwendiger Datenübertragungen berücksichtigen können.

Beispiel:

```text
CPU Data
   |
   | 12 ms
   v
GPU
   |
   | 1 ms Compute
   v
Result
```

gegenüber:

```text
CPU Data
   |
   | 4 ms Compute
   v
Result
```

Die CPU ist in diesem Beispiel insgesamt günstiger.

---

## 47. Data Locality

Der Scheduler SOLL ein Device bevorzugen können, auf dem benötigte Daten bereits resident sind.

Beispiel:

```text
Buffer A -> GPU0
Buffer B -> GPU0
```

Ein nachfolgender Workload auf diesen Buffern SOLL GPU0 gegenüber GPU1 bevorzugen können.

---

## 48. Memory Availability

Ein Device DARF nicht ausgewählt werden, wenn die benötigten Ressourcen nicht bereitgestellt werden können.

Der Scheduler MUSS mindestens berücksichtigen:

```text
AvailableDeviceMemory
RequiredDeviceMemory
TemporaryMemory
LocalMemory
```

---

## 49. Memory Pressure

Bei hohem Speicherdruck DARF der Scheduler:

- andere Devices auswählen,
- Jobs verzögern,
- Speicher evicten lassen,
- kleinere Batches verwenden,
- Workloads aufteilen.

---

## 50. NUMA Awareness

Auf NUMA-Systemen SOLL der Scheduler die Speicherlokalität berücksichtigen.

Beispiel:

```text
NUMA Node 0
 |
 +-- CPU0
 |
 +-- RAM0
 |
 +-- GPU0
```

Ein Workload mit Daten in `RAM0` SOLL nach Möglichkeit lokal ausgeführt werden.

---

## 51. Device Topology

Der Scheduler SOLL die physische Compute-Topologie berücksichtigen.

Mögliche Topologieinformationen:

```text
NUMA Node
CPU Socket
PCIe Root
PCIe Generation
PCIe Width
Shared Memory Domain
Shared Cache
High-Speed Interconnect
```

---

## 52. Topology Cost

Direkt verbundene Devices SOLLEN gegenüber topologisch entfernten Devices bevorzugt werden können.

Beispiel:

```text
GPU0 <---- High-Speed Link ----> GPU1
```

kann für Multi-GPU-Workloads günstiger sein als:

```text
GPU0 -> RAM -> GPU2
```

---

## 53. Small Workload Scheduling

Kleine Workloads SOLLEN nicht automatisch auf einen Beschleuniger verschoben werden.

Der Scheduler MUSS berücksichtigen:

```text
DispatchOverhead
TransferOverhead
SynchronizationOverhead
CompilationOverhead
```

---

## 54. Large Workload Scheduling

Große Workloads SOLLEN bevorzugt auf Devices ausgeführt werden können, die hohen Durchsatz bieten.

Der Scheduler DARF einen großen Workload auf mehrere Devices aufteilen.

---

## 55. Workload Splitting

Ein teilbarer Workload DARF in mehrere Teiljobs zerlegt werden.

Beispiel:

```text
Workload
   |
   +---- Part 0 -> GPU0
   |
   +---- Part 1 -> GPU1
   |
   +---- Part 2 -> CPU
```

Die Aufteilung DARF nur erfolgen, wenn die Semantik dies erlaubt.

---

## 56. Split Capability

Ein Job SOLL explizit deklarieren können, ob er teilbar ist.

Beispiel:

```text
SPLITTABLE
```

Ohne entsprechende Deklaration DARF die Runtime einen semantisch atomaren Job nicht willkürlich aufteilen.

---

## 57. Multi-Device Scheduling

Der Scheduler SOLL mehrere Devices gleichzeitig verwenden können.

Beispiel:

```text
            +--> GPU0
            |
Input ------+--> GPU1
            |
            +--> GPU2
```

---

## 58. Heterogeneous Scheduling

Ein Job DARF verschiedene Device-Klassen verwenden.

Beispiel:

```text
Input
 |
 v
CPU Preprocessing
 |
 v
GPU Transform
 |
 v
NPU Inference
 |
 v
CPU Postprocessing
```

---

## 59. Pipeline Scheduling

Der Scheduler SOLL Pipelines über mehrere Devices unterstützen können.

Beispiel:

```text
Frame 1: CPU -> GPU -> NPU
Frame 2:       CPU -> GPU -> NPU
Frame 3:             CPU -> GPU -> NPU
```

Dadurch können mehrere Pipeline-Stufen gleichzeitig aktiv sein.

---

## 60. Batch Scheduling

Mehrere kleine Workloads SOLLEN zu größeren Batches zusammengefasst werden können.

Dies ist insbesondere sinnvoll für:

- GPU Dispatch,
- NPU Inference,
- kleine Matrixoperationen,
- ähnliche AI-Anfragen.

Batching DARF Latenzgrenzen nicht verletzen.

---

## 61. Batch Window

Der Scheduler DARF für batchfähige Hintergrundjobs kurz auf weitere kompatible Jobs warten.

Beispiel:

```text
BatchWindow = 2 ms
```

Für `LATENCY_CRITICAL` SOLL kein unnötiges Batch-Warten erfolgen.

---

## 62. Queue Selection

Nach Device Selection MUSS eine geeignete Device Queue ausgewählt werden.

Dabei können berücksichtigt werden:

```text
QueueType
QueuePriority
QueueLoad
OrderingRequirements
PreemptionSupport
```

---

## 63. Queue Types

Ein Backend DARF unterschiedliche Queue-Typen bereitstellen.

Beispiele:

```text
COMPUTE
TRANSFER
HIGH_PRIORITY_COMPUTE
LOW_PRIORITY_COMPUTE
```

Der Scheduler SOLL spezialisierte Queues verwenden können.

---

## 64. Transfer Queues

Wenn ein Device separate Transfer Engines besitzt, SOLL der Scheduler Speichertransfers parallel zu Compute Workloads ausführen können.

Beispiel:

```text
Compute Engine:  Kernel A

Transfer Engine: Upload B
```

---

## 65. Overlap

Der Scheduler SOLL folgende Operationen überlappen können, sofern Hardware und Abhängigkeiten dies erlauben:

```text
Compute
Transfer
Compilation
Preparation
```

---

## 66. Preemption

Wenn ein Device Preemption unterstützt, SOLL der Scheduler laufende Workloads unterbrechen können.

Typischer Fall:

```text
BACKGROUND Job running
        |
        v
INTERACTIVE Job arrives
        |
        v
Preempt Background
        |
        v
Run Interactive
        |
        v
Resume Background
```

---

## 67. Preemption Granularity

Ein Device kann unterschiedliche Preemption-Granularitäten besitzen.

Beispiele:

```text
QUEUE
DISPATCH
WORKGROUP
WAVE
INSTRUCTION
```

Die Runtime MUSS die tatsächlichen Fähigkeiten des Devices berücksichtigen.

---

## 68. Preemption Cost

Preemption DARF nicht automatisch erfolgen.

Der Scheduler SOLL berücksichtigen:

```text
SaveStateCost
RestoreStateCost
CacheLoss
PipelineFlushCost
```

---

## 69. Cooperative Preemption

Wenn Hardware keine echte Preemption unterstützt, DARF die Runtime kooperative Mechanismen verwenden.

Beispiele:

- kleinere Dispatches,
- Checkpoints,
- Job-Slicing,
- kontrollierte Yield Points.

---

## 70. Time Slicing

Gemeinsam genutzte Devices DÜRFEN mittels Time Slicing zwischen Contexts aufgeteilt werden.

Die Slice-Dauer ist implementierungsabhängig.

---

## 71. Adaptive Time Slice

Die Runtime DARF die Time-Slice-Länge dynamisch anpassen.

Kurze Slices:

- bessere Interaktivität,
- höherer Scheduling-Overhead.

Lange Slices:

- höherer Durchsatz,
- schlechtere Interaktivität.

---

## 72. Deadline Scheduling

Jobs DÜRFEN eine gewünschte Deadline deklarieren.

Beispiel:

```text
Deadline = 16 ms
```

Der Scheduler SOLL versuchen, diese einzuhalten.

Eine normale Compute Deadline stellt keine harte Echtzeitgarantie dar.

---

## 73. Deadline Feasibility

Der Scheduler DARF vor Ausführung abschätzen, ob eine Deadline realistisch erreichbar ist.

Beispiel:

```text
EstimatedExecution = 25 ms
DeadlineRemaining  = 10 ms
```

Die Runtime kann den Workload als gefährdet markieren.

---

## 74. Deadline Boost

Ein Job, dessen Deadline näher rückt, DARF temporär eine höhere effektive Priorität erhalten.

---

## 75. Deadline Miss

Eine verfehlte Deadline MUSS nicht automatisch zum Abbruch führen.

Mögliche Policies:

```text
CONTINUE
CANCEL
DEGRADE
NOTIFY
```

---

## 76. Power-Aware Scheduling

Der Scheduler MUSS Power Policies berücksichtigen können.

Mögliche Modi:

```text
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 77. Performance Mode

Im Performance-Modus DARF der Scheduler:

- leistungsfähigere Devices bevorzugen,
- höhere Taktraten begünstigen,
- mehr parallele Devices verwenden,
- aggressiver vorladen.

Thermische und Sicherheitsgrenzen DÜRFEN nicht überschritten werden.

---

## 78. Efficiency Mode

Im Efficiency-Modus SOLLEN energieeffiziente Devices bevorzugt werden.

Beispiele:

```text
NPU statt GPU

Efficiency Core statt Performance Core
```

sofern die Workload-Anforderungen erfüllt werden.

---

## 79. Battery Saver

Im Battery-Saver-Modus DARF die Runtime:

- Hintergrundcompute reduzieren,
- Batchgrößen verändern,
- leistungsintensive Devices vermeiden,
- Jobs verzögern,
- niedrigere Performance States verwenden.

---

## 80. Thermal-Aware Scheduling

Der Scheduler MUSS thermische Zustände berücksichtigen können.

Mindestens:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

---

## 81. Thermal Migration

Ein Workload DARF von einem thermisch belasteten Device auf ein anderes Device verschoben werden, sofern:

- der Job dies erlaubt,
- notwendige Capabilities vorhanden sind,
- Datenmigration möglich ist,
- die Semantik erhalten bleibt.

---

## 82. Thermal Distribution

Bei mehreren Devices DARF die Runtime Last verteilen, um lokale Überhitzung zu reduzieren.

Beispiel:

```text
GPU0 = 90 °C
GPU1 = 60 °C
```

Neue geeignete Workloads können bevorzugt auf GPU1 geplant werden.

---

## 83. Device Throttling

Der Scheduler MUSS berücksichtigen, wenn ein Device bereits hardwareseitig gedrosselt wird.

Nominale Spitzenleistung DARF dann nicht als aktuelle Leistung angenommen werden.

---

## 84. Dynamic Performance Data

Scheduling-Entscheidungen SOLLEN aktuelle Messwerte verwenden können.

Beispiele:

```text
CurrentClock
CurrentUtilization
MemoryBandwidth
QueueLatency
Temperature
PowerDraw
```

---

## 85. Historical Performance Model

NovaOS DARF historische Compute-Messungen speichern und für Scheduling verwenden.

Möglicher Schlüssel:

```text
KernelHash
InputDimensions
DataType
DeviceArchitecture
BackendVersion
```

Mögliche Werte:

```text
AverageExecutionTime
AverageTransferTime
EnergyEstimate
Variance
```

---

## 86. Cold Start

Wenn keine historischen Daten existieren, MUSS der Scheduler mit statischen oder heuristischen Schätzungen arbeiten können.

---

## 87. Online Learning

Nach abgeschlossenen Jobs DARF der Scheduler sein internes Performance-Modell aktualisieren.

Beispiel:

```text
Prediction = 8 ms
Actual     = 11 ms
```

Die zukünftige Schätzung kann entsprechend angepasst werden.

---

## 88. Confidence

Historische Performance-Schätzungen SOLLEN einen Vertrauenswert besitzen können.

Beispiel:

```text
EstimatedRuntime = 4.2 ms
Confidence       = 0.94
```

Niedrige Confidence SOLL vorsichtiger gewichtet werden.

---

## 89. Exploration

Die Runtime DARF gelegentlich alternative Devices testen, um bessere Scheduling-Daten zu erhalten.

Dies DARF nicht erfolgen, wenn:

- Determinismus verlangt wird,
- harte Sicherheitsrichtlinien entgegenstehen,
- kritische Latenzanforderungen bestehen,
- der Benutzer dies deaktiviert hat.

---

## 90. Migration

Ein noch nicht gestarteter Job DARF jederzeit auf ein anderes kompatibles Device umgeplant werden.

---

## 91. Running Job Migration

Die Migration eines bereits laufenden Jobs ist nur erlaubt, wenn:

- Backend und Device dies unterstützen,
- der Job migrationsfähig ist,
- der Zustand sicher übertragen werden kann.

---

## 92. Migration Cost

Vor einer Migration MUSS der Scheduler deren Kosten berücksichtigen.

```text
MigrationCost =
    StateTransfer
  + DataTransfer
  + RestartCost
  + CacheWarmup
```

---

## 93. Fallback

Wenn das ausgewählte Device nicht mehr verfügbar ist, SOLL der Scheduler nach einem kompatiblen Fallback suchen.

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

---

## 94. Fallback Policy

Ein Job MUSS deklarieren können:

```text
ALLOW_FALLBACK
NO_FALLBACK
```

Bei `NO_FALLBACK` MUSS der Job fehlschlagen, wenn das erforderliche Device nicht verfügbar ist.

---

## 95. Required Device

Ein explizit angefordertes Device MUSS respektiert werden.

Beispiel:

```text
RequiredDevice = GPU0
```

Fällt `GPU0` aus, MUSS der Job fehlschlagen, sofern die Anwendung nicht ausdrücklich alternative Devices zugelassen hat.

---

## 96. Preferred Device

Ein bevorzugtes Device stellt lediglich einen Scheduling Hint dar.

Beispiel:

```text
PreferredDevice = NPU
```

Die Runtime DARF ein anderes Device verwenden.

---

## 97. Device Failure

Bei einem Device Failure MUSS der Scheduler:

1. das Device für neue Jobs sperren,
2. laufende Jobs erfassen,
3. betroffene Queues stoppen,
4. Jobs entsprechend ihrem Status behandeln,
5. alternative Devices prüfen,
6. Recovery koordinieren.

---

## 98. Retry-Safe Jobs

Jobs mit:

```text
RETRY_SAFE
```

DÜRFEN nach einem Device Failure automatisch neu geplant werden.

---

## 99. Non-Retry-Safe Jobs

Nicht als `RETRY_SAFE` markierte Jobs DÜRFEN nicht automatisch wiederholt werden, wenn dadurch externe Seiteneffekte doppelt auftreten könnten.

---

## 100. Device Recovery

Während eines Device Reset MUSS das Device aus der normalen Scheduling-Menge entfernt werden.

```text
AVAILABLE
   |
   v
FAILED
   |
   v
RESETTING
   |
   v
RECOVERING
   |
   v
AVAILABLE
```

---

## 101. Queue Drain

Vor geplanten Device-Wartungsmaßnahmen SOLL der Scheduler Queues kontrolliert leeren können.

---

## 102. Device Quiesce

Ein Device SOLL in einen Zustand versetzt werden können, in dem:

- keine neuen Jobs angenommen werden,
- bestehende Jobs beendet werden,
- anschließend das Device sicher gewartet oder deaktiviert werden kann.

Zustand:

```text
QUIESCING
```

---

## 103. Resource Reservation

Jobs DÜRFEN Ressourcen vor Ausführung reservieren.

Beispiele:

```text
DeviceMemory
QueueSlot
ComputeUnits
LocalMemory
```

---

## 104. Reservation Deadlock

Der Scheduler MUSS verhindern, dass mehrere Jobs durch partielle Ressourcenreservierungen dauerhaft aufeinander warten.

---

## 105. Reservation Strategy

Eine Implementierung SOLL entweder:

- atomare Reservierung,
- geordnete Ressourcenreservierung,
- Rollback unvollständiger Reservierungen

verwenden.

---

## 106. Memory Pressure Scheduling

Bei hohem Speicherdruck DARF der Scheduler neue Jobs verzögern.

Ein Job SOLL nicht gestartet werden, wenn bereits vorhersehbar ist, dass seine zwingend benötigten Ressourcen nicht bereitgestellt werden können.

---

## 107. Compute Pressure

NovaOS SOLL einen globalen Compute-Pressure-Wert bereitstellen können.

Beispiel:

```text
LOW
MODERATE
HIGH
CRITICAL
```

Dieser kann von Systemdiensten und Anwendungen verwendet werden.

---

## 108. Backpressure

Wenn Compute Queues überlastet sind, MUSS die Runtime Backpressure erzeugen können.

Mögliche Mechanismen:

- verzögerte Admission,
- Queue Limits,
- Submission Throttling,
- Prioritätsreduktion,
- Fehler bei Ressourcenerschöpfung.

---

## 109. Queue Depth Limits

Jeder Context SOLL maximale Queue-Tiefen besitzen können.

Dadurch wird verhindert, dass ein Prozess unbegrenzt zukünftige Arbeit einreiht.

---

## 110. Outstanding Job Limits

Die Anzahl gleichzeitig ausstehender Jobs MUSS begrenzbar sein.

---

## 111. Scheduler Overhead

Der Scheduler selbst MUSS auf geringe Laufzeitkosten optimiert werden.

Insbesondere SOLLEN vermieden werden:

- globale Locks auf Hot Paths,
- unnötige Speicherallokationen,
- vollständige Device-Scans für jeden Dispatch,
- unnötige Kernel/User-Transitions,
- unnötige Datenkopien.

---

## 112. Fast Path

Für einfache Fälle SOLL ein Fast Path existieren.

Beispiel:

```text
Known Kernel
+
Known Device
+
Free Queue
+
Resident Data
=
Direct Fast Submission
```

---

## 113. Slow Path

Komplexe Entscheidungen DÜRFEN über einen Slow Path verarbeitet werden.

Beispiele:

- Device Migration,
- Multi-Device Planning,
- thermische Neuverteilung,
- Device Failure,
- komplexe Dependency Graphs.

---

## 114. Scheduling Cache

Wiederkehrende Workloads DÜRFEN Scheduling-Entscheidungen cachen.

Beispiel:

```text
KernelHash
+
InputSize
+
SystemStateClass
=
PreferredDevice
```

Der Cache MUSS bei relevanten Hardware- oder Backend-Änderungen invalidiert werden.

---

## 115. Foreground Awareness

Der Scheduler SOLL wissen können, welcher Prozess oder welche Anwendung aktuell im Vordergrund steht.

Foreground-Status DARF die effektive Priorität beeinflussen.

---

## 116. User Interaction Boost

Nach direkter Benutzerinteraktion DARF eine Anwendung kurzfristig einen Compute Boost erhalten.

Beispiel:

```text
Mouse Click
   |
   v
Interactive Compute Request
   |
   v
Temporary Priority Boost
```

Der Boost MUSS zeitlich begrenzt sein.

---

## 117. Background Demotion

Verliert eine Anwendung den Vordergrundstatus, DÜRFEN nicht interaktive Compute Jobs schrittweise auf normale oder Background-Priorität reduziert werden.

---

## 118. Graphics Coordination

GPU Compute MUSS mit Grafik-Workloads koordiniert werden können.

Der Desktop-Compositor und interaktive Grafik DÜRFEN nicht durch normale Compute Jobs dauerhaft blockiert werden.

---

## 119. Graphics Reserve

Die Runtime DARF einen Teil der GPU-Kapazität für Grafik reservieren.

---

## 120. Media Coordination

Hardware, die Compute und Media Engines teilt, SOLL entsprechend koordiniert werden.

Beispiele:

- Video Decode,
- Video Encode,
- Image Processing,
- GPU Compute.

---

## 121. AI Coordination

NPU-Workloads unterschiedlicher Anwendungen MÜSSEN fair geplant werden können.

Systemweite AI-Funktionen DÜRFEN reservierte oder bevorzugte Kapazität besitzen, wenn dies für die Systemfunktion notwendig ist.

---

## 122. Scheduler und CPU Backend

CPU Compute Jobs MÜSSEN mit dem normalen Kernel-CPU-Scheduler koordiniert werden.

Die Compute Runtime DARF CPU-Threads nicht außerhalb des regulären Kernel-Scheduling-Modells ausführen.

---

## 123. CPU Worker Pool

Das CPU Compute Backend SOLL einen verwalteten Worker Pool verwenden können.

Beispiel:

```text
Compute Scheduler
      |
      v
CPU Worker Pool
      |
 +----+----+----+
 |    |    |    |
CPU0 CPU1 CPU2 CPU3
```

---

## 124. Hybrid CPU Scheduling

Bei Hybrid CPUs SOLL zwischen Kernklassen unterschieden werden.

Beispiel:

```text
P-Core
E-Core
```

Mögliche Policy:

```text
INTERACTIVE
    -> P-Core

BACKGROUND
    -> E-Core
```

Die tatsächliche Entscheidung MUSS aktuelle Auslastung und Energiepolitik berücksichtigen.

---

## 125. CPU Affinity

Compute Jobs DÜRFEN CPU-Affinity-Hints besitzen.

Diese SOLLEN nur dann zwingend sein, wenn die Anwendung eine entsprechende Berechtigung oder einen technischen Grund besitzt.

---

## 126. Cache Locality

CPU Compute Scheduling SOLL Cache-Lokalität berücksichtigen.

Ein Workload kann bevorzugt auf einem Kern weiterlaufen, dessen Cache bereits relevante Daten enthält.

---

## 127. SMT Awareness

Der Scheduler SOLL Simultaneous Multithreading berücksichtigen.

Zwei rechenintensive Threads auf demselben physischen Kern können sich gegenseitig stärker beeinträchtigen als Threads auf getrennten Kernen.

---

## 128. SMT Policy

Bei stark rechenintensiven Workloads DARF der Scheduler zunächst unterschiedliche physische Kerne verwenden, bevor zusätzliche SMT-Threads belegt werden.

Bei anderen Workloads kann SMT höhere Gesamtauslastung ermöglichen.

---

## 129. Device Exclusivity

Ein Job DARF exklusiven Device-Zugriff anfordern, wenn dies technisch notwendig ist.

Exklusiver Zugriff MUSS privilegiert oder durch Policy beschränkt sein.

---

## 130. Exclusive Scheduling

Während exklusiver Nutzung DÜRFEN normale Jobs nicht auf dem betreffenden Device gestartet werden.

Systemkritische Sicherheits- oder Recovery-Funktionen DÜRFEN Vorrang besitzen.

---

## 131. Deterministic Queueing

Im deterministischen Modus SOLL die Reihenfolge relevanter Jobs reproduzierbar sein.

Dynamische Load-Balancing-Entscheidungen DÜRFEN dafür eingeschränkt werden.

---

## 132. Scheduler Security

Scheduling-Entscheidungen DÜRFEN keine Sicherheitsgrenzen umgehen.

Ein höher priorisierter Job erhält dadurch nicht automatisch Zugriff auf:

- fremde Buffers,
- fremde Queues,
- verbotene Devices,
- privilegierte Device Features.

---

## 133. Priority Abuse Protection

Normale Anwendungen DÜRFEN ihre Priorität nicht unbegrenzt erhöhen.

Die Runtime MUSS Prioritätsanforderungen gegen Berechtigungen und Policies validieren.

---

## 134. Resource Exhaustion Protection

Der Scheduler MUSS gegen absichtliche oder unbeabsichtigte Ressourcenerschöpfung geschützt sein.

Beispiele:

- Millionen leerer Jobs,
- extrem tiefe Dependency Graphs,
- übermäßig viele Queues,
- permanent blockierte Jobs.

---

## 135. Dependency Limits

Die Runtime DARF Limits setzen für:

```text
MaximumDependenciesPerJob
MaximumGraphDepth
MaximumGraphNodes
```

---

## 136. Scheduler Isolation

Ein Fehler in einem normalen Compute Context DARF den Scheduler anderer Contexts nicht beschädigen.

---

## 137. Diagnostics

Der Scheduler MUSS Diagnoseinformationen bereitstellen können.

Mindestens:

```text
ReadyJobs
BlockedJobs
RunningJobs

JobsPerDevice

QueueDepth

AverageWaitTime
AverageExecutionTime

DeviceUtilization

PreemptionCount
MigrationCount
FallbackCount

DeadlineMisses

RejectedJobs
```

---

## 138. Scheduling Trace Events

Der Scheduler SOLL mindestens folgende Trace Events erzeugen können:

```text
COMPUTE_SCHED_ADMIT

COMPUTE_SCHED_READY

COMPUTE_SCHED_BLOCK

COMPUTE_SCHED_SELECT_DEVICE

COMPUTE_SCHED_QUEUE

COMPUTE_SCHED_DISPATCH

COMPUTE_SCHED_START

COMPUTE_SCHED_PREEMPT

COMPUTE_SCHED_RESUME

COMPUTE_SCHED_MIGRATE

COMPUTE_SCHED_FALLBACK

COMPUTE_SCHED_COMPLETE

COMPUTE_SCHED_FAIL
```

---

## 139. Scheduling Decision Trace

Debug- und Profiling-Modi SOLLEN erklären können, warum ein Device ausgewählt wurde.

Beispiel:

```text
Selected Device: NPU0

Reasons:

+ Required FP16 supported
+ Tensor acceleration available
+ Input already resident
+ Queue empty
+ Low power consumption

Rejected GPU0:

- Queue wait estimated at 18 ms

Rejected CPU:

- Estimated runtime 42 ms
```

---

## 140. Explainability

Automatische Scheduling-Entscheidungen SOLLEN für Entwickler diagnostizierbar sein.

Die Runtime MUSS jedoch nicht sämtliche internen Heuristiken als öffentliches ABI offenlegen.

---

## 141. Performance Metrics

Der Scheduler SOLL mindestens messen können:

```text
SubmissionLatency
AdmissionLatency
QueueWaitTime
DispatchLatency
ExecutionTime
TransferTime
TotalJobLatency
```

---

## 142. Device Metrics

Pro Device SOLLEN mindestens folgende Werte verfügbar sein:

```text
Utilization
QueueDepth
MemoryPressure
Temperature
PowerState
HealthState
```

---

## 143. Scheduler Metrics

Globale Metriken können umfassen:

```text
JobsPerSecond
AverageSchedulingLatency
DeviceSelectionTime
PreemptionsPerSecond
MigrationsPerSecond
FallbacksPerSecond
```

---

## 144. Privacy

Historische Scheduling-Daten DÜRFEN für lokale Optimierung verwendet werden.

Sie SOLLEN standardmäßig lokal bleiben.

Eine externe Übertragung DARF nicht allein aufgrund des Compute Scheduling erfolgen.

---

## 145. Persistent Learning Data

Persistente Performance-Profile SOLLEN keine unnötigen personenbezogenen Informationen enthalten.

Bevorzugt werden technische Schlüssel wie:

```text
KernelHash
DeviceArchitecture
InputShape
BackendVersion
```

---

## 146. Scheduler Reset

Die Compute Runtime MUSS den Scheduler kontrolliert neu initialisieren können.

Bereits laufende Jobs MÜSSEN dabei entweder:

- abgeschlossen,
- kontrolliert abgebrochen,
- als fehlgeschlagen markiert

werden.

---

## 147. Runtime Shutdown

Beim Herunterfahren MUSS der Scheduler:

1. neue Admissions stoppen,
2. Queues kontrolliert schließen,
3. laufende Jobs entsprechend der Shutdown-Policy behandeln,
4. Ressourcen freigeben,
5. Backends quiescen.

---

## 148. Suspend

Vor System-Suspend MUSS die Runtime aktive Compute Workloads berücksichtigen.

Mögliche Aktionen:

```text
COMPLETE
PAUSE
CHECKPOINT
CANCEL
FAIL
```

abhängig von Device und Job.

---

## 149. Resume

Nach Resume MUSS der Scheduler:

- Devices neu validieren,
- Backend-Zustände prüfen,
- verlorene Devices erkennen,
- gespeicherte Jobs wiederherstellen, sofern unterstützt,
- Scheduling-Daten aktualisieren.

---

## 150. Hot-Plug

Beim Hinzufügen eines neuen Compute Devices SOLL dieses nach erfolgreicher Initialisierung in die Scheduling-Menge aufgenommen werden.

---

## 151. Hot-Unplug

Vor einem geplanten Hot-Unplug SOLL das Device:

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

durchlaufen.

---

## 152. Ungeplanter Device-Verlust

Bei ungeplantem Entfernen MUSS unmittelbar:

```text
DEVICE_LOST
```

erzeugt werden.

Der Scheduler MUSS betroffene Jobs kontrolliert behandeln.

---

## 153. Scheduler Backend Interface

Compute Backends SOLLEN dem Scheduler mindestens folgende Informationen bereitstellen können:

```text
DeviceCapabilities

QueueCapabilities

CurrentLoad

MemoryAvailability

PreemptionSupport

EstimatedDispatchCost

HealthState

PowerState

ThermalState
```

---

## 154. Backend Scheduling Hints

Backends DÜRFEN zusätzliche Scheduling Hints bereitstellen.

Beispiele:

```text
PreferredWorkgroupSize
PreferredBatchSize
MaximumConcurrentQueues
OptimalTransferSize
```

Diese Hints DÜRFEN nicht als Sicherheitsgarantie interpretiert werden.

---

## 155. Hardware Scheduler

Besitzt ein Device einen eigenen Hardware Scheduler, SOLL NovaOS mit diesem kooperieren.

Die Compute Runtime bleibt dennoch für globale Entscheidungen zwischen Devices und Contexts verantwortlich.

---

## 156. Scheduler Hierarchie

Eine mögliche Scheduling-Hierarchie lautet:

```text
Nova Compute Scheduler
          |
          v
Device Scheduler
          |
          v
Backend Scheduler
          |
          v
Hardware Scheduler
```

Nicht jede Ebene MUSS auf jeder Plattform vorhanden sein.

---

## 157. Scheduling Granularity

Die Runtime SOLL je nach Backend auf unterschiedlichen Ebenen planen können:

```text
Job
Command
Dispatch
Workgroup
Task
```

Feinere Hardwareebenen können dem Backend überlassen werden.

---

## 158. Fast Submission

Wenn die Anwendung ein explizites Device und eine explizite Queue verwendet, DARF die Runtime komplexe automatische Device Selection überspringen.

Sicherheits- und Ressourcenprüfungen MÜSSEN weiterhin erfolgen.

---

## 159. Automatic Submission

Bei automatischer Submission übernimmt der Scheduler:

```text
Dependency Check
     |
     v
Capability Filter
     |
     v
Device Evaluation
     |
     v
Cost Calculation
     |
     v
Device Selection
     |
     v
Queue Selection
     |
     v
Dispatch
```

---

## 160. Scheduling Algorithm

Diese Spezifikation schreibt keinen einzelnen festen Scheduling-Algorithmus vor.

Zulässig sind unter anderem:

- Priority Scheduling,
- Weighted Fair Queuing,
- Deadline Scheduling,
- Work Stealing,
- Cost-Based Scheduling,
- Heuristic Scheduling,
- adaptive Kombinationen.

Die beobachtbare Semantik dieser Spezifikation MUSS eingehalten werden.

---

## 161. Work Stealing

CPU Worker Pools und geeignete Multi-Queue-Systeme DÜRFEN Work Stealing verwenden.

Beispiel:

```text
Worker 0 Queue: [A][B][C]

Worker 1 Queue: []

Worker 1 steals C
```

Dies kann die CPU-Auslastung verbessern.

---

## 162. Locality-Aware Work Stealing

Work Stealing SOLL Daten- und Cache-Lokalität berücksichtigen.

Ein Task DARF nicht unnötig verschoben werden, wenn die Migrationskosten größer als der erwartete Nutzen sind.

---

## 163. Scheduler Scalability

Der Scheduler MUSS mit steigender Anzahl von:

- CPU-Kernen,
- Compute Devices,
- Queues,
- Prozessen,
- Jobs

skalieren können.

Eine einzelne globale Scheduling Queue SOLL vermieden werden, wenn sie auf großen Systemen zum Bottleneck wird.

---

## 164. Locking

Performancekritische Scheduler-Datenstrukturen SOLLEN:

- lock-arm,
- partitioniert,
- per-Device,
- per-CPU

organisiert werden können.

Lock-Free-Strukturen DÜRFEN verwendet werden, sind jedoch nicht verpflichtend.

---

## 165. Concurrency

Mehrere Threads MÜSSEN gleichzeitig Compute Jobs einreichen können.

Der Scheduler MUSS konkurrierende Submission korrekt synchronisieren.

---

## 166. Ordering

Scheduling-Optimierungen DÜRFEN keine explizit definierten Ordering-Anforderungen verletzen.

---

## 167. Memory Ordering

Der Scheduler MUSS notwendige Memory Barriers respektieren.

Ein Job DARF nicht nur aufgrund besserer Performance vor eine notwendige Speicherabhängigkeit verschoben werden.

---

## 168. Cancellation Race

Folgende Race Condition MUSS korrekt behandelt werden:

```text
Thread A:
Cancel(Job)

Thread B:
Dispatch(Job)
```

Der resultierende Zustand MUSS definiert sein.

---

## 169. Device-Lost Race

Auch gleichzeitige Ereignisse wie:

```text
Job Completion
+
Device Lost
```

MÜSSEN zu einem konsistenten Job-Zustand führen.

---

## 170. Testing — Basic Scheduling

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
Single Job
Multiple Jobs
Single Device
Multiple Devices
Single Context
Multiple Contexts
```

---

## 171. Testing — Priority

Zu testen sind:

- LOW gegen NORMAL,
- NORMAL gegen HIGH,
- Foreground gegen Background,
- Priority Aging,
- Starvation Prevention.

---

## 172. Testing — Device Selection

Testfälle MÜSSEN mindestens umfassen:

```text
Fast GPU / Busy GPU
Slow CPU / Free CPU
Data Locality
Insufficient Device Memory
Missing Capability
Thermal Throttling
Power Saving
```

---

## 173. Testing — Dependencies

Zu testen sind:

```text
Linear Dependencies
Parallel Dependencies
Cross-Queue Dependencies
Cross-Device Dependencies
Invalid Cycles
Failed Dependency
Cancelled Dependency
```

---

## 174. Testing — Fairness

Mehrere Prozesse MÜSSEN gleichzeitig hohe Compute-Last erzeugen.

Der Test MUSS prüfen, dass ein normaler Prozess andere normale Prozesse nicht dauerhaft verdrängen kann.

---

## 175. Testing — Starvation

Ein LOW-Priority-Job MUSS bei dauerhafter normaler Systemlast schließlich Scheduling-Fortschritt erzielen, sofern keine explizite Policy dies verhindert.

---

## 176. Testing — Preemption

Zu testen sind:

```text
Background -> Interactive

Normal -> High

Preempt -> Resume

Repeated Preemption

Device without Preemption
```

---

## 177. Testing — Deadlines

Zu testen sind:

- erreichbare Deadline,
- nicht erreichbare Deadline,
- Deadline Boost,
- Deadline Miss,
- mehrere konkurrierende Deadlines.

---

## 178. Testing — Power Policies

Die Device Selection MUSS unter verschiedenen Power Policies getestet werden.

Beispiel:

```text
PERFORMANCE
BALANCED
EFFICIENCY
BATTERY_SAVER
```

---

## 179. Testing — Thermal Policies

Zu testen sind:

```text
NORMAL
WARM
HOT
THROTTLED
CRITICAL
```

Der Scheduler MUSS auf simulierte Temperaturänderungen reagieren können.

---

## 180. Testing — Device Failure

Zu testen sind:

- Device Lost vor Dispatch,
- Device Lost während Dispatch,
- Device Reset,
- Retry-Safe Job,
- Non-Retry-Safe Job,
- Fallback auf alternatives Device.

---

## 181. Testing — Memory Pressure

Der Scheduler MUSS unter geringem Device Memory getestet werden.

Zu prüfen sind:

- Job Delay,
- alternative Device Selection,
- Out-of-Memory,
- Memory Eviction,
- keine Deadlocks.

---

## 182. Testing — Multi-Device

Mindestens:

```text
CPU + GPU

GPU0 + GPU1

GPU + NPU

CPU + GPU + NPU
```

---

## 183. Testing — Topology

Auf simulierten NUMA- und Multi-GPU-Systemen MUSS geprüft werden, ob lokale Devices bevorzugt werden können.

---

## 184. Stress Tests

Stress Tests SOLLEN unter anderem umfassen:

```text
100000+ Jobs

1000+ Queues

Multiple Processes

Multiple Devices

Continuous Submission

Continuous Cancellation

Device Reset under Load

Memory Pressure

Thermal Changes

Power Policy Changes
```

Die tatsächlichen Grenzwerte sind plattformabhängig.

---

## 185. Fuzzing

Folgende Scheduler-Eingaben SOLLTEN fuzz-getestet werden:

- Job Descriptor,
- Priority,
- QoS,
- Dependency Graph,
- Device Constraints,
- Deadlines,
- Resource Requirements,
- Scheduling Hints.

---

## 186. Fault Injection

Die Testumgebung SOLL Fehler künstlich injizieren können.

Beispiele:

```text
DEVICE_LOST
DEVICE_BUSY
OUT_OF_MEMORY
QUEUE_FAILURE
TIMEOUT
THERMAL_CRITICAL
BACKEND_FAILURE
```

---

## 187. Determinism Tests

Der deterministische Scheduling-Modus MUSS bei identischen Eingaben wiederholbare Scheduling-Entscheidungen erzeugen, soweit dies durch Hardware und spezifizierte Semantik garantiert werden kann.

---

## 188. Performance Tests

Mindestens folgende Scheduler-Kosten SOLLEN gemessen werden:

```text
AdmissionLatency
DeviceSelectionLatency
QueueSelectionLatency
DispatchLatency
CancellationLatency
```

---

## 189. Regression Tests

Änderungen an Scheduling-Heuristiken MÜSSEN gegen definierte Referenzworkloads getestet werden.

Dabei sollen mindestens überwacht werden:

```text
Latency
Throughput
Energy
Fairness
DeadlineMisses
ThermalBehavior
```

---

## 190. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
Single CPU Device

Multiple Compute Jobs

Priority Scheduling

Basic Fairness

Dependency Handling

In-Order Queues

Resource Validation

Job Cancellation

Error Handling
```

---

## 191. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
CPU
GPU
NPU
DSP

Multi-Device Scheduling

Automatic Device Selection

Cost-Based Scheduling

Data Locality

NUMA Awareness

Preemption

Deadlines

Power-Aware Scheduling

Thermal-Aware Scheduling

Adaptive Performance Model

Workload Splitting

Pipeline Scheduling

Device Migration

Device Recovery
```

---

## 192. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0002` MUSS:

1. Compute Jobs schedulen können.
2. Job-Abhängigkeiten respektieren.
3. inkompatible Devices ausschließen.
4. Context Device Restrictions respektieren.
5. Prioritäten berücksichtigen.
6. Fairness zwischen normalen Contexts gewährleisten.
7. Starvation verhindern oder begrenzen.
8. Device-Zustände berücksichtigen.
9. Ressourcenverfügbarkeit prüfen.
10. Jobs kontrolliert abbrechen können.
11. Device Failures behandeln.
12. Thread-safe Submission unterstützen.
13. explizites Ordering respektieren.
14. Sicherheitsrichtlinien respektieren.
15. Diagnoseinformationen bereitstellen.
16. mindestens CPU Compute schedulen können.
17. Ressourcenerschöpfung begrenzen.
18. Prozessende korrekt behandeln.
19. Scheduling-Fehler definiert melden.
20. zukünftige Device-Klassen unterstützen können.

---

## 193. Empfohlene interne Scheduler-Komponenten

Eine vollständige Implementierung SOLL logisch in folgende Komponenten zerlegt werden können:

```text
ComputeScheduler
 |
 +-- AdmissionController
 |
 +-- DependencyManager
 |
 +-- ReadyQueueManager
 |
 +-- PriorityManager
 |
 +-- DeviceSelector
 |
 +-- CostModel
 |
 +-- QueueSelector
 |
 +-- LoadBalancer
 |
 +-- FairnessManager
 |
 +-- DeadlineManager
 |
 +-- PowerPolicyManager
 |
 +-- ThermalPolicyManager
 |
 +-- MigrationManager
 |
 +-- RecoveryManager
 |
 +-- SchedulerDiagnostics
```

Diese Aufteilung ist logisch und schreibt keine konkrete Dateistruktur vor.

---

## 194. Beziehung zum Kernel Scheduler

Der Compute Scheduler und der Kernel Scheduler erfüllen unterschiedliche Aufgaben.

```text
                 NovaOS
                   |
        +----------+----------+
        |                     |
        v                     v
Kernel Scheduler       Compute Scheduler
        |                     |
        v                     v
CPU Threads          Compute Workloads
                              |
                    +---------+---------+
                    |         |         |
                    v         v         v
                   CPU       GPU       NPU
```

Für CPU Compute müssen beide Scheduler zusammenarbeiten.

Der Compute Scheduler bestimmt:

```text
Welche CPU-Compute-Aufgabe?
Welche Priorität?
Welche Parallelität?
Welche Kernklasse?
```

Der Kernel Scheduler bestimmt anschließend:

```text
Welcher konkrete Thread?
Welcher CPU-Kern?
Zu welchem Zeitpunkt?
```

---

## 195. Beziehung zur Compute Runtime

`NPSPEC-COMPUTE-RUNTIME-0001` definiert die übergeordnete Compute Runtime.

Diese Spezifikation konkretisiert deren Scheduling-Komponente.

```text
NPSPEC-COMPUTE-RUNTIME-0001
            |
            v
NPSPEC-COMPUTE-RUNTIME-0002
         Scheduler
```

---

## 196. Erweiterungsspezifikationen

Auf dieser Spezifikation können unter anderem folgende detaillierte Spezifikationen aufbauen:

```text
NPSPEC-COMPUTE-SCHED-PRIORITY-0001
Priority Scheduling

NPSPEC-COMPUTE-SCHED-FAIR-0001
Fair-Share Scheduling

NPSPEC-COMPUTE-SCHED-DEVICE-0001
Device Selection

NPSPEC-COMPUTE-SCHED-COST-0001
Compute Cost Model

NPSPEC-COMPUTE-SCHED-DEADLINE-0001
Deadline Scheduling

NPSPEC-COMPUTE-SCHED-POWER-0001
Power-Aware Scheduling

NPSPEC-COMPUTE-SCHED-THERMAL-0001
Thermal-Aware Scheduling

NPSPEC-COMPUTE-SCHED-MULTI-0001
Multi-Device Scheduling

NPSPEC-COMPUTE-SCHED-MIGRATE-0001
Compute Migration

NPSPEC-COMPUTE-SCHED-RECOVERY-0001
Scheduler Recovery

NPSPEC-COMPUTE-SCHED-CPU-0001
CPU Compute Scheduling

NPSPEC-COMPUTE-SCHED-GPU-0001
GPU Compute Scheduling

NPSPEC-COMPUTE-SCHED-NPU-0001
NPU Compute Scheduling

NPSPEC-COMPUTE-SCHED-TEST-0001
Scheduler Conformance Tests
```

---

## 197. Zusammenfassung

Der NovaOS Compute Scheduler bildet die zentrale Entscheidungsinstanz für die Ausführung heterogener Compute Workloads.

Der grundlegende Ablauf lautet:

```text
Compute Job
    |
    v
Admission Control
    |
    v
Dependency Resolution
    |
    v
Ready Queue
    |
    v
Capability Filter
    |
    v
Device Candidates
    |
    v
Cost Model
    |
    +-- Performance
    +-- Queue Load
    +-- Data Locality
    +-- Transfer Cost
    +-- Memory
    +-- Power
    +-- Thermal State
    +-- Priority
    +-- Deadline
    |
    v
Device Selection
    |
    v
Queue Selection
    |
    v
Dispatch
    |
    v
Execution
    |
    v
Measurement
    |
    v
Adaptive Scheduler Model
```

Der Scheduler soll nicht ausschließlich nach maximaler theoretischer Rechenleistung entscheiden.

Stattdessen wird die tatsächlich günstigste Ausführungsstrategie für den aktuellen Workload und Systemzustand gesucht.

Damit kann beispielsweise:

```text
kleiner Workload
    -> CPU SIMD

großer paralleler Workload
    -> GPU

AI Inference
    -> NPU

interaktiver Workload
    -> aktuell schnellstes verfügbares Device

Background Workload
    -> energieeffizientes Device

GPU überlastet
    -> CPU oder NPU

GPU thermisch gedrosselt
    -> alternatives Device

Daten bereits auf GPU1
    -> bevorzugt GPU1
```

gewählt werden.

Das grundlegende Architekturprinzip lautet:

> Der NovaOS Compute Scheduler plant nicht nur Rechenzeit. Er optimiert die gesamte Ausführung unter Berücksichtigung von Rechenleistung, Datenbewegung, Latenz, Auslastung, Energie, Temperatur, Fairness und den Anforderungen des jeweiligen Workloads.