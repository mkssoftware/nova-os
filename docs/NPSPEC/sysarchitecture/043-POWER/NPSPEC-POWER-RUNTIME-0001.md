# NPSPEC-POWER-RUNTIME-0001 – Nova Runtime Power Management

## Status

Angenommen

## Kategorie

Power / Runtime

## Zweck

NovaOS definiert Runtime Power Management als automatische Energieverwaltung aktiver Systemkomponenten während des normalen Betriebs.

Nicht benötigte Hardware darf dynamisch in energiesparende Zustände wechseln und bei Bedarf reaktiviert werden, ohne dass das gesamte System suspendiert werden muss.

## Grundprinzipien

```text
Runtime Power ≠ System Suspend
Runtime Idle ≠ Device Removal
Idle ≠ Unused Forever
Power Saving ≠ Loss of State
Wake Request ≠ User Interaction
Runtime PM ≠ Power Policy
```

## Architektur

```text
Workload / I/O
      ↓
Activity Tracking
      ↓
Runtime Power Manager
      ↓
Power Policy
      ↓
Device / Power Domain
      ↓
Platform / HAL
```

Runtime Power Management koordiniert vorhandene Power-Mechanismen, definiert jedoch keine hardwarespezifischen Zustände.

## Runtime Context

```text
RuntimePowerContext
├── TargetID
├── ActivityState
├── CurrentPowerState
├── IdleDuration
├── TransitionLatency
├── Dependencies[]
├── WakeRequirements[]
├── Constraints
└── Policy
```

Ein Target kann beispielsweise sein:

```text
Device
Controller
Bus
CPU Domain
GPU
Accelerator
Power Domain
```

## Aktivität

NovaOS muss relevante Nutzung erkennen können:

```text
Active Requests
Pending I/O
Open Operations
Scheduled Work
Interrupt Activity
Dependencies
```

Fehlende Aktivität darf erst nach Berücksichtigung aller relevanten Abhängigkeiten als Idle interpretiert werden.

## Autosuspend

Targets dürfen nach einer definierten Inaktivitätszeit automatisch einen niedrigeren Energiezustand erreichen:

```text
Active
  ↓
Idle
  ↓ timeout
Autosuspend
  ↓
Low Power
```

Die Verzögerung darf dynamisch an Nutzungsmuster und Transition-Kosten angepasst werden.

## Runtime Resume

Neue Nutzung muss eine Reaktivierung auslösen:

```text
Request
   ↓
Runtime Resume
   ↓
State Restore
   ↓
Verify
   ↓
Operation
```

Eine Operation darf erst fortgesetzt werden, wenn der erforderliche Zustand tatsächlich erreicht wurde.

## Referenzen

Aktive Nutzer eines Targets dürfen dessen Suspend verhindern.

```text
Runtime Reference
├── Acquire
└── Release
```

Solange relevante Referenzen bestehen, darf kein inkompatibler Power State aktiviert werden.

Referenzen müssen kontrolliert freigegeben werden, damit Komponenten nicht dauerhaft unnötig aktiv bleiben.

## Abhängigkeiten

Runtime-Power-Abhängigkeiten müssen berücksichtigt werden:

```text
Device
  ↓
Controller
  ↓
Bus
  ↓
Power Domain
```

Ein übergeordnetes Target darf nur suspendiert werden, wenn alle abhängigen Anforderungen dies zulassen.

## Adaptive Optimierung

NovaOS darf anhand bisheriger Nutzung:

```text
Idle Duration
Wake Frequency
Transition Cost
Energy Saving
Latency Requirements
```

geeignete Suspend-Verzögerungen und Power States bestimmen.

Adaptive Entscheidungen dürfen Hard Constraints nicht verletzen.

## Fehlerverhalten

Schlägt Suspend oder Resume fehl:

```text
Failure
  ↓
Verify Effective State
  ↓
Safe Supported State
  ↓
Report / Degrade
```

Ein angeforderter Zustand darf niemals ungeprüft als erreicht gelten.

## Normative Anforderungen

1. NovaOS MUSS Runtime Power Management unabhängig vom globalen System-Suspend unterstützen.
2. Inaktive Targets MÜSSEN automatisch energiesparende Zustände erreichen können.
3. Neue Nutzung MUSS eine kontrollierte Reaktivierung auslösen können.
4. Aktive I/O- und Arbeitsanforderungen MÜSSEN vor Suspend berücksichtigt werden.
5. Runtime-Referenzen MÜSSEN Suspend kontrolliert verhindern können.
6. Abhängigkeiten zwischen Geräten, Controllern und Power Domains MÜSSEN berücksichtigt werden.
7. Transition-Latenz und erwartete Idle-Dauer SOLLEN bei der Zustandswahl berücksichtigt werden.
8. Autosuspend-Verzögerungen DÜRFEN adaptiv optimiert werden.
9. Runtime Power Management DARF Hard Constraints nicht verletzen.
10. Angeforderter und effektiver Power State MÜSSEN getrennt bleiben.
11. Fehlgeschlagene Suspend- oder Resume-Transitionen MÜSSEN sicher behandelt werden.
12. Aktivitätszustand, Runtime-Referenzen, Power State, Transitionen und Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`

## Ergebnis

NovaOS kann Hardware während des normalen Systembetriebs automatisch und fein granular in energiesparende Zustände versetzen. Aktivität, Abhängigkeiten, Transition-Kosten und Latenzanforderungen bestimmen dynamisch, wann Komponenten suspendiert oder reaktiviert werden, ohne einen globalen System-Suspend zu erfordern.