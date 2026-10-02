# NPSPEC-VERIFY-TEMPORAL-0001 – Nova Temporal Verification

## Status

Angenommen

## Kategorie

Verification / Temporal Verification / Verified Core

## Zweck

NovaOS definiert Temporal Verification zur formalen Überprüfung zeitabhängiger Systemeigenschaften.

Damit wird nicht nur geprüft, **welcher Zustand** erlaubt ist, sondern auch **wann**, **in welcher Reihenfolge** und **unter welchen zeitlichen Bedingungen** Zustände und Ereignisse auftreten dürfen.

```text
State
  ↓
Event
  ↓
Time Constraint
  ↓
Transition
  ↓
Temporal Property
```

## Grundprinzipien

```text
Temporal Correctness ≠ Functional Correctness
Deadline ≠ Timeout
Timeout ≠ Failure
Event Order ≠ Wall-Clock Order
Eventually ≠ Immediately
No Response ≠ No Execution
Timing Guarantee ≠ Performance Estimate
Measured Latency ≠ Proven Bound
```

## Temporal Model

```text
TemporalProperty
├── PropertyID
├── Target
├── Events
├── States
├── Ordering
├── TimeConstraints
└── Assumptions
```

Optional:

```text
Deadline
MinimumDelay
MaximumDelay
Period
Jitter
ClockDomain
RealtimeProfile
FailurePolicy
VerificationArtifact
```

## Temporal Properties

NovaOS muss unterschiedliche zeitliche Eigenschaften ausdrücken können.

Beispiele:

```text
Always P

Eventually P

P Until Q

P Before Q

P After Q

P Within T

P Never After Q
```

Beispiel:

```text
Capability Revoked
      ↓
Capability must never
be successfully used afterwards
```

## Safety

Temporale Safety Properties beschreiben Ereignisse, die niemals in einer verbotenen Reihenfolge auftreten dürfen.

```text
Commit
must not occur
before
Validation
```

Formal vereinfacht:

```text
Commit → Previously(Validated)
```

## Liveness

Liveness beschreibt erforderlichen Fortschritt.

```text
Request Accepted
      ↓
Eventually
      ↓
Completed / Failed / Cancelled
```

Liveness-Garantien müssen ihre Scheduling-, Ressourcen- und Failure-Annahmen explizit angeben.

## Ordering

Kritische Ereignisse benötigen definierte Ordnungsbeziehungen.

```text
Validate
   ↓
Prepare
   ↓
Commit
   ↓
Verify
```

Nicht jede reale Ausführung benötigt eine globale Zeitordnung.

Verteilte Systeme dürfen kausale oder partielle Ordnung verwenden.

```text
Causal Order ≠ Global Clock Order
```

## Deadlines

Eine Deadline definiert den spätesten zulässigen Zeitpunkt für eine relevante Eigenschaft.

```text
Start
  ↓
Execution
  ↓
Deadline
```

Deadline-Verifikation muss zwischen:

```text
Soft
Firm
Hard
```

unterscheiden.

## Hard Realtime

Hard-Realtime-Garantien benötigen beweisbare oder konservativ begrenzte Worst-Case-Eigenschaften.

Zu berücksichtigen sind insbesondere:

```text
Scheduling
Interrupt Latency
Locking
Memory Access
Page Faults
I/O
Execution Time
Preemption
Resource Contention
```

```text
Average Execution Time ≠ WCET
```

## Timeout Semantik

Ein Timeout beschreibt eine Beobachtungs- oder Wartegrenze.

```text
Request
   ↓
Timeout
```

Daraus folgt nicht:

```text
Operation Failed
Operation Cancelled
Operation Never Executed
```

Der tatsächliche Zustand kann `Unknown` sein.

## Cancellation

Temporal Verification muss Cancellation-Races berücksichtigen.

```text
Operation Running
      ↓
Cancel Requested
      ↓
Operation Completes
```

```text
Cancellation Requested ≠ Cancelled
```

Die zulässigen Transitionen müssen formal definiert werden.

## Transactions

Transaction Ordering muss überprüfbar sein.

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Commit Decision
  ↓
Commit
  ↓
Verify
```

Unzulässige Reihenfolgen müssen als Property-Verletzung erkannt werden können.

## Capability Revocation

Revocation besitzt kritische temporale Eigenschaften.

```text
Revocation Effective
        ↓
Future Capability Use
        ↓
Must Fail
```

Concurrent Use und Revocation müssen explizit modelliert werden.

## State Reconciliation

Temporal Properties können Reconciliation begrenzen.

```text
Drift Detected
      ↓
Within T
      ↓
