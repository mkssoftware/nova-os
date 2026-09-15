# ADR-UPDATE-0001 – Deklarativer Softwarezustand

## Status

Angenommen

## Kategorie

Software Architecture / State Management / System Model

## Kontext

NovaOS soll Software nicht primär als Folge imperativer Befehle verwalten, sondern über einen **deklarativ beschriebenen gewünschten Zustand**.

Klassische imperative Systeme beschreiben hauptsächlich:

```text
Do A
  ↓
Do B
  ↓
Do C
```

NovaOS beschreibt dagegen:

```text
Desired State
      ↓
Reconciliation
      ↓
Actual State
```

Dadurch können Änderungen, Recovery, Updates, Migrationen und Selbstheilung auf einer gemeinsamen Zustandsbeschreibung aufbauen.

## Entscheidung

NovaOS verwendet einen **deklarativen Softwarezustand** als zentrale Beschreibung des gewünschten Zustands von Software und ihren systemrelevanten Eigenschaften.

Der deklarative Zustand beschreibt insbesondere:

- gewünschte Softwareversion,
- aktivierte oder deaktivierte Komponenten,
- Konfiguration,
- Abhängigkeiten,
- Capabilities,
- Ressourcenanforderungen,
- Sicherheitsanforderungen,
- Daten- und Speicheranforderungen,
- gewünschte Betriebszustände.

Der deklarative Zustand ist eine Beschreibung dessen, **was gelten soll**, nicht zwingend eine Beschreibung dessen, **wie es hergestellt wird**.

## Grundprinzip

```text
Declared Desired State
          ↓
      Validation
          ↓
     Reconciliation
          ↓
      Actual State
```

Die Implementierung darf den notwendigen Ausführungsweg selbst bestimmen, solange die expliziten System- und Sicherheitsverträge eingehalten werden.

## Desired State und Actual State

NovaOS unterscheidet strikt zwischen:

```text
Desired State
    = Was soll gelten?

Actual State
    = Was gilt tatsächlich?
```

Beide Zustände können vorübergehend voneinander abweichen.

```text
Desired State
      │
      │ Reconciliation
      ↓
Actual State
```

Eine Abweichung ist nicht automatisch ein Fehler. Sie kann beispielsweise durch:

- laufende Installation,
- laufendes Update,
- Ressourcenmangel,
- Recovery,
- Hardwareänderungen,
- temporäre Provider-Ausfälle

entstehen.

## Zustandsänderung

Eine Änderung des gewünschten Softwarezustands erfolgt als transaktionale Zustandsänderung.

```text
Current Desired State
          ↓
     New Desired State
          ↓
       Validate
          ↓
      Transaction
          ↓
   Desired State'
```

Die Änderung des deklarativen Zustands muss atomar sichtbar werden.

Die anschließende physische Umsetzung kann länger dauern.

## Reconciliation

Ein Reconciler vergleicht Desired State und Actual State und bestimmt die notwendigen Aktionen.

```text
Desired State ─────┐
                   ↓
               Reconciler
                   ↓
Actual State ──────┘
                   ↓
             Operations
```

Der Reconciler darf keine Sicherheits- oder Konsistenzregeln umgehen.

Er muss insbesondere die geltenden:

- Capabilities,
- Security Policies,
- Data-Sovereignty-Regeln,
- Resource Budgets,
- Execution Contracts

berücksichtigen.

## Idempotenz

Reconciliation soll idempotent sein.

Wird derselbe gewünschte Zustand mehrfach angewendet, darf daraus kein unerwünschter zusätzlicher Seiteneffekt entstehen.

```text
Desired State X
      ↓
Reconcile
      ↓
Actual State X

Reconcile erneut
      ↓
keine unnötige Zustandsänderung
```

Dies ist insbesondere für Recovery und Self-Healing erforderlich.

## Softwarezustand als Systemobjekt

Der deklarative Softwarezustand wird als systemverwaltetes Objekt behandelt.

Er kann dadurch:

- versioniert,
- transaktional geändert,
- validiert,
- persistiert,
- verglichen,
- wiederhergestellt,
- auditiert

werden.

Die konkrete Speicherung folgt der Storage-Architektur.

## Versionierung

Jede relevante Änderung des deklarativen Zustands muss einer Version beziehungsweise einem eindeutig identifizierbaren Zustand zugeordnet werden können.

```text
State V1
   ↓
Update
   ↓
State V2
   ↓
Update
   ↓
State V3
```

Damit werden:

- Rollback,
- Recovery,
- Diff,
- Audit,
- Update-Analyse

ermöglicht.

## Update

Ein Softwareupdate verändert zunächst den gewünschten Zustand.

```text
Installed V1
Desired V1
     ↓
Update Request
     ↓
Desired V2
     ↓
Reconciliation
     ↓
Installed V2
```

Das Update ist damit nicht primär eine Folge einzelner Installationsbefehle, sondern eine Änderung des deklarativen Zielzustands.

## Fehler während eines Updates

