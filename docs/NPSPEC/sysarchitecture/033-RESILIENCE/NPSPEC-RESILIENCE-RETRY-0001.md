# NPSPEC-RESILIENCE-RETRY-0001 – Nova Resilience Retry

## Status

Angenommen

## Kategorie

Resilience / Recovery / Retry

## Zweck

NovaOS definiert einen kontrollierten Retry-Mechanismus für fehlgeschlagene oder vorübergehend nicht ausführbare Operationen.

```text
Operation
   ↓
Failure
   ↓
Classify
   ↓
Retry Decision
   ↓
Wait / Backoff
   ↓
Retry
   ↓
Verify
```

Retry dient der Behandlung vorübergehender Fehler und darf nicht zu unbegrenzten Wiederholungsschleifen führen.

## Grundprinzipien

```text
Retry ≠ Recovery
Retry ≠ Restart
Retry ≠ Idempotency
Timeout ≠ Safe to Retry
Failure ≠ Retry Required
Repeated Retry ≠ Progress
Retry Success ≠ System Healthy
```

Ein Retry darf nur erfolgen, wenn die Operation und ihr Zustand eine Wiederholung zulassen.

## Retry Model

```text
RetryOperation
├── RetryID
├── OperationID
├── FailureID
├── Attempt
├── RetryPolicy
├── RetryBudget
└── State
```

Optional:

```text
ExecutionID
TransactionID
IdempotencyKey
Deadline
Backoff
Jitter
FailureClass
ProviderID
RecoveryPolicy
ProvenanceID
```

## Retry States

```text
Pending
Waiting
Retrying
Succeeded
Failed
Cancelled
BudgetExhausted
Unknown
```

## Retry Eligibility

Vor jedem Retry muss geprüft werden:

```text
Failure Class
      +
Operation Semantics
      +
Current State
      +
Remaining Deadline
      +
Retry Budget
      ↓
Retry Allowed?
```

Nicht jeder Fehler ist retryfähig.

## Retryable Failures

Typische Kandidaten sind:

```text
Temporary Resource Contention
Transient Network Failure
Temporary Provider Unavailability
Device Busy
Temporary Queue Saturation
```

Persistente oder sicherheitskritische Fehler sollen nicht durch blindes Retry verdeckt werden.

## Idempotency

Wiederholbare Operationen müssen ihre Semantik berücksichtigen.

```text
Retryable ≠ Idempotent
```

Bei nicht-idempotenten Operationen muss NovaOS feststellen können, ob die ursprüngliche Operation bereits wirksam wurde.

```text
Request
   ↓
Timeout
   ↓
Execution State Unknown
```

In diesem Zustand darf eine Wiederholung nicht automatisch erfolgen.

## Idempotency Keys

Geeignete Operationen können stabile Idempotency Keys verwenden.

```text
Operation
   +
IdempotencyKey
      ↓
Duplicate Detection
```

Dadurch können unbeabsichtigte Mehrfachausführungen verhindert werden.

## Retry Budget

Retries müssen begrenzt sein.

```text
RetryBudget
├── MaxAttempts
├── MaxDuration
├── Deadline
├── CPU Budget
├── IO Budget
└── Network Budget
```

Ist das Budget erschöpft:

```text
Retry
 ↓
Retry
 ↓
Budget Exhausted
 ↓
Escalate
```

## Backoff

Wiederholungen sollen nicht unmittelbar unbegrenzt aufeinander folgen.

NovaOS kann unterstützen:

```text
Fixed Backoff
Linear Backoff
Exponential Backoff
Adaptive Backoff
```

Optionales Jitter kann synchronisierte Retry-Stürme verhindern.

## Retry Storm Prevention

Viele gleichzeitig fehlschlagende Komponenten können eine positive Rückkopplung erzeugen.

```text
Service Failure
     ↓
Thousands of Retries
     ↓
Higher Load
     ↓
More Failures
```

NovaOS soll dagegen verwenden können:

```text
Backoff
Jitter
Rate Limiting
Retry Budgets
Circuit Breaker
Load Shedding
```

## Deadline Awareness

Retries müssen verbleibende zeitliche Anforderungen berücksichtigen.

```text
Current Time
     ↓
Remaining Deadline
     ↓
Retry Useful?
```

Ein Retry soll nicht begonnen werden, wenn die Operation ihre relevante Deadline dadurch nicht mehr sinnvoll erreichen kann.

## Cancellation

Retries müssen abbrechbar sein.

```text
Retry Loop
   ↓
Cancellation
   ↓
Stop Further Attempts
```

Structured-Concurrency-Regeln bleiben gültig.

## Provider Failover

Retry muss nicht denselben Provider verwenden.

```text
Attempt 1
→ Provider A
→ Failure

Attempt 2
→ Provider B
```

Provider-Wechsel muss weiterhin Capability-, Trust-, Sovereignty- und Execution-Constraints erfüllen.

## Distributed Operations

Bei Remote-Operationen ist besondere Vorsicht erforderlich.

```text
Request Sent
    ↓
Connection Lost
    ↓
Remote Execution State Unknown
```

```text
No Response ≠ Operation Not Executed
```

NovaOS muss `UnknownExecutionState` erhalten können.

## Transaction Integration

Retries innerhalb von Transaktionen müssen deren Semantik respektieren.

```text
Transaction
   ↓
Operation Failure
   ↓
Retry / Rollback / Abort
```

Retry darf keine doppelte oder inkonsistente Zustandsänderung erzeugen.

## Realtime Integration

Bei Realtime-Executions muss jeder Retry in das vorhandene Zeitbudget passen.

```text
Execution Budget
      ↓
Retry Cost
      ↓
Remaining Deadline
```

Hard-Realtime-Ausführung darf nicht von unbeschränkten Retries abhängen.

## Escalation

Wenn Retry nicht erfolgreich ist:

```text
Retry Budget Exhausted
        ↓
Classification Update
        ↓
Restart / Failover / Degrade / Recovery
```

Wiederholte transiente Fehler können als persistent reklassifiziert werden.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
RetryID
OperationID
Attempt
Failure Class
Retry Policy
Backoff
Remaining Budget
Remaining Deadline
Provider
Last Result
Execution State
Final State
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Retries fehlgeschlagener Operationen unterstützen können.
2. Retry MUSS von Restart und vollständiger Recovery getrennt bleiben.
3. Nicht jeder Fehler DARF automatisch einen Retry auslösen.
4. Vor Retry MUSS die Wiederholbarkeit der Operation geprüft werden.
5. Timeout DARF NICHT automatisch bedeuten, dass eine Operation nicht ausgeführt wurde.
6. `UnknownExecutionState` MUSS explizit darstellbar sein.
7. Nicht-idempotente Operationen DÜRFEN bei unbekanntem Ausführungszustand NICHT blind wiederholt werden.
8. Idempotency Keys SOLLEN unterstützt werden können.
9. Retry-Versuche MÜSSEN durch Budgets begrenzt sein.
10. Retry Policies SOLLEN Backoff unterstützen.
11. Jitter SOLL zur Vermeidung synchronisierter Retry-Stürme unterstützt werden.
12. Retry Storms MÜSSEN begrenzbar sein.
13. Retries MÜSSEN verbleibende Deadlines berücksichtigen.
14. Hard-Realtime-Executions DÜRFEN NICHT von unbegrenzten Retries abhängen.
15. Retry-Schleifen MÜSSEN cancellierbar sein.
16. Provider Failover MUSS bestehende Capability-, Trust- und Sovereignty-Regeln respektieren.
17. Retries innerhalb von Transaktionen DÜRFEN keine doppelte Zustandsänderung erzeugen.
18. Erschöpfte Retry Budgets MÜSSEN eskalierbar sein.
19. Wiederholt auftretende Fehler MÜSSEN eine Reclassification ermöglichen.
20. Retry-Zustände und Ergebnisse MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RESTART-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `ADR-ARCH-0117`

## Ergebnis

```text
Operation Failure
       ↓
Classify
       ↓
Retry Safe?
├── No → Escalate
└── Yes
      ↓
   Budget Check
      ↓
   Backoff
      ↓
   Retry
      ↓
   Verify
   ├── Success → Continue
   └── Failure
          ↓
     Budget Exhausted?
     ├── No → Retry
     └── Yes → Escalate
```

NovaOS erhält damit einen kontrollierten Retry-Mechanismus, der transiente Fehler behandeln kann, ohne durch blinde Wiederholungen doppelte Zustandsänderungen, Retry-Stürme, Deadline-Verletzungen oder endlose Fehlerschleifen zu erzeugen.