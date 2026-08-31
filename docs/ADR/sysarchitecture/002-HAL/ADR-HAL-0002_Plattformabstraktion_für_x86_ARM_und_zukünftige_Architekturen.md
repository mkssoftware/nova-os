# ADR-HAL-0002 – Plattformabstraktion für x86, ARM und zukünftige Architekturen

## Status

Angenommen

## Kategorie

HAL / Plattformarchitektur / CPU-Portabilität / Architekturabstraktion / Zukunftssicherheit

## Kontext

NovaOS soll nicht dauerhaft an eine einzelne Prozessorarchitektur gebunden sein.

Die erste Implementierung kann auf x86 beziehungsweise x86_64 entstehen, die Architektur muss jedoch von Anfang an so gestaltet werden, dass weitere Plattformen integriert werden können.

Mindestens zu berücksichtigen sind:

```text
x86 / x86_64
ARM / ARM64
RISC-V
zukünftige Architekturen
```

Unterschiedliche CPU-Architekturen unterscheiden sich erheblich bei:

- Exception- und Interruptmodellen,
- Privilege Levels,
- Memory Management,
- Page Tables,
- TLB-Verhalten,
- Atomics,
- Memory Ordering,
- CPU-Startmechanismen,
- Timerquellen,
- Firmware,
- I/O-Modellen,
- Virtualisierung,
- Debug-Mechanismen,
- Cache-Architektur,
- Energiezuständen.

Diese Unterschiede dürfen nicht unkontrolliert in den hardwareunabhängigen Kernelkern durchsickern.

Gleichzeitig darf NovaOS nicht versuchen, alle Architekturen auf einen künstlichen kleinsten gemeinsamen Nenner zu reduzieren.

---

## Entscheidung

NovaOS verwendet eine explizite **Plattform- und Architekturabstraktion** innerhalb der HAL.

Das Modell lautet:

```text
Hardware-independent Kernel
            │
            ▼
Common HAL Contracts
            │
            ▼
Architecture Layer
            │
            ▼
Platform Layer
            │
            ▼
Hardware / Firmware
```

Dabei werden zwei unterschiedliche Aspekte getrennt:

```text
Architecture
```

beschreibt die CPU- und ISA-Eigenschaften.

```text
Platform
```

beschreibt die konkrete Systemumgebung, in der diese Architektur verwendet wird.

Beispiel:

```text
Architecture:
    ARM64

Platform:
    Generic ARM Server
```

oder:

```text
Architecture:
    x86_64

Platform:
    PC / UEFI / ACPI
```

---

## Architektur-Backends

Für jede unterstützte Prozessorarchitektur existiert ein Architecture Backend.

Beispiele:

```text
hal/arch/x86
hal/arch/x86_64
hal/arch/arm64
hal/arch/riscv64
```

Ein Architecture Backend kapselt insbesondere:

- CPU Initialisierung,
- Exception Handling,
- Registermodell,
- Context Switching,
- Privilege Levels,
- Page Table Mechanismen,
- TLB Operationen,
- Memory Barriers,
- atomare Primitive,
- CPU Feature Detection,
- Architektur-Timer,
- SMP Startup.

---

## Plattform-Backends

Die konkrete Plattform wird separat modelliert.

Beispiele:

```text
hal/platform/pc
hal/platform/generic-arm
hal/platform/virt
hal/platform/server
```

Plattformabhängige Bereiche umfassen beispielsweise:

- Firmware,
- Gerätebeschreibung,
- Interrupt Routing,
- Power Management,
- Board-spezifische Geräte,
- Bootinformationen,
- Systemtopologie.

---

## Architektur und Plattform dürfen kombiniert werden

Eine Architektur kann auf mehreren Plattformen verwendet werden.

Beispiel:

```text
ARM64
├── Server Platform
├── Embedded Platform
├── Virtual Platform
└── Mobile-class Platform
```

Ebenso darf ein Plattformmodell unterschiedliche CPU-Generationen unterstützen.

Die Architekturidentität darf deshalb nicht automatisch die gesamte Plattformidentität bestimmen.

---

## Gemeinsame HAL-Schnittstellen

