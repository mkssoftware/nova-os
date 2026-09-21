# ADR-ALGORITHM-0003 – Explizites Algorithmus Override

## Status

Angenommen

## Kategorie

Algorithm Architecture / Algorithm Selection

## Kontext

NovaOS wählt innerhalb einer Algorithm Family standardmäßig automatisch einen geeigneten Algorithmus.

Für bestimmte Anwendungsfälle muss jedoch eine explizite Auswahl möglich sein.

Beispiele:

```text
Debugging
Benchmarking
Reproduzierbare Tests
Forschung
Kompatibilität
Validierung
Spezielle Qualitätsanforderungen
```

Eine explizite Auswahl darf dabei die grundlegenden Execution Contracts nicht umgehen.

## Entscheidung

NovaOS unterstützt ein **explizites Algorithmus Override**.

Dabei werden zwei Formen unterschieden:

```text
Preferred Algorithm
Required Algorithm
```

`Preferred` beeinflusst die automatische Auswahl.

`Required` legt den Algorithmus verbindlich fest.

## Preferred Algorithm

Ein Preferred Algorithm ist eine Soft Constraint.

```text
Preferred = Algorithm A
       ↓
A erfüllt Contract?
 ├── Ja   → bevorzugen
 └── Nein → automatische Auswahl
```

NovaOS darf einen anderen Algorithmus wählen, wenn der bevorzugte Algorithmus ungeeignet oder nicht verfügbar ist.

## Required Algorithm

Ein Required Algorithm ist eine Hard Constraint.

```text
Required = Algorithm B
       ↓
B erfüllt Contract?
 ├── Ja   → verwenden
 └── Nein → Request ablehnen
```

Ein stiller Wechsel auf einen anderen Algorithmus ist nicht zulässig.

## Algorithm Family

Ein Override muss innerhalb der semantisch passenden Algorithm Family erfolgen.

```text
Capability
    ↓
Algorithm Family
    ↓
Explicit Algorithm
```

Ein Algorithmus aus einer semantisch inkompatiblen Family darf nicht durch ein Override erzwungen werden.

## Execution Contract

Ein Algorithmus Override umgeht nicht den Execution Contract.

```text
Algorithm Override
       ↓
Semantic Validation
       ↓
Execution Contract
       ↓
Allowed / Rejected
```

Insbesondere bleiben bindend:

```text
Correctness
Safety
Security
Required Precision
Determinism
Hard Realtime
Resource Limits
Data Sovereignty
```

## Providerwahl

Die explizite Algorithmuswahl ist von der Compute-Providerwahl getrennt.

```text
Required Algorithm
       ↓
Compatible Providers
       ↓
Provider Selection
       ↓
Execution
```

NovaOS kann weiterhin automatisch CPU, GPU, NPU, DSP, FPGA oder einen Remote Provider auswählen, sofern kein zusätzliches Provider Override besteht.

## Kombination mit Provider Override

Algorithmus- und Provider Override können kombiniert werden.

```text
Required Algorithm
        +
Required Provider
        ↓
Compatibility Check
        ↓
Execution
```

Sind Algorithmus und Provider inkompatibel, muss der Request abgelehnt werden.

## Deterministic Mode

Explizite Algorithmus Overrides können für reproduzierbare Ausführungen verwendet werden.

```text
Fixed Algorithm
      +
Execution Contract
      +
Deterministic Provider
      ↓
Reproducible Execution
```

Ein festgelegter Algorithmus allein garantiert keinen Determinismus.

## Versionierung

Falls unterschiedliche Algorithmusversionen semantisch relevante Unterschiede besitzen, muss ein Override eine konkrete Version oder kompatible Versionsanforderung adressieren können.

```text
Algorithm ID
    +
Version Constraint
```

Dadurch können reproduzierbare Tests und langfristige Kompatibilität unterstützt werden.

## Observability

NovaOS soll explizite Algorithmus Overrides sichtbar machen.

Beispiel:

```text
Selection Mode: Required Override
Algorithm Family: Image.Resize
Requested Algorithm: Lanczos
Selected Algorithm: Lanczos
Reason: Explicit Algorithm Override
```

Fehlgeschlagene Overrides müssen einen strukturierten Ablehnungsgrund liefern.

## Normative Anforderungen

1. NovaOS MUSS explizite Algorithmus Overrides unterstützen.
2. `Preferred Algorithm` MUSS als Soft Constraint behandelt werden.
3. `Required Algorithm` MUSS als Hard Constraint behandelt werden.
4. Ein Required Override DARF keinen stillen Algorithmus-Fallback erlauben.
5. Der gewählte Algorithmus MUSS zur erforderlichen Algorithm Family gehören.
6. Overrides DÜRFEN den Execution Contract nicht umgehen.
7. Algorithmus- und Provider Override MÜSSEN getrennte Konzepte bleiben.
8. NovaOS MUSS die Kompatibilität zwischen explizitem Algorithmus und Compute Provider prüfen.
9. Algorithmus Overrides MÜSSEN bei Bedarf Versionsanforderungen ausdrücken können.
10. Ein Override DARF keine Safety-, Security-, Correctness- oder Sovereignty-Anforderungen abschwächen.
11. Overrides SOLLEN über Introspection und Logging nachvollziehbar sein.
12. Fehlgeschlagene Overrides MÜSSEN einen strukturierten Ablehnungsgrund liefern.

## Konsequenzen

### Positive Konsequenzen

- gezielte Algorithmuswahl für Tests und Benchmarks,
- reproduzierbare Experimente werden erleichtert,
- spezielle Algorithmen können bewusst erzwungen werden,
- automatische Auswahl bleibt der Standardfall,
- Algorithmus- und Hardwarewahl bleiben voneinander getrennt.

### Negative Konsequenzen

- manuelle Auswahl kann Performance oder Energieeffizienz verschlechtern,
- Required Overrides können Requests nicht ausführbar machen,
- Versionierung konkreter Algorithmen muss verwaltet werden.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-COMPUTE-0008_Explizites_Provider_Override`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-OVERRIDE-0001`
- `NPSPEC-ALGORITHM-PREFERENCE-0001`
- `NPSPEC-ALGORITHM-REQUIREMENT-0001`
- `NPSPEC-ALGORITHM-VERSION-CONSTRAINT-0001`

## Ergebnis

NovaOS erlaubt die automatische Algorithmusauswahl gezielt zu beeinflussen oder verbindlich einzuschränken:

```text
Capability
    ↓
Algorithm Family
    ↓
Algorithm Override
    ↓
Contract Validation
    ↓
Algorithm
    ↓
Provider Selection
    ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
NovaOS wählt Algorithmen
standardmäßig automatisch.

Eine explizite Auswahl ist möglich,
aber sie darf Semantik und
Execution Contracts niemals umgehen.
```