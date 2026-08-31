# ADR-HAL-0003 – Firmware über Platform Provider abstrahieren

## Status

Angenommen

## Kategorie

HAL / Plattformabstraktion / Firmware / Boot / Hardwarebeschreibung

## Kontext

NovaOS muss mit unterschiedlichen Firmwaremodellen umgehen können.

Abhängig von Architektur und Plattform können unter anderem verwendet werden:

```text
UEFI
ACPI
Device Tree
PSCI
SBI
Legacy BIOS derived data
platform-specific firmware interfaces
```

Diese Systeme unterscheiden sich deutlich hinsichtlich:

- Bootübergabe,
- Hardwarebeschreibung,
- CPU-Topologie,
- Interruptinformationen,
- Power Management,
- Runtime Services,
- Geräteerkennung,
- Speicherinformationen,
- Plattformkonfiguration.

Direkte Abhängigkeiten höherer Kernelkomponenten von einzelnen Firmwaretechnologien würden die Plattformportabilität einschränken.

Nicht gewünscht ist beispielsweise:

```text
Scheduler
  ↓
ACPI Parser
```

oder:

```text
Power Manager
  ↓
UEFI Runtime Service
```

Stattdessen benötigt NovaOS eine einheitliche Plattformgrenze.

---

## Entscheidung

Firmware wird in NovaOS ausschließlich über **Platform Provider** in die höhere Systemarchitektur eingebunden.

Das Grundmodell lautet:

```text
Kernel / System Services
        │
        ▼
Platform Contracts
        │
        ▼
Platform Provider
        │
        ├── UEFI
        ├── ACPI
        ├── Device Tree
        ├── PSCI
        ├── SBI
        └── platform-specific sources
```

Höhere Systemkomponenten greifen nicht direkt auf Firmwareformate zu, sofern dafür ein Platform Contract existiert.

Der Platform Provider übersetzt firmware- und plattformspezifische Informationen in ein einheitliches internes Modell.

---

## Grundprinzip

Es gilt:

```text
Firmware describes the platform.

Platform Providers translate it.

The kernel consumes platform semantics.
```

Firmware ist damit Datenquelle und Mechanismus, aber nicht das native Architekturmodell von NovaOS.

---

## Platform Provider

Ein Platform Provider repräsentiert eine konkrete Plattformintegration.

Beispiele:

```text
PcUefiPlatformProvider
GenericArmPlatformProvider
RiscVPlatformProvider
VirtualPlatformProvider
```

Ein Provider kann mehrere Firmwarequellen kombinieren.

Beispiel:

```text
PC Platform Provider
├── UEFI
├── ACPI
├── SMBIOS
└── PCI configuration
```

Der Kernel muss diese Quellen nicht einzeln koordinieren.

---

## Gemeinsames Plattformmodell

Platform Provider erzeugen ein normalisiertes internes Plattformmodell.

Beispiel:

```text
PlatformModel {
    architecture
    firmware
    cpus
    memory
    numa
    interruptControllers
    timers
    devices
    power
    topology
}
```

Dieses Modell ist semantisch und darf nicht unnötig die Struktur eines bestimmten Firmwareformats spiegeln.

---

## Bootinformationen

Der Bootloader übergibt eine standardisierte Boot Information Structure an den Kernel.

Beispiel:

```text
BootInfo {
    architecture
    platform
    memoryMap
    framebuffer
    firmwareReferences
    initModules
    commandLine
}
```

Firmware-spezifische Handles oder Pointer dürfen enthalten sein, müssen jedoch von der Platform Layer interpretiert werden.

Höhere Kernelkomponenten sollen keine direkten Abhängigkeiten davon besitzen.

---

## UEFI

UEFI wird als Firmwarebackend unterstützt.

UEFI kann insbesondere liefern:

- Memory Map,
- Graphics Output Protocol,
- Firmware Tables,
- Boot Services,
- Runtime Services,
- Configuration Tables.

