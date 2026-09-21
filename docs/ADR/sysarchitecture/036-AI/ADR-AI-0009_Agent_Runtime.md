# ADR-AI-0009 – Agent Runtime

## Status

Angenommen

## Kategorie

AI / Agents / Runtime / Autonomy

## Kontext

NovaOS soll AI-Agenten unterstützen, die komplexe Aufgaben nicht nur beantworten, sondern über mehrere Schritte hinweg planen und ausführen können.

Beispiele:

```text
User Intent
    ↓
Analyze
    ↓
Plan
    ↓
Use Capabilities
    ↓
Observe Result
    ↓
Continue / Replan
```

Ein Agent darf dabei jedoch weder uneingeschränkten Systemzugriff erhalten noch eine zweite, von NovaOS unabhängige Autonomiearchitektur bilden.

Agenten müssen dieselben:

```text
Capabilities
Permissions
Execution Contracts
Policies
Constraints
Privacy Rules
Trust Rules
```

verwenden wie andere NovaOS-Komponenten.

## Entscheidung

NovaOS erhält eine systemweite **Nova.Agent Runtime** auf Basis der `Nova.AI Runtime`.

```text
User / System Intent
        ↓
Nova.Agent Runtime
        ↓
Context
        ↓
Planning
        ↓
Capability Selection
        ↓
Policy + Constraint Check
        ↓
Execution
        ↓
Observation
        ↓
Continue / Replan / Complete
```

Die Agent Runtime orchestriert bestehende NovaOS-Capabilities, statt privilegierte Sonderzugriffe auf das System bereitzustellen.

## Agent Identity

Jede Agent-Ausführung besitzt eine eindeutige Identität.

Mindestens:

```text
Agent ID
Agent Version
Execution ID
Security Context
Owner / Initiator
```

Agent Identity ist von Model Identity getrennt.

```text
Agent ≠ Model
```

Ein Agent kann unterschiedliche AI-Modelle verwenden.

## Agent Execution

Agenten arbeiten als begrenzte Executions.

```text
Intent
   ↓
Agent Execution
   ├── Context
   ├── Plan
   ├── Capabilities
   ├── Resources
   └── Constraints
```

Eine Agent-Ausführung besitzt einen definierten Start, Scope und Abschlusszustand.

## Planning

Agenten dürfen Aufgaben in mehrere Schritte zerlegen.

```text
Goal
 ↓
Step 1
 ↓
Step 2
 ↓
Step 3
 ↓
Goal Verification
```

Pläne sind keine Autorisierung.

Jeder ausführbare Schritt muss weiterhin zulässig sein.

```text
Planned Action
      ≠
Authorized Action
```

## Capability-Based Actions

Agenten interagieren mit NovaOS über Capabilities.

Beispiele:

```text
File.Read
Document.Convert
Image.Analyze
Search
Simulation.Run
Notification.Create
```

Direkter unkontrollierter Zugriff auf Kernel-, Geräte- oder Anwendungsinternas wird nicht vorgesehen.

## Least Privilege

Ein Agent erhält nur die Capabilities, die für seine Aufgabe erforderlich sind.

```text
Agent Goal
    ↓
Required Capabilities
    ↓
Authorized Capability Set
```

Ein Sprachmodell erhält nicht automatisch dieselben Rechte wie der Nutzer.

## Delegation

Nutzer, Anwendungen oder Systemdienste können einem Agenten begrenzte Rechte delegieren.

```text
User Authority
      ↓
Attenuated Delegation
      ↓
Agent Capability
```

Delegierte Rechte dürfen nicht erweitert werden.

## Context

Agenten beziehen Kontext über `Context Services`.

```text
Agent
  ↓
Context Requirements
  ↓
Context Services
  ↓
Authorized Context View
```

Ein Agent darf nicht selbstständig das gesamte System nach möglicherweise relevanten Daten durchsuchen.

## Tool und Capability Discovery

Agenten dürfen verfügbare Capabilities semantisch entdecken.

```text
Goal
  ↓
Required Function
  ↓
Capability Discovery
  ↓
Compatible Capabilities
```

Discovery bedeutet nicht automatisch Authorization.

## Execution Contracts

Jede relevante Agent-Aktion kann einen `Nova.ExecutionContract` besitzen.

Beispiele:

```text
Deadline
Resource Budget
Data Sovereignty
Trust
Determinism
Locality
Forced Provider
User Constraints
```

Dadurch unterliegen Agent-Aktionen derselben Ausführungsarchitektur wie andere NovaOS-Operationen.

## Action Classification

Agent-Aktionen werden hinsichtlich ihrer Auswirkungen klassifiziert.

Beispielsweise:

```text
Read-Only
Reversible
Compensatable
Transactional
Externally Visible
Irreversible
Security Critical
```

