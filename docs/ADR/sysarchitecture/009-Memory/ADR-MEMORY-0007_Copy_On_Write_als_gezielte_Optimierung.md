# ADR-MEMORY-0007 – Copy-On-Write als gezielte Optimierung

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Virtual Memory / Copy-On-Write / Optimization

## Kontext

`ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation` erlaubt NovaOS, physische Speicherressourcen erst bei tatsächlichem Bedarf zu materialisieren.

Ein verwandtes Problem entsteht, wenn mehrere Execution Domains oder Memory Objects zunächst denselben Datenbestand verwenden, sich dieser Zustand später jedoch unabhängig verändern kann.

Eine sofortige vollständige Kopie wäre häufig unnötig.

Beispiel:

```text
Original Memory
      ↓
Create Snapshot
      ↓
Copy 2 GiB immediately
```

Obwohl anschließend möglicherweise nur wenige Pages verändert werden.

Copy-On-Write (COW) ermöglicht stattdessen:

```text
Initial State

Domain A ──┐
           ├── Shared Read-only Pages
Domain B ──┘

First Write
     ↓
Copy affected Page
     ↓
Private Writable Page
```

COW kann dadurch Speicherverbrauch und Kopierkosten erheblich reduzieren.

NovaOS soll COW jedoch nicht als grundlegendes Prozess- oder Speichersemantikmodell verwenden. Insbesondere darf die Architektur nicht wie klassische Fork-orientierte Systeme davon abhängen, komplette Address Spaces logisch zu duplizieren und anschließend über COW zu optimieren.

COW wird deshalb als gezielte Optimierung für geeignete Anwendungsfälle definiert.

---

## Entscheidung

NovaOS unterstützt **Copy-On-Write als expliziten, gezielt eingesetzten Virtual-Memory-Mechanismus**.

COW darf insbesondere verwendet werden für:

- Snapshots,
- Checkpoints,
- private file-backed Mappings,
- Memory-Object-Clones,
- Fork-Kompatibilität,
- transaktionale beziehungsweise versionierte Speicherzustände.

COW ist kein grundlegendes NovaOS-Prozessmodell.

```text
Shared Backing
      ↓
Read-only COW Mappings
      ↓
Read
      ↓
Same Physical Page

Write
      ↓
COW Fault
      ↓
Private Copy
      ↓
Writable Mapping
```

Die semantische Korrektheit eines Mechanismus darf nicht davon abhängen, dass COW tatsächlich verwendet wird. Eine vollständige Kopie muss grundsätzlich dieselbe sichtbare Semantik erzeugen können.

---

## Grundprinzip

```text
Share while identical.

Copy when divergence begins.

Use COW where it saves real work.

Do not build the architecture around it.
```

---

## COW Memory Objects

COW wird auf Basis expliziter Memory Objects und deren Mappings modelliert.

Beispiel:

```text
Memory Object A
      │
      ├──── shared backing ────┐
      │                        │
      ↓                        ↓
Domain A                   Domain B
```

Nach einer Änderung:

```text
Domain A                  Domain B
   ↓                         ↓
Original Page          Private COW Page
```

Die virtuelle Adresse ist dabei nicht die Identität des zugrunde liegenden Objekts.

---

## COW Mapping

Ein COW Mapping erlaubt Lesen des bestehenden Backings, behandelt einen Schreibzugriff jedoch als COW Fault.

Konzeptionell:

```text
Read
 ↓
Existing Page

Write
 ↓
COW Fault
 ↓
Allocate Page
 ↓
Copy Content
 ↓
Update Mapping
 ↓
Resume
```

Das neue Mapping erhält anschließend die für den privaten Zustand vorgesehenen Schreibrechte.

---

## Page-granulare Trennung

COW erfolgt grundsätzlich auf der Granularität der verwendeten Memory-Mapping-Einheit.

Damit müssen nicht vollständige Memory Objects kopiert werden.

Beispiel:

```text
Object
[0][1][2][3][4][5]

Write Page 3
      ↓

A: [0][1][2][3][4][5]
             \
B:            [3']
```

Nur tatsächlich divergierende Bereiche benötigen privates Backing.

---

## Race-Sicherheit

Mehrere Tasks können gleichzeitig auf dieselbe COW Page schreiben.

Die COW-Auflösung muss deshalb race-safe sein.

Es muss verhindert werden, dass konkurrierende Faults:

- widersprüchliche Mappings erzeugen,
- Referenzen verlieren,
- Backing vorzeitig freigeben,
- Isolation zwischen Domains verletzen.

Die konkrete Synchronisation wird in den zugehörigen NPSPECs definiert.

---

## Ownership

COW verändert die Ownership-Struktur eines Memory Objects beziehungsweise seines Backings.

Vor dem ersten Schreibzugriff können mehrere Mappings dasselbe Backing referenzieren.

Nach der Trennung besitzt die schreibende Seite ein eigenes Backing für den betroffenen Bereich.

```text
Before:

Backing X
 ├── Domain A
 └── Domain B

After COW:

Backing X ── Domain A
Backing Y ── Domain B
```

Ownership, Reference Counting und Lifetime Management müssen entsprechend aktualisiert werden.

---

## Resource Accounting

Gemeinsam verwendete COW Pages dürfen nicht naiv jeder Domain vollständig als exklusiver physischer Speicherverbrauch zugerechnet werden.

Gleichzeitig muss berücksichtigt werden, dass spätere Schreibzugriffe zusätzlichen Speicherbedarf erzeugen können.

Das Accounting unterscheidet deshalb mindestens:

```text
Shared COW Backing
Private COW Backing
Potential COW Expansion
```

Die genaue Kostenverteilung ist Bestandteil der Resource-Economy-Spezifikation.

---

## Memory Pressure

COW kann zunächst Speicher sparen, später jedoch zu einer schnellen Zunahme des physischen Speicherverbrauchs führen.

```text
Large Shared Snapshot
        ↓
Many Writes
        ↓
Many COW Faults
        ↓
Rapid Memory Growth
```

Memory Pressure und Domain Budgets müssen daher auch COW-bedingte Materialisierung berücksichtigen.

COW darf nicht als kostenloser Speicher behandelt werden.

---

## Domain-Local OOM

Kann eine notwendige COW-Kopie aufgrund eines Domain-Limits oder fehlender physischer Ressourcen nicht erzeugt werden, wird der Fault in das Domain-Local-OOM-Modell überführt.

```text
Write
 ↓
COW Fault
 ↓
Private Allocation fails
 ↓
Domain-local OOM Handling
```

NovaOS darf in diesem Fall nicht einfach das ursprünglich geteilte Backing beschreibbar machen, da dies die Isolation verletzen würde.

---

## NUMA Placement

Neue COW Pages müssen NUMA-aware platziert werden können.

Die Kopie kann entsprechend der aktuellen beziehungsweise vereinbarten Locality des schreibenden Owners platziert werden.

```text
Shared Page on Node 0
        ↓
Domain on Node 1 writes
        ↓
COW
        ↓
Private Page on Node 1
```

Required Memory Placement bleibt verbindlich.

COW kann dadurch gleichzeitig eine Gelegenheit zur Verbesserung der Datenlokalität darstellen.

---

## Snapshots

Snapshots sind ein zentraler geeigneter COW-Anwendungsfall.

```text
Live State
    ↓
Snapshot
    ↓
Shared Pages

Future Writes
    ↓
Private COW Pages
```

Dadurch kann ein konsistenter logischer Zustand erhalten bleiben, ohne sofort sämtliche Daten zu duplizieren.

Die Snapshot-Konsistenz selbst wird jedoch nicht durch COW allein garantiert; dafür sind entsprechende Snapshot- beziehungsweise Transaktionsmechanismen erforderlich.

---

## Checkpoints

Checkpointbare Execution Domains dürfen COW verwenden, um den Zustand während der Checkpoint-Erstellung effizient zu stabilisieren.

