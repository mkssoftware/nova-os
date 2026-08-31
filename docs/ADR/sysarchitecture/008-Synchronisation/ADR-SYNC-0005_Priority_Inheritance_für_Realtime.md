# ADR-SYNC-0005 – Priority Inheritance für Realtime

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / Scheduler / Realtime / Priority Inversion

## Kontext

In einem präemptiven System kann ein hochpriorisierter oder zeitkritischer Task auf eine Ressource warten, die von einem weniger dringlichen Task gehalten wird.

Ohne Gegenmaßnahmen entsteht Priority Inversion:

```text
High Priority Task
        ↓ waits
Low Priority Lock Owner
        ↓ preempted by
Medium Priority Task
```

Der High-Priority-Task kann dadurch indirekt von beliebig vielen weniger wichtigen Tasks verzögert werden.

Für normale Workloads kann dies hauptsächlich ein Latenzproblem darstellen. Für Realtime-Workloads kann dadurch jedoch eine Deadline oder eine garantierte zeitliche Grenze verletzt werden.

NovaOS benötigt deshalb eine kontrollierte Integration von Priority Inheritance in Scheduler und Synchronisationsarchitektur.

---

## Entscheidung

NovaOS unterstützt **Priority Inheritance (PI)** für owner-basierte Synchronisationsprimitive, insbesondere für Realtime- und andere zeitkritische Workloads.

Wartet ein dringlicherer Task auf eine Ressource, deren Owner weniger dringlich ausgeführt wird, kann der Owner temporär die relevante Scheduling-Dringlichkeit des Waiters erben.

```text
High Priority Waiter
        ↓
      Mutex
        ↓
Low Priority Owner
        ↓
Priority Inheritance
        ↓
Temporarily Elevated Owner
        ↓
Release Mutex
        ↓
Restore Effective Priority
```

Priority Inheritance verändert ausschließlich die für die Auflösung der Blockierung notwendige Scheduling-Dringlichkeit.

Es überträgt keine Security-, Capability-, Trust- oder Resource-Authority.

---

## Grundprinzip

```text
A high-priority task must not be
indefinitely blocked because its
lock owner cannot get CPU time.

Boost the owner only as much
and as long as necessary.
```

---

## Owner-basierte Synchronisation

Priority Inheritance setzt eine eindeutig bestimmbare Owner-Beziehung voraus.

Geeignete Primitive können insbesondere sein:

- Mutexes,
- realtime-fähige Mutex-Varianten,
- andere explizit owner-basierte Locks.

Nicht jedes Synchronisationsprimitive besitzt einen Owner.

Insbesondere dürfen:

- Events,
- allgemeine Wait Queues,
- Condition Variables,
- Semaphores ohne eindeutige Ownership

nicht automatisch als PI-Mutex behandelt werden.

---

## Effective Priority

NovaOS unterscheidet zwischen Basis- und effektiver Scheduling-Dringlichkeit.

```text
Base Priority
     +
Inherited Priority
     ↓
Effective Priority
```

Die Basispriorität eines Tasks wird durch Priority Inheritance nicht dauerhaft verändert.

Nach Wegfall der relevanten Abhängigkeit wird die geerbte Dringlichkeit entfernt.

---

## Transitive Priority Inheritance

Priority Inversion kann über mehrere Lock-Abhängigkeiten propagieren.

Beispiel:

```text
Task A (High)
   ↓ waits for
Task B
   ↓ waits for
Task C (Low)
```

In diesem Fall muss die relevante Dringlichkeit bis zu Task C propagiert werden können:

```text
A → B → C
```

Damit wird verhindert, dass verschachtelte Lock-Abhängigkeiten die PI-Wirkung umgehen.

---

## Mehrere Waiter

Ein Lock Owner kann mehrere Waiter besitzen.

Seine effektive geerbte Dringlichkeit muss die stärkste aktuell relevante Anforderung berücksichtigen.

```text
Owner
 ├── Waiter A: Priority 20
 ├── Waiter B: Priority 40
 └── Waiter C: Priority 30

Effective inherited priority:
    40
```

Nach Wegfall eines Waiters muss die effektive Dringlichkeit aus den verbleibenden Abhängigkeiten neu bestimmt werden.

