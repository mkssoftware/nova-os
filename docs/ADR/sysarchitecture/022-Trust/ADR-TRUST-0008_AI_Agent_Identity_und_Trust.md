# ADR-TRUST-0008 – AI Agent Identity und Trust

## Status

Angenommen

## Kategorie

Trust Architecture / AI Agents / Agent Governance

## Kontext

KI-Agenten können in NovaOS eigenständig Aufgaben planen, Capabilities aufrufen, Daten verarbeiten, mit Diensten kommunizieren und im Auftrag von Benutzern oder anderen Systemkomponenten handeln.

Dabei darf ein Agent weder mit der Identity seines Auftraggebers gleichgesetzt werden noch allein aufgrund seiner Herkunft oder seines Modells als vertrauenswürdig gelten.

NovaOS benötigt deshalb ein einheitliches Modell für **Agent Identity, Delegation, Trust und Authority**.

## Entscheidung

KI-Agenten erhalten in NovaOS eine explizite **Agent Identity** und werden vollständig in Nova.Trust integriert.

```text
Principal
    ↓
Delegated Context
    ↓
AI Agent Identity
    ↓
Nova.Trust
    ↓
Capabilities
    ↓
Agent Action
    ↓
Audit
```

Ein Agent handelt als eigene identifizierbare Entität innerhalb eines delegierten Kontexts.

## Grundprinzip

```text
Agent Identity ≠ User Identity
Agent Trust    ≠ Agent Authority
```

Zusätzlich gilt:

```text
Acting for a user
≠
Becoming the user
```

## Agent Identity

Jede sicherheitsrelevant handelnde Agenteninstanz muss eindeutig identifizierbar sein.

NovaOS unterscheidet insbesondere:

```text
Agent Identity
      ↓
Agent Instance Identity
      ↓
Current Execution Domain
```

Eine stabile Agent Identity kann einen langfristig bekannten Agenten repräsentieren.

Kurzlebige Agenteninstanzen können zusätzlich eine Ephemeral Workload Identity erhalten.

## Delegierter Kontext

Agenten handeln im Auftrag eines Principals über einen expliziten Delegated Context.

Dieser kann unter anderem enthalten:

- Auftraggeber,
- Agent Identity,
- delegierte Capabilities,
- Purpose,
- Scope,
- Lifetime,
- Resource Budget,
- Data-Sovereignty Constraints,
- Trust Requirements,
- Causation.

Die Identity des ursprünglichen Auftraggebers bleibt dabei erhalten und nachvollziehbar.

## Agent Trust

Nova.Trust kann Agenten anhand verschiedener Evidence bewerten.

Dazu können beispielsweise gehören:

```text
Agent Identity
Software Provenance
Model Provenance
Runtime Integrity
Execution Domain
Publisher Identity
Configuration
Trust Anchors
Previous Security State
```

Die konkrete Trust-Bewertung wird durch Trust Policies bestimmt.

## Modell und Agent

NovaOS unterscheidet zwischen:

```text
AI Model
AI Agent
Agent Instance
```

Ein Modell ist ein ausführbarer beziehungsweise verwendbarer KI-Bestandteil.

Ein Agent ist eine handelnde Systementität, die ein oder mehrere Modelle und weitere Capabilities verwenden kann.

Ein Agentenwechsel muss deshalb nicht automatisch einen Modellwechsel bedeuten und umgekehrt.

## Authority

Agenten erhalten keine implizite Authority.

```text
Agent
  +
Delegated Capabilities
  +
System Policies
  ↓
Permitted Actions
```

Ein vertrauenswürdiger Agent darf ohne entsprechende Capability keine geschützte Operation durchführen.

## Agentenketten

Ein Agent kann andere Agenten oder Dienste verwenden.

Dabei muss die Delegations- und Causation-Kette erhalten bleiben.

```text
User
  ↓
Agent A
  ↓
Agent B
  ↓
Capability Provider
```

Agent A darf Agent B nicht mehr Authority übertragen, als Agent A selbst delegieren darf.

## Adaptive Systeme

Lernende oder adaptive Eigenschaften eines Agenten dürfen seine Authority nicht selbstständig erweitern.

```text
Learning
Prediction
Optimization
        ↓
Soft Policy Adaptation
```

