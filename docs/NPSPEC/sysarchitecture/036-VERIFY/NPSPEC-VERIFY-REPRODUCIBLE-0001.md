# NPSPEC-VERIFY-REPRODUCIBLE-0001 – Nova Reproducible Verification

## Status

Angenommen

## Kategorie

Verification / Reproducibility / Verified Core

## Zweck

NovaOS definiert reproduzierbare Verifikation als Voraussetzung dafür, dass ein Verifikationsergebnis eindeutig einer Spezifikation, Implementierung, Toolchain, Konfiguration und einem Build zugeordnet und unabhängig erneut geprüft werden kann.

```text
Specification
     +
Source
     +
Toolchain
     +
Verification Configuration
     ↓
Verification
     ↓
Reproducible Result
```

## Grundprinzipien

```text
Reproducible Build ≠ Reproducible Verification
Same Source ≠ Same Verification Environment
Same Tool ≠ Same Tool Version
Same Result ≠ Same Evidence
Verification Passed ≠ Verification Reproducible
Cached Result ≠ Current Result
Old Proof ≠ Valid Proof for New Build
```

## Verification Identity

Jeder relevante Verifikationslauf erhält eine eindeutige Identität.

```text
VerificationRun
├── VerificationID
├── SpecificationID
├── SpecificationVersion
├── SourceVersion
├── BuildID
├── ToolchainID
├── ConfigurationID
└── Result
```

Optional:

```text
ModelVersion
ContractVersion
CompilerVersion
AnalyzerVersion
SolverVersion
TargetArchitecture
Dependencies
Environment
Assumptions
ProofArtifacts
Timestamp
ProvenanceID
```

## Deterministische Eingaben

Alle für das Ergebnis relevanten Eingaben müssen identifizierbar sein.

```text
Verification Inputs
├── Specification
├── Source
├── Models
├── Contracts
├── Rulesets
├── Dependencies
├── Tool Versions
├── Configuration
└── Target Definition
```

Versteckte oder nicht dokumentierte Eingaben müssen vermieden werden.

## Toolchain

Die verwendete Verification Toolchain muss versioniert werden.

Beispiele:

```text
Compiler
Static Analyzer
Model Checker
Theorem Prover
SMT Solver
Runtime Contract Generator
IR Tooling
Build System
```

```text
Tool Name ≠ Tool Version
```

## Environment

Falls das Verification Environment das Ergebnis beeinflussen kann, muss es beschrieben werden.

Beispiele:

```text
Architecture
Compiler Flags
Feature Flags
Verification Profile
Environment Variables
Solver Configuration
Resource Limits
```

Nicht relevante Umgebungsdetails müssen nicht Bestandteil der Verification Identity sein.

## Verification Configuration

Verifikationsparameter müssen reproduzierbar gespeichert werden.

Beispiele:

```text
Checked Properties
Model Bounds
Timeouts
State Reductions
Solver Options
Ruleset
Suppressions
Assumptions
Verification Profile
```

Insbesondere begrenzte Prüfungen müssen ihren Bound dokumentieren.

## Verification Artifacts

Ein Verifikationslauf kann erzeugen:

```text
Proof
Counterexample
Static Analysis Report
Model Checking Result
Contract Result
Runtime Verification Metadata
Solver Certificate
Verification Log
```

Artefakte müssen eindeutig dem VerificationRun zugeordnet werden können.

## Content Identification

Relevante Artefakte sollen über kryptografische Content IDs identifizierbar sein.

```text
Artifact
   ↓
Cryptographic Hash
   ↓
ContentID
```

Dadurch können unbeabsichtigte Veränderungen erkannt werden.

## Reproduction

Ein VerificationRun soll rekonstruierbar sein:

```text
VerificationID
      ↓
Resolve Inputs
      ↓
Restore Toolchain
      ↓
Restore Configuration
      ↓
Execute Verification
      ↓
Compare Results
```

## Result Comparison

Reproduzierbarkeit kann unterschiedliche Ebenen besitzen:

```text
Semantic Reproduction
Artifact Reproduction
Bit-for-Bit Reproduction
```

Nicht jedes Verifikationswerkzeug muss bitidentische Ausgaben erzeugen.

Entscheidend ist, dass das relevante Verifikationsergebnis reproduzierbar vergleichbar ist.

## Build Binding

Verifikationsergebnisse müssen an den tatsächlich geprüften Build gebunden sein.

```text
Source
  ↓
Build
  ↓
BuildID
  ↕
Verification Result
```

```text
Verified Source ≠ Verified Binary
```

Wenn eine Eigenschaft nur auf Source-Ebene geprüft wurde, darf daraus nicht automatisch eine Binary-Verifikation abgeleitet werden.

## Dependency Changes

Änderungen relevanter Abhängigkeiten können Verifikation ungültig machen.

```text
Dependency v1
     ↓
Verified

Dependency v2
     ↓
Reverification Required?
```