---

## Scheduler-Integration

Priority Inheritance ist eine gemeinsame Funktion von Synchronisations- und Scheduler-Subsystem.

Der Synchronisationsmechanismus kennt:

```text
Owner
Waiter
Dependency
```

Der Scheduler kennt:

```text
Scheduling Class
Priority
Deadline
CPU Placement
Reservations
```

Beide müssen die resultierende effektive Scheduling-Dringlichkeit konsistent behandeln.

---

## Scheduling-Klassen

Priority Inheritance darf nicht als einfache globale numerische Priorität über alle Scheduling-Klassen hinweg modelliert werden.

NovaOS verwendet einen Mehrklassen-Scheduler.

Daher muss PI die Semantik der beteiligten Scheduling-Klassen berücksichtigen.

Ein Realtime-Waiter kann beispielsweise eine stärkere Scheduling-Behandlung des Owners erforderlich machen, ohne dessen ursprünglichen Execution Contract dauerhaft zu verändern.

Die genaue klassenübergreifende Abbildung wird in den NPSPECs definiert.

---

## Deadline-Integration

Bei Deadline-basierten Workloads kann nicht nur numerische Priorität, sondern auch zeitliche Dringlichkeit relevant sein.

Priority-Inheritance-Infrastruktur muss deshalb mit Deadline Scheduling integrierbar sein.

Konzeptionell:

```text
Waiter Deadline
      ↓
Blocking Dependency
      ↓
Owner Scheduling Urgency
```

Eine geerbte Deadline erzeugt jedoch keine zusätzlichen Ressourcenreserven.

---

## Realtime

Für Realtime Concurrency Domains muss eine geeignete Behandlung von Priority Inversion verfügbar sein.

Hard-Realtime-Synchronisation erfordert zusätzlich, dass:

- kritische Abschnitte begrenzt sind,
- Lock-Abhängigkeiten analysierbar sind,
- Blocking-Zeiten begrenzbar sind,
- benötigte Ressourcen reserviert sind.

Priority Inheritance allein erzeugt keine Hard-Realtime-Garantie.

---

## Resource Reservations

Priority Inheritance darf Scheduling-Dringlichkeit übertragen, aber keine unbegrenzte Ressourcenkapazität erzeugen.

Ein geboosteter Owner bleibt grundsätzlich seiner Resource Economy und seinen Execution-Domain-Regeln zugeordnet.

Falls Realtime-Garantien zusätzliche Budget- oder Reservation-Inheritance benötigen, muss dies explizit durch die Realtime-Spezifikation geregelt werden.

---

## CPU Affinity

Ein Priority Boost darf Required CPU Affinity nicht umgehen.

Ist ein Lock Owner aufgrund harter Affinity- oder Isolation-Constraints nicht auf einer benötigten CPU ausführbar, muss der Scheduler innerhalb der zulässigen CPU-Menge optimieren.

PI erzeugt keine neue CPU-Berechtigung.

---

## Security und Capabilities

Priority Inheritance überträgt ausschließlich Scheduling-Dringlichkeit.

Insbesondere werden nicht übertragen:

```text
Capabilities
Permissions
Trust
Security Identity
Data Authority
Memory Access
Device Access
```

Der Owner führt weiterhin mit seiner eigenen Authority aus.

---

## Futex-artige Synchronisation

`ADR-SYNC-0004_Futex_artige_User_Kernel_Synchronisation` stellt die Grundlage für effiziente Userspace-Synchronisation bereit.

Owner-basierte Futex-Primitive können PI unterstützen, wenn der Kernel die Owner-/Waiter-Beziehung zuverlässig bestimmen kann.

Der uncontended Fast Path soll weiterhin möglichst im Userspace verbleiben.

Der Kernel wird insbesondere bei Contention und notwendiger PI-Koordination beteiligt.

---

## Cancellation und Timeout

Wird ein Waiter durch:

- Cancellation,
- Timeout,
- Deadline-Ablauf,
- Domain-Termination

aus einer Lock-Warteschlange entfernt, muss seine Priority-Inheritance-Wirkung ebenfalls entfernt werden.

