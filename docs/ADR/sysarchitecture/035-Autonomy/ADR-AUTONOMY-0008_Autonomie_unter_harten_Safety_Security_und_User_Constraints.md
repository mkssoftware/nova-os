# ADR-AUTONOMY-0008 – Autonomie unter harten Safety-, Security- und User-Constraints

## Status

Angenommen

## Kategorie

Autonomy / Safety / Security / User Authority

## Kontext

NovaOS soll weitreichend autonom handeln können.

Autonomie darf jedoch niemals bedeuten, dass das System grundlegende Grenzen selbstständig verändern oder umgehen kann.

Besonders relevant sind drei Constraint-Klassen:

```text
Safety Constraints
Security Constraints
User Constraints
```

Diese definieren den verbindlichen Handlungsrahmen autonomer Funktionen.

## Entscheidung

NovaOS führt autonome Entscheidungen ausschließlich innerhalb explizit bestimmter **Safety-, Security- und User-Constraints** aus.

```text
Intent
   ↓
Safety Constraints
   ↓
Security Constraints
   ↓
User Constraints
   ↓
Valid Decision Space
   ↓
Autonomous Decision
```

Adaptive Modelle, Optimierung und Self-* Funktionen arbeiten ausschließlich innerhalb dieses gültigen Raums.

## Safety Constraints

Safety Constraints schützen vor Zuständen oder Aktionen, die Menschen, Geräte, Daten oder kontrollierte Prozesse gefährden können.

Beispiele:

```text
Thermal Limits
Electrical Limits
Hardware Safety
Critical Resource Limits
Safe Operating States
Realtime Safety Requirements
Irreversible Operation Guards
```

Safety Constraints sind Hard Constraints.

Autonomie darf sie weder abschwächen noch umgehen.

## Security Constraints

Security Constraints schützen Identität, Autorisierung, Integrität, Vertraulichkeit und Trust.

Beispiele:

```text
Authorization
Capability Boundaries
Trust Requirements
Data Sovereignty
Integrity Requirements
Isolation
Information Flow
Cryptographic Requirements
```

Eine autonome Aktion darf nur ausgeführt werden, wenn die erforderlichen Security Constraints erfüllt sind.

```text
Unknown Security State
        ≠
Authorized
```

## User Constraints

Nutzer können den autonomen Handlungsspielraum ausdrücklich einschränken.

Beispiele:

```text
Keep Data Local
Do Not Use Cellular
Do Not Migrate
Never Delete Automatically
Ask Before Irreversible Action
Disable Automatic Optimization
Use Specific Provider
```

Diese Vorgaben werden als explizite Constraints behandelt und nicht als bloße adaptive Präferenzen.

## Priorität

Für autonome Entscheidungen gilt grundsätzlich:

```text
Safety
   ↓
Security
   ↓
Data Sovereignty / Trust
   ↓
System Hard Constraints
   ↓
Explicit User Constraints
   ↓
Soft User Preferences
   ↓
Adaptive Optimization
```

Eine niedrigere Ebene darf eine höhere Ebene nicht überschreiben.

## Nutzerautorität

Innerhalb des sicheren und autorisierten Lösungsraums besitzt die explizite Nutzerentscheidung Vorrang vor autonomen Entscheidungen.

```text
Valid System Space
      ↓
User Constraint
      ↓
Remaining Space
      ↓
Autonomous Optimization
```

NovaOS darf nicht versuchen, eine unerwünschte Nutzerentscheidung durch wiederholte adaptive Gegensteuerung zu umgehen.

## Constraint Composition

Constraints werden vor der autonomen Entscheidung zu einem gemeinsamen Constraint Set kombiniert.

```text
Safety
Security
ExecutionContract
System Policy
User Constraints
Current State
      ↓
Constraint Composition
      ↓
Valid Decision Space
```

Erst danach darf eine autonome Policy eine konkrete Aktion auswählen.

## Konflikte

Widersprechen sich Hard Constraints, darf NovaOS keinen stillen Kompromiss erzeugen.

