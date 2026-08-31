# ADR-SYNC-0002 – Lock-Free-Strukturen gezielt einsetzen

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / Concurrency / Lock-Free / Datenstrukturen

## Kontext

Mit `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation` stellt NovaOS atomare Operationen und ein definiertes Memory Model als Grundlage der Low-Level-Synchronisation bereit.

Darauf können lockfreie Datenstrukturen aufgebaut werden.

Lock-Free-Verfahren können insbesondere in stark parallelen oder latenzkritischen Kernelpfaden Vorteile bieten:

- keine Blockierung durch einen Lock-Owner,
- geringere Lock-Contention,
- bessere Skalierbarkeit bei geeigneten Zugriffsmustern,
- Verwendung in bestimmten Interrupt- oder Scheduler-Pfaden,
- Fortschritt auch bei verzögerten Execution Contexts.

Lock-Free ist jedoch nicht automatisch schneller, einfacher oder besser.

Unter hoher Contention können atomare Retry-Schleifen erhebliche Kosten verursachen. Zusätzlich entstehen komplexe Anforderungen an Memory Ordering, Objektlebensdauer, ABA-Schutz und Memory Reclamation.

NovaOS soll Lock-Free deshalb **gezielt und begründet**, nicht als allgemeines Architekturziel einsetzen.

---

## Entscheidung

NovaOS verwendet Lock-Free-Datenstrukturen nur dort, wo sie gegenüber einfacheren Synchronisationsmechanismen einen nachweisbaren architektonischen oder messbaren praktischen Vorteil besitzen.

```text
Synchronization Requirement
        ↓
Can simple synchronization satisfy it?
        ├── Yes → prefer simple mechanism
        │
        └── No / significant limitation
                  ↓
           Evaluate Lock-Free
                  ↓
       Correctness + Progress
       + Contention + Cost
                  ↓
           Select mechanism
```

Lock-Free wird damit als spezialisierter Synchronisationsmechanismus behandelt, nicht als Standardlösung.

---

## Grundprinzip

```text
Prefer the simplest correct synchronization.

Use lock-free where blocking is harmful
or scalability clearly benefits.

Do not use lock-free for prestige.
```

---

## Bevorzugte Einsatzgebiete

Lock-Free-Strukturen sind insbesondere für Pfade relevant, bei denen mindestens eine der folgenden Eigenschaften vorliegt:

- Blocking ist nicht zulässig,
- Lock-Owner-Stalls wären besonders problematisch,
- sehr hohe Parallelität erzeugt erhebliche Lock-Contention,
- kurze atomare Zustandsübergänge dominieren,
- Messungen zeigen einen klaren Skalierungsvorteil.

Mögliche Kandidaten sind:

```text
Scheduler Queues
Completion Queues
Event Queues
Interrupt Communication
Reference Counters
State Machines
Telemetry Buffers
```

Die Aufnahme in diese Liste bedeutet nicht, dass die jeweilige Struktur zwingend lock-free implementiert wird.

---

## Kein Lock-Free-by-Default

Normale Kernel-Datenstrukturen sollen nicht automatisch lock-free implementiert werden.

Wenn beispielsweise ein kurzer Spinlock oder Mutex:

- einfacher,
- besser überprüfbar,
- ausreichend performant,
- leichter deterministisch analysierbar

ist, soll dieser bevorzugt werden.

---

## Progress Guarantees

Jede lockfreie Struktur muss ihre tatsächliche Progress-Garantie definieren.

Mögliche Kategorien sind:

```text
Wait-Free
Lock-Free
Obstruction-Free
```

Die Verwendung atomarer Operationen allein reicht nicht aus, um eine dieser Garantien zu beanspruchen.

Insbesondere gilt:

```text
Atomic != Lock-Free
Lock-Free != Wait-Free
Lock-Free != Realtime
```

---

## Retry-Schleifen

Viele Lock-Free-Algorithmen basieren auf Compare-and-Exchange-Retry-Schleifen.

```text
Read State
    ↓
Compute New State
    ↓
CAS
 ┌──┴──┐
Fail Success
 ↓
Retry
```

Solche Schleifen müssen auf Contention und mögliche Starvation untersucht werden.

Unbegrenzte Retry-Kosten dürfen nicht ignoriert werden.

---

## Contention

Lock-Free kann unter hoher Contention schlechter skalieren als eine geeignete Lock-basierte Lösung.

Ursachen können sein:

- wiederholte CAS-Fehlschläge,
- Cache-Line-Bouncing,
- erhöhte Memory-Coherence-Kosten,
- konkurrierende Schreibzugriffe.

Die Wahl einer Lock-Free-Struktur muss deshalb das reale Zugriffsmuster berücksichtigen.

---

## Memory Ordering

Lock-Free-Strukturen müssen die in `ADR-SYNC-0001` definierte Atomic- und Memory-Ordering-Semantik verwenden.

Jede notwendige Synchronisationsbeziehung muss explizit nachvollziehbar sein.

Architekturspezifische Annahmen über ein besonders starkes CPU-Memory-Model sind unzulässig.

