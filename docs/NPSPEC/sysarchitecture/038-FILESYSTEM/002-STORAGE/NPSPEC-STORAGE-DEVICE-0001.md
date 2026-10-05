# NPSPEC-STORAGE-DEVICE-0001 – Nova Storage Device Model

## Status

Angenommen

## Kategorie

Storage / Device / Hardware Abstraction

## Zweck

NovaOS definiert ein einheitliches Modell für physische und virtuelle Speichergeräte.

Die Storage-Schicht trennt dabei konsequent:

```text
Device
≠
Partition
≠
Filesystem
≠
Volume
≠
Mountpoint
≠
Namespace
```

Ein Storage Device beschreibt die zugrunde liegende Speicherressource. Dateisysteme, Volumes und Namespace-Projections werden darüber in getrennten Schichten aufgebaut.

## Grundmodell

```text
Physical / Virtual Device
          ↓
      Storage Device
          ↓
    Partition / Region
          ↓
      Filesystem
          ↓
        Volume
          ↓
       Namespace
```

Nicht jede Ebene muss vorhanden sein.

Ein Storage Device kann beispielsweise direkt ein Dateisystem enthalten oder mehrere Partitionen bereitstellen.

## Device Identity

Jedes erkannte Speichergerät erhält eine stabile NovaOS-Geräteidentität:

```text
DeviceID
```

Die `DeviceID` ist unabhängig von:

```text
/dev/sda
/dev/nvme0n1
USB Port
Controller Port
Mountpoint
Volume Name
Filesystem Path
```

Temporäre Hardwarepfade dürfen nicht als dauerhafte Geräteidentität verwendet werden.

## Device Model

```text
StorageDevice
├── DeviceID
├── DeviceType
├── State
├── Capacity
├── BlockSize
├── AccessMode
└── Capabilities
```

Optional:

```text
Vendor
Model
Serial
Firmware
ControllerID
ParentDeviceID
Transport
PhysicalSectorSize
LogicalSectorSize
Alignment
Removable
Hotplug
ReadOnly
HealthState
EncryptionState
PerformanceProfile
ProvenanceID
```

## Gerätetypen

NovaOS muss unterschiedliche Storage-Technologien über dasselbe logische Modell darstellen können.

Beispiele:

```text
HDD
SSD
NVMe
USB Storage
SD / eMMC
Optical
RAM Disk
Virtual Disk
Network-backed Block Device
Firmware Storage
```

Technologiespezifische Funktionen bleiben über Device Capabilities verfügbar.

## Hierarchische Geräte

Storage Devices dürfen andere Devices enthalten oder bereitstellen.

```text
Physical NVMe
     ↓
Namespace
     ↓
Block Device
```

oder:

```text
Physical Disk
     ↓
Encrypted Device
     ↓
Logical Device
```

Dafür kann `ParentDeviceID` verwendet werden.

Die Geräteidentitäten der einzelnen Ebenen bleiben getrennt.

## Discovery

Storage Devices werden durch passende Treiber erkannt und registriert.

```text
Hardware
   ↓
Driver
   ↓
Storage Discovery
   ↓
DeviceID
   ↓
Storage Registry
```

Discovery gewährt noch keine Zugriffsberechtigung.

```text
Discovered ≠ Authorized
```

## Device Capabilities

Geräte können ihre unterstützten Funktionen deklarieren.

Beispiele:

```text
Read
Write
Flush
Discard
SecureErase
SMART
TRIM
Hotplug
PowerManagement
FirmwareUpdate
AtomicWrite
WriteBarrier
```

Höhere Storage-Schichten dürfen Funktionen nur voraussetzen, wenn sie vom Gerät oder einer darüberliegenden Abstraktionsschicht garantiert werden.

## I/O

Blockorientierte Geräte stellen Operationen über eine einheitliche Storage-I/O-Schnittstelle bereit.

```text
Request
├── DeviceID
├── Operation
├── Offset / Block
├── Length
├── Buffer
├── Priority
└── Deadline
```

Der I/O-Pfad kann asynchron arbeiten.

```text
Request
   ↓
Storage Scheduler
   ↓
Driver
   ↓
Device
   ↓
Completion
```

Zero-Copy, DMA, Scatter/Gather und Shared Buffers sollen genutzt werden können, sofern Hardware, Treiber und Sicherheitsmodell dies erlauben.

## Zustände

Ein Device kann mindestens folgende Zustände besitzen:

```text
Discovered
Initializing
Ready
Degraded
ReadOnly
Unavailable
Removing
Removed
Failed
Unknown
```

`Unknown` darf nicht als `Ready` interpretiert werden.

## Hotplug

Entfernbare Geräte müssen kontrolliert eingebunden und entfernt werden können.

```text
Device Attached
      ↓
Discover
      ↓
Validate
      ↓
Register
      ↓
Ready
```

Entfernung:

```text
Removal Requested
       ↓
Block New Operations
       ↓
Drain / Flush
       ↓
Detach Volumes
       ↓
Remove Device
```

Ein unerwartetes Entfernen muss als Fehlerzustand behandelt werden.

## Health

Falls vom Gerät unterstützt, kann NovaOS Gesundheitsinformationen erfassen:

```text
Temperature
Wear
Media Errors
SMART Data
Remaining Lifetime
Controller Errors
```

Diese Informationen können durch Monitoring, Self-Healing und Predictive Maintenance genutzt werden.

