# NPSPEC-DRIVER-MODEL-0001 – Nova Driver Model

## Status

Angenommen

## Kategorie

Driver / Architecture / Kernel

## Zweck

NovaOS definiert ein einheitliches Treibermodell für Hardware, virtuelle Geräte und systemnahe I/O-Provider.

```text
Consumer
   ↓
Nova API
   ↓
Driver Framework
   ↓
Driver
   ↓
HAL / Hardware
```

Treiber sollen konkrete Geräte implementieren, ohne höhere Systemschichten direkt an Hardwaredetails zu koppeln.

## Grundprinzip

```text
Driver ≠ Device
Driver ≠ HAL
Driver ≠ Policy
```

Der Treiber implementiert gerätespezifische Mechanismen. Globale Scheduling-, Security-, Ressourcen- und Systemrichtlinien verbleiben in den zuständigen NovaOS-Subsystemen.

## Driver Object

Ein geladener Treiber wird durch eine definierte Treiberinstanz repräsentiert.

```text
Driver
├── DriverID
├── Version
├── Supported Devices
├── Interfaces
├── State
└── Capabilities
```

Optional:

```text
Dependencies
Resource Requirements
Security Properties
Provider Features
```

## Gerät und Treiber

Geräteidentität und Treiberidentität müssen getrennt bleiben.

```text
Device
   ↓ match
Driver
   ↓ bind
Driver Instance
```

Ein Treiber darf mehrere kompatible Geräteinstanzen verwalten.

Ein Gerät darf abhängig vom Gerätetyp mehrere spezialisierte Treiberkomponenten verwenden.

## Driver Lifecycle

Treiber müssen einen kontrollierten Lifecycle besitzen.

```text
Discovered
   ↓
Matched
   ↓
Loaded
   ↓
Initialized
   ↓
Active
   ↓
Stopping
   ↓
Unloaded
```

Fehlerzustände müssen eindeutig darstellbar sein.

## Device Binding

Die Auswahl eines Treibers darf anhand definierter Eigenschaften erfolgen.

Beispiele:

```text
Device Class
Vendor ID
Device ID
Interface
Firmware Information
Capability Requirements
```

Die Bindung muss nachvollziehbar und introspektierbar sein.

## Driver Interfaces

Treiber stellen standardisierte Interfaces für höhere Schichten bereit.

```text
Driver
├── Storage Provider
├── Network Provider
├── Audio Provider
├── Display Provider
└── Generic Device Interface
```

Subsysteme sollen nach Möglichkeit gegen das standardisierte Interface und nicht gegen konkrete Treiber implementiert werden.

## I/O Integration

Treiber müssen in das NovaOS-I/O-Modell integrierbar sein.

```text
IORequest
   ↓
I/O Scheduler
   ↓
Driver
   ↓
Hardware
   ↓
Completion
```

Treiber dürfen Requests intern in gerätespezifische Operationen übersetzen.

## Ressourcen

Treiber dürfen Hardware-Ressourcen nur über kontrollierte Systemmechanismen erhalten.

Beispiele:

```text
MMIO
I/O Ports
Interrupts
DMA
Memory
Firmware Interfaces
```

Direkter Zugriff auf nicht zugewiesene Ressourcen ist nicht zulässig.

## DMA und IOMMU

DMA-fähige Treiber müssen die NovaOS-DMA- und IOMMU-Infrastruktur verwenden.

```text
Driver
   ↓
DMA Mapping
   ↓
IOMMU
   ↓
Device
```

DMA-Zugriffe müssen auf die benötigten Speicherbereiche begrenzt werden.

## Fehlerbehandlung

Treiberfehler dürfen das Gesamtsystem möglichst nicht unkontrolliert blockieren.

Treiber müssen Fehler wie:

```text
Device Timeout
Device Removed
Invalid Response
DMA Failure
Initialization Failure
```

an höhere Schichten melden können.

Unbegrenztes Warten auf Hardware ist nicht zulässig.

## Hotplug

Hotplug-fähige Geräte müssen dynamisch eingebunden und entfernt werden können.

```text
Device Added
   ↓
Discover → Match → Bind

Device Removed
   ↓
Stop → Complete/Cancel I/O → Unbind
```

Ein unerwartetes Entfernen darf keine dauerhaft hängenden I/O-Requests erzeugen.

## Live Evolution

Treiber sollen austauschbar oder aktualisierbar sein, sofern Gerät und Systemzustand dies sicher erlauben.

```text
Old Driver
   ↓
Quiesce
   ↓
State Transfer
   ↓
New Driver
   ↓
Verify
```

Ein sicherer Fallback muss möglich bleiben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
Version
State
Bound Devices
Interfaces
Resources
Pending I/O
Error State
Capabilities
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Treibermodell bereitstellen.
2. Geräte- und Treiberidentität MÜSSEN getrennt bleiben.
3. Treiber MÜSSEN einen definierten Lifecycle besitzen.
4. Device Binding MUSS anhand definierter Geräteinformationen erfolgen.
5. Treiber SOLLEN standardisierte Subsystem-Interfaces bereitstellen.
6. Treiber MÜSSEN in das NovaOS-`IORequest`- und Completion-Modell integrierbar sein.
7. Hardware-Ressourcen DÜRFEN nur über autorisierte Systemmechanismen verwendet werden.
8. DMA-fähige Treiber MÜSSEN die definierte DMA-/IOMMU-Infrastruktur verwenden.
9. Treiber DÜRFEN NICHT unbegrenzt auf Hardwareantworten warten.
10. Hotplug-fähige Treiber MÜSSEN kontrolliertes Entfernen unterstützen.
11. Treiber SOLLEN kontrolliert aktualisierbar oder ersetzbar sein.
12. Treiberzustand, Ressourcen und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `ADR-DRIVER-0001`

## Ergebnis

```text
Hardware
   ↕
Driver
   ↕
Standardized Interface
   ↕
NovaOS Subsystem
```

NovaOS erhält damit ein einheitliches Treibermodell, das Hardwaredetails kapselt und Treiber kontrolliert in I/O, Ressourcenverwaltung, Hotplug und Live Evolution integriert.