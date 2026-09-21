# ADR-AI-0012 – Agent Auditing

## Status

Angenommen

## Kategorie

AI / Agents / Auditing / Observability

## Kontext

AI-Agenten können selbstständig planen, Capabilities auswählen und mehrere Aktionen nacheinander ausführen.

Eine Agent-Ausführung kann beispielsweise so aussehen:

```text
User Intent
    ↓
Agent Plan
    ↓
Read Object
    ↓
Analyze Content
    ↓
Invoke AI Model
    ↓
Modify Object
    ↓
Send Result
```

Für Sicherheit, Fehlersuche und Nachvollziehbarkeit reicht es nicht, nur das Endergebnis zu protokollieren.

NovaOS muss rekonstruieren können:

```text
Wer startete den Agenten?
Was sollte er tun?
Welche Rechte hatte er?
Welche Aktionen führte er aus?
Welche Entscheidungen wurden getroffen?
Welche Daten und Capabilities wurden verwendet?
Was war das Ergebnis?
```

Dabei dürfen Audit-Daten selbst nicht zu einer unkontrollierten Sammlung sensibler Nutzer- oder AI-Daten werden.

## Entscheidung

NovaOS führt für Agent-Ausführungen ein systemweites **Agent Auditing** ein.

```text
Agent Execution
      ↓
Plans
Decisions
Capability Calls
Permission Checks
External Effects
Results
      ↓
Agent Audit Trail
```

Der Audit Trail verbindet Agent-, Execution-, Capability-, Security- und Decision-Ereignisse zu einer nachvollziehbaren Ausführungskette.

## Audit Identity

Jede Agent-Ausführung erhält korrelierbare Identifikatoren.

Mindestens:

```text
Agent ID
Agent Version
Execution ID
Sandbox ID
Initiator
```

Einzelne Aktionen erhalten zusätzlich eine:

```text
Action ID
```

## Audit Chain

Agent-Aktionen werden als zusammenhängende Kette erfasst.

```text
Execution ID
   ↓
Plan Version
   ↓
Action 1
   ↓
Action 2
   ↓
Replan
   ↓
Action 3
   ↓
Final Result
```

Dadurch kann eine Ausführung später rekonstruiert werden.

## Zu protokollierende Ereignisse

Relevante Ereignisse umfassen mindestens:

```text
Agent Start
Agent Stop
Plan Creation
Plan Change
Capability Request
Capability Grant
Capability Denial
Capability Invocation
Authorization Decision
User Confirmation
External Effect
Retry
Replanning
Sandbox Restriction
Capability Revocation
Failure
Final Result
```

Nicht jedes interne Model Token oder jeder Reasoning-Schritt wird protokolliert.

## Keine Speicherung interner Reasoning-Prozesse

Agent Auditing zeichnet Entscheidungen und beobachtbare Aktionen auf, nicht interne Modellgedanken.

Beispielsweise:

```text
Selected Action
Decision Inputs
Applicable Constraints
Authorization Result
Execution Result
```

statt eines vollständigen internen Reasoning-Protokolls.

## Capability Auditing

Jede relevante Capability Invocation muss mit der Agent-Ausführung korrelierbar sein.

```text
Agent
  ↓
Capability
  ↓
Authorization
  ↓
Execution
  ↓
Result
```

Erfasst werden können:

```text
Capability ID
Operation
Scope
Target Object
Authorization Result
Execution Result
```

## Permission Auditing

Änderungen an Agent Permissions werden erfasst.

```text
Grant
Attenuation
Delegation
Revocation
Expiration
Denial
```

Dadurch lässt sich feststellen, welche Authority ein Agent zu einem bestimmten Zeitpunkt tatsächlich besaß.

## Plan Auditing

Pläne erhalten eine Version.

```text
Plan v1
   ↓
Unexpected Result
   ↓
Plan v2
```

Ein Replanning überschreibt den vorherigen Plan nicht unbemerkt.

Auditierbar bleiben:

```text
Plan Version
Change Reason
Affected Actions
Decision ID
```

## External Effects

Extern sichtbare oder irreversible Aktionen erhalten erhöhte Audit-Relevanz.

Beispiele:

```text
Delete Object
Send Message
Publish Data
Modify Remote System
Change Configuration
Control Device
Export Data
```

Der Audit Trail muss erkennen lassen, welcher Agent diese Aktion ausgelöst und welche Authorization sie erlaubt hat.

## User Confirmation

Wenn eine Aktion eine Nutzerbestätigung benötigt:

```text
Agent Request
      ↓
Confirmation Required
      ↓
User Decision
      ↓
Execute / Deny
```

wird die Entscheidung mit der betreffenden Action ID verbunden.

Sensitive Inhalte der Bestätigung müssen dafür nicht vollständig gespeichert werden.

## Context Auditing

Agent Auditing zeichnet auf, welche Context Views verwendet wurden.

Bevorzugt werden Referenzen:

```text
Context View ID
Context Types
Object IDs
Object Versions
Context Version
```

statt vollständiger Kopien der Kontextdaten.

## AI Model Auditing

Für AI-gestützte Agent-Entscheidungen müssen relevante Runtime-Informationen korrelierbar sein:

```text
Model ID
Model Version
Provider
Execution ID
Determinism Mode
```

Prompts und Outputs werden nicht automatisch vollständig in den Audit Trail übernommen.

## Privacy

Agent Auditing folgt Data Minimization.

```text
Audit what happened
not everything that was processed
```

Sensitive Inhalte werden nur gespeichert, wenn dies für einen definierten Audit-Zweck erforderlich und erlaubt ist.

## Secrets

Audit Records dürfen keine Secrets enthalten, sofern dies vermeidbar ist.

Beispiele:

```text
Passwords
Private Keys
Access Tokens
Session Secrets
Raw Credentials
```

Stattdessen werden Referenzen oder nicht-sensitive Identifikatoren verwendet.

## Tamper Evidence

Security-relevante Audit Records müssen gegen unbemerkte Veränderung geschützt werden können.

Beispielsweise durch:

```text
Append-Only Storage
Hash Chains
Signed Checkpoints
Trusted Timestamps
```

Die konkrete Implementierung wird durch die Audit-Infrastruktur definiert.

## Audit Levels

NovaOS kann unterschiedliche Audit Levels unterstützen.

Beispiel:

```text
Minimal
Standard
Security
Diagnostic
```

Hard Security Events dürfen durch einen niedrigeren Agent-Audit-Level nicht vollständig deaktiviert werden.

## Retention

Agent Audit Records besitzen definierte Retention Policies.

```text
Create
  ↓
Retain
  ↓
Expire
  ↓
Delete / Archive
```

Audit-Daten werden nicht grundsätzlich unbegrenzt gespeichert.

## Distributed Agents

Bei verteilten Agent-Ausführungen müssen Audit-Ereignisse über Nodes hinweg korrelierbar bleiben.

```text
Agent Execution
   ├── Node A
   ├── Node B
   └── Remote Provider
```

Gemeinsame IDs ermöglichen die Rekonstruktion der verteilten Ausführung.

## Failure und Unknown State

Auch unvollständige Agent-Ausführungen müssen auditierbar sein.

```text
Action Started
     ↓
Connection Lost
     ↓
Result Unknown
```

Der Audit Trail darf daraus nicht fälschlich:

```text
Success
```

oder:

```text
Failure
```

ableiten.

`Unknown` bleibt ein eigener Zustand.

## Deterministic Mode

Für deterministische Agent-Ausführungen müssen relevante Audit-Daten eine spätere Rekonstruktion unterstützen.

Dazu gehören beispielsweise:

```text
Agent Version
Model Version
Plan Version
Capability Versions
Context Version
Policy Version
Execution Contract
```

Auditierbarkeit bedeutet jedoch nicht automatisch vollständige Replay-Fähigkeit.

## Zugriff auf Audit-Daten

Audit-Daten sind selbst geschützte Systemobjekte.

```text
Audit Data
    ↓
Authorization
    ↓
Authorized Viewer
```

Agenten dürfen ihre eigenen Audit Records nicht automatisch verändern oder löschen.

## Agent Self-Audit

Ein Agent darf Informationen über seine eigene aktuelle Ausführung erhalten, wenn dies autorisiert ist.

Er darf jedoch nicht:

```text
Rewrite Audit History
Delete Security Events
Forge Audit Events
Suppress Mandatory Events
```

## Observability Integration

Agent Auditing verwendet die bestehende NovaOS-Observability-Infrastruktur.

```text
Agent Runtime
     ↓
Decision Tracing
Distributed Tracing
Resource Tracing
Security Audit
     ↓
Correlated Agent Audit Trail
```

Es entsteht keine vollständig getrennte Logging-Infrastruktur nur für AI-Agenten.

## Normative Anforderungen

