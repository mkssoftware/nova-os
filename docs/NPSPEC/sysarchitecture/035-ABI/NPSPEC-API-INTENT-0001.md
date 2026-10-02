# NPSPEC-API-INTENT-0001 – Nova Intent API

## Status

Angenommen

## Kategorie

API / Intent / Semantic Execution

## Zweck

NovaOS definiert Intent APIs als deklarative Schnittstelle, über die ein Consumer beschreibt, **was erreicht werden soll**, ohne zwingend festzulegen, welche konkrete API, Anwendung, Implementierung oder welcher Provider dies ausführt.

```text
User / Application Intent
          ↓
      Intent API
          ↓
Semantic Resolution
          ↓
Execution Contract
          ↓
Provider / Capability
          ↓
Execution
```

## Grundprinzipien

```text
Intent ≠ Command
Intent ≠ Implementation
Intent ≠ Provider Selection
Intent ≠ Authority
Intent ≠ Execution
Intent Resolution ≠ Permission
Requested Outcome ≠ Guaranteed Outcome
```

Der Intent beschreibt das gewünschte Ergebnis.

NovaOS bestimmt daraus einen zulässigen Ausführungsweg.

## Intent Model

```text
Intent
├── IntentID
├── IntentType
├── Inputs
├── DesiredOutcome
├── Constraints
└── State
```

Optional:

```text
SemanticTypes
ExecutionContractID
RequiredCapabilities
PreferredProvider
ForcedProvider
ResourceBudget
Latency
Deadline
Determinism
TrustRequirements
SovereigntyRequirements
InteractionPolicy
TransactionPolicy
ProvenanceID
```

## Zustände

```text
Created
Validating
Resolving
Resolved
AwaitingAuthorization
Executing
Completed
Failed
Cancelled
Ambiguous
Unsupported
Unknown
```

## Intent Types

Beispiele:

```text
Open
Edit
Convert
Render
Print
Share
Store
Search
Analyze
Calculate
Simulate
Communicate
Execute
```

Intent Types besitzen stabile semantische Identitäten und können erweitert werden.

## Ergebnisorientierung

Beispiel:

```text
Intent:
"Dieses Bild anzeigen"
```

Der Consumer muss nicht vorgeben:

```text
Start Program X
Load Library Y
Call Function Z
```

NovaOS kann stattdessen bestimmen:

```text
Image
  ↓
Render Intent
  ↓
Semantic API
  ↓
Compatible Provider
```

## Intent Resolution

```text
Intent
  ↓
Validate Input
  ↓
Resolve Semantic Types
  ↓
Resolve Required Capability
  ↓
Find Semantic API
  ↓
Discover Providers
  ↓
Apply Constraints
  ↓
Execution Plan
```

Resolution erzeugt noch keine Ausführungsberechtigung.

## Semantic Integration

Intent APIs verwenden Semantic Types und Semantic APIs.

```text
Intent
├── Input: Image
├── Action: Convert
└── Output: Document
```

NovaOS kann daraus geeignete Konvertierungs- und Verarbeitungspfade bestimmen.

## Ambiguity

Ein Intent kann mehrdeutig sein.

```text
Intent
   ↓
Multiple Valid Interpretations
   ↓
Ambiguous
```

NovaOS darf dann:

```text
Ask User
Use Explicit Policy
Use Stored Preference
Select Safe Default
Reject
```

Eine unsichere Interpretation darf bei sicherheitskritischen Operationen nicht stillschweigend gewählt werden.

## Constraints

Intents können harte und weiche Anforderungen enthalten.

```text
Hard:
Security
Capability
Trust
Sovereignty
Deadline
Required Format

Soft:
Preferred Provider
Energy Preference
Performance Preference
User Preference
```

Harte Constraints dürfen nicht zugunsten einer bevorzugten Lösung verletzt werden.

## Capability Integration

Intent Resolution erzeugt keine Authority.

```text
Intent
   ↓
Required Capability
   ↓
Capability Validation
   ↓
Execution
```

```text
Intent ≠ Permission
```

## Execution Contract

Aus einem Intent kann ein Execution Contract erzeugt werden.

```text
Intent
   ↓
Resolved Requirements
   ↓
Execution Contract
   ↓
Execution
```

Der Contract konkretisiert unter anderem:

```text
Operation
Semantic I/O
Resources
Latency
Deadline
Determinism
Trust
Sovereignty
Provider Constraints
```

## Provider Selection

Mehrere Provider können denselben Intent erfüllen.

```text
Intent
  ↓
Semantic API
├── Provider A
├── Provider B
└── Provider C
```

Die Auswahl erfolgt anhand des effektiven Execution Contracts und der System Policies.

## Composition

Komplexe Intents können in Teil-Intents zerlegt werden.

