# ADR-RESILIENCE-0004 – Kleinster sicherer Recovery Scope

## Status

Angenommen

## Kategorie

Resilience Architecture / Recovery Scope / Failure Isolation

## Kontext

Nova.Resilience kann auf Fehler mit unterschiedlich weitreichenden Recovery-Maßnahmen reagieren.

Ein einzelner Fehler rechtfertigt jedoch nicht automatisch den Neustart oder die Wiederherstellung einer größeren Systemeinheit. Eine unnötig große Recovery kann zusätzliche Workloads unterbrechen, Zustand verlieren und den Blast Radius erhöhen.

Gleichzeitig darf der Recovery Scope nicht so klein gewählt werden, dass Abhängigkeiten, beschädigter Zustand oder gemeinsame Fehlerursachen außerhalb des gewählten Bereichs bestehen bleiben.

## Entscheidung

Nova.Resilience verwendet grundsätzlich den **kleinsten sicheren Recovery Scope**, der geeignet ist, einen gültigen Systemzustand wiederherzustellen.

```text
Failure
   ↓
Affected Scope
   ↓
Candidate Recovery Scopes
   ↓
Smallest Safe Scope
   ↓
Recovery
```

Die Auswahl erfolgt anhand der Failure Domain, Abhängigkeiten, Zustandsbeziehungen und geltenden Hard Constraints.

## Grundprinzip

```text
Recover as locally as possible,
as broadly as necessary.
```

Die kleinste Recovery ist nicht automatisch die richtige Recovery.

Entscheidend ist die kleinste **sichere und ausreichende** Recovery.

## Recovery-Hierarchie

Typische Recovery Scopes können hierarchisch angeordnet sein:

```text
Operation
   ↓
Task
   ↓
Workload
   ↓
Service
   ↓
Subsystem
   ↓
System
```

Je höher die Recovery eskaliert, desto größer ist typischerweise der Blast Radius.

## Scope-Bestimmung

Nova.Resilience berücksichtigt bei der Auswahl insbesondere:

- betroffene Failure Domain,
- Fehlerklasse,
- gemeinsame Abhängigkeiten,
- Zustand und State Ownership,
- Integrity State,
- laufende Transaktionen,
- Capability Provider,
- Resource Dependencies,
- Security- und Trust-Grenzen.

Ein Scope darf nur gewählt werden, wenn innerhalb dieses Scopes ein verifizierbarer gültiger Zustand wiederhergestellt werden kann.

## Beispiel

Ein abgestürzter Task soll zunächst lokal behandelt werden:

```text
Task Failure
    ↓
Task Recovery
```

Kann der Task nicht sicher wiederhergestellt werden:

```text
Task Recovery
    ↓ failed
Workload Recovery
    ↓ failed
Service Recovery
```

Eine Eskalation erfolgt nur soweit erforderlich.

## Shared State

Gemeinsam genutzter Zustand kann einen größeren Recovery Scope erforderlich machen.

```text
Workload A ─┐
            ├── Shared State
Workload B ─┘
```

Ist der gemeinsame Zustand beschädigt, reicht die isolierte Wiederherstellung von Workload A möglicherweise nicht aus.

Nova.Resilience muss deshalb State Ownership und Shared Dependencies berücksichtigen.

## Recovery-Kosten

Zwischen mehreren sicheren Recovery-Strategien darf Nova.Resilience deren Auswirkungen berücksichtigen.

Beispiele:

```text
Downtime
State Loss
Affected Workloads
Resource Cost
Latency
User Impact
```

Diese Faktoren dienen der Optimierung.

Correctness, Security, Trust und Datenintegrität haben Vorrang.

## Eskalation

Kann der gewählte Scope den gültigen Zustand nicht wiederherstellen oder schlägt die Verification fehl, wird auf die nächsthöhere geeignete Failure Domain eskaliert.

```text
Recover
   ↓
Verify
   ↓ failed
Expand Scope
   ↓
Recover
```

Die Eskalation muss begrenzt und nachvollziehbar bleiben.

## Deeskalation

Ein Fehler auf einer größeren sichtbaren Ebene darf nach Diagnose auf einen kleineren tatsächlichen Scope reduziert werden.

Beispiel:

```text
Service Failure
      ↓
Diagnosis
      ↓
Single Provider Failure
      ↓
Provider Replacement
```

Damit wird verhindert, dass Symptome automatisch den Recovery Scope bestimmen.

## System Recovery

