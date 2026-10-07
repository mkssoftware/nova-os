# NPSPEC-POWER-BOOST-0001 – Nova Performance Boost

## Status

Angenommen

## Kategorie

Power / Performance Boost

## Zweck

NovaOS definiert Boost als temporäre Erhöhung der verfügbaren Rechenleistung über den nominalen Performance-Bereich hinaus.

Boost darf ausschließlich innerhalb validierter Hardware-, Energie- und Thermalgrenzen genutzt werden und stellt keine garantierte Dauerleistung dar.

## Grundprinzipien

```text
Boost ≠ Nominal Performance
Boost ≠ Guaranteed Performance
Boost ≠ Overclocking
Boost ≠ Fixed Frequency
Boost ≠ Unlimited Power
Boost Request ≠ Boost Activation
Boost Availability ≠ Authority
```

## Architektur

```text
Workload Demand
      ↓
Scheduler / Execution Contract
      ↓
CPU Performance Manager
      ↓
Boost Policy
      ↓
Platform / DVFS Provider
      ↓
Hardware Boost Mechanism
```

## Boost State

```text
BoostState
├── DomainID
├── Availability
├── RequestedLevel
├── EffectiveLevel
├── Duration
├── PowerLimit
├── ThermalLimit
└── State
```

Boost kann für unterschiedliche Performance Domains verfügbar sein:

```text
CPU Core
CPU Cluster
CPU Package
GPU
Accelerator
SoC Domain
```

## Aktivierung

Boost darf aktiviert werden, wenn:

```text
Performance Demand
      +
Execution Contract
      +
Available Power
      +
Thermal Headroom
      +
Hardware Limits
      +
Policy
      ↓
Boost Decision
```

Alle Hard Constraints müssen erfüllt bleiben.

## Temporärer Charakter

Boost ist grundsätzlich zeitlich oder ressourcenbedingt begrenzt.

```text
Boost
 ↓
Energy Consumption
 ↓
Thermal / Power Pressure
 ↓
Reduced Headroom
 ↓
Boost Reduction / Exit
```

NovaOS darf Boost reduzieren oder beenden, sobald dessen Voraussetzungen nicht mehr erfüllt sind.

## Hardwaresteuerung

Moderne Hardware darf Boost intern steuern.

NovaOS kann dabei lediglich:

```text
Allow Boost
Restrict Boost
Set Performance Bounds
Set Power Bounds
Provide Performance Hints
```

Die tatsächlich erreichte Frequenz oder Leistung kann vollständig durch die Hardware bestimmt werden.

## Scheduler-Integration

Boost soll bevorzugt dort eingesetzt werden, wo kurzfristig höhere Leistung einen messbaren Nutzen bietet:

```text
Interactive Work
Latency-Critical Task
Deadline Workload
Short Compute Burst
```

Hintergrundarbeit soll Boost nicht unnötig dauerhaft beanspruchen.

## Thermal Integration

```text
Boost Range
    ∩
Thermal Headroom
    ∩
Power Budget
    =
Effective Boost Range
```

Thermal Safety besitzt jederzeit Vorrang.

## Energiequelle

Die verfügbare Boost-Strategie darf von der Energiequelle abhängen.

Beispielsweise kann Batteriebetrieb strengere Boost-Grenzen verwenden als externe Stromversorgung.

Dies ist Policy und keine feste Hardwareannahme.

## Fehlerverhalten

Nicht verfügbarer Boost darf nicht als Fehler der normalen CPU-Ausführung behandelt werden.

```text
Boost unavailable
      ↓
Nominal Supported Performance
```

Eine Anwendung darf für korrekte Funktion niemals zwingend von Boost abhängig sein.

## Normative Anforderungen

1. NovaOS MUSS Boost von nominaler Performance unterscheiden.
2. Boost MUSS als temporäre Leistungsreserve behandelt werden.
3. Boost DARF keine Hardware-Sicherheitsgrenzen überschreiten.
4. Thermal- und Power-Limits MÜSSEN jederzeit berücksichtigt werden.
5. Boost Request und tatsächliche Boost-Aktivierung MÜSSEN unterscheidbar sein.
6. Boost DARF nicht als garantierte Frequenz oder Leistung dargestellt werden.
7. Hardwaregesteuerte Boost-Mechanismen MÜSSEN unterstützt werden können.
8. NovaOS MUSS Boost begrenzen oder deaktivieren können.
9. Scheduler und Execution Contracts DÜRFEN Boost-Anforderungen beeinflussen.
10. Anwendungen DÜRFEN Boost nicht als Voraussetzung für korrekte Ausführung benötigen.
11. Bei fehlendem Boost MUSS normale unterstützte Performance verfügbar bleiben.
12. Boost-Verfügbarkeit, Limits, Zustand und effektive Nutzung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-CPUPERFORMANCE-0001`
- `NPSPEC-POWER-DVFS-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-THROTTLING-0001`
- `NPSPEC-THERMAL-SAFETY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS behandelt Boost als kontrollierte, temporäre Leistungsreserve. Scheduler und Execution Contracts können zusätzlichen Performancebedarf ausdrücken, während Hardware, Power Budget und Thermal Headroom bestimmen, ob und in welchem Umfang Boost tatsächlich bereitgestellt werden kann.