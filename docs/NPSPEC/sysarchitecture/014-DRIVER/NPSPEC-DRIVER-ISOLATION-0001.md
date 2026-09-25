# NPSPEC-DRIVER-ISOLATION-0001 – Nova Driver Isolation

## Status

Angenommen

## Kategorie

Driver / Isolation / Security / Resilience

## Zweck

NovaOS definiert ein einheitliches Isolationsmodell für Treiber, um Fehler, Speicherzugriffe und Ressourcenverbrauch möglichst auf den betroffenen Treiber und dessen Geräte zu begrenzen.

```text
Driver
   ↓
Isolation Domain
   ↓
Controlled Resources
   ↓
Device
```

Treiber sollen nur auf Ressourcen zugreifen können, die für ihre Funktion tatsächlich erforderlich sind.

## Grundprinzip

```text
Driver Identity ≠ Kernel Authority
Device Access ≠ Memory Access
Driver Failure ≠ System Failure
```

Isolation muss nach dem Prinzip der geringstmöglichen Berechtigung erfolgen.

## Isolation Domain

Treiber können abhängig von ihren Anforderungen unterschiedlichen Isolationsdomänen zugeordnet werden.

```text
Driver Isolation
├── User-Mode Domain
├── Restricted Kernel Domain
└── Shared Trusted Kernel Domain
```

User-Mode soll bevorzugt werden, wenn keine zwingenden technischen Anforderungen für Kernel-Mode bestehen.

## Ressourcenisolation

Jede Treiberinstanz erhält ausschließlich explizit zugewiesene Ressourcen.

Beispiele:

```text
MMIO Range
I/O Ports
Interrupts
DMA Context
Memory
Device Handle
Shared Buffer
```

Ein Treiber darf Ressourcen anderer Treiber oder Geräte nicht ohne explizite Berechtigung verwenden.

## Speicherisolation

User-Mode-Treiber müssen über getrennte Adressräume oder gleichwertige Schutzmechanismen isoliert werden.

Kernel-Mode-Treiber sollen soweit technisch möglich eingeschränkte Speicher- und Ressourcenbereiche verwenden.

```text
Driver A Memory
      ≠
Driver B Memory
      ≠
Kernel Memory
```

Gemeinsamer Speicher muss explizit eingerichtet und autorisiert werden.

## DMA-Isolation

DMA darf die Speicherisolation nicht umgehen.

```text
Driver
   ↓
DMA Mapping
   ↓
IOMMU
   ↓
Authorized Memory
```

DMA-fähige Geräte dürfen nur auf Speicherbereiche zugreifen, die für die jeweilige Operation freigegeben wurden.

## Interrupt-Isolation

Interrupts müssen eindeutig Geräten beziehungsweise Treiberinstanzen zugeordnet werden können.

Ein Treiber darf nicht unkontrolliert Interrupt-Zustände fremder Geräte beeinflussen.

Aufwendige Verarbeitung soll außerhalb des unmittelbaren Interrupt-Kontexts erfolgen.

## I/O-Isolation

I/O-Requests müssen dem zuständigen Treiber, Gerät und Security Context zugeordnet bleiben.

```text
IORequest
├── Owner
├── Target
├── Driver
└── Security Context
```

Treiber dürfen Requests nicht verwenden, um Capability- oder Security-Prüfungen zu umgehen.

## Fehlerisolation

Fehler sollen auf die kleinstmögliche betroffene Domäne begrenzt werden.

```text
Driver Failure
     ↓
Contain
     ↓
Stop / Reset
     ↓
Recover / Restart
```

Insbesondere User-Mode-Treiber sollen unabhängig vom Kernel neu gestartet werden können.

## Ressourcenbegrenzung

Treiberressourcen müssen begrenzbar sein.

```text
Memory
CPU Time
DMA Memory
Queue Depth
Interrupt Rate
IPC Capacity
```

Ein fehlerhafter Treiber darf keine unbegrenzte Ressourcenerschöpfung verursachen.

## Abhängigkeiten

Mehrere Treiber dürfen kontrolliert zusammenarbeiten.

```text
Driver A
   ↓ Capability / Interface
Driver B
```

Eine Abhängigkeit darf nicht automatisch sämtliche Rechte des anderen Treibers übertragen.

## Gerätefehler

Ein fehlerhaftes oder kompromittiertes Gerät muss soweit hardwareseitig möglich isoliert werden können.

Dazu können insbesondere verwendet werden:

```text
IOMMU
Bus Reset
Device Reset
Interrupt Masking
DMA Revocation
Resource Revocation
```

## Recovery

Nach einem Treiberfehler soll NovaOS abhängig vom Gerät folgende Maßnahmen ermöglichen:

```text
Restart Driver
Reset Device
Rebind Driver
Fallback Driver
Disable Device
```

`Unknown` oder nicht verifizierter Zustand darf nicht automatisch als sicher behandelt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
Isolation Domain
Bound Devices
Granted Resources
DMA Mappings
Memory Usage
Pending I/O
Failure State
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS Treiberressourcen kontrolliert isolieren können.
2. Treiber SOLLEN nach dem Least-Privilege-Prinzip ausgeführt werden.
3. User-Mode-Treiber MÜSSEN vom Kernel und anderen Treibern isoliert sein.
4. Gemeinsamer Speicher MUSS explizit eingerichtet und autorisiert werden.
5. Treiber DÜRFEN nicht zugewiesene Hardware-Ressourcen NICHT verwenden.
6. DMA DARF Speicherisolation NICHT umgehen.
7. DMA-fähige Geräte MÜSSEN soweit verfügbar durch IOMMU oder gleichwertige Mechanismen begrenzt werden.
8. I/O-Requests MÜSSEN ihrer ursprünglichen Security- und Ressourcen-Domäne zugeordnet bleiben.
9. Treiberabhängigkeiten DÜRFEN Rechte NICHT automatisch übertragen.
10. Ressourcenverbrauch von Treibern MUSS begrenzbar sein.
11. Treiberfehler SOLLEN auf die kleinstmögliche Domäne begrenzt und kontrolliert recoverbar sein.
12. Isolation, Ressourcen und Fehlerzustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-KERNELMODE-0001`
- `NPSPEC-DRIVER-USERMODE-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DRIVER-0005`

## Ergebnis

```text
Driver
   ↓
Isolation Domain
   ├── Memory
   ├── DMA
   ├── Interrupts
   ├── I/O
   └── Resources
          ↓
     Controlled Device Access
```

NovaOS erhält damit ein Treiber-Isolationsmodell, das Fehler und Berechtigungen auf möglichst kleine Domänen begrenzt und gleichzeitig kontrollierte Hardwarezugriffe sowie Recovery ermöglicht.