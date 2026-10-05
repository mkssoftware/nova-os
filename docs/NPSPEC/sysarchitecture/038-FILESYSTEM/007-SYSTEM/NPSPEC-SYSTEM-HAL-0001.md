# NPSPEC-SYSTEM-HAL-0001 – Nova Hardware Abstraction Layer

## Status

Angenommen

## Kategorie

System / Kernel / HAL

## Zweck

NovaOS definiert die Hardware Abstraction Layer (HAL) als einheitliche Abstraktionsschicht zwischen hardwarespezifischen Plattformmechanismen und den übrigen Kernel-Subsystemen.

Die HAL verhindert, dass Kernelkomponenten direkt von konkreten Plattform-, Firmware- oder Controllerdetails abhängig werden.

## Grundprinzipien

```text
HAL ≠ Driver Framework
HAL ≠ Firmware
HAL ≠ Hardware Policy
Hardware Interface ≠ Device Identity
Platform Detail ≠ Kernel-wide Dependency
```

## Architektur

```text
Kernel Subsystems
       ↓
      HAL
       ↓
Platform-specific Implementation
       ↓
Firmware / Hardware
```

Kernelkomponenten verwenden definierte HAL-Schnittstellen statt direkter plattformspezifischer Zugriffe.

## Verantwortlichkeiten

Die HAL abstrahiert insbesondere:

```text
CPU / Platform
Interrupt Controller
Timers
Firmware Interfaces
Memory Topology
NUMA Topology
DMA / IOMMU
Hardware Discovery
Power Interfaces
Platform Reset / Shutdown
Hotplug Mechanisms
```

Gerätespezifische Funktionalität verbleibt grundsätzlich im jeweiligen Treiber.

## Plattformimplementierungen

NovaOS darf mehrere HAL-Backends unterstützen:

```text
HAL Interface
├── x86 Implementation
├── x86-64 Implementation
├── ARM Implementation
└── Future Platforms
```

Nicht unterstützte Funktionen müssen explizit als nicht verfügbar gemeldet werden.

## Hardware Topology

Die HAL stellt eine normalisierte Sicht auf die Hardware bereit:

```text
CPU
Core
NUMA Node
Memory Region
Interrupt Controller
IOMMU
Bus
```

Höhere Subsysteme müssen dadurch möglichst unabhängig von der konkreten Firmwarebeschreibung bleiben.

## Interrupts und Timer

Kernelkomponenten verwenden abstrahierte Interrupt- und Timermechanismen.

```text
Kernel
  ↓
HAL Interrupt / Timer API
  ↓
Platform Controller
```

Plattformspezifische Implementierungen dürfen unterschiedliche Hardwaremechanismen verwenden, solange der definierte HAL-Vertrag eingehalten wird.

## DMA und IOMMU

Die HAL stellt die grundlegenden Plattformmechanismen für DMA und IOMMU bereit.

Treiber und I/O-Subsysteme müssen diese Mechanismen über kontrollierte Schnittstellen verwenden können.

## Firmware

Firmwareinformationen werden durch die HAL normalisiert.

```text
BIOS / UEFI / ACPI / Platform Firmware
                ↓
               HAL
                ↓
        Normalized Interface
```

Höhere Kernelbereiche sollen Firmwaredetails nicht selbst interpretieren müssen.

## Sicherheit

Die HAL läuft im privilegierten Systemkontext und muss besonders klein und kontrollierbar bleiben.

Hardwarezugriff über die HAL erzeugt keine zusätzliche Authority für aufrufende Prozesse oder Treiber.

## Normative Anforderungen

1. NovaOS MUSS eine definierte Hardware Abstraction Layer besitzen.
2. Kernel-Subsysteme SOLLEN plattformspezifische Hardware ausschließlich über definierte HAL-Schnittstellen verwenden.
3. HAL-Schnittstellen MÜSSEN von konkreten Plattformimplementierungen getrennt sein.
4. Gerätespezifische Logik SOLL im Driver Framework verbleiben.
5. Interrupt-, Timer-, DMA- und IOMMU-Mechanismen MÜSSEN abstrahierbar sein.
6. Hardware- und Speicher-Topologie MUSS normalisiert bereitgestellt werden können.
7. Firmwareinformationen SOLLEN über ein einheitliches HAL-Modell bereitgestellt werden.
8. Nicht unterstützte Hardwarefunktionen MÜSSEN eindeutig erkennbar sein.
9. Die HAL DARF keine zusätzliche Authority für aufrufende Komponenten erzeugen.
10. HAL-Zustand und erkannte Plattformfunktionen MÜSSEN introspektierbar sein.
11. Neue Plattformimplementierungen DÜRFEN keine Änderungen an allen höheren Kernel-Subsystemen erfordern.
12. Die HAL SOLL klein, deterministisch und für sicherheitskritische Prüfung geeignet bleiben.

## Abhängigkeiten

- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt mit der HAL eine klar definierte Grenze zwischen Kernelarchitektur und konkreter Hardwareplattform. Neue Plattformen können durch eigene HAL-Implementierungen unterstützt werden, während Speicher-, Interrupt-, I/O- und andere Kernel-Subsysteme weitgehend hardwareunabhängig bleiben.