# NPSPEC-SYNC-SEMAPHORE-0001 – Nova Semaphore

## Status

Angenommen

## Kategorie

Synchronization / Semaphore / Concurrency / Resource Control

## Zweck

NovaOS definiert Semaphore zur Synchronisation von Tasks und zur kontrollierten Verwaltung einer begrenzten Anzahl gleichzeitig verfügbarer Ressourcen.

```text
Available Resources = N
        ↓
Semaphore Counter
        ↓
Tasks acquire / release
```

Im Gegensatz zum Mutex besitzt eine Semaphore keinen exklusiven Besitzer.

## Grundprinzip

Eine Semaphore verwaltet einen Zähler:

```text
Count > 0
   ↓
Acquire
   ↓
Count - 1
```

Ist keine Einheit verfügbar:

```text
Count = 0
   ↓
Acquire
   ↓
Wait
```

Bei Freigabe:

```text
Release
   ↓
Count + 1
   ↓
Wake Waiter
```

Dabei gilt:

```text
Semaphore ≠ Mutex
```

Eine Semaphore modelliert verfügbare Einheiten oder Ereignisse, nicht Besitz.

## Semaphore-Typen

NovaOS unterstützt mindestens:

```text
Counting Semaphore
Binary Semaphore
```

### Counting Semaphore

Verwaltet mehrere verfügbare Einheiten.

```text
Initial Count = 4

Task A → Acquire → 3
Task B → Acquire → 2
Task C → Acquire → 1
Task D → Acquire → 0
Task E → Wait
```

### Binary Semaphore

Besitzt effektiv die Zustände:

```text
0
1
```

Eine Binary Semaphore darf zur Signalisierung verwendet werden, besitzt jedoch weiterhin keine Mutex-Ownership-Semantik.

## Semaphore-Zustand

Eine Semaphore enthält mindestens:

```text
Count
MaximumCount
Waiters
```

Optional:

```text
SemaphoreID
WaiterCount
AcquireCount
ReleaseCount
ContentionCount
```

## Acquire

Acquire versucht, eine Einheit der Semaphore zu reservieren.

```text
Acquire
   ↓
Count > 0?
 ┌────┴────┐
Yes        No
 ↓          ↓
Count--    Wait
 ↓
Continue
```

Prüfung und Änderung des Counters müssen atomar erfolgen.

## Release

Release gibt eine oder mehrere Einheiten zurück.

```text
Release
   ↓
Waiting Tasks?
 ┌────┴────┐
Yes        No
 ↓          ↓
Wake       Increment
Waiter     Count
```

Die Implementierung muss verhindern, dass verfügbare Einheiten verloren gehen oder mehrfach vergeben werden.

## Wait Queue

Kann Acquire nicht sofort erfüllt werden, wird der Task in eine Wait Queue eingetragen.

```text
Semaphore
    ↓
Wait Queue
├── Task A
├── Task B
└── Task C
```

Die Auswahl eines Waiters darf Scheduling-Priorität, QoS, Deadline oder Realtime-Anforderungen berücksichtigen.

FIFO-Verhalten ist nicht grundsätzlich vorgeschrieben.

## Blocking

Normale Semaphore sollen wartende Tasks blockieren, statt dauerhaft aktiv zu warten.

```text
Acquire Failed
      ↓
Register Waiter
      ↓
Task → Waiting
      ↓
Scheduler
```

Dadurch bleibt CPU-Zeit für andere ausführbare Tasks verfügbar.

## TryAcquire

NovaOS soll eine nicht blockierende Variante unterstützen.

```text
TryAcquire
   ↓
Available?
 ┌───┴───┐
Yes      No
 ↓        ↓
Success  Failure
```

`TryAcquire` darf den Task nicht blockieren.

## Timeout

Semaphore-Waits dürfen optional zeitlich begrenzt werden.

```text
Acquire
   ↓
Wait
   ↓
Resource Available / Timeout / Cancellation
```

Timeout muss eindeutig von erfolgreichem Acquire unterscheidbar sein.

## Cancellation

Ein wartender Task darf kontrolliert aus der Wait Queue entfernt werden.

```text
Waiting
  ↓
Cancellation
  ↓
Remove Waiter
  ↓
Cancelled
```

Dabei darf keine Semaphore-Einheit verloren gehen oder unbeabsichtigt vergeben werden.

## Ressourcenmodell

Counting Semaphores eignen sich zur Begrenzung paralleler Ressourcennutzung.

Beispiele:

```text
Connection Slots
I/O Requests
Worker Capacity
Buffer Pool
Device Queue Slots
Concurrent Operations
```

Beispiel:

```text
GPU Queue Capacity = 8
        ↓
Semaphore(8)
        ↓
Maximum 8 Concurrent Requests
```

## Producer / Consumer

Semaphore können zur Koordination von Producer-Consumer-Strukturen verwendet werden.

```text
Producer
   ↓
Produce Item
   ↓
Release(items)
   ↓
Consumer
   ↓
Acquire(items)
```

Die eigentlichen Daten benötigen gegebenenfalls zusätzliche Synchronisation.

## Realtime

Bei Realtime-Tasks muss das Blockierungsverhalten berücksichtigt werden.

```text
Realtime Task
      ↓
Semaphore Wait
      ↓
Bounded Resource Availability?
```

Eine Semaphore allein garantiert keine begrenzte Wartezeit.

Hard-Realtime-Systeme müssen Ressourcenanzahl, Konkurrenz und maximale Blockierungsdauer analysieren.

## Priority Inversion

Da Semaphore keine eindeutige Ownership besitzen, ist klassische Priority Inheritance nicht immer direkt anwendbar.

Für zeitkritische Ressourcen sollen deshalb geeignete Ressourcen-, Admission- oder Scheduling-Policies verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS Counting Semaphores unterstützen.
2. NovaOS SOLL Binary Semaphores unterstützen.
3. Acquire und Counter-Änderung MÜSSEN atomar erfolgen.
4. Ein erfolgreicher Acquire MUSS genau eine angeforderte Ressourceneinheit reservieren.
5. Release MUSS Ressourcen kontrolliert wieder verfügbar machen.
6. Der Semaphore Counter DARF den definierten `MaximumCount` NICHT überschreiten.
7. Wartende Tasks SOLLEN blockiert werden, statt dauerhaft aktiv zu warten.
8. Eine nicht blockierende `TryAcquire`-Operation SOLL verfügbar sein.
9. Timeout und Cancellation SOLLEN für blockierende Waits unterstützt werden.
10. Cancellation DARF keine Semaphore-Einheit verlieren oder duplizieren.
11. Semaphore DÜRFEN keine Mutex-Ownership-Semantik vortäuschen.
12. Counter, Waiter und Contention SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-SYNC-0003`

## Ergebnis

```text
Limited Resource Pool
        ↓
Semaphore Counter
        ↓
Acquire / Wait
        ↓
Controlled Parallel Access
        ↓
Release / Wake
```

NovaOS erhält damit einen allgemeinen Synchronisationsmechanismus zur kontrollierten Verwaltung begrenzter Ressourcen und zur effizienten Koordination konkurrierender Tasks ohne zwingende Ownership-Semantik.