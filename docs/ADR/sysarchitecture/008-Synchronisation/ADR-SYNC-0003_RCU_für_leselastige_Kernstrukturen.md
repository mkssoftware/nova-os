# ADR-SYNC-0003 – RCU für leselastige Kernstrukturen

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / Concurrency / RCU / Read-Mostly Data Structures

## Kontext

NovaOS besitzt Kernelstrukturen, die sehr häufig gelesen, aber vergleichsweise selten verändert werden.

Beispiele können sein:

- Routing- und Lookup-Strukturen,
- Capability-Metadaten,
- Prozess- und Execution-Domain-Verzeichnisse,
- Hardware- und Topologieinformationen,
- Scheduler-Metadaten,
- Registry-Strukturen,
- Konfigurations-Snapshots.

Klassische Reader/Writer Locks können bei solchen Workloads unnötige Kosten erzeugen, weil auch reine Leser Synchronisationszustand verändern oder miteinander um gemeinsame Cache Lines konkurrieren können.

`ADR-SYNC-0002_Lock_Free_Strukturen_gezielt_einsetzen` legt bereits fest, dass komplexe Synchronisationsverfahren nur dort eingesetzt werden sollen, wo sie einen konkreten Vorteil besitzen.

Für stark leselastige Kernstrukturen soll NovaOS deshalb **Read-Copy-Update (RCU)** als spezialisierten Synchronisationsmechanismus unterstützen.

---

## Entscheidung

NovaOS verwendet RCU gezielt für geeignete **read-mostly Kernelstrukturen**.

Das grundlegende Modell lautet:

```text
Readers
   ↓
Read Current Version
   ↓
No traditional reader lock

Writer
   ↓
Create / Modify Version
   ↓
Publish New State
   ↓
Grace Period
   ↓
Reclaim Old State
```

Leser greifen auf eine veröffentlichte Version zu, während Writer Änderungen kontrolliert publizieren.

Alte Zustände dürfen erst freigegeben werden, nachdem sichergestellt ist, dass kein relevanter Reader sie noch verwendet.

RCU wird nicht zur universellen Synchronisationsstrategie von NovaOS.

---

## Grundprinzip

```text
Readers should be cheap.

Writers may do more work.

Publication must be atomic.

Reclamation waits for readers.
```

---

## Einsatzkriterien

RCU soll insbesondere dann eingesetzt werden, wenn:

- Lesezugriffe deutlich häufiger als Schreibzugriffe sind,
- sehr niedrige Reader-Overheads relevant sind,
- Leser überwiegend kurze kritische Abschnitte besitzen,
- verzögerte Freigabe alter Versionen akzeptabel ist,
- die Datenstruktur eine geeignete Update-Semantik besitzt.

RCU soll nicht allein deshalb verwendet werden, weil eine Struktur parallel verwendet wird.

---

## Reader-Semantik

RCU-Reader betreten einen definierten Read-Side Critical Section.

Konzeptionell:

```text
RCU Read Begin
      ↓
Load Published Pointer
      ↓
Read Data
      ↓
RCU Read End
```

Während dieses Bereichs muss die gelesene Version gültig bleiben.

Reader sollen dabei möglichst keine globalen Locks erwerben müssen.

---

## Writer-Semantik

Writer verändern eine veröffentlichte Struktur nicht unkontrolliert unter aktiven Readern.

Typisches Modell:

```text
Current State
      ↓
Prepare Updated State
      ↓
Atomic Publication
      ↓
Old State
      ↓
Grace Period
      ↓
Reclamation
```

Die konkrete Update-Strategie kann je nach Datenstruktur variieren.

---

## Publication

Die Veröffentlichung eines neuen Zustands muss atomar und mit der erforderlichen Memory-Ordering-Semantik erfolgen.

Ein Reader darf niemals einen nur teilweise initialisierten Zustand beobachten.

RCU baut deshalb auf dem Atomic- und Memory-Ordering-Modell aus `ADR-SYNC-0001` auf.

---

## Grace Period

Eine Grace Period bezeichnet den Zeitraum, nach dessen Ende garantiert werden kann, dass relevante Reader der vorherigen Generation ihren Read-Side Critical Section verlassen haben.

```text
Publish New Version
        ↓
Wait for Grace Period
        ↓
Old Version no longer visible to old readers
        ↓
Safe Reclamation
```

Die genaue Ermittlung einer Grace Period wird in den NPSPECs definiert.

---

## Quiescent States

NovaOS darf Quiescent States verwenden, um den Fortschritt von Grace Periods zu bestimmen.

Ein Quiescent State zeigt an, dass ein Execution Context keinen relevanten älteren RCU-Read-Side Critical Section mehr hält.

Die konkrete Erkennung ist abhängig von:

- Scheduler,
- Preemption-Modell,
- CPU-Zustand,
- RCU-Variante.

---

## Memory Reclamation

RCU und Memory Reclamation sind untrennbar miteinander verbunden.

Nach Entfernung eines Objekts gilt:

```text
Unlinked != Free
```

