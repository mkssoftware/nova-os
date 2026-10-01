# NPSPEC-DISTRIBUTED-EXECUTION-0001 – Nova Distributed Execution

## Status

Angenommen

## Kategorie

Distributed / Execution / Location Transparency

## Zweck

NovaOS definiert ein einheitliches Modell für die Ausführung semantischer Operationen über mehrere lokale oder entfernte Systeme hinweg.

Eine Operation beschreibt weiterhin **was** ausgeführt werden soll. Der Execution Contract bestimmt die Bedingungen. NovaOS entscheidet anschließend, ob die Ausführung lokal, remote oder verteilt erfolgt.

```text
Semantic Operation
      ↓
ExecutionContract
      ↓
Execution Planning
      ↓
Local / Remote / Distributed
      ↓
Execution
      ↓
Semantic Result
```

## Grundprinzipien

```text
Operation ≠ Location
Provider ≠ Location
Remote Address ≠ Identity
Network Access ≠ Authority
Distributed ≠ Trusted
Distributed ≠ Replicated
Distributed ≠ Automatically Faster

Identity ≠ Location
Transparent Location ≠ Transparent Authority
```

## Distributed Execution Model

Eine verteilte Ausführung wird beschrieben durch:

```text
DistributedExecution
├── ExecutionID
├── ExecutionContractID
├── OperationID
├── Coordinator
├── Participants
└── State
```

Optional:

```text
ExecutionPlan
Task Graph
Data Placement
Resource Assignment
Provider Assignment
Deadline
Trust Constraints
Sovereignty Constraints
Failure Policy
Recovery Policy
```

## Execution Participants

Teilnehmer können sein:

```text
Local CPU
Local GPU
Local NPU
Other NovaOS Device
Trusted Server
Remote Compute Node
Distributed Cluster
Specialized Accelerator
```

Jeder Teilnehmer besitzt eine eigene:

```text
Identity
Location
Trust State
Capabilities
Resources
Security Domain
Sovereignty Domain
```

## Execution Planning

NovaOS zerlegt eine Operation nur dann, wenn dies semantisch zulässig ist.

```text
Operation
   ↓
Execution Graph
   ↓
Task A ──→ Node A
Task B ──→ Node B
Task C ──→ Node C
   ↓
Result Composition
```

Abhängigkeiten zwischen Tasks müssen explizit bleiben.

## Location Transparency

Die semantische Operation ist unabhängig vom Ausführungsort.

```text
Operation
├── Local
├── Remote
└── Distributed
```

Ein Wechsel des Ausführungsortes darf die definierte Semantik nicht verändern.

Location Transparency bedeutet jedoch nicht, dass jeder Ort zulässig ist.

## Execution Contract

Alle Teilnehmer müssen die für ihren Teil relevanten Hard Requirements erfüllen.

Dazu gehören:

```text
Semantic Types
Resource Budgets
Latency
Deadline
Determinism
Trust
Sovereignty
Security
Precision
Algorithm Requirements
Provider Requirements
```

## Provider Selection

Provider können über mehrere Systeme verteilt sein.

```text
Provider Discovery
      ↓
Contract Filtering
      ↓
Eligible Providers
      ↓
Placement
```

Ein entfernter Provider darf nur gewählt werden, wenn alle relevanten Constraints erfüllt sind.

## Capability Security

Distributed Execution erzeugt keine neue Authority.

```text
ExecutionContract
      ↓
Required Authority
      ↓
Minimal Capabilities
      ↓
Explicit Delegation
```

Capabilities dürfen nur kontrolliert an entfernte Teilnehmer delegiert werden.

Delegierte Capabilities sollen:

```text
Attenuated
Task-Bound
Time-Bound
Revocable
Minimal
```

sein.

## Data Placement

Daten werden nur zu Teilnehmern übertragen, die sie tatsächlich benötigen.

```text
Task
 +
Required Data
 +
Required Capability
      ↓
Participant
```

Unnötige Datenverteilung ist zu vermeiden.

Zero-Copy kann lokal verwendet werden; zwischen Systemen werden geeignete Transport- und Serialisierungsmechanismen eingesetzt.

## Sovereignty

Vor jeder Datenübertragung muss geprüft werden:

```text
Source Domain
Destination Domain
Data Classification
Sovereignty Policy
Execution Contract
```

Temporäre Daten, Zwischenergebnisse und Caches unterliegen denselben relevanten Sovereignty Constraints.

## Trust

Remote Teilnehmer müssen entsprechend dem Execution Contract bewertet werden.

Mögliche Evidenz:

```text
Identity
Attestation
Code Integrity
Software Provenance
Provider Trust
Device Trust
Revocation State
```

```text
Unknown Trust ≠ Trusted
```

## Resource Coordination

Jeder Teilnehmer besitzt lokale Ressourcenbudgets.

```text
Global Execution Budget
        ↓
Node A Budget
Node B Budget
Node C Budget
```

