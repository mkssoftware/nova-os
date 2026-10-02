# NPSPEC-VERIFY-STATICANALYSIS-0001 – Nova Static Analysis

## Status

Angenommen

## Kategorie

Verification / Static Analysis / Verified Core

## Zweck

NovaOS definiert Static Analysis als automatisierte Analyse von Quellcode, Intermediate Representation und erzeugungsnahen Artefakten, ohne die analysierte Komponente ausführen zu müssen.

```text
Source Code
     ↓
Static Analysis
     ↓
Findings
     ↓
Verification / Build Decision
```

Static Analysis ergänzt formale Verifikation, Model Checking, Compiler-Prüfungen, Tests und Runtime Verification.

## Grundprinzipien

```text
Static Analysis ≠ Formal Proof
No Finding ≠ No Defect
Warning ≠ Confirmed Defect
Type Correct ≠ Memory Safe
Memory Safe ≠ Authorized
Analyzer Result ≠ Runtime Result
Suppressed Finding ≠ Fixed Finding
```

## Analysis Model

```text
StaticAnalysis
├── AnalysisID
├── Target
├── SourceVersion
├── AnalysisProfile
├── Rules
├── Assumptions
└── Findings
```

Optional:

```text
BuildID
CompilerVersion
IRVersion
AnalyzerVersion
VerificationProfile
SuppressionSet
Baseline
ProvenanceID
```

## Analyseebenen

NovaOS darf Static Analysis auf mehreren Ebenen einsetzen:

```text
Source Code
AST
Compiler IR
Nova IR
Binary-adjacent IR
Interface Definitions
Configuration
```

Unterschiedliche Ebenen können unterschiedliche Fehlerklassen erkennen.

## Fehlerklassen

Static Analysis soll insbesondere erkennen können:

```text
Invalid Memory Access
Use-after-Free
Double Free
Null Dereference
Integer Overflow
Invalid Cast
Type Confusion
Uninitialized Data
Resource Leak
Locking Error
Deadlock Risk
Race Risk
Unchecked Error
Invalid Lifetime
Capability Misuse
Information Flow Violation
```

Nicht jede Fehlerklasse muss ausschließlich durch Static Analysis vollständig beweisbar sein.

## Dataflow Analysis

Datenflüsse sollen analysiert werden können.

```text
Input
  ↓
Transformation
  ↓
State
  ↓
Output
```

Dabei können beispielsweise verfolgt werden:

```text
Definitions
Uses
Ownership
Initialization
Taint
Security Labels
Capability References
```

## Control Flow Analysis

Kontrollflüsse müssen auf problematische Pfade untersucht werden können.

```text
Entry
 ├── Path A
 ├── Path B
 └── Path C
       ↓
      Exit
```

Analysen sollen unerreichbaren Code, fehlende Fehlerbehandlung und ungültige Zustandsübergänge erkennen können.

## Interprocedural Analysis

Kritische Eigenschaften dürfen über Funktions- und Modulgrenzen hinweg analysiert werden.

```text
Function A
    ↓
Function B
    ↓
Function C
```

Lokale Korrektheit darf nicht automatisch als globale Korrektheit interpretiert werden.

## Memory Safety

Static Analysis soll Memory-Safety-Verifikation unterstützen.

Zu prüfen sind insbesondere:

```text
Bounds
Lifetime
Ownership
Initialization
Pointer Arithmetic
Allocation / Release
```

Für nicht beweisbare Fälle können Runtime Checks erforderlich bleiben.

## Type Safety

Typoperationen sollen statisch gegen definierte Regeln geprüft werden.

```text
Value Type
    ↓
Operation Requirement
    ↓
Compatible?
```

Unsichere Casts und Typumgehungen müssen explizit erkennbar sein.

## Capability Safety

Static Analysis soll Capability-Flows untersuchen können.

```text
Capability
   ↓
Delegate
   ↓
Transfer
   ↓
Use
```

Dabei sollen mögliche Verstöße wie:

```text
Unauthorized Transfer
Authority Amplification
Missing Validation
Use after Revocation Boundary
Ambient Authority
```

erkennbar sein, soweit sie statisch ableitbar sind.

## Information Flow

Security Labels und sensitive Datenflüsse sollen statisch verfolgt werden können.

```text
Restricted Source
       ↓
Dataflow
       ↓
Public Sink
```

Ein unzulässiger Flow soll als Finding erzeugt werden.

## Concurrency

Static Analysis soll bekannte Nebenläufigkeitsprobleme erkennen können:

```text
Lock Ordering
Missing Synchronization
Potential Race
Deadlock Cycle
Unsafe Shared State
Invalid Atomic Usage
```

Komplexe Interleavings können zusätzlich Model Checking erfordern.

## Resource Analysis

Ressourcen-Lifetimes sollen analysiert werden können.

Beispiele:

```text
Memory
Handle
File
Socket
Capability
Lock
DMA Mapping
Shared Buffer
```

```text
Acquire
   ↓
Use
   ↓
Release
```

Fehlende oder mehrfache Freigaben sollen erkannt werden können.

## Error Handling

Fehlerpfade müssen Bestandteil der Analyse sein.

```text
Operation
├── Success
└── Error
      ↓
   Cleanup?
```

Ignorierte kritische Fehler und unvollständiges Cleanup sollen erkannt werden können.

## Unsafe Boundaries

Unsichere Operationen müssen besonders analysiert werden.

Beispiele:

```text
Raw Pointer
Inline Assembly
FFI
MMIO
DMA
Architecture Code
Boot Code
Unchecked Cast
```

