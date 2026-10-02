# NPSPEC-RESILIENCE-CONTAINMENT-0001 – Nova Resilience Failure Containment

## Status

Angenommen

## Kategorie

Resilience / Containment / Failure Management

## Zweck

NovaOS definiert ein systemweites Failure-Containment-Modell, das die Auswirkungen erkannter Fehler begrenzt und eine weitere Ausbreitung auf gesunde Systembereiche verhindert.

```text
Failure
   ↓
Detection
   ↓
Classification
   ↓
Isolation
   ↓
Containment
   ↓
Stable Boundary
   ↓
Diagnosis / Recovery
```

Isolation stellt die technische Trennung her. Containment stellt sicher, dass Auswirkungen innerhalb der vorgesehenen Grenze bleiben.

## Grundprinzipien

```text
Containment ≠ Isolation
Containment ≠ Recovery
Containment ≠ Repair
Containment ≠ Termination
Contained ≠ Healthy
Contained ≠ Recovered
Unknown ≠ Contained
```

Containment hat Vorrang vor Recovery, wenn eine weitere Fehlerausbreitung möglich ist.

## Containment Model

```text
FailureContainment
├── ContainmentID
├── DomainID
├── FailureID
├── Boundary
├── ContainmentActions
├── State
└── VerificationState
```

Optional:

```text
DetectionID
ClassificationID
IsolationID
AffectedResources
AffectedDependencies
RevokedCapabilities
Criticality
PropagationRisk
RecoveryPolicy
ProvenanceID
```

## Containment States

```text
Requested
Establishing
Contained
PartiallyContained
ContainmentFailed
Released
Unknown
```

```text
Isolation Complete ≠ Containment Verified
```

## Containment Boundary

Die Containment Boundary definiert die maximale zulässige Ausbreitung eines Fehlers.

```text
Healthy System
┌────────────────────────────┐
│                            │
│   ┌──────────────────┐     │
│   │ Failure Domain   │     │
│   │                  │     │
│   │     Failure      │     │
│   └──────────────────┘     │
│          ↑ Boundary        │
└────────────────────────────┘
```

Mögliche Grenzen:

```text
Task
Process
Service
Driver
Device
Provider
Subsystem
Node
Storage Domain
Network Domain
```

## Containment Actions

NovaOS kann abhängig vom Fehler kombinieren:

```text
Suspend Execution
Terminate Execution
Revoke Capabilities
Block IPC
Remove Memory Mapping
Stop DMA
Restrict Resources
Quarantine Device
Disable Provider
Block Network Communication
Freeze Storage Writes
Detach Dependency
```

Die Maßnahmen sollen minimal ausreichend sein.

## Propagation Control

Containment muss bekannte Ausbreitungspfade berücksichtigen.

```text
Failure
├── Shared Memory
├── IPC
├── DMA
├── Storage
├── Network
├── Shared Device
└── Dependencies
```

Ein isolierter Prozess gilt nicht als vollständig contained, solange beispielsweise unkontrolliertes DMA weiterlaufen kann.

## State Containment

Neben Ressourcen müssen auch Zustandsänderungen begrenzt werden.

```text
Faulty Component
      ↓
Invalid State Change
      X
Protected System State
```

Transaktionen, Versionierung und Rollback sollen verhindern, dass teilweise ausgeführte Änderungen dauerhaft andere Domains beschädigen.

## Resource Containment

Fehlerhafte Domains dürfen Ressourcen nicht unbegrenzt verbrauchen.

```text
CPU
Memory
IO
Network
Storage
Energy
Queue Capacity
Handles
```

Resource Budgets bleiben während eines Fehlers durchsetzbar.

## Dependency Containment

Abhängige Komponenten müssen kontrolliert reagieren können.

```text
Failed Provider
      ↓
Consumers
├── Failover
├── Degrade
├── Pause
└── Stop
```

Ein Fehler soll nicht allein durch unkontrollierte Retry-Ketten weitere Domains überlasten.

## Cascading Failures

NovaOS muss Kaskadeneffekte berücksichtigen.

```text
Failure A
   ↓
Resource Pressure
   ↓
Failure B
   ↓
Failure C
```

Gegenmaßnahmen können sein:

```text
Retry Limits
Backoff
Circuit Breaker
Load Shedding
Resource Limits
Dependency Isolation
```

## Distributed Containment

Bei verteilten Systemen können Nodes oder Provider quarantänisiert werden.

