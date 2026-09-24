# NPSPEC-DRIVER-USERMODE-0001 – Nova User-Mode Drivers

## Status

Entwurf

## Kategorie

Driver / User Mode / Isolation

## Zweck

NovaOS definiert User-Mode-Treiber für Geräte und Funktionen, die ohne direkten privilegierten Kernelzugriff betrieben werden können.

```text
Application / Subsystem
        ↓
Driver Framework
        ↓
User-Mode Driver
        ↓
Controlled Kernel Interface
        ↓
Hardware
```

User-Mode soll bevorzugt werden, wenn die erforderliche Funktionalität mit akzeptabler Leistung und Latenz außerhalb des Kernels realisierbar ist.

## Grundprinzip

```text
Driver Failure ≠ Kernel Failure
User Mode ≠ Direct Hardware Authority
Isolation ≠ Unlimited IPC
```

Ein User-Mode-Treiber läuft in einer isolierten Ausführungs- und Sicherheitsdomäne und erhält ausschließlich explizit freigegebene Ressourcen.

## Einsatzbereich

User-Mode eignet sich insbesondere für:

```text
USB Devices
Input Devices
Audio Devices
Sensors
Virtual Devices
Protocol Drivers
Non-critical Device Services
```

Bootkritische, extrem latenzkritische oder hardwarebedingt privilegierte Funktionen dürfen weiterhin Kernel-Mode benötigen.

## Isolation

Jeder User-Mode-Treiber soll in einem kontrollierten Prozess- oder Driver-Domain-Kontext ausgeführt werden.

```text
Driver Domain
├── Address Space
├── Capabilities
├── Resource Limits
└── Security Context
```

Ein Fehler darf nicht automatisch Speicher oder Zustand des Kernels oder anderer Treiber beschädigen.

## Hardwarezugriff

User-Mode-Treiber dürfen Hardware nicht unkontrolliert direkt ansprechen.

Benötigte Ressourcen werden kontrolliert bereitgestellt:

```text
MMIO Mapping
Interrupt Endpoint
DMA Mapping
Device Handle
Shared Buffer
```

Die Freigabe muss an die konkrete Treiber- und Geräteinstanz gebunden sein.

## I/O Integration

User-Mode-Treiber müssen in das gemeinsame NovaOS-I/O-Modell integriert werden.

```text
IORequest
   ↓
Driver Framework
   ↓
IPC / Shared Buffer
   ↓
User Driver
   ↓
Device
   ↓
Completion
```

Das Driver Framework soll unnötige Datenkopien vermeiden.

Zero-Copy und Shared Buffers dürfen verwendet werden, wenn Isolation und Ownership eindeutig gewährleistet sind.

## Interrupts

Hardware-Interrupts werden zunächst durch die privilegierte Interrupt-Infrastruktur verarbeitet.

```text
Hardware IRQ
    ↓
Kernel
    ↓
Interrupt Endpoint
    ↓
User Driver
```

User-Mode-Treiber dürfen daraus Ereignisse oder Deferred Work verarbeiten.

## DMA

DMA muss über kontrollierte DMA- und IOMMU-Mechanismen erfolgen.

```text
User Driver
    ↓
DMA Request
    ↓
Kernel / IOMMU
    ↓
Device
```

Ein Gerät darf nur auf explizit freigegebene Speicherbereiche zugreifen.

## Kommunikation

Die Kommunikation mit Kernel und Subsystemen muss über definierte Schnittstellen erfolgen.

Beispiele:

```text
IPC
Shared Buffer
IORequest
Completion Queue
Capability Handle
```

Direkte Abhängigkeiten von internen Kernelstrukturen sind nicht zulässig.

## Fehler und Neustart

Ein abgestürzter User-Mode-Treiber soll unabhängig vom Kernel neu gestartet werden können.

```text
Driver Failure
     ↓
Detect
     ↓
Stop / Cleanup
     ↓
Restart
     ↓
Rebind
```

Ausstehende I/O-Requests müssen dabei kontrolliert abgeschlossen, fehlgeschlagen oder erneut eingereicht werden.

## Ressourcenlimits

User-Mode-Treiber müssen begrenzbare Ressourcen besitzen.

```text
Memory
CPU Time
I/O Queue Depth
DMA Memory
IPC Capacity
```

Ein fehlerhafter Treiber darf dadurch nicht unbegrenzt Systemressourcen verbrauchen.

## Performance

User-Mode darf nicht unnötig zusätzliche Kopien oder Kontextwechsel erzeugen.

NovaOS soll insbesondere verwenden können:

```text
Shared Memory
Zero-Copy
Batching
Completion Queues
Async I/O
```

Falls User-Mode die erforderlichen harten Latenzanforderungen nicht erfüllen kann, darf ein Kernel-Mode-Treiber erforderlich sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
Process / Domain
Bound Devices
Capabilities
Granted Resources
Pending I/O
Resource Usage
Restart Count
Error State
```

## Normative Anforderungen

1. NovaOS MUSS User-Mode-Treiber unterstützen.
2. User-Mode SOLL bevorzugt werden, wenn keine zwingenden Kernel-Mode-Anforderungen bestehen.
3. User-Mode-Treiber MÜSSEN von Kernel und anderen Treibern isoliert sein.
4. Hardware-Ressourcen DÜRFEN nur explizit freigegeben werden.
5. Direkter unkontrollierter Hardwarezugriff DARF NICHT möglich sein.
6. User-Mode-Treiber MÜSSEN in das gemeinsame Driver- und I/O-Modell integrierbar sein.
7. DMA MUSS über kontrollierte DMA-/IOMMU-Mechanismen erfolgen.
8. Kernel-Kommunikation MUSS über definierte Schnittstellen erfolgen.
9. Treiberfehler SOLLEN ohne Kernel-Neustart behandelbar sein.
10. User-Mode-Treiber SOLLEN kontrolliert neu gestartet und erneut gebunden werden können.
11. Ressourcenverbrauch MUSS begrenzbar sein.
12. Zustand, Ressourcen und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-KERNELMODE-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-DRIVER-0004`

## Ergebnis

```text
Hardware
    ↕
Controlled Kernel Interface
    ↕
Isolated User-Mode Driver
    ↕
Driver Framework
    ↕
NovaOS Subsystem
```

NovaOS erhält damit ein isoliertes User-Mode-Treibermodell, bei dem Treiberfehler möglichst auf den jeweiligen Treiber begrenzt bleiben, während Performance durch Async I/O, Shared Buffers und Zero-Copy erhalten werden kann.