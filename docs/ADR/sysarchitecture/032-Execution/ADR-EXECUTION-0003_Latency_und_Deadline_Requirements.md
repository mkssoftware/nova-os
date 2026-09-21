# ADR-EXECUTION-0003 – Latency und Deadline Requirements

## Status

Angenommen

## Kategorie

Execution Architecture / Timing Contracts

## Kontext

Für viele NovaOS-Ausführungen ist nicht nur das Ergebnis relevant, sondern auch **wann** dieses Ergebnis verfügbar sein muss.

Beispiele:

```text
UI Interaction
Audio Processing
Video Processing
Simulation
Industrial Control
Background Tasks
Realtime Workloads
```

Eine reine Optimierung auf maximale Performance reicht nicht aus.

NovaOS benötigt deshalb explizite Latency- und Deadline-Anforderungen im `Nova.ExecutionContract`.

## Entscheidung

`Nova.ExecutionContract` unterstützt explizite **Latency Requirements** und **Deadlines**.

```text
Execution Request
       ↓
Latency / Deadline Requirements
       ↓
Execution Planning
       ↓
Algorithm + Provider + Location
       ↓
Execution
       ↓
Timing Verification
```

Dabei wird zwischen bevorzugter Latenz und verbindlicher Deadline unterschieden.

## Latency Requirement

Eine gewünschte Latenz beschreibt ein Optimierungsziel.

Beispiel:

```text
Preferred Latency ≤ 20 ms
```

Sie ist standardmäßig ein Soft Constraint.

NovaOS versucht, diese Latenz zu erreichen, darf jedoch davon abweichen, wenn höher priorisierte Anforderungen dies erfordern.

## Maximum Latency

Eine maximale Latenz kann als Hard Constraint definiert werden.

```text
Maximum Latency ≤ 50 ms
```

Kann NovaOS diese Anforderung nicht gewährleisten, darf die Ausführung nicht als vertragskonform geplant werden.

## Deadline

Eine Deadline definiert einen Zeitpunkt, bis zu dem eine Operation abgeschlossen sein muss.

```text
Start
  ↓
Execution
  ↓
Result
  ↓
Deadline
```

Deadlines können relativ oder absolut beschrieben werden.

Beispiele:

```text
Complete within 5 ms
Complete before T
```

## End-to-End Latency

NovaOS betrachtet nicht nur die reine Rechenzeit.

```text
Total Latency =
    Queueing
  + Startup
  + Data Transfer
  + Execution
  + Synchronization
  + Result Transfer
```

Alle relevanten Bestandteile müssen bei der Planung berücksichtigt werden.

## Algorithmusauswahl

Die erwartete Laufzeit eines Algorithmus beeinflusst seine Zulässigkeit.

```text
Valid Algorithms
       ↓
Latency Model
       ↓
Deadline Check
       ↓
Eligible Algorithms
```

Ein theoretisch schneller Algorithmus ist nicht automatisch die beste Wahl, wenn beispielsweise Initialisierung oder Datenübertragung hohe Kosten verursachen.

## Providerwahl

Auch die Providerwahl berücksichtigt die vollständige erwartete Latenz.

```text
CPU
GPU
NPU
Remote Provider
      ↓
End-to-End Latency
      ↓
Contract Matching
```

Beispielsweise kann eine CPU für kleine Workloads schneller reagieren als eine GPU, obwohl die GPU eine höhere Rechenleistung besitzt.

## Remote Execution

Bei Remote Compute müssen zusätzliche Komponenten berücksichtigt werden.

```text
Request Transfer
      +
Remote Queueing
      +
Remote Execution
      +
Result Transfer
```

Netzwerklatenz und entfernte Auslastung sind Bestandteil der erwarteten End-to-End-Latency.

Unsichere Netzwerkbedingungen dürfen nicht als garantierte Deadline-Erfüllung behandelt werden.

## Hard Realtime

Eine Hard-Realtime-Deadline ist ein Hard Constraint.

```text
Hard Deadline
     ↓
Provably / Reliably Satisfiable?
 ├── Yes → Eligible
 └── No  → Reject
```

Best-Effort-Schätzungen reichen für Hard-Realtime-Anforderungen nicht aus.

## Deadline Risk

