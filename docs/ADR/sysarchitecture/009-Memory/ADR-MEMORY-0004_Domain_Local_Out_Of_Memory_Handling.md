# ADR-MEMORY-0004 – Domain-Local Out-of-Memory Handling

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Resource Economy / OOM / Execution Domains

## Kontext

`ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal` definiert Memory Pressure als hierarchisches systemweites Ressourcensignal.

Speichermangel muss jedoch nicht das gesamte System betreffen.

Eine einzelne Execution Domain kann ihr Memory Budget oder ihre zulässigen Speicherressourcen erschöpfen, während im restlichen System weiterhin ausreichend Speicher vorhanden ist.

Beispiel:

```text
System Memory
    64 GiB total
    20 GiB available

Execution Domain A
    Budget: 4 GiB
    Used:   4 GiB
    ↓
Domain-local OOM
```

Ein solches Ereignis darf nicht automatisch als globaler Out-of-Memory-Zustand behandelt werden.

Andernfalls könnte eine einzelne fehlerhafte oder speicherintensive Anwendung andere Execution Domains oder sogar systemkritische Dienste beeinträchtigen.

NovaOS benötigt deshalb ein hierarchisches OOM-Modell, das Speichermangel möglichst dort behandelt, wo er entsteht.

---

## Entscheidung

NovaOS verwendet **Domain-Local Out-of-Memory Handling** als primäre OOM-Strategie.

OOM wird zunächst innerhalb der betroffenen Ressourcen- und Ownership-Domain behandelt:

```text
Allocation Failure
       ↓
Identify Resource Domain
       ↓
Domain-local Recovery
       ↓
Still unsatisfied?
       ↓
Controlled Escalation
       ↓
Parent / System Policy
```

Ein lokaler OOM-Zustand darf nicht automatisch Ressourcen anderer Domains beanspruchen oder beliebige fremde Prozesse terminieren.

Globale OOM-Behandlung ist eine Eskalationsstufe für tatsächlich systemweiten oder nicht lokal lösbaren Speichermangel.

---

## Grundprinzip

```text
Contain memory failure locally.

Recover inside the responsible domain first.

Escalate only when necessary.

Do not punish unrelated workloads.
```

---

## OOM als hierarchischer Zustand

NovaOS betrachtet OOM entsprechend der Ressourcenhierarchie.

Beispiel:

```text
System
 ├── Domain A
 │    ├── Scope A1
 │    └── Scope A2
 │
 └── Domain B
```

Speichermangel kann auftreten auf Ebene von:

```text
Allocation
Execution Scope
Execution Domain
Memory Domain
System
```

Die betroffene Ebene muss vor einer Recovery-Entscheidung bestimmt werden.

---

## Ursachen eines Domain-Local OOM

Ein Domain-Local OOM kann unter anderem entstehen durch:

- erreichtes Memory Budget,
- erreichtes Hard Limit,
- ausgeschöpfte Reservation,
- nicht erfüllbare Memory-Class-Anforderung,
- nicht erfüllbares Required-NUMA-Placement,
- fehlenden contiguous Memory,
- DMA-/IOMMU-Constraints,
- zu viel pinned Memory,
- fehlgeschlagene lokale Reclamation.

Daher gilt:

```text
OOM != necessarily global RAM exhaustion
```

---

## Allocation Failure vs. OOM

Nicht jedes Allocation Failure ist automatisch ein OOM-Ereignis.

Eine einzelne Allokation kann beispielsweise aufgrund eines speziellen Hard Constraints scheitern.

Erst die zuständige Memory- und Resource-Policy entscheidet, ob daraus ein OOM-Zustand für die betreffende Domain entsteht.

Die Diagnose muss die Ursache unterscheiden können.

---

## Domain-Local Recovery

Vor einer Eskalation sollen innerhalb der betroffenen Domain geeignete Recovery-Maßnahmen versucht werden.

Abhängig vom Contract können dazu gehören:

```text
Reclaim Domain-local Caches
Release Optional Memory
Cancel Speculative Work
Reduce Parallelism
Discard Reconstructable State
Rollback Transaction
Cancel Execution Scope
Restart Recoverable Component
```

Die Reihenfolge und Zulässigkeit werden durch Resource-, Execution- und Supervision-Policies bestimmt.

---

## Cooperative Release

Execution Domains sollen auf drohenden oder eingetretenen lokalen Speichermangel reagieren können.

Sie können beispielsweise:

- interne Caches verkleinern,
- Preloads verwerfen,
- optionale Buffers freigeben,
- ungenutzte Modelle entladen,
- temporäre Arbeitsdaten reduzieren.

Kooperative Freigabe wird bevorzugt, wenn dadurch der OOM ohne Verlust wesentlicher Arbeit behoben werden kann.

---

## Scope-Local Recovery

Wenn der Speicherverbrauch eindeutig einem untergeordneten Execution Scope zugeordnet werden kann, darf die Recovery auf diesen Scope begrenzt werden.

Beispiel:

```text
Application Domain
      ↓
Image Processing Scope
      ↓
Exceeds memory budget
      ↓
Cancel only this scope
```

Damit muss nicht zwangsläufig der gesamte Prozess beziehungsweise die gesamte Execution Domain beendet werden.

Dies nutzt die Ownership-Struktur der Structured Concurrency.

---

## Domain Termination

Kann ein Domain-Local OOM nicht innerhalb der Domain behoben werden, darf die Supervision Policy die betroffene Execution Domain kontrolliert terminieren oder neu starten.

Dies ist insbesondere gegenüber einem globalen OOM-Killer vorzuziehen.

Die Terminierung muss:

- Ressourcen freigeben,
- abhängige Tasks kontrolliert abbrechen,
- Shared Resources bereinigen,
- Supervisoren informieren,
- einen diagnostizierbaren Exit Reason erzeugen.

---

## Keine globale zufällige Opferauswahl

NovaOS verwendet keinen klassischen globalen OOM-Killer als primäres Modell, der anhand einer Heuristik irgendeinen Prozess auswählt.

Nicht vorgesehen ist:

```text
Memory problem
      ↓
Score all processes
      ↓
Kill arbitrary "best victim"
```

Stattdessen basiert die Entscheidung auf:

```text
Ownership
Budgets
Contracts
Supervision
Criticality
Recovery Policy
```

Dadurch bleibt nachvollziehbar, warum eine bestimmte Workload degradiert oder beendet wurde.

---

## Memory Budgets

Execution Domains besitzen explizite Memory Budgets und Limits.

Dabei können unterschieden werden:

```text
Current Usage
Soft Budget
Hard Limit
Reservation
```

Das Überschreiten eines Soft Budgets kann Memory Pressure oder Degradation auslösen.

Das Überschreiten eines Hard Limits darf nicht durch stilles Beanspruchen fremder Ressourcen umgangen werden.

---

## Reservations

Eine Domain darf reservierten Speicher anderer Domains nicht aufgrund ihres eigenen OOM-Zustands beanspruchen.

Insbesondere bleiben geschützt:

- Kernel Emergency Reserves,
- Realtime Reservations,
- Recovery Resources,
- sicherheitskritische Systemdienste.

Dies verhindert, dass lokaler Speichermangel zu systemweiter Instabilität eskaliert.

---

## Shared Memory

Shared und Zero-Copy Memory kann mehreren Domains zugeordnet oder von mehreren Domains referenziert werden.

Die Resource Economy muss deshalb zwischen:

```text
Ownership
Usage
Reference
Reservation
```

unterscheiden.

Eine Domain darf nicht allein deshalb vollständig für einen Buffer belastet werden, weil sie eine Referenz darauf besitzt.

Die konkrete Accounting-Semantik wird separat spezifiziert.

---

## NUMA

Domain-Local OOM und NUMA-local OOM sind unterschiedliche Konzepte.

Beispiel:

```text
Domain A:
    within budget

Required NUMA Node 0:
    no valid memory available
```

Hier kann eine Allokation aufgrund eines Placement Constraints scheitern, obwohl die Domain ihr Gesamtbudget noch nicht ausgeschöpft hat.

Ist Remote Memory laut Contract zulässig, kann kontrolliert ausgewichen werden.

Bei Required Placement muss die Allokation fehlschlagen oder eine explizite Degradationspolicy greifen.

---

## Realtime

Realtime Memory Reservations dürfen durch OOM anderer Domains nicht beeinträchtigt werden.

Eine Realtime Domain, die ihre eigene garantierte Reservation überschreitet, erhält dadurch keinen automatischen Anspruch auf weitere garantierte Ressourcen.

Zusätzliche Ressourcen müssen gemäß Admission-, Resource- und Realtime-Policy behandelt werden.

---

## Kernel OOM

Kernelkritische Allokationen benötigen besondere Behandlung.

NovaOS muss begrenzte Emergency Resources für Operationen vorsehen können, die zur:

- Recovery,
- Reclamation,
- Domain-Termination,
- Fehlerdiagnose

