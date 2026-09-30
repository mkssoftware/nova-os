# NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Evidence-Daten abgefragt und für Nutzer oder Systemkomponenten verständlich erklärt werden können.

Ziel ist, Fragen wie folgende beantworten zu können:

```text
Warum entstand dieses Ergebnis?
Welche Quellen wurden verwendet?
Welche Capability hat es erzeugt?
Welche Annahmen und Prüfungen gab es?
```

## Grundprinzip

```text
Result
    ↓
Evidence Bundle
    ↓
Evidence Query
    ↓
Explanation
```

## Query-Modell

Evidence muss mindestens nach folgenden Informationen abfragbar sein:

```text
sources
provenance
algorithm
model
tool
capability
assumptions
verification
```

Beispiel:

```text
query:
    sources of object:report:81
```

## Provenance Query

NovaOS muss die Herkunft eines Ergebnisses nachvollziehbar darstellen können.

```text
Dataset
    ↓
Filter
    ↓
Analysis
    ↓
Report
```

Dabei darf auf `Nova.Causality` und vorhandene Lineage-Daten zurückgegriffen werden.

## Explanation

Eine Erklärung soll Evidence-Daten in eine verständliche Darstellung überführen.

Beispiel:

```text
Result:
    Report

Created from:
    Dataset 17

Processed by:
    nova.statistics.analyze@3

Verification:
    PASS
```

Die Erklärung darf die zugrunde liegenden Evidence-Daten nicht verändern.

## Detailstufen

Erklärungen dürfen unterschiedliche Detailstufen besitzen.

```text
SUMMARY
DETAILED
TECHNICAL
```

Die zugrunde liegenden Evidence-Daten bleiben dabei identisch.

## Unsicherheit und fehlende Daten

Fehlende oder unsichere Evidence muss ausdrücklich erkennbar bleiben.

Beispiel:

```text
Source:
    UNKNOWN
```

darf nicht als gesicherte Herkunft dargestellt werden.

## Zugriff

Evidence-Abfragen unterliegen weiterhin:

```text
permissions
information_flow
privacy
```

Eine Erklärung darf keine geschützten Evidence-Daten offenlegen, die der anfragende Kontext nicht lesen darf.

## Beispiel

```text
query:
    explain object:analysis:81

result:
    source:
        object:dataset:17

    capability:
        statistics.analyze

    tool:
        nova.statistics.engine@3

    verification:
        PASS
```

## Normative Anforderungen

1. Evidence Bundles MÜSSEN strukturiert abfragbar sein.
2. Quellen, Provenance und Ausführungsidentität MÜSSEN separat abfragbar sein.
3. Evidence MUSS in verständliche Erklärungen überführbar sein.
4. Fehlende oder unsichere Evidence DARF nicht als bestätigte Information dargestellt werden.
5. Unterschiedliche Detailstufen MÜSSEN dieselben zugrunde liegenden Evidence-Daten verwenden können.
6. Evidence Queries MÜSSEN bestehende Zugriffs- und Information-Flow-Regeln einhalten.
7. Erklärungen DÜRFEN die zugrunde liegenden Evidence-Daten nicht verändern.

## Abgrenzung

Diese NPSPEC definiert:

- Evidence Queries
- Provenance-Abfragen
- Evidence-Erklärungen
- Detailstufen

Nicht Bestandteil sind:

- Evidence-Erzeugung
- Integrität und Signierung
- allgemeine Causality Queries
- Retention und Privacy

## Zugehörige NPSPECs

- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`
- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`
- `NPSPEC-EVIDENCE-0003 – Algorithm, Model & Tool Identity`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`
- `NPSPEC-EVIDENCE-0006 – Evidence Retention & Privacy`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`