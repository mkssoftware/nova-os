# ADR-DRIVER-0002 – Driver Sandboxing

## Status

Angenommen

## Kategorie

Driver / Security / Isolation / Sandboxing / Reliability

## Kontext

Treiber verarbeiten Daten aus Hardware, Firmware und externen Geräten und besitzen häufig Zugriff auf privilegierte Systemressourcen.

Fehlerhafte oder kompromittierte Treiber können dadurch erhebliche Auswirkungen auf Sicherheit und Stabilität des Gesamtsystems haben.

`ADR-DRIVER-0001` definiert deshalb ein hybrides Kernel- und User-Mode-Drivermodell. Insbesondere User-Mode-Treiber benötigen eine starke Isolation, bei der nur die tatsächlich erforderlichen Hardware- und Systemressourcen verfügbar sind.

Auch Kernel-Mode-Treiber sollen soweit technisch möglich durch eingeschränkte Ressourcen- und Zugriffskontexte begrenzt werden.

---

## Entscheidung

NovaOS behandelt **Driver Sandboxing als grundlegendes Sicherheitsprinzip der Treiberarchitektur**.

Ein Treiber erhält ausschließlich die Ressourcen und Authorities, die für seine deklarierte Funktion erforderlich sind.

```text
Driver
  ↓
Driver Sandbox
  ├── Device Resources
  ├── Memory Access
  ├── DMA Authority
  ├── Interrupt Access
  ├── I/O Authority
  └── System Capabilities
```

User-Mode-Treiber werden grundsätzlich innerhalb isolierter Driver Execution Domains ausgeführt.

---

## Grundprinzip

```text
A driver controls its device.

It does not control the system.
```

---

## Driver Execution Domain

Ein User-Mode-Treiber besitzt eine isolierte Execution Domain mit eigenem Security- und Resource Context.

Die Isolation umfasst insbesondere:

- Virtual Address Space,
- Resource Budgets,
- Capabilities,
- Hardware-Zuweisungen,
- I/O-Zugriffe.

Ein Treiber darf nicht allein aufgrund seiner Rolle als Treiber auf beliebige Systemressourcen zugreifen.

---

## Least Privilege

Treiber erhalten nur die für ihren Device Contract erforderlichen Authorities.

Beispielsweise kann ein Treiber Zugriff erhalten auf:

```text
Specific Device
Specific MMIO Region
Specific Interrupt
Specific DMA Context
Specific I/O Resources
```

statt auf:

```text
All Devices
All Physical Memory
All Interrupts
```

Damit wird der mögliche Schadensbereich eines fehlerhaften oder kompromittierten Treibers begrenzt.

---

## Memory und MMIO

User-Mode-Treiber dürfen nur explizit freigegebene Memory- und MMIO-Bereiche abbilden.

```text
Device Resource
      ↓
Authority Check
      ↓
Controlled Mapping
      ↓
Driver Address Space
```

Andere Kernel-, Prozess- oder Device-Speicherbereiche bleiben unzugänglich.

---

## DMA und IOMMU

DMA muss Bestandteil der Driver Sandbox sein.

Ein Treiber darf ein Gerät nur auf Speicherbereiche zugreifen lassen, die für den jeweiligen DMA-Kontext autorisiert wurden.

```text
Driver
   ↓
DMA Request
   ↓
IOMMU Domain
   ↓
Authorized Buffers
```

Eine Driver Sandbox darf nicht durch DMA umgangen werden.

Fehlt geeignete Hardwareisolation, muss NovaOS einen sicheren alternativen Pfad verwenden oder die betreffende Konfiguration ablehnen.

---

## Interrupts

Treiber erhalten ausschließlich die ihnen zugeordneten Interrupt-Ressourcen.

Die Interrupt-Architektur muss verhindern, dass ein User-Mode-Treiber beliebige Interrupts manipulieren oder andere Driver Domains beeinflussen kann.

Die konkrete Zustellung an User-Mode-Treiber wird separat spezifiziert.

---

## Systemzugriffe

Treiber verwenden kontrollierte Kernel-, HAL- und I/O-Schnittstellen.

Direkter Zugriff auf beliebige Kernelstrukturen ist für User-Mode-Treiber nicht zulässig.

```text
Driver
   ↓
Defined Driver Contract
   ↓
Kernel / HAL / I/O
```

Die Sandbox-Grenze soll damit gleichzeitig eine explizite Architektur- und ABI-Grenze bilden.

---

## Kernel-Mode-Treiber

Kernel-Mode-Treiber können aufgrund ihres Ausführungsmodus nicht dieselbe Isolation wie User-Mode-Treiber erhalten.

Dennoch sollen auch sie nur die logisch erforderlichen Ressourcen und Interfaces verwenden.

Wo Hardwaremechanismen oder Kernelarchitektur dies ermöglichen, sollen Zugriffe zusätzlich eingeschränkt werden.

Kernel Mode darf nicht als uneingeschränkte Authority interpretiert werden.

---

## Fehlerisolation und Recovery