Boot Services enden grundsätzlich mit:

```text
ExitBootServices()
```

Danach dürfen Kernelkomponenten nicht versehentlich weiterhin Boot-Service-Funktionen verwenden.

---

## UEFI Runtime Services

UEFI Runtime Services dürfen unterstützt werden.

Sie sind jedoch optional.

Beispiele:

```text
Variables
Time
ResetSystem
Firmware Services
```

Der Kernel darf für grundlegende Korrektheit nicht von Runtime Services abhängig sein.

Wenn Runtime Services nicht verfügbar oder deaktiviert sind, muss NovaOS weiter funktionieren können.

---

## ACPI

ACPI wird auf geeigneten Plattformen als Firmware- und Plattformdatenquelle verwendet.

Typische Informationen:

```text
CPU topology
APIC topology
NUMA
Power states
Timers
Interrupt routing
Platform devices
```

ACPI-Daten werden geparst und anschließend in interne Plattformobjekte übersetzt.

Höhere Kernelkomponenten sollen nicht mit ACPI-Tabellenstrukturen arbeiten.

Nicht:

```text
Scheduler
    ↓
MADT
```

sondern:

```text
Scheduler
    ↓
CpuTopology
```

---

## Device Tree

Device Tree wird insbesondere für ARM- und andere Plattformen unterstützt.

Informationen können umfassen:

```text
CPU
Memory
Interrupt Controller
Timers
Buses
Devices
Clocks
Reset Controllers
Power Domains
```

Device-Tree-Nodes werden ebenfalls in gemeinsame Plattformobjekte übersetzt.

---

## PSCI

Auf ARM-Plattformen kann PSCI für CPU- und Power-Operationen verwendet werden.

Beispiel:

```text
CPU_ON
CPU_OFF
SYSTEM_OFF
SYSTEM_RESET
```

Diese Operationen werden durch den Platform Provider hinter semantischen NovaOS-Operationen gekapselt.

Beispiel:

```text
Platform.StartCpu()
Platform.Shutdown()
Platform.Reset()
```

---

## SBI

Auf RISC-V-Plattformen kann SBI eine vergleichbare Rolle übernehmen.

Auch SBI darf nicht direkt in höheren Kernelkomponenten verwendet werden.

Stattdessen:

```text
RISC-V SBI
    ↓
RiscVPlatformProvider
    ↓
Platform Contracts
```

---

## Legacy BIOS

NovaOS kann auf Legacy-BIOS-Systemen booten.

Nach dem Übergang in den geschützten beziehungsweise Long Mode soll der Kernel jedoch nicht dauerhaft von BIOS-Interruptdiensten abhängig sein.

Legacy-BIOS-Informationen werden soweit nötig während Boot und Platform Initialization in interne Strukturen übernommen.

---

## Mehrere Firmwarequellen

Eine Plattform kann Informationen aus mehreren Quellen erhalten.

Beispiel:

```text
UEFI
+
ACPI
+
SMBIOS
+
PCI discovery
```

Der Platform Provider ist für Zusammenführung und Konfliktbehandlung verantwortlich.

---

## Priorität von Quellen

Wenn mehrere Quellen dieselbe Eigenschaft beschreiben, muss eine definierte Prioritäts- oder Validierungsregel existieren.

Beispiel:

```text
Authoritative Platform Source
        ↓
Validated Secondary Source
        ↓
Fallback Discovery
```

Diese Regeln sind plattformspezifisch und gehören in den jeweiligen Platform Provider.

---

## Inkonsistente Firmware

Firmwaredaten dürfen nicht grundsätzlich als korrekt angenommen werden.

Platform Provider müssen relevante Daten validieren.

Beispiele:

- ungültige Tabellenlängen,
- fehlerhafte Checksums,
- überlappende Speicherbereiche,
- ungültige CPU-IDs,
- widersprüchliche Topologie,
- ungültige Device Resources.

