# ADR-IO-0005 – Deadline-Aware I/O

## Status

Angenommen

## Kategorie

I/O / Deadlines / QoS / Realtime / Scheduling

## Kontext

NovaOS unterstützt Deadlines bereits als systemweite Eigenschaft von Structured Concurrency und Execution Contracts.

I/O-Operationen sind häufig Teil eines größeren zeitkritischen Workloads. Eine Deadline darf deshalb nicht an der Grenze zwischen Task Scheduler und I/O-System verloren gehen.

Beispiele sind:

- Audio- und Video-Pipelines,
- interaktive Benutzeraktionen,
- industrielle Steuerung,
- Echtzeitverarbeitung,
- Storage- und Netzwerkoperationen innerhalb zeitkritischer Tasks.

Eine reine Priorisierung reicht dafür nicht aus. Eine hohe Priorität beschreibt relative Wichtigkeit, während eine Deadline einen konkreten zeitlichen Constraint ausdrückt.

---

## Entscheidung

NovaOS macht **Deadlines zu einer nativen Eigenschaft des I/O-Modells**.

Ein I/O Request kann eine Deadline aus seinem Execution Context beziehungsweise `Nova.ExecutionContract` übernehmen.

```text
Execution Scope
      ↓
   Deadline
      ↓
  I/O Request
      ↓
I/O Scheduling
      ↓
 Completion
```

Das I/O-System berücksichtigt diese Deadline bei Queueing, Provider-Auswahl und Ressourcenplanung.

---

## Grundprinzip

```text
A deadline follows the work
through the I/O path.
```

---

## Deadline Propagation

Deadlines sollen entlang einer Ausführungskette propagiert werden.

```text
Task Deadline
     ↓
I/O Request
     ↓
Provider
     ↓
Completion
```

Unteroperationen dürfen dabei keine spätere effektive Deadline erhalten als der verursachende Scope, sofern nicht ausdrücklich ein anderer zulässiger Contract besteht.

Damit bleibt das zeitliche Ziel über Systemgrenzen hinweg erhalten.

---

## Deadline und Priority

Deadline und Priority bleiben getrennte Konzepte.

```text
Priority
→ relative importance

Deadline
→ required completion time
```

Ein hochpriorisierter Request kann keine harte Deadline besitzen.

Umgekehrt kann ein Request mit Deadline aufgrund seiner verbleibenden Zeit bevorzugt behandelt werden.

`ADR-IO-0003` definiert die allgemeine QoS- und Priority-Architektur.

---

## Deadline-Aware Scheduling

I/O-Provider sollen verbleibende Deadline-Zeit bei ihrer Scheduling-Entscheidung berücksichtigen können.

```text
Current Time
     ↓
Remaining Deadline
     ↓
Eligible Requests
     ↓
Provider Scheduling
```

Die konkrete Scheduling-Strategie bleibt Provider- und Policy-abhängig.

Diese ADR schreibt keinen bestimmten Deadline-Scheduling-Algorithmus vor.

---

## Machbarkeit

Eine vorhandene Deadline bedeutet nicht automatisch, dass sie erfüllbar ist.

Provider können anhand ihrer Fähigkeiten und aktuellen Ressourcen feststellen, dass ein Request seine Deadline voraussichtlich nicht einhalten kann.

```text
I/O Request
     ↓
Deadline Feasibility
 ├── Feasible
 └── Not Feasible
```

Nicht erfüllbare harte Deadline-Anforderungen dürfen nicht als garantiert angenommen werden.

---

## Hard Realtime

Bei Hard-Realtime-I/O ist die Deadline ein Hard Constraint.

Solche Requests können zusätzliche Voraussetzungen verlangen, beispielsweise reservierte:

- Queue-Kapazität,
- I/O-Bandbreite,
- Buffer,
- DMA-Ressourcen,
- Provider-Ressourcen.

Eine Hard-Realtime-Garantie darf nur angeboten werden, wenn der gesamte relevante I/O-Pfad die erforderlichen Garantien unterstützt.

---

## Deadline Miss

Wird eine Deadline überschritten, muss dies als expliziter Zustand erkennbar sein.

```text
Completion
 ├── Within Deadline
 └── Deadline Miss
```

Ein Deadline Miss darf nicht automatisch mit Cancellation gleichgesetzt werden.

Abhängig vom Contract kann ein verspätetes Ergebnis weiterhin relevant oder bereits wertlos sein.

Die konkrete Reaktion wird durch den verursachenden Execution Scope und dessen Policy bestimmt.

---

## Cancellation

Deadline und Cancellation arbeiten zusammen, bleiben aber getrennte Mechanismen.

Eine Policy kann bei Überschreitung einer Deadline eine Cancellation auslösen.

Andere Workloads können dagegen ein verspätetes Ergebnis weiterhin akzeptieren.

```text
Deadline Miss
      ↓
Policy
 ├── Continue
 ├── Cancel
 └── Degrade
```

