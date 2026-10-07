# NPSPEC-POWER-HETEROGENEOUSCPU-0001 – Nova Heterogeneous CPU Power Management

## Status

Angenommen

## Kategorie

Power / Heterogeneous CPU

## Zweck

NovaOS definiert die energie- und leistungsbewusste Steuerung heterogener CPU-Topologien.

Unterschiedliche CPU-Klassen werden anhand ihrer tatsächlichen Eigenschaften modelliert, ohne feste Annahmen über Performance- oder Efficiency-Cores vorauszusetzen.

## Grundprinzipien

```text
CPU Core ≠ Identical Core
Core Class ≠ Fixed Architecture
Performance Core ≠ Always Preferred
Efficiency Core ≠ Always Slower
Placement ≠ Authority
Topology ≠ Policy
```

## Modell

```text
CPUClass
├── ClassID
├── CPUs[]
├── PerformanceCapacity
├── EnergyEfficiency
├── SupportedStates[]
├── PerformanceDomain
├── ThermalDomain
└── Constraints
```

Eine Plattform darf beliebig viele unterschiedliche CPU-Klassen besitzen.

## Topologie

NovaOS erkennt die vorhandene CPU-Struktur über HAL und Platform Provider:

```text
System
├── CPU Class A
│   ├── CPU 0
│   └── CPU 1
├── CPU Class B
│   ├── CPU 2
│   └── CPU 3
└── CPU Class C
```

Die Klassen werden anhand ihrer Eigenschaften und nicht allein anhand herstellerspezifischer Bezeichnungen beschrieben.

## Workload-Zuordnung

Scheduler und Power Manager koordinieren die Platzierung:

```text
Workload
   +
Execution Contract
   +
Performance Demand
   +
Energy Budget
   +
Thermal State
      ↓
CPU Class Selection
      ↓
Scheduler Placement
```

Interaktive oder latenzkritische Arbeit kann leistungsfähigere Kerne bevorzugen.

Hintergrund- oder energieoptimierte Arbeit kann effizientere Kerne bevorzugen.

Dies sind Policy-Entscheidungen und keine festen Regeln.

## Performance Capacity

NovaOS muss unterschiedliche Rechenkapazitäten berücksichtigen können.

```text
CPU Utilization ≠ Equal Performance Demand
```

Eine identische Auslastung auf zwei unterschiedlichen CPU-Klassen bedeutet nicht zwangsläufig identische Rechenleistung oder Energieaufnahme.

## Migration

Tasks dürfen zwischen CPU-Klassen migriert werden:

```text
CPU Class A
     ↓
Migration
     ↓
CPU Class B
```

Dabei müssen berücksichtigt werden:

```text
Migration Cost
Cache Locality
NUMA Locality
Deadline
Affinity
Thermal State
Energy Cost
```

Unnötiges Hin- und Her-Migrieren soll vermieden werden.

## Performance Domains

CPU-Klassen und Performance Domains müssen getrennt bleiben.

Mehrere CPU-Klassen können:

```text
eine gemeinsame Performance Domain
```

besitzen oder:

```text
unabhängige Performance Domains
```

verwenden.

DVFS- und Boost-Entscheidungen müssen diese Abhängigkeiten berücksichtigen.

## Thermal Integration

Thermische Belastung darf die bevorzugte CPU-Klasse verändern.

```text
Preferred CPU
     ↓
Thermal Constraint
     ↓
Alternative CPU Class
```

Thermal Safety besitzt Vorrang vor Performance-Optimierung.

## Hardwareinformationen

NovaOS darf Hardwarehinweise verwenden, beispielsweise:

```text
Relative Performance Capacity
Efficiency Information
Preferred Core
Frequency Range
Hardware Scheduling Hint
Firmware Information
```

Diese Informationen müssen validiert und mit beobachtbarem Verhalten kombinierbar sein.

## Fallback

Sind keine zuverlässigen Unterschiede zwischen CPU-Klassen bekannt, muss NovaOS die Prozessoren konservativ als gleichwertig behandeln können.

```text
Unknown Heterogeneity
        ↓
Generic CPU Scheduling
```

Unbekannte Eigenschaften dürfen nicht erfunden werden.

## Normative Anforderungen

1. NovaOS MUSS heterogene CPU-Topologien darstellen können.
2. CPU-Klassen MÜSSEN anhand von Eigenschaften statt ausschließlich Herstellernamen beschrieben werden.
3. Scheduler und Power Manager MÜSSEN CPU-Klassen gemeinsam berücksichtigen können.
4. Performance Capacity und CPU-Auslastung MÜSSEN getrennt behandelbar sein.
5. Execution Contracts MÜSSEN die CPU-Platzierung beeinflussen können.
6. Energieeffizienz DARF bei der CPU-Auswahl berücksichtigt werden.
7. Tasks MÜSSEN zwischen kompatiblen CPU-Klassen migrierbar sein.
8. Migration MUSS Locality-, Energie- und Performancekosten berücksichtigen können.
9. CPU-Klasse und Performance Domain MÜSSEN getrennte Konzepte bleiben.
10. DVFS-, Boost- und Thermal Constraints MÜSSEN berücksichtigt werden.
11. Bei unbekannter Topologie MUSS ein sicherer generischer Fallback möglich sein.
12. CPU-Klassen, Kapazitäten, Platzierungsentscheidungen und aktive Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-CPUPERFORMANCE-0001`
- `NPSPEC-POWER-DVFS-0001`
- `NPSPEC-POWER-BOOST-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS kann heterogene Prozessoren als unterschiedliche CPU-Klassen mit eigenen Performance-, Energie- und Thermal-Eigenschaften behandeln. Scheduler und Power Management können Workloads dadurch dynamisch auf geeignete Kerne verteilen, ohne an ein bestimmtes Herstellerkonzept wie Performance- und Efficiency-Cores gebunden zu sein.