Das Objekt darf erst freigegeben oder wiederverwendet werden, wenn die notwendige Grace Period abgeschlossen ist.

Dies verhindert Use-after-Free durch Reader älterer Generationen.

---

## Update-Synchronisation

RCU synchronisiert primär Reader mit der Lebensdauer veröffentlichter Versionen.

Es löst nicht automatisch konkurrierende Writer.

Falls mehrere Writer dieselbe Struktur verändern können, benötigen sie eine geeignete Update-Synchronisation, beispielsweise:

- Mutex,
- Spinlock,
- atomare State Transition,
- serialisierte Update Queue.

RCU ersetzt daher nicht sämtliche Locks einer Datenstruktur.

---

## Preemption und Scheduler

RCU muss mit dem präemptiven NovaOS-Scheduler integriert sein.

Der Scheduler muss die für die jeweilige RCU-Variante notwendigen Informationen über:

- aktive Reader,
- Quiescent States,
- CPU-Wechsel,
- Task Preemption,
- CPU Hotplug

bereitstellen beziehungsweise berücksichtigen können.

RCU darf dabei keine versteckte Abhängigkeit von einer bestimmten Scheduling-Klasse erzeugen.

---

## Interrupt-Kontext

RCU kann für geeignete Datenstrukturen auch in Interrupt- und Deferred-Work-Pfaden verwendet werden.

Dabei müssen die verwendete RCU-Domain und ihre Quiescent-State-Semantik ausdrücklich für diese Execution Contexts geeignet sein.

Es darf nicht vorausgesetzt werden, dass jede RCU-Variante automatisch in jedem Kontext korrekt ist.

---

## Realtime

RCU kann Reader-Latenzen reduzieren und damit für bestimmte Realtime-Pfade interessant sein.

Grace-Period-Verhalten und Writer-Seite können jedoch komplexer sein.

Hard-Realtime-Verwendung erfordert deshalb:

- begrenzbare Reader-Pfade,
- kontrollierbare Grace Periods,
- kontrollierte Reclamation,
- keine unbeschränkten Synchronisationswartezeiten im kritischen Pfad.

RCU wird nicht automatisch als realtime-fähig betrachtet.

---

## Speicherverbrauch

Da alte Versionen während einer Grace Period erhalten bleiben, kann RCU zusätzlichen temporären Speicherverbrauch erzeugen.

Die Resource Economy muss diesen Speicherverbrauch berücksichtigen können.

Bei hoher Update-Rate muss insbesondere verhindert werden, dass ausstehende Reclamation unbegrenzt wächst.

---

## NUMA und Cache-Verhalten

RCU kann Cache-Coherence-Kosten reduzieren, da Reader häufig keine gemeinsam veränderte Lock-State-Cache-Line benötigen.

Auf NUMA-Systemen können RCU-Metadaten und Reclamation dennoch relevante Kosten verursachen.

Die Implementierung soll deshalb mit Topology- und NUMA-Awareness kompatibel sein.

---

## Hotplug

CPU Hotplug darf Grace Periods nicht dauerhaft blockieren.

Das RCU-System muss CPU-Zustandsänderungen mit dem Hardware- und Scheduler-Lifecycle koordinieren.

Eine offline gehende CPU muss bezüglich ausstehender RCU-Reader eindeutig behandelt werden.

---

## Deterministic Mode

RCU darf im Deterministic Mode verwendet werden, wenn Grace-Period- und Reclamation-Verhalten mit den geforderten Determinismus-Eigenschaften vereinbar sind.

Asynchrone Reclamation darf keine beobachtbare nicht deterministische Semantik erzeugen, wenn der Execution Contract dies verbietet.

---

## Architecture Introspection

RCU muss introspektierbar sein.

Mindestens relevant sind:

```text
RCU Domain
Current Generation
Pending Grace Periods
Pending Reclamation
Oldest Pending Generation
Memory awaiting reclamation
```

Diagnosewerkzeuge sollen insbesondere ungewöhnlich lange Grace Periods und wachsende Reclamation Backlogs erkennen können.

---

## Fail-Safe Verhalten

Kann nicht sicher festgestellt werden, dass eine Grace Period abgeschlossen ist, darf der alte Zustand nicht freigegeben werden.

Es gilt:

```text
Unknown reader state
        ↓
Do not reclaim
```

Speicherverlust beziehungsweise verzögerte Reclamation ist in diesem Fall sicherer als ein Use-after-Free.

Dauerhafte Störungen müssen jedoch diagnostizierbar und durch definierte Recovery-Mechanismen behandelbar sein.

---

## Normative Anforderungen

