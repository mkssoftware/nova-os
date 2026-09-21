# ADR-AI-0010 – Agent Sandboxing

## Status

Angenommen

## Kategorie

AI / Agents / Security / Isolation

## Kontext

AI-Agenten können selbstständig planen und mehrere NovaOS-Capabilities miteinander kombinieren.

Dadurch können sie Aktionen ausführen wie:

```text
Read Objects
Create Objects
Modify Documents
Execute Capabilities
Use Network Resources
Invoke AI Models
Start Subtasks
Interact with Services
```

Auch wenn jede einzelne Capability autorisiert ist, kann die Kombination mehrerer Aktionen unerwartete Auswirkungen erzeugen.

Agenten benötigen deshalb eine isolierte Ausführungsumgebung, die ihre Rechte, Ressourcen, Datenzugriffe und Kommunikationswege technisch begrenzt.

## Entscheidung

Jede Agent-Ausführung läuft innerhalb einer **Agent Sandbox**.

```text
Agent
  ↓
Agent Sandbox
  ├── Capability Set
  ├── Resource Budget
  ├── Context View
  ├── Network Policy
  ├── Storage View
  └── Execution Constraints
        ↓
NovaOS Capabilities
```

Die Sandbox erzwingt die Grenzen unabhängig vom verwendeten AI-Modell.

## Sandbox Identity

Jede Sandbox besitzt eine eindeutige Identität.

```text
Sandbox ID
Agent ID
Execution ID
Security Context
Owner / Initiator
```

Mehrere Ausführungen desselben Agenten können unterschiedliche Sandboxen besitzen.

## Capability Boundary

Die Sandbox enthält nur die für die Aufgabe freigegebenen Capabilities.

```text
Available System Capabilities
          ↓
Authorization
          ↓
Delegation / Attenuation
          ↓
Agent Capability Set
```

Nicht delegierte Capabilities sind für den Agenten nicht verfügbar.

## Kein direkter Systemzugriff

Agenten erhalten keinen direkten Zugriff auf:

```text
Kernel Interfaces
Raw Devices
Physical Memory
Arbitrary Processes
Security Databases
Credential Stores
Unrestricted Filesystem
```

Systemaktionen erfolgen über kontrollierte NovaOS-Capabilities.

## Storage Isolation

Die Sandbox erhält eine explizite Sicht auf erlaubte Objects und Storage-Bereiche.

```text
System Objects
     ↓
Permission Filter
     ↓
Sandbox Object View
```

Ein Agent darf nicht allein aufgrund eines bekannten Pfades oder einer Object ID auf ein Objekt zugreifen.

## Context Isolation

Context Services liefern ausschließlich den für die Agent-Ausführung autorisierten Context View.

```text
Global Context
      ↓
Context Services
      ↓
Authorization + Privacy
      ↓
Sandbox Context
```

Andere Agenten, Anwendungen oder Nutzerkontexte bleiben isoliert.

## Network Isolation

Netzwerkzugriff wird explizit kontrolliert.

Mögliche Policies sind:

```text
No Network
Local Network Only
Specific Endpoints
Trusted Services Only
Internet Access
```

Ein Agent darf Netzwerkzugriff nicht durch andere Capabilities umgehen.

## Resource Isolation

Jede Sandbox besitzt begrenzbare Ressourcen.

Beispiele:

```text
CPU
Memory
Storage
Network
GPU / NPU
AI Tokens
Execution Time
Process Count
Action Count
```

Ressourcenverbrauch wird der jeweiligen Agent-Ausführung zugerechnet.

## Subprocesses und Subagents

Von einem Agenten erzeugte Subprozesse oder Subagents dürfen keine zusätzlichen Rechte erhalten.

```text
Parent Sandbox
      ↓
Child Execution
      ↓
Same or Reduced Authority
```

Es gilt:

```text
Child Authority
    ≤
Parent Authority
```

## Temporary Workspace

Agenten dürfen einen temporären Arbeitsbereich erhalten.

```text
Agent Sandbox
     ↓
Temporary Workspace
     ↓
Intermediate Objects
```

Temporäre Daten unterliegen definierten Lifecycle- und Privacy-Regeln.

Sie werden nicht automatisch persistent.

## External Effects

Aktionen mit extern sichtbaren Auswirkungen werden besonders behandelt.

Beispiele:

```text
Send Message
Publish Data
Modify Remote System
Delete Persistent Object
Trigger Physical Device
```

Die Sandbox kann solche Aktionen:

```text
Allow
Deny
Require Confirmation
Restrict
```

