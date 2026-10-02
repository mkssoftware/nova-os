# NPSPEC-RESILIENCE-INTROSPECTION-0001 – Nova Resilience Introspection

## Status

Angenommen

## Kategorie

Resilience / Introspection / Observability

## Zweck

NovaOS definiert eine systemweite Resilience Introspection, über die autorisierte Komponenten den aktuellen Zustand der Resilience-Architektur strukturiert beobachten können.

```text
Failures
Recovery
Health
Failure Domains
Redundancy
Degradation
      ↓
Resilience Introspection
      ↓
Structured System View
```

Ziel ist ein einheitliches Modell für Diagnose, Self-Healing, Recovery, Administration und Systemanalyse.

## Grundprinzipien

```text
Introspection ≠ Authority
Observation ≠ Modification
Health ≠ Availability
Recovered ≠ Verified
Unknown ≠ Healthy
Telemetry ≠ Complete Truth
```

Introspection darf keine zusätzlichen Rechte auf die beobachteten Ressourcen erzeugen.

## Resilience Introspection Model

```text
ResilienceView
├── ViewID
├── TargetID
├── DomainID
├── HealthState
├── FailureState
├── RecoveryState
└── Timestamp
```

Optional:

```text
FailureClass
Criticality
Dependencies
RedundancyState
DegradationState
RecoveryPolicyID
RecoveryBudget
VerificationState
ProviderState
ProvenanceID
```

## Beobachtbare Zustände

Resilience Introspection soll insbesondere erfassen können:

```text
Healthy
Degraded
SuspectedFailure
Failed
Contained
Recovering
Unavailable
Unknown
```

Der Zustand `Unknown` muss explizit erhalten bleiben.

## Failure Information

Beobachtbar sein können:

```text
FailureID
DetectionID
Classification
Severity
Scope
Persistence
Confidence
Propagation Risk
Affected Domain
```

Detection, Classification und Diagnosis müssen unterscheidbar bleiben.

## Failure Domains

NovaOS soll die Resilience-Struktur sichtbar machen können.

```text
System
├── Failure Domain A
│   ├── Service
│   └── Provider
│
└── Failure Domain B
    ├── Driver
    └── Device
```

Gemeinsame Abhängigkeiten und mögliche korrelierte Fehlerpfade sollen erkennbar sein.

## Dependency View

```text
Service A
   ↓
Provider B
   ↓
Device C
```

Bei Fehlern muss nachvollziehbar sein können, welche Komponenten direkt betroffen und welche lediglich abhängig sind.

```text
Failure Origin ≠ Failure Effect
```

## Recovery State

Resilience Introspection soll laufende Recovery sichtbar machen.

```text
Detected
   ↓
Contained
   ↓
Diagnosing
   ↓
Planning
   ↓
Recovering
   ↓
Verifying
   ↓
Recovered
```

Dabei sollen verwendete Recovery-Strategien erkennbar sein:

```text
Retry
Restart
Failover
Rollback
Checkpoint Restore
Degradation
Recovery Mode
```

## Recovery Budget

Autorisierte Komponenten sollen verbleibende Recovery-Ressourcen beobachten können.

```text
Attempts
Time
CPU
Memory
IO
Network
Energy
```

Dadurch können bevorstehende Eskalationen nachvollzogen werden.

## Redundancy View

Für redundante Ressourcen sollen sichtbar sein:

```text
Redundancy Group
Members
Member Health
Failure Domains
Available Capacity
Required Capacity
Active Member
Standby Members
```

```text
Service Available ≠ Full Redundancy Available
```

## Degradation View

Bei Graceful Degradation sollen sichtbar sein:

```text
Normal Profile
Active Profile
Lost Capabilities
Available Capabilities
Affected Guarantees
Restoration Conditions
```

Damit können abhängige Komponenten ihren eigenen Zustand neu bewerten.

## Verification State

Recovery Verification muss separat dargestellt werden.

```text
Recovery Completed
        ↓
Verification Pending
```

Ein Ziel darf nicht allein aufgrund abgeschlossener Recovery als `Healthy` erscheinen.

## Historical State

Resilience Introspection soll relevante Zustandsübergänge nachvollziehbar machen können.

```text
Healthy
  ↓
Degraded
  ↓
Failed
  ↓
Contained
  ↓
Recovering
  ↓
Verified
  ↓
Healthy
```

Langfristige Historie gehört primär in Observability- und Provenance-Systeme.

## Structured Interface

Informationen sollen maschinenlesbar bereitgestellt werden.

```text
Resilience API
├── Query Domain
├── Query Health
├── Query Failures
├── Query Recovery
├── Query Dependencies
└── Query Verification
```

Textuelle Logs sind kein Ersatz für strukturierte Introspection.

## Event Integration

Zustandsänderungen können Events erzeugen:

```text
FailureDetected
HealthChanged
ContainmentEstablished
RecoveryStarted
RecoveryFailed
VerificationCompleted
DegradationChanged
RedundancyChanged
```

Consumer müssen dafür explizit autorisiert sein.

## Security

Resilience-Daten können sensible Informationen enthalten.

Beispiele:

```text
System Topology
Failure Locations
Provider Information
Security State
Recovery Configuration
```

Deshalb müssen Zugriff und Detailgrad Capability-basiert kontrollierbar sein.

## Sensitive Data

Introspection darf keine geheimen Inhalte offenlegen.

Insbesondere:

```text
Capability Tokens
Private Keys
Credentials
Secrets
Protected Memory
```

dürfen nicht über Resilience Introspection ausgegeben werden.

## Distributed Introspection

Verteilte Zustände müssen Herkunft und Aktualität behalten.

```text
Node A → Healthy @ T1
Node B → Unknown @ T2
Node C → Degraded @ T3
```

```text
Stale Observation ≠ Current State
Remote Unknown ≠ Remote Failed
```

Lokale und globale Sicht müssen unterscheidbar bleiben.

## Consistency

Introspection ist eine Beobachtung eines sich verändernden Systems.

Eine perfekte globale Momentaufnahme wird nicht vorausgesetzt.

Informationen sollen deshalb enthalten können:

```text
Timestamp
Version
Source
Confidence
Freshness
```

## Self-Healing Integration

Self-Healing kann Introspection verwenden:

```text
Observe
   ↓
Resilience State
   ↓
Diagnose
   ↓
Recovery Decision
```

Introspection selbst darf jedoch keine Recovery-Aktion autorisieren.

## Benutzeroberfläche

NovaOS kann Resilience-Daten verständlich darstellen:

```text
Systemzustand: Eingeschränkt

Storage:
Redundanz reduziert

Recovery:
Wiederherstellung läuft

Datenintegrität:
Verifiziert
```

Technische Details können über erweiterte Ansichten verfügbar sein.

## Provenance

Wichtige Zustände sollen auf ihre Herkunft zurückführbar sein:

```text
Observation
Detection
Classification
Recovery Decision
Recovery Action
Verification
```

Dadurch bleibt nachvollziehbar, warum NovaOS einen bestimmten Resilience-Zustand meldet.

## Normative Anforderungen

1. NovaOS MUSS eine strukturierte Resilience Introspection bereitstellen.
2. Introspection DARF keine zusätzliche Authority erzeugen.
3. Health-, Failure-, Recovery- und Verification-State MÜSSEN unterscheidbar sein.
4. `Unknown` DARF NICHT als `Healthy` interpretiert werden.
5. Failure Origin und Failure Effects MÜSSEN unterscheidbar sein.
6. Failure Domains und relevante Dependencies MÜSSEN introspektierbar sein.
7. Laufende Recovery MUSS strukturiert beobachtbar sein.
8. Recovery Budgets SOLLEN introspektierbar sein.
9. Redundanzverlust MUSS unabhängig von Service Availability darstellbar sein.
10. Degradation Profiles und verlorene Capabilities MÜSSEN darstellbar sein.
11. Recovery Verification MUSS separat vom Recovery-Abschluss dargestellt werden.
12. Strukturierte Introspection DARF NICHT ausschließlich durch Textlogs ersetzt werden.
13. Zustandsänderungen SOLLEN als autorisierte Events verfügbar sein.
14. Zugriff auf Resilience-Daten MUSS Capability-basiert kontrollierbar sein.
15. Capability Tokens, Credentials und Secrets DÜRFEN NICHT ausgegeben werden.
16. Distributed Introspection MUSS Herkunft und Freshness berücksichtigen.
17. Veraltete Beobachtungen DÜRFEN NICHT als aktueller Zustand dargestellt werden.
18. Remote `Unknown` DARF NICHT automatisch als `Failed` gelten.
19. Introspection DARF keine perfekte globale Momentaufnahme voraussetzen.
20. Resilience-Zustände SOLLEN mit Provenance verknüpfbar sein.
21. Self-Healing DARF Introspection als Beobachtungsquelle verwenden.
22. Introspection selbst DARF keine Recovery Authority verleihen.

## Abhängigkeiten

- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-REDUNDANCY-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-DISASTERRECOVERY-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0132`

## Ergebnis

```text
System State
     ↓
Structured Observation
     ↓
Resilience Introspection
     ↓
Health + Failure + Recovery
     + Dependencies
     + Redundancy
     + Verification
     ↓
Authorized Consumers
├── Self-Healing
├── Diagnostics
├── Administration
└── User Interface
```

NovaOS erhält damit eine einheitliche Resilience-Introspection-Schicht, über die Fehlerzustände, Failure Domains, Abhängigkeiten, Recovery, Redundanz, Degradation und Verification strukturiert und sicher beobachtet werden können, ohne Beobachtung mit Authority zu vermischen.