Beispielsweise:

```text
Execution continues
       ↓
Checkpoint view
       ↓
COW preserves old pages
```

COW ist dabei eine Implementierungsstrategie.

Das portable Checkpoint-Format darf nicht davon abhängen, dass das Zielsystem denselben COW-Mechanismus besitzt.

---

## Fork-Kompatibilität

`ADR-PROCESS-0004_Fork_nur_als_Kompatibilitätssemantik` erlaubt Fork ausschließlich als Kompatibilitätsmechanismus.

COW darf verwendet werden, um einen solchen Fork effizient umzusetzen.

```text
Compatibility Fork
       ↓
COW Address Space
```

Dies ändert nicht die NovaOS-Grundentscheidung:

```text
Native process creation = Spawn

Fork = Compatibility
```

Die Architektur des VMM darf deshalb nicht primär um Fork herum aufgebaut werden.

---

## Private File Mappings

Private file-backed Mappings können COW verwenden.

```text
File Page
   ↓
Private Mapping
   ↓
Read → shared cached page
   ↓
Write → private COW page
```

Die Änderung verändert dabei nicht automatisch das zugrunde liegende File Object.

---

## Zero Pages

Die in `ADR-MEMORY-0006` beschriebenen Zero Pages sind ein spezialisierter COW-ähnlicher Anwendungsfall.

Mehrere virtuelle Pages können zunächst eine gemeinsame schreibgeschützte Zero Page referenzieren.

Erst beim ersten Schreibzugriff wird privates Backing erzeugt.

---

## Huge Pages

COW muss mit unterschiedlichen Page-Größen umgehen können.

Ein Schreibzugriff auf einen kleinen Teil einer Huge Page darf eine Policy-Entscheidung auslösen:

```text
Copy Huge Page
```

oder:

```text
Split Huge Page
      ↓
COW smaller unit
```

Die konkrete Strategie ist eine Performance- und Memory-Policy.

COW darf nicht voraussetzen, dass alle Mappings dieselbe Page-Größe verwenden.

---

## Realtime

COW Faults können:

- physische Allokation,
- Datenkopie,
- Mapping-Änderung,
- TLB-Arbeit

erfordern.

Damit besitzen sie variable Latenz.

Hard-Realtime-Pfade dürfen deshalb nicht unkontrolliert von COW Faults abhängig sein.

Vor einem kritischen Realtime-Abschnitt muss eine Domain erforderliche private Pages gegebenenfalls vorab materialisieren können.

```text
COW Mapping
    ↓
Pre-break / Materialize
    ↓
Private Pages
    ↓
Realtime Execution
```

---

## Deterministic Mode

COW kann abhängig von der tatsächlichen Schreibreihenfolge unterschiedliche Fault- und Allocation-Sequenzen erzeugen.

Deterministic Mode muss deshalb:

- COW kontrollieren,
- relevante Pages vorab trennen,
- oder eine reproduzierbare COW-Policy verwenden

können.

COW darf keine versteckte Voraussetzung für deterministische Korrektheit sein.

---

## Transaktionale Systemoperationen

COW kann als technische Optimierung für versionierte oder transaktionale Speicherzustände eingesetzt werden.

Dabei gilt jedoch:

```text
COW != System Transaction
```

COW stellt lediglich getrennte Speicherstände bereit.

Commit-, Abort-, Atomicity- und Recovery-Semantik werden durch die transaktionale Systemarchitektur definiert.

---

## Hot Replacement

Hot Replacement kann COW verwenden, wenn bestehende Daten zunächst gemeinsam weiterverwendet und nur bei Änderungen getrennt werden sollen.

Auch hier bleibt COW ein Optimierungsmechanismus und definiert nicht selbst die Konsistenz- oder Versionssemantik des Hot-Replacement-Protokolls.

---

## Security

COW darf niemals dazu führen, dass private Änderungen einer Execution Domain für andere Domains sichtbar werden.

