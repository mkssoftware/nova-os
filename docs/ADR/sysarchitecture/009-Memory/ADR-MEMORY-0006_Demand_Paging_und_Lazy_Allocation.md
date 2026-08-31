# ADR-MEMORY-0006 – Demand Paging und Lazy Allocation

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Virtual Memory / Paging / Resource Economy

## Kontext

`ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain` definiert virtuelle Adressräume pro Execution Domain und trennt virtuelle Reservation von physischem Speicherverbrauch.

Eine virtuelle Speicherregion muss nicht zwangsläufig sofort vollständig mit physischen Pages hinterlegt werden.

Beispiel:

```text
Reserve 1 GiB Virtual Memory

Virtual:
[================================]

Physical backing after creation:
[                                ]

After actual use:
[####                            ]
```

Eine sofortige physische Belegung jeder virtuellen Reservation würde:

- Speicher für möglicherweise nie verwendete Bereiche verbrauchen,
- große Address Spaces unnötig teuer machen,
- Sparse Data Structures erschweren,
- Startzeiten erhöhen,
- Memory Pressure früher auslösen.

NovaOS benötigt deshalb Demand Paging und Lazy Allocation als reguläre Mechanismen des Virtual Memory Managers.

Gleichzeitig dürfen diese Mechanismen nicht unkontrolliert eingesetzt werden. Page Faults, physische Allokationen und gegebenenfalls I/O können zusätzliche und schwer vorhersehbare Latenz erzeugen, was insbesondere für Realtime- und deterministische Workloads relevant ist.

---

## Entscheidung

NovaOS unterstützt **Demand Paging und Lazy Allocation als native Virtual-Memory-Mechanismen**.

Virtuelle Speicherbereiche können reserviert werden, ohne sofort vollständiges physisches Backing zu erhalten.

Physische Ressourcen werden abhängig von Mapping- und Commit-Semantik erst dann bereitgestellt, wenn sie tatsächlich benötigt werden.

Grundmodell:

```text
Virtual Reservation
        ↓
Access
        ↓
Page Fault
        ↓
Validate Mapping
        ↓
Obtain Backing
        ↓
Install Mapping
        ↓
Resume Execution
```

Demand Paging ist eine Optimierung der Ressourcenbereitstellung und darf keine Zugriffsrechte oder Resource Contracts umgehen.

---

## Grundprinzip

```text
Reserve address space when needed.

Commit resources according to contract.

Materialize pages when required.

Never confuse virtual availability
with guaranteed physical capacity.
```

---

## Reserve, Commit und Resident

NovaOS unterscheidet mindestens drei relevante Zustände:

```text
Reserved
Committed
Resident
```

### Reserved

Ein virtueller Adressbereich wurde für eine bestimmte Nutzung reserviert.

Physische Ressourcen müssen noch nicht gebunden sein.

### Committed

NovaOS hat entsprechend dem Memory Contract die notwendigen Ressourcen beziehungsweise Backing-Verpflichtungen übernommen.

### Resident

Die betreffende Page befindet sich aktuell in direkt zugreifbarem physischem Speicher.

Damit gilt:

```text
Reserved != Committed != Resident
```

Die genaue Commit-Semantik wird in den NPSPECs festgelegt.

---

## Lazy Anonymous Allocation

Anonymer Speicher darf zunächst ohne physische Pages reserviert werden.

Beispiel:

```text
Allocate 256 MiB
      ↓
Reserve Virtual Range
      ↓
Pages physically materialized
on first relevant access
```

Dadurch wird nur Speicher gebunden, der tatsächlich verwendet wird.

---

## Demand Paging

Ein gültiger Zugriff auf eine noch nicht residente Page erzeugt einen kontrollierten Page Fault.

Der Virtual Memory Manager prüft:

```text
Is address valid?
      ↓
Is access permitted?
      ↓
Is backing obtainable?
      ↓
Install mapping
      ↓
Resume task
```

Nur ein gültiger Demand Fault darf zur automatischen Materialisierung führen.

---

## Invalid Access

Demand Paging darf ungültige Speicherzugriffe nicht in gültige Zugriffe verwandeln.

Beispiel:

```text
Access
  ↓
No valid region
  ↓
Memory Fault
```

oder:

```text
Write
  ↓
Read-only mapping
  ↓
Protection Fault
```

Demand Paging greift nur innerhalb bereits autorisierter Virtual-Memory-Semantik.

---

## Zero Pages

NovaOS darf gemeinsam verwendete Zero Pages einsetzen.

Noch nicht beschriebene anonyme Pages können zunächst auf eine schreibgeschützte Zero Page zeigen.

Beim ersten Schreibzugriff kann anschließend eine private physische Page erzeugt werden.

