# ADR-AI-0011 – Agent Permissions über Capabilities

## Status

Angenommen

## Kategorie

AI / Agents / Security / Capabilities

## Kontext

AI-Agenten können Aufgaben selbstständig planen und dabei verschiedene NovaOS-Funktionen verwenden.

Ein Agent benötigt dafür Rechte auf konkrete Systemfunktionen wie:

```text
Read Object
Modify Document
Search Data
Use Network
Send Message
Run Simulation
Create File
Invoke Service
```

Klassische pauschale Rollen oder vollständige Nutzerrechte wären für Agenten zu weitreichend.

Ein Agent soll deshalb nicht die Identität und vollständigen Berechtigungen des Nutzers übernehmen.

## Entscheidung

Agent Permissions werden ausschließlich über **NovaOS Capabilities** bereitgestellt.

```text
User / Service
      ↓
Delegation
      ↓
Agent Sandbox
      ↓
Capability Set
      ↓
NovaOS Resources
```

Der Agent besitzt nur die ihm explizit delegierten Capabilities.

Es gilt:

```text
Agent Authority
    =
Delegated Capabilities
```

## Keine impliziten Nutzerrechte

Ein Agent übernimmt nicht automatisch die vollständigen Rechte seines Auftraggebers.

```text
User Permissions
      ≠
Agent Permissions
```

Der Nutzer kann beispielsweise ein Dokument lesen dürfen, ohne dass jeder von ihm gestartete Agent automatisch Zugriff darauf erhält.

## Capability Set

Jede Agent-Ausführung besitzt ein explizites Capability Set.

Beispiel:

```text
Agent Capability Set
 ├── Document.Read(Object A)
 ├── Document.Write(Object B)
 ├── AI.Text.Generate
 └── Network.Access(Service X)
```

Alles außerhalb dieses Sets ist nicht verfügbar.

## Capability Delegation

Capabilities können von einem autorisierten Principal an einen Agenten delegiert werden.

```text
User Capability
      ↓
Delegation
      ↓
Agent Capability
```

Delegation darf keine neuen Rechte erzeugen.

```text
Delegated Authority
    ≤
Original Authority
```

## Capability Attenuation

Delegierte Rechte sollen möglichst eingeschränkt werden.

Beispiel:

```text
User:
Document.Read(All Documents)

Agent:
Document.Read(Object 42)
```

oder:

```text
User:
Network.Access(Internet)

Agent:
Network.Access(api.example)
```

Agenten erhalten damit die kleinste für ihre Aufgabe notwendige Authority.

## Object-Bound Permissions

Capabilities können an konkrete NovaOS Objects gebunden sein.

```text
Capability
   ↓
Object ID
   ↓
Allowed Operation
```

Beispiel:

```text
Document.Read(ObjectID: 81A...)
```

Ein bekannter Dateipfad oder eine bekannte Object ID erzeugt keine Berechtigung.

## Operation-Bound Permissions

Capabilities unterscheiden konkrete Operationen.

```text
Read
Write
Create
Delete
Execute
Share
Export
Transform
```

Beispielsweise:

```text
Document.Read
```

impliziert nicht:

```text
Document.Write
Document.Delete
Document.Share
```

## Scope

Agent Capabilities können einen expliziten Scope besitzen.

Beispiele:

```text
Object
Workspace
Application
Service
Endpoint
Session
Execution
```

Ein Agent erhält dadurch keine unnötig globale Authority.

## Time-Bounded Permissions

Agent Capabilities können zeitlich begrenzt sein.

```text
Capability
    ↓
Valid Until
    ↓
Automatic Expiration
```

Kurzlebige Agent-Ausführungen sollen bevorzugt kurzlebige Capabilities erhalten.

## Execution-Bound Permissions

Capabilities können an eine einzelne Agent Execution gebunden werden.

```text
Capability
    ↓
Execution ID
```

Nach Ende der Ausführung verliert die Capability ihre Gültigkeit.

## Purpose Binding

Wo sinnvoll, können Capabilities zusätzlich an einen Zweck gebunden werden.

