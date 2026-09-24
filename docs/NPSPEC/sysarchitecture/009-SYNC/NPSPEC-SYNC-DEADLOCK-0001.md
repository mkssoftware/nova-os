# NPSPEC-SYNC-DEADLOCK-0001 – Nova Deadlock Management

## Status

Entwurf

## Kategorie

Synchronization / Deadlock / Concurrency / Diagnostics

## Zweck

NovaOS definiert Mechanismen zur Vermeidung, Erkennung und Diagnose von Deadlocks zwischen Tasks und synchronisierten Ressourcen.

```text
Task A owns Resource 1
   ↓ waits for
Resource 2

Task B owns Resource 2
   ↓ waits for
Resource 1
```

Ein Deadlock darf insbesondere in kritischen Kernel-, Realtime- und Systempfaden nicht unkontrolliert zu dauerhaftem Stillstand führen.

## Grundprinzip

Ein Deadlock entsteht durch zyklische Abhängigkeiten zwischen wartenden Ausführungskontexten und Ressourcen.

```text
Task A → waits → Lock B
  ↑                 ↓
Lock A ← owns ← Task B
```

Dabei gilt:

```text
Waiting ≠ Deadlock
Contention ≠ Deadlock
Long Wait ≠ Deadlock
```

Ein Deadlock liegt erst vor, wenn kein beteiligter Teilnehmer ohne externe Intervention Fortschritt erzielen kann.

## Deadlock-Bedingungen

Klassische Deadlocks können entstehen, wenn gleichzeitig folgende Bedingungen gelten:

```text
Mutual Exclusion
Hold and Wait
No Preemption
Circular Wait
```

NovaOS soll Deadlocks bevorzugt durch Architektur und Lock-Regeln vermeiden, statt ausschließlich nachträglich darauf zu reagieren.

## Lock Ordering

Für kritische Komponenten sollen definierte Lock-Hierarchien verwendet werden.

```text
Lock Level 1
    ↓
Lock Level 2
    ↓
Lock Level 3
```

Erlaubt:

```text
L1 → L2 → L3
```

Nicht erlaubt:

```text
L3 → L1
```

Eine konsistente Reihenfolge reduziert zyklische Lock-Abhängigkeiten.

## Wait-For Graph

NovaOS darf Synchronisationsabhängigkeiten als Wait-For Graph modellieren.

```text
Task A → Mutex B
Mutex B → Task B
Task B → Mutex A
Mutex A → Task A
```

Ein Zyklus kann auf einen Deadlock hinweisen.

Der Graph darf für Diagnose, Debugging und Runtime-Erkennung verwendet werden.

## Deadlock Prevention

Deadlocks sollen möglichst durch strukturelle Regeln verhindert werden.

Mögliche Maßnahmen:

```text
Global / Local Lock Ordering
Short Critical Sections
No Blocking while Holding Critical Locks
TryLock
Timeouts
Structured Resource Acquisition
Reduced Nested Locking
```

Komponenten dürfen strengere Regeln definieren.

## Deadlock Detection

NovaOS soll insbesondere in Debug-, Diagnose- und Verifikationsmodi Deadlocks erkennen können.

Mögliche Erkennung:

```text
Wait-For Cycle Detection
Lock Dependency Tracking
Timeout Analysis
Ownership Inspection
Scheduler Stall Detection
```

Eine erkannte zyklische Abhängigkeit muss diagnostizierbar sein.

## TryLock

Nicht blockierende Lock-Versuche können zur Deadlock-Vermeidung verwendet werden.

```text
TryLock(Resource B)
      ↓
Failed
      ↓
Release Resource A
      ↓
Retry Later
```

Retry-Strategien müssen jedoch Livelocks vermeiden.

## Timeout

Blockierende Synchronisationsoperationen dürfen optional zeitlich begrenzt werden.

```text
Acquire
   ↓
Wait
   ↓
Timeout
```

Ein Timeout ist kein Beweis für einen Deadlock.

```text
Timeout ≠ Deadlock
```

Er kann jedoch als Diagnose- oder Recovery-Signal dienen.

## Cancellation

Cancellation darf verwendet werden, um kontrollierbare Wait-Abhängigkeiten aufzulösen.

Dabei muss berücksichtigt werden, welche Ressourcen der Task bereits besitzt.

