# NPSPEC-RESOURCE-RESERVATION-0001 – Nova Resource Reservation

## Status

Angenommen

## Kategorie

Resource / Reservation / Resource Management

## Zweck

NovaOS definiert ein systemweites Modell zur verbindlichen oder kontrolliert bestmöglichen Reservierung von Ressourcen vor und während einer Ausführung.

```text
Resource Requirement
        ↓
Reservation Request
        ↓
Feasibility + Policy
        ↓
Reservation
        ↓
Execution
        ↓
Release
```

Resource Reservation ermöglicht vorhersehbare Ausführung für Realtime-, Deadline-, QoS- und andere ressourcenkritische Workloads.

## Grundprinzipien

```text
Reservation ≠ Allocation
Reservation ≠ Consumption
Reservation ≠ Ownership
Reservation ≠ Capability
Reservation ≠ Guaranteed Completion
Priority ≠ Reservation
Budget ≠ Reservation
Unused Reservation ≠ Permanent Entitlement
```

## Reservation

Eine Reservierung wird beschrieben durch:

```text
ResourceReservation
├── ReservationID
├── ResourceTypeID
├── Amount
├── Owner / Accounting Domain
├── Start
├── Duration
└── State
```

Optional:

```text
ResourceID
ExecutionContractID
TransactionID
Minimum Guarantee
Maximum Limit
Deadline
Priority
Renewal Policy
Expiration
Fallback Policy
```

## Reservierbare Ressourcen

Mindestens folgende Ressourcen können Reservierungen unterstützen:

```text
CPU Time
Memory
I/O Bandwidth
Network Bandwidth
GPU
NPU
Energy
Device Capacity
```

Nicht jede Ressource muss dieselben Reservierungsmechanismen besitzen.

## Reservation States

Eine Reservierung besitzt einen expliziten Zustand.

```text
Requested
   ↓
Pending
   ↓
Reserved
   ↓
Active
   ↓
Released
```

Weitere Zustände:

```text
Rejected
Expired
Revoked
Failed
```

## Admission Control

Vor einer verbindlichen Reservierung muss geprüft werden, ob die angeforderte Kapazität verfügbar ist.

```text
Request
  ↓
Available Capacity
  +
Existing Reservations
  +
System Constraints
  ↓
Admission Control
  ↓
Accept / Reject
```

NovaOS darf keine Garantie aussprechen, deren Ressourcenbasis nicht ausreichend abgesichert ist.

## Hard und Soft Reservation

NovaOS unterscheidet:

```text
Hard Reservation
Soft Reservation
```

### Hard Reservation

Die zugesagte Ressource wird für den reservierenden Workload geschützt.

### Soft Reservation

NovaOS versucht die Ressource bereitzuhalten, darf sie jedoch unter definierten Bedingungen anderweitig verwenden.

Der Reservation Type muss explizit erkennbar sein.

## Budget und Reservation

Budgets und Reservierungen erfüllen unterschiedliche Aufgaben.

```text
Budget:
Wie viel darf verbraucht werden?

Reservation:
Wie viel wird bereitgehalten?
```

Beispiel:

```text
CPU Budget:       50 ms
CPU Reservation:  20 ms
```

Eine Reservierung darf das zulässige Budget nicht automatisch erhöhen.

## Hierarchische Reservation

Reservierungen können aus übergeordneten Resource Domains abgeleitet werden.

```text
System Capacity
      ↓
Application Reservation
      ↓
Process Reservation
      ↓
Task Reservation
```

Untergeordnete Reservierungen dürfen die zugesicherte Kapazität ihrer Parent Domain nicht überschreiten.

## ExecutionContract

Reservierungen können aus einem ExecutionContract entstehen.

```text
ExecutionContract
      ↓
Resource Requirements
      ↓
Reservation Planning
      ↓
Admission Control
      ↓
Execution
```

Beispiele:

```text
CPU Reservation
Memory Reservation
I/O Reservation
Network Reservation
GPU Reservation
NPU Reservation
```

## Deadline Integration

Deadline-kritische Ausführung kann Ressourcen vorab reservieren.

```text
Deadline
   +
Execution Budget
   ↓
Required Reservation
   ↓
Admission Control
```

