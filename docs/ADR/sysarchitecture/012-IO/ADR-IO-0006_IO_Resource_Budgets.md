# ADR-IO-0006 – I/O Resource Budgets

## Status

Angenommen

## Kategorie

I/O / Resource Economy / QoS / Resource Management

## Kontext

I/O verbraucht neben CPU-Zeit weitere begrenzte Systemressourcen.

Dazu gehören unter anderem:

- Bandbreite,
- Queue-Kapazität,
- I/O-Operationen,
- DMA-Ressourcen,
- Buffer,
- Pinned Memory,
- Provider- und Gerätekapazität.

Ohne systemweite Budgets könnte eine einzelne Anwendung oder Execution Domain unverhältnismäßig viele I/O-Ressourcen belegen und dadurch andere Workloads beeinträchtigen.

Da NovaOS eine systemweite Resource Economy besitzt, muss auch I/O vollständig in dieses Modell integriert werden.

---

## Entscheidung

NovaOS behandelt **I/O-Ressourcen als budgetierbare Systemressourcen**.

I/O Requests werden einem Resource Context zugeordnet und gegen dessen verfügbare I/O Budgets bewertet.

```text
Execution Domain
       ↓
Resource Budget
       ↓
   I/O Request
       ↓
I/O Resource Accounting
       ↓
     Provider
```

Budgets definieren zulässigen Ressourcenverbrauch. Sie ersetzen keine QoS-, Priority- oder Realtime-Contracts.

---

## Grundprinzip

```text
I/O consumes resources.

Resource consumption must be attributable and bounded.
```

---

## Resource Context

Jeder relevante I/O Request muss einem verursachenden Resource Context zugeordnet werden können.

Dies kann beispielsweise sein:

```text
Execution Domain
Task Group
System Service
Realtime Domain
```

Dadurch bleibt I/O-Verbrauch auch dann zurechenbar, wenn die eigentliche Operation später asynchron durch Kernel, Treiber oder Hardware ausgeführt wird.

---

## Budgetierbare Ressourcen

Das Budgetmodell darf unterschiedliche I/O-Ressourcen berücksichtigen.

Beispiele:

```text
Bandwidth
Operations
Queue Capacity
Outstanding Requests
DMA Resources
Pinned Memory
Buffer Capacity
Provider Resources
```

Welche Ressourcen ein konkreter Provider budgetiert, wird durch dessen Contracts spezifiziert.

---

## Hierarchische Budgets

I/O Budgets sollen hierarchisch in die Resource Economy eingebunden werden.

```text
System
  ↓
Execution Domain
  ↓
Task Group
  ↓
I/O Requests
```

Untergeordnete Workloads dürfen die ihnen verfügbaren Ressourcen nicht unabhängig von den Grenzen ihrer übergeordneten Resource Domains erweitern.

---

## Budgets und QoS

Resource Budget und QoS bleiben getrennte Konzepte.

```text
Budget
→ How much may be consumed?

QoS
→ What service quality is required?
```

Ein hoher QoS- oder Priority-Wert darf kein unbegrenztes I/O Budget erzeugen.

Umgekehrt garantiert ein vorhandenes Budget keine bestimmte Latenz oder Deadline.

---

## Reservierungen

Zeitkritische Workloads dürfen I/O-Ressourcen reservieren können.

Dies ist insbesondere für Realtime- und Deadline-Workloads relevant.

```text
Realtime Contract
       ↓
I/O Reservation
       ↓
Guaranteed Resource Capacity
```

Reservierte Ressourcen müssen explizit verwaltet und der verursachenden Resource Domain zugerechnet werden.

---

## Budgetüberschreitung

Kann ein Request aufgrund seines Resource Budgets nicht unmittelbar ausgeführt werden, muss dies kontrolliert behandelt werden.

Abhängig vom Contract kann ein Request beispielsweise:

- warten,
- gedrosselt werden,
- abgelehnt werden,
- über eine explizite Degradation behandelt werden.

NovaOS darf Budgetgrenzen nicht stillschweigend ignorieren.

---

## Providerübergreifende Ressourcen

Ein I/O-Pfad kann mehrere Ressourcen gleichzeitig beanspruchen.

Beispiel:

```text
Shared Buffer
     +
Pinned Memory
     +
DMA Mapping
     +
Device Queue
     +
Bandwidth
```

Die gemeinsame Resource Economy muss solche Kosten über Subsystemgrenzen hinweg zurechenbar machen.

Dadurch darf beispielsweise Zero-Copy nicht als „kostenlos“ betrachtet werden, nur weil keine CPU-Kopie erfolgt.

