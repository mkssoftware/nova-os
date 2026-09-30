# NPSPEC-SEMANTIC-EXECUTION-0001 – Nova Semantic Execution

## Status

Angenommen

## Kategorie

Semantic / Execution / Architecture

## Zweck

NovaOS definiert eine semantische Ausführungsschicht, bei der eine gewünschte Operation über ihre Bedeutung, Ein- und Ausgaben sowie Anforderungen beschrieben wird, ohne eine konkrete Anwendung, Implementierung oder Hardware fest vorzugeben.

```text
Intent
  ↓
Semantic Operation
  ↓
ExecutionContract
  ↓
Discovery + Resolution
  ↓
Capability + Provider + Resources
  ↓
Execution
  ↓
Semantic Result
```

## Grundprinzipien

```text
Semantic Operation ≠ Implementation
Operation ≠ Application
Execution Intent ≠ Authority
Provider Selection ≠ Authorization
Resource Selection ≠ Authority
Semantic Compatibility ≠ Validation
Successful Execution ≠ Valid Result
Location Transparency ≠ Authority Transparency
```

## Semantic Execution Request

Eine Ausführung beschreibt mindestens:

```text
SemanticExecution
├── OperationID
├── Input Semantic Types
├── Expected Output Semantic Types
└── ExecutionContract
```

Optional:

```text
Capability Requirements
Resource Requirements
Quality Requirements
Precision
Deadline
Determinism
Trust Requirements
Sovereignty Requirements
Location Constraints
Transaction Context
```

## Semantische Operation

`OperationID` beschreibt die gewünschte Wirkung.

Beispiele:

```text
Document.Render
Image.Resize
Audio.Transcribe
Data.Analyze
Archive.Extract
Model.Execute
```

Die Operation ist unabhängig davon, welcher Provider sie implementiert.

## Execution Resolution

NovaOS löst die abstrakte Operation in eine ausführbare Konfiguration auf.

```text
Semantic Operation
        ↓
Semantic Discovery
        ↓
Compatibility
        ↓
Capability Resolution
        ↓
Resource Resolution
        ↓
Provider Selection
        ↓
Executable Plan
```

## ExecutionContract

Der `Nova.ExecutionContract` definiert die verbindlichen Anforderungen.

```text
ExecutionContract
├── Hard Requirements
├── Soft Preferences
├── Resource Budget
├── Deadline
├── Determinism
├── Trust
├── Sovereignty
└── Security Context
```

Hard Requirements dürfen während der Auflösung nicht abgeschwächt werden.

## Capability Resolution

Die Ausführung darf nur mit expliziter Autorität erfolgen.

```text
Required Operation
       ↓
Required Capabilities
       ↓
Authorization
       ↓
Attenuated Capability Set
```

Es gilt:

```text
Semantic Execution ≠ Capability Grant
```

Fehlende Autorität darf nicht durch die semantische Ausführungsschicht erzeugt werden.

## Resource Resolution

Benötigte Ressourcen werden semantisch aufgelöst.

```text
Compute Requirement
        ↓
CPU / GPU / Accelerator / Remote Compute
```

Die konkrete Ressource kann gewählt werden anhand von:

```text
Performance
Latency
Energy
Availability
Locality
Trust
Sovereignty
Resource Budget
```

## Provider Selection

Mehrere Provider können dieselbe Operation implementieren.

```text
Document.Render
├── Provider A
├── Provider B
└── Provider C
```

Die Auswahl folgt:

```text
Safety
→ Security
→ Sovereignty / Trust
→ Hard System Constraints
→ Explicit User Decisions
→ Soft Preferences
→ Adaptive Optimization
```

## Conversion

Sind Ein- oder Ausgaben nicht direkt kompatibel, kann ein Conversion Path Bestandteil des Execution Plans werden.

```text
Input
 ↓
Conversion
 ↓
Compatible Input
 ↓
Operation
 ↓
Conversion
 ↓
Requested Output
```

Verlustbehaftete Konvertierungen müssen mit dem ExecutionContract vereinbar sein.

## Execution Plan

Die Auflösung erzeugt einen expliziten Plan.

```text
ExecutionPlan
├── Operation
├── Provider
├── Capabilities
├── Resources
├── Conversions
├── Data Flow
└── Constraints
```

Der Plan muss vor der Ausführung validierbar sein.

## Transaktionen

Mehrstufige semantische Operationen können Transaktionen verwenden.

```text
Begin
 ↓
Validate
 ↓
Prepare
 ↓
Execute
 ↓
Commit
 ↓
Verify
```

Fehler können abhängig von der Operationssemantik Rollback oder kontrollierte Kompensation auslösen.

## Structured Concurrency

Unteroperationen sollen einer gemeinsamen Task-Hierarchie folgen.

```text
Semantic Execution
└── Task Group
    ├── Conversion
    ├── Provider Operation
    └── Validation
```

Cancellation und Deadlines propagieren kontrolliert durch die Hierarchie.

## Ergebnisvalidierung

Ein erfolgreich beendeter Provider bedeutet nicht automatisch ein semantisch gültiges Ergebnis.

```text
Provider Result
      ↓
Semantic Validation
      ↓
Valid / Invalid / Unknown
```

Erst danach darf das Ergebnis gemäß Contract weiterverwendet werden.

## Dynamische Re-Resolution

Ändern sich relevante Bedingungen:

```text
Provider Failure
Resource Loss
Capability Revocation
Trust Change
Deadline Risk
```

kann NovaOS eine erneute Auflösung durchführen.

```text
Current Plan
    ↓
Re-Evaluation
    ↓
Alternative Plan
```

Hard Requirements und Autoritätsgrenzen bleiben erhalten.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ExecutionID
OperationID
ExecutionContract
Input / Output Types
Execution Plan
Provider
Capabilities
Resources
Conversions
Execution State
Validation State
```

## Normative Anforderungen

1. NovaOS MUSS Operationen semantisch und implementierungsunabhängig beschreiben können.
2. Semantic Execution MUSS `Nova.ExecutionContract` integrieren.
3. Semantic Execution DARF keine Autorität erzeugen.
4. Capability-, Resource- und Provider-Auflösung MÜSSEN getrennt kontrollierbar bleiben.
5. Hard Requirements DÜRFEN während der Auflösung NICHT abgeschwächt werden.
6. Conversion Paths MÜSSEN expliziter Bestandteil des Execution Plans sein.
7. Execution Plans MÜSSEN vor Ausführung validierbar sein.
8. Mehrstufige Ausführungen SOLLEN Structured Concurrency und Transactions unterstützen.
9. Ergebnisse SOLLEN semantisch validiert werden.
10. Semantic Execution MUSS introspektierbar und bei relevanten Zustandsänderungen neu auflösbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-IPC-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-SEMANTIC-VALIDATION-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0020`

## Ergebnis

```text
Semantic Intent
      ↓
Semantic Operation
      ↓
ExecutionContract
      ↓
Discovery + Compatibility
      ↓
Capabilities + Resources + Provider
      ↓
Validated Execution Plan
      ↓
Execution
      ↓
Semantic Validation
      ↓
Semantic Result
```

NovaOS erhält damit eine semantische Ausführungsschicht, bei der Software primär beschreibt, **was getan werden soll und welche Anforderungen gelten**, während das System kontrolliert bestimmt, **wie, wo und durch welchen Provider** die Operation ausgeführt wird.