---

## Resource Economy

Deadline-Aware I/O darf Ressourcen priorisieren oder reservieren, muss diese jedoch weiterhin der systemweiten Resource Economy zurechnen.

Eine Deadline erzeugt nicht automatisch unbegrenzte Ressourcen.

Hard-Realtime-Reservierungen müssen explizit vorhanden sein.

---

## Provider Contracts

I/O-Provider müssen deklarieren können, welche zeitlichen Eigenschaften sie unterstützen.

Dabei muss zwischen:

- Best Effort,
- Deadline-Aware,
- reservierbarer QoS,
- garantierbaren Realtime-Eigenschaften

unterschieden werden können.

NovaOS darf keine stärkeren Garantien darstellen, als der zugrunde liegende I/O-Pfad tatsächlich besitzt.

---

## Deterministic Mode

Deadline-Aware I/O muss mit Deterministic Mode kombinierbar sein.

Adaptive Provider- oder Scheduling-Entscheidungen können dabei eingeschränkt oder durch reproduzierbare Policies ersetzt werden.

Harte Deadline-Garantien dürfen nicht von nicht deterministischen Lernmechanismen abhängen.

---

## Normative Anforderungen

1. NovaOS MUSS Deadlines als native Eigenschaft von I/O Requests unterstützen.
2. Deadlines MÜSSEN aus Execution Contexts und Execution Contracts propagierbar sein.
3. Deadline und Priority MÜSSEN getrennte Konzepte bleiben.
4. I/O-Provider SOLLEN verbleibende Deadline-Zeit bei Scheduling-Entscheidungen berücksichtigen können.
5. Die Architektur DARF keinen bestimmten Deadline-Scheduling-Algorithmus erzwingen.
6. Provider MÜSSEN ihre relevanten Deadline- und Realtime-Fähigkeiten deklarieren können.
7. Nicht erfüllbare harte Deadline-Anforderungen MÜSSEN erkennbar sein.
8. Hard-Realtime-Deadlines MÜSSEN als Hard Constraints behandelt werden.
9. Realtime-Garantien DÜRFEN nur bei geeigneten Ressourcenreservierungen und Provider-Garantien zugesichert werden.
10. Ein Deadline Miss MUSS explizit erkennbar sein.
11. Deadline Miss und Cancellation DÜRFEN NICHT grundsätzlich gleichgesetzt werden.
12. Deadline-Aware I/O MUSS in die Resource Economy integriert sein.
13. NovaOS DARF keine stärkeren Deadline-Garantien darstellen, als der vollständige I/O-Pfad tatsächlich unterstützt.
14. Deterministic Mode MUSS adaptive Deadline-Optimierungen kontrollieren können.

---

## Konsequenzen

### Positive Konsequenzen

- zeitkritische Anforderungen bleiben über I/O-Grenzen erhalten,
- bessere Unterstützung für Audio, Video, Interaktion und Realtime,
- Scheduler und I/O-System verwenden dieselben zeitlichen Contracts,
- frühzeitige Erkennung nicht erfüllbarer Anforderungen,
- Grundlage für End-to-End-Latenzgarantien.

### Negative Konsequenzen

- Provider müssen zeitliche Fähigkeiten beschreiben,
- Deadline-Scheduling erhöht die I/O-Planungskomplexität,
- echte Garantien benötigen reservierte Ressourcen.

---

## Verworfene Alternativen

### Deadlines nur im CPU-Scheduler berücksichtigen

Verworfen.

Zeitkritische Tasks könnten weiterhin an nicht deadlinefähigem I/O blockieren.

### Deadlines als hohe Priorität abbilden

Verworfen.

Priorität beschreibt keine konkrete zeitliche Anforderung.

### Jede Deadline automatisch als Hard Realtime behandeln

Verworfen.

Viele Deadlines sind Soft- oder Best-Effort-Anforderungen und benötigen keine harte Garantie.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IO-0001_Completion_Based_Asynchronous_IO`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`
- `ADR-IO-0003_QoS_und_Priority_Aware_IO`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-IO-DEADLINE-PROPAGATION-0001`
- `NPSPEC-IO-DEADLINE-SCHEDULING-0001`
- `NPSPEC-IO-DEADLINE-PROVIDER-0001`
- `NPSPEC-IO-REALTIME-0001`
- `NPSPEC-IO-DEADLINE-TEST-0001`

---

## Ergebnis

Deadlines begleiten zeitkritische Arbeit durch den vollständigen I/O-Pfad:

```text
Execution Contract
        ↓
     Deadline
        ↓
    I/O Request
        ↓
Deadline-Aware Provider
        ↓
    Completion
```

Damit wird die zeitliche Semantik eines Workloads nicht an der I/O-Grenze verloren.

Die zentrale Architekturregel lautet:

```text
Deadlines follow the work,
including through I/O.
```