NovaOS muss Abhängigkeiten zwischen Verification Artifacts und ihren Inputs nachvollziehen können.

## Cache

Verification Results dürfen gecacht werden.

Ein Cache-Treffer ist nur gültig, wenn alle relevanten Inputs übereinstimmen.

```text
Same Verification Key
        ↓
Cached Result Reusable
```

```text
Partial Match ≠ Valid Verification Cache
```

## Live Evolution

Hot Replacement oder Live Update benötigt eine neue Verification Identity.

```text
Component v1
   ↓
Verification A

Component v2
   ↓
Verification B
```

Alte Evidence darf nur weiterverwendet werden, wenn die betroffene Eigenschaft nachweislich unverändert gültig bleibt.

## Supply Chain

Verification Toolchains und Verification Artifacts müssen in das Trust- und Supply-Chain-Modell integrierbar sein.

```text
Trusted Source
     ↓
Trusted Toolchain
     ↓
Verification
     ↓
Signed / Identified Evidence
```

Ein reproduzierbares Ergebnis ist nicht automatisch vertrauenswürdig, wenn die verwendete Toolchain kompromittiert ist.

## Continuous Verification

NovaOS soll Reverification automatisiert auslösen können bei:

```text
Source Change
Specification Change
Model Change
Contract Change
Dependency Change
Toolchain Change
Ruleset Change
Critical Configuration Change
```

## Provenance

Verification Evidence muss nachvollziehbar sein.

```text
Specification
    ↓
Implementation
    ↓
Toolchain
    ↓
VerificationRun
    ↓
Artifacts
    ↓
Result
```

Diese Beziehung soll über Provenance rekonstruierbar bleiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
VerificationID
Specification Version
Source Version
BuildID
Toolchain
Configuration
Verified Properties
Assumptions
Artifacts
Reproducibility Status
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS sicherheitskritische Verifikation reproduzierbar dokumentieren.
2. Jeder relevante VerificationRun MUSS eindeutig identifizierbar sein.
3. Specification-, Source- und Build-Version MÜSSEN dem Ergebnis zugeordnet werden können.
4. Relevante Tool-Versionen MÜSSEN dokumentiert werden.
5. Verification Configuration MUSS reproduzierbar gespeichert werden.
6. Verwendete Annahmen MÜSSEN explizit dokumentiert werden.
7. Model-Checking-Bounds MÜSSEN Teil der Verification Evidence sein.
8. Relevante Verification Artifacts MÜSSEN ihrem VerificationRun zugeordnet sein.
9. Kritische Artefakte SOLLEN kryptografisch identifizierbar sein.
10. Verified Source DARF NICHT automatisch als Verified Binary interpretiert werden.
11. Verifikationsergebnisse MÜSSEN an den tatsächlich geprüften Build gebunden werden können.
12. Änderungen relevanter Inputs MÜSSEN eine Reverification auslösen können.
13. Verification Cache DARF nur bei übereinstimmenden relevanten Inputs wiederverwendet werden.
14. Ein Cache-Treffer DARF NICHT allein anhand der Source-Version bestimmt werden.
15. Reproduktion MUSS relevante Verification Inputs rekonstruieren können.
16. NovaOS MUSS zwischen semantischer, Artefakt- und Bit-für-Bit-Reproduzierbarkeit unterscheiden können.
17. Bitidentische Tool-Ausgabe DARF NICHT generell vorausgesetzt werden.
18. Live Evolution MUSS Verification Evidence berücksichtigen.
19. Alte Evidence DARF NICHT ungeprüft auf geänderte Komponenten übertragen werden.
20. Verification Toolchains SOLLEN in das Trust-Modell integriert werden.
21. Reproduzierbarkeit DARF NICHT mit Vertrauenswürdigkeit gleichgesetzt werden.
22. Continuous Verification SOLL Änderungen relevanter Verification Inputs erkennen.
23. Verification Provenance MUSS nachvollziehbar sein.
24. Verification Evidence MUSS langfristig einer konkreten Systemversion zugeordnet werden können.
25. Reproducibility Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-VERIFY-STATICANALYSIS-0001`
- `NPSPEC-VERIFY-RUNTIME-0001`
- `NPSPEC-VERIFY-CONTRACT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `ADR-VERIFY-0010`

## Ergebnis

```text
Specification + Source + Build
             ↓
      Toolchain Identity
             ↓
 Verification Configuration
             ↓
         Verification
             ↓
      VerificationID
             ↓
   Evidence + Artifacts
             ↓
     Reproduce Later
             ↓
      Compare Result
```

NovaOS erhält damit eine reproduzierbare Verifikationskette, durch die formale Beweise, Model Checking, Static Analysis, Runtime Contracts und andere Verification Evidence eindeutig einer konkreten Spezifikation, Implementierung, Toolchain und Systemversion zugeordnet und später erneut überprüft werden können.