# ADR-SYNC-0001 – Atomics als Fundament der Low-Level-Synchronisation

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / Concurrency / Atomics / Memory Model

## Kontext

NovaOS benötigt Synchronisationsmechanismen für parallele Ausführung auf:

- mehreren CPU-Kernen,
- SMT-Threads,
- Kernel-Threads,
- Interrupt-Kontexten,
- lockfreien Datenstrukturen,
- Scheduler- und Runtime-Strukturen.

Höhere Synchronisationsmechanismen wie:

- Spinlocks,
- Mutexes,
- Semaphores,
- Wait Queues,
- Reader/Writer Locks,
- lockfreie Queues

benötigen ein gemeinsames, eindeutig definiertes Low-Level-Fundament.

Direkte Verwendung architekturspezifischer Instruktionen im gesamten Kernel würde Synchronisationssemantik, Portabilität und das NovaOS-Memory-Model fragmentieren.

---

## Entscheidung

NovaOS definiert **Atomics als grundlegende Low-Level-Synchronisationsabstraktion**.

```text
Hardware Atomic Instructions
          ↓
Architecture / HAL Layer
          ↓
Nova Atomic Operations
          ↓
Low-Level Synchronization
          ↓
Higher Synchronization Primitives
```

Architekturspezifische atomare Instruktionen werden hinter einer einheitlichen NovaOS-Abstraktion gekapselt.

Höhere Synchronisationsprimitive bauen auf diesem Fundament auf.

---

## Grundprinzip

```text
Atomics provide the primitive.

Memory ordering defines visibility.

Synchronization primitives provide policy.

Applications use higher abstractions by default.
```

---

## Atomare Operationen

Die Low-Level-Abstraktion muss mindestens die für Synchronisationsprimitive notwendigen Operationen bereitstellen können:

```text
Atomic Load
Atomic Store
Exchange
Compare-and-Exchange
Fetch-and-Modify
```

Konkrete Datentypen, Breiten und zusätzliche Operationen werden in den zugehörigen NPSPECs definiert.

---

## Compare-and-Exchange

Compare-and-Exchange bildet einen zentralen Baustein für lockfreie Zustandsübergänge.

Konzeptionell:

```text
if current == expected:
    current = desired
    success
else:
    expected = current
    failure
```

Die API muss erfolgreiche und fehlgeschlagene Operationen eindeutig unterscheiden.

---

## Memory Ordering

Atomizität allein definiert nicht die Sichtbarkeit anderer Speicheroperationen.

NovaOS benötigt deshalb explizite Memory-Ordering-Semantik.

Mindestens müssen geeignete Äquivalente für folgende Ordnungen ausdrückbar sein:

```text
Relaxed
Acquire
Release
Acquire-Release
Sequentially Consistent
```

Die exakte NovaABI-/NovaLang-Repräsentation wird separat spezifiziert.

---

## Acquire und Release

Acquire/Release bildet das bevorzugte Modell für viele Synchronisationspfade.

Typisches Muster:

```text
Writer:
    write data
    ↓
    Release Store

Reader:
    Acquire Load
    ↓
    read data
```

Damit kann ein Happens-Before-Verhältnis zwischen beteiligten Ausführungskontexten hergestellt werden.

---

## Sequential Consistency

Sequentially Consistent Atomics bleiben verfügbar, werden jedoch nicht als einziges Synchronisationsmodell vorgeschrieben.

NovaOS soll schwächere, aber ausreichende Memory Orders verwenden können, wenn deren Korrektheit eindeutig begründet ist.

Performanceoptimierung darf niemals eine notwendige Speicherordnung entfernen.

---

## Architekturunabhängigkeit

Das NovaOS-Atomic-Modell darf nicht auf die Eigenschaften einer einzelnen CPU-Architektur zugeschnitten werden.

Insbesondere darf Code nicht implizit voraussetzen, dass alle Zielarchitekturen dasselbe Memory Ordering wie x86 bereitstellen.

```text
Nova Atomic Semantics
        ↓
x86 Implementation
ARM Implementation
Future Architecture
```

Jedes Backend muss mindestens die vom gemeinsamen Modell verlangte Semantik gewährleisten.

---

## Compiler Ordering

Synchronisation muss sowohl CPU- als auch Compiler-Reordering berücksichtigen.

Compiler Barriers und Hardware Memory Barriers sind unterschiedliche Mechanismen und dürfen nicht semantisch verwechselt werden.

Die Atomic-Abstraktion muss die erforderlichen Compiler- und Hardware-Eigenschaften gemeinsam korrekt abbilden.

---

## Memory Barriers

Explizite Memory Barriers dürfen für Low-Level-Systemcode verfügbar sein, wenn Atomics allein die benötigte Synchronisationsbeziehung nicht ausreichend ausdrücken.

Sie sollen jedoch nicht als Ersatz für klar definierte Atomic-Operationen verwendet werden.

---

## Lock-Free ist kein Selbstzweck

Atomics ermöglichen lockfreie Algorithmen, verpflichten NovaOS jedoch nicht dazu, jede Synchronisation lockfrei umzusetzen.

