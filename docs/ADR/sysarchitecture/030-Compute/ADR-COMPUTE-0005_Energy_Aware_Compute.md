# ADR-COMPUTE-0005 – Energy-Aware Compute

## Status

Angenommen

## Kategorie

Compute Architecture / Energy Management

## Kontext

Compute Provider unterscheiden sich nicht nur bei Leistung und Latenz, sondern auch beim Energieverbrauch.

```text
CPU
GPU
NPU
DSP
FPGA
Remote Compute
```

Die schnellste Ausführung ist nicht zwangsläufig die energieeffizienteste.

Insbesondere auf mobilen Geräten, Akkusystemen, Embedded Hardware und energiebegrenzten Systemen muss NovaOS den Energiebedarf bei Compute-Entscheidungen berücksichtigen können.

## Entscheidung

NovaOS unterstützt **Energy-Aware Compute**.

Energie wird als planungsrelevante Ressource in die Compute-Auswahl einbezogen.

```text
Compute Request
      ↓
Valid Providers
      ↓
Performance
Energy
Latency
Resource Cost
      ↓
Provider Selection
```

Energy Optimization ist grundsätzlich eine Soft Constraint und darf keine höheren Anforderungen verletzen.

## Energy State

NovaOS kann den aktuellen Energiezustand berücksichtigen.

Beispiele:

```text
AC Power
Battery Level
Battery Health
Power Budget
Thermal State
Energy Policy
```

Der Zustand kann die Compute-Planung beeinflussen.

## Energy Cost

Compute Provider können Energieeigenschaften bereitstellen.

```text
Provider
 ├── Estimated Energy Cost
 ├── Power State
 ├── Performance State
 └── Efficiency Characteristics
```

Die Runtime kann daraus die erwarteten Kosten einer Ausführung abschätzen.

## Performance pro Energie

Die Auswahl darf nicht ausschließlich auf minimalem Energieverbrauch basieren.

Relevant ist das Verhältnis zwischen:

```text
Execution Time
      +
Energy Consumption
      ↓
Effective Compute Cost
```

Ein schneller Accelerator kann trotz höherer Leistungsaufnahme insgesamt weniger Energie benötigen, wenn die Berechnung deutlich schneller abgeschlossen wird.

## Energy Budget

Ein Execution Contract kann ein Energy Budget definieren.

```text
Energy Budget
      ↓
Compute Planning
      ↓
Valid Providers
```

Ein verpflichtendes Energy Budget wird als Constraint behandelt.

## Power Modes

NovaOS kann unterschiedliche Compute Policies verwenden.

Beispiele:

```text
Performance
Balanced
Efficiency
Battery Critical
```

Diese beeinflussen Soft Constraints der Provider-Auswahl.

Sie dürfen keine Safety-, Security-, Correctness- oder Hard-Realtime-Anforderungen verletzen.

## Thermal State

Energie und Temperatur sind miteinander verbunden.

```text
High Compute Load
      ↓
Temperature
      ↓
Throttling
      ↓
Reduced Performance
```

Thermische Einschränkungen müssen bei längeren oder intensiven Compute-Operationen berücksichtigt werden können.

## Remote Compute

Remote Compute darf nicht automatisch als energiesparend gelten.

```text
Local Compute Energy
        vs.
Network Transfer
+
Remote Compute
```

Netzwerk- und Datenübertragungskosten müssen in die Bewertung einfließen.

## Adaptive Optimierung

NovaOS kann gemessene Ausführungen zur Verbesserung zukünftiger Energieabschätzungen verwenden.

```text
Estimated Energy
      ↓
Execution
      ↓
Measured Energy
      ↓
Prediction Error
      ↓
Model Adjustment
```

Adaptive Optimierung darf ausschließlich innerhalb zulässiger Lösungen arbeiten.

## Normative Anforderungen

1. NovaOS MUSS Energie als planungsrelevante Compute-Ressource behandeln können.
2. Compute Provider SOLLEN Energieeigenschaften bereitstellen können.
3. Der aktuelle Energiezustand SOLL bei der Provider-Auswahl berücksichtigt werden.
4. Execution Contracts MÜSSEN Energy Budgets ausdrücken können.
5. Energy Optimization DARF Hard Constraints nicht überschreiben.
6. Performance und Energieverbrauch SOLLEN gemeinsam bewertet werden.
7. Thermal State MUSS bei relevanten Compute-Entscheidungen berücksichtigt werden können.
8. Remote Compute MUSS Datenübertragungskosten in die Energiebewertung einbeziehen können.
9. Power Modes DÜRFEN die Compute-Auswahl als Policy beeinflussen.
10. Adaptive Energieoptimierung DARF Safety, Security, Correctness, Determinism oder Realtime-Anforderungen nicht verletzen.

## Konsequenzen

### Positive Konsequenzen

- längere Akkulaufzeit auf mobilen Geräten,
- energieeffizientere Nutzung spezialisierter Hardware,
- Compute-Auswahl kann an den aktuellen Energiezustand angepasst werden,
- Energie wird Teil der systemweiten Ressourcenökonomie.

### Negative Konsequenzen

- Energieverbrauch muss geschätzt oder gemessen werden,
- optimale Auswahl hängt teilweise vom aktuellen Hardwarezustand ab.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_CPU_GPU_NPU_DSP_FPGA_als_Compute_Provider`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-ENERGY-0001`
- `NPSPEC-COMPUTE-ENERGY-BUDGET-0001`
- `NPSPEC-COMPUTE-POWER-POLICY-0001`
- `NPSPEC-COMPUTE-ENERGY-METRICS-0001`

## Ergebnis

NovaOS betrachtet Energieverbrauch als Teil der Compute-Planung:

```text
Compute Request
      ↓
Valid Providers
      ↓
Performance + Energy + Thermal State
      ↓
Policy
      ↓
Provider Selection
      ↓
Execution
      ↓
Measured Result
```

Der zentrale Grundsatz lautet:

```text
Nicht maximale Leistung
ist immer optimal.

NovaOS wählt die Rechenressource,
die den benötigten Compute Contract
mit angemessenem Energieaufwand erfüllt.
```