Diese Eigenschaften beeinflussen den zulässigen Autonomy Level.

## Confirmation Boundaries

Bestimmte Aktionen können explizite Nutzerbestätigung verlangen.

Beispiele:

```text
Delete Persistent Data
Send External Message
Purchase
Change Security Policy
Expose Private Data
Irreversible Operation
```

Die Agent Runtime darf solche Confirmation Boundaries nicht selbstständig umgehen.

## Multi-Step Authorization

Eine einmal autorisierte Aufgabe bedeutet nicht automatisch unbegrenzte Autorisierung aller später geplanten Schritte.

```text
Goal Authorized
      ↓
Plan Generated
      ↓
Action A → Check
Action B → Check
Action C → Check
```

Relevante Aktionen werden gegen die aktuellen Rechte und Constraints geprüft.

## Dynamic Replanning

Agenten dürfen auf unerwartete Ergebnisse reagieren.

```text
Plan
 ↓
Action
 ↓
Unexpected Result
 ↓
Observe
 ↓
Replan
```

Ein neuer Plan erweitert jedoch nicht die vorhandenen Rechte.

## Agent State

Agent State kann enthalten:

```text
Current Goal
Current Plan
Completed Steps
Pending Steps
Execution Results
Context References
Decision State
```

Agent State ist von persistenter AI Memory getrennt.

```text
Agent State ≠ Persistent Memory
```

## Resource Budgets

Agent-Ausführungen erhalten begrenzbare Ressourcen.

Beispiele:

```text
CPU
Memory
AI Tokens
Model Runtime
Network
Storage
Execution Time
Number of Actions
```

Dadurch werden unbegrenzte Agent-Schleifen verhindert.

## Loop Protection

Die Runtime muss wiederholte oder nicht fortschreitende Agent-Schleifen erkennen können.

```text
Plan
 ↓
Action
 ↓
No Progress
 ↓
Replan
 ↓
No Progress
 ↓
Stop / Escalate
```

Mögliche Grenzen sind:

```text
Action Budget
Retry Budget
Time Budget
Replanning Budget
Resource Budget
```

## Failure Handling

Fehler einzelner Aktionen führen nicht automatisch zum vollständigen Scheitern.

```text
Action Failed
     ↓
Diagnose
     ↓
Retry / Alternative / Replan / Abort
```

Retries müssen weiterhin die Semantik der jeweiligen Operation berücksichtigen.

## Agent und Self-Healing

Agenten können Self-Healing-Funktionen verwenden, erhalten dadurch jedoch keine erweiterten Recovery-Rechte.

```text
Agent Failure
    ↓
Self-Diagnosis
    ↓
Allowed Recovery
```

Systemweites Self-Healing bleibt Aufgabe der bestehenden Autonomy- und Resilience-Architektur.

## Multi-Agent Execution

Mehrere Agenten dürfen gleichzeitig aktiv sein.

```text
Agent A ─┐
Agent B ─┼→ NovaOS Capabilities
Agent C ─┘
```

Sie bleiben hinsichtlich:

```text
Identity
Permissions
Context
State
Resources
Execution
```

voneinander isoliert.

Direkte Agent-zu-Agent-Kommunikation erfolgt über explizite IPC- oder Capability-Schnittstellen.

## AI Model Independence

Die Agent Runtime ist nicht an ein bestimmtes Sprachmodell gebunden.

```text
Agent Runtime
      ↓
Nova.AI Capability
      ↓
Model Selection
```

Modelle können abhängig von Aufgabe, Contract und Ressourcen ausgetauscht werden.

## Privacy

Agenten unterliegen vollständig `Privacy-Preserving AI`.

Insbesondere gilt:

```text
Agent Goal
    ≠
Permission to access all user data
```

Context, Tool Inputs, Outputs und persistente Daten müssen einzeln den jeweiligen Privacy- und Sovereignty-Regeln entsprechen.

## Security

Agent-generierte Inhalte werden als nicht vertrauenswürdige Eingaben behandelt, wenn daraus Aktionen entstehen.

```text
Model Output
     ↓
Parse / Validate
     ↓
Policy + Authorization
     ↓
Capability Invocation
```

Textausgabe eines Modells darf niemals direkt eine privilegierte Systemoperation autorisieren.

## Deterministic Mode

Agent-Ausführungen können nur dann deterministisch sein, wenn relevante:

```text
Model Version
Context
Capability Versions
Policies
Planning Rules
External Inputs
Execution Results
```

reproduzierbar sind.

Der Execution Contract muss festlegen können, ob Determinismus erforderlich ist.

## Observability

Agent-Ausführungen müssen über mehrere Schritte hinweg nachvollziehbar sein.

Relevante Informationen sind:

```text
Agent ID
Execution ID
Goal
Plan Version
Action ID
Capability
Decision ID
Authorization Result
Execution Result
Replanning
Final Status
```

