# ADR-ADAPTIVE-0008 – Adaptive Power Management

## Status

Angenommen

## Kategorie

Adaptive Systems / Power Management

## Kontext

NovaOS soll Energieverbrauch und Leistung dynamisch an Workload, Hardwarezustand und Nutzeranforderungen anpassen.

Relevante Faktoren sind beispielsweise:

```text
CPU / GPU / NPU Load
Execution Deadlines
Battery State
Power Source
Thermal State
Energy Budget
Provider Efficiency
Workload History
```

Statische Energiesparprofile können diese Bedingungen nur begrenzt berücksichtigen.

## Entscheidung

NovaOS unterstützt **Adaptive Power Management** auf Basis von Messwerten, Vorhersagen und Prediction Error.

```text
System State
     ↓
Energy Prediction
     ↓
Power Decision
     ↓
Execution
     ↓
Measured Energy / Performance
     ↓
Prediction Error
     ↓
Model Correction
```

Adaptive Energieoptimierung erfolgt ausschließlich innerhalb bestehender Hard Constraints.

## Power Decisions

Adaptive Entscheidungen können insbesondere betreffen:

```text
Performance State
Power State
Idle State
Device Activation
Device Suspension
Accelerator Usage
Workload Placement
Execution Timing
```

Hardwareabhängige Mechanismen bleiben von der übergeordneten Policy getrennt.

## Energy Prediction

NovaOS darf den erwarteten Energiebedarf einer Ausführung vorhersagen.

```text
Workload
   +
Algorithm
   +
Provider
   +
Hardware State
   ↓
Expected Energy Cost
```

Diese Vorhersage kann gemeinsam mit Performance-, Latency- und Resource-Modellen verwendet werden.

## Prediction Error

Vorhergesagter und tatsächlicher Energieverbrauch werden verglichen.

```text
Predicted Energy
      ↓
Execution
      ↓
Measured Energy
      ↓
Prediction Error
      ↓
Energy Model Correction
```

Wiederholte Abweichungen sollen zukünftige Power Decisions verbessern.

## Performance und Energie

Energieverbrauch wird nicht isoliert optimiert.

```text
Performance
Latency
Energy
Resource Usage
      ↓
Joint Evaluation
```

Eine energiesparende Entscheidung ist nur zulässig, wenn alle relevanten Hard Requirements weiterhin erfüllt werden.

## Execution Contracts

Power Management berücksichtigt insbesondere:

```text
Deadline
Latency Requirement
Energy Budget
Resource Budget
Required Performance
Required Hardware
Determinism
```

Beispiel:

```text
Energy Saving
     ↓
Would violate Deadline
     ↓
Rejected
```

## Battery und Power Source

Adaptive Policies dürfen den aktuellen Energiezustand berücksichtigen.

Beispiele:

```text
AC Power
Battery
Battery Level
Charging State
Available Energy Budget
```

Dadurch kann dieselbe Workload abhängig von der Energieversorgung unterschiedlich optimiert werden.

## Thermal State

Thermische Zustände sind Bestandteil der Power-Entscheidung.

```text
Workload
   ↓
Power Consumption
   ↓
Thermal State
   ↓
Performance / Power Adjustment
```

Thermische Sicherheitsgrenzen sind Hard Constraints und dürfen nicht adaptiv abgeschwächt werden.

## Provider Selection

Adaptive Power Management kann die Auswahl von Compute Providern beeinflussen.

Beispiel:

```text
CPU
GPU
NPU
DSP
Remote Provider
      ↓
Energy + Performance Evaluation
      ↓
Provider Selection
```

Die theoretisch schnellste Ressource muss nicht die energieeffizienteste sein.

## Idle und Sleep Prediction

NovaOS darf vorhersehbare Inaktivität nutzen, um Komponenten früher in geeignete Energiesparzustände zu versetzen.

```text
Activity
   ↓
Idle Prediction
   ↓
Power State Transition
   ↓
Actual Activity
   ↓
Prediction Error
```

Fehlvorhersagen dürfen keine funktionalen Fehler verursachen.

Wake-up-Kosten müssen berücksichtigt werden.

## Predictive Preloading

Power Management und Predictive Preloading müssen koordiniert werden.

```text
Expected Latency Benefit
        vs.
Preload Energy Cost
```

Bei geringer erwarteter Nutzung kann ein Preload aus Energiegründen unterbleiben.

Bei expliziten Performance- oder Latency-Anforderungen kann dagegen Preloading bevorzugt werden.

## Distributed Compute

Adaptive Power Management darf lokale und entfernte Ausführung energetisch vergleichen.

Dabei müssen berücksichtigt werden:

```text
Local Compute Energy
Network Energy
Transfer Cost
Remote Compute Energy
Latency
Data Sovereignty
Trust
```

Remote Execution darf nicht allein deshalb als energiesparend gelten, weil der lokale Energieverbrauch sinkt.

