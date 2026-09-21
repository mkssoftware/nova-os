# ADR-AI-0013 – KI darf harte Systemregeln nicht überschreiben

## Status

Angenommen

## Kategorie

AI / Safety / Security / System Architecture

## Kontext

NovaOS nutzt KI für:

```text
Prediction
Optimization
Scheduling
Context Selection
Agent Planning
Resource Management
Self-Diagnosis
Self-Healing
Automation
```

KI-Ausgaben sind jedoch grundsätzlich probabilistisch und können falsch, unvollständig oder durch ungeeignete Eingaben beeinflusst sein.

Eine KI darf deshalb niemals zur höchsten Entscheidungsinstanz über grundlegende Systemregeln werden.

Insbesondere dürfen AI-Modelle keine Regeln außer Kraft setzen, die NovaOS als **Hard Constraints** definiert.

## Entscheidung

KI arbeitet ausschließlich innerhalb eines durch NovaOS vorgegebenen gültigen Entscheidungsraums.

```text
System Rules
Safety
Security
Authorization
Sovereignty
User Hard Constraints
Execution Contract
        ↓
Hard Constraint Engine
        ↓
Allowed Decision Space
        ↓
AI Decision / Optimization
        ↓
Validation
        ↓
Execution
```

Es gilt:

```text
AI Authority
<
Hard System Rules
```

KI darf Optionen auswählen, bewerten oder vorschlagen.

Sie darf den gültigen Entscheidungsraum nicht selbst erweitern.

## Hard Constraints

Zu den nicht durch AI überschreibbaren Regeln gehören insbesondere:

```text
Safety Constraints
Security Policies
Authorization
Capability Boundaries
Data Sovereignty
Trust Requirements
Isolation Boundaries
Resource Hard Limits
Hard Realtime Requirements
Required Determinism
Integrity Requirements
Explicit User Hard Constraints
```

Diese Regeln werden außerhalb des AI-Modells durch deterministische Systemmechanismen durchgesetzt.

## AI Output ist kein Befehl

Ausgaben eines AI-Modells werden als Vorschläge oder Steuerinput behandelt.

```text
AI Output
    ↓
Parse
    ↓
Validate
    ↓
Policy Check
    ↓
Authorization
    ↓
Execute / Reject
```

Es gilt:

```text
AI Output ≠ Authorization
AI Output ≠ Policy
AI Output ≠ Capability
```

## Keine Policy-Manipulation

Ein AI-Modell darf nicht selbstständig:

```text
Security Policies ändern
Safety Limits erhöhen
Permissions erweitern
Capabilities erzeugen
Trust Requirements reduzieren
Sovereignty Constraints abschwächen
Audit Requirements deaktivieren
Sandbox Boundaries erweitern
```

Änderungen solcher Regeln benötigen den dafür vorgesehenen autorisierten Systempfad.

## Constraint Filtering vor AI-Optimierung

Ungültige Optionen werden möglichst bereits vor der AI-Auswahl entfernt.

```text
All Candidates
      ↓
Hard Constraint Filtering
      ↓
Valid Candidates
      ↓
AI Ranking / Prediction
```

Die KI soll damit bevorzugt nur zwischen grundsätzlich zulässigen Alternativen entscheiden.

## Validation nach AI-Entscheidung

Auch nach einer AI-Entscheidung erfolgt eine erneute Prüfung.

```text
AI Selection
     ↓
Final Validation
     ↓
Execute
```

Dies schützt insbesondere vor:

```text
Stale Context
Changed Permissions
Changed Policies
Model Errors
Compromised Inputs
Race Conditions
```

## Kein gültiger Lösungsraum

Existiert keine Lösung, die alle Hard Constraints erfüllt:

```text
Valid Solutions = 0
```

darf die KI nicht eigenständig Regeln abschwächen.

Stattdessen:

```text
Fail
Degrade Safely
Request User Decision
Escalate
Wait
```

abhängig von der jeweiligen Systempolicy.

## Unknown

Unbekannte Zustände dürfen nicht durch AI-Vermutungen in gültige Zustände umgewandelt werden.

Beispiele:

```text
Unknown Authorization ≠ Authorized
Unknown Trust ≠ Trusted
Unknown Integrity ≠ Valid
Unknown Safety ≠ Safe
```