Ungültige Daten müssen explizit behandelt werden.

---

## Fail-Safe Defaults

Bei unsicheren Firmwareinformationen gelten Fail-Safe Defaults.

Beispiel:

```text
Unknown memory region
    ↓
Do not treat as freely allocatable RAM
```

```text
Invalid interrupt routing
    ↓
Do not enable device interrupt
```

```text
Unknown DMA capability
    ↓
Restrict DMA
```

---

## Firmware ist nicht vertrauenswürdig per Definition

Firmware befindet sich unterhalb beziehungsweise außerhalb großer Teile der NovaOS-Vertrauenskette.

Daher müssen Firmwaredaten als extern bereitgestellte Plattforminformationen behandelt werden.

Sie dürfen nicht ungeprüft:

- Kernelprivilegien erzeugen,
- Speicher freigeben,
- DMA autorisieren,
- Security Policies umgehen.

---

## Platform Contracts

Höhere Systemteile sollen gegen semantische Platform Contracts arbeiten.

Beispiele:

```text
PlatformCpuTopology
PlatformMemoryMap
PlatformInterruptTopology
PlatformTimerRegistry
PlatformPowerControl
PlatformDeviceGraph
PlatformFirmwareServices
```

Diese Contracts müssen versionierbar sein.

---

## Plattformfähigkeiten

Platform Provider melden explizit, welche Funktionen verfügbar sind.

Beispiel:

```text
PlatformCapabilities {
    cpuHotplug
    systemReset
    systemPowerOff
    suspend
    firmwareVariables
    runtimeClock
}
```

Nicht unterstützte Funktionen werden explizit gemeldet.

---

## Optionalität

Firmwarefeatures dürfen nicht stillschweigend als universell vorausgesetzt werden.

Beispielsweise darf NovaOS nicht voraussetzen, dass jede Plattform besitzt:

```text
ACPI
UEFI Runtime Services
Device Tree
PSCI
SBI
```

Stattdessen werden Funktionen anhand von Platform Capabilities verwendet.

---

## Power Management

Der Power Manager verwendet semantische Plattformoperationen.

Beispiel:

```text
PlatformPower {
    shutdown
    reboot
    suspend
    resume
    cpuPowerState
}
```

Die konkrete Umsetzung kann beispielsweise über:

```text
ACPI
PSCI
SBI
UEFI
```

erfolgen.

---

## CPU Discovery

CPU-Discovery wird durch Architecture- und Platform Provider gemeinsam aufgebaut.

Firmware kann CPU-Informationen liefern.

Die Architecture Layer ergänzt ISA-spezifische Informationen.

Ergebnis:

```text
CpuTopology
```

statt firmwareabhängiger Rohdaten.

---

## Speicherkarte

Firmware Memory Maps werden in ein gemeinsames NovaOS-Modell übersetzt.

Beispiel:

```text
MemoryRegion {
    base
    length
    type
    attributes
    numaNode
}
```

Mögliche Typen:

```text
Usable
Reserved
Firmware
MMIO
Persistent
BootData
Kernel
Unknown
```

Die konkrete Firmwarecodierung bleibt außerhalb höherer Speicherverwaltung.

---

## Gerätebeschreibung

Platform Provider stellen initial erkannte Plattformgeräte bereit.

Danach können reguläre Bus- und Treibermechanismen weitere Geräte entdecken.

Firmwarebeschreibung und Runtime Discovery dürfen kombiniert werden.

---

## Runtime Discovery

Firmware ist nicht zwingend die einzige Quelle für Hardwareinformationen.

Beispiel:

```text
Firmware Platform Description
        +
PCI Enumeration
        +
USB Enumeration
        +
Runtime Hotplug
        ↓
Hardware Graph
```

Der Platform Provider stellt den initialen Kontext bereit.

---

## Firmware Services