Beim COW Fault muss das neue private Backing vollständig vorbereitet sein, bevor das schreibbare Mapping veröffentlicht wird.

Fehler während der COW-Auflösung müssen fail-safe behandelt werden.

Insbesondere gilt:

```text
COW allocation failure
        !=
make shared page writable
```

---

## Data Sovereignty

COW verändert physische Platzierung und möglicherweise Memory Domains.

Neue private Backings müssen daher weiterhin:

- Data-Sovereignty-Constraints,
- Security Policies,
- Required NUMA Placement,
- Memory-Class-Anforderungen

erfüllen.

Eine COW-Optimierung darf keine Daten in eine unzulässige Memory Domain verschieben.

---

## COW-Auflösung

COW Sharing soll beendet werden können, wenn es keinen Nutzen mehr bietet.

Beispielsweise kann bei nur noch einem verbleibenden Owner ein exklusives Mapping wieder direkt auf das vorhandene Backing verweisen, sofern die Schutz- und Ownership-Regeln dies erlauben.

Unnötige COW-Zustände müssen nicht dauerhaft erhalten bleiben.

---

## Adaptive Verwendung

NovaOS darf anhand realer Workload-Daten entscheiden, ob COW für einen bestimmten optionalen Anwendungsfall sinnvoll ist.

Beispiel:

```text
COW Snapshot
    ↓
95 % of pages modified immediately
    ↓
COW overhead > eager copy
    ↓
Future policy adjustment
```

Prediction Error und Laufzeitmetriken dürfen die Auswahl zwischen:

```text
COW
Eager Copy
```

beeinflussen.

Dies bleibt eine Soft-Policy.

Explizite semantische Anforderungen dürfen dadurch nicht verändert werden.

---

## Architecture Introspection

COW muss introspektierbar sein.

Relevant sind insbesondere:

```text
COW Memory Objects
Shared COW Pages
Private COW Pages
COW Fault Count
Copied Bytes
COW Allocation Failures
Huge-Page Splits
COW Memory Growth
Owner / Domain
NUMA Placement
```

Damit kann beurteilt werden, ob COW tatsächlich Speicher und Kopierarbeit einspart oder lediglich zusätzlichen Fault- und Verwaltungsaufwand erzeugt.

---

## Normative Anforderungen

1. NovaOS MUSS Copy-On-Write als optionalen Virtual-Memory-Mechanismus unterstützen.
2. COW DARF NICHT das grundlegende NovaOS-Prozessmodell bestimmen.
3. COW MUSS auf expliziten Memory Objects und Mappings abbildbar sein.
4. Schreibzugriffe auf COW-Mappings MÜSSEN kontrollierte COW Faults auslösen können.
5. COW-Auflösung MUSS race-safe erfolgen.
6. Private COW-Änderungen DÜRFEN NICHT für andere isolierte Domains sichtbar werden.
7. COW MUSS mit dem Memory-Object-Lifetime- und Ownership-Modell integriert sein.
8. Shared und private COW Pages MÜSSEN im Resource Accounting unterscheidbar sein.
9. COW-bedingter zusätzlicher Speicherverbrauch MUSS der verantwortlichen Resource Domain zurechenbar sein.
10. COW Allocation Failure MUSS mit Domain-Local OOM Handling integrierbar sein.
11. Ein fehlgeschlagener COW Fault DARF NICHT durch Schreibfreigabe des gemeinsam genutzten Backings umgangen werden.
12. Neue COW Pages MÜSSEN NUMA-aware platziert werden können.
13. Required Memory Placement und Data-Sovereignty-Constraints MÜSSEN bei COW erhalten bleiben.
14. Snapshots und Checkpoints DÜRFEN COW als Optimierung verwenden.
15. Portable Checkpoint-Semantik DARF NICHT von einer konkreten COW-Implementierung abhängen.
16. Fork-Kompatibilität DARF COW verwenden, ohne Fork zum nativen Prozessmodell zu machen.
17. Private file-backed Mappings MÜSSEN über COW implementierbar sein.
18. COW MUSS mit unterschiedlichen Page-Größen integrierbar sein.
19. Huge-Page-COW MUSS eine kontrollierte Copy- oder Split-Strategie unterstützen können.
20. Hard-Realtime-Pfade DÜRFEN NICHT von unkontrollierten COW Faults abhängig sein.
21. Realtime Domains MÜSSEN COW Pages vorab privatisieren können.
22. Deterministic Mode MUSS COW-Verhalten kontrollieren können.
23. COW DARF NICHT mit transaktionaler Systemsemantik gleichgesetzt werden.
24. COW-Optimierungen DÜRFEN Security- oder Isolationseigenschaften NICHT verändern.
25. Adaptive COW-Policies DÜRFEN ausschließlich die Wahl der Optimierungsstrategie beeinflussen.
26. COW-Nutzung, Faults, Speicherwachstum und Kopierkosten MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Snapshots und Checkpoints können erheblich weniger initiale Kopierarbeit benötigen,
- identische Daten können physisch geteilt werden,
- private file-backed Mappings lassen sich effizient realisieren,
- Fork-Kompatibilität kann ohne eager Address-Space-Copy bereitgestellt werden,
- COW kann physische Speicherbelegung reduzieren,
- COW-Trennung kann gleichzeitig NUMA-lokales Backing erzeugen.

