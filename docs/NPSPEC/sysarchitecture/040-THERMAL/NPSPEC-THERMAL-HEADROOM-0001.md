# NPSPEC-THERMAL-HEADROOM-0001 – Nova Thermal Headroom

## Status

Angenommen

## Kategorie

Thermal / Headroom

## Zweck

NovaOS definiert Thermal Headroom als Maß für den verbleibenden thermischen Spielraum einer Thermal Zone.

Thermal Headroom ermöglicht Scheduler, Resource Economy und Execution Contracts, thermische Reserven zu berücksichtigen, bevor kritische Temperaturen oder hardwareseitiges Throttling erreicht werden.

## Grundprinzipien

```text
Thermal Headroom ≠ Temperature
Headroom ≠ Performance
Headroom ≠ Power Budget
High Temperature ≠ Automatically Low Headroom
Thermal Limit ≠ Operating Target
Unknown Headroom ≠ Available Headroom
```

## Modell

```text
ThermalHeadroom
├── ZoneID
├── CurrentTemperature
├── EffectiveLimit
├── Headroom
├── Trend
├── Confidence
└── Timestamp
```

Grundsätzlich beschreibt der Headroom den Abstand zu einer relevanten thermischen Grenze:

```text
Headroom = EffectiveLimit - CurrentTemperature
```

Die tatsächliche Bewertung darf zusätzlich Temperaturtrend, Sensorqualität, Hardwarezustand und Thermal Policy berücksichtigen.

## Zustände

NovaOS darf Thermal Headroom abstrahiert klassifizieren:

```text
High
Moderate
Low
Critical
None
Unknown
```

Die konkreten Schwellenwerte werden pro Thermal Zone und Hardwareplattform bestimmt.

## Trend

Der aktuelle Temperaturwert allein reicht nicht für jede Entscheidung aus.

```text
Headroom
   +
Temperature Trend
   ↓
Thermal Capacity
```

Eine Zone mit schnell steigender Temperatur darf daher konservativer bewertet werden als eine Zone mit gleichem Headroom und stabiler Temperatur.

## Resource Economy

Thermal Headroom beeinflusst die tatsächlich nutzbaren Ressourcen:

```text
Hardware Capacity
      ↓
Thermal Headroom
      ↓
Available Resource Budget
```

Bei geringem Headroom dürfen beispielsweise CPU-, GPU- oder Accelerator-Budgets reduziert werden.

## Scheduler

Der Scheduler darf Thermal Headroom zur Workload-Platzierung verwenden:

```text
Workload
   ↓
Candidate Resources
   ↓
Thermal Headroom
   ↓
Placement
```

Workloads sollen bevorzugt auf Ressourcen mit ausreichender thermischer Reserve verteilt werden, sofern dadurch keine höher priorisierten Anforderungen verletzt werden.

## Execution Contracts

Execution Contracts dürfen thermische Anforderungen enthalten:

```text
Minimum Thermal Headroom
Sustained Performance Requirement
Thermal Constraint
```

Kann ein Hard Requirement nicht erfüllt werden, darf die Ausführung nicht allein durch Überlastung der Hardware erzwungen werden.

## Vorhersage

NovaOS darf zukünftigen Headroom aus Messverlauf, Last und Temperaturtrend abschätzen.

```text
Current Headroom
      +
Trend
      +
Expected Workload
      ↓
Predicted Headroom
```

Vorhersagen bleiben von gemessenen Werten unterscheidbar.

## Fehlerverhalten

Kann der Headroom aufgrund fehlender oder ungültiger Sensor- oder Grenzwertdaten nicht zuverlässig bestimmt werden:

```text
Headroom = Unknown
```

`Unknown` darf nicht als verfügbare thermische Reserve behandelt werden.

## Normative Anforderungen

1. Thermal Headroom MUSS pro Thermal Zone bestimmbar sein können.
2. Headroom MUSS von der absoluten Temperatur getrennt betrachtet werden.
3. Die Berechnung MUSS eine relevante effektive Thermalgrenze verwenden.
4. Temperaturtrend SOLL bei der Bewertung berücksichtigt werden können.
5. Gemessener und vorhergesagter Headroom MÜSSEN unterscheidbar bleiben.
6. `Unknown` DARF nicht als verfügbare thermische Reserve interpretiert werden.
7. Thermal Headroom MUSS der Resource Economy bereitgestellt werden können.
8. Der Scheduler MUSS Thermal Headroom bei Platzierungsentscheidungen berücksichtigen können.
9. Execution Contracts DÜRFEN Thermal-Headroom-Anforderungen definieren.
10. Hardware-Sicherheitsgrenzen DÜRFEN durch Headroom-Optimierung nicht überschritten werden.
11. Headroom-Berechnungen MÜSSEN bei geänderten Thermalgrenzen aktualisiert werden.
12. Headroom, Trend, verwendete Grenze und Confidence MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-THERMAL-SENSOR-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt thermische Reserve als explizite Systemressource. Thermal Headroom verbindet aktuelle Temperatur, thermische Grenzen und Temperaturtrend mit Scheduler, Resource Economy und Execution Contracts, sodass Leistung frühzeitig und kontrolliert an die tatsächlich verfügbare thermische Kapazität angepasst werden kann.