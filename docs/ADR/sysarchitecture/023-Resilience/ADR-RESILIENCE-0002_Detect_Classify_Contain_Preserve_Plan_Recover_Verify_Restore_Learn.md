# ADR-RESILIENCE-0002 – Detect, Classify, Contain, Preserve, Plan, Recover, Verify, Restore, Learn

## Status

Angenommen

## Kategorie

Resilience Architecture / Recovery Pipeline / Self-Healing

## Kontext

`Nova.Resilience` benötigt einen einheitlichen Ablauf für die Behandlung erkannter Fehler.

Ein unmittelbarer Recovery-Versuch ist nicht immer sinnvoll. Vor einer Reparatur muss NovaOS beispielsweise bestimmen, welche Art von Fehler vorliegt, dessen Auswirkungen begrenzen und schützenswerte Zustände sichern.

Ebenso darf eine ausgeführte Recovery nicht automatisch als erfolgreich gelten. Der resultierende Zustand muss überprüft werden, bevor der normale Betrieb wiederhergestellt wird.

## Entscheidung

Nova.Resilience verwendet eine systemweit einheitliche **neunstufige Resilience Pipeline**:

```text
Detect
  ↓
Classify
  ↓
Contain
  ↓
Preserve
  ↓
Plan
  ↓
Recover
  ↓
Verify
  ↓
Restore
  ↓
Learn
```

Die Pipeline definiert die semantischen Phasen einer Recovery. Konkrete Implementierungen dürfen Phasen zusammenfassen oder überspringen, wenn deren Funktion für den jeweiligen Fehler nachweislich nicht erforderlich ist.

## 1. Detect

`Detect` erkennt eine Abweichung vom erwarteten Zustand.

Mögliche Signale sind:

```text
Crash
Timeout
Health Check Failure
Invariant Violation
Integrity Failure
Resource Exhaustion
Hardware Error
Data Corruption
```

Detection stellt zunächst fest, dass ein potenzieller Fehler vorliegt.

## 2. Classify

`Classify` bestimmt Art, Schweregrad und betroffenen Scope des Fehlers.

Beispielsweise:

```text
Transient
Persistent
Integrity Related
Resource Related
Hardware Related
Security Related
Data Related
Unknown
```

Die Klassifikation beeinflusst die nachfolgenden Recovery-Entscheidungen.

## 3. Contain

`Contain` begrenzt die Auswirkungen des Fehlers.

Mögliche Maßnahmen sind:

```text
Suspend Workload
Isolate Component
Stop Data Flow
Revoke Access
Disable Provider
Quarantine Resource
```

Containment soll verhindern, dass sich ein Fehler auf weitere Systembereiche ausbreitet.

## 4. Preserve

`Preserve` sichert relevante Informationen und Zustände, bevor Recovery-Maßnahmen diese verändern.

Dazu können gehören:

```text
State Snapshot
Transaction State
Diagnostic Evidence
Crash Information
Integrity Evidence
Causation Context
Relevant Logs
```

Dabei gelten weiterhin Privacy-, Security- und Data-Sovereignty-Regeln.

Preserve bedeutet ausdrücklich nicht, dass beliebige Secrets oder personenbezogene Daten vollständig gespeichert werden dürfen.

## 5. Plan

`Plan` bestimmt eine geeignete Recovery-Strategie.

```text
Failure Classification
        +
Current State
        +
Desired State
        +
Available Recovery Actions
        +
Security / Trust Constraints
        ↓
Recovery Plan
```

Nach Möglichkeit wird die kleinste sichere und ausreichende Recovery gewählt.

## 6. Recover

`Recover` führt den gewählten Recovery Plan aus.

Mögliche Aktionen sind:

```text
Retry
Restart
Rollback
Failover
Provider Replacement
State Reconstruction
Resource Reallocation
Hot Replacement
Degraded Mode
```

Recovery-Aktionen sollen, wo möglich, transaktional durchgeführt werden.

## 7. Verify

`Verify` prüft, ob die Recovery tatsächlich einen gültigen Zustand hergestellt hat.

```text
Recovered State
      ↓
Health Checks
Integrity Checks
Invariant Checks
Contract Validation
      ↓
Valid / Invalid
```

Nur eine erfolgreich verifizierte Recovery darf als erfolgreich gelten.

Bei fehlgeschlagener Verification erfolgt eine erneute Planung oder Eskalation.

## 8. Restore

`Restore` stellt nach erfolgreicher Verification den normalen oder definierten degradierten Betrieb wieder her.

Dazu können gehören:

```text
Resume Workload
Restore Routing
Re-enable Provider
Reconnect Dependencies
Release Quarantine
Resume Data Flow
```

`Recover` und `Restore` bleiben getrennt:

```text
Recover = gültigen Zustand herstellen
Restore = Betrieb kontrolliert wieder aufnehmen
```

## 9. Learn

`Learn` wertet Fehler, Recovery-Entscheidung und Ergebnis aus.