Hardwareunabhängige Kernelkomponenten verwenden gemeinsame Interfaces.

Beispiel:

```text
HalCpu
HalException
HalInterrupt
HalMemory
HalTimer
HalAtomic
HalTopology
```

Die Implementierung wird beim Build, Boot oder über definierte Binding-Mechanismen an das passende Backend gebunden.

---

## Keine ISA-Abhängigkeiten im Kernelkern

Der hardwareunabhängige Kernel soll keine direkten Annahmen enthalten wie:

```text
CR3
CR0
APIC
GDT
IDT
MSR
```

oder:

```text
EL1
TTBR
GIC
```

Solche Elemente gehören in die jeweilige Architecture- oder Platform-Schicht.

Der Kernel darf jedoch abstrahierte semantische Konzepte verwenden:

```text
Address Space
Interrupt Controller
Privilege Level
CPU Local State
Page Mapping
```

---

## Exception-Modell

Unterschiedliche Architekturen besitzen unterschiedliche Exceptionmodelle.

Die Architecture Layer übersetzt diese auf ein gemeinsames Kernelmodell.

Konzeptionell:

```text
Architecture Exception
        ↓
HAL Translation
        ↓
Kernel Exception Model
```

Dabei müssen architekturspezifische Details weiterhin verfügbar bleiben, wenn sie für Diagnose oder Recovery benötigt werden.

---

## Privilege Model

NovaOS darf nicht davon ausgehen, dass jede Architektur exakt dieselben Rings besitzt.

Beispielsweise:

```text
x86:
Ring 0
Ring 3
```

gegen:

```text
ARM64:
EL0
EL1
EL2
EL3
```

Der Kernel arbeitet stattdessen mit semantischen Privilege Domains.

Beispiel:

```text
User
Kernel
Hypervisor
Secure/Firmware
```

Nicht jede Plattform muss jede Domain besitzen.

---

## Memory Management

Die gemeinsame VM-Architektur definiert semantische Operationen.

Beispielsweise:

```text
MapPage
UnmapPage
ProtectPage
SwitchAddressSpace
InvalidateTranslation
```

Das Architecture Backend implementiert daraus:

```text
x86_64 Page Tables
ARM64 Translation Tables
RISC-V Sv39/Sv48
```

---

## Page Size

NovaOS darf nicht global von einer einzigen Page Size ausgehen.

Ein Architecture Backend muss unterstützte Seitengrößen melden können.

Beispiel:

```text
4 KiB
16 KiB
64 KiB
2 MiB
1 GiB
```

Höhere Schichten dürfen Preferences ausdrücken, aber keine nicht unterstützten Größen erzwingen.

---

## Address Width

Virtuelle und physische Adressbreiten müssen explizit modelliert werden.

Nicht jede 64-Bit-Architektur besitzt tatsächlich einen 64-Bit-Adressraum.

Beispiel:

```text
Pointer Width:
    64 bit

Virtual Address Width:
    48 bit

Physical Address Width:
    52 bit
```

Diese Werte werden über die HAL bereitgestellt.

---

## Endianness

NovaOS darf Endianness nicht unnötig implizit voraussetzen.

Architekturen können beispielsweise sein:

```text
Little Endian
Big Endian
Bi-Endian
```

Native Datenstrukturen dürfen architekturabhängig sein.

Persistente und interoperable Formate müssen dagegen explizite Byte Order besitzen.

---

## Memory Ordering

Unterschiedliche Architekturen besitzen unterschiedliche Memory Models.

Beispielsweise ist x86 typischerweise stärker geordnet als ARM.

NovaOS darf deshalb keine Korrektheit aus implizitem x86-Verhalten ableiten.

Die HAL muss explizite Memory Ordering Primitive bereitstellen.

---

## Atomics

Atomare Operationen werden über gemeinsame Semantik beschrieben.

Beispiel:

```text
AtomicLoad
AtomicStore
CompareExchange
FetchAdd
```

mit definiertem Memory Ordering:

```text
Relaxed
Acquire
Release
AcquireRelease
SequentiallyConsistent
```

Die konkrete ISA-Implementierung bleibt architekturabhängig.

---

## Context Switching

