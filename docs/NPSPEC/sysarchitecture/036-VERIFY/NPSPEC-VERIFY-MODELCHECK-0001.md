# NPSPEC-VERIFY-MODELCHECK-0001 – Nova Model Checking

## Status

Angenommen

## Kategorie

Verification / Model Checking / Verified Core

## Zweck

NovaOS verwendet Model Checking zur systematischen Überprüfung kritischer Zustandsmodelle, Transitionen, Nebenläufigkeit und Protokolle.

```text
Formal Model
     ↓
State Space
     ↓
Explore Transitions
     ↓
Check Properties
     ↓
Valid / Counterexample
```

Model Checking ergänzt mathematische Beweise, statische Analyse, Tests und Runtime Verification.

## Grundprinzipien

```text
Model Checking ≠ Testing
Model Checking ≠ Formal Specification
Model Checking ≠ Complete System Proof
Model Correct ≠ Implementation Correct
Property Holds in Model ≠ Property Holds Everywhere
Counterexample ≠ Root Cause
State Space Exhausted ≠ All Real-World Behavior Covered
```

## Model

Ein prüfbares Modell beschreibt mindestens:

```text
Model
├── States
├── InitialStates
├── Transitions
├── Inputs
├── Constraints
└── Properties
```

Optional:

```text
Concurrency
Timing
Failure Model
Capability Model
Resource Model
Environment Assumptions
```

## State Space

Aus dem Modell entsteht ein Zustandsraum:

```text
Initial State
├── State A
│   ├── State C
│   └── State D
└── State B
    ├── State E
    └── State F
```

Der Model Checker untersucht erreichbare Zustände und Transitionen auf Verletzungen definierter Eigenschaften.

## Safety Properties

Safety Properties beschreiben Zustände, die niemals erreichbar sein dürfen.

Beispiele:

```text
UnauthorizedMemoryAccess == false

CapabilityAmplification == false

InvalidKernelState == false

DoubleOwnership == false
```

Formal:

```text
Always(SafetyInvariant)
```

## Liveness Properties

Liveness beschreibt, dass ein erwarteter Fortschritt grundsätzlich eintreten kann oder muss.

Beispiele:

```text
Request → Eventually Response

RecoveryStarted → Eventually Recovered OR Failed

PreparedTransaction → Eventually Commit OR Abort
```

Liveness muss immer unter expliziten Scheduling- und Failure-Annahmen betrachtet werden.

## Temporal Properties

Zeitliche und sequenzielle Eigenschaften sollen ausdrückbar sein.

Beispiele:

```text
Revoked
   ↓
Never Usable Again
```

```text
Commit
   ↓
Eventually Verification
```

Hierfür können geeignete temporale Logiken verwendet werden.

## Invariants

Kritische Invarianten aus der formalen Spezifikation werden über alle erreichbaren modellierten Zustände geprüft.

```text
Initial State
     ↓
Transition*
     ↓
Invariant always true?
```

Wird eine Verletzung gefunden, erzeugt der Model Checker einen Gegenbeispielpfad.

## Counterexamples

Ein Counterexample soll mindestens enthalten:

```text
Initial State
Transition Sequence
Relevant Inputs
Failure Point
Violated Property
```

Beispiel:

```text
S0
 ↓ Task A reads version
S1
 ↓ Task B modifies state
S2
 ↓ Task A commits stale state
S3
 ↓ Invariant violated
```

Counterexamples sollen reproduzierbar und analysierbar sein.

## Concurrency

Model Checking soll besonders für nebenläufige Komponenten verwendet werden.

Zu untersuchen sind beispielsweise:

```text
Race Conditions
Deadlocks
Livelocks
Lost Updates
Invalid Interleavings
Ordering Violations
Double Commit
Use-after-Revoke
```

Unterschiedliche relevante Interleavings sollen systematisch untersucht werden.

## Capability Model

Kritische Capability-Operationen sollen modelliert werden können:

```text
Create
Delegate
Attenuate
Transfer
Use
Revoke
Expire
```

Zu prüfende Kernregel:

```text
Authority(Output)
⊆
AuthorizedAuthority(Input)
```

Unzulässige Capability Amplification muss als Property-Verletzung erkennbar sein.

## Transaction Model

Model Checking soll kritische Transaction-Abläufe prüfen können.

```text
Begin
 ↓
Prepare
 ↓
Commit
```

mit möglichen Störungen:

```text
Crash
Timeout
Concurrent Change
Capability Revocation
Participant Failure
Network Partition
```

Eigenschaften wie:

```text
No Double Commit
No Invalid State Publication
Prepared ≠ Committed
Unknown ≠ Aborted
```

sollen überprüfbar sein.

## State Machines

Nova State Machines eignen sich direkt für Model Checking.

```text
States
+
Transitions
+
Guards
+
Invariants
     ↓
Model Checker
```

Nicht erreichbare, ungültige oder problematische Zustände können dadurch früh erkannt werden.

## Distributed Systems

Verteilte Protokolle sollen mit Failure-Szenarien modelliert werden können.

Beispiele:

```text
Message Loss
Message Delay
Reordering
Duplicate Message
Node Failure
Network Partition
Recovery
```