1. Jede Agent-Ausführung MUSS eindeutig auditierbar sein.
2. Agent-, Execution-, Sandbox- und Action-IDs MÜSSEN korrelierbar sein.
3. Capability Invocations MÜSSEN der verursachenden Agent-Ausführung zugeordnet werden können.
4. Capability Grants, Denials und Revocations MÜSSEN auditierbar sein.
5. Planänderungen und Replanning MÜSSEN nachvollziehbar bleiben.
6. Extern sichtbare und irreversible Agent-Aktionen MÜSSEN auditierbar sein.
7. Erforderliche Nutzerbestätigungen MÜSSEN mit der jeweiligen Aktion korrelierbar sein.
8. Context-Nutzung SOLL bevorzugt über IDs und Versionen statt Inhaltskopien protokolliert werden.
9. Verwendete AI-Modelle und Provider MÜSSEN mit der Agent-Ausführung korrelierbar sein.
10. Interne Model-Reasoning-Prozesse MÜSSEN nicht als vollständiger Audit Trail gespeichert werden.
11. Agent Auditing MUSS Data Minimization berücksichtigen.
12. Secrets DÜRFEN nicht unnötig in Audit Records gespeichert werden.
13. Security-relevante Audit Records MÜSSEN gegen unbemerkte Manipulation schützbar sein.
14. Audit Records MÜSSEN einer Retention Policy unterliegen.
15. Verteilte Agent-Ausführungen MÜSSEN über Systemgrenzen hinweg korrelierbar sein.
16. `Unknown` MUSS als eigener Execution- und Action-Status darstellbar sein.
17. Deterministic Mode MUSS relevante Versionen und Policies im Audit rekonstruierbar machen können.
18. Zugriff auf Agent Audit Records MUSS autorisiert werden.
19. Agenten DÜRFEN verpflichtende Audit-Ereignisse nicht unterdrücken.
20. Agenten DÜRFEN ihre Audit-Historie nicht selbst manipulieren.
21. Agent Auditing SOLL die bestehende NovaOS-Observability-Infrastruktur verwenden.
22. Auditing DARF nicht von der Kooperation des verwendeten AI-Modells abhängen.

## Konsequenzen

### Positive Konsequenzen

- Agent-Aktionen werden systemweit nachvollziehbar,
- Sicherheitsvorfälle können einer konkreten Agent-Ausführung zugeordnet werden,
- Capability Grants und tatsächliche Nutzung lassen sich vergleichen,
- Replanning und Multi-Step-Ausführungen bleiben rekonstruierbar,
- verteilte Agent-Ausführungen können über gemeinsame IDs verfolgt werden,
- Auditing funktioniert unabhängig vom eingesetzten AI-Modell.

### Negative Konsequenzen

- Agent-Ausführungen erzeugen zusätzliche Audit-Daten,
- Retention und Zugriffsschutz müssen verwaltet werden,
- starke Auditierbarkeit erfordert systemweite Korrelation zwischen mehreren Subsystemen.

## Abhängigkeiten

- `ADR-AI-0008_Context_Services`
- `ADR-AI-0009_Agent_Runtime`
- `ADR-AI-0010_Agent_Sandboxing`
- `ADR-AI-0011_Agent_Permissions_über_Capabilities`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_und_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0008_Provenance_Inspection`
- `ADR-PRIVACY-0001`
- `ADR-SECURITY-0003_Least_Privilege`

## Zugehörige NPSPECs

- `NPSPEC-AI-AGENT-AUDIT-0001`
- `NPSPEC-AI-AGENT-AUDIT-EVENT-0001`
- `NPSPEC-AI-AGENT-AUDIT-CORRELATION-0001`
- `NPSPEC-AI-AGENT-AUDIT-INTEGRITY-0001`
- `NPSPEC-AI-AGENT-AUDIT-RETENTION-0001`
- `NPSPEC-AI-AGENT-AUDIT-PRIVACY-0001`

## Ergebnis

Agent Auditing erzeugt eine nachvollziehbare Kette der tatsächlich relevanten Systemereignisse:

```text
Intent
  ↓
Agent
  ↓
Plan
  ↓
Permission Decision
  ↓
Capability Invocation
  ↓
System Effect
  ↓
Result
  ↓
Audit Trail
```

Der zentrale Grundsatz lautet:

```text
NovaOS muss nachvollziehen können,
was ein Agent tatsächlich getan hat,
mit welchen Rechten,
auf welcher Grundlage
und mit welchem Ergebnis.

Auditiert werden Entscheidungen
und beobachtbare Systemaktionen –

nicht die vollständigen
internen Gedanken des AI-Modells.
```