# NPSPEC-MEMORY-PAGETABLE-0001 – Nova Page Table Management

## Status

Angenommen

## Kategorie

Memory / Page Tables / Virtual Memory / MMU

## Zweck

NovaOS definiert Page Tables als architekturspezifische Datenstrukturen zur hardwaregestützten Übersetzung virtueller Adressen in physische Adressen.

```text
Virtual Address
      ↓
Page Table Walk
      ↓
Page Table Entry
      ↓
Physical Frame
```

Die allgemeine Virtual-Memory-Architektur bleibt von der konkreten Page-Table-Struktur der jeweiligen CPU-Architektur getrennt.

## Grundprinzip

Page Tables sind die Hardware-Repräsentation der vom Virtual Memory Manager definierten Mappings.

```text
VAS / VMM
    ↓
Logical Mapping
    ↓
Page Table Manager
    ↓
Architecture Page Tables
    ↓
MMU
```

Dabei gilt:

```text
Page Table ≠ Virtual Address Space
Page Table Entry ≠ Physical Frame
```

Der VAS beschreibt die logische Speicherstruktur.

Page Tables setzen diese Struktur für die jeweilige Hardware um.

## Page-Table-Hierarchie

NovaOS muss mehrstufige Page Tables unterstützen können.

Konzeptionell:

```text
Root Table
   ↓
Level 1
   ↓
Level 2
   ↓
Level N
   ↓
Page Table Entry
   ↓
Physical Frame
```

Anzahl und Aufbau der Ebenen sind architekturabhängig.

Die allgemeine Memory-API darf keine feste Anzahl von Ebenen voraussetzen.

## Page Table Entry

Ein Page Table Entry (PTE) beschreibt mindestens:

```text
Physical Frame
Present State
Access Permissions
```

Architekturabhängig können weitere Eigenschaften vorhanden sein:

```text
Read
Write
Execute
User / Kernel
Accessed
Dirty
Global
Cache Policy
Page Size
Architecture Flags
```

Softwaredefinierte Metadaten dürfen verwendet werden, sofern sie nicht mit Hardwaresemantik kollidieren.

## Root Page Table

Jeder hardwareisolierte Address Space besitzt eine definierte Root-Page-Table-Struktur.

```text
AddressSpace
     ↓
PageTableRoot
     ↓
Page Tables
```

Der Scheduler beziehungsweise Architecture Layer muss beim Address-Space-Wechsel die entsprechende Hardwarekonfiguration aktivieren können.

## Mapping

Beim Erstellen eines Mappings muss der Page Table Manager erforderliche Zwischenstrukturen erzeugen können.

```text
Map Virtual Page
       ↓
Walk Page Tables
       ↓
Missing Intermediate Table?
       ↓
Allocate Table
       ↓
Create PTE
```

Neue Page-Table-Seiten werden über die physische Speicherverwaltung bereitgestellt.

## Unmapping

Beim Entfernen eines Mappings:

```text
Locate PTE
    ↓
Remove Mapping
    ↓
Invalidate Translation
    ↓
Optional Table Cleanup
```

Leere Zwischen-Page-Tables dürfen freigegeben werden, sofern sie nicht mehr benötigt werden.

Das Entfernen des PTE bedeutet nicht automatisch die Freigabe des gemappten Frames.

```text
Remove PTE ≠ Free Frame
```

## Protection Changes

Berechtigungen bestehender Mappings müssen geändert werden können.

```text
Read/Write
    ↓
Protect
    ↓
Read-Only
```

Nach einer relevanten Änderung müssen betroffene gecachte Translationen invalidiert werden.

## Huge Pages

Page Tables müssen von der Hardware unterstützte größere Page-Größen abbilden können.

```text
Normal Page
Huge Page
Architecture-specific Large Page
```

Ein Huge-Page-Mapping kann Teile der normalen Page-Table-Hierarchie überspringen.

Die konkrete Umsetzung bleibt architekturabhängig.

## Page-Table-Speicher

Page Tables benötigen selbst physischen Speicher.

```text
Frame Allocator
      ↓
Page-Table Frame
      ↓
Page Table Manager
```

Dieser Speicher muss:

```text
Reserved
Tracked
Protected
```

werden.

Normale Prozesse dürfen Page-Table-Speicher nicht direkt verändern.

## Concurrency

Page Tables können gleichzeitig durch mehrere CPUs verwendet werden.

Operationen wie:

```text
Map
Unmap
Protect
Fault Resolution
Migration
```

müssen ausreichend synchronisiert sein.

Ein anderer CPU-Kern darf keinen teilweise aktualisierten Mapping-Zustand beobachten.

## Atomic Updates

Soweit die Architektur dies erfordert oder ermöglicht, sollen einzelne PTE-Änderungen atomar erfolgen.

```text
Old PTE
   ↓
Atomic Update
   ↓
New PTE
```

