# NPSPEC-POWER-DVFS-0001 – Nova Dynamic Voltage and Frequency Scaling

## Status

Angenommen

## Kategorie

Power / DVFS

## Zweck

NovaOS definiert Dynamic Voltage and Frequency Scaling als hardwarenahe Methode zur dynamischen Anpassung von Taktfrequenz und Versorgungsspannung leistungsfähiger Hardwaredomänen.

DVFS setzt abstrakte Performance-Anforderungen der höheren Power-Schichten in unterstützte Hardwarezustände um.

## Grundprinzipien

```text
DVFS ≠ Power Policy
DVFS ≠ CPU Idle
Frequency ≠ Performance
Voltage ≠ Performance Level
Higher Frequency ≠ Always Faster
Requested State ≠ Actual State
DVFS State ≠ Execution Contract
```

## Architektur

```text
CPU Performance Manager
          ↓
Performance Request
          ↓
DVFS Provider
          ↓
Voltage / Frequency Selection
          ↓
Platform / HAL
          ↓
Hardware
```

DVFS ist ein Mechanismus unterhalb der allgemeinen Performance Policy.

## DVFS Domain

Hardwarekomponenten können gemeinsame DVFS-Domänen besitzen:

```text
DVFSDomain
├── DomainID
├── Components[]
├── OperatingPoints[]
├── CurrentPoint
├── RequestedPoint
├── TransitionLatency
├── Constraints
└── State
```

Eine Domäne kann beispielsweise umfassen:

```text
CPU Core
CPU Cluster
CPU Package
GPU
Accelerator
SoC Domain
```

## Operating Points

Ein Operating Point beschreibt eine validierte Hardwarekonfiguration:

```text
OperatingPoint
├── Frequency
├── Voltage
├── PerformanceLevel
├── PowerEstimate
└── Constraints
```

Nur von Hardware, Firmware oder validierten Plattformdaten unterstützte Kombinationen dürfen verwendet werden.

NovaOS darf keine beliebigen Spannungs-/Frequenzkombinationen erzeugen.

## Auswahl

```text
Performance Request
       +
Energy Budget
       +
Thermal Headroom
       +
Hardware Limits
       ↓
Operating Point Selection
```

Die Auswahl muss alle Hard Constraints einhalten.

## Transition

Ein Wechsel erfolgt kontrolliert:

```text
Current Point
     ↓
Validate Target
     ↓
Prepare Transition
     ↓
Voltage / Frequency Change
     ↓
Verify
     ↓
Effective Point
```

Die erforderliche Reihenfolge von Spannungs- und Frequenzänderungen ist plattformspezifisch und wird vom Provider kontrolliert.

## Hardwaregesteuertes DVFS

Moderne Hardware darf interne DVFS-Entscheidungen selbst treffen.

In diesem Fall kann NovaOS statt konkreter Operating Points abstrakte Grenzen oder Performance-Hinweise übergeben:

```text
Minimum Performance
Maximum Performance
Desired Performance
Energy Preference
```

Die tatsächlich gewählte Frequenz und Spannung bleiben dann Hardwareentscheidung.

## Thermal Integration

Thermische Grenzen dürfen den verfügbaren DVFS-Bereich reduzieren:

```text
Supported Range
      ∩
Thermal Limit
      ∩
Power Limit
      =
Effective DVFS Range
```

Thermal Safety besitzt Vorrang vor Performanceanforderungen.

## Stabilität und Sicherheit

Ungültige Operating Points dürfen niemals aktiviert werden.

Bei Fehlern muss ein sicherer unterstützter Zustand gewählt werden können.

```text
Invalid / Failed Point
        ↓
Safe Supported Point
```

DVFS darf keine Hardware-Sicherheitsgrenzen umgehen.

## Introspection

NovaOS muss mindestens bereitstellen können:

```text
DVFS Domain
Supported Operating Points
Requested Point
Effective Point
Frequency
Voltage
Transition State
Active Constraints
Provider
```

Bei hardwaregesteuertem DVFS dürfen nicht verfügbare Werte als unbekannt gekennzeichnet werden.

## Normative Anforderungen

1. NovaOS MUSS DVFS von höherer Power Policy trennen.
2. DVFS und CPU Idle MÜSSEN getrennte Mechanismen bleiben.
3. DVFS-Domänen MÜSSEN mehrere Hardwarekomponenten umfassen können.
4. Nur validierte Operating Points DÜRFEN verwendet werden.
5. NovaOS DARF keine ungültigen Spannungs-/Frequenzkombinationen erzeugen.
6. DVFS-Transitionen MÜSSEN plattformspezifische Reihenfolgen und Limits beachten.
7. Energie- und Thermal Constraints MÜSSEN berücksichtigt werden.
8. Hardwaregesteuertes DVFS MUSS unterstützt werden können.
9. Angeforderter und tatsächlicher Zustand MÜSSEN unterscheidbar bleiben.
10. Fehlerhafte Transitionen MÜSSEN auf einen sicheren Zustand zurückfallen können.
11. Hardware-Sicherheitsgrenzen DÜRFEN nicht umgangen werden.
12. DVFS-Domäne, Operating Points, aktueller Zustand und Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-CPUPERFORMANCE-0001`
- `NPSPEC-POWER-CPUIDLE-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-THROTTLING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS besitzt eine hardwareunabhängig eingebundene DVFS-Schicht, die abstrakte Performance-Anforderungen in sichere und energieeffiziente Spannungs-/Frequenzzustände übersetzt. Konkrete Hardwaremechanismen bleiben hinter Platform und HAL verborgen, während Energie-, Thermal- und Hardwaregrenzen jederzeit Vorrang behalten.