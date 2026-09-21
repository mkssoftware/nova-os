# ADR-AUTONOMY-0002 – Constraint-Driven Autonomy

## Status

Angenommen

## Kategorie

Autonomy / Constraints / System Control

## Kontext

Policy-Driven Autonomy definiert, welche autonomen Entscheidungen NovaOS grundsätzlich treffen darf.

Für eine konkrete Entscheidung müssen jedoch zahlreiche technische und semantische Constraints gleichzeitig erfüllt werden.

Beispiele:

```text
Security
Safety
Data Sovereignty
Trust
Authorization
Correctness
Deadline
Determinism
Resource Budget
Locality
Hardware Requirements
User Constraints
```

Autonomie darf daher nicht zuerst eine Aktion wählen und anschließend prüfen, ob diese zulässig ist.

## Entscheidung

NovaOS verwendet **Constraint-Driven Autonomy**.

Autonome Entscheidungen entstehen ausschließlich aus dem Lösungsraum, der alle relevanten Hard Constraints erfüllt.

```text
Intent
   +
System State
   +
Constraints
   ↓
Constraint Evaluation
   ↓
Valid Solution Space
   ↓
Autonomous Selection
   ↓
Execution
   ↓
Verification
```

## Constraint Space

Für jede autonome Entscheidung wird zunächst der gültige Entscheidungsraum bestimmt.

```text
Possible Actions
      ↓
Hard Constraint Filtering
      ↓
Valid Actions
      ↓
Soft Optimization
      ↓
Selected Action
```

Eine ungültige Aktion darf nicht durch gute Performance, niedrigen Energieverbrauch oder eine hohe adaptive Bewertung legitimiert werden.

## Hard Constraints

Hard Constraints müssen vollständig erfüllt werden.

Typische Hard Constraints sind:

```text
Safety
Security
Authorization
Data Sovereignty
Trust
Correctness
Hard Realtime
Required Determinism
Resource Limits
Required Hardware
Forced Algorithm
Forced Provider
Explicit User Constraints
```

Kann ein Hard Constraint nicht erfüllt werden, ist die betreffende Lösung ungültig.

## Soft Constraints

Nach dem Hard Filtering dürfen Soft Constraints zur Auswahl zwischen gültigen Lösungen verwendet werden.

Beispiele:

```text
Performance
Preferred Latency
Energy Efficiency
Locality
Resource Efficiency
Preferred Provider
Preferred Algorithm
Adaptive Preference
```

```text
Valid Solutions
      ↓
Soft Constraint Evaluation
      ↓
Best Applicable Plan
```

## Constraint Composition

Constraints können aus mehreren Quellen stammen:

```text
System
Security
Safety
ExecutionContract
Resource Policy
Sovereignty Policy
User
Administrator
Capability
Object
Environment
```

Sie werden zu einem gemeinsamen Constraint Set zusammengeführt.

## Constraint Conflicts

Widersprechen sich Hard Constraints, darf NovaOS keine scheinbar optimale Kompromisslösung erzeugen.

```text
Hard Constraint A
        +
Hard Constraint B
        ↓
No Valid Solution
        ↓
Unsatisfied
```

Der Konflikt muss als solcher sichtbar und erklärbar sein.

## Dynamic Constraints

Einige Constraints können sich während einer Ausführung verändern.

Beispiele:

```text
Resource Availability
Network State
Trust State
Thermal Limits
Provider Availability
Storage Availability
```

Bei relevanten Änderungen muss NovaOS prüfen, ob die aktuelle Ausführung weiterhin gültig ist.

```text
Constraint Change
      ↓
Reevaluation
      ↓
Still Valid?
   ├── Yes → Continue
   └── No  → Replan / Safe Action
```

## Replanning

Wird eine geplante Aktion ungültig, darf NovaOS autonom neu planen.

Der neue Plan muss dieselben weiterhin gültigen Hard Constraints erfüllen.

```text
Current Plan
     ↓
Constraint Violation
     ↓
Valid Solution Space
     ↓
Alternative Plan
```

Constraints dürfen beim Replanning nicht stillschweigend abgeschwächt werden.

## Nutzerconstraints

Explizite Nutzerentscheidungen können den autonomen Lösungsraum einschränken.

Beispiele:

```text
Keep Local
Do Not Use Cellular
Use Provider X
Do Not Migrate
Prefer Performance
```

Dabei bleiben übergeordnete Safety-, Security- und andere Hard Constraints verbindlich.

## Adaptive Systeme

Adaptive Modelle dürfen innerhalb des gültigen Constraint Space optimieren.

```text
Hard Constraints
       ↓
Valid Solutions
       ↓
Adaptive Prediction
       ↓
Adaptive Selection
```

Prediction Error kann zukünftige Bewertungen verbessern, aber niemals die Gültigkeit eines Hard Constraints verändern.

## Unknown State

Kann NovaOS nicht feststellen, ob ein erforderlicher Hard Constraint erfüllt ist, gilt:

```text
Unknown ≠ Satisfied
```

Die betreffende Lösung darf nicht als gültig behandelt werden.

## Constraint Evidence

Die Erfüllung kritischer Constraints muss auf geeigneter Evidence beruhen können.

