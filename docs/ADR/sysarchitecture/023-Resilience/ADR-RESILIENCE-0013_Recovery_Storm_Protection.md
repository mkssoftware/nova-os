# ADR-RESILIENCE-0013 – Recovery Storm Protection

## Status

Angenommen

## Kategorie

Resilience Architecture / Recovery Coordination / Overload Protection

## Kontext

Automatische Recovery erhöht die Widerstandsfähigkeit von NovaOS. Bei gemeinsamen oder großflächigen Fehlern können jedoch viele Komponenten gleichzeitig Recovery-Maßnahmen auslösen.

Beispiele sind:

- gleichzeitige Provider-Ausfälle,
- Netzwerkstörungen,
- Storage-Ausfälle,
- Ressourcenknappheit,
- gemeinsame Dependency Failures,
- Hardwarefehler,
- Cascading Failures.

Ohne Koordination können dadurch zahlreiche Restarts, Retries, Failovers, Revalidierungen oder Provider-Wechsel gleichzeitig stattfinden.

```text
Shared Failure
     ↓
Many Failure Domains
     ↓
Parallel Recovery
     ↓
Additional Load
     ↓
More Failures
```

Die Recovery selbst kann dadurch zum Verstärker einer Störung werden.

## Entscheidung

Nova.Resilience implementiert eine systemweite **Recovery Storm Protection**.

Recovery-Aktionen werden bei erhöhter Fehlerdichte koordiniert, begrenzt und priorisiert.

```text
Failure Events
      ↓
Recovery Demand
      ↓
Storm Protection
      ↓
Budget + Coordination + Priority
      ↓
Controlled Recovery
```

## Grundprinzip

```text
More failures
≠
More recovery concurrency
```

Bei großflächigen Störungen muss NovaOS Recovery kontrollierter statt aggressiver durchführen.

## Recovery Storm

Ein Recovery Storm liegt vor, wenn gleichzeitig oder in kurzer Folge so viele Recovery-Aktivitäten entstehen, dass diese selbst relevante Systemressourcen belasten oder die ursprüngliche Störung verstärken können.

Dabei können unter anderem berücksichtigt werden:

```text
Recovery Rate
Concurrent Recoveries
Failure Rate
Affected Failure Domains
Resource Pressure
Retry Activity
Provider Availability
Recovery Failure Rate
```

## Recovery Budgets

Recovery-Aktivitäten unterliegen systemweiten und hierarchischen Budgets.

```text
System Budget
     ↓
Subsystem Budget
     ↓
Service Budget
     ↓
Failure Domain Budget
```

Ein verfügbares lokales Budget darf übergeordnete Recovery-Grenzen nicht umgehen.

Budgets können beispielsweise begrenzen:

```text
Concurrent Recoveries
Recovery Rate
CPU Consumption
Memory Consumption
I/O
Network Traffic
Restart Frequency
```

## Priorisierung

Wenn nicht alle Recovery-Aktionen gleichzeitig ausgeführt werden können, müssen sie priorisierbar sein.

Dabei können berücksichtigt werden:

```text
System Criticality
Failure Severity
Dependency Importance
User Impact
Recovery Cost
Required Restore Order
Security Relevance
```

Correctness und sicherheitskritische Systemfunktionen haben Vorrang vor reinem Komfort oder Performance.

## Hierarchische Koordination

Recovery Storm Protection integriert sich mit hierarchischen Failure Domains.

```text
System
 ├── Subsystem A
 │      ├── Service A
 │      └── Service B
 │
 └── Subsystem B
```

Treten viele Fehler innerhalb derselben übergeordneten Failure Domain auf, kann Nova.Resilience diese als gemeinsame Störung behandeln, statt jede Child Domain vollständig unabhängig recovern zu lassen.

## Shared Cause Detection

Mehrere gleichzeitig auftretende Fehler können dieselbe Ursache besitzen.

```text
Service A Failure ─┐
Service B Failure ─┼→ Shared Storage Failure
Service C Failure ─┘
```

Nova.Resilience soll gemeinsame Fehlerursachen erkennen können.

Wird eine gemeinsame Ursache erkannt, sollen abhängige Recovery-Aktionen soweit möglich koordiniert oder zurückgestellt werden, bis die Ursache behandelt wurde.

