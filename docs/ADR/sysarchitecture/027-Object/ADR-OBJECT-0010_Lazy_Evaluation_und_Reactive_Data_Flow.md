# ADR-OBJECT-0010 – Lazy Evaluation und Reactive Data Flow

## Status

Angenommen

## Kategorie

Object Architecture / Execution / Data Flow

## Kontext

Object Pipelines können aus mehreren voneinander abhängigen Verarbeitungsschritten bestehen.

Eine sofortige Ausführung jedes Schrittes erzeugt unnötige Berechnungen, wenn Ergebnisse nicht benötigt werden oder sich Eingaben häufig ändern.

NovaOS benötigt deshalb ein Modell für bedarfsgesteuerte und reaktive Verarbeitung.

## Entscheidung

NovaOS unterstützt **Lazy Evaluation und Reactive Data Flow** für Object Pipelines.

```text
Source Object
     ↓
Capability A
     ↓
Capability B
     ↓
Result Object
```

Pipeline-Schritte können erst ausgeführt werden, wenn ihr Ergebnis tatsächlich benötigt wird.

## Lazy Evaluation

Eine Operation kann zunächst als ausstehende Berechnung repräsentiert werden.

```text
Input
  ↓
Deferred Operation
  ↓
Result requested
  ↓
Execute
```

Nicht benötigte Ergebnisse müssen nicht berechnet werden.

## Reactive Data Flow

Abhängigkeiten zwischen Objekten können reaktiv ausgewertet werden.

```text
Source changes
      ↓
Invalidate dependent result
      ↓
Recompute when required
```

Änderungen propagieren entlang expliziter Abhängigkeiten.

## Dependency Graph

NovaOS kann Object Pipelines als Abhängigkeitsgraph darstellen.

```text
A ──→ B ──→ D
 \          ↑
  └──→ C ──┘
```

Der Graph bestimmt, welche Ergebnisse von welchen Eingaben abhängen.

## Invalidierung

Wird ein Eingabeobjekt oder dessen relevante Version verändert, werden abhängige Ergebnisse als veraltet markiert.

```text
Object V1
   ↓
Result R1

Object V2
   ↓
R1 invalid
```

Eine Invalidierung bedeutet nicht zwingend sofortige Neuberechnung.

## Caching

Bereits berechnete Ergebnisse dürfen wiederverwendet werden, wenn ihre Abhängigkeiten und Contracts weiterhin gültig sind.

```text
Inputs unchanged
      +
Contract unchanged
      ↓
Reuse Result
```

## Determinismus

Deterministische Operationen eignen sich besonders für Lazy Evaluation und Caching.

Nichtdeterministische Operationen müssen dies in ihrem Execution Contract deklarieren.

## Side Effects

Operationen mit Side Effects dürfen nicht allein aufgrund von Lazy Evaluation verschoben, wiederholt oder übersprungen werden.

```text
Pure Operation
→ freely lazy

Side Effect
→ explicit execution semantics
```

Side Effects benötigen einen expliziten Execution Contract.

## Reactive Loops

Zyklische Abhängigkeiten müssen erkannt werden.

```text
A → B → C
↑       ↓
└───────┘
```

Zyklen dürfen nur ausgeführt werden, wenn dafür eine explizite Semantik definiert ist.

## Normative Anforderungen

1. NovaOS MUSS Lazy Evaluation für geeignete Object Pipelines unterstützen können.
2. NovaOS MUSS explizite reaktive Abhängigkeiten zwischen Objekten darstellen können.
3. Änderungen an Abhängigkeiten MÜSSEN abhängige Ergebnisse invalidieren können.
4. Invalidierung DARF nicht automatisch sofortige Neuberechnung erzwingen.
5. Gültige berechnete Ergebnisse DÜRFEN gecacht und wiederverwendet werden.
6. Nichtdeterministische Operationen MÜSSEN als solche erkennbar sein.
7. Operationen mit Side Effects MÜSSEN eine explizite Ausführungssemantik besitzen.
8. Reactive Dependency Cycles MÜSSEN erkannt werden.
9. Lazy Evaluation DARF die semantische Korrektheit einer Pipeline nicht verändern.

## Konsequenzen

### Positive Konsequenzen

- unnötige Berechnungen werden vermieden,
- Änderungen können gezielt propagiert werden,
- Ergebnisse können effizient wiederverwendet werden,
- komplexe Datenmodelle können reaktiv arbeiten.

### Negative Konsequenzen

- Dependency Graph und Invalidierung müssen verwaltet werden,
- Side Effects und nichtdeterministische Operationen benötigen besondere Behandlung.

## Abhängigkeiten

- `ADR-ARCH-0009_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-OBJECT-0007_Immutable_und_Mutable_Object_Semantik`
- `ADR-OBJECT-0008_Unified_Object_Pipelines`
- `ADR-OBJECT-0009_Zero_Copy_Object_Pipelines`
- `ADR-SEMANTIC-0012_Semantic_Execution`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-LAZY-EVALUATION-0001`
- `NPSPEC-OBJECT-REACTIVE-DATAFLOW-0001`
- `NPSPEC-OBJECT-DEPENDENCY-GRAPH-0001`

## Ergebnis

NovaOS berechnet Pipeline-Ergebnisse bedarfsgesteuert und propagiert Änderungen über explizite Abhängigkeiten:

```text
Object Change
     ↓
Invalidate
     ↓
Result Needed?
  ┌──┴───┐
  ↓      ↓
 Yes     No
  ↓      ↓
Compute  Wait
```

Der zentrale Grundsatz lautet:

```text
Berechne nur,
was tatsächlich benötigt wird,

und aktualisiere nur,
was von einer Änderung betroffen ist.
```