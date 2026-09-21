# ADR-ALGORITHM-0005 – Data-Dependent Algorithm Selection

## Status

Angenommen

## Kategorie

Algorithm Architecture / Algorithm Selection

## Kontext

Die Eignung eines Algorithmus hängt nicht nur von Hardware, Ressourcen und Execution Contracts ab, sondern häufig auch von den Eigenschaften der konkreten Eingabedaten.

Beispiele:

```text
Image Processing:
    Resolution
    Color Depth
    Noise Level

Compression:
    Data Size
    Entropy
    Data Structure

Sorting:
    Element Count
    Existing Order
    Distribution

Simulation:
    Model Size
    Sparsity
    Precision
```

Ein Algorithmus kann für kleine oder strukturierte Daten optimal sein, bei großen oder anders verteilten Daten jedoch deutlich schlechter abschneiden.

## Entscheidung

NovaOS unterstützt **Data-Dependent Algorithm Selection**.

Die automatische Algorithmusauswahl darf relevante Eigenschaften der konkreten Eingabedaten berücksichtigen.

```text
Input Objects
      ↓
Semantic + Structural Properties
      ↓
Algorithm Family
      ↓
Valid Algorithms
      ↓
Data-Dependent Evaluation
      ↓
Selected Algorithm
```

Die Analyse der Daten dient ausschließlich der Auswahl innerhalb der durch den Execution Contract erlaubten Algorithmen.

## Data Characteristics

Für die Auswahl können unter anderem berücksichtigt werden:

```text
Size
Dimensions
Structure
Distribution
Sparsity
Entropy
Precision
Semantic Type
Data Locality
```

Welche Eigenschaften relevant sind, wird durch die jeweilige Algorithm Family definiert.

## Semantic Metadata

Bereits vorhandene semantische Metadaten sollen bevorzugt verwendet werden.

```text
Object
 ├── Semantic Type
 ├── Metadata
 └── Structural Properties
        ↓
Algorithm Selection
```

Dadurch sollen unnötige vollständige Datenanalysen vermieden werden.

## Selection Model

Die Algorithmusbewertung kann von den Eingabeeigenschaften abhängen.

```text
Algorithm A:
    efficient for small datasets

Algorithm B:
    efficient for sparse datasets

Algorithm C:
    efficient for large parallel workloads
```

Die Auswahl erfolgt anhand des tatsächlichen Workloads statt ausschließlich anhand statischer Algorithmuseigenschaften.

## Analyseaufwand

Die Analyse der Eingangsdaten verursacht selbst Kosten.

```text
Analysis Cost
      +
Expected Selection Benefit
      ↓
Worth Analyzing?
```

NovaOS soll keine aufwendige Datenanalyse durchführen, wenn deren erwarteter Nutzen geringer als ihre Kosten ist.

## Sampling

Bei großen Datenmengen darf NovaOS Stichproben verwenden, sofern dies für die jeweilige Auswahlsemantik zulässig ist.

```text
Large Dataset
      ↓
Representative Sample
      ↓
Data Characteristics
      ↓
Algorithm Selection
```

Unsicherheit aus Sampling muss bei der Auswahl berücksichtigt werden können.

## Algorithmus und Provider

Data-Dependent Selection kann gemeinsam mit der Compute-Providerwahl erfolgen.

```text
Input Characteristics
        +
Algorithms
        +
Compute Providers
        ↓
Valid Combinations
        ↓
Cost Evaluation
        ↓
Algorithm + Provider
```

Dadurch kann dieselbe Datenstruktur zu unterschiedlichen Algorithmus-Provider-Kombinationen führen.

## Dynamische Umschaltung

Bei segmentierbaren oder langfristigen Workloads darf NovaOS den Algorithmus während der Verarbeitung wechseln, wenn die Operationssemantik dies erlaubt.

```text
Data Segment A → Algorithm A
Data Segment B → Algorithm B
Data Segment C → Algorithm A
```

Ein Wechsel darf Ergebnissemantik, Determinismus oder Konsistenz nicht verletzen.

