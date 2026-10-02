# NPSPEC-TRANSACTION-RESOURCE-0001 – Nova Transactional Resource Management

## Status

Angenommen

## Kategorie

Transaction / Resource Management / Resource Economy

## Zweck

NovaOS definiert Ressourcenänderungen und -reservierungen als transaktionale Operationen. Ressourcen werden zunächst geprüft und vorbereitet, bevor ihre Zuweisung verbindlich aktiviert wird.

```text
Resource Request
      ↓
Validate
      ↓
Reserve
      ↓
Prepare
      ↓
Commit
      ↓
Activate
      ↓
Verify
```

Dadurch sollen partielle Ressourcenvergaben, Leaks, Überbuchung und inkonsistente Zustände vermieden werden.

## Grundprinzipien

```text
Reservation ≠ Allocation
Allocation ≠ Ownership
Available ≠ Reserved
Reserved ≠ Committed
Committed ≠ Usable
Released ≠ Reclaimed
Resource Transaction ≠ Authority
```

## Resource Transaction Model

```text
ResourceTransaction
├── TransactionID
├── OwnerID
├── ResourceSet
├── Requirements
├── Reservations
├── ResourceBudget
└── State
```

Optional:

```text
ExecutionContractID
ParentTransactionID
Deadline
Priority
Guarantee
RecoveryPolicy
ProviderID
FailureDomainID
ProvenanceID
```

## Zustände

```text
Created
Validating
Reserving
Reserved
Preparing
Prepared
Committing
Committed
Activating
Active
Releasing
Completed
Aborted
Failed
Unknown
```

## Ressourcen

Transaktionen können unter anderem folgende Ressourcen betreffen:

```text
CPU Time
Memory
IO Bandwidth
Storage
Network Bandwidth
GPU
NPU
Device Access
Energy
Thermal Budget
Latency Budget
```

## Ablauf

```text
Request
  ↓
Validate Requirements
  ↓
Check Availability
  ↓
Reserve Resources
  ↓
Prepare Providers
  ↓
Commit
  ↓
Activate
  ↓
Verify
```

Bei Fehler:

```text
Failure
   ↓
Release Reservations
   ↓
Abort / Recover
```

## Reservation

Eine Reservation verhindert, dass zugesagte Ressourcen gleichzeitig anderweitig verbindlich vergeben werden.

```text
Available
   ↓
Reserved
   ↓
Committed
```

Eine Reservation ist noch keine aktive Ressourcennutzung.

Reservationen müssen zeitlich und mengenmäßig begrenzbar sein.

## Atomic Resource Sets

Eine Operation kann mehrere Ressourcen gleichzeitig benötigen.

```text
Execution
├── CPU
├── Memory
├── IO
└── Network
```

Wenn eine notwendige Hard Resource nicht bereitgestellt werden kann, darf nicht stillschweigend nur ein Teil der erforderlichen Ressourcen committed werden.

```text
Partial Hard Reservation ≠ Valid Contract
```

## Hard und Soft Requirements

```text
Hard Requirement
→ muss erfüllt werden

Soft Requirement
→ darf angepasst werden
```

Beispiel:

```text
Memory:
Minimum 256 MiB
Preferred 512 MiB
```

Kann nur das Minimum garantiert werden, darf die Transaktion entsprechend dem Execution Contract fortgesetzt werden.

## Resource Budget

Jede Resource Transaction kann durch ein Budget begrenzt werden.

```text
ResourceBudget
├── CPU
├── Memory
├── IO
├── Network
├── Energy
└── Time
```

Eine Transaktion darf ihr autorisiertes Budget nicht eigenständig erweitern.

## Capability Security

Ressourcenvergabe benötigt Authority.

```text
Resource Request
      +
Capability
      ↓
Authorized Reservation
```

```text
Resource Availability ≠ Resource Authority
```

Eine freie Ressource darf nicht allein aufgrund ihrer Verfügbarkeit verwendet werden.

## Provider Integration

Ressourcen können von unterschiedlichen Providern bereitgestellt werden.

```text
Resource Requirement
       ↓
Provider Selection
       ↓
Reservation
       ↓
Commit
```

Provider müssen Anforderungen aus:

```text
Capability
Security
Trust
Sovereignty
Execution Contract
Failure Domain
```

erfüllen.

## Concurrent Transactions

Mehrere Transaktionen können um dieselben Ressourcen konkurrieren.

```text
Transaction A ─┐
Transaction B ─┼→ Resource Arbitration
Transaction C ─┘
```

Arbitration entscheidet nach System Policy, Guarantees, Priority, Fairness und vorhandenen Budgets.

## Deadlock Prevention

Mehrere Ressourcenreservierungen können zyklische Abhängigkeiten erzeugen.

```text
Transaction A → Resource X
      ↑             ↓
Resource Y ← Transaction B
```

NovaOS muss solche Situationen erkennen, vermeiden oder kontrolliert auflösen können.

Mögliche Strategien:

```text
Ordered Reservation
Timeout
Abort
Preemption
Resource Replanning
```

## Reservation Expiration

Nicht committed Reservationen müssen verfallen können.

```text
Reserved
   ↓
Expiration
   ↓
Released
```

Dadurch dürfen abgestürzte oder blockierte Transaktionen keine Ressourcen dauerhaft blockieren.

## Commit

Beim Commit werden vorbereitete Ressourcen verbindlich der Transaktion beziehungsweise ihrem Ziel zugeordnet.

```text
Prepared Reservation
        ↓
Commit Point
        ↓
Committed Resources
```

Der Commit Point muss eindeutig definiert sein.

