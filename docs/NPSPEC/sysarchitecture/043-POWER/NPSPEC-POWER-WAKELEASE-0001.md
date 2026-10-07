# NPSPEC-POWER-WAKELEASE-0001 – Nova Wake Lease

## Status

Angenommen

## Kategorie

Power / Wake Lease

## Zweck

NovaOS definiert Wake Leases als zeitlich und funktional begrenzte Anforderungen, die verhindern, dass ein benötigter Systemteil während einer laufenden Aufgabe in einen inkompatiblen Energiesparzustand wechselt.

Wake Leases ersetzen dauerhaftes „Keep Awake“ durch kontrollierte, nachvollziehbare und automatisch auslaufende Anforderungen.

## Grundprinzipien

```text
Wake Lease ≠ Wake Source
Wake Lease ≠ Power State
Wake Lease ≠ Permission
Wake Lease ≠ Permanent Keep-Awake
Wake Lease ≠ Authority
Lease Expiration ≠ Task Failure
```

## Modell

```text
WakeLease
├── LeaseID
├── OwnerID
├── Scope
├── Reason
├── MinimumPowerState
├── Expiration
├── Constraints
└── State
```

## Scope

Ein Wake Lease darf nur den tatsächlich benötigten Bereich betreffen:

```text
Device
Power Domain
CPU
Display
Network
Storage
Accelerator
System
```

Ein Lease für ein Netzwerkgerät darf beispielsweise nicht automatisch verhindern, dass Display oder andere unabhängige Geräte Energie sparen.

## Ablauf

```text
Task / Service
      ↓
Request Wake Lease
      ↓
Validate
      ↓
Grant Limited Lease
      ↓
Required Work
      ↓
Release / Expire
      ↓
Normal Power Policy
```

Nach Ende des Leases entscheidet die normale Power Policy erneut über den Energiezustand.

## Lebensdauer

Wake Leases müssen begrenzt sein durch mindestens eines der folgenden Kriterien:

```text
Explicit Release
Timeout
Task Lifetime
Process Lifetime
Operation Completion
Cancellation
Owner Termination
```

Unbegrenzte Leases sollen vermieden werden.

## Mindestzustand

Ein Lease beschreibt den minimal erforderlichen Zustand und nicht zwingend vollständige Aktivität.

Beispiel:

```text
Download
├── Network → Active
├── Storage → Available
├── Display → darf Off sein
└── System → Low Power möglich
```

Dadurch kann NovaOS trotz aktiver Hintergrundarbeit weiterhin Energie sparen.

## Aggregation

Mehrere Wake Leases dürfen gleichzeitig existieren:

```text
Lease A ─┐
Lease B ─┼→ Effective Power Constraint
Lease C ─┘
```

Der Power Manager bestimmt daraus die minimale Zustandsmenge, welche alle gültigen Leases erfüllt.

## Verlängerung

Ein Lease darf verlängert werden, wenn die zugehörige Operation weiterhin läuft.

```text
Lease
  ↓
Renew Request
  ↓
Validation
  ↓
Extended Lease
```

Automatische Verlängerungen müssen begrenzt und nachvollziehbar bleiben.

## Missbrauchsschutz

NovaOS darf Wake Leases begrenzen anhand von:

```text
Policy
Resource Budget
Background Execution Rules
Energy State
Battery State
Trust
Usage History
```

Ein Lease darf keine Capability-, Sandbox- oder Ressourcenbegrenzung umgehen.

## Suspend und Low Power Idle

Ein Wake Lease kann bestimmte Power States verhindern, ohne zwingend alle Energiesparzustände zu blockieren.

```text
Requested Power State
        +
Active Wake Leases
        ↓
Allowed Power State
```

Ein System-Suspend darf blockiert werden, wenn ein gültiger Lease eine laufende kritische Operation schützt.

## Fehlerverhalten

Verliert ein Owner seinen Ausführungskontext, müssen dessen nicht übertragene Leases automatisch freigegeben werden.

```text
Owner Failure
     ↓
Lease Revocation
     ↓
Power Re-evaluation
```

Verwaiste Wake Leases dürfen das System nicht dauerhaft aktiv halten.

## Normative Anforderungen

1. NovaOS MUSS Wake Leases als begrenzte Power-Anforderungen unterstützen.
2. Wake Lease und Wake Source MÜSSEN getrennte Konzepte bleiben.
3. Ein Wake Lease DARF keine Authority erzeugen.
4. Leases MÜSSEN einen expliziten Scope besitzen.
5. Leases SOLLEN den minimal erforderlichen Power State ausdrücken.
6. Leases MÜSSEN freigegeben, beendet oder zeitlich begrenzt werden können.
7. Mehrere aktive Leases MÜSSEN aggregiert werden können.
8. Ein Lease DARF unabhängige Komponenten nicht unnötig aktiv halten.
9. Verwaiste Leases MÜSSEN automatisch entfernt werden können.
10. Wake Leases DÜRFEN Sicherheits-, Capability- oder Ressourcenlimits nicht umgehen.
11. Nach Ende eines Leases MUSS die Power Policy den Zustand neu bewerten können.
12. LeaseID, Owner, Scope, Grund, Ablaufzeit und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-WAKE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-POWER-LOWPOWERIDLE-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS verhindert mit Wake Leases, dass benötigte Ressourcen während laufender Arbeit zu früh schlafen gelegt werden, ohne dafür das gesamte System dauerhaft wach zu halten. Anforderungen bleiben auf den notwendigen Scope und Zeitraum begrenzt und verschwinden automatisch, sobald ihre Aufgabe endet.