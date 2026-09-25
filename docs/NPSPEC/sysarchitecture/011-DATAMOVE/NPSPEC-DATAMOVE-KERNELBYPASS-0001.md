# NPSPEC-DATAMOVE-KERNELBYPASS-0001 – Nova Kernel Bypass Data Path

## Status

Angenommen

## Kategorie

DataMove / Kernel Bypass / Zero-Copy / DMA / High-Performance I/O

## Zweck

NovaOS definiert Kernel Bypass als kontrollierten Datenpfad, bei dem autorisierte Komponenten Daten direkt mit Hardware oder gemeinsam genutzten I/O-Strukturen austauschen können, ohne dass der Kernel jeden einzelnen Datentransfer aktiv verarbeiten muss.

```text
Normal Path:

Application
    ↓
Kernel
    ↓
Driver
    ↓
Device

Kernel Bypass:

Application / Service
        ↓
Shared Queues / Buffers
        ↓
Device
```

Ziel sind geringere Latenz, weniger Context Switches, weniger Kopien und geringere CPU-Belastung bei datenintensiven Workloads.

## Grundprinzip

Kernel Bypass bedeutet nicht, dass der Kernel seine Kontrolle über Ressourcen oder Sicherheit verliert.

```text
Kernel Bypass
      ≠
Security Bypass
```

Der Kernel kontrolliert weiterhin:

```text
Authorization
Resource Allocation
Memory Mapping
IOMMU
Device Assignment
Lifecycle
Revocation
Failure Handling
```

Nur der schnelle Datenpfad darf nach erfolgreicher Einrichtung ohne permanente Kernelbeteiligung arbeiten.

## Control Plane und Data Plane

NovaOS trennt:

```text
Control Plane
     ↓
Kernel

Data Plane
     ↓
Direct User/Service ↔ Device Path
```

Der Kernel richtet den Datenpfad ein:

```text
Authorize
   ↓
Allocate Resources
   ↓
Map Buffers
   ↓
Configure IOMMU
   ↓
Configure Queues
   ↓
Activate
```

Danach darf der Datenverkehr direkt erfolgen.

## Bypass Context

Ein Kernel-Bypass-Pfad wird durch einen expliziten Kontext repräsentiert.

Dieser besitzt mindestens:

```text
BypassID
Owner
Device
Queues
Buffers
Capabilities
State
```

Optional:

```text
IOMMUDomain
ResourceBudget
NUMANode
InterruptMode
Deadline
SecurityContext
```

## Zustände

Ein Bypass Context kann mindestens folgende Zustände besitzen:

```text
Created
Configured
Active
Quiescing
Revoked
Destroyed
```

Nur ein vollständig validierter Kontext darf `Active` werden.

## Shared Queues

Kommunikation mit Hardware darf über gemeinsam genutzte Queue-Strukturen erfolgen.

```text
Application
    ↓
Submission Ring
    ↓
Device
    ↓
Completion Ring
    ↓
Application
```

Ring Buffers und Descriptor Queues sollen bevorzugt verwendet werden, wenn die Hardware dies unterstützt.

## Shared Buffers

Payload-Daten sollen über kontrollierte Shared Buffers übertragen werden können.

```text
Application Buffer
       ↓
DMA Mapping
       ↓
Device
```

Dadurch kann ein Datenpfad entstehen:

```text
Application
    ↓
Shared Buffer
    ↓
DMA
    ↓
Device
```

ohne zusätzliche Kernel-Payload-Kopie.

## DMA

Kernel Bypass darf DMA verwenden.

Der Kernel muss vor Aktivierung sicherstellen:

```text
Buffer Valid
Capability Valid
DMA Direction Valid
Lifetime Valid
Device Authorized
```

DMA-Zugriffe dürfen ausschließlich auf freigegebene Speicherbereiche erfolgen.

## IOMMU

Eine verfügbare IOMMU soll als zentrale Schutzgrenze für Kernel-Bypass-DMA verwendet werden.

```text
Device
  ↓
IOMMU Domain
  ↓
Authorized Buffers
```

Ein Bypass Context darf nicht automatisch Zugriff auf den gesamten physischen Speicher erhalten.

## Gerätezuweisung

NovaOS darf:

```text
Whole Device
Queue
Virtual Function
Hardware Context
```

einem Bypass Context zuweisen.

Feingranulare Hardwarepartitionierung soll bevorzugt werden, wenn das Gerät sie unterstützt.

```text
Device
├→ Queue A → Service A
├→ Queue B → Service B
└→ Queue C → Kernel
```

## Capability-Modell

Kernel Bypass benötigt explizite Capabilities.

Beispiele:

```text
UseDevice
CreateQueue
MapDMA
Submit
Receive
Configure
Reset
```

Eine Data-Plane-Capability darf nicht automatisch administrative Device-Rechte gewähren.

```text
Submit Packets
      ≠
Reconfigure Device
```

## Interrupts

Completion kann über verschiedene Mechanismen erfolgen:

```text
Interrupt
Polling
Adaptive Polling
Completion Queue
Event
```

High-Performance-Pfade dürfen Polling verwenden.

Dauerhaftes Busy Polling muss jedoch in:

```text
CPU Budget
Energy Policy
Realtime Policy
```

berücksichtigt werden.

## Polling

Für niedrige Latenz kann ein Worker direkt eine Queue beobachten.

```text
Worker
  ↓
Poll Queue
  ↓
Process Completion
```

NovaOS darf adaptiv zwischen Polling und Interrupts wechseln.

```text
High Traffic → Polling
Low Traffic  → Interrupt
```

Diese Entscheidung ist eine Soft Optimization.

## NUMA und Locality