Teilbudgets dürfen zusammen die zulässigen Grenzen des übergeordneten Execution Contracts nicht überschreiten.

## Deadline Propagation

Eine globale Deadline kann auf Teiloperationen verteilt werden.

```text
Global Deadline
      ↓
Task Deadlines
      ↓
Communication Budget
      ↓
Synchronization Budget
```

Netzwerk- und Synchronisationslatenz müssen berücksichtigt werden.

## Structured Concurrency

Verteilte Tasks bleiben Teil einer gemeinsamen strukturierten Execution-Hierarchie.

```text
Distributed Execution
├── Task Group A
│   ├── Remote Task
│   └── Local Task
└── Task Group B
```

Cancellation muss kontrolliert propagiert werden können.

## Failure Model

Distributed Execution muss partielle Fehler erwarten.

Beispiele:

```text
Node Failure
Network Failure
Provider Failure
Timeout
Capability Revocation
Trust Change
Resource Exhaustion
Partial Completion
```

NovaOS muss insbesondere unterscheiden:

```text
NotStarted
Running
Completed
Failed
Cancelled
UnknownExecutionState
```

Ein Verbindungsabbruch bedeutet nicht automatisch, dass eine entfernte Operation nicht ausgeführt wurde.

## Retry

Retries sind nur zulässig, wenn die Operationssemantik dies erlaubt.

```text
Retry ≠ Safe
```

NovaOS muss insbesondere Idempotency und den aktuellen Execution State berücksichtigen.

## Replanning

Bei einem Ausfall kann NovaOS:

```text
Retry
Replace Provider
Move Task
Recompute
Degrade
Cancel
Fail
```

Der neue Execution Plan muss weiterhin alle Hard Requirements erfüllen.

## Transactions

Verteilte Ausführung bedeutet nicht automatisch globale ACID-Transaktionen.

Falls atomare Zustandsänderungen benötigt werden, müssen diese explizit über das NovaOS Transaction Model koordiniert werden.

```text
Distributed Execution ≠ Global Transaction
```

## Determinismus

Bei:

```text
Determinism = Required
```

müssen auch:

```text
Task Ordering
Input Versions
Algorithms
Provider Versions
External Inputs
Synchronization
Result Composition
```

die geforderte Determinismusklasse erfüllen.

## Observability

Eine verteilte Ausführung soll über eine gemeinsame Identität verfolgt werden können.

```text
ExecutionID
TraceID
TaskID
ProviderID
NodeID
TransactionID
```

Damit bleibt die gesamte Ausführung über Systemgrenzen hinweg nachvollziehbar.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ExecutionID
Participants
Task Graph
Execution State
Provider Assignment
Resource Assignment
Data Placement
Deadlines
Trust State
Sovereignty State
Network State
Failures
Retries
Replanning
```

## Normative Anforderungen

1. NovaOS MUSS lokale und verteilte Ausführung über dasselbe semantische Execution-Modell unterstützen.
2. Operation Identity MUSS unabhängig vom Ausführungsort bleiben.
3. Jeder Teilnehmer MUSS die relevanten Hard Requirements des Execution Contracts erfüllen.
4. Remote Execution DARF keine implizite Authority erzeugen.
5. Capability Delegation MUSS explizit und möglichst minimal erfolgen.
6. Daten DÜRFEN nur innerhalb zulässiger Security-, Trust- und Sovereignty-Domains übertragen werden.
7. Resource Budgets und Deadlines MÜSSEN auf verteilte Teiloperationen propagierbar sein.
8. Distributed Tasks MÜSSEN in Structured Concurrency integrierbar sein.
9. Kommunikationsfehler DÜRFEN NICHT automatisch als bestätigtes Execution Failure interpretiert werden.
10. Retries DÜRFEN nur bei geeigneter Operationssemantik erfolgen.
11. Replanning DARF Hard Constraints NICHT abschwächen.
12. Distributed Execution DARF NICHT automatisch globale Transaktionssemantik voraussetzen.
13. Required Determinism MUSS auch bei verteilter Ausführung erhalten bleiben.
14. Verteilte Ausführung MUSS über ExecutionID und Trace-Kontext autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-EXECUTION-ALGORITHM-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `NPSPEC-DISTCOMM-TRACE-0001`
- `ADR-ARCH-0059`

## Ergebnis

```text
Semantic Operation
      ↓
ExecutionContract
      ↓
Distributed Execution Plan
      ↓
Capability + Trust + Sovereignty Validation
      ↓
Task Placement
      ↓
Local + Remote Execution
      ↓
Coordination + Monitoring
      ↓
Semantic Result
```

NovaOS erhält damit ein location-transparentes Distributed-Execution-Modell, bei dem lokale und entfernte Ressourcen gemeinsam genutzt werden können, ohne Identität, Authority, Trust, Sovereignty oder die semantischen Anforderungen einer Operation durch die Verteilung aufzuweichen.