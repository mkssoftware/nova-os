# NPSPEC-RESILIENCE-FAULTINJECTION-0001 – Nova Resilience Fault Injection

## Status

Angenommen

## Kategorie

Resilience / Testing / Fault Injection

## Zweck

NovaOS definiert einen kontrollierten Fault-Injection-Mechanismus, mit dem Fehler gezielt simuliert oder erzeugt werden können, um Detection, Containment, Recovery und Self-Healing reproduzierbar zu testen.

```text
Test Scenario
     ↓
Inject Fault
     ↓
Observe System
     ↓
Detect + Contain
     ↓
Recovery
     ↓
Verify
```

Fault Injection dient der Validierung der Resilience-Architektur und darf den normalen Betrieb nicht unkontrolliert gefährden.

## Grundprinzipien

```text
Injected Fault ≠ Real Fault
Fault Injection ≠ Chaos Without Control
Fault Injection ≠ Security Attack
Expected Recovery ≠ Verified Recovery
Test Environment ≠ Production Permission
```

## Fault Injection Model

```text
FaultInjection
├── InjectionID
├── TargetID
├── DomainID
├── FaultType
├── Parameters
├── ExpectedBehavior
└── State
```

Optional:

```text
CampaignID
ExecutionID
FailureClass
Duration
Trigger
RecoveryPolicy
VerificationPolicy
SafetyBoundary
ProvenanceID
```

## Zustände

```text
Defined
Validated
Armed
Injecting
Active
Stopping
Completed
Aborted
Failed
Unknown
```

## Fault Types

NovaOS soll unterschiedliche Fehlerklassen injizieren können:

```text
Crash
Hang
Timeout
Latency
Dropped Message
Corrupted Data
Unavailable Dependency
Resource Exhaustion
Allocation Failure
IO Failure
Device Failure
Network Failure
Provider Failure
Integrity Failure
```

Die konkrete Unterstützung hängt vom jeweiligen Subsystem ab.

## Injection Scope

Fault Injection kann gezielt auf begrenzte Bereiche angewendet werden:

```text
Operation
Task
Process
Service
Driver
Device
Provider
Storage
Network
Subsystem
Node
Distributed Service
```

Standardmäßig soll der kleinstmögliche ausreichende Scope verwendet werden.

## Injection Points

Subsysteme können explizite Injection Points bereitstellen.

```text
Allocation
IPC Send
IPC Receive
Storage Read
Storage Write
Driver Request
Network Send
Provider Call
Scheduler Event
Transaction Phase
```

Injection Points müssen eindeutig identifizierbar und introspektierbar sein.

## Deterministic Injection

Für reproduzierbare Tests soll Fault Injection deterministisch steuerbar sein.

Beispiel:

```text
After Request 100
Inject IO Failure

or

At Execution Step X
Inject Timeout
```

Deterministic Execution und Record & Replay können zur exakten Reproduktion verwendet werden.

## Trigger

Fehler können ausgelöst werden durch:

```text
Manual Trigger
Operation Count
Time
System State
Specific Event
Deterministic Replay Point
Test Scenario
```

Produktive Systeme dürfen keine unbegrenzten oder versteckten Injection Trigger besitzen.

## Fault Campaign

Mehrere Fault Injections können zu einer Testkampagne kombiniert werden.

```text
FaultCampaign
├── Scenario
├── Injection Sequence
├── Expected States
├── Recovery Expectations
└── Verification Criteria
```

Dadurch können komplexe Fehlerketten getestet werden.

## Expected Behavior

Ein Test kann erwartetes Verhalten definieren:

```text
Failure Detected
Domain Isolated
Retry Limited
Failover Triggered
Degradation Activated
Recovery Completed
Verification Passed
```

Abweichungen müssen als Testergebnis erfasst werden.

## Safety Boundary

Jede Injection muss eine definierte Sicherheitsgrenze besitzen.

```text
Injection
   ↓
Safety Boundary
   ↓
Allowed Effects
```

Die Fault Injection darf nicht unbeabsichtigt außerhalb ihres genehmigten Testbereichs wirken.

## Capability Security

Fault Injection benötigt explizite Authority.

```text
Fault Injection Capability
          ↓
Target
Scope
Fault Types
Duration
Environment
```

```text
Debug Access ≠ Fault Injection Authority
```

Es darf keine universelle implizite Fault-Injection-Berechtigung geben.

## Production Protection

In produktiven Systemen muss Fault Injection standardmäßig eingeschränkt oder deaktiviert sein.

Aktivierung kann abhängig sein von:

```text
Developer Mode
Test Environment
Explicit User Authorization
Signed Test Policy
Physical Presence
Recovery Environment
```

Sicherheitskritische Grenzen dürfen dadurch nicht automatisch aufgehoben werden.

## Resource Faults

NovaOS soll Ressourcenfehler simulieren können:

```text
Memory Allocation Failure
CPU Starvation
IO Saturation
Storage Exhaustion
Network Congestion
Energy Constraint
```

Reale Ressourcen müssen dabei nicht zwingend vollständig erschöpft werden.

## Distributed Fault Injection

Verteilte Tests können simulieren:

```text
Node Failure
Network Partition
Packet Loss
High Latency
Replica Failure
Provider Unavailability
```

Dabei muss zwischen simuliertem Node-Ausfall und simulierter Kommunikationsstörung unterschieden werden.

## Recovery Validation

Fault Injection soll insbesondere prüfen:

```text
Detection
Classification
Isolation
Containment
Retry
Backoff
Circuit Breaker
Restart
Failover
Rollback
Degradation
Self-Healing
Recovery Verification
```

## Verification

Nach einem Test muss sowohl das erwartete Recovery-Verhalten als auch der Systemzustand geprüft werden.

```text
Inject
   ↓
Recover
   ↓
Verify Expected Behavior
   ↓
Verify System Integrity
```

Ein bestandener Recovery-Pfad darf nicht angenommen werden, nur weil das System weiterläuft.

## Cleanup

Nach Fault Injection muss der künstlich erzeugte Zustand entfernt werden.

```text
Stop Injection
     ↓
Cleanup
     ↓
Verify
     ↓
Normal State
```

Bleibt der Systemzustand unsicher, muss Recovery oder Isolation erfolgen.

## Provenance

Jede Injection muss nachvollziehbar sein.

```text
Who Authorized
What Was Injected
Target
Parameters
Timestamp
Expected Behavior
Observed Behavior
Recovery Actions
Verification Result
```

In Logs muss klar zwischen realen und injizierten Fehlern unterschieden werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
InjectionID
CampaignID
Target
Fault Type
Injection Point
Current State
Safety Boundary
Expected Behavior
Observed Behavior
Recovery State
Verification Result
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Fault Injection für Resilience-Tests unterstützen können.
2. Injizierte Fehler MÜSSEN eindeutig von real beobachteten Fehlern unterscheidbar sein.
3. Fault Injection MUSS explizite Targets und Scopes verwenden.
4. Der kleinstmögliche ausreichende Injection Scope SOLL bevorzugt werden.
5. Injection Points MÜSSEN eindeutig identifizierbar sein.
6. Deterministische Fault Injection SOLL unterstützt werden.
7. Fault Injection MUSS explizite Capability Authority benötigen.
8. Debug Authority DARF NICHT automatisch Fault-Injection-Authority bedeuten.
9. Fault Injection MUSS durch Safety Boundaries begrenzbar sein.
10. Produktive Systeme MÜSSEN Fault Injection standardmäßig einschränken können.
11. Fault Injection DARF Security- und Trust-Grenzen NICHT implizit umgehen.
12. Ressourcenfehler SOLLEN ohne tatsächliche vollständige Ressourcenerschöpfung simulierbar sein.
13. Distributed Fault Injection MUSS Kommunikations- und Node-Fehler unterscheiden können.
14. Testkampagnen SOLLEN mehrere kontrollierte Fault Injections kombinieren können.
15. Erwartetes Recovery-Verhalten MUSS definierbar sein.
16. Recovery MUSS nach Fault Injection verifiziert werden können.
17. Nach Injection MUSS ein kontrollierter Cleanup erfolgen.
18. Ein unsicherer Zustand nach Cleanup MUSS Recovery oder Isolation auslösen können.
19. Injizierte Fehler und ihre Auswirkungen MÜSSEN nachvollziehbar protokolliert werden.
20. Fault-Injection-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-WATCHDOG-0001`
- `NPSPEC-RESILIENCE-FAILOVER-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-REALTIME-REPLAY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0129`

## Ergebnis

```text
Define Scenario
      ↓
Authorize Injection
      ↓
Validate Safety Boundary
      ↓
Inject Fault
      ↓
Observe Detection + Recovery
      ↓
Verify Expected Behavior
      ↓
Cleanup
      ↓
Verify System State
      ↓
Test Result
```

NovaOS erhält damit einen kontrollierten Fault-Injection-Mechanismus, mit dem Fehler gezielt, reproduzierbar und sicher erzeugt werden können, um die tatsächliche Funktionsfähigkeit von Detection, Containment, Recovery, Failover und Self-Healing systematisch zu überprüfen.