Beispiele:

```text
Resource State
Trust Evidence
Authorization State
Location Evidence
Provider Capabilities
Execution Properties
```

Eine bloße Prediction darf keinen erforderlichen Nachweis ersetzen.

## Constraint Verification

Constraints werden nicht ausschließlich während der Planung geprüft.

```text
Plan
  ↓
Pre-Execution Validation
  ↓
Execution
  ↓
Post-Execution Verification
```

Damit kann NovaOS erkennen, ob erwartete Bedingungen tatsächlich eingehalten wurden.

## Declarative System Model

Constraint-Driven Autonomy ergänzt das deklarative Systemmodell.

```text
Desired State
      ↓
Constraints
      ↓
Valid Transition Space
      ↓
Autonomous Planning
      ↓
Reconciliation
```

NovaOS darf selbstständig gültige Übergänge auswählen, aber nicht eigenmächtig den Desired State oder seine Grenzen verändern.

## Explainability

Autonome Entscheidungen müssen relevante Constraints nachvollziehbar machen können.

NovaOS soll beantworten können:

```text
Welche Constraints galten?

Welche Kandidaten wurden ausgeschlossen?

Warum wurden sie ausgeschlossen?

Welche gültigen Alternativen blieben?

Warum wurde die gewählte Alternative verwendet?
```

Decision Tracing verwendet dafür stabile Constraint- und Reason-Codes.

## Deterministic Mode

Constraint Evaluation muss im Deterministic Mode reproduzierbar sein.

Relevante:

```text
Constraint Set
Constraint Versions
System State
Policy Version
Adaptive State
Tie-Breaking Rules
```

müssen fixiert, versioniert oder kontrolliert werden können.

## Fail-Safe Verhalten

Kann das Constraint-System keine sichere Entscheidung bestimmen, muss NovaOS fail-safe reagieren.

```text
No Valid Solution
       ↓
Do Not Violate Constraint
       ↓
Fail / Degrade / Request Decision
```

Eine autonome Aktion ist nicht wichtiger als die Einhaltung ihrer Grenzen.

## Normative Anforderungen

1. NovaOS MUSS autonome Entscheidungen aus einem gültigen Constraint Space ableiten.
2. Hard Constraints MÜSSEN vor Soft Optimization ausgewertet werden.
3. Eine Lösung mit verletztem Hard Constraint DARF nicht ausgewählt werden.
4. Soft Constraints DÜRFEN ausschließlich zwischen gültigen Lösungen optimieren.
5. Constraints aus unterschiedlichen Systembereichen MÜSSEN kombinierbar sein.
6. Konfligierende Hard Constraints MÜSSEN als nicht erfüllbar erkannt werden.
7. Hard Constraints DÜRFEN nicht automatisch abgeschwächt werden.
8. Dynamische Constraint-Änderungen MÜSSEN eine Neubewertung auslösen können.
9. Replanning MUSS weiterhin alle gültigen Hard Constraints erfüllen.
10. Explizite Nutzerconstraints MÜSSEN innerhalb übergeordneter Hard Constraints berücksichtigt werden.
11. Adaptive Modelle DÜRFEN nur innerhalb des gültigen Constraint Space entscheiden.
12. Prediction DARF erforderliche Constraint Evidence nicht ersetzen.
13. `Unknown` DARF nicht als erfüllter Hard Constraint behandelt werden.
14. Kritische Constraints MÜSSEN vor der Ausführung validierbar sein.
15. Constraint-Erfüllung SOLL nach relevanten Ausführungen verifizierbar sein.
16. Constraint Decisions MÜSSEN mit Decision Tracing korrelierbar sein.
17. Deterministic Mode MUSS Constraint Evaluation reproduzierbar machen können.
18. Existiert keine gültige Lösung, MUSS NovaOS fail-safe reagieren.

## Konsequenzen

### Positive Konsequenzen

- autonome Entscheidungen bleiben systemweit kontrolliert,
- Security, Safety und Sovereignty werden Bestandteil des Entscheidungsraums,
- adaptive Optimierung kann keine Hard Constraints umgehen,
- Replanning verwendet dieselben Regeln wie ursprüngliche Planung,
- abgelehnte und ausgewählte Lösungen werden erklärbar.

### Negative Konsequenzen

- Constraint Composition erhöht die Planungslogik,
- widersprüchliche Constraints können Ausführungen vollständig verhindern,
- dynamische Constraints können zusätzliche Replanning-Vorgänge verursachen.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0009_Architecture_Introspection_als_Systemfunktion`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-CONSTRAINTS-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-COMPOSITION-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-EVALUATION-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-REPLANNING-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-VERIFICATION-0001`

## Ergebnis

NovaOS leitet autonome Entscheidungen aus den gültigen Möglichkeiten ab:

```text
Intent
   ↓
Constraints
   ↓
Valid Solution Space
   ↓
Autonomous Selection
   ↓
Execution
   ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
NovaOS entscheidet nicht zuerst
und prüft danach die Grenzen.

Die Grenzen bestimmen zuerst,
welche Entscheidungen überhaupt
möglich sind.

Autonomie beginnt erst
innerhalb dieses gültigen Raums.
```