Bypass Worker, Device und Buffer sollen möglichst lokal zueinander platziert werden.

```text
Device
  ↓
Local NUMA Node
  ├→ DMA Buffers
  └→ Worker
```

Ungünstige Remote-Zugriffe können den Vorteil von Kernel Bypass erheblich reduzieren.

## Scheduling

Kernel-Bypass-Worker bleiben normale kontrollierte Execution Entities.

Sie unterliegen weiterhin:

```text
Scheduler
CPU Budget
Priority
Realtime Policy
Resource Economy
```

Kernel Bypass darf keine unbegrenzte CPU-Nutzung erzeugen.

## Memory Protection

Gemappte Queue- und Bufferbereiche müssen auf das notwendige Minimum beschränkt werden.

```text
Application VAS
├→ Queue Mapping
├→ Buffer Mapping
└→ No Device-global Memory
```

Device Register dürfen nur gemappt werden, wenn dies für das konkrete Bypass-Modell notwendig und sicher ist.

## Revocation

Der Kernel muss einen Bypass Context widerrufen können.

```text
Revoke
  ↓
Stop New Submissions
  ↓
Quiesce Device / Queue
  ↓
Verify DMA stopped
  ↓
Remove IOMMU Mappings
  ↓
Unmap Buffers
  ↓
Release Resources
```

Memory darf erst freigegeben werden, wenn kein Hardwarezugriff mehr möglich ist.

## Device Failure

Kernel Bypass darf die zentrale Fehlerkontrolle nicht verhindern.

Bei:

```text
Device Reset
IOMMU Fault
Timeout
Queue Corruption
Device Removal
```

muss der Kernel den betroffenen Bypass Context isolieren können.

Ein fehlerhafter Context darf andere Device-Nutzer nicht unnötig beeinträchtigen.

## Fallback

Kernel Bypass ist eine Optimierung.

```text
Bypass Available?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
Direct Path   Kernel Path
```

NovaOS soll einen kontrollierten Kernel-I/O-Pfad als Fallback unterstützen können.

Anwendungen sollen nicht zwingend von einem bestimmten Hardware-Bypass-Mechanismus abhängig sein.

## Realtime

Kernel Bypass kann für Realtime-Datenpfade verwendet werden.

Dafür sollen:

```text
Buffers preallocated
Queues preconfigured
DMA mappings prepared
CPU placement fixed
Interrupt / Polling policy bounded
```

werden können.

Unvorhersehbare Setup-Operationen dürfen aus Hard-Realtime-Pfaden herausgehalten werden.

## Resource Economy

Kernel Bypass besitzt eigene Kosten.

Dazu gehören:

```text
Pinned Memory
Dedicated Queues
CPU Polling
IOMMU Entries
Device Resources
Interrupt Resources
```

Diese Ressourcen müssen budgetierbar sein.

Kernel Bypass darf nicht unbegrenzt Hardware- oder Systemspeicher reservieren.

## Sicherheit

Der Kernel bleibt die Autoritätsinstanz für Einrichtung und Widerruf.

Vor Aktivierung müssen mindestens geprüft werden:

```text
Identity
Capability
Device
Queue
Memory Range
DMA Direction
Resource Budget
Security Domain
```

Kernel Bypass darf keine globale Sicherheitsgrenze umgehen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Active Bypass Contexts
Assigned Devices
Assigned Queues
DMA Mappings
Pinned Memory
Submission Rate
Completion Rate
Polling CPU Cost
Interrupt Rate
IOMMU Faults
Revocations
Fallback Events
```

Sensible Hardware- und Adressinformationen müssen entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Kernel Bypass als optionalen DataMove-Mechanismus behandeln.
2. Kernel Bypass DARF Security-, Capability- oder Isolation-Regeln NICHT umgehen.
3. Control Plane und Data Plane MÜSSEN logisch getrennt werden.
4. Der Kernel MUSS Einrichtung, Autorisierung und Widerruf eines Bypass Contexts kontrollieren.
5. DMA-Zugriffe MÜSSEN auf explizit autorisierte Speicherbereiche begrenzt werden.
6. Eine verfügbare IOMMU SOLL zur Isolation von Kernel-Bypass-DMA verwendet werden.
7. Shared Queues und Buffer MÜSSEN einen eindeutig definierten Lifecycle besitzen.
8. Speicher DARF NICHT freigegeben werden, solange Hardware möglicherweise darauf zugreift.
9. Kernel-Bypass-Worker MÜSSEN weiterhin Scheduling- und Resource-Budget-Regeln unterliegen.
10. Polling DARF NICHT automatisch unbegrenzte CPU-Zeit beanspruchen.
11. NovaOS SOLL einen kontrollierten Nicht-Bypass-Fallback unterstützen können.
12. Bypass Contexts, Ressourcenverbrauch, DMA-Mappings, Fehler und Revocations SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DATAMOVE-RINGBUFFER-0001`
- `NPSPEC-DATAMOVE-LOCALITY-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DATAMOVE-0007`

## Ergebnis

```text
                 Kernel
                   ↓
        Authorize + Configure
                   ↓
            Bypass Context
                   ↓
      ┌────────────┴────────────┐
      ↓                         ↓
Shared Queues             Shared Buffers
      ↓                         ↓
      └────────────┬────────────┘
                   ↓
             IOMMU / DMA
                   ↓
                 Device

        Fast Data Plane
        Kernel-controlled
```

NovaOS erhält damit einen kontrollierten Kernel-Bypass-Datenpfad, der direkte Hochleistungs-I/O-Kommunikation ermöglicht, während Kernel, Capability-System und IOMMU weiterhin Sicherheit, Ressourcenverwaltung und Lifecycle kontrollieren.