# ADR-ALGORITHM-0004 – Cost, Performance, Hardware und Energy Models

## Status

Angenommen

## Kategorie

Algorithm Architecture / Execution Planning

## Kontext

Die automatische Algorithmusauswahl benötigt mehr Informationen als die reine semantische Kompatibilität.

Algorithmen können sich erheblich unterscheiden bei:

```text
Execution Cost
Performance
Hardware Suitability
Energy Consumption
Memory Usage
Data Transfer
Startup Overhead
```

Diese Eigenschaften hängen häufig zusätzlich von Eingabedaten, Compute Provider und aktuellem Systemzustand ab.

Ein statisch als „schnell“ klassifizierter Algorithmus ist deshalb nicht unter allen Bedingungen die beste Wahl.

## Entscheidung

NovaOS verwendet **Cost-, Performance-, Hardware- und Energy-Models** zur Bewertung zulässiger Algorithmen.

```text
Valid Algorithms
       ↓
Execution Models
       ↓
Cost
Performance
Hardware
Energy
       ↓
Algorithm + Provider Planning
```

Die Modelle dienen ausschließlich der Auswahl innerhalb der durch den Execution Contract erlaubten Kandidaten.

## Cost Model

Das Cost Model beschreibt den erwarteten Gesamtaufwand einer Ausführung.

Es kann berücksichtigen:

```text
Compute Time
Memory Usage
Startup Cost
Data Transfer
Synchronization
Network Cost
Resource Occupancy
```

Die Kosten werden möglichst für die konkrete Kombination aus Algorithmus, Daten und Provider bestimmt.

## Performance Model

Das Performance Model beschreibt die erwartete Leistungscharakteristik.

```text
Input Size
    +
Algorithm
    +
Provider
    ↓
Expected Performance
```

Relevante Eigenschaften können sein:

```text
Latency
Throughput
Scaling
Parallelism
Startup Overhead
```

Theoretische Spitzenleistung allein ist kein ausreichendes Auswahlkriterium.

## Hardware Model

Das Hardware Model beschreibt, wie gut ein Algorithmus zu einem Compute Provider passt.

Beispiele:

```text
Algorithm A → CPU optimized
Algorithm B → SIMD optimized
Algorithm C → GPU optimized
Algorithm D → NPU optimized
```

Berücksichtigt werden können:

```text
Instruction Sets
Parallelism
Memory Architecture
Cache Topology
Vector Width
Accelerator Features
Precision Support
```

## Energy Model

Das Energy Model schätzt den Energiebedarf einer Algorithmus-Provider-Kombination.

```text
Algorithm
    +
Provider
    +
Workload
    ↓
Estimated Energy Cost
```

Dabei muss nicht die Hardware mit der geringsten Leistungsaufnahme die energieeffizienteste Ausführung liefern.

Entscheidend ist der Energiebedarf der gesamten Operation.

## Kombinierte Bewertung

Die Modelle werden nicht isoliert betrachtet.

```text
Algorithm
    +
Provider
    +
Input Characteristics
    +
Current System State
       ↓
Cost Model
Performance Model
Hardware Model
Energy Model
       ↓
Candidate Evaluation
```

NovaOS kann dadurch Algorithmus und Compute Provider gemeinsam bewerten.

## Messwerte

Modelle dürfen auf statischen Eigenschaften und gemessenen Laufzeitdaten basieren.

```text
Prediction
    ↓
Execution
    ↓
Measurement
    ↓
Prediction Error
    ↓
Model Update
```

Messwerte können beispielsweise enthalten:

```text
Actual Runtime
Memory Consumption
Transfer Volume
Energy Consumption
Queueing Time
```

## Adaptive Modelle

NovaOS darf die Modelle anhand realer Ausführungen verbessern.

Adaptive Modelle beeinflussen jedoch nur Soft-Constraint-Optimierungen.

```text
Hard Constraints
      ↓
Valid Candidates
      ↓
Adaptive Cost Models
      ↓
Optimization
```

Ein gelerntes Modell darf keinen eigentlich unzulässigen Algorithmus zulassen.

## Unsicherheit

Kostenmodelle sind Schätzungen und können unvollständig sein.

