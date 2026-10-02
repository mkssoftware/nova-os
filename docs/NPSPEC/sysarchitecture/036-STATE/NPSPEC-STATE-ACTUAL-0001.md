# NPSPEC-STATE-ACTUAL-0001 – Nova Actual State

## Status

Angenommen

## Kategorie

State / Actual State / System Model

## Zweck

NovaOS definiert Actual State als beobachtbaren tatsächlichen Zustand eines Systems, Subsystems, Objekts, Dienstes oder einer Ressource.

```text
Desired State
     ↓
Compare
     ↑
Actual State
```

Actual State bildet die Grundlage für Reconciliation, Self-Healing, Monitoring und kontrollierte Systementscheidungen.

## Grundprinzipien

```text
Actual State ≠ Desired State
Actual State ≠ Cached State
Actual State ≠ Reported State
Actual State ≠ Assumed State
Observed State ≠ Guaranteed Current State
Unknown ≠ Healthy
Unavailable ≠ Failed
```

## Actual State Model

```text
ActualState
├── StateID
├── StateType
├── OwnerID
├── Version
├── ObservedValue
├── Validity
└── ObservationTime
```

Optional:

```text
SourceID
Generation
Confidence
Location
TransactionID
ProvenanceID
SecurityState
TrustState
Dependencies
```

## State Sources

Actual State kann aus unterschiedlichen autoritativen Quellen stammen:

```text
Kernel
Driver
Service
Device
Storage
Object
Runtime
Remote Node
```

Die Quelle muss eindeutig identifizierbar sein.

## Observation

Actual State entsteht durch Beobachtung des realen Systemzustands.

```text
System
   ↓
Observe
   ↓
Actual State
```

Eine Beobachtung besitzt einen Zeitpunkt und kann veralten.

```text
Observed at T1 ≠ Guaranteed State at T2
```

## Authoritative State

NovaOS muss zwischen autoritativem und abgeleitetem Zustand unterscheiden.

```text
Authoritative State
        ↓
Observation
        ↓
Derived / Cached Views
```

Eine gecachte oder abgeleitete Darstellung darf nicht automatisch als autoritative Wahrheit behandelt werden.

## Versioning

Actual State soll versioniert oder über Generationen unterscheidbar sein.

```text
Actual v42
    ↓
Transition
    ↓
Actual v43
```

Dadurch können veraltete Beobachtungen und konkurrierende Änderungen erkannt werden.

## Validity

Actual State kann einen expliziten Gültigkeitszustand besitzen:

```text
Valid
Stale
Invalid
Unavailable
Unknown
```

```text
Stale ≠ Invalid
Unavailable ≠ Failed
Unknown ≠ Valid
```

## Desired-State-Vergleich

Actual State wird mit Desired State verglichen.

```text
Desired
   ↓
Compare
   ↑
Actual
```

Ergebnis:

```text
Satisfied
Drifted
Partially Satisfied
Blocked
Unknown
```

Eine Abweichung wird als State Drift behandelt.

## State Drift

Beispiele für Ursachen:

```text
Failure
External Change
Manual Change
Resource Loss
Provider Change
Capability Revocation
Configuration Change
Hardware Change
Distributed Update
```

Drift bedeutet nicht automatisch einen Fehler.

Die Reconciliation Policy entscheidet über die notwendige Reaktion.

## State Transitions

Änderungen des Actual State müssen als Zustandsübergänge beobachtbar sein.

```text
State A
   ↓
Transition
   ↓
State B
```

Relevante Transitionen sollen Ursache und Provenance besitzen.

## Transactions

Bei transaktionalen Änderungen muss zwischen Zwischenzustand und committed Actual State unterschieden werden.

```text
Old Actual State
      ↓
Transaction
      ↓
Prepared State
      ↓
Commit
      ↓
New Actual State
```

```text
Prepared ≠ Committed
Committed ≠ Verified
```

## Concurrent Changes

Actual State kann sich während einer Operation ändern.

```text
Observe v10
    ↓
Plan
    ↓
Actual becomes v11
    ↓
Revalidate
```

Kritische Änderungen müssen vor Commit oder Ausführung erneut validiert werden können.

## Distributed Actual State

In verteilten Systemen kann es unterschiedliche Beobachtungen geben.

