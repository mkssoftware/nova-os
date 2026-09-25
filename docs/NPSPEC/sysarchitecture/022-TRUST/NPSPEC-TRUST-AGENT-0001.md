# NPSPEC-TRUST-AGENT-0001 – Nova Agent Trust

## Status

Angenommen

## Kategorie

Trust / Agents / Autonomous Systems

## Zweck

NovaOS definiert Agent Trust für autonome oder teilautonome Software-Agenten, die im Auftrag eines Nutzers, Dienstes oder Systembestandteils handeln.

```text
Principal
   ↓
Delegation
   ↓
Agent Identity
   ↓
Agent Trust Evaluation
   ↓
Capabilities + ExecutionContract
   ↓
Controlled Action
```

Ein Agent erhält weder durch Intelligenz noch durch Autonomie automatisch Vertrauen oder Autorität.

## Grundprinzipien

```text
Agent ≠ Principal
Agent Identity ≠ User Identity
Trust ≠ Authority
Delegation ≠ Unlimited Authority
Autonomy ≠ Permission
AI Decision ≠ Trusted Decision
Agent Output ≠ Verified Result
Trusted Agent ≠ Trusted Action
```

## Agent-Trust-Modell

Eine Bewertung besitzt mindestens:

```text
AgentTrust
├── Agent Identity
├── Principal Identity
├── Trust Domain
├── Delegation Context
├── Trust State
└── State Version
```

Optional:

```text
Agent Type
Software Trust
Model Identity
Provider
Execution Environment
Attestation
Provenance
Security Domain
ExecutionContract
Expiration
```

## Agent Identity

Jeder sicherheitsrelevante Agent muss eine eigene Identity besitzen.

```text
User Identity
     ↓ delegates
Agent Identity
     ↓ acts through
Capabilities
```

Ein Agent darf nicht einfach unter der Identität seines Principals auftreten.

## Delegation

Autorität muss explizit delegiert werden.

```text
Authority(Agent) ⊆ Delegated Authority(Principal)
```

Delegation kann eingeschränkt werden nach:

```text
Operation
Resource
Purpose
Duration
Location
Provider
Resource Budget
Security Domain
```

Ein Agent darf seine delegierte Autorität nicht selbst erweitern.

## Trust Evidence

Agent Trust kann unter anderem berücksichtigen:

```text
Agent Identity
Software Trust
Signature Trust
Provenance Trust
Runtime Attestation
Provider Trust
Execution Environment
Principal Trust
Security History
```

Kein einzelner Nachweis erzeugt automatisch vollständigen Agent Trust.

## Autonomie

NovaOS muss unterschiedliche Autonomiestufen unterstützen können.

```text
Observe
Suggest
Prepare
Execute with Confirmation
Execute Autonomously
```

Trust und Capabilities bestimmen gemeinsam, welche Stufen zulässig sind.

Ein Agent kann beispielsweise Dateien analysieren dürfen, ohne sie verändern oder versenden zu dürfen.

## ExecutionContract

Agenten müssen in `Nova.ExecutionContract` integrierbar sein.

```text
Agent Request
     ↓
ExecutionContract
├── Operation
├── Security Context
├── Resource Budget
├── Deadline
├── Sovereignty
├── Trust Requirement
└── Execution Location
```

Damit gelten für Agenten dieselben systemweiten Ausführungsregeln wie für andere NovaOS-Komponenten.

## Externe Agenten

Cloud- oder Remote-Agenten müssen lokal bewertet werden.

```text
Remote Agent
    ↓
Identity + Attestation + Provider
    ↓
Local Trust Policy
    ↓
Restricted Capabilities
```

Remote-Verfügbarkeit oder Provider-Vertrauen erzeugt keine automatische lokale Autorität.

## Agent Chains

Agenten dürfen andere Agenten nur kontrolliert beauftragen.

```text
Principal
   ↓
Agent A
   ↓
Agent B
```

Dabei gilt:

```text
Authority(B) ⊆ Authority(A) ⊆ Authority(Principal)
```

Trust darf nicht automatisch transitiv werden.

## Sensitive Operations

Für kritische Aktionen können zusätzliche Anforderungen gelten:

```text
Agent
  ↓
Sensitive Operation
  ↓
Step-Up / User Confirmation / Additional Trust
  ↓
Execution
```

Dies kann insbesondere gelten für:

```text
Credential Changes
Key Operations
Software Installation
Data Export
Financial Actions
Security Configuration
Administrative Changes
```

## Ergebnisse

Agent-Ausgaben müssen von deren Vertrauensstatus getrennt behandelt werden.

```text
Trusted Agent
     ≠
Correct Output
```

Ergebnisse können zusätzliche Verifikation, Provenance oder Nutzerbestätigung benötigen.

## Revocation

Agent Trust und delegierte Autorität müssen widerrufbar sein.

```text
Agent Active
    ↓
Revocation
    ↓
Capabilities Revoked
    ↓
Running Operations Re-Evaluated
```

Langlaufende Agent-Aufgaben müssen auf Revocation reagieren können.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Agent Identity
Principal Identity
Trust State
Delegated Capabilities
Autonomy Level
Execution Location
Provider
Active Operations
Expiration
```

Der Nutzer soll erkennen können, welcher Agent in wessen Auftrag welche Autorität verwendet.

## Normative Anforderungen

1. Sicherheitsrelevante Agenten MÜSSEN eine eigene Identity besitzen.
2. Agent Trust MUSS von Agent Authority getrennt behandelt werden.
3. Agenten DÜRFEN nur explizit delegierte Autorität verwenden.
4. Delegierte Autorität DARF NICHT selbstständig erweitert werden.
5. Agent Trust MUSS kontextabhängig bewertbar sein.
6. Agent Chains MÜSSEN attenuierte Delegation verwenden.
7. Remote-Agenten MÜSSEN lokal bewertbar bleiben.
8. Kritische Agent-Aktionen MÜSSEN zusätzliche Trust- oder Bestätigungsanforderungen unterstützen.
9. Agent Trust und delegierte Capabilities MÜSSEN widerrufbar sein.
10. Agent-Aktivitäten SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-STEPUP-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-TRUST-0010`

## Ergebnis

```text
Principal
   ↓
Explicit Delegation
   ↓
Agent Identity + Trust
   ↓
ExecutionContract
   ↓
Restricted Capabilities
   ↓
Controlled Autonomous Action
```

NovaOS erhält damit eine Trust-Architektur für autonome Agenten, bei der Agenten eindeutig identifizierbar, überprüfbar, begrenzt delegiert und jederzeit widerrufbar handeln, ohne Autonomie mit uneingeschränkter Autorität gleichzusetzen.