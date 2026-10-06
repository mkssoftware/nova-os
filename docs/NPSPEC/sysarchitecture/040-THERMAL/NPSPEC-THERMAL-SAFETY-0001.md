# NPSPEC-THERMAL-SAFETY-0001 – Nova Thermal Safety

## Status

Angenommen

## Kategorie

Thermal / Safety

## Zweck

NovaOS definiert verbindliche Sicherheitsregeln zum Schutz von Hardware, Daten und Systemstabilität bei thermischen Grenz- und Fehlerzuständen.

Thermal Safety bildet die höchste Schutzebene der Thermal-Architektur und darf Performance-, Komfort- und Nutzerpräferenzen übersteuern.

## Grundprinzipien

```text
Safety > Performance
Safety > Energy Optimization
Safety > Noise Preference
Safety > User Preference
Unknown ≠ Safe
Hardware Protection > Software Policy
Fail Safe > Continue Unsafely
```

## Sicherheitsmodell

```text
Sensors
   ↓
Thermal Zones
   ↓
Safety Evaluation
   ↓
Thermal State
   ↓
Protection Action
```

Sicherheitsentscheidungen berücksichtigen mindestens:

```text
Temperature
Thermal Limits
Headroom
Temperature Trend
Sensor State
Cooling State
Thermal Coupling
Hardware Protection State
```

## Schutzstufen

NovaOS darf Schutzmaßnahmen abgestuft einsetzen:

```text
Normal
   ↓
Increase Cooling
   ↓
Reduce Performance
   ↓
Strong Throttling
   ↓
Suspend Workloads
   ↓
Disable Component
   ↓
Emergency Shutdown
```

Bei unmittelbarer Gefahr dürfen Zwischenstufen übersprungen werden.

## Hardware-Sicherheitsgrenzen

Von Hardware, Firmware oder vertrauenswürdigen Plattforminformationen definierte kritische Grenzwerte gelten als harte Grenzen.

NovaOS darf diese nicht durch Nutzerkonfiguration, Thermal Policy oder Performance-Anforderungen abschwächen.

Hardwareeigene Schutzmechanismen bleiben unabhängig von NovaOS zulässig und dürfen nicht deaktiviert werden.

## Fehlerzustände

Thermal Safety muss auch bei unvollständigen Informationen funktionieren.

Kritische Fälle umfassen:

```text
Sensor Failure
Sensor Stale
Cooling Failure
Fan Stall
Unknown Temperature
Invalid Limits
Thermal Manager Failure
Unexpected Temperature Rise
```

Kann ein sicherer Zustand nicht bestätigt werden, muss NovaOS konservativ reagieren.

## Emergency Protection

Bei unmittelbar gefährlichem Thermalzustand darf NovaOS ohne vorherige Nutzerbestätigung:

```text
Throttle
Suspend
Revoke Resource Budgets
Disable Devices
Stop Workloads
Initiate Emergency Shutdown
```

Ein Emergency Shutdown soll kontrolliert erfolgen, sofern die verbleibende thermische Zeit dies erlaubt.

Ist dies nicht möglich, besitzt Hardware-Schutz Vorrang vor Datenpersistenz.

## Unabhängigkeit

Kritischer Thermal-Schutz darf nicht ausschließlich von einem einzelnen Userspace-Dienst, Sensor oder Cooling Provider abhängen.

Für sicherheitskritische Plattformen muss ein minimaler Schutzpfad auch bei Ausfall höherer Thermal-Komponenten erhalten bleiben.

```text
Hardware Protection
      +
Minimal Nova Protection
      +
Thermal Manager
```

## Wiederherstellung

Nach einem kritischen Zustand darf die normale Leistung erst wieder freigegeben werden, wenn:

```text
Temperature Safe
AND
Headroom Sufficient
AND
Cooling Operational
AND
Critical Fault Cleared
```

Hysterese und Cooldown-Zeiten dürfen ein sofortiges Wiederhochfahren verhindern.

## Normative Anforderungen

1. Thermal Safety MUSS Vorrang vor Performance- und Komfortzielen besitzen.
2. Hardware-Sicherheitsgrenzen DÜRFEN nicht durch Software abgeschwächt werden.
3. `Unknown` DARF nicht als sicherer Thermalzustand interpretiert werden.
4. Sensor- und Cooling-Fehler MÜSSEN sicher behandelt werden.
5. Kritische Schutzmaßnahmen MÜSSEN automatisch ausgelöst werden können.
6. Bei unmittelbarer Gefahr DÜRFEN Schutzstufen übersprungen werden.
7. Emergency Protection DARF keine Nutzerbestätigung voraussetzen.
8. Kritischer Thermal-Schutz DARF nicht von einer einzelnen Userspace-Komponente abhängen.
9. Hardwareeigene Schutzmechanismen DÜRFEN nicht deaktiviert oder umgangen werden.
10. Nach einem kritischen Zustand MUSS die thermische Sicherheit vor vollständiger Leistungsfreigabe erneut bestätigt werden.
11. Hysterese und Cooldown MÜSSEN unterstützt werden können.
12. Ursache, Schutzstufe, ausgelöste Maßnahmen und Thermalzustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-THERMAL-SENSOR-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-THROTTLING-0001`
- `NPSPEC-THERMAL-COUPLING-0001`
- `NPSPEC-THERMAL-COOLING-0001`
- `NPSPEC-THERMAL-FAN-0001`

## Ergebnis

NovaOS besitzt eine mehrstufige Thermal-Sicherheitsarchitektur, die auch bei Sensor-, Kühlungs- oder Softwarefehlern einen sicheren Systemzustand anstrebt. Kritische Hardwaregrenzen bleiben unverhandelbar und können automatische Schutzmaßnahmen bis hin zum Emergency Shutdown auslösen.