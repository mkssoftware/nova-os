# NPSPEC-ADAPTIVE-POWER-0001 – Nova Adaptive Power

## Status

Angenommen

## Kategorie

Adaptive System / Power / Energy / Runtime Optimization

## Zweck

NovaOS definiert eine adaptive Energieverwaltung, die aktuelle Systemlast, Energiezustand, thermische Bedingungen, Vorhersagen und Feedback nutzt, um den Energieverbrauch dynamisch zu optimieren.

```text
Observe
   ↓
Predict Workload
   ↓
Evaluate Constraints
   ↓
Adaptive Power Decision
   ↓
Hardware / Execution Adjustment
   ↓
Feedback
```

Adaptive Power ergänzt die reguläre Energieverwaltung und darf Performance-, Deadline-, Realtime- oder Sicherheitsanforderungen nicht unkontrolliert überschreiben.

## Grundprinzipien

```text
Power Saving ≠ Highest Priority
Prediction ≠ Future Load
Low Utilization ≠ Idle
Idle ≠ Safe Power-Off
Energy Optimization ≠ Performance Guarantee
Power State ≠ Execution State
Adaptive Decision ≠ Hardware Authority
```

## Adaptive Power Model

```text
AdaptivePowerState
├── PowerSource
├── EnergyAvailable
├── CurrentConsumption
├── PowerBudget
├── PerformanceDemand
└── ThermalState
```

Optional:

```text
BatteryState
PredictedDemand
ExecutionID
ResourceID
DeviceID
ProviderID
NodeID
PredictionID
PolicyVersion
```

## Power Targets

Adaptive Power kann steuern oder beeinflussen:

```text
CPU
GPU
NPU
Memory
Storage
Network
Devices
Display
Accelerators
Platform Power States
```

Die tatsächliche Hardwaresteuerung erfolgt über autorisierte HAL-, Driver- und Firmware-Mechanismen.

## Workload Prediction

NovaOS kann zukünftige Lasten abschätzen.

```text
Current Load
     +
Execution History
     +
Expected Work
     ↓
Predicted Demand
```

Damit können Energiezustände vorbereitet werden, bevor sich die Last tatsächlich ändert.

## Performance States

Adaptive Power kann geeignete Performance States auswählen.

```text
High Demand
    ↓
High Performance State

Low Demand
    ↓
Efficient State
```

Mögliche Mechanismen:

```text
Frequency Scaling
Voltage Scaling
Clock Gating
Device Power States
Idle States
Accelerator Power States
```

## Predictive Wakeup

Komponenten können vor erwarteter Nutzung rechtzeitig aktiviert werden.

```text
Predicted Usage
      ↓
Wakeup Preparation
      ↓
Component Ready
      ↓
Execution
```

Damit kann Energie gespart werden, ohne unnötige Wakeup-Latenz zu erzeugen.

## Predictive Sleep

Bei erwarteter längerer Inaktivität kann ein tieferer Energiesparzustand gewählt werden.

```text
Predicted Idle Duration
        ↓
Break-even Evaluation
        ↓
Select Power State
```

Dabei müssen berücksichtigt werden:

```text
Entry Cost
Exit Cost
Wakeup Latency
Expected Idle Time
Energy Saving
```

## Break-even

Ein tieferer Power State ist nur sinnvoll, wenn:

```text
Expected Energy Saving
        >
Transition Cost
```

Kurze Idle-Phasen dürfen dadurch nicht zu ineffizienten Zustandswechseln führen.

## Execution Contract

Execution Contracts können Energieanforderungen definieren.

Beispiele:

```text
Energy Budget
Latency Requirement
Deadline
Performance Requirement
Realtime Requirement
```

Adaptive Power muss diese Anforderungen bei Entscheidungen berücksichtigen.

## Priority

Die Entscheidungsreihenfolge bleibt:

```text
Safety
  ↓
Security
  ↓
Realtime / Deadline
  ↓
Hard Execution Constraints
  ↓
Explicit User Policy
  ↓
Resource Guarantees
  ↓
Energy Optimization
```

## Thermal Integration

Energie- und Temperaturmanagement müssen zusammenarbeiten.

```text
Power
  ↓
Heat
  ↓
Thermal State
  ↓
Adaptive Power Decision
```

Thermische Sicherheitsgrenzen besitzen Vorrang vor Performanceoptimierung.

## Battery Operation

Bei batteriebetriebenen Geräten können Policies stärker auf Energieeffizienz ausgerichtet werden.

Berücksichtigt werden können:

```text
Battery Level
Discharge Rate
Remaining Runtime
Charging State
Expected Workload
User Power Mode
```

## Adaptive Scheduling

Der Adaptive Scheduler kann Energieinformationen verwenden.

```text
Workload
   ↓
Scheduler
   +
Power State
   ↓
Energy-aware Placement
```

Beispiele:

```text
Consolidate Workloads
Wake Fewer Cores
Use Efficient Cores
Select Efficient Accelerator
Avoid Thermal Hotspots
```

