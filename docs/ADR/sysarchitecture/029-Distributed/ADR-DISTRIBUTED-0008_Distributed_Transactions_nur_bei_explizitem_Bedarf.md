# ADR-DISTRIBUTED-0008 – Distributed Transactions nur bei explizitem Bedarf

## Status

Angenommen

## Kategorie

Distributed Architecture / Transactions

## Kontext

Transaktionen über mehrere Nodes oder Services erhöhen die Komplexität erheblich.

Sie erzeugen zusätzliche Anforderungen an:

```text
Coordination
Failure Handling
Recovery
Consistency
Availability
Latency
```

NovaOS soll deshalb lokale Operationen nicht automatisch in globale Distributed Transactions verwandeln.

## Entscheidung

NovaOS verwendet **Distributed Transactions nur bei explizitem semantischem Bedarf**.

```text
Operation
   ↓
Local möglich?
 ├── Yes → Local Transaction
 └── No
      ↓
Distributed Atomicity Required?
 ├── No  → Independent Operations / Reconciliation
 └── Yes → Distributed Transaction
```

Lokale Transaktionen bleiben der bevorzugte Standard.

## Expliziter Bedarf

Eine Distributed Transaction darf verwendet werden, wenn der Contract eine gemeinsame atomare Zustandsänderung über mehrere unabhängige Teilnehmer verlangt.

Beispiel:

```text
Participant A
      +
Participant B
      +
Participant C
      ↓
Atomic Commit Required
```

Der Bedarf muss aus der Operationssemantik oder einem expliziten Contract hervorgehen.

## Kein globales ACID

NovaOS definiert kein universelles globales ACID-Modell für alle verteilten Operationen.

```text
Distributed System
       ≠
One Global Transaction
```

Services und Ressourcen bleiben grundsätzlich unabhängig.

## Bevorzugte Alternativen

Wenn globale Atomarität nicht erforderlich ist, werden bevorzugt verwendet:

```text
Local Transactions
Idempotent Operations
Reconciliation
Compensation
Versioning
Desired State
```

Dadurch bleiben Failure Domains möglichst unabhängig.

## Participants

Jeder Teilnehmer einer Distributed Transaction muss seine Transaktionssemantik explizit unterstützen.

```text
Coordinator
   ├── Participant A
   ├── Participant B
   └── Participant C
```

Ein nicht transaktionsfähiger Teilnehmer darf nicht stillschweigend als atomarer Teilnehmer behandelt werden.

## Partial Failures

Distributed Transactions müssen Partial Failures als normalen Fehlerfall behandeln.

```text
A = Prepared
B = Committed
C = Unknown
```

`Unknown` ist ein eigenständiger Zustand und darf nicht automatisch als `Failed` interpretiert werden.

## Commit Evidence

Ein erfolgreicher Commit muss durch belastbare Commit Evidence bestimmbar sein.

```text
Commit Requested
      ↓
Durable Evidence
      ↓
Committed
```

Lokaler Erfolg eines einzelnen Teilnehmers beweist keinen global erfolgreichen Commit.

## Irreversible Operations

Irreversible Operationen dürfen nicht unkontrolliert Bestandteil einer atomaren Distributed Transaction werden.

```text
Reversible Phase
      ↓
Rollback Barrier
      ↓
Irreversible Operation
```

Für solche Operationen müssen Compensation oder Reconciliation explizit definiert werden.

## Timeout und Netzwerkfehler

Ein Timeout bedeutet nicht automatisch:

```text
Transaction = Failed
```

Sondern kann bedeuten:

```text
Transaction = Unknown
```

Der Zustand muss über Recovery-, Query- oder Reconciliation-Mechanismen geklärt werden.

## Normative Anforderungen

1. NovaOS DARF Distributed Transactions nur bei explizitem semantischem Bedarf verwenden.
2. Lokale Transaktionen MÜSSEN bevorzugt werden, wenn sie ausreichend sind.
3. NovaOS DARF kein universelles globales ACID-Modell voraussetzen.
4. Teilnehmer MÜSSEN ihre Distributed-Transaction-Semantik explizit deklarieren.
5. Partial Failures MÜSSEN unterstützt werden.
6. `Unknown` MUSS von `Failed` und `Committed` unterschieden werden.
7. Globaler Commit MUSS durch geeignete Commit Evidence bestimmbar sein.
8. Timeouts DÜRFEN nicht automatisch als Rollback oder Failure interpretiert werden.
9. Irreversible Operationen MÜSSEN explizite Compensation-, Barrier- oder Reconciliation-Semantik besitzen.
10. Reconciliation und Compensation SOLLEN bevorzugt werden, wenn globale Atomarität nicht erforderlich ist.

## Konsequenzen

### Positive Konsequenzen

- Distributed Transactions bleiben auf notwendige Fälle begrenzt,
- lokale Failure Domains bleiben stärker unabhängig,
- Verfügbarkeit und Skalierbarkeit werden weniger durch globale Koordination eingeschränkt,
- Reconciliation kann viele verteilte Zustandsänderungen einfacher behandeln.

### Negative Konsequenzen

- nicht atomare Workflows benötigen Compensation oder Reconciliation,
- echte Distributed Transactions benötigen komplexe Recovery-Semantik.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0007_Replication_und_Consistency_Profiles`
- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0005_Multi_Service_Transactions`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-DECLARATIVE-0002_State_Reconciliation`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-PARTICIPANT-0001`
- `NPSPEC-DISTRIBUTED-COMMIT-EVIDENCE-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-RECOVERY-0001`

## Ergebnis

NovaOS verwendet verteilte Atomarität nur dort, wo sie tatsächlich Bestandteil des Contracts ist:

```text
Distributed Operation
        ↓
Atomicity Required?
   ┌────┴────┐
   No       Yes
   ↓         ↓
Reconcile  Distributed
Compensate Transaction
   ↓         ↓
Verified Final State
```

Der zentrale Grundsatz lautet:

```text
Nicht jede verteilte Operation
braucht eine verteilte Transaktion.

Globale Atomarität wird nur verwendet,
wenn die Semantik sie wirklich verlangt.
```