Dabei gilt:

```text
No Response ≠ Remote Failure
Local State ≠ Global Truth
```

## Failure Injection

Das Modell soll definierte Fehler systematisch einführen können.

```text
Normal Transition
Failure Transition
Recovery Transition
```

Dadurch können Recovery- und Resilience-Eigenschaften überprüft werden.

## State-Space Explosion

Der Zustandsraum kann exponentiell wachsen.

NovaOS darf deshalb Verfahren einsetzen wie:

```text
State Abstraction
Partial Order Reduction
Symmetry Reduction
Bounded Model Checking
Compositional Verification
State Pruning
Environment Constraints
```

Reduktionen dürfen relevante Eigenschaften nicht unbemerkt entfernen.

## Bounded Model Checking

Bei sehr großen Zustandsräumen darf die Prüfung begrenzt werden.

```text
Depth ≤ N
```

Das Ergebnis muss dann ausdrücklich als begrenzt gekennzeichnet werden.

```text
No Counterexample Found Within Bound
≠
Property Proven Universally
```

## Implementation Mapping

Modelle müssen auf reale Komponenten zurückführbar sein.

```text
Model Transition
      ↓
Interface / Operation
      ↓
Implementation Component
```

Änderungen an relevanter Implementierung müssen eine Prüfung auslösen, ob Modell und Verifikation weiterhin gültig sind.

## Verification Pipeline

```text
Formal Specification
        ↓
Model
        ↓
Model Checker
        ↓
Property Result
        ↓
Counterexample?
├── Yes → Analyze → Fix → Recheck
└── No  → Record Verification Result
```

## Verification Artifacts

Ergebnisse sollen mindestens referenzieren können:

```text
ModelID
ModelVersion
SpecificationID
Properties
Assumptions
Tool
ToolVersion
Bounds
Result
Counterexamples
BuildID
```

## Deterministische Reproduktion

Counterexamples sollen nach Möglichkeit in ausführbare Test- oder Replay-Szenarien überführt werden.

```text
Counterexample
      ↓
Transition Sequence
      ↓
Replay / Test
      ↓
Implementation Validation
```

Damit verbindet NovaOS formale Modellprüfung mit realer Implementierungsprüfung.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ModelID
ModelVersion
Target
Checked Properties
Assumptions
State-Space Size
Applied Reductions
Bounds
Verification Result
Counterexamples
Tool Version
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS Model Checking für kritische Komponenten unterstützen können.
2. Modelle MÜSSEN ihre Initial States und Transitions explizit definieren.
3. Zu prüfende Properties MÜSSEN explizit angegeben werden.
4. Kritische Safety Invariants SOLLEN systematisch geprüft werden.
5. Relevante Liveness Properties SOLLEN prüfbar sein.
6. Temporal Properties MÜSSEN bei geeigneten Komponenten modellierbar sein.
7. Nebenläufige Komponenten SOLLEN auf relevante Interleavings geprüft werden.
8. Capability Amplification MUSS modellprüfbar sein können.
9. Kritische Transaction States SOLLEN modellgeprüft werden.
10. State Machines SOLLEN direkt als Grundlage für Model Checking verwendbar sein.
11. Distributed Models MÜSSEN relevante Partial-Failure-Szenarien darstellen können.
12. Failure Transitions MÜSSEN modellierbar sein.
13. Gefundene Property-Verletzungen SOLLEN reproduzierbare Counterexamples erzeugen.
14. Counterexample DARF NICHT automatisch als Root Cause interpretiert werden.
15. State-Space-Reduktionen MÜSSEN dokumentierbar sein.
16. Bounded Model Checking MUSS als begrenzte Verifikation gekennzeichnet werden.
17. Ein innerhalb eines Bounds fehlender Counterexample DARF NICHT als universeller Beweis interpretiert werden.
18. Modelle MÜSSEN mit ihrer formalen Spezifikation verknüpfbar sein.
19. Modelle SOLLEN auf reale Implementierungskomponenten zurückführbar sein.
20. Relevante Implementierungsänderungen MÜSSEN eine Revalidierung des Modells auslösen können.
21. Model-Checking-Ergebnisse MÜSSEN versionierbar sein.
22. Counterexamples SOLLEN in Tests oder Replay-Szenarien überführbar sein.
23. Model-Checking-Ergebnisse MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-RESILIENCE-FAULTINJECTION-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-0001`
- `ADR-VERIFY-0002`

## Ergebnis

```text
Formal Specification
        ↓
Build State Model
        ↓
Define Properties
        ↓
Explore State Space
        ↓
Check All Modeled Paths
        ↓
Property Violation?
├── Yes → Counterexample → Fix → Recheck
└── No  → Record Verified Model Property
        ↓
Map to Implementation
        ↓
Tests / Replay / Continuous Verification
```

NovaOS erhält damit einen systematischen Model-Checking-Ansatz, mit dem kritische Zustandsmaschinen, Nebenläufigkeit, Capability-Regeln, Transaktionen und verteilte Protokolle auf unerlaubte Zustände und problematische Ablaufpfade untersucht werden können.