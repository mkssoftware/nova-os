# NPSPEC-POWER-CPUPERFORMANCE-0001 – Nova CPU Performance Management

## Status

Angenommen

## Kategorie

Power / CPU Performance

## Zweck

NovaOS definiert die dynamische Steuerung der CPU-Leistung unter Berücksichtigung von Performancebedarf, Energieverbrauch, thermischen Grenzen und Execution Contracts.

Die Steuerung bleibt von CPU Idle getrennt und abstrahiert architektur- sowie plattformspezifische Mechanismen.

## Grundprinzipien

```text
Performance State ≠ Idle State
Performance Request ≠ Fixed Frequency
Frequency ≠ Performance
Maximum Performance ≠ Optimal Performance
Requested Performance ≠ Delivered Performance
Boost ≠ Guaranteed Performance
```

## Architektur

```text
Execution Contracts
       +
Scheduler
       +
Resource Economy
       ↓
CPU Performance Manager
       ↓
Performance Policy
       ↓
Platform / HAL
       ↓
CPU Hardware
```

## Performance Domain

CPUs können gemeinsame Leistungsdomänen besitzen:

```text
CPUPerformanceDomain
├── DomainID
├── CPUs[]
├── MinPerformance
├── MaxPerformance
├── NominalPerformance
├── CurrentPerformance
├── BoostCapability
├── Constraints
└── State
```

Eine Domain kann gelten für:

```text
Logical CPU
Core
Cluster
Package
SoC Domain
```

## Leistungsanforderung

NovaOS verwendet bevorzugt abstrakte Performance-Level statt direkt festgelegter Frequenzen.

```text
Workload Demand
      +
Execution Contract
      +
Scheduler State
      +
Power Policy
      +
Thermal Headroom
      ↓
Performance Request
```

Die Hardware darf die konkrete Kombination aus Frequenz, Spannung und internen Leistungszuständen bestimmen.

## Performance Levels

Abstrakte Bereiche können sein:

```text
Minimum
Efficiency
Balanced
Nominal
Performance
Maximum
Boost
```

Diese stellen keine festen Taktraten dar.

## Dynamische Anpassung

NovaOS darf CPU-Leistung anhand von:

```text
CPU Utilization
Runnable Tasks
Deadlines
Latency Requirements
Workload Priority
Energy Budget
Thermal Headroom
Power Source
User Policy
```

dynamisch anpassen.

## Boost

Temporäre Boost-Zustände dürfen genutzt werden, sofern:

```text
Hardware Limits
Thermal Limits
Power Budget
Execution Contract
Policy
```

dies zulassen.

Boost darf nicht als dauerhaft verfügbare Leistung betrachtet werden.

## Scheduler-Integration

Der Scheduler darf Performance-Anforderungen für Workloads ausdrücken.

Beispielsweise kann ein interaktiver Task kurzfristig höhere Leistung erhalten, während Hintergrundarbeit energieeffizient ausgeführt wird.

```text
Task Demand
    ↓
Scheduler
    ↓
Performance Hint
    ↓
CPU Performance Manager
```

Ein Hint erzeugt keine Garantie für eine konkrete Frequenz.

## Heterogene CPUs

NovaOS muss Systeme mit unterschiedlichen CPU-Klassen unterstützen können:

```text
Performance Cores
Efficiency Cores
Specialized Cores
Heterogeneous Clusters
```

Performance-Anforderung und CPU-Platzierung dürfen gemeinsam optimiert werden.

## Thermal Integration

```text
Performance
    ↓
Power Consumption
    ↓
Heat
    ↓
Thermal Constraints
    ↓
Effective Performance Limit
```

Thermische Sicherheitsgrenzen besitzen Vorrang vor Performanceanforderungen.

## Hardwaremechanismen

Der Platform Provider darf unterschiedliche Mechanismen verwenden:

```text
ACPI P-States
ACPI CPPC
Hardware-Controlled Performance
Architecture-Specific DVFS
SoC Performance Controllers
```

Die NovaOS-Performance-Schnittstelle bleibt davon unabhängig.

## Fehlerverhalten

Nicht verfügbare Performance-Level müssen auf einen sicheren unterstützten Zustand zurückfallen.

```text
Requested Performance
        ↓ unavailable
Nearest Safe Supported State
```

Unbekannte Hardwaregrenzen dürfen nicht als unbegrenzt interpretiert werden.

## Normative Anforderungen

1. NovaOS MUSS CPU-Performance-Steuerung hardwareunabhängig abstrahieren.
2. CPU Performance und CPU Idle MÜSSEN getrennte Mechanismen bleiben.
3. Performance-Anforderungen SOLLEN als abstrakte Leistungsanforderungen ausdrückbar sein.
4. NovaOS DARF keine feste Beziehung zwischen Frequenz und tatsächlicher Leistung voraussetzen.
5. Scheduler und Execution Contracts MÜSSEN Performance-Anforderungen liefern können.
6. Energie- und Thermal Constraints MÜSSEN berücksichtigt werden.
7. Boost MUSS als temporärer und begrenzter Zustand behandelt werden.
8. Gemeinsame Performance Domains MÜSSEN unterstützt werden können.
9. Heterogene CPU-Topologien MÜSSEN berücksichtigt werden können.
10. Angeforderte und tatsächlich gelieferte Performance MÜSSEN unterscheidbar sein.
11. Nicht verfügbare Zustände MÜSSEN auf sichere unterstützte Zustände zurückfallen können.
12. Performance Domain, Anforderungen, Limits und effektiver Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-ACPI-0001`
- `NPSPEC-POWER-CPUIDLE-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-THROTTLING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS kann CPU-Leistung dynamisch an Workload, Scheduler, Energiezustand und thermische Grenzen anpassen. Anwendungen und Systemkomponenten arbeiten mit abstrakten Performance-Anforderungen, während plattformspezifische Mechanismen wie DVFS, P-States oder hardwaregesteuerte Performance hinter der Platform-Power-Schicht verborgen bleiben.