Der Scheduler verwendet eine abstrakte Context-Struktur.

Architekturabhängige Registerzustände werden durch das Backend verwaltet.

Beispiel:

```text
ThreadContext
    │
    └── ArchitectureContext
```

Der Scheduler muss nicht wissen, welche konkreten Register gespeichert werden.

---

## CPU Local Storage

Architekturspezifische Mechanismen für CPU-lokalen Zustand werden abstrahiert.

Mögliche Implementierungen:

```text
x86 GS Base
ARM TPIDR
architecture-specific register
```

Nach außen existiert ein gemeinsames CPU-local-Konzept.

---

## SMP Startup

CPU-Startmechanismen unterscheiden sich stark zwischen Plattformen.

Beispiele:

```text
x86 INIT/SIPI
ARM PSCI
RISC-V SBI
```

Die HAL kapselt diese Unterschiede hinter einem gemeinsamen Modell:

```text
StartCpu(cpuId)
StopCpu(cpuId)
```

sofern die Plattform die Operation unterstützt.

---

## Interrupt Controller

Interruptarchitektur wird nicht an x86 APIC gekoppelt.

Mögliche Backends:

```text
PIC
APIC
IOAPIC
GIC
PLIC
Virtual Interrupt Controller
```

Der Kernel verwendet gemeinsame Interrupt-Contracts.

---

## Timer

Die Architecture- und Platform-Schicht melden verfügbare Timerquellen.

Beispiele:

```text
TSC
APIC Timer
HPET
ARM Generic Timer
RISC-V Timer
Virtual Timer
```

Die höhere Timerarchitektur entscheidet anhand von Anforderungen und Policy.

---

## Firmware-Abstraktion

Firmwaremodelle unterscheiden sich zwischen Plattformen.

Beispiele:

```text
UEFI
ACPI
Device Tree
PSCI
SBI
```

Diese Mechanismen werden nicht direkt zu universellen Kernelannahmen.

Stattdessen werden sie in ein gemeinsames internes Plattformmodell übersetzt.

---

## Bootübergabe

Der Bootloader übergibt eine architektur- und plattformneutral beschreibbare Boot Information Structure.

Beispiel:

```text
BootInfo {
    memoryMap
    framebuffer
    firmware
    cpuArchitecture
    platform
    commandLine
    initModules
}
```

Architekturspezifische Zusatzdaten dürfen separat angehängt werden.

---

## Hardware Capability Discovery

Jedes Backend meldet Hardwarefähigkeiten über ein gemeinsames Modell.

Beispiel:

```text
ArchitectureCapabilities {
    atomics
    virtualization
    simd
    pageSizes
    addressWidths
    protectionFeatures
}
```

Höhere Schichten dürfen Capability Detection verwenden, ohne konkrete ISA-Checks selbst auszuführen.

---

## Optional Features

Kein optionales CPU-Feature darf zur unbeabsichtigten Mindestanforderung werden.

Beispiel:

```text
AVX2 available
```

darf Optimierung ermöglichen.

Es darf aber nicht automatisch bedeuten:

```text
NovaOS requires AVX2
```

sofern dies nicht ausdrücklich als Plattformminimum definiert wurde.

---

## Baseline und optimierte Pfade

Architecture Backends dürfen mehrere Implementierungspfade besitzen.

Beispiel:

```text
Memory Copy
├── Generic
├── SSE2
├── AVX2
└── AVX512
```

oder:

```text
ARM64
├── Generic
└── SVE optimized
```

NovaOS kann geeignete Implementierungen auswählen.

---

## ABI-Abstraktion

Die native NovaABI muss architekturbezogene Calling Conventions berücksichtigen.

Die semantische System-API bleibt jedoch möglichst architekturunabhängig.

Beispiel:

```text
Nova API
    ↓
Architecture-specific NovaABI Binding
    ↓
Syscall Mechanism
```

Syscall-Mechanismen können beispielsweise sein:

```text
SYSCALL
SYSENTER
SVC
ECALL
```

---

## Treiber

Treiber dürfen architekturspezifische Erweiterungen nutzen, wenn ihre Contracts dies explizit beschreiben.