```text
Node A ←→ Node B ←→ Node C
             ↓
           Failure
             ↓
         Containment
```

Netzwerkpartitionen dürfen nicht automatisch als bestätigte Fehler interpretiert werden.

Split-Brain muss verhindert oder kontrolliert werden.

## Security Containment

Bei möglicher Kompromittierung können strengere Maßnahmen gelten:

```text
Revoke Authority
Block Communication
Protect Credentials
Disable Provider
Quarantine Domain
Preserve Evidence
```

Security und Safety dürfen höhere Priorität als Availability besitzen.

## Containment Verification

Nach Anwendung der Maßnahmen muss geprüft werden:

```text
Containment Actions
        ↓
Verify Boundary
        ↓
Propagation Stopped?
     ├── Yes → Contained
     └── No  → Escalate
```

Containment darf nicht allein aufgrund ausgeführter Aktionen als erfolgreich gelten.

## Escalation

Reicht die aktuelle Grenze nicht aus:

```text
Process
   ↓
Service
   ↓
Subsystem
   ↓
Node
   ↓
System
```

NovaOS erweitert die Containment Domain nur soweit erforderlich.

## Recovery Integration

Erst nach stabiler Containment Boundary soll Recovery beginnen.

```text
Contain
   ↓
Diagnose
   ↓
Recover
   ↓
Verify
   ↓
Reintegrate
```

Recovery darf die Containment Boundary nicht unkontrolliert aufheben.

## Release

Containment darf erst aufgehoben werden, wenn relevante Bedingungen erfüllt sind:

```text
Health Valid
Integrity Valid
Recovery Verified
Dependencies Valid
Trust Valid
Capabilities Revalidated
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
ContainmentID
Failure Domain
Boundary
Containment State
Propagation Risk
Active Actions
Revoked Capabilities
Restricted Resources
Affected Dependencies
Verification State
Escalation Level
Release Conditions
```

## Normative Anforderungen

1. NovaOS MUSS Failure Containment als eigenständigen Resilience-Schritt unterstützen.
2. Containment MUSS von Isolation, Diagnosis und Recovery getrennt bleiben.
3. Containment SOLL die kleinste ausreichende Boundary verwenden.
4. Bekannte Fehlerausbreitungspfade MÜSSEN berücksichtigt werden.
5. IPC-, Memory-, DMA-, Storage- und Network-Ausbreitung MÜSSEN begrenzbar sein.
6. Fehlerhafte Domains DÜRFEN Ressourcen NICHT unbegrenzt verbrauchen.
7. Capability Authority MUSS bei Bedarf widerrufbar oder einschränkbar sein.
8. Unkontrollierte Retry-Kaskaden MÜSSEN begrenzbar sein.
9. Cascading Failures SOLLEN erkannt und eingedämmt werden können.
10. Zustandsänderungen SOLLEN transaktional begrenzt werden.
11. Distributed Containment MUSS Netzwerkpartitionen und Split-Brain berücksichtigen.
12. Security-kritisches Containment DARF Availability einschränken, wenn zwingende Schutzanforderungen dies verlangen.
13. Containment MUSS nach Anwendung der Maßnahmen verifiziert werden.
14. Ausgeführte Containment Actions DÜRFEN NICHT automatisch als erfolgreiches Containment gelten.
15. Fehlgeschlagenes Containment MUSS eskalierbar sein.
16. Eskalation SOLL die kleinste ausreichende größere Domain verwenden.
17. Recovery DARF eine aktive Containment Boundary NICHT unkontrolliert umgehen.
18. Containment DARF erst nach erfolgreicher Revalidierung aufgehoben werden.
19. `Unknown` DARF NICHT als erfolgreich contained interpretiert werden.
20. Containment-Zustände und Auswirkungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFPROTECTION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RECLAIM-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0113`

## Ergebnis

```text
Failure
   ↓
Detect + Classify
   ↓
Isolate
   ↓
Establish Containment Boundary
   ↓
Stop Propagation
   ↓
Verify Containment
   ├── Failed → Escalate
   └── Valid
         ↓
      Diagnose
         ↓
      Recover
         ↓
      Revalidate
         ↓
      Release
```

NovaOS erhält damit eine systemweite Failure-Containment-Schicht, die Fehlerausbreitung über Execution, Ressourcen, Speicher, IPC, DMA, Storage, Netzwerk und Abhängigkeiten kontrolliert begrenzt und eine stabile Grundlage für Diagnosis und Recovery schafft.