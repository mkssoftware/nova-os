# NPSPEC-DISTRIBUTED-TRANSACTION-0001 – Nova Distributed Transaction

## Status

Angenommen

## Kategorie

Distributed / Transaction / State Coordination

## Zweck

NovaOS definiert ein kontrolliertes Transaktionsmodell für Operationen, die mehrere Nodes, Objekte, Ressourcen oder Dienste betreffen.

Verteilte Transaktionen sollen konsistente Zustandsänderungen ermöglichen, ohne eine universelle globale ACID-Transaktion für das gesamte System vorauszusetzen.

```text
Distributed Operation
        ↓
Transaction Scope
        ↓
Participants
        ↓
Prepare
        ↓
Commit / Abort
        ↓
Verify
```

## Grundprinzipien

```text
Transaction ≠ Consistency Model
Transaction ≠ Replication
Transaction ≠ Distributed Lock
Transaction ≠ Exactly-Once
Transaction ≠ Automatic Rollback
Local Atomicity ≠ Global Atomicity
Timeout ≠ Abort
Network Failure ≠ Transaction Failure
```

## Transaction Model

Eine verteilte Transaktion wird beschrieben durch:

```text
DistributedTransaction
├── TransactionID
├── Coordinator
├── Participants
├── Scope
├── Policy
└── State
```

Optional:

```text
ExecutionID
Deadline
Isolation Requirements
Consistency Requirements
Resource Reservations
Compensation Policy
Recovery Information
Trust Requirements
Sovereignty Constraints
```

## Transaction Scope

Der Scope muss explizit begrenzt sein.

Beispiele:

```text
Objects
Storage Operations
Metadata
Service State
Resource Reservations
Distributed Tasks
```

NovaOS soll keine globale Systemtransaktion erzeugen, wenn eine kleinere Transaktionsgrenze ausreichend ist.

## Transaction Lifecycle

Das bevorzugte Grundmodell lautet:

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Commit
  ↓
Verify
```

Bei Fehlern:

```text
Prepare
   ↓
Abort / Compensate / Recover
```

## Transaction States

Mindestens folgende Zustände müssen unterscheidbar sein:

```text
Created
Validating
Preparing
Prepared
Committing
Committed
Aborting
Aborted
Compensating
Failed
Unknown
```

Dabei gilt:

```text
Unknown ≠ Aborted
Unknown ≠ Committed
```

## Participants

Teilnehmer können sein:

```text
Storage Node
Service
Object Provider
Resource Manager
Remote Task
Cluster Node
```

Jeder Teilnehmer bleibt eine eigenständige Security- und Failure-Domain.

## Coordinator

Ein Coordinator steuert den Transaktionsablauf.

```text
Coordinator
├── Participant A
├── Participant B
└── Participant C
```

Der Coordinator erhält dadurch keine universelle Authority.

Er darf ausschließlich mit den für die Transaktion delegierten Capabilities arbeiten.

## Prepare

Während `Prepare` prüfen Teilnehmer:

```text
Capabilities
Current Version
Constraints
Resources
Trust
Sovereignty
Local Transaction Feasibility
```

Ein erfolgreicher Prepare-Schritt bedeutet noch keinen Commit.

```text
Prepared ≠ Committed
```

## Commit

Commit darf erst erfolgen, wenn die verwendete Transaction Policy dies zulässt.

```text
Prepared Participants
        ↓
Commit Decision
        ↓
Commit
```

Die Commit-Entscheidung muss eindeutig identifizierbar und bei Bedarf rekonstruierbar sein.

## Abort

Solange die Transaction Policy einen Abort zulässt:

```text
Transaction
    ↓
Abort
    ↓
Participant Cleanup
```

Reservierungen und temporäre Capabilities müssen anschließend freigegeben oder widerrufen werden können.

## Partial Failure

Verteilte Systeme können während Commit oder Kommunikation ausfallen.

```text
Participant A → Committed
Participant B → Unknown
Participant C → Unreachable
```

NovaOS darf diesen Zustand nicht als vollständigen Erfolg oder vollständigen Abort darstellen.

## Unknown Transaction State

Bei Verbindungsverlust:

```text
Commit Sent
    ↓
Connection Lost
    ↓
Unknown
```

Ein Retry des Commit darf nur erfolgen, wenn das verwendete Protokoll und die Operation dies sicher erlauben.

```text
Retry ≠ Safe
```

## Idempotency

Transaktionale Operationen sollen, wo möglich, idempotente Identifikatoren verwenden.

```text
TransactionID
   +
OperationID
```

Dadurch können wiederholte Nachrichten erkannt werden.

Idempotency ersetzt jedoch keine Transaction Coordination.

## Compensation

Nicht jede Operation kann technisch zurückgerollt werden.

Für solche Operationen kann NovaOS Compensation verwenden.

```text
Operation
   ↓
Committed Effect
   ↓
