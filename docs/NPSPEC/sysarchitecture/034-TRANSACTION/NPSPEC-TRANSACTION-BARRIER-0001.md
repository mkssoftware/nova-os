# NPSPEC-TRANSACTION-BARRIER-0001 – Nova Transaction Barrier

## Status

Angenommen

## Kategorie

Transaction / Synchronization / Commit Coordination

## Zweck

NovaOS definiert Transaction Barriers als explizite Synchronisationspunkte innerhalb komplexer Transaktionen. Eine Barrier stellt sicher, dass definierte Voraussetzungen, Participants oder Zustandsänderungen einen festgelegten Punkt erreicht haben, bevor die nächste Transaktionsphase beginnen darf.

```text
Operations
    ↓
Barrier
    ↓
Validate Required State
    ↓
Release
    ↓
Next Phase
```

Transaction Barriers koordinieren Transaktionsphasen, ohne automatisch globale Locks oder globale Synchronisation zu erzeugen.

## Grundprinzipien

```text
Barrier ≠ Lock
Barrier ≠ Commit
Barrier ≠ Global Synchronization
Barrier ≠ Transaction
Barrier Reached ≠ Transaction Successful
Participant Arrived ≠ Participant Valid
Timeout ≠ Participant Failed
```

## Barrier Model

```text
TransactionBarrier
├── BarrierID
├── TransactionID
├── Participants
├── RequiredCondition
├── BarrierPolicy
├── Deadline
└── State
```

Optional:

```text
RequiredParticipants
Quorum
Dependencies
ExecutionContractID
FailurePolicy
RecoveryPolicy
ResourceBudget
ProvenanceID
```

## Zustände

```text
Created
Waiting
Evaluating
Satisfied
Released
TimedOut
Cancelled
Failed
Unknown
```

## Barrier Types

NovaOS kann unterschiedliche Barrier-Typen unterstützen:

```text
Participant Barrier
State Barrier
Resource Barrier
Dependency Barrier
Commit Barrier
Verification Barrier
Phase Barrier
```

## Participant Barrier

Eine Participant Barrier wartet auf definierte Participants.

```text
Service A ─┐
Service B ─┼→ Barrier → Continue
Service C ─┘
```

Dabei können unterschiedliche Policies gelten:

```text
All
Required Set
Quorum
Policy-defined Set
```

Nicht jede Multi-Service-Transaktion muss auf alle Participants warten.

## State Barrier

Eine Barrier kann einen bestimmten Zustand voraussetzen.

```text
Storage Prepared
Network Prepared
Provider Prepared
        ↓
Barrier
        ↓
Commit Phase
```

Das bloße Erreichen der Barrier reicht nicht aus; der erwartete Zustand muss validiert werden.

## Phase Barrier

Transaktionen können in Phasen strukturiert werden:

```text
Prepare
   ↓
Barrier A
   ↓
Commit
   ↓
Barrier B
   ↓
Verify
```

Damit können nachfolgende Operationen erst beginnen, wenn die vorherige Phase ausreichend abgeschlossen ist.

## Commit Barrier

Vor einem kritischen Commit kann eine Barrier sicherstellen:

```text
Required Participants Prepared
Resources Reserved
Capabilities Valid
Dependencies Valid
Transaction Log Durable
```

```text
Commit Barrier Passed ≠ Commit Performed
```

Die Barrier autorisiert den Commit nicht selbst.

## Verification Barrier

Nach verteilten Änderungen kann eine Barrier auf erforderliche Verification-Ergebnisse warten.

```text
Service A Verified ─┐
Service B Verified ─┼→ Verification Barrier
Service C Verified ─┘
```

Erst danach darf der Gesamtzustand gegebenenfalls als verifiziert gelten.

## Dependency Handling

Barriers müssen Abhängigkeiten berücksichtigen können.

```text
A
↓
B
↓
Barrier
↓
C
```

Zyklische Barrier-Abhängigkeiten müssen erkannt oder durch Architekturregeln verhindert werden.

```text
Barrier A waits for B
Barrier B waits for A
        ↓
Deadlock
```

## Deadline

Jede Barrier kann zeitlich begrenzt werden.

```text
Waiting
   ↓
Deadline Reached
   ↓
Barrier Policy
```

Mögliche Reaktionen:

```text
Abort
Retry
Degrade
Reduce Participant Set
Recovery
Escalate
```

Eine Reduktion erforderlicher Participants ist nur zulässig, wenn die Policy dies explizit erlaubt.

```text
Timeout ≠ Failure
```

## Cancellation

Wird die übergeordnete Transaktion abgebrochen, muss eine wartende Barrier abbrechbar sein.

```text
Transaction Cancel
       ↓
Barrier Cancel
       ↓
Release Waiting Resources
```

Cancellation darf bereits ausgeführte Effekte nicht automatisch als rückgängig gemacht betrachten.

