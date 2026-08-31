# ADR-CONCURRENCY-0006 – Work Stealing für allgemeine Task Workloads

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Scheduling / Tasks / Work Stealing / Load Balancing / Locality

## Kontext

NovaOS verwendet Structured Concurrency, Task Groups und Async Tasks als grundlegendes Modell für allgemeine nebenläufige Arbeit.

Solche Workloads können aus sehr vielen kleinen oder mittelgroßen Tasks bestehen. Eine statische Zuordnung von Tasks zu Worker Threads kann dabei zu schlechter CPU-Auslastung führen:

```text
Worker 0: ████████████
Worker 1: ██
Worker 2:
Worker 3: █
```

Gleichzeitig darf Load Balancing nicht dazu führen, dass NovaOS wichtige Eigenschaften wie:

- NUMA-Lokalität,
- Cache-Lokalität,
- CPU-Affinity,
- Realtime,
- Determinismus,
- Ressourcenbudgets

ignoriert.

NovaOS benötigt deshalb ein effizientes Standardverfahren für allgemeine Task Workloads, ohne Work Stealing zum universellen Scheduler für jede Art von Ausführung zu machen.

---

## Entscheidung

NovaOS verwendet **Work Stealing als bevorzugte Load-Balancing-Strategie für allgemeine parallelisierbare Task Workloads**.

Worker verwalten lokale Runnable Queues.

```text
Worker 0 → Local Queue
Worker 1 → Local Queue
Worker 2 → Local Queue
Worker 3 → Local Queue
```

Besitzt ein Worker keine geeignete lokale Arbeit, darf er Tasks von anderen Workern übernehmen:

```text
Worker 2 empty
      ↓
Select victim
      ↓
Steal eligible task
      ↓
Execute locally
```

Work Stealing ist eine Scheduling Policy oberhalb der grundlegenden Task- und Execution-Scope-Semantik.

---

## Grundprinzip

```text
Execute locally when possible.

Steal when idle.

Preserve contracts while balancing work.
```

---

## Anwendungsbereich

Work Stealing ist primär für allgemeine Task Workloads vorgesehen, insbesondere:

- Task Groups,
- parallele Algorithmen,
- CPU-bound Tasks,
- allgemeine Worker Pools,
- Async Continuations,
- unabhängige Pipeline-Arbeit.

Es ist nicht automatisch die bevorzugte Strategie für:

- Hard Realtime,
- streng CPU-gepinnte Tasks,
- Interrupt Handler,
- hochspezialisierte Device Queues,
- deterministisch fest zugeordnete Arbeit.

---

## Lokale Queues

Worker sollen bevorzugt ihre eigene lokale Queue abarbeiten.

Dadurch werden:

- Synchronisationskosten,
- Cache Misses,
- globale Scheduler-Konkurrenz

reduziert.

Eine globale zentrale Task Queue ist nicht das primäre Modell für allgemeine Task Workloads.

---

## Stealing

Ein Worker versucht Work Stealing, wenn er keine ausreichend geeignete lokale Arbeit besitzt.

Nur Tasks, deren Execution Constraints eine Migration zum stehlenden Worker erlauben, dürfen übernommen werden.

---

## Stealable Tasks

Tasks können hinsichtlich ihrer Verschiebbarkeit klassifiziert werden.

Konzeptionell:

```text
Stealable
Restricted
Pinned
```

`Pinned` Work darf nicht durch normales Work Stealing auf einen anderen Worker verschoben werden.

---

## Ownership bleibt erhalten

Work Stealing verändert nur den ausführenden Worker.

Es verändert nicht:

- Execution Scope,
- Task Group,
- Cancellation Context,
- Deadline,
- Capability Set,
- Resource Account,
- Data Policy.

Es gilt:

```text
Scheduling Location
    ≠
Task Ownership
```

---

## Hierarchische Workloads

Task Groups dürfen intern über mehrere Worker verteilt werden.

