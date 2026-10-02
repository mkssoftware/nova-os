# NPSPEC-RESILIENCE-ISOLATION-0001 – Nova Resilience Failure Isolation

## Status

Angenommen

## Kategorie

Resilience / Isolation / Failure Containment

## Zweck

NovaOS definiert eine systemweite Failure Isolation, damit Fehler möglichst innerhalb ihrer ursprünglichen Failure Domain begrenzt werden und gesunde Systembereiche weiterarbeiten können.

```text
Failure
   ↓
Detect
   ↓
Classify
   ↓
Isolate
   ↓
Contain
   ↓
Preserve Healthy Domains
```

Isolation erfolgt vor einer möglichen Recovery, wenn eine weitere Fehlerausbreitung verhindert werden muss.

## Grundprinzipien

```text
Isolation ≠ Recovery
Isolation ≠ Repair
Isolation ≠ Termination
Isolation ≠ Security Sandbox
Failure Domain ≠ Process
Contained ≠ Healthy
Unknown ≠ Safe
```

Isolation soll möglichst die kleinste notwendige Systemgrenze betreffen.

## Isolation Model

```text
FailureIsolation
├── IsolationID
├── TargetID
├── DomainID
├── IsolationType
├── Reason
├── State
└── Timestamp
```

Optional:

```text
DetectionID
ClassificationID
AffectedDependencies
CapabilitySet
ResourceSet
RecoveryPolicy
Criticality
ProvenanceID
```

## Isolation States

```text
Requested
Preparing
Isolated
PartiallyIsolated
Released
Failed
Unknown
```

Eine teilweise Isolation muss explizit sichtbar bleiben.

## Isolation Scope

NovaOS muss unterschiedliche Isolationsebenen unterstützen können:

```text
Operation
Task
Process
Service
Driver
Device
Provider
Subsystem
Node
Distributed Service
```

Es gilt:

```text
Smallest Sufficient Isolation Domain
```

Ein einzelner Treiberfehler soll beispielsweise nicht automatisch das gesamte System isolieren.

## Isolation Types

Je nach Fehler können unterschiedliche Mechanismen eingesetzt werden:

```text
Execution Isolation
Memory Isolation
Capability Isolation
Resource Isolation
Device Isolation
Driver Isolation
Storage Isolation
Network Isolation
Provider Isolation
Node Isolation
```

Mehrere Isolation Types können kombiniert werden.

## Execution Isolation

Fehlerhafte Executions können:

```text
Suspend
Terminate
Quarantine
Detach
```

werden.

Andere unabhängige Executions sollen möglichst weiterlaufen.

## Capability Isolation

Eine fehlerhafte Domain kann ihre Autorität verlieren.

```text
Failure Domain
      ↓
Capability Revocation
      ↓
Restricted Authority
```

Betroffen sein können:

```text
Device Access
Storage Access
Network Access
IPC
Shared Resources
Privileged Operations
```

Isolation darf keine neuen Capabilities erzeugen.

## Memory Isolation

Speicherfehler dürfen nicht unkontrolliert andere Domains beeinflussen.

Mechanismen können umfassen:

```text
Address-Space Isolation
Mapping Removal
Shared-Memory Revocation
Page Quarantine
DMA Isolation
IOMMU Restrictions
```

Gemeinsam genutzte Speicherbereiche müssen besonders berücksichtigt werden.

## Driver und Device Isolation

Fehlerhafte Treiber oder Geräte können vom aktiven System getrennt werden.

```text
Driver Failure
     ↓
Stop Requests
     ↓
Revoke Device Capabilities
     ↓
Quiesce Device
     ↓
Isolate
```

DMA muss vor Freigabe betroffener Speicherbereiche kontrolliert beendet werden.

## Resource Isolation

Fehlerhafte Komponenten dürfen Ressourcen nicht unbegrenzt weiter verbrauchen.

Beispiele:

```text
CPU Budget
Memory
IO
Network
Energy
Handles
Queue Capacity
```

Resource Accounting liefert hierfür die notwendigen Grenzen.

## Network Isolation

Netzwerkbezogene Isolation kann umfassen:

```text
Block Connections
Disable Interface
Restrict Routes
Quarantine Endpoint
Remove Remote Provider
```

Security Policy und Sovereignty-Regeln bleiben wirksam.

## Storage Isolation

Beschädigte oder inkonsistente Storage-Domains können beispielsweise:

```text
Read-only
Detached
Quarantined
Offline
```

gesetzt werden.

Isolation darf Daten nicht automatisch löschen.

## Dependency Handling

Vor Isolation müssen abhängige Komponenten berücksichtigt werden.

```text
Failed Provider
      ↓
Consumers
├── Can Failover
├── Can Degrade
└── Must Stop
```

Isolation einer Komponente kann abhängige Guarantees ungültig machen.

Diese müssen revalidiert werden.

## Shared Resources

Besondere Vorsicht gilt bei:

```text
Shared Memory
Shared Device
Shared Cache
Shared Storage
Shared Provider
Shared Network Path
```

Isolation einer Domain darf gesunde Nutzer einer gemeinsam genutzten Ressource nicht unnötig beeinträchtigen.

## Distributed Isolation

In verteilten Systemen kann Isolation einen Node oder Provider betreffen.

```text
Cluster
├── Node A → Healthy
├── Node B → Isolated
└── Node C → Healthy
```

Ein Kommunikationsverlust allein darf nicht automatisch destruktive Isolation auslösen.

```text
Network Partition ≠ Confirmed Node Failure
```

Split-Brain-Risiken müssen berücksichtigt werden.

## Security Integration

Bei möglicher Kompromittierung kann stärkere Isolation erforderlich sein.

```text
Suspicion
   ↓
Self-Protection
   ↓
Restricted / Quarantined Domain
```

Security-Isolation kann gegenüber normaler Availability priorisiert werden.

## Recovery Integration

Nach erfolgreicher Isolation kann Recovery beginnen.

```text
Detect
  ↓
Classify
  ↓
Isolate
  ↓
Diagnose
  ↓
Recover
  ↓
Verify
  ↓
Release Isolation
```

Isolation darf erst aufgehoben werden, wenn die notwendigen Bedingungen wieder erfüllt sind.

## Isolation Release

Vor Wiedereingliederung muss geprüft werden:

```text
Health
Integrity
Dependencies
Capabilities
Resource State
Trust
Configuration
```

```text
Recovered ≠ Safe to Reintegrate
```

## Failure of Isolation

Kann eine notwendige Isolation nicht hergestellt werden:

```text
Isolation Failed
      ↓
Escalate
```

Mögliche Eskalation:

```text
Larger Isolation Domain
Subsystem Shutdown
Safe Mode
Recovery Environment
System Fail-safe
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
IsolationID
TargetID
DomainID
Isolation Type
Isolation Reason
Isolation State
DetectionID
Classification
Revoked Capabilities
Affected Resources
Affected Dependencies
Recovery State
Release Conditions
```

## Normative Anforderungen

1. NovaOS MUSS Fehler innerhalb definierter Failure Domains isolieren können.
2. Isolation MUSS von Detection, Diagnosis und Recovery getrennt bleiben.
3. NovaOS SOLL die kleinste ausreichende Isolation Domain verwenden.
4. Fehlerhafte Executions MÜSSEN kontrolliert suspendierbar oder terminierbar sein.
5. Capabilities einer isolierten Domain MÜSSEN kontrolliert einschränkbar oder widerrufbar sein.
6. Isolation DARF keine zusätzliche Autorität erzeugen.
7. Speicherzugriffe isolierter Domains MÜSSEN begrenzbar sein.
8. DMA MUSS bei Driver- oder Device-Isolation berücksichtigt werden.
9. Fehlerhafte Komponenten DÜRFEN Ressourcen NICHT unbegrenzt weiter verbrauchen.
10. Storage Isolation DARF Daten NICHT automatisch löschen.
11. Abhängigkeiten MÜSSEN bei Isolation berücksichtigt werden.
12. Betroffene Guarantees MÜSSEN nach Isolation revalidiert werden.
13. Gemeinsam genutzte Ressourcen MÜSSEN kontrolliert behandelt werden.
14. Netzwerkpartitionen DÜRFEN NICHT automatisch als bestätigter Node-Ausfall gelten.
15. Distributed Isolation MUSS Split-Brain-Risiken berücksichtigen.
16. Security-kritische Isolation DARF Availability zugunsten zwingender Security-Anforderungen einschränken.
17. Isolation DARF erst nach erfolgreicher Revalidierung aufgehoben werden.
18. Fehlgeschlagene Isolation MUSS eskalierbar sein.
19. Teilweise Isolation MUSS explizit erkennbar sein.
20. Isolation-Zustände und Auswirkungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFPROTECTION-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-SANDBOX-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `ADR-ARCH-0112`

## Ergebnis

```text
Failure
   ↓
Detection
   ↓
Classification
   ↓
Smallest Isolation Domain
   ↓
Containment
   ↓
Healthy Domains Continue
   ↓
Diagnosis + Recovery
   ↓
Verification
   ↓
Controlled Reintegration
```

NovaOS erhält damit eine systemweite Failure-Isolation-Architektur, die fehlerhafte Komponenten gezielt begrenzt, ihre Autorität und Ressourcen kontrolliert und gesunde Systembereiche möglichst unbeeinträchtigt weiterarbeiten lässt.