## Explizite Nutzerentscheidung

Explizite Nutzerentscheidungen besitzen Vorrang vor adaptiven Energiepräferenzen, soweit keine Hard Constraints verletzt werden.

Beispiele:

```text
Prefer Performance
Prefer Efficiency
Battery Saving
Disable Adaptive Power Optimization
```

Adaptive Modelle dürfen solche Einstellungen nicht selbstständig überschreiben.

## Deterministic Mode

Adaptive Power Management darf deterministische Ausführungen nicht semantisch verändern.

Falls Power State oder Provider Selection für Reproduzierbarkeit relevant sind, müssen die entsprechenden adaptiven Zustände:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

## Observability

Power Decisions und deren Auswirkungen müssen messbar sein.

Relevante Informationen können sein:

```text
Predicted Energy
Measured Energy
Power State
Thermal State
Selected Provider
Performance Impact
Wake-up Cost
Prediction Error
```

Damit können Energie- und Performance-Modelle kontinuierlich verbessert werden.

## Explainability

NovaOS soll relevante Power Decisions erklären können.

Beispiele:

```text
Warum wurde der CPU-Takt reduziert?

Warum wurde die GPU statt der CPU verwendet?

Warum wurde ein Gerät in Sleep versetzt?

Warum wurde ein Preload nicht ausgeführt?

Warum wurde ein Energiesparzustand verlassen?
```

## Cold Start

Ohne ausreichende Erfahrungswerte verwendet NovaOS:

```text
Hardware Power Information
Static Energy Models
Firmware Information
Declared Provider Properties
Safe Defaults
```

Adaptive Modelle sind keine Voraussetzung für korrektes Power Management.

## Normative Anforderungen

1. NovaOS SOLL Adaptive Power Management unterstützen.
2. Adaptive Power Decisions SOLLEN Prediction Error als Lernsignal verwenden.
3. Vorhergesagter und tatsächlicher Energieverbrauch MÜSSEN korrelierbar sein können.
4. Performance, Latency und Energy MÜSSEN gemeinsam bewertbar sein.
5. Hard Requirements DÜRFEN durch Energieoptimierung nicht abgeschwächt werden.
6. Energy Budgets MÜSSEN berücksichtigt werden.
7. Battery-, Power- und Thermal-State DÜRFEN adaptive Entscheidungen beeinflussen.
8. Thermische Sicherheitsgrenzen MÜSSEN Hard Constraints bleiben.
9. Provider Selection DARF Energy Efficiency berücksichtigen.
10. Idle- und Sleep-Prediction DARF verwendet werden.
11. Wake-up- und Transition-Kosten MÜSSEN berücksichtigt werden können.
12. Predictive Preloading und Power Management MÜSSEN koordinierbar sein.
13. Distributed Energy Evaluation SOLL lokale, Netzwerk- und entfernte Kosten berücksichtigen.
14. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Energiepräferenzen besitzen.
15. Deterministic Mode DARF durch adaptive Power States nicht unkontrolliert beeinflusst werden.
16. Power Decisions MÜSSEN mit Observability und Resource Tracing korrelierbar sein.
17. Relevante Power Decisions SOLLEN erklärbar sein.
18. Ohne adaptive Modelle MUSS korrektes Power Management weiterhin möglich sein.

## Konsequenzen

### Positive Konsequenzen

- Energieverbrauch kann workloadabhängig optimiert werden,
- Battery Runtime kann verbessert werden,
- Performance und Energie können gemeinsam geplant werden,
- Provider können nach realer Energieeffizienz bewertet werden,
- wiederholte Fehlprognosen verbessern zukünftige Power Decisions.

### Negative Konsequenzen

- Energie- und Thermalmodelle erzeugen zusätzlichen Systemzustand,
- Fehlvorhersagen können unnötige Power-State-Wechsel verursachen,
- Hardwareplattformen liefern unterschiedlich genaue Energieinformationen.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-ADAPTIVE-0006_Predictive_Preloading`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0007_Explainable_Resource_Allocation`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-POWER-0001`
- `NPSPEC-ADAPTIVE-ENERGY-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-POWER-STATE-0001`
- `NPSPEC-ADAPTIVE-THERMAL-0001`
- `NPSPEC-ADAPTIVE-POWER-FEEDBACK-0001`

## Ergebnis

NovaOS erweitert Power Management um einen kontrollierten adaptiven Feedback-Zyklus:

```text
Observe
   ↓
Predict Energy / Load
   ↓
Power Decision
   ↓
Execute
   ↓
Measure
   ↓
Prediction Error
   ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht einfach
möglichst wenig Energie verbrauchen.

Es soll nur so viel Energie einsetzen,
wie für die gewünschte Aufgabe
und ihre Contracts sinnvoll notwendig ist.

Aus der Abweichung zwischen
erwartetem und tatsächlichem Verbrauch
lernt das System für die Zukunft.
```