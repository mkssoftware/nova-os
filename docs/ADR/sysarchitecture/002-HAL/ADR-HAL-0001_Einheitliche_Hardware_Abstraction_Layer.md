# ADR-HAL-0001 – Einheitliche Hardware Abstraction Layer

## Status

Angenommen

## Kategorie

HAL / Kernel / Hardwareabstraktion / Plattformportabilität / Treiberarchitektur

## Kontext

NovaOS soll auf unterschiedlichen Hardwareplattformen laufen können, ohne dass Kernel, Scheduler, Speicherverwaltung, IPC oder höhere Systemdienste dauerhaft an konkrete Prozessoren, Interrupt-Controller, Timer, Busse oder Geräte gebunden werden.

Ohne klar definierte Hardwareabstraktion entstehen typischerweise direkte Abhängigkeiten wie:

```text
Kernel Core
  ↓
x86 APIC
  ↓
PCI
  ↓
specific timer
  ↓
specific CPU instructions
```

Solche Abhängigkeiten erschweren:

- Portierung auf andere CPU-Architekturen,
- Unterstützung neuer Plattformgenerationen,
- Testbarkeit,
- Virtualisierung,
- Hardwareemulation,
- Austausch von Geräten,
- Treiberisolation,
- langfristige Evolution.

NovaOS benötigt deshalb eine einheitliche HAL, die hardwareabhängige Mechanismen hinter stabilen, versionierten Systemgrenzen kapselt.

---

## Entscheidung

NovaOS verwendet eine **einheitliche Hardware Abstraction Layer**, kurz HAL.

Die HAL bildet die definierte Grenze zwischen:

```text
Hardware-dependent mechanisms
```

und:

```text
Hardware-independent kernel architecture
```

Das Grundmodell lautet:

```text
Kernel / System Services
        │
        ▼
HAL Contracts
        │
        ▼
Architecture Backend
        │
        ▼
Platform Backend
        │
        ▼
Device / Firmware / Hardware
```

Die HAL darf intern mehrere Ebenen besitzen, muss nach außen jedoch konsistente Contracts bereitstellen.

---

## Ziele

Die HAL soll insbesondere:

- CPU-Architekturdetails kapseln,
- Interrupt-Infrastruktur abstrahieren,
- Timer und Clocks vereinheitlichen,
- SMP- und CPU-Topologie beschreiben,
- Memory-Management-Unterstützung bereitstellen,
- IOMMU und DMA integrieren,
- Bus- und Geräteerkennung abstrahieren,
- Firmware-Schnittstellen kapseln,
- Power- und Energiezustände beschreiben,
- Hardware-Capabilities bereitstellen,
- Virtualisierung und emulierte Plattformen unterstützen.

---

## Nicht-Ziel

Die HAL soll keine universelle High-Level-Geräte-API für Anwendungen darstellen.

Nicht:

```text
Application
  ↓
HAL
```

sondern:

```text
Application
  ↓
Nova Capabilities / System APIs
  ↓
Driver / Service
  ↓
HAL
```

Die HAL bleibt eine Low-Level-Systemgrenze.

---

## Schichtenmodell

Die HAL darf logisch in mehrere Bereiche aufgeteilt werden.

### Architecture HAL

Architekturspezifische Mechanismen, beispielsweise:

```text
x86
x86_64
ARM64
RISC-V
```

Dazu gehören:

- CPU-Modi,
- Registerzustände,
- Exceptions,
- atomare Operationen,
- Speicherbarrieren,
- Seitentabellen,
- Context Switch Support,
- Architektur-Timer.

### Platform HAL

Plattformabhängige Eigenschaften, beispielsweise:

```text
PC
Server
ARM SoC
Virtual Machine
Embedded Board
```

Dazu gehören:

- Firmware,
- Interrupt Routing,
- Board Topology,
- Timerquellen,
- Power Management,
- Gerätebeschreibung.

### Device-facing HAL

Gemeinsame Mechanismen für Treiber, beispielsweise:

- MMIO,
- Port I/O,
- DMA,
- IOMMU,
- Interrupt Binding,
- Device Resources,
- Bus Enumeration.

---

## HAL Contracts

HAL-Schnittstellen müssen explizite Contracts besitzen.

Beispiel:

