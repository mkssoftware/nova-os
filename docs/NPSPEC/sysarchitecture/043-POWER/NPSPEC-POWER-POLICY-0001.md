# NPSPEC-POWER-POLICY-0001 – Nova Power Policy

## Status

Angenommen

## Kategorie

Power / Policy

## Zweck

NovaOS definiert eine zentrale Power Policy, die Energie-, Performance- und Power-Entscheidungen systemweit koordiniert.

Die Policy bestimmt Ziele und Prioritäten, während konkrete Hardwaremechanismen durch Power Manager, HAL, Treiber und Platform Provider umgesetzt werden.

## Grundprinzipien

```text
Policy ≠ Mechanism
Policy ≠ Power State
Policy ≠ Energy Budget
Policy ≠ Hardware Limit
Preference ≠ Hard Constraint
Optimization ≠ Authority
```

## Prioritäten

Power-Entscheidungen folgen grundsätzlich:

```text
Safety
  ↓
Security / Integrity
  ↓
Hardware Constraints
  ↓
Explicit User Decisions
  ↓
Execution Contracts
  ↓
Energy Budgets
  ↓
System Policy
  ↓
Adaptive Optimization
```

Eine niedrigere Ebene darf keine höhere Einschränkung verletzen.

## Modell

```text
PowerPolicy
├── PolicyID
├── Scope
├── Profile
├── Constraints[]
├── Preferences[]
├── EnergyTargets[]
├── Overrides[]
└── State
```

## Scope

Policies dürfen gelten für:

```text
System
User
Session
Workspace
App
Solution
Task
Device
Power Domain
```

Spezifischere Policies dürfen allgemeinere Policies verfeinern, aber keine übergeordneten Hard Constraints aufheben.

## Power Profiles

NovaOS darf Profile bereitstellen wie:

```text
Performance
Balanced
Efficiency
Battery Saver
Critical
Custom
```

Ein Profil ist eine Sammlung von Policy-Präferenzen und keine feste Hardwarekonfiguration.

## Policy-Auflösung

```text
Hardware Limits
      +
System Constraints
      +
User Preferences
      +
Execution Contracts
      +
Energy Budgets
      +
Current Context
        ↓
Policy Resolution
        ↓
Effective Power Policy
```

Die Auflösung muss deterministisch nachvollziehbar sein.

## Kontext

Power Policy darf unter anderem berücksichtigen:

```text
Power Source
Battery State
Thermal Headroom
Workload
User Activity
Energy Budget
Performance Demand
Latency
Deadline
Device Availability
```

Eine Kontextänderung darf eine erneute Policy-Auswertung auslösen.

## Auswirkungen

Die effektive Policy darf Entscheidungen beeinflussen für:

```text
CPU Performance
CPU Idle
Boost
Accelerators
Devices
Display
Storage
Network
Runtime Power
Suspend
Low Power Idle
Background Work
```

Die konkrete Umsetzung verbleibt bei den jeweiligen Subsystemen.

## Benutzerkontrolle

Explizite Benutzerentscheidungen sollen erhalten bleiben, solange keine höheren Constraints entgegenstehen.

Beispiele:

```text
Preferred Power Profile
Display Timeout
Performance Preference
Wake Configuration
Battery Saving Preference
```

NovaOS darf diese Einstellungen adaptiv ergänzen, aber nicht stillschweigend entgegen ihrer Bedeutung umkehren.

## Adaptive Policy

NovaOS darf Nutzungsmuster und Energy Accounting für Optimierungen verwenden.

```text
Observed Behavior
      ↓
Adaptive Optimization
      ↓
Policy Recommendation
```

Adaptive Entscheidungen bleiben optional und müssen durch explizite Einstellungen oder Hard Constraints übersteuerbar sein.

## Konflikte

Bei widersprüchlichen Anforderungen muss NovaOS die Priorität der beteiligten Constraints bestimmen.

```text
Performance Request
       ↕
Energy Budget
       ↕
Thermal Limit
       ↓
Constraint Resolution
```

Nicht erfüllbare Anforderungen müssen als solche erkennbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS Power Policy und Hardwaremechanismen trennen.
2. Power-Entscheidungen MÜSSEN einer definierten Constraint-Priorität folgen.
3. Safety- und Hardwaregrenzen DÜRFEN durch Policy nicht abgeschwächt werden.
4. Policies MÜSSEN unterschiedliche Scopes besitzen können.
5. Spezifische Policies DÜRFEN übergeordnete Hard Constraints nicht umgehen.
6. Power Profiles MÜSSEN von konkreten Hardwarezuständen getrennt bleiben.
7. Execution Contracts und Energy Budgets MÜSSEN in die Policy-Auflösung einfließen können.
8. Kontextänderungen MÜSSEN eine Neubewertung ermöglichen.
9. Explizite Benutzerentscheidungen MÜSSEN gegenüber adaptiver Optimierung Vorrang besitzen.
10. Adaptive Optimierung DARF keine zusätzliche Authority erzeugen.
11. Nicht erfüllbare Policy-Anforderungen MÜSSEN erkennbar sein.
12. Effektive Policy, Herkunft, Constraints, Overrides und Entscheidungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-ENERGYACCOUNTING-0001`
- `NPSPEC-POWER-ENERGYBUDGET-0001`
- `NPSPEC-POWER-BATTERY-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine zentrale, hierarchische Power Policy, die Benutzerpräferenzen, Execution Contracts, Energy Budgets, Hardwaregrenzen und aktuelle Systembedingungen zu einer effektiven Energiepolitik zusammenführt. Die Policy entscheidet über Ziele und Prioritäten, während die jeweiligen Power-Subsysteme die konkrete Hardwaresteuerung übernehmen.