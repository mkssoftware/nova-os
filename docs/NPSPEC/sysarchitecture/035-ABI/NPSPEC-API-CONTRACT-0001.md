# NPSPEC-API-CONTRACT-0001 – Nova API Contract

## Status

Angenommen

## Kategorie

API / Contract / Interface Architecture

## Zweck

NovaOS definiert API Contracts als explizite, maschinenlesbare Beschreibung der Semantik und Garantien einer API.

```text
Consumer
   ↓
API Contract
   ↓
Provider
```

Ein API Contract beschreibt nicht nur Funktionsnamen und Datentypen, sondern auch Verhalten, Voraussetzungen, Ergebnisse, Fehler, Ressourcen-, Sicherheits- und Nebenwirkungssemantik.

## Grundprinzipien

```text
API Contract ≠ ABI
API Contract ≠ Implementation
API Contract ≠ Documentation Only
API Availability ≠ Authority
Valid Request ≠ Authorized Request
Success ≠ Verified Result
Signature Compatibility ≠ Semantic Compatibility
```

## Contract Model

```text
APIContract
├── API_ID
├── ContractVersion
├── Operations
├── Types
├── Preconditions
├── Postconditions
├── ErrorSemantics
└── Guarantees
```

Optional:

```text
RequiredCapabilities
ResourceRequirements
ExecutionRequirements
ConcurrencySemantics
TransactionSemantics
SideEffects
CancellationSemantics
DeadlineSemantics
Determinism
TrustRequirements
SovereigntyRequirements
ProvenanceRequirements
```

## Operation Contract

Jede relevante API-Operation soll ihren eigenen Vertrag besitzen.

```text
OperationContract
├── OperationID
├── Inputs
├── Outputs
├── Preconditions
├── Postconditions
├── Errors
└── SideEffects
```

## Preconditions

Preconditions beschreiben Bedingungen vor der Ausführung.

Beispiele:

```text
Valid Input
Valid Handle
Required Capability
Required Object State
Resource Availability
Supported Feature
Compatible Version
```

Nicht erfüllte Preconditions müssen eindeutig behandelbar sein.

## Postconditions

Postconditions beschreiben garantierte Eigenschaften nach erfolgreicher Ausführung.

```text
Operation Success
      ↓
Defined Postconditions Hold
```

Ein Provider darf `Success` nicht melden, wenn verpflichtende Postconditions nicht erfüllt sind.

## Invariants

APIs können Invarianten definieren:

```text
Object Identity remains stable
Capability Authority never increases implicitly
Committed State remains internally consistent
Resource Usage remains within granted limits
```

Invarianten gelten unabhängig von der konkreten Provider-Implementierung.

## Error Semantics

Fehler gehören zum Contract.

```text
InvalidArgument
AccessDenied
Conflict
Unavailable
Timeout
Cancelled
ResourceLimit
Unsupported
UnknownState
```

Dabei gilt:

```text
Timeout ≠ Operation Failed
Cancelled Request ≠ Operation Cancelled
UnknownState ≠ Failure
```

Der Contract muss festlegen, welche Aussagen nach einem Fehler noch sicher getroffen werden können.

## Side Effects

Operationen müssen relevante Nebenwirkungen deklarieren können.

```text
None
Local State Change
Persistent State Change
External Effect
Network Effect
Device Effect
Irreversible Effect
```

Side Effects sind insbesondere für Transactions, Retry und Compensation relevant.

## Idempotency

Der Contract soll deklarieren können:

```text
Idempotent
Conditionally Idempotent
Non-Idempotent
Unknown
```

Dadurch können Retry-Mechanismen sicher entscheiden, ob eine Operation wiederholt werden darf.

## Transaction Semantics

API-Operationen können deklarieren:

```text
Transactional
Transaction Participant
Rollback-capable
Compensatable
Irreversible
```

```text
API Success ≠ Transaction Commit
```

Commit-, Rollback- und Compensation-Semantik bleiben explizit.

## Ownership und Lifetime

Für übergebene Objekte und Buffer muss definiert werden:

```text
Owned
Borrowed
Shared
Transferred
Retained
```

Zusätzlich müssen relevante Lifetime-Regeln eindeutig sein.

## Concurrency

Der Contract kann festlegen:

```text
Thread-safe
Serialized
Concurrent
Reentrant
Single-owner
Ordering Required
```

Consumer dürfen keine stärkeren Concurrency-Garantien annehmen als der Contract zusichert.

## Asynchronität

Asynchrone Operationen müssen ihren Lifecycle definieren.

```text
Submit
  ↓
Accepted
  ↓
Running
  ↓
Completed / Failed / Cancelled / Unknown
```

`Accepted` bedeutet nicht `Completed`.

## Deadline und Cancellation

Der Contract muss relevante Semantik definieren:

```text
Deadline
Cancellation
Timeout
```

Dabei gilt:

```text
Deadline Reached ≠ Operation Never Executed
Cancellation Requested ≠ Cancellation Completed
```

## Resource Contract

