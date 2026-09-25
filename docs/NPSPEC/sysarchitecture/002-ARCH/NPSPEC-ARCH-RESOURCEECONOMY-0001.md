# NPSPEC-ARCH-RESOURCEECONOMY-0001 – Nova Resource Economy

## Status

Angenommen

## Kategorie

Architecture / Resources / Scheduling / Execution

## Zweck

NovaOS verwaltet Ressourcen systemweit statt isoliert pro Subsystem.

Ziel ist eine gemeinsame Ressourcenökonomie für:

```text
CPU
Memory
GPU / Accelerators
Storage I/O
Network
Energy
Thermal Budget
```

## Grundprinzip

```text
Resource Demand
      +
Execution Contract
      +
System State
      ↓
Resource Economy
      ↓
Allocation / Scheduling
```

Subsysteme dürfen Ressourcen anfordern, aber nicht unabhängig voneinander unbegrenzt beanspruchen.

## Ressourcenmodell

Ressourcen werden über gemeinsame Eigenschaften beschrieben:

```text
ResourceID
Type
Capacity
Available
Reserved
Owner
Priority
Constraints
```

Physische und virtuelle Ressourcen können über dasselbe Grundmodell verwaltet werden.

## Budgets

Ausführungen können Ressourcenbudgets erhalten.

```text
CPU Budget
Memory Budget
GPU Budget
I/O Budget
Network Budget
Energy Budget
```

Budgets können aus `Nova.ExecutionContract` und System-Policies entstehen.

## Prioritäten

Bei Ressourcenkonflikten gilt grundsätzlich:

```text
Safety
↓
Security
↓
Realtime / Hard Constraints
↓
Explicit User Activity
↓
System Services
↓
Background Work
↓
Adaptive Optimization
```

Priorität allein erzeugt keine Berechtigung auf eine Ressource.

## Reservierung

Für garantierte Anforderungen können Ressourcen reserviert werden.

```text
Request
  ↓
Validate
  ↓
Reserve
  ↓
Execute
  ↓
Release
```

Eine Garantie darf nur erteilt werden, wenn die benötigten Ressourcen tatsächlich verfügbar oder reserviert sind.

## Ressourcenknappheit

Bei Ressourcenknappheit kann NovaOS:

```text
Priorisieren
Drosseln
Verschieben
Suspendieren
Qualität reduzieren
Alternative Provider wählen
```

Harte Anforderungen dürfen dabei nicht stillschweigend verletzt werden.

## Adaptive Optimierung

Adaptive Mechanismen dürfen historische Nutzung und aktuelle Last zur Optimierung verwenden.

Sie dürfen jedoch keine:

```text
Hard Constraints
Security Policies
Execution Contracts
Explicit User Decisions
```

überschreiben.

## Observability

NovaOS soll nachvollziehbar machen können:

```text
Wer nutzt eine Ressource?
Wie viel wird verwendet?
Warum wurde eine Anfrage abgelehnt?
Warum wurde eine Ausführung gedrosselt?
```

## Normative Anforderungen

1. NovaOS MUSS Ressourcen systemweit koordinieren können.
2. Ressourcen MÜSSEN eindeutig identifizierbar und typisiert sein.
3. Ressourcenbudgets MÜSSEN durch Execution Contracts ausdrückbar sein.
4. Harte Ressourcenanforderungen MÜSSEN vor Soft Preferences behandelt werden.
5. Ressourcenreservierungen MÜSSEN vor ihrer Zusage validiert werden.
6. Subsysteme DÜRFEN globale Ressourcenlimits nicht eigenständig umgehen.
7. Ressourcenknappheit MUSS kontrolliert behandelt werden.
8. Adaptive Optimierung DARF harte Constraints nicht überschreiben.
9. Ressourcenentscheidungen SOLLEN beobachtbar und erklärbar sein.
10. Freigegebene oder abgebrochene Ausführungen MÜSSEN reservierte Ressourcen kontrolliert zurückgeben.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0009_Hard_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatic_Execution_Planning`

## Ergebnis

NovaOS erhält eine gemeinsame Ressourcenökonomie:

```text
System Demand
     ↓
Contracts + Policies
     ↓
Resource Economy
     ↓
Allocation
     ↓
CPU / Memory / GPU / I/O / Network / Energy
```

Damit werden Ressourcen nicht isoliert von einzelnen Komponenten, sondern koordiniert im Kontext des gesamten Systems verwaltet.