## Activation

Committed Ressourcen können anschließend aktiviert werden.

```text
Committed
    ↓
Bind / Map / Schedule
    ↓
Active
```

Beispiele:

```text
Memory Mapping
CPU Scheduling
DMA Mapping
Network Queue Activation
Device Binding
```

## Verification

Nach Aktivierung muss geprüft werden können:

```text
Resource Exists
Correct Owner
Correct Amount
Correct Provider
Required Guarantee Active
Capability Valid
Execution Contract Satisfied
```

```text
Committed ≠ Verified
```

## Release

Ressourcen müssen kontrolliert freigegeben werden.

```text
Active
  ↓
Quiesce
  ↓
Release
  ↓
Reclaim
  ↓
Available
```

Noch laufende DMA-, IO- oder asynchrone Operationen müssen vor Wiederverwendung berücksichtigt werden.

## Abort

Vor Commit:

```text
Abort
  ↓
Release Reservations
```

Nach Commit können zusätzliche Maßnahmen erforderlich sein:

```text
Quiesce
Detach
Unmap
Release
Reclaim
Recover
```

## Failure Handling

Bei Fehlern darf kein dauerhaft unbekannter Ressourcenbesitz entstehen.

```text
Transaction Failure
        ↓
Determine Resource State
        ↓
Release / Reclaim / Isolate
```

Wenn der Zustand nicht sicher bestimmbar ist:

```text
Unknown ≠ Free
```

Die Ressource muss bis zur Klärung isoliert bleiben können.

## Resource Pressure

Bei Ressourcenknappheit können Policies:

```text
Reject
Reclaim
Degrade
Rebalance
Migrate
Preempt
```

auslösen.

Bereits zugesagte Hard Guarantees dürfen nicht stillschweigend verletzt werden.

## Realtime Integration

Firm- und Hard-Realtime-Operationen können Ressourcen vor Ausführung reservieren.

```text
Admission Control
      ↓
Resource Transaction
      ↓
Guaranteed Reservation
      ↓
Execution
```

Eine Realtime-Ausführung darf erst zugelassen werden, wenn notwendige Garantien tatsächlich gewährt wurden.

## Distributed Resources

Remote Ressourcen benötigen zusätzliche Zustände:

```text
Reserved
Committed
Unavailable
Lease Expired
Ownership Unknown
```

```text
Network Timeout ≠ Reservation Released
```

Leases, Idempotency und Recovery-Protokolle können zur Koordination verwendet werden.

## Provenance

Relevante Ressourcenänderungen sollen nachvollziehbar sein:

```text
TransactionID
Owner
Resource
Amount
Provider
Reservation
Commit
Release
Authority
Verification
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
TransactionID
OwnerID
ResourceSet
Reservations
Committed Resources
Active Resources
Resource Budget
Provider
Deadline
Guarantees
Conflicts
State
Verification State
```

## Normative Anforderungen

1. NovaOS MUSS Ressourcenreservierungen transaktional durchführen können.
2. Reservation, Commit und Activation MÜSSEN getrennte Zustände sein.
3. Resource Transactions MÜSSEN stabile Transaction IDs besitzen.
4. Ressourcenanforderungen MÜSSEN vor Reservation validiert werden.
5. Hard Requirements DÜRFEN NICHT stillschweigend teilweise erfüllt werden.
6. Soft Requirements DÜRFEN innerhalb ihrer Policies angepasst werden.
7. Reservationen MÜSSEN durch Resource Budgets begrenzbar sein.
8. Resource Availability DARF NICHT als Authority interpretiert werden.
9. Ressourcenvergabe MUSS Capability-basiert kontrollierbar sein.
10. Provider MÜSSEN relevante Execution-, Trust- und Sovereignty-Anforderungen erfüllen.
11. Concurrent Resource Transactions MÜSSEN durch Arbitration koordinierbar sein.
12. Zyklische Ressourcenabhängigkeiten MÜSSEN behandelbar sein.
13. Nicht committed Reservationen MÜSSEN verfallen oder explizit freigegeben werden können.
14. Commit Points MÜSSEN eindeutig definierbar sein.
15. Committed Ressourcen MÜSSEN vor Nutzung verifiziert werden können.
16. Ressourcenfreigabe MUSS laufende asynchrone Nutzung berücksichtigen.
17. Bei unbekanntem Ownership-State DARF eine Ressource NICHT als frei gelten.
18. Resource Pressure DARF Hard Guarantees NICHT stillschweigend verletzen.
19. Realtime Admission MUSS notwendige Ressourcen vor Ausführung garantieren können.
20. Remote Timeouts DÜRFEN NICHT automatisch als Resource Release gelten.
21. Fehlgeschlagene Resource Transactions MÜSSEN Recovery und Reclaim unterstützen.
22. Resource Transactions MÜSSEN autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-RESOURCE-RECLAIM-0001`
- `NPSPEC-REALTIME-TEMPORALISOLATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0135`

## Ergebnis

```text
Resource Intent
      ↓
Validate
      ↓
Reserve Resource Set
      ↓
Prepare
      ↓
Commit
      ↓
Activate
      ↓
Verify
├── Valid → Resource Available to Owner
└── Invalid
      ↓
Release / Reclaim / Recover
```

NovaOS erhält damit ein transaktionales Ressourcenmanagement, das Ressourcen nicht nur verteilt, sondern deren Reservation, Commit, Aktivierung, Nutzung und Freigabe als kontrollierte Zustandsübergänge behandelt und dadurch partielle Vergaben, Ressourcenlecks und inkonsistente Ownership-Zustände verhindert.