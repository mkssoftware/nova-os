# ADR-MEMORY-0008 – Shared Memory über explizite Shared Objects

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Shared Memory / IPC / Zero-Copy / Isolation

## Kontext

`ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain` definiert getrennte Virtual Address Spaces für isolierte Execution Domains.

Trotz dieser Isolation müssen Domains Daten effizient gemeinsam verwenden können.

Typische Anwendungsfälle sind:

- IPC,
- Zero-Copy-Pipelines,
- gemeinsam genutzte Buffers,
- Producer-/Consumer-Systeme,
- Multimedia-Pipelines,
- GPU-/NPU-/Device-Zugriffe,
- Memory-Mapped Data,
- gemeinsam verwendete Caches.

Shared Memory darf dabei nicht als implizite Ausnahme von der Domain-Isolation entstehen.

Insbesondere soll NovaOS keine Architektur verwenden, bei der Prozesse lediglich globale Namen, physische Adressen oder bekannte virtuelle Adressen verwenden, um Speicher gemeinsam zugänglich zu machen.

Shared Memory benötigt stattdessen eine explizite systemweite Identität mit kontrollierter:

- Ownership,
- Authority,
- Protection,
- Lifetime,
- Resource Accounting,
- Data Sovereignty,
- Introspection.

---

## Entscheidung

NovaOS modelliert Shared Memory ausschließlich über explizite **Shared Memory Objects**.

Ein Shared Memory Object ist ein vom Virtual Address Space unabhängiges Kernelobjekt, das kontrolliert in mehrere Address Spaces gemappt werden kann.

```text
                  Shared Memory Object
                  /        |         \
                 /         |          \
                ↓          ↓           ↓
           Domain A    Domain B      Device
              │           │
              ↓           ↓
           Mapping     Mapping
```

Die Domains müssen das Objekt nicht an derselben virtuellen Adresse einblenden.

```text
Domain A: 0x40000000 ──┐
                       ├── Shared Object X
Domain B: 0x7A200000 ──┘
```

Die Identität des gemeinsamen Speichers liegt im Shared Object und nicht in einer Adresse.

---

## Grundprinzip

```text
Isolation by default.

Sharing by explicit object.

Authority controls mapping.

The object carries identity,
not the virtual address.
```

---

## Shared Memory Object

Ein Shared Memory Object beschreibt eine gemeinsam nutzbare Speicherressource.

Konzeptionell besitzt es:

```text
Shared Memory Object
    ├── Identity
    ├── Size
    ├── Ownership
    ├── Access Authority
    ├── Memory Policy
    ├── Backing
    ├── Lifetime
    ├── Accounting
    └── Mappings
```

Die konkrete interne Struktur wird in den zugehörigen NPSPECs spezifiziert.

---

## Objektidentität

Shared Memory Objects besitzen eine stabile systeminterne Objektidentität.

Diese Identität ist unabhängig von:

- virtueller Adresse,
- physischer Adresse,
- NUMA Node,
- aktueller Execution Domain,
- konkretem Mapping.

Damit kann physisches Backing verändert oder migriert werden, ohne die logische Identität des Shared Objects zu ändern.

---

## Explizite Erstellung

Shared Memory entsteht durch eine explizite Operation.

Konzeptionell:

```text
Create Shared Object
        ↓
Define Properties
        ↓
Acquire Authority
        ↓
Map into Domain
```

Ein normaler privater Speicherbereich wird nicht allein dadurch global geteilt, dass eine andere Domain dessen Adresse kennt.

---

## Explizites Mapping

Jede teilnehmende Execution Domain benötigt ein eigenes Mapping auf das Shared Object.

```text
Shared Object
    │
    ├── Mapping A: Read/Write
    ├── Mapping B: Read
    └── Mapping C: Read/Write
```

Mappings dürfen unterschiedliche Zugriffsrechte besitzen.

Dadurch kann beispielsweise ein Producer schreiben, während mehrere Consumer nur lesen dürfen.

---

## Authority und Capabilities

Das Recht zur Nutzung eines Shared Objects wird durch explizite Authority beziehungsweise Capabilities kontrolliert.

Mögliche Rechte umfassen konzeptionell:

```text
Map Read
Map Write
Transfer
Delegate
Resize
Manage
Destroy
```

Der Besitz einer Object-ID allein gewährt keine Berechtigung.

Capabilities dürfen mit eingeschränkten Rechten delegiert werden.

Beispiel:

```text
Owner
  ↓
Read/Write Capability
  ↓
Delegate Read-only Capability
  ↓
Consumer
```

---

## Least Privilege

Eine Domain erhält nur die für ihre Aufgabe notwendigen Mapping-Rechte.

Beispiel:

```text
Camera Service
    ↓ Write
Shared Frame Object
    ↓ Read
Image Analyzer
```

Der Image Analyzer benötigt dadurch keine Schreibberechtigung auf den Eingabepuffer.

---

## Ownership und Lifetime

Ownership und Mapping sind getrennte Konzepte.

Eine Domain kann ein Shared Object mappen, ohne dessen Owner zu sein.

```text
Owner Domain
     │
     ↓
Shared Object
   ↙     ↘
Mapping   Mapping
A          B
```

Das Entfernen eines einzelnen Mappings beendet deshalb nicht automatisch das Shared Object.

Die Lifetime wird über explizite Ownership- und Reference-Semantik verwaltet.

---

## Domain-Ende

Wird eine Execution Domain beendet:

```text
Domain exits
     ↓
Remove its mappings
     ↓
Release references
     ↓
Shared Object remains
if valid ownership/references remain
```

Ein Shared Object darf somit die Lebensdauer einer einzelnen Domain übersteigen.

Existieren keine gültigen Owner oder Referenzen mehr, kann das Backing freigegeben werden.

---

## Zero-Copy

Shared Memory Objects sind ein zentraler Mechanismus für Zero-Copy zwischen Execution Domains.

```text
Producer
    ↓
Shared Memory Object
    ↓
Consumer
```

Anstatt Daten:

```text
Producer Buffer
      ↓ copy
Kernel Buffer
      ↓ copy
Consumer Buffer
```

zu kopieren, können beide Domains kontrollierte Mappings desselben Memory Objects erhalten.

Zero-Copy ist dabei keine Ausnahme von Security oder Ownership.

---

## Ownership Transfer

Für Pipelines darf die logische Nutzungs-Ownership eines Shared Objects beziehungsweise Buffers übertragen werden.

Beispiel:

```text
Producer owns writable buffer
        ↓
Publish
        ↓
Consumer receives ownership
```

Der Transfer kann Mapping-Rechte entsprechend verändern.

Dadurch können Daten ohne Kopie weitergegeben werden, während gleichzeitig klar bleibt, welche Komponente aktuell schreiben darf.

---

## Shared Mutable State

Shared writable Memory erzeugt Synchronisationsbedarf.

```text
Shared Memory
      +
Multiple Writers
      =
Synchronization Requirement
```

Shared Memory Objects stellen ausschließlich den gemeinsamen Speicher bereit.

Sie definieren nicht automatisch:

- Mutex-Semantik,
- Atomics,
- Futex-Synchronisation,
- RCU,
- Lock-Free-Protokolle.

Die jeweilige Datenstruktur muss ein explizites Synchronisationsmodell besitzen.

---

## Memory Ordering

Shared Memory verändert nicht die CPU-Memory-Ordering-Regeln.

Wenn mehrere CPUs auf dasselbe Shared Object zugreifen, müssen Atomics und Memory Barriers entsprechend der Synchronisationssemantik verwendet werden.

Shared Mapping allein erzeugt keine Happens-Before-Beziehung.

---

## Futex-artige Synchronisation

Die futex-artige User/Kernel-Synchronisation aus `ADR-SYNC-0004` kann auf geeigneten Shared Memory Objects verwendet werden.

Dadurch können mehrere Domains:

```text
Shared State
     +
Userspace Atomics
     +
Kernel Wait/Wake
```

effizient koordinieren.

Die Futex-Identität muss dabei auf dem gemeinsamen Memory Object beziehungsweise dessen stabiler Shared-Memory-Identität basieren und darf nicht lediglich eine domainlokale virtuelle Adresse verwenden.

---

## Resource Accounting

Shared Memory benötigt explizites Resource Accounting.

Dabei muss zwischen mindestens folgenden Rollen unterschieden werden können:

```text
Owner
Physical Backing
Mapped User
Pinned User
Reservation
```

Ein physischer Shared Buffer darf nicht einfach vollständig jeder Mapping-Domain als exklusiver Speicherverbrauch zugerechnet werden.

Gleichzeitig dürfen Shared Objects nicht verwendet werden, um Memory Budgets zu umgehen.

Die konkrete Accounting-Policy wird in einer NPSPEC definiert.

