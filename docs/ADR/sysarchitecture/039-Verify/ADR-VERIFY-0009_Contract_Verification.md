# ADR-VERIFY-0009 – Contract Verification

## Status

Angenommen

## Kategorie

Verification / Contracts / Execution / Correctness

## Kontext

NovaOS verwendet explizite Contracts, um Anforderungen zwischen Komponenten, Capabilities und Execution Providern zu beschreiben.

Ein Contract kann unter anderem enthalten:

```text
Input / Output Types
Permissions
Resource Budgets
Latency / Deadline
Determinism
Trust Requirements
Data Sovereignty
Algorithm Constraints
```

Ein Contract ist nur sinnvoll, wenn überprüft werden kann, ob seine Anforderungen tatsächlich erfüllt werden.

## Entscheidung

NovaOS behandelt **Contract Verification als verbindlichen Bestandteil der Contract-Ausführung**.

```text
Contract
   ↓
Validate
   ↓
Plan
   ↓
Execute
   ↓
Verify
```

Contracts werden sowohl vor als auch – soweit erforderlich – während und nach der Ausführung überprüft.

## Pre-Execution Verification

Vor der Ausführung wird geprüft, ob ein geplanter Provider die Anforderungen des Contracts erfüllen kann.

```text
Contract Requirements
        ↓
Provider Properties
        ↓
Verification
        ↓
Accept / Reject
```

Nicht nachweisbare Hard Requirements gelten als nicht erfüllt.

## Runtime Verification

Dynamische Contract-Eigenschaften können während der Ausführung überwacht werden.

Beispiele:

```text
Resource Budget
Deadline
Data Location
Provider Identity
Execution State
```

Die Überwachung muss entsprechend der Kritikalität des Contracts erfolgen.

## Post-Execution Verification

Nach der Ausführung können überprüfbare Ergebnisbedingungen validiert werden.

```text
Execution
   ↓
Result
   ↓
Output / State Validation
```

Dazu gehören insbesondere Output-Typen, Zustandsänderungen und definierte Postconditions.

## Hard und Soft Requirements

Contract Verification unterscheidet zwischen:

```text
Hard Requirement
Soft Requirement
```

Hard Requirements müssen erfüllt sein.

Soft Requirements dürfen für Planung und Optimierung verwendet werden, können aber innerhalb definierter Regeln verletzt oder angepasst werden.

```text
Hard Constraints
      ↓
Valid Solutions
      ↓
Soft Optimization
```

## Contract Composition

Werden mehrere Capabilities kombiniert, müssen deren Contracts kompatibel sein.

```text
Contract A
    ↓
Contract B
    ↓
Contract C
```

Eine End-to-End-Garantie darf nicht stärker sein als die Garantien ihrer notwendigen Abhängigkeiten.

## Verification Result

Das Ergebnis einer Contract Verification muss eindeutig sein.

```text
Satisfied
Violated
Unknown
```

`Unknown` darf bei Hard Requirements nicht als `Satisfied` interpretiert werden.

## Contract Violation

Bei einer Verletzung muss NovaOS entsprechend der Contract-Policy reagieren können.

```text
Reject
Abort
Rollback
Degrade
Replan
Recover
Safe State
```

Eine Verletzung darf nicht stillschweigend ignoriert werden.

## Normative Anforderungen

1. NovaOS MUSS Execution Contracts überprüfbar machen.
2. Hard Requirements MÜSSEN vor der Ausführung geprüft werden, soweit dies erforderlich und möglich ist.
3. Nicht nachweisbare Hard Requirements MÜSSEN als `Unknown` oder nicht erfüllt behandelt werden.
4. Dynamische Contract-Eigenschaften MÜSSEN zur Laufzeit überprüfbar sein können.
5. Definierte Postconditions SOLLEN nach der Ausführung geprüft werden.
6. Contract Composition MUSS Abhängigkeiten und deren Garantien berücksichtigen.
7. Soft Requirements DÜRFEN Hard Requirements niemals überschreiben.
8. Contract Violations MÜSSEN explizit erkennbar sein.
9. Verification Results MÜSSEN der jeweiligen Contract-, Provider- und Komponenten-Version zuordenbar sein.

## Konsequenzen

### Positiv

- Contracts werden zu überprüfbaren Systemgarantien,
- ungeeignete Provider können vor der Ausführung ausgeschlossen werden,
- Verletzungen werden während der Ausführung erkennbar,
- Execution Planning erhält eine verlässliche Entscheidungsgrundlage.

### Negativ

- Contract Verification erzeugt zusätzlichen Prüfaufwand,
- manche Eigenschaften können erst zur Laufzeit vollständig bestimmt werden,
- zusammengesetzte Garantien erhöhen die Komplexität.

## Abhängigkeiten

- `ADR-VERIFY-0008_Static_und_Runtime_Verification`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatic_Execution_Planning`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Ergebnis

NovaOS behandelt Contracts nicht nur als Beschreibung von Anforderungen:

```text
Contract
   ↓
Verification
   ↓
Enforced Execution
```

Eine zugesicherte Systemeigenschaft gilt nur dann als erfüllt, wenn NovaOS sie mit der für diese Eigenschaft erforderlichen Verifikation begründen kann.