Die Wahl zwischen:

```text
Lock-Free
Spinlock
Mutex
Blocking Synchronization
```

erfolgt anhand von:

- Korrektheit,
- Contention,
- Latenz,
- Realtime-Anforderungen,
- Komplexität,
- Plattformverhalten.

---

## Progress Guarantees

Lockfreie Strukturen müssen ihre Progress-Garantie explizit definieren können.

Relevante Kategorien sind beispielsweise:

```text
Wait-Free
Lock-Free
Obstruction-Free
Blocking
```

Eine Datenstruktur darf nicht allein aufgrund der Verwendung von Atomics als `lock-free` bezeichnet werden.

---

## ABA-Problem

Compare-and-Exchange-basierte Algorithmen können vom ABA-Problem betroffen sein.

Die Atomic-Abstraktion löst dieses Problem nicht automatisch.

Betroffene Datenstrukturen müssen geeignete Mechanismen verwenden, beispielsweise:

- Version Tags,
- Generation Counters,
- geeignete Memory-Reclamation-Verfahren.

Die konkrete Strategie gehört in die jeweilige NPSPEC.

---

## Memory Reclamation

Lockfreie Datenstrukturen benötigen kontrollierte Objektlebensdauer.

Atomics allein verhindern nicht, dass ein anderer Execution Context auf bereits freigegebenen Speicher zugreift.

NovaOS muss deshalb geeignete Memory-Reclamation-Mechanismen separat definieren.

---

## Interrupt-Kontext

Atomics müssen in dafür vorgesehenen Low-Level-Pfaden auch zwischen normalen Kernel-Kontexten und Interrupt-Kontexten verwendbar sein.

Dies bedeutet nicht, dass jede atomare Operation automatisch für jeden Interrupt-Pfad geeignet ist.

Blocking Synchronization bleibt im Interrupt-Kontext grundsätzlich unzulässig.

---

## Realtime

Atomics können für Realtime-Pfade geeignet sein, sofern:

- ihre Ausführungszeit ausreichend begrenzt ist,
- keine unkontrollierten Retry-Schleifen entstehen,
- die verwendete Hardware die benötigten Eigenschaften bereitstellt.

Lock-free bedeutet nicht automatisch realtime-fähig.

---

## Deterministic Mode

Deterministic Mode verändert die Semantik atomarer Operationen nicht.

Er kann jedoch Algorithmen einschränken, deren Ergebnis oder Fortschritt stark von nicht deterministischer Contention abhängt.

Atomics bleiben auch im Deterministic Mode korrekt synchronisiert.

---

## Atomics und Scheduler

Der Scheduler darf Atomics für besonders kurze und kritische interne Zustandsübergänge verwenden.

Beispiele sind:

- Runnable-State-Transitions,
- Queue-Metadaten,
- Flags,
- Referenzzähler,
- Wakeup-Zustände.

Komplexe Scheduler-Koordination soll nicht automatisch in große CAS-Schleifen überführt werden.

---

## Atomics und Structured Concurrency

Atomics sind ein Low-Level-Mechanismus und ersetzen nicht:

- Task Ownership,
- Execution Scopes,
- Cancellation,
- Join,
- Structured Concurrency.

Sie dienen der Implementierung interner Synchronisation, nicht der Modellierung von Lebensdauer und Ownership.

---

## Atomics und Zero-Copy

Zero-Copy-Strukturen können atomare Ownership- und State-Transitions benötigen.

Beispiel:

```text
Producer Ownership
        ↓
Atomic Transition
        ↓
Consumer Ownership
```

Die konkrete Ownership-Semantik wird durch den jeweiligen Buffer- oder Object-Contract definiert.

---

## Public API

Low-Level-Atomics dürfen für Systemkomponenten und NovaLang verfügbar sein, sollen aber nicht die bevorzugte Synchronisationsabstraktion für normalen Anwendungscode darstellen.

Standardmäßig sollen Entwickler höherwertige Mechanismen verwenden.

Direkte Atomics sind für Fälle vorgesehen, in denen deren Semantik tatsächlich erforderlich ist.

---

## Introspection und Debugging

Debug- und Analysewerkzeuge sollen atomare Synchronisationsfehler soweit technisch möglich diagnostizieren können.

Dazu gehören insbesondere:

- Data Races,
- fehlerhafte Memory Ordering Annahmen,
- hohe CAS-Fehlerraten,
- übermäßige Contention.

Die Instrumentierung darf im Produktionspfad deaktivierbar beziehungsweise kostenkontrolliert sein.

---

## Fail-Safe Defaults

Wenn keine spezielle Memory Order erforderlich und ausreichend begründet ist, soll eine sichere Standardsemantik verwendet werden.

Optimierungen zu schwächeren Memory Orders müssen explizit und überprüfbar erfolgen.

Architekturspezifische Annahmen dürfen nicht still in portable Kernel-Komponenten einfließen.