Beispiel:

```text
Read Document X
for
Generate Summary
```

Die Capability darf dann nicht automatisch für eine unabhängige spätere Aufgabe wiederverwendet werden.

## Dynamic Capability Requests

Benötigt ein Agent während der Ausführung zusätzliche Rechte:

```text
Agent
  ↓
Capability Required
  ↓
Request
  ↓
Authorization / User Confirmation
  ↓
Grant or Deny
```

Der Agent darf die Capability nicht selbst erzeugen.

## Confirmation

Bestimmte Capabilities können eine explizite Bestätigung verlangen.

Beispiele:

```text
Delete.Persistent
Message.Send.External
Data.Export
Security.Change
Purchase.Execute
Device.Control
```

Die Bestätigung gilt nur für den definierten Scope.

## Capability Revocation

Agent Capabilities müssen widerrufbar sein.

```text
Active Capability
      ↓
Revoke
      ↓
Future Invocation Denied
```

Ein widerrufenes Recht darf nicht durch gecachte Agent-Zustände weiterverwendet werden.

## Subagents

Agenten dürfen Capabilities nur dann an Subagents delegieren, wenn die Capability selbst Delegation erlaubt.

```text
Parent Agent
     ↓
Attenuated Delegation
     ↓
Subagent
```

Es gilt:

```text
Subagent Authority
    ≤
Parent Agent Authority
```

Eine Delegationskette darf niemals Rechte erweitern.

## Capability Discovery

Ein Agent darf herausfinden, welche Capabilities grundsätzlich existieren.

```text
Capability Discovery
      ≠
Capability Permission
```

Das Wissen über eine Capability erzeugt kein Recht zu ihrer Verwendung.

## Capability Invocation

Jede relevante Invocation wird gegen die aktuelle Capability geprüft.

```text
Agent Action
     ↓
Capability Token / Reference
     ↓
Validation
     ↓
Scope Check
     ↓
Constraint Check
     ↓
Execute / Deny
```

Die Prüfung erfolgt außerhalb des AI-Modells.

## Credentials

Agenten sollen Credentials nicht direkt erhalten, wenn eine Capability die Operation stellvertretend durchführen kann.

```text
Agent
  ↓
Service Capability
  ↓
Credential Broker
  ↓
External Service
```

Damit besitzt der Agent die Fähigkeit zur erlaubten Operation, aber nicht zwingend das zugrunde liegende Secret.

## Model Independence

Permissions gehören zur Agent Execution und nicht zum verwendeten AI-Modell.

```text
Agent Permissions
      ≠
Model Permissions
```

Ein Modellwechsel verändert deshalb nicht automatisch die Authority des Agenten.

## Context Services

Context Services berücksichtigen das Capability Set des Agenten.

```text
Context Request
      ↓
Agent Capabilities
      ↓
Authorized Context View
```

Ein Agent darf keinen Kontext erhalten, auf dessen zugrunde liegende Daten er keine entsprechende Authority besitzt.

## Remote AI

Wird ein Remote AI Provider verwendet, werden Agent Capabilities nicht automatisch an diesen Provider übertragen.

```text
Agent Capability
      ≠
Remote Provider Capability
```

Der Remote Provider erhält ausschließlich die für die konkrete AI-Ausführung freigegebenen Daten.

## Fail-Safe Verhalten

Kann eine Capability nicht eindeutig validiert werden:

```text
Capability State = Unknown
        ↓
Deny
```

Es gilt:

```text
Unknown Authority
    ≠
Authorized
```

## Audit und Observability

Capability-Nutzung durch Agenten muss nachvollziehbar sein.

Relevante Informationen:

```text
Agent ID
Execution ID
Capability ID
Capability Type
Scope
Delegator
Grant
Attenuation
Invocation
Revocation
Authorization Result
Decision ID
```

Secrets und private Nutzdaten werden dabei nicht unnötig protokolliert.

## Normative Anforderungen