---

## Memory Reclamation

Lock-Free-Datenstrukturen dürfen Speicher nicht freigeben, solange konkurrierende Execution Contexts noch darauf zugreifen können.

Dafür muss ein geeignetes Memory-Reclamation-Verfahren verwendet werden.

Mögliche Mechanismen umfassen beispielsweise:

```text
Epoch-Based Reclamation
Hazard Pointers
Quiescent-State Verfahren
Reference Management
```

Diese ADR legt kein universelles Verfahren fest.

Die konkrete Auswahl wird separat spezifiziert.

---

## ABA

CAS-basierte Strukturen müssen prüfen, ob das ABA-Problem relevant ist.

Falls erforderlich, müssen geeignete Gegenmaßnahmen eingesetzt werden, beispielsweise:

```text
Generation Counter
Tagged Pointer
Versioned State
```

ABA-Sicherheit darf nicht implizit angenommen werden.

---

## Scheduler und Interrupts

Lock-Free-Strukturen können besonders für kurze Kommunikationspfade zwischen:

- CPUs,
- Scheduler-Kontexten,
- Interrupt Handlern,
- Deferred Work

geeignet sein.

Sie dürfen jedoch nicht dazu führen, dass komplexe Logik in unbeschränkte atomare Retry-Schleifen verlagert wird.

Für Interrupt-Pfade muss das Worst-Case-Verhalten besonders berücksichtigt werden.

---

## Realtime

Lock-Free wird nicht automatisch als realtime-fähig betrachtet.

Hard-Realtime-Pfade benötigen begrenzbares Verhalten.

Ein Lock-Free-Algorithmus mit theoretisch unbegrenzter Wiederholung erfüllt diese Anforderung nicht allein aufgrund seiner Lock-Freiheit.

Wait-Free- oder anderweitig begrenzbare Verfahren können für entsprechende Realtime-Pfade geeigneter sein.

---

## Deterministic Mode

Lock-Free-Strukturen bleiben im Deterministic Mode zulässig, wenn ihre Verwendung mit den geforderten Determinismus-Eigenschaften vereinbar ist.

Algorithmen, deren beobachtbares Verhalten stark von konkurrierenden CAS-Races oder unkontrollierter Retry-Reihenfolge abhängt, müssen gegebenenfalls durch deterministischere Varianten ersetzt oder eingeschränkt werden.

---

## Structured Concurrency

Lock-Free-Strukturen sind Implementierungsmechanismen.

Sie ersetzen keine systemweiten Konzepte wie:

- Ownership,
- Task Groups,
- Execution Scopes,
- Cancellation,
- Join,
- Resource Accounting.

Eine lockfreie Queue darf insbesondere nicht zur Umgehung strukturierter Task-Lebensdauer verwendet werden.

---

## Zero-Copy

Lock-Free-Strukturen können mit Zero-Copy-Pipelines kombiniert werden.

Dabei müssen Ownership-Transitions und Objektlebensdauer eindeutig definiert bleiben.

```text
Producer
    ↓
Atomic Ownership Transfer
    ↓
Queue
    ↓
Consumer
```

Lock-Free und Zero-Copy sind unabhängige Eigenschaften und müssen jeweils separat korrekt umgesetzt werden.

---

## Fallback und Portabilität

NovaOS darf für unterschiedliche Hardwarearchitekturen unterschiedliche interne Implementierungen derselben Datenstruktur verwenden.

Falls eine Plattform bestimmte atomare Fähigkeiten nicht effizient bereitstellt, kann eine alternative Synchronisationsimplementierung verwendet werden, sofern die äußere Semantik erhalten bleibt.

Damit bleibt die API vom konkreten Lock-Free-Algorithmus getrennt.

---

## Messbarkeit

Performance darf nicht allein theoretisch begründet werden.

Für performancekritische Lock-Free-Strukturen sollen insbesondere beobachtbar sein:

- Operationsrate,
- CAS-Fehlerrate,
- Retry-Anzahl,
- Contention,
- Cache-Coherence-Kosten,
- Tail Latency.

Dadurch kann überprüft werden, ob Lock-Free tatsächlich einen Vorteil liefert.

---

## Introspection und Debugging

Lock-Free-Strukturen müssen soweit technisch sinnvoll diagnostizierbar sein.

Debug-Builds und Analysewerkzeuge sollen insbesondere Fehler in:

- Memory Ordering,
- Memory Reclamation,
- Ownership,
- ABA-Schutz,
- Progress-Verhalten

erkennbar machen können.

---

## Normative Anforderungen