---

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche Low-Level-Atomic-Abstraktion bereitstellen.
2. Höhere Low-Level-Synchronisationsprimitive SOLLEN auf dieser Abstraktion aufbauen.
3. Atomare Operationen MÜSSEN architekturunabhängige Semantik besitzen.
4. Hardware-Backends MÜSSEN mindestens die spezifizierte Atomic-Semantik gewährleisten.
5. Atomic Load, Store, Exchange und Compare-and-Exchange MÜSSEN unterstützt werden.
6. Geeignete Fetch-and-Modify-Operationen MÜSSEN unterstützt werden können.
7. Memory Ordering MUSS explizit definierbar sein.
8. Acquire- und Release-Semantik MÜSSEN unterstützt werden.
9. Sequentially Consistent Ordering MUSS verfügbar sein.
10. Schwächere Memory Orders DÜRFEN nur verwendet werden, wenn sie die erforderliche Synchronisationssemantik erhalten.
11. Compiler- und Hardware-Reordering MÜSSEN korrekt berücksichtigt werden.
12. Explizite Memory Barriers MÜSSEN für notwendige Low-Level-Fälle verfügbar sein.
13. NovaOS DARF NICHT von einem x86-spezifisch starken Memory Model ausgehen.
14. Atomics DÜRFEN NICHT automatisch als Ersatz für höherwertige Synchronisation verwendet werden.
15. Lockfreie Algorithmen MÜSSEN ihre Progress-Garantien eindeutig definieren.
16. CAS-basierte Strukturen MÜSSEN mögliche ABA-Probleme berücksichtigen.
17. Lockfreie Strukturen MÜSSEN sichere Memory-Reclamation gewährleisten.
18. Blocking Synchronization DARF NICHT durch Atomics im Interrupt-Kontext versteckt werden.
19. Realtime-Code DARF Atomics nur mit ausreichend begrenztem Verhalten verwenden.
20. Unbegrenzte CAS-Retry-Schleifen DÜRFEN NICHT als Hard-Realtime-Garantie behandelt werden.
21. Deterministic Mode DARF die definierte Atomic-Semantik NICHT verändern.
22. Atomics DÜRFEN Structured Concurrency und Ownership NICHT ersetzen.
23. Zero-Copy-Ownership-Transitions MÜSSEN atomar modellierbar sein, wenn konkurrierender Zugriff möglich ist.
24. Anwendungen SOLLEN standardmäßig höherwertige Synchronisationsprimitive verwenden.
25. Architekturspezifische Atomic-Instruktionen SOLLEN außerhalb der zuständigen Low-Level-Schicht nicht direkt verwendet werden.
26. Debugging- und Analysemechanismen SOLLEN Atomic Contention und Synchronisationsfehler diagnostizierbar machen.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches Fundament für alle Low-Level-Synchronisationsprimitive,
- portable Synchronisationssemantik über x86, ARM und zukünftige Architekturen,
- explizites Memory Model,
- Grundlage für effiziente lockfreie Datenstrukturen,
- bessere Trennung zwischen Hardwaremechanismus und Synchronisationspolicy.

### Negative Konsequenzen

- korrektes Memory Ordering erfordert hohe Implementierungsdisziplin,
- lockfreie Algorithmen bleiben komplex,
- unterschiedliche Hardware-Memory-Models erhöhen den Testaufwand,
- falsche Verwendung von Atomics kann schwer reproduzierbare Fehler erzeugen.

---

## Verworfene Alternativen

### Ausschließlich Locks verwenden

Verworfen.

Einige Kernelpfade benötigen atomare Zustandsübergänge als Fundament für Locks und andere Synchronisationsmechanismen.

### Architekturspezifische Atomics direkt verwenden

Verworfen.

Dies würde portable Kernel-Komponenten an einzelne CPU-Memory-Models koppeln.

### Nur Sequential Consistency unterstützen

Verworfen.

Dies wäre einfach, würde aber notwendige Optimierungsmöglichkeiten auf schwächer geordneten Architekturen unnötig einschränken.

### Atomics als bevorzugte Anwendungssynchronisation

Verworfen.

Direkte Atomics sind fehleranfällig und sollen dort eingesetzt werden, wo höherwertige Synchronisationsmodelle nicht ausreichen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-MEMORY-ORDER-0001`
- `NPSPEC-SYNC-CAS-0001`
- `NPSPEC-SYNC-BARRIER-0001`
- `NPSPEC-SYNC-LOCKFREE-0001`
- `NPSPEC-SYNC-MEMORY-RECLAMATION-0001`
- `NPSPEC-SYNC-ATOMIC-TEST-0001`

---

## Ergebnis

Atomics bilden das gemeinsame Low-Level-Fundament der NovaOS-Synchronisationsarchitektur:

```text
CPU Atomic Instructions
        ↓
Nova Atomic Model
        ↓
Memory Ordering
        ↓
Synchronization Primitives
        ↓
Kernel / Runtime / System Services
```

Sie stellen atomare Zustandsübergänge und definierte Speicherordnung bereit, ohne höherwertige Synchronisations-, Ownership- oder Concurrency-Modelle zu ersetzen.

Die zentrale Architekturregel lautet:

```text
Atomics define the primitive.

Memory ordering defines visibility.

Higher abstractions define coordination.

Use the lowest level only when necessary.
```