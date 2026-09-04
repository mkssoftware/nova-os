# ADR-RESILIENCE-0005 – Retry nur nach expliziter Operationssemantik

## Status

Angenommen

## Kategorie

Resilience Architecture / Retry / Operation Semantics

## Kontext

Retry ist eine grundlegende Recovery-Maßnahme für transiente Fehler. Eine fehlgeschlagene Operation einfach erneut auszuführen ist jedoch nicht grundsätzlich sicher.

Nach einem Timeout oder Kommunikationsfehler kann beispielsweise unbekannt sein, ob die ursprüngliche Operation:

- nicht begonnen wurde,
- teilweise ausgeführt wurde,
- vollständig ausgeführt wurde,
- committed wurde, die Antwort aber verloren ging.

Ein unkontrollierter Retry kann dadurch Operationen mehrfach ausführen, Daten verändern, Ressourcen doppelt erzeugen oder externe Effekte wiederholen.

Nova.Resilience darf Retry deshalb nicht als generische Reaktion auf Fehler behandeln.

## Entscheidung

NovaOS erlaubt automatische Retries nur, wenn die **Operationssemantik explizit definiert**, für Nova.Resilience verfügbar und mit einem Retry vereinbar ist.

```text
Operation Failure
       ↓
Operation Semantics
       ↓
Retry Safe?
   ├── Yes → Retry Policy
   └── No  → Alternative Recovery
```

Fehlt eine ausreichende Retry-Semantik, darf Nova.Resilience die Operation nicht automatisch wiederholen.

## Grundprinzip

```text
Unknown execution state
+
Unknown operation semantics
=
No automatic retry
```

Retry ist damit eine semantische und keine rein technische Entscheidung.

## Operationssemantik

Eine Operation muss relevante Eigenschaften deklarieren können.

Beispiele:

```text
ReadOnly
Idempotent
Deduplicated
Transactional
Compensatable
AtMostOnceRequired
NonRepeatable
```

Die konkrete Darstellung dieser Eigenschaften wird in NPSPECs definiert.

## Idempotente Operationen

Eine idempotente Operation kann mehrfach mit denselben semantischen Eingaben ausgeführt werden, ohne dadurch einen anderen beabsichtigten Endzustand zu erzeugen.

```text
Operation(X)
Operation(X)
Operation(X)

→ Same intended state
```

Idempotenz kann automatische Retries ermöglichen, ersetzt jedoch nicht die Prüfung weiterer Constraints.

## Deduplication

Nicht-idempotente Operationen können retryfähig sein, wenn das Ziel eine zuverlässige Deduplication unterstützt.

```text
Operation
   +
Operation ID
   ↓
Receiver
   ↓
Already processed?
 ├── Yes → Return previous result
 └── No  → Execute
```

Die Operation Identity muss über Retry-Versuche hinweg stabil bleiben.

## Transaktionale Operationen

Bei transaktionalen Operationen muss Nova.Resilience den Transaktionszustand berücksichtigen.

```text
Not Started
In Progress
Committed
Rolled Back
Unknown
```

Ein Retry darf nicht blind eine neue Transaktion erzeugen, wenn der Commit-Status der ursprünglichen Operation unbekannt ist.

## Side Effects

Operationen mit externen oder irreversiblen Side Effects benötigen besonders strenge Retry-Regeln.

Beispiele:

```text
Payment
Message Delivery
Device Command
Secret Rotation
External API Mutation
Physical Actuation
```

Ein Timeout bedeutet bei solchen Operationen nicht automatisch, dass die Aktion nicht ausgeführt wurde.

## Retry Policy

Ist Retry semantisch zulässig, bestimmt eine Retry Policy die konkrete Durchführung.

Sie kann unter anderem definieren:

```text
Maximum Attempts
Backoff
Deadline
Retryable Error Classes
Jitter Policy
Resource Budget
Cancellation
```

Die Retry Policy darf die ursprünglichen Execution-Contract-Grenzen nicht eigenständig erweitern.

## Deadlines und Cancellation

Retries müssen bestehende Deadlines und Structured-Concurrency-Lifetimes respektieren.

```text
Parent Operation
      ↓
Deadline / Cancellation
      ↓
Retry Attempts
```

Eine bereits abgebrochene oder abgelaufene Operation darf nicht durch Nova.Resilience unbegrenzt weitergeführt werden.

## Resource Budgets

Retries verbrauchen Ressourcen und dürfen keine unkontrollierte Lastverstärkung verursachen.

```text
Failure
  ↓
Retry
  ↓
More Load
  ↓
More Failures
```

Nova.Resilience muss Retry Storms begrenzen können.

## Fehlerklassifikation

Nicht jeder Fehler ist retryfähig.

Typische transiente Fehler können einen Retry erlauben, während beispielsweise folgende Zustände normalerweise eine andere Recovery benötigen:

```text
Invalid Input
Permission Denied
Capability Revoked
Trust Failure
Integrity Failure
Policy Violation
Data Sovereignty Violation
```

Die endgültige Entscheidung basiert auf Operationssemantik und Fehlerklassifikation.

## Provider Replacement

Ein Retry muss nicht zwingend beim selben Provider erfolgen.

```text
Operation
   ↓
Provider A
   ↓ transient failure
Nova.Resilience
   ↓
Compatible Provider B
   ↓
Retry
```

Ein Providerwechsel ist nur zulässig, wenn der ursprüngliche Execution Contract und die Operationssemantik dies erlauben.

## Retry und Determinismus

Im Deterministic Mode müssen Retry-Entscheidungen und relevante Retry-Ereignisse reproduzierbar beziehungsweise nachvollziehbar sein.

Nicht deterministische Backoff- oder Provider-Auswahl darf die geforderten Determinismusgarantien nicht verletzen.

## Normative Anforderungen

1. Nova.Resilience DARF automatische Retries nur bei explizit definierter Operationssemantik durchführen.
2. Fehlende oder unbekannte Retry-Semantik MUSS für automatische Retries als nicht ausreichend gelten.
3. Operationen MÜSSEN ihre relevanten Retry-Eigenschaften deklarieren können.
4. Idempotenz MUSS explizit ausdrückbar sein.
5. Nicht-idempotente Operationen DÜRFEN nur mit geeigneter zusätzlicher Semantik automatisch wiederholt werden.
6. Deduplicated Operations MÜSSEN eine über Retry-Versuche stabile Operation Identity verwenden.
7. Transaktionale Operationen MÜSSEN ihren bekannten oder unbekannten Commit-Zustand berücksichtigen.
8. Ein unbekannter Commit-Status DARF nicht automatisch als fehlgeschlagene Ausführung interpretiert werden.
9. Operationen mit irreversiblen Side Effects MÜSSEN explizite Retry-Regeln besitzen.
10. Retry Policies MÜSSEN maximale Versuche, Deadlines oder andere geeignete Begrenzungen unterstützen.
11. Retries MÜSSEN Cancellation und Structured-Concurrency-Lifetimes respektieren.
12. Retries MÜSSEN bestehende Resource Budgets berücksichtigen.
13. Nova.Resilience MUSS unkontrollierte Retry Storms verhindern können.
14. Security-, Trust-, Capability-, Integrity- oder Sovereignty-Fehler DÜRFEN nicht durch Retry umgangen werden.
15. Provider Replacement während eines Retries DARF nur bei semantischer und vertraglicher Kompatibilität erfolgen.
16. Retry-Entscheidungen und relevante Retry-Ergebnisse MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- verhindert unbeabsichtigte Mehrfachausführung,
- Retry wird Teil der definierten API- und Operationssemantik,
- transiente Fehler können weiterhin effizient behandelt werden,
- Transaktionen und externe Side Effects werden korrekt berücksichtigt,
- Retry Storms können systemweit kontrolliert werden.

### Negative Konsequenzen

- APIs und Capability Provider müssen ihre Operationssemantik expliziter beschreiben,
- einige Fehler können nicht automatisch durch Retry behandelt werden,
- Deduplication und Operation IDs verursachen zusätzlichen Implementierungsaufwand.

## Verworfene Alternativen

### Retry automatisch bei jedem transienten Fehler

Verworfen, da ein transienter Transportfehler nichts darüber aussagt, ob die Operation bereits ausgeführt wurde.

### Retry nur anhand des Fehlercodes entscheiden

Verworfen, da Retry-Sicherheit sowohl von der Fehlerart als auch von der Semantik der Operation abhängt.

### Alle Operationen idempotent machen

Verworfen, da bestimmte Operationen inhärent einmalige oder externe Side Effects besitzen.

### At-most-once für alle Operationen erzwingen

Verworfen, da dies für viele verteilte und fehlertolerante Operationen unnötig restriktiv wäre.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0009_Structured_Concurrency_systemweit`
- `ADR-ARCH-0010_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RETRY-SEMANTICS-0001`
- `NPSPEC-RESILIENCE-IDEMPOTENCY-0001`
- `NPSPEC-RESILIENCE-DEDUPLICATION-0001`
- `NPSPEC-RESILIENCE-OPERATION-IDENTITY-0001`
- `NPSPEC-RESILIENCE-RETRY-POLICY-0001`
- `NPSPEC-RESILIENCE-RETRY-BUDGET-0001`
- `NPSPEC-RESILIENCE-RETRY-TEST-0001`

## Ergebnis

Nova.Resilience behandelt Retry nicht als universelle Fehlerbehandlung:

```text
Failure
   ↓
Classify Error
   ↓
Inspect Operation Semantics
   ↓
Retry Safe?
 ├── Yes → Bounded Retry
 └── No  → Alternative Recovery
```

Damit wird eine Operation nur dann automatisch wiederholt, wenn NovaOS ihre Semantik ausreichend kennt und garantieren kann, dass der Retry unter den geltenden Contracts und Hard Constraints zulässig ist.