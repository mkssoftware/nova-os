# ADR-EXECUTION-0001 – Nova.ExecutionContract

## Status

Angenommen

## Kategorie

Execution Architecture / Contracts

## Kontext

NovaOS trennt fachliche Anforderungen von der konkreten Ausführung.

Eine Capability kann durch unterschiedliche:

```text
Algorithmen
Compute Provider
Hardware
Standorte
Implementierungen
```

ausgeführt werden.

Damit NovaOS automatisch planen kann, benötigt jede Ausführung einen gemeinsamen Vertrag, der beschreibt, **welche Eigenschaften eingehalten werden müssen**, ohne den konkreten Ausführungsweg festzulegen.

## Entscheidung

NovaOS führt den **`Nova.ExecutionContract`** als systemweiten Ausführungsvertrag ein.

```text
Intent / Capability Request
           ↓
Nova.ExecutionContract
           ↓
Execution Planning
           ↓
Algorithm Selection
           ↓
Provider Selection
           ↓
Execution
           ↓
Verification
```

Der Execution Contract beschreibt Anforderungen und Constraints einer Ausführung.

Er beschreibt nicht die konkrete Implementierung.

## Kernmodell

Ein Execution Contract kann insbesondere enthalten:

```text
Semantic Types
Correctness Requirements
Determinism Requirement
Latency / Deadline
Resource Budget
Energy Budget
Data Sovereignty Policy
Trust Requirement
Locality Constraints
Required Hardware
Preferred / Required Algorithm
Preferred / Required Provider
```

Nicht jede Ausführung muss alle Felder verwenden.

## Hard und Soft Constraints

Der Contract unterscheidet zwischen verbindlichen und optimierbaren Anforderungen.

```text
Hard Constraints
    ↓
Valid Execution Space
    ↓
Soft Constraints
    ↓
Optimization
```

### Hard Constraints

Beispiele:

```text
Safety
Security
Data Sovereignty
Hard Realtime
Required Determinism
Correctness
Required Precision
Required Trust
Required Hardware
Required Algorithm
Required Provider
```

Eine Ausführung darf nicht stattfinden, wenn ein Hard Constraint nicht erfüllt werden kann.

### Soft Constraints

Beispiele:

```text
Performance
Energy
Preferred Latency
Preferred Algorithm
Preferred Provider
Data Locality Preference
Adaptive Policy
User Preference
```

Soft Constraints bestimmen die Optimierung innerhalb des gültigen Lösungsraums.

## Prioritätsordnung

NovaOS verwendet für konkurrierende Anforderungen folgende systemweite Grundordnung:

```text
Safety
  ↓
Security
  ↓
Data Sovereignty
  ↓
Hard Realtime
  ↓
Determinism
  ↓
Correctness
  ↓
Explicit User Constraints
  ↓
Resource Budget
  ↓
Performance
  ↓
Energy
  ↓
Adaptive Policy
  ↓
Preference
```

Niedrigere Ebenen dürfen höhere Ebenen nicht verletzen.

## Semantic Types

Execution Contracts verwenden Semantic Types für Ein- und Ausgaben.

```text
Typed Input
     ↓
Execution Contract
     ↓
Capability
     ↓
Typed Output
```

Primitive Datentypen allein reichen nicht aus, wenn die fachliche Bedeutung für die Ausführung relevant ist.

## Algorithmuswahl

Der Contract begrenzt die zulässigen Algorithmen.

```text
Algorithm Family
       ↓
Execution Contract
       ↓
Valid Algorithms
       ↓
Algorithm Selection
```

Automatische Algorithmusauswahl darf ausschließlich innerhalb dieser Menge erfolgen.

## Providerwahl

Nach oder gemeinsam mit der Algorithmuswahl werden geeignete Compute Provider bestimmt.

```text
Algorithm
    +
Execution Contract
    ↓
Valid Providers
    ↓
CPU / GPU / NPU / DSP / FPGA / Remote
```

Provider müssen alle für sie relevanten Hard Constraints erfüllen.

## Ressourcen

Der Contract kann Ressourcenbudgets definieren.

Beispiele:

```text
CPU Time
Memory
Storage
Network
Energy
Accelerator Time
```

Ressourcenbudgets sind Bestandteil der systemweiten Resource Economy.

## Locality

Der Contract kann Ausführungsorte begrenzen.

Beispiele:

```text
LocalOnly
Specific Device
Trusted Cluster
Required Region
Data Locality
```

Location Transparency gilt nur innerhalb dieser Grenzen.

## Trust und Data Sovereignty

Remote oder verteilte Ausführung muss zusätzliche Anforderungen erfüllen.

```text
Execution Contract
       ↓
Trust
       ↓
Data Sovereignty
       ↓
Authorization
       ↓
Valid Execution Location
```

Performance darf diese Bedingungen niemals überschreiben.

## Determinismus

Der Contract kann unterschiedliche Determinismusanforderungen ausdrücken.

