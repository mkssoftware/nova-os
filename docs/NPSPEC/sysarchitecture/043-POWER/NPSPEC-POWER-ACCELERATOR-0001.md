# NPSPEC-POWER-ACCELERATOR-0001 – Nova Accelerator Power Management

## Status

Angenommen

## Kategorie

Power / Accelerator

## Zweck

NovaOS definiert die energieeffiziente Steuerung spezialisierter Hardwarebeschleuniger.

Beschleuniger dürfen abhängig von Workload, Execution Contract, Energiebedarf und thermischen Grenzen aktiviert, skaliert, in Energiesparzustände versetzt oder vollständig deaktiviert werden.

## Grundprinzipien

```text
Accelerator ≠ CPU
Accelerator ≠ GPU
Availability ≠ Active State
Power State ≠ Execution State
Performance Request ≠ Fixed Frequency
Accelerator Selection ≠ Authority
Power Saving ≠ State Loss
```

## Modell

```text
AcceleratorPowerContext
├── AcceleratorID
├── Type
├── PowerDomainID
├── SupportedStates[]
├── PerformanceLevels[]
├── CurrentState
├── Utilization
├── EnergyCost
├── ThermalState
└── Constraints
```

Beschleuniger können beispielsweise sein:

```text
GPU
NPU
AI Accelerator
DSP
Media Engine
Crypto Accelerator
Compute Accelerator
FPGA
Specialized SoC Engine
```

## Architektur

```text
Workload
   ↓
Execution Contract
   ↓
Accelerator Selection
   ↓
Accelerator Power Manager
   ↓
Driver / Platform / HAL
   ↓
Hardware
```

Die Power-Schicht entscheidet nicht selbst über die semantische Eignung eines Accelerators, sondern verarbeitet die Anforderungen der Ressourcen- und Ausführungsplanung.

## Aktivierung

Ein Accelerator darf bei Bedarf aktiviert werden:

```text
Workload Request
      ↓
Capability / Resource Resolution
      ↓
Power-Up
      ↓
Initialization
      ↓
State Verification
      ↓
Execution
```

Die Ausführung darf erst beginnen, wenn der erforderliche Hardwarezustand tatsächlich erreicht wurde.

## Energiezustände

NovaOS abstrahiert mindestens:

```text
Active
Idle
LowPower
Standby
Off
Unavailable
```

Nicht jeder Accelerator muss alle Zustände unterstützen.

## Performance Scaling

Unterstützte Accelerators dürfen mehrere Leistungsstufen besitzen:

```text
Efficiency
Balanced
Performance
Maximum
Boost
```

Diese Stufen sind abstrakt und müssen nicht direkt einer festen Frequenz entsprechen.

DVFS oder andere hardwarespezifische Mechanismen bleiben hinter dem Provider verborgen.

## Runtime Power Management

Nicht verwendete Accelerators dürfen automatisch heruntergefahren werden:

```text
Active
  ↓ inactivity
Idle
  ↓
LowPower
  ↓
Off
```

Dabei müssen Context-, State- und Resume-Kosten berücksichtigt werden.

Ein tiefer Power State ist nicht automatisch optimal.

## Workload-Platzierung

Die Wahl eines Accelerators darf berücksichtigen:

```text
Performance
Latency
Energy Cost
Thermal Headroom
Data Locality
Transfer Cost
Availability
Execution Contract
```

Eine energieeffiziente Ausführung kann deshalb einen anderen Accelerator wählen als eine latenzoptimierte Ausführung.

## Gemeinsame Power Domains

Mehrere Accelerators oder Komponenten dürfen dieselbe Power Domain verwenden:

```text
Power Domain
├── GPU
├── NPU
└── Media Engine
```

Eine Domain darf nur deaktiviert werden, wenn keine abhängige Komponente sie benötigt.

## Thermal Integration

```text
Accelerator Load
       ↓
Power Consumption
       ↓
Thermal Pressure
       ↓
Effective Performance Limit
```

Thermal Safety besitzt Vorrang vor Accelerator-Performance.

## Fallback

Ist ein Accelerator nicht verfügbar, darf NovaOS einen kompatiblen alternativen Ausführungspfad verwenden:

```text
Preferred Accelerator
       ↓ unavailable
Alternative Accelerator
       ↓
CPU / Software Fallback
```

Ein Fallback darf nur erfolgen, wenn Execution Contract und Semantik erhalten bleiben.

## Normative Anforderungen

1. NovaOS MUSS unterschiedliche Accelerator-Typen einheitlich verwalten können.
2. Accelerator Power State und Execution State MÜSSEN getrennt bleiben.
3. Accelerators MÜSSEN bedarfsgesteuert aktiviert und deaktiviert werden können.
4. Runtime Power Management MUSS unterstützt werden können.
5. Performance Scaling MUSS hardwareunabhängig abstrahiert werden.
6. Energie-, Latenz- und Performance-Anforderungen MÜSSEN bei der Accelerator-Auswahl berücksichtigt werden können.
7. Gemeinsame Power Domains MÜSSEN berücksichtigt werden.
8. State- und Resume-Kosten MÜSSEN bei tiefen Energiesparzuständen berücksichtigt werden können.
9. Thermal Constraints MÜSSEN jederzeit berücksichtigt werden.
10. Alternative Accelerators oder Software-Fallbacks DÜRFEN verwendet werden, wenn der Execution Contract dies erlaubt.
11. Angeforderter und effektiver Power State MÜSSEN unterscheidbar sein.
12. AcceleratorID, Power State, Performance Level, Auslastung und aktive Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-POWER-DVFS-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS kann GPUs, NPUs, DSPs und andere spezialisierte Beschleuniger energie- und leistungsbewusst steuern. Accelerators werden nur bei Bedarf aktiviert, können dynamisch skaliert und bei Inaktivität heruntergefahren werden, während Execution Contracts, Datenlokalität, thermische Grenzen und alternative Ausführungspfade berücksichtigt bleiben.