## Distributed Power

In verteilten Systemen kann Placement Energiezustände verschiedener Nodes berücksichtigen.

```text
Execution
    ↓
Candidate Nodes
    ↓
Performance + Energy + Constraints
    ↓
Placement
```

Energy Optimization darf dabei Sovereignty-, Trust- oder Location-Anforderungen nicht überschreiben.

## Feedback

Adaptive Power bewertet reale Auswirkungen:

```text
Power Decision
     ↓
Execution
     ↓
Measured Energy
     ↓
Performance Result
     ↓
Feedback
```

Messbar sind beispielsweise:

```text
Energy Saved
Latency Impact
Deadline Impact
Wakeup Cost
Idle Efficiency
Thermal Impact
Prediction Accuracy
```

## Policy Learning

Kontrolliert lernbar können sein:

```text
Idle Threshold
Wakeup Threshold
Performance State Preference
Power State Selection
Prediction Weight
Energy / Performance Balance
```

Hardware-Sicherheitsgrenzen und Hard Constraints bleiben unveränderlich.

## Stability

Adaptive Power muss schnelle Zustandsoszillation verhindern.

```text
High Power
   ↓
Low Power
   ↓
High Power
   ↓
Low Power
```

Mechanismen können sein:

```text
Hysteresis
Cooldown
Minimum Residency
Transition Cost
Confidence Threshold
Rate Limiting
```

## Safe Fallback

Bei:

```text
Prediction Failure
Policy Failure
Missing Telemetry
Unsupported Hardware
Adaptive Instability
```

muss NovaOS auf eine sichere reguläre Power Policy zurückfallen.

```text
Adaptive Power unavailable
          ↓
Base Power Management
```

## Security

Adaptive Power darf keine Hardware-Authority erzeugen.

```text
Prediction ≠ Device Capability
Power Policy ≠ Hardware Permission
```

Hardwareänderungen benötigen weiterhin die erforderlichen Capabilities.

## Resource Economy

Adaptive Power selbst besitzt Kosten.

```text
Prediction Cost
Monitoring Cost
Transition Cost
Learning Cost
```

Diese Kosten müssen gegen den erwarteten Energiegewinn abgewogen werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
Power State
Energy Consumption
Power Budget
Battery State
Predicted Demand
Selected Performance State
Decision Reason
Transition Cost
Energy Saved
Latency Impact
Thermal Impact
Prediction Error
Policy Version
Fallback State
```

## Normative Anforderungen

1. NovaOS MUSS Adaptive Power von der grundlegenden Energieverwaltung trennen.
2. Base Power Management MUSS ohne adaptive Komponenten vollständig funktionieren.
3. Prediction DARF NICHT als garantierte zukünftige Last behandelt werden.
4. Adaptive Power MUSS Safety-, Security-, Realtime- und Deadline-Anforderungen respektieren.
5. Hardware-Sicherheitsgrenzen DÜRFEN NICHT durch adaptive Policies verändert werden.
6. Power-State-Wechsel SOLLEN Transition Cost und Wakeup Latency berücksichtigen.
7. Predictive Wakeup und Predictive Sleep SOLLEN unterstützt werden können.
8. Tiefe Power States SOLLEN anhand ihres Break-even-Punktes bewertet werden.
9. Energy Optimization MUSS Execution Contracts berücksichtigen.
10. Thermal State MUSS in relevante Power-Entscheidungen einfließen können.
11. Adaptive Scheduling SOLL Energieinformationen verwenden können.
12. Distributed Placement DARF Energiezustände berücksichtigen.
13. Adaptive Power MUSS gegen schnelle Zustandsoszillation begrenzbar sein.
14. Power-Entscheidungen SOLLEN Feedback erzeugen.
15. Adaptive Power-Parameter DÜRFEN kontrolliert gelernt werden.
16. Adaptive Power DARF keine zusätzliche Hardware-Authority erzeugen.
17. Bei Ausfall adaptiver Mechanismen MUSS eine sichere Base Power Policy verfügbar bleiben.
18. Adaptive Power MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-PREDICTIONERROR-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-ADAPTIVE-POLICYLEARNING-0001`
- `NPSPEC-ADAPTIVE-SCHEDULER-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-EXECUTION-ENERGY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `ADR-ARCH-0089`

## Ergebnis

```text
System State
     +
Expected Workload
     +
Energy State
     ↓
Adaptive Power Decision
     ↓
Performance / Power State
     ↓
Execution
     ↓
Energy + Performance Measurement
     ↓
Feedback
     ↺
```

NovaOS erhält damit eine adaptive Energieverwaltung, die Energieverbrauch und Performance vorausschauend ausbalancieren kann. Hardwarezustände können passend zur erwarteten Last vorbereitet werden, während Safety, Realtime, Deadlines, Execution Contracts und Hardwaregrenzen jederzeit Vorrang behalten.