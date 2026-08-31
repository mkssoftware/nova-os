# ADR-SCHED-0002 – Fair, Deadline und Realtime Scheduling in einer Architektur

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Fair Scheduling / Deadline Scheduling / Realtime / Execution Contracts

## Kontext

`ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler` definiert einen gemeinsamen Scheduler-Core mit mehreren Scheduling Classes.

NovaOS muss dabei drei unterschiedliche Scheduling-Ziele innerhalb derselben Architektur unterstützen:

- **Fair Scheduling** für allgemeine Workloads,
- **Deadline Scheduling** für zeitlich begrenzte Arbeit,
- **Realtime Scheduling** für Workloads mit expliziten zeitlichen Garantien.

Diese Modelle dürfen nicht als drei voneinander unabhängige Scheduler implementiert werden. Gleichzeitig wäre es falsch, sie auf eine einzige globale Prioritätszahl zu reduzieren.

NovaOS benötigt daher ein gemeinsames Scheduling-Modell, das unterschiedliche zeitliche und faire Anforderungen ausdrücken kann, ohne deren Semantik zu vermischen.

---

## Entscheidung

NovaOS integriert **Fair-, Deadline- und Realtime-Scheduling in einem gemeinsamen Scheduler-Framework**.

```text
Runnable Task
      ↓
Execution Contract
      ↓
Scheduling Class
      ↓
Class-specific Policy
      ↓
Global Constraint Resolution
      ↓
CPU Placement / Dispatch
```

Der gemeinsame Scheduler-Core verwaltet:

- Runnable States,
- Präemption,
- CPU Placement,
- Affinity,
- Topologie,
- Accounting,
- Resource Budgets.

Die konkrete Auswahl innerhalb einer Scheduling Class erfolgt durch spezialisierte Policies.

---

## Grundprinzip

```text
Fairness, deadlines and realtime
are different scheduling semantics.

They share infrastructure,
not guarantees.
```

---

## Fair Scheduling

Fair Scheduling ist das Standardmodell für allgemeine Workloads.

Es verteilt verfügbare CPU-Ressourcen entsprechend:

- Resource Budgets,
- Scheduling Weights,
- Prioritäten,
- Runnable-Zeit,
- Systempolicy.

Fairness bedeutet dabei nicht zwingend gleiche CPU-Zeit.

```text
Equal weight
    → approximately equal share

Different weight
    → weighted fair share
```

Hierarchische Resource Budgets müssen berücksichtigt werden können.

---

## Deadline Scheduling

Deadline Scheduling behandelt Arbeit mit einer expliziten zeitlichen Grenze.

```text
Task
Deadline = T
```

Die Deadline kann die Scheduling-Entscheidung beeinflussen.

Eine Deadline allein erzeugt jedoch keine garantierte Realtime-Ausführung.

```text
Deadline
    ≠
Realtime Guarantee
```

Normale oder interaktive Tasks dürfen daher Deadlines besitzen, ohne Teil einer Realtime Concurrency Domain zu sein.

---

## Realtime Scheduling

Realtime Scheduling wird nur für explizit zugelassene Realtime Workloads verwendet.

```text
Realtime Contract
      ↓
Admission Control
      ↓
Resource Reservation
      ↓
Realtime Scheduling
```

Hard-Realtime-Garantien setzen kontrollierbare Ressourcen und geeignete Scheduling Policies voraus.

---

## Gemeinsamer Scheduler-Core

Fair-, Deadline- und Realtime-Scheduling verwenden dieselben grundlegenden Kernelmechanismen.

```text
              Scheduler Core
                    │
       ┌────────────┼────────────┐
       ↓            ↓            ↓
     Fair        Deadline      Realtime
    Policy        Policy        Policy
```

Dadurch werden insbesondere:

- CPU-Zustand,
- Accounting,
- Affinity,
- Topologie,
- Migration,
- Präemption

nicht mehrfach implementiert.

---

## Keine globale Prioritätszahl

NovaOS bildet nicht alle Scheduling-Eigenschaften auf eine einzige globale numerische Priorität ab.

Beispielsweise sind:

```text
priority = 100
deadline = 20 ms
hard realtime
```

semantisch unterschiedliche Eigenschaften.

Der Scheduler muss diese Informationen getrennt behandeln.

---

## Constraint-Reihenfolge

Scheduling-Entscheidungen folgen den systemweiten Architekturprioritäten.

Vereinfacht:

```text
Safety
   ↓
Security / Isolation
   ↓
Hard Realtime Constraints
   ↓
Deterministic Constraints
   ↓
Required Affinity / Resources
   ↓
Deadline Policy
   ↓
Fairness / Class Policy
   ↓
Locality / Performance / Energy
```