Fehler eines User-Mode-Treibers sollen grundsätzlich auf dessen Driver Domain begrenzt bleiben.

```text
Driver Fault
     ↓
Sandbox Containment
     ↓
Driver Termination
     ↓
Supervision / Recovery
```

Ein restartbarer Treiber kann anschließend über die Driver-Supervision kontrolliert neu gestartet und das Gerät erneut initialisiert werden.

---

## Resource Economy

Driver Sandboxes besitzen eigene beziehungsweise zugeordnete Resource Budgets.

Dies betrifft insbesondere:

- Memory,
- Pinned Memory,
- DMA,
- I/O Bandwidth,
- Queue Resources,
- CPU-Zeit.

Ein Treiber darf durch Ressourcenverbrauch keine unbegrenzte systemweite Wirkung erzielen.

---

## Trust und Device Binding

Die Zuweisung eines Geräts an einen Treiber erfolgt kontrolliert.

NovaOS muss prüfen können, ob:

```text
Driver Identity
      +
Driver Trust
      +
Device Compatibility
      +
Required Authority
```

eine zulässige Bindung ergeben.

Eine erfolgreiche Geräteerkennung allein autorisiert keinen beliebigen Treiber.

---

## Normative Anforderungen

1. NovaOS MUSS Driver Sandboxing als Bestandteil der Treiberarchitektur unterstützen.
2. User-Mode-Treiber MÜSSEN grundsätzlich in isolierten Driver Execution Domains ausgeführt werden.
3. Treiber MÜSSEN nach dem Least-Privilege-Prinzip autorisiert werden.
4. Treiber DÜRFEN nur explizit zugewiesene Hardware-Ressourcen verwenden.
5. User-Mode-Treiber DÜRFEN keinen allgemeinen Zugriff auf physischen Speicher besitzen.
6. MMIO-Zugriffe MÜSSEN auf autorisierte Device-Ressourcen begrenzt werden.
7. DMA MUSS innerhalb der Driver Sandbox kontrolliert werden.
8. IOMMU-Isolation SOLL verwendet werden, wenn die Plattform sie bereitstellt.
9. Fehlende DMA-Isolation DARF NICHT stillschweigend zu uneingeschränktem Speicherzugriff führen.
10. Interrupt-Zugriffe MÜSSEN auf zugewiesene Ressourcen begrenzt werden.
11. User-Mode-Treiber MÜSSEN über definierte Driver Contracts mit Kernel, HAL und I/O-Infrastruktur interagieren.
12. Kernel-Mode-Treiber SOLLEN ebenfalls auf die erforderlichen Ressourcen und Interfaces begrenzt werden.
13. Driver-Ressourcen MÜSSEN der Resource Economy zurechenbar sein.
14. Driver Identity, Trust und Device Compatibility MÜSSEN vor einer Gerätebindung prüfbar sein.
15. Sandbox-Konfiguration und zugewiesene Ressourcen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- kleinere Auswirkungen fehlerhafter Treiber,
- geringere Angriffsfläche,
- kontrollierter Hardwarezugriff,
- sichere DMA-Nutzung,
- bessere Grundlage für restartbare Treiber,
- klare Driver-/Kernel-Grenzen.

### Negative Konsequenzen

- zusätzliche Isolation und Resource Mapping erforderlich,
- nicht jede Hardware unterstützt gleich starke Isolation,
- Kernel-Mode-Treiber bleiben grundsätzlich sicherheitskritischer als User-Mode-Treiber.

---

## Verworfene Alternativen

### Treiber erhalten generell privilegierten Systemzugriff

Verworfen.

Die Treiberrolle allein rechtfertigt keine uneingeschränkte System Authority.

### Sandboxing nur auf Prozessspeicher beschränken

Verworfen.

DMA, MMIO, Interrupts und andere Hardware-Ressourcen könnten die Isolation sonst umgehen.

### Sandboxing ausschließlich für nicht vertrauenswürdige Treiber

Verworfen.

Auch vertrauenswürdiger Code kann Fehler enthalten.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`
- `ADR-IO-0006_IO_Resource_Budgets`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-SANDBOX-0001`
- `NPSPEC-DRIVER-SANDBOX-MEMORY-0001`
- `NPSPEC-DRIVER-SANDBOX-DMA-0001`
- `NPSPEC-DRIVER-SANDBOX-INTERRUPT-0001`
- `NPSPEC-DRIVER-SANDBOX-RESOURCE-0001`
- `NPSPEC-DRIVER-SANDBOX-TEST-0001`

---

## Ergebnis

Treiber erhalten in NovaOS keinen pauschalen Zugriff auf das System, sondern eine kontrollierte Ausführungsumgebung:

```text
             NovaOS
               ↓
        Driver Sandbox
       /       |       \
   Memory     DMA     Device
      ↓        ↓         ↓
   only explicitly authorized resources
```

Damit wird der mögliche Einfluss eines Treibers auf die tatsächlich benötigten Ressourcen begrenzt.

Die zentrale Architekturregel lautet:

```text
Trust the driver only as far
as its assigned resources require.
```