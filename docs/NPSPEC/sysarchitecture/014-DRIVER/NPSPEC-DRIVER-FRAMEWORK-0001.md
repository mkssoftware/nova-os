# NPSPEC-DRIVER-FRAMEWORK-0001 – Nova Driver Framework

## Status

Entwurf

## Kategorie

Driver / Framework / Kernel

## Zweck

NovaOS definiert ein gemeinsames Driver Framework für Registrierung, Laden, Bindung, Lifecycle und Kommunikation aller Treiber.

```text
Driver Package
     ↓
Driver Framework
     ├── Registration
     ├── Device Matching
     ├── Lifecycle
     ├── Resource Access
     └── I/O Integration
```

Das Framework stellt gemeinsame Mechanismen bereit, damit einzelne Treiber keine eigene Infrastruktur für grundlegende Systemfunktionen implementieren müssen.

## Grundprinzip

```text
Framework = Mechanism
Driver = Device-specific Implementation
Subsystem = Policy
```

Das Driver Framework koordiniert Treiber, ohne gerätespezifische Logik selbst zu übernehmen.

## Driver Registration

Treiber müssen sich mit einer definierten Beschreibung registrieren.

```text
DriverDescriptor
├── DriverID
├── Version
├── Device Matches
├── Interfaces
├── Dependencies
└── Capabilities
```

Die Registrierung darf statisch beim Boot oder dynamisch zur Laufzeit erfolgen.

## Device Matching

Das Framework ordnet erkannte Geräte passenden Treibern zu.

```text
Device Discovery
      ↓
Device Information
      ↓
Driver Matching
      ↓
Driver Binding
```

Matching-Regeln dürfen unter anderem verwenden:

```text
Device Class
Vendor ID
Device ID
Interface ID
Firmware Data
Required Capabilities
```

Mehrere passende Treiber müssen durch eine definierte Auswahlregel behandelt werden.

## Driver Lifecycle

Das Framework verwaltet den grundlegenden Treiber-Lifecycle.

```text
Register
   ↓
Load
   ↓
Initialize
   ↓
Bind
   ↓
Active
   ↓
Stop
   ↓
Unload
```

Fehler während einer Phase müssen kontrolliert behandelt werden und dürfen keine halbinitialisierten Ressourcen zurücklassen.

## Ressourcenverwaltung

Treiber fordern benötigte Ressourcen über das Framework beziehungsweise die zuständigen Kernel-Subsysteme an.

```text
Driver
   ↓
Resource Request
   ↓
Framework / HAL
   ├── MMIO
   ├── I/O Ports
   ├── Interrupts
   ├── DMA
   └── Memory
```

Ressourcen müssen einer konkreten Treiber- oder Geräteinstanz zugeordnet werden.

Beim Entfernen des Treibers müssen sie kontrolliert freigegeben werden.

## I/O Integration

Das Framework verbindet Treiber mit dem NovaOS-I/O-Modell.

```text
IORequest
   ↓
Driver Framework
   ↓
Driver
   ↓
Device
   ↓
Completion
```

Das Framework darf gemeinsame Funktionen für Request-Tracking, Cancellation, Timeout und Completion bereitstellen.

## Interrupts und DMA

Treiber müssen Interrupt- und DMA-Infrastruktur über definierte NovaOS-Schnittstellen verwenden.

```text
Device
 ├── Interrupt → Interrupt Subsystem
 └── DMA       → DMA / IOMMU
```

Das Framework darf Hilfsabstraktionen bereitstellen, aber die jeweiligen HAL- und Kernel-Regeln nicht umgehen.

## Abhängigkeiten

Treiber dürfen andere Treiber oder Systemdienste voraussetzen.

```text
Driver A
   ↓ requires
Driver B
```

Abhängigkeiten müssen vor Aktivierung validiert werden.

Zyklische oder nicht erfüllbare Abhängigkeiten müssen erkannt werden.

## Hotplug

Das Framework muss dynamische Geräteereignisse unterstützen können.

```text
Add
 ↓
Discover → Match → Load → Bind

Remove
 ↓
Quiesce → Cancel/Complete I/O → Unbind → Release
```

Ein unerwartetes Entfernen muss in einen definierten Fehlerzustand überführt werden.

## Fehlerisolation

Ein Treiberfehler soll möglichst auf den betroffenen Treiber oder das betroffene Gerät begrenzt bleiben.

Das Framework muss mindestens erkennen können:

```text
Initialization Failure
Timeout
Device Failure
Invalid State
Resource Failure
Unexpected Removal
```

Unbegrenzte Retry- oder Wait-Schleifen sind nicht zulässig.

## Live Evolution

Das Framework soll kontrollierten Treiberaustausch unterstützen.

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

Ein fehlgeschlagener Austausch muss einen sicheren Fallback ermöglichen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Registered Drivers
Loaded Drivers
Driver Version
Bound Devices
Dependencies
Resources
Pending I/O
Lifecycle State
Error State
```

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Driver Framework bereitstellen.
2. Treiber MÜSSEN sich über eine definierte Driver Description registrieren.
3. Das Framework MUSS Device Matching und Binding koordinieren können.
4. Treiber MÜSSEN einen kontrollierten Lifecycle durchlaufen.
5. Ressourcen MÜSSEN einer konkreten Treiber- oder Geräteinstanz zugeordnet werden.
6. Fehlgeschlagene Initialisierung MUSS bereits reservierte Ressourcen kontrolliert freigeben.
7. Das Framework MUSS Treiber in das NovaOS-I/O-Modell integrieren können.
8. Treiber DÜRFEN Interrupt-, DMA- oder Hardware-Sicherheitsmechanismen NICHT umgehen.
9. Treiberabhängigkeiten MÜSSEN vor Aktivierung validierbar sein.
10. Hotplug und unerwartetes Entfernen MÜSSEN kontrolliert behandelbar sein.
11. Unbegrenzte Hardware-Warte- oder Retry-Schleifen DÜRFEN NICHT verwendet werden.
12. Framework-, Treiber- und Binding-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `ADR-DRIVER-0002`

## Ergebnis

```text
Devices
   ↓
Driver Framework
   ├── Match
   ├── Bind
   ├── Lifecycle
   ├── Resources
   └── I/O
        ↓
      Drivers
```

NovaOS erhält damit eine gemeinsame Treiberinfrastruktur, die Device Binding, Lifecycle, Ressourcen und I/O zentral koordiniert, während gerätespezifische Implementierungen modular bleiben.