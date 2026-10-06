# NPSPEC-THERMAL-ZONE-0001 – Nova Thermal Zone

## Status

Angenommen

## Kategorie

Thermal / Zone

## Zweck

NovaOS definiert Thermal Zones als logische thermische Bereiche des Systems.

Eine Thermal Zone fasst einen oder mehrere Temperatursensoren, thermisch relevante Komponenten, Grenzwerte und mögliche Gegenmaßnahmen zu einer gemeinsamen steuerbaren Einheit zusammen.

## Grundprinzipien

```text
Thermal Zone ≠ Sensor
Thermal Zone ≠ Device
Temperature ≠ Thermal State
Zone State ≠ Single Sensor Value
Zone Identity ≠ Physical Location
Unknown ≠ Safe
```

## Modell

```text
ThermalZone
├── ZoneID
├── Type
├── Sensors[]
├── Components[]
├── Limits
├── CoolingResources[]
├── State
└── Trend
```

`ZoneID` identifiziert eine Thermal Zone stabil und unabhängig von Sensorpfaden oder Gerätenamen.

## Zonentypen

Typische Thermal Zones sind:

```text
CPU
GPU
SoC
Memory
Storage
Battery
VRM
Accelerator
Platform
```

Hersteller- und gerätespezifische Zonen dürfen zusätzlich definiert werden.

## Sensoren

Eine Zone darf mehrere Sensoren aggregieren:

```text
Sensor A ─┐
Sensor B ─┼─→ Thermal Zone
Sensor C ─┘
```

Die Bewertung darf abhängig von Hardware und Policy beispielsweise Maximalwert, gewichtete Werte oder thermische Trends berücksichtigen.

Ein einzelner fehlerhafter Sensor darf nicht unkontrolliert den gesamten Thermalzustand bestimmen.

## Komponenten

Eine physische Komponente darf mehreren Thermal Zones zugeordnet sein, wenn thermische Abhängigkeiten dies erfordern.

Beispiel:

```text
CPU Package
├── CPU Zone
└── Platform Zone
```

Zonen dürfen hierarchische oder überlappende Beziehungen besitzen.

## Grenzwerte

Eine Zone verwaltet thermisch relevante Grenzwerte:

```text
Normal
Elevated
Hot
Critical
Emergency
```

Die konkreten Temperaturen werden nicht global fest vorgegeben, sondern aus Hardware-, Firmware-, Treiber- und Policy-Informationen bestimmt.

Hardwaredefinierte Sicherheitsgrenzen dürfen durch Software nicht überschrieben werden.

## Zustand

Der aktuelle Zonenzustand wird aus validierten Messwerten und deren Entwicklung bestimmt.

```text
Measurements
    +
Trend
    +
Limits
    ↓
Thermal State
```

Kurzzeitige Messspitzen dürfen durch geeignete Hysterese behandelt werden, sofern dadurch keine Hardware-Sicherheitsgrenze verletzt wird.

## Cooling Resources

Einer Zone können geeignete Gegenmaßnahmen zugeordnet werden:

```text
Fan
Frequency Control
Power Limit
Scheduler Migration
Device Power State
Workload Throttling
```

Eine Cooling Resource darf mehreren Thermal Zones zugeordnet sein.

## Fehlerverhalten

Sind Sensorwerte nicht verfügbar oder widersprüchlich, kann die Zone den Zustand `Unknown` annehmen.

NovaOS muss in diesem Fall entsprechend dem Risiko konservative Schutzmaßnahmen anwenden können.

## Normative Anforderungen

1. Jede Thermal Zone MUSS eine stabile `ZoneID` besitzen.
2. Eine Zone MUSS mehrere Sensoren unterstützen können.
3. Eine Komponente DARF mehreren Thermal Zones zugeordnet sein.
4. Thermal Zones DÜRFEN sich überlappen oder hierarchisch organisiert sein.
5. Der Zonenzustand DARF nicht zwingend an einen einzelnen Sensor gebunden sein.
6. Hardwaredefinierte Sicherheitsgrenzen MÜSSEN Vorrang besitzen.
7. Sensorfehler und widersprüchliche Messwerte MÜSSEN erkannt werden können.
8. `Unknown` DARF nicht als sicherer Zustand interpretiert werden.
9. Hysterese DARF zur Stabilisierung von Zustandswechseln verwendet werden.
10. Einer Zone MÜSSEN mehrere Cooling Resources zugeordnet werden können.
11. Thermal State und Trend MÜSSEN für Thermal Policy verfügbar sein.
12. Zone, Sensoren, Grenzwerte, Zustand und aktive Maßnahmen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS bildet thermisch zusammenhängende Hardware in stabil identifizierten Thermal Zones ab. Mehrere Sensoren, Komponenten, Grenzwerte und Cooling Resources können gemeinsam bewertet werden, sodass Thermal Policy und Ressourcensteuerung auf einem hardwareunabhängigen und robusten thermischen Zustandsmodell aufbauen.