```text
HalInterruptController
HalTimer
HalClock
HalCpu
HalMemory
HalDma
HalIommu
HalFirmware
HalPower
HalBus
```

Die konkrete Implementierung darf je Plattform unterschiedlich sein.

---

## CPU-Abstraktion

Die HAL stellt grundlegende CPU-Informationen und CPU-nahe Mechanismen bereit.

Beispielsweise:

```text
CpuId
Architecture
Vendor
Features
Topology
CoreId
PackageId
NUMANode
Frequency
ExecutionState
```

Höhere Kernelkomponenten sollen nicht direkt auf herstellerspezifische CPU-Erkennung zugreifen müssen.

---

## CPU Features

Hardwarefeatures sollen über strukturierte Capability-Informationen beschrieben werden.

Beispiele:

```text
SIMD
AES Acceleration
Virtualization
Atomic Extensions
NX
SMEP
SMAP
AVX
AVX2
AVX512
SVE
RISC-V Extensions
```

Das Vorhandensein eines Features bedeutet nicht automatisch, dass es verwendet werden muss.

Die Auswahl bleibt Policy- und Execution-abhängig.

---

## Interrupt-Abstraktion

Die HAL abstrahiert konkrete Interrupt-Controller.

Beispiele:

```text
8259 PIC
Local APIC
IOAPIC
MSI
MSI-X
GIC
PLIC
```

Der Kernel soll mit einem gemeinsamen Interruptmodell arbeiten.

Konzeptionell:

```text
InterruptSource
    ↓
HAL Routing
    ↓
Interrupt Vector
    ↓
Kernel Handler
```

---

## Timer und Clocks

NovaOS unterscheidet:

```text
Timer
```

und:

```text
Clock Source
```

Die HAL soll unterschiedliche Hardwarequellen abstrahieren.

Beispiele:

```text
PIT
HPET
TSC
APIC Timer
ARM Generic Timer
Platform Timers
```

Höhere Schichten verwenden semantische Timer- und Clock-Interfaces statt konkrete Hardware.

---

## SMP und CPU-Topologie

Die HAL beschreibt Systemtopologie.

Beispiel:

```text
System
├── Package 0
│   ├── Core 0
│   │   ├── Thread 0
│   │   └── Thread 1
│   └── Core 1
└── Package 1
```

Zusätzlich können beschrieben werden:

- NUMA-Zugehörigkeit,
- Cache-Sharing,
- Performance-Klassen,
- Efficiency-Klassen,
- CPU-Hotplug-Fähigkeit.

Diese Informationen stehen Scheduler und Resource Economy zur Verfügung.

---

## Memory Management Support

Die HAL stellt architekturspezifische Mechanismen für virtuelle Speicherverwaltung bereit.

Dazu gehören:

- Page Table Operations,
- TLB Operations,
- Page Attributes,
- Execute Protection,
- Cache Attributes,
- Address-Space Activation,
- Architecture-specific Mapping Constraints.

Die eigentliche Speicherpolitik verbleibt außerhalb der HAL.

---

## DMA

DMA muss über definierte HAL-Mechanismen verwaltet werden.

Treiber sollen nicht eigenständig unkontrollierte physische Speicheradressen an Geräte vergeben.

Die HAL unterstützt insbesondere:

```text
DMA Mapping
DMA Unmapping
Scatter/Gather
Coherency
Cache Synchronization
Address Constraints
```

---

## IOMMU

Sofern vorhanden, wird eine IOMMU in die HAL integriert.

Die HAL stellt Mechanismen bereit für:

- Device Address Spaces,
- DMA Isolation,
- Mapping,
- Unmapping,
- Fault Reporting,
- Domain Binding.

IOMMU-Policy und Sicherheitsentscheidung werden durch höhere Systemkomponenten bestimmt.

---

## MMIO und Port I/O

Hardwarezugriffe müssen über definierte Primitive erfolgen.

Beispiele:

```text
ReadMmio8
ReadMmio16
ReadMmio32
ReadMmio64

WriteMmio8
WriteMmio16
WriteMmio32
WriteMmio64
```

Für Architekturen mit Port I/O können zusätzliche Primitive existieren.

Direkte unstrukturierte Zugriffe sollen außerhalb klar abgegrenzter Low-Level-Komponenten vermieden werden.

