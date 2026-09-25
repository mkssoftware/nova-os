# NPSPEC-MEMORY-MAPPING-0001 – Nova Memory Mapping

## Status

Angenommen

## Kategorie

Memory / Virtual Memory / Mapping / Memory Objects

## Zweck

NovaOS definiert Memory Mapping als kontrollierte Verbindung zwischen einem virtuellen Adressbereich und einem zugrunde liegenden Speicherobjekt.

```text
Virtual Region
     ↓
Mapping
     ↓
Backing Object
     ↓
Physical Frames
```

Mappings bilden die zentrale Verbindung zwischen Virtual Address Spaces, Paging und den tatsächlichen Speicherressourcen.

## Grundprinzip

Ein Mapping ist mehr als ein einzelner Page-Table-Eintrag.

```text
Mapping
├── Virtual Range
├── Backing Object
├── Offset
├── Permissions
├── Sharing Mode
└── Mapping Policy
```

Dabei gilt:

```text
Mapping ≠ Physical Allocation
Mapping ≠ Page Table Entry
Mapping ≠ Ownership
```

Ein Mapping beschreibt die semantische Beziehung zwischen virtuellem Speicher und seinem Backing.

## Mapping-Typen

NovaOS muss unterschiedliche Mapping-Arten unterstützen können:

```text
Anonymous
Private
Shared
File-backed
Object-backed
Device
Persistent
Executable
Copy-on-Write
```

Mehrere Eigenschaften dürfen kombiniert werden, sofern ihre Semantik kompatibel ist.

## Mapping-Modell

Ein Mapping besitzt mindestens:

```text
MappingID
AddressSpaceID
VirtualRange
BackingObject
BackingOffset
Permissions
MappingType
State
```

Optional:

```text
NUMAPolicy
CachePolicy
PageSizePolicy
ResourceBudget
SecurityContext
```

## Anonymous Mapping

Anonymous Memory besitzt kein externes persistentes Backing Object.

```text
Virtual Range
     ↓
Anonymous Mapping
     ↓
Demand Paging
     ↓
Physical Frames
```

Physische Frames dürfen erst bei tatsächlicher Nutzung bereitgestellt werden.

## Object-backed Mapping

NovaOS darf Dateien und andere Systemobjekte direkt in einen Address Space abbilden.

```text
Nova Object
    ↓
Mapping
    ↓
Virtual Address Space
```

Der Mapping-Layer darf dabei nicht auf klassische Dateien beschränkt sein.

Geeignete Backing Objects können beispielsweise sein:

```text
Files
Shared Memory Objects
Executable Images
Persistent Objects
Memory Objects
```

## Private Mapping

Bei einem privaten Mapping dürfen Änderungen vom ursprünglichen Backing getrennt werden.

```text
Backing Object
      ↓
Private Mapping
      ↓
Write
      ↓
Copy-on-Write
```

Private Änderungen dürfen andere Mapper nicht unbeabsichtigt verändern.

## Shared Mapping

Ein Shared Mapping erlaubt mehreren Address Spaces Zugriff auf dasselbe Backing.

```text
VAS A ─┐
       ├→ Shared Mapping → Backing Object
VAS B ─┘
```

Die einzelnen Mappings dürfen unterschiedliche Permissions besitzen.

## Permissions

Jedes Mapping muss explizite Zugriffsrechte besitzen.

Mindestens:

```text
Read
Write
Execute
```

Zusätzlich:

```text
User / Kernel
Shared / Private
Device
Cache Policy
```

Berechtigungen des Mappings dürfen die Berechtigungen des Backing Objects nicht umgehen.

```text
Effective Permission
=
Mapping Permission
∩
Object Permission
∩
Security Policy
```

## Mapping-Erstellung

Die Erstellung erfolgt konzeptionell:

```text
Mapping Request
      ↓
Validate Address Range
      ↓
Validate Backing Object
      ↓
Validate Permissions
      ↓
Reserve VAS Region
      ↓
Create Mapping Metadata
      ↓
Install Immediate Pages
or
Enable Demand Paging
```

Nicht alle Pages müssen sofort physisch gemappt werden.

## Unmapping

Mappings müssen vollständig oder teilweise entfernt werden können.

```text
Mapping
   ↓
Unmap
   ↓
Remove Page Mappings
   ↓
TLB Invalidation
   ↓
Release References
```

Dabei gilt:

```text
Unmap ≠ Delete Backing Object
Unmap ≠ Automatically Free Shared Frames
```

## Partial Mapping

NovaOS soll Teilbereiche eines Mappings verändern oder entfernen können.

```text
Original Mapping

[ A | B | C ]

Unmap B

[ A ]   [ C ]
```

Dadurch kann ein Mapping geteilt werden.