NovaOS darf erkennen, dass eine bereits laufende Ausführung ihre Deadline wahrscheinlich nicht mehr erfüllen kann.

```text
Execution
    ↓
Progress / Timing Observation
    ↓
Deadline Risk
    ↓
Replan / Abort / Continue
```

Die zulässige Reaktion hängt von der Operationssemantik und dem Execution Contract ab.

Ein automatischer Retry ist nur erlaubt, wenn die Operation dies semantisch zulässt.

## Adaptive Modelle

Gemessene Laufzeiten dürfen zur Verbesserung zukünftiger Latenzprognosen verwendet werden.

```text
Predicted Latency
       ↓
Execution
       ↓
Measured Latency
       ↓
Prediction Error
       ↓
Model Adjustment
```

Adaptive Modelle dürfen Hard Deadlines jedoch niemals abschwächen.

## Unsicherheit

Latenzprognosen können unterschiedliche Vertrauensgrade besitzen.

```text
Measured
Estimated
Unknown
```

Für Hard Constraints gilt:

```text
Unknown ≠ Satisfied
```

Eine unbekannte Deadline-Erfüllung darf nicht als garantiert behandelt werden.

## Timing Verification

Nach der Ausführung kann NovaOS prüfen, ob die zugesicherte Timing-Anforderung eingehalten wurde.

```text
Execution Start
       ↓
Execution End
       ↓
Measured Latency
       ↓
Contract Verification
```

Deadline-Verletzungen müssen als Contract Violation erkennbar sein.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Latency- und Deadline-Anforderungen ausdrücken können.
2. Preferred Latency MUSS als Soft Constraint modellierbar sein.
3. Maximum Latency MUSS als Hard Constraint modellierbar sein.
4. Relative und absolute Deadlines MÜSSEN unterstützt werden können.
5. NovaOS MUSS End-to-End-Latency statt ausschließlich Compute Time berücksichtigen.
6. Algorithmusauswahl MUSS relevante Timing-Anforderungen berücksichtigen.
7. Providerwahl MUSS relevante Timing-Anforderungen berücksichtigen.
8. Remote Execution MUSS Netzwerk-, Queueing- und Transferlatenzen berücksichtigen.
9. Hard-Realtime-Deadlines DÜRFEN nicht auf Best-Effort-Annahmen gestützt werden.
10. `Unknown` DARF einen Hard Timing Constraint nicht erfüllen.
11. Adaptive Modelle DÜRFEN Hard Deadlines nicht abschwächen.
12. Deadline-Risiken SOLLEN während der Ausführung erkennbar sein.
13. Retry oder Replanning MUSS die Operationssemantik berücksichtigen.
14. Messbare Timing-Zusicherungen MÜSSEN nach der Ausführung verifizierbar sein.
15. Deadline-Verletzungen MÜSSEN als Contract Violation sichtbar sein.

## Konsequenzen

### Positive Konsequenzen

- interaktive und zeitkritische Workloads können explizit geplant werden,
- Algorithmus- und Providerwahl berücksichtigen reale End-to-End-Kosten,
- Hard-Realtime-Anforderungen bleiben von Best-Effort-Optimierung getrennt,
- Remote Compute kann realistisch gegen lokale Ausführung bewertet werden.

### Negative Konsequenzen

- Latenzmodelle benötigen aktuelle Messwerte,
- garantierte Deadlines reduzieren die Menge zulässiger Provider,
- verteilte Ausführung erschwert belastbare Timing-Garantien.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-COMPUTE-0006_Latency_Aware_Compute`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-END-TO-END-LATENCY-0001`
- `NPSPEC-EXECUTION-TIMING-VERIFICATION-0001`

## Ergebnis

Latency und Deadline werden zu expliziten Bestandteilen des Execution Contracts:

```text
Execution Request
       ↓
Latency / Deadline
       ↓
Constraint Solving
       ↓
Algorithm
       +
Provider
       +
Location
       ↓
Execution
       ↓
Timing Verification
```

Der zentrale Grundsatz lautet:

```text
NovaOS optimiert nicht nur,
wie schnell etwas berechnet wird.

Der Execution Contract beschreibt,
wann das Ergebnis benötigt wird.

Eine verbindliche Deadline
ist keine Präferenz,
sondern eine Ausführungsbedingung.
```