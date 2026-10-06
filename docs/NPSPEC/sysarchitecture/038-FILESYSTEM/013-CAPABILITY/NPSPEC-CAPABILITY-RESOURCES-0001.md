# NPSPEC-CAPABILITY-RESOURCES-0001 – Nova Capability Resources

## Status

Angenommen

## Kategorie

Capability / Resources

## Zweck

NovaOS definiert die Ressourcenverwaltung für Capability-Ausführungen.

Capabilities dürfen Ressourcen benötigen, reservieren und verwenden, erhalten daraus jedoch weder zusätzliche Authority noch unbegrenzten Zugriff auf Systemressourcen.

## Grundprinzipien

```text
Resource Access ≠ Authority
Resource Availability ≠ Permission
Resource Requirement ≠ Reservation
Reservation ≠ Ownership
Capability ≠ Unlimited Resources
Provider ≠ Resource Owner
```

## Ressourcenmodell

Eine Capability-Implementierung kann ihren Ressourcenbedarf deklarieren:

```text
CapabilityResources
├── CPU
├── Memory
├── Storage
├── I/O
├── Network
├── GPU
├── Accelerator
├── Device
├── Handles
├── IPC
└── Temporary Resources
```

Der tatsächliche Bedarf einer Ausführung wird durch Implementation, Execution Contract und aktuellen Systemzustand bestimmt.

## Ressourcenanforderungen

Eine Implementierung darf Anforderungen beschreiben als:

```text
Minimum
Maximum
Preferred
HardLimit
SoftLimit
Reservation
```

Beispiel:

```text
Memory
├── Minimum: 64 MiB
├── Preferred: 256 MiB
└── Maximum: 512 MiB
```

## Ressourcenbudget

Der Execution Contract kann ein konkretes Budget festlegen:

```text
ExecutionContract
      ↓
ResourceBudget
├── CPU Time
├── Memory
├── I/O
├── Network
├── GPU Time
└── Deadline
```

Die Capability muss innerhalb dieses Budgets arbeiten oder kontrolliert degradieren beziehungsweise fehlschlagen.

## Reservierung

Vor der Ausführung dürfen benötigte Ressourcen reserviert werden:

```text
Requirements
      ↓
Policy
      ↓
Resource Accounting
      ↓
Reservation
      ↓
Execution
```

Eine Reservierung garantiert ausschließlich die zugesagten Ressourcen.

## Geräte und Beschleuniger

Hardware-Ressourcen wie:

```text
GPU
NPU
DSP
Camera
Storage Device
Network Interface
```

erfordern zusätzlich die notwendige Capability-Authority.

Die bloße Verfügbarkeit eines Geräts gewährt keinen Zugriff.

## Ressourcenökonomie

NovaOS darf zwischen mehreren kompatiblen Implementierungen anhand ihrer Ressourcenprofile auswählen:

```text
Implementation A → CPU
Implementation B → GPU
Implementation C → Low Energy
```

Dabei gelten Hard Requirements vor Optimierungszielen.

## Ressourcenknappheit

Bei Ressourcenknappheit darf NovaOS:

```text
Reclaim Cache
Reduce Budget
Throttle
Degrade
Suspend
Select Alternative Implementation
Cancel
Deny Execution
```

Sicherheits-, Integritäts- und Hard Requirements dürfen dabei nicht verletzt werden.

## Accounting

Ressourcenverbrauch muss mindestens einer ausführenden Einheit zugeordnet werden können:

```text
Capability Execution
├── Provider
├── Implementation
├── Principal
├── Process
└── Execution Contract
```

Dadurch können Budgets, Limits und Diagnose nachvollzogen werden.

## Freigabe

Nach Abschluss, Abbruch oder Fehler müssen ausführungsgebundene Ressourcen freigegeben werden.

```text
Complete / Cancel / Fail
          ↓
Release Resources
          ↓
Update Accounting
```

Persistente Ressourcen benötigen einen ausdrücklich definierten Lifecycle.

## Authority

Ressourcenbudgets und Ressourcenreservierungen erzeugen keine Authority.

```text
Reserved GPU Time
       ≠
GPU Access Authority
```

Für geschützte Ressourcen müssen Ressourcenfreigabe und Capability-Permission gemeinsam erfüllt sein.

## Normative Anforderungen

1. Capability-Implementierungen MÜSSEN ihren Ressourcenbedarf deklarieren können.
2. Execution Contracts MÜSSEN Ressourcenbudgets definieren können.
3. Minimum-, Maximum- und bevorzugte Ressourcenwerte MÜSSEN unterscheidbar sein.
4. Ressourcenanforderung und tatsächliche Reservierung MÜSSEN getrennt bleiben.
5. Ressourcenreservierung DARF keine Capability-Authority erzeugen.
6. Geschützte Geräte und Ressourcen MÜSSEN weiterhin Capability- und Policy-Prüfungen unterliegen.
7. Ressourcenverbrauch MUSS einem Ausführungskontext zuordenbar sein.
8. Ressourcenbudgets MÜSSEN durchsetzbar sein.
9. Ressourcenknappheit MUSS kontrolliert behandelt werden.
10. Alternative Implementierungen DÜRFEN anhand ihrer Ressourcenprofile ausgewählt werden.
11. Hard Requirements DÜRFEN durch Ressourcenoptimierung nicht verletzt werden.
12. Temporäre Ressourcen MÜSSEN nach Ende ihrer Lifetime freigegeben werden.
13. Abbruch und Fehler MÜSSEN Ressourcen kontrolliert freigeben.
14. Ressourcenbedarf, Budget, Reservierung und tatsächlicher Verbrauch MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-FSCAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-ACTIVATION-0001`
- `NPSPEC-SYSTEM-RESOURCES-0001`
- `NPSPEC-TEMP-QUOTA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Ressourcenmodell für Capability-Ausführungen. Ressourcenbedarf, Budgets, Reservierungen, tatsächlicher Verbrauch und Authority bleiben getrennte Konzepte, sodass Capabilities effizient ausgeführt und gleichzeitig systemweit kontrolliert, begrenzt und nachvollzogen werden können.