Hard Constraints aus Security, Privacy, Sovereignty, Trust und Capability Authority bleiben davon unberührt.

## Remote Agents

Remote ausgeführte Agenten verwenden dasselbe Identity-, Trust- und Delegationsmodell.

Location Transparency darf dabei nicht dazu führen, dass Trust-, Sovereignty- oder Authority-Grenzen verborgen werden.

## Audit

Sicherheitsrelevante Agentenoperationen müssen die Causation Chain nachvollziehbar erhalten können.

```text
Who requested?
      ↓
Which agent acted?
      ↓
Under which delegated authority?
      ↓
Which capability was used?
      ↓
What happened?
```

## Normative Anforderungen

1. Sicherheitsrelevant handelnde KI-Agenten MÜSSEN eine explizite Identity besitzen.
2. Agent Identity und User Identity MÜSSEN getrennt bleiben.
3. Agenteninstanzen MÜSSEN von ihrer stabilen Agent Identity unterscheidbar sein können.
4. Agenten MÜSSEN über explizite delegierte Authority handeln.
5. Agent Trust DARF keine Capability Authority erzeugen.
6. Nova.Trust MUSS Agenten als native Trust-Entitäten unterstützen.
7. Software-, Modell- und Runtime-Provenance MÜSSEN als Agent Trust Evidence verwendbar sein können.
8. AI Model, AI Agent und Agent Instance MÜSSEN semantisch getrennte Entitäten sein.
9. Delegation zwischen Agenten MUSS die bestehenden Authority-Grenzen einhalten.
10. Adaptive oder lernende Mechanismen DÜRFEN Hard Constraints und Authority nicht selbstständig verändern.
11. Remote Agents MÜSSEN denselben grundlegenden Trust- und Delegationsregeln unterliegen.
12. Sicherheitsrelevante Agentenaktionen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Agenten können eindeutig identifiziert und kontrolliert werden,
- Benutzer müssen ihre eigene Identity nicht an Agenten abgeben,
- Agent Trust und Agent Authority bleiben getrennt,
- Multi-Agent-Systeme behalten nachvollziehbare Delegationsketten,
- lokale und entfernte Agenten verwenden dasselbe Sicherheitsmodell,
- Nova.Trust bildet eine gemeinsame Grundlage für AI Governance.

### Negative Konsequenzen

- Agent Runtime und AI Framework müssen Identity- und Delegation-Kontexte propagieren,
- Multi-Agent-Systeme benötigen konsequente Causation- und Authority-Verfolgung.

## Verworfene Alternativen

### Agent handelt direkt als Benutzer

Verworfen, da dadurch Verantwortlichkeit, Delegation und Least Privilege verloren gehen.

### Vertrauenswürdiger Agent erhält automatisch weitreichende Rechte

Verworfen, da Trust keine Authority ersetzt.

### KI-Agenten außerhalb des normalen Security-Modells behandeln

Verworfen, da dadurch eine parallele und schwer kontrollierbare Sicherheitsarchitektur entstehen würde.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0005_Trust_Graph_statt_globalem_Trust_Score`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-IDENTITY-0003_Ephemeral_Workload_Identity`
- `ADR-IDENTITY-0008_Delegated_Context_statt_Impersonation`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Voraussetzung`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-AI-AGENT-IDENTITY-0001`
- `NPSPEC-TRUST-AI-AGENT-INSTANCE-0001`
- `NPSPEC-TRUST-AI-AGENT-EVIDENCE-0001`
- `NPSPEC-TRUST-AI-AGENT-DELEGATION-0001`
- `NPSPEC-TRUST-AI-AGENT-AUDIT-0001`
- `NPSPEC-TRUST-AI-AGENT-TEST-0001`

## Ergebnis

KI-Agenten werden in NovaOS nicht als Sonderfall außerhalb der Systemarchitektur behandelt:

```text
Principal
    ↓
Delegated Context
    ↓
Agent Identity
    ↓
Nova.Trust
    ↓
Capability Authority
    ↓
Execution
    ↓
Audit
```

Damit können KI-Agenten autonom arbeiten, ohne Identity, Trust und Authority miteinander zu vermischen. Agenten bleiben eindeutig identifizierbare, begrenzte und nachvollziehbare Teilnehmer der NovaOS-Architektur.