Metadaten müssen anschließend konsistent bleiben.

## Remapping

Mappings dürfen verschoben, erweitert, verkleinert oder hinsichtlich ihrer Eigenschaften verändert werden, sofern die jeweilige Operation zulässig ist.

```text
Old Mapping
     ↓
Validate
     ↓
Transform
     ↓
New Mapping
```

Die Operation darf keine teilweise sichtbaren inkonsistenten Zustände erzeugen.

## Demand Paging

Mappings können physisch noch nicht vorhandene Pages enthalten.

```text
Mapping Metadata
      ↓
Page Not Present
      ↓
Access
      ↓
Demand Fault
      ↓
Resolve Backing
```

Das Mapping liefert dem Fault Handler die Informationen zur Auflösung.

## Copy-on-Write

Private oder abgeleitete Mappings dürfen COW verwenden.

```text
Shared Frame
    ↓
Write Fault
    ↓
Private Copy
```

Die COW-Semantik wird durch `NPSPEC-MEMORY-COW-0001` definiert.

## Device Mapping

Hardware-Ressourcen dürfen kontrolliert in Address Spaces eingebunden werden.

```text
Device Memory / MMIO
        ↓
Device Mapping
        ↓
Authorized Address Space
```

Dabei müssen:

```text
Capability
Permissions
Cache Policy
Device Ownership
```

berücksichtigt werden.

Normale Prozesse dürfen physische Gerätebereiche nicht beliebig mappen.

## Alignment

Mappings müssen die Anforderungen der verwendeten Page-Größe erfüllen.

```text
Virtual Address
Backing Offset
Mapping Length
```

müssen entsprechend der Mapping- und Architekturregeln ausgerichtet oder kontrolliert normalisiert werden.

## NUMA

Mappings dürfen NUMA-Policies besitzen.

```text
Mapping
  ↓
NUMA Policy
  ↓
Demand Allocation
  ↓
Physical Placement
```

Die Policy beeinflusst die physischen Backing Frames, nicht die virtuelle Adresse selbst.

## Concurrency

Mapping-Operationen müssen gegenüber konkurrierenden Änderungen geschützt sein.

Dies betrifft insbesondere:

```text
Map
Unmap
Remap
Protect
Page Fault
COW
Migration
```

Ein Task darf keinen teilweise konstruierten oder zerstörten Mapping-Zustand beobachten.

## Lifecycle

```text
Create
  ↓
Active
  ↓
Modify / Fault / Protect
  ↓
Unmap
  ↓
Release
```

Backing Objects müssen während der gesamten Mapping-Lebensdauer gültig referenziert bleiben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
MappingID
AddressSpaceID
VirtualRange
BackingType
Permissions
SharingMode
ResidentPages
DemandPages
PageSize
NUMAPolicy
```

Sicherheitskritische Informationen müssen entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Memory Mappings als eigenständige semantische Objekte verwalten können.
2. Ein Mapping MUSS Virtual Range, Backing, Permissions und Mapping-Semantik eindeutig beschreiben.
3. Mapping und physische Speicherallokation MÜSSEN logisch getrennt bleiben.
4. Anonymous, Private und Shared Mappings MÜSSEN unterstützt werden können.
5. Object-backed Mappings SOLLEN unabhängig vom konkreten Backing-Object-Typ funktionieren.
6. Mapping-Permissions DÜRFEN die Berechtigungen des Backing Objects NICHT erweitern.
7. Unmapping DARF ein weiterhin referenziertes Backing Object oder Shared Frame NICHT automatisch zerstören.
8. Teilweises Unmapping und Protection Changes MÜSSEN konsistente Mapping-Metadaten erhalten.
9. Mapping-Operationen MÜSSEN SMP-sicher sein.
10. Demand Paging und Copy-on-Write MÜSSEN über Mapping-Metadaten eindeutig auflösbar sein.
11. Device Mappings MÜSSEN explizit autorisiert und mit korrekter Cache-Policy eingerichtet werden.
12. Mappings SOLLEN sicherheitsgefiltert introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-PAGETABLE-0001`
- `NPSPEC-MEMORY-TLB-0001`
- `NPSPEC-MEMORY-DEMANDPAGING-0001`
- `NPSPEC-MEMORY-COW-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-MEMORY-0016`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Virtual Address Space
        ↓
Memory Mapping
        ↓
Backing Object
        ↓
Demand / COW / Shared Policy
        ↓
Page Tables
        ↓
Physical Frames
```

NovaOS erhält damit eine einheitliche Mapping-Abstraktion, die virtuelle Speicherbereiche mit anonymem, gemeinsamem, objektbasiertem oder gerätebezogenem Speicher verbindet und dabei Paging, Demand Paging, COW, Permissions und NUMA kontrolliert integriert.