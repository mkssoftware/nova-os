# NPSPEC-DATAMOVE-ZEROCOPY-0001 – Nova Zero-Copy Data Movement

## Status

Entwurf

## Kategorie

DataMove / Zero-Copy / Memory / IPC / I/O

## Zweck

NovaOS definiert Zero-Copy als Mechanismus zur Übertragung oder gemeinsamen Nutzung von Daten, ohne deren Payload unnötig zwischen Speicherbereichen zu kopieren.

```text
Traditional:

Producer
   ↓
Copy
   ↓
Intermediate Buffer
   ↓
Copy
   ↓
Consumer

Zero-Copy:

Producer
   ↓
Shared / Mapped Buffer
   ↓
Consumer
```

Ziel ist die Reduktion von Speicherbandbreite, CPU-Zeit, Cache-Belastung und Latenz.

## Grundprinzip

NovaOS trennt logischen Datentransfer von physischem Kopieren.

```text
Data Transfer ≠ Memory Copy
```

Ein Datentransfer kann durch Änderung von:

```text
Mapping
Ownership
Reference
Capability
Descriptor
```

erfolgen, ohne den eigentlichen Payload zu bewegen.

Dabei gilt:

```text
Zero-Copy ≠ Zero Work
Zero-Copy ≠ Shared Mutable Memory
Zero-Copy ≠ Unrestricted Access
```

## Data Buffer

Zero-Copy-fähige Daten werden über kontrollierte Buffer-Objekte repräsentiert.

Ein Buffer besitzt mindestens:

```text
BufferID
Size
Backing
Owner
State
```

Optional:

```text
Permissions
NUMAPolicy
DeviceMapping
Lifetime
Alignment
SecurityContext
```

Die Identität eines Buffers ist unabhängig von seiner virtuellen Adresse.

## Transfer-Modell

Ein Zero-Copy-Transfer erfolgt konzeptionell:

```text
Producer
   ↓
Buffer
   ↓
Transfer Reference / Ownership
   ↓
Map / Authorize
   ↓
Consumer
```

Der Consumer erhält Zugriff auf denselben Payload statt auf eine Kopie.

## Transferarten

NovaOS darf mehrere Zero-Copy-Mechanismen verwenden:

```text
Shared Mapping
Page Remapping
Ownership Transfer
Buffer Reference Passing
DMA Buffer Sharing
Memory Object Mapping
Scatter/Gather
```

Die Auswahl hängt vom jeweiligen Datenpfad ab.

## Shared Mapping

Mehrere Komponenten dürfen denselben physischen Speicher kontrolliert mappen.

```text
Producer VAS ─┐
              ├→ Physical Frames
Consumer VAS ─┘
```

Die virtuellen Adressen müssen nicht identisch sein.

Permissions werden pro Mapping festgelegt.

## Ownership Transfer

Wenn der Producer den Buffer nach Übergabe nicht mehr benötigt, kann Ownership übertragen werden.

```text
Producer owns Buffer
        ↓
Transfer
        ↓
Consumer owns Buffer
```

Dadurch kann Shared-Memory-Synchronisation vermieden werden.

Nach erfolgreichem exklusivem Transfer darf der alte Owner den Buffer nicht weiter verändern.

## Reference Passing

Bei read-only oder gemeinsam nutzbaren Daten kann lediglich eine autorisierte Referenz übertragen werden.

```text
Buffer
 ├→ Reader A
 ├→ Reader B
 └→ Reader C
```

Referenzzählung oder ein äquivalenter Lifecycle-Mechanismus muss verhindern, dass der Buffer zu früh freigegeben wird.

## Copy-on-Write

Gemeinsam gelesene Daten dürfen über COW geschützt werden.

```text
Shared Buffer
     ↓
Read by multiple consumers
     ↓
Write
     ↓
COW
     ↓
Private Buffer
```

Dadurch kann Zero-Copy für den normalen Lesepfad erhalten bleiben.

## Scatter/Gather

Nicht zusammenhängende Speicherbereiche dürfen als logischer Datenstrom behandelt werden.

```text
Frame A ─┐
Frame B ─┼→ Scatter/Gather Descriptor → Device / Consumer
Frame C ─┘
```

Dadurch muss kein zusätzlicher zusammenhängender Zwischenbuffer erzeugt werden.

## DMA

Geräte dürfen Zero-Copy über DMA verwenden.

```text
Application / Service
        ↓
Authorized Buffer
        ↓
IOMMU Mapping
        ↓
Device DMA
```

Device-Zugriffe müssen über DMA- und IOMMU-Regeln geschützt werden.

Zero-Copy darf einem Gerät keinen Zugriff auf nicht autorisierten Speicher ermöglichen.

## IPC

IPC soll große Payloads nicht unnötig durch Kernel-Zwischenbuffer kopieren.

```text
Sender
  ↓
Buffer Capability
  ↓
IPC Metadata
  ↓
Receiver Mapping
```

Kleine Nachrichten dürfen weiterhin kopiert werden, wenn dies effizienter ist.

```text
Small Copy
<
Zero-Copy Setup Cost
```

Zero-Copy ist daher keine universelle Vorgabe.