```text
Task Group
   ├── Task A → CPU 1
   ├── Task B → CPU 3
   ├── Task C → CPU 2
   └── Task D → CPU 0
```

Die hierarchische Lebensdauer der Group bleibt davon unberührt.

---

## Locality Awareness

Work Stealing muss die Hardwaretopologie berücksichtigen können.

Bevorzugte Reihenfolge kann beispielsweise sein:

```text
same core/cache domain
      ↓
same NUMA node
      ↓
remote NUMA node
```

Die genaue Reihenfolge ist Policy und hängt von Hardware und Workload ab.

---

## NUMA

NUMA-Grenzen dürfen nicht wie gleichwertige Worker-Beziehungen behandelt werden.

Remote Stealing kann höhere Kosten verursachen durch:

- Remote Memory Access,
- Cache-Verlust,
- Memory Migration.

Der Scheduler muss diese Kosten berücksichtigen können.

---

## Affinity

Explizite CPU- oder CPU-Set-Affinity begrenzt Work Stealing.

Ein Task darf nur auf Worker gestohlen werden, die innerhalb seiner effektiven Affinity liegen.

---

## Resource Economy

Work Stealing muss die systemweite Resource Economy berücksichtigen.

Ein freier Worker ist nicht automatisch ein zulässiger Worker.

Beispielsweise können:

- CPU Budgets,
- Power Limits,
- Thermal Limits,
- Reservations

eine Ausführung verhindern oder weniger attraktiv machen.

---

## Scheduling Policy

Work Stealing ist Mechanismus und Policy-Baustein, nicht die alleinige Schedulingentscheidung.

Die Zielwahl kann unter anderem berücksichtigen:

```text
Task Eligibility
Affinity
Deadline
Locality
CPU Load
Resource Budget
Energy
Thermal State
```

Hard Constraints besitzen Vorrang vor Load Balancing.

---

## Stealing Granularity

NovaOS darf einzelne Tasks oder geeignete Gruppen von Tasks übertragen.

Zu feingranulares Stealing kann mehr Schedulingkosten als Nutzen erzeugen.

Die Granularität darf daher adaptiv beziehungsweise policygesteuert gewählt werden.

---

## Stealing Frequency

Worker dürfen nicht permanent alle anderen Queues durchsuchen.

Victim Selection und Stealing Frequency müssen so gestaltet werden, dass Scheduler-Overhead begrenzt bleibt.

---

## Victim Selection

Die Auswahl eines Victim Workers ist Policy.

Sie kann beispielsweise:

- Topologie,
- Queue Length,
- NUMA-Distanz,
- aktuelle Last

berücksichtigen.

Es darf keine Architekturabhängigkeit von einem einzelnen festen Victim-Selection-Algorithmus entstehen.

---

## Idle Verhalten

Kann ein Worker keine geeignete Arbeit finden, darf er nach begrenzten Stealing-Versuchen in einen effizienteren Idle State wechseln.

```text
Local Queue Empty
      ↓
Steal Attempts
      ↓
No Work
      ↓
Idle / Sleep
```

Unbegrenztes Busy-Spinning ist nicht der allgemeine Standard.

---

## Wakeup

Neue Arbeit muss schlafende Worker bei Bedarf effizient aktivieren können.

Die Wakeup Policy soll vermeiden:

- unnötige CPU-Wakeups,
- Thundering Herd,
- unnötige Energieverluste.

---

## Async/Await

Async Continuations können als allgemeine Runnable Tasks in Work-Stealing-Pools ausgeführt werden.

Eine Continuation muss nicht auf dem Worker fortgesetzt werden, auf dem die Task zuvor lief.

Explizite Affinity- oder Executor-Anforderungen bleiben bindend.

---

## Blocking Tasks

Länger blockierende Operationen dürfen einen allgemeinen Work-Stealing-Pool nicht dauerhaft seiner Worker berauben.

Native asynchrone I/O soll deshalb bevorzugt werden.