---

## Memory Pressure

Shared Objects nehmen an der systemweiten Memory-Pressure-Architektur teil.

Relevant ist insbesondere, ob Backing:

```text
Resident
Reclaimable
Pinned
Device-bound
Lazy
```

ist.

Ein häufig gemapptes Shared Object ist nicht automatisch reclaimbar.

Pressure-Entscheidungen müssen Lifetime, Ownership und aktive Nutzer berücksichtigen.

---

## Lazy Allocation

Shared Memory Objects dürfen Lazy Allocation verwenden, sofern ihr Contract dies zulässt.

```text
Create 1 GiB Shared Object
        ↓
Virtual Mappings
        ↓
Physical backing materialized
on demand
```

Die Materialisierung muss bei konkurrierenden Page Faults race-safe erfolgen.

Eine einmal gemeinsam materialisierte Page muss für alle relevanten Mappings konsistent dasselbe logische Backing repräsentieren.

---

## Copy-On-Write

Shared Memory und Copy-On-Write besitzen unterschiedliche Semantik.

```text
Shared Writable Memory
        ↓
Changes visible to participants
```

gegenüber:

```text
Copy-On-Write
        ↓
Write creates private divergence
```

Ein Shared Object darf COW-basierte Snapshots oder private Ableitungen besitzen, aber COW darf nicht versehentlich die vereinbarte Shared-Memory-Semantik verändern.

---

## NUMA Placement

Shared Memory besitzt häufig mehrere Nutzer mit unterschiedlichen Locality-Anforderungen.

Die Physical-Memory-Policy muss deshalb mögliche Nutzungstopologien berücksichtigen.

Beispiel:

```text
Domain A → NUMA 0
Domain B → NUMA 1
        ↓
Shared Object
```

Mögliche Strategien umfassen:

```text
Home Node
Interleaving
Partitioning
Replication for read-mostly data
Device-local placement
```

Die konkrete Auswahl ist Policy.

Required Placement Constraints bleiben verbindlich.

---

## Device- und Accelerator-Sharing

Shared Memory Objects dürfen auch als Grundlage für kontrolliert gemeinsam verwendeten Speicher zwischen:

- CPU,
- GPU,
- NPU,
- NIC,
- anderen DMA-fähigen Devices

dienen.

```text
CPU Domain
     ↓
Shared Memory Object
     ↓
IOMMU Mapping
     ↓
Device
```

Device-Zugriff benötigt eine explizite, durch DMA/IOMMU-Policy autorisierte Mapping-Beziehung.

Ein CPU-Mapping allein gewährt einem Device keinen Zugriff.

---

## Cache Coherency

NovaOS darf nicht voraussetzen, dass jede Hardwareplattform vollständig kohärenten Shared Memory zwischen allen CPUs und Devices bereitstellt.

Shared Objects müssen deshalb Eigenschaften der zugrunde liegenden Memory Domain beziehungsweise Device-Coherency berücksichtigen können.

Notwendige:

- Cache Maintenance,
- Synchronisation,
- Ownership Transitions

werden durch die entsprechenden HAL-, DMA- und Memory-Spezifikationen definiert.

---

## Data Sovereignty

Shared Memory stellt einen expliziten Informationsfluss zwischen Domains dar.

Vor dem Erzeugen beziehungsweise Delegieren eines Zugriffs muss deshalb geprüft werden, ob der Informationsfluss mit der Data-Sovereignty-Policy vereinbar ist.

```text
Domain A
   ↓
Shared Object
   ↓
Domain B

=> Information Flow
```

Ein technisch mögliches Mapping ist nicht automatisch ein zulässiges Mapping.

---

## Security und Revocation

Mapping-Rechte müssen kontrolliert widerrufbar sein, soweit der zugrunde liegende Capability Contract dies vorsieht.

Eine Revocation kann erfordern:

```text
Revoke Authority
      ↓
Prevent New Mappings
      ↓
Invalidate Existing Mapping
      ↓
TLB / Device Mapping Update
```

Die genaue Revocation-Semantik muss explizit definiert werden.

Ein widerrufenes Mapping darf nicht aufgrund veralteter Hardwareübersetzungen weiterhin nutzbar bleiben.

---

## Realtime

Realtime-Domains dürfen Shared Memory verwenden.

Für garantierte Latenz können Shared Objects:

- vorab erstellt,
- vorab gemappt,
- vorab materialisiert,
- physisch reserviert,
- gegebenenfalls gepinnt