Soft Optimierungen dürfen Hard Constraints niemals verletzen.

---

## Fairness unter Realtime-Last

Realtime Work darf reservierte Ressourcen gemäß seinem Contract verwenden.

Die verbleibende CPU-Kapazität wird weiterhin durch die anderen Scheduling Classes verteilt.

```text
CPU Capacity
    ├── Reserved Realtime Capacity
    └── General Capacity
            ↓
        Fair Scheduling
```

Realtime-Arbeit darf allgemeine Workloads nur soweit verdrängen, wie dies durch zugelassene Contracts und Systempolicy vorgesehen ist.

---

## Realtime Admission Control

NovaOS darf keine Hard-Realtime-Workloads zulassen, deren Ressourcenanforderungen nicht ausreichend erfüllbar sind.

Dadurch wird verhindert, dass beliebig viele Realtime Tasks die allgemeine Scheduling-Architektur dominieren.

---

## Deadline ohne Reservation

Eine normale Task darf eine Deadline besitzen, ohne Ressourcen reserviert zu haben.

In diesem Fall behandelt der Scheduler die Deadline als Scheduling Constraint beziehungsweise Optimierungsziel.

Eine Garantie entsteht daraus nicht.

---

## Deadline mit Reservation

Eine Deadline kann Bestandteil eines stärkeren Execution Contracts sein.

```text
Deadline
    +
Resource Reservation
    +
Admission Control
    ↓
Realtime Guarantee
```

Die konkrete Garantie hängt von der Realtime-Klasse und Plattformfähigkeit ab.

---

## Deadline Propagation

Der Scheduler verwendet die durch Structured Concurrency bestimmte effektive Deadline.

```text
Parent Deadline
      ↓
Child Deadline
      ↓
Effective Deadline
      ↓
Scheduler
```

Der Scheduler erzeugt keine eigene davon unabhängige Deadline-Hierarchie.

---

## Hierarchische Fairness

Fair Scheduling muss mit den hierarchischen Resource Accounts von NovaOS zusammenarbeiten.

Beispiel:

```text
Application A
   ├── Task 1
   └── Task 2

Application B
   ├── Task 3
   └── Task 4
```

Viele Tasks innerhalb einer Anwendung dürfen nicht automatisch dazu führen, dass diese Anwendung einen unverhältnismäßig größeren CPU-Anteil erhält.

Fairness muss daher auf geeigneten Resource- und Execution-Domain-Grenzen durchsetzbar sein.

---

## Gewichte

Fair-Scheduling-Gruppen können unterschiedliche Gewichte erhalten.

Gewichte beschreiben relative CPU-Anteile unter Konkurrenz.

Sie stellen keine Hard Reservation dar.

```text
Weight
    ≠
Reservation
```

---

## Präemption

Eine laufende Task darf präemptiert werden, wenn:

- eine zugelassene Realtime Task ausführbar wird,
- eine dringlichere Deadline Policy dies verlangt,
- ihr Budget endet,
- Fairness einen Wechsel verlangt.

Die genaue Entscheidung bleibt Policy.

---

## Priority Inversion

Scheduling-Class-Grenzen dürfen Priority Inversion nicht ignorieren.

Hält eine niedrigere Klasse eine Ressource, die eine Realtime- oder höherwertige Task benötigt, müssen geeignete Synchronisationsmechanismen eingreifen können.

Scheduling Class und Lock-Priorität dürfen deshalb nicht vollständig unabhängig voneinander implementiert werden.

---

## CPU Affinity und Topologie

Alle drei Scheduling-Modelle verwenden dieselbe Affinity- und Topologie-Infrastruktur.

Hard Affinity besitzt Vorrang vor:

- Fairness,
- Deadline-Optimierung,
- Work Stealing,
- Locality-Optimierung.

NUMA- und Cache-Lokalität bleiben wichtige Soft-Ziele.

---

## Work Stealing

Work Stealing wird primär für allgemeine Fair-Scheduling-Workloads verwendet.

Deadline Tasks dürfen daran teilnehmen, wenn ihre Constraints dies zulassen.

Hard-Realtime-Workloads verwenden Work Stealing nur unter explizit kontrollierten Bedingungen.

---

## Resource Economy

Fairness, Deadline Scheduling und Realtime Reservations greifen auf die gemeinsame Resource Economy zurück.

Der Scheduler darf CPU-Zeit nicht unabhängig von den Resource Accounts vergeben.

Damit können beispielsweise:

```text
Execution Domain
    ↓
Resource Budget
    ↓
Scheduling Group
    ↓
Tasks
```

