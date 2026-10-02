# NPSPEC-STATE-GLOBAL-0001 – Nova Global State Model

## Status

Angenommen

## Kategorie

State / Architecture / System Model

## Zweck

NovaOS definiert ein einheitliches Modell für systemweit relevante Zustände.

Dabei wird kein einzelner universeller globaler Speicher eingeführt. Stattdessen entsteht der globale Systemzustand aus eindeutig verantworteten, versionierten und beobachtbaren Teilzuständen.

```text
Global System State
├── Kernel State
├── Resource State
├── Security State
├── Object State
├── Service State
├── Device State
├── Configuration State
└── Distributed State
```

## Grundprinzipien

```text
Global State ≠ Global Variable
Global State ≠ Single Database
Global State ≠ Single Source of Truth
Observed State ≠ Desired State
Cached State ≠ Authoritative State
Distributed State ≠ Immediately Consistent State
State Visibility ≠ Authority
```

## State Model

```text
StateRecord
├── StateID
├── StateType
├── OwnerID
├── Version
├── Value
├── Validity
└── Timestamp
```

Optional:

```text
Generation
SourceID
TransactionID
ProvenanceID
SecurityLabel
TrustState
Location
Dependencies
```

## State Ownership

Jeder autoritative Zustand benötigt einen eindeutig bestimmbaren Owner.

```text
State
  ↓
Owner
  ↓
Authority over State Mutation
```

Mehrere Komponenten dürfen denselben Zustand beobachten.

Die Schreibverantwortung muss jedoch explizit definiert sein.

```text
Many Readers
     +
Defined Writer / Coordination Policy
```

## State Identity

Zustände erhalten stabile Identitäten.

```text
StateID ≠ Memory Address
StateID ≠ Process ID
StateID ≠ Storage Location
```

Dadurch kann Zustand unabhängig von seiner physischen Speicherung referenziert werden.

## Desired und Actual State

NovaOS unterscheidet:

```text
Desired State
     ↕
Reconciliation
     ↕
Actual State
```

Änderungen am Desired State bedeuten nicht automatisch, dass der Actual State bereits angepasst wurde.

## State Transitions

Zustandsänderungen müssen kontrollierte Übergänge bilden.

```text
State A
   ↓
Validate Transition
   ↓
State B
```

Ungültige Übergänge müssen abgelehnt werden.

## Versioning

Zustände müssen Änderungen erkennen können.

```text
State v17
   ↓
Update
   ↓
State v18
```

Versionen oder Generationsnummern ermöglichen:

```text
Conflict Detection
Stale-State Detection
Transaction Validation
Cache Validation
Replay
```

## Transaction Integration

Mehrere zusammengehörige Zustandsänderungen sollen über das Nova Transaction System koordiniert werden.

```text
Current State
     ↓
Transaction
     ↓
Prepare
     ↓
Commit
     ↓
New State
```

```text
State Modified ≠ Transaction Committed
```

## Concurrency

Concurrent State Updates müssen explizit behandelt werden.

Mögliche Mechanismen:

```text
Version Check
Atomic Operation
Lock
RCU
Transaction
Compare-and-Swap
Conflict Resolution
```

Die Strategie hängt vom jeweiligen State Type ab.

## State Observation

Komponenten können relevante Zustände beobachten.

```text
State Change
    ↓
Observation
    ↓
Subscriber
```

Observation erzeugt keine Änderungsberechtigung.

```text
Observe State ≠ Modify State
```

## Cached State

State darf gecacht werden.

```text
Authoritative State
       ↓
Cache
       ↓
Consumer
```

Cache-Einträge benötigen Informationen über:

```text
Version
Generation
Validity
Source
```

```text
Cached State ≠ Current State
```

## Derived State

Zustände können aus anderen Zuständen berechnet werden.

```text
State A
   +
State B
   ↓
Derived State C
```

Derived State muss seine Abhängigkeiten nachvollziehbar machen können.

Er ist nicht automatisch autoritativ.

## Distributed State

Bei verteilten Komponenten kann es mehrere Beobachtungen desselben logischen Zustands geben.

```text
Node A State
Node B State
Node C State
      ↓
Consistency Model
```

NovaOS setzt keine universelle sofortige globale Konsistenz voraus.

```text
No Response ≠ Failed
Remote State ≠ Current Global Truth
Network Partition ≠ Remote Failure
```