Firmwarefunktionen, die nach dem Boot weiterhin nutzbar sind, werden explizit als Services modelliert.

Sie dürfen nicht als globale implizite Funktionen verfügbar sein.

Beispiel:

```text
FirmwareVariableService
FirmwareResetService
FirmwareTimeService
```

---

## Isolation

Firmware Runtime Services können besondere Adressraum-, Locking- oder CPU-Anforderungen besitzen.

Solche Anforderungen müssen hinter dem Firmware-Service-Backend gekapselt werden.

Höhere Komponenten sollen diese Besonderheiten nicht selbst implementieren müssen.

---

## Fehlerbehandlung

Firmwareoperationen benötigen explizite Fehlerzustände.

Beispiele:

```text
Unsupported
Unavailable
InvalidFirmwareData
FirmwareFault
Timeout
AccessDenied
CorruptedData
InconsistentTopology
```

Firmwarefehler dürfen nicht automatisch zum Kernel Panic führen, sofern ein sicherer Fallback möglich ist.

---

## Fallback

Platform Provider können alternative Mechanismen verwenden.

Beispiel:

```text
Shutdown
├── ACPI
├── UEFI Runtime
└── platform fallback
```

oder:

```text
Topology
├── ACPI
└── hardware discovery
```

Fallbacks müssen jedoch semantisch sicher sein.

---

## Deterministic Mode

Firmwareabhängige Entscheidungen müssen für deterministische Ausführung kontrollierbar sein, wenn sie das Laufzeitverhalten beeinflussen.

Nach der initialen Plattformerkennung sollen stabile normalisierte Plattformdaten verwendet werden.

---

## Architecture Introspection

Die Plattform- und Firmwarekonfiguration muss introspektierbar sein.

Beispiel:

```text
Platform:
    PC

Architecture:
    x86_64

Firmware:
    UEFI

Platform Sources:
    ACPI
    SMBIOS

Runtime Services:
    Variables
    Reset
```

Zusätzlich sollen Fehler und Fallbacks sichtbar sein.

Beispiel:

```text
ACPI NUMA Table:
    invalid

Fallback:
    single NUMA domain
```

---

## Evolvierbarkeit

Platform Provider müssen austausch- und versionierbar sein.

Neue Firmwaremodelle sollen durch neue Provider oder Provider-Erweiterungen eingebunden werden können.

Der Kernelkern darf dafür nicht grundlegend verändert werden müssen.

---

## KI-Unabhängigkeit

Firmware Parsing, Hardwareinitialisierung und Platform Provider Selection müssen ohne KI funktionieren.

KI darf Firmwaredaten analysieren oder Diagnosen unterstützen, ist aber keine Voraussetzung für korrekte Plattforminitialisierung.

---

## Sicherheitsgrenze

Platform Provider besitzen eine besonders privilegierte Position.

Deshalb müssen sie:

- kleine, klar definierte Contracts besitzen,
- Eingabedaten validieren,
- keine unnötigen Privilegien weiterreichen,
- Fehler explizit melden,
- möglichst isolierbar sein.

---

## Normative Anforderungen

