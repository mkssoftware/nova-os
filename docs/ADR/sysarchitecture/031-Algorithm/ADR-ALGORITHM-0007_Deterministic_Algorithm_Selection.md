# ADR-ALGORITHM-0007 – Deterministic Algorithm Selection

## Status

Angenommen

## Kategorie

Algorithm Architecture / Determinism

## Kontext

NovaOS kann Algorithmen automatisch anhand von Daten, Hardware, Ressourcen, Profilen und dynamischem Systemzustand auswählen.

Dadurch kann dieselbe Operation zu unterschiedlichen Zeitpunkten unterschiedliche Algorithmen verwenden.

```text
Same Request
    ↓
Different Runtime State
    ↓
Different Algorithm
```

Für reproduzierbare Berechnungen, Tests, Simulationen, wissenschaftliche Anwendungen und Debugging muss NovaOS eine deterministische Algorithmusauswahl ermöglichen.

## Entscheidung

NovaOS unterstützt **Deterministic Algorithm Selection** als Bestandteil des systemweiten Deterministic Mode.

```text
Capability Request
       ↓
Execution Contract
       ↓
Deterministic Selection Required?
       ↓
Deterministic Candidate Set
       ↓
Deterministic Selection Rule
       ↓
Selected Algorithm
```

Bei identischem relevanten Zustand muss dieselbe Auswahlentscheidung reproduzierbar sein.

## Relevanter Zustand

Eine deterministische Auswahl basiert ausschließlich auf definierten Eingaben.

Dazu können gehören:

```text
Capability
Algorithm Family
Input Characteristics
Execution Contract
Algorithm Versions
Provider Capabilities
Selection Policy Version
Fixed Profile State
```

Nicht definierte oder volatile Zustände dürfen die Auswahl nicht beeinflussen.

## Deterministische Kandidatenmenge

Zunächst werden alle Algorithmen entfernt, die den Determinism Contract nicht erfüllen.

```text
Algorithm Family
       ↓
Contract Filtering
       ↓
Deterministic Algorithms
       ↓
Selection
```

Ein deterministisch ausgewählter Algorithmus muss nicht automatisch selbst deterministisch sein.

Beide Eigenschaften werden getrennt geprüft.

## Stabile Auswahlregeln

Die Reihenfolge und Bewertung von Kandidaten muss reproduzierbar definiert sein.

```text
Same Candidates
+
Same Contract
+
Same Selection State
      ↓
Same Algorithm
```

Nicht definierte Iterationsreihenfolgen, zufällige Tie-Breaker oder volatile Messwerte sind im Deterministic Mode unzulässig.

## Tie-Breaking

Sind mehrere Kandidaten gleichwertig, muss eine stabile Regel verwendet werden.

Beispiel:

```text
Equal Score
    ↓
Stable Priority
    ↓
Algorithm ID
    ↓
Selected Algorithm
```

Die konkrete Tie-Breaking-Regel muss versionierbar sein.

## Profile-Guided Selection

Runtime Profiles dürfen im Deterministic Mode nur verwendet werden, wenn ihr Zustand reproduzierbar festgelegt ist.

```text
Profile State
 ├── Fixed
 ├── Versioned
 └── Ignored
```

Live-Lernen darf die Auswahl während einer deterministischen Ausführung nicht unkontrolliert verändern.

## Data-Dependent Selection

Datenabhängige Auswahl bleibt zulässig, sofern die verwendeten Eigenschaften deterministisch ermittelt werden.

```text
Same Input Data
       ↓
Same Data Characteristics
       ↓
Same Selection
```

Sampling mit zufälliger Auswahl darf nur mit einem explizit kontrollierten deterministischen Seed verwendet werden.

## Algorithmusversionen

Für langfristige Reproduzierbarkeit muss die relevante Algorithmusversion berücksichtigt werden können.

```text
Algorithm ID
    +
Algorithm Version
    +
Selection Policy Version
```

Ein später hinzugefügter Algorithmus darf eine reproduzierbare historische Auswahl nicht unkontrolliert verändern.

## Providerwahl