Ein Health-Wert darf jedoch nicht automatisch als Garantie für Datenintegrität interpretiert werden.

## Fehlerbehandlung

Storage-Fehler müssen strukturiert weitergegeben werden.

Beispiele:

```text
Timeout
MediaError
ControllerError
DeviceRemoved
ReadFailure
WriteFailure
IntegrityFailure
UnsupportedOperation
ResourceUnavailable
```

Fehler dürfen nicht stillschweigend als erfolgreiche Operation behandelt werden.

## Device vs. Volume

Ein Device ist keine Benutzer-Volume-Identität.

```text
DeviceID
   ↓
Storage Resource

VolumeID
   ↓
Logical Storage Volume
```

Ein Device kann:

```text
0..n Volumes
```

bereitstellen.

Ein Volume kann abhängig von der Storage-Architektur auch auf mehreren Devices basieren.

## Namespace

Storage Devices müssen nicht automatisch im normalen Benutzer-Namespace sichtbar sein.

Administrative oder diagnostische Projections dürfen Geräte beispielsweise unter einer kontrollierten Ansicht darstellen.

```text
Device Registry
      ↓
Authorized Projection
      ↓
Storage Management UI
```

Eine solche Projection verändert weder `DeviceID` noch Authority.

## Capability Security

Gerätezugriff erfolgt capability-basiert.

```text
Process
   ↓
Device Capability
   ↓
Authorized Handle
   ↓
Storage Operation
```

Rechte können beispielsweise getrennt werden:

```text
Read
Write
Flush
Discard
HealthRead
Format
SecureErase
FirmwareUpdate
RawAccess
```

Insbesondere `RawAccess`, `Format`, `SecureErase` und `FirmwareUpdate` benötigen eigenständige starke Authority.

```text
Filesystem Access
≠
Raw Device Access
```

## Resource Economy

Storage-I/O muss in NovaOS-Ressourcensteuerung integrierbar sein.

Berücksichtigt werden können:

```text
Bandwidth
IOPS
Queue Depth
Latency
Memory
DMA Resources
Energy
Thermal State
```

Storage Scheduler und Execution Contracts können diese Informationen für Priorisierung und QoS verwenden.

## Introspection

Autorisierte Komponenten sollen mindestens abfragen können:

```text
DeviceID
DeviceType
State
Capacity
Block Size
Transport
Capabilities
Health
Performance
Parent Device
Attached Volumes
Driver
```

Introspection darf keine zusätzliche Device Authority erzeugen.

## Normative Anforderungen

1. Jedes registrierte Storage Device MUSS eine eindeutige `DeviceID` besitzen.
2. `DeviceID` MUSS unabhängig von temporären Hardwarepfaden sein.
3. Device, Filesystem, Volume und Namespace MÜSSEN getrennte Konzepte bleiben.
4. NovaOS MUSS physische und virtuelle Storage Devices über ein gemeinsames Modell darstellen können.
5. Hierarchische Storage Devices MÜSSEN unterstützt werden können.
6. Device Discovery DARF KEINE Zugriffsberechtigung erzeugen.
7. Gerätefunktionen MÜSSEN als Capabilities beziehungsweise unterstützte Features beschreibbar sein.
8. Höhere Schichten DÜRFEN nicht unterstützte Hardwarefunktionen NICHT voraussetzen.
9. Storage-I/O MUSS asynchron ausführbar sein können.
10. Zero-Copy und DMA SOLLEN verwendet werden können, sofern sicher und unterstützt.
11. Device-Zustände MÜSSEN explizit darstellbar sein.
12. `Unknown` DARF NICHT als `Ready` interpretiert werden.
13. Hotplug MUSS kontrolliert behandelt werden können.
14. Unerwartete Device-Entfernung MUSS als Fehlerzustand erkennbar sein.
15. Storage-Fehler MÜSSEN strukturiert weitergegeben werden.
16. Fehlgeschlagene I/O-Operationen DÜRFEN NICHT stillschweigend als erfolgreich gelten.
17. Device Health SOLL introspektierbar sein, sofern vom Gerät unterstützt.
18. Filesystem-Zugriff DARF NICHT automatisch Raw-Device-Zugriff gewähren.
19. Kritische Device-Operationen MÜSSEN gesondert autorisiert werden.
20. Device-Zugriffe MÜSSEN capability-basiert kontrollierbar sein.
21. Storage Devices MÜSSEN in Resource Accounting und QoS integrierbar sein.
22. Device Identity MUSS bei Namespace- oder Mountpoint-Änderungen stabil bleiben.
23. Administrative Device-Projections DÜRFEN keine zusätzliche Authority erzeugen.
24. Device State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-CAPABILITY-DRIVER-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`

## Ergebnis

```text
Physical / Virtual Storage
           ↓
        Driver
           ↓
      Storage Device
           ↓
        DeviceID
           ↓
 ┌─────────┼──────────┐
 ↓         ↓          ↓
Health   Device I/O  Capabilities
           ↓
    Partition / Volume
           ↓
       Filesystem
           ↓
       Namespace
```

NovaOS erhält damit eine stabile, hardwareunabhängige Storage-Device-Schicht. Physische Speichergeräte, virtuelle Datenträger und daraus aufgebaute logische Storage-Strukturen bleiben eindeutig getrennt, während ein gemeinsames Device-Modell Discovery, I/O, Hotplug, Health Monitoring, Ressourcensteuerung und capability-basierten Zugriff bereitstellt.