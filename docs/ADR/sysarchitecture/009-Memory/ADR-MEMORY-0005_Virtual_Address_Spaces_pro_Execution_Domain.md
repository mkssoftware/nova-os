# ADR-MEMORY-0005 – Virtual Address Spaces pro Execution Domain

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Virtual Memory / Execution Domains / Isolation

## Kontext

NovaOS definiert die **Execution Domain** als primäre kontrollierte Ausführungseinheit. Eine Execution Domain bündelt unter anderem:

- Address Space,
- Capability Set,
- Resource Budget,
- Security- und Trust-Kontext,
- Data-Sovereignty-Policy,
- Execution Scopes und Tasks.

Damit muss auch die virtuelle Speicherarchitektur an Execution Domains und nicht lediglich an klassische Prozesse gekoppelt werden.

Ein gemeinsamer globaler virtueller Adressraum würde die Isolation zwischen Domains schwächen und Fehler, Speicherzugriffe sowie Sicherheitsgrenzen unnötig miteinander koppeln.

Gleichzeitig benötigt NovaOS kontrollierte Mechanismen für:

- Shared Memory,
- Zero-Copy,
- Memory-Mapped Files,
- IPC,
- gemeinsam verwendete Systemobjekte,
- Accelerator- und Device-Mappings.

Die Architektur muss daher starke Isolation mit explizitem, kontrolliertem Sharing verbinden.

---

## Entscheidung

Jede isolierte NovaOS Execution Domain besitzt grundsätzlich einen eigenen **Virtual Address Space (VAS)**.

```text
Execution Domain
      │
      ├── Security / Capabilities
      ├── Resource Budget
      ├── Execution Scopes
      └── Virtual Address Space
                ↓
        Virtual Memory Mappings
                ↓
        Physical / Shared Objects
```

Tasks innerhalb derselben Execution Domain teilen standardmäßig denselben Virtual Address Space.

Adressräume verschiedener Domains sind voneinander isoliert.

Gemeinsamer Speicher entsteht ausschließlich durch explizite, autorisierte Mappings gemeinsamer Memory Objects.

---

## Grundprinzip

```text
Execution Domain
    owns
Virtual Address Space

Isolation is default.

Sharing is explicit.

Mappings express authority.
```

---

## Virtual Address Space als Domain-Ressource

Der VAS gehört zur Execution Domain und besitzt denselben grundlegenden Lebenszyklus.

```text
Create Domain
     ↓
Create Address Space
     ↓
Create Mappings
     ↓
Execute Tasks
     ↓
Destroy Domain
     ↓
Destroy Address Space
```

Ein Task besitzt nicht automatisch einen eigenen Address Space.

Mehrere Tasks derselben Domain können denselben Speicher sehen und gemeinsam verwenden.

---

## Isolation

Mappings einer Execution Domain sind für andere Domains standardmäßig nicht sichtbar.

Ein virtueller Adresswert besitzt daher nur innerhalb seines Address-Space-Kontexts Bedeutung.

```text
Domain A
0x100000 → Object A

Domain B
0x100000 → Object B
```

Identische virtuelle Adressen implizieren keine gemeinsame physische Ressource.

---

## Kernel Address Space

Kernel-Speicher bleibt von normalen Domain-Mappings logisch getrennt und wird durch die Architektur geschützt.

Die konkrete technische Umsetzung darf plattformabhängig sein, beispielsweise:

- getrennte Kernel-Adressbereiche,
- kontrollierte gemeinsame Kernel-Mappings,
- getrennte Page-Table-Kontexte.

Die Sicherheitssemantik darf nicht davon abhängen, welche dieser Varianten eine CPU-Architektur bevorzugt.

---

## Virtual Memory Mappings

Ein Mapping verbindet einen virtuellen Adressbereich mit einem Memory Object beziehungsweise einer geeigneten physischen Ressource.

Konzeptionell:

```text
Virtual Range
    ↓
Mapping
    ↓
Memory Object
    ↓
Physical Backing
```

Mappings besitzen explizite Eigenschaften, insbesondere:

```text
Read
Write
Execute
Shared / Private
Memory Type
Backing Object
```

Weitere Attribute werden in den zugehörigen NPSPECs definiert.

---

## Memory Objects

Shared Memory und Zero-Copy werden nicht durch implizit gemeinsame virtuelle Adressräume realisiert.

Stattdessen verwendet NovaOS explizite Memory Objects.

```text
Memory Object
   ├── Mapping → Domain A
   ├── Mapping → Domain B
   └── Mapping → Device
```

Dadurch bleiben:

- Ownership,
- Zugriffsrechte,
- Lebensdauer,
- Accounting,
- Sharing-Beziehungen