## Netzwerk

Netzwerkpfade dürfen Buffer direkt zwischen:

```text
NIC
Network Stack
Protocol Processing
Application
```

weiterreichen.

Buffer Ownership und Lifetime müssen dabei eindeutig bleiben.

## Storage

Storage-I/O darf geeignete User- oder Cache-Buffer direkt verwenden.

```text
Storage Device
      ↕
DMA Buffer
      ↕
Application / Cache
```

Zusätzliche Kopien sollen vermieden werden, sofern Alignment, Sicherheit und Hardwareanforderungen erfüllt sind.

## Sicherheit

Zero-Copy darf keine Sicherheitsgrenzen umgehen.

Vor jeder Freigabe oder Übertragung müssen mindestens geprüft werden:

```text
Capability
Permissions
Security Domain
Operation
Lifetime
```

Ein Empfänger darf nur die für ihn autorisierten Bereiche des Buffers sehen.

## Information Leakage

Wiederverwendete Buffer dürfen keine Daten vorheriger Sicherheitskontexte offenlegen.

```text
Old Owner
   ↓
Buffer Reuse
   ↓
New Owner
```

Falls notwendig, muss der Speicher vor einer sicherheitsdomänenübergreifenden Wiederverwendung bereinigt werden.

## Synchronisation

Gemeinsame Buffer benötigen explizite Synchronisation.

```text
Producer writes
      ↓
Publish
      ↓
Consumer reads
```

Geeignete Mechanismen können sein:

```text
Atomics
Futex
Semaphore
Ownership Transfer
Completion Objects
```

Zero-Copy selbst stellt keine Synchronisationsgarantie bereit.

## Cache-Kohärenz

Bei CPU-/Device-Sharing müssen Cache-Kohärenzanforderungen der Plattform berücksichtigt werden.

```text
CPU Cache
   ↕
Memory
   ↕
Device
```

Auf nicht kohärenten Plattformen müssen erforderliche Flush-, Clean- oder Invalidate-Operationen über die HAL erfolgen.

## NUMA

Zero-Copy bedeutet nicht automatisch optimale Datenlokalität.

```text
Consumer Node 1
      ↓
Buffer on Node 0
```

NovaOS muss abwägen können:

```text
Remote Access Cost
vs.
Copy / Migration Cost
```

In bestimmten Fällen kann eine gezielte Kopie effizienter sein als dauerhaftes Remote Zero-Copy.

## Fallback

Jeder Zero-Copy-Pfad soll einen sicheren Copy-Fallback besitzen können.

```text
Zero-Copy possible?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
Reference       Safe Copy
```

Gründe für einen Fallback können sein:

```text
Alignment
Security Boundary
Device Limitation
Lifetime Conflict
Memory Pressure
Incompatible Mapping
```

## Resource Economy

Die Entscheidung zwischen Copy und Zero-Copy darf berücksichtigen:

```text
Payload Size
Setup Cost
Memory Bandwidth
CPU Cost
Cache Pollution
NUMA Distance
Latency
Energy
Device Capability
```

Zero-Copy soll nur eingesetzt werden, wenn es funktional zulässig und wirtschaftlich sinnvoll ist.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Zero-Copy Transfers
Copied Transfers
Transferred Bytes
Avoided Copies
Buffer Ownership
Mapping Count
DMA Transfers
Fallback Reasons
Transfer Latency
NUMA Placement
```

## Normative Anforderungen

1. NovaOS MUSS Datentransfer und physisches Kopieren logisch voneinander trennen.
2. Zero-Copy MUSS über kontrollierte Buffer-, Mapping- oder Ownership-Mechanismen realisierbar sein.
3. Buffer-Identität DARF NICHT von einer bestimmten virtuellen Adresse abhängen.
4. Zero-Copy DARF Security-, Capability- oder Isolation-Regeln NICHT umgehen.
5. Buffer-Lifetime MUSS während des gesamten Transfers garantiert sein.
6. Ownership Transfer MUSS eindeutig definieren, wer den Buffer verändern darf.
7. Shared Buffer MÜSSEN explizite Synchronisation verwenden, sofern konkurrierende Zugriffe möglich sind.
8. DMA-Zugriffe MÜSSEN auf autorisierte Speicherbereiche begrenzt werden.
9. Cache-Kohärenz MUSS entsprechend der Hardwareplattform korrekt behandelt werden.
10. NUMA-Kosten SOLLEN bei Zero-Copy-Entscheidungen berücksichtigt werden.
11. NovaOS SOLL einen sicheren Copy-Fallback bereitstellen können.
12. Zero-Copy-Nutzung, Kosten und Fallback-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-COW-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DATAMOVE-0001`

## Ergebnis

```text
Producer
   ↓
Data Buffer
   ↓
Reference / Mapping / Ownership
   ↓
IPC / Device / Network / Storage
   ↓
Consumer

Payload stays where possible
```

NovaOS erhält damit eine einheitliche Zero-Copy-Grundlage, bei der Datenbewegung bevorzugt durch kontrollierte Referenz-, Mapping- und Ownership-Operationen statt durch unnötige Payload-Kopien erfolgt.