hierarchisch kontrolliert werden.

---

## Overload

Kann das System nicht alle Soft-Anforderungen gleichzeitig erfüllen, muss es kontrolliert priorisieren und degradieren.

Beispiel:

```text
Overload
   ↓
Suppress Idle Work
   ↓
Throttle Background Work
   ↓
Reduce Optional Work
   ↓
Protect Required Work
   ↓
Protect admitted Realtime Contracts
```

Hard Safety-, Security- und Sovereignty-Anforderungen bleiben übergeordnet.

---

## Deadline Miss

Eine verfehlte Deadline muss entsprechend ihrer Semantik behandelt werden.

### Soft Deadline

Weiterarbeiten, degradieren oder abbrechen gemäß Policy.

### Firm Deadline

Das verspätete Ergebnis darf verworfen beziehungsweise die Arbeit gecancelt werden.

### Hard Deadline

Die Verfehlung ist eine explizite Contract-Verletzung und muss sichtbar behandelt werden.

---

## Deterministic Mode

Deterministic Mode kann:

- Fairness-Reihenfolgen,
- Deadline-Tie-Breaking,
- CPU Placement,
- Präemptionsentscheidungen

einschränken.

Adaptive Scheduler-Heuristiken müssen für entsprechend deterministische Workloads deaktivierbar sein.

---

## Heterogene CPUs

Fair-, Deadline- und Realtime-Scheduling dürfen nicht voraussetzen, dass alle CPUs dieselbe Leistung besitzen.

Der Scheduler kann Leistungsfähigkeit und Energieeffizienz unterschiedlicher CPU-Klassen berücksichtigen.

Eine Hard-Realtime-Garantie darf jedoch nur auf ausreichend vorhersehbaren Ressourcen basieren.

---

## Introspection

Für jede runnable beziehungsweise laufende Task sollen abhängig von Berechtigungen mindestens folgende Eigenschaften introspektierbar sein:

```text
Scheduling Class
Priority / Weight
Deadline
Realtime Class
CPU Budget
CPU Assignment
Affinity
Runtime
```

Zusätzlich müssen Deadline Misses und Realtime Contract Violations sichtbar sein.

---

## Adaptive Optimierung

Adaptive Systeme dürfen Soft Scheduling Policies optimieren.

Dazu gehören beispielsweise:

- Fairness-Tuning,
- Interactive Boosting,
- Deadline-Prognosen,
- CPU Placement,
- Work-Stealing-Parameter.

Prediction Error kann zur Korrektur dieser Modelle verwendet werden.

Hard-Realtime-Garantien und Hard Constraints dürfen davon nicht abhängen.

---

## Fail-Safe Defaults

Kann der Scheduler eine Hard-Realtime-Garantie nicht mehr gewährleisten, darf er sie nicht still als erfüllt behandeln.

