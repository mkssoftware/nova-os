# NPSPEC-RESILIENCE-FAILOVER-0001 – Nova Resilience Failover

## Status

Angenommen

## Kategorie

Resilience / Recovery / Failover

## Zweck

NovaOS definiert einen systemweiten Failover-Mechanismus, der bei Ausfall oder Degradation einer Komponente kontrolliert auf einen alternativen Provider, eine Replica, ein Gerät, einen Pfad oder eine andere geeignete Ressource wechseln kann.

```text
Primary
   ↓
Failure
   ↓
Detect + Classify
   ↓
Select Alternative
   ↓
Validate
   ↓
Failover
   ↓
Verify
```

Failover soll die Verfügbarkeit erhöhen, ohne Security-, Trust-, Sovereignty-, Konsistenz- oder Execution-Garantien zu umgehen.

## Grundprinzipien

```text
Failover ≠ Recovery
Failover ≠ Replication
Failover ≠ Retry
Failover ≠ Migration
Available Alternative ≠ Valid Alternative
Replica ≠ Backup
Switched ≠ Healthy
```

## Failover Model

```text
FailoverOperation
├── FailoverID
├── SourceID
├── TargetID
├── DomainID
├── Reason
├── Mode
└── State
```

Optional:

```text
DetectionID
ClassificationID
ExecutionContractID
ReplicaID
ProviderID
CheckpointID
ConsistencyState
RecoveryBudget
ProvenanceID
```

## Failover Targets

Failover kann zwischen unterschiedlichen Ressourcen erfolgen:

```text
Provider → Provider
Replica → Replica
Device → Device
Network Path → Network Path
Storage → Storage
Node → Node
Accelerator → Accelerator
Service Instance → Service Instance
```

## Failover States

```text
Requested
Selecting
Validating
Preparing
Switching
Verifying
Completed
Failed
RolledBack
Unknown
```

## Alternative Selection

Ein alternatives Ziel muss vor Verwendung geprüft werden.

```text
Candidates
   ↓
Capability
   ↓
Health
   ↓
Trust
   ↓
Security
   ↓
Sovereignty
   ↓
Resource / Realtime Constraints
   ↓
Valid Target
```

Die bloße Erreichbarkeit eines Providers reicht nicht aus.

## Failover Modes

NovaOS kann unterscheiden:

```text
Hot Failover
Warm Failover
Cold Failover
Graceful Failover
Emergency Failover
```

### Hot Failover

Ein bereits aktives alternatives Ziel kann unmittelbar übernehmen.

### Warm Failover

Das alternative Ziel ist vorbereitet, benötigt jedoch Aktivierung oder Synchronisation.

### Cold Failover

Das Ziel muss zunächst gestartet oder initialisiert werden.

## State Transfer

Falls erforderlich, muss Zustand kontrolliert übertragen oder rekonstruiert werden.

```text
Primary State
     ↓
Validate
     ↓
Transfer / Restore
     ↓
Alternative
```

Mögliche Quellen:

```text
Replica State
Checkpoint
Snapshot
Transaction Log
Persistent Storage
```

Ungültiger oder inkompatibler Zustand darf nicht übernommen werden.

## Consistency

Failover darf keine unkontrollierten konkurrierenden Primärinstanzen erzeugen.

```text
Primary A
   ↓ failure
Failover
   ↓
Primary B
```

Insbesondere bei verteilten Systemen muss Split-Brain verhindert oder kontrolliert werden.

```text
Network Partition ≠ Primary Failure
```

## Capability Handling

Ein alternatives Ziel benötigt eigene gültige Authority.

```text
Old Provider Capability
        ≠
New Provider Capability
```

Capabilities dürfen nicht allein aufgrund eines Failovers implizit übertragen werden.

## Execution Contract

Das neue Ziel muss den relevanten Execution Contract erfüllen.

Zu prüfen sind insbesondere:

```text
Semantic Compatibility
Resource Budget
Latency
Deadline
Determinism
Trust
Security
Sovereignty
Location
```

Ist keine vollständig kompatible Alternative verfügbar, darf NovaOS nur policy-konform degradieren oder fehlschlagen.

## Realtime Failover

Für Realtime-Ausführung gilt:

```text
Alternative Available
        ≠
Realtime-Capable Alternative
```

Failover-Zeit, Zustandsübertragung und Aktivierung müssen innerhalb der erforderlichen zeitlichen Garantien liegen.

## Provider Failover

Capability-basierte Services können alternative Provider besitzen.

```text
Capability Request
      ↓
Provider A → Failed
Provider B → Valid
      ↓
Rebind
```

Provider Discovery allein autorisiert keinen Failover.

## Device Failover

Bei Geräten muss berücksichtigt werden:

```text
Outstanding IO
DMA
Device State
Driver State
Data Consistency
```