```text
Read
 ↓
Shared Zero Page

Write
 ↓
Fault
 ↓
Allocate Private Page
```

Dies ist eine optionale Implementierungsoptimierung.

---

## File-backed Memory

Memory-Mapped Files und andere Backing Objects dürfen demand-paged sein.

```text
Virtual Mapping
      ↓
Page Fault
      ↓
Backing Object
      ↓
Storage / Cache
      ↓
Physical Page
```

Der VMM koordiniert dafür mit dem jeweiligen Storage- beziehungsweise Object-Subsystem.

Ein Page Fault kann dadurch asynchrone I/O-Arbeit auslösen.

---

## Page Fault Handling

Page Faults werden nach ihrer Ursache klassifiziert.

Mindestens müssen unterscheidbar sein:

```text
Demand Fault
Protection Fault
Copy-on-Write Fault
Invalid Mapping Fault
Backing Failure
Resource Failure
```

Diese Ursachen dürfen nicht als ein einziger generischer Fehler behandelt werden.

---

## Resource Accounting

Lazy Allocation darf Resource Accounting nicht umgehen.

Insbesondere muss unterschieden werden zwischen:

```text
Virtual Reservation
Commitment
Resident Memory
Shared Backing
```

Eine Domain darf nicht unbegrenzt virtuellen Speicher reservieren, wenn dies selbst eine knappe oder missbrauchbare Ressource darstellt.

Commitments müssen in die Resource Economy integriert werden.

---

## Memory Pressure

Demand Paging reduziert unnötige physische Belegung, beseitigt Memory Pressure jedoch nicht.

Werden viele zuvor unmaterialisierte Pages gleichzeitig verwendet, kann der physische Speicherbedarf schnell steigen.

```text
Large Lazy Region
       ↓
Rapid First Touch
       ↓
Physical Allocation Burst
       ↓
Memory Pressure
```

Der Memory Manager muss solche Allokationen weiterhin den zuständigen Domains zurechnen.

---

## Domain-Local OOM

Kann ein gültiger Demand Fault aufgrund eines Domain-Limits oder nicht erfüllbarer physischer Ressourcen nicht bedient werden, wird dies in das Domain-Local-OOM-Modell integriert.

```text
Demand Fault
     ↓
Physical Allocation
     ↓
Domain Limit / Resource Failure
     ↓
Domain-local OOM Policy
```

Ein virtueller Adressbereich garantiert daher nicht automatisch, dass jede zukünftige Page materialisiert werden kann.

Garantierte Commit-Semantik muss explizit vereinbart werden.

---

## NUMA Placement

Lazy materialisierte Pages müssen NUMA-aware platziert werden können.

Je nach Mapping Policy kann die Platzierung beispielsweise erfolgen anhand von:

- First Touch,
- Preferred NUMA Domain,
- Required NUMA Domain,
- Execution-Scope-Locality,
- Interleave Policy.

Demand Paging darf Required-NUMA-Constraints nicht umgehen.

---

## Realtime

Hard-Realtime-Workloads dürfen nicht unkontrolliert von Demand Faults im kritischen Pfad abhängig sein.

Realtime Domains müssen Speicher:

- vorab committen,
- vorab materialisieren,
- gegebenenfalls pinnen,
- mit geeigneten Reservations absichern

können.

Konzeptionell:

```text
Realtime Preparation
       ↓
Reserve
       ↓
Commit
       ↓
Prefault / Materialize
       ↓
Enter Critical Execution
```

Demand Paging bleibt außerhalb solcher explizit vorbereiteten Bereiche möglich.

---

## Deterministic Mode

Demand Paging kann Ausführungslatenzen abhängig vom bisherigen Zugriffsverlauf verändern.

Deterministic Mode muss deshalb kontrollieren können:

- welche Bereiche lazy materialisiert werden,
- wann Prefaulting erfolgt,
- ob file-backed Demand Paging zulässig ist,
- welche Page-Fault-Reihenfolge relevant ist.

Für determinismuskritische Bereiche kann vollständige Vormaterialisierung erforderlich sein.

---

## Prefaulting

NovaOS unterstützt explizites Prefaulting beziehungsweise Pre-Materialization.

Damit kann eine Workload Pages vor einem latenzkritischen Abschnitt bewusst materialisieren.

```text
Lazy Mapping
     ↓
Prefault
     ↓
Resident Working Set
     ↓
Latency-critical Work
```

Prefaulting ist insbesondere für Realtime, deterministische Ausführung und vorhersehbare I/O-Pipelines relevant.

---

## Preloading und Prediction

NovaOS darf vorhergesagte Speicherzugriffe nutzen, um Pages opportunistisch vorab zu materialisieren.