Das jeweilige Subsystem definiert sein Consistency Model.

## Security State

Sicherheitskritische Zustände benötigen besondere Regeln.

Beispiele:

```text
Capability Revocation
Trust State
Security Policy
Key State
Integrity State
Identity State
```

Monotone Sicherheitsentscheidungen dürfen nicht durch veralteten State rückgängig gemacht werden.

Beispiel:

```text
Revoked Capability
      ↓
Old Cached State
      ↓
MUST NOT become Valid again
```

## Unknown State

NovaOS behandelt unbekannten Zustand explizit.

```text
Known Valid
Known Invalid
Unknown
```

```text
Unknown ≠ Valid
Unknown ≠ Invalid
```

Die jeweilige Policy entscheidet über das sichere Verhalten.

## Failure und Recovery

Nach Absturz oder Kommunikationsverlust kann State unsicher sein.

```text
Failure
   ↓
Recover State
   ↓
Validate
   ↓
Reconcile
   ↓
Verify
```

Recovery darf Zustand nicht allein aufgrund alter Cache-Daten als korrekt deklarieren.

## Live Evolution

Bei Component Replacement kann State übertragen werden.

```text
Old Component
      ↓
Validate State
      ↓
Transfer
      ↓
New Component
      ↓
Verify
```

```text
ABI Compatible ≠ State Compatible
```

State Migration benötigt einen eigenen kompatiblen Contract.

## Determinismus

Deterministische Modi müssen relevante State Transitions reproduzierbar machen können.

Dazu können aufgezeichnet werden:

```text
Input
State Version
Transition
Ordering
External Events
Result
```

## Provenance

Kritische Zustandsänderungen sollen nachvollziehbar sein.

```text
Who
What
When
Why
Source
Transaction
Previous Version
New Version
```

Sensitive Inhalte bleiben geschützt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
StateID
StateType
Owner
Version
Validity
Source
Dependencies
Transaction State
Location
Last Transition
```

## Normative Anforderungen

1. NovaOS MUSS systemweit relevante Zustände eindeutig modellieren können.
2. Global State DARF NICHT als universelle globale Variable implementiert werden.
3. Autoritativer State MUSS einen definierten Owner besitzen.
4. State Identity MUSS von Speicherort und Speicheradresse getrennt bleiben.
5. Desired State und Actual State MÜSSEN unterscheidbar sein.
6. State Transitions MÜSSEN validierbar sein.
7. Relevanter State MUSS versionierbar sein.
8. Veralteter State MUSS erkennbar sein können.
9. Concurrent Updates MÜSSEN über definierte Synchronisationsmechanismen koordiniert werden.
10. Mehrteilige State Changes SOLLEN Transactions verwenden.
11. State Observation DARF keine Änderungsberechtigung erzeugen.
12. Cached State DARF NICHT automatisch als aktuell betrachtet werden.
13. Derived State MUSS von autoritativem State unterscheidbar sein.
14. Distributed State DARF NICHT universelle sofortige Konsistenz voraussetzen.
15. Das jeweilige Subsystem MUSS sein Consistency Model definieren können.
16. Security State MUSS gegen veraltete oder unautorisierte Änderungen geschützt werden.
17. Monotone Security States DÜRFEN durch Rollback NICHT unzulässig zurückgesetzt werden.
18. Unknown State MUSS explizit repräsentierbar sein.
19. Unknown State DARF NICHT automatisch als gültig interpretiert werden.
20. Recovery MUSS State nach Wiederherstellung validieren.
21. Live Replacement MUSS State Compatibility getrennt von ABI Compatibility behandeln.
22. Deterministische Modi MÜSSEN relevante State Transitions reproduzierbar machen können.
23. Kritische State Changes SOLLEN Provenance besitzen.
24. Global State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0153`

## Ergebnis

```text
Desired State
      ↓
Validate
      ↓
Transaction / Transition
      ↓
Authoritative State Owner
      ↓
Actual State
      ↓
Observe
      ↓
Verify
      ↓
Reconcile
```

NovaOS erhält damit ein einheitliches Global-State-Modell, bei dem systemweiter Zustand nicht durch unkontrollierte globale Variablen, sondern durch identifizierte, verantwortete, versionierte, transaktionale und beobachtbare Teilzustände gebildet wird.