Das alte Gerät muss soweit erforderlich isoliert werden, bevor das neue aktiv wird.

## Network Failover

Alternative Netzwerkpfade können verwendet werden, wenn bestehende Anforderungen weiterhin erfüllt werden.

```text
Path A → Failed
Path B → Validate
Path B → Active
```

Sovereignty- oder Security-Regeln dürfen durch alternative Routen nicht umgangen werden.

## Storage Failover

Storage Failover muss zusätzlich prüfen:

```text
Replica Freshness
Durability
Integrity
Consistency
Transaction State
```

Eine veraltete Replica darf nicht unbemerkt als aktuelle Wahrheit verwendet werden.

## Failback

Nach Wiederherstellung des ursprünglichen Ziels darf optional ein kontrollierter Failback erfolgen.

```text
Alternative Active
      ↓
Original Recovered
      ↓
Validate
      ↓
Failback?
```

Failback ist kein Automatismus.

Unnötiges Hin- und Herschalten muss vermieden werden.

## Flapping Protection

```text
A → B → A → B → A
```

NovaOS soll dies begrenzen durch:

```text
Hysteresis
Cooldown
Minimum Stable Time
Failover Budget
Health Verification
```

## Failure Domains

Redundante Ziele sollen möglichst unterschiedliche Failure Domains besitzen.

```text
Primary   → Domain A
Secondary → Domain B
```

```text
Different Provider ≠ Independent Failure Domain
```

Gemeinsame Hardware-, Netzwerk-, Storage- oder Energieabhängigkeiten müssen berücksichtigt werden.

## Verification

Nach dem Wechsel muss das Ziel geprüft werden.

```text
Failover
   ↓
Health
Integrity
State
Dependencies
Contracts
   ↓
Verified?
```

```text
Failover Completed ≠ Recovery Verified
```

## Failed Failover

Scheitert der Wechsel:

```text
Failover Failed
      ↓
Alternative Target?
├── Yes → Validate
└── No
      ↓
Degrade / Contain / Recovery / Fail-safe
```

Failover-Versuche müssen durch Recovery Budgets begrenzt bleiben.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
FailoverID
Source
Target
Reason
Mode
State
Failure Domain
Consistency State
Execution Contract
Failover Duration
Verification Result
Failback State
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierten Failover zwischen geeigneten Ressourcen unterstützen können.
2. Failover MUSS von Retry, Restart, Migration und Recovery getrennt bleiben.
3. Alternative Ziele MÜSSEN vor Aktivierung validiert werden.
4. Erreichbarkeit allein DARF NICHT als ausreichende Eignung gelten.
5. Capability-, Security-, Trust- und Sovereignty-Anforderungen MÜSSEN erhalten bleiben.
6. Failover DARF keine zusätzliche Authority erzeugen.
7. Execution Contracts MÜSSEN für das alternative Ziel revalidiert werden.
8. Zustandsübernahme MUSS Integrität und Kompatibilität prüfen.
9. Distributed Failover MUSS Split-Brain berücksichtigen.
10. Netzwerkpartitionen DÜRFEN NICHT automatisch als bestätigter Primary-Ausfall gelten.
11. Storage Failover MUSS Replica Freshness und Consistency berücksichtigen.
12. Device Failover MUSS IO, DMA und Device State berücksichtigen.
13. Realtime Failover MUSS bestehende zeitliche Garantien erfüllen.
14. Redundante Ziele SOLLEN unterschiedliche Failure Domains verwenden.
15. Unterschiedliche Provider DÜRFEN NICHT automatisch als unabhängige Failure Domains gelten.
16. Failover MUSS nach dem Wechsel verifiziert werden.
17. Failback MUSS kontrolliert und policy-gesteuert erfolgen.
18. Flapping MUSS durch geeignete Stabilitätsmechanismen begrenzbar sein.
19. Failover-Versuche MÜSSEN durch Recovery Budgets begrenzbar sein.
20. Failover-Zustände und Entscheidungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RETRY-0001`
- `NPSPEC-RESILIENCE-BACKOFF-0001`
- `NPSPEC-RESILIENCE-CIRCUITBREAKER-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `ADR-ARCH-0120`

## Ergebnis

```text
Primary Failure
      ↓
Detect + Classify
      ↓
Contain
      ↓
Find Alternatives
      ↓
Validate Constraints
      ↓
Select Target
      ↓
State Transfer / Rebind
      ↓
Failover
      ↓
Verify
├── Valid → Continue
└── Failed → Alternative / Degrade / Escalate
```

NovaOS erhält damit einen kontrollierten Failover-Mechanismus, der bei Ausfällen auf geeignete alternative Ressourcen wechseln kann, ohne Konsistenz, Authority, Trust, Sovereignty, Realtime-Anforderungen oder Failure-Domain-Grenzen zu verletzen.