werden.

Unkontrollierte Demand Faults oder Mapping-Erzeugung im kritischen Pfad sollen dadurch vermieden werden.

---

## Deterministic Mode

Deterministic Mode muss die Verwendung von Shared Objects reproduzierbar konfigurieren können.

Insbesondere können:

- Mapping-Reihenfolge,
- physische Placement Policy,
- Lazy Allocation,
- Ownership Transfers

festgelegt oder eingeschränkt werden.

Unkontrollierte Race Conditions in gemeinsamem Speicher werden durch Deterministic Mode nicht automatisch behoben.

Die Synchronisationssemantik bleibt Aufgabe des verwendeten Protokolls.

---

## Checkpoint und Migration

Shared Objects erschweren Checkpoint und Migration, da mehrere Execution Domains dasselbe Objekt referenzieren können.

Ein Checkpoint muss deshalb die Objektidentität erhalten.

Nicht zulässig wäre:

```text
Shared Object X
      ↓ checkpoint
Two independent copies
```

wenn die ursprüngliche Semantik gemeinsames Memory verlangt.

Stattdessen muss die Beziehung rekonstruiert werden:

```text
Domain A ──┐
           ├── Shared Object X'
Domain B ──┘
```

Für verteilte oder nicht migrierbare Shared Objects kann ein expliziter Migration Contract erforderlich sein.

---

## Location Transparency

Ein Shared Object darf durch höhere Systemschichten ortstransparent referenziert werden, ohne physische Adressen offenzulegen.

Die Kosten und Einschränkungen des tatsächlichen Backings bleiben jedoch sichtbar.

Nicht jedes Shared Object ist automatisch:

- migrierbar,
- remote nutzbar,
- device-accessible,
- gleich schnell.

Location Transparency darf diese Eigenschaften nicht verschleiern.

---

## Architecture Introspection

Shared Memory Objects müssen kontrolliert introspektierbar sein.

Relevant sind insbesondere:

```text
Object Identity
Owner
Size
Backing State
Memory Domain
Mapped Domains
Mapping Rights
Reference Count
Pinned State
Device Mappings
Resource Accounting
Data-Sovereignty Policy
```

Sensitive Mapping- und Ownership-Informationen dürfen nur autorisierten Beobachtern offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Shared Memory über explizite Shared Memory Objects modellieren.
2. Shared Memory Objects MÜSSEN unabhängig von virtuellen und physischen Adressen identifizierbar sein.
3. Jede teilnehmende Execution Domain MUSS ein explizites Mapping besitzen.
4. Unterschiedliche Domains DÜRFEN dasselbe Shared Object an unterschiedlichen virtuellen Adressen mappen.
5. Der Besitz einer Object-ID DARF keine Zugriffsberechtigung verleihen.
6. Mapping-Rechte MÜSSEN über Capabilities beziehungsweise explizite Authority kontrolliert werden.
7. Unterschiedliche Mappings desselben Objects MÜSSEN unterschiedliche Schutzrechte besitzen können.
8. Ownership, Mapping und Lifetime MÜSSEN getrennte Konzepte sein.
9. Das Ende einer einzelnen Domain DARF ein weiterhin gültig referenziertes Shared Object NICHT automatisch zerstören.
10. Shared Objects MÜSSEN für Zero-Copy zwischen Execution Domains verwendbar sein.
11. Ownership Transfer MUSS ohne notwendige Kopie des Nutzdateninhalts möglich sein.
12. Shared writable Memory MUSS ein explizites Synchronisationsprotokoll verwenden.
13. Shared Mapping allein DARF KEINE Synchronisations- oder Memory-Ordering-Garantie implizieren.
14. Futex-artige Synchronisation MUSS auf geeigneten Shared Objects möglich sein.
15. Shared Memory MUSS mit der Resource Economy und Memory Budgets integriert sein.
16. Shared Memory DARF NICHT zur Umgehung von Hard Memory Limits verwendet werden.
17. Shared Objects MÜSSEN an Memory-Pressure- und OOM-Policies teilnehmen.
18. Lazy Backing MUSS für geeignete Shared Objects möglich sein.
19. Konkurrierende Materialisierung gemeinsamen Backings MUSS race-safe erfolgen.
20. Shared-Memory- und Copy-On-Write-Semantik MÜSSEN eindeutig unterschieden werden.
21. Shared Objects MÜSSEN NUMA-aware platziert werden können.
22. CPU-/Device-Sharing MUSS über explizite DMA-/IOMMU-Autorisierung möglich sein.
23. NovaOS DARF universelle Hardware-Cache-Coherency NICHT voraussetzen.
24. Das Erteilen von Shared-Memory-Zugriff MUSS mit Data-Sovereignty- und Security-Policies vereinbar sein.
25. Revocation bestehender Zugriffe MUSS entsprechend dem Capability Contract unterstützt werden können.
26. Realtime Domains MÜSSEN Shared Objects vorab materialisieren und reservieren können.
27. Checkpoint und Migration MÜSSEN die logische Identität und Sharing-Beziehungen gemeinsamer Objects erhalten können.
28. Shared Objects, Mappings, Ownership und Resource Accounting MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Shared Memory besitzt eine klare systemweite Objektidentität,
- Execution-Domain-Isolation bleibt trotz effizientem Sharing erhalten,
- Zero-Copy-IPC wird zu einem nativen Architekturmechanismus,
- unterschiedliche Domains können unterschiedliche Zugriffsrechte erhalten,
- Shared Memory lässt sich mit Capabilities und Data Sovereignty verbinden,
- CPU-, GPU-, NPU- und Device-Sharing kann auf demselben Objektmodell aufbauen,
- Checkpoint und Migration können Sharing-Beziehungen explizit rekonstruieren.

