# NPSPEC-RESOURCE-THERMAL-0001 – Nova Thermal Resource Model

## Status

Angenommen

## Kategorie

Resource / Thermal / Resource Management

## Zweck

NovaOS definiert thermische Kapazität als explizite, messbare und kontrollierbare Systemressource.

```text
Workload
   ↓
Power Consumption
   ↓
Heat Generation
   ↓
Thermal State
   ↓
Resource Policy
```

Das Thermal Resource Model verhindert, dass Performance-Optimierung, Parallelität oder Hardwarebeschleunigung sichere thermische Betriebsgrenzen überschreiten.

## Grundprinzipien

```text
Temperature ≠ Thermal Capacity
Power ≠ Temperature
Energy ≠ Heat
Thermal Limit ≠ Performance Target
Thermal Headroom ≠ Unlimited Compute Capacity
Cooling Capacity ≠ Compute Authority
Thermal Throttling ≠ Failure
Sensor Value ≠ Guaranteed Exact Temperature
```

## Thermal Resource

Eine thermische Ressource wird beschrieben durch:

```text
ThermalResource
├── ResourceID
├── ResourceTypeID
├── Thermal Domain
├── State
└── Thermal Headroom
```

Optional:

```text
Temperature
Warning Threshold
Critical Threshold
Cooling Capacity
Power Limit
Sensor Quality
Affected Resources
Thermal Trend
```

## Thermal Domains

Hardware kann in thermische Domains gruppiert werden.

```text
System
├── CPU Domain
├── GPU Domain
├── NPU Domain
├── Memory Domain
└── Device Domain
```

Mehrere Ressourcen können dieselbe thermische Grenze beeinflussen.

```text
CPU ─┐
GPU ─┼→ Shared Cooling Domain
NPU ─┘
```

## Thermal States

NovaOS definiert mindestens:

```text
Normal
Elevated
Constrained
Critical
Emergency
```

State-Übergänge sollen Hysterese unterstützen, damit kurzfristige Temperaturschwankungen keine permanenten Policy-Wechsel erzeugen.

## Thermal Headroom

Thermal Headroom beschreibt die verbleibende thermische Belastbarkeit.

```text
Current Thermal State
        ↓
Thermal Limits
        ↓
Available Headroom
```

Headroom kann bei Resource Selection und Scheduling berücksichtigt werden.

## Thermal Budget

Workloads können indirekt thermische Budgets erhalten.

```text
ThermalBudget
├── Thermal Domain
├── Allowed Load
├── Time Window
└── Enforcement Policy
```

Thermal Budgets können mit Energy- und Power-Budgets gekoppelt werden.

## Thermal Accounting

Thermische Belastung kann soweit technisch sinnvoll Workloads zugerechnet werden.

```text
Workload
   ↓
Resource Usage
   ↓
Power Estimate
   ↓
Thermal Contribution
```

Die Messqualität muss kenntlich gemacht werden:

```text
Exact
Estimated
Sampled
Derived
Unknown
```

## Scheduling

Der Scheduler kann Thermal State bei Placement-Entscheidungen berücksichtigen.

```text
Hot CPU
   ↓
Task Migration
   ↓
Cooler CPU
```

Mögliche Maßnahmen:

```text
Migration
Frequency Reduction
Reduced Parallelism
Accelerator Change
Workload Delay
Load Distribution
```

## CPU, GPU und NPU

Compute-Ressourcen liefern thermische Zustände an die Resource Economy.

```text
CPU ─┐
GPU ─┼→ Thermal Model
NPU ─┘
       ↓
Resource Selection
```

NovaOS kann dadurch einen alternativen Provider wählen, sofern der ExecutionContract dies erlaubt.

## Cooling

Aktive Kühlung kann als steuerbare Hardwarefunktion betrachtet werden.

Beispiele:

```text
Fan Speed
Pump Control
Platform Cooling State
```

Direkte Steuerung benötigt entsprechende Capabilities.

## Thermal Pressure

Steigende thermische Belastung erzeugt Thermal Pressure.

```text
Normal
 ↓
Elevated
 ↓
Constrained
 ↓
Critical
 ↓
Emergency
```

Mögliche Reaktionen:

```text
Reduce Boost
Throttle
Migrate
Reduce Parallelism
Pause Background Work
Disable Accelerator
Emergency Shutdown
```

## Safety

Thermische Sicherheitsgrenzen besitzen Vorrang vor Performance- und Komfortzielen.

```text
Hardware Safety
      ↓
Critical Thermal Constraints
      ↓
Realtime / Hard Requirements
      ↓
Performance
      ↓
Energy Optimization
```

Kann ein Workload aufgrund thermischer Grenzen nicht sicher ausgeführt werden, muss NovaOS ihn begrenzen, verschieben oder kontrolliert abbrechen.

## ExecutionContract

Ein ExecutionContract kann thermische Anforderungen enthalten.

```text
ExecutionContract
├── Thermal Constraints
├── Power Budget
├── Energy Budget
├── Performance Requirement
└── Deadline
```

Thermal Constraints können die Auswahl von CPU, GPU, NPU oder anderen Accelerators einschränken.

## Sensoren

Thermische Entscheidungen können auf mehreren Sensoren basieren.

```text
CPU Sensor
GPU Sensor
Board Sensor
Battery Sensor
Device Sensor
```

Fehlende oder ungültige Sensordaten dürfen nicht automatisch als sicher interpretiert werden.

```text
Unknown Thermal State ≠ Safe Thermal State
```

## Capability Security

Privilegierte Thermal-Control-Operationen benötigen explizite Capabilities.

Beispiele:

```text
Thermal Policy Control
Fan Control
Power Limit Control
Frequency Control
Emergency Override
```

Normale Workloads erhalten keine direkte Kontrolle über globale Thermal Policies.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Thermal Domains
Thermal State
Temperature
Thermal Headroom
Thresholds
Cooling State
Affected Resources
Thermal Pressure
Measurement Quality
Active Mitigations
```

## Normative Anforderungen

1. NovaOS MUSS thermische Belastung als systemweite Ressourcenbegrenzung behandeln.
2. Thermal Domains MÜSSEN mehrere gemeinsam gekoppelte Ressourcen darstellen können.
3. Thermal States MÜSSEN mindestens Normal, Elevated, Constrained, Critical und Emergency unterscheiden können.
4. Thermal Headroom SOLL für Resource Selection und Scheduling verfügbar sein.
5. Thermische Messwerte MÜSSEN ihre Messqualität darstellen können.
6. Unbekannte Thermal States DÜRFEN nicht automatisch als sicher behandelt werden.
7. Thermal Policies MÜSSEN CPU-, GPU-, NPU- und Energy-Management beeinflussen können.
8. Hardware-Sicherheitsgrenzen MÜSSEN Vorrang vor Performance-Optimierungen besitzen.
9. Privilegierte Thermal-Control-Funktionen MÜSSEN Capability-kontrolliert sein.
10. Thermal Resources MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SCHED-THERMAL-0001`
- `NPSPEC-SCHED-ENERGY-0001`
- `NPSPEC-SCHED-HETEROGENEOUS-0001`
- `ADR-ARCH-0040`

## Ergebnis

```text
Compute Load
     ↓
Power + Heat
     ↓
Thermal Measurement
     ↓
Thermal Pressure
     ↓
Scheduling + Resource Policy
     ↓
Safe Controlled Execution
```

NovaOS erhält damit ein systemweites Thermal Resource Model, das thermische Grenzen direkt in Resource Economy, Scheduling und Hardwareauswahl integriert und sichere Ausführung gegenüber maximaler Performance priorisiert.