### Negative Konsequenzen

- Schreibzugriffe können unerwartete Fault- und Kopierkosten erzeugen,
- Memory Accounting wird komplexer,
- parallele COW Faults benötigen Synchronisation,
- hohe Änderungsraten können COW teurer als eine sofortige Kopie machen,
- Huge Pages erschweren die optimale COW-Granularität,
- Realtime- und deterministische Workloads benötigen zusätzliche Kontrolle.

---

## Verworfene Alternativen

### COW als Grundlage aller Memory Objects

Verworfen.

Viele Speicherobjekte profitieren nicht von COW und würden nur zusätzliche Komplexität und Fault-Kosten erhalten.

### Fork + COW als natives Prozessmodell

Verworfen.

NovaOS verwendet Spawn als bevorzugtes natives Prozessmodell und Fork ausschließlich für Kompatibilität.

### Immer vollständige Kopie

Verworfen.

Für Snapshots, Checkpoints und weitgehend unveränderte Daten würde unnötig Speicher und Speicherbandbreite verbraucht.

### COW immer verwenden, wenn Sharing möglich ist

Verworfen.

Bei hoher Änderungsrate kann eine eager Copy günstiger und vorhersehbarer sein.

### Shared Page bei COW-OOM beschreibbar machen

Verworfen.

Dies würde die Isolation und private Memory-Semantik verletzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-PROCESS-0004_Fork_nur_als_Kompatibilitätssemantik`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-COW-0001`
- `NPSPEC-MEMORY-COW-FAULT-0001`
- `NPSPEC-MEMORY-COW-OBJECT-0001`
- `NPSPEC-MEMORY-COW-ACCOUNTING-0001`
- `NPSPEC-MEMORY-COW-NUMA-0001`
- `NPSPEC-MEMORY-COW-HUGEPAGE-0001`
- `NPSPEC-MEMORY-COW-REALTIME-0001`
- `NPSPEC-MEMORY-COW-TEST-0001`

---

## Ergebnis

NovaOS verwendet Copy-On-Write dort, wo vorhandene identische Daten zunächst effizient geteilt werden können:

```text
Shared Backing
      ↓
Multiple COW Mappings
      ↓
Read → Share
      ↓
Write → COW Fault
      ↓
Private NUMA-aware Backing
```

COW bleibt dabei eine Optimierung und keine grundlegende Voraussetzung für Prozesse, Snapshots, Checkpoints oder Systemtransaktionen.

Die zentrale Architekturregel lautet:

```text
Share while identical.

Copy only on divergence.

Keep correctness independent of COW.

Optimize deliberately, not universally.
```