## Capability Security

Eine Barrier erzeugt keine Authority.

```text
Barrier Passed
     ≠
Operation Authorized
```

Nachfolgende Operationen müssen weiterhin ihre erforderlichen Capabilities besitzen.

Bei sicherheitskritischen Phasen kann eine Revalidation vor Release erforderlich sein.

## Resource Integration

Barriers dürfen reservierte Ressourcen nicht unbegrenzt blockieren.

```text
Barrier Waiting
      +
Reserved Resources
      ↓
Bounded Waiting
```

Resource Budgets, Deadlines und Reservation Expiration müssen berücksichtigt werden.

## Multi-Service Integration

Transaction Barriers können Multi-Service-Transaktionen koordinieren:

```text
Service A → Prepared
Service B → Prepared
Service C → Prepared
             ↓
       Prepare Barrier
             ↓
       Commit Decision
```

Ein Participant mit Zustand `Unknown` darf nicht automatisch als erfolgreich angekommen gelten.

## Distributed Barriers

Verteilte Barriers müssen Netzwerkfehler berücksichtigen.

```text
No Response ≠ Not Arrived
No Response ≠ Failed
Local Barrier State ≠ Global Truth
```

NovaOS setzt keine universelle globale Barrier über alle Nodes voraus.

Quorum-, Consensus- oder transaktionsspezifische Mechanismen können verwendet werden.

## Determinismus

Deterministische Ausführung kann definierte Barrier-Reihenfolgen verwenden.

```text
Operation Set A
      ↓
Barrier 1
      ↓
Operation Set B
```

Barrier Events können für Record & Replay aufgezeichnet werden.

## Failure Handling

Kann eine Barrier nicht erfüllt werden:

```text
Barrier Unsatisfied
       ↓
Failure Policy
       ↓
Retry / Abort / Rollback
Compensation / Degradation
Recovery
```

Der tatsächliche Zustand aller Participants muss erhalten bleiben.

## Provenance

Nachvollziehbar sein sollen:

```text
BarrierID
TransactionID
Participants
Required Condition
Arrival States
Release Decision
Timeout
Failure
Authority Revalidation
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
BarrierID
TransactionID
Barrier Type
Participants
Required Participants
Arrived Participants
Unknown Participants
Required Condition
Deadline
Current State
Release Condition
Failure State
```

## Normative Anforderungen

1. NovaOS MUSS Transaction Barriers als explizite Transaktions-Synchronisationspunkte unterstützen können.
2. Barriers MÜSSEN stabile Barrier IDs besitzen.
3. Barrier und Commit MÜSSEN getrennte Konzepte bleiben.
4. Eine Barrier DARF keine implizite Authority erzeugen.
5. Participants und Required Conditions MÜSSEN explizit definierbar sein.
6. Barriers MÜSSEN unterschiedliche Completion Policies unterstützen können.
7. `Unknown` DARF NICHT automatisch als erfolgreicher Participant-State gelten.
8. Phase Barriers MÜSSEN Transaktionsphasen koordinieren können.
9. Commit Barriers SOLLEN kritische Voraussetzungen vor Commit validieren können.
10. Verification Barriers SOLLEN verteilte Verification koordinieren können.
11. Zyklische Barrier-Abhängigkeiten MÜSSEN erkennbar oder vermeidbar sein.
12. Barriers MÜSSEN Deadlines unterstützen können.
13. Timeout DARF NICHT automatisch als Participant Failure interpretiert werden.
14. Required Participant Sets DÜRFEN nur entsprechend expliziter Policy reduziert werden.
15. Barriers MÜSSEN Cancellation unterstützen.
16. Wartende Barriers DÜRFEN Ressourcen NICHT unbegrenzt blockieren.
17. Capabilities MÜSSEN nach einer Barrier weiterhin gültig sein.
18. Distributed Barriers DÜRFEN keine perfekte globale Sicht voraussetzen.
19. Barrier Events SOLLEN Deterministic Replay unterstützen können.
20. Fehlgeschlagene Barriers MÜSSEN an Transaction Recovery eskalierbar sein.
21. Participant States MÜSSEN bei Barrier Failure erhalten bleiben.
22. Barrier-Zustände MÜSSEN autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-RESOURCE-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-0001`
- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0140`

## Ergebnis

```text
Transaction Phase
       ↓
Participants Execute
       ↓
Transaction Barrier
       ↓
Validate Conditions
├── Satisfied
│      ↓
│   Release
│      ↓
│   Next Phase
│
└── Unsatisfied / Unknown
       ↓
Wait / Abort / Recover / Escalate
```

NovaOS erhält damit explizite Transaction Barriers, die komplexe und verteilte Transaktionen an klar definierten Phasengrenzen koordinieren, ohne globale Synchronisation, erfolgreiche Ausführung oder zusätzliche Authority implizit vorauszusetzen.