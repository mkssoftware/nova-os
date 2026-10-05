# NPSPEC-REGISTRY-DEVICE-0001 – Nova Device Registry

## Status

Angenommen

## Kategorie

Registry / Device

## Zweck

NovaOS definiert die Device Registry als systemweites Verzeichnis erkannter physischer und virtueller Geräte.

Sie verbindet stabile Geräteidentitäten mit Geräteeigenschaften, Treibern, Capabilities und aktuellem Gerätezustand, ohne selbst Hardwarezugriff oder Authority bereitzustellen.

## Grundprinzipien

```text
DeviceID ≠ Device Path
DeviceID ≠ DriverID
Device ≠ Driver
Registration ≠ Authority
Discovery ≠ Access
Device Presence ≠ Device Availability
```

## Registry-Modell

Ein Geräteeintrag kann enthalten:

```text
DeviceRegistryEntry
├── DeviceID
├── DeviceType
├── ParentDeviceID
├── Properties
├── DriverID
├── Capabilities
├── State
└── Generation
```

Optional:

```text
HardwareIdentity
FirmwareInformation
Topology
Health
TrustState
ProviderID
```

Die `DeviceID` bildet die stabile NovaOS-Identität des Geräts.

## Registrierung

Geräte werden durch das Hardware- und Storage-Discovery-System erkannt:

```text
Hardware / Virtual Device
        ↓
Discovery
        ↓
Identify Device
        ↓
Assign / Resolve DeviceID
        ↓
Driver Matching
        ↓
Register
```

Bereits bekannte Geräte sollen ihrer bestehenden stabilen `DeviceID` zugeordnet werden können.

## Gerätehierarchie

Geräte dürfen hierarchische Beziehungen besitzen:

```text
PCI Controller
├── Device A
└── Device B

USB Controller
└── USB Hub
    ├── Device C
    └── Device D
```

Hierfür kann `ParentDeviceID` verwendet werden.

## Zustände

Mindestens folgende Zustände müssen darstellbar sein:

```text
Discovered
Initializing
Ready
Degraded
Unavailable
Removing
Removed
Failed
Unknown
```

`Discovered` bedeutet nicht automatisch, dass das Gerät betriebsbereit ist.

## Treiberbindung

```text
DeviceID
   ↓
Device Properties
   ↓
Driver Matching
   ↓
DriverID
   ↓
Binding
```

Treiberidentität und Geräteidentität bleiben getrennt.

Ein Treiberwechsel verändert nicht automatisch die `DeviceID`.

## Capabilities

Die Registry darf beschreiben, welche Funktionen ein Gerät grundsätzlich unterstützt.

Beispiele:

```text
Read
Write
Audio
Display
Input
Network
Storage
Compute
DMA
Power Management
```

Diese Angaben beschreiben Gerätefähigkeiten und stellen keine erteilten Capability-Tokens dar.

## Hotplug

Geräteänderungen müssen dynamisch registrierbar sein:

```text
Added
Changed
Removed
Unavailable
```

Beim Entfernen eines Geräts müssen Registry-Zustand und abhängige Komponenten kontrolliert aktualisiert werden.

## Sicherheit

Die Device Registry gewährt keinen Hardwarezugriff.

```text
Device Discovery
      ↓
DeviceID
      ↓
Capability Check
      ↓
Authorized Handle
      ↓
Device Operation
```

Kenntnis einer `DeviceID` erzeugt keine Authority.

## Normative Anforderungen

1. NovaOS MUSS eine systemweite Device Registry bereitstellen.
2. Geräte MÜSSEN über stabile `DeviceID`s registrierbar sein.
3. `DeviceID` DARF nicht von Gerätepfad, Port oder Treiber abhängen.
4. Geräte- und Treiberidentität MÜSSEN getrennt bleiben.
5. Physische und virtuelle Geräte MÜSSEN unterstützt werden können.
6. Gerätehierarchien MÜSSEN darstellbar sein.
7. Gerätezustände MÜSSEN explizit abbildbar sein.
8. Hotplug-Ereignisse MÜSSEN die Registry aktualisieren können.
9. Ein Treiberwechsel DARF nicht automatisch eine neue Geräteidentität erzeugen.
10. Die Registry DARF keine aktiven Capability-Tokens enthalten.
11. Discovery und Registrierung DÜRFEN keine Authority erzeugen.
12. Geräteidentität, Typ, Zustand, Treiber und verfügbare Funktionen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-STORAGE-DISCOVERY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine zentrale Registry für physische und virtuelle Geräte. Geräte bleiben über stabile `DeviceID`s unabhängig von Pfaden, Ports und Treibern identifizierbar, während Discovery, Treiberbindung, Hotplug und Capability-basierter Zugriff klar voneinander getrennt bleiben.