## Irreversible Actions

Irreversible Aktionen benötigen eine explizite Policy.

```text
Agent Action
     ↓
Irreversible?
 ├── No  → Normal Authorization
 └── Yes → Additional Constraint / Confirmation
```

Eine Sandbox darf diese Grenze nicht selbst verändern.

## Model Output Isolation

Model Output wird als nicht vertrauenswürdiger Steuerinput behandelt.

```text
Model Output
     ↓
Agent Runtime
     ↓
Parse + Validate
     ↓
Sandbox Boundary
     ↓
Capability Invocation
```

Generierter Text oder Tool-Aufruf erhält dadurch keine impliziten Rechte.

## Prompt Injection

Externe Inhalte dürfen die Sandbox-Berechtigungen nicht verändern.

Beispiel:

```text
Web Content
Document
Message
Tool Output
    ↓
Untrusted Input
    ↓
Agent Reasoning
```

Eine darin enthaltene Anweisung kann keine:

```text
Capability
Permission
Credential
Policy
Sandbox Boundary
```

erzeugen oder erweitern.

## Secrets

Credentials und Secrets werden Agenten nicht unnötig direkt bereitgestellt.

Stattdessen sollen autorisierte Operationen über Broker oder Capabilities erfolgen.

```text
Agent
  ↓
Authorized Capability
  ↓
Credential Broker
  ↓
External Service
```

Der Agent benötigt dadurch nicht zwingend Zugriff auf das eigentliche Secret.

## Dynamic Capability Grant

Während einer Ausführung können zusätzliche Rechte erforderlich werden.

```text
Agent
  ↓
Additional Capability Required
  ↓
Authorization / Confirmation
  ↓
Sandbox Update
```

Die Erweiterung erfolgt ausschließlich durch eine autorisierte externe Entscheidung.

Der Agent darf seine Sandbox nicht selbst erweitern.

## Revocation

Capabilities müssen während einer laufenden Agent-Ausführung widerrufbar sein.

```text
Capability Active
      ↓
Revocation
      ↓
Sandbox Update
      ↓
Future Access Denied
```

Laufende Operationen folgen der jeweiligen Revocation- und Transaktionssemantik.

## Multi-Agent Isolation

Jeder Agent besitzt grundsätzlich seine eigene Sandbox.

```text
Agent A → Sandbox A

Agent B → Sandbox B
```

Kommunikation erfolgt ausschließlich über explizite:

```text
IPC
Capabilities
Shared Objects
Message Channels
```

mit entsprechender Authorization.

## Sandbox Failure

Ein Fehler innerhalb einer Agent Sandbox soll möglichst auf diese Sandbox begrenzt bleiben.

```text
Agent Failure
     ↓
Terminate Sandbox
     ↓
Release Resources
     ↓
Preserve Required Evidence
```

Andere Agenten und Systemdienste sollen davon nicht direkt betroffen sein.

## Sandbox Lifecycle

Der Lebenszyklus lautet grundsätzlich:

```text
Create
  ↓
Configure
  ↓
Authorize
  ↓
Execute
  ↓
Restrict / Update
  ↓
Terminate
  ↓
Cleanup
```

Persistente Ergebnisse werden nur entsprechend der jeweiligen Capability- und Storage-Semantik übernommen.

## Self-Protection

Erkennt NovaOS verdächtiges Agent-Verhalten, darf Self-Protection die Sandbox einschränken.

```text
Security Signal
      ↓
Restrict
      ↓
Suspend
      ↓
Quarantine
      ↓
Terminate
```

Ein Agent kann diese Schutzmaßnahmen nicht selbst aufheben.

## Deterministic Mode

Für reproduzierbare Agent-Ausführungen müssen relevante Sandbox-Eigenschaften fixierbar sein:

```text
Capability Set
Resource Limits
Context View
Network Policy
Storage View
Policy Version
```

Änderungen müssen nachvollziehbar bleiben.

## Observability

Sandbox-Aktivitäten müssen mit Agent-, Security- und Decision-Tracing korrelierbar sein.

Relevante Informationen sind:

```text
Sandbox ID
Agent ID
Execution ID
Capability Grants
Capability Revocations
Resource Usage
Denied Operations
Network Access
External Effects
Policy Decisions
Termination Reason
```

Sensitive Inhalte und Secrets dürfen dabei nicht unnötig protokolliert werden.

## Normative Anforderungen

