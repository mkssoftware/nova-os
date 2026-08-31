# ADR-SCHED-0012 – Hard Constraints vor Optimierungszielen

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Policy / Constraints / Optimierung / Execution Contracts

## Kontext

Der NovaOS-Scheduler berücksichtigt gleichzeitig zahlreiche Ziele und Anforderungen:

- Safety,
- Security und Isolation,
- Data Sovereignty,
- Realtime,
- Deadlines,
- Determinismus,
- Resource Budgets,
- CPU-Affinity,
- NUMA- und Cache-Lokalität,
- QoS und Latenz,
- Performance,
- Energie,
- Thermal State,
- heterogene CPUs,
- Accelerator-Auswahl,
- adaptive und predictive Optimierung.

Diese Anforderungen besitzen nicht dieselbe Verbindlichkeit.

Einige definieren, **welche Scheduling-Entscheidungen überhaupt zulässig sind**. Andere bestimmen lediglich, **welche der zulässigen Entscheidungen bevorzugt werden soll**.

Ohne eine klare Trennung könnte eine Optimierung beispielsweise:

- einen Required-Affinity-Constraint verletzen,
- einen nicht autorisierten Accelerator verwenden,
- Daten in eine unzulässige Location verschieben,
- eine Realtime-Reservation beeinträchtigen,
- Determinismus zugunsten höherer Performance aufgeben.

NovaOS benötigt deshalb eine verbindliche Reihenfolge zwischen Hard Constraints und Optimierungszielen.

---

## Entscheidung

NovaOS verwendet für Scheduling-Entscheidungen das Prinzip:

**Hard Constraints werden vollständig aufgelöst, bevor Soft Constraints und Optimierungsziele bewertet werden.**

```text
Scheduling Request
       ↓
Hard Constraints
       ↓
Feasible Set
       ↓
Soft Constraints
       ↓
Optimization Objectives
       ↓
Best Permitted Decision
```

Optimierung darf ausschließlich innerhalb der durch Hard Constraints definierten zulässigen Lösungsmenge stattfinden.

Existiert keine zulässige Lösung, darf der Scheduler keine Constraint-Verletzung als vermeintlich beste Lösung auswählen.

---

## Grundprinzip

```text
First determine what is allowed.

Then determine what is preferred.

Never optimize an invalid decision.
```

---

## Hard Constraints

Hard Constraints definieren verbindliche Grenzen einer Scheduling-Entscheidung.

Dazu können insbesondere gehören:

- Safety Constraints,
- Security- und Isolation-Constraints,
- Data-Sovereignty-Constraints,
- Required Capabilities,
- Hard-Realtime-Reservations,
- Deterministic Requirements,
- Required CPU-/Device-Affinity,
- Required CPU Features,
- Required Provider,
- verbindliche Resource Limits.

Eine Scheduling-Entscheidung, die einen solchen Constraint verletzt, ist nicht zulässig.

---

## Soft Constraints

Soft Constraints beschreiben Präferenzen, deren Verletzung zulässig ist.

Beispiele:

- Preferred CPU,
- Preferred NUMA Node,
- Preferred Provider,
- Cache Locality,
- bevorzugte CPU-Klasse,
- Energy Preference,
- Latency Preference.

Soft Constraints können gewichtet und gegeneinander abgewogen werden.

---

## Optimierungsziele

Nachdem die zulässige Lösungsmenge bestimmt wurde, darf NovaOS innerhalb dieser Menge optimieren.

Typische Ziele sind:

```text
Performance
Latency
Throughput
Fairness
Locality
Energy Efficiency
Thermal Distribution
Migration Cost
Resource Efficiency
```

Die konkrete Gewichtung ist Policy.

---

## Constraint Resolution

Die Scheduling-Entscheidung erfolgt konzeptionell in drei Phasen:

```text
1. Feasibility
      ↓
2. Policy Preference
      ↓
3. Optimization
```

### Phase 1 – Feasibility

Alle Hard Constraints werden angewendet.

Ergebnis:

```text
Eligible CPUs / Providers / Resources
```

### Phase 2 – Policy Preference

Soft Constraints und Scheduling-Class-Regeln reduzieren beziehungsweise bewerten die verbleibenden Möglichkeiten.

### Phase 3 – Optimization

Innerhalb der zulässigen Möglichkeiten wird die beste verfügbare Entscheidung gewählt.

---

## Globale Prioritätsordnung

Wenn mehrere verbindliche Systemregeln miteinander interagieren, gilt grundsätzlich die NovaOS-Policy-Reihenfolge:

```text
Safety
   ↓
Security / Isolation
   ↓
Data Sovereignty
   ↓
Hard Realtime
   ↓
Deterministic Requirements
   ↓
Correctness Requirements
   ↓
Explicit Required Constraints
   ↓
Resource Budgets / Reservations
   ↓
Scheduling Policy
   ↓
Performance / Latency / Locality
   ↓
Energy / Adaptive Optimization
```

Diese Reihenfolge beschreibt Architekturprioritäten. Detailregeln und zulässige Konfliktauflösungen werden durch die jeweiligen Contracts und NPSPECs spezifiziert.

---

## Unlösbare Constraints

Hard Constraints können gemeinsam eine leere Lösungsmenge erzeugen.

Beispiel:

```text
Required CPU Feature = AVX-X
Required NUMA Node = 2

Node 2:
    no compatible CPU
```

Der Scheduler darf dann nicht still einen Constraint ignorieren.

Stattdessen muss abhängig vom Contract eine definierte Reaktion erfolgen:

```text
Reject
Wait
Suspend
Fail
Replan
Explicit Degradation
```

---

## Explizite Degradation

Ein Hard Constraint darf nur dann abgeschwächt werden, wenn der zugehörige Contract ausdrücklich eine entsprechende Degradationsregel definiert.

Beispiel:

```text
PreferredProvider = NPU
Fallback = GPU
Fallback = CPU
```

Ein `RequiredProvider = NPU` darf dagegen nicht automatisch ersetzt werden.

Damit bleibt Graceful Degradation explizit und überprüfbar.

---

## Realtime

Hard-Realtime-Reservations und zugelassene Realtime Constraints gehören zur Feasibility-Phase.

Performance-, Energy-, Cache- oder Adaptive-Policies dürfen reservierte Realtime-Ressourcen nicht unzulässig verwenden.

Thermal Safety bleibt als Hardware-Safety-Grenze über einer Realtime-Garantie.

---

## Affinity und Topology

Required Affinity begrenzt die zulässige CPU-Menge.

Topology Awareness optimiert anschließend innerhalb dieser Menge.

```text
Required Affinity
       ↓
Eligible CPUs
       ↓
NUMA / Cache / Load Optimization
```

Damit kann Locality niemals eine Required Affinity überschreiben.

---

## Heterogene CPUs

Required CPU Capabilities werden vor Performance- oder Energieoptimierung geprüft.

Erst danach darf NovaOS zwischen kompatiblen CPUs anhand von:

- Performance,
- Energie,
- Thermal State,
- Locality

wählen.

---

## Accelerator Scheduling

Dasselbe Prinzip gilt für Compute Provider:

```text
Required Semantics
Trust
Data Sovereignty
Determinism
Required Provider Constraints
        ↓
Eligible Providers
        ↓
Performance / Energy / Latency Optimization
```

Ein schnellerer Accelerator ist irrelevant, wenn er für den Execution Contract nicht zulässig ist.

---

## QoS und Latency

Soft QoS-Ziele dürfen die Auswahl innerhalb der zulässigen Ressourcen beeinflussen.

Sie dürfen jedoch keine:

- Realtime Reservation,
- Resource Isolation,
- Security Policy,
- Data-Sovereignty-Regel

umgehen.

---

## Energy und Thermal

Energy Efficiency ist grundsätzlich ein Optimierungsziel.

Thermal Safety ist dagegen eine harte Systemgrenze.

Damit gilt:

```text
Thermal Safety
      >
Energy Optimization
```

und gegebenenfalls:

```text
Thermal Safety
      >
Realtime Guarantee
```

wenn Hardware andernfalls gefährdet würde.

---

## Predictive und Adaptive Scheduling

Predictive Scheduling darf nur die Bewertung zulässiger Entscheidungen verändern.

```text
Hard Constraints
      ↓
Feasible Set
      ↓
Prediction
      ↓
Optimization
```

Nicht zulässig ist:

```text
Prediction
      ↓
Ignore Constraint
```

Auch hohe Modell-Confidence erzeugt keine zusätzliche Authority.

---

## Automatische Algorithmusauswahl

Das Prinzip gilt ebenfalls für die automatische Algorithmusauswahl von NovaOS.

```text
Semantic Correctness
Safety
Security
Data Sovereignty
Determinism
Resource Constraints
        ↓
Compatible Algorithms
        ↓
Performance / Energy / Latency Optimization
```

Ein schnellerer Algorithmus darf nicht gewählt werden, wenn er einen Hard Constraint verletzt.

---

## Fail-Safe Verhalten

Kann die Constraint Engine nicht sicher feststellen, dass eine Entscheidung zulässig ist, darf sie diese nicht aufgrund eines vermuteten Optimierungsvorteils wählen.

Für Hard Constraints gilt:

```text
Unknown != Allowed
```

Die konkrete konservative Reaktion hängt vom jeweiligen Constraint-Typ ab.

---

## Introspection

NovaOS muss nachvollziehbar machen können:

- welche Hard Constraints angewendet wurden,
- welche Kandidaten ausgeschlossen wurden,
- welche Soft Constraints wirkten,
- welche Optimierungsziele verwendet wurden,
- warum die finale Entscheidung gewählt wurde.

Beispiel:

```text
Candidate GPU-1:
    rejected
    reason: Data Sovereignty

Candidate NPU-0:
    eligible

Candidate CPU-4:
    eligible

Selected:
    NPU-0

Reason:
    lowest expected latency
```

---

## Normative Anforderungen

1. NovaOS MUSS Hard Constraints vor Soft Constraints und Optimierungszielen auswerten.
2. Optimierung DARF ausschließlich innerhalb der durch Hard Constraints zulässigen Lösungsmenge erfolgen.
3. Eine Constraint-verletzende Entscheidung DARF NICHT aufgrund besserer Performance oder Effizienz gewählt werden.
4. Hard und Soft Constraints MÜSSEN semantisch unterscheidbar sein.
5. Required und Preferred Anforderungen MÜSSEN getrennt behandelt werden.
6. Safety Constraints MÜSSEN Vorrang vor Scheduling-Optimierung besitzen.
7. Security- und Isolation-Constraints MÜSSEN vor Performanceoptimierung geprüft werden.
8. Data-Sovereignty-Constraints MÜSSEN vor Provider- und Locality-Optimierung geprüft werden.
9. Hard-Realtime-Reservations MÜSSEN vor allgemeiner Ressourcenoptimierung berücksichtigt werden.
10. Deterministic Requirements DÜRFEN nicht durch adaptive Optimierung aufgehoben werden.
11. Required Affinity MUSS vor Topology- und Locality-Optimierung angewendet werden.
12. Required CPU Capabilities MÜSSEN vor CPU-Class-Optimierung geprüft werden.
13. Required Provider Constraints MÜSSEN vor automatischer Provider-Auswahl angewendet werden.
14. Resource Budgets und Reservations DÜRFEN nicht durch QoS- oder Performanceoptimierung umgangen werden.
15. Eine leere zulässige Lösungsmenge MUSS explizit behandelt werden.
16. Der Scheduler DARF bei unlösbaren Hard Constraints NICHT still einen Constraint entfernen.
17. Degradation eines Hard Constraints MUSS explizit durch den zugehörigen Contract erlaubt sein.
18. Soft Constraints DÜRFEN gegeneinander abgewogen werden.
19. Performance, Latenz, Locality und Energie DÜRFEN als Optimierungsziele verwendet werden.
20. Thermal Safety MUSS unabhängig von Optimierungszielen durchsetzbar bleiben.
21. Predictive Scheduling DARF ausschließlich zulässige Entscheidungen bewerten.
22. Modell-Confidence DARF keine zusätzliche Scheduling-Authority erzeugen.
23. Automatische Algorithmusauswahl MUSS dieselbe Hard-Constraint-vor-Optimierung-Regel befolgen.
24. Unbekannte Hard-Constraint-Kompatibilität DARF NICHT optimistisch als zulässig behandelt werden.
25. Constraint- und Optimierungsentscheidungen MÜSSEN introspektierbar sein.
26. Scheduler-Korrektheit MUSS unabhängig von adaptiven Optimierungsmodellen gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- klare Trennung zwischen Zulässigkeit und Optimierung,
- Hard Constraints können nicht versehentlich durch Heuristiken verletzt werden,
- konsistentes Verhalten über CPU-, Accelerator- und Algorithmusauswahl,
- Predictive Scheduling bleibt sicher begrenzt,
- Entscheidungen werden besser überprüfbar und introspektierbar,
- neue Optimierungsziele können hinzugefügt werden, ohne die Sicherheitsarchitektur zu verändern.

### Negative Konsequenzen

- Constraint Resolution wird zu einem expliziten Scheduling-Schritt,
- widersprüchliche Hard Constraints können Scheduling verhindern,
- Optimierungsalgorithmen müssen mit einer bereits eingeschränkten Lösungsmenge arbeiten,
- Degradationsregeln müssen explizit modelliert werden.

---

## Verworfene Alternativen

### Alle Anforderungen über gewichtete Scores behandeln

Verworfen.

Ein ausreichend hoher Performance-Score könnte dadurch theoretisch einen Security- oder Safety-Constraint überstimmen.

### Optimieren und anschließend Constraints prüfen

Verworfen.

Dies erzeugt unnötige ungültige Kandidaten und erschwert eine saubere Policy-Semantik.

### Hard Constraints bei Ressourcenknappheit automatisch abschwächen

Verworfen.

Eine solche Degradation muss ausdrücklich im Execution Contract erlaubt sein.

### Adaptive Modelle dürfen Constraint-Prioritäten verändern

Verworfen.

Prediction Error darf Optimierungsmodelle verbessern, aber keine Systemregeln neu definieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0008_Heterogeneous_CPU_Scheduling`
- `ADR-SCHED-0009_GPU_NPU_und_Accelerator_Scheduling`
- `ADR-SCHED-0010_QoS_und_Latency_Aware_Scheduling`
- `ADR-SCHED-0011_Predictive_und_Adaptive_Scheduling`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-CONSTRAINT-0001`
- `NPSPEC-SCHED-CONSTRAINT-RESOLUTION-0001`
- `NPSPEC-SCHED-HARD-CONSTRAINT-0001`
- `NPSPEC-SCHED-SOFT-CONSTRAINT-0001`
- `NPSPEC-SCHED-OPTIMIZATION-0001`
- `NPSPEC-SCHED-DEGRADATION-0001`
- `NPSPEC-SCHED-CONSTRAINT-INTROSPECTION-0001`
- `NPSPEC-SCHED-CONSTRAINT-TEST-0001`

---

## Ergebnis

NovaOS trennt Scheduling konsequent in Zulässigkeit und Optimierung:

```text
Hard Constraints
      ↓
What is allowed?
      ↓
Feasible Set
      ↓
Soft Constraints + Objectives
      ↓
What is best?
      ↓
Scheduling Decision
```

Damit können zukünftige Optimierungen, adaptive Modelle und neue Hardwareklassen den Scheduler verbessern, ohne seine verbindlichen Systemregeln zu unterlaufen.

Die zentrale Architekturregel lautet:

```text
Correctness before optimization.

Constraints before preferences.

Feasibility before scoring.

Optimize only what is allowed.
```