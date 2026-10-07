# NPSPEC-POWER-ENERGYACCOUNTING-0001 – Nova Energy Accounting

## Status

Angenommen

## Kategorie

Power / Energy Accounting

## Zweck

NovaOS definiert ein systemweites Energy Accounting zur Erfassung, Zuordnung und Auswertung des Energieverbrauchs.

Energieverbrauch soll soweit technisch möglich Geräten, Prozessen, Tasks, Apps, Solutions, Services und anderen Workloads zugeordnet werden können, ohne geschätzte Werte als exakte Messwerte darzustellen.

## Grundprinzipien

```text
Energy ≠ Power
Measured ≠ Estimated
Energy Usage ≠ Resource Authority
Accounting ≠ Power Policy
High Consumption ≠ Misbehavior
Attribution ≠ Physical Measurement
Unknown ≠ Zero
```

## Modell

```text
EnergyRecord
├── RecordID
├── SubjectID
├── ResourceID
├── StartTime
├── EndTime
├── Energy
├── AveragePower
├── PeakPower
├── Source
├── Confidence
└── State
```

`SubjectID` kann unter anderem referenzieren:

```text
Process
Task
Program
Solution
Service
Workspace
User Session
System
```

## Energiequellen

Messdaten dürfen aus unterschiedlichen Quellen stammen:

```text
Hardware Energy Counters
Battery Telemetry
Device Telemetry
Firmware
Platform Interfaces
Driver Statistics
Runtime Measurements
Derived Estimates
```

Die Herkunft eines Wertes muss nachvollziehbar bleiben.

## Messung und Schätzung

NovaOS unterscheidet ausdrücklich:

```text
Measured
Derived
Estimated
Unknown
```

Schätzwerte dürfen verwendet werden, wenn keine direkte Messung verfügbar ist.

Sie dürfen jedoch nicht als physisch exakt gemessene Energie dargestellt werden.

## Attribution

Gemeinsam genutzte Ressourcen erfordern eine Zuordnung:

```text
Shared Resource Energy
        ↓
Usage Accounting
        ↓
Attribution Model
        ↓
Subjects
```

Zuordnungsmodelle dürfen beispielsweise berücksichtigen:

```text
CPU Time
GPU Time
I/O Activity
Network Traffic
Device Residency
Memory Activity
Accelerator Usage
```

Nicht eindeutig zuordenbarer Verbrauch bleibt als gemeinsamer oder nicht zugeordneter Systemverbrauch sichtbar.

## Hierarchie

Energy Accounting muss aggregierbar sein:

```text
Task
 ↓
Process
 ↓
App / Solution
 ↓
Workspace / Session
 ↓
System
```

Dabei darf dieselbe Energie nicht mehrfach in übergeordneten Summen gezählt werden.

## Zeiträume

Verbrauch muss über unterschiedliche Zeiträume auswertbar sein:

```text
Instantaneous
Operation
Task Lifetime
Session
Since Boot
Custom Interval
```

Power beschreibt dabei eine momentane oder gemittelte Leistungsaufnahme, Energy die über Zeit verbrauchte Energie.

## Resource Economy

Energy Accounting liefert Daten an die Resource Economy:

```text
Energy Accounting
       ↓
Observed Consumption
       ↓
Resource Economy
       ↓
Budget / Optimization
```

Accounting selbst entscheidet nicht über Drosselung oder Ressourcenvergabe.

## Execution Contracts

Der tatsächliche Energieverbrauch darf mit einem vereinbarten Energy Budget verglichen werden.

```text
Execution Contract
       ↓
Energy Budget
       ↕
Measured / Estimated Usage
```

Budgetüberschreitungen werden an zuständige Policy-Mechanismen gemeldet.

## Datenschutz

Verbrauchsdaten können Rückschlüsse auf Nutzeraktivitäten ermöglichen.

Daher müssen detaillierte Energy Records denselben Zugriffs- und Datenschutzregeln wie vergleichbare Telemetriedaten unterliegen.

## Normative Anforderungen

1. NovaOS MUSS systemweites Energy Accounting unterstützen.
2. Energy und Power MÜSSEN getrennte Größen bleiben.
3. Gemessene und geschätzte Werte MÜSSEN unterscheidbar sein.
4. Unbekannter Verbrauch DARF nicht als Nullverbrauch interpretiert werden.
5. Energieverbrauch MUSS soweit möglich Subjects und Ressourcen zugeordnet werden können.
6. Nicht eindeutig zuordenbarer Verbrauch MUSS separat darstellbar bleiben.
7. Gemeinsame Ressourcen MÜSSEN über nachvollziehbare Attribution Models behandelt werden.
8. Energy Records MÜSSEN hierarchisch aggregierbar sein.
9. Aggregation DARF Energie nicht mehrfach zählen.
10. Energy Accounting DARF selbst keine zusätzliche Authority oder Power Policy erzeugen.
11. Energy Budgets aus Execution Contracts MÜSSEN mit tatsächlichem Verbrauch vergleichbar sein.
12. Herkunft, Messmethode, Confidence, Zeitraum und Attribution MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-BATTERY-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-ACCELERATOR-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Energieverbrauch systemweit erfassen, hierarchisch zuordnen und über definierte Zeiträume auswerten. Direkte Messungen, abgeleitete Werte und Schätzungen bleiben klar unterscheidbar, sodass Resource Economy und Execution Contracts auf nachvollziehbaren Energiedaten aufbauen können.