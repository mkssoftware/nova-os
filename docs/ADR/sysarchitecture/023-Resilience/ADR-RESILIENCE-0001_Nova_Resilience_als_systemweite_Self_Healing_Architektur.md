# ADR-RESILIENCE-0001 – Nova.Resilience als systemweite Self-Healing-Architektur

## Status

Angenommen

## Kategorie

Resilience Architecture / Self-Healing / Reliability

## Kontext

NovaOS soll Fehler nicht ausschließlich erkennen und an Benutzer oder Anwendungen weiterreichen, sondern soweit sicher möglich selbstständig auf Störungen reagieren können.

Fehler können auf unterschiedlichen Ebenen auftreten:

- Prozesse und Workloads,
- Dienste,
- Capability Provider,
- Treiber,
- Speicher und Daten,
- Systemkonfiguration,
- Ressourcen,
- Hardware,
- verteilte Komponenten.

Einzelne, voneinander unabhängige Recovery-Mechanismen würden zu inkonsistentem Verhalten führen und könnten sich gegenseitig beeinflussen.

NovaOS benötigt deshalb eine gemeinsame systemweite Resilience-Architektur.

## Entscheidung

NovaOS führt **`Nova.Resilience` als systemweite Self-Healing-Architektur** ein.

```text
Observe
   ↓
Detect
   ↓
Diagnose
   ↓
Plan
   ↓
Recover
   ↓
Verify
   ↓
Learn
```

`Nova.Resilience` koordiniert Fehlererkennung, Recovery, Zustandswiederherstellung und anschließende Verifikation über Systemgrenzen hinweg.

## Grundprinzip

```text
Detect failure
      ↓
Restore valid state
      ↓
Verify recovery
```

Self-Healing bedeutet nicht, Fehler beliebig zu verstecken.

Eine Recovery gilt erst als erfolgreich, wenn der resultierende Zustand überprüft wurde.

## Systemweite Resilience

`Nova.Resilience` bildet eine gemeinsame Architektur für unterschiedliche Systembereiche.

```text
                 Nova.Resilience
                       │
      ┌────────────────┼────────────────┐
      ↓                ↓                ↓
   Workloads        Services          Storage
      ↓                ↓                ↓
   Drivers          Providers         Data
      │                │                │
      └────────────────┼────────────────┘
                       ↓
                 Recovery Policy
```

Die konkreten Recovery-Mechanismen bleiben domänenspezifisch.

## Fehlererkennung

Fehler können durch unterschiedliche Evidence erkannt werden.

Beispiele:

```text
Crash
Timeout
Health Check Failure
Invariant Violation
Integrity Failure
Resource Exhaustion
Provider Failure
Hardware Error
Data Corruption
```

Ein einzelnes Signal muss nicht automatisch eine Recovery auslösen.

Policies bestimmen, wie Fehler klassifiziert und behandelt werden.

## Recovery Actions

Abhängig von Fehler und Kontext können unterschiedliche Recovery-Aktionen verwendet werden.

Beispiele:

```text
Retry
Restart
Provider Replacement
Failover
Rollback
State Reconstruction
Resource Reallocation
Isolation
Degraded Mode
Quarantine
```

Recovery-Aktionen müssen explizit definierte Sicherheits- und Zustandsbedingungen einhalten.

## Recovery Scope

Nova.Resilience soll möglichst den kleinsten sicheren Recovery Scope verwenden.

```text
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

Ein lokaler Fehler soll nicht unnötig einen vollständigen Systemneustart verursachen.

## Declarative Desired State

Nova.Resilience integriert sich mit dem Declarative System Model.

```text
Desired State
      ↓
Observed State
      ↓
Difference
      ↓
Recovery
      ↓
Desired State Restored
```

Self-Healing wird dadurch primär als Wiederherstellung eines gültigen deklarativen Systemzustands verstanden.

## Transaktionale Recovery

Wo möglich sollen Recovery-Aktionen transaktional ausgeführt werden.

```text
Current State
      ↓
Recovery Transaction
   ├── Commit
   └── Rollback
```

Fehlgeschlagene Recovery darf das System nicht unnötig in einen schlechter definierten Zwischenzustand versetzen.

## Capability Provider

Fällt ein Capability Provider aus, kann Nova.Resilience gemeinsam mit der Capability Registry einen alternativen Provider auswählen.

```text
Capability Request
       ↓
Provider A
       ↓ failure
Nova.Resilience
       ↓
Provider B
```

Die semantischen Anforderungen des ursprünglichen Capability- beziehungsweise Execution Contracts müssen dabei erhalten bleiben.

## State Recovery

Ein Neustart allein gilt nicht automatisch als vollständige Recovery.

Stateful Components benötigen definierte Mechanismen zur Wiederherstellung eines konsistenten Zustands.

```text
Restart
   +
State Restore
   +
Invariant Verification
   =
Recovery
```

## Verification

Jede automatische Recovery muss einen überprüfbaren Endzustand besitzen.

```text
Recovery Action
      ↓
Health / Integrity / Invariant Check
      ↓
Valid?
 ├── Yes → Resume
 └── No  → Escalate
```

Damit wird verhindert, dass eine technisch ausgeführte, aber wirkungslose Recovery als erfolgreich behandelt wird.

## Escalation

Nicht jeder Fehler kann automatisch behoben werden.

Nova.Resilience muss deshalb definierte Eskalationspfade unterstützen.

```text
Automatic Recovery
       ↓ failed
Alternative Recovery
       ↓ failed
Degraded / Isolated State
       ↓