```text
Constraint A
     +
Constraint B
     ↓
No Valid Solution
```

Das Ergebnis muss als nicht erfüllbarer Constraint-Konflikt behandelt werden.

## Autonomy Levels

Constraints dürfen zusätzlich festlegen, welchen Autonomiegrad NovaOS besitzt.

Beispiel:

```text
Observe
Recommend
Prepare
Execute
Execute + Verify
Execute + Recover
```

Ein Nutzer oder eine übergeordnete Policy kann beispielsweise festlegen:

```text
Automatic Optimization → Execute

Software Removal → Recommend

Irreversible Storage Operation → Require Confirmation
```

## Irreversible Aktionen

Irreversible oder schwer rückgängig zu machende Aktionen benötigen besondere Grenzen.

```text
Delete Persistent Data
Destroy Encryption Keys
Firmware Modification
Permanent External Action
Irreversible Device Operation
```

Policies oder User Constraints können dafür explizite Bestätigung verlangen.

## Self-Configuration

Self-Configuration darf nur Einstellungen verändern, die innerhalb ihrer Autonomy Boundary liegen.

Explizite Nutzerkonfigurationen dürfen nicht stillschweigend überschrieben werden.

## Self-Healing

Self-Healing darf Safety-, Security- oder User-Constraints nicht verletzen, um Verfügbarkeit wiederherzustellen.

```text
Recovery Candidate
      ↓
Constraint Check
      ↓
Allowed Recovery
```

Eine sichere Degradation besitzt Vorrang vor einer verbotenen Recovery.

## Self-Optimization

Self-Optimization arbeitet ausschließlich im verbleibenden Optimierungsraum.

```text
Hard Constraints
      ↓
User Constraints
      ↓
Valid Optimization Space
      ↓
Self-Optimization
```

Performance oder Energieeffizienz rechtfertigen keine Constraint-Verletzung.

## Self-Protection

Self-Protection darf bei Security-Ereignissen bestehende Nutzungsmöglichkeiten einschränken.

Beispiel:

```text
Security Threat
      ↓
Containment
      ↓
Temporary Restriction
```

Dies gilt insbesondere dann, wenn eine Fortsetzung des normalen Betriebs gegen einen Security Hard Constraint verstoßen würde.

## Adaptive Systeme

Adaptive Modelle dürfen Constraints niemals selbstständig verändern.

```text
Constraints
    ↓
Valid Space
    ↓
Adaptive Model
    ↓
Decision
```

Policy Learning lernt ausschließlich die Auswahl innerhalb dieses Raums.

## Constraint Changes

Ändert sich ein relevanter Constraint während einer laufenden autonomen Aktion, muss die Gültigkeit neu bewertet werden.

```text
Running Action
      ↓
Constraint Change
      ↓
Reevaluation
      ↓
Still Valid?
 ├── Yes → Continue
 └── No  → Stop / Replan / Safe State
```

## Unknown State

Für Hard Constraints gilt:

```text
Unknown ≠ Satisfied
```

Kann NovaOS die Einhaltung einer erforderlichen Grenze nicht feststellen, darf die betreffende autonome Aktion nicht als zulässig betrachtet werden.

## Explainability

NovaOS soll bei autonomen Entscheidungen nachvollziehbar machen können:

```text
Welche Safety Constraints galten?

Welche Security Constraints galten?

Welche User Constraints galten?

Welche Kandidaten wurden ausgeschlossen?

Welche Handlungsmöglichkeiten blieben?

Warum wurde die konkrete Aktion gewählt?
```

## Deterministic Mode

Für reproduzierbare autonome Entscheidungen müssen relevante:

```text
Constraint Sets
Constraint Versions
User Constraints
Policies
System State
Adaptive State
```

fixiert oder versioniert werden können.

## Observability

Constraint-basierte autonome Entscheidungen müssen mit Decision Tracing korrelierbar sein.

Relevante Informationen sind:

```text
Decision ID
Constraint Set
Constraint Version
Rejected Candidates
Selected Action
Autonomy Level
Verification Result
```

Sensitive Security-Informationen dürfen dabei nur entsprechend ihrer Berechtigungen sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS autonome Entscheidungen innerhalb expliziter Hard Constraints ausführen.
2. Safety Constraints MÜSSEN Vorrang vor autonomen Optimierungen besitzen.
3. Security Constraints MÜSSEN vor jeder sicherheitsrelevanten autonomen Aktion erfüllt sein.
4. Explizite User Constraints MÜSSEN innerhalb übergeordneter Hard Constraints verbindlich sein.
5. Adaptive Systeme DÜRFEN Safety-, Security- oder User-Constraints nicht selbstständig verändern.
6. Self-Healing DARF Hard Constraints nicht zugunsten von Verfügbarkeit abschwächen.
7. Self-Optimization DARF Constraints nicht zugunsten von Performance oder Effizienz verletzen.
8. Self-Configuration DARF explizite Nutzerkonfigurationen nicht stillschweigend überschreiben.
9. Self-Protection DARF bei notwendigen Sicherheitsmaßnahmen den normalen Betrieb einschränken.
10. Irreversible Aktionen MÜSSEN durch geeignete Autonomy Policies begrenzbar sein.
11. Constraint-Konflikte DÜRFEN nicht durch stilles Abschwächen aufgelöst werden.
12. Änderungen relevanter Hard Constraints MÜSSEN eine Neubewertung laufender autonomer Entscheidungen auslösen können.
13. `Unknown` DARF nicht als erfüllter Hard Constraint interpretiert werden.
14. Autonomy Levels MÜSSEN durch Policies oder Constraints begrenzbar sein.
15. Relevante Constraint Decisions SOLLEN erklärbar sein.
16. Deterministic Mode MUSS Constraint-basierte autonome Entscheidungen reproduzierbar machen können.
17. Constraint Decisions MÜSSEN mit Decision Tracing und Observability korrelierbar sein.
18. Existiert keine zulässige autonome Aktion, MUSS NovaOS fail-safe reagieren.

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann weitreichend autonom handeln, ohne grundlegende Grenzen zu verlieren,
- Safety und Security bleiben unabhängig von adaptiven Modellen verbindlich,
- Nutzer können den autonomen Handlungsspielraum ausdrücklich begrenzen,
- Self-* Funktionen verwenden dasselbe Constraint-Modell,
- autonome Entscheidungen bleiben kontrollierbar und erklärbar.

### Negative Konsequenzen

- starke Constraints können autonome Optimierungsmöglichkeiten einschränken,
- Constraint Composition erhöht die Planungslogik,
- widersprüchliche User- und Systemanforderungen können Aktionen verhindern.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0003_Self_Configuration`
- `ADR-AUTONOMY-0005_Self_Healing`
- `ADR-AUTONOMY-0006_Self_Optimization`
- `ADR-AUTONOMY-0007_Self_Protection`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-HARD-CONSTRAINTS-0001`
- `NPSPEC-AUTONOMY-SAFETY-CONSTRAINTS-0001`
- `NPSPEC-AUTONOMY-SECURITY-CONSTRAINTS-0001`
- `NPSPEC-AUTONOMY-USER-CONSTRAINTS-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-PRIORITY-0001`

## Ergebnis

NovaOS besitzt weitreichende Autonomie, aber keinen unbegrenzten Handlungsspielraum:

```text
Safety
   ↓
Security
   ↓
System Hard Constraints
   ↓
User Constraints
   ↓
Valid Decision Space
   ↓
Autonomy
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf selbstständig handeln,
aber nicht selbstständig entscheiden,
welche Grenzen für sein Handeln gelten.

Safety schützt die sicheren Grenzen.

Security schützt Vertrauen,
Rechte und Daten.

Der Nutzer bestimmt innerhalb
dieses sicheren Raums,
wie weit NovaOS autonom handeln darf.
```