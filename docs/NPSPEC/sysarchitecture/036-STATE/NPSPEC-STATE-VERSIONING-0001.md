# NPSPEC-STATE-VERSIONING-0001 – Nova State Versioning

## Status

Angenommen

## Kategorie

State / Versioning / Consistency

## Zweck

NovaOS definiert ein einheitliches Versionierungsmodell für veränderlichen Systemzustand.

State Versioning ermöglicht:

```text
Change Detection
Conflict Detection
Stale-State Detection
Transactions
Snapshots
Reconciliation
Recovery
Replay
Live Evolution
```

Jede relevante Zustandsänderung kann dadurch eindeutig einer Version oder Generation zugeordnet werden.

## Grundprinzipien

```text
State Version ≠ API Version
State Version ≠ Object Identity
State Version ≠ Timestamp
Version Number ≠ State Content
Newer Version ≠ Valid State
Same Version ≠ Same Object
Observed Version ≠ Current Version
Rollback ≠ Version Reuse
```

## State Version Model

```text
StateVersion
├── StateID
├── Version
├── Generation
└── Validity
```

Optional:

```text
PreviousVersion
TransactionID
SnapshotID
Timestamp
SourceID
ProvenanceID
IntegrityState
```

## State Identity

Versionierung verändert nicht die Identität des Zustands.

```text
StateID = A

A:v1
A:v2
A:v3
```

Alle Versionen gehören weiterhin zum selben logischen State.

```text
StateID ≠ StateVersion
```

## Version Progression

Versionen müssen innerhalb ihres definierten Kontexts eindeutig geordnet werden können.

```text
v10
 ↓
v11
 ↓
v12
```

Eine erfolgreich veröffentlichte neue State-Version darf eine bereits verwendete Versionskennung nicht erneut verwenden.

## Generation

Eine Generation kann größere Lifecycle-Grenzen kennzeichnen.

```text
Generation 4
├── v1
├── v2
└── v3

Restart / Recreate
       ↓

Generation 5
├── v1
└── v2
```

Damit können Versionen auch bei neu erzeugten Instanzen eindeutig unterschieden werden.

## Optimistic Concurrency

State Versioning ermöglicht Compare-and-Update.

```text
Read State v20
      ↓
Prepare Change
      ↓
Current Version == v20?
├── Yes → Apply → v21
└── No  → Conflict
```

Veraltete Änderungen dürfen nicht unbemerkt neueren State überschreiben.

## Reconciliation

Desired und Actual State besitzen unabhängige Versionen.

```text
Desired v8
Actual v15
```

Ein Reconciliation Plan muss festhalten, auf welchen Versionen er basiert.

```text
Plan
├── DesiredVersion = 8
└── ActualVersion = 15
```

Ändert sich eine relevante Version, kann Revalidation oder Replanning erforderlich sein.

## Transactions

Transactions können erwartete State-Versionen erfassen.

```text
Begin
  ↓
Read v30
  ↓
Prepare
  ↓
Validate Current == v30
  ↓
Commit → v31
```

```text
Prepared Version ≠ Committed Version
```

## Snapshots

Snapshots referenzieren konkrete State-Versionen.

```text
Snapshot S1
    ↓
State A:v42
```

Damit bleibt nachvollziehbar, welchen Zustand ein Snapshot repräsentiert.

## Rollback

Rollback erzeugt grundsätzlich eine neue aktuelle Version.

```text
v40
 ↓
v41
 ↓
v42
 ↓
Rollback to content of v40
 ↓
v43
```

Nicht:

```text
v42 → v40
```

Dadurch bleibt die tatsächliche Historie erhalten.

## Security

State Versioning darf Sicherheitszustände nicht zurücksetzen.

Insbesondere:

```text
Capability Revocation
Trust Revocation
Minimum Security Version
Compromised Key State
Security Counter
Rollback Protection
```

dürfen nicht durch Wiederherstellung alter State-Inhalte unzulässig rückgängig gemacht werden.

## Distributed State

In verteilten Systemen existiert nicht zwingend eine einzige globale Versionsfolge.

```text
Node A → Version A
Node B → Version B
Node C → Version C
```