Unvermeidbar blockierende Workloads dürfen separate Scheduling- beziehungsweise Worker-Policies verwenden.

---

## Realtime

Hard-Realtime-Tasks verwenden Work Stealing nicht automatisch.

Wenn Work Stealing innerhalb eines Realtime Contracts erlaubt wird, müssen:

- zulässige Worker,
- Schedulingkosten,
- Ressourcen,
- Deadline-Garantien

vorher ausreichend bestimmt sein.

---

## Deterministic Mode

Normales dynamisches Work Stealing kann die konkrete Ausführungsreihenfolge verändern.

Für Deterministic Mode muss Work Stealing daher:

- deaktivierbar,
- eingeschränkt,
- oder deterministisch gesteuert

sein.

Deterministische Workloads dürfen nicht von zufälliger Victim Selection abhängig sein.

---

## Graceful Degradation

Unter Ressourcenknappheit kann NovaOS den Parallelitätsgrad einer Task Group reduzieren.

Work Stealing darf nicht versuchen, durch aggressivere Verteilung Hard Resource Limits zu umgehen.

---

## Adaptive Optimierung

NovaOS darf Soft Policies adaptiv optimieren.

Beispielsweise können historische Messungen beeinflussen:

- Stealing Frequency,
- Victim Selection,
- Parallelism,
- Locality Weighting.

Prediction Error kann zur Korrektur solcher Modelle verwendet werden.

Hard Constraints bleiben davon unabhängig.

---

## Introspection

Work-Stealing-Entscheidungen müssen ausreichend introspektierbar sein.

Beispiel:

```text
Task:
    image.filter.partition.17

Original Worker:
    CPU 2

Current Worker:
    CPU 6

Reason:
    Local worker idle

NUMA:
    same node
```

Für Performanceanalyse sollen zusätzlich aggregierte Statistiken verfügbar sein können.

---

## Fail-Safe Defaults

Ist nicht eindeutig bekannt, ob ein Task auf einem anderen Worker ausgeführt werden darf, wird er nicht gestohlen.

Hard Affinity-, Realtime-, Security- oder Execution-Contract-Constraints dürfen nicht zugunsten besserer CPU-Auslastung verletzt werden.

---

## Keine KI-Abhängigkeit

Work Stealing muss vollständig ohne KI funktionieren.

KI oder adaptive Modelle dürfen ausschließlich Soft Policies optimieren.

Die grundlegende Scheduler-Korrektheit darf nicht von Vorhersagemodellen abhängen.

---

## Normative Anforderungen

