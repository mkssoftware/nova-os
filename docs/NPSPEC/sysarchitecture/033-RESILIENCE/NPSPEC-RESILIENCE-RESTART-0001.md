# NPSPEC-RESILIENCE-RESTART-0001 – Nova Resilience Restart

## Status

Angenommen

## Kategorie

Resilience / Recovery / Restart

## Zweck

NovaOS definiert einen kontrollierten Restart-Mechanismus für fehlerhafte oder nicht mehr funktionsfähige Komponenten.

```text
Failure
   ↓
Containment
   ↓
Restart Decision
   ↓
Stop
   ↓
Clean State
   ↓
Start
   ↓
Verify
```

Restart ist eine Recovery-Strategie und darf nicht automatisch mit erfolgreicher Wiederherstellung gleichgesetzt werden.

## Grundprinzipien

```text
Restart ≠ Recovery
Restarted ≠ Healthy
Restart ≠ Reset
Restart ≠ Retry
Crash ≠ Restart Required
Repeated Restart ≠ Progress
```

NovaOS soll immer die kleinste ausreichende Restart-Domain verwenden.

## Restart Model

```text
RestartOperation
├── RestartID
├── TargetID
├── DomainID
├── Reason
├── RestartMode
├── Attempt
└── State
```

Optional:

```text
DetectionID
ClassificationID
CheckpointID
RecoveryPolicy
RestartBudget
DependencySet
Criticality
ProvenanceID
```

## Restart Domains

Restart kann auf unterschiedlichen Ebenen erfolgen:

```text
Task
Process
Service
Driver
Provider
Subsystem
Device
Node
System
```

Es gilt:

```text
Smallest Sufficient Restart Domain
```

Ein einzelner Servicefehler soll keinen vollständigen Systemneustart erzwingen.

## Restart Modes

NovaOS soll mindestens unterscheiden:

```text
Clean Restart
Stateful Restart
Checkpoint Restart
Provider Restart
Dependency Restart
Cold Restart
```

### Clean Restart

Die Komponente startet aus einem definierten Grundzustand.

### Stateful Restart

Explizit als sicher deklarierter Zustand wird übernommen.

### Checkpoint Restart

Die Komponente startet von einem validierten Checkpoint.

### Provider Restart

Nur der betroffene Capability Provider wird neu gestartet.

### Dependency Restart

Eine fehlerhafte Abhängigkeit wird gezielt neu gestartet.

### Cold Restart

Die gesamte betroffene Domain wird vollständig neu initialisiert.

## Restart Lifecycle

```text
Request
   ↓
Validate
   ↓
Quiesce
   ↓
Stop
   ↓
Cleanup
   ↓
Initialize
   ↓
Start
   ↓
Verify
   ↓
Healthy / Degraded / Failed
```

Jede Phase muss einen expliziten Zustand besitzen können.

## Quiescing

Vor einem Restart sollen laufende Aktivitäten soweit möglich kontrolliert beendet werden.

```text
Stop New Work
     ↓
Drain / Cancel Work
     ↓
Flush Required State
     ↓
Restart
```

Bei kritischen Fehlern darf ein sofortiger Abbruch erforderlich sein.

## State Handling

Nicht jeder Zustand darf über einen Restart hinweg erhalten bleiben.

```text
Persistent State
Validated Recoverable State
Ephemeral State
Invalid State
```

Ungültiger oder möglicherweise beschädigter Zustand darf nicht ungeprüft wiederverwendet werden.

## Dependency Handling

Vor einem Restart müssen Abhängigkeiten berücksichtigt werden.

```text
Service A
   ↓
Service B
   ↓
Provider C
```

NovaOS muss unterscheiden können zwischen:

```text
Restart Target
Restart Dependency
Restart Dependent
```

Unnötige Restart-Kaskaden sollen vermieden werden.

## Capability Handling

Beim Restart müssen Capabilities kontrolliert behandelt werden.

```text
Old Instance
    ↓
Revoke / Expire Handles
    ↓
New Instance
    ↓
Reauthorize
```

Eine neue Instanz darf nicht automatisch alte Autorität übernehmen, wenn diese nicht mehr gültig ist.

## Resource Cleanup

Vor Wiederverwendung müssen Ressourcen kontrolliert behandelt werden:

```text
Memory
Handles
IPC Channels
IO Requests
DMA
Locks
Timers
Network Connections
Device State
```

Verwaiste Ressourcen dürfen nicht unkontrolliert bestehen bleiben.

## Restart Budget

Restart-Versuche müssen begrenzt sein.

```text
RestartBudget
├── MaxAttempts
├── TimeWindow
├── Cooldown
├── CPU Budget
└── Recovery Time
```

Beispiel:

```text
Restart
   ↓ fails
Restart
   ↓ fails
Budget Exhausted
   ↓
Escalate
```

