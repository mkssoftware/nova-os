# NPSPEC-MEMORY-OOM-0001 – Nova Out-of-Memory Management

## Status

Entwurf

## Kategorie

Memory / Out-of-Memory / Resource Exhaustion / Recovery

## Zweck

NovaOS definiert Out-of-Memory (OOM) als kontrollierten Systemzustand, in dem eine Speicheranforderung trotz zulässiger Reclamation- und Fallback-Maßnahmen nicht erfüllt werden kann.

```text
Allocation Request
       ↓
Memory unavailable
       ↓
Reclamation
       ↓
Fallback
       ↓
Still unavailable
       ↓
OOM
```

Ziel ist nicht, Speichererschöpfung vollständig zu verhindern, sondern sie vorhersehbar, diagnostizierbar und möglichst lokal begrenzt zu behandeln.

## Grundprinzip

OOM ist das Ergebnis einer konkreten nicht erfüllbaren Speicheranforderung.

```text
Memory Pressure ≠ OOM
Allocation Failure ≠ Global OOM
```

Ein lokaler Pool, NUMA-Node oder Resource Budget kann erschöpft sein, obwohl global noch Speicher verfügbar ist.

NovaOS muss deshalb den tatsächlichen OOM-Bereich bestimmen.

## OOM-Scope

OOM kann unterschiedliche Gültigkeitsbereiche besitzen:

```text
Allocation
Memory Pool
Resource Budget
Process
Service
NUMA Node
System
```

Die Recovery soll im kleinstmöglichen betroffenen Scope erfolgen.

```text
Local Failure
     ↓
Local Recovery
```

Ein lokales OOM darf nicht automatisch zu einer systemweiten Recovery führen.

## OOM-Erkennung

Vor einer OOM-Klassifikation müssen die für den jeweiligen Kontext zulässigen Maßnahmen berücksichtigt werden.

```text
Allocation Failed
      ↓
Reclaim allowed?
      ↓
Fallback allowed?
      ↓
Alternative Node allowed?
      ↓
Retry allowed?
      ↓
OOM
```

Nicht jede Operation darf alle Maßnahmen verwenden.

Beispielsweise kann ein Hard-Realtime-Pfad Reclamation oder Remote-Fallback verbieten.

## OOM-Kontext

Ein OOM-Ereignis soll mindestens beschreiben:

```text
RequestedSize
Requester
MemoryScope
NUMANode
ResourceBudget
AllocationConstraints
PressureLevel
ReclaimResult
FallbackResult
```

Damit kann zwischen tatsächlicher globaler Speichererschöpfung und einer unerfüllbaren Spezialallokation unterschieden werden.

## Recovery-Hierarchie

NovaOS soll OOM kontrolliert eskalieren.

```text
Allocation Failure
      ↓
Local Reclamation
      ↓
Subsystem Reclamation
      ↓
Allowed Fallback
      ↓
Graceful Degradation
      ↓
Fail Operation
      ↓
Supervisor Recovery
      ↓
System Recovery
```

Nicht jede Stufe ist in jedem Kontext zulässig.

## Keine willkürliche Prozessbeendigung

NovaOS darf nicht grundsätzlich einen beliebigen Prozess anhand eines einfachen OOM-Scores beenden.

```text
Low Memory
    ↓
Kill arbitrary process
```

ist keine allgemeine NovaOS-Strategie.

Recovery muss Resource Ownership, Budgets, Kritikalität, Abhängigkeiten und Wiederherstellbarkeit berücksichtigen.

## Resource Budgets

Resource Budgets dienen als wichtige OOM-Grenze.

```text
Process / Service
      ↓
Memory Budget
      ↓
Budget Exhausted
```

Wird nur das Budget eines Verbrauchers überschritten, soll zunächst dieser Scope behandelt werden.

Andere Komponenten dürfen dadurch nicht automatisch ihre garantierten Ressourcen verlieren.

## Graceful Degradation

Vor schwerwiegender Recovery sollen nichtkritische Funktionen kontrolliert reduziert werden können.

Beispiele:

```text
Reduce Caches
Disable Prefetch
Reduce History
Drop Reconstructable Data
Suspend Background Work
Reduce Optional Buffers
```

Dabei gilt:

```text
Graceful Degradation
        ≠
Violation of Hard Constraints
```

## Kritische Reserven

NovaOS darf geschützte Speicherreserven für kritische Systemfunktionen besitzen.

```text
Normal Memory
Critical Reserve
Emergency Reserve
```

Diese dürfen beispielsweise für folgende Funktionen vorgesehen sein:

```text
OOM Handling
Recovery
Crash Reporting
Critical Logging
Supervisor Actions
NovaDOS Transition
```