```text
Node A → State v10
Node B → State v11
Node C → Unknown
```

NovaOS setzt keine universelle sofortige globale Sicht voraus.

Das jeweilige Consistency Model bestimmt die Interpretation.

## Security

Actual State darf keine Authority erzeugen.

```text
Observed Resource
      ≠
Permission to modify Resource
```

Sicherheitskritische Zustände müssen gegen manipulierte, veraltete oder nicht vertrauenswürdige Quellen geschützt werden.

## Trust

Bei externem oder entferntem State muss die Vertrauenswürdigkeit der Quelle berücksichtigt werden.

```text
Reported State
     ↓
Identity
     ↓
Trust / Attestation
     ↓
Accepted Observation
```

```text
Reported ≠ Trusted
```

## Failure und Unknown State

Kann der tatsächliche Zustand nicht zuverlässig bestimmt werden:

```text
Actual State = Unknown
```

NovaOS darf daraus nicht automatisch ableiten:

```text
Healthy
Failed
Stopped
Completed
```

Die jeweilige Sicherheits- und Recovery-Policy bestimmt das Verhalten.

## Self-Healing

Self-Healing verwendet Actual State zur Erkennung von Abweichungen.

```text
Observe Actual
      ↓
Compare Desired
      ↓
Drift Detected
      ↓
Diagnose
      ↓
Repair
      ↓
Observe Again
      ↓
Verify
```

## Event Integration

Änderungen können Events erzeugen:

```text
Actual State Change
       ↓
State Event
       ↓
Subscribers
```

Events dienen der Beobachtung.

Sie ersetzen nicht die erneute Prüfung des autoritativen Zustands.

```text
Event ≠ Current State
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
StateID
StateType
ObservedValue
Version
Validity
ObservationTime
Source
Owner
Desired State
Drift
Dependencies
Transaction State
Trust State
```

## Normative Anforderungen

1. NovaOS MUSS Actual State getrennt von Desired State modellieren.
2. Actual State MUSS aus beobachtbarem Systemzustand ableitbar sein.
3. Die Quelle eines relevanten Actual State MUSS identifizierbar sein.
4. Autoritativer, gecachter und abgeleiteter State MÜSSEN unterscheidbar sein.
5. Beobachtungen MÜSSEN einen zeitlichen Bezug besitzen können.
6. Veraltete Beobachtungen MÜSSEN erkennbar sein können.
7. Actual State SOLL versionierbar oder generationierbar sein.
8. Valid, Stale, Invalid, Unavailable und Unknown MÜSSEN unterscheidbar sein können.
9. Unknown DARF NICHT automatisch als Healthy oder Failed interpretiert werden.
10. Actual State MUSS mit Desired State vergleichbar sein.
11. State Drift MUSS explizit erkennbar sein.
12. Drift DARF NICHT automatisch als Systemfehler interpretiert werden.
13. Relevante State Transitions SOLLEN nachvollziehbar sein.
14. Prepared State DARF NICHT als committed Actual State interpretiert werden.
15. Committed State DARF NICHT automatisch als verifiziert gelten.
16. Concurrent State Changes MÜSSEN erkennbar sein können.
17. Kritische Operationen MÜSSEN Actual State revalidieren können.
18. Distributed Actual State DARF keine universelle sofortige Konsistenz voraussetzen.
19. Actual State DARF keine Authority erzeugen.
20. Nicht vertrauenswürdige State Sources MÜSSEN validierbar sein.
21. Self-Healing MUSS Actual State zur Drift-Erkennung verwenden können.
22. State Events DÜRFEN NICHT automatisch als aktueller State interpretiert werden.
23. Actual State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0155`

## Ergebnis

```text
System / Resource / Object
          ↓
       Observe
          ↓
     Actual State
          ↓
Validate Source + Freshness
          ↓
Compare with Desired State
          ↓
Drift?
├── No  → Maintain / Monitor
├── Yes → Reconcile
└── Unknown → Validate / Recover
          ↓
        Verify
```

NovaOS erhält damit ein explizites Actual-State-Modell, das den tatsächlich beobachteten Systemzustand von Desired State, Cache, Annahmen und bloßen Statusmeldungen trennt und dadurch eine zuverlässige Grundlage für Reconciliation, Self-Healing und Systementscheidungen bildet.