Dies ist ausschließlich eine Soft-Policy.

```text
Predicted Access
      ↓
Optional Prefault
      ↓
Prediction Error
      ↓
Policy Adjustment
```

Predictive Prefaulting darf:

- Memory Budgets,
- Memory Pressure,
- Security,
- Data Sovereignty,
- Realtime Reservations

nicht verletzen.

Unter Memory Pressure muss spekulatives Prefaulting reduzierbar sein.

---

## Huge Pages

Demand Paging und Lazy Allocation müssen mit unterschiedlichen Page-Größen integrierbar sein.

Die Entscheidung über:

- Base Pages,
- Huge Pages,
- zukünftige Page Sizes

ist eine separate Placement- und Performance-Policy.

Eine große virtuelle Region darf nicht automatisch vollständig als Huge Page materialisiert werden.

---

## Shared Memory

Shared Memory Objects dürfen ebenfalls lazy Backing verwenden, sofern ihre Contracts dies erlauben.

Mehrere Domains können dabei dieselbe materialisierte physische Page referenzieren.

Die Materialisierung muss race-safe erfolgen, sodass konkurrierende Faults nicht unbeabsichtigt mehrere widersprüchliche Backings erzeugen.

---

## Zero-Copy

Lazy Allocation bleibt mit Zero-Copy kompatibel.

Sobald ein Buffer jedoch an:

- Devices,
- Accelerators,
- DMA,
- andere Execution Domains

übergeben wird, müssen die für diesen Zugriff notwendigen Pages entsprechend der jeweiligen Memory- und DMA-Contracts verfügbar gemacht werden.

Lazy Backing darf nicht zu einem impliziten Zugriff eines Devices auf nicht materialisierten Speicher führen.

---

## Cancellation

Kann ein Demand Fault blockierende Arbeit auslösen, muss diese Arbeit dem verursachenden Execution Context zugeordnet bleiben.

Wird der zugehörige Scope abgebrochen, muss eine noch laufende Fault-/Backing-Operation entsprechend ihrer Cancellation-Semantik behandelt werden können.

Damit bleibt auch Demand Paging mit Structured Concurrency vereinbar.

---

## Security

Demand Paging darf keine Daten aus zuvor verwendeten physischen Pages offenlegen.

Neu bereitgestellter anonymer Speicher muss vor Sichtbarkeit für die Domain einen definierten Initialzustand besitzen.

Für gewöhnlichen anonymen Speicher bedeutet dies grundsätzlich logisch nullinitialisierten Inhalt.

Lazy Zeroing darf intern optimiert werden, solange die Sicherheitssemantik erhalten bleibt.

---

## Fail-Safe Verhalten

Kann ein gültiger Fault nicht sicher erfüllt werden, darf NovaOS kein inkompatibles oder unautorisiertes Backing installieren.

Stattdessen muss ein definierter Fehlerpfad ausgelöst werden.

```text
Fault
  ↓
Cannot satisfy constraints
  ↓
Fail / OOM / Contract violation
```

Hard Constraints bleiben auch während des Fault Handlings verbindlich.

---

## Architecture Introspection

Demand-Paging-Zustände müssen diagnostizierbar sein.

Relevant sind insbesondere:

```text
Virtual Reserved
Committed
Resident
Lazy Pages
Fault Count
Demand Faults
Protection Faults
Copy-on-Write Faults
Backing Failures
Prefault Activity
```

Diese Informationen müssen der jeweiligen Execution Domain und ihren Memory Objects zuordenbar sein.

---

## Normative Anforderungen

