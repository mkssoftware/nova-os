# NPSPEC-AUTONOMY-SELFHEALING-0001 – Nova Autonomous Self-Healing

## Status

Angenommen

## Kategorie

Autonomy / Self-Healing / Recovery / Reliability

## Zweck

NovaOS definiert einen kontrollierten Self-Healing-Mechanismus, der Fehler, beschädigte Zustände und degradierte Komponenten automatisch erkennen, eingrenzen und – soweit sicher möglich – reparieren kann.

```text
Observe
   ↓
Detect Failure
   ↓
Diagnose
   ↓
Recovery Plan
   ↓
Constraint Validation
   ↓
Repair
   ↓
Verify
```

Self-Healing soll die Systemverfügbarkeit erhöhen, darf aber niemals durch unkontrollierte Reparaturmaßnahmen zusätzliche Schäden verursachen.

## Grundprinzipien

```text
Failure Detection ≠ Diagnosis
Diagnosis ≠ Proof
Recovery ≠ Repair
Repair ≠ Verified Recovery
Self-Healing ≠ Unlimited Authority

Unknown ≠ Healthy
Started ≠ Healthy
Recovered ≠ Verified
```

## Self-Healing Model

```text
HealingOperation
├── HealingID
├── Target
├── FailureState
├── Diagnosis
├── RecoveryPlan
├── RiskClass
└── State
```

Optional:

```text
ObjectID
ResourceID
ExecutionID
ProviderID
DeviceID
FailureID
PolicyID
CheckpointID
SnapshotID
TransactionID
ProvenanceID
```

## Fehlerquellen

Self-Healing kann reagieren auf:

```text
Integrity Failure
Service Failure
Driver Failure
Configuration Failure
Storage Corruption
Resource Failure
Execution Failure
Dependency Failure
Boot Failure
Health Degradation
```

## Failure Classification

Fehler sollen klassifiziert werden:

```text
Transient
Recoverable
Degraded
Persistent
Critical
Unknown
```

Die Klassifikation bestimmt mögliche Recovery-Strategien.

## Diagnose

Vor einer Reparatur soll NovaOS verfügbare Evidenz auswerten.

```text
Failure
  +
Logs
  +
Metrics
  +
Traces
  +
Health State
  +
Provenance
      ↓
Diagnosis
```

Unsichere Diagnosen müssen als solche gekennzeichnet bleiben.

## Recovery Plan

Vor komplexeren Reparaturen wird ein Plan erzeugt.

```text
RecoveryPlan
├── Target
├── Failure
├── Proposed Actions
├── Dependencies
├── Constraints
├── Expected Result
├── Risk
└── Rollback Strategy
```

## Recovery Strategies

Mögliche Maßnahmen sind:

```text
Restart
Reload
Reconfigure
Reconnect
Remount
Replace Provider
Restore Version
Restore Snapshot
Rollback Transaction
Failover
Reallocate Resource
Rebuild Derived State
Isolate Component
Enter Degraded Mode
```

## Isolation

Fehlerhafte Komponenten sollen bei Bedarf zuerst isoliert werden.

```text
Fault Detected
      ↓
Contain
      ↓
Prevent Propagation
      ↓
Diagnose
      ↓
Recover
```

Insbesondere nicht verifizierte Treiber oder Provider dürfen nicht unkontrolliert andere Systembereiche beschädigen.

## Transactional Repair

Reparaturen sollen nach Möglichkeit transaktional erfolgen.

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Repair
  ↓
Verify
  ↓
Commit
```

Bei Fehlschlag:

```text
Failure
   ↓
Rollback
```

## Checkpoints und Snapshots

Self-Healing kann bekannte gültige Zustände verwenden.

```text
Current Broken State
        ↓
Known Good State
        ↓
Restore
        ↓
Verify
```

Ein älterer Zustand darf nicht automatisch als sicher oder aktuell angenommen werden.

## Autonomous Recovery

Die Autonomy Policy bestimmt den erlaubten Reparaturgrad.

```text
Low Risk
→ Automatic Recovery

Moderate Risk
→ Policy Controlled

High Risk
→ Confirmation / Escalation

Critical
→ Safe Containment
```

## User Override

Explizite Nutzerentscheidungen müssen berücksichtigt werden.

Ein User Override darf jedoch keine zwingenden Safety- oder Security-Maßnahmen verhindern, sofern die geltende Systempolicy dies nicht erlaubt.

## Self-Healing Loop

```text
Detect
  ↓
Contain
  ↓
Diagnose
  ↓
Plan
  ↓
Repair
  ↓
Verify
  ↓
