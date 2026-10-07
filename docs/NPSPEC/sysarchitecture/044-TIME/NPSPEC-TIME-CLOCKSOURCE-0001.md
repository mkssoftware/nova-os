# NPSPEC-TIME-CLOCKSOURCE-0001 – Nova Clock Source

## Status

Angenommen

## Kategorie

Time / Clock Source

## Zweck

NovaOS definiert ein einheitliches Modell für Hardware-, Firmware-, virtuelle und paravirtualisierte Zeitquellen.

Clock Sources liefern fortlaufende Zählerwerte an das Timekeeping Core. Sie bestimmen nicht selbst Wall Clock, Civil Time, Timer oder Scheduling.

## Grundprinzipien

```text
Clock Source ≠ Wall Clock
Clock Source ≠ RTC
Clock Source ≠ Timer
Clock Source ≠ Clock Event
Counter ≠ Time
Frequency ≠ Accuracy
Available ≠ Suitable
```

## Modell

```text
ClockSource
├── ClockSourceID
├── Provider
├── Counter
├── Frequency
├── Resolution
├── Precision
├── Accuracy
├── Stability
├── ReadLatency
├── Capabilities[]
└── State
```

## Quellen

NovaOS darf unterschiedliche Clock Sources verwenden:

```text
TSC
HPET
ACPI PM Timer
PIT
Architecture Timer
SoC Counter
Firmware Counter
Paravirtualized Clock
Emulated Clock
Registered Clock Provider
```

Keine konkrete Quelle ist für die allgemeine Time Architecture vorgeschrieben.

## Registrierung

Clock Sources werden über HAL oder registrierte Provider bereitgestellt:

```text
Hardware / Hypervisor
        ↓
Clock Provider
        ↓
Clock Source Registration
        ↓
Validation
        ↓
Timekeeping Core
```

Registrierung bedeutet nicht automatisch Auswahl als aktive Quelle.

## Validierung

Eine Clock Source muss hinsichtlich relevanter Eigenschaften bewertet werden:

```text
Monotonicity
Frequency Stability
Resolution
Read Cost
Cross-CPU Consistency
Suspend Behavior
Virtualization Behavior
Reliability
```

Nicht zuverlässig bestimmbare Eigenschaften müssen als unbekannt behandelt werden.

## Auswahl

NovaOS darf mehrere Clock Sources gleichzeitig kennen.

```text
Available Clock Sources
          ↓
Validation
          ↓
Suitability Ranking
          ↓
Active Clock Source
```

Die höchste Frequenz ist nicht automatisch die beste Quelle.

Auswahlkriterien dürfen je nach Zeitdomäne unterschiedlich gewichtet werden.

## Kalibrierung

Clock Sources dürfen gegen andere bekannte Zeitquellen kalibriert werden:

```text
Reference Source
      ↕
Measured Source
      ↓
Frequency / Drift Estimate
```

Kalibrierung darf regelmäßig wiederholt werden, wenn Hardware oder Plattform dies erfordern.

## Source-Wechsel

Eine aktive Clock Source darf zur Laufzeit ersetzt werden:

```text
Source A
   ↓
Capture Time State
   ↓
Validate Source B
   ↓
Calculate Continuity Offset
   ↓
Switch
   ↓
Source B
```

Der Wechsel darf die darüberliegende monotone Zeitlinie nicht rückwärts springen lassen.

## Multi-CPU

Bei Mehrprozessorsystemen muss NovaOS berücksichtigen, ob Counter:

```text
Synchronized
Per-CPU
Invariant
Frequency-Dependent
Migratable
```

sind.

Task-Migration zwischen CPUs darf keine fehlerhaften Zeitdifferenzen erzeugen.

## Suspend

Jede Clock Source muss ihr Suspend-Verhalten beschreiben können:

```text
Continues During Suspend
Stops During Suspend
Resets During Suspend
Unknown
```

Nach Resume darf eine erneute Validierung oder Kalibrierung erforderlich sein.

## Fehlerverhalten

Fehlerhafte oder instabile Quellen müssen degradierbar sein:

```text
Clock Anomaly
     ↓
Validation
     ↓
Degraded / Failed
     ↓
Select Alternative Source
     ↓
Preserve Time Continuity
```

Der Ausfall einer einzelnen Clock Source darf die Time Architecture nicht unnötig zum Stillstand bringen.

## Normative Anforderungen

1. NovaOS MUSS Clock Sources über eine einheitliche Abstraktion behandeln.
2. Clock Source und RTC MÜSSEN getrennte Konzepte bleiben.
3. Registrierung DARF nicht automatisch die aktive Auswahl bedeuten.
4. Mehrere Clock Sources MÜSSEN gleichzeitig unterstützt werden können.
5. Clock Sources MÜSSEN vor produktiver Nutzung validierbar sein.
6. Frequenz, Auflösung, Genauigkeit und Stabilität MÜSSEN getrennte Eigenschaften bleiben.
7. Clock Sources MÜSSEN kalibrierbar sein können.
8. Die aktive Clock Source MUSS zur Laufzeit austauschbar sein.
9. Source-Wechsel DÜRFEN die monotone Zeitkontinuität nicht verletzen.
10. Multi-CPU- und Suspend-Verhalten MÜSSEN berücksichtigt werden.
11. Fehlerhafte Clock Sources MÜSSEN degradiert oder deaktiviert werden können.
12. ClockSourceID, Provider, Eigenschaften, Validierungsstatus und aktive Auswahl MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-HIGHRES-0001`
- `NPSPEC-TIME-RTC-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine austauschbare und validierbare Clock-Source-Infrastruktur. Unterschiedliche Hardware-, Firmware- und virtuelle Zähler können parallel verfügbar sein, bewertet, kalibriert und bei Bedarf gewechselt werden, während die darüberliegenden Zeitdomänen ihre konsistente Zeitlinie beibehalten.