Veraltete Priority Boosts dürfen nicht bestehen bleiben.

---

## Lock Release

Beim Freigeben eines Locks muss die zugehörige PI-Abhängigkeit beendet werden.

Besitzt der Owner weitere Locks mit dringlicheren Waitern, kann ein anderer Boost weiterhin bestehen bleiben.

Es gilt daher nicht:

```text
unlock → restore base priority
```

sondern:

```text
unlock
   ↓
recalculate remaining inheritance
   ↓
new effective priority
```

---

## Deadlock

Priority Inheritance verhindert keine Deadlocks.

Beispiel:

```text
Task A owns Lock 1
Task B owns Lock 2

A waits for Lock 2
B waits for Lock 1
```

PI kann die beteiligten Tasks beschleunigen, aber die zyklische Abhängigkeit nicht auflösen.

Deadlock-Prävention, -Erkennung und Lock-Ordering sind separate Mechanismen.

---

## Unbounded Critical Sections

Priority Inheritance kann einen langsamen Owner schneller zur Ausführung bringen.

Es kann jedoch keinen unbeschränkt langen kritischen Abschnitt begrenzen.

Realtime-fähige Locks benötigen deshalb zusätzlich definierte Grenzen für kritische Abschnitte und Blocking-Verhalten.

---

## Lock-Free und RCU

Lock-Free- und RCU-basierte Strukturen besitzen normalerweise keinen klassischen Lock Owner.

Priority Inheritance ist dort daher nicht unmittelbar anwendbar.

Dies ist ein Grund, warum Lock-Free oder RCU für bestimmte read-mostly oder besonders latenzkritische Pfade sinnvoll sein können.

Die Wahl bleibt jedoch gemäß `ADR-SYNC-0002` workloadabhängig.

---

## Deterministic Mode

Priority Inheritance muss auch im Deterministic Mode eindeutig reproduzierbare Regeln besitzen.

Die resultierende effektive Dringlichkeit darf nicht von adaptiven oder KI-basierten Entscheidungen abhängen.

---

## Introspection

Priority-Inheritance-Zustände müssen introspektierbar sein.

Mindestens relevant sind:

```text
Task
Base Priority
Effective Priority
Inherited From
Blocking Resource
Inheritance Chain
Inheritance Duration
```

Dadurch können insbesondere lange Priority-Inversion-Ketten und fehlerhafte Lock-Nutzung diagnostiziert werden.

---

## Fail-Safe Verhalten

Kann bei einem PI-fähigen Primitive die Owner-Beziehung nicht zuverlässig bestimmt werden, darf der Kernel keine beliebige Priority-Inheritance-Beziehung konstruieren.

Für Realtime-Pfade muss eine solche Situation als nicht erfüllbare Synchronisationsanforderung behandelt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Priority Inheritance für geeignete owner-basierte Synchronisationsprimitive unterstützen.
2. Priority Inheritance MUSS insbesondere für Realtime-Synchronisation verfügbar sein.
3. PI DARF nur verwendet werden, wenn eine eindeutige Owner-/Waiter-Beziehung besteht.
4. Nicht-owner-basierte Synchronisationsprimitive DÜRFEN NICHT automatisch PI-Semantik erhalten.
5. Basis- und effektive Scheduling-Dringlichkeit MÜSSEN getrennt behandelt werden.
6. Priority Boosts MÜSSEN temporär sein.
7. Transitive Priority Inheritance MUSS unterstützt werden können.
8. Mehrere gleichzeitig wirkende Inheritance-Beziehungen MÜSSEN korrekt aggregiert werden.
9. Nach Wegfall einer Abhängigkeit MUSS die effektive Dringlichkeit neu bestimmt werden.
10. PI MUSS mit dem Mehrklassen-Scheduler integrierbar sein.
11. PI MUSS mit Deadline Scheduling integrierbar sein.
12. PI DARF keine zusätzlichen Security- oder Capability-Rechte übertragen.
13. PI DARF Required CPU Affinity und Isolation NICHT umgehen.
14. Resource Reservations DÜRFEN nicht implizit durch einen Priority Boost erzeugt werden.
15. Realtime-fähige PI-Primitive MÜSSEN begrenzbares Blocking ermöglichen.
16. Priority Inheritance DARF NICHT als Ersatz für begrenzte kritische Abschnitte betrachtet werden.
17. Futex-artige owner-basierte Synchronisation MUSS mit PI integrierbar sein.
18. Cancellation eines Waiters MUSS dessen Inheritance-Effekt entfernen.
19. Timeout beziehungsweise Deadline-Ablauf MUSS dessen Inheritance-Effekt entfernen.
20. Lock Release MUSS verbleibende Inheritance-Beziehungen korrekt neu bewerten.
21. Priority Inheritance DARF NICHT als Deadlock-Lösung betrachtet werden.
22. Lock-Free- und RCU-Strukturen MÜSSEN nicht künstlich mit Owner-Semantik versehen werden.
23. Deterministic Mode MUSS reproduzierbare PI-Regeln verwenden.
24. PI-Zustände und Inheritance Chains MÜSSEN introspektierbar sein.
25. Adaptive Scheduling DARF die verbindliche PI-Semantik NICHT verändern.
26. Unbekannte Owner-Beziehungen DÜRFEN NICHT optimistisch für Realtime-Garantien verwendet werden.

