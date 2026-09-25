# NPSPEC-SYNC-CONDITION-0001 – Nova Condition Variable

## Status

Angenommen

## Kategorie

Synchronization / Condition Variable / Waiting / Concurrency

## Zweck

NovaOS definiert Condition Variables zur effizienten Synchronisation von Tasks, die auf eine Zustandsänderung warten müssen.

```text
Task
 ↓
Check Condition
 ↓
Condition False
 ↓
Wait
 ↓
State Changed
 ↓
Wake
 ↓
Recheck Condition
```

Condition Variables ermöglichen blockierendes Warten ohne dauerhaftes Polling oder Busy Waiting.

## Grundprinzip

Eine Condition Variable repräsentiert kein gespeichertes Ereignis und keinen Ressourcenbesitz.

```text
Condition Variable ≠ Mutex
Condition Variable ≠ Semaphore
Condition Variable ≠ Event Counter
```

Sie signalisiert lediglich, dass sich ein relevanter Zustand geändert haben könnte.

Deshalb gilt:

```text
Wakeup ≠ Condition True
```

Nach jedem Wakeup muss die tatsächliche Bedingung erneut geprüft werden.

## Verbindung mit Mutex

Eine Condition Variable wird typischerweise zusammen mit einem Mutex verwendet.

```text
Lock Mutex
    ↓
Check Predicate
    ↓
False?
    ↓
Wait(Condition, Mutex)
```

`Wait` muss den Mutex atomar freigeben und den Task als Waiter registrieren.

Nach dem Wakeup wird der Mutex erneut erworben.

## Wait-Ablauf

Der grundlegende Ablauf lautet:

```text
Acquire Mutex
     ↓
while Predicate == false
     ↓
Condition.Wait(Mutex)
     ↓
Wake
     ↓
Reacquire Mutex
     ↓
Recheck Predicate
```

Das Prüfen innerhalb einer Schleife ist erforderlich.

## Atomarer Übergang

Zwischen dem Prüfen der Bedingung und dem Beginn des Wartens darf kein Signal verloren gehen.

Problematischer Ablauf:

```text
Check Predicate
      ↓
Unlock
      ↓
Signal occurs
      ↓
Begin Wait
      ↓
Lost Wakeup
```

Deshalb muss der Übergang:

```text
Register Waiter
+
Release Mutex
+
Block Task
```

synchronisiert erfolgen.

## Signal

`Signal` weckt mindestens einen geeigneten wartenden Task.

```text
Condition
   ↓
Signal
   ↓
Wake One Waiter
```

Das Signal garantiert nicht, dass der geweckte Task sofort ausgeführt wird.

Es garantiert ebenfalls nicht, dass dessen Predicate beim tatsächlichen Scheduling noch erfüllt ist.

## Broadcast

`Broadcast` macht alle aktuell relevanten Waiter wieder ausführbar.

```text
Condition
    ↓
Broadcast
    ↓
Task A
Task B
Task C
```

Alle geweckten Tasks müssen ihre Bedingung erneut prüfen.

Broadcast soll nur verwendet werden, wenn mehrere Waiter von derselben Zustandsänderung betroffen sein können.

## Spurious Wakeups

NovaOS darf Spurious Wakeups zulassen.

```text
Wait
 ↓
Wake
 ↓
Predicate still false
 ↓
Wait again
```

Anwendungen dürfen daher nicht davon ausgehen, dass jedes Wakeup einem expliziten Signal entspricht.

## Wait Queue

Eine Condition Variable kann eine interne Wait Queue besitzen.

```text
Condition
   ↓
Wait Queue
├── Task A
├── Task B
└── Task C
```

Die konkrete Auswahl eines Waiters darf Scheduling-Priorität, Deadline, QoS und Realtime-Anforderungen berücksichtigen.

Eine strikte FIFO-Reihenfolge ist nicht erforderlich.

## Timeout

Condition Waits dürfen zeitlich begrenzt werden.

```text
Wait
 ↓
Signal / Broadcast / Timeout
```

Ein Timeout muss eindeutig vom normalen Wakeup unterscheidbar sein.

Auch nach einem normalen Wakeup muss das Predicate erneut geprüft werden.

## Cancellation

Ein wartender Task darf kontrolliert abbrechbar sein.

```text
Waiting
  ↓
Cancellation
  ↓
Remove Waiter
  ↓
Cancelled
```

Cancellation darf keine beschädigten Wait-Queue- oder Mutex-Zustände hinterlassen.

## Memory Ordering

Die Sichtbarkeit gemeinsam genutzter Daten wird primär durch den zugehörigen Synchronisationsmechanismus hergestellt.

Typisches Muster:

```text
Producer:
Lock
Modify State
Signal
Unlock

Consumer:
Lock
Wait / Recheck
Read State
Unlock
```

Condition Variables allein ersetzen keine erforderliche Speicherordnung.

## Producer / Consumer

Typischer Einsatz:

```text
Producer
   ↓
Add Item
   ↓
Signal

Consumer
   ↓
Queue Empty?
   ↓ Yes
Wait
   ↓
Recheck Queue
```

Der Zustand selbst wird durch den Mutex oder einen anderen geeigneten Mechanismus geschützt.

## Realtime

Bei Realtime-Tasks müssen Wakeup- und Reacquisition-Latenzen berücksichtigt werden.

```text
Signal
  ↓
Scheduler Wakeup
  ↓
Mutex Reacquisition
  ↓
Task Execution
```

Ein Signal allein stellt keine Deadline-Garantie dar.

## Normative Anforderungen

1. NovaOS MUSS Condition Variables für blockierendes Zustands-Warten bereitstellen.
2. `Wait` MUSS das Registrieren des Waiters und Freigeben des zugehörigen Mutex ohne Lost-Wakeup-Fenster ermöglichen.
3. Nach einem Wakeup MUSS der zugehörige Mutex erneut erworben werden.
4. Ein Wakeup DARF NICHT als Garantie für ein erfülltes Predicate interpretiert werden.
5. Waiter MÜSSEN ihre Bedingung nach dem Wakeup erneut prüfen.
6. NovaOS DARF Spurious Wakeups zulassen.
7. `Signal` MUSS mindestens einen geeigneten Waiter wecken können.
8. `Broadcast` MUSS alle relevanten aktuellen Waiter wecken können.
9. Wartende Tasks SOLLEN blockiert werden und kein dauerhaftes Busy Waiting verwenden.
10. Timeout und Cancellation SOLLEN unterstützt werden.
11. Cancellation DARF Wait-Queue- und Mutex-Zustände NICHT beschädigen.
12. Waiter, Wakeups und Contention SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-MUTEX-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-SCHEDULER-0001`
- `ADR-SYNC-0005`

## Ergebnis

```text
Shared State
     ↓
Mutex + Predicate
     ↓
Condition Wait
     ↓
Task Sleeps
     ↓
State Change + Signal
     ↓
Wake + Reacquire
     ↓
Recheck Predicate
```

NovaOS erhält damit einen effizienten Synchronisationsmechanismus für zustandsabhängiges Warten, der Lost Wakeups verhindert und sauber mit Mutexes, Scheduler, Cancellation und Deadlines zusammenarbeitet.