## Datenschutz

Die Analyse von Daten zur Algorithmusauswahl unterliegt denselben Sicherheits- und Datenschutzregeln wie die eigentliche Verarbeitung.

```text
Input Data
    ↓
Allowed Metadata / Analysis
    ↓
Algorithm Selection
```

Data-Dependent Selection darf keine geschützten Daten an unberechtigte Komponenten oder Remote Provider offenlegen.

## Lernen aus Ausführungen

NovaOS darf gemessene Zusammenhänge zwischen Dateneigenschaften und Algorithmusleistung verwenden.

```text
Data Characteristics
       ↓
Prediction
       ↓
Algorithm Selection
       ↓
Execution
       ↓
Measured Result
       ↓
Prediction Error
       ↓
Model Adjustment
```

Adaptive Modelle dürfen nur innerhalb zulässiger Kandidaten optimieren.

## Deterministic Mode

Im Deterministic Mode müssen relevante Dateneigenschaften und Auswahlregeln zu einer reproduzierbaren Entscheidung führen.

```text
Same Data
+
Same Contract
+
Same Relevant State
      ↓
Same Algorithm Selection
```

## Normative Anforderungen

1. NovaOS MUSS dateneigenschaftsabhängige Algorithmusauswahl unterstützen können.
2. Relevante Dateneigenschaften MÜSSEN durch Algorithm Families beschreibbar sein.
3. Vorhandene semantische Metadaten SOLLEN gegenüber erneuter Datenanalyse bevorzugt werden.
4. Analyseaufwand MUSS als Teil der Auswahlkosten berücksichtigt werden können.
5. Sampling DARF verwendet werden, wenn die Algorithmussemantik dies erlaubt.
6. Unsicherheit aus Analyse oder Sampling MUSS darstellbar sein.
7. Data-Dependent Selection DARF nur innerhalb semantisch und vertraglich gültiger Algorithmen arbeiten.
8. Algorithmus- und Providerwahl SOLLEN gemeinsam optimierbar sein.
9. Dynamische Algorithmuswechsel DÜRFEN nur bei geeigneter Operationssemantik erfolgen.
10. Sicherheits-, Privacy- und Data-Sovereignty-Constraints MÜSSEN auch für die Datenanalyse gelten.
11. Adaptive Modelle DÜRFEN Hard Constraints nicht verändern.
12. Deterministic Mode MUSS reproduzierbare datenabhängige Auswahl ermöglichen.

## Konsequenzen

### Positive Konsequenzen

- Algorithmen können passend zu tatsächlichen Daten gewählt werden,
- Performance und Ressourcenverbrauch können verbessert werden,
- unterschiedliche Datenstrukturen können unterschiedliche Optimierungen nutzen,
- Algorithmus- und Providerwahl werden workload-spezifisch.

### Negative Konsequenzen

- Datenanalyse verursacht zusätzlichen Aufwand,
- Schätzungen und Sampling können Unsicherheit erzeugen,
- Auswahlmodelle werden komplexer.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-SEMANTIC-0007_Semantic_Metadata`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-DATA-DEPENDENT-SELECTION-0001`
- `NPSPEC-ALGORITHM-DATA-CHARACTERISTICS-0001`
- `NPSPEC-ALGORITHM-DATA-SAMPLING-0001`
- `NPSPEC-ALGORITHM-DATA-SELECTION-MODEL-0001`

## Ergebnis

NovaOS betrachtet bei der Algorithmusauswahl auch die tatsächlichen Eigenschaften des Workloads:

```text
Input Data
    ↓
Semantic + Structural Properties
    ↓
Valid Algorithms
    ↓
Data-Dependent Models
    ↓
Algorithm + Provider Selection
    ↓
Execution
    ↓
Measurement
```

Der zentrale Grundsatz lautet:

```text
Der beste Algorithmus hängt
nicht nur von der Hardware ab.

Er hängt auch davon ab,
welche Daten tatsächlich
verarbeitet werden.
```