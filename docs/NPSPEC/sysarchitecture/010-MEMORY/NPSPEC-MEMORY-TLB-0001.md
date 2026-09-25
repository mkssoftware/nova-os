# NPSPEC-MEMORY-TLB-0001 – Nova Translation Lookaside Buffer Management

## Status

Angenommen

## Kategorie

Memory / TLB / Virtual Memory / MMU / SMP

## Zweck

NovaOS definiert die Verwaltung des Translation Lookaside Buffer (TLB) als Bestandteil der virtuellen Speicherarchitektur.

```text
Virtual Address
      ↓
     TLB
   ↙     ↘
 Hit     Miss
  ↓       ↓
Physical  Page Table Walk
Address        ↓
           TLB Update
```

Ziel ist eine schnelle Adressübersetzung bei gleichzeitig garantierter Kohärenz zwischen Page Tables und den von CPUs gecachten Translationen.

## Grundprinzip

Der TLB ist ein hardwareseitiger Cache für virtuelle-zu-physische Adressübersetzungen.

```text
Page Table
    ↓
Translation
    ↓
TLB Cache
```

Dabei gilt:

```text
TLB ≠ Page Table
TLB Entry ≠ Mapping Authority
```

Die Page Tables definieren den gültigen Mapping-Zustand.

Der TLB beschleunigt lediglich dessen Nutzung.

## TLB Hit

Ist eine passende Translation vorhanden:

```text
Virtual Address
      ↓
TLB Lookup
      ↓
Hit
      ↓
Physical Address
```

Ein Page-Table-Walk ist nicht erforderlich.

## TLB Miss

Ist keine passende Translation vorhanden:

```text
Virtual Address
      ↓
TLB Miss
      ↓
Page Table Walk
      ↓
Valid Mapping?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
TLB Fill       Page Fault
```

Der konkrete Hardwareablauf ist architekturabhängig.

## TLB Entries

Ein TLB-Eintrag repräsentiert konzeptionell:

```text
Virtual Page
Physical Frame
Permissions
Page Size
AddressSpace Identifier
```

Weitere Eigenschaften sind architekturabhängig.

NovaOS darf keine feste TLB-Struktur voraussetzen.

## Mapping-Änderungen

Wird ein Mapping verändert, können vorhandene TLB-Einträge veraltet sein.

```text
Page Table
Old Mapping → New Mapping

TLB
Old Mapping
```

Vor weiterer Verwendung muss die veraltete Translation invalidiert werden.

Betroffene Operationen umfassen insbesondere:

```text
Unmap
Remap
Permission Change
Copy-on-Write Resolution
Page Migration
Page-Size Change
```

## Lokale Invalidierung

Betrifft eine Änderung nur die aktuelle CPU:

```text
Page Table Change
       ↓
Local TLB Invalidate
```

NovaOS soll möglichst nur die tatsächlich betroffene Adresse oder Region invalidieren.

## TLB Shootdown

Auf SMP-Systemen können mehrere CPUs Translationen desselben Address Spaces besitzen.

```text
CPU 0 ─┐
CPU 1 ─┼→ Same Address Space
CPU 2 ─┘
```

Ändert CPU 0 ein Mapping:

```text
CPU 0
  ↓
Modify Page Table
  ↓
Determine affected CPUs
  ↓
Send Shootdown
  ↓
Remote TLB Invalidate
  ↓
Completion
```

Die Änderung darf nicht als vollständig abgeschlossen gelten, solange relevante CPUs eine sicherheitskritisch veraltete Translation weiterverwenden können.

## Targeted Shootdown

Globale TLB-Invalidierungen sollen vermieden werden.

NovaOS soll betroffene CPUs anhand der Nutzung des jeweiligen Address Spaces bestimmen.

```text
AddressSpace
     ↓
Active / Relevant CPU Set
     ↓
Targeted Shootdown
```

Dadurch werden unnötige Inter-Processor Interrupts und TLB-Verluste reduziert.

## Shootdown Batching

Mehrere Mapping-Änderungen dürfen zusammengefasst werden.

```text
Unmap A
Unmap B
Protect C
    ↓
Batch
    ↓
Single Shootdown
```

Batching darf die korrekte Speicher- und Sicherheitssemantik nicht verletzen.

## Address-Space Identifier

Unterstützt die Hardware:

```text
ASID
PCID
oder vergleichbare Mechanismen
```

soll NovaOS diese nutzen können.

```text
TLB Entry
├── AddressSpaceID
├── Virtual Page
└── Physical Frame
```

Dadurch können Translationen verschiedener Address Spaces gleichzeitig im TLB verbleiben.

## Identifier-Reuse

Hardware-Identifier besitzen möglicherweise einen begrenzten Namensraum.

Bei Wiederverwendung muss verhindert werden:

```text
Old Address Space
      ↓
Stale TLB Entry
      ↓
Identifier Reused
      ↓
Wrong Address Space
```

Vor einer unsicheren Wiederverwendung müssen relevante Translationen invalidiert werden.

## Context Switch

Ein Context Switch zwischen Address Spaces darf abhängig von der Hardware unterschiedliche Strategien verwenden.

```text
Context Switch
      ↓
ASID available?
  ┌──────┴──────┐
 Yes            No
  ↓              ↓
Switch ID     Required
             Invalidation
```

Unnötige vollständige TLB-Flushes sollen vermieden werden.

## Huge Pages

TLB-Einträge dürfen unterschiedliche Page-Größen repräsentieren.

```text
4 KiB
2 MiB
1 GiB
Architecture-specific
```

Wird ein Huge-Page-Mapping geteilt oder verändert, müssen alle davon betroffenen Translationen korrekt invalidiert werden.

## Permission Changes

Sicherheitsrelevante Änderungen benötigen besondere Sorgfalt.

Beispiel:

```text
Read/Write
    ↓
Read-Only
```

Eine CPU darf nach Abschluss der Änderung keinen alten schreibbaren TLB-Eintrag weiterverwenden.

Dies gilt insbesondere für:

```text
Write Removal
Execute Removal
User Access Removal
Unmapping
```

## Synchronisation

Die Reihenfolge zwischen Page-Table-Änderung und TLB-Invalidierung muss durch die Architekturabstraktion korrekt umgesetzt werden.

Konzeptionell:

```text
Update Page Table
      ↓
Required Memory Ordering
      ↓
Invalidate TLB
      ↓
Wait for Required Completion
      ↓
Mapping Change Complete
```

Notwendige Memory Barriers sind architekturabhängig.

## CPU Hotplug

Beim Offline-Schalten einer CPU muss deren TLB-Zustand im globalen Shootdown-Modell berücksichtigt werden.

```text
CPU Offline
    ↓
Remove from Active Sets
    ↓
No Future Shootdown Dependency
```

Eine CPU darf nicht gleichzeitig als offline gelten und dennoch eine notwendige Shootdown-Bestätigung blockieren.

## Performance

TLB-Management ist ein kritischer Performancepfad.

NovaOS soll insbesondere minimieren:

```text
Global Flushes
Unnecessary Shootdowns
Excessive IPIs
Repeated Invalidations
Address-Space Flushes
```

Optimierungen dürfen jedoch niemals Mapping-Korrektheit oder Isolation schwächen.

## Introspection

NovaOS soll mindestens folgende Informationen bereitstellen können:

```text
TLB Invalidations
Local Invalidations
Remote Shootdowns
Shootdown Targets
Shootdown Latency
Global Flushes
Batched Invalidations
ASID / PCID Usage
```

Hardwareabhängige Hit/Miss-Statistiken dürfen eingebunden werden, sofern verfügbar.

## Normative Anforderungen

1. NovaOS MUSS TLB-Kohärenz nach relevanten Page-Table-Änderungen gewährleisten.
2. Veraltete Translationen DÜRFEN nach abgeschlossenen sicherheitsrelevanten Mapping-Änderungen NICHT weiter verwendbar sein.
3. Lokale Invalidierungen SOLLEN auf die tatsächlich betroffenen Adressbereiche begrenzt werden.
4. SMP-Systeme MÜSSEN Remote-TLB-Shootdowns unterstützen.
5. Shootdowns SOLLEN nur an tatsächlich relevante CPUs gesendet werden.
6. Mehrere Invalidierungen SOLLEN sicher gebündelt werden können.
7. Hardwarebasierte Address-Space-Identifier SOLLEN genutzt werden können.
8. Identifier-Reuse DARF keine Translation eines vorherigen Address Spaces sichtbar machen.
9. Context Switches SOLLEN unnötige vollständige TLB-Flushes vermeiden.
10. Page-Table-Update, Memory Ordering und TLB-Invalidierung MÜSSEN architekturkorrekt geordnet werden.
11. CPU-Hotplug DARF die Shootdown- und Completion-Semantik NICHT beschädigen.
12. TLB-Invalidierungen und Shootdowns SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-PAGETABLE-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-INTERRUPT-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `ADR-MEMORY-0013`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Page Table Change
       ↓
Determine Affected Translation
       ↓
Determine Affected CPUs
       ↓
Local / Remote Invalidation
       ↓
Completion
       ↓
TLB and Page Tables Coherent
```

NovaOS erhält damit ein SMP-fähiges TLB-Management, das schnelle Adressübersetzung mit korrekter Page-Table-Kohärenz verbindet und unnötige Flushes sowie systemweite Shootdowns möglichst vermeidet.