1. NovaOS DARF Lock-Free-Strukturen gezielt als Low-Level-Synchronisationsmechanismus einsetzen.
2. Lock-Free DARF NICHT als Standardimplementierung aller parallelen Datenstrukturen gelten.
3. Die einfachste ausreichend korrekte Synchronisationslösung SOLL bevorzugt werden.
4. Lock-Free SOLL verwendet werden, wenn Blocking unzulässig ist oder ein relevanter Skalierungs- beziehungsweise Latenzvorteil besteht.
5. Lock-Free-Strukturen MÜSSEN auf dem definierten NovaOS-Atomic- und Memory-Ordering-Modell aufbauen.
6. Architekturspezifische Memory-Ordering-Annahmen DÜRFEN NICHT in portable Lock-Free-Komponenten einfließen.
7. Jede Lock-Free-Struktur MUSS ihre Progress-Garantie dokumentieren.
8. Atomic Operationen DÜRFEN NICHT automatisch als Nachweis einer Lock-Free-Garantie gelten.
9. Lock-Free DARF NICHT automatisch als Wait-Free oder Realtime interpretiert werden.
10. CAS-Retry-Schleifen MÜSSEN auf Contention und Starvation untersucht werden.
11. Lock-Free-Strukturen MÜSSEN sichere Objektlebensdauer gewährleisten.
12. Benötigte Memory-Reclamation-Verfahren MÜSSEN explizit definiert werden.
13. ABA-Risiken MÜSSEN bei CAS-basierten Strukturen geprüft werden.
14. Notwendige ABA-Schutzmechanismen MÜSSEN explizit implementiert werden.
15. Interrupt-Pfade DÜRFEN keine unkontrollierten Lock-Free-Retry-Kosten voraussetzen.
16. Hard-Realtime-Code MUSS begrenzbares Progress-Verhalten besitzen.
17. Deterministic Mode MUSS ungeeignete Lock-Free-Verfahren einschränken oder ersetzen können.
18. Lock-Free-Strukturen DÜRFEN Structured Concurrency und Ownership NICHT umgehen.
19. Zero-Copy- und Lock-Free-Mechanismen MÜSSEN ihre Ownership-Semantik eindeutig koordinieren.
20. Die öffentliche Semantik einer Datenstruktur DARF NICHT unnötig von ihrer Lock-Free-Implementierung abhängen.
21. Plattformabhängige alternative Implementierungen MÜSSEN möglich bleiben.
22. Performancekritische Lock-Free-Strukturen SOLLEN messbare Contention- und Retry-Metriken bereitstellen.
23. Lock-Free-Optimierungen MÜSSEN gegen einfachere Synchronisationsalternativen evaluierbar sein.
24. Systemkorrektheit DARF NICHT von einem angenommenen Performancevorteil einer Lock-Free-Struktur abhängen.

---

## Konsequenzen

### Positive Konsequenzen

- Lock-Free wird dort eingesetzt, wo es tatsächlich Vorteile bringt,
- unnötige Synchronisationskomplexität wird vermieden,
- kritische Kernelpfade können ohne Lock-Owner-Abhängigkeit arbeiten,
- Portabilität zwischen unterschiedlichen CPU-Architekturen bleibt erhalten,
- Performanceentscheidungen werden messbar statt dogmatisch getroffen.

### Negative Konsequenzen

- mehrere Synchronisationsstrategien müssen unterstützt werden,
- Lock-Free-Komponenten benötigen besonders sorgfältige Verifikation,
- Memory Reclamation und ABA-Schutz erhöhen die Komplexität,
- Performancevorteile sind workload- und hardwareabhängig.

---

## Verworfene Alternativen

### Lock-Free überall

Verworfen.

Dies erhöht Komplexität und Fehlerrisiko, ohne generell bessere Performance zu garantieren.

### Lock-Free grundsätzlich vermeiden

Verworfen.

Bestimmte Kernel-, Scheduler- und Interrupt-Pfade profitieren von blockierungsfreien Strukturen oder benötigen sie.

### Atomics ohne definierte Progress-Garantie als Lock-Free bezeichnen

Verworfen.

Atomizität und Progress sind unterschiedliche Eigenschaften.

### Ein universeller Lock-Free-Algorithmus für alle Plattformen

Verworfen.

Hardwarefähigkeiten, Memory Models und Workload-Eigenschaften unterscheiden sich.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-LOCKFREE-0001`
- `NPSPEC-SYNC-LOCKFREE-PROGRESS-0001`
- `NPSPEC-SYNC-LOCKFREE-QUEUE-0001`
- `NPSPEC-SYNC-MEMORY-RECLAMATION-0001`
- `NPSPEC-SYNC-ABA-0001`
- `NPSPEC-SYNC-LOCKFREE-INSTRUMENTATION-0001`
- `NPSPEC-SYNC-LOCKFREE-TEST-0001`

---

## Ergebnis

NovaOS betrachtet Lock-Free nicht als Selbstzweck, sondern als gezieltes Werkzeug:

```text
Synchronization Problem
        ↓
Simplest Correct Mechanism
        ↓
Is Blocking / Contention a Problem?
       ┌─────────┴─────────┐
      No                  Yes
       ↓                    ↓
Simple Synchronization   Evaluate Lock-Free
                            ↓
                 Correctness + Progress
                 + Measured Benefit
```

Die zentrale Architekturregel lautet:

```text
Lock-free where it matters.

Locks where they are simpler.

Measure before optimizing.

Correctness always comes first.
```