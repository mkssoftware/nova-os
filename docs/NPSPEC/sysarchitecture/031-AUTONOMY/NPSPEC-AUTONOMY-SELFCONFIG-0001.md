# NPSPEC-AUTONOMY-SELFCONFIG-0001 – Nova Autonomous Self-Configuration

## Status

Angenommen

## Kategorie

Autonomy / Self-Configuration / System Management

## Zweck

NovaOS definiert einen kontrollierten Self-Configuration-Mechanismus, mit dem das System seine Konfiguration automatisch an Hardware, Ressourcen, Umgebung, Workloads und Nutzeranforderungen anpassen kann.

```text
Observe System
      ↓
Detect Configuration Need
      ↓
Generate Configuration Plan
      ↓
Constraint Validation
      ↓
Apply
      ↓
Verify
```

Self-Configuration reduziert manuelle Einrichtung, ohne Security, Authority oder Nutzerkontrolle zu umgehen.

## Grundprinzipien

```text
Self-Configuration ≠ Unlimited Configuration
Detected Hardware ≠ Permission
Recommended Setting ≠ Required Setting
Optimization ≠ Configuration Authority
Automatic ≠ Irreversible

Autonomous Configuration
    ⊆
Authorized Configuration
```

## Configuration Model

```text
SelfConfiguration
├── ConfigurationID
├── Target
├── CurrentState
├── DesiredState
├── ChangeSet
└── State
```

Optional:

```text
PolicyID
DecisionID
ExecutionID
IdentityID
ResourceID
DeviceID
ProviderID
AutonomyLevel
RiskClass
RollbackPoint
ProvenanceID
```

## Configuration Targets

Self-Configuration kann betreffen:

```text
Hardware
Drivers
Devices
Network
Storage
Memory
Scheduler
Power
Services
Providers
Capabilities
System Modules
Performance Policies
```

## Reconciliation

Self-Configuration folgt dem deklarativen NovaOS-Systemmodell:

```text
Desired State
      ↕
Reconciliation
      ↕
Actual State
```

Ablauf:

```text
Observe
   ↓
Compare
   ↓
Plan
   ↓
Validate
   ↓
Execute
   ↓
Verify
```

## Hardware Configuration

NovaOS kann erkannte Hardware automatisch konfigurieren.

Beispiele:

```text
CPU Topology
NUMA
Memory
Storage
Network Interfaces
GPU
NPU
Audio
Displays
Peripheral Devices
```

Hardwareerkennung allein erzeugt keine Device Capability.

## Provider Configuration

Für benötigte Capabilities kann NovaOS geeignete Provider auswählen und konfigurieren.

```text
Required Capability
       ↓
Provider Discovery
       ↓
Constraint Evaluation
       ↓
Provider Configuration
```

Provider müssen weiterhin Trust-, Security- und Capability-Anforderungen erfüllen.

## Configuration Planning

Änderungen sollen vor ihrer Anwendung als Plan dargestellt werden.

```text
ConfigurationPlan
├── Required Changes
├── Dependencies
├── Constraints
├── Expected Effects
├── Risk
└── Rollback Strategy
```

Damit können mehrere zusammenhängende Änderungen kontrolliert ausgeführt werden.

## Transactional Configuration

Zusammengehörige Änderungen sollen nach Möglichkeit transaktional erfolgen.

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Apply
  ↓
Verify
  ↓
Commit
```

Bei Fehler:

```text
Failure
   ↓
Rollback
```

## Risk Classification

Konfigurationsänderungen können klassifiziert werden:

```text
Low
Moderate
High
Critical
```

Risikoarme und reversible Änderungen können einen höheren Autonomiegrad erhalten.

Kritische oder irreversible Änderungen können stärkere Bestätigung verlangen.

## Autonomy Policy

Self-Configuration muss die geltende Autonomy Policy beachten.

```text
Configuration Proposal
        ↓
Autonomy Policy
        ↓
Autonomous / Confirm / Manual
```

Der Self-Configuration-Mechanismus darf seinen eigenen Autonomiegrad nicht erhöhen.

## Constraints

Vor jeder Änderung müssen relevante Constraints geprüft werden:

```text
Safety
Security
Capabilities
Trust
Sovereignty
Resources
Execution Contracts
User Overrides
Hardware Limits
```

```text
Unknown Hard Constraint ≠ Satisfied
```

## Adaptive Integration

Adaptive Systeme können Self-Configuration Änderungen vorschlagen.

Beispiele:

```text
Scheduler Parameters
Cache Limits
Prefetch Limits
Power Policy
Storage Placement Policy
Network Preferences
```

Adaptive Erkenntnisse bleiben Vorschläge innerhalb der Autonomy Constraints.

## User Override

Explizite Nutzerkonfiguration besitzt Vorrang vor adaptiver Selbstkonfiguration.

```text
Adaptive Proposal
      ↓
User Override Exists?
      ↓
Preserve User Decision
```

NovaOS darf bewusst gesetzte Nutzerwerte nicht ohne entsprechende Policy überschreiben.

## Configuration Drift

NovaOS kann Abweichungen zwischen gewünschter und tatsächlicher Konfiguration erkennen.

```text
Desired State
      ≠
Actual State
      ↓
Configuration Drift
```

Je nach Policy kann NovaOS:

```text
Report
Repair
Reconcile
Request Confirmation
```

## Verification

Nach einer Änderung muss geprüft werden, ob der gewünschte Zustand erreicht wurde.

```text
Apply
  ↓
Observe
  ↓
Verify
```

```text
Applied ≠ Verified
```

Fehlgeschlagene Verifikation kann Rollback oder Degradation auslösen.

## Safe Fallback

Bei:

```text
Invalid Plan
Constraint Violation
Configuration Failure
Verification Failure
Unknown State
```

muss NovaOS einen sicheren Zustand erhalten oder wiederherstellen.

```text
Failed Configuration
        ↓
Rollback / Safe Baseline
```

## Security

Konfigurationsänderungen benötigen entsprechende Authority.

Beispiele:

```text
ConfigurationRead
ConfigurationPlan
ConfigurationApply
ConfigurationRollback
ConfigurationOverride
```

Planung und Anwendung sollen getrennte Capabilities besitzen können.

## Provenance

Relevante Änderungen sollen nachvollziehbar sein:

```text
Who / What
Changed What
Previous State
New State
Reason
Policy
Decision
Timestamp
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ConfigurationID
Target
Current State
Desired State
ChangeSet
Autonomy Level
Constraints
Risk Class
Decision Reason
Verification State
Rollback State
User Override
Policy Version
Provenance
```

## Normative Anforderungen

1. NovaOS MUSS autonome Self-Configuration durch Autonomy Policy und Constraints begrenzen.
2. Self-Configuration DARF keine zusätzliche Authority erzeugen.
3. Konfigurationsänderungen MÜSSEN die erforderlichen Capabilities besitzen.
4. Current State und Desired State MÜSSEN unterscheidbar sein.
5. Self-Configuration SOLL dem deklarativen Reconciliation-Modell folgen.
6. Änderungen SOLLEN vor Ausführung als Configuration Plan darstellbar sein.
7. Zusammengehörige Änderungen SOLLEN transaktional ausgeführt werden.
8. Reversible Änderungen SOLLEN einen Rollback unterstützen.
9. `Applied` DARF NICHT automatisch als `Verified` behandelt werden.
10. Hardwareerkennung DARF keine Device Authority erzeugen.
11. Self-Configuration MUSS Safety-, Security-, Trust-, Sovereignty- und Resource Constraints respektieren.
12. Explizite User Overrides MÜSSEN Vorrang vor adaptiver Self-Configuration besitzen.
13. Self-Configuration DARF ihren eigenen Autonomy Level NICHT erhöhen.
14. Configuration Drift MUSS erkennbar sein können.
15. Adaptive Systeme DÜRFEN Konfigurationsänderungen vorschlagen, aber keine Hard Constraints umgehen.
16. Bei fehlgeschlagener Verifikation MUSS Rollback, Degradation oder Safe Fallback möglich sein.
17. Konfigurationsänderungen SOLLEN Provenance besitzen.
18. Self-Configuration MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `NPSPEC-ADAPTIVE-POLICYLEARNING-0001`
- `NPSPEC-ADAPTIVE-USEROVERRIDE-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0095`

## Ergebnis

```text
Actual System State
        ↓
Desired State
        ↓
Difference
        ↓
Configuration Plan
        ↓
Authority + Constraints
        ↓
Transactional Apply
        ↓
Verification
        ↓
Commit / Rollback
```

NovaOS erhält damit eine kontrollierte Self-Configuration-Schicht, die Hardware, Ressourcen und Systemkomponenten weitgehend automatisch konfigurieren und an veränderte Bedingungen anpassen kann, während Authority, Constraints, Nutzerentscheidungen und sichere Rollback-Pfade jederzeit verbindlich bleiben.