Sensitive Context- oder Prompt-Inhalte dürfen nicht standardmäßig vollständig geloggt werden.

## Normative Anforderungen

1. NovaOS SOLL eine systemweite Agent Runtime bereitstellen.
2. Agenten MÜSSEN bestehende NovaOS-Capabilities für Systemaktionen verwenden.
3. Agenten DÜRFEN keinen privilegierten Sonderzugriff auf Systemressourcen erhalten.
4. Agent Identity MUSS von Model Identity getrennt sein.
5. Agent-Ausführungen MÜSSEN einen definierten Scope besitzen.
6. Agent-Pläne DÜRFEN nicht als Authorization behandelt werden.
7. Ausführbare Agent-Aktionen MÜSSEN gegen aktuelle Policies, Permissions und Constraints geprüft werden.
8. Agenten MÜSSEN dem Least-Privilege-Prinzip folgen.
9. Delegierte Capabilities DÜRFEN durch Agenten nicht erweitert werden.
10. Agent Context MUSS über autorisierte Context Views bereitstellbar sein.
11. Capability Discovery DARF keine Authorization erzeugen.
12. Agent-Aktionen MÜSSEN Execution Contracts verwenden können.
13. Irreversible und sicherheitskritische Aktionen MÜSSEN durch Confirmation Boundaries begrenzbar sein.
14. Dynamic Replanning DARF Rechte oder Hard Constraints nicht erweitern.
15. Agent State MUSS von persistenter AI Memory getrennt sein.
16. Agent-Ausführungen MÜSSEN durch Resource- und Action-Budgets begrenzbar sein.
17. Nicht fortschreitende Agent-Loops MÜSSEN begrenzbar sein.
18. Retries MÜSSEN die Semantik der betroffenen Operation berücksichtigen.
19. Mehrere Agenten MÜSSEN hinsichtlich Identity, Context, Permissions und State isolierbar sein.
20. Die Agent Runtime DARF nicht an ein konkretes AI-Modell gebunden sein.
21. Model Output DARF keine privilegierte Operation direkt autorisieren.
22. Agenten MÜSSEN Privacy-, Sovereignty- und Trust-Regeln vollständig einhalten.
23. Agent-Ausführungen MÜSSEN mit Decision Tracing und Distributed Tracing korrelierbar sein.
24. NovaOS MUSS ohne Agent Runtime vollständig grundlegend funktionsfähig bleiben.

## Konsequenzen

### Positive Konsequenzen

- komplexe Aufgaben können systemweit über mehrere Capabilities orchestriert werden,
- Agenten verwenden dieselben Security- und Execution-Regeln wie andere NovaOS-Komponenten,
- AI-Modelle erhalten keinen direkten privilegierten Systemzugriff,
- Agenten können Modelle und Provider austauschen,
- Multi-Agent-Szenarien bleiben isoliert und kontrollierbar,
- Endlosschleifen und unkontrollierter Ressourcenverbrauch können begrenzt werden.

### Negative Konsequenzen

- Multi-Step Authorization erhöht den Runtime-Aufwand,
- Agent State und Replanning erhöhen die Ausführungskomplexität,
- Capability-Schnittstellen müssen ausreichend semantisch beschreibbar sein.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0002_Model_Registry`
- `ADR-AI-0006_Privacy_Preserving_AI`
- `ADR-AI-0008_Context_Services`
- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0020_KI_nicht_Kernel_Voraussetzung`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-RESILIENCE-0001`

## Zugehörige NPSPECs

- `NPSPEC-AI-AGENT-RUNTIME-0001`
- `NPSPEC-AI-AGENT-IDENTITY-0001`
- `NPSPEC-AI-AGENT-PLANNING-0001`
- `NPSPEC-AI-AGENT-AUTHORIZATION-0001`
- `NPSPEC-AI-AGENT-STATE-0001`
- `NPSPEC-AI-AGENT-BUDGET-0001`
- `NPSPEC-AI-MULTI-AGENT-0001`

## Ergebnis

NovaOS behandelt AI-Agenten als kontrollierte Orchestratoren bestehender Systemfähigkeiten:

```text
Intent
   ↓
Agent Runtime
   ↓
Plan
   ↓
Authorized Context
   ↓
Capability Selection
   ↓
Policy + Constraint Check
   ↓
Execute
   ↓
Observe
   ↓
Replan / Complete
```

Der zentrale Grundsatz lautet:

```text
Ein Agent darf planen,
entscheiden und orchestrieren.

Aber ein Plan erzeugt keine Rechte.

Jede reale Systemaktion läuft
weiterhin durch die normalen
Capabilities, Permissions,
Policies und Constraints
von NovaOS.

Der Agent arbeitet im System –
nicht über dem System.
```