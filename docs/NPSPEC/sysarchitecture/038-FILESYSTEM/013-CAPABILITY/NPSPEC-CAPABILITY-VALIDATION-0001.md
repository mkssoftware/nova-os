# NPSPEC-CAPABILITY-VALIDATION-0001 – Nova Capability Validation

## Status

Angenommen

## Kategorie

Capability / Validation

## Zweck

NovaOS definiert die Validierung von Capabilities und ihrer Bestandteile vor Registrierung, Aktivierung und Ausführung.

Validierung stellt fest, ob deklarierte Identitäten, Interfaces, Daten, Abhängigkeiten und Ausführungsbedingungen strukturell und semantisch gültig sind.

## Grundprinzipien

```text
Validation ≠ Trust
Validation ≠ Permission
Validation ≠ Compatibility
Validation ≠ Authority
Valid ≠ Trusted
Valid ≠ Executable
```

## Validierungsbereiche

NovaOS unterscheidet mindestens:

```text
Identity Validation
Manifest Validation
Interface Validation
Input Validation
Parameter Validation
Output Validation
Dependency Validation
Implementation Validation
Execution Contract Validation
```

## Ablauf

```text
Capability Component
        ↓
Structural Validation
        ↓
Identity Validation
        ↓
Semantic Validation
        ↓
Constraint Validation
        ↓
Reference Validation
        ↓
Result
```

Anschließend können eigenständige Prüfungen für Trust, Compatibility, Permission und Policy erfolgen.

## Strukturvalidierung

Deklarative Daten müssen gegen ihre jeweilige Spezifikation geprüft werden.

Beispiele:

```text
Manifest Schema
Required Fields
Data Types
Version Syntax
CapabilityID Syntax
Interface Structure
```

Fehlende oder ungültige Pflichtangaben führen zu einem Validierungsfehler.

## Identitätsvalidierung

Identitäten müssen ihrem jeweiligen Modell entsprechen:

```text
CapabilityID
ProviderID
ImplementationID
PackageID
SemanticTypeID
```

Eine Validierung darf diese Identitäten nicht miteinander gleichsetzen oder neu interpretieren.

## Interface-Validierung

Capability Interfaces werden auf Konsistenz geprüft:

```text
Operations
Inputs
Outputs
Parameters
Semantic Types
Constraints
Error Model
Execution Semantics
```

Referenzierte Typen und Operationen müssen eindeutig auflösbar sein.

## Input und Parameter

Vor einer Ausführung müssen konkrete Eingaben und Parameter gegen den Interface-Vertrag geprüft werden:

```text
Input
  ↓
Semantic Type
  ↓
Constraints
  ↓
Lifetime / Transfer Rules
```

Parameter werden zusätzlich gegen Typ, Wertebereich und weitere deklarierte Constraints geprüft.

## Output

Erzeugte Ergebnisse müssen gegen den erwarteten Output-Vertrag validierbar sein.

Ein Provider darf nicht stillschweigend einen semantisch inkompatiblen Ergebnistyp liefern.

## Abhängigkeiten

Deklarierte Abhängigkeiten müssen strukturell gültig und auflösbar sein.

Zyklen, ungültige Referenzen und widersprüchliche Versionsanforderungen müssen erkannt werden können.

## Execution Contract

Vor der Ausführung muss geprüft werden, ob der Contract vollständig und widerspruchsfrei ist.

Beispiele:

```text
Required Operation exists
Input Types valid
Parameters valid
Output expectations valid
Resource constraints consistent
Deadline valid
Determinism requirements valid
```

## Validierungszustände

Mindestens:

```text
Valid
Invalid
Incomplete
Unknown
```

`Unknown` darf nicht automatisch als `Valid` behandelt werden.

## Fehler

Validierungsfehler müssen strukturiert zurückgegeben werden können:

```text
ValidationError
├── Component
├── Field
├── Rule
├── Expected
├── Actual
└── ErrorCode
```

Dadurch können NovaLang Studio, Solution Editor, Installer und Systemdiagnose Fehler präzise anzeigen.

## Sicherheit

Validierung erzeugt keine Authority.

```text
Valid Capability
       ≠
Authorized Capability
```

Auch vollständig gültige Komponenten müssen weiterhin Trust-, Policy-, Permission- und Compatibility-Prüfungen durchlaufen.

## Normative Anforderungen

1. Capability-Komponenten MÜSSEN vor ihrer Verwendung validierbar sein.
2. Struktur-, Identitäts-, semantische und Constraint-Validierung MÜSSEN unterscheidbar sein.
3. Manifest und Interface MÜSSEN gegen ihre jeweiligen Verträge geprüft werden.
4. Capability-, Provider-, Package- und Implementation-Identitäten MÜSSEN validierbar sein.
5. Inputs und Parameter MÜSSEN vor der Ausführung validiert werden.
6. Outputs MÜSSEN gegen den definierten Output-Vertrag prüfbar sein.
7. Referenzierte SemanticTypeIDs MÜSSEN validierbar sein.
8. Abhängigkeiten und Versionsanforderungen MÜSSEN auf Konsistenz geprüft werden.
9. Execution Contracts MÜSSEN vor Ausführung validiert werden.
10. `Unknown` DARF nicht automatisch als `Valid` gelten.
11. Validierung DARF keine Trust-, Permission- oder Authority-Entscheidung ersetzen.
12. Validierungsfehler MÜSSEN strukturiert und maschinenlesbar sein.
13. Validierungsergebnis und Fehlerursache MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-INPUT-0001`
- `NPSPEC-CAPABILITY-OUTPUT-0001`
- `NPSPEC-CAPABILITY-PARAMETER-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`

## Ergebnis

NovaOS besitzt einen einheitlichen Validierungsmechanismus für Capability-Identitäten, Verträge, Daten, Abhängigkeiten und Ausführungsanforderungen. Fehler können frühzeitig und strukturiert erkannt werden, während Validierung klar von Trust, Compatibility, Permission und Authority getrennt bleibt.