Monitor
```

Schlägt die Reparatur wiederholt fehl, muss eine Endlosschleife verhindert werden.

Mechanismen:

```text
Retry Limit
Cooldown
Failure Counter
Recovery Budget
Escalation
```

## Verification

Nach jeder relevanten Reparatur muss der Zustand erneut geprüft werden.

```text
Repair Complete
      ↓
Integrity Check
      ↓
Health Check
      ↓
Functional Verification
      ↓
Healthy / Degraded / Failed
```

```text
Repair Executed ≠ Repair Successful
```

## Degraded Mode

Ist vollständige Wiederherstellung nicht möglich:

```text
Normal
   ↓
Failure
   ↓
Degraded Mode
```

NovaOS soll verbleibende sichere Funktionen möglichst weiter bereitstellen.

## Boot und Recovery

Self-Healing kann mit dem Boot- und Recovery-System zusammenarbeiten.

```text
Boot Failure
    ↓
Health Evaluation
    ↓
Previous Slot / Snapshot / Recovery
    ↓
Verification
```

NovaDOS kann als unabhängige Recovery-Umgebung verwendet werden, wenn das Hauptsystem nicht mehr zuverlässig reparierbar ist.

## Security

Self-Healing darf Security nicht umgehen.

```text
Repair Authority
      ≠
Universal Authority
```

Reparaturaktionen benötigen passende Capabilities und müssen Code Integrity, Trust und Security Policies respektieren.

## Provenance

Reparaturen sollen nachvollziehbar dokumentieren:

```text
Failure
Diagnosis
Action
Previous State
Resulting State
Verification
Rollback
Timestamp
Policy
Authority
```

## Safe Fallback

Kann kein sicherer Recovery-Pfad bestimmt werden:

```text
Unknown / Unsafe Recovery
          ↓
Contain
          ↓
Degraded / Safe Mode
          ↓
Escalation
```

NovaOS darf keine riskante Reparatur nur deshalb durchführen, um Verfügbarkeit zu erzwingen.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
HealingID
Target
Failure
Diagnosis
Confidence
Recovery Plan
Risk Class
Actions
Verification State
Retry Count
Rollback State
Health State
Policy Version
Provenance
```

## Normative Anforderungen

1. NovaOS MUSS Self-Healing durch Autonomy Policy und Constraints begrenzen.
2. Self-Healing DARF keine unbegrenzte Authority besitzen.
3. Fehlererkennung, Diagnose, Reparatur und Verifikation MÜSSEN getrennte Zustände sein.
4. `Unknown` DARF NICHT automatisch als `Healthy` gelten.
5. Fehlerhafte Komponenten SOLLEN vor weiterer Ausbreitung isolierbar sein.
6. Reparaturen SOLLEN nach Möglichkeit transaktional erfolgen.
7. Kritische Reparaturen SOLLEN Rollback, Snapshot oder einen vergleichbaren Recovery-Pfad besitzen.
8. Reparierte Komponenten MÜSSEN erneut verifiziert werden können.
9. `Repair Executed` DARF NICHT als `Repair Successful` interpretiert werden.
10. Self-Healing MUSS einen Degraded Mode unterstützen können.
11. Wiederholte fehlgeschlagene Reparaturen MÜSSEN begrenzt werden.
12. Recovery-Loops MÜSSEN durch Retry Limits, Cooldowns oder Recovery Budgets kontrollierbar sein.
13. Self-Healing MUSS Security-, Trust-, Sovereignty- und Capability-Grenzen respektieren.
14. Adaptive Prediction DARF eine Diagnose unterstützen, aber NICHT als Beweis eines Fehlers gelten.
15. Bei fehlendem sicheren Recovery-Pfad MUSS Containment oder Eskalation möglich sein.
16. Boot- und Recovery-Mechanismen SOLLEN mit Self-Healing integrierbar sein.
17. Relevante Reparaturen SOLLEN vollständige Provenance besitzen.
18. Self-Healing MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `NPSPEC-AUTONOMY-SELFCONFIG-0001`
- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-ADAPTIVE-USEROVERRIDE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `ADR-ARCH-0096`

## Ergebnis

```text
Failure
   ↓
Contain
   ↓
Diagnose
   ↓
Recovery Plan
   ↓
Authority + Constraints
   ↓
Repair
   ↓
Verify
   ↓
┌─────────┬──────────┬──────────┐
│ Healthy │ Degraded │ Escalate │
└─────────┴──────────┴──────────┘
```

NovaOS erhält damit eine kontrollierte autonome Self-Healing-Schicht, die Fehler erkennen, eindämmen und geeignete Reparaturen durchführen kann, ohne Verfügbarkeit über Sicherheit und Integrität zu stellen. Jede Reparatur bleibt durch Authority, Constraints, Verifikation und sichere Recovery-Pfade begrenzt.