Je nach Consistency Model können verwendet werden:

```text
Generation Numbers
Logical Clocks
Version Vectors
Transaction Versions
Consensus Indexes
```

```text
Higher Local Version ≠ Globally Newer State
```

## Derived State

Abgeleiteter State muss seine Quellversionen referenzieren können.

```text
Derived State D
├── Source A:v10
└── Source B:v27
```

Ändert sich eine Quelle, kann D als stale markiert oder neu berechnet werden.

## Cached State

Caches müssen die Version des zugrunde liegenden States speichern können.

```text
Cache Entry
├── StateID
├── Version
└── Value
```

```text
Cached v12
Current v14
     ↓
Stale
```

## Unknown Version

Kann die aktuelle Version nicht zuverlässig bestimmt werden:

```text
Version = Unknown
```

Unknown darf nicht als kompatibel oder aktuell angenommen werden.

## Live Evolution

State Migration kann Versionstransformationen benötigen.

```text
State Schema v2
      ↓
Migration
      ↓
State Schema v3
```

Dabei müssen State-Content-Version und State-Schema-Version getrennt behandelbar sein.

```text
State Version ≠ State Schema Version
```

## Provenance

Relevante Versionswechsel sollen nachvollziehbar sein.

```text
StateID
PreviousVersion
NewVersion
Actor
Operation
TransactionID
Timestamp
Reason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
StateID
Current Version
Generation
Previous Version
Validity
Source
Transaction
Snapshot References
Derived Dependencies
Last Transition
```

## Normative Anforderungen

1. NovaOS MUSS relevante Systemzustände versionieren können.
2. State Identity und State Version MÜSSEN getrennte Konzepte bleiben.
3. State Version und API-/ABI-Version MÜSSEN getrennt bleiben.
4. Veröffentlichte Versionen MÜSSEN innerhalb ihres Kontexts eindeutig sein.
5. Bereits verwendete Versionskennungen DÜRFEN NICHT für neue Zustände wiederverwendet werden.
6. Generationen MÜSSEN größere Lifecycle-Grenzen darstellen können.
7. State Versioning MUSS Stale-State-Erkennung ermöglichen.
8. Concurrent Updates MÜSSEN Versionskonflikte erkennen können.
9. Veraltete Änderungen DÜRFEN neueren State NICHT unbemerkt überschreiben.
10. Reconciliation Plans MÜSSEN ihre zugrunde liegenden State-Versionen referenzieren können.
11. Transactions MÜSSEN erwartete State-Versionen validieren können.
12. Snapshots MÜSSEN konkrete State-Versionen referenzieren können.
13. Rollback SOLL eine neue State-Version erzeugen.
14. Rollback DARF historische Versionskennungen NICHT als neue aktuelle Version wiederverwenden.
15. State Versioning DARF monotone Security States NICHT unzulässig zurücksetzen.
16. Distributed State DARF keine universelle globale Versionsfolge voraussetzen.
17. Das jeweilige Consistency Model MUSS geeignete Versionierungsmechanismen definieren können.
18. Derived State SOLL seine Quellversionen referenzieren.
19. Cached State MUSS seine zugrunde liegende Version speichern können.
20. Unknown Version DARF NICHT automatisch als aktuell interpretiert werden.
21. State Version und State Schema Version MÜSSEN getrennt behandelbar sein.
22. Live Evolution MUSS State-Versionstransformationen unterstützen können.
23. Kritische Versionswechsel SOLLEN Provenance besitzen.
24. State-Versionen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-RECONCILIATION-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `ADR-ARCH-0159`

## Ergebnis

```text
StateID
   ↓
Current Version
   ↓
Observe / Modify
   ↓
Validate Expected Version
   ↓
Transition / Transaction
   ↓
Create New Version
   ↓
Publish
   ↓
Provenance + Introspection
```

NovaOS erhält damit ein einheitliches State-Versionierungsmodell, das Änderungen eindeutig nachvollziehbar macht, veraltete Zustände erkennt, konkurrierende Änderungen absichert und eine gemeinsame Grundlage für Reconciliation, Transactions, Snapshots, Rollback, Recovery und Live Evolution bildet.