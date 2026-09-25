# NPSPEC-HAL-FIRMWARE-0001 – Nova Firmware Abstraction

## Status

Angenommen

## Kategorie

HAL / Firmware / Platform / Boot

## Zweck

NovaOS kapselt Firmware-Schnittstellen hinter einer einheitlichen Abstraktion, damit höhere Systemschichten nicht direkt von UEFI, ACPI oder anderen Firmwarestandards abhängig sind.

```text
Kernel / HAL
     ↓
Firmware Abstraction
     ↓
UEFI / ACPI / Device Tree / Platform Firmware
```

## Grundprinzip

Firmware dient als Informations- und Kontrollquelle der Plattform, bestimmt jedoch nicht die interne Architektur von NovaOS.

```text
Firmware Data
     ↓
Validate
     ↓
Normalize
     ↓
Nova Platform Model
```

## Firmwarequellen

Die Abstraktion kann Informationen aus unterschiedlichen Quellen zusammenführen:

```text
UEFI
ACPI
Device Tree
Bootloader Information
Platform-specific Firmware
```

Die konkrete Quelle bleibt für höhere Systemschichten möglichst transparent.

## Firmwareinformationen

Normalisiert bereitgestellt werden können unter anderem:

```text
Memory Map
CPU Topology
Interrupt Topology
NUMA Information
Timers
Power Management
Device Information
Platform Tables
Boot Services Information
```

## Boot-Übergang

Während des Bootvorgangs verfügbare Firmwareinformationen müssen übernommen werden, bevor nicht dauerhaft verfügbare Firmwaredienste beendet werden.

```text
Firmware
   ↓
Bootloader
   ↓
Validated Boot Information
   ↓
Firmware Abstraction
   ↓
Runtime System
```

## UEFI

UEFI-spezifische Funktionen werden nicht direkt im allgemeinen Kernel verwendet.

Nach `ExitBootServices()` darf NovaOS nicht mehr von UEFI Boot Services abhängig sein.

UEFI Runtime Services dürfen nur über kontrollierte Firmware-Schnittstellen verwendet werden.

## ACPI

ACPI-Tabellen werden validiert, geparst und in interne NovaOS-Strukturen überführt.

```text
ACPI
 ↓
Validation
 ↓
Parsing
 ↓
Normalized Platform Information
```

Andere Kernel-Komponenten sollen möglichst nicht direkt mit ACPI-Tabellen arbeiten.

## Validierung

Firmwaredaten gelten nicht automatisch als vertrauenswürdig.

Geprüft werden müssen, soweit anwendbar:

```text
Signatures
Checksums
Lengths
Versions
Address Ranges
Table Relationships
```

Ungültige Daten dürfen nicht ungeprüft verwendet werden.

## Fehlende Informationen

Nicht verfügbare Firmwareinformationen müssen explizit dargestellt werden.

```text
Unknown ≠ Valid
Unknown ≠ Unsupported
```

Falls möglich, dürfen alternative Erkennungsmechanismen verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS Firmwarezugriffe hinter definierten Schnittstellen kapseln.
2. Firmwaredaten MÜSSEN vor ihrer Verwendung validiert werden.
3. Firmwareinformationen SOLLEN in interne NovaOS-Strukturen normalisiert werden.
4. Höhere Kernel-Komponenten SOLLEN nicht direkt von UEFI oder ACPI abhängig sein.
5. NovaOS DARF nach `ExitBootServices()` nicht von UEFI Boot Services abhängig sein.
6. UEFI Runtime Services MÜSSEN kontrolliert verwendet werden.
7. ACPI-Informationen MÜSSEN vor ihrer Nutzung validiert werden.
8. Fehlende und ungültige Firmwareinformationen MÜSSEN unterscheidbar sein.
9. Firmwarefehler DÜRFEN nicht automatisch zu unsicheren Standardannahmen führen.
10. Alternative Firmwaremechanismen SOLLEN integrierbar bleiben.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-HAL-0006`

## Ergebnis

```text
UEFI / ACPI / Firmware
          ↓
Validation + Normalization
          ↓
Nova Firmware Abstraction
          ↓
Platform / HAL / Kernel
```

NovaOS erhält damit eine kontrollierte und austauschbare Firmware-Schnittstelle, ohne seine Kernelarchitektur an einen bestimmten Firmwarestandard zu binden.