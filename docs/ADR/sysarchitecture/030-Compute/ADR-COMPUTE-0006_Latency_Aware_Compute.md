# ADR-COMPUTE-0006 – Latency-Aware Compute

## Status

Angenommen

## Kategorie

Compute Architecture / Latency

## Kontext

Compute Provider unterscheiden sich bei ihrer tatsächlichen End-to-End-Latenz.

```text
CPU
GPU
NPU
DSP
FPGA
Remote Compute
```

Die reine Rechenleistung eines Providers reicht nicht aus, um seine Eignung zu bestimmen.

Startup, Scheduling, Datenübertragung und Netzwerkkommunikation können die tatsächliche Latenz erheblich beeinflussen.

## Entscheidung

NovaOS unterstützt **Latency-Aware Compute**.

Die erwartete End-to-End-Latenz wird bei der Compute-Planung berücksichtigt.

```text
Compute Request
      ↓
Latency Requirement
      ↓
Candidate Providers
      ↓
Latency Evaluation
      ↓
Provider Selection
```

## End-to-End-Latenz

NovaOS betrachtet nicht nur die eigentliche Rechenzeit.

```text
Total Latency =
    Queueing
  + Startup
  + Data Transfer
  + Execution
  + Synchronization
  + Result Transfer
```

Für Remote Compute kommen insbesondere Netzwerkzeiten hinzu.

## Latency Contract

Ein Execution Contract kann Latenzanforderungen definieren.

Beispiele:

```text
Preferred Latency
Maximum Latency
Deadline
Interactive
Background
```

Eine verpflichtende maximale Latenz wird als Hard Constraint behandelt.

## Provider-Auswahl

Provider werden anhand ihrer erwarteten Gesamtlatenz bewertet.

```text
CPU:
    4 ms execution
    0.2 ms overhead

GPU:
    1 ms execution
    6 ms transfer

Result:
    CPU preferred
```

Der theoretisch schnellste Accelerator ist daher nicht automatisch der Provider mit der niedrigsten Gesamtlatenz.

## Datenlokalität

Datenlokalität ist Bestandteil der Latenzbewertung.

```text
Data Location
      +
Compute Location
      ↓
Transfer Latency
```

Compute und Datenplatzierung sollen gemeinsam betrachtet werden.

## Queueing

Aktuelle Provider-Auslastung kann zusätzliche Wartezeit verursachen.

```text
Fast GPU
   +
Long Queue
   ↓
High Effective Latency
```

Die Runtime soll deshalb nicht ausschließlich statische Hardwarewerte verwenden.

## Remote Compute

Remote Provider müssen Netzwerk- und Kommunikationslatenz berücksichtigen.

```text
Request Transfer
      +
Remote Queue
      +
Execution
      +
Result Transfer
      ↓
Remote Latency
```

Remote Compute darf nicht gewählt werden, wenn dadurch verpflichtende Latenz- oder Deadline-Anforderungen verletzt werden.

## Messung und Lernen

NovaOS kann reale Ausführungszeiten zur Verbesserung zukünftiger Schätzungen verwenden.

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

Adaptive Schätzungen dürfen Hard Constraints nicht abschwächen.

## Replanning

Ändert sich die erwartete Latenz vor der Ausführung erheblich, kann NovaOS neu planen.

```text
Provider Congested
       ↓
Latency Contract at Risk
       ↓
Replanning
```

Bereits laufende Operationen dürfen nur migriert oder wiederholt werden, wenn ihre Operationssemantik dies erlaubt.

## Normative Anforderungen

1. NovaOS MUSS End-to-End-Latenz bei Compute-Entscheidungen berücksichtigen können.
2. Execution Contracts MÜSSEN Latenzanforderungen ausdrücken können.
3. Harte Latenzgrenzen MÜSSEN als Hard Constraints behandelt werden.
4. Startup-, Transfer-, Queueing- und Execution-Latenz MÜSSEN berücksichtigt werden können.
5. Datenlokalität SOLL in die Latenzbewertung einfließen.
6. Aktuelle Provider-Auslastung SOLL berücksichtigt werden.
7. Remote Compute MUSS Netzwerk- und Transferlatenz berücksichtigen.
8. Provider Selection DARF nicht ausschließlich auf theoretischer Rechenleistung basieren.
9. Gemessene Latenzen DÜRFEN zur Verbesserung zukünftiger Entscheidungen verwendet werden.
10. Adaptive Optimierung DARF verpflichtende Latenz- oder Deadline-Contracts nicht verletzen.
11. Replanning MUSS möglich sein, wenn ein Provider den Contract voraussichtlich nicht mehr erfüllen kann.

## Konsequenzen

### Positive Konsequenzen

- interaktive Aufgaben reagieren schneller,
- Accelerator-Overhead wird realistisch berücksichtigt,
- Datenlokalität fließt in Compute-Entscheidungen ein,
- Remote Compute wird nur genutzt, wenn seine Gesamtlatenz geeignet ist.

### Negative Konsequenzen

- Latenz muss laufend geschätzt oder gemessen werden,
- dynamische Auslastung kann häufigere Replanning-Entscheidungen verursachen.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-LATENCY-0001`
- `NPSPEC-COMPUTE-LATENCY-CONTRACT-0001`
- `NPSPEC-COMPUTE-LATENCY-METRICS-0001`
- `NPSPEC-COMPUTE-LATENCY-PREDICTION-0001`

## Ergebnis

NovaOS bewertet Compute Provider anhand der tatsächlich erwarteten End-to-End-Latenz:

```text
Compute Request
      ↓
Latency Contract
      ↓
Execution + Transfer + Queueing
      ↓
Valid Providers
      ↓
Provider Selection
      ↓
Measured Latency
```

Der zentrale Grundsatz lautet:

```text
Nicht die schnellste Hardware
liefert automatisch das schnellste Ergebnis.

Entscheidend ist die gesamte Zeit
vom Auftrag bis zum nutzbaren Resultat.
```