```text
Unsafe Boundary
      ↓
Stricter Analysis
```

Unsafe Code darf nicht von der Analyse ausgenommen werden, nur weil er explizit als unsafe markiert ist.

## Suppressions

False Positives dürfen kontrolliert unterdrückt werden.

Eine Suppression soll enthalten:

```text
Rule
Location
Reason
Scope
Owner
Optional Expiration
```

```text
Suppression ≠ Proof of Safety
```

Breite globale Suppressions sollen vermieden werden.

## Severity

Findings können klassifiziert werden:

```text
Info
Warning
Error
Critical
```

Für den Verified Core können bestimmte Klassen den Build oder Verification Status blockieren.

## Build Integration

Static Analysis soll Bestandteil der Build- und Verification-Pipeline sein.

```text
Source
  ↓
Compile / Analyze
  ↓
Critical Finding?
├── Yes → Reject
└── No  → Continue
```

Regeln für Build Blocking müssen reproduzierbar definiert sein.

## Incremental Analysis

Für schnelle Entwicklungszyklen darf NovaOS inkrementelle Analyse verwenden.

```text
Changed Code
     ↓
Affected Dependencies
     ↓
Reanalyze
```

Release- oder Verified-Core-Builds dürfen strengere vollständige Analysen verlangen.

## Formal Verification Integration

Static Analysis kann Voraussetzungen für formale Verifikation liefern.

```text
Static Analysis
      ↓
Reduced Defect Surface
      ↓
Formal Verification
```

Ein Static-Analysis-Ergebnis darf jedoch nicht als formaler Beweis dargestellt werden, sofern der verwendete Analyzer diesen Beweis nicht tatsächlich liefert.

## Runtime Verification

Nicht statisch entscheidbare Eigenschaften können Runtime Contracts erzeugen.

```text
Static Analysis
      ↓
Property Unknown
      ↓
Insert / Require Runtime Check
```

Damit entsteht:

```text
Static Verification
        +
Runtime Verification
```

## Verification Artifacts

Analyseergebnisse müssen versionierbar sein.

```text
AnalysisID
SourceVersion
BuildID
RulesetVersion
Analyzer
AnalyzerVersion
Findings
Suppressions
Result
```

Änderungen an Code, Analyzer oder relevanten Regeln können eine erneute Analyse erforderlich machen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Analysis Status
Ruleset
Analyzer Version
Findings
Severity
Suppressions
Coverage
Source Version
Build ID
Last Analysis
```

## Normative Anforderungen

1. NovaOS MUSS Static Analysis für sicherheitskritische Komponenten unterstützen.
2. Der Verified Core MUSS einer definierten Static-Analysis-Policy unterliegen.
3. Static Analysis DARF NICHT automatisch als formaler Beweis interpretiert werden.
4. Source-, AST- und IR-basierte Analysen MÜSSEN unterstützt werden können.
5. Memory-Safety-Probleme SOLLEN statisch erkannt werden, soweit technisch möglich.
6. Type-Safety-Verstöße SOLLEN statisch erkannt werden.
7. Capability-Flows SOLLEN statisch analysierbar sein.
8. Kritische Information Flows SOLLEN statisch analysierbar sein.
9. Resource Lifetimes SOLLEN analysierbar sein.
10. Fehlerpfade MÜSSEN Bestandteil relevanter Analysen sein.
11. Nebenläufigkeitsprobleme SOLLEN statisch erkannt werden, soweit möglich.
12. Unsafe Boundaries MÜSSEN explizit identifizierbar sein.
13. Unsafe Code DARF NICHT pauschal von Static Analysis ausgeschlossen werden.
14. Findings MÜSSEN klassifizierbar sein.
15. Kritische Findings MÜSSEN Builds blockieren können.
16. Suppressions MÜSSEN explizit und nachvollziehbar sein.
17. Suppression DARF NICHT als Beweis für Sicherheit gelten.
18. Verified-Core-Builds SOLLEN strengere Analyseprofile verwenden können.
19. Inkrementelle Analyse MUSS unterstützt werden können.
20. Release-Verifikation MUSS vollständige Reanalyse verlangen können.
21. Nicht statisch entscheidbare Eigenschaften SOLLEN an Runtime Verification übergeben werden können.
22. Analyseergebnisse MÜSSEN versioniert und reproduzierbar sein.
23. Änderungen an Code oder Rulesets MÜSSEN Reanalyse auslösen können.
24. Static-Analysis-Ergebnisse MÜSSEN mit konkreten Build-Versionen verknüpfbar sein.
25. Static-Analysis-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-VERIFY-MEMORYSAFETY-0001`
- `NPSPEC-VERIFY-TYPESAFETY-0001`
- `NPSPEC-VERIFY-CAPABILITYSAFETY-0001`
- `NPSPEC-VERIFY-TEMPORAL-0001`
- `NPSPEC-VERIFY-INFORMATIONFLOW-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-FFI-0001`
- `ADR-VERIFY-0008`

## Ergebnis

```text
Source / IR
     ↓
Static Analysis
     ↓
Dataflow + Control Flow
     ↓
Memory + Type + Capability + Flow Checks
     ↓
Findings
├── Proven Safe Property
├── Violation → Reject / Fix
└── Unknown → Runtime / Formal Verification
     ↓
Versioned Verification Artifact
```

NovaOS erhält damit eine systematische Static-Analysis-Schicht, die sicherheitskritische Fehler bereits vor der Ausführung erkennt, unsichere Grenzen sichtbar macht und nicht statisch entscheidbare Eigenschaften gezielt an formale oder laufzeitbasierte Verifikationsmechanismen weitergibt.