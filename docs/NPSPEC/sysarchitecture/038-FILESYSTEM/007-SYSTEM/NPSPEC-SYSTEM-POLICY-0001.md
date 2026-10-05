# NPSPEC-SYSTEM-POLICY-0001 – Nova System Policy

## Status

Angenommen

## Kategorie

System / Policy

## Zweck

NovaOS definiert ein einheitliches Policy-Modell für systemweite Entscheidungen.

Policies bestimmen, welche zulässige Option unter gegebenen Bedingungen gewählt wird, ohne die zugrunde liegenden Kernel-, Capability- oder Systemmechanismen selbst zu ersetzen.

## Grundprinzipien

```text
Policy ≠ Mechanism
Policy ≠ Authority
Policy ≠ Capability
Policy ≠ Trust
Policy Decision ≠ Implementation
```

## Policy-Modell

Eine Policy kann mindestens enthalten:

```text
Policy
├── PolicyID
├── Scope
├── Conditions
├── Rules
├── Priority
├── Source
└── Version
```

Policies müssen eindeutig identifizierbar und versionierbar sein.

## Scopes

Policies können für unterschiedliche Kontexte gelten:

```text
System
User
Program
Solution
Workspace
Service
Device
Resource
```

Mehrere Policies dürfen gleichzeitig auf eine Entscheidung wirken.

## Prioritäten

Bei konkurrierenden Anforderungen gilt grundsätzlich:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine niedrigere Ebene darf eine höhere Einschränkung nicht überschreiben.

## Policy Evaluation

```text
Request
   ↓
Context
   ↓
Applicable Policies
   ↓
Constraint Evaluation
   ↓
Conflict Resolution
   ↓
Policy Decision
   ↓
System Mechanism
```

Die Ausführung erfolgt anschließend durch die zuständige Systemkomponente.

## Hard und Soft Constraints

Policies unterscheiden zwischen:

```text
Hard Constraint
Soft Preference
```

Hard Constraints müssen erfüllt sein.

Soft Preferences dürfen zur Optimierung verwendet werden, sofern keine höherrangige Regel verletzt wird.

## Benutzerentscheidungen

Explizite Benutzerentscheidungen haben Vorrang vor adaptiven oder automatisch erlernten Optimierungen, solange sie keine Safety-, Security- oder zwingenden Systembedingungen verletzen.

```text
User Decision
      >
Adaptive Prediction
```

## Dynamische Policies

Policies dürfen zur Laufzeit aktualisiert werden.

Änderungen müssen kontrolliert, versioniert und bei sicherheitsrelevanten Policies autorisiert erfolgen.

Bestehende Entscheidungen können bei relevanten Policy-Änderungen neu bewertet werden.

## Fail-Safe

Kann keine eindeutige sichere Entscheidung getroffen werden, gilt für sicherheitskritische Operationen:

```text
Unknown / Conflict
        ↓
Deny / Restrict / Safe Fallback
```

Fehler dürfen nicht automatisch zu erweiterten Rechten führen.

## Introspection

Policy-Entscheidungen müssen nachvollziehbar sein.

Mindestens folgende Informationen sollen verfügbar sein:

```text
Applied Policies
Decision
Reason
Priority
Constraints
Fallback
```

## Normative Anforderungen

1. NovaOS MUSS Mechanismus und Policy logisch trennen.
2. Policies MÜSSEN stabile Identitäten und Versionen besitzen können.
3. Policies MÜSSEN unterschiedliche Scopes unterstützen.
4. Mehrere gleichzeitig geltende Policies MÜSSEN deterministisch auflösbar sein.
5. Safety und Security MÜSSEN Vorrang vor Optimierungszielen besitzen.
6. Hard Constraints DÜRFEN nicht durch Soft Preferences überschrieben werden.
7. Explizite Benutzerentscheidungen MÜSSEN adaptive Optimierungen übersteuern können.
8. Policies DÜRFEN keine Authority oder Capability erzeugen.
9. Sicherheitsrelevante Policy-Änderungen MÜSSEN autorisiert sein.
10. Unsichere oder unbekannte Zustände MÜSSEN einen sicheren Fallback ermöglichen.
11. Policy-Entscheidungen MÜSSEN introspektierbar sein.
12. Policy-Auswertung SOLL deterministisch reproduzierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS besitzt ein einheitliches und nachvollziehbares Policy-Modell. Systementscheidungen folgen einer klaren Priorität von Safety und Security bis zu Benutzerpräferenzen und adaptiver Optimierung, während die eigentlichen Mechanismen, Capabilities und Authority strikt getrennt bleiben.