1. NovaOS MUSS Demand Paging und Lazy Allocation unterstützen.
2. Virtuelle Reservation DARF ohne sofortige vollständige physische Belegung möglich sein.
3. Reserved, Committed und Resident MÜSSEN als unterschiedliche Zustände modellierbar sein.
4. Demand Paging DARF nur für gültige und autorisierte Mappings erfolgen.
5. Ungültige Zugriffe DÜRFEN NICHT durch Lazy Allocation legitimiert werden.
6. Protection Faults MÜSSEN von Demand Faults unterscheidbar sein.
7. Copy-on-Write Faults MÜSSEN separat klassifizierbar sein.
8. Anonymer neu sichtbarer Speicher MUSS einen definierten sicheren Initialzustand besitzen.
9. File-backed Mappings MÜSSEN demand-paged implementierbar sein.
10. Demand Faults MÜSSEN mit dem jeweiligen Backing Object koordinierbar sein.
11. Lazy Allocation DARF Resource Accounting NICHT umgehen.
12. Virtual Reservation, Commitment und physischer Verbrauch MÜSSEN getrennt erfassbar sein.
13. Commitments MÜSSEN mit der Resource Economy integrierbar sein.
14. Demand-Paging-Allokationen MÜSSEN Domain-Local-OOM-Policies berücksichtigen.
15. Lazy materialisierte Pages MÜSSEN NUMA-aware platziert werden können.
16. Required Memory Placement DARF durch Demand Paging NICHT verletzt werden.
17. Realtime Domains MÜSSEN Speicher vorab committen und materialisieren können.
18. Hard-Realtime-Pfade DÜRFEN NICHT von unkontrollierten Demand Faults abhängig sein.
19. Deterministic Mode MUSS Lazy Allocation und Demand Paging kontrollieren können.
20. Explizites Prefaulting MUSS unterstützt werden können.
21. Predictive Prefaulting DARF nur als Soft-Policy verwendet werden.
22. Predictive Prefaulting MUSS unter Memory Pressure reduzierbar sein.
23. Demand Paging MUSS mit unterschiedlichen Page-Größen integrierbar sein.
24. Shared-Memory-Materialisierung MUSS bei konkurrierenden Faults race-safe sein.
25. Zero-Copy- und DMA-Nutzung MUSS notwendiges Backing vor dem tatsächlichen Device-Zugriff sicherstellen.
26. Kann ein Fault nicht unter Einhaltung aller Hard Constraints erfüllt werden, MUSS ein definierter Fehler- oder OOM-Pfad verwendet werden.
27. Demand-Paging-Aktivität MUSS der verursachenden Domain zurechenbar sein.
28. Reserved-, Committed-, Resident- und Fault-Zustände MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- virtuelle Address Spaces können effizient und sparsam genutzt werden,
- physischer Speicher wird erst bei tatsächlichem Bedarf gebunden,
- große Sparse Data Structures werden praktikabel,
- Startzeiten und unnötiger Speicherverbrauch können reduziert werden,
- File-backed Memory und Copy-on-Write lassen sich sauber integrieren,
- NUMA-aware First-Touch und andere Placement-Policies werden möglich.

### Negative Konsequenzen

- Page Faults erzeugen zusätzliche und variable Latenz,
- Commit- und Accounting-Semantik werden komplexer,
- Realtime-Workloads benötigen explizites Prefaulting und Reservations,
- starke First-Touch-Phasen können kurzfristig hohen Memory Pressure erzeugen,
- file-backed Demand Paging kann I/O in Speicherzugriffe einführen.

---

## Verworfene Alternativen

### Jede virtuelle Reservation sofort physisch hinterlegen

Verworfen.

Dies würde physische Ressourcen für möglicherweise nie verwendeten Speicher binden.

### Unbegrenztes Overcommit ohne explizite Commit-Semantik

Verworfen.

Virtuelle Verfügbarkeit darf nicht mit garantierter physischer Erfüllbarkeit verwechselt werden.

### Demand Paging auch für Hard-Realtime-Kernpfade voraussetzen

Verworfen.

Unvorhersehbare Fault-, Allocation- und I/O-Latenzen sind mit harten zeitlichen Garantien nicht vereinbar.

### First-Touch als einzige NUMA-Policy

Verworfen.

Initialisierungsort und langfristiger Nutzungsort können unterschiedlich sein.

### Page Fault als generischer Fehler ohne Ursachenklassifikation

Verworfen.

Demand Paging, Protection Violations, Copy-on-Write und Resource Failures besitzen unterschiedliche Semantik und Recovery.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0011_Predictive_und_Adaptive_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-DEMAND-PAGING-0001`
- `NPSPEC-MEMORY-LAZY-ALLOCATION-0001`
- `NPSPEC-MEMORY-COMMIT-0001`
- `NPSPEC-MEMORY-PAGEFAULT-0001`
- `NPSPEC-MEMORY-PREFAULT-0001`
- `NPSPEC-MEMORY-FILE-BACKING-0001`
- `NPSPEC-MEMORY-ZERO-PAGE-0001`
- `NPSPEC-MEMORY-DEMAND-PAGING-TEST-0001`

---

## Ergebnis

NovaOS trennt virtuelle Reservation konsequent von tatsächlicher physischer Materialisierung:

```text
Virtual Reservation
        ↓
     Commit
        ↓
Actual Access
        ↓
Demand Fault
        ↓
Constraint Validation
        ↓
NUMA-aware Physical Backing
        ↓
Resident Mapping
```

Realtime- und deterministische Workloads können diese Lazy-Semantik durch explizites Commit, Prefaulting und Reservations kontrollieren.

Die zentrale Architekturregel lautet:

```text
Reserve lazily.

Commit explicitly.

Materialize on demand.

Guarantee resources only when the contract says so.
```