Generische Treiber sollen jedoch möglichst gegen gemeinsame HAL- und Bus-Interfaces geschrieben werden.

Beispiel:

```text
NVMe Driver
    ↓
PCIe API
    ↓
HAL
```

statt:

```text
NVMe Driver
    ↓
direct x86 assumptions
```

---

## Architekturabhängige Optimierungen

NovaOS erlaubt architekturspezifische Optimierung.

Die Plattformabstraktion soll Unterschiede kapseln, nicht verhindern.

Daher gilt:

```text
Portable semantics
+
Architecture-specific optimization
```

und nicht:

```text
Lowest common denominator only
```

---

## Resource Economy

Architecture Backends stellen relevante Ressourceninformationen bereit.

Beispiele:

```text
Performance Core
Efficiency Core
Cache Hierarchy
NUMA Topology
SIMD Width
Power States
```

Diese Informationen können in die Resource Economy und Execution Planning einfließen.

---

## Lokalitätsmodell

Die Plattformabstraktion liefert Topologieinformationen für `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`.

Dazu gehören insbesondere:

```text
CPU → Cache
CPU → NUMA Node
Device → NUMA Node
Accelerator → Bus
Memory → Domain
```

---

## Deterministic Mode

Architekturabhängige Optimierungen müssen deterministisch kontrollierbar sein.

Ein Deterministic Contract kann beispielsweise eine konkrete:

```text
Algorithm Implementation
CPU Feature Set
Execution Target
```

fixieren.

---

## Architektur-Evolution

Gemäß `ADR-ARCH-0019_Evolvierbare_Systemarchitektur` müssen neue Architekturen hinzugefügt werden können, ohne bestehende Kernelkomponenten grundlegend umzuschreiben.

Ein neues Backend implementiert die notwendigen Contracts.

Beispiel:

```text
Future Architecture X
        ↓
Architecture HAL Backend
        ↓
Existing NovaOS Kernel
```

---

## Nicht unterstützte Architekturfeatures

Wenn eine Architektur eine bestimmte Funktion nicht unterstützt, muss dies explizit dargestellt werden.

Beispiel:

```text
Feature:
    CPU Hotplug

Status:
    Unsupported
```

Höhere Komponenten können daraufhin:

- Fallback verwenden,
- Funktion deaktivieren,
- Operation ablehnen.

---

## Architektur-spezifische Erweiterungen

Nicht jede Fähigkeit muss in das gemeinsame Interface aufgenommen werden.

Backend-spezifische Erweiterungen dürfen existieren.

Sie müssen jedoch klar gekennzeichnet sein.

Beispiel:

```text
HalExtension<x86.Virtualization.VMX>
```

Solche Erweiterungen dürfen nicht unbemerkt Voraussetzung allgemeiner Kernelkorrektheit werden.

---

## Testbarkeit

Architecture Backends müssen separat testbar sein.

Tests umfassen mindestens:

- Feature Detection,
- Exception Handling,
- Context Switching,
- Page Table Operations,
- TLB Invalidation,
- Atomics,
- Memory Ordering,
- Timer,
- SMP Startup.

Zusätzlich sollen gemeinsame HAL-Conformance-Tests existieren.

---

## Cross-Architecture Builds

NovaOS soll Cross Compilation für unterstützte Architekturen ermöglichen.

Beispiel:

```text
build --arch=x86_64
build --arch=arm64
build --arch=riscv64
```

Die konkrete Buildsyntax wird nicht durch diese ADR festgelegt.

---

## Emulation

Neue Architekturports sollen zunächst unter Emulation oder Virtualisierung getestet werden können.

Beispiele:

```text
QEMU x86_64
QEMU ARM64
QEMU RISC-V
```

Dies ersetzt keine Tests auf realer Hardware.

---

## KI-Unabhängigkeit

Die Auswahl und Initialisierung eines Architecture Backends darf keine KI voraussetzen.

Hardwareerkennung und Basiskonfiguration müssen deterministisch erfolgen.

KI darf später bei Optimierungen unterstützen.

---

## Normative Anforderungen

