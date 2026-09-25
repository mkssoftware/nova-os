# NPSPEC-DRIVER-CAPABILITY-0001 – Nova Driver Capabilities

## Status

Angenommen

## Kategorie

Driver / Capability / Security

## Zweck

NovaOS definiert ein Capability-Modell für Treiber, über das Zugriffe auf Geräte, Hardware-Ressourcen und Systemdienste explizit autorisiert werden.

```text
Driver
   ↓
Capability
   ↓
Authorized Resource
```

Treiber erhalten keine implizite Systemautorität allein durch Laden, Gerätebindung oder Kernel-Nähe.

## Grundprinzip

```text
Driver Identity ≠ Permission
Device Binding ≠ Full Device Authority
Capability ≠ Resource
Possession of Reference ≠ Access
```

Jeder privilegierte Zugriff muss durch eine passende Capability oder eine gleichwertige explizite Autorisierung gedeckt sein.

## Capability-Typen

Treiber-Capabilities dürfen unter anderem folgende Ressourcen kontrollieren:

```text
DeviceAccess
MMIOAccess
PortAccess
InterruptAccess
DMAAccess
SharedBufferAccess
FirmwareAccess
IPCServiceAccess
PowerControl
DeviceReset
```

Capabilities sollen möglichst fein auf die tatsächlich benötigte Funktion begrenzt werden.

## Capability Scope

Eine Capability muss ihren Gültigkeitsbereich eindeutig beschreiben können.

```text
Capability
├── Resource
├── Operations
├── Scope
└── Constraints
```

Beispiele:

```text
MMIO: Read/Write → Range 0x1000–0x1FFF
DMA: Read        → Buffer A
Interrupt        → IRQ Endpoint B
DeviceReset      → Device C
```

Eine Capability für eine Ressource darf keine automatische Berechtigung für andere Ressourcen erzeugen.

## Vergabe

Capabilities werden durch vertrauenswürdige NovaOS-Komponenten vergeben.

```text
Driver Requirements
        ↓
Policy Validation
        ↓
Capability Grant
        ↓
Driver
```

Angeforderte und tatsächlich gewährte Berechtigungen müssen getrennt bleiben.

Ein Treiber darf zusätzliche Capabilities nicht selbst erzeugen.

## Weitergabe

Capabilities dürfen nur kontrolliert übertragen werden.

```text
Driver A
   ↓ delegated capability
Driver B
```

Eine Delegation muss einschränkbar sein.

Eine abgeleitete Capability darf nicht mehr Rechte besitzen als ihre Quelle.

## Widerruf

NovaOS muss Capabilities entziehen können.

```text
Active Capability
       ↓
Revoke
       ↓
Access Denied
```

Widerruf kann insbesondere erforderlich sein bei:

```text
Driver Stop
Device Removal
Sandbox Violation
Driver Failure
Security Policy Change
Device Rebinding
```

Laufende Operationen müssen dabei kontrolliert abgeschlossen oder abgebrochen werden.

## DMA

DMA benötigt eine explizite Capability und ein gültiges Mapping.

```text
DMA Capability
      +
Buffer Mapping
      ↓
IOMMU
      ↓
Device
```

Eine allgemeine Geräte-Capability darf nicht automatisch uneingeschränkten DMA-Zugriff erlauben.

## Interrupts

Interrupt-Zugriff muss an definierte Interrupt-Ressourcen gebunden sein.

```text
Interrupt Capability
        ↓
IRQ / MSI Endpoint
        ↓
Driver
```

Ein Treiber darf Interrupt-Ressourcen anderer Geräte nicht ohne entsprechende Capability verändern.

## Driver Framework

Das Driver Framework verwaltet die Zuordnung zwischen:

```text
Driver
Device
Capabilities
Resources
Lifecycle
```

Beim Unbinding oder Unload müssen nicht mehr benötigte Capabilities entzogen werden.

## Kernel- und User-Mode

Dasselbe logische Capability-Modell soll für Kernel- und User-Mode-Treiber gelten.

```text
Kernel Driver ─┐
               ├→ Capability Model → Resource
User Driver ───┘
```

Die technische Durchsetzung darf abhängig vom Ausführungsmodus unterschiedlich erfolgen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
Capability Type
Resource
Allowed Operations
Constraints
Grant Source
State
Delegation
```

Geheime Capability-Werte oder sicherheitskritische Handles dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Treiberzugriffe durch explizite Capabilities kontrollieren können.
2. Device Binding DARF NICHT automatisch vollständige Geräteautorität erzeugen.
3. Capabilities MÜSSEN auf konkrete Ressourcen und Operationen begrenzbar sein.
4. Treiber DÜRFEN Capabilities NICHT selbstständig erweitern oder erzeugen.
5. Angeforderte und gewährte Capabilities MÜSSEN unterscheidbar bleiben.
6. Delegierte Capabilities DÜRFEN NICHT mehr Rechte als ihre Quelle besitzen.
7. Capabilities MÜSSEN widerrufbar sein.
8. DMA MUSS eine explizite Autorisierung und ein gültiges Mapping erfordern.
9. Interrupt-Zugriffe MÜSSEN auf autorisierte Interrupt-Ressourcen begrenzt sein.
10. Kernel-Mode DARF Capability-Prüfungen NICHT grundsätzlich aufheben.
11. Nicht mehr benötigte Capabilities MÜSSEN beim Driver-Unbinding oder Unload entzogen werden.
12. Capability-Zustände und Zuordnungen SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-KERNELMODE-0001`
- `NPSPEC-DRIVER-USERMODE-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-SANDBOX-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-IO-REQUEST-0001`
- `ADR-DRIVER-0007`

## Ergebnis

```text
Driver
   ↓
Explicit Capability
   ↓
Scoped Authority
   ↓
Resource / Device
```

NovaOS erhält damit ein einheitliches Capability-Modell für Treiber, bei dem Autorität explizit, begrenzt, delegierbar und widerrufbar ist, anstatt allein aus Treibermodus oder Gerätebindung zu entstehen.