```text
Intent
"Dokument als PDF senden"

        ↓

Render Document
        ↓
Convert PDF
        ↓
Select Recipient
        ↓
Transmit
```

Die Abhängigkeiten zwischen Teil-Intents müssen explizit bleiben.

## Transactions

Mehrstufige Intents können Transaktionen erzeugen.

```text
Intent
   ↓
Execution Plan
   ↓
Transaction
```

Operationen müssen dabei ihre:

```text
Rollback
Compensation
Idempotency
Irreversibility
```

deklarieren.

## User Interaction

NovaOS kann vor kritischen Aktionen Bestätigung verlangen.

```text
Intent
   ↓
Risk / Authority Check
   ↓
Confirmation Required?
├── Yes → User Decision
└── No  → Continue
```

Automatisierung darf explizite User Decisions nicht umgehen.

## Generated UI Integration

Intent APIs bilden eine Grundlage für generierte Oberflächen.

```text
UI Interaction
      ↓
Intent
      ↓
Semantic API
      ↓
Capability
      ↓
Execution
```

Damit kann dieselbe Systemfähigkeit über unterschiedliche Oberflächen verwendet werden.

Die generierte UI ist nicht selbst die Authority.

## Location Transparency

Ein Intent kann lokal, remote oder verteilt erfüllt werden.

```text
Intent
├── Local Provider
├── Remote Provider
└── Distributed Provider
```

Standorttransparenz darf Trust-, Sovereignty- oder Capability-Prüfungen nicht umgehen.

## Cancellation

Intents können abbrechbar sein.

```text
Cancel Intent
     ↓
Cancel Execution Plan
     ↓
Propagate Cancellation
```

```text
Intent Cancelled ≠ All Effects Reversed
```

Bereits ausgeführte Effekte können Rollback oder Compensation benötigen.

## Provenance

Nachvollziehbar sein sollen:

```text
IntentID
Intent Type
Origin
Resolved Meaning
Selected API
Selected Provider
Constraints
Authorization
Execution Result
```

Sensitive Inhalte müssen geschützt bleiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
IntentID
IntentType
State
Semantic Inputs
Desired Outcome
Constraints
Resolved API
Selected Provider
Required Capabilities
ExecutionContractID
TransactionID
```

## Normative Anforderungen

1. NovaOS MUSS deklarative Intent APIs unterstützen können.
2. Intent MUSS von konkreter Implementierung und Provider getrennt bleiben.
3. Intents MÜSSEN stabile semantische Intent Types besitzen können.
4. Intent Resolution DARF NICHT als Authority interpretiert werden.
5. Erforderliche Capabilities MÜSSEN vor Ausführung validiert werden.
6. Intent Inputs und Outcomes SOLLEN Semantic Types verwenden.
7. Intent Resolution SOLL Semantic APIs verwenden.
8. Intents MÜSSEN harte und weiche Constraints unterscheiden können.
9. Harte Constraints DÜRFEN NICHT durch Optimierung oder Präferenzen verletzt werden.
10. Mehrdeutige Intents MÜSSEN explizit behandelbar sein.
11. Sicherheitskritische Mehrdeutigkeit DARF NICHT stillschweigend aufgelöst werden.
12. Aus Intents MÜSSEN Execution Contracts erzeugt werden können.
13. Provider Selection MUSS den effektiven Execution Contract respektieren.
14. Komplexe Intents MÜSSEN in abhängige Teil-Intents zerlegbar sein.
15. Mehrstufige Intents MÜSSEN mit Transactions integrierbar sein.
16. Irreversible Effekte MÜSSEN explizit erkennbar sein.
17. Kritische Aktionen MÜSSEN User Confirmation verlangen können.
18. Automatisierung DARF explizite User Decisions NICHT umgehen.
19. Intent APIs MÜSSEN Generated UI unterstützen können.
20. Location Transparency DARF Security-, Trust- und Sovereignty-Regeln NICHT umgehen.
21. Cancellation DARF NICHT automatisch als Rollback interpretiert werden.
22. Intent-Ausführung MUSS nachvollziehbar und autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-API-VERSIONING-0001`
- `NPSPEC-API-CONTRACT-0001`
- `NPSPEC-API-SEMANTIC-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `ADR-ARCH-0149`

## Ergebnis

```text
User / Application
        ↓
      Intent
        ↓
Semantic Resolution
        ↓
Constraint Resolution
        ↓
Capability Validation
        ↓
Execution Contract
        ↓
Provider Selection
        ↓
Execution
        ↓
Desired Semantic Outcome
```

NovaOS erhält damit eine deklarative Intent-Schicht, über die Nutzer, Anwendungen und generierte Oberflächen gewünschte Ergebnisse beschreiben können, während das System selbstständig einen sicheren, kompatiblen und den definierten Constraints entsprechenden Ausführungsweg bestimmt.