1. NovaOS MUSS Architektur- und Plattformabstraktion voneinander unterscheiden.
2. Der hardwareunabhängige Kernel SOLL keine direkten ISA-spezifischen Abhängigkeiten besitzen.
3. Jede unterstützte CPU-Architektur MUSS ein Architecture Backend besitzen.
4. Plattformabhängige Funktionen SOLLEN in separaten Platform Backends implementiert werden.
5. Eine Architecture MUSS mit mehreren Plattformen kombinierbar sein können.
6. Gemeinsame HAL-Contracts MÜSSEN architekturübergreifende Semantik definieren.
7. Architekturspezifische Registermodelle MÜSSEN gekapselt werden.
8. Exception-Mechanismen MÜSSEN auf ein gemeinsames Kernelmodell abgebildet werden.
9. Privilege Models MÜSSEN semantisch abstrahiert werden.
10. Speicherverwaltungsmechanismen MÜSSEN architekturspezifisch implementierbar sein.
11. Unterstützte Page Sizes MÜSSEN abfragbar sein.
12. Virtuelle und physische Address Widths MÜSSEN abfragbar sein.
13. Endianness MUSS explizit modellierbar sein.
14. Memory Ordering DARF NICHT aus x86-spezifischem Verhalten abgeleitet werden.
15. Atomare Operationen MÜSSEN eine architekturunabhängige Semantik besitzen.
16. Context Switching MUSS architekturspezifische Registerzustände kapseln.
17. CPU-local Storage MUSS architekturunabhängig nutzbar sein.
18. SMP Startup MUSS hinter HAL-Mechanismen gekapselt werden.
19. Interrupt Controller MÜSSEN über gemeinsame Contracts eingebunden werden.
20. Timerquellen MÜSSEN architekturspezifisch bereitgestellt werden können.
21. Firmwaremechanismen DÜRFEN keine universellen Kernelannahmen erzeugen.
22. Bootinformationen SOLLEN in eine gemeinsame Bootstruktur überführt werden.
23. CPU- und ISA-Capabilities MÜSSEN maschinenlesbar verfügbar sein.
24. Optionale CPU-Features DÜRFEN NICHT automatisch zu Systempflichten werden.
25. Architecture Backends DÜRFEN optimierte Hardwarepfade bereitstellen.
26. Für kritische optimierte Pfade SOLL ein geeigneter Baseline-Pfad existieren.
27. NovaABI MUSS architekturspezifische Bindings unterstützen.
28. Semantische Nova APIs SOLLEN architekturunabhängig bleiben.
29. Generische Treiber SOLLEN keine unnötigen ISA-Annahmen besitzen.
30. Architekturspezifische Optimierungen DÜRFEN genutzt werden.
31. Die HAL DARF NICHT auf den kleinsten gemeinsamen Nenner aller Architekturen reduziert werden.
32. Resource Economy MUSS relevante Architektur- und Topologieinformationen beziehen können.
33. Lokalitätsplanung MUSS Architektur- und Plattformtopologie nutzen können.
34. Deterministic Mode MUSS relevante architekturspezifische Optimierungsentscheidungen fixieren können.
35. Neue Architekturen SOLLEN ohne grundlegenden Umbau des Kernelkerns integrierbar sein.
36. Nicht unterstützte Funktionen MÜSSEN explizit als nicht unterstützt gemeldet werden.
37. Architekturspezifische HAL-Erweiterungen DÜRFEN existieren.
38. Architekturspezifische Erweiterungen DÜRFEN keine versteckte allgemeine Kernelvoraussetzung erzeugen.
39. Architecture Backends MÜSSEN HAL-Conformance-Tests bestehen.
40. Cross-Architecture Builds SOLLEN unterstützt werden.
41. Emulation SOLL für frühe Architekturports nutzbar sein.
42. Reale Hardware MUSS für produktionsreife Ports zusätzlich getestet werden.
43. Architecture Backend Selection MUSS ohne KI möglich sein.
44. Basishardwareinitialisierung MUSS ohne KI möglich sein.
45. NovaOS MUSS zukünftige CPU-Architekturen durch neue Backendimplementierungen aufnehmen können.

---

## Konsequenzen

### Positive Konsequenzen