---

## Systemkritische Ressourcen

NovaOS darf reservierte I/O-Kapazitäten für kritische Systemfunktionen besitzen.

Dazu können gehören:

- Recovery,
- Storage-Konsistenz,
- kritische Systemdienste,
- Hard-Realtime-Domänen.

Normale Workloads dürfen diese Reserven nicht verbrauchen.

---

## Architecture Introspection

I/O Resource Budgets müssen kontrolliert introspektierbar sein.

Relevant sind insbesondere:

- zugewiesenes Budget,
- aktueller Verbrauch,
- Reservierungen,
- Drosselungen,
- Budgetüberschreitungen,
- verursachende Execution Domain,
- betroffener Provider.

---

## Normative Anforderungen

1. NovaOS MUSS I/O vollständig in die systemweite Resource Economy integrieren.
2. Relevante I/O Requests MÜSSEN einem Resource Context zurechenbar sein.
3. I/O Budgets MÜSSEN unterschiedliche Ressourcenarten abbilden können.
4. Provider MÜSSEN ihre relevanten budgetierbaren Ressourcen deklarieren können.
5. I/O Budgets SOLLEN hierarchisch organisiert werden können.
6. Untergeordnete Resource Domains DÜRFEN übergeordnete Budgetgrenzen NICHT umgehen.
7. Resource Budget und QoS MÜSSEN getrennte Konzepte bleiben.
8. Hohe Priority oder QoS DARF NICHT automatisch zusätzliche unbegrenzte Ressourcen gewähren.
9. Realtime- und Deadline-Workloads MÜSSEN I/O-Ressourcen reservieren können.
10. Reservierte Ressourcen MÜSSEN der verursachenden Resource Domain zugerechnet werden.
11. Budgetüberschreitungen MÜSSEN kontrolliert behandelt werden.
12. Budgetgrenzen DÜRFEN NICHT stillschweigend ignoriert werden.
13. Shared Buffers, DMA und Pinned Memory MÜSSEN in die I/O-Ressourcenabrechnung integrierbar sein.
14. Systemkritische I/O-Reserven MÜSSEN vor normalen Workloads geschützt werden.
15. Budget, Verbrauch und Reservierungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- kontrollierbarer I/O-Ressourcenverbrauch,
- Schutz vor I/O-Monopolisierung,
- bessere Isolation zwischen Execution Domains,
- Grundlage für zuverlässige QoS- und Realtime-Garantien,
- vollständige Kostenbetrachtung von DMA und Zero-Copy,
- systemweite Ressourcenplanung.

### Negative Konsequenzen

- zusätzliches Accounting im I/O-Pfad,
- Provider müssen Ressourcenverbrauch messbar machen,
- Budgetierung und Reservierungen erhöhen die Ressourcenverwaltungskomplexität.

---

## Verworfene Alternativen

### I/O ohne Ressourcenbudgets

Verworfen.

Einzelne Workloads könnten I/O-Ressourcen unkontrolliert dominieren.

### Nur Bandbreite budgetieren

Verworfen.

I/O verbraucht zusätzlich Queue-, Buffer-, DMA-, Pinned-Memory- und Provider-Ressourcen.

### Priority als Ressourcenbudget verwenden

Verworfen.

Priorität beschreibt relative Wichtigkeit und nicht zulässigen Ressourcenverbrauch.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IO-0001_Completion_Based_Asynchronous_IO`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`
- `ADR-IO-0003_QoS_und_Priority_Aware_IO`
- `ADR-IO-0004_Zero_Copy_IO_Pipelines`
- `ADR-IO-0005_Deadline_Aware_IO`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`

---

## Zugehörige NPSPECs

- `NPSPEC-IO-RESOURCE-BUDGET-0001`
- `NPSPEC-IO-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-IO-RESOURCE-RESERVATION-0001`
- `NPSPEC-IO-RESOURCE-THROTTLING-0001`
- `NPSPEC-IO-RESOURCE-PROVIDER-0001`
- `NPSPEC-IO-RESOURCE-TEST-0001`

---

## Ergebnis

I/O wird in NovaOS nicht als unbegrenzte Nebenressource behandelt, sondern als Bestandteil der systemweiten Ressourcenökonomie:

```text
Execution Contract
        ↓
Resource Budget
        ↓
    I/O Request
        ↓
Resource Accounting
        ↓
      Provider
```

Die zentrale Architekturregel lautet:

```text
Every I/O resource has a cost.

Every cost has an owner.
```