## Recovery Backpressure

Nova.Resilience kann Backpressure auf Recovery-Aktivitäten anwenden.

```text
Recovery Requests
       ↓
Recovery Capacity
       ↓
Backpressure
       ↓
Controlled Queue
```

Neue Recovery-Anforderungen können verzögert, zusammengeführt oder priorisiert werden.

Hard Deadlines und sicherheitskritische Maßnahmen dürfen dabei nicht unkontrolliert verzögert werden.

## Recovery Deduplication

Mehrere Recovery-Anforderungen für dieselbe Ursache oder Failure Domain sollen zusammengeführt werden können.

```text
Recovery Request A ─┐
Recovery Request B ─┼→ Single Coordinated Recovery
Recovery Request C ─┘
```

Dadurch werden redundante Neustarts oder Reparaturversuche vermieden.

## Restart Storm Protection

Wiederholte Restarts derselben Komponente müssen begrenzt werden.

```text
Restart
  ↓
Failure
  ↓
Restart
  ↓
Failure
  ↓
Restart
  ↓
Suppression / Escalation
```

Nach wiederholtem Fehlschlag muss Nova.Resilience eine alternative Recovery-Strategie wählen oder eskalieren.

## Integration mit Retry und Circuit Breaker

Recovery Storm Protection ergänzt Retry Budgets und Circuit Breaker.

```text
Retry Budget
      +
Circuit Breaker
      +
Recovery Budget
      ↓
System Load Protection
```

Retry Storms und Recovery Storms werden damit als zusammenhängende, aber unterschiedliche Phänomene behandelt.

## Provider Replacement

Ein gemeinsamer Provider-Ausfall kann viele gleichzeitige Provider-Wechsel verursachen.

Nova.Resilience muss verhindern, dass alle Workloads gleichzeitig auf denselben alternativen Provider wechseln und diesen dadurch überlasten.

```text
Provider A Failed
       ↓
Many Workloads
       ↓
Provider Selection Coordination
       ↓
Provider B / C / D
```

Provider-Auswahl kann dabei Resource Budgets, Health State und verfügbare Kapazität berücksichtigen.

## Recovery Waves

Bei großflächigen Ausfällen darf Nova.Resilience Recovery in kontrollierten Wellen durchführen.

```text
Wave 1
  ↓ Verify
Wave 2
  ↓ Verify
Wave 3
```

Eine weitere Recovery-Welle kann von der erfolgreichen Verification vorheriger Wiederherstellungen abhängig gemacht werden.

Damit wird verhindert, dass das gesamte System gleichzeitig in einen instabilen Recovery-Zustand versetzt wird.

## Degraded Mode

Wenn die verfügbare Recovery-Kapazität nicht ausreicht, darf NovaOS bewusst in einen verifizierten degradierten Zustand wechseln.

```text
Insufficient Recovery Capacity
          ↓
Prioritize Critical Functions
          ↓
Degraded Mode
```

Nicht kritische Funktionen können vorübergehend zurückgestellt werden.

## Stabilisierung

Nach einem Recovery Storm darf NovaOS nicht sofort alle zurückgestellten Operationen gleichzeitig wieder aktivieren.

Die Rückkehr zum normalen Betrieb soll kontrolliert erfolgen.

```text
Recovery Storm
      ↓
Stabilization
      ↓
Controlled Restore
      ↓
Normal Operation
```

Damit wird ein erneuter Lastsprung nach erfolgreicher Recovery verhindert.

## Hard Constraints

Recovery Storm Protection darf grundlegende Hard Constraints nicht abschwächen.

Insbesondere gelten weiterhin:

```text
Security
Trust
Integrity
Capabilities
Data Sovereignty
Deadlines
Critical Safety Requirements
```

Eine Recovery darf nicht allein deshalb zugelassen werden, weil ausreichend Recovery Budget vorhanden ist.

## Normative Anforderungen

