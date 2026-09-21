# ADR-EXECUTION-0004 – Resource Budgets im ExecutionContract

## Status

Angenommen

## Kategorie

Execution Architecture / Resource Management

## Kontext

Eine technisch mögliche Ausführung ist nicht automatisch eine zulässige Ausführung.

Operationen können unterschiedliche Mengen an:

```text
CPU Time
Memory
Storage
Network Bandwidth
I/O
Accelerator Time
Energy
```

benötigen.

NovaOS benötigt deshalb eine Möglichkeit, Ressourcenverbrauch bereits bei der Ausführungsplanung zu begrenzen.

## Entscheidung

`Nova.ExecutionContract` unterstützt explizite **Resource Budgets**.

```text
Execution Request
       ↓
Resource Budget
       ↓
Execution Planning
       ↓
Algorithm + Provider
       ↓
Resource Reservation
       ↓
Execution
       ↓
Resource Accounting
```

Ein Resource Budget beschreibt, welche Ressourcen eine Ausführung maximal oder bevorzugt verwenden darf.

## Budgettypen

Ein Execution Contract kann Budgets für unterschiedliche Ressourcen enthalten.

Beispiele:

```text
CPU Time
Memory
Storage
Network
I/O
GPU / NPU / Accelerator Time
Energy
```

Weitere Ressourcentypen können später ergänzt werden.

## Hard und Soft Budgets

Resource Budgets können verbindlich oder bevorzugt sein.

```text
Hard Budget
    → darf nicht überschritten werden

Soft Budget
    → soll möglichst eingehalten werden
```

Beispiel:

```text
Maximum Memory = 512 MiB
Preferred Memory = 256 MiB
```

Hard Budgets sind Hard Constraints des Execution Contracts.

## Algorithmusauswahl

Der erwartete Ressourcenverbrauch beeinflusst die Algorithmusauswahl.

```text
Candidate Algorithms
       ↓
Resource Requirements
       ↓
Budget Check
       ↓
Valid Algorithms
```

Ein schnellerer Algorithmus kann ausgeschlossen werden, wenn sein Ressourcenbedarf das Budget überschreitet.

## Providerwahl

Auch Compute Provider werden gegen das Resource Budget geprüft.

```text
Algorithm
    +
Candidate Providers
    +
Resource Budget
       ↓
Valid Provider Combinations
```

Algorithmus und Provider müssen deshalb gemeinsam betrachtet werden.

## Ressourcenreservierung

Wo eine garantierte Ressource erforderlich ist, muss sie vor der Ausführung reserviert werden können.

```text
Plan
 ↓
Reserve
 ↓
Execute
 ↓
Release
```

Eine geplante, aber nicht verfügbare Ressource darf nicht als garantiert behandelt werden.

Reservierte Ressourcen gelten systemweit nicht als frei.

## Resource Accounting

NovaOS soll den tatsächlichen Ressourcenverbrauch erfassen können.

```text
Budget
  ↓
Execution
  ↓
Measured Usage
  ↓
Accounting
  ↓
Contract Verification
```

Diese Messwerte können zugleich Cost- und Profile-Guided-Modelle verbessern.

## Budgetüberschreitung

Eine drohende oder tatsächliche Überschreitung muss erkennbar sein.

Mögliche Reaktionen sind abhängig von Operationssemantik und Contract:

```text
Throttle
Replan
Degrade
Abort
Report Violation
```

NovaOS darf ein Hard Budget nicht stillschweigend erhöhen.

## Verschachtelte Ausführungen

Startet eine Operation weitere Ausführungen, müssen deren Ressourcen dem übergeordneten Budget zugeordnet werden können.

```text
Parent Budget
    ├── Child Execution A
    ├── Child Execution B
    └── Child Execution C
```

Unteroperationen dürfen das Budget nicht durch versteckten Ressourcenverbrauch umgehen.

## Verteilte Ausführung

Bei Remote Execution müssen auch lokale und entfernte Ressourcen berücksichtigt werden.

```text
Local Resources
      +
Network Resources
      +
Remote Resources
      ↓
Execution Budget
```

Remote Compute darf nicht als ressourcenfrei betrachtet werden.

## Resource Economy

Resource Budgets sind Bestandteil der systemweiten NovaOS Resource Economy.

Dadurch können mehrere konkurrierende Workloads koordiniert werden.

```text
Execution Contracts
       ↓
Resource Economy
       ↓
Reservation / Scheduling
       ↓
Execution
```

## Unsicherheit

Ressourcenbedarf kann:

```text
Known
Estimated
Unknown
```

sein.

Für garantierte Hard Budgets gilt:

```text
Unknown ≠ Satisfied
```

Kann NovaOS nicht ausreichend bestimmen, ob ein Hard Budget eingehalten wird, darf es keine entsprechende Garantie annehmen.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Resource Budgets ausdrücken können.
2. Resource Budgets MÜSSEN Hard und Soft Constraints unterstützen.
3. CPU-, Memory-, Storage-, Network-, I/O- und Accelerator-Ressourcen MÜSSEN modellierbar sein.
4. Energy Budgets MÜSSEN integrierbar sein.
5. Algorithmusauswahl MUSS Resource Budgets berücksichtigen.
6. Providerwahl MUSS Resource Budgets berücksichtigen.
7. Garantierte Ressourcen MÜSSEN reservierbar sein.
8. Reservierte Ressourcen DÜRFEN nicht gleichzeitig als frei behandelt werden.
9. Tatsächlicher Ressourcenverbrauch SOLL messbar und dem Request zuordenbar sein.
10. Hard Budgets DÜRFEN nicht automatisch erhöht oder abgeschwächt werden.
11. Unteroperationen MÜSSEN dem relevanten übergeordneten Budget zugeordnet werden können.
12. Remote Execution MUSS Netzwerk- und entfernte Ressourcen berücksichtigen.
13. `Unknown` DARF die Einhaltung eines garantierten Hard Budgets nicht implizieren.
14. Budgetverletzungen MÜSSEN als Contract Violation erkennbar sein.

## Konsequenzen

### Positive Konsequenzen

- Ressourcenverbrauch wird Bestandteil der Ausführungsplanung,
- einzelne Workloads können das Gesamtsystem weniger leicht verdrängen,
- Algorithmus- und Providerwahl können Ressourcen realistisch berücksichtigen,
- garantierte Workloads können Ressourcen reservieren,
- Resource Accounting verbessert adaptive Modelle.

### Negative Konsequenzen

- Ressourcenbedarf muss geschätzt oder gemessen werden,
- Reservierungen können die Gesamtauslastung reduzieren,
- verschachtelte und verteilte Budgets erhöhen den Planungsaufwand.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-RESOURCE-BUDGET-0001`
- `NPSPEC-EXECUTION-RESOURCE-RESERVATION-0001`
- `NPSPEC-EXECUTION-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-EXECUTION-BUDGET-HIERARCHY-0001`

## Ergebnis

Resource Budgets werden zu einem festen Bestandteil des Execution Contracts:

```text
Execution Request
       ↓
Resource Budget
       ↓
Constraint Solving
       ↓
Algorithm + Provider
       ↓
Reservation
       ↓
Execution
       ↓
Accounting
       ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
NovaOS entscheidet nicht nur,
ob eine Operation ausgeführt werden kann.

Der Execution Contract bestimmt auch,
welche Ressourcen sie dafür
verwenden darf.
```