Wenn die Umsetzung des gewünschten Zustands fehlschlägt, bleibt der gewünschte Zustand erhalten, sofern die Transaktion erfolgreich committed wurde.

```text
Desired V2
   ↓
Reconciliation
   ↓
Failure
   ↓
Recovery / Retry / Rollback
```

NovaOS darf nicht einfach den Desired State stillschweigend verändern, um einen Implementierungsfehler zu verstecken.

Eine bewusste Änderung des Desired State muss eine eigene Zustandsänderung sein.

## Rollback

Ein vorheriger deklarativer Zustand kann als Zielzustand wiederhergestellt werden.

```text
Desired V1
   ↓
Desired V2
   ↓
Problem
   ↓
Rollback
   ↓
Desired V1
```

Die tatsächliche Wiederherstellung des Systems erfolgt anschließend durch Reconciliation.

## Partial State

Ein System darf während der Reconciliation einen teilweise angewendeten Zustand besitzen.

Dieser Zustand muss erkennbar sein.

```text
Desired State V2
      ↓
Reconciliation
      ↓
Partial Actual State
      ↓
Continue / Recover
```

Ein partieller Actual State darf nicht fälschlicherweise als vollständig konform zum Desired State gemeldet werden.

## Abhängigkeiten

Softwarezustände können voneinander abhängen.

```text
Application
    ↓
Runtime
    ↓
System Capability
    ↓
Hardware Provider
```

Der deklarative Zustand muss solche Abhängigkeiten beschreiben können.

Die tatsächliche Auflösung erfolgt über die zuständigen Resolver und Provider.

## Capabilities

Software soll benötigte Fähigkeiten deklarieren können, ohne zwingend einen konkreten Provider vorzugeben.

```text
Application
    ↓
Required Capability
    ↓
Capability Resolver
    ↓
Provider
```

Dadurch bleibt die Software gegenüber austauschbaren Implementierungen entkoppelt.

## Ressourcen

Der deklarative Softwarezustand kann Ressourcenanforderungen enthalten.

Beispiele:

```text
CPU
Memory
Storage
GPU
NPU
Network
Energy
```

Die tatsächliche Ressourcenbereitstellung erfolgt über die System Resource Economy.

Ein Desired State darf nicht automatisch eine Ressourcenzuteilung erzwingen, wenn entsprechende Policies oder Budgets dies nicht erlauben.

## Security

Der deklarative Zustand darf keine Berechtigungen allein durch seine Existenz erhalten.

```text
Desired State
      ≠
Authorization
```

Jede Änderung muss über die normale Security- und Capability-Architektur autorisiert werden.

Ein manipuliertes Desired-State-Objekt darf nicht dazu führen, dass unautorisierte Software oder Capabilities aktiviert werden.

## Data Sovereignty

Softwarezustände können Datenflüsse, Speicherorte und Provideranforderungen beschreiben.

Diese Eigenschaften müssen mit den geltenden Data-Sovereignty-Regeln kompatibel sein.

```text
Desired State
      ↓
Data Policy Validation
      ↓
Reconciliation
```

Ein gewünschter Zustand darf nicht hergestellt werden, wenn seine Umsetzung eine harte Data-Sovereignty-Regel verletzt.

## Determinismus

Für sicherheits- oder korrektheitskritische Zustände muss die Reconciliation-Semantik deterministisch beziehungsweise explizit definiert sein.

Adaptive Auswahl darf nur dort verwendet werden, wo sie keine harte Systemgarantie verändert.

```text
Hard Constraint
      ↓
Deterministic Decision

Soft Preference
      ↓
Adaptive Decision
```

## Recovery

Der deklarative Softwarezustand bildet eine wichtige Grundlage für Recovery.

Nach einem Fehler kann NovaOS den gewünschten Zustand erneut gegen den tatsächlichen Zustand prüfen.

```text
Failure
   ↓
Actual State
   ↓
Desired State
   ↓
Reconciliation
   ↓
Verified State
```

Damit kann Recovery denselben Zustandsmechanismus verwenden wie normale Updates.

## Self-Healing

Self-Healing basiert auf der Erkennung einer Abweichung zwischen Desired State und Actual State.

```text
Desired State
      ↓
Compare
      ↓
Deviation
      ↓
Recovery Action
      ↓
Verification
```

Die Selbstheilung darf den Desired State nicht eigenmächtig verändern, sofern keine explizite Policy dies erlaubt.

## Benutzerinteraktion

Benutzer müssen nicht jeden einzelnen technischen Reconciliation-Schritt bestätigen.

Die relevante Benutzerentscheidung ist grundsätzlich die Änderung des gewünschten Zustands.

```text
User Intent
    ↓
Desired State Change
    ↓
System Reconciliation
```

Das reduziert unnötige Interaktion und hält die technische Umsetzung vom Benutzerwunsch getrennt.

## Audit und Provenance

Änderungen am deklarativen Softwarezustand müssen nachvollziehbar sein.

Es muss feststellbar sein können:

- welcher Zustand geändert wurde,
- welcher neue Zustand entstand,
- wann die Änderung erfolgte,
- welcher Kontext die Änderung ausgelöst hat,
- welche Reconciliation daraus resultierte.

Dabei gelten die bestehenden Privacy- und Audit-Regeln.

## Normative Anforderungen

1. NovaOS MUSS einen deklarativen Desired State für systemrelevante Softwarezustände unterstützen.
2. Desired State und Actual State MÜSSEN getrennt behandelt werden.
3. Änderungen des Desired State MÜSSEN transaktional erfolgen.
4. Der deklarative Zustand MUSS versionierbar sein.
5. Reconciliation MUSS Desired State und Actual State vergleichen können.
6. Reconciliation SOLL idempotent sein.
7. Ein partieller Actual State DARF nicht als vollständig konform gemeldet werden.
8. Fehler bei der Umsetzung DÜRFEN den Desired State nicht stillschweigend verändern.
9. Rollback MUSS über die Wiederherstellung eines vorherigen gültigen Desired State unterstützt werden können.
10. Der deklarative Softwarezustand MUSS Security- und Capability-Regeln unterliegen.
11. Der deklarative Softwarezustand MUSS Data-Sovereignty-Regeln berücksichtigen.
12. Ressourcenanforderungen MÜSSEN mit der System Resource Economy abgeglichen werden.
13. Abhängigkeiten MÜSSEN deklarativ beschreibbar sein.
14. Capability-Anforderungen SOLLEN providerunabhängig beschrieben werden können.
15. Recovery MUSS den Desired State als Referenz für die Wiederherstellung verwenden können.
16. Self-Healing MUSS zwischen Desired State und Actual State unterscheiden.
17. Adaptive Mechanismen DÜRFEN keine harten Security-, Safety-, Determinismus- oder Konsistenzgarantien außer Kraft setzen.
18. Änderungen am Desired State MÜSSEN nachvollziehbar sein.
19. Ein Desired State DARF allein keine zusätzliche Authority erzeugen.
20. Die tatsächliche Reconciliation MUSS vor dem Abschluss entsprechend den geltenden Recovery- und Verification-Regeln überprüft werden.

## Konsequenzen

### Positive Konsequenzen

- Softwarezustand wird explizit und nachvollziehbar,
- Updates werden zu kontrollierten Zustandsänderungen,
- Recovery und Self-Healing können denselben Mechanismus verwenden,
- Rollback wird auf Zustands- statt auf Befehlebene möglich,
- Provider und Implementierungsdetails bleiben vom gewünschten Zustand getrennt,
- wiederholbare und idempotente Reconciliation wird möglich.

### Negative Konsequenzen

- Desired State und Actual State müssen zusätzlich verwaltet werden,
- Reconciliation benötigt eigene Systemlogik,
- teilweise angewendete Zustände müssen explizit behandelt werden,
- Zustandsversionierung erhöht den Verwaltungsaufwand.

## Verworfene Alternativen

### Rein imperative Updates

Verworfen, da Zustand, Recovery und Wiederholbarkeit dadurch schwerer systemweit beschreibbar werden.

### Desired State direkt als Actual State behandeln

Verworfen, da physische Umsetzung Zeit benötigen und fehlschlagen kann.

### Reconciler darf Desired State automatisch ändern

Verworfen, da dadurch Benutzerintention und Systemfehler vermischt würden.

### Jede Reconciliation als Benutzerbestätigung

Verworfen, da technische Umsetzungsschritte keine eigenständige Benutzerentscheidung benötigen sollen.

### Softwarezustand ausschließlich über Paketinstallation verwalten

Verworfen, da Paketinstallation nur einen Teil des systemweiten Softwarezustands beschreibt.

## Abhängigkeiten

- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0003_Configuration_Transactions`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0020_Explainable_Self-Healing`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-0001_Declarative_Software_State`
- `NPSPEC-UPDATE-0002_Desired_State`
- `NPSPEC-UPDATE-0003_Actual_State`
- `NPSPEC-UPDATE-0004_Reconciliation`
- `NPSPEC-UPDATE-0005_Software_State_Versioning`
- `NPSPEC-UPDATE-0006_Software_State_Recovery`
- `NPSPEC-UPDATE-0007_Software_State_Verification`
- `NPSPEC-UPDATE-0008_Declarative_Software_State_Test`

## Ergebnis

NovaOS behandelt den Softwarezustand als deklarative Zustandsbeschreibung:

```text
User Intent
    ↓
Desired Software State
    ↓
Validation
    ↓
Transactional State Change
    ↓
Reconciliation
    ↓
Actual Software State
    ↓
Verification
```

Damit werden Update, Recovery, Rollback und Self-Healing auf eine gemeinsame Zustandsarchitektur gestellt.

Der zentrale Grundsatz lautet:

```text
NovaOS beschreibt,
was gelten soll,
und reconciliert,
wie dieser Zustand erreicht wird.
```