Reconciliation Started
```

Dies darf nur als Garantie spezifiziert werden, wenn die notwendigen Ressourcen und Scheduling-Eigenschaften garantiert sind.

## Distributed Systems

Verteilte Temporal Verification muss berücksichtigen:

```text
Clock Drift
Network Delay
Message Reordering
Partitions
Node Failure
Unknown Remote State
```

NovaOS darf keine perfekte globale Uhr voraussetzen.

Mögliche Ordnungsmodelle:

```text
Local Time
Monotonic Time
Logical Clock
Causal Order
Synchronized Clock
```

## Clock Model

Jede zeitkritische Spezifikation muss ihren Clock Context definieren können.

```text
ClockDomain
├── ClockSource
├── Resolution
├── Monotonicity
├── Accuracy
└── SynchronizationAssumptions
```

Wall Clock darf nicht automatisch für monotone Deadline-Berechnungen verwendet werden.

## Model Checking

Temporale Eigenschaften sollen durch Model Checking geprüft werden können.

Beispiele:

```text
Deadlock
Livelock
Starvation
Missed Deadline
Invalid Ordering
Infinite Retry
Infinite Recovery Loop
Revocation Race
Cancellation Race
```

## Determinismus

Deterministische Modi müssen zeitliche Einflüsse kontrollieren oder aufzeichnen können.

```text
Initial State
+
Inputs
+
Event Ordering
+
Temporal Inputs
      ↓
Replay
```

Wall-Clock-Zeit darf nicht unkontrolliert deterministische Ausführung beeinflussen.

## Runtime Verification

Nicht vollständig statisch beweisbare Temporal Properties dürfen zur Laufzeit überwacht werden.

```text
Temporal Contract
       ↓
Runtime Monitor
       ↓
Violation?
├── No  → Continue
└── Yes → Policy Action
```

Mögliche Reaktionen:

```text
Warn
Cancel
Degrade
Failover
Recover
Enter Safe State
```

## Verification Artifacts

Temporal Verification soll mindestens referenzieren können:

```text
SpecificationID
PropertyID
Component
Temporal Model
Clock Assumptions
Scheduling Assumptions
Resource Assumptions
Verified Bounds
Tool
BuildID
Verification Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Temporal Properties
Realtime Profile
Deadlines
Verified Bounds
Clock Domain
Assumptions
Verification Status
Runtime Violations
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS temporale Eigenschaften formal spezifizieren können.
2. Temporal Correctness MUSS von funktionaler Correctness getrennt bleiben.
3. Safety- und Liveness-Properties MÜSSEN unterscheidbar sein.
4. Kritische Event-Reihenfolgen MÜSSEN formal beschreibbar sein.
5. Deadlines MÜSSEN explizit modellierbar sein.
6. Soft-, Firm- und Hard-Realtime-Anforderungen MÜSSEN unterscheidbar sein.
7. Hard-Realtime-Garantien DÜRFEN NICHT auf Durchschnittswerten basieren.
8. Timeout DARF NICHT automatisch als Failure interpretiert werden.
9. Cancellation Requested DARF NICHT automatisch als Cancelled interpretiert werden.
10. Transaction Ordering MUSS formal überprüfbar sein können.
11. Capability Revocation MUSS temporal überprüfbar sein.
12. Concurrent Revocation und Capability Use MÜSSEN modellierbar sein.
13. Reconciliation Deadlines MÜSSEN mit verfügbaren Garantien vereinbar sein.
14. Distributed Temporal Verification DARF keine perfekte globale Uhr voraussetzen.
15. Zeitkritische Spezifikationen MÜSSEN ihren Clock Context definieren können.
16. Monotone Zeit SOLL für Deadline-Messung verwendet werden.
17. Clock Drift und Synchronisationsfehler MÜSSEN bei verteilten Garantien berücksichtigt werden.
18. Deadlocks, Livelocks und Starvation SOLLEN temporal überprüfbar sein.
19. Deterministische Modi MÜSSEN relevante temporale Einflüsse kontrollieren oder erfassen können.
20. Nicht statisch beweisbare Temporal Properties SOLLEN durch Runtime Verification überwachbar sein.
21. Runtime-Verletzungen MÜSSEN definierte Policy-Reaktionen auslösen können.
22. Verifizierte Zeitgrenzen MÜSSEN ihre Annahmen dokumentieren.
23. Temporal-Verification-Ergebnisse MÜSSEN mit konkreten Implementierungsversionen verknüpfbar sein.
24. Temporal-Verification-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-VERIFY-CAPABILITYSAFETY-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `ADR-VERIFY-0006`

## Ergebnis

```text
System Behavior
      ↓
States + Events
      ↓
Ordering + Time Constraints
      ↓
Temporal Properties
      ↓
Formal / Model Verification
      ↓
Runtime Monitoring
      ↓
Temporal Guarantees
```

NovaOS erhält damit ein formales Temporal-Verification-Modell, das nicht nur korrekte Zustände, sondern auch Reihenfolge, Fortschritt, Deadlines, Revocation, Cancellation und zeitabhängige Systemgarantien überprüfbar macht.