---

## Memory Ordering

Die HAL stellt architekturspezifische Memory Barriers bereit.

Beispielsweise:

```text
LoadBarrier
StoreBarrier
FullBarrier
IoBarrier
```

Höhere Komponenten sollen keine impliziten Annahmen über das Memory Model einer konkreten CPU machen.

---

## Atomare Operationen

Grundlegende atomare Primitive können durch die HAL oder eine unmittelbar darauf aufbauende Kernel-Abstraktion bereitgestellt werden.

Beispiele:

```text
CompareExchange
Exchange
FetchAdd
FetchAnd
FetchOr
```

Semantik und Memory Ordering müssen eindeutig spezifiziert sein.

---

## Firmware

Firmware-Schnittstellen werden hinter einer gemeinsamen Plattformgrenze gekapselt.

Mögliche Backends:

```text
UEFI
ACPI
Device Tree
SMBIOS
Legacy BIOS derived information
```

Höhere Komponenten sollen nicht unnötig direkt von einzelnen Firmwareformaten abhängen.

---

## ACPI

Auf PC-Plattformen darf ACPI als wichtige Informationsquelle dienen.

Die HAL bzw. Plattformschicht kann daraus unter anderem ableiten:

- CPU-Topologie,
- Interrupt Routing,
- Power States,
- NUMA,
- Geräteinformationen.

ACPI ist jedoch kein universelles NovaOS-Systemmodell.

---

## Device Tree

Auf geeigneten Plattformen kann Device Tree als Plattformbeschreibung verwendet werden.

Device Tree und ACPI sollen in eine gemeinsame interne Hardwarebeschreibung überführt werden können.

---

## Hardware Discovery

Die HAL stellt einen strukturierten Hardwaregraphen bereit.

Beispiel:

```text
HardwareRoot
├── CPU
├── Memory
├── PCIe
│   ├── GPU
│   └── NVMe
├── USB
└── FirmwareDevices
```

Hardwareidentität, Ressourcen und Topologie müssen getrennt beschreibbar sein.

---

## Bus-Abstraktion

Busse werden über definierte Modelle eingebunden.

Beispiele:

```text
PCI
PCI Express
USB
I2C
SPI
Platform Bus
Virtual Bus
```

Bus-spezifische Enumerationslogik bleibt in entsprechenden Backends.

---

## Device Resources

Geräte können Ressourcen besitzen wie:

```text
MMIO Regions
I/O Ports
Interrupts
DMA Capabilities
Clock Inputs
Power Domains
Reset Lines
```

Diese Ressourcen müssen explizit beschrieben und kontrolliert zugewiesen werden.

---

## Treiberintegration

Die HAL arbeitet eng mit dem NovaOS-Treibermodell zusammen.

Treiber sollen über definierte HAL- und Device-Contracts auf Hardware zugreifen.

Dadurch können Treiber je nach Sicherheits- und Performanceanforderung ausgeführt werden als:

```text
Kernel Driver
User Mode Driver
Isolated Driver
Virtualized Driver
```

---

## Keine HAL-Policy

Die HAL implementiert primär Mechanismen.

Beispiel:

```text
HAL:
    CPU3 can be powered down
```

Policy entscheidet:

```text
Power Manager:
    CPU3 should be powered down
```

Damit bleibt die Trennung aus `ADR-ARCH-0002_Mechanism_Policy_Separation` erhalten.

---

## Resource Economy

Die HAL liefert Hardwareinformationen an die systemweite Ressourcenökonomie.

Beispiele:

- CPU Capacity,
- Memory Domains,
- GPU Resources,
- NUMA Topology,
- Power States,
- Thermal Sensors,
- Accelerator Availability.

Die HAL selbst wird dadurch nicht zum globalen Resource Scheduler.

---

## Lokalität

Die HAL liefert die physische Grundlage für `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`.

Dazu gehören insbesondere:

```text
NUMA Topology
Cache Topology
Device Topology
Memory Domains
Bus Topology
```

Diese Daten können von Scheduler und Execution Planner verwendet werden.

---

## Hotplug

Die HAL muss dynamische Hardwareänderungen grundsätzlich unterstützen können.

Beispiele:

```text
CPU added
CPU removed
Device added
Device removed
```