selbst Speicher benötigen.

Diese Reserven dürfen nicht als allgemeiner zusätzlicher Speicherpool verwendet werden.

---

## Globaler OOM

Ein globaler OOM liegt vor, wenn der Speicherbedarf nicht mehr durch lokale Recovery, Reclamation oder zulässige Ressourcenverschiebung behoben werden kann und systemweite Funktionsfähigkeit gefährdet ist.

Dann erfolgt kontrollierte Eskalation:

```text
Local Recovery
      ↓
Domain Recovery
      ↓
System Reclamation
      ↓
Graceful Degradation
      ↓
Supervision-based Termination
      ↓
Emergency System Policy
```

Auch in dieser Situation sollen kritische System- und Recovery-Funktionen möglichst erhalten bleiben.

---

## Transaktionale Operationen

Entsteht OOM während einer transaktionalen Systemoperation, kann der zugehörige Transaction Contract einen kontrollierten Abort und Rollback vorsehen.

```text
Transaction
    ↓
Allocation Failure
    ↓
OOM Policy
    ↓
Abort + Rollback
```

Dadurch kann Speicher freigegeben werden, ohne einen teilweise veränderten Systemzustand zu hinterlassen.

---

## Graceful Degradation

OOM Recovery folgt dem Prinzip der Graceful Degradation.

Optionale Funktionen sollen vor kritischen Funktionen reduziert werden.

Beispiel:

```text
Preloading
    ↓
Optional Cache
    ↓
Background Work
    ↓
Optional Execution Scope
    ↓
Recoverable Service
    ↓
Critical Components
```

Die tatsächliche Reihenfolge wird durch Contracts und Systemkritikalität bestimmt.

---

## Fail-Safe Verhalten

OOM Recovery darf keine Hard Constraints verletzen, um eine Allokation künstlich erfolgreich erscheinen zu lassen.

Insbesondere dürfen nicht still verletzt werden:

- Security Isolation,
- Data Sovereignty,
- Required NUMA Placement,
- Realtime Reservations,
- Memory Class Requirements,
- Hard Resource Limits.

Wenn keine zulässige Recovery existiert, muss die betroffene Operation oder Domain kontrolliert fehlschlagen.

---

## Deterministic Mode

Deterministic Mode muss reproduzierbare OOM-Policies unterstützen.

Insbesondere dürfen adaptive Victim- oder Recovery-Entscheidungen eingeschränkt werden.

Für gleiche Ressourcenlimits und relevante Systemzustände soll eine deterministische Recovery-Policy dieselbe logische Entscheidung erzeugen können.

---

## Architecture Introspection

OOM-Ereignisse müssen nachvollziehbar sein.

Mindestens relevant sind:

```text
OOM Event ID
Affected Domain
Affected Scope
Allocation Request
Memory Usage
Budget
Hard Limit
Reservation
Memory Domain
Allocation Constraints
Pressure State
Recovery Actions
Final Outcome
```

Damit kann festgestellt werden, ob ein Fehler durch:

- tatsächliche Speichererschöpfung,
- Budgetüberschreitung,
- NUMA Constraints,
- Fragmentierung,
- spezielle Memory Requirements

verursacht wurde.

---

## Normative Anforderungen

