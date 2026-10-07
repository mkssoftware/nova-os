# NPSPEC-POWER-BATTERY-0001 – Nova Battery Management

## Status

Angenommen

## Kategorie

Power / Battery

## Zweck

NovaOS definiert ein einheitliches Modell zur Überwachung und Einbindung von Batterien und anderen wiederaufladbaren Energiespeichern.

Das Battery Management stellt Ladezustand, verfügbare Energie, Gesundheitszustand und Laufzeitprognosen für Power Policy, Resource Economy und Benutzeroberfläche bereit.

## Grundprinzipien

```text
Battery Level ≠ Remaining Runtime
Battery Capacity ≠ Energy Budget
Charge State ≠ Power Source
Battery Health ≠ Charge Level
Estimated Runtime ≠ Guaranteed Runtime
Unknown ≠ Empty
Unknown ≠ Full
```

## Modell

```text
Battery
├── BatteryID
├── State
├── ChargeLevel
├── EnergyRemaining
├── FullChargeCapacity
├── DesignCapacity
├── ChargeRate
├── DischargeRate
├── Health
├── Temperature
└── EstimatedRuntime
```

Nicht verfügbare Werte müssen explizit als unbekannt dargestellt werden.

## Batteriezustände

NovaOS unterscheidet mindestens:

```text
Charging
Discharging
Full
Idle
Critical
Unavailable
Unknown
```

Ladezustand und aktuelle Energiequelle bleiben getrennte Informationen.

## Mehrere Batterien

Ein System darf mehrere Energiespeicher besitzen:

```text
System
├── Internal Battery A
├── Internal Battery B
└── External Battery
```

NovaOS muss Einzelzustände sowie einen aggregierten Systemzustand bereitstellen können.

## Ladezustand

Der angezeigte Ladezustand wird aus validierten Hardwareinformationen bestimmt.

```text
Battery Telemetry
       ↓
Validation
       ↓
Normalized Battery State
       ↓
Power Manager
```

Ein Prozentwert darf nicht direkt als verbleibende Laufzeit interpretiert werden.

## Laufzeitprognose

NovaOS darf die verbleibende Laufzeit anhand von:

```text
Energy Remaining
Current Consumption
Recent Consumption
Workload
Power Profile
Battery Health
```

abschätzen.

Die Prognose muss als Schätzung behandelt und bei veränderter Last aktualisiert werden.

## Battery Health

NovaOS darf Gesundheitsinformationen erfassen:

```text
Design Capacity
Full Charge Capacity
Cycle Count
Temperature
Degradation
Hardware Health
```

Der Gesundheitszustand beeinflusst Prognosen, darf aber nicht mit dem aktuellen Ladezustand verwechselt werden.

## Kritischer Energiezustand

Bei geringer verfügbarer Energie kann NovaOS abgestuft reagieren:

```text
Low Battery
    ↓
Energy Saving
    ↓
Critical Battery
    ↓
Hibernate / Safe Shutdown
```

Safety und Datenintegrität besitzen Vorrang vor fortgesetzter Ausführung.

## Power Integration

Der Batteriezustand darf beeinflussen:

```text
Power Profile
CPU Performance
Boost
Display
Network
Accelerators
Background Work
Runtime Power Management
```

Explizite Benutzerentscheidungen bleiben wirksam, sofern keine kritischen Sicherheits- oder Energiegrenzen entgegenstehen.

## Hotplug

Batterien dürfen zur Laufzeit hinzugefügt oder entfernt werden.

```text
Battery Added / Removed
        ↓
Recalculate Energy State
        ↓
Reevaluate Power Policy
```

Der Verlust einer Batterie darf nicht automatisch als vollständiger Energieverlust interpretiert werden, wenn weitere Energiequellen vorhanden sind.

## Fehlerverhalten

Fehlende oder widersprüchliche Telemetrie muss erkannt werden.

```text
Invalid Telemetry
      ↓
Unknown / Degraded
      ↓
Conservative Power Policy
```

Unbekannte Batteriedaten dürfen nicht als sichere Energiereserve interpretiert werden.

## Normative Anforderungen

1. NovaOS MUSS Batterien als eigenständige Energiequellen modellieren können.
2. Mehrere Batterien MÜSSEN gleichzeitig unterstützt werden können.
3. Einzelne und aggregierte Batteriezustände MÜSSEN darstellbar sein.
4. Ladezustand, Kapazität, Gesundheit und Laufzeit MÜSSEN getrennte Werte bleiben.
5. Nicht verfügbare Batteriedaten MÜSSEN explizit als unbekannt behandelt werden.
6. Laufzeitprognosen MÜSSEN als Schätzungen behandelt werden.
7. Battery Health MUSS unabhängig vom aktuellen Ladezustand darstellbar sein.
8. Hotplug von Batterien MUSS unterstützt werden können.
9. Kritische Energiezustände MÜSSEN kontrollierte Schutzmaßnahmen auslösen können.
10. Datenintegrität MUSS bei kritischem Energiemangel Vorrang besitzen.
11. Batteriezustände DÜRFEN die systemweite Power Policy beeinflussen.
12. BatteryID, Ladezustand, Kapazität, Gesundheit, Energiefluss und Laufzeitprognose MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-POWER-HIBERNATE-0001`
- `NPSPEC-THERMAL-SAFETY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Battery Management für einzelne und mehrere Energiespeicher. Ladezustand, Energieinhalt, Gesundheitszustand und Laufzeitprognosen werden getrennt modelliert und ermöglichen der Power Architecture, Energieverbrauch und Schutzmaßnahmen dynamisch an die tatsächlich verfügbare Energie anzupassen.