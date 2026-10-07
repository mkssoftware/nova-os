# NPSPEC-TIME-CALIBRATION-0001 – Nova Time Calibration

## Status

Angenommen

## Kategorie

Time / Calibration

## Zweck

NovaOS definiert die Kalibrierung von Clock Sources und Zeitdomänen.

Kalibrierung bestimmt Frequenz, Drift und Abweichungen einer Zeitquelle gegenüber einer geeigneten Referenz, ohne dabei Clock Source, Zeitsynchronisation und Wall-Clock-Korrektur miteinander zu vermischen.

## Grundprinzipien

```text
Calibration ≠ Synchronization
Calibration ≠ Wall Clock Adjustment
Calibration ≠ Clock Source Selection
Nominal Frequency ≠ Measured Frequency
Drift ≠ Instantaneous Error
Reference Clock ≠ Automatically Trusted Clock
```

## Modell

```text
CalibrationState
├── ClockSourceID
├── ReferenceSourceID
├── NominalFrequency
├── MeasuredFrequency
├── FrequencyCorrection
├── DriftEstimate
├── MeasurementWindow
├── Confidence
├── Timestamp
└── State
```

## Ablauf

```text
Target Clock Source
        +
Reference Source
        ↓
Measurement
        ↓
Frequency Comparison
        ↓
Drift Estimation
        ↓
Validation
        ↓
Calibration Parameters
```

Die Kalibrierung verändert nicht zwingend die Hardwarequelle selbst.

## Referenzquellen

Geeignete Referenzen können sein:

```text
Alternative Clock Source
Platform Timer
RTC
Firmware Time Source
Paravirtualized Clock
External Synchronized Time
Registered Reference Provider
```

Die Qualität der Referenz muss bei der Bewertung berücksichtigt werden.

## Frequenzkalibrierung

Die tatsächliche Frequenz darf von der nominalen Frequenz abweichen.

```text
Counter Delta
      ÷
Reference Duration
      ↓
Measured Frequency
```

Der ermittelte Wert darf zur Umrechnung von Counter-Werten in Zeit verwendet werden.

## Drift

NovaOS darf längerfristige Abweichungen erfassen:

```text
Expected Time
     ↕
Observed Time
     ↓
Drift Estimate
```

Drift kann durch Temperatur, Energiezustände, Hardwareeigenschaften oder Virtualisierung beeinflusst werden.

## Laufzeitkalibrierung

Kalibrierung darf:

```text
At Boot
After Resume
After Clock Source Change
Periodically
After Detected Drift
On Platform Change
```

durchgeführt werden.

Eine Rekalibrierung darf die monotone Zeitkontinuität nicht verletzen.

## Multi-CPU

Bei per-CPU oder nicht vollständig synchronisierten Countern muss NovaOS Unterschiede zwischen Prozessoren erkennen können.

```text
CPU 0 Counter
CPU 1 Counter
CPU 2 Counter
      ↓
Cross-CPU Calibration
```

Task-Migration darf dadurch keine falschen Zeitdifferenzen erzeugen.

## Unsicherheit

Kalibrierungsergebnisse müssen ihre Qualität ausdrücken können:

```text
Confidence
Measurement Error
Reference Quality
Sample Count
Measurement Duration
```

Unzureichende Messdaten dürfen nicht als präzise Kalibrierung dargestellt werden.

## Fehlerverhalten

```text
Calibration Failure
        ↓
Mark Result Invalid
        ↓
Retain Safe Previous Parameters
        oder
Select Alternative Clock Source
```

Ein fehlerhafter Kalibrierungsversuch darf eine funktionierende Zeitbasis nicht unnötig beschädigen.

## Normative Anforderungen

1. NovaOS MUSS Clock Sources kalibrieren können.
2. Kalibrierung und Zeitsynchronisation MÜSSEN getrennte Mechanismen bleiben.
3. Nominale und gemessene Frequenz MÜSSEN unterscheidbar sein.
4. Referenzquellen MÜSSEN hinsichtlich ihrer Qualität bewertbar sein.
5. Drift MUSS erfassbar und kompensierbar sein können.
6. Kalibrierung MUSS beim Boot durchgeführt werden können.
7. Rekalibrierung MUSS nach relevanten Systemereignissen möglich sein.
8. Rekalibrierung DARF monotone Zeitkontinuität nicht verletzen.
9. Multi-CPU-Counter MÜSSEN auf relevante Abweichungen geprüft werden können.
10. Kalibrierungsergebnisse MÜSSEN eine Unsicherheit oder Confidence ausdrücken können.
11. Fehlgeschlagene Kalibrierung MUSS einen sicheren Fallback ermöglichen.
12. Quelle, Referenz, Frequenz, Drift, Confidence und Kalibrierungszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-HIGHRES-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-RTC-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann die tatsächlichen Eigenschaften seiner Zeitquellen bestimmen und Veränderungen ihrer Frequenz oder Drift erkennen. Kalibrierung bleibt von Zeitsynchronisation und Wall-Clock-Korrektur getrennt und ermöglicht stabile Zeitmessung auch bei Hardwarewechseln, Suspend, Multi-CPU-Systemen und variierenden Plattformbedingungen.