1. Nova.Resilience MUSS systemweite Recovery Storm Protection bereitstellen.
2. Recovery-Aktivitäten MÜSSEN systemweit begrenzbar sein.
3. Recovery Budgets MÜSSEN hierarchisch definierbar sein.
4. Untergeordnete Recovery Budgets DÜRFEN übergeordnete Limits nicht umgehen.
5. Gleichzeitige Recovery-Aktionen MÜSSEN priorisierbar sein.
6. Recovery-Priorisierung MUSS Systemkritikalität berücksichtigen können.
7. Nova.Resilience MUSS gemeinsame Ursachen mehrerer Fehler berücksichtigen können.
8. Recovery-Aktionen derselben Ursache SOLLEN soweit möglich koordiniert oder dedupliziert werden.
9. Recovery Backpressure MUSS unterstützt werden können.
10. Wiederholte Restart-Schleifen MÜSSEN erkannt und begrenzt werden.
11. Fehlgeschlagene wiederholte Recovery MUSS alternative Recovery oder Eskalation ermöglichen.
12. Provider Replacement MUSS verfügbare Provider-Kapazität berücksichtigen können.
13. Nova.Resilience MUSS kontrollierte Recovery Waves unterstützen können.
14. Recovery Waves SOLLEN vor weiterer Eskalation verifiziert werden können.
15. Ein verifizierter Degraded Mode MUSS bei unzureichender Recovery-Kapazität möglich sein.
16. Die Rückkehr aus einem Recovery Storm MUSS kontrolliert erfolgen können.
17. Recovery Storm Protection DARF Hard Constraints nicht umgehen.
18. Recovery-Budgets, Suppression, Priorisierung und relevante Storm-Zustände MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- verhindert, dass Self-Healing selbst Systemausfälle verstärkt,
- begrenzt gleichzeitige Restarts und Recovery-Aktionen,
- gemeinsame Fehlerursachen können koordiniert behandelt werden,
- kritische Systemfunktionen können priorisiert wiederhergestellt werden,
- alternative Provider werden vor Recovery-Lastspitzen geschützt,
- großflächige Wiederherstellung kann kontrolliert in Wellen erfolgen.

### Negative Konsequenzen

- Recovery benötigt systemweite Koordination,
- weniger kritische Komponenten können länger im degradierten Zustand verbleiben,
- Recovery-Priorisierung und Budgets erhöhen die Planungslogik von Nova.Resilience.

## Verworfene Alternativen

### Jede Failure Domain recovered unabhängig

Verworfen, da gemeinsame Fehlerursachen dadurch massive parallele Recovery-Aktivität erzeugen können.

### Unbegrenzte parallele Recovery

Verworfen, da Recovery selbst CPU, Speicher, I/O, Netzwerk und Provider-Kapazität benötigt.

### Feste globale Recovery-Grenze

Verworfen, da unterschiedliche Failure Domains und Systemzustände unterschiedliche Recovery-Kapazitäten besitzen.

### Alle Komponenten gleichzeitig nach Ende einer Störung wiederherstellen

Verworfen, da dadurch unmittelbar ein neuer Lastsprung entstehen kann.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0006_Retry_Budgets_Backoff_und_Jitter`
- `ADR-RESILIENCE-0007_Systemweite_Circuit_Breaker`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RECOVERY-STORM-0001`
- `NPSPEC-RESILIENCE-RECOVERY-BUDGET-0001`
- `NPSPEC-RESILIENCE-RECOVERY-BACKPRESSURE-0001`
- `NPSPEC-RESILIENCE-RECOVERY-DEDUPLICATION-0001`
- `NPSPEC-RESILIENCE-RECOVERY-PRIORITY-0001`
- `NPSPEC-RESILIENCE-RECOVERY-WAVE-0001`
- `NPSPEC-RESILIENCE-RECOVERY-STORM-TEST-0001`

## Ergebnis

Nova.Resilience begrenzt nicht nur Fehler, sondern auch die Auswirkungen seiner eigenen Recovery-Aktivitäten:

```text
Many Failures
     ↓
Recovery Demand
     ↓
Storm Detection
     ↓
Budget + Priority + Deduplication
     ↓
Controlled Recovery Waves
     ↓
Verification
     ↓
Stabilization
     ↓
Controlled Restore
```

Damit verhindert NovaOS, dass systemweites Self-Healing bei großflächigen Störungen selbst zu einem **Recovery Storm** und damit zu einer zusätzlichen Fehlerquelle wird.