1. NovaOS SOLL Work Stealing als bevorzugte Load-Balancing-Strategie für allgemeine Task Workloads unterstützen.
2. Worker SOLLEN lokale Runnable Queues besitzen können.
3. Lokale ausführbare Arbeit SOLL vor Remote Work Stealing bevorzugt werden.
4. Idle Worker MÜSSEN geeignete Arbeit anderer Worker übernehmen können.
5. Work Stealing DARF nur für Tasks erfolgen, deren Constraints dies erlauben.
6. Tasks MÜSSEN als stealable, eingeschränkt verschiebbar oder pinned klassifizierbar sein.
7. Work Stealing DARF Task Ownership und Execution-Scope-Zugehörigkeit NICHT verändern.
8. Cancellation-, Deadline- und Resource-Kontexte MÜSSEN beim Worker-Wechsel erhalten bleiben.
9. CPU- und CPU-Set-Affinity MÜSSEN Work Stealing begrenzen können.
10. Work Stealing MUSS Hardwaretopologie berücksichtigen können.
11. NUMA-Lokalität MUSS als Scheduling-Kostenfaktor berücksichtigt werden können.
12. Cache- und Datenlokalität SOLLEN bei Victim- und Task-Auswahl berücksichtigt werden können.
13. Hard Constraints MÜSSEN Vorrang vor Load Balancing besitzen.
14. Work Stealing MUSS mit der systemweiten Resource Economy integrierbar sein.
15. Stealing Frequency MUSS begrenzbar sein.
16. Victim Selection MUSS als austauschbare Scheduling Policy behandelt werden.
17. Stealing Granularity MUSS optimierbar sein können.
18. Unbegrenztes Busy-Spinning DARF NICHT der allgemeine Idle-Standard sein.
19. Worker-Wakeup MUSS ohne unnötigen Thundering-Herd-Effekt möglich sein.
20. Async Continuations MÜSSEN über Work-Stealing-Worker ausführbar sein können.
21. Explizite Executor- oder Affinity-Anforderungen MÜSSEN Work Stealing überschreiben können.
22. Lang blockierende Operationen SOLLEN allgemeine Work-Stealing-Pools NICHT dauerhaft blockieren.
23. Hard-Realtime-Workloads DÜRFEN NICHT ungeprüft dynamischem Work Stealing unterliegen.
24. Work Stealing MUSS für Deterministic Mode deaktivierbar oder deterministisch einschränkbar sein.
25. Work Stealing DARF Resource Limits NICHT umgehen.
26. Adaptive Optimierung DARF nur Soft Scheduling Policies verändern.
27. Scheduler-Overhead durch Work Stealing MUSS mess- und begrenzbar sein.
28. Work-Stealing-Zustand und relevante Statistiken MÜSSEN introspektierbar sein.
29. Unklare Task-Migrationsfähigkeit MUSS fail-safe als nicht stealable behandelt werden.
30. Die grundlegende Work-Stealing-Funktionalität MUSS vollständig ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- gute CPU-Auslastung bei ungleichmäßigen Task Workloads,
- weniger zentrale Scheduler-Konkurrenz,
- natürliche Integration mit Task Groups,
- gute Skalierung paralleler Algorithmen,
- Locality und NUMA können berücksichtigt werden,
- Async Continuations können effizient verteilt werden.

### Negative Konsequenzen

- Scheduling wird komplexer als bei einer einzelnen globalen Queue,
- NUMA-Systeme benötigen topology-aware Policies,
- dynamisches Stealing erschwert deterministische Ausführung,
- falsche Stealing-Entscheidungen können Cache- und Locality-Kosten erhöhen.

---

## Verworfene Alternativen

### Eine globale Runnable Queue für alle allgemeinen Tasks

Verworfen.

Sie kann auf großen Systemen zu unnötiger Synchronisation und schlechterer Lokalität führen.

### Statische Task-Zuordnung ohne Load Balancing

Verworfen.

Ungleichmäßige Workloads können einzelne Worker überlasten, während andere untätig bleiben.

### Aggressives globales Work Stealing ohne Topologie

Verworfen.

Dies kann NUMA- und Cache-Lokalität erheblich verschlechtern.

### Work Stealing für sämtliche Systemarbeit

Verworfen.

Realtime-, Interrupt-, Device- und gepinnte Workloads besitzen teilweise andere Scheduling-Anforderungen.

### KI als notwendiger Victim Selector

Verworfen.

Scheduler-Korrektheit und grundlegendes Load Balancing müssen ohne KI funktionieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0005_Async_Await_als_native_Abstraktion`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-WORKSTEALING-0001`
- `NPSPEC-CONCURRENCY-WORKQUEUE-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-LOCALITY-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-NUMA-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-VICTIM-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-IDLE-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-DETERMINISTIC-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-TEST-0001`

---

## Ergebnis

NovaOS verwendet Work Stealing als bevorzugte Load-Balancing-Strategie für allgemeine Task Workloads:

```text
Local Work
    ↓
Execute Locally

No Local Work
    ↓
Find Eligible Remote Work
    ↓
Steal
    ↓
Execute
```

Work Stealing bleibt dabei den übergeordneten Execution Contracts untergeordnet.

Die zentrale Architekturregel lautet:

```text
Prefer local work.

Steal only when useful.

Respect topology.

Never steal across hard constraints.
```