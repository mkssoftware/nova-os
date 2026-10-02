# NPSPEC-UPDATE-CANARY-0001 – Nova Canary Update

## Status

Angenommen

## Kategorie

Update / Canary Deployment / Risk Control

## Zweck

NovaOS definiert Canary Updates als kontrollierte Einführung einer neuen Version auf einer kleinen Teilmenge geeigneter Instanzen, bevor ein vollständiger Rollout erfolgt.

```text
Stable Population
       ↓
Select Canary
       ↓
Deploy New Version
       ↓
Observe + Verify
      ↙           ↘
   Healthy       Failure
      ↓             ↓
   Expand        Stop /
   Rollout       Rollback
```

Canary Updates sollen Fehler früh erkennen und den möglichen Auswirkungsbereich eines fehlerhaften Updates begrenzen.

## Grundprinzipien

```text
Canary ≠ Rolling Update
Canary ≠ Testing Environment
Canary Healthy ≠ Globally Safe
Small Deployment ≠ Small Risk
Observation ≠ Verification
No Detected Failure ≠ Proven Correct
Canary Success ≠ Automatic Rollout Authorization
```

## Canary Model

```text
CanaryUpdate
├── UpdateID
├── DeploymentID
├── TargetVersion
├── BaselineVersion
├── CanarySet
├── SelectionPolicy
├── ObservationPolicy
├── SuccessCriteria
├── FailureCriteria
└── State
```

Optional:

```text
CanarySize
ObservationWindow
ResourceBudget
RollbackPolicy
ExpansionPolicy
ExecutionContract
ProvenanceID
```

## States

```text
Planned
Preparing
Deploying
Observing
Verified
Expanding
Completed
Paused
Failed
RollingBack
Unknown
```

`Unknown` darf nicht als erfolgreicher Canary interpretiert werden.

## Canary Selection

Canary-Instanzen müssen kontrolliert ausgewählt werden.

Kriterien können sein:

```text
Hardware Class
Architecture
Workload Type
Region
Failure Domain
Provider Type
Risk Class
Resource Profile
```

Die Auswahl soll ausreichend repräsentativ für den geplanten Rollout sein.

## Failure Domain

Canary-Instanzen sollen so gewählt werden, dass ein Fehler möglichst begrenzt bleibt.

```text
Canary
  ↓
Limited Failure Domain
```

Kritische redundante Instanzen dürfen nicht gleichzeitig so ausgewählt werden, dass ein gemeinsamer Ausfall die Gesamtverfügbarkeit gefährdet.

## Deployment

```text
Validate Update
      ↓
Select Canary Set
      ↓
Deploy
      ↓
Activate
      ↓
Observe
      ↓
Verify
```

Nicht ausgewählte Instanzen bleiben zunächst auf der bekannten Version.

## Baseline Comparison

Canary-Instanzen können mit einer stabilen Baseline verglichen werden.

```text
Stable Group v1
      ↕
Comparison
      ↕
Canary Group v2
```

Verglichen werden können:

```text
Error Rate
Latency
Resource Consumption
Health State
Contract Violations
Crash Rate
Availability
Functional Results
```

## Observation Window

Nach Aktivierung muss eine definierte Beobachtungsphase möglich sein.

```text
Deploy
  ↓
Observe
  ↓
Decision
```

Die Länge kann abhängig sein von:

```text
Risk
Workload
Update Type
Failure History
Expected Event Frequency
```

## Success Criteria

Erfolgskriterien müssen vor oder während der Planung explizit festgelegt werden.

Beispiele:

```text
Health = Healthy
No Critical Contract Violations
Error Rate within Limit
Latency within Limit
No Integrity Failures
No Security Violations
```

```text
No Error Observed
≠
Success Criteria Satisfied
```

## Failure Criteria

Der Canary muss automatisch stoppbar sein bei:

```text
Crash
Integrity Failure
Security Violation
Health Degradation
Contract Violation
Error Threshold Exceeded
Latency Threshold Exceeded
Resource Limit Violation
```

## Expansion

Nach erfolgreicher Canary-Phase kann der Rollout schrittweise erweitert werden.

```text
Canary 1%
   ↓
Verified
   ↓
10%
   ↓
Verified
   ↓
25%
   ↓
Verified
   ↓
Rolling Update
```

Prozentwerte sind Policy-Beispiele und keine festen NovaOS-Vorgaben.

## Rollback

Bei Fehlern:

```text
Canary Failure
      ↓
Stop Expansion
      ↓
Contain
      ↓
Rollback Canary
      ↓
Verify Recovery
```

Nicht betroffene Instanzen sollen unverändert bleiben.

## Security

Security-kritische Fehler müssen sofortige Eskalation ermöglichen.

Ein Canary darf keine Sicherheitsrichtlinien lockern, nur um eine neue Version weiter testen zu können.

```text
Security Constraint
>
Canary Optimization
```

## State Migration

Canary Updates müssen Shared-State-Kompatibilität berücksichtigen.

```text
Stable v1
+
Canary v2
+
Shared State
```

State Migration darf verbleibende v1-Instanzen nicht inkompatibel machen.

Nicht rückwärtskompatible Migrationen können Canary Deployment ausschließen.

## Capability Safety

Canary-Instanzen erhalten keine zusätzlichen Rechte allein aufgrund ihres Canary-Status.

```text
Canary Status ≠ Additional Authority
```

Capabilities müssen normal validiert werden.

## Rolling Update Integration

Canary kann als Vorstufe eines Rolling Updates dienen.

```text
Canary
   ↓
Verification
   ↓
Rolling Update
```

Beide Phasen bleiben logisch unterscheidbar.

## Resource Economy

Parallel betriebene Versionen können zusätzliche Ressourcen benötigen.

Zu berücksichtigen sind:

```text
CPU
Memory
Storage
Network
Energy
Temporary Capacity
Observability Cost
```

## Observability

Canary Updates benötigen ausreichende Telemetrie.

```text
Metrics
Logs
Traces
Health
Contracts
Resource Usage
Security Events
```

Fehlende Beobachtbarkeit kann den Rollout blockieren.

```text
Cannot Observe
→
Cannot Safely Promote
```

## Adaptive Decisions

Adaptive Systeme dürfen Canary-Daten analysieren und Empfehlungen erzeugen.

Sie dürfen harte Kriterien nicht überschreiben.

```text
Adaptive Recommendation
<
Security
Safety
Trust
Explicit Update Policy
```

## Provenance

NovaOS soll nachvollziehen können:

```text
UpdateID
DeploymentID
CanarySet
Selection Reason
Baseline
Target Version
Observation Window
Metrics
Success Criteria
Failure Criteria
Decision
Rollback Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Canary State
Canary Instances
Baseline Version
Target Version
Observation Progress
Health
Metrics
Success Criteria
Failure Criteria
Expansion State
Rollback Availability
```

## Normative Anforderungen

1. NovaOS MUSS Canary Updates für geeignete Deployments unterstützen können.
2. Canary Updates MÜSSEN eine explizite Canary-Menge besitzen.
3. Canary-Auswahl MUSS kontrollierbar sein.
4. Canary-Auswahl SOLL Failure Domains berücksichtigen.
5. Canary Deployment DARF harte Availability Constraints NICHT verletzen.
6. Nicht ausgewählte Instanzen SOLLEN zunächst unverändert bleiben.
7. Canary und stabile Baseline MÜSSEN unterscheidbar bleiben.
8. Eine definierte Observation Phase MUSS unterstützt werden.
9. Erfolgskriterien MÜSSEN explizit definierbar sein.
10. Fehlerkriterien MÜSSEN explizit definierbar sein.
11. Kritische Fehler MÜSSEN weitere Expansion stoppen können.
12. Canary-Erfolg DARF NICHT automatisch globale Sicherheit beweisen.
13. Expansion MUSS schrittweise erfolgen können.
14. Jede Expansion SOLL erneut verifizierbar sein.
15. Fehlgeschlagene Canary Updates MÜSSEN Rollback oder Recovery ermöglichen.
16. Shared-State-Kompatibilität MUSS berücksichtigt werden.
17. Inkompatible irreversible Migrationen MÜSSEN Canary Deployment blockieren können.
18. Canary-Status DARF Capabilities NICHT erweitern.
19. Security Policy DARF für Canary Tests NICHT implizit abgeschwächt werden.
20. Canary Updates MÜSSEN mit Rolling Updates kombinierbar sein.
21. Resource Budgets MÜSSEN berücksichtigt werden.
22. Ausreichende Observability MUSS vor Promotion verfügbar sein.
23. Adaptive Entscheidungen DÜRFEN harte Update-Kriterien NICHT überschreiben.
24. `Unknown` DARF NICHT als erfolgreicher Canary interpretiert werden.
25. Canary-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
26. Canary-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-ROLLING-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `ADR-ARCH-0179`

## Ergebnis

```text
Stable Deployment
       ↓
Select Canary
       ↓
Deploy Target Version
       ↓
Observe + Compare
       ↓
Verify Criteria
      ↙           ↘
   Success       Failure
      ↓             ↓
Expand Rollout   Stop Expansion
      ↓             ↓
Rolling Update   Rollback Canary
```

NovaOS erhält damit einen risikobegrenzten Update-Mechanismus, bei dem neue Versionen zunächst auf einer kontrollierten Teilmenge realer Instanzen ausgeführt und anhand expliziter Kriterien beobachtet werden, bevor der Rollout auf weitere Systeme ausgeweitet wird.