Nicht jede Plattform muss jede Hotplug-Klasse unterstützen.

Änderungen müssen als strukturierte Events an höhere Komponenten weitergegeben werden.

---

## Hardware Events

Hardwarezustandsänderungen sollen nicht über unstrukturierte globale Flags kommuniziert werden.

Beispiel:

```text
HardwareEvent {
    type
    device
    oldState
    newState
}
```

---

## Fehlerbehandlung

HAL-Fehler müssen explizit sein.

Beispiele:

```text
Unsupported
NotPresent
Unavailable
Busy
Timeout
HardwareFault
InvalidResource
PermissionDenied
```

Fehler dürfen nicht stillschweigend als erfolgreiche Hardwareoperation interpretiert werden.

---

## Fail-Safe Defaults

Bei unbekannten oder inkonsistenten Hardwareeigenschaften gelten Fail-Safe Defaults.

Beispiele:

```text
Unknown DMA capability
    ↓
Do not enable unrestricted DMA
```

```text
Unknown CPU feature
    ↓
Do not execute corresponding instruction
```

---

## Fallbacks

Die HAL kann mehrere Hardwarebackends oder Fallbackpfade unterstützen.

Beispiel:

```text
Preferred Timer:
    TSC Deadline

Fallback:
    APIC Timer

Fallback:
    HPET
```

Die Auswahl erfolgt gemäß Contracts und Policy.

---

## Deterministic Mode

Hardwareabhängige Entscheidungen müssen soweit notwendig fixierbar sein.

Beispielsweise:

```text
Clock Source
Timer Source
CPU Set
Device Provider
```

Adaptive Hardwareauswahl darf Deterministic Contracts nicht verletzen.

---

## Virtualisierung

Virtuelle Hardware wird über dieselben grundlegenden HAL-Konzepte eingebunden.

Mögliche Plattformen:

```text
QEMU
KVM
Hyper-V
VMware
VirtualBox
Custom Nova Virtual Platform
```

Virtualisierung ist jedoch kein notwendiges Fundament der HAL.

---

## Simulation und Tests

HAL-Contracts müssen durch simulierte oder Mock-Backends testbar sein.

Beispiel:

```text
Kernel
  ↓
Test HAL
  ↓
Synthetic Hardware Model
```

Dadurch können Kernelkomponenten unabhängig von realer Hardware getestet werden.

---

## Architekturportierung

Eine neue CPU-Architektur soll primär durch Implementierung eines neuen Architecture-HAL-Backends integrierbar sein.

Beispiel:

```text
HAL
├── x86_64
├── arm64
└── riscv64
```

Plattformdetails bleiben davon getrennt.

---

## 32-Bit und 64-Bit

Die HAL darf verschiedene Address Widths unterstützen.

Architekturspezifische Größen dürfen nicht unnötig in höhere Systemschichten durchsickern.

Kernel-Interfaces müssen dafür geeignete explizite Typen verwenden.

---

## Capability-Modell

Hardwarefeatures können als systeminterne Hardware Capabilities beschrieben werden.

Beispiel:

```text
hardware.cpu.simd.avx2
hardware.iommu.translation
hardware.gpu.compute
```

Diese Hardware Capabilities sind nicht identisch mit Anwendungsberechtigungen.

---

## Architecture Introspection

Die erkannte Hardwarearchitektur muss introspektierbar sein.

Beispiel:

```text
CPU0
Architecture: x86_64
NUMA Node: 0
Features:
    AVX2
    AES
    VMX
```

Ebenso sollen relevante:

- Busse,
- Geräte,
- Interruptcontroller,
- Timer,
- Memory Domains,
- IOMMU-Domains

sichtbar gemacht werden können.

Sicherheitsrelevante Details dürfen gefiltert werden.

---

## Evolvierbarkeit

Diese ADR folgt `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`.

HAL-Contracts müssen versionierbar sein.

Neue Hardwaremechanismen sollen ergänzt werden können, ohne bestehende Architekturen unnötig zu brechen.

Neue Backends dürfen parallel zu bestehenden Backends existieren.

---

## KI-Unabhängigkeit

Gemäß `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels` darf die HAL keine KI benötigen, um grundlegende Hardware korrekt anzusteuern.

KI darf beispielsweise:

- Power States prognostizieren,
- Device Placement optimieren,
- Performance States empfehlen.

Die grundlegenden HAL-Mechanismen müssen deterministisch funktionieren.

---

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche Hardware Abstraction Layer besitzen.
2. Hardwareabhängige Kernelmechanismen SOLLEN hinter HAL-Contracts gekapselt werden.
3. Die HAL MUSS zwischen hardwareabhängigen und hardwareunabhängigen Systemteilen eine definierte Grenze bilden.
4. CPU-Architektur und Plattformmodell SOLLEN voneinander unterscheidbar sein.
5. Architecture-HAL-Backends MÜSSEN unabhängig implementierbar sein.
6. Platform-HAL-Backends SOLLEN unabhängig implementierbar sein.
7. Die HAL MUSS CPU-Topologie beschreiben können.
8. Die HAL SOLL NUMA-Topologie beschreiben können.
9. Die HAL MUSS Interruptmechanismen abstrahieren.
10. Die HAL MUSS Timer- und Clock-Mechanismen abstrahieren.
11. Die HAL MUSS architekturspezifische Speicherverwaltungsmechanismen abstrahieren.
12. Die HAL MUSS Memory Ordering korrekt abbilden.
13. Die HAL MUSS DMA-Mechanismen unterstützen.
14. Die HAL SOLL IOMMU-Mechanismen unterstützen, sofern Hardware vorhanden ist.
15. DMA-Zugriff DARF nicht unkontrolliert erfolgen.
16. MMIO MUSS über definierte Low-Level-Primitive zugreifbar sein.
17. Port I/O DARF auf entsprechenden Architekturen unterstützt werden.
18. Firmwaremechanismen SOLLEN hinter Plattformabstraktionen gekapselt werden.
19. ACPI DARF als Plattformbackend verwendet werden.
20. Device Tree DARF als Plattformbackend verwendet werden.
21. Hardware Discovery MUSS strukturierte Geräteinformationen erzeugen.
22. Busse SOLLEN über definierte Busmodelle eingebunden werden.
23. Device Resources MÜSSEN explizit beschrieben werden.
24. Treiber SOLLEN Hardware über definierte HAL- und Device-Contracts verwenden.
25. Die HAL DARF keine unnötige globale Systempolicy implementieren.
26. Resource Economy MUSS Hardwareinformationen über definierte Interfaces beziehen können.
27. Scheduler MUSS relevante Topologieinformationen beziehen können.
28. Lokalitätsplanung MUSS relevante Hardwaretopologie beziehen können.
29. Hardware-Hotplug SOLL unterstützt werden, wenn die Plattform dies ermöglicht.
30. Hardwareänderungen SOLLEN als strukturierte Events sichtbar werden.
31. HAL-Fehler MÜSSEN explizit propagiert werden.
32. Unbekannte Hardwareeigenschaften DÜRFEN NICHT als unterstützte Features behandelt werden.
33. Die HAL MUSS Fallback-Backends ermöglichen können.
34. Deterministic Mode MUSS relevante Hardwareentscheidungen kontrollieren können.
35. Virtuelle Plattformen MÜSSEN über dieselben grundlegenden HAL-Prinzipien integrierbar sein.
36. HAL-Contracts SOLLEN Mock- und Testbackends ermöglichen.
37. Neue CPU-Architekturen SOLLEN primär durch neue HAL-Backends integrierbar sein.
38. Architekturspezifische Integer- und Pointergrößen SOLLEN nicht unnötig in höhere APIs durchsickern.
39. Hardware Capabilities SOLLEN maschinenlesbar beschrieben werden können.
40. Hardwarezustand SOLL über Architecture Introspection sichtbar sein.
41. HAL-Contracts MÜSSEN versionierbar sein.
42. Neue HAL-Versionen SOLLEN kontrollierte Kompatibilitätsregeln besitzen.
43. Die HAL MUSS ohne KI vollständig korrekt funktionieren.
44. KI DARF HAL-Policy oder Optimierung unterstützen, aber keine fundamentale HAL-Korrektheit ersetzen.
45. NovaOS MUSS Hardwareunterschiede kapseln, ohne relevante Hardwareeigenschaften für Planung und Optimierung unsichtbar zu machen.

---

