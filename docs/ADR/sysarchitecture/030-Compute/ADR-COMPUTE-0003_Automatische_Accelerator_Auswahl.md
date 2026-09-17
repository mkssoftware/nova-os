# ADR-COMPUTE-0003 – Automatische Accelerator-Auswahl

## Status

Angenommen

## Kategorie

Compute Architecture / Provider Selection

## Kontext

NovaOS kann dieselbe Compute Capability über unterschiedliche Provider ausführen:

```text
CPU
GPU
NPU
DSP
FPGA
Remote Accelerator
```

Der Consumer soll nicht selbst entscheiden müssen, welche Hardware für eine Berechnung optimal geeignet ist.

Eine feste Zuordnung würde Hardwareabhängigkeiten erzeugen und zukünftige Beschleuniger unnötig ausschließen.

## Entscheidung

Die Unified Compute Runtime übernimmt die **automatische Accelerator-Auswahl**.

```text
Compute Request
      ↓
Candidate Providers
      ↓
Hard Constraints
      ↓
Valid Providers
      ↓
Cost Evaluation
      ↓
Selected Provider
```

Die Auswahl erfolgt dynamisch anhand des Compute Contracts und des aktuellen Systemzustands.

## Candidate Providers

Zunächst werden alle Provider bestimmt, welche die benötigte Compute Capability unterstützen.

```text
Required Capability
       ↓
CPU GPU NPU DSP FPGA
       ↓
Compatible Candidates
```

Nicht kompatible Provider werden ausgeschlossen.

## Hard Constraints

Anschließend werden verpflichtende Anforderungen geprüft.

Beispiele:

```text
Correctness
Security
Trust
Data Sovereignty
Determinism
Realtime / Deadline
Required Precision
Required Hardware Features
```

Ein Provider, der einen Hard Constraint nicht erfüllt, darf nicht ausgewählt werden.

## Auswahlkriterien

Innerhalb der gültigen Kandidaten können unter anderem berücksichtigt werden:

```text
Execution Time
Transfer Cost
Memory Usage
Energy
Current Load
Startup Cost
Data Locality
Resource Budget
```

Die theoretisch schnellste Hardware ist daher nicht automatisch die beste Wahl.

## Datenbewegung

Die Kosten notwendiger Datenübertragungen sind Teil der Auswahl.

```text
Compute Cost
     +
Transfer Cost
     ↓
Effective Cost
```

Eine GPU-Ausführung kann beispielsweise verworfen werden, wenn der Datentransfer mehr kostet als die CPU-Ausführung.

## Kleine Workloads

Für kleine Berechnungen kann die CPU trotz langsamerer theoretischer Rechenleistung bevorzugt werden.

```text
Small Workload

CPU:
Low Startup Cost

GPU:
Transfer + Startup Cost

→ CPU
```

## Provider Load

Die aktuelle Auslastung darf berücksichtigt werden.

```text
GPU 0 = Busy
GPU 1 = Available
CPU   = Available
```

Die Auswahl soll auf dem tatsächlichen Ressourcenstatus basieren.

## Fallback

Wird der ausgewählte Accelerator vor der Ausführung ungültig, kann die Runtime neu planen.

```text
Selected Provider
       ↓
Unavailable
       ↓
Replanning
       ↓
Alternative Provider
```

Fallback ist nur zulässig, wenn der Compute Contract weiterhin erfüllt wird.

## Explizite Auswahl

Consumer können einen Provider explizit verlangen, wenn dies fachlich notwendig ist.

```text
Preferred Provider = GPU
```

oder:

```text
Required Provider = FPGA
```

`Preferred` bleibt eine Soft Constraint.

`Required` wird als Hard Constraint behandelt.

## Lernende Optimierung

NovaOS darf historische Messwerte zur Verbesserung zukünftiger Auswahlentscheidungen verwenden.

```text
Prediction
    ↓
Execution
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Model Adjustment
```

Adaptive Auswahl darf jedoch niemals Hard Constraints überschreiben.

## Normative Anforderungen

1. NovaOS MUSS Compute Provider automatisch auswählen können.
2. Die Auswahl MUSS auf dem Compute Contract basieren.
3. Nur semantisch kompatible Provider DÜRFEN berücksichtigt werden.
4. Hard Constraints MÜSSEN vor Optimierungszielen geprüft werden.
5. Datenbewegungs- und Startup-Kosten MÜSSEN berücksichtigt werden können.
6. Aktuelle Ressourcenverfügbarkeit SOLL berücksichtigt werden.
7. Provider-Auswahl DARF nicht ausschließlich auf theoretischer Rechenleistung basieren.
8. Fallback MUSS möglich sein, sofern der Compute Contract dies erlaubt.
9. Consumer MÜSSEN Provider bevorzugen oder explizit verlangen können.
10. Adaptive Optimierung DARF keine Hard Constraints überschreiben.
11. Deterministic Mode MUSS reproduzierbare Provider-Auswahl ermöglichen.

## Konsequenzen

### Positive Konsequenzen

- Hardwarebeschleuniger können automatisch optimal genutzt werden,
- Anwendungen bleiben hardwareunabhängig,
- Transfer- und Startup-Kosten werden berücksichtigt,
- neue Accelerator-Typen können ohne Änderungen an Consumern integriert werden.

### Negative Konsequenzen

- die Runtime benötigt aktuelle Leistungs- und Ressourceninformationen,
- Auswahlentscheidungen verursachen zusätzlichen Planungsaufwand.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_CPU_GPU_NPU_DSP_FPGA_als_Compute_Provider`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-ACCELERATOR-SELECTION-0001`
- `NPSPEC-COMPUTE-COST-MODEL-0001`
- `NPSPEC-COMPUTE-PROVIDER-FALLBACK-0001`

## Ergebnis

NovaOS wählt Compute Hardware anhand der tatsächlichen Anforderungen einer Berechnung:

```text
Compute Request
      ↓
Compatible Providers
      ↓
Hard Constraints
      ↓
Cost + Resources + Locality
      ↓
Provider Selection
      ↓
Execution
      ↓
Measured Result
```

Der zentrale Grundsatz lautet:

```text
Der Consumer wählt nicht
CPU, GPU oder NPU.

Er beschreibt die Berechnung.

NovaOS wählt den geeigneten
Compute Provider.
```