1. Agent Permissions MÜSSEN über NovaOS Capabilities dargestellt werden.
2. Agenten DÜRFEN nicht automatisch die vollständigen Rechte ihres Auftraggebers übernehmen.
3. Jede Agent-Ausführung MUSS ein explizites Capability Set besitzen können.
4. Delegierte Authority DARF die Authority des Delegators nicht überschreiten.
5. Agent Capabilities SOLLEN nach dem Least-Privilege-Prinzip attenuiert werden.
6. Capabilities MÜSSEN an konkrete Operations und Scopes bindbar sein.
7. Capabilities SOLLEN an konkrete Objects bindbar sein.
8. Agent Capabilities MÜSSEN zeitlich oder an eine Execution begrenzbar sein.
9. Purpose Binding SOLL für geeignete sensitive Operationen unterstützt werden.
10. Agenten DÜRFEN zusätzliche Capabilities nur anfordern, nicht selbst erzeugen.
11. Sensitive Capability Grants MÜSSEN eine Nutzer- oder Policy-Bestätigung verlangen können.
12. Agent Capabilities MÜSSEN widerrufbar sein.
13. Widerrufene Capabilities DÜRFEN nicht über gecachte Agent-Zustände weiterverwendet werden.
14. Subagents DÜRFEN höchstens gleiche oder geringere Authority erhalten.
15. Capability Discovery DARF keine Authorization erzeugen.
16. Capability Validation MUSS außerhalb des AI-Modells erfolgen.
17. Credentials SOLLEN gegenüber Agenten verborgen bleiben, wenn Brokered Capabilities ausreichen.
18. Ein Modelwechsel DARF Agent Permissions nicht verändern.
19. Context Services MÜSSEN Agent Capabilities berücksichtigen.
20. Remote AI Provider DÜRFEN Agent Capabilities nicht automatisch übernehmen.
21. Nicht eindeutig validierbare Authority MUSS fail-safe abgelehnt werden.
22. Capability Grants, Nutzung und Revocation MÜSSEN auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Agenten erhalten exakt begrenzbare Rechte,
- vollständige Nutzerrechte müssen nicht an AI-Agenten übertragen werden,
- Capabilities können nach Object, Operation, Scope und Zeit eingeschränkt werden,
- Subagents können sicher mit reduzierten Rechten arbeiten,
- Credentials müssen Agenten häufig nicht offengelegt werden,
- Rechte bleiben unabhängig vom verwendeten AI-Modell.

### Negative Konsequenzen

- Agenten benötigen häufiger explizite Capability Grants,
- Capability Delegation und Attenuation erhöhen den Verwaltungsaufwand,
- komplexe Aufgaben können mehrere fein abgestufte Berechtigungen benötigen.

## Abhängigkeiten

- `ADR-AI-0008_Context_Services`
- `ADR-AI-0009_Agent_Runtime`
- `ADR-AI-0010_Agent_Sandboxing`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-CAPABILITY-0010_Capability_Sandboxing`
- `ADR-SECURITY-0003_Least_Privilege`
- `ADR-IDENTITY-0006_Delegated_Identity_Context`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AI-AGENT-PERMISSIONS-0001`
- `NPSPEC-AI-AGENT-CAPABILITY-SET-0001`
- `NPSPEC-AI-AGENT-CAPABILITY-DELEGATION-0001`
- `NPSPEC-AI-AGENT-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-AI-AGENT-CAPABILITY-REVOCATION-0001`
- `NPSPEC-AI-AGENT-CAPABILITY-REQUEST-0001`

## Ergebnis

Agenten erhalten keine abstrakten oder pauschalen Systemrechte:

```text
User Authority
      ↓
Delegation
      ↓
Attenuation
      ↓
Agent Capability Set
      ↓
Sandbox
      ↓
Authorized Operations
```

Der zentrale Grundsatz lautet:

```text
Ein Agent bekommt nicht
die Rechte des Nutzers.

Er bekommt einzelne,
explizit delegierte Fähigkeiten.

Er darf nur das tun,
wofür er eine gültige
Capability besitzt.

Was nicht delegiert wurde,
existiert aus Sicht des Agenten
als ausführbare Berechtigung nicht.
```