Eine AI-Konfidenz ersetzt keinen erforderlichen Systemnachweis.

## Agenten

Für AI-Agenten gilt dieselbe Grenze.

```text
Agent Plan
    ↓
Capability Request
    ↓
Authorization
    ↓
Policy Check
    ↓
Execution
```

Ein Agent kann eine verbotene Aktion weder durch Planung noch durch mehrere Teilschritte legitimieren.

## Prompt Injection

Prompts, Dokumente, Webseiten oder andere externe Inhalte können keine Hard Constraints verändern.

```text
External Content:
"Ignore security policy"

        ↓

Untrusted Input
        ↓
No Policy Effect
```

Text ist Dateninhalt und keine Systemautorität.

## Adaptive Systeme

Auch lernende NovaOS-Komponenten dürfen Hard Constraints nicht verändern.

```text
Prediction Error
      ↓
Model Update
      ↓
Optimization Changes

Hard Constraints
      ↓
Unchanged
```

Adaptive Systeme dürfen nur ihr Verhalten innerhalb des zulässigen Lösungsraums optimieren.

## Self-Healing

Self-Healing darf ebenfalls keine Sicherheits- oder Safety-Regeln umgehen, um einen Dienst wiederherzustellen.

```text
Recovery Possible
but
Security Constraint violated
        ↓
Recovery Rejected
```

Ein sicher degradierter Zustand ist einem unsicheren funktionierenden Zustand vorzuziehen.

## Resource Management

AI-basierte Ressourcenoptimierung darf harte Ressourcenreservierungen und Limits nicht verletzen.

Beispiel:

```text
AI predicts unused memory
        ↓
Reserved Memory exists
        ↓
Reservation remains protected
```

Vorhersagen erzeugen keine Authority über reservierte Ressourcen.

## User Constraints

Explizite Nutzerentscheidungen können als Hard Constraints definiert werden.

Beispiele:

```text
Never use remote AI
Never delete automatically
Ask before external communication
Keep this data local
Do not migrate this workload
```

Adaptive oder AI-basierte Optimierung darf diese Regeln nicht entgegen der definierten Policy abschwächen.

## AI-generierte Policies

KI darf Policies vorschlagen.

```text
AI
 ↓
Policy Proposal
 ↓
Validation
 ↓
Authorization
 ↓
Policy Installation
```

Ein Vorschlag wird erst durch einen autorisierten Systemmechanismus zur tatsächlichen Policy.

## Trusted Model

Auch ein kryptografisch verifiziertes oder als vertrauenswürdig eingestuftes Modell erhält keine Sonderrechte.

```text
Trusted Model
    ≠
Policy Authority
```

Trust beschreibt die Vertrauenswürdigkeit eines Artefakts, nicht dessen Berechtigungen.

## Modelwechsel

Systemregeln dürfen nicht vom verwendeten Modell abhängen.

```text
Model A
Model B
Model C
   ↓
Same Hard Constraints
```

Dadurch können Modelle ausgetauscht werden, ohne das Sicherheitsmodell zu verändern.

## Fail-Safe Verhalten

Kann die AI Runtime eine erforderliche Regel nicht überprüfen:

```text
Required Constraint
       ↓
Cannot Verify
       ↓
Reject / Safe Degradation
```

Es gilt:

```text
Unknown ≠ Allowed
```

## Deterministic Enforcement

Die Durchsetzung harter Regeln muss unabhängig von probabilistischen AI-Ausgaben funktionieren.

NovaOS muss daher auch bei:

```text
AI Runtime unavailable
Model failure
Invalid output
No model installed
AI disabled
```

seine Hard Constraints vollständig durchsetzen können.

## Auditing

Versuche einer AI oder eines Agenten, eine Hard Constraint verletzende Aktion auszuführen, müssen auditierbar sein.

Relevante Informationen:

```text
Execution ID
Agent ID
Requested Action
Constraint
Validation Result
Decision ID
Rejected Action
```

Sensitive Inhalte werden dabei weiterhin minimiert.

## Normative Anforderungen