Deterministische Algorithmuswahl und deterministische Providerwahl sind getrennte, aber koordinierte Entscheidungen.

```text
Deterministic Algorithm Selection
              ↓
Selected Algorithm
              ↓
Deterministic Provider Selection
              ↓
Execution
```

Beide müssen den Execution Contract erfüllen.

## Explizites Override

Ein `Required Algorithm` kann die automatische Auswahl vollständig festlegen.

```text
Required Algorithm
       ↓
Contract Validation
       ↓
Selected Algorithm
```

Das Override garantiert jedoch nur die Algorithmusidentität.

Die Ausführung selbst muss weiterhin die geforderten Determinismus-Eigenschaften erfüllen.

## Reproduzierbarkeit

Für reproduzierbare Ausführungen soll NovaOS die relevanten Auswahlparameter introspektierbar machen können.

Beispiel:

```text
Algorithm Family
Selected Algorithm
Algorithm Version
Selection Policy Version
Relevant Profile Version
Execution Contract
```

Dadurch kann nachvollzogen werden, warum eine bestimmte Auswahl getroffen wurde.

## Normative Anforderungen

1. NovaOS MUSS deterministische Algorithmusauswahl unterstützen.
2. Der Deterministic Mode MUSS reproduzierbare Auswahlregeln verwenden.
3. Gleiche relevante Eingaben MÜSSEN zur gleichen Algorithmusauswahl führen.
4. Volatile Systemzustände DÜRFEN eine deterministische Auswahl nicht unkontrolliert beeinflussen.
5. Tie-Breaking MUSS stabil und definiert sein.
6. Auswahlregeln MÜSSEN versionierbar sein können.
7. Runtime Profiles DÜRFEN nur verwendet werden, wenn ihr relevanter Zustand reproduzierbar ist.
8. Data-Dependent Selection MUSS deterministisch reproduzierbare Dateneigenschaften verwenden.
9. Zufallsbasierte Auswahl DARF nur mit explizit kontrollierter deterministischer Zufallsquelle erfolgen.
10. Algorithmusversionen MÜSSEN für reproduzierbare Ausführungen fixierbar sein.
11. Deterministische Algorithmus- und Providerwahl MÜSSEN getrennt modelliert bleiben.
12. Explizite Overrides DÜRFEN den Execution Contract nicht umgehen.
13. Die für eine Auswahl relevanten Informationen SOLLEN über Introspection nachvollziehbar sein.

## Konsequenzen

### Positive Konsequenzen

- reproduzierbare Compute-Ausführungen,
- zuverlässigere Tests und Benchmarks,
- wissenschaftliche und technische Ergebnisse werden besser nachvollziehbar,
- adaptive Algorithmusauswahl bleibt außerhalb des Deterministic Mode möglich.

### Negative Konsequenzen

- dynamische Optimierungen können im Deterministic Mode eingeschränkt sein,
- Algorithmus-, Profil- und Policy-Versionen müssen kontrolliert werden.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0003_Explizites_Algorithmus_Override`
- `ADR-ALGORITHM-0005_Data_Dependent_Algorithm_Selection`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-DETERMINISTIC-SELECTION-0001`
- `NPSPEC-ALGORITHM-DETERMINISTIC-TIEBREAK-0001`
- `NPSPEC-ALGORITHM-SELECTION-POLICY-VERSION-0001`
- `NPSPEC-ALGORITHM-REPRODUCIBILITY-0001`

## Ergebnis

NovaOS kann die automatische Algorithmusauswahl vollständig reproduzierbar ausführen:

```text
Capability + Input
       ↓
Execution Contract
       ↓
Fixed Selection State
       ↓
Deterministic Candidate Filtering
       ↓
Stable Selection Rule
       ↓
Algorithm + Version
       ↓
Deterministic Execution
```

Der zentrale Grundsatz lautet:

```text
Automatische Auswahl
und Reproduzierbarkeit
sind kein Widerspruch.

Im Deterministic Mode muss NovaOS
nicht nur deterministisch rechnen,
sondern auch deterministisch entscheiden,
wie gerechnet wird.
```