1. NovaOS MUSS Firmware über Platform Provider abstrahieren.
2. Höhere Kernelkomponenten SOLLEN keine direkten Firmwareformat-Abhängigkeiten besitzen.
3. Platform Provider MÜSSEN Firmwareinformationen in semantische interne Strukturen übersetzen.
4. Architektur- und Plattformabstraktion MÜSSEN getrennt bleiben.
5. Ein Platform Provider DARF mehrere Firmwarequellen kombinieren.
6. UEFI MUSS als unterstützbares Firmwarebackend vorgesehen werden.
7. ACPI MUSS auf geeigneten PC-Plattformen unterstützt werden können.
8. Device Tree MUSS auf geeigneten Plattformen unterstützt werden können.
9. PSCI SOLL auf ARM-Plattformen integrierbar sein.
10. SBI SOLL auf RISC-V-Plattformen integrierbar sein.
11. Legacy BIOS DARF für den Bootpfad unterstützt werden.
12. Der Kernel DARF nach Plattforminitialisierung NICHT unnötig von BIOS-Interruptdiensten abhängen.
13. Bootinformationen SOLLEN in eine gemeinsame Bootstruktur überführt werden.
14. Firmware-spezifische Handles MÜSSEN hinter Plattformgrenzen gekapselt werden.
15. Firmwaredaten MÜSSEN vor sicherheitsrelevanter Nutzung validiert werden.
16. Ungültige Firmwaredaten DÜRFEN NICHT stillschweigend akzeptiert werden.
17. Firmware Memory Maps MÜSSEN in ein gemeinsames Speicherregionsmodell übersetzt werden.
18. CPU-Informationen MÜSSEN in ein gemeinsames Topologiemodell überführt werden.
19. Interruptinformationen SOLLEN in gemeinsame Interruptstrukturen überführt werden.
20. Firmwaregeräte SOLLEN in einen gemeinsamen Hardwaregraphen überführt werden.
21. Platform Capabilities MÜSSEN explizit abfragbar sein.
22. Nicht unterstützte Firmwarefunktionen MÜSSEN explizit gemeldet werden.
23. UEFI Runtime Services DÜRFEN optional unterstützt werden.
24. UEFI Runtime Services DÜRFEN keine Voraussetzung für grundlegende Kernelkorrektheit sein.
25. Platform Power Operations MÜSSEN von konkreten Firmwaremechanismen abstrahiert sein.
26. Mehrere Firmwarequellen MÜSSEN definierte Prioritäts- oder Validierungsregeln besitzen.
27. Unbekannte Speicherregionen DÜRFEN NICHT automatisch als frei nutzbar gelten.
28. Ungültiges Interrupt Routing DARF NICHT automatisch aktiviert werden.
29. Firmwareinformationen DÜRFEN NICHT automatisch als vertrauenswürdig gelten.
30. Firmwaredaten DÜRFEN keine Security- oder Capability-Prüfungen umgehen.
31. Firmware Runtime Services SOLLEN über explizite Service-Interfaces verfügbar sein.
32. Firmware Runtime Services SOLLEN keine unstrukturierten globalen Kernelabhängigkeiten erzeugen.
33. Platform Provider SOLLEN alternative Fallbackmechanismen unterstützen können.
34. Firmwarefehler MÜSSEN explizit propagiert werden.
35. Sichere Firmware-Fallbacks SOLLEN bevorzugt werden, wenn primäre Quellen ausfallen.
36. Platform Provider und ihre Contracts MÜSSEN versionierbar sein.
37. Neue Firmwaremodelle SOLLEN durch neue Provider integrierbar sein.
38. Platform Initialization MUSS ohne KI funktionieren.
39. Firmware Parsing MUSS ohne KI funktionieren.
40. Architecture Introspection SOLL verwendete Firmwarequellen und Provider darstellen können.
41. Architecture Introspection SOLL relevante Firmwarefehler und Fallbacks darstellen können.
42. Platform Provider SOLLEN möglichst kleine privilegierte Oberflächen besitzen.
43. Platform Provider MÜSSEN externe Firmwareeingaben defensiv behandeln.
44. Firmware darf die native NovaOS-Systemarchitektur NICHT definieren.
45. NovaOS MUSS Plattformsemantik konsumieren, nicht Firmwareformate.

---

## Konsequenzen

### Positive Konsequenzen

- UEFI, ACPI, Device Tree, PSCI und SBI bleiben hinter einer gemeinsamen Architekturgrenze.
- höhere Kernelkomponenten werden portabler.
- Firmwarefehler können zentral validiert und behandelt werden.
- neue Plattformen lassen sich einfacher hinzufügen.
- Power Management und Topologie werden firmwareunabhängiger.
- Legacy- und moderne Plattformen können dieselben höheren Kernelmodelle verwenden.
- Tests mit simulierten Platform Providern werden möglich.

