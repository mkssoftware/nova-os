# NPSPEC-THERMAL-SENSOR-0001 – Nova Thermal Sensor

## Status

Angenommen

## Kategorie

Thermal / Sensor

## Zweck

NovaOS definiert ein einheitliches Modell für Temperatursensoren und andere thermisch relevante Messquellen.

Thermal Sensoren liefern Messdaten an Thermal Zones und den Thermal Manager. Sie erfassen Zustand, treffen jedoch selbst keine Thermal-Policy-Entscheidungen.

## Grundprinzipien

```text
Sensor ≠ Thermal Zone
Sensor ≠ Thermal Policy
Measurement ≠ Thermal State
Sensor Limit ≠ System Policy
Invalid Measurement ≠ Safe Value
Missing Sensor ≠ Zero Temperature
```

## Modell

```text
ThermalSensor
├── SensorID
├── Type
├── Source
├── Unit
├── Value
├── Precision
├── ValidRange
├── UpdateRate
├── State
└── Timestamp
```

`SensorID` identifiziert einen Sensor stabil und unabhängig von Treiberpfad, Gerätename oder aktueller Thermal Zone.

## Sensorquellen

Messwerte dürfen aus unterschiedlichen Quellen stammen:

```text
CPU Sensor
GPU Sensor
SoC Sensor
Memory Sensor
Storage Sensor
Battery Sensor
VRM Sensor
Board Sensor
Firmware Sensor
External Sensor
Virtual Sensor
```

Die konkrete Hardware wird durch HAL und Treiber abstrahiert.

## Messwerte

Intern sollen Temperaturen in einer einheitlichen kanonischen Einheit verarbeitet werden.

Ein Messwert besteht mindestens aus:

```text
Value
Timestamp
Validity
SensorID
```

Optional dürfen zusätzliche Informationen wie Genauigkeit, Rohwert oder Messintervall bereitgestellt werden.

## Sensorzustand

Ein Sensor besitzt einen expliziten Zustand:

```text
Available
Degraded
Stale
Invalid
Unavailable
Failed
```

Ein alter Messwert muss als `Stale` erkennbar sein und darf nicht unbegrenzt als aktueller Temperaturwert verwendet werden.

## Validierung

Messwerte werden vor ihrer Verwendung validiert:

```text
Raw Measurement
      ↓
Range Check
      ↓
Plausibility Check
      ↓
Freshness Check
      ↓
Validated Measurement
```

Offensichtlich ungültige Werte dürfen nicht als reguläre Temperaturmessung verwendet werden.

## Aktualisierung

Sensoren dürfen unterschiedliche Aktualisierungsraten besitzen.

NovaOS darf die Abfragerate abhängig von Thermalzustand, Hardwareeigenschaften und Energieverbrauch anpassen.

Bei steigender thermischer Belastung darf die Messfrequenz erhöht werden.

## Thermal Zones

Ein Sensor darf einer oder mehreren Thermal Zones zugeordnet sein:

```text
ThermalSensor
    ├── CPU Zone
    └── Platform Zone
```

Die Zuordnung erzeugt keine eigenständige Policy im Sensor.

## Virtuelle Sensoren

NovaOS darf virtuelle oder berechnete Sensoren bereitstellen.

Diese können Werte aus mehreren Messquellen ableiten:

```text
Sensor A ─┐
Sensor B ─┼─→ Virtual Sensor
Sensor C ─┘
```

Virtuelle Messwerte müssen als abgeleitet erkennbar bleiben.

## Fehlerverhalten

Bei Sensorfehlern darf NovaOS keinen erfundenen sicheren Temperaturwert verwenden.

```text
Invalid / Missing Measurement
        ↓
Unknown Thermal Information
        ↓
Conservative Thermal Handling
```

Kritischer Hardware-Schutz darf nicht ausschließlich von einem einzelnen softwaregelesenen Sensor abhängig sein.

## Normative Anforderungen

1. Jeder Thermal Sensor MUSS eine stabile `SensorID` besitzen.
2. Sensoridentität MUSS von Treiberpfad und Thermal Zone getrennt bleiben.
3. Messwerte MÜSSEN einen Zeitbezug besitzen.
4. Ungültige und veraltete Messwerte MÜSSEN erkennbar sein.
5. Fehlende Messwerte DÜRFEN nicht als Null- oder sichere Temperatur interpretiert werden.
6. Messwerte MÜSSEN vor ihrer Verwendung validierbar sein.
7. Ein Sensor DARF mehreren Thermal Zones zugeordnet sein.
8. Sensoren DÜRFEN keine eigenständige systemweite Thermal Policy durchsetzen.
9. Virtuelle Sensoren MÜSSEN als abgeleitete Messquellen erkennbar sein.
10. Sensorabfragen DÜRFEN dynamisch an den Thermalzustand angepasst werden.
11. Sensorfehler MÜSSEN konservativ behandelbar sein.
12. SensorID, Wert, Zustand, Aktualität und Zuordnung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches und hardwareunabhängiges Sensormodell für thermische Messdaten. Sensorwerte werden validiert, zeitlich bewertet und Thermal Zones zugeordnet, während Thermal Policy und Schutzentscheidungen außerhalb der Sensoren bleiben.