```text
Expected Result
      ↓
Observed Result
      ↓
Prediction Error
      ↓
Adaptation
```

Wiederkehrende Fehler oder ineffektive Recovery-Strategien können dadurch zukünftige Entscheidungen beeinflussen.

Learning ist eine Optimierungsschicht und keine Voraussetzung für System Correctness.

## Fehler während der Pipeline

Jede Phase kann selbst fehlschlagen.

Nova.Resilience muss deshalb definierte Eskalationspfade besitzen.

```text
Phase
  ↓ failure
Retry / Alternative
  ↓
Escalation
  ↓
Safe Degraded State / Isolation
```

Recovery-Schleifen müssen erkannt und begrenzt werden.

## Hard Constraints

Keine Phase der Pipeline darf grundlegende Systemgrenzen umgehen.

Insbesondere gelten weiterhin:

```text
Security
Trust
Capabilities
Data Sovereignty
Information Flow
Resource Budgets
Integrity
```

Diese Constraints haben Vorrang vor einer möglichst schnellen Wiederherstellung.

## Normative Anforderungen

1. Nova.Resilience MUSS die Phasen `Detect`, `Classify`, `Contain`, `Preserve`, `Plan`, `Recover`, `Verify`, `Restore` und `Learn` semantisch unterscheiden.
2. Detection MUSS Fehler oder relevante Abweichungen vom erwarteten Zustand erfassen können.
3. Classification MUSS Fehlerart, Schweregrad und betroffenen Scope bestimmen können.
4. Containment MUSS die Ausbreitung eines Fehlers begrenzen können.
5. Preserve MUSS relevante Recovery- und Diagnoseinformationen vor destruktiven Änderungen sichern können.
6. Preserve MUSS Privacy-, Security- und Data-Sovereignty-Regeln einhalten.
7. Planning MUSS Desired State, Fehlerklasse und Hard Constraints berücksichtigen.
8. Recovery SOLL den kleinsten sicheren ausreichenden Recovery Scope verwenden.
9. Recovery SOLL, wo sinnvoll, transaktional erfolgen.
10. Eine Recovery DARF erst nach erfolgreicher Verification als erfolgreich gelten.
11. Restore DARF erst nach ausreichender Verification erfolgen.
12. Fehlgeschlagene Phasen MÜSSEN eskalierbar sein.
13. Recovery-Schleifen MÜSSEN erkannt und begrenzt werden können.
14. Learning DARF Recovery-Strategien optimieren, aber keine Hard Constraints verändern.
15. Correctness-kritische Phasen MÜSSEN ohne KI funktionieren können.
16. Der Ablauf und seine relevanten Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- einheitlicher Recovery-Ablauf für unterschiedliche Subsysteme,
- Fehler werden vor Recovery zunächst eingegrenzt,
- wichtige Zustände und Evidence können erhalten bleiben,
- Recovery und Wiederaufnahme des Betriebs sind sauber getrennt,
- jede Recovery wird explizit verifiziert,
- Erfahrungen können zukünftige Recovery-Entscheidungen verbessern.

### Negative Konsequenzen

- Recovery benötigt mehr Zustands- und Kontextinformationen,
- Komponenten müssen geeignete Detection-, Verification- und Restore-Schnittstellen bereitstellen,
- komplexe Fehler können mehrere Pipeline-Durchläufe benötigen.

## Verworfene Alternativen

### Detect → Restart

Verworfen, da Klassifikation, Containment, State Preservation und Verification fehlen.

### Recover ohne Verification

Verworfen, da eine ausgeführte Recovery nicht zwangsläufig einen gültigen Zustand hergestellt hat.

### Recover und Restore als identische Phase

Verworfen, da die Reparatur eines Zustands und die Wiederaufnahme des normalen Betriebs unterschiedliche Sicherheitsbedingungen besitzen.

### Learn als correctness-kritische Phase

Verworfen, da NovaOS auch ohne lernende oder KI-basierte Komponenten zuverlässig wiederherstellbar bleiben muss.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Voraussetzung`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-PIPELINE-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-PRESERVATION-0001`
- `NPSPEC-RESILIENCE-PLANNING-0001`
- `NPSPEC-RESILIENCE-RECOVERY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-RESTORE-0001`
- `NPSPEC-RESILIENCE-LEARNING-0001`
- `NPSPEC-RESILIENCE-PIPELINE-TEST-0001`

## Ergebnis

Nova.Resilience verwendet einen klar definierten Recovery-Lifecycle:

```text
Detect
  ↓
Classify
  ↓
Contain
  ↓
Preserve
  ↓
Plan
  ↓
Recover
  ↓
Verify
  ↓
Restore
  ↓
Learn
```

Damit reagiert NovaOS auf Fehler nicht unmittelbar mit einer einzelnen Reparaturaktion, sondern führt eine kontrollierte Pipeline von der Erkennung über Eindämmung und Wiederherstellung bis zur Verifikation und anschließenden Verbesserung zukünftiger Recovery-Entscheidungen aus.