```text
Determinism = Required
```

Dann müssen sowohl:

```text
Algorithm Selection
Provider Selection
Execution
Result
```

die geforderte Reproduzierbarkeit unterstützen.

## Adaptive Optimierung

NovaOS darf Messwerte und Prediction Error zur Optimierung verwenden.

```text
Prediction
    ↓
Execution
    ↓
Actual Result
    ↓
Prediction Error
    ↓
Model Adjustment
```

Adaptive Entscheidungen dürfen ausschließlich Soft Constraints beeinflussen.

Hard Constraints bleiben unveränderlich.

## Contract Validation

Vor der Ausführung muss NovaOS prüfen, ob ein gültiger Ausführungsplan existiert.

```text
Execution Contract
       ↓
Constraint Solving
       ↓
Valid Plan?
 ├── Yes → Execute
 └── No  → Unsatisfied Contract
```

NovaOS darf Anforderungen nicht stillschweigend reduzieren.

## Contract Verification

Eine erfolgreiche Ausführung bedeutet nicht automatisch, dass der Contract erfüllt wurde.

```text
Execution
    ↓
Result
    ↓
Contract Verification
    ↓
Valid Result
```

Wo relevant müssen zugesicherte Eigenschaften nach der Ausführung überprüft werden.

## Introspection

Execution Contracts und Auswahlentscheidungen müssen nachvollziehbar sein.

NovaOS soll beantworten können:

```text
Welche Constraints galten?
Welcher Algorithmus wurde gewählt?
Welcher Provider wurde gewählt?
Warum wurde dieser Provider gewählt?
Welche Kandidaten wurden ausgeschlossen?
Welcher Constraint verhinderte eine Ausführung?
```

## Normative Anforderungen

1. NovaOS MUSS `Nova.ExecutionContract` als gemeinsamen Ausführungsvertrag verwenden.
2. Execution Contracts MÜSSEN Hard und Soft Constraints unterscheiden können.
3. Hard Constraints MÜSSEN vor Optimierungszielen ausgewertet werden.
4. Hard Constraints DÜRFEN nicht automatisch abgeschwächt werden.
5. Semantic Types MÜSSEN als Ein- und Ausgabeanforderungen verwendbar sein.
6. Algorithmusauswahl MUSS den Execution Contract berücksichtigen.
7. Providerwahl MUSS den Execution Contract berücksichtigen.
8. Ressourcenbudgets MÜSSEN ausdrückbar sein.
9. Determinismus-, Latenz-, Trust-, Sovereignty- und Locality-Anforderungen MÜSSEN ausdrückbar sein.
10. Preferred und Required Algorithm MÜSSEN unterscheidbar sein.
11. Preferred und Required Provider MÜSSEN unterscheidbar sein.
12. Adaptive Optimierung DARF ausschließlich innerhalb gültiger Lösungen arbeiten.
13. Ist kein gültiger Plan möglich, MUSS der Contract als nicht erfüllbar gemeldet werden.
14. Relevante Contract- und Selection-Informationen MÜSSEN introspektierbar sein.
15. Ergebnisse MÜSSEN dort verifiziert werden, wo der Contract eine überprüfbare Zusicherung verlangt.

## Konsequenzen

### Positive Konsequenzen

- ein gemeinsames Contract-Modell für systemweite Ausführung,
- Algorithmus-, Provider- und Standortwahl können gemeinsam geplant werden,
- Sicherheit und Korrektheit bleiben von Optimierungen getrennt,
- Hardware und Ausführungsort bleiben weitgehend transparent,
- Entscheidungen werden nachvollziehbar und reproduzierbar.

### Negative Konsequenzen

- Execution Contracts müssen präzise definiert werden,
- Constraint Solving erhöht den Planungsaufwand,
- Provider und Algorithmen müssen ihre Eigenschaften zuverlässig deklarieren.

## Abhängigkeiten

- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-DECLARATIVE-0008_Intent_Based_APIs`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0008_Algorithm_Registry`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-CONSTRAINT-0001`
- `NPSPEC-EXECUTION-CONTRACT-VALIDATION-0001`
- `NPSPEC-EXECUTION-CONTRACT-VERIFICATION-0001`
- `NPSPEC-EXECUTION-PLANNING-0001`

## Ergebnis

`Nova.ExecutionContract` bildet die gemeinsame Grundlage der NovaOS-Ausführungsplanung:

```text
Intent
  ↓
Capability
  ↓
Nova.ExecutionContract
  ↓
Constraint Solving
  ↓
Algorithm
  +
Compute Provider
  +
Execution Location
  ↓
Execution
  ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
Der Consumer beschreibt nicht,
wie und wo etwas ausgeführt wird.

Er beschreibt,
welches Ergebnis benötigt wird
und welche Bedingungen gelten.

NovaOS bestimmt innerhalb
dieser Grenzen den Ausführungsweg.
```