Ist die notwendige Kapazität nicht verfügbar, kann NovaOS die Ausführung frühzeitig ablehnen oder neu planen.

## Aktivierung

Eine Reservierung muss nicht sofort Ressourcen konsumieren.

```text
Reserved
   ↓
Execution Starts
   ↓
Active
   ↓
Consumption
```

Dadurch bleiben Reservation und Consumption getrennt messbar.

## Borrowing

Nicht genutzte Soft Reservations können temporär anderen Workloads zur Verfügung gestellt werden.

```text
Reserved Capacity
      ↓
Currently Unused
      ↓
Temporary Borrowing
```

Dabei muss die Ressource rechtzeitig für den ursprünglichen Besitzer zurückgewonnen werden können.

Hard Reservations dürfen dadurch nicht verletzt werden.

## Overcommit

NovaOS darf Soft Reservations kontrolliert überbuchen.

```text
Physical Capacity
       <
Soft Reservations
```

Hard Reservations dürfen nur überbucht werden, wenn die zugrunde liegende Ressource oder Garantie dies ausdrücklich unterstützt.

## Revocation

Reservierungen können unter definierten Bedingungen widerrufen werden.

```text
Reservation
   ↓
Revocation
   ↓
Notify
   ↓
Degrade / Replan / Cancel
```

Hard Reservations dürfen nicht willkürlich widerrufen werden.

Safety- oder Hardware-Schutzmechanismen besitzen jedoch Vorrang.

## Expiration

Reservierungen müssen zeitlich begrenzbar sein.

```text
Reserve
  ↓
Expiration
  ↓
Automatic Release
```

Verwaiste Reservierungen dürfen Ressourcen nicht dauerhaft blockieren.

## Resource Accounting

Accounting unterscheidet:

```text
Reserved
Allocated
Consumed
Borrowed
Released
```

Dadurch kann NovaOS erkennen, ob reservierte Ressourcen tatsächlich genutzt werden.

## Pressure

Bei Ressourcenknappheit gilt grundsätzlich:

```text
Unreserved Capacity
        ↓
Soft Reservations
        ↓
Hard Reservations
```

Safety-, Security- und Hardwaregrenzen bleiben davon unabhängig vorrangig.

## Capability Security

Das Erstellen oder Verändern einer Reservierung benötigt entsprechende Autorität.

```text
Resource Capability
        +
Reservation Authority
        ↓
Reservation Request
```

Eine Reservation selbst gewährt keinen zusätzlichen Zugriff auf die reservierte Ressource.

```text
Reservation ≠ Authority
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ReservationID
Resource Type
Reserved Amount
Reservation Type
Owner
Accounting Domain
State
Start
Expiration
Usage
Borrowing State
ExecutionContract
```

## Normative Anforderungen

1. NovaOS MUSS Resource Reservations explizit modellieren können.
2. Reservation, Allocation, Consumption und Budget MÜSSEN getrennte Konzepte bleiben.
3. NovaOS MUSS Hard und Soft Reservations unterscheiden können.
4. Verbindliche Reservierungen MÜSSEN Admission Control durchlaufen.
5. Reservierungen MÜSSEN hierarchisch begrenzbar sein.
6. ExecutionContracts MÜSSEN Resource Reservations anfordern können.
7. Soft Reservations DÜRFEN kontrolliertes Borrowing und Overcommit unterstützen.
8. Hard Reservations DÜRFEN durch normale Ressourcenoptimierung NICHT verletzt werden.
9. Reservierungen MÜSSEN freigegeben, widerrufen oder automatisch ablaufen können.
10. Reservation States und Ressourcenverbrauch MÜSSEN autorisiert introspektierbar und abrechenbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-NETWORK-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0043`

## Ergebnis

```text
Resource Requirement
        ↓
Reservation Planning
        ↓
Admission Control
        ↓
Hard / Soft Reservation
        ↓
Execution + Accounting
        ↓
Release
```

NovaOS erhält damit ein systemweites Reservierungsmodell, mit dem Ressourcen vor einer Ausführung kontrolliert zugesichert werden können, ohne Reservation, Budget, tatsächlichen Verbrauch und Zugriffsautorität miteinander zu vermischen.