explizit nachvollziehbar.

---

## Shared Memory

Eine Domain darf ein Memory Object nur dann mappen, wenn sie die dafür notwendige Authority besitzt.

```text
Domain A
   ↓
Shared Memory Object
   ↑
Domain B
```

Beide Domains dürfen das Objekt an unterschiedlichen virtuellen Adressen einblenden.

Die Identität eines Shared-Memory-Objekts darf deshalb nicht von seiner virtuellen Adresse abhängen.

---

## Zero-Copy

Das VAS-Modell unterstützt Zero-Copy durch mehrfaches kontrolliertes Mapping desselben Backings.

```text
Producer Domain
       ↓
   Memory Object
       ↓
Consumer Domain
```

Ein Ownership-Transfer oder zusätzliches Mapping muss nicht automatisch eine Kopie der Nutzdaten erzeugen.

Security-, Lifetime- und Data-Sovereignty-Regeln bleiben dabei verbindlich.

---

## Mapping Authority

Das Recht, auf ein Memory Object zuzugreifen, ist von der Existenz eines virtuellen Adressbereichs getrennt.

Ein Mapping wird nur erzeugt, wenn:

```text
Memory Object exists
        +
Capability / Authority permits access
        +
Requested protection is permitted
        +
Resource constraints are satisfied
```

Das bloße Kennen einer virtuellen Adresse oder Object-ID gewährt keinen Zugriff.

---

## Schutzrechte

Mappings besitzen explizite Zugriffsrechte.

Mindestens werden unterschieden:

```text
Read
Write
Execute
```

NovaOS folgt grundsätzlich dem Least-Privilege-Prinzip.

Write und Execute sollen nicht gleichzeitig vergeben werden, sofern kein explizit autorisierter Anwendungsfall dies erfordert.

JIT- und ähnliche Runtimes müssen kontrollierte Übergänge verwenden.

---

## Guard Regions

Virtual Address Spaces müssen nicht gemappte Guard Regions unterstützen können.

Sie dienen insbesondere der Erkennung von:

- Stack Overflow,
- Buffer Overrun,
- ungültigen Pointer-Zugriffen,
- Zugriffen außerhalb definierter Memory Objects.

Guard Regions besitzen kein gültiges Backing und lösen bei Zugriff einen kontrollierten Fault aus.

---

## Address Space Layout

Die konkrete virtuelle Adressraumaufteilung darf zwischen:

- CPU-Architekturen,
- Page-Table-Formaten,
- 32-Bit- und 64-Bit-Plattformen

variieren.

NovaOS definiert deshalb keine universelle feste physische oder virtuelle Adresskarte als Architekturprinzip.

Stabile ABI-Bereiche müssen explizit spezifiziert werden, wenn sie benötigt werden.

---

## ASLR

Execution Domains sollen Address Space Layout Randomization unterstützen.

ASLR ist eine Security Policy und verändert nicht die logische Identität von Memory Objects oder Mappings.

Deterministic Mode kann ein reproduzierbares Layout verlangen, sofern dies mit den geltenden Security Policies vereinbar ist.

---

## Page Tables

Die konkrete Page-Table-Struktur ist HAL- beziehungsweise architekturspezifisch.

Der generische Virtual Memory Manager arbeitet mit einer abstrakten Mapping-Semantik.

```text
Generic VMM
    ↓
Architecture VM Backend
    ↓
CPU Page Tables / MMU
```

Damit bleiben x86, ARM und zukünftige Architekturen hinter derselben VAS-Semantik nutzbar.

---

## Context Switching

Beim Wechsel zwischen Execution Domains muss der Scheduler den korrekten Address-Space-Kontext aktivieren.

Die Implementierung darf Hardwaremechanismen wie Address-Space-Identifier oder vergleichbare TLB-Optimierungen verwenden.

Diese Optimierungen verändern die Isolationsemantik nicht.

---

## Copy-on-Write

Copy-on-Write darf als Memory-Management-Mechanismus unterstützt werden.

Es kann insbesondere für:

- Snapshots,
- Checkpoints,
- kompatibilitätsorientiertes Fork,
- private Mappings

verwendet werden.

Copy-on-Write ist jedoch kein Grund, Fork zum nativen Prozessmodell zu machen.

NovaOS verwendet weiterhin Spawn als bevorzugtes Modell.

---

## Checkpoint und Migration

Checkpointbare Execution Domains benötigen eine strukturierte Beschreibung ihres Virtual Address Space.

Dazu gehören insbesondere:

```text
Mappings
Memory Objects
Protection
Backing
Ownership
Reconstruction Information
```