Mehrstufige Änderungen benötigen zusätzliche Synchronisation.

## TLB-Kohärenz

Nach Mapping- oder Protection-Änderungen müssen veraltete TLB-Einträge berücksichtigt werden.

```text
Page Table Change
       ↓
Affected CPUs
       ↓
TLB Invalidation
```

NovaOS soll unnötige globale TLB-Invalidierungen vermeiden.

## TLB Shootdown

Auf SMP-Systemen muss der Page Table Manager bestimmen können, welche CPUs einen Address Space aktuell oder kürzlich verwendet haben.

```text
AddressSpace
     ↓
Active CPU Set
     ↓
Targeted TLB Shootdown
```

Nur betroffene CPUs sollen invalidiert werden, sofern die Hardware dies ermöglicht.

## Address-Space Identifier

Unterstützt die Hardware ASIDs, PCIDs oder vergleichbare Identifikatoren, dürfen Page Tables damit verbunden werden.

```text
PageTableRoot
     +
AddressSpace Identifier
```

Dadurch können TLB-Einträge mehrerer Address Spaces gleichzeitig erhalten bleiben.

Identifier müssen kontrolliert wiederverwendet werden, damit keine veralteten Translationen einem falschen Address Space zugeordnet werden.

## Kernel Mappings

Kernel-Mappings dürfen abhängig von der Architektur:

```text
Shared
Per-Address-Space
Hybrid
```

organisiert werden.

Gemeinsam verwendete Kernel-Mappings müssen bei Änderungen über alle betroffenen Page Tables konsistent bleiben.

## User / Kernel Isolation

Page Tables müssen die Trennung zwischen User- und Kernel-Speicher durchsetzen können.

```text
User Mode
   ↓
User Mapping → Allowed
Kernel Mapping → Denied
```

Hardwarebasierte Schutzmechanismen sollen genutzt werden, sofern verfügbar.

## Page Fault Integration

Der Page Fault Handler verwendet Page-Table-Zustände zur Bestimmung der Fault-Ursache.

```text
Page Fault
    ↓
Inspect Mapping
    ↓
Not Present?
Permission?
Copy-on-Write?
Guard?
Invalid?
```

Der Page Table Manager stellt dafür die erforderlichen Mapping-Informationen bereit.

## Fehlerbehandlung

Inkonsistente Page Tables stellen einen kritischen Systemzustand dar.

Beispiele:

```text
Invalid Physical Frame
Corrupted Entry
Invalid Permission Combination
Broken Hierarchy
Unexpected Mapping
```

NovaOS muss solche Zustände in kritischen Bereichen erkennen und kontrolliert behandeln können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
PageTableRoot
Allocated Page-Table Frames
Page-Table Levels
Mapping Count
Page Sizes
Permission Distribution
TLB Invalidations
Shootdown Count
Page-Table Memory Usage
```

Direkte physische Adressen und sicherheitskritische Mapping-Informationen müssen entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Page Tables über eine architekturabhängige Implementierung hinter einer allgemeinen Memory-Abstraktion verwalten.
2. Die allgemeine Memory-Architektur DARF keine feste Anzahl von Page-Table-Ebenen voraussetzen.
3. Jeder hardwareisolierte Address Space MUSS eine eindeutig bestimmbare Page-Table-Root besitzen.
4. Page-Table-Speicher MUSS durch die physische Speicherverwaltung kontrolliert bereitgestellt werden.
5. Map-, Unmap- und Protection-Operationen MÜSSEN SMP-sicher sein.
6. Andere CPUs DÜRFEN keine teilweise aktualisierten Page-Table-Zustände beobachten.
7. Leere Page-Table-Strukturen SOLLEN kontrolliert freigegeben werden können.
8. Das Entfernen eines PTE DARF den zugehörigen physischen Frame NICHT automatisch freigeben.
9. Relevante Mapping-Änderungen MÜSSEN mit korrekter TLB-Invalidierung verbunden sein.
10. TLB Shootdowns SOLLEN auf tatsächlich betroffene CPUs begrenzt werden.
11. User- und Kernel-Zugriffsrechte MÜSSEN hardwaregestützt durchgesetzt werden, sofern die Architektur dies unterstützt.
12. Page-Table-Zustände und Translation-Aktivitäten SOLLEN sicherheitsgefiltert introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `ADR-MEMORY-0012`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Virtual Address Space
        ↓
Logical Mapping
        ↓
Page Table Manager
        ↓
Architecture Page Tables
        ↓
MMU / TLB
        ↓
Physical Frames
```

NovaOS erhält damit eine architekturunabhängig nutzbare Page-Table-Verwaltung, die logische VAS-Mappings sicher auf hardwareabhängige MMU-Strukturen überträgt und SMP, TLB-Kohärenz, Isolation sowie unterschiedliche Page-Größen berücksichtigt.