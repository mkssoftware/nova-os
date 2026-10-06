# NPSPEC-THERMAL-COOLING-0001 – Nova Thermal Cooling

## Status

Angenommen

## Kategorie

Thermal / Cooling

## Zweck

NovaOS definiert ein einheitliches Modell für aktive und passive Kühlmechanismen.

Cooling Resources werden Thermal Zones zugeordnet und durch Thermal Policy gesteuert, um thermische Reserven zu erhalten und Throttling möglichst zu vermeiden oder zu reduzieren.

## Grundprinzipien

```text
Cooling ≠ Throttling
Cooling Resource ≠ Thermal Zone
Cooling Capability ≠ Authority
Maximum Cooling ≠ Default Strategy
Cooling Failure ≠ Safe State
Safety > Noise > Performance Preference
```

## Modell

```text
CoolingResource
├── CoolingID
├── Type
├── Provider
├── AffectedZones[]
├── Levels[]
├── CurrentLevel
├── State
└── Capabilities
```

`CoolingID` identifiziert eine Cooling Resource unabhängig von Gerät, Treiber oder Thermal Zone.

## Cooling Resources

NovaOS darf unterschiedliche Kühlmechanismen verwenden:

```text
Fan
Pump
Passive Cooling
Platform Cooling
External Cooling
Firmware Controlled Cooling
Virtual Cooling
```

Eine Cooling Resource darf mehrere Thermal Zones beeinflussen.

## Steuerung

```text
Thermal Zones
      ↓
Thermal State / Headroom
      ↓
Thermal Policy
      ↓
Cooling Decision
      ↓
Cooling Provider
      ↓
Physical Cooling
```

Die Thermal Policy entscheidet über den gewünschten Kühlzustand. Die konkrete Hardwareansteuerung erfolgt über den zuständigen Provider.

## Cooling Levels

Cooling Resources dürfen diskrete oder kontinuierliche Leistungsstufen unterstützen.

```text
Off
Low
Medium
High
Maximum
```

Alternativ darf ein kontinuierlicher Sollwert verwendet werden.

Die tatsächliche Abbildung auf Drehzahl, Pumpenleistung oder andere Hardwareparameter bleibt providerspezifisch.

## Koordination

Mehrere Cooling Resources dürfen gemeinsam gesteuert werden:

```text
CPU Zone ──→ Fan A
         └─→ Pump

GPU Zone ──→ Fan B
         └─→ Pump
```

Thermal Coupling muss bei gemeinsam genutzten Cooling Resources berücksichtigt werden können.

## Policy

Cooling soll möglichst früh eingesetzt werden, wenn dadurch stärkeres Thermal Throttling vermieden werden kann.

Die Policy darf berücksichtigen:

```text
Thermal State
Headroom
Temperature Trend
Thermal Coupling
Cooling Efficiency
Energy Cost
Noise Preference
Workload Requirements
```

Sicherheitsrelevante Kühlung besitzt Vorrang vor Nutzerpräferenzen.

## Hardwaresteuerung

Firmware oder Hardware darf eigene Cooling-Regelungen besitzen.

NovaOS darf diese verwenden, ergänzen oder – sofern sicher unterstützt – kontrolliert steuern.

Hardwareseitige Sicherheitsmechanismen dürfen nicht deaktiviert oder umgangen werden.

## Fehlerverhalten

Cooling Resources besitzen mindestens folgende Zustände:

```text
Available
Degraded
Unavailable
Failed
Unknown
```

Bei Ausfall einer Cooling Resource muss der Thermal Manager verbleibende Kühlmöglichkeiten und verfügbare Thermal Headroom neu bewerten.

Falls notwendig müssen Throttling oder Emergency Protection aktiviert werden.

## Normative Anforderungen

1. Cooling Resources MÜSSEN unabhängig von Thermal Zones modelliert werden.
2. Jede Cooling Resource MUSS eine stabile `CoolingID` besitzen.
3. Eine Cooling Resource MUSS mehreren Thermal Zones zugeordnet werden können.
4. Mehrere Cooling Resources MÜSSEN gemeinsam steuerbar sein.
5. Diskrete und kontinuierliche Cooling Levels MÜSSEN unterstützt werden können.
6. Thermal Policy und konkrete Hardwaresteuerung MÜSSEN getrennt bleiben.
7. Thermal Headroom und Temperaturtrend SOLLEN bei Cooling-Entscheidungen berücksichtigt werden.
8. Thermal Coupling MUSS bei gemeinsam wirkenden Cooling Resources berücksichtigt werden können.
9. Hardwareseitige Schutzmechanismen DÜRFEN nicht umgangen werden.
10. Cooling-Fehler MÜSSEN eine Neubewertung der thermischen Ressourcen auslösen können.
11. Sicherheitsrelevante Kühlung MUSS Vorrang vor Komfort- und Performance-Präferenzen besitzen.
12. Cooling Resource, Zustand, Level und betroffene Zones MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-THERMAL-THROTTLING-0001`
- `NPSPEC-THERMAL-COUPLING-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`

## Ergebnis

NovaOS behandelt Kühlung als eigenständige, steuerbare Thermal-Ressource. Cooling Resources können mehreren Thermal Zones zugeordnet und koordiniert geregelt werden, sodass verfügbare thermische Reserven effizient genutzt und unnötiges Throttling vermieden werden, ohne Hardware-Sicherheitsgrenzen zu gefährden.