Operationen können Ressourcenanforderungen und Limits deklarieren:

```text
CPU
Memory
IO
Network
Storage
Accelerator
Energy
Latency
```

Diese Angaben können mit dem Execution Contract verbunden werden.

## Capability Contract

API Contracts können benötigte Authority deklarieren.

```text
Operation
    ↓
Required Capability Set
```

```text
API Contract ≠ Capability Grant
```

Der Contract beschreibt erforderliche Authority, stellt sie aber nicht bereit.

## Trust und Sovereignty

Operationen können zusätzliche Anforderungen definieren:

```text
Minimum Trust Level
Allowed Provider
Allowed Location
Allowed Jurisdiction
Required Attestation
```

Provider-Auswahl muss diese Anforderungen berücksichtigen.

## Provider Independence

Der Contract beschreibt die Schnittstelle unabhängig von der Implementierung.

```text
Consumer
   ↓
API Contract
   ↓
Provider A
Provider B
Provider C
```

Alle Provider müssen die ausgehandelte Contract-Version erfüllen.

## Contract Negotiation

Optionale Eigenschaften können ausgehandelt werden.

```text
Consumer Requirements
        ∩
Provider Contract
        ∩
Security Policy
        ∩
Trust Policy
        ↓
Effective Contract
```

Der resultierende Contract darf keine Anforderungen stillschweigend abschwächen.

## Contract Evolution

Contracts sind versioniert.

```text
Contract 1.0
    ↓
Compatible Extension
    ↓
Contract 1.1
```

Inkompatible semantische Änderungen benötigen eine neue Major-Version.

## Contract Validation

NovaOS soll Contracts maschinenlesbar validieren können.

Prüfbar sind beispielsweise:

```text
Types
Required Fields
Version
Capabilities
Preconditions
Resource Limits
Feature Requirements
Provider Compatibility
```

Runtime Validation ergänzt statische Prüfung.

## Verification Integration

Kritische Contract-Eigenschaften können mit formaler oder Runtime-Verifikation verbunden werden.

```text
Contract
   ↓
Static Verification
   +
Runtime Contracts
   ↓
Verified Behavior
```

Nicht jede API muss formal verifiziert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
API_ID
ContractVersion
Operations
Types
RequiredCapabilities
SideEffects
Idempotency
TransactionSemantics
ConcurrencySemantics
ResourceRequirements
Provider
```

## Normative Anforderungen

1. NovaOS MUSS explizite API Contracts unterstützen.
2. API Contracts MÜSSEN von ABI und Implementierung getrennt bleiben.
3. Relevante API-Operationen MÜSSEN eindeutig identifizierbar sein.
4. Contracts SOLLEN Preconditions und Postconditions definieren können.
5. Verpflichtende Postconditions MÜSSEN bei erfolgreicher Operation erfüllt sein.
6. Fehlersemantik MUSS Bestandteil des Contracts sein.
7. Side Effects MÜSSEN für relevante Operationen deklarierbar sein.
8. Idempotency MUSS explizit beschreibbar sein.
9. Transaction-, Rollback- und Compensation-Semantik MUSS deklarierbar sein.
10. Ownership und Lifetime MÜSSEN an relevanten API-Grenzen eindeutig sein.
11. Concurrency-Semantik MUSS explizit beschreibbar sein.
12. Asynchrone Operationen MÜSSEN ihren Lifecycle eindeutig definieren.
13. Timeout und Cancellation DÜRFEN NICHT automatisch als Nichtausführung interpretiert werden.
14. Ressourcenanforderungen SOLLEN im Contract ausdrückbar sein.
15. Benötigte Capabilities MÜSSEN deklarierbar sein.
16. API Contracts DÜRFEN keine Authority erzeugen.
17. Trust- und Sovereignty-Anforderungen MÜSSEN ausdrückbar sein.
18. Provider MÜSSEN den ausgehandelten Contract erfüllen.
19. Contract Negotiation DARF verpflichtende Anforderungen NICHT stillschweigend abschwächen.
20. Contracts MÜSSEN versionierbar sein.
21. Inkompatible semantische Änderungen MÜSSEN eine neue Major-Version erfordern.
22. Contracts SOLLEN maschinenlesbar validierbar sein.
23. Kritische Contracts SOLLEN mit Verification-Mechanismen integrierbar sein.
24. API Contracts MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-API-VERSIONING-0001`
- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `ADR-ARCH-0147`

## Ergebnis

```text
Consumer Request
      ↓
API Contract
      ↓
Validate
├── Types
├── Preconditions
├── Capabilities
├── Resources
├── Security
├── Trust
└── Sovereignty
      ↓
Provider Execution
      ↓
Validate Result
      ↓
Postconditions + Defined Semantics
```

NovaOS erhält damit ein explizites API-Contract-Modell, das die tatsächliche Semantik einer Schnittstelle maschinenlesbar beschreibt und APIs unabhängig von Provider, Programmiersprache und interner Implementierung zuverlässig, sicher und evolvierbar macht.