### Negative Konsequenzen

- Platform Provider müssen mehrere sehr unterschiedliche Firmwaremodelle vereinheitlichen.
- zusätzliche Übersetzungs- und Validierungslogik ist erforderlich.
- plattformspezifische Sonderfälle bleiben innerhalb der Provider bestehen.
- fehlerhafte Firmware kann komplexe Fallbacklogik erfordern.

---

## Verworfene Alternativen

### ACPI als universelles NovaOS-Plattformmodell

Verworfen.

ACPI ist nicht auf allen Zielarchitekturen vorhanden und beschreibt ein externes Firmwaremodell.

### Device Tree als universelles Plattformmodell

Verworfen.

Auch Device Tree ist nicht für jede Zielplattform die geeignete oder vorhandene Firmwarequelle.

### Direkter Firmwarezugriff aus höheren Kernelkomponenten

Verworfen.

Dies würde Plattformabhängigkeiten über den gesamten Kernel verteilen.

### UEFI als permanente Kernelgrundlage

Verworfen.

NovaOS soll auch ohne UEFI beziehungsweise ohne UEFI Runtime Services funktionieren können.

### Firmwaredaten ungeprüft übernehmen

Verworfen.

Firmware kann fehlerhafte oder inkonsistente Daten bereitstellen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-PLATFORM-PROVIDER-0001`
- `NPSPEC-HAL-PLATFORM-MODEL-0001`
- `NPSPEC-HAL-BOOTINFO-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `NPSPEC-HAL-FIRMWARE-SERVICE-0001`
- `NPSPEC-HAL-UEFI-0001`
- `NPSPEC-HAL-UEFI-RUNTIME-0001`
- `NPSPEC-HAL-ACPI-0001`
- `NPSPEC-HAL-DEVICETREE-0001`
- `NPSPEC-HAL-PSCI-0001`
- `NPSPEC-HAL-SBI-0001`
- `NPSPEC-HAL-BIOS-0001`
- `NPSPEC-HAL-FIRMWARE-VALIDATION-0001`
- `NPSPEC-HAL-FIRMWARE-FALLBACK-0001`
- `NPSPEC-HAL-PLATFORM-CAPABILITY-0001`
- `NPSPEC-HAL-PLATFORM-POWER-0001`
- `NPSPEC-HAL-PLATFORM-TOPOLOGY-0001`
- `NPSPEC-HAL-PLATFORM-INTROSPECTION-0001`

---

## Verifikation

Die Architektur muss insbesondere folgende Szenarien abdecken:

- UEFI-Boot mit ACPI,
- UEFI-Boot ohne Runtime Services,
- Legacy-BIOS-Boot,
- ARM64-Plattform mit Device Tree,
- ARM64-Plattform mit PSCI,
- RISC-V-Plattform mit SBI,
- mehrere gleichzeitig verwendete Firmwarequellen,
- beschädigte Firmwaretabelle,
- widersprüchliche Topologieinformationen,
- ungültige Speicherregion,
- nicht verfügbare Power-Funktion,
- Firmware-Fallback,
- Platform Provider Mock für Kerneltests.

---

## Ergebnis

Firmware wird in NovaOS nicht als natives Kernelmodell verwendet.

Stattdessen gilt:

```text
Firmware
    │
    ▼
Platform Provider
    │
    ▼
Normalized Platform Model
    │
    ▼
HAL / Kernel / System Services
```

Dadurch können unterschiedliche Firmwaretechnologien nebeneinander unterstützt werden, ohne die höheren Systemschichten an sie zu binden.

Die zentrale Architekturregel lautet:

```text
Firmware is an input.

Platform semantics are the interface.

The kernel must not become
an ACPI, UEFI or Device Tree implementation detail.
```