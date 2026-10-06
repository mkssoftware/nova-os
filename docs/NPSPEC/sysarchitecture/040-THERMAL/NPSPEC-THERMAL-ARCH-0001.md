# NPSPEC-THERMAL-ARCH-0001 – Nova Thermal Architecture

## Status

Angenommen

## Kategorie

Thermal / Architecture

## Zweck

NovaOS definiert eine zentrale Thermal-Architektur zur Überwachung und Steuerung thermischer Zustände des Systems.

Sie verbindet Temperatursensoren, Hardwarelimits, Energieverwaltung, Scheduler und Kühlmechanismen zu einem einheitlichen Modell, ohne thermische Policy fest in einzelne Treiber oder den Kernel einzubauen.

## Grundprinzipien

```text
Temperature ≠ Thermal State
Sensor ≠ Policy
Thermal Limit ≠ Performance Target
Cooling ≠ Throttling Only
Thermal Control ≠ Scheduler
Safety > Performance
```

## Architektur

```text
Thermal Sensors
      ↓
Thermal HAL / Drivers
      ↓
Thermal Manager
├── Thermal Zones
├── Limits
├── Trends
├── Cooling Devices
└── Thermal State
      ↓
Thermal Policy
      ↓
Actions
├── Scheduler
├── Power Management
├── Frequency Control
├── Cooling
└── Emergency Protection
```

## Thermal Zones

Hardware wird zu logischen Thermal Zones zusammengefasst:

```text
CPU
GPU
Memory
Storage
Battery
VRM
SoC
Accelerators
Platform
```

Eine Zone darf einen oder mehrere Sensoren und mehrere betroffene Komponenten besitzen.

## Thermal State

NovaOS verwendet abstrahierte Zustände:

```text
Normal
Elevated
Hot
Critical
Emergency
Unknown
```

Die konkreten Temperaturgrenzen werden aus Hardwareinformationen, Firmware, Treibern und Policy bestimmt.

`Unknown` darf nicht automatisch als sicherer Zustand behandelt werden.

## Thermal Manager

Der Thermal Manager sammelt und normalisiert:

```text
Temperature
Temperature Trend
Thermal Limits
Sensor Health
Cooling State
Power State
Workload
```

Entscheidungen sollen nicht ausschließlich auf einzelnen Momentanwerten beruhen.

## Steuerung

Mögliche thermische Maßnahmen umfassen:

```text
Fan Control
Frequency Reduction
Voltage / Power Reduction
Scheduler Migration
Workload Throttling
Accelerator Limiting
Device Power State
Task Degradation
Emergency Shutdown
```

Die mildeste geeignete Maßnahme soll bevorzugt werden.

## Scheduler-Integration

Der Scheduler darf thermische Informationen bei Platzierungsentscheidungen berücksichtigen:

```text
Hot CPU Core
    ↓
Scheduler
    ↓
Migrate / Reduce Load
```

Thermische Optimierung darf harte Scheduling-, Realtime- oder Sicherheitsanforderungen nicht verletzen.

## Resource Economy

Thermalzustände fließen in die verfügbare Ressourcenmenge ein.

```text
Physical Resources
      ↓
Thermal Constraints
      ↓
Usable Resource Budget
```

Execution Contracts dürfen dadurch reduzierte CPU-, GPU- oder Accelerator-Budgets erhalten.

## Sicherheit

Hardwaredefinierte kritische Grenzwerte haben Vorrang vor Performance-, Nutzer- und Optimierungspräferenzen.

Bei unmittelbar gefährlichen Zuständen darf NovaOS ohne vorherige Nutzerinteraktion Schutzmaßnahmen bis hin zum kontrollierten Abschalten ausführen.

## Fehlerverhalten

Bei Sensorfehlern oder widersprüchlichen Messwerten muss NovaOS konservativ reagieren.

Thermal-Schutz darf nicht vollständig von einem einzelnen Sensor oder Userspace-Dienst abhängig sein.

## Introspection

Mindestens folgende Informationen sollen introspektierbar sein:

```text
Thermal Zone
Temperature
Trend
Thermal State
Limits
Sensor State
Active Constraints
Cooling Actions
Throttling Reason
```

## Normative Anforderungen

1. NovaOS MUSS thermische Hardware über ein einheitliches Thermal-Modell abstrahieren.
2. Sensorerfassung und Thermal Policy MÜSSEN getrennt bleiben.
3. Hardware MUSS in logische Thermal Zones gruppierbar sein.
4. Mehrere Sensoren pro Thermal Zone MÜSSEN unterstützt werden können.
5. `Unknown` DARF nicht automatisch als sicher bewertet werden.
6. Hardware-Sicherheitsgrenzen MÜSSEN Vorrang vor Performance-Zielen besitzen.
7. Thermal Policy MUSS Scheduler und Resource Economy beeinflussen können.
8. Thermal Constraints MÜSSEN in Execution Contracts berücksichtigt werden können.
9. NovaOS SOLL zunächst die mildeste ausreichende Schutzmaßnahme verwenden.
10. Kritische Zustände MÜSSEN automatische Schutzmaßnahmen ermöglichen.
11. Sensorfehler MÜSSEN kontrolliert und konservativ behandelt werden.
12. Thermalzustand, Limits und aktive Maßnahmen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-CAPABILITY-RESOURCES-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine zentrale, hardwareunabhängige Thermal-Architektur, die Temperaturen, thermische Grenzen und Kühlmöglichkeiten in die systemweite Ressourcen- und Ausführungssteuerung integriert. Sicherheit hat dabei stets Vorrang vor Performance, während Scheduler, Power Management und Hardwaresteuerung koordiniert auf thermische Zustände reagieren können.