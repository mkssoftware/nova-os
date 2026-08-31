# ADR-MEMORY-0014 – Pinned Memory als budgetierte Ressource

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Resource Economy / DMA / Realtime

## Kontext

Bestimmte Speicherbereiche dürfen während ihrer Nutzung nicht verschoben, ausgelagert, komprimiert oder anderweitig aus ihrem garantierten physischen Backing entfernt werden.

Dies ist insbesondere notwendig für:

- DMA und Device-I/O,
- bestimmte Zero-Copy-Pipelines,
- Hard-Realtime-Workloads,
- Hardware- oder Treiberstrukturen,
- explizit adressgebundene Speicherbereiche.

Solcher **Pinned Memory** schränkt jedoch die Freiheitsgrade des Memory Managers erheblich ein. Große oder unbegrenzte Mengen gepinnten Speichers können Reclamation, NUMA-Balancing, Hotplug und allgemeine Systemstabilität beeinträchtigen.

Pinned Memory darf deshalb in NovaOS keine unbegrenzt verfügbare Nebenwirkung eines Mappings sein.

---

## Entscheidung

NovaOS behandelt Pinned Memory als **explizite, budgetierte und zurechenbare Systemressource**.

```text
Pin Request
    ↓
Authority + Hard Constraints
    ↓
Resource Budget
    ↓
Memory Manager
    ↓
Pinned Memory
```

Jede relevante Resource beziehungsweise Execution Domain besitzt kontrollierbare Grenzen für gepinnten Speicher.

Pinning ist eine Eigenschaft der physischen Verfügbarkeit eines Memory Backings und keine alternative Form von Ownership oder Zugriffsberechtigung.

---

## Grundprinzip

```text
Pinned memory is scarce memory.

Pin explicitly.

Account it explicitly.

Unpin as soon as possible.
```

---

## Explizites Pinning

Speicher wird nicht allein durch:

- Mapping,
- Shared Memory,
- Zero-Copy,
- Device-Nutzung

automatisch dauerhaft gepinnt.

Eine Komponente muss Pinning explizit anfordern oder über einen Mechanismus verwenden, dessen Contract Pinning ausdrücklich beinhaltet.

Die Anforderung unterliegt Authority-, Resource- und Memory-Policies.

---

## Budgetierung

Pinned Memory wird in die systemweite Resource Economy integriert.

Mindestens muss pro relevanter Resource Domain unterschieden werden können zwischen:

```text
Resident Memory
Pinned Memory
Reserved Pinned Memory
Pinned Memory Limit
```

Pinned Memory darf nicht verwendet werden, um allgemeine Memory Budgets zu umgehen.

Für kritische Systemkomponenten und Realtime-Domains können explizite Reservierungen vorgesehen werden.

---

## Pinning-Lifetime

Pinning besitzt eine definierte Lebensdauer.

```text
Acquire Pin
    ↓
Use Memory
    ↓
Release Pin
```

Temporäre Hardware- oder I/O-Operationen sollen Speicher nur für den tatsächlich erforderlichen Zeitraum pinnen.

Verwaiste Pins müssen durch Ownership- und Lifetime-Management verhindert beziehungsweise beim Ende der verantwortlichen Domain kontrolliert aufgelöst werden.

---

## DMA und Zero-Copy

DMA kann physisch verfügbares und gegebenenfalls gepinntes Backing benötigen.

Die konkrete Notwendigkeit hängt von Hardware, IOMMU, Device und Transfermodell ab.

```text
Memory Object
    ↓
Pin / Prepare
    ↓
IOMMU Mapping
    ↓
Device
```

Zero-Copy impliziert nicht automatisch dauerhaftes Pinning.

NovaOS soll kurzlebige oder vermeidbare Pins bevorzugen.

---

## NUMA und Locality

Pinned Memory kann nicht beliebig migriert werden.

Daher muss die NUMA-Placement-Entscheidung möglichst vor dem Pinning erfolgen.

Ein Pin darf Required-NUMA-, Device-Locality- oder Data-Sovereignty-Constraints nicht verletzen.

Adaptive NUMA-Migration darf gepinnte Pages nicht ohne kontrollierte Aufhebung des Pins verschieben.

---

## Memory Pressure und OOM

Pinned Memory ist grundsätzlich nicht normal reclaimbar.

Das Memory-Pressure-Modell muss deshalb den Umfang gepinnten Speichers explizit berücksichtigen.

Eine Domain mit übermäßigem Pinning kann:

- weitere Pin Requests abgelehnt bekommen,
- durch ihre Resource Policy begrenzt werden,
- kontrolliert degradiert werden.

Memory Pressure darf bestehende gültige Pins nicht still brechen.

---

## Realtime

Realtime-Domains können Pinned Memory explizit reservieren.

Damit kann ein kritisches Working Set vor Beginn der Realtime-Phase:

```text
Allocate
    ↓
Materialize
    ↓
Pin
    ↓
Enter Realtime
```

vorbereitet werden.

Realtime-Reservierungen bleiben Teil der Resource Economy und dürfen nicht durch normale Workloads verbraucht werden.

---

## Verhältnis zu anderen Memory-Optimierungen

Solange Speicher gepinnt ist, dürfen Mechanismen wie:

- Memory Compression,
- Memory Deduplication,
- Reclamation,
- Migration

ihn nicht in einer Weise verändern, die die Pinning-Garantie verletzt.

Huge Pages und Shared Memory können gepinnt werden, sofern ihre jeweiligen Contracts dies erlauben.

---

## Security und Authority

Pinning kann die Verfügbarkeit physischer Ressourcen erheblich beeinflussen und stellt deshalb eine privilegierte Ressourcenoperation dar.

Eine Domain darf nur innerhalb ihrer Authority und ihrer Resource Budgets pinnen.

Nicht vertrauenswürdige Workloads dürfen durch massenhaftes Pinning keinen systemweiten Memory-Denial-of-Service erzeugen können.

---

## Hotplug

Pinned Memory kann das Offline-Nehmen einer Memory Region verhindern.

Beim Memory Hotplug muss NovaOS versuchen:

```text
Unpin
or
Relocate after authorized transition
or
Block Hotplug
```

Eine gültige Pinning-Garantie darf nicht still gebrochen werden.

---

## Architecture Introspection

NovaOS muss Pinned Memory kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- gepinnte Bytes,
- verantwortliche Resource Domain,
- Pinning-Grund,
- Lifetime,
- NUMA-/Memory-Domain,
- Device-Bezug,
- Reservierung und Budget,
- abgelehnte Pin Requests.

Damit können Memory Pressure und Ressourcenblockaden nachvollzogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS Pinned Memory als explizite Systemressource behandeln.
2. Pinned Memory MUSS vollständig in die Resource Economy integriert sein.
3. Pinning MUSS einer verantwortlichen Resource beziehungsweise Execution Domain zurechenbar sein.
4. Domains MÜSSEN für Pinned Memory budgetiert beziehungsweise begrenzbar sein.
5. Pinning DARF allgemeine Memory Budgets NICHT umgehen.
6. Pinning MUSS eine definierte Lifetime besitzen.
7. DMA und Zero-Copy DÜRFEN Pinning verwenden, aber NICHT grundsätzlich dauerhaftes Pinning voraussetzen.
8. NUMA- und Placement-Constraints MÜSSEN vor beziehungsweise während des Pinning erhalten bleiben.
9. Gepinnter Speicher DARF während eines gültigen Pins NICHT unkontrolliert migriert, komprimiert, dedupliziert oder reclaimed werden.
10. Memory Pressure MUSS Pinned Memory als nicht normal reclaimbare Ressource berücksichtigen.
11. Realtime-Domains MÜSSEN Pinned Memory reservieren können.
12. Pinning MUSS durch Authority- und Security-Policies kontrolliert werden.
13. Memory Hotplug DARF gültige Pinning-Garantien NICHT still brechen.
14. Nutzung, Budgets, Reservierungen und Ursachen von Pinned Memory MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- DMA- und Realtime-Anforderungen erhalten verlässliches physisches Backing,
- unbegrenztes Pinning durch einzelne Workloads wird verhindert,
- Pinned Memory wird in Memory Pressure und OOM sichtbar,
- Zero-Copy bleibt möglich, ohne dauerhaft große RAM-Bereiche zu blockieren,
- NUMA-, Device- und Resource-Policies bleiben kontrollierbar.

### Negative Konsequenzen

- Pinning benötigt zusätzliches Accounting und Lifetime-Management,
- Budgetgrenzen können Pin Requests ablehnen,
- gepinnte Pages reduzieren Reclamation- und Migrationsmöglichkeiten,
- Memory Hotplug kann durch aktive Pins blockiert werden.

---

## Verworfene Alternativen

### Unbegrenztes Pinning

Verworfen.

Eine Domain könnte große Teile des physischen Speichers dauerhaft der normalen Speicherverwaltung entziehen.

### Pinning außerhalb der Resource Economy

Verworfen.

Dies würde Memory Budgets und Ressourcenverantwortung umgehen.

### Zero-Copy bedeutet automatisch dauerhaftes Pinning

Verworfen.

Zero-Copy und Pinning sind unterschiedliche Eigenschaften und dürfen nicht unnötig gekoppelt werden.

### Memory Pressure darf Pins automatisch aufheben

Verworfen.

Dies würde die Garantie des Pinning-Contracts verletzen und könnte DMA- oder Realtime-Korrektheit gefährden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-MEMORY-0009_Huge_Pages_policygesteuert`
- `ADR-MEMORY-0010_Memory_Compression_policygesteuert`
- `ADR-MEMORY-0011_Memory_Deduplication_nur_innerhalb_geeigneter_Trust_Domänen`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-PINNED-0001`
- `NPSPEC-MEMORY-PINNED-BUDGET-0001`
- `NPSPEC-MEMORY-PINNED-LIFETIME-0001`
- `NPSPEC-MEMORY-PINNED-DMA-0001`
- `NPSPEC-MEMORY-PINNED-REALTIME-0001`
- `NPSPEC-MEMORY-PINNED-TEST-0001`

---

## Ergebnis

NovaOS behandelt Pinned Memory nicht als gewöhnlichen Resident Memory, sondern als begrenzte Ressource mit besonderer Auswirkung auf das gesamte Memory Management:

```text
Memory
   ↓
Explicit Pin Request
   ↓
Authority + Budget
   ↓
Pinned Backing
   ↓
DMA / Realtime / Zero-Copy
   ↓
Explicit Unpin
```

Die zentrale Architekturregel lautet:

```text
Pin only when required.

Budget every pin.

Preserve hard guarantees.

Release pins as early as possible.
```