Kann eine Deadline nur unter Verletzung von Safety-, Security-, Sovereignty- oder Resource-Constraints erreicht werden, muss die Deadline verletzt beziehungsweise die Operation abgelehnt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Fair-, Deadline- und Realtime-Scheduling innerhalb eines gemeinsamen Scheduler-Frameworks unterstützen.
2. Diese Scheduling-Modelle MÜSSEN einen gemeinsamen Scheduler-Core verwenden können.
3. Fairness, Deadline und Realtime MÜSSEN semantisch getrennte Eigenschaften bleiben.
4. NovaOS DARF diese Eigenschaften NICHT auf eine einzige globale Prioritätszahl reduzieren.
5. Fair Scheduling MUSS der Standard für allgemeine Workloads sein können.
6. Fair Scheduling MUSS gewichtete CPU-Anteile unterstützen können.
7. Fair Scheduling MUSS hierarchische Resource Accounts berücksichtigen können.
8. Die Anzahl der Tasks eines Execution Domains DARF NICHT automatisch dessen fairen CPU-Anteil erhöhen.
9. Normale Tasks MÜSSEN Deadlines besitzen können, ohne Realtime Tasks zu werden.
10. Eine Deadline allein DARF NICHT als Realtime-Garantie interpretiert werden.
11. Realtime-Garantien MÜSSEN explizite Realtime Execution Contracts voraussetzen.
12. Hard-Realtime-Workloads MÜSSEN Admission Control unterliegen.
13. Realtime Reservations MÜSSEN gegenüber allgemeiner Fairness berücksichtigt werden.
14. Nicht reservierte CPU-Kapazität SOLL weiterhin fair zwischen zulässigen allgemeinen Workloads verteilt werden.
15. Der Scheduler MUSS die systemweit propagierte effektive Deadline verwenden.
16. Scheduling DARF keine unabhängige konkurrierende Deadline-Hierarchie erzeugen.
17. Gewichte und Resource Reservations MÜSSEN getrennte Konzepte bleiben.
18. Präemption MUSS zwischen Scheduling Classes möglich sein.
19. Priority-Inversion-Mechanismen MÜSSEN Scheduling-Class-Grenzen berücksichtigen können.
20. Required Affinity MUSS Vorrang vor Soft Scheduling-Optimierungen besitzen.
21. Topologie und NUMA-Lokalität SOLLEN in allen Scheduling-Modellen berücksichtigt werden können.
22. Work Stealing MUSS auf dafür geeignete Scheduling Classes und Tasks beschränkbar sein.
23. CPU Scheduling MUSS mit der systemweiten Resource Economy integriert sein.
24. Overload MUSS kontrollierte Degradation weniger wichtiger Arbeit ermöglichen.
25. Hard-Realtime-Contracts DÜRFEN bei Overload NICHT still degradiert werden.
26. Deadline Misses MÜSSEN entsprechend ihrer Soft-, Firm- oder Hard-Semantik behandelbar sein.
27. Deterministic Mode MUSS dynamische Scheduling-Entscheidungen einschränken können.
28. Scheduling Class, Deadline, Realtime State und Resource Usage MÜSSEN introspektierbar sein.
29. Adaptive Optimierungen DÜRFEN nur Soft Policies verändern.
30. Hard Scheduling Constraints und Scheduler-Korrektheit MÜSSEN vollständig ohne KI gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliche Scheduling-Architektur,
- Fairness für allgemeine Workloads,
- Deadline-Unterstützung ohne künstliche Realtime-Klassifizierung,
- echte Realtime-Garantien über Admission Control und Reservations,
- gemeinsame CPU-, Topologie- und Resource-Infrastruktur,
- keine Duplizierung mehrerer unabhängiger Scheduler.

### Negative Konsequenzen

- Class-übergreifende Scheduling-Entscheidungen werden komplexer,
- Resource Accounting und Admission Control werden zentral für Scheduler-Korrektheit,
- Priority Inversion muss über Scheduling-Class-Grenzen berücksichtigt werden,
- Realtime-Reservations reduzieren bewusst die für Fair Scheduling verfügbare Flexibilität.

---

## Verworfene Alternativen

### Drei vollständig getrennte Scheduler

Verworfen.

CPU Placement, Accounting, Affinity, Topologie und Präemption würden unnötig dupliziert und könnten widersprüchliche Entscheidungen treffen.

### Eine globale Priorität für alle Workloads

Verworfen.

Fairness, Deadlines und Realtime-Garantien besitzen unterschiedliche Semantik.

### Jede Deadline automatisch als Realtime behandeln

Verworfen.

Viele normale Operationen besitzen sinnvolle Deadlines, ohne garantierte Ressourcen zu benötigen.

### Realtime ohne Admission Control

Verworfen.

Nicht kontrollierte Realtime-Anforderungen könnten Garantien gegenseitig unmöglich machen.

### Realtime darf immer alle anderen Workloads verdrängen

Verworfen.

Realtime Authority entsteht nur aus zugelassenen Contracts und reservierten Ressourcen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-FAIR-0001`
- `NPSPEC-SCHED-DEADLINE-0001`
- `NPSPEC-SCHED-REALTIME-0001`
- `NPSPEC-SCHED-WEIGHT-0001`
- `NPSPEC-SCHED-HIERARCHY-0001`
- `NPSPEC-SCHED-ADMISSION-0001`
- `NPSPEC-SCHED-OVERLOAD-0001`
- `NPSPEC-SCHED-DEADLINE-MISS-0001`
- `NPSPEC-SCHED-TEST-0002`

---

## Ergebnis

NovaOS integriert Fair-, Deadline- und Realtime-Scheduling in einer gemeinsamen Scheduler-Architektur:

```text
                    Scheduler Core
                          │
             ┌────────────┼────────────┐
             ↓            ↓            ↓
          Fair        Deadline      Realtime
             │            │            │
             └────────────┼────────────┘
                          ↓
              Constraint Resolution
                          ↓
                    CPU Dispatch
```

Dabei bleiben die Garantien klar getrennt:

```text
Fair
    → controlled CPU sharing

Deadline
    → temporal constraint

Realtime
    → admitted temporal guarantee
```

Die zentrale Architekturregel lautet:

```text
Share mechanisms.

Separate semantics.

Reserve before guaranteeing.

Never confuse urgency with authority.
```