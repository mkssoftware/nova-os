# NPSPEC-MEMORY-DEMANDPAGING-0001 – Nova Demand Paging

## Status

Entwurf

## Kategorie

Memory / Virtual Memory / Demand Paging / Lazy Allocation

## Zweck

NovaOS definiert Demand Paging als Mechanismus zur verzögerten Bereitstellung physischer Speicherseiten erst bei tatsächlichem Zugriff.

```text
Reserve Virtual Memory
        ↓
No Physical Frame
        ↓
First Access
        ↓
Page Fault
        ↓
Resolve Backing
        ↓
Create Mapping
```

Dadurch können virtuelle Address Spaces größer als der unmittelbar belegte physische Speicher sein und physische Ressourcen nur für tatsächlich verwendete Daten eingesetzt werden.

## Grundprinzip

Die Reservierung virtuellen Speichers führt nicht automatisch zur Allokation physischer Frames.

```text
Virtual Reservation
        ≠
Physical Allocation
```

Stattdessen wird ein gültiger virtueller Bereich zunächst ohne präsentes Mapping angelegt.

```text
Virtual Region
     ↓
Not Present
     ↓
Access
     ↓
Demand Fault
```

Der Page Fault Handler entscheidet anschließend, ob und wie die Seite bereitgestellt wird.

## Demand-Paging-Zustand

Eine virtuelle Seite kann konzeptionell folgende Zustände besitzen:

```text
Reserved
Not Present
Resolving
Present
Failed
```

Optional kann das Backing zusätzlich klassifiziert werden:

```text
Anonymous
File-backed
Shared
Executable
Zero-backed
Persistent
```

## Demand Fault

Ein Zugriff auf eine gültige, aber noch nicht präsente Seite erzeugt einen Demand Fault.

```text
Memory Access
     ↓
Page Not Present
     ↓
Valid VAS Region?
   ┌──────┴──────┐
  No             Yes
   ↓              ↓
Invalid Fault   Resolve Page
```

Ein Demand Fault ist damit kein normaler Speicherfehler, sondern Teil der vorgesehenen Speicherverwaltung.

## Fault Resolution

Die Auflösung erfolgt konzeptionell:

```text
Validate Address
      ↓
Validate Permissions
      ↓
Determine Backing
      ↓
Acquire / Allocate Frame
      ↓
Populate Content
      ↓
Install Mapping
      ↓
TLB Handling
      ↓
Resume Task
```

Kann eine Stufe nicht erfolgreich abgeschlossen werden, muss der Fault kontrolliert fehlschlagen.

## Anonymous Memory

Für anonymen Speicher kann beim ersten Zugriff ein neuer Frame bereitgestellt werden.

```text
Anonymous Page
      ↓
First Access
      ↓
Allocate Frame
      ↓
Initialize
      ↓
Map
```

Neu bereitgestellter Speicher muss entsprechend der Sicherheitsregeln initialisiert sein.

Ein Prozess darf keine Daten eines vorherigen Frame-Besitzers erhalten.

## Zero Pages

NovaOS darf gemeinsame schreibgeschützte Zero Pages verwenden.

```text
Multiple Virtual Pages
         ↓
Shared Zero Frame
```

Beim ersten Schreibzugriff:

```text
Write Fault
    ↓
Allocate Private Frame
    ↓
Zero / Copy
    ↓
Writable Mapping
```

Dies reduziert unnötige physische Allokationen für noch unbeschriebenen Speicher.

## File-Backed Memory

Demand Paging darf Daten aus einem Backing Object laden.

```text
Virtual Page
     ↓
Page Fault
     ↓
File / Object Offset
     ↓
Storage / Cache
     ↓
Physical Frame
     ↓
Mapping
```

I/O-Fehler müssen vom normalen Demand Fault unterscheidbar sein.

## Executable Images

Programme und Bibliotheken dürfen ebenfalls bedarfsgesteuert geladen werden.

```text
Executable Image
      ↓
Mapped Region
      ↓
First Instruction/Data Access
      ↓
Demand Paging
```

Dadurch müssen nicht verwendete Teile eines Programms nicht unmittelbar in physischen Speicher geladen werden.

## Shared Memory

Mehrere Address Spaces dürfen auf dasselbe demand-geladene Backing Object zugreifen.

```text
VAS A ─┐
       ├→ Backing Object → Physical Frame
VAS B ─┘
```

Die Sharing- und Permission-Semantik muss erhalten bleiben.

## Concurrent Faults

Mehrere Tasks können gleichzeitig dieselbe noch nicht vorhandene Seite anfordern.

```text
Task A ─┐
Task B ─┼→ Same Virtual Page
Task C ─┘
```