Normale Anwendungen dürfen diese Reserven nicht verbrauchen.

## Realtime

Garantierter Realtime-Speicher darf durch allgemeine OOM-Recovery nicht entzogen werden.

```text
Reserved Realtime Memory
          ↓
Protected
```

Kann eine neue Realtime-Anforderung nicht garantiert werden, muss sie vor ihrer Annahme abgelehnt werden.

Ein bereits akzeptierter harter Vertrag darf nicht erst während der Ausführung durch normale OOM-Entscheidungen verletzt werden.

## Deadlock-Vermeidung

OOM-Recovery darf nicht selbst von Speicherallokationen abhängen, die unter OOM möglicherweise scheitern.

Kritische OOM-Pfade müssen deshalb mit:

```text
Preallocated Metadata
Emergency Memory
Bounded Data Structures
Allocation-Free Logging
```

arbeiten können.

```text
OOM Handler
    ↓
Needs Memory
    ↓
OOM Handler
```

darf keinen rekursiven Fehlerzyklus erzeugen.

## Retry

Fehlgeschlagene Allokationen dürfen nur kontrolliert wiederholt werden.

```text
Fail
 ↓
Reclaim
 ↓
Retry
```

Unbegrenzte Retry-Schleifen sind nicht zulässig.

Retry-Budgets und Backoff dürfen eingesetzt werden.

## Prozess- und Service-Recovery

Ist ein Verbraucher für seinen OOM-Zustand verantwortlich, darf dessen Supervisor Recovery-Maßnahmen durchführen.

Beispiele:

```text
Fail Request
Restart Worker
Restart Service
Reduce Resource Limit
Terminate Recoverable Component
```

Die Auswahl muss anhand definierter Policies erfolgen.

## Systemkritisches OOM

Kann der Kernel selbst keine für seine korrekte Funktion erforderlichen Ressourcen mehr bereitstellen:

```text
Kernel Critical Allocation
        ↓
Recovery impossible
        ↓
Fail-Safe State
```

NovaOS muss einen kontrollierten Fail-Safe-Pfad besitzen.

Dieser kann abhängig vom Zustand umfassen:

```text
Emergency Logging
Crash Dump
Recovery Boot Request
Controlled Restart
Panic
```

Unkontrollierte Weiterführung mit inkonsistentem Zustand ist nicht zulässig.

## Introspection

Ein OOM-Bericht soll mindestens enthalten können:

```text
OOM Scope
Requester
Requested Memory
Available Memory
Reclaimable Memory
Resource Budget
NUMA State
Allocation Constraints
Reclamation Attempts
Fallback Attempts
Recovery Action
```

OOM-Entscheidungen müssen nachträglich nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS OOM als expliziten Ressourcenfehler behandeln.
2. Memory Pressure und OOM MÜSSEN getrennte Zustände bleiben.
3. NovaOS MUSS den kleinstmöglichen betroffenen OOM-Scope bestimmen können.
4. Zulässige Reclamation- und Fallback-Maßnahmen SOLLEN vor einer endgültigen OOM-Eskalation geprüft werden.
5. Harte Allocation Constraints DÜRFEN zur OOM-Vermeidung NICHT verletzt werden.
6. Ein lokaler OOM DARF NICHT automatisch als globaler OOM behandelt werden.
7. NovaOS DARF NICHT grundsätzlich beliebige Prozesse zur Speichergewinnung beenden.
8. Resource Budgets und garantierte Reservierungen MÜSSEN bei OOM-Entscheidungen berücksichtigt werden.
9. Kritische und Emergency-Reserven MÜSSEN vor normaler Speichernutzung geschützt bleiben.
10. OOM-Recovery MUSS ohne unbeschränkte zusätzliche Speicherallokationen ausführbar sein.
11. Retry- und Recovery-Schleifen MÜSSEN begrenzt sein.
12. OOM-Ursache, Scope und Recovery-Aktion SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-POOL-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-MEMORY-0009`

## Ergebnis

```text
Allocation Failure
       ↓
Reclaim + Allowed Fallback
       ↓
Still Failed
       ↓
Determine OOM Scope
       ↓
Local Recovery
       ↓
Graceful Degradation
       ↓
Fail Operation / Supervisor
       ↓
System Fail-Safe
only if unavoidable
```

NovaOS erhält damit ein kontrolliertes OOM-Modell, das Speichererschöpfung möglichst lokal behandelt, harte Ressourcenverträge schützt, willkürliche Prozessbeendigung vermeidet und selbst unter kritischem Speichermangel einen definierten Recovery- oder Fail-Safe-Pfad sicherstellt.