1. Jede Agent-Ausführung MUSS innerhalb einer kontrollierten Sandbox ausführbar sein.
2. Sandbox-Grenzen MÜSSEN unabhängig vom verwendeten AI-Modell durchgesetzt werden.
3. Agenten DÜRFEN ausschließlich explizit bereitgestellte Capabilities verwenden.
4. Agenten DÜRFEN keinen uneingeschränkten direkten Systemzugriff erhalten.
5. Storage Access MUSS auf autorisierte Objects und Bereiche begrenzbar sein.
6. Context Views MÜSSEN zwischen Agent-Ausführungen isolierbar sein.
7. Netzwerkzugriff MUSS pro Sandbox begrenzbar sein.
8. Ressourcen MÜSSEN pro Agent-Ausführung budgetierbar sein.
9. Child Executions und Subagents DÜRFEN keine größere Authority als ihre Eltern erhalten.
10. Temporäre Agent-Daten MÜSSEN einem definierten Lifecycle unterliegen.
11. Externe und irreversible Aktionen MÜSSEN separat begrenzbar sein.
12. Model Output DARF keine Authorization erzeugen.
13. Externe Inhalte DÜRFEN Sandbox-Grenzen nicht verändern.
14. Secrets SOLLEN bevorzugt über autorisierte Broker oder Capabilities verwendet werden.
15. Ein Agent DARF seine eigene Sandbox nicht selbst erweitern.
16. Zusätzliche Capability Grants MÜSSEN durch eine autorisierte externe Entscheidung erfolgen.
17. Capabilities MÜSSEN während einer Agent-Ausführung widerrufbar sein.
18. Mehrere Agenten MÜSSEN voneinander isolierbar sein.
19. Sandbox Failures SOLLEN auf den betroffenen Agenten begrenzt bleiben.
20. Self-Protection MUSS Agent-Sandboxen einschränken, suspendieren oder terminieren können.
21. Deterministic Mode MUSS relevante Sandbox-Eigenschaften fixieren können.
22. Sandbox-Aktivitäten MÜSSEN mit Observability und Decision Tracing korrelierbar sein.
23. Das Beenden einer Sandbox MUSS deren temporäre Ressourcen kontrolliert freigeben können.

## Konsequenzen

### Positive Konsequenzen

- AI-Agenten erhalten keinen uneingeschränkten Systemzugriff,
- Fehler und kompromittierte Agenten bleiben stärker isoliert,
- Prompt Injection kann keine zusätzlichen Systemrechte erzeugen,
- Ressourcenverbrauch und Netzwerkzugriff werden kontrollierbar,
- Capabilities können dynamisch delegiert und widerrufen werden,
- Multi-Agent-Systeme bleiben voneinander getrennt.

### Negative Konsequenzen

- Capability-, Context- und Storage-Vermittlung erzeugt zusätzlichen Runtime-Aufwand,
- komplexe Agent-Aufgaben können zusätzliche Authorization-Schritte benötigen,
- Sandbox-Lifecycle und Resource Accounting erhöhen die Systemkomplexität.

## Abhängigkeiten

- `ADR-AI-0006_Privacy_Preserving_AI`
- `ADR-AI-0008_Context_Services`
- `ADR-AI-0009_Agent_Runtime`
- `ADR-AUTONOMY-0007_Self_Protection`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-CAPABILITY-0010_Capability_Sandboxing`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-SECURITY-0003_Least_Privilege`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AI-AGENT-SANDBOX-0001`
- `NPSPEC-AI-AGENT-CAPABILITY-BOUNDARY-0001`
- `NPSPEC-AI-AGENT-RESOURCE-ISOLATION-0001`
- `NPSPEC-AI-AGENT-NETWORK-ISOLATION-0001`
- `NPSPEC-AI-AGENT-SECRET-BROKER-0001`
- `NPSPEC-AI-AGENT-SANDBOX-LIFECYCLE-0001`

## Ergebnis

Jede Agent-Ausführung erhält einen technisch begrenzten Handlungsraum:

```text
Agent
  ↓
Sandbox
  ├── Allowed Capabilities
  ├── Authorized Context
  ├── Storage View
  ├── Network Policy
  └── Resource Budget
        ↓
NovaOS
```

Der zentrale Grundsatz lautet:

```text
Ein Agent darf nur das sehen,
nutzen und verändern,
was seine Sandbox ausdrücklich erlaubt.

Das AI-Modell entscheidet,
was es tun möchte.

Die Sandbox entscheidet,
was es tatsächlich tun kann.

Kein Prompt,
kein Modell und kein Agent
darf seine eigenen Grenzen erweitern.
```