---

## Konsequenzen

### Positive Konsequenzen

- begrenzt klassische Priority Inversion,
- verbessert Vorhersagbarkeit von Realtime-Synchronisation,
- integriert Mutex-Synchronisation mit dem Mehrklassen-Scheduler,
- unterstützt transitive Lock-Abhängigkeiten,
- bleibt mit Futex-artiger Userspace-Synchronisation kombinierbar.

### Negative Konsequenzen

- Scheduler und Synchronisationssystem werden enger gekoppelt,
- transitive Inheritance erhöht die Implementierungskomplexität,
- mehrere Waiter und verschachtelte Locks erfordern dynamische Neuberechnung,
- Priority Inheritance allein löst weder Deadlocks noch unbeschränkte kritische Abschnitte.

---

## Verworfene Alternativen

### Keine Priority Inheritance

Verworfen.

Unkontrollierte Priority Inversion ist insbesondere für Realtime-Workloads nicht akzeptabel.

### Priority Boost nur für direkte Lock Owner

Verworfen.

Verschachtelte Lock-Abhängigkeiten können transitive Priority Inversion erzeugen.

### Priority Inheritance überträgt vollständigen Execution Contract

Verworfen.

Der Owner benötigt Scheduling-Dringlichkeit, aber keine Security-, Capability- oder sonstige Authority des Waiters.

### Jeder Wait erhält Priority Inheritance

Verworfen.

PI benötigt eine eindeutige Owner-Beziehung und ist nicht für beliebige Events oder Wait Queues geeignet.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-SYNC-0002_Lock_Free_Strukturen_gezielt_einsetzen`
- `ADR-SYNC-0003_RCU_für_leselastige_Kernstrukturen`
- `ADR-SYNC-0004_Futex_artige_User_Kernel_Synchronisation`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-PI-0001`
- `NPSPEC-SYNC-PI-MUTEX-0001`
- `NPSPEC-SYNC-PI-CHAIN-0001`
- `NPSPEC-SYNC-PI-SCHEDULER-0001`
- `NPSPEC-SYNC-PI-DEADLINE-0001`
- `NPSPEC-SYNC-PI-FUTEX-0001`
- `NPSPEC-SYNC-PI-REALTIME-0001`
- `NPSPEC-SYNC-PI-TEST-0001`

---

## Ergebnis

NovaOS integriert Priority Inheritance gezielt in owner-basierte Synchronisation:

```text
High-Urgency Waiter
        ↓
Blocking Dependency
        ↓
Low-Urgency Owner
        ↓
Temporary Inheritance
        ↓
Owner completes critical section
        ↓
Lock Release
        ↓
Inheritance removed
```

Damit wird Priority Inversion begrenzt, ohne Security-, Capability- oder Ressourcenrechte zwischen Tasks zu übertragen.

Die zentrale Architekturregel lautet:

```text
Inherit urgency,
not authority.

Boost only while blocking matters.

Propagate through dependency chains.

Restore when the dependency ends.
```