NovaOS muss verhindern, dass dadurch mehrere widersprüchliche Frames oder Mappings erzeugt werden.

Konzeptionell:

```text
First Resolver → Resolving
Other Tasks    → Wait / Join Resolution
```

## NUMA

Die Frame-Auswahl bei einem Demand Fault soll NUMA-Policies berücksichtigen.

```text
Demand Fault
     ↓
VAS NUMA Policy
     ↓
Task / CPU Locality
     ↓
Frame Allocation
```

First-Touch-Allokation darf dadurch natürlich umgesetzt werden.

## Memory Pressure

Demand Paging reduziert initialen Speicherverbrauch, kann aber physische Allokationen in spätere Ausführungspfade verschieben.

Bei Memory Pressure kann eine Demand-Allokation:

```text
Reclaim
Fallback
Remote Allocate
Fail
```

auslösen, sofern die jeweilige Policy dies erlaubt.

## Realtime

Unkontrolliertes Demand Paging ist für Hard-Realtime-Pfade ungeeignet.

```text
Page Fault
   ↓
Allocation / I/O / Reclaim
   ↓
Unbounded Latency
```

Hard-Realtime-Speicher muss daher vorab resident gemacht oder über entsprechend garantierte Ressourcen bereitgestellt werden.

```text
Hard Realtime
     ↓
Pre-Fault / Preallocate
     ↓
Resident Pages
```

## Prefaulting

NovaOS darf virtuelle Bereiche gezielt vorab materialisieren.

```text
Virtual Region
     ↓
Prefault
     ↓
Allocate / Load Pages
     ↓
Resident
```

Dies ist insbesondere für Realtime, kritische Services und vorhersehbare Workloads relevant.

## Prefetching

Der Memory Manager darf bei erkannten Zugriffsmustern zusätzliche Seiten vorbereiten.

```text
Fault Page N
    ↓
Sequential Pattern
    ↓
Prefetch N+1, N+2
```

Prefetching ist eine Soft Optimization.

Es muss bei Memory Pressure reduziert oder deaktiviert werden können.

## Fehlerbehandlung

Demand Paging muss mindestens folgende Fehler unterscheiden:

```text
Invalid Address
Permission Violation
Backing Object Failure
I/O Failure
Allocation Failure
OOM
Corrupted Backing
```

Ein nicht auflösbarer Fault darf nicht zu einer endlosen Fault-Schleife führen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Demand Fault Count
Resolved Faults
Failed Faults
Anonymous Faults
File-backed Faults
Zero-Page Faults
Prefaulted Pages
Prefetched Pages
Fault Latency
NUMA Placement
```

## Normative Anforderungen

1. NovaOS MUSS virtuelle Speicherreservierung ohne sofortige physische Frame-Allokation unterstützen können.
2. Ein Zugriff auf eine gültige nicht präsente Seite MUSS als auflösbarer Demand Fault behandelbar sein.
3. Demand Faults MÜSSEN von ungültigen Speicherzugriffen unterscheidbar sein.
4. Die Fault-Auflösung MUSS Address Range und Permissions vor der Bereitstellung validieren.
5. Neu bereitgestellte Frames DÜRFEN keine Daten vorheriger Sicherheitskontexte offenlegen.
6. Anonymous, Shared und Backing-Object-basierte Demand Pages SOLLEN unterstützt werden.
7. Gleichzeitige Faults derselben Seite DÜRFEN keine widersprüchlichen Mappings erzeugen.
8. NUMA-Policies SOLLEN bei der physischen Frame-Auswahl berücksichtigt werden.
9. Demand Paging MUSS mit Memory Pressure und OOM kontrolliert interagieren.
10. Hard-Realtime-Speicher SOLL vor der zeitkritischen Ausführung resident gemacht werden können.
11. Prefetching DARF harte Ressourcenanforderungen NICHT verdrängen.
12. Demand-Fault-Ursachen, Latenzen und Ergebnisse SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-MEMORY-OOM-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-PAGETABLE-0001`
- `NPSPEC-MEMORY-TLB-0001`
- `ADR-MEMORY-0014`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Virtual Reservation
        ↓
First Access
        ↓
Demand Fault
        ↓
Validate
        ↓
Resolve Backing
        ↓
Allocate / Load Frame
        ↓
Install Mapping
        ↓
Resume Execution
```

NovaOS erhält damit eine bedarfsgesteuerte Speicherverwaltung, die physische Ressourcen erst bei tatsächlicher Nutzung bereitstellt und dabei VAS, Paging, NUMA, Memory Pressure und Realtime-Anforderungen kontrolliert miteinander verbindet.