Hardwareabhängige Page-Table-Strukturen selbst sind nicht das portable Checkpoint-Format.

Dadurch können Address Spaces kontrolliert rekonstruiert werden.

---

## Memory Accounting

Virtueller Adressraum und physischer Speicherverbrauch sind unterschiedliche Ressourcen.

```text
Reserved Virtual Range
        !=
Committed Physical Memory
```

Resource Accounting muss daher unter anderem unterscheiden können:

- virtuelle Reservation,
- committed Memory,
- resident Memory,
- shared Memory,
- pinned Memory.

Shared Memory darf nicht naiv mehrfach als physisch exklusiver Verbrauch gezählt werden.

---

## Memory Pressure

Große virtuelle Address Spaces erzeugen nicht automatisch hohen Physical Memory Pressure.

Pressure entsteht primär durch tatsächlich gebundene physische und anderweitig knappe Ressourcen.

Der VMM muss deshalb mit dem Physical Memory Manager und der Resource Economy koordiniert werden.

---

## NUMA

Virtuelle Adressen enthalten keine implizite NUMA-Semantik.

Das physische Backing eines Mappings kann entsprechend der NUMA-Placement-Policy gewählt oder später migriert werden.

```text
Virtual Address
      ↓
Memory Object
      ↓
NUMA-aware Physical Backing
```

Damit bleiben virtuelle Identität und physische Locality getrennt.

---

## Realtime

Realtime Execution Domains können zusätzliche Anforderungen an ihren Address Space besitzen, beispielsweise:

- vorab etablierte Mappings,
- vorab committed Memory,
- keine unerwarteten Page Faults im kritischen Pfad,
- gepinnte oder reservierte physische Ressourcen.

Die bloße Existenz eines virtuellen Mappings stellt keine Realtime-Garantie dar.

---

## Fault Handling

Ungültige oder nicht erfüllbare Speicherzugriffe erzeugen einen kontrollierten Memory Fault.

Der Fault muss dem verursachenden Execution Context und seiner Domain zugeordnet werden können.

Abhängig von Fault-Typ und Policy kann dies beispielsweise zu:

```text
Demand Mapping
Copy-on-Write
Exception Delivery
Scope Failure
Domain Failure
```

führen.

Security-Verletzungen dürfen nicht durch automatische permissive Mappings repariert werden.

---

## Domain Lifecycle

Beim Beenden einer Execution Domain müssen deren privaten Mappings entfernt werden.

Shared Memory Objects bleiben bestehen, wenn andere gültige Owner oder Referenzen existieren.

```text
Domain exits
    ↓
Remove its mappings
    ↓
Release references
    ↓
Destroy backing only when lifetime ends
```

VAS-Lebensdauer und Memory-Object-Lebensdauer sind deshalb getrennt.

---

## Hot Replacement

Hot Replacement darf neue Code- oder Datenobjekte kontrolliert in einen bestehenden Address Space einbringen.

Mapping-Änderungen müssen dabei atomar beziehungsweise transaktional sichtbar gemacht werden können, wenn die Operation dies erfordert.

Teilweise aktualisierte ausführbare Zustände dürfen nicht als gültiger Endzustand sichtbar werden.

---

## Architecture Introspection

Virtual Address Spaces müssen introspektierbar sein.

Mindestens relevant sind:

```text
Execution Domain
Virtual Regions
Mapping Type
Protection
Backing Object
Shared / Private
Committed State
Resident State
NUMA Placement
Ownership
```

Security-Policies bestimmen, welche Domain welche Details anderer Address Spaces sehen darf.

---

## Normative Anforderungen