1. NovaOS MUSS Domain-Local OOM Handling als primäre OOM-Strategie unterstützen.
2. OOM-Zustände MÜSSEN hierarchisch einer Ressourcen- beziehungsweise Ownership-Domain zuordenbar sein.
3. Ein Domain-Local OOM DARF NICHT automatisch als globaler OOM behandelt werden.
4. Allocation Failure und OOM MÜSSEN als unterschiedliche Konzepte behandelt werden.
5. Execution Domains MÜSSEN Memory Budgets und Hard Limits besitzen können.
6. Hard Memory Limits DÜRFEN NICHT still überschritten werden.
7. Domain-lokale Recovery MUSS vor globaler Eskalation möglich sein.
8. Cooperative Memory Release SOLL vor destruktiver Recovery verwendet werden können.
9. Execution Scopes MÜSSEN als mögliche lokale Recovery-Grenzen verwendet werden können.
10. Structured Concurrency MUSS kontrollierte Scope-Cancellation bei OOM ermöglichen.
11. Domain-Termination MUSS über die Supervision-Architektur erfolgen können.
12. NovaOS DARF keinen heuristischen globalen OOM-Killer als primäres Recovery-Modell voraussetzen.
13. OOM-Entscheidungen SOLLEN Ownership, Budgets, Contracts und Criticality berücksichtigen.
14. Reservierter Speicher anderer Domains DARF NICHT aufgrund eines lokalen OOM still beansprucht werden.
15. Kernel Emergency Reserves MÜSSEN vor allgemeiner Nutzung geschützt werden.
16. Realtime Reservations MÜSSEN vor OOM anderer Domains geschützt werden.
17. Shared- und Zero-Copy-Memory MUSS mit expliziter Ownership- und Accounting-Semantik behandelt werden.
18. NUMA Placement Failure MUSS von allgemeiner Budgeterschöpfung unterscheidbar sein.
19. Required Memory Placement DARF zur OOM-Vermeidung NICHT still verletzt werden.
20. Globaler OOM DARF erst nach geeigneter lokaler beziehungsweise systemweiter Recovery eskaliert werden.
21. Transaktionale Operationen MÜSSEN OOM als möglichen Abort-Grund behandeln können.
22. Graceful Degradation MUSS in die OOM-Recovery integrierbar sein.
23. Security-, Sovereignty- und Realtime-Hard-Constraints DÜRFEN durch OOM-Recovery NICHT still verletzt werden.
24. Deterministic Mode MUSS reproduzierbare OOM-Recovery-Policies unterstützen können.
25. OOM-Ereignisse, Ursachen und Recovery-Maßnahmen MÜSSEN introspektierbar sein.
26. Kann keine zulässige Recovery durchgeführt werden, MUSS die betroffene Operation oder Domain kontrolliert fehlschlagen.

---

## Konsequenzen

### Positive Konsequenzen

- Speicherfehler werden möglichst auf den verursachenden Bereich begrenzt,
- unabhängige Anwendungen werden nicht unnötig beeinträchtigt,
- Memory Budgets erhalten tatsächlich durchsetzbare Bedeutung,
- Structured Concurrency ermöglicht feinere Recovery als reine Prozessbeendigung,
- Realtime- und Systemreserven bleiben geschützt,
- OOM-Entscheidungen werden nachvollziehbarer als bei globaler heuristischer Opferauswahl.

### Negative Konsequenzen

- Memory Accounting muss Ownership und Ressourcenhierarchien zuverlässig erfassen,
- Shared Memory erschwert eindeutige Kostenverteilung,
- OOM-Recovery benötigt Integration mit Supervision und Structured Concurrency,
- globale Extremsituationen benötigen weiterhin eine systemweite Emergency Policy.

---

## Verworfene Alternativen

### Globaler OOM-Killer als Standardlösung

Verworfen.

Ein lokales Ressourcenproblem soll nicht zur willkürlichen Terminierung unabhängiger Workloads führen.

### Memory Limits nur als Monitoring-Werte

Verworfen.

Hard Limits müssen tatsächlich durchsetzbar sein.

### Sofortige Terminierung der gesamten Execution Domain

Verworfen.

Scope-lokale oder kooperative Recovery kann häufig ausreichend sein.

### Fremde Reservations bei OOM automatisch freigeben

Verworfen.

Dadurch könnten Realtime-, Recovery- oder andere garantierte Ressourcen verletzt werden.

### Allocation Constraints bei OOM ignorieren

Verworfen.

Eine erfolgreiche, aber semantisch unzulässige Allokation ist keine gültige Recovery.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-OOM-0001`
- `NPSPEC-MEMORY-OOM-DOMAIN-0001`
- `NPSPEC-MEMORY-OOM-SCOPE-0001`
- `NPSPEC-MEMORY-OOM-RECOVERY-0001`
- `NPSPEC-MEMORY-OOM-ACCOUNTING-0001`
- `NPSPEC-MEMORY-OOM-EMERGENCY-0001`
- `NPSPEC-MEMORY-OOM-INTROSPECTION-0001`
- `NPSPEC-MEMORY-OOM-TEST-0001`

---

## Ergebnis

NovaOS behandelt Out-of-Memory nicht primär als globales Systemereignis, sondern innerhalb der Ressourcenhierarchie:

```text
Allocation Failure
        ↓
Identify Cause + Owner
        ↓
Scope-local Recovery
        ↓
Domain-local Recovery
        ↓
Supervision
        ↓
System-wide Escalation
        ↓
Emergency Policy
```

Damit wird Speichermangel möglichst dort isoliert und behoben, wo er verursacht wurde.

Die zentrale Architekturregel lautet:

```text
OOM is local until proven global.

Recover within ownership boundaries first.

Protect unrelated and reserved resources.

Escalate deliberately, not heuristically.
```