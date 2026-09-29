# NPSPEC-UNCERTAINTY-0006 – Uncertainty Presentation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Unsicherheit für Nutzer verständlich darstellt.

Ziel ist, Unsicherheit sichtbar zu machen, ohne die Oberfläche unnötig zu überladen oder eine höhere Sicherheit vorzutäuschen als tatsächlich vorhanden ist.

## Grundprinzip

```text
Uncertain Value
    ↓
Presentation Policy
    ↓
verständliche Darstellung
```

Die Darstellung muss zur Art der Unsicherheit und zum Nutzungskontext passen.

## Darstellungsformen

NovaOS muss mindestens folgende Formen unterstützen können:

```text
VALUE ± ERROR
RANGE
CONFIDENCE
QUALITATIVE
DISTRIBUTION_SUMMARY
```

Beispiele:

```text
23.7 ± 0.4 °C
```

```text
18–22 °C
```

```text
94 % confidence
```

```text
Unsicherheit: hoch
```

## Kontextabhängigkeit

Die Darstellung darf je nach Kontext unterschiedlich detailliert sein.

Beispiel:

```text
Alltag:
    ca. 24 °C

Technische Ansicht:
    23.7 ± 0.4 °C

Analyse:
    23.7 °C
    confidence: 95 %
    source: Sensor A
```

Die zugrunde liegenden Daten bleiben identisch.

## Detailstufen

NovaOS soll mindestens folgende Präsentationsstufen unterstützen:

```text
SIMPLE
STANDARD
DETAILED
EXPERT
```

### `SIMPLE`

Nur die für die Entscheidung relevante Unsicherheit wird sichtbar gemacht.

### `STANDARD`

Wert und grundlegende Unsicherheit werden angezeigt.

### `DETAILED`

Zusätzliche Confidence-, Quellen- und Fehlerangaben werden dargestellt.

### `EXPERT`

Die vollständigen verfügbaren Unsicherheitsinformationen können angezeigt werden.

## Keine falsche Präzision

Die Darstellung darf keine höhere Genauigkeit suggerieren als die Daten rechtfertigen.

Beispiel:

Ungünstig:

```text
23.742381 °C ± 0.5 °C
```

Bevorzugt:

```text
23.7 ± 0.5 °C
```

Die Zahl der dargestellten Stellen soll zur Unsicherheit passen.

## Unbekannte Unsicherheit

Unbekannte Unsicherheit muss erkennbar sein.

Beispiele:

```text
Unsicherheit unbekannt
```

oder:

```text
23.7 °C
confidence: unknown
```

Fehlende Unsicherheitsinformation darf nicht wie ein exakter Wert dargestellt werden.

## Entscheidungsdarstellung

Bei unsicheren Entscheidungen muss das Ergebnis entsprechend dargestellt werden.

Beispiel:

```text
Schwelle möglicherweise überschritten
```

statt:

```text
Schwelle überschritten
```

wenn das Ergebnis laut `Nova.Uncertainty` nicht eindeutig ist.

Mögliche Zustände:

```text
confirmed
unlikely
uncertain
indeterminate
```

## Confidence

Confidence-Werte dürfen numerisch oder qualitativ dargestellt werden.

Beispiele:

```text
Confidence: 0.94
```

```text
94 %
```

```text
hohe Sicherheit
```

Die Darstellung muss zur tatsächlichen Semantik des Confidence-Werts passen.

Ein Modell-Confidence-Wert darf nicht automatisch als statistische Wahrscheinlichkeit dargestellt werden.

## Visuelle Darstellung

Unsicherheit darf zusätzlich visuell dargestellt werden.

Beispiele:

```text
error bars
confidence bands
range overlays
uncertainty areas
```

Farbe allein darf nicht die einzige Informationsträgerin sein.

Die Darstellung muss auch ohne Farbwahrnehmung verständlich bleiben.

## Vergleich

Bei mehreren unsicheren Werten soll NovaOS Überlappungen sichtbar machen können.

Beispiel:

```text
A: 10 ± 2
B: 11 ± 2
```

Die Darstellung soll erkennen lassen, dass keine eindeutige Trennung besteht.

## Herkunft

Auf Wunsch soll der Nutzer erkennen können, woher eine Unsicherheitsangabe stammt.

Beispiel:

```text
23.7 ± 0.4 °C

Quelle:
    Sensor temp_01

Unsicherheit:
    Herstellerangabe
```

Detaillierte Provenienz kann über `Nova.Causality` und `Nova.Evidence` geöffnet werden.

## Adaptivität

NovaOS darf die Darstellungsstufe an Aufgabe und Nutzerkontext anpassen.

Beispiel:

```text
Navigation:
    einfache Genauigkeitsanzeige

Wissenschaftliche Analyse:
    vollständige Fehler- und Confidence-Daten
```

Die zugrunde liegenden Unsicherheitsdaten dürfen dabei nicht verändert werden.

## Export

Beim Export muss Unsicherheit erhalten bleiben können.

Beispiele:

```text
CSV
Document
Chart
Report
Scientific Data
```

Wenn ein Zielformat Unsicherheit nicht vollständig unterstützt, muss der Informationsverlust erkennbar sein.

## Beispiel

Interne Daten:

```text
Measurement {
    value:
        23.74 °C

    uncertainty:
        ±0.42 °C

    confidence:
        0.95
}
```

Standarddarstellung:

```text
23.7 ± 0.4 °C
```

Detailansicht:

```text
Temperature:
    23.74 °C

Uncertainty:
    ±0.42 °C

Confidence:
    95 %

Source:
    sensor:temp_01
```

## Normative Anforderungen

1. Unsicherheit MUSS für den Nutzer darstellbar sein.
2. Die Darstellung DARF keine höhere Sicherheit oder Präzision vortäuschen als die zugrunde liegenden Daten besitzen.
3. Unbekannte Unsicherheit MUSS als solche erkennbar sein.
4. Unterschiedliche Detailstufen MÜSSEN unterstützt werden können.
5. Unsichere Entscheidungen DÜRFEN nicht als eindeutig dargestellt werden.
6. Confidence-Darstellung MUSS zur tatsächlichen Confidence-Semantik passen.
7. Visuelle Unsicherheit DARF nicht ausschließlich über Farbe vermittelt werden.
8. Exportvorgänge SOLLEN Unsicherheitsinformationen erhalten oder Informationsverlust kennzeichnen.

## Abgrenzung

Diese NPSPEC definiert:

- Darstellung unsicherer Werte
- Detailstufen
- Vermeidung falscher Präzision
- Darstellung unsicherer Entscheidungen
- grundlegende Visualisierung

Nicht Bestandteil sind:

- mathematische Unsicherheitsberechnung
- Vergleichsalgorithmen
- konkrete UI-Komponenten
- konkrete Diagramm-Engine

## Zugehörige NPSPECs

- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-UNCERTAINTY-0002 – Confidence & Error Representation`
- `NPSPEC-UNCERTAINTY-0003 – Uncertainty Propagation`
- `NPSPEC-UNCERTAINTY-0004 – Uncertain Comparison & Decision Semantics`
- `NPSPEC-UNCERTAINTY-0005 – Uncertainty Serialization`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`