## Restart Loop Protection

NovaOS muss Restart Loops erkennen können.

```text
Start
 ↓
Crash
 ↓
Restart
 ↓
Crash
 ↓
Restart Loop
```

Mögliche Reaktionen:

```text
Backoff
Disable Component
Provider Failover
Degraded Mode
Larger Recovery Domain
Recovery Environment
```

## Supervision

Supervisoren können Restart Policies verwalten.

```text
Component
   ↓ failure
Supervisor
   ↓
Restart Policy
```

Der Supervisor darf nur innerhalb seiner autorisierten Recovery Domain handeln.

## Driver Restart

Treiber-Restart erfordert zusätzliche Kontrolle.

```text
Stop IO
  ↓
Stop DMA
  ↓
Quiesce Device
  ↓
Restart Driver
  ↓
Reinitialize Device
  ↓
Verify
```

DMA und Device State müssen vor Wiederverwendung sicher kontrolliert sein.

## Distributed Restart

Bei verteilten Komponenten muss Restart mit Cluster- und Consistency-Zuständen abgestimmt werden.

```text
Node Restart
     ↓
Membership Revalidation
     ↓
State Synchronization
     ↓
Rejoin
```

Ein Restart darf kein Split-Brain erzeugen.

## Realtime Integration

Restart kann bestehende Realtime-Garantien ungültig machen.

```text
Restart
   ↓
Guarantee Lost
   ↓
Contract Revalidation
```

Abhängige Realtime-Executions müssen entsprechend reagieren.

## Verification

Ein erfolgreicher Prozessstart genügt nicht.

```text
Restart Complete
      ↓
Health Check
      ↓
Integrity Check
      ↓
Dependency Check
      ↓
Contract Check
      ↓
Healthy
```

```text
Running ≠ Healthy
```

## Escalation

Scheitert Restart:

```text
Local Restart
     ↓
Provider Failover
     ↓
Domain Recovery
     ↓
Subsystem Recovery
     ↓
System Recovery
```

Eskalation soll nur soweit erfolgen, wie zur Wiederherstellung erforderlich.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
RestartID
TargetID
DomainID
Restart Mode
Restart Reason
Attempt Count
Restart Budget
Current Phase
Previous Failure
Dependency State
Verification Result
Final Health State
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Restarts einzelner Systemkomponenten unterstützen können.
2. Restart MUSS von Detection, Isolation, Containment und Diagnosis getrennt bleiben.
3. NovaOS SOLL die kleinste ausreichende Restart-Domain verwenden.
4. Ein Restart DARF NICHT automatisch als erfolgreiche Recovery gelten.
5. Laufende Arbeit SOLL vor Restart kontrolliert beendet oder abgebrochen werden.
6. Ungültiger Zustand DARF NICHT ungeprüft über einen Restart übernommen werden.
7. Checkpoints MÜSSEN vor Verwendung validiert werden.
8. Abhängigkeiten MÜSSEN vor Restart berücksichtigt werden.
9. Restart-Kaskaden SOLLEN minimiert werden.
10. Alte Capability Handles MÜSSEN nach Bedarf widerrufen oder invalidiert werden.
11. Ressourcen MÜSSEN vor Wiederverwendung kontrolliert bereinigt werden.
12. DMA MUSS bei Driver- und Device-Restarts sicher beendet oder isoliert werden.
13. Restart-Versuche MÜSSEN durch Budgets begrenzt sein.
14. Restart Loops MÜSSEN erkannt werden können.
15. Wiederholte erfolglose Restarts MÜSSEN eskalierbar sein.
16. Distributed Restart MUSS Membership, Consistency und Split-Brain berücksichtigen.
17. Betroffene Realtime-Garantien MÜSSEN nach Restart revalidiert werden.
18. Eine neu gestartete Komponente MUSS vor Rückkehr in den Normalbetrieb verifiziert werden.
19. `Running` DARF NICHT automatisch als `Healthy` gelten.
20. Restart-Zustände und Ergebnisse MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-WATCHDOG-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-PROCESS-SUPERVISION-0001`
- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-DRIVER-LIVEREPLACE-0001`
- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-ARCH-0116`

## Ergebnis

```text
Failure
   ↓
Contain
   ↓
Select Restart Domain
   ↓
Quiesce
   ↓
Restart
   ↓
Restore / Reinitialize
   ↓
Verify
   ├── Healthy → Resume
   └── Failed
         ↓
      Escalate
```

NovaOS erhält damit einen kontrollierten Restart-Mechanismus, der fehlerhafte Komponenten möglichst lokal neu starten kann, Restart-Schleifen verhindert, Ressourcen und Zustände sicher behandelt und erst nach erfolgreicher Verifikation in den normalen Betrieb zurückkehrt.