## Konsequenzen

### Positive Konsequenzen

- bessere Portierbarkeit,
- klarere Kernelarchitektur,
- geringere Abhängigkeit von x86-spezifischen Strukturen,
- einfachere Unterstützung neuer Plattformen,
- bessere Testbarkeit,
- bessere Treiberisolation,
- einheitliche Hardwaretopologie,
- bessere Grundlage für NUMA- und Lokalitätsoptimierung,
- bessere Virtualisierungsunterstützung,
- langfristig evolvierbare Hardwareunterstützung.

### Negative Konsequenzen

- zusätzliche Abstraktionsschicht,
- HAL-Contracts müssen sorgfältig entworfen werden,
- nicht jede Hardware lässt sich vollständig vereinheitlichen,
- zu generische HAL-Interfaces können Performance oder Hardwarefeatures verstecken,
- mehrere Plattformbackends erhöhen Testaufwand.

---

## Verworfene Alternativen

### Direkter Hardwarezugriff aus dem gesamten Kernel

Verworfen.

Dies erzeugt starke Plattformkopplung und erschwert Portierung und Tests.

### Eine vollständig generische HAL ohne architekturspezifische Erweiterungen

Verworfen.

Unterschiedliche Architekturen besitzen relevante Eigenschaften, die nicht sinnvoll auf den kleinsten gemeinsamen Nenner reduziert werden dürfen.

### HAL als High-Level-Geräteframework

Verworfen.

High-Level-Gerätefunktionen gehören in Treiber, Services und Capabilities.

### HAL als Policy Engine

Verworfen.

Die HAL stellt Mechanismen und Hardwareinformationen bereit; Policy verbleibt in dafür vorgesehenen Systemkomponenten.

### Hypervisor als verpflichtende HAL

Verworfen.

NovaOS soll direkt auf Hardware laufen können.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-CORE-0001`
- `NPSPEC-HAL-ARCHITECTURE-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-CPU-0001`
- `NPSPEC-HAL-CPUFEATURE-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-HAL-TIMER-0001`
- `NPSPEC-HAL-CLOCK-0001`
- `NPSPEC-HAL-MEMORY-0001`
- `NPSPEC-HAL-ATOMIC-0001`
- `NPSPEC-HAL-BARRIER-0001`
- `NPSPEC-HAL-MMIO-0001`
- `NPSPEC-HAL-PORTIO-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `NPSPEC-HAL-ACPI-0001`
- `NPSPEC-HAL-DEVICETREE-0001`
- `NPSPEC-HAL-BUS-0001`
- `NPSPEC-HAL-DEVICE-RESOURCE-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-HAL-POWER-0001`
- `NPSPEC-HAL-VIRTUAL-0001`
- `NPSPEC-HAL-INTROSPECTION-0001`
- `NPSPEC-HAL-TESTBACKEND-0001`

---

## Verifikation

Die HAL muss insbesondere getestet werden für:

- Boot auf realer Hardware,
- Boot unter QEMU,
- verschiedene Interruptcontroller,
- verschiedene Timerquellen,
- SMP-Systeme,
- NUMA-Systeme,
- CPU Feature Detection,
- MMIO,
- DMA,
- IOMMU,
- PCI/PCIe Enumeration,
- Firmwaredaten,
- Geräte-Hotplug,
- Hardwarefehler,
- Fallback auf alternative Timer oder Controller,
- Test-HAL ohne reale Hardware,
- mindestens zwei unterschiedliche Architecture Backends, sobald diese implementiert sind.

---

## Ergebnis

NovaOS erhält eine klare Low-Level-Grenze zwischen Kernelarchitektur und konkreter Hardware.

Das Architekturmodell lautet:

```text
NovaOS Kernel
      │
      ▼
Stable HAL Contracts
      │
      ├── CPU Architecture Backend
      ├── Platform Backend
      ├── Firmware Backend
      └── Device Mechanisms
                │
                ▼
             Hardware
```

Die HAL versteckt dabei keine für Planung relevanten Eigenschaften.

Stattdessen gilt:

```text
Abstract hardware mechanisms.

Expose hardware capabilities.

Keep policy above the HAL.
```

Damit lautet die zentrale Architekturregel:

```text
Hardware may change.

Kernel semantics should not have to.
```