Escalation
```

Sicherheit und Datenintegrität haben Vorrang vor erzwungener Verfügbarkeit.

## Learning

Nova.Resilience kann aus wiederkehrenden Fehlern und Recovery-Ergebnissen lernen.

```text
Prediction
    ↓
Recovery Decision
    ↓
Observed Result
    ↓
Prediction Error
    ↓
Model Adaptation
```

Lernende Verfahren dürfen Recovery-Auswahl und Priorisierung verbessern.

Sie dürfen jedoch keine Hard Constraints aus Security, Trust, Data Sovereignty oder System Correctness verändern.

## AI-Unabhängigkeit

Self-Healing darf nicht von KI abhängig sein.

Alle correctness-kritischen Recovery-Pfade müssen deterministisch und ohne AI-Komponenten funktionieren können.

KI kann Diagnose, Vorhersage und Optimierung unterstützen, ist aber keine Voraussetzung für grundlegende Systemwiederherstellung.

## Security und Trust

Recovery darf keine Sicherheitsgrenzen umgehen.

```text
Recovery
   ↓
Security
Trust
Capabilities
Data Sovereignty
Integrity
   ↓
Permitted Recovery Action
```

Eine Recovery-Aktion benötigt dieselben notwendigen Sicherheitsgarantien wie eine normale Systemoperation.

## Audit und Introspection

Nova.Resilience muss relevante Informationen über Fehler und Recovery bereitstellen können.

Dazu gehören insbesondere:

```text
Detected Failure
Affected Component
Recovery Decision
Executed Action
Result
Verification
Escalation
```

Damit bleibt nachvollziehbar, warum das System seinen Zustand verändert hat.

## Normative Anforderungen

1. NovaOS MUSS `Nova.Resilience` als systemweite Resilience- und Self-Healing-Architektur bereitstellen.
2. Fehlererkennung, Diagnose, Recovery und Verification MÜSSEN als getrennte Phasen modellierbar sein.
3. Recovery MUSS nach Möglichkeit den kleinsten sicheren Scope verwenden.
4. Nova.Resilience MUSS mit dem Declarative System Model integrierbar sein.
5. Recovery SOLL einen definierten Desired State wiederherstellen.
6. Recovery-Aktionen SOLLEN, wo sinnvoll, transaktional ausgeführt werden.
7. Capability Provider MÜSSEN bei Ausfall austauschbar sein können, sofern kompatible Provider verfügbar sind.
8. Stateful Components MÜSSEN definierte State-Recovery-Mechanismen bereitstellen können.
9. Eine Recovery DARF erst nach erfolgreicher Verification als abgeschlossen gelten.
10. Fehlgeschlagene Recovery MUSS eskalierbar sein.
11. Sicherheit und Datenintegrität MÜSSEN Vorrang vor erzwungener Verfügbarkeit besitzen.
12. Lernende Systeme DÜRFEN Recovery-Entscheidungen optimieren, aber keine Hard Constraints verändern.
13. Correctness-kritische Recovery DARF NICHT von KI abhängig sein.
14. Recovery DARF Capability-, Security-, Trust-, Integrity- oder Data-Sovereignty-Regeln NICHT umgehen.
15. Wiederholte Recovery-Schleifen MÜSSEN erkannt und begrenzt werden können.
16. Relevante Fehler-, Recovery- und Verification-Ereignisse MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- einheitliches Self-Healing-Modell für NovaOS,
- Fehler können möglichst lokal behandelt werden,
- Provider und Dienste können automatisch ersetzt werden,
- deklarativer Desired State bildet die Grundlage für Recovery,
- Recovery-Ergebnisse werden explizit verifiziert,
- lernende Optimierung bleibt von System Correctness getrennt.

### Negative Konsequenzen

- Komponenten benötigen definierte Health-, State- und Recovery-Schnittstellen,
- Recovery-Koordination über mehrere Subsysteme erhöht die Architekturkomplexität,
- fehlerhafte Recovery Policies können zusätzliche Störungen verursachen.

## Verworfene Alternativen

### Self-Healing separat in jeder Komponente

Verworfen, da dadurch inkonsistente Recovery-Semantik und unkoordinierte Recovery-Aktionen entstehen würden.

### Neustart als universelle Recovery

Verworfen, da Neustarts weder State Recovery noch Fehlerbehebung garantieren.

### KI als zentraler Recovery Controller

Verworfen, da correctness-kritische Systemfunktionen nicht von probabilistischen AI-Komponenten abhängig sein dürfen.

### Verfügbarkeit um jeden Preis

Verworfen, da Security, Trust und Datenintegrität höhere Priorität besitzen können.

## Abhängigkeiten

- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Voraussetzung`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-CORE-0001`
- `NPSPEC-RESILIENCE-FAILURE-DETECTION-0001`
- `NPSPEC-RESILIENCE-DIAGNOSIS-0001`
- `NPSPEC-RESILIENCE-RECOVERY-POLICY-0001`
- `NPSPEC-RESILIENCE-RECOVERY-ACTION-0001`
- `NPSPEC-RESILIENCE-STATE-RECOVERY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-ESCALATION-0001`
- `NPSPEC-RESILIENCE-LEARNING-0001`
- `NPSPEC-RESILIENCE-TEST-0001`

## Ergebnis

NovaOS erhält mit `Nova.Resilience` eine gemeinsame Architektur für systemweites Self-Healing:

```text
Observe
   ↓
Detect
   ↓
Diagnose
   ↓
Plan
   ↓
Recover
   ↓
Verify
   ↓
Learn
```

Damit wird Self-Healing nicht als Sammlung einzelner Neustartmechanismen verstanden, sondern als kontrollierte Wiederherstellung eines gültigen Systemzustands unter Einhaltung von Correctness, Security, Trust und Data Sovereignty.