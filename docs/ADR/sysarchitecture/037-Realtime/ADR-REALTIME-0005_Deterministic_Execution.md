# ADR-REALTIME-0005 – Deterministic Execution

## Status

Angenommen

## Kategorie

Realtime / Determinism / Scheduling / Execution Contracts

## Kontext

Realtime-Systeme benötigen vorhersehbares Verhalten.

Dabei müssen zwei Eigenschaften klar getrennt werden:

```text
Temporal Determinism
```

und:

```text
Functional Determinism
```

Temporal Determinism bedeutet, dass relevante Ausführungsschritte innerhalb definierter zeitlicher Grenzen erfolgen.

Functional Determinism bedeutet, dass gleiche definierte Eingaben und gleicher definierter Systemzustand reproduzierbar zum gleichen Ergebnis führen.

Für NovaOS-Realtime-Workloads können beide Eigenschaften gemeinsam erforderlich sein.

Unkontrollierte Faktoren wie:

```text
Dynamic Scheduling
Paging
Unbounded Allocation
Randomized Algorithms
Adaptive Decisions
Model Selection
Network Variability
Unbounded Locks
Background Interference
```

dürfen eine zugesicherte deterministische Ausführung nicht unvorhersehbar verändern.

## Entscheidung

NovaOS unterstützt **Deterministic Execution** als explizite Eigenschaft des `Nova.ExecutionContract`.

```text
Execution Contract
      ↓
Determinism Requirements
      ↓
Execution Planning
      ↓
Fixed / Bounded Environment
      ↓
Execution
      ↓
Verification
```

Deterministische Realtime-Ausführung bedeutet nicht, dass das gesamte Betriebssystem global deterministisch arbeiten muss.

Die Garantie gilt für den definierten Execution Scope.

## Determinism Profile

Ein Execution Contract kann festlegen:

```text
Determinism:
    None
    Temporal
    Functional
    Temporal + Functional
```

Dadurch können Workloads exakt ausdrücken, welche Art von Vorhersagbarkeit erforderlich ist.

## Temporal Determinism

Temporal Determinism verlangt begrenzbares zeitliches Verhalten.

```text
Event
  ↓
Bounded Response
  ↓
Bounded Execution
  ↓
Result
```

Dazu gehören insbesondere:

```text
Scheduling Latency
Interrupt Latency
Blocking Time
Execution Time
IPC Latency
I/O Latency
```

Temporal Determinism bildet damit eine Grundlage für Hard-Realtime-Deadline-Garantien.

## Functional Determinism

Functional Determinism verlangt reproduzierbares Verhalten unter definierten Bedingungen.

```text
Same Input
+
Same Relevant State
+
Same Versions
+
Same Deterministic Configuration
=
Same Defined Result
```

Dafür müssen relevante Einflüsse explizit kontrolliert oder versioniert werden.

## Determinism Scope

Determinismus gilt immer innerhalb eines definierten Scopes.

Beispiele:

```text
Single Function
Capability Invocation
Process
Realtime Domain
Execution Pipeline
Complete Execution Contract
```

NovaOS darf nicht aus einer lokal deterministischen Komponente automatisch auf globale Determiniertheit schließen.

## Fixed Execution Environment

Für deterministische Executions können relevante Komponenten fixiert werden.

Beispiele:

```text
Algorithm Version
Capability Version
Driver Version
Model Version
Provider
Configuration Version
Policy Version
Input Object Version
```

Dynamische Änderungen während der Execution dürfen diese Bindungen nicht unkontrolliert verändern.

## Algorithm Selection

Automatische Algorithmuswahl bleibt möglich, muss bei gefordertem Functional Determinism jedoch reproduzierbar sein.

Beispiel:

```text
Execution Contract
      ↓
Deterministic Selection
      ↓
Algorithm A v3
```

Eine adaptive Auswahl darf nicht bei identischem deterministischen Zustand zufällig zwischen verschiedenen Implementierungen wechseln.

## Scheduling Determinism

Realtime Scheduling muss nicht zwingend eine identische globale Task-Reihenfolge erzeugen.

Es muss jedoch die im Contract zugesicherten zeitlichen Eigenschaften deterministisch einhalten können.

Für strengere Workloads kann zusätzlich eine reproduzierbare Scheduling Policy verlangt werden.

```text
Same Realtime Domain
+
Same Inputs
+
Same Scheduling State
→
Defined Scheduling Behavior
```

## Memory

Deterministische Realtime-Pfade dürfen nicht von unbounded oder unvorhersehbaren Speichermechanismen abhängen.

Zu vermeiden sind insbesondere:

```text
Unexpected Page Faults
Swap
Unbounded Allocation
Unbounded Reclamation
Non-deterministic Memory Migration
```

Benötigter Speicher soll vor Beginn der kritischen Execution reserviert werden.

## Synchronisation

Synchronisationsmechanismen auf deterministischen Realtime-Pfaden müssen begrenzbares Verhalten besitzen.