- NovaOS bleibt langfristig von einer einzelnen ISA unabhängig.
- ARM64 und RISC-V können später integriert werden.
- Kernelcode bleibt sauberer von Hardwaredetails getrennt.
- neue Plattformen erfordern weniger Änderungen an höheren Schichten.
- Hardwareoptimierungen bleiben trotzdem möglich.
- bessere Testbarkeit und Emulation.
- unterschiedliche Firmwaremodelle können vereinheitlicht werden.
- heterogene Systeme lassen sich besser unterstützen.

### Negative Konsequenzen

- Architecture- und Platform-Layer erhöhen die Strukturkomplexität.
- HAL-Contracts müssen verschiedene Architekturen sinnvoll abdecken.
- manche Optimierungen benötigen architekturspezifische Sonderpfade.
- Cross-Architecture-Tests erhöhen Entwicklungsaufwand.
- ein schlecht gestaltetes gemeinsames Interface kann entweder zu generisch oder zu einschränkend werden.

---

## Verworfene Alternativen

### NovaOS ausschließlich für x86_64

Verworfen.

Dies würde die langfristige Hardwareentwicklung unnötig einschränken.

### Ein identisches Hardwaremodell für alle Architekturen

Verworfen.

ARM, x86, RISC-V und zukünftige Architekturen besitzen relevante Unterschiede.

### Architekturspezifischer Kernel pro ISA

Verworfen.

Zu große Teile des Systems würden dupliziert.

### Kleinster gemeinsamer Nenner

Verworfen.

NovaOS soll moderne Hardwarefeatures vollständig nutzen können.

### ISA-Erkennung direkt in allen Kernelkomponenten

Verworfen.

Dies würde die Plattformkopplung über den gesamten Kernel verteilen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-ARCHITECTURE-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-ARCH-BINDING-0001`
- `NPSPEC-HAL-ARCH-CAPABILITY-0001`
- `NPSPEC-HAL-PRIVILEGE-0001`
- `NPSPEC-HAL-EXCEPTION-0001`
- `NPSPEC-HAL-CONTEXT-0001`
- `NPSPEC-HAL-PAGETABLE-0001`
- `NPSPEC-HAL-TLB-0001`
- `NPSPEC-HAL-ENDIANNESS-0001`
- `NPSPEC-HAL-MEMORYORDER-0001`
- `NPSPEC-HAL-SMPSTART-0001`
- `NPSPEC-HAL-BOOTINFO-0001`
- `NPSPEC-HAL-ARCH-EXTENSION-0001`
- `NPSPEC-HAL-CONFORMANCE-0001`
- `NPSPEC-HAL-X86-0001`
- `NPSPEC-HAL-X86_64-0001`
- `NPSPEC-HAL-ARM64-0001`
- `NPSPEC-HAL-RISCV64-0001`
- `NPSPEC-HAL-VIRTUALPLATFORM-0001`

---

## Verifikation

Die Architektur muss mindestens folgende Szenarien unterstützen:

- Build eines x86_64-Kernels,
- Build eines zweiten Architecture Backends,
- CPU Feature Detection je Backend,
- Architektur-spezifische Exceptionbehandlung,
- Context Switch,
- virtuelle Speicherverwaltung,
- unterschiedliche Page Sizes,
- atomare Operationen,
- Memory Ordering Tests,
- SMP Startup,
- Timerinitialisierung,
- Interruptinitialisierung,
- gemeinsame HAL-Conformance-Tests,
- Boot unter einer emulierten Nicht-x86-Plattform.

---

## Ergebnis

NovaOS behandelt CPU-Architekturen als austauschbare Hardwarebackends und nicht als fundamentale Definition des Betriebssystems.

Das Architekturmodell lautet:

```text
                 NovaOS Kernel
                       │
                       ▼
                Common HAL API
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
        x86_64       ARM64       RISC-V
          │            │            │
          ▼            ▼            ▼
      Platforms     Platforms    Platforms
```

Damit gilt:

```text
The ISA defines how the machine works.

It must not define what NovaOS is.
```

Die zentrale Architekturregel lautet:

```text
Portable semantics.

Architecture-specific mechanisms.

Platform-specific integration.

Future architectures by new backends,
not by redesigning the operating system.
```