1. NovaOS MUSS RCU als spezialisierten Synchronisationsmechanismus für geeignete leselastige Kernelstrukturen unterstützen können.
2. RCU DARF NICHT als universelle Synchronisationsstrategie verwendet werden.
3. Der Einsatz von RCU MUSS durch ein überwiegend leselastiges Zugriffsmuster oder vergleichbare Anforderungen begründet sein.
4. RCU-Reader SOLLEN ohne klassischen globalen Reader Lock arbeiten können.
5. Read-Side Critical Sections MÜSSEN eindeutig definiert sein.
6. Veröffentlichte Zustände MÜSSEN vollständig initialisiert sein, bevor Reader sie beobachten können.
7. Publication MUSS mit geeigneter atomarer Memory-Ordering-Semantik erfolgen.
8. Alte Zustände DÜRFEN vor Abschluss der notwendigen Grace Period NICHT freigegeben oder unsicher wiederverwendet werden.
9. Grace Periods MÜSSEN eine eindeutig definierte Semantik besitzen.
10. Quiescent States MÜSSEN für die verwendete RCU-Variante eindeutig definiert sein.
11. RCU MUSS mit dem NovaOS-Scheduler und dessen Preemption-Modell integrierbar sein.
12. CPU Hotplug MUSS bei der Bestimmung von Grace Periods berücksichtigt werden.
13. RCU MUSS sichere Memory Reclamation gewährleisten.
14. Das Entfernen eines Objekts aus einer Struktur DARF NICHT automatisch dessen sofortige Freigabe bedeuten.
15. Konkurrierende Writer MÜSSEN bei Bedarf separat synchronisiert werden.
16. RCU DARF NICHT als automatischer Ersatz für Writer-Synchronisation betrachtet werden.
17. RCU-Verwendung im Interrupt-Kontext MUSS explizit für die jeweilige RCU-Domain zulässig sein.
18. Hard-Realtime-Verwendung MUSS begrenzbares Verhalten nachweisen.
19. Ausstehende RCU-Reclamation MUSS der Resource Economy zurechenbar sein.
20. Unbegrenztes Wachstum ausstehender Reclamation MUSS erkennbar und kontrollierbar sein.
21. RCU MUSS mit NUMA- und Topology-Aware Kernelimplementierungen kompatibel sein.
22. Deterministic Mode MUSS RCU-Verhalten bei Bedarf einschränken können.
23. Grace Periods und Reclamation Backlogs MÜSSEN introspektierbar sein.
24. Bei unbekanntem Reader-Zustand DARF Speicher NICHT optimistisch freigegeben werden.
25. RCU MUSS auf dem definierten NovaOS-Atomic- und Memory-Ordering-Modell aufbauen.
26. Systemkorrektheit MUSS Vorrang vor frühzeitiger Memory Reclamation besitzen.

---

## Konsequenzen

### Positive Konsequenzen

- sehr geringe Reader-Overheads für geeignete Kernstrukturen,
- weniger Reader/Writer-Lock-Contention,
- bessere Skalierbarkeit auf vielen CPU-Kernen,
- geringere Cache-Coherence-Kosten auf häufig gelesenen Strukturen,
- sichere Versionierung und verzögerte Freigabe gelesener Objekte.

### Negative Konsequenzen

- Writer- und Reclamation-Seite wird komplexer,
- alte Versionen benötigen temporär zusätzlichen Speicher,
- Scheduler und RCU müssen eng bei Grace Periods kooperieren,
- falsche Lifetime-Behandlung kann schwerwiegende Use-after-Free-Fehler verursachen.

---

## Verworfene Alternativen

### Reader/Writer Locks für alle leselastigen Strukturen

Verworfen.

Sie bleiben verfügbar, können aber bei extrem häufigen Reads unnötige Reader- und Cache-Coherence-Kosten erzeugen.

### RCU für alle Kernelstrukturen

Verworfen.

Bei schreibintensiven oder einfachen Strukturen überwiegt häufig die zusätzliche Komplexität.

### Alte Objekte unmittelbar nach Publication freigeben

Verworfen.

Reader können weiterhin Referenzen auf die vorherige Version besitzen.

### RCU ersetzt sämtliche Writer Locks

Verworfen.

RCU löst primär Reader-Lifetime- und Publication-Probleme; konkurrierende Updates können weiterhin Synchronisation benötigen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-SYNC-0002_Lock_Free_Strukturen_gezielt_einsetzen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-RCU-0001`
- `NPSPEC-SYNC-RCU-READ-0001`
- `NPSPEC-SYNC-RCU-PUBLISH-0001`
- `NPSPEC-SYNC-RCU-GRACE-PERIOD-0001`
- `NPSPEC-SYNC-RCU-QUIESCENT-STATE-0001`
- `NPSPEC-SYNC-RCU-RECLAMATION-0001`
- `NPSPEC-SYNC-RCU-SCHEDULER-0001`
- `NPSPEC-SYNC-RCU-TEST-0001`

---

## Ergebnis

NovaOS verwendet RCU gezielt für Kernstrukturen, bei denen sehr häufige Reads und vergleichsweise seltene Updates auftreten:

```text
Many Readers
     ↓
Current Published State
     ↑
Atomic Publication
     ↑
Writer creates new state

Old State
     ↓
Grace Period
     ↓
Safe Reclamation
```

RCU ergänzt Atomics, Locks und Lock-Free-Strukturen, ersetzt diese jedoch nicht.

Die zentrale Architekturregel lautet:

```text
Optimize the common read path.

Publish updates atomically.

Wait before reclaiming old state.

Use RCU only where read-mostly behavior justifies it.
```