```text
Cancel Task
    ↓
Cleanup
    ↓
Release Owned Resources
```

Ressourcen dürfen nicht unkontrolliert oder ohne Wahrung ihrer Invarianten freigegeben werden.

## Livelock

NovaOS muss Deadlock und Livelock unterscheiden.

```text
Deadlock:
No Participant Progresses

Livelock:
Participants Execute
but
Useful Progress = None
```

Aggressive Retry- oder Backoff-Algorithmen dürfen keinen dauerhaften Livelock erzeugen.

## Priority Inversion

Priority Inversion ist ebenfalls kein Deadlock.

```text
Priority Inversion ≠ Deadlock
```

Sie kann jedoch lange Blockierungszeiten verursachen und muss getrennt diagnostiziert werden.

## Realtime

Hard-Realtime-Pfade dürfen nicht von unbeschränkt blockierenden Lock-Abhängigkeiten abhängen.

```text
Hard Realtime
     ↓
Bounded Synchronization
     ↓
Known Lock Dependencies
```

Lock-Reihenfolgen und maximale Blockierungszeiten müssen für kritische Pfade analysierbar sein.

## Recovery

Nicht jeder Deadlock kann sicher automatisch aufgelöst werden.

Mögliche Reaktionen:

```text
Diagnostic Report
Fail Operation
Cancel Task
Restart Component
Supervisor Recovery
System Recovery
Panic
```

Automatische Recovery darf nur erfolgen, wenn die Konsistenz des betroffenen Zustands gewährleistet werden kann.

## Diagnose

Ein Deadlock-Bericht soll mindestens enthalten können:

```text
Affected Tasks
Held Locks
Requested Locks
Wait Duration
Lock Acquisition Chain
Scheduler State
Relevant Stack Traces
```

Beispiel:

```text
Task A
 └─ owns Mutex X
    └─ waits Mutex Y
       └─ owned by Task B
          └─ waits Mutex X
```

Dadurch muss die Ursache eines Deadlocks nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS Deadlocks als von normaler Contention getrennten Zustand behandeln.
2. Kritische Komponenten SOLLEN definierte Lock-Reihenfolgen verwenden.
3. NovaOS SOLL Lock-Abhängigkeiten diagnostizieren können.
4. Debug- und Verifikationsmodi SOLLEN zyklische Wait-Abhängigkeiten erkennen können.
5. Ein Timeout DARF NICHT automatisch als Deadlock klassifiziert werden.
6. Deadlock, Livelock und Priority Inversion MÜSSEN diagnostisch unterscheidbar sein.
7. Cancellation DARF Ressourceninvarianten NICHT verletzen.
8. Automatische Deadlock-Recovery DARF nur bei sicher wiederherstellbarer Konsistenz erfolgen.
9. Hard-Realtime-Pfade DÜRFEN keine unbeschränkt analysierbaren Lock-Zyklen enthalten.
10. Retry-basierte Vermeidung SOLL geeignete Backoff- oder Fortschrittsmechanismen besitzen.
11. Erkannte Deadlocks SOLLEN Task-, Lock- und Abhängigkeitsinformationen bereitstellen.
12. Deadlock-Erkennung DARF im Produktionsbetrieb deaktivierbar oder kostenreduziert implementiert werden, sofern die grundlegenden Sicherheitsregeln erhalten bleiben.

## Abhängigkeiten

- `NPSPEC-SYNC-MUTEX-0001`
- `NPSPEC-SYNC-RWLOCK-0001`
- `NPSPEC-SYNC-SEMAPHORE-0001`
- `NPSPEC-SYNC-CONDITION-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SYNC-0010`

## Ergebnis

```text
Synchronization Dependencies
          ↓
Lock Ordering + Tracking
          ↓
Prevent / Detect
          ↓
Deadlock?
   ┌──────┴──────┐
  No            Yes
   ↓              ↓
Continue       Diagnose
                  ↓
            Safe Recovery
            if possible
```

NovaOS erhält damit ein systematisches Deadlock-Management, das Deadlocks bevorzugt durch klare Synchronisationsregeln verhindert, verbleibende zyklische Abhängigkeiten diagnostizierbar macht und automatische Recovery nur dort zulässt, wo die Systemkonsistenz erhalten werden kann.