1. Jede isolierte NovaOS Execution Domain MUSS grundsätzlich einen eigenen Virtual Address Space besitzen.
2. Tasks derselben Execution Domain SOLLEN standardmäßig denselben VAS verwenden.
3. Address Spaces unterschiedlicher Domains MÜSSEN standardmäßig voneinander isoliert sein.
4. Virtuelle Adressen DÜRFEN nur innerhalb ihres Address-Space-Kontexts interpretiert werden.
5. Gemeinsamer Speicher MUSS über explizite Shared Memory Objects beziehungsweise autorisierte Mappings realisierbar sein.
6. Die Identität eines Memory Objects DARF NICHT von seiner virtuellen Adresse abhängen.
7. Mapping und Memory-Object-Ownership MÜSSEN getrennte Konzepte sein.
8. Mappings MÜSSEN explizite Zugriffsrechte besitzen.
9. Das Erzeugen eines Mappings MUSS durch Capability- und Security-Regeln kontrolliert werden.
10. Das Kennen einer Adresse oder Object-ID DARF keine Zugriffsberechtigung verleihen.
11. Guard Regions MÜSSEN unterstützt werden können.
12. Die generische VAS-Semantik MUSS von architekturspezifischen Page-Table-Formaten getrennt sein.
13. Context Switches zwischen Domains MÜSSEN den korrekten Address-Space-Kontext aktivieren.
14. Hardwarebasierte TLB- und Address-Space-Optimierungen DÜRFEN die Isolation nicht abschwächen.
15. Copy-on-Write MUSS als optionaler Mechanismus integrierbar sein.
16. Fork-Kompatibilität DARF das native Domain-/VAS-Modell NICHT bestimmen.
17. Checkpoints MÜSSEN Address Spaces anhand portabler Mapping- und Memory-Object-Semantik beschreiben können.
18. Virtuelle Reservation und physischer Speicherverbrauch MÜSSEN im Accounting unterscheidbar sein.
19. Shared Memory MUSS ohne naive Mehrfachzählung physischer Ownership erfassbar sein.
20. NUMA Placement MUSS unabhängig von der virtuellen Adresse behandelt werden.
21. Realtime Domains MÜSSEN vorab etablierte und reservierte Memory Mappings verwenden können.
22. Memory Faults MÜSSEN dem verursachenden Execution Context zuordenbar sein.
23. Security Faults DÜRFEN NICHT durch implizite permissive Mappings umgangen werden.
24. Beim Domain-Ende MÜSSEN private Mappings kontrolliert entfernt werden.
25. Shared Memory Objects DÜRFEN die Lebensdauer einer einzelnen Domain übersteigen, wenn gültige Ownership besteht.
26. Virtual Address Spaces und ihre relevanten Mapping-Eigenschaften MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Execution Domains erhalten eine klare hardwaregestützte Speicherisolationsgrenze,
- Shared Memory und Zero-Copy bleiben trotzdem möglich,
- Memory Ownership wird von virtuellen Adressen entkoppelt,
- Checkpoint, Migration und Hot Replacement können auf strukturierten Memory Objects aufbauen,
- NUMA Placement bleibt unabhängig vom virtuellen Layout optimierbar,
- die Architektur ist auf unterschiedliche MMU- und Page-Table-Systeme portierbar.

### Negative Konsequenzen

- Domain-Wechsel können Address-Space- und TLB-Kosten verursachen,
- Shared Memory benötigt explizite Lifetime- und Authority-Verwaltung,
- Accounting gemeinsamer Memory Objects ist komplexer,
- Checkpoint und Migration müssen Mappings und Backing strukturiert rekonstruieren.

---

## Verworfene Alternativen

### Ein globaler Virtual Address Space für das gesamte System

Verworfen.

Dies würde Isolation und Fehlerbegrenzung zwischen Execution Domains unnötig schwächen.

### Eigener Address Space pro Task

Verworfen.

Tasks innerhalb einer Execution Domain sollen effizient gemeinsamen Zustand verwenden können.

### Shared Memory über identische virtuelle Adressen erzwingen

Verworfen.

Die Identität eines gemeinsamen Objekts darf nicht von identischen Address-Space-Layouts abhängen.

### Virtuelle Adresse als globale Memory-Object-ID

Verworfen.

Virtuelle Adressen sind domainlokale Mapping-Eigenschaften und keine systemweiten Objektidentitäten.

### Hardware-Page-Tables als öffentliches VAS-Modell

Verworfen.

NovaOS muss unterschiedliche MMU-Architekturen unterstützen und darf sein generisches Memory Model nicht an ein konkretes Page-Table-Format koppeln.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-PROCESS-0004_Fork_nur_als_Kompatibilitätssemantik`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-PROCESS-0007_Migrierbare_Workloads`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-OBJECT-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-MEMORY-PAGEFAULT-0001`
- `NPSPEC-MEMORY-COW-0001`
- `NPSPEC-MEMORY-VAS-TEST-0001`

---

## Ergebnis

NovaOS bindet den Virtual Address Space direkt an die Execution Domain:

```text
Execution Domain A             Execution Domain B
       │                              │
       ↓                              ↓
Virtual Address Space A        Virtual Address Space B
       │                              │
       ├──────────┐        ┌──────────┤
       ↓          ↓        ↓          ↓
Private A    Shared Memory Object    Private B
                  ↓
          Physical Backing
```

Damit bleibt Isolation der Standard, während Shared Memory und Zero-Copy durch explizite Memory Objects kontrolliert möglich bleiben.

Die zentrale Architekturregel lautet:

```text
One isolated domain,
one virtual address space.

Virtual addresses are local.

Memory objects carry identity.

Sharing is explicit and authorized.
```