Compensating Operation
```

```text
Compensation ≠ Rollback
```

Eine Compensation erzeugt eine neue Zustandsänderung, die den vorherigen Effekt semantisch ausgleicht.

## Isolation

Distributed Transactions können Isolation Requirements definieren.

Die konkrete Isolation hängt vom beteiligten Subsystem ab.

NovaOS darf keine stärkere Isolation behaupten, als tatsächlich gewährleistet wird.

## Consistency

Transaction und Distributed Consistency werden getrennt modelliert.

```text
Transaction
    +
Consistency Policy
    ↓
Defined State Semantics
```

Ein erfolgreicher Commit bedeutet nicht automatisch, dass alle Replicas sofort denselben Zustand besitzen.

## Replication

Replikation eines transaktional geänderten Objekts folgt der definierten Replication- und Consistency-Policy.

```text
Transaction Commit
      ↓
New Version
      ↓
Replication
      ↓
Replica Convergence
```

## Deadlines

Distributed Transactions können Deadlines besitzen.

```text
Transaction Deadline
├── Prepare Budget
├── Commit Budget
└── Recovery Budget
```

Eine überschrittene Deadline bedeutet nicht automatisch, dass keine Operation ausgeführt wurde.

## Capabilities

Jeder Teilnehmer benötigt minimale explizite Authority.

```text
Transaction
      ↓
Required Operations
      ↓
Attenuated Capabilities
      ↓
Participants
```

Nach Abschluss sollen temporäre Capabilities widerrufen oder freigegeben werden.

## Trust und Sovereignty

Alle Teilnehmer müssen relevante Anforderungen erfüllen.

```text
Participant
   ↓
Trust Check
   ↓
Sovereignty Check
   ↓
Eligible
```

Ein laufender Trust- oder Sovereignty-Verstoß kann:

```text
Abort
Block
Replan
Compensate
Fail
```

auslösen, soweit der aktuelle Transaction State dies zulässt.

## Recovery

Nach Node-, Coordinator- oder Netzwerkfehlern muss der Zustand rekonstruierbar sein.

```text
Recover
   ↓
TransactionID
   ↓
Participant States
   ↓
Commit Decision
   ↓
Continue / Abort / Compensate
```

Recovery darf keine Commit-Entscheidung raten.

## Logging und Durability

Kritische Transaction States können dauerhaft protokolliert werden.

Beispiele:

```text
Prepared
Commit Decision
Committed
Aborted
```

Logs dürfen keine Capability Secrets enthalten.

## Distributed Execution

Execution Contracts können eine Distributed Transaction anfordern.

```text
ExecutionContract
      ↓
Transactional Scope
      ↓
Distributed Transaction
      ↓
Execution
```

Nicht transaktionale Operationen dürfen nicht automatisch als rollbackfähig behandelt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TransactionID
State
Coordinator
Participants
Scope
Deadline
Prepared Participants
Commit Decision
Consistency Requirements
Recovery State
Compensation State
```

## Normative Anforderungen

1. NovaOS MUSS Distributed Transactions als explizit begrenzte Koordinationsmechanismen modellieren.
2. Distributed Transactions DÜRFEN NICHT als universelle globale ACID-Schicht vorausgesetzt werden.
3. Transaction Scope und Participants MÜSSEN explizit bestimmbar sein.
4. `Prepared` DARF NICHT als `Committed` behandelt werden.
5. `Unknown` DARF NICHT automatisch als `Committed` oder `Aborted` interpretiert werden.
6. Netzwerkfehler DÜRFEN NICHT automatisch als Transaction Abort interpretiert werden.
7. Coordinator-Rollen DÜRFEN keine universelle Authority erzeugen.
8. Teilnehmer MÜSSEN ausschließlich notwendige Capabilities erhalten.
9. Retries MÜSSEN Transaction State und Idempotency berücksichtigen.
10. Compensation MUSS von Rollback unterschieden werden.
11. NovaOS DARF keine stärkere Isolation oder Atomicity behaupten, als tatsächlich gewährleistet wird.
12. Transaction und Consistency MÜSSEN getrennte Konzepte bleiben.
13. Replication MUSS die resultierende Transaction-Version eindeutig behandeln können.
14. Deadlines DÜRFEN NICHT automatisch als bestätigter Abort interpretiert werden.
15. Recovery MUSS bestehende Commit-Entscheidungen rekonstruieren können.
16. Trust- und Sovereignty-Anforderungen MÜSSEN während der Transaktion erhalten bleiben.
17. Capability Secrets DÜRFEN NICHT in Transaction Logs gespeichert werden.
18. Transaction-, Participant-, Commit- und Recovery-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `ADR-ARCH-0068`

## Ergebnis

```text
Distributed Operation
        ↓
Bounded Transaction Scope
        ↓
Validate Participants
        ↓
Prepare
        ↓
Commit Decision
        ↓
Commit / Abort
        ↓
Verify
        ↓
Recovery / Compensation
```

NovaOS erhält damit ein begrenztes und fehlertolerantes Distributed-Transaction-Modell, das atomare Koordination dort ermöglicht, wo sie erforderlich ist, ohne das gesamte verteilte System von globalen Transaktionen abhängig zu machen.