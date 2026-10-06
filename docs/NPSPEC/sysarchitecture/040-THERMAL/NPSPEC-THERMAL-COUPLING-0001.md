# NPSPEC-THERMAL-COUPLING-0001 – Nova Thermal Coupling

## Status

Angenommen

## Kategorie

Thermal / Coupling

## Zweck

NovaOS definiert Thermal Coupling zur Beschreibung thermischer Abhängigkeiten zwischen Thermal Zones und Hardwarekomponenten.

Dadurch kann das System berücksichtigen, dass Last oder Wärmeentwicklung einer Komponente die Temperatur und den verfügbaren Thermal Headroom anderer Komponenten beeinflusst.

## Grundprinzipien

```text
Thermal Zone ≠ Isolated System
Coupling ≠ Shared Sensor
Coupling ≠ Shared Device
Heat Source ≠ Affected Zone
Correlation ≠ Causality
Unknown Coupling ≠ No Coupling
```

## Modell

```text
ThermalCoupling
├── CouplingID
├── SourceZone
├── TargetZone
├── Strength
├── Direction
├── Delay
├── Confidence
└── State
```

Eine Kopplung darf gerichtet oder bidirektional sein.

```text
CPU ──────→ SoC
GPU ──────→ SoC
CPU ←────→ VRM
Storage ──→ Platform
```

## Kopplungsarten

NovaOS darf unterschiedliche thermische Beziehungen abbilden:

```text
Direct
SharedCooling
SharedPower
SharedPackage
Environmental
Platform
Derived
```

Mehrere Kopplungen dürfen gleichzeitig auf eine Thermal Zone wirken.

## Stärke

Die Stärke einer Kopplung beschreibt, wie stark thermische Veränderungen einer Zone eine andere Zone beeinflussen.

Sie darf aus folgenden Quellen stammen:

```text
Hardware Description
Firmware
Driver
Platform Profile
Calibration
Runtime Observation
```

Gelernte oder geschätzte Werte müssen von hardwaredefinierten Werten unterscheidbar bleiben.

## Zeitverhalten

Thermische Kopplungen dürfen zeitverzögert wirken.

```text
Source Load
    ↓
Heat Generation
    ↓
Propagation Delay
    ↓
Target Temperature
```

NovaOS darf diese Verzögerung bei Trend- und Headroom-Berechnungen berücksichtigen.

## Thermal Headroom

Der effektive Headroom einer Zone darf durch gekoppelte Zonen beeinflusst werden.

```text
Local Headroom
      +
Coupled Thermal Pressure
      ↓
Effective Headroom
```

Eine aktuell kühle Zone darf daher bereits vorsichtiger bewertet werden, wenn eine stark gekoppelte Nachbarzone schnell Wärme erzeugt.

## Scheduler

Der Scheduler darf Thermal Coupling bei der Workload-Platzierung berücksichtigen.

Das Verschieben von Last auf eine andere Ressource gilt nur dann als thermisch sinnvoll, wenn dadurch nicht dieselbe gekoppelte Thermal Zone weiter belastet wird.

## Throttling

Thermal Throttling darf gekoppelte Komponenten gemeinsam berücksichtigen.

```text
GPU Load ↑
   ↓
Shared Thermal Pressure
   ↓
CPU Headroom ↓
   ↓
Coordinated Constraint
```

Dadurch können lokale Gegenmaßnahmen vermieden werden, die das thermische Problem lediglich auf eine gekoppelte Komponente verschieben.

## Fehlerverhalten

Ist eine bekannte Kopplung nicht zuverlässig bewertbar, muss ihr Zustand als `Unknown` oder `Degraded` darstellbar sein.

Eine unbekannte Kopplung darf nicht automatisch als nicht vorhanden behandelt werden.

## Normative Anforderungen

1. Thermal Zones MÜSSEN thermische Beziehungen zu anderen Zones beschreiben können.
2. Kopplungen MÜSSEN gerichtet und bidirektional modellierbar sein.
3. Mehrere Kopplungen pro Thermal Zone MÜSSEN unterstützt werden.
4. Kopplungsstärke und zeitliche Verzögerung MÜSSEN darstellbar sein.
5. Hardwaredefinierte und abgeleitete Kopplungsinformationen MÜSSEN unterscheidbar bleiben.
6. `Unknown` DARF nicht als fehlende Kopplung interpretiert werden.
7. Thermal Headroom MUSS gekoppelte thermische Belastungen berücksichtigen können.
8. Scheduler und Resource Economy MÜSSEN Thermal Coupling berücksichtigen können.
9. Throttling DARF gekoppelte Ressourcen koordiniert steuern.
10. Thermal Coupling DARF Hardware-Sicherheitsgrenzen nicht abschwächen.
11. Dynamisch ermittelte Kopplungen MÜSSEN eine Confidence besitzen können.
12. Kopplungen, Stärke, Richtung, Zustand und Quelle MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-THERMAL-SENSOR-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-THROTTLING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS betrachtet Thermal Zones nicht isoliert, sondern als thermisch gekoppelte Teile eines Gesamtsystems. Dadurch können Headroom, Scheduling und Throttling auch Wärmeübertragung und gemeinsame Kühl- oder Leistungsgrenzen berücksichtigen und thermische Last systemweit sinnvoll verteilen.