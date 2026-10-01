# NPSPEC-EXECUTION-ENERGY-0001 – Nova Execution Energy

## Status

Angenommen

## Kategorie

Execution / Energy / Resource-Aware Execution

## Zweck

NovaOS definiert Energieanforderungen als expliziten Bestandteil des Execution Contracts.

Damit kann eine Operation festlegen, wie Energieverbrauch bei Algorithmus-, Provider-, Hardware- und Ressourcenwahl berücksichtigt werden soll.

```text
ExecutionContract
      ↓
Energy Requirements
      ↓
Algorithm + Provider Planning
      ↓
Execution
      ↓
Energy Accounting
      ↓
Verification
```

## Grundprinzipien

```text
Energy ≠ Power
Energy ≠ Performance
Energy ≠ Thermal State
Energy Budget ≠ Energy Guarantee
Low Energy ≠ Lowest Latency
Energy Optimization ≠ Permission to violate Constraints
```

Energieoptimierung ist grundsätzlich nachrangig gegenüber Hard Requirements.

## Energy Requirement

Ein Execution Contract kann enthalten:

```text
ExecutionEnergyRequirement
├── Energy Policy
└── Requirement Class
```

Optional:

```text
Maximum Energy
Energy Budget
Maximum Power
Preferred Efficiency
Minimum Runtime Reserve
Energy Source Preference
Battery Constraints
Provider Constraints
Fallback Policy
```

## Requirement Classes

NovaOS unterscheidet:

```text
Required
Preferred
NotRequired
```

Bei `Required` müssen definierte Energiegrenzen eingehalten werden.

Bei `Preferred` wird Energieverbrauch als Optimierungsziel verwendet.

## Energy Policies

Beispiele:

```text
MinimizeEnergy
MinimizePower
PreferEfficiency
PreserveBattery
PerformanceBalanced
NoEnergyPreference
```

Eine Policy darf keine höher priorisierten Constraints überschreiben.

## Energy Budget

Ein Execution Contract kann einen maximalen Energieverbrauch definieren.

```text
Energy Budget:
20 J
```

Während der Ausführung gilt:

```text
Remaining Energy =
Energy Budget
-
Consumed Energy
```

Der Verbrauch wird über Resource Accounting erfasst.

## Energy und Power

NovaOS trennt:

```text
Energy = verbrauchte Energiemenge
Power  = Energieverbrauch pro Zeit
```

Eine Operation kann beispielsweise wenig Energie verbrauchen, aber kurzfristig hohe Leistung benötigen.

Daher können beide Werte separat begrenzt werden.

## Algorithm Selection

Algorithmen können unterschiedliche Energieprofile besitzen.

```text
Algorithm A
├── Fast
└── High Energy

Algorithm B
├── Slower
└── Low Energy
```

Der Execution Planner darf Algorithm B bevorzugen, wenn:

```text
Energy = Preferred
```

und keine anderen Contract-Anforderungen verletzt werden.

## Provider Selection

Provider können anhand ihrer erwarteten Energieeffizienz verglichen werden.

```text
Operation
   ↓
CPU / GPU / NPU
   ↓
Predicted Energy
   ↓
Provider Selection
```

Beispiel:

```text
CPU → 12 J
GPU → 7 J
NPU → 3 J
```

Der niedrigste Energieverbrauch ist jedoch nicht automatisch die beste Wahl.

Zusätzlich gelten:

```text
Latency
Deadline
Precision
Determinism
Trust
Sovereignty
Security
Resource Budget
```

## Hardware Efficiency

NovaOS kann Hardwareeigenschaften berücksichtigen:

```text
Performance per Watt
Accelerator Efficiency
Memory Transfer Cost
Wake-Up Cost
Idle Cost
Device Power State
```

Dadurch kann vermieden werden, einen Accelerator zu aktivieren, dessen Initialisierung mehr Energie benötigt als die eigentliche Operation einspart.

## Data Movement

Datenbewegung besitzt ebenfalls Energiekosten.

```text
Compute
 +
Memory Transfer
 +
I/O
 +
Network
 =
Execution Energy
```

Provider Selection soll daher die gesamte Ausführung und nicht nur Compute-Energie berücksichtigen.

## Local und Remote Execution

Remote Execution ist nicht automatisch energieeffizienter.

```text
Local Compute
```

muss verglichen werden mit:

```text
Serialization
Network Transfer
Remote Compute
Return Transfer
```

Sovereignty-, Trust- und Security-Anforderungen besitzen weiterhin Vorrang.

## Deadline und Latency

Energieoptimierung darf zeitliche Hard Constraints nicht verletzen.

```text
Energy Optimization
       ↓
Deadline Check
       ↓
Latency Check
       ↓
Eligible Plan
```

Eine langsamere energieeffiziente Ausführung darf nur gewählt werden, wenn die zeitlichen Anforderungen dies erlauben.

## Thermal Integration

Energieverbrauch und thermische Belastung sind verbunden, bleiben aber getrennte Modelle.

```text
Energy Consumption
       ↓
Power
       ↓
Thermal Impact
```

Ein energieeffizienter Plan kann dennoch kurzfristig thermisch ungeeignet sein.

Thermal Constraints werden separat geprüft.

## Battery Awareness

Auf mobilen Systemen kann der Execution Contract berücksichtigen:

```text
Battery Level
Charging State
Remaining Runtime
Energy Source
System Energy Policy
```

Bei niedrigem Energiestand kann NovaOS beispielsweise:

```text
Reduce Parallelism
Prefer Efficient Provider
Delay Background Work
Reduce Optional Work
```

Hard Requirements bleiben erhalten.

## Runtime Monitoring

Während der Ausführung:

```text
Predicted Energy
      ↓
Measured Energy
      ↓
Remaining Budget
```

Zustände können sein:

```text
Normal
ApproachingLimit
AtRisk
Exceeded
```

## Replanning

Droht eine Überschreitung:

```text
Energy AtRisk
      ↓
Replan
```

Mögliche Maßnahmen:

```text
Change Algorithm
Change Provider
Reduce Parallelism
Reduce Optional Work
Delay Noncritical Work
Graceful Degradation
```

Nur vom Execution Contract erlaubte Änderungen sind zulässig.

## Adaptive Optimization

NovaOS kann vergangene Messwerte verwenden.

```text
Predicted Energy
       ↓
Execution
       ↓
Measured Energy
       ↓
Prediction Error
       ↓
Energy Model Update
```

Dadurch können zukünftige Provider- und Algorithmusentscheidungen verbessert werden.

Vorhersagen sind keine Garantien.

## Safety

Die Priorität bleibt:

```text
Hardware Safety
      ↓
Security
      ↓
Sovereignty / Trust
      ↓
Hard Execution Constraints
      ↓
Explicit User Decisions
      ↓
Energy Optimization
```

Energieeinsparung darf keine Sicherheitsanforderung abschwächen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Energy Policy
Energy Budget
Consumed Energy
Remaining Energy
Power Limit
Predicted Energy
Measured Energy
Selected Algorithm
Selected Provider
Energy Source
Energy State
Violation State
```

## Normative Anforderungen

1. NovaOS MUSS Energieanforderungen als Bestandteil von Execution Contracts unterstützen.
2. Energy und Power MÜSSEN getrennte Konzepte bleiben.
3. Execution Energy MUSS unabhängig von Thermal State modelliert werden.
4. Hard Energy Limits DÜRFEN nicht stillschweigend abgeschwächt werden.
5. Algorithmus- und Provider-Auswahl SOLLEN Energieverbrauch berücksichtigen können.
6. Datenbewegung SOLL bei der Bewertung des Gesamtenergieverbrauchs berücksichtigt werden.
7. Energieoptimierung DARF Deadline-, Latency-, Security-, Trust- oder Sovereignty-Constraints NICHT verletzen.
8. Energy Budgets MÜSSEN mit Resource Accounting integrierbar sein.
9. NovaOS SOLL Battery- und Energy-Source-Zustände berücksichtigen können.
10. Runtime Replanning DARF nur innerhalb des Execution Contracts erfolgen.
11. Adaptive Energy Prediction DARF Hard Constraints NICHT überschreiben.
12. Energieverbrauch und Energy State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-EXECUTION-ALGORITHM-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0058`

## Ergebnis

```text
ExecutionContract
      ↓
Energy Requirements
      ↓
Algorithm + Provider Evaluation
      ↓
Energy-Aware Execution Plan
      ↓
Execution + Accounting
      ↓
Measurement + Replanning
```

NovaOS erhält damit ein durchgängiges Execution-Energy-Modell, bei dem Energieverbrauch bereits bei Algorithmus-, Provider- und Ressourcenwahl berücksichtigt und während der Ausführung gemessen und kontrolliert wird, ohne Energieoptimierung über Safety-, Security- oder andere Hard Constraints zu stellen.