# ADR-ALGORITHM-0001 – Algorithm Families

## Status

Angenommen

## Kategorie

Algorithm Architecture / Execution

## Kontext

Viele NovaOS-Capabilities können durch unterschiedliche Algorithmen implementiert werden.

Beispiele:

```text
Compression
 ├── Fast Algorithm
 ├── Memory Efficient Algorithm
 └── High Compression Algorithm

Image Scaling
 ├── Nearest Neighbor
 ├── Bilinear
 └── High Quality Algorithm

Path Finding
 ├── Dijkstra
 ├── A*
 └── Specialized Algorithm
```

Ein einzelner fest verdrahteter Algorithmus ist nicht für alle Hardware-, Daten- und Ausführungssituationen optimal.

NovaOS benötigt daher eine Abstraktion oberhalb konkreter Algorithmen.

## Entscheidung

NovaOS führt **Algorithm Families** als logische Gruppierung semantisch austauschbarer Algorithmen ein.

```text
Capability
    ↓
Algorithm Family
    ↓
 ┌──┼──┬──┐
 A1  A2 A3 A4
```

Eine Algorithm Family beschreibt **welches Ergebnis und welche Semantik erwartet werden**, während konkrete Algorithmen unterschiedliche Implementierungsstrategien darstellen.

## Algorithm Family

Eine Algorithm Family besitzt eine stabile logische Identität.

Beispiel:

```text
Image.Resize
    ↓
Algorithm Family
    ├── Nearest
    ├── Bilinear
    └── Lanczos
```

Die Family definiert den gemeinsamen semantischen Rahmen ihrer Mitglieder.

## Algorithm Descriptor

Ein konkreter Algorithmus beschreibt mindestens:

```text
Algorithm ID
Algorithm Family
Supported Semantic Types
Execution Properties
Resource Requirements
Precision Properties
Determinism Properties
Compatible Compute Providers
```

Zusätzliche Eigenschaften können über erweiterbare Metadaten beschrieben werden.

## Semantische Austauschbarkeit

Algorithmen derselben Family müssen nicht identische interne Verfahren verwenden.

Sie müssen jedoch innerhalb ihres deklarierten Contracts semantisch kompatibel sein.

```text
Input
  ↓
Algorithm A ──┐
Algorithm B ──┼→ Contract-compatible Result
Algorithm C ──┘
```

Unterschiede bei Qualität, Genauigkeit oder Verlustbehaftung müssen explizit beschrieben werden.

## Trennung von Algorithmus und Provider

Algorithmus und Compute Provider sind getrennte Entscheidungen.

```text
Capability
    ↓
Algorithm Family
    ↓
Algorithm
    ↓
Compute Provider
```

Beispielsweise kann derselbe Algorithmus auf CPU oder GPU implementiert sein.

Ebenso können unterschiedliche Algorithmen auf demselben Provider verfügbar sein.

## Auswahl

Consumer sollen normalerweise die gewünschte Capability und Anforderungen beschreiben, nicht einen konkreten Algorithmus.

```text
Intent
  ↓
Capability
  ↓
Algorithm Family
  ↓
Valid Algorithms
  ↓
Provider Selection
  ↓
Execution
```

NovaOS kann dadurch Algorithmus und Compute Provider gemeinsam planen.

## Execution Contracts

Die Auswahl eines Algorithmus muss den Execution Contract berücksichtigen.

Relevante Kriterien können sein:

```text
Correctness
Precision
Determinism
Latency
Deadline
Resource Budget
Energy Budget
Quality
Data Locality
Required / Preferred Algorithm
```

Hard Constraints dürfen durch Algorithmusoptimierung nicht verletzt werden.

## Algorithmus-Varianten

Eine Family kann spezialisierte Varianten enthalten.

```text
General Algorithm
SIMD Variant
GPU Variant
Low-Memory Variant
Realtime Variant
High-Precision Variant
```

Varianten können unterschiedliche Anforderungen besitzen, solange ihre Semantik korrekt beschrieben ist.

## Erweiterbarkeit

Neue Algorithmen können einer bestehenden Family hinzugefügt werden, ohne Consumer ändern zu müssen.

```text
Existing Family
     ↓
New Algorithm Registered
     ↓
Available Candidate
```

Die Aufnahme eines neuen Algorithmus darf bestehende Contracts nicht verändern.

## Normative Anforderungen

1. NovaOS MUSS Algorithmen zu logischen Algorithm Families gruppieren können.
2. Algorithm Families MÜSSEN eine stabile Identität besitzen.
3. Algorithmen MÜSSEN ihre Family-Zugehörigkeit deklarieren.
4. Mitglieder einer Family MÜSSEN ihre semantischen Eigenschaften explizit beschreiben.
5. Qualitäts-, Präzisions- oder Verlustunterschiede DÜRFEN nicht verborgen werden.
6. Algorithmus und Compute Provider MÜSSEN getrennte Architekturkonzepte bleiben.
7. Die Algorithmuswahl MUSS Execution Contracts berücksichtigen.
8. Hard Constraints DÜRFEN durch Algorithmusoptimierung nicht verletzt werden.
9. Neue Algorithmen MÜSSEN ohne Änderungen an bestehenden Consumern integrierbar sein können.
10. Consumer SOLLEN normalerweise gegen Capabilities und Algorithm Families statt konkrete Implementierungen arbeiten.

## Konsequenzen

### Positive Konsequenzen

- Algorithmen können unabhängig von Consumern ausgetauscht werden,
- NovaOS kann unterschiedliche Optimierungsstrategien verwenden,
- neue Algorithmen lassen sich nachträglich integrieren,
- Algorithmus- und Hardwarewahl können gemeinsam optimiert werden,
- Capabilities bleiben von konkreten Implementierungen entkoppelt.

### Negative Konsequenzen

- Algorithmen benötigen präzise semantische Beschreibungen,
- die Runtime muss zwischen Algorithmus- und Provider-Auswahl unterscheiden.

## Abhängigkeiten

- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-FAMILY-0001`
- `NPSPEC-ALGORITHM-DESCRIPTOR-0001`
- `NPSPEC-ALGORITHM-SEMANTICS-0001`
- `NPSPEC-ALGORITHM-REGISTRATION-0001`

## Ergebnis

NovaOS trennt fachliche Fähigkeiten, Algorithmen und ausführende Hardware:

```text
Capability
    ↓
Algorithm Family
    ↓
Algorithm Selection
    ↓
Compute Provider Selection
    ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
Eine Capability beschreibt,
was erreicht werden soll.

Eine Algorithm Family beschreibt,
welche Lösungswege dafür existieren.

Der konkrete Algorithmus bestimmt,
wie es berechnet wird.
```