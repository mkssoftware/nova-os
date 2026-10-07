# NPSPEC-POWER-ARCH-0001 – Nova Power Architecture

## Status

Angenommen

## Kategorie

Power / Architecture

## Zweck

NovaOS definiert eine zentrale Power Architecture zur koordinierten Steuerung von Energiezuständen, Leistungsaufnahme, Energiequellen und Energieanforderungen.

Power Management wird systemweit mit Scheduler, Resource Economy, Thermal Management, Geräten und Execution Contracts koordiniert.

## Grundprinzipien

```text
Power ≠ Performance
Power State ≠ Device State
Energy Policy ≠ Hardware Mechanism
Power Saving ≠ Forced Throttling
Battery Level ≠ Energy Budget
User Preference ≠ Hard Constraint
```

Die Priorität lautet grundsätzlich:

```text
Safety
  ↓
Hardware Constraints
  ↓
Security / Integrity
  ↓
Explicit User Decisions
  ↓
Execution Contracts
  ↓
Energy Policy
  ↓
Adaptive Optimization
```

## Architektur

```text
Applications / Solutions / Tasks
              ↓
       Execution Contracts
              ↓
        Resource Economy
              ↓
        Power Manager
       ↙      ↓       ↘
 Scheduler  Thermal   Devices
              ↓
        HAL / Firmware
              ↓
           Hardware
```

Der Power Manager koordiniert Entscheidungen, ersetzt jedoch nicht die hardwarespezifischen Mechanismen von HAL, Firmware oder Gerätetreibern.

## Power Context

```text
PowerContext
├── PowerSource
├── EnergyState
├── PowerProfile
├── EnergyBudget
├── SystemLoad
├── ThermalState
├── HardwareLimits
└── UserPolicy
```

Der effektive Zustand entsteht aus allen verfügbaren Informationen.

## Energiequellen

NovaOS muss unterschiedliche Energiequellen modellieren können:

```text
AC
Battery
UPS
External Battery
Wireless Power
Unknown
```

Mehrere Energiequellen dürfen gleichzeitig vorhanden sein.

## Power Profiles

Systemweite Profile können beispielsweise sein:

```text
Performance
Balanced
Efficiency
Battery Saver
Critical
Custom
```

Ein Profil beschreibt Policy und Zielsetzung, keine feste Hardwarekonfiguration.

## Systemzustände

NovaOS unterstützt mindestens:

```text
Active
Idle
Low Power
Suspend
Hibernate
Shutdown
```

Hardware darf zusätzliche Plattformzustände bereitstellen.

Der Wechsel erfolgt über kontrollierte Power Transitions.

## Komponentensteuerung

Power Management darf unter anderem koordinieren:

```text
CPU
GPU
Accelerators
Memory
Storage
Display
Network
USB
Audio
Sensors
Peripheral Devices
```

Einzelne Komponenten dürfen unabhängig in geeignete Energiesparzustände wechseln.

## Execution Contracts

Execution Contracts dürfen Energieanforderungen definieren:

```text
Energy Budget
Performance Requirement
Latency Requirement
Deadline
Preferred Power Behavior
```

Power Policy darf Soft Constraints optimieren.

Hard Constraints dürfen nicht stillschweigend verletzt werden.

## Thermal Integration

Power und Thermal Management arbeiten zusammen:

```text
Power Consumption
       ↓
Heat Generation
       ↓
Thermal State
       ↓
Power / Performance Adjustment
```

Thermische Sicherheitsmaßnahmen besitzen Vorrang vor Leistungs- oder Energiepräferenzen.

## Adaptive Optimierung

NovaOS darf Energieverbrauch anhand von:

```text
Workload
Usage Pattern
Device State
Battery State
Thermal Headroom
Execution Contracts
User Preferences
```

adaptiv optimieren.

Adaptive Entscheidungen müssen jederzeit durch höhere Constraints begrenzt bleiben.

## Fehlerverhalten

Nicht verfügbare oder ungültige Energiedaten dürfen nicht als sicherer Zustand interpretiert werden.

```text
Unknown ≠ Safe
Unknown ≠ Unlimited Energy
```

Bei kritischen Unsicherheiten muss NovaOS konservativ reagieren.

## Introspection

Mindestens folgende Informationen müssen introspektierbar sein:

```text
Power Source
Power Profile
Energy State
Energy Budget
Current Constraints
Power Decisions
Device Power States
Thermal Interaction
```

## Normative Anforderungen

1. NovaOS MUSS eine zentrale Power Architecture bereitstellen.
2. Hardwaremechanismus und Power Policy MÜSSEN getrennt bleiben.
3. Mehrere Energiequellen MÜSSEN modellierbar sein.
4. Power Profiles MÜSSEN von konkreten Hardwarezuständen getrennt bleiben.
5. System- und Geräteenergiezustände MÜSSEN getrennt steuerbar sein.
6. Power Management MUSS mit Resource Economy und Scheduler integrierbar sein.
7. Power Management MUSS Thermal Constraints berücksichtigen.
8. Execution Contracts MÜSSEN Energieanforderungen ausdrücken können.
9. Hard Constraints DÜRFEN durch Energieoptimierung nicht verletzt werden.
10. Explizite Benutzerentscheidungen MÜSSEN gegenüber adaptiver Optimierung berücksichtigt werden.
11. Unbekannte Energiezustände DÜRFEN nicht als sicher oder unbegrenzt behandelt werden.
12. Power-Zustand, Policy, Constraints und Entscheidungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-SAFETY-0001`
- `NPSPEC-SYSTEM-HAL-0001`

## Ergebnis

NovaOS besitzt eine zentrale, policygesteuerte Power Architecture, die Energiequellen, Systemzustände, Geräte, Ressourcen, thermische Grenzen und Execution Contracts gemeinsam betrachtet. Energieverbrauch kann dadurch dynamisch optimiert werden, ohne Sicherheit, Hardwaregrenzen oder explizite Benutzerentscheidungen zu verletzen.