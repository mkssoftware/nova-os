# ADR-ALGORITHM-0002 – Automatische Algorithmusauswahl als Standard

## Status

Angenommen

## Kategorie

Algorithm Architecture / Algorithm Selection

## Kontext

Eine Algorithm Family kann mehrere semantisch kompatible Algorithmen enthalten.

```text
Algorithm Family
    ├── Algorithm A
    ├── Algorithm B
    ├── Algorithm C
    └── Algorithm D
```

Diese können sich unter anderem bei Performance, Speicherbedarf, Energieverbrauch, Latenz, Präzision und geeigneter Hardware unterscheiden.

Der Consumer soll diese technischen Unterschiede normalerweise nicht selbst verwalten müssen.

## Entscheidung

NovaOS verwendet die **automatische Algorithmusauswahl als Standardverhalten**.

```text
Capability Request
       ↓
Algorithm Family
       ↓
Execution Contract
       ↓
Valid Algorithms
       ↓
Automatic Selection
       ↓
Selected Algorithm
```

Der Consumer beschreibt primär das gewünschte Ergebnis und seine Anforderungen.

NovaOS bestimmt daraus einen geeigneten Algorithmus.

## Auswahlprozess

Die Auswahl erfolgt mehrstufig:

```text
Algorithm Family
       ↓
Semantic Compatibility
       ↓
Hard Constraints
       ↓
Valid Algorithms
       ↓
Soft Optimization
       ↓
Selected Algorithm
```

Nur Algorithmen, welche die geforderte Semantik erfüllen, dürfen berücksichtigt werden.

## Hard Constraints

Verbindliche Anforderungen werden vor jeder Optimierung geprüft.

Beispiele:

```text
Correctness
Required Precision
Determinism
Safety
Security
Hard Realtime
Required Algorithm
Required Hardware Properties
```

Ein Algorithmus, der einen Hard Constraint nicht erfüllt, wird ausgeschlossen.

## Soft Constraints

Innerhalb der gültigen Kandidatenmenge kann NovaOS optimieren nach:

```text
Performance
Latency
Energy
Memory Usage
Data Locality
Compute Cost
Quality Preference
Preferred Algorithm
```

Soft Constraints dürfen Hard Constraints nicht abschwächen.

## Gemeinsame Algorithmus- und Providerwahl

Algorithmus und Compute Provider werden getrennt modelliert, können aber gemeinsam geplant werden.

```text
Algorithm Candidates
        +
Compute Providers
        ↓
Valid Combinations
        ↓
Cost Evaluation
        ↓
Algorithm + Provider
```

Dadurch kann beispielsweise ein GPU-optimierter Algorithmus gegen einen effizienteren CPU-Algorithmus bewertet werden.

## Aktueller Systemzustand

Die automatische Auswahl darf dynamische Zustände berücksichtigen.

```text
Available Memory
Provider Load
Energy State
Thermal State
Data Location
Network State
```

Daher muss nicht bei jeder Ausführung derselbe Algorithmus gewählt werden.

## Adaptive Auswahl

NovaOS darf historische Messwerte zur Verbesserung zukünftiger Entscheidungen verwenden.

```text
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

Adaptive Auswahl beeinflusst ausschließlich zulässige Kandidaten.

Sie darf keine Hard Constraints verändern.

## Deterministic Mode

Im Deterministic Mode muss die Algorithmusauswahl reproduzierbar sein.

```text
Same Input
+
Same Contract
+
Same Relevant State
      ↓
Same Selection
```

Nicht deterministische adaptive Entscheidungen dürfen in diesem Modus nicht die reproduzierbare Auswahl beeinflussen.

## Explizite Auswahl

Automatische Auswahl ist der Standard, aber keine Verpflichtung für jeden Anwendungsfall.

Ein Contract kann enthalten:

```text
Preferred Algorithm
Required Algorithm
```

`Preferred Algorithm` ist eine Soft Constraint.

`Required Algorithm` ist eine Hard Constraint.

## Keine gültige Auswahl

Existiert kein Algorithmus, der den Execution Contract erfüllt, darf NovaOS keinen ungeeigneten Algorithmus verwenden.

```text
Algorithm Candidates
       ↓
Contract Filtering
       ↓
∅
       ↓
Unsatisfied Contract
```

Die Anforderungen dürfen nicht stillschweigend reduziert werden.

## Normative Anforderungen

1. NovaOS MUSS automatische Algorithmusauswahl als Standard unterstützen.
2. Consumer SOLLEN normalerweise keinen konkreten Algorithmus auswählen müssen.
3. Die Auswahl MUSS innerhalb einer passenden Algorithm Family erfolgen.
4. Semantische Kompatibilität MUSS vor Optimierung geprüft werden.
5. Hard Constraints MÜSSEN vor Soft Constraints ausgewertet werden.
6. Ungeeignete Algorithmen DÜRFEN nicht aufgrund besserer Performance gewählt werden.
7. Algorithmus- und Providerwahl MÜSSEN getrennt modelliert, aber gemeinsam optimierbar sein.
8. Dynamische Ressourcen- und Hardwarezustände DÜRFEN berücksichtigt werden.
9. Adaptive Auswahl DARF nur innerhalb gültiger Kandidaten arbeiten.
10. Deterministic Mode MUSS eine reproduzierbare Algorithmusauswahl ermöglichen.
11. `Preferred Algorithm` MUSS als Soft Constraint behandelt werden.
12. `Required Algorithm` MUSS als Hard Constraint behandelt werden.
13. Ist kein Algorithmus geeignet, MUSS der Contract als nicht erfüllbar gemeldet werden.

## Konsequenzen

### Positive Konsequenzen

- Consumer bleiben von konkreten Algorithmen entkoppelt,
- NovaOS kann Algorithmen passend zu Hardware und Systemzustand wählen,
- neue Algorithmen können automatisch genutzt werden,
- Algorithmus- und Providerwahl können gemeinsam optimiert werden.

### Negative Konsequenzen

- Algorithmen benötigen vergleichbare Eigenschaften und Contracts,
- die Runtime benötigt zuverlässige Kosten- und Leistungsinformationen.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-SELECTION-0001`
- `NPSPEC-ALGORITHM-SELECTION-CONSTRAINTS-0001`
- `NPSPEC-ALGORITHM-COST-MODEL-0001`
- `NPSPEC-ALGORITHM-ADAPTIVE-SELECTION-0001`

## Ergebnis

NovaOS behandelt die Auswahl des konkreten Algorithmus als Aufgabe der Ausführungsplanung:

```text
Capability
    ↓
Algorithm Family
    ↓
Execution Contract
    ↓
Valid Algorithms
    ↓
Algorithm + Provider Planning
    ↓
Execution
    ↓
Measured Result
```

Der zentrale Grundsatz lautet:

```text
Der Consumer beschreibt,
was erreicht werden soll
und welche Anforderungen gelten.

NovaOS entscheidet standardmäßig,
welcher Algorithmus dafür
am besten geeignet ist.
```