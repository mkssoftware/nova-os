# NPSPEC-RESILIENCE-CIRCUITBREAKER-0001 – Nova Resilience Circuit Breaker

## Status

Angenommen

## Kategorie

Resilience / Failure Containment / Retry Control

## Zweck

NovaOS definiert einen systemweiten Circuit Breaker, der wiederholte Zugriffe auf fehlerhafte oder überlastete Komponenten temporär unterbindet.

```text
Requests
   ↓
Failures
   ↓
Failure Threshold
   ↓
Circuit Open
   ↓
Cooldown
   ↓
Probe
   ↓
Recover / Reopen
```

Der Circuit Breaker verhindert, dass wiederholte Retries einen bestehenden Fehler verstärken oder weitere Systembereiche überlasten.

## Grundprinzipien

```text
Circuit Breaker ≠ Retry
Circuit Breaker ≠ Backoff
Circuit Breaker ≠ Rate Limit
Circuit Breaker ≠ Failure Isolation
Circuit Open ≠ Component Failed
Circuit Closed ≠ Component Healthy
Probe Success ≠ Full Recovery
```

## Circuit Breaker Model

```text
CircuitBreaker
├── CircuitID
├── TargetID
├── DomainID
├── FailureThreshold
├── Cooldown
├── State
└── Policy
```

Optional:

```text
FailureCount
SuccessCount
ObservationWindow
ProbeLimit
FailureClass
ProviderID
ExecutionContractID
LastTransition
ProvenanceID
```

## States

NovaOS verwendet mindestens:

```text
Closed
Open
HalfOpen
```

### Closed

```text
Requests
   ↓
Target
```

Operationen werden normal zugelassen und relevante Fehler beobachtet.

### Open

Wird der definierte Failure Threshold überschritten:

```text
Closed
   ↓
Threshold Exceeded
   ↓
Open
```

Neue Operationen werden schnell abgewiesen oder zu einem zulässigen Fallback weitergeleitet.

### HalfOpen

Nach Ablauf der Cooldown-Phase:

```text
Open
  ↓
Cooldown
  ↓
HalfOpen
  ↓
Limited Probe
```

Nur eine begrenzte Anzahl kontrollierter Probe-Operationen wird zugelassen.

## State Transitions

```text
Closed
  │
  │ Failure Threshold
  ▼
Open
  │
  │ Cooldown
  ▼
HalfOpen
  ├── Success → Closed
  └── Failure → Open
```

Übergänge müssen policy-gesteuert und introspektierbar sein.

## Failure Threshold

Die Öffnung kann abhängig sein von:

```text
Failure Count
Failure Rate
Failure Class
Timeout Rate
Resource Pressure
Observation Window
Criticality
```

Ein einzelner Fehler muss nicht zwangsläufig den Circuit öffnen.

Kritische Fehler dürfen jedoch eine sofortige Öffnung auslösen.

## Failure Classification

Nicht jeder Fehler darf gleich behandelt werden.

```text
Transient
→ Count / Observe

Repeated Transient
→ Open Possible

Persistent
→ Open

Critical
→ Immediate Containment Possible
```

Nicht relevante Fehler dürfen den Circuit-Zustand nicht verfälschen.

## Cooldown

Ein geöffneter Circuit besitzt eine definierte Ruhephase.

```text
Open
 ↓
Cooldown
 ↓
HalfOpen
```

Cooldown kann statisch oder kontrolliert adaptiv bestimmt werden.

```text
Cooldown ≠ Guaranteed Recovery Time
```

## Half-Open Probes

Probe-Operationen müssen begrenzt sein.

```text
HalfOpen
├── Probe 1
├── Probe 2
└── Probe N
```

Normale Last darf während der Prüfung nicht sofort vollständig wiederhergestellt werden.

## Retry Integration

Circuit Breaker und Retry arbeiten zusammen:

```text
Failure
   ↓
Retry
   ↓
Repeated Failure
   ↓
Circuit Open
   ↓
Retries Suppressed
```

Ein offener Circuit muss weitere automatische Retries für das betroffene Ziel unterbinden können.

## Backoff Integration

Backoff kontrolliert den Abstand zwischen Versuchen.

Circuit Breaker kontrolliert, ob Versuche überhaupt zugelassen werden.

```text
Backoff → When to Retry
Circuit Breaker → Whether to Retry
```

## Provider Failover

Ein geöffneter Circuit für einen Provider muss nicht die gesamte Capability blockieren.

```text
Provider A
→ Circuit Open

Provider B
→ Available
```

Failover muss weiterhin erfüllen:

```text
Capability
Trust
Security
Sovereignty
ExecutionContract
Resource Constraints
```

## Dependency Protection

Circuit Breaker schützen auch abhängige Systeme.

```text
Failed Service
     ↓
Circuit Open
     ↓
Requests Stopped
     ↓
Upstream Protected
```

Dadurch können kaskadierende Fehler reduziert werden.

## Failure Domains

Circuit Breaker können gebunden sein an:

```text
Operation
Endpoint
Connection
Provider
Service
Device
Failure Domain
```

Die Scope muss explizit definiert sein.

## Distributed Systems

Circuit-Zustände können lokal bleiben oder kontrolliert zwischen Nodes geteilt werden.

```text
Local Observation ≠ Global Failure
```

Ein Node darf eine lokale Netzwerkstörung nicht automatisch als globalen Provider-Ausfall verbreiten.

## Realtime Integration

Ein offener Circuit ermöglicht schnelles Scheitern:

```text
Request
   ↓
Circuit Open
   ↓
Immediate Failure / Fallback
```

Dadurch können unnötige Timeouts vermieden werden.

Hard-Realtime-Fallbacks müssen bereits vorab die erforderlichen Garantien erfüllen.

## Recovery Integration

Der Circuit Breaker repariert keine Komponente.

```text
Circuit Open
     ↓
Self-Healing / Recovery
     ↓
Verification
     ↓
HalfOpen Probe
     ↓
Closed
```

Recovery und Circuit State bleiben getrennte Konzepte.

## Adaptive Integration

NovaOS darf Schwellenwerte innerhalb definierter Grenzen adaptiv optimieren.

Adaptive Logik darf jedoch keine Hard Constraints, Security Policies oder Realtime-Garantien umgehen.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
CircuitID
TargetID
State
Failure Count
Failure Rate
Failure Threshold
Observation Window
Cooldown
Probe State
Failure Classification
Last Transition
Provider
Fallback State
```

## Normative Anforderungen

1. NovaOS MUSS Circuit Breaker für wiederholt fehlschlagende Abhängigkeiten unterstützen können.
2. Circuit Breaker MUSS von Retry, Backoff, Isolation und Recovery getrennt bleiben.
3. Closed, Open und HalfOpen MÜSSEN unterstützt werden.
4. Failure Thresholds MÜSSEN definierbar sein.
5. Thresholds SOLLEN Failure Classification berücksichtigen können.
6. Kritische Fehler MÜSSEN eine sofortige Eskalation ermöglichen.
7. Ein offener Circuit MUSS weitere automatische Requests begrenzen können.
8. Open State MUSS eine definierte Cooldown-Policy besitzen.
9. HalfOpen MUSS nur kontrollierte Probe-Operationen zulassen können.
10. Probe-Anzahl MUSS begrenzbar sein.
11. Probe-Erfolg DARF NICHT automatisch vollständige Systemgesundheit beweisen.
12. Circuit Breaker MÜSSEN Retry Storms begrenzen können.
13. Backoff und Circuit Breaker MÜSSEN kombinierbar sein.
14. Provider-spezifische Circuits MÜSSEN möglich sein.
15. Ein Provider-Circuit DARF alternative zulässige Provider NICHT unnötig blockieren.
16. Distributed Circuit States DÜRFEN lokale Fehler NICHT automatisch als globale Fehler interpretieren.
17. Realtime-Fallbacks MÜSSEN bestehende Realtime-Garantien erfüllen.
18. Adaptive Thresholds MÜSSEN durch Hard Constraints begrenzt bleiben.
19. Circuit-Zustandsänderungen MÜSSEN nachvollziehbar sein.
20. Circuit-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-RETRY-0001`
- `NPSPEC-RESILIENCE-BACKOFF-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `ADR-ARCH-0119`

## Ergebnis

```text
Requests
   ↓
Failure Monitoring
   ↓
Threshold Exceeded?
├── No → Closed
└── Yes
      ↓
     Open
      ↓
 Suppress Requests
      ↓
   Cooldown
      ↓
   HalfOpen
      ↓
 Limited Probes
   ├── Success → Closed
   └── Failure → Open
```

NovaOS erhält damit einen systemweiten Circuit-Breaker-Mechanismus, der wiederholt fehlschlagende Abhängigkeiten temporär aus dem aktiven Ausführungspfad nimmt, Retry-Stürme und kaskadierende Fehler begrenzt und eine kontrollierte Wiederaufnahme nach Recovery ermöglicht.