1. KI DARF keine Hard Constraints von NovaOS überschreiben.
2. Hard Constraints MÜSSEN außerhalb des AI-Modells durchgesetzt werden.
3. AI Output DARF nicht als Authorization behandelt werden.
4. AI Output DARF keine Capability erzeugen.
5. AI DARF Security-, Safety-, Sovereignty- oder Trust-Regeln nicht eigenständig abschwächen.
6. Ungültige Kandidaten SOLLEN vor AI-basierter Auswahl entfernt werden.
7. AI-Entscheidungen MÜSSEN vor ihrer Ausführung gegen aktuelle Hard Constraints validiert werden.
8. Existiert keine gültige Lösung, DARF AI keine Constraints lockern, um eine Lösung zu erzeugen.
9. `Unknown` DARF nicht durch AI-Vermutung als gültiger Sicherheitszustand behandelt werden.
10. Agent Planning DARF bestehende Systemregeln nicht umgehen.
11. Mehrstufige Agent-Aktionen DÜRFEN keine indirekte Constraint-Umgehung ermöglichen.
12. Prompt Injection DARF Systempolicies oder Capability Boundaries nicht verändern.
13. Adaptive Systeme DÜRFEN nur innerhalb des gültigen Constraint Space lernen und optimieren.
14. Self-Healing DARF Hard Security oder Safety Constraints nicht verletzen.
15. AI-basierte Ressourcenoptimierung DARF Hard Resource Reservations nicht überschreiben.
16. Explizite User Hard Constraints MÜSSEN von AI respektiert werden.
17. AI-generierte Policies MÜSSEN vor Aktivierung durch einen autorisierten Systempfad validiert werden.
18. Ein Trusted Model DARF keine zusätzliche Authority allein aufgrund seines Trust-Status erhalten.
19. Ein Modelwechsel DARF Hard System Rules nicht verändern.
20. Hard Constraint Enforcement MUSS ohne funktionierende AI Runtime möglich sein.
21. Fehlerhafte oder nicht verfügbare AI MUSS zu definiertem Fail-Safe-Verhalten führen.
22. Abgewiesene AI-Aktionen aufgrund harter Regeln MÜSSEN auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- KI kann NovaOS optimieren, ohne zur obersten Systemautorität zu werden,
- Sicherheits- und Safety-Eigenschaften bleiben unabhängig vom verwendeten Modell,
- Prompt Injection kann keine Systemregeln neu definieren,
- fehlerhafte AI-Entscheidungen werden vor der Ausführung abgefangen,
- NovaOS bleibt auch ohne AI sicher und funktionsfähig,
- AI-Modelle können ausgetauscht werden, ohne das grundlegende Policy-Modell zu verändern.

### Negative Konsequenzen

- jede AI-Aktion benötigt eine unabhängige Validierung,
- manche AI-Vorschläge können trotz erwarteter Vorteile nicht ausgeführt werden,
- Constraint Enforcement benötigt eine vollständig AI-unabhängige Systemimplementierung.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0009_Agent_Runtime`
- `ADR-AI-0010_Agent_Sandboxing`
- `ADR-AI-0011_Agent_Permissions_über_Capabilities`
- `ADR-AI-0012_Agent_Auditing`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0020_KI_nicht_Kernel_Voraussetzung`
- `ADR-CAPABILITY-0001`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AI-HARD-CONSTRAINT-ENFORCEMENT-0001`
- `NPSPEC-AI-OUTPUT-VALIDATION-0001`
- `NPSPEC-AI-POLICY-BOUNDARY-0001`
- `NPSPEC-AI-FAIL-SAFE-0001`
- `NPSPEC-AI-CONSTRAINT-AUDIT-0001`

## Ergebnis

KI befindet sich bewusst unterhalb der verbindlichen NovaOS-Systemregeln:

```text
Safety
Security
Authorization
Sovereignty
User Hard Constraints
        ↓
Hard Constraint Boundary
        ↓
Allowed Decision Space
        ↓
AI / Agent / Adaptation
        ↓
Final Validation
        ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
KI darf NovaOS helfen,
innerhalb der Regeln
bessere Entscheidungen zu treffen.

KI darf die Regeln selbst
nicht außer Kraft setzen.

Wenn zwischen
"intelligent funktionieren"
und
"harte Systemregel einhalten"
gewählt werden muss,

gewinnt immer
die harte Systemregel.
```