Eine systemweite Recovery ist die letzte reguläre Eskalationsstufe.

```text
Local Recovery
      ↓
Domain Recovery
      ↓
Subsystem Recovery
      ↓
System Recovery
```

Ein vollständiger Neustart oder Rollback des Systems darf nicht als Standard-Recovery für lokal behandelbare Fehler verwendet werden.

## Hard Constraints

Die Minimierung des Recovery Scope darf keine Hard Constraints verletzen.

Insbesondere gelten weiterhin:

```text
Correctness
Security
Trust
Integrity
Capabilities
Data Sovereignty
Information Flow
```

Wenn ein größerer Scope notwendig ist, um diese Bedingungen wiederherzustellen, muss entsprechend eskaliert werden.

## Normative Anforderungen

1. Nova.Resilience MUSS grundsätzlich den kleinsten sicheren und ausreichenden Recovery Scope wählen.
2. Der Recovery Scope MUSS auf der tatsächlichen Failure Domain und nicht ausschließlich auf sichtbaren Symptomen basieren.
3. Shared Dependencies und Shared State MÜSSEN bei der Scope-Bestimmung berücksichtigt werden.
4. Ein Recovery Scope DARF nur gewählt werden, wenn darin ein gültiger Zustand wiederherstellbar ist.
5. Lokale Recovery MUSS gegenüber unnötig breiter Recovery bevorzugt werden.
6. Fehlgeschlagene Verification MUSS eine erneute Planung oder Scope-Eskalation ermöglichen.
7. Recovery Scope MUSS hierarchisch eskalierbar sein.
8. Eine diagnostizierte kleinere Fehlerursache MUSS eine Deeskalation des Recovery Scope ermöglichen können.
9. Recovery Planning SOLL den erwarteten Blast Radius berücksichtigen.
10. Recovery-Kosten DÜRFEN zwischen mehreren sicheren Strategien als Optimierungskriterium verwendet werden.
11. Correctness, Security, Trust und Datenintegrität MÜSSEN Vorrang vor Scope-Minimierung besitzen.
12. Systemweite Recovery DARF nicht als Standardlösung für lokal behandelbare Fehler verwendet werden.
13. Recovery-Eskalationen MÜSSEN begrenzt werden, um Endlosschleifen zu verhindern.
14. Die Auswahl und Veränderung des Recovery Scope MUSS kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- kleinerer Blast Radius,
- weniger unnötige Neustarts,
- geringere Auswirkungen auf nicht betroffene Workloads,
- State Loss und Downtime können reduziert werden,
- Recovery kann gezielt entlang der Failure-Domain-Hierarchie eskalieren.

### Negative Konsequenzen

- die Bestimmung des korrekten Recovery Scope benötigt genaue Abhängigkeitsinformationen,
- Shared State kann lokale Recovery erschweren,
- Recovery Planning wird komplexer als ein pauschaler Neustart.

## Verworfene Alternativen

### Immer vollständigen Service neu starten

Verworfen, da einzelne Tasks oder Workloads häufig lokal wiederhergestellt werden können.

### Immer kleinstmöglichen Scope verwenden

Verworfen, da der kleinste technische Scope nicht zwangsläufig ausreichend oder sicher ist.

### Systemneustart als universeller Fallback

Verworfen, da systemweite Recovery einen sehr großen Blast Radius besitzt und nur bei entsprechendem Failure Scope eingesetzt werden soll.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0009_Structured_Concurrency_systemweit`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RECOVERY-SCOPE-0001`
- `NPSPEC-RESILIENCE-SCOPE-SELECTION-0001`
- `NPSPEC-RESILIENCE-SCOPE-ESCALATION-0001`
- `NPSPEC-RESILIENCE-SHARED-STATE-0001`
- `NPSPEC-RESILIENCE-BLAST-RADIUS-0001`
- `NPSPEC-RESILIENCE-RECOVERY-SCOPE-TEST-0001`

## Ergebnis

Nova.Resilience wählt Recovery nicht nach dem größtmöglichen Sicherheitsabstand, sondern nach dem kleinsten Scope, der den gültigen Zustand zuverlässig wiederherstellen kann:

```text
Failure
   ↓
Determine Actual Scope
   ↓
Select Smallest Safe Recovery
   ↓
Recover
   ↓
Verify
   ├── Success → Restore
   └── Failure → Expand Scope
```

Damit bleibt ein lokaler Fehler möglichst lokal, während NovaOS bei Bedarf kontrolliert auf größere Failure Domains eskalieren kann.