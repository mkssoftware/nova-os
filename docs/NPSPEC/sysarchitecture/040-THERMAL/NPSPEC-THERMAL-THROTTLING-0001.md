# NPSPEC-THERMAL-THROTTLING-0001 – Nova Thermal Throttling

## Status

Angenommen

## Kategorie

Thermal / Throttling

## Zweck

NovaOS definiert Thermal Throttling als kontrollierte Reduzierung von Leistung oder Ressourcenverbrauch, wenn thermische Grenzen dies erfordern.

Throttling ist eine Schutz- und Regelmaßnahme innerhalb der Thermal-Architektur und wird koordiniert mit Scheduler, Resource Economy, Power Management und Execution Contracts durchgeführt.

## Grundprinzipien

```text
Throttling ≠ Failure
Throttling ≠ Cooling Only
Throttling ≠ Fixed Frequency Limit
Thermal State ≠ Throttle Level
Performance Preference < Thermal Safety
Hardware Protection > Software Policy
```

## Modell

```text
Thermal State
     +
Headroom
     +
Temperature Trend
     ↓
Thermal Policy
     ↓
Throttle Decision
     ↓
Resource Constraints
```

## Throttling-Ziele

Thermal Throttling darf auf unterschiedliche Ressourcen angewendet werden:

```text
CPU
GPU
Accelerator
Memory
Storage
Device
Workload
```

Dabei können beispielsweise Frequenz, Leistungsaufnahme, Parallelität oder zulässige Ressourcenbudgets reduziert werden.

## Throttle Level

NovaOS abstrahiert die Stärke einer Drosselung:

```text
None
Light
Moderate
Strong
Critical
Emergency
```

Die konkrete technische Umsetzung bleibt der jeweiligen Hardware und dem zuständigen Provider überlassen.

## Steuerung

Thermal Throttling soll schrittweise erfolgen:

```text
Normal Operation
      ↓
Reduce Boost
      ↓
Reduce Power / Frequency
      ↓
Reduce Workload Budget
      ↓
Migrate / Suspend Workload
      ↓
Emergency Protection
```

Nicht jede Plattform muss jede Stufe unterstützen.

## Scheduler-Integration

Der Scheduler darf gedrosselte Ressourcen bei der Workload-Platzierung berücksichtigen.

Wenn geeignete thermische Reserven vorhanden sind, darf Last auf kühlere Ressourcen verschoben werden, bevor stärkere Drosselung notwendig wird.

## Execution Contracts

Throttling kann die erfüllbaren Eigenschaften eines Execution Contracts verändern.

```text
Requested Budget
      ∩
Thermal Constraint
      =
Effective Budget
```

Hard Requirements dürfen nicht stillschweigend verletzt werden. Kann ein Vertrag thermisch nicht mehr erfüllt werden, muss dies als Constraint-Verletzung behandelt werden.

## Hardware-Throttling

Hardware darf unabhängig von NovaOS eigene Schutzmechanismen aktivieren.

NovaOS muss solche Zustände erkennen können und darf sie nicht umgehen.

```text
Hardware Throttling
        ↓
Observed Constraint
        ↓
Resource Economy
```

## Hysterese

Throttling darf Hysterese und zeitliche Stabilisierung verwenden, um schnelle Wechsel zwischen Leistungsstufen zu vermeiden.

Eine Reduzierung darf schneller erfolgen als ihre Rücknahme, wenn dies thermisch sinnvoll ist.

## Emergency

Bei kritischer thermischer Gefahr darf NovaOS stärkere Maßnahmen ausführen:

```text
Throttle
Suspend
Disable Device
Emergency Shutdown
```

Thermische Sicherheit besitzt dabei Vorrang vor Performance und Nutzerpräferenzen.

## Normative Anforderungen

1. Thermal Throttling MUSS durch den Thermalzustand steuerbar sein.
2. Throttling MUSS unterschiedliche Hardware- und Ressourcenklassen unterstützen können.
3. Die konkrete Hardwaresteuerung MUSS von der übergeordneten Thermal Policy getrennt bleiben.
4. Throttling SOLL abgestuft erfolgen.
5. Hardwareseitige Schutzmechanismen DÜRFEN nicht umgangen werden.
6. Hardware-Throttling MUSS als Ressourcenconstraint erkennbar sein können.
7. Scheduler und Resource Economy MÜSSEN aktive Thermal Constraints berücksichtigen können.
8. Execution Contracts MÜSSEN unter veränderten Thermal Constraints neu bewertet werden können.
9. Hard Requirements DÜRFEN nicht stillschweigend durch Throttling verletzt werden.
10. Hysterese DARF zur Stabilisierung verwendet werden.
11. Kritische Zustände MÜSSEN stärkere Schutzmaßnahmen bis zum Emergency Shutdown ermöglichen.
12. Ursache, Ziel, Stärke und Dauer einer Drosselung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-THERMAL-ZONE-0001`
- `NPSPEC-THERMAL-SENSOR-0001`
- `NPSPEC-THERMAL-HEADROOM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt Thermal Throttling als kontrollierten systemweiten Ressourcenconstraint. Leistung wird nur soweit reduziert, wie es der thermische Zustand erfordert, während Scheduler, Resource Economy und Execution Contracts koordiniert reagieren und Hardware-Sicherheitsgrenzen jederzeit Vorrang behalten.