NovaOS muss deshalb zwischen bekannten und geschätzten Eigenschaften unterscheiden können.

```text
Measured
Estimated
Unknown
```

Unsicherheit darf bei der Auswahl berücksichtigt werden.

Für Hard Constraints gilt weiterhin:

```text
Unknown ≠ Satisfied
```

## Deterministic Mode

Im Deterministic Mode muss die Verwendung dynamischer Modelle reproduzierbare Entscheidungen ermöglichen.

Nicht deterministische Mess- oder Lernzustände dürfen die Auswahl nicht unkontrolliert verändern.

## Explizite Overrides

Explizite Algorithmus- oder Provider-Overrides haben Vorrang vor Soft-Optimierungen.

```text
Required Algorithm
        oder
Required Provider
        ↓
Contract Validation
        ↓
Models optimize remaining choices
```

Sie dürfen weiterhin keine übergeordneten Hard Constraints verletzen.

## Normative Anforderungen

1. NovaOS MUSS Algorithmen anhand mehrerer Ausführungsmodelle bewerten können.
2. Cost-, Performance-, Hardware- und Energy-Models MÜSSEN getrennt beschreibbar sein.
3. Modelle SOLLEN Algorithmus und Compute Provider gemeinsam berücksichtigen können.
4. Eingabegröße und relevante Dateneigenschaften SOLLEN in Modelle einfließen können.
5. Datenbewegungs- und Startup-Kosten MÜSSEN berücksichtigt werden können.
6. Hardwareeigenschaften MÜSSEN in die Algorithmusbewertung einfließen können.
7. Energieverbrauch MUSS als eigenständige Optimierungsgröße modellierbar sein.
8. Reale Messwerte DÜRFEN zur Verbesserung zukünftiger Schätzungen verwendet werden.
9. Adaptive Modelle DÜRFEN ausschließlich innerhalb gültiger Kandidaten optimieren.
10. Unsicherheit und unbekannte Werte MÜSSEN darstellbar sein.
11. Unbekannte Eigenschaften DÜRFEN nicht als erfüllte Hard Constraints interpretiert werden.
12. Deterministic Mode MUSS reproduzierbare Auswahlentscheidungen ermöglichen.
13. Explizite Overrides MÜSSEN gegenüber modellbasierten Soft-Optimierungen berücksichtigt werden.

## Konsequenzen

### Positive Konsequenzen

- Algorithmuswahl berücksichtigt reale Ausführungskosten,
- Algorithmus und Hardware können gemeinsam optimiert werden,
- Energie, Performance und Datenbewegung werden vergleichbar,
- NovaOS kann aus tatsächlichen Ausführungen lernen,
- neue Hardware kann durch neue Modelle berücksichtigt werden.

### Negative Konsequenzen

- Modelle müssen gepflegt und kalibriert werden,
- Schätzungen können ungenau sein,
- die Ausführungsplanung wird komplexer.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0003_Explizites_Algorithmus_Override`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-COMPUTE-0006_Latency_Aware_Compute`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-COST-MODEL-0001`
- `NPSPEC-ALGORITHM-PERFORMANCE-MODEL-0001`
- `NPSPEC-ALGORITHM-HARDWARE-MODEL-0001`
- `NPSPEC-ALGORITHM-ENERGY-MODEL-0001`
- `NPSPEC-ALGORITHM-MODEL-MEASUREMENT-0001`

## Ergebnis

NovaOS bewertet nicht nur einzelne Algorithmen, sondern die tatsächliche Kombination aus Algorithmus, Workload und Compute Provider:

```text
Algorithm Candidates
        +
Compute Providers
        +
Workload
        ↓
Cost / Performance
Hardware / Energy Models
        ↓
Valid Combinations
        ↓
Optimization
        ↓
Algorithm + Provider
        ↓
Execution
        ↓
Measurement
```

Der zentrale Grundsatz lautet:

```text
Der beste Algorithmus existiert
nicht unabhängig von seinem Kontext.

NovaOS bewertet Algorithmus,
Hardware, Daten und Ressourcen
als gemeinsame Ausführungsentscheidung.
```