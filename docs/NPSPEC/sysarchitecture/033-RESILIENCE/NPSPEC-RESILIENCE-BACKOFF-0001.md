# NPSPEC-RESILIENCE-BACKOFF-0001 – Nova Resilience Backoff

## Status

Angenommen

## Kategorie

Resilience / Recovery / Retry Control

## Zweck

NovaOS definiert ein systemweites Backoff-Modell, das wiederholte Recovery-, Retry- und Reconnect-Versuche zeitlich begrenzt und dadurch Überlastung, Retry-Stürme und kaskadierende Fehler verhindert.

```text
Failure
   ↓
Retry Decision
   ↓
Backoff
   ↓
Wait
   ↓
Retry
```

Backoff ist ein Kontrollmechanismus und keine eigenständige Recovery-Strategie.

## Grundprinzipien

```text
Backoff ≠ Retry
Backoff ≠ Recovery
Backoff ≠ Timeout
Backoff ≠ Rate Limit
Waiting ≠ Progress
Longer Backoff ≠ Better Recovery
```

Backoff muss immer mit Retry Budget, Deadline und Failure Classification zusammenarbeiten.

## Backoff Model

```text
BackoffPolicy
├── PolicyID
├── Strategy
├── InitialDelay
├── MaximumDelay
├── Attempt
├── JitterPolicy
└── State
```

Optional:

```text
RetryID
FailureID
DomainID
Multiplier
Deadline
RetryBudget
Cooldown
Criticality
ProviderID
ProvenanceID
```

## Backoff Strategies

NovaOS soll mindestens unterstützen:

```text
Fixed
Linear
Exponential
Adaptive
PolicyDefined
```

### Fixed

```text
1s → 1s → 1s → 1s
```

### Linear

```text
1s → 2s → 3s → 4s
```

### Exponential

```text
1s → 2s → 4s → 8s
```

Eine maximale Verzögerung muss definierbar sein.

## Jitter

Synchronisierte Clients können ohne Jitter gleichzeitig erneut versuchen.

```text
Failure
  ↓
1000 Clients
  ↓
Same Backoff
  ↓
1000 Retries
```

Jitter verteilt diese Versuche zeitlich.

```text
Base Backoff
     +
Controlled Jitter
     ↓
Retry Delay
```

Jitter muss innerhalb definierter Grenzen bleiben.

## Retry Storm Prevention

Backoff dient insbesondere der Vermeidung positiver Rückkopplungen:

```text
Failure
   ↓
Retries
   ↓
Higher Load
   ↓
More Failures
   ↓
More Retries
```

NovaOS kann Backoff kombinieren mit:

```text
Retry Budget
Rate Limiting
Circuit Breaker
Load Shedding
Resource Accounting
Failure Containment
```

## Deadline Awareness

Backoff darf nicht unabhängig von einer Deadline berechnet werden.

```text
Backoff Delay
      <
Remaining Useful Time
```

Wenn nach Ablauf des Backoffs keine sinnvolle Ausführung mehr möglich ist:

```text
Do Not Retry
     ↓
Cancel / Fail / Escalate
```

## Retry Budget Integration

Backoff setzt Retry Budgets nicht außer Kraft.

```text
Retry Budget
├── Attempts
├── Duration
├── Resources
└── Deadline
```

```text
Backoff ≠ Budget Extension
```

## Adaptive Backoff

NovaOS darf Backoff anhand beobachteter Bedingungen anpassen.

Mögliche Eingaben:

```text
Failure Frequency
Provider Load
Resource Pressure
Network Congestion
Previous Recovery Time
Failure Classification
```

Adaptive Entscheidungen bleiben durch Hard Constraints und Policy begrenzt.

## Failure-Class Awareness

Unterschiedliche Fehler können unterschiedliche Backoff-Regeln verwenden.

```text
Transient
→ Short Backoff

Repeated Transient
→ Increasing Backoff

Persistent
→ Stop Retry / Escalate

Critical
→ Immediate Containment
```

Backoff darf persistente oder kritische Fehler nicht durch endloses Warten verdecken.

## Provider Awareness

Bei mehreren Providern kann Backoff providerbezogen sein.

```text
Provider A
→ Backoff

Provider B
→ Available
```

Ein Backoff für einen Provider muss nicht zwangsläufig die gesamte Capability blockieren.

## Domain Backoff

Backoff kann auf unterschiedlichen Ebenen gelten:

```text
Operation
Connection
Provider
Service
Device
Failure Domain
```

Dadurch können wiederholte Fehler einer ganzen Domain begrenzt werden.

## Cooldown

Nach wiederholten Fehlern kann eine längere Ruhephase aktiviert werden.

```text
Repeated Failures
      ↓
Cooldown
      ↓
Revalidation
      ↓
Retry Allowed?
```

Cooldown kann mit Circuit-Breaker-Verhalten kombiniert werden.

## Distributed Systems

Verteilte Systeme müssen Backoff verwenden können, ohne globale Synchronisation vorauszusetzen.

Jitter ist insbesondere bei vielen Nodes wichtig.

```text
Node A → 1.2s
Node B → 1.8s
Node C → 1.4s
```

Dadurch werden synchronisierte Recovery-Wellen reduziert.

## Realtime Integration

Hard-Realtime-Operationen dürfen nicht von unbeschränktem Backoff abhängen.

```text
Deadline
   ↓
Remaining Budget
   ↓
Backoff Fits?
├── Yes → Wait
└── No  → Fail / Escalate
```

## Cancellation

Wartende Backoff-Operationen müssen abbrechbar sein.

```text
Backoff
   ↓
Cancellation
   ↓
No Retry
```

Structured Concurrency bleibt gültig.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
PolicyID
RetryID
Strategy
Attempt
Current Delay
Maximum Delay
Jitter
Remaining Retry Budget
Remaining Deadline
Failure Class
Cooldown State
Next Retry
```

## Normative Anforderungen

1. NovaOS MUSS kontrolliertes Backoff für wiederholte Retry-Versuche unterstützen können.
2. Backoff MUSS von Retry, Timeout und Recovery getrennt bleiben.
3. Fixed, Linear und Exponential Backoff SOLLEN unterstützt werden.
4. Eine maximale Backoff-Dauer MUSS definierbar sein.
5. Jitter SOLL zur Vermeidung synchronisierter Retry-Wellen unterstützt werden.
6. Jitter MUSS innerhalb definierter Grenzen bleiben.
7. Backoff MUSS Retry Budgets respektieren.
8. Backoff DARF ein Retry Budget NICHT implizit verlängern.
9. Backoff MUSS verbleibende Deadlines berücksichtigen.
10. Ein Retry DARF nach Backoff NICHT erfolgen, wenn seine relevante Deadline nicht mehr sinnvoll erreichbar ist.
11. Backoff SOLL Failure Classification berücksichtigen.
12. Persistente Fehler DÜRFEN NICHT durch unbegrenzten Backoff verdeckt werden.
13. Kritische Fehler MÜSSEN weiterhin sofortiges Containment ermöglichen.
14. Provider-spezifisches Backoff MUSS möglich sein.
15. Backoff SOLL auf Operation-, Provider- und Failure-Domain-Ebene anwendbar sein.
16. Wiederholte Fehler SOLLEN einen Cooldown auslösen können.
17. Adaptive Backoff-Entscheidungen MÜSSEN Hard Constraints respektieren.
18. Hard-Realtime-Ausführung DARF NICHT von unbeschränktem Backoff abhängen.
19. Wartende Backoff-Operationen MÜSSEN cancellierbar sein.
20. Backoff-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RETRY-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `ADR-ARCH-0118`

## Ergebnis

```text
Failure
   ↓
Retry Allowed?
├── No → Escalate
└── Yes
      ↓
   Backoff Policy
      ↓
   Delay + Jitter
      ↓
   Budget / Deadline Check
   ├── Invalid → Stop / Escalate
   └── Valid
         ↓
       Retry
         ↓
      Success / Repeat
```

NovaOS erhält damit einen kontrollierten Backoff-Mechanismus, der Wiederholungsversuche zeitlich entzerrt, Retry-Stürme und kaskadierende Überlastung verhindert und dabei Retry Budgets, Deadlines, Failure Classification und Systemzustand berücksichtigt.