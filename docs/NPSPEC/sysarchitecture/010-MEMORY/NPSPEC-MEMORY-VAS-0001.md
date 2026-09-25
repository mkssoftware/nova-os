# NPSPEC-MEMORY-VAS-0001 – Nova Virtual Address Space

## Status

Angenommen

## Kategorie

Memory / Virtual Address Space / Address Isolation / Virtual Memory

## Zweck

NovaOS definiert den Virtual Address Space (VAS) als logischen Adressraum, in dem Prozesse, Kernel und andere isolierte Ausführungskontexte virtuelle Speicherbereiche verwenden.

```text
Virtual Address
      ↓
Virtual Address Space
      ↓
Page Mapping
      ↓
Physical Frame
```

Der VAS trennt die von Software verwendeten Adressen von der tatsächlichen physischen Speicherposition.

## Grundprinzip

Virtuelle und physische Adressen sind voneinander unabhängig.

```text
Virtual Address ≠ Physical Address
```

Mehrere Address Spaces dürfen dieselbe virtuelle Adresse verwenden und dabei auf unterschiedliche physische Frames zeigen.

```text
Process A
0x1000 → Physical Frame 42

Process B
0x1000 → Physical Frame 91
```

Dadurch entstehen Isolation, flexible Speicherplatzierung und kontrolliertes Sharing.

## Address Space

Ein Virtual Address Space besitzt mindestens:

```text
AddressSpaceID
Owner
AddressRange
Mappings
State
```

Optional:

```text
SecurityContext
NUMAPolicy
ResourceBudget
Generation
Statistics
```

Die Identität eines Address Spaces darf nicht allein von seiner Page-Table-Adresse abhängen.

## Address-Space-Typen

NovaOS darf unterschiedliche VAS-Klassen verwenden:

```text
Kernel Address Space
Process Address Space
Compatibility Address Space
Shared Address Space
Device / Special Mapping Space
```

Die konkrete Architektur darf gemeinsame Kernelbereiche zwischen Prozessen abbilden, sofern Isolation und Sicherheitsregeln erhalten bleiben.

## Virtuelle Regionen

Ein VAS wird in virtuelle Regionen unterteilt.

```text
Virtual Address Space
├── Code
├── Data
├── Heap
├── Shared Memory
├── Memory-Mapped Objects
├── Stack
└── Reserved Regions
```

Regionen müssen nicht unmittelbar physische Frames besitzen.

```text
Reserved Virtual Range
        ≠
Allocated Physical Memory
```

## Reservierung

Virtuelle Adressbereiche dürfen zunächst nur reserviert werden.

```text
Reserve Range
     ↓
Virtual Region exists
     ↓
No Physical Frame yet
```

Dadurch können große logische Bereiche bereitgestellt werden, ohne sofort physischen Speicher zu verbrauchen.

## Mapping

Ein Mapping verbindet virtuelle Pages mit physischen Frames oder anderen Memory Objects.

```text
Virtual Page
     ↓
Mapping
     ↓
Physical Frame
```

Ein Mapping besitzt mindestens:

```text
Virtual Range
Backing Object / Physical Frames
Permissions
Mapping Type
```

## Unmapping

Mappings müssen kontrolliert entfernt werden können.

```text
Virtual Mapping
      ↓
Unmap
      ↓
Invalidate Translation
      ↓
Release Reference
```

Das Entfernen eines Mappings bedeutet nicht automatisch, dass der zugrunde liegende physische Speicher freigegeben wird.

```text
Unmap ≠ Free Physical Memory
```

Andere Mappings oder Besitzer können weiterhin auf denselben Speicher verweisen.

## Permissions

Mappings müssen Zugriffsrechte besitzen können.

Mindestens:

```text
Read
Write
Execute
```

Zusätzliche Eigenschaften dürfen umfassen:

```text
User / Kernel
Cache Policy
Device Memory
Guard
Copy-on-Write
Shared
```

NovaOS soll das Prinzip:

```text
W^X
```

unterstützen.

Speicher soll also nicht gleichzeitig schreibbar und ausführbar sein, sofern dies nicht explizit autorisiert wurde.

## Isolation

Prozesse besitzen grundsätzlich voneinander isolierte Address Spaces.

```text
Process A VAS
      X
Process B VAS
```

Zugriff zwischen Address Spaces darf nur über explizit autorisierte Mechanismen erfolgen.

Beispiele:

```text
Shared Memory
IPC
Explicit Mapping
Capability-authorized Access
```

## Shared Memory

Ein physischer Speicherbereich darf kontrolliert in mehrere Address Spaces eingebunden werden.

```text
Process A VAS ─┐
               ├→ Shared Frames
Process B VAS ─┘
```

Jedes Mapping darf eigene Zugriffsrechte besitzen.

```text
Process A → Read/Write
Process B → Read
```

## Address Space Layout

NovaOS darf die virtuelle Speicheraufteilung dynamisch bestimmen.

```text
Low Address
    ↓
User Regions
Shared Regions
Memory-Mapped Objects
Stacks
Guard Regions
    ↓
High Address
```

Feste historische Layouts sollen nicht Teil der nativen Nova ABI sein, sofern keine Architekturabhängigkeit dies erfordert.

## Address Space Randomization

Virtuelle Regionen dürfen randomisiert platziert werden.

```text
Executable
Libraries
Stacks
Mapped Objects
```

Randomisierung ist eine zusätzliche Sicherheitsmaßnahme und ersetzt keine Memory-Safety- oder Capability-Regeln.

## Guard Regions

Nicht gemappte Guard Pages dürfen kritische Regionen voneinander trennen.

```text
Stack
 ↓
Guard Page
 ↓
Other Memory
```

Ein Zugriff auf eine Guard Region muss einen kontrollierten Fault erzeugen.

## Sparse Address Spaces

NovaOS muss große virtuelle Address Spaces unterstützen können, ohne für jeden möglichen virtuellen Bereich physische Ressourcen zu reservieren.

```text
Huge Virtual Range
      ↓
Only Used Regions Mapped
```

Metadaten sollen proportional zur tatsächlichen Nutzung skalieren.

## VAS und Page Tables

Der VAS beschreibt die logische Speicherstruktur.

Page Tables bilden diese Struktur für die jeweilige CPU-Architektur ab.

```text
VAS Model
   ↓
Virtual Memory Manager
   ↓
Page Tables
   ↓
MMU
```

Damit gilt:

```text
VAS ≠ Page Table
```

Die Page-Table-Struktur ist ein architekturabhängiges Implementierungsdetail.

## Context Switch

Beim Wechsel zwischen Address Spaces muss die aktive Hardwareübersetzung entsprechend angepasst werden.

```text
Task A / VAS A
      ↓
Context Switch
      ↓
Task B / VAS B
```

Architekturen dürfen Mechanismen wie Address-Space-Identifier verwenden, um unnötige Translation-Invalidierungen zu vermeiden.

## NUMA

Virtuelle Adressen besitzen selbst keine NUMA-Lokalität.

```text
Virtual Address
      ↓
Physical Mapping
      ↓
NUMA Node
```

NUMA-Policies wirken auf die Auswahl der physischen Backing Frames, nicht auf die virtuelle Adresse selbst.

## Lifecycle

Ein Address Space besitzt einen kontrollierten Lebenszyklus.

```text
Create
  ↓
Reserve
  ↓
Map / Unmap
  ↓
Use
  ↓
Destroy
```

Beim Zerstören müssen alle Mappings, Referenzen und zugehörigen Ressourcen kontrolliert behandelt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
AddressSpaceID
Owner
Mapped Regions
Reserved Regions
Permissions
Backing Types
Shared Mappings
Virtual Memory Usage
Mapping Count
Fault Statistics
```

Sicherheitskritische Informationen müssen entsprechend dem aufrufenden Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS virtuelle und physische Speicheradressen klar trennen.
2. Prozesse MÜSSEN grundsätzlich isolierte Virtual Address Spaces besitzen.
3. Virtuelle Bereiche MÜSSEN unabhängig von physischer Speicherallokation reservierbar sein.
4. Mappings MÜSSEN explizite Zugriffsrechte besitzen.
5. Shared Memory MUSS ausschließlich über kontrollierte Mappings bereitgestellt werden.
6. Das Entfernen eines Mappings DARF physischen Speicher NICHT automatisch freigeben, solange weitere Referenzen bestehen.
7. Guard Regions SOLLEN unterstützt werden.
8. NovaOS SOLL Write-XOR-Execute für normale ausführbare Speicherbereiche durchsetzen können.
9. Die VAS-Abstraktion DARF NICHT von einer bestimmten Hardware-Page-Table-Struktur abhängig sein.
10. NUMA-Policies MÜSSEN über die physischen Backing Frames abbildbar sein.
11. Das Zerstören eines Address Spaces MUSS alle zugehörigen Mappings kontrolliert auflösen.
12. Address Spaces und Mappings SOLLEN sicherheitsgefiltert introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-MEMORY-0010`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Process / Kernel
       ↓
Virtual Address Space
       ↓
Virtual Regions
       ↓
Mappings + Permissions
       ↓
Physical Frames
       ↓
PMM
```

NovaOS erhält damit eine hardwareunabhängige Virtual-Address-Space-Abstraktion, die Prozessisolation, kontrolliertes Sharing, flexible Speicherplatzierung und die Grundlage für die weitere virtuelle Speicherverwaltung bereitstellt.