```text
Lock Acquisition
Barrier
Semaphore
IPC Wait
```

Unbounded Waiting ist mit einer entsprechenden temporalen Garantie nicht vereinbar.

## I/O

Deterministische Execution kann nur so stark garantiert werden wie ihre I/O-Abhängigkeiten.

```text
Execution
   ↓
Driver
   ↓
Device
```

Ein Gerät mit unbekannter oder unbounded Response Time verhindert Temporal Determinism für den davon abhängigen Pfad.

## Randomness

Benötigt eine Execution Zufallswerte, muss der Contract definieren, ob diese:

```text
True Random
Controlled Random
Seeded Deterministic
```

sein sollen.

Für reproduzierbare Executions können deterministische Seeds verwendet werden.

Security-relevante kryptografische Zufälligkeit darf dadurch nicht abgeschwächt werden.

## Time Sources

Zeitabhängige Logik muss ihre Zeitquelle explizit behandeln.

Beispiele:

```text
Monotonic Clock
Realtime Clock
External Clock
Virtual Deterministic Clock
```

Unkontrollierte Änderungen der Wall Clock dürfen eine deterministische Execution nicht unbeabsichtigt verändern.

## External Inputs

Externe Daten können Functional Determinism verhindern.

Beispiele:

```text
Network Responses
Sensors
User Input
Remote Services
Current Time
```

Soll eine Execution reproduzierbar sein, müssen relevante Inputs:

```text
Captured
Versioned
Recorded
or
Explicitly Declared Non-Deterministic
```

werden.

## Distributed Execution

Verteilte Systeme besitzen zusätzliche Quellen von Nichtdeterminismus:

```text
Network Ordering
Latency Variation
Node Failure
Clock Differences
Concurrent Updates
```

NovaOS darf Distributed Deterministic Execution nur zusichern, wenn diese Faktoren ausreichend kontrolliert werden.

Location Transparency darf diese Grenzen nicht verbergen.

## AI

AI-Ausführungen gelten nicht automatisch als funktional deterministisch.

Ein AI-Workload kann nur als deterministisch behandelt werden, wenn relevante Faktoren fixiert sind.

Beispiele:

```text
Model ID
Model Version
Model Variant
Runtime Version
Provider
Inference Parameters
Seed
Context Version
Input Versions
```

Wenn der verwendete Provider keine entsprechende Garantie unterstützt, darf NovaOS keine Functional-Determinism-Garantie behaupten.

## Adaptive Systeme

Adaptive Mechanismen werden während deterministischer Executions entweder:

```text
Fixed
Versioned
Disabled
or
Excluded from the Deterministic Path
```

Adaptive Optimierung darf außerhalb des garantierten Pfades weiterhin stattfinden.

## Prediction

Vorhersagen dürfen zur Vorbereitung einer deterministischen Execution verwendet werden.

Beispielsweise:

```text
Predictive Preloading
Resource Preparation
Cache Warming
```

Die Korrektheit oder Garantie darf jedoch nicht davon abhängen, dass eine probabilistische Vorhersage richtig ist.

## Realtime Domains

Eine Realtime Domain kann als deterministische Execution Domain konfiguriert werden.

```text
Deterministic Realtime Domain
 ├── Fixed Scheduling Constraints
 ├── Reserved Resources
 ├── Bounded Dependencies
 ├── Fixed Versions
 └── Controlled Inputs
```

Damit kann NovaOS vollständige zeitkritische Pipelines kontrollieren.

## Failure Verhalten

Kann eine geforderte Determinism Guarantee nicht mehr eingehalten werden:

```text
Determinism Guarantee
        ↓
Violation
        ↓
Explicit Failure
```

NovaOS darf die Execution nicht stillschweigend als weiterhin deterministisch behandeln.

Mögliche Reaktionen:

```text
Abort
Fallback
Restart
Safe State
Reject Result
Escalate
```

## Replay

Functional Determinism kann reproduzierbare Ausführungen erleichtern.

```text
Recorded Inputs
+
Versions
+
Configuration
+
Deterministic Execution
        ↓
Replay
```

Deterministic Execution bedeutet jedoch nicht automatisch, dass vollständiges Replay verfügbar ist.

Replay benötigt zusätzlich die notwendigen Inputs und Zustandsinformationen.

## Observability

Deterministische Executions müssen ihre relevanten Bedingungen nachvollziehbar machen.

Dazu gehören:

```text
Execution ID
Determinism Profile
Algorithm Version
Capability Version
Provider
Input Versions
Configuration Version
Policy Version
Resource Reservation
Timing Bounds
Determinism Violation
```

Observability darf den deterministischen Realtime-Pfad selbst nicht unkontrolliert verändern.

## Verhältnis zu Realtime

Realtime und Determinismus bleiben getrennte Dimensionen.

```text
Fast
≠
Realtime

Realtime
≠
Deterministic

Deterministic
≠
Realtime
```

Ein Hard-Realtime-Workload kann jedoch beide Eigenschaften benötigen:

```text
Temporal Determinism
+
Bounded Latency
+
Temporal Isolation
+
Deadline Guarantee
```

## Normative Anforderungen

1. NovaOS MUSS Deterministic Execution über Execution Contracts ausdrücken können.
2. Temporal und Functional Determinism MÜSSEN getrennt spezifizierbar sein.
3. Beide Determinism-Arten MÜSSEN gemeinsam anforderbar sein.
4. Determinism MUSS für einen expliziten Execution Scope definiert werden.
5. Relevante Algorithmen, Capabilities, Provider und Versionen MÜSSEN fixierbar sein.
6. Functional Determinism MUSS relevante Eingaben und Zustände berücksichtigen.
7. Temporal Determinism MUSS Scheduling-, Blocking-, Execution- und relevante I/O-Latenzen berücksichtigen.
8. Unbounded Blocking DARF nicht Bestandteil eines entsprechend garantierten temporaldeterministischen Pfades sein.
9. Unvorhersehbare Memory Operations DÜRFEN entsprechende Realtime-Garantien nicht beeinträchtigen.
10. Automatische Algorithmuswahl MUSS bei gefordertem Functional Determinism reproduzierbar sein.
11. Zufallsabhängige Executions MÜSSEN ihre Randomness-Semantik definieren können.
12. Deterministische Seeds DÜRFEN kryptografische Sicherheitsanforderungen nicht abschwächen.
13. Zeitabhängige Executions MÜSSEN ihre relevante Zeitquelle definieren können.
14. Externe Inputs MÜSSEN für reproduzierbare Executions kontrollierbar oder erfassbar sein.
15. Distributed Determinism DARF nur garantiert werden, wenn der relevante verteilte Pfad ausreichende Garantien besitzt.
16. AI-Ausführungen DÜRFEN nicht automatisch als funktional deterministisch gelten.
17. Für deterministische AI-Ausführung MÜSSEN relevante Model-, Runtime-, Provider- und Input-Eigenschaften fixierbar sein.
18. Adaptive Mechanismen DÜRFEN eine zugesicherte Determinism Guarantee nicht verändern.
19. Probabilistische Vorhersagen DÜRFEN keine notwendige Determinism Guarantee ersetzen.
20. Verlust einer zugesicherten Determinism-Eigenschaft MUSS explizit signalisiert werden.
21. Determinism Violations MÜSSEN beobachtbar und auditierbar sein.
22. NovaOS DARF keine Determinism Guarantee behaupten, die der vollständige relevante Execution Path nicht erfüllen kann.
23. Deterministic Execution MUSS unabhängig davon funktionieren können, ob AI verfügbar ist.
24. Realtime und Determinism MÜSSEN als getrennte, kombinierbare Contract-Eigenschaften behandelt werden.

## Konsequenzen

### Positive Konsequenzen

- Realtime-Verhalten wird reproduzierbarer und analysierbarer,
- zeitliche und funktionale Determiniertheit werden sauber getrennt,
- Algorithmen, Provider und Versionen können für kritische Executions fixiert werden,
- adaptive und AI-basierte Optimierungen bleiben möglich, ohne harte Garantien zu kontrollieren,
- reproduzierbare Tests und Fehleranalysen werden erleichtert,
- deterministische Realtime Domains können vollständige kritische Pipelines kapseln.

### Negative Konsequenzen

- dynamische Optimierungen müssen auf deterministischen Pfaden teilweise eingeschränkt werden,
- externe und verteilte Abhängigkeiten erschweren starke Determinism Guarantees,
- reproduzierbare Ausführung benötigt zusätzliche Versions-, Input- und Zustandsinformationen.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-EXECUTION-0008_Preferred_und_Forced_Algorithm`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ALGORITHM-0007_Deterministische_Algorithmusauswahl`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-AI-0013_KI_darf_harte_Systemregeln_nicht_überschreiben`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-DETERMINISTIC-EXECUTION-0001`
- `NPSPEC-REALTIME-TEMPORAL-DETERMINISM-0001`
- `NPSPEC-REALTIME-FUNCTIONAL-DETERMINISM-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-DOMAIN-0001`
- `NPSPEC-REALTIME-DETERMINISM-VIOLATION-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-REPLAY-0001`

## Ergebnis

NovaOS behandelt Determinismus als explizite Eigenschaft einer Execution:

```text
Execution Contract
      ↓
Determinism Profile
      ↓
Fixed Versions
Controlled Inputs
Bounded Resources
Bounded Timing
      ↓
Deterministic Execution
```

Der zentrale Grundsatz lautet:

```text
Realtime sagt,
wann ein Ergebnis vorliegen muss.

Functional Determinism sagt,
welches Ergebnis unter definierten
Bedingungen entsteht.

Temporal Determinism sagt,
dass relevante zeitliche Abläufe
innerhalb definierter Grenzen bleiben.

NovaOS behandelt diese Eigenschaften
explizit und kombiniert sie nur dort,
wo der Execution Contract
sie tatsächlich verlangt.
```