### Negative Konsequenzen

- Lifetime- und Ownership-Verwaltung werden komplexer,
- Shared Memory benötigt explizites Resource Accounting,
- veränderlicher gemeinsamer Speicher benötigt zusätzliche Synchronisation,
- Revocation bestehender Mappings kann teure MMU-, TLB- und IOMMU-Operationen erfordern,
- NUMA-Placement ist bei Nutzern auf mehreren Nodes nicht immer eindeutig optimal.

---

## Verworfene Alternativen

### Globale Shared-Memory-Adressen

Verworfen.

Virtuelle Adressen sind domainlokal und dürfen keine systemweite Objektidentität darstellen.

### Physische Adressen als Shared-Memory-Handles

Verworfen.

Dies würde Hardwaredetails offenlegen und Isolation, Migration sowie Memory Management umgehen.

### Implizites Sharing zwischen verwandten Prozessen

Verworfen.

NovaOS benötigt explizite Authority-, Ownership- und Informationsflussbeziehungen.

### Shared Memory ausschließlich über Files

Verworfen.

File-backed Shared Memory bleibt möglich, aber nicht jeder Shared Buffer besitzt sinnvolle Dateisemantik.

### Automatische Kopie zwischen Domains

Verworfen.

Dies würde Zero-Copy-Pipelines verhindern und große Datenströme unnötig duplizieren.

### Ein globaler gemeinsamer Address Space

Verworfen.

Dies würde die Isolation des Execution-Domain-Modells grundlegend schwächen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation`
- `ADR-MEMORY-0007_Copy_On_Write_als_gezielte_Optimierung`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-PROCESS-0007_Migrierbare_Workloads`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-SYNC-0004_Futex_artige_User_Kernel_Synchronisation`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-SHARED-OBJECT-0001`
- `NPSPEC-MEMORY-SHARED-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-AUTHORITY-0001`
- `NPSPEC-MEMORY-SHARED-LIFETIME-0001`
- `NPSPEC-MEMORY-SHARED-ACCOUNTING-0001`
- `NPSPEC-MEMORY-SHARED-NUMA-0001`
- `NPSPEC-MEMORY-SHARED-DEVICE-0001`
- `NPSPEC-MEMORY-SHARED-TEST-0001`

---

## Ergebnis

NovaOS behandelt gemeinsam genutzten Speicher als explizite, kontrollierte Systemressource:

```text
                 Shared Memory Object
                 /        |         \
                /         |          \
               ↓          ↓           ↓
          Domain A    Domain B      Device
             │           │             │
          Mapping     Mapping      IOMMU Mapping
             │           │             │
             └───────────┴─────────────┘
                         ↓
                  Shared Backing
```

Dadurch bleiben Isolation, Ownership, Capabilities, Resource Accounting und Data Sovereignty erhalten, während gleichzeitig effiziente Zero-Copy-Kommunikation möglich wird.

Die zentrale Architekturregel lautet:

```text
Memory is private by default.

Sharing requires an explicit object.

Access requires explicit authority.

Share backing, not address spaces.
```