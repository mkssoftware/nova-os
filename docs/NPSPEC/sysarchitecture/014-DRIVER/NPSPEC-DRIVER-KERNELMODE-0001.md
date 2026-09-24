# NPSPEC-DRIVER-KERNELMODE-0001 – Nova Kernel-Mode Drivers

## Status

Entwurf

## Kategorie

Driver / Kernel Mode / Security

## Zweck

NovaOS definiert Kernel-Mode-Treiber für Hardware und Funktionen, die privilegierten Zugriff auf Kernel-, Interrupt-, DMA- oder Hardware-Ressourcen benötigen.

```text
NovaOS Kernel
     ↓
Kernel Driver
     ↓
HAL
     ↓
Hardware
```

Kernel-Mode ist ausschließlich für Treiber vorgesehen, deren Anforderungen nicht sicher oder effizient durch weniger privilegierte Ausführungsmodelle erfüllt werden können.

## Grundprinzip

```text
Kernel Mode = Privileged
Kernel Mode ≠ Unlimited Authority
Driver Privilege ≠ Universal Kernel Access
```

Auch Kernel-Treiber sollen nur die Ressourcen und Kernel-Schnittstellen erhalten, die sie tatsächlich benötigen.

## Einsatzbereich

Kernel-Mode darf insbesondere erforderlich sein für:

```text
Interrupt Handling
DMA
MMIO
I/O Ports
Low-Level Bus Access
Boot-critical Devices
Timing-critical Hardware
```

Treiber sollen nicht allein aus Gründen der Einfachheit im Kernel ausgeführt werden.

## Privilegien

Kernel-Mode-Treiber dürfen privilegierte Operationen ausführen, müssen jedoch durch das Driver Framework kontrolliert werden.

```text
Driver
   ↓
Declared Requirements
   ↓
Validation
   ↓
Granted Resources
```

Hardwarezugriff soll möglichst über HAL und definierte Kernel-APIs erfolgen.

## Ressourcen

Ein Kernel-Treiber darf nur explizit zugewiesene Ressourcen verwenden.

Beispiele:

```text
MMIO Range
I/O Port Range
Interrupt
DMA Context
Memory Region
Device Instance
```

Nicht zugewiesene Hardware- oder Speicherbereiche dürfen nicht verwendet werden.

## Speicher

Kernel-Treiber arbeiten im privilegierten Adressraum und müssen daher besonders strikte Speicherregeln einhalten.

```text
Driver Memory
├── Code
├── Read-only Data
├── Private Data
├── DMA Buffers
└── Shared Buffers
```

Ungültige Pointer, Buffer Overflows oder Use-after-Free können die Systemintegrität gefährden und müssen soweit möglich verhindert oder erkannt werden.

## I/O

Kernel-Treiber müssen das gemeinsame NovaOS-I/O-Modell verwenden.

```text
IORequest
   ↓
Driver Framework
   ↓
Kernel Driver
   ↓
Hardware
   ↓
Completion
```

Direkte Sonderpfade sollen nur verwendet werden, wenn dies technisch erforderlich und ausdrücklich definiert ist.

## Interrupts

Interrupt Handler müssen kurz und begrenzt bleiben.

```text
Interrupt
   ↓
Minimal Handler
   ↓
Deferred Work
```

Aufwendige Verarbeitung soll in Deferred Work oder geeignete Tasks verschoben werden.

Unbegrenztes Warten innerhalb eines Interrupt-Kontexts ist nicht zulässig.

## DMA

DMA-fähige Kernel-Treiber müssen die NovaOS-DMA- und IOMMU-Infrastruktur verwenden.

```text
Driver
   ↓
DMA Mapping
   ↓
IOMMU
   ↓
Device
```

Geräte dürfen nur Zugriff auf die für ihre Operation freigegebenen Speicherbereiche erhalten.

## Fehlerbehandlung

Ein Kernel-Treiber darf bei Hardwarefehlern nicht unbegrenzt warten.

Mindestens folgende Zustände müssen behandelbar sein:

```text
Timeout
Device Failure
DMA Failure
Invalid Response
Device Removed
Resource Failure
```

Fehler sollen an Driver Framework und zuständige Subsysteme weitergegeben werden.

## Laden und Vertrauen

Kernel-Treiber müssen vor Aktivierung überprüft werden können.

```text
Driver Image
   ↓
Identity / Integrity / Signature
   ↓
Policy Check
   ↓
Load
```

Nicht vertrauenswürdige oder manipulierte Kernel-Treiber dürfen nicht regulär geladen werden.

## Live Evolution

Kernel-Treiber sollen kontrolliert aktualisierbar sein, wenn ihr Zustand sicher übertragen werden kann.

```text
Active
  ↓
Quiesce
  ↓
State Transfer
  ↓
Replace
  ↓
Verify
```

Bootkritische oder nicht sicher austauschbare Treiber dürfen einen Neustart verlangen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
Version
Kernel State
Bound Devices
Granted Resources
Interrupts
DMA Mappings
Pending I/O
Error State
Trust State
```

## Normative Anforderungen

1. NovaOS MUSS Kernel-Mode-Treiber für privilegierte Hardwarefunktionen unterstützen.
2. Kernel-Mode DARF NICHT automatisch unbegrenzte Ressourcenberechtigung bedeuten.
3. Treiber SOLLEN nur die tatsächlich benötigten Ressourcen erhalten.
4. Hardwarezugriff SOLL über definierte HAL- und Kernel-Schnittstellen erfolgen.
5. Kernel-Treiber MÜSSEN in das Driver Framework integriert sein.
6. Kernel-Treiber MÜSSEN das gemeinsame I/O-Modell verwenden können.
7. Interrupt Handler MÜSSEN begrenzt bleiben und SOLLEN aufwendige Arbeit verschieben.
8. DMA-fähige Treiber MÜSSEN die definierte DMA-/IOMMU-Infrastruktur verwenden.
9. Kernel-Treiber DÜRFEN NICHT unbegrenzt auf Hardware reagieren oder warten.
10. Kernel-Treiber MÜSSEN vor Aktivierung auf Integrität und Trust-Policy prüfbar sein.
11. Sicher austauschbare Kernel-Treiber SOLLEN Live Evolution unterstützen.
12. Privilegien, Ressourcen, Zustand und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `ADR-DRIVER-0003`

## Ergebnis

```text
Kernel Driver
     ↓
Controlled Privileges
     ↓
HAL / I/O / DMA / Interrupts
     ↓
Hardware
```

NovaOS erhält damit ein kontrolliertes Kernel-Treibermodell für hardwarekritische Funktionen, ohne Kernel-Mode mit unbegrenzter Autorität gleichzusetzen.