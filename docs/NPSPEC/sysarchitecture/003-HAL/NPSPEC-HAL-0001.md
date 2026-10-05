# NPSPEC-HAL-0001 – Nova Hardware Abstraction Layer

## Status

Angenommen

## Kategorie

HAL / Hardware / Kernel / Drivers

## Zweck

Die Nova Hardware Abstraction Layer (HAL) trennt hardwareabhängige Implementierungen von den hardwareunabhängigen Kernel- und Systemkomponenten.

```text
Kernel / System
       ↓
      HAL
       ↓
Architecture / Platform / Hardware
```

Dadurch können Kernel-Komponenten dieselben Schnittstellen verwenden, unabhängig von konkreter CPU-, Plattform- oder Hardwareimplementierung.

## Grundprinzip

```text
Mechanism
   ↓
HAL Interface
   ↓
Platform Implementation
```

Hardwaredetails dürfen nicht unnötig in höhere Systemschichten gelangen.

## HAL-Bereiche

Die HAL abstrahiert insbesondere:

```text
CPU
Interrupts
Timers
Memory Management Hardware
Multiprocessing
Firmware Interfaces
Power Management
Platform I/O
```

Gerätespezifische Funktionen verbleiben grundsätzlich in den jeweiligen Treibern.

## Architekturimplementierungen

Architekturspezifische Implementierungen liegen hinter stabilen HAL-Schnittstellen.

Beispiel:

```text
HAL
 ├── x86
 ├── x86_64
 ├── ARM64
 └── weitere Architekturen
```

Eine neue Architektur soll implementiert werden können, ohne hardwareunabhängige Kernel-Komponenten neu entwerfen zu müssen.

## CPU

Die HAL stellt grundlegende CPU-Funktionen bereit, beispielsweise:

```text
CPU Identification
CPU Features
CPU State
Architecture Registers
CPU Control
```

Architekturspezifische CPU-Funktionen dürfen nur kontrolliert nach außen sichtbar gemacht werden.

## Interrupts und Timer

Kernel-Komponenten verwenden gemeinsame HAL-Schnittstellen für:

```text
Interrupt Controller
Interrupt Routing
System Timer
High Resolution Timer
CPU Timer
```

Die konkrete Hardware kann beispielsweise APIC, IOAPIC oder andere Controller verwenden.

## Memory Management Hardware

Die HAL abstrahiert architekturspezifische Mechanismen wie:

```text
Page Tables
TLB
Address Spaces
Memory Barriers
Cache Control
```

Die eigentliche Speicherpolitik verbleibt außerhalb der HAL.

## Multiprocessing

Die HAL stellt Mechanismen für mehrere CPUs bereit.

```text
CPU Discovery
CPU Startup
Inter-Processor Interrupts
CPU Local State
Synchronization Primitives
```

Scheduling-Entscheidungen gehören nicht zur HAL.

## Firmware

Firmwareabhängige Informationen werden über definierte Plattform-Schnittstellen bereitgestellt.

Beispiele:

```text
UEFI
ACPI
Platform Tables
Boot Information
```

Höhere Kernel-Komponenten sollen nicht direkt von einer bestimmten Firmwareimplementierung abhängig sein.

## Mechanism und Policy

Die HAL implementiert Mechanismen, keine Systempolitik.

```text
HAL:
"Diese CPU kann in einen Energiesparzustand wechseln."

Policy:
"Diese CPU soll jetzt in diesen Zustand wechseln."
```

Entscheidungen über Scheduling, Energiepolitik oder Ressourcenprioritäten liegen außerhalb der HAL.

## Normative Anforderungen

1. NovaOS MUSS hardwareabhängige Mechanismen hinter definierten HAL-Schnittstellen kapseln.
2. Hardwareunabhängige Kernel-Komponenten SOLLEN nicht direkt auf architekturspezifische Hardware zugreifen.
3. HAL-Schnittstellen MÜSSEN klar zwischen Mechanism und Policy trennen.
4. CPU-, Interrupt-, Timer- und Memory-Management-Hardware MUSS über definierte HAL-Abstraktionen erreichbar sein.
5. Multiprocessing-Grundmechanismen MÜSSEN durch die HAL bereitgestellt werden.
6. Architekturspezifische Implementierungen MÜSSEN austauschbar bleiben.
7. Gerätespezifische Logik SOLL in Treibern und nicht in der allgemeinen HAL liegen.
8. Nicht unterstützte Hardwarefunktionen MÜSSEN eindeutig als nicht verfügbar gemeldet werden.
9. Die HAL DARF keine unnötigen Hardwaredetails in höhere Systemschichten leaken.
10. Neue Plattformen SOLLEN ohne grundlegende Änderungen an hardwareunabhängigen Kernel-Komponenten integrierbar sein.

## Abhängigkeiten

- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-HAL-0001`
- `ADR-HAL-0002`
- `ADR-HAL-0003`
- `ADR-HAL-0004`
- `ADR-HAL-0005`
- `ADR-HAL-0006`
- `ADR-HAL-0007`

## Ergebnis

```text
Hardware-independent Kernel
          ↓
         HAL
          ↓
Architecture-specific Implementation
          ↓
Hardware
```

Die HAL bildet damit die stabile Grenze zwischen der allgemeinen NovaOS-Kernelarchitektur und konkreten Hardwareplattformen.