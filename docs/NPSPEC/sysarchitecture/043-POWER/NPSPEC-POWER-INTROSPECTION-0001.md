# NPSPEC-POWER-INTROSPECTION-0001 – Nova Power Introspection

## Status

Angenommen

## Kategorie

Power / Introspection

## Zweck

NovaOS definiert eine einheitliche Introspection-Schnittstelle für Power Management.

Sie macht Energiezustände, Energieverbrauch, Budgets, Policies, Hardwaregrenzen und Power-Entscheidungen nachvollziehbar, ohne dadurch Steuerungsrechte oder zusätzliche Authority zu erzeugen.

## Grundprinzipien

```text
Introspection ≠ Authority
Observation ≠ Control
Reported State ≠ Requested State
Measured ≠ Estimated
Unknown ≠ Zero
Power State ≠ Power Policy
```

## Modell

```text
PowerIntrospection
├── SystemState
├── PowerSources[]
├── PowerDomains[]
├── Devices[]
├── EnergyAccounting
├── EnergyBudgets[]
├── EffectivePolicy
├── ActiveConstraints[]
├── WakeState
└── Diagnostics
```

## Systeminformationen

Mindestens folgende Informationen müssen abfragbar sein:

```text
Current Power Profile
Power Source
Battery State
System Power State
Low Power Idle State
Suspend / Hibernate State
Active Wake Leases
Wake Sources
Energy Consumption
Energy Budgets
Thermal Constraints
```

## Komponenten

Für unterstützte Komponenten dürfen detaillierte Zustände bereitgestellt werden:

```text
CPU
├── Idle State
├── Performance State
├── DVFS State
└── Boost State

Devices
├── Power State
├── Runtime State
└── Wake State

Accelerators
├── Power State
├── Performance Level
└── Utilization
```

## Entscheidungsnachvollziehbarkeit

NovaOS muss erklären können, warum ein bestimmter Power-Zustand gewählt oder verhindert wurde.

```text
Requested State
      +
Policy
      +
Execution Contract
      +
Energy Budget
      +
Thermal Constraint
      +
Hardware Limit
        ↓
Effective Decision
```

Beispiel:

```text
Requested: Deep Idle
Effective: Shallow Idle
Reason: Wake latency exceeds active deadline
```

## Herkunft

Power-Daten müssen ihre Herkunft kenntlich machen können:

```text
Hardware Measurement
Firmware
Driver
Platform Provider
Energy Accounting
Derived
Estimated
Unknown
```

Geschätzte Werte dürfen nicht als direkt gemessen dargestellt werden.

## Historie

NovaOS darf relevante Power-Ereignisse protokollieren:

```text
Power State Transition
Wake Event
Suspend / Resume
Hibernate / Resume
Policy Change
Budget Violation
Boost Activation
Thermal Restriction
Device Power Failure
```

Die Historie muss zeitlich zuordenbar und diagnostisch nutzbar sein.

## Hierarchie

Power-Zustände müssen entlang der Systemstruktur untersuchbar sein:

```text
System
├── Power Domain
│   ├── CPU
│   ├── GPU
│   └── Device
├── App / Solution
├── Process
└── Task
```

Energy Accounting darf Verbrauch mit diesen Kontexten verbinden.

## Sicherheit und Datenschutz

Introspection darf keine Steuerungsrechte erzeugen.

Sensible Informationen über:

```text
User Activity
Workloads
Applications
Network Activity
Energy Usage
```

müssen durch Capability-, Privacy- und Policy-Regeln geschützt werden.

## Fehlerdiagnose

Power Introspection muss Zustände wie:

```text
Unavailable
Unsupported
Degraded
Unknown
Failed
```

klar unterscheiden können.

Fehlende Telemetrie darf nicht als normaler oder sicherer Zustand dargestellt werden.

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche Power-Introspection-Schnittstelle bereitstellen.
2. Introspection DARF keine zusätzliche Authority erzeugen.
3. Angeforderte und effektive Power-Zustände MÜSSEN unterscheidbar sein.
4. Gemessene, abgeleitete und geschätzte Werte MÜSSEN unterscheidbar sein.
5. Power-Entscheidungen MÜSSEN ihre relevanten Constraints nachvollziehbar machen können.
6. Power Sources, Domains, Geräte und Systemzustände MÜSSEN introspektierbar sein.
7. Energy Accounting und Energy Budgets MÜSSEN einsehbar sein können.
8. Wake Sources und aktive Wake Leases MÜSSEN nachvollziehbar sein.
9. Power-Transitionen und relevante Fehler SOLLEN historisch nachvollziehbar sein.
10. `Unknown`, `Unavailable`, `Unsupported`, `Degraded` und `Failed` MÜSSEN unterscheidbar bleiben.
11. Sensible Power-Telemetrie MUSS bestehenden Sicherheits- und Datenschutzregeln unterliegen.
12. Introspection MUSS maschinenlesbare Daten für Diagnose, UI und Systemwerkzeuge bereitstellen können.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-POLICY-0001`
- `NPSPEC-POWER-ENERGYACCOUNTING-0001`
- `NPSPEC-POWER-ENERGYBUDGET-0001`
- `NPSPEC-POWER-WAKE-0001`
- `NPSPEC-POWER-WAKELEASE-0001`
- `NPSPEC-POWER-BATTERY-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS macht das gesamte Power Management nachvollziehbar. Aktuelle Zustände, Energieverbrauch, Budgets, Policies, Wake-Ursachen, Hardwaregrenzen und Entscheidungsgründe können einheitlich untersucht werden, ohne Beobachtung mit Steuerungsrechten oder Authority zu vermischen.