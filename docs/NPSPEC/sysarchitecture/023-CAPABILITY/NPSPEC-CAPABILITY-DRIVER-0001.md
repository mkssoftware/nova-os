# NPSPEC-CAPABILITY-DRIVER-0001 – Nova Driver Capability

## Status

Angenommen

## Kategorie

Capability / Driver / Security

## Zweck

NovaOS definiert den Zugriff von Treibern auf Hardware und Systemressourcen vollständig über explizite Capabilities.

```text
Driver
  ↓
Driver Capability Set
  ↓
Authorized Hardware Resources
```

Ein Treiber erhält ausschließlich die Autorität, die für die Verwaltung seiner zugewiesenen Geräte erforderlich ist.

## Grundprinzipien

```text
Driver Identity ≠ Driver Authority
Device Discovery ≠ Device Access
Driver Loaded ≠ Hardware Access
Device Capability ≠ Global Hardware Access
DMA Capability ≠ Physical Memory Access
Interrupt Capability ≠ Interrupt Controller Authority
Kernel Driver ≠ Unlimited Authority
Trusted Driver ≠ Unlimited Authority
```

## Driver Capability Domain

Jede Treiberinstanz besitzt einen expliziten Capability-Kontext.

```text
DriverCapabilityDomain
├── Driver Identity
├── Device Capabilities
├── Memory Capabilities
├── DMA Capabilities
├── Interrupt Capabilities
├── I/O Capabilities
└── Service Capabilities
```

Nicht benötigte Autorität darf nicht bereitgestellt werden.

## Device Capability

Hardwaregeräte werden über stabile Geräteidentitäten referenziert.

```text
Driver
  ↓
Device Capability
  ↓
DeviceID
```

Eine Device Capability kann Rechte enthalten wie:

```text
Configure
Read
Write
Reset
PowerControl
QueryState
```

Der Zugriff auf ein Gerät erzeugt keine Autorität über andere Geräte.

## MMIO und Port I/O

Direkter Hardwarezugriff muss explizit autorisiert werden.

```text
MMIO Capability
├── Address Range
└── Rights

PortIO Capability
├── Port Range
└── Rights
```

Treiber dürfen außerhalb dieser Bereiche keine Hardwarezugriffe durchführen.

## DMA

DMA benötigt eine eigene Capability.

```text
Driver
  ↓
DMA Capability
  ↓
IOMMU Mapping
  ↓
Authorized Memory
```

Dabei gilt:

```text
DMA Authority ⊆ Explicitly Mapped Memory
```

Eine DMA-fähige Hardware darf keinen allgemeinen Zugriff auf physischen Speicher erhalten.

## Interrupts

Interrupt-Zugriff wird separat autorisiert.

```text
Interrupt Capability
├── Interrupt Source
├── Delivery Target
└── Allowed Operations
```

Ein Treiber darf nur die ihm zugewiesenen Interrupt-Ressourcen kontrollieren.

## Driver Framework

Das Nova Driver Framework stellt Capabilities beim Binden eines Treibers bereit.

```text
Device Discovery
      ↓
Driver Match
      ↓
Policy Validation
      ↓
Capability Construction
      ↓
Driver Start
```

Discovery selbst erzeugt keine Autorität.

## User-Mode Driver

User-Mode-Treiber verwenden dieselben Capability-Prinzipien.

```text
User Driver
   ↓
Capability Sandbox
   ↓
Device / DMA / Interrupt Capabilities
```

Der Kernel vermittelt privilegierte Hardwareoperationen.

## Kernel-Mode Driver

Auch Kernel-Mode-Treiber sollen logisch auf explizite Capability-Sets begrenzt werden.

```text
Kernel Mode ≠ Unlimited Driver Authority
```

Architekturen mit technischer Durchsetzbarkeit sollen diese Grenzen zusätzlich durch Memory Protection, IOMMU und isolierte Driver Domains absichern.

## Hotplug

Bei Geräteentfernung:

```text
Device Removed
      ↓
Device Capability Revoked
      ↓
Dependent Capabilities Re-Evaluated
      ↓
Driver Notified
```

Ein Treiber darf nach Entfernung des Geräts keine stale Autorität weiterverwenden.

## Live Replacement

Bei Driver Live Replacement werden Capabilities kontrolliert übertragen.

```text
Old Driver
    ↓
Prepare Replacement
    ↓
Validate New Driver
    ↓
Capability Transfer
    ↓
Switch
    ↓
Revoke Old Driver
```

Die neue Treiberinstanz darf keine zusätzliche Autorität durch den Austausch erhalten.

## Delegation

Treiber dürfen Capabilities nur delegieren, wenn dies ausdrücklich erlaubt ist.

Beispielsweise kann ein Storage-Treiber eine eingeschränkte I/O-Capability an einen Storage-Service weitergeben.

```text
Driver Capability
      ↓
Attenuation
      ↓
Delegated Capability
```

Es gilt:

```text
Authority(Delegated)
⊆ Authority(Driver)
```

## Fehlerisolation

Ein kompromittierter oder fehlerhafter Treiber soll maximal auf sein Capability Set zugreifen können.

```text
Faulty Driver
     ↓
Capability Boundary
     ↓
Bounded Hardware Authority
```

Dies ist insbesondere für User-Mode- und isolierte Treiber verpflichtendes Architekturziel.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Driver Identity
Bound DeviceIDs
Device Capabilities
MMIO / PortIO Ranges
DMA Authority
Interrupt Authority
Delegated Capabilities
Revocation State
```

## Normative Anforderungen

1. NovaOS MUSS Hardwareautorität über explizite Driver Capabilities modellieren.
2. Device Discovery DARF keine Device Authority erzeugen.
3. Geräte-, DMA-, Interrupt- und Memory-Autorität MÜSSEN getrennt kontrollierbar sein.
4. DMA MUSS auf explizit autorisierte Speicherbereiche begrenzbar sein.
5. Treiber DÜRFEN keine implizite Autorität über nicht zugewiesene Geräte erhalten.
6. User-Mode-Treiber MÜSSEN capability-basiert isolierbar sein.
7. Kernel-Mode-Treiber SOLLEN logisch nach denselben Capability-Regeln arbeiten.
8. Hotplug MUSS Capability Revocation auslösen können.
9. Live Replacement DARF keine zusätzliche Autorität erzeugen.
10. Driver Capability Sets MÜSSEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-CAPABILITY-SANDBOX-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-USERMODE-0001`
- `NPSPEC-DRIVER-LIVEREPLACE-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-CAPABILITY-0013`

## Ergebnis

```text
Hardware
   ↓
Device Identity
   ↓
Explicit Driver Capabilities
   ↓
Isolated Driver Domain
   ↓
Controlled Hardware Access
```

NovaOS erhält damit ein capability-basiertes Treibermodell, bei dem selbst systemnahe Komponenten nur explizit zugewiesene Hardware-, Speicher-, DMA- und Interrupt-Autorität erhalten und Treiberfehler möglichst auf diesen Autoritätsraum begrenzt bleiben.