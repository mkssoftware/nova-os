# NPSPEC-THERMAL-FAN-0001 – Nova Thermal Fan

## Status

Angenommen

## Kategorie

Thermal / Fan

## Zweck

NovaOS definiert ein einheitliches Modell für Lüfter als aktive Cooling Resources.

Die Thermal-Architektur steuert Lüfter über abstrahierte Fan Provider, ohne hardware- oder herstellerspezifische Steuerlogik in die Thermal Policy einzubauen.

## Grundprinzipien

```text
Fan ≠ Thermal Zone
Fan ≠ Thermal Policy
Fan Speed ≠ Cooling Effect
Requested Speed ≠ Actual Speed
Fan Failure ≠ Safe State
Hardware Safety > User Preference
```

## Modell

```text
ThermalFan
├── FanID
├── Provider
├── AffectedZones[]
├── MinSpeed
├── MaxSpeed
├── RequestedSpeed
├── ActualSpeed
├── ControlMode
└── State
```

`FanID` identifiziert einen Lüfter stabil und unabhängig von Treiberpfad, Anschluss oder Thermal Zone.

## Steuerungsmodi

Ein Lüfter darf unterschiedliche Steuerungsmodi unterstützen:

```text
Automatic
Firmware
Manual
Fixed
Unavailable
```

`Automatic` bedeutet Steuerung durch die NovaOS Thermal Policy.

`Firmware` überlässt die Regelung der Plattform oder Firmware.

`Manual` erlaubt eine autorisierte direkte Vorgabe innerhalb sicherer Grenzen.

## Geschwindigkeit

Lüfter dürfen diskrete Stufen oder kontinuierliche Sollwerte unterstützen.

```text
Requested Speed
      ↓
Fan Provider
      ↓
Hardware
      ↓
Actual Speed
```

Soll- und Ist-Geschwindigkeit müssen getrennt behandelt werden.

Hardwareabhängige Einheiten wie RPM oder PWM werden durch den Provider abstrahiert.

## Thermal Zones

Ein Lüfter darf eine oder mehrere Thermal Zones beeinflussen:

```text
Fan A
├── CPU Zone
├── VRM Zone
└── Platform Zone
```

Mehrere Lüfter dürfen gemeinsam dieselbe Zone kühlen.

## Regelung

Die Thermal Policy darf Lüfterleistung anhand folgender Informationen bestimmen:

```text
Thermal State
Headroom
Temperature Trend
Thermal Coupling
Current Fan State
Cooling Requirement
Noise Preference
```

Schnelle Änderungen sollen durch geeignete Rampen oder Hysterese vermieden werden, sofern keine kritische thermische Situation besteht.

## Fehlererkennung

NovaOS muss Abweichungen zwischen angeforderter und tatsächlicher Lüfterleistung erkennen können.

Mögliche Zustände sind:

```text
Available
Degraded
Stalled
Unavailable
Failed
Unknown
```

Ein blockierter oder ausgefallener Lüfter muss eine Neubewertung der betroffenen Thermal Zones auslösen.

## Sicherheit

Hardware- und Firmware-Sicherheitsmechanismen dürfen nicht umgangen werden.

Nutzerdefinierte Lüftereinstellungen dürfen nur innerhalb sicherer Grenzen wirken.

Bei kritischen Temperaturen darf NovaOS Komfort-, Lautstärke- und manuelle Präferenzen übersteuern.

## Normative Anforderungen

1. Jeder Lüfter MUSS eine stabile `FanID` besitzen.
2. Lüfter MÜSSEN als Cooling Resources modelliert werden.
3. Ein Lüfter MUSS mehreren Thermal Zones zugeordnet werden können.
4. Soll- und Ist-Geschwindigkeit MÜSSEN getrennt darstellbar sein.
5. Diskrete und kontinuierliche Lüftersteuerung MÜSSEN unterstützt werden können.
6. Hardwaredetails MÜSSEN durch Fan Provider abstrahiert werden.
7. Thermal Policy und Hardwaresteuerung MÜSSEN getrennt bleiben.
8. Lüfteränderungen SOLLEN Rampen oder Hysterese unterstützen.
9. Lüfterausfälle und Stillstand MÜSSEN erkennbar sein.
10. Ein Lüfterfehler MUSS eine thermische Neubewertung auslösen können.
11. Sicherheitsanforderungen MÜSSEN Nutzer- und Geräuschpräferenzen übersteuern können.
12. FanID, Modus, Sollwert, Istwert, Zustand und betroffene Zones MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-THERMAL-COOLING-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-COUPLING-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`

## Ergebnis

NovaOS behandelt Lüfter als abstrahierte aktive Cooling Resources. Die Thermal Policy kann ihre Kühlleistung kontrolliert anfordern, während Fan Provider die konkrete Hardwaresteuerung übernehmen und Ausfälle, tatsächliche Drehzahl sowie Sicherheitsgrenzen zuverlässig überwacht werden.