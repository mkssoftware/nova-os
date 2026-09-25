# NPSPEC-HAL-PLATFORM-0001 – Nova Platform Abstraction

## Status

Angenommen

## Kategorie

HAL / Platform / Firmware / Hardware Discovery

## Zweck

Die Nova Platform Abstraction kapselt plattformspezifische Eigenschaften, die nicht direkt zur CPU-Architektur oder zu einzelnen Gerätetreibern gehören.

```text
Kernel / Drivers
       ↓
Platform Abstraction
       ↓
Firmware / Board / Machine
```

## Grundprinzip

```text
Architecture ≠ Platform ≠ Device
```

Die CPU-Architektur beschreibt grundlegende Prozessorfunktionen.

Die Plattform beschreibt die konkrete Systemumgebung, in der diese Architektur betrieben wird.

## Plattforminformationen

Die Platform Abstraction stellt strukturierte Informationen bereit über:

```text
Firmware
Platform Tables
CPU Topology
Interrupt Topology
Memory Map
Timer Sources
Power Management
Device Discovery
Boot Environment
```

## Firmware

Firmwarezugriffe werden hinter gemeinsamen Plattform-Schnittstellen gekapselt.

Unterstützte Quellen können beispielsweise sein:

```text
UEFI
ACPI
Device Tree
Platform-specific Firmware
```

Höhere Systemschichten sollen nicht direkt von einer bestimmten Firmware abhängig sein.

## Hardware Discovery

Die Plattformschicht stellt Informationen bereit, die zur Erkennung und Initialisierung der Hardware benötigt werden.

```text
Firmware Tables
      ↓
Platform Discovery
      ↓
Normalized Platform Information
      ↓
HAL / Driver System
```

Die eigentliche Gerätelogik verbleibt in den Gerätetreibern.

## Plattformbeschreibung

Eine erkannte Plattform soll mindestens beschreiben können:

```text
PlatformID
Architecture
Firmware
CPU Topology
Memory Regions
Interrupt Controllers
Timers
Available Buses
Platform Features
```

## Boot-Übergabe

Informationen des Bootloaders werden validiert und in die interne Plattformdarstellung übernommen.

```text
Bootloader
    ↓
Boot Information
    ↓
Validation
    ↓
Platform State
```

Firmware- oder Bootloaderdaten dürfen nicht ungeprüft als vertrauenswürdig behandelt werden.

## Initialisierung

Die Plattforminitialisierung erfolgt in definierter Reihenfolge.

```text
Boot Information
      ↓
Platform Detection
      ↓
Firmware Parsing
      ↓
Topology Discovery
      ↓
HAL Initialization
      ↓
Driver Discovery
```

## Mehrere Plattformen

NovaOS soll unterschiedliche Plattformimplementierungen hinter derselben Schnittstelle unterstützen.

```text
Platform API
 ├── PC / UEFI / ACPI
 ├── Embedded Platform
 ├── Virtual Machine
 └── zukünftige Plattformen
```

## Virtualisierung

Virtuelle Maschinen dürfen als eigene Plattformvarianten erkannt werden.

Plattformspezifische Optimierungen sind zulässig, dürfen jedoch die allgemeine HAL-Architektur nicht umgehen.

## Normative Anforderungen

1. NovaOS MUSS plattformspezifische Funktionen hinter einer definierten Platform Abstraction kapseln.
2. Architektur-, Plattform- und Gerätelogik MÜSSEN logisch getrennt bleiben.
3. Firmwareinformationen MÜSSEN vor ihrer Verwendung validiert werden.
4. Bootinformationen MÜSSEN in eine definierte interne Plattformdarstellung überführt werden.
5. CPU-, Memory-, Interrupt- und Timer-Topologien MÜSSEN strukturiert bereitgestellt werden können.
6. Höhere Systemschichten SOLLEN nicht direkt von UEFI, ACPI oder anderen Firmwarestandards abhängig sein.
7. Gerätelogik DARF NICHT unnötig in die Platform Abstraction verschoben werden.
8. Nicht verfügbare Plattformfunktionen MÜSSEN eindeutig erkennbar sein.
9. Virtuelle und physische Plattformen SOLLEN über dieselben grundlegenden Schnittstellen unterstützt werden.
10. Neue Plattformimplementierungen SOLLEN ohne grundlegende Änderungen am hardwareunabhängigen Kernel integrierbar sein.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-HAL-0001`
- `ADR-HAL-0002`
- `ADR-HAL-0003`

## Ergebnis

```text
Firmware / Machine
        ↓
Platform Abstraction
        ↓
Normalized Platform Model
        ↓
HAL / Kernel / Drivers
```

NovaOS erhält damit eine einheitliche Sicht auf unterschiedliche physische und virtuelle Plattformen, ohne plattformspezifische Details im restlichen Kernel zu verteilen.