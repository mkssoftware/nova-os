# NPSPEC-MEMORY-SHARED-0001 – Nova Shared Memory

## Status

Entwurf

## Kategorie

Memory / Shared Memory / IPC / Virtual Memory

## Zweck

NovaOS definiert Shared Memory als kontrollierten Mechanismus, mit dem mehrere Address Spaces auf dasselbe physische oder logisch gemeinsame Speicherobjekt zugreifen können.

```text
VAS A ─┐
       ├→ Shared Memory Object → Physical Frames
VAS B ─┘
```

Shared Memory ermöglicht effizienten Datenaustausch ohne zwingende Kopien zwischen Prozessen oder Systemkomponenten.

## Grundprinzip

Shared Memory basiert auf einem gemeinsamen Backing Object, das in mehrere Virtual Address Spaces gemappt wird.

```text
Shared Object
   ├→ Mapping A → VAS A
   ├→ Mapping B → VAS B
   └→ Mapping C → VAS C
```

Dabei gilt:

```text
Shared Memory ≠ Shared Address
Shared Memory ≠ Synchronization
Shared Memory ≠ Unrestricted Access
```

Die virtuelle Adresse darf in jedem Address Space unterschiedlich sein.

## Shared Memory Object

Gemeinsam verwendeter Speicher wird als eigenständiges Objekt modelliert.

Mindestens:

```text
SharedMemoryID
Size
Backing
State
References
```

Optional:

```text
Owner
SecurityContext
NUMAPolicy
ResourceBudget
PageSizePolicy
Persistence
```

Die Objektidentität ist unabhängig von virtuellen Adressen oder physischen Frame-Adressen.

## Erstellung

Ein Shared Memory Object wird zunächst unabhängig von seinen Mappings erzeugt.

```text
Create Shared Object
        ↓
Define Size / Policy
        ↓
Authorize Participants
        ↓
Map into Address Spaces
```

Physische Frames dürfen sofort oder über Demand Paging bereitgestellt werden.

## Mapping

Jeder Teilnehmer erhält ein eigenes Mapping.

```text
Process A
0x400000 → Shared Object

Process B
0x900000 → Shared Object
```

Die unterschiedlichen virtuellen Adressen referenzieren denselben logischen Speicher.

## Permissions

Zugriffsrechte werden pro Mapping bestimmt.

```text
Shared Object
├── Process A → Read/Write
├── Process B → Read
└── Process C → Read/Write
```

Die effektiven Rechte ergeben sich aus:

```text
Object Permission
∩
Mapping Permission
∩
Security Policy
∩
Capability
```

Ein Mapping darf keine höheren Rechte erhalten als das Shared Memory Object erlaubt.

## Capability-basierter Zugriff

Der Zugriff auf Shared Memory muss explizit autorisiert werden.

Eine Capability darf beispielsweise folgende Rechte vermitteln:

```text
Map
Read
Write
Delegate
Resize
Seal
Destroy
```

Delegation darf Rechte abschwächen.

```text
Read/Write Capability
        ↓
Delegate
        ↓
Read-Only Capability
```

## Synchronisation

Shared Memory stellt keine automatische Synchronisation bereit.

```text
Shared Memory
     ≠
Mutex / Atomic / Semaphore
```

Mehrere Teilnehmer, die dieselben Daten verändern, müssen geeignete Synchronisationsmechanismen verwenden.

Beispiele:

```text
Atomics
Mutex
Semaphore
Futex
Lock-Free Structures
RCU
```

## Memory Ordering

Bei gemeinsamem Speicher müssen die Memory-Ordering-Regeln der verwendeten Synchronisationsprimitive eingehalten werden.

```text
Write Data
    ↓
Release
    ↓
Shared Memory
    ↓
Acquire
    ↓
Read Data
```

Die bloße Tatsache, dass Speicher gemeinsam gemappt ist, erzeugt keine Synchronisationsgarantie.

## Demand Paging

Shared Memory darf demand-geladen werden.

```text
Shared Mapping
      ↓
Page Not Present
      ↓
Fault
      ↓
Resolve Shared Backing
      ↓
Map Shared Frame
```

Mehrere gleichzeitige Faults derselben Shared Page dürfen nicht zu unabhängigen widersprüchlichen Backing Frames führen.

## Zero-Copy

Shared Memory ist ein zentraler Mechanismus für Zero-Copy-Datenübertragung.

```text
Producer
   ↓
Shared Buffer
   ↓
Consumer
```

Anstatt Daten:

```text
Producer → Copy → Kernel → Copy → Consumer
```

zu übertragen, kann ein autorisiertes gemeinsames Memory Object verwendet werden.

Zero-Copy ist eine Optimierung und darf Isolation oder Sicherheitsregeln nicht umgehen.

## Ownership

Shared Memory benötigt klare Ownership- und Lifecycle-Regeln.

```text
Object Lifetime
      ≠
Single Mapping Lifetime
```

Das Entfernen eines Mappings darf das Shared Memory Object nicht zerstören, solange weitere gültige Referenzen bestehen.

## Referenzverwaltung

Das System muss gültige Referenzen verfolgen können.

```text
Shared Object
├── Mapping A
├── Mapping B
├── Kernel Reference
└── Capability Reference
```

Erst wenn keine relevante Referenz mehr besteht, darf das Objekt entsprechend seiner Lifecycle-Policy freigegeben werden.

## Resize

Shared Memory Objects dürfen optional vergrößert oder verkleinert werden.

```text
Resize Request
      ↓
Validate
      ↓
Update Object
      ↓
Update affected Mappings
```

Eine Verkleinerung darf keine weiterhin gültigen Zugriffe auf entfernte Bereiche ermöglichen.

## Sealing

Shared Memory darf gegen bestimmte spätere Änderungen versiegelt werden.

Beispiele:

```text
No Resize
No Write
No New Mappings
No Permission Increase
```

Ein gesetztes irreversibles Seal darf nicht nachträglich entfernt werden.

## NUMA

Shared Memory kann von Tasks auf mehreren NUMA-Nodes verwendet werden.

Mögliche Policies:

```text
Preferred Node
Interleave
First Touch
Replicate Read-Mostly
Explicit Placement
```

Die Platzierung soll Zugriffskosten aller relevanten Teilnehmer berücksichtigen.

## Realtime

Realtime-Komponenten dürfen Shared Memory verwenden.

Für harte zeitliche Garantien müssen benötigte Seiten:

```text
Preallocated
Resident
Pinned if required
```

sein.

Unkontrollierte Demand Faults oder Reclamation dürfen einen garantierten Realtime-Pfad nicht beeinträchtigen.

## Device Sharing

Shared Memory darf für CPU-/Device-Kommunikation verwendet werden, sofern DMA-, IOMMU- und Cache-Kohärenzregeln dies erlauben.

```text
Process
   ↓
Shared Buffer
   ↓
Device
```

Device-Zugriff benötigt explizite Autorisierung.

## Isolation

Ein Prozess darf Shared Memory nur sehen, wenn ihm der Zugriff ausdrücklich gewährt wurde.

```text
No Capability
     ↓
No Mapping
```

Shared Memory darf keine implizite Hintertür zwischen isolierten Prozessen erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
SharedMemoryID
Size
Owner
Mapping Count
Reference Count
Permissions
Participants
Resident Pages
NUMA Placement
Resource Usage
```

Informationen über andere Sicherheitskontexte müssen entsprechend gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Shared Memory als eigenständiges Speicherobjekt modellieren können.
2. Die Identität eines Shared Memory Objects DARF NICHT von seiner virtuellen Adresse abhängen.
3. Dasselbe Shared Memory Object MUSS in mehrere Address Spaces gemappt werden können.
4. Teilnehmer DÜRFEN unterschiedliche Mapping-Adressen und Permissions besitzen.
5. Zugriff auf Shared Memory MUSS explizit autorisiert werden.
6. Shared Memory DARF NICHT als impliziter Synchronisationsmechanismus behandelt werden.
7. Gleichzeitige Demand Faults derselben Shared Page MÜSSEN konsistent aufgelöst werden.
8. Das Entfernen eines Mappings DARF ein weiterhin referenziertes Shared Memory Object NICHT zerstören.
9. Zero-Copy DARF Sicherheits- und Isolationseigenschaften NICHT umgehen.
10. NUMA-Policies SOLLEN für Shared Memory unterstützt werden.
11. Realtime Shared Memory MUSS vor unkontrollierten Fault- und Reclaim-Latenzen geschützt werden können.
12. Shared Memory Objects und deren Mappings SOLLEN sicherheitsgefiltert introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-DEMANDPAGING-0001`
- `NPSPEC-MEMORY-TLB-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-FUTEX-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `ADR-MEMORY-0017`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Shared Memory Object
        ↓
Authorized Mappings
   ┌────┼────┐
 VAS A VAS B VAS C
   └────┼────┘
        ↓
Shared Physical Backing
        ↓
Explicit Synchronization
```

NovaOS erhält damit einen sicheren Shared-Memory-Mechanismus für effiziente IPC-, Zero-Copy- und Device-Workflows, ohne Prozessisolation, Berechtigungen oder Synchronisationsregeln aufzuweichen.