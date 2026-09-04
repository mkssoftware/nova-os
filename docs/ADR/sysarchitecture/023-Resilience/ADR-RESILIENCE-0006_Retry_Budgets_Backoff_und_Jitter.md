# ADR-RESILIENCE-0006 – Retry Budgets, Backoff und Jitter

## Status

Angenommen

## Kategorie

Resilience Architecture / Retry / Resource Control

## Kontext

Auch wenn eine Operation gemäß ihrer expliziten Operationssemantik sicher wiederholt werden darf, können unkontrollierte Retries selbst zu einer Systemstörung führen.

Bei einem gemeinsamen Fehler können viele Workloads gleichzeitig Wiederholungsversuche starten. Dadurch entstehen zusätzliche Last, Warteschlangen und weitere Timeouts. Aus einem begrenzten Fehler kann so eine systemweite Überlastung entstehen.

Nova.Resilience benötigt deshalb systemweite Regeln zur Begrenzung und zeitlichen Steuerung von Retries.

## Entscheidung

NovaOS kombiniert automatische Retries mit **Retry Budgets, Backoff und Jitter**.

```text
Retry-safe Failure
        ↓
Retry Budget
        ↓
Backoff
        ↓
Jitter
        ↓
Retry Attempt
```

Ein semantisch zulässiger Retry bedeutet nicht automatisch, dass ein Retry zu jedem Zeitpunkt oder beliebig oft ausgeführt werden darf.

## Grundprinzip

```text
Retry Safety
≠
Unlimited Retry
```

Retries müssen sowohl semantisch korrekt als auch ressourcenökonomisch vertretbar sein.

## Retry Budget

Jede automatisch wiederholbare Operation muss einem begrenzten Retry Budget unterliegen.

Ein Budget kann beispielsweise begrenzen:

```text
Maximum Attempts
Maximum Retry Time
Maximum Resource Cost
Maximum Retry Rate
```

Retry Budgets können auf mehreren Ebenen existieren:

```text
Operation
   ↓
Workload
   ↓
Service
   ↓
Failure Domain
   ↓
System
```

Untergeordnete Budgets dürfen übergeordnete Limits nicht umgehen.

## Budget Exhaustion

Ist das zulässige Retry Budget erschöpft, werden keine weiteren automatischen Retries ausgeführt.

```text
Retry
  ↓
Budget Available?
 ├── Yes → Continue
 └── No  → Alternative Recovery / Escalation
```

Budget Exhaustion ist ein expliziter Resilience-Zustand und kein Grund für einen impliziten Budget-Reset.

## Backoff

Wiederholungsversuche sollen bei geeigneten Fehlerklassen zeitlich auseinandergezogen werden.

Nova.Resilience unterstützt dafür Backoff Policies.

Beispielsweise:

```text
Fixed Backoff
Linear Backoff
Exponential Backoff
Provider-defined Backoff
```

Die konkrete Strategie hängt von Operation, Fehlerklasse und Execution Contract ab.

## Jitter

Bei parallelen Workloads können identische Backoff-Zeiten dazu führen, dass alle Clients gleichzeitig erneut versuchen, dieselbe Ressource zu verwenden.

```text
Failure
   ↓
Same Backoff
   ↓
Synchronized Retry
   ↓
Load Spike
```

Jitter verteilt Retry-Zeitpunkte innerhalb eines zulässigen Bereichs:

```text
Backoff
   +
Jitter
   ↓
Distributed Retry Attempts
```

Dadurch werden synchronisierte Retry-Wellen reduziert.

## Retry Storm Prevention

Nova.Resilience muss Retry Storms systemweit erkennen und begrenzen können.

Dazu können insbesondere verwendet werden:

- Retry Budgets,
- Rate Limits,
- Backpressure,
- Circuit Breaker,
- Provider Health,
- Failure-Domain-Status.

Wenn ein gemeinsamer Provider oder eine gemeinsame Ressource ausgefallen ist, sollen nicht sämtliche abhängigen Workloads unabhängig aggressive Retries durchführen.

## Deadlines

Retry Budgets dürfen bestehende Deadlines nicht verlängern.

```text
Operation Deadline
       ↓
Available Retry Window
       ↓
Retry Attempts
```

Reicht die verbleibende Zeit nicht für einen sinnvollen Retry, soll dieser nicht begonnen werden.

## Resource Budgets

Retries sind Teil der systemweiten Ressourcenökonomie.

CPU-Zeit, Netzwerk, Energie, Speicher, I/O und andere Ressourcen, die durch Retries verbraucht werden, müssen bestehenden Resource Budgets zugerechnet werden.

Ein Retry erzeugt kein zusätzliches implizites Ressourcenbudget.

## Provider Signaling

Capability Provider oder Remote Services können Retry-relevante Informationen liefern, beispielsweise:

```text
Retry After
Temporary Overload
Unavailable
Rate Limited
Estimated Recovery
```

Nova.Resilience kann solche Informationen berücksichtigen, sofern sie mit lokalen Policies und Deadlines vereinbar sind.

Ein externer Provider darf dadurch keine lokalen Hard Constraints überschreiben.

## Deterministic Mode

Jitter führt bewusst Variabilität in Retry-Zeitpunkte ein.

Im Deterministic Mode muss diese Variabilität kontrolliert werden.

NovaOS muss deshalb deterministische Jitter-Strategien oder eine reproduzierbare Randomness-Quelle verwenden können.

```text
Deterministic Seed
       ↓
Jitter Function
       ↓
Reproducible Retry Timing
```

Secure Randomness und Deterministic Randomness bleiben dabei getrennt.

## Cancellation

Wird die ursprüngliche Operation oder ihr Structured-Concurrency-Scope abgebrochen, müssen ausstehende Retry-Versuche ebenfalls beendet werden.

```text
Parent Cancellation
        ↓
Pending Backoff
        ↓
Cancel Retry
```

Ein Retry darf die Lifetime seiner ursprünglichen Operation nicht eigenständig verlängern.

## Normative Anforderungen

1. Automatische Retries MÜSSEN einem begrenzten Retry Budget unterliegen.
2. Retry Budgets MÜSSEN maximale Versuche oder eine vergleichbare harte Begrenzung unterstützen.
3. Retry Budgets MÜSSEN hierarchisch begrenzbar sein können.
4. Untergeordnete Retry Budgets DÜRFEN übergeordnete Limits nicht umgehen.
5. Budget Exhaustion MUSS weitere automatische Retries verhindern.
6. Nova.Resilience MUSS Backoff Policies unterstützen.
7. Nova.Resilience MUSS Jitter zur Vermeidung synchronisierter Retry-Wellen unterstützen können.
8. Retry Storms MÜSSEN systemweit begrenzbar sein.
9. Retries MÜSSEN bestehende Operation Deadlines respektieren.
10. Retries DÜRFEN bestehende Resource Budgets nicht implizit erweitern.
11. Provider Retry Signaling DARF lokale Policies und Hard Constraints nicht überschreiben.
12. Cancellation MUSS ausstehende Retry-Versuche und Backoff-Wartezeiten abbrechen können.
13. Im Deterministic Mode MUSS Retry-Verhalten reproduzierbar steuerbar sein.
14. Deterministic Jitter DARF nicht auf Secure Randomness angewiesen sein.
15. Security-, Trust-, Capability- oder Sovereignty-Fehler DÜRFEN nicht durch wiederholte Retries umgangen werden.
16. Retry Budget, Backoff-Entscheidungen und Budget Exhaustion MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- verhindert unbegrenzte Retry-Schleifen,
- reduziert Retry Storms und Thundering-Herd-Effekte,
- schützt überlastete Provider vor zusätzlicher Last,
- integriert Retries in die systemweite Ressourcenökonomie,
- Deadlines und Cancellation bleiben erhalten,
- Deterministic Mode bleibt mit Jitter vereinbar.

### Negative Konsequenzen

- Retry Policies benötigen zusätzliche Zustandsverwaltung,
- hierarchische Budgets müssen über Failure Domains hinweg koordiniert werden,
- zu restriktive Budgets können grundsätzlich recoverbare Operationen frühzeitig eskalieren.

## Verworfene Alternativen

### Feste Anzahl Retries für alle Operationen

Verworfen, da unterschiedliche Operationen, Fehlerklassen und Ressourcen unterschiedliche Retry-Anforderungen besitzen.

### Sofortiger Retry ohne Backoff

Verworfen, da dadurch bereits überlastete Ressourcen zusätzlich belastet werden können.

### Backoff ohne Jitter

Verworfen, da viele gleichzeitig fehlgeschlagene Workloads ihre Retries weiterhin synchronisieren könnten.

### Unbegrenzte Retries bis zum Erfolg

Verworfen, da dadurch Ressourcen erschöpft, Deadlines verletzt und Fehler verstärkt werden können.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0009_Structured_Concurrency_systemweit`
- `ADR-ARCH-0010_Deterministic_Mode`
- `ADR-CRYPTO-0009_Secure_Randomness_getrennt_von_Deterministic_Randomness`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RETRY-BUDGET-0001`
- `NPSPEC-RESILIENCE-BACKOFF-0001`
- `NPSPEC-RESILIENCE-JITTER-0001`
- `NPSPEC-RESILIENCE-RETRY-STORM-0001`
- `NPSPEC-RESILIENCE-RETRY-RATE-LIMIT-0001`
- `NPSPEC-RESILIENCE-RETRY-BUDGET-TEST-0001`

## Ergebnis

Nova.Resilience behandelt einen semantisch zulässigen Retry als begrenzte Ressource:

```text
Retry-safe Operation
        ↓
Retry Budget
        ↓
Deadline + Resource Budget
        ↓
Backoff + Jitter
        ↓
Retry
        ↓
Success / Budget Exhaustion
```

Damit können transiente Fehler automatisch behandelt werden, ohne dass Retries selbst zu Überlastung, synchronisierten Retry-Wellen oder unbegrenzten Recovery-Schleifen führen.