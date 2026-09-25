# NPSPEC-SYNC-BARRIER-0001 – Nova Barrier Synchronization

## Status

Angenommen

## Kategorie

Synchronization / Barrier / Concurrency / Parallel Execution

## Zweck

NovaOS definiert Barriers zur Synchronisation mehrerer Tasks an einem gemeinsamen Ausführungspunkt.

```text
Task A ──────┐
Task B ──────┤
Task C ──────┼→ Barrier → Continue
Task D ──────┘
```

Kein teilnehmender Task darf die Barrier passieren, bevor die für diese Synchronisationsphase erforderlichen Teilnehmer angekommen sind.

## Grundprinzip

Eine Barrier koordiniert eine Gruppe paralleler Tasks.

```text
Parallel Work
     ↓
Barrier
     ↓
All Required Tasks Arrived?
   ┌────────┴────────┐
  No                Yes
   ↓                  ↓
 Wait              Release
```

Dabei gilt:

```text
Barrier ≠ Mutex
Barrier ≠ Semaphore
```

Eine Barrier schützt keine Ressource, sondern synchronisiert Ausführungsphasen.

## Barrier-Zustand

Eine Barrier verwaltet mindestens:

```text
ParticipantCount
ArrivalCount
Generation
Waiters
State
```

Mögliche Zustände:

```text
Open
Waiting
Releasing
Broken
```

`Generation` identifiziert die aktuelle Synchronisationsrunde.

## Arrival

Ein Task meldet seine Ankunft an der Barrier.

```text
Arrive
  ↓
ArrivalCount++
  ↓
Required Count reached?
```

Ist die erforderliche Teilnehmerzahl noch nicht erreicht, wartet der Task.

Der letzte erforderliche Teilnehmer löst die Freigabe der Barrier aus.

## Wait

Typischer Ablauf:

```text
Task
 ↓
Complete Phase N
 ↓
ArriveAndWait
 ↓
Barrier
 ↓
All Participants Arrived
 ↓
Continue Phase N+1
```

Wartende Tasks sollen blockiert werden, statt CPU-Zeit durch dauerhaftes Busy Waiting zu verbrauchen.

## Generationen

Eine wiederverwendbare Barrier muss einzelne Synchronisationsrunden eindeutig trennen.

```text
Generation 0
    ↓
Release
    ↓
Generation 1
    ↓
Release
    ↓
Generation 2
```

Ein verspätetes Wakeup einer vorherigen Generation darf nicht als Ankunft oder Freigabe einer späteren Generation interpretiert werden.

## Wiederverwendung

Nach erfolgreicher Freigabe darf eine Barrier für die nächste Phase zurückgesetzt werden.

```text
ArrivalCount = 0
Generation++
```

Dieser Übergang muss atomar gegenüber neuen Ankünften erfolgen.

## Dynamische Teilnehmer

NovaOS darf Barriers mit dynamischer Teilnehmerzahl unterstützen.

Mögliche Operationen:

```text
RegisterParticipant
RemoveParticipant
ArriveAndRemove
```

Änderungen der Teilnehmerzahl müssen eindeutig einer Barrier-Generation zugeordnet sein.

Ein Task darf sich nicht unkontrolliert entfernen, wenn dadurch andere Teilnehmer dauerhaft blockiert würden.

## Cancellation

Cancellation eines wartenden Tasks kann die Barrier unerfüllbar machen.

```text
Task Cancelled
      ↓
Barrier Policy
      ↓
Remove Participant
or
Break Barrier
or
Continue Waiting
```

Das Verhalten muss explizit definiert sein.

Cancellation darf nicht stillschweigend die Synchronisationssemantik verändern.

## Broken Barrier

Kann eine Barrier nicht mehr korrekt erfüllt werden, darf sie in den Zustand `Broken` wechseln.

Beispiele:

```text
Participant Failure
Cancellation
Timeout
Invalid Participant Change
```

```text
Barrier
   ↓
Broken
   ↓
Wake Waiters
   ↓
Return Failure
```

Wartende Tasks dürfen in diesem Fall nicht unbegrenzt blockiert bleiben.

## Timeout

Barrier Waits dürfen optional eine Deadline oder einen Timeout besitzen.

```text
Wait
 ↓
Release / Timeout / Cancellation / Failure
```

Ein Timeout muss eindeutig von einer erfolgreichen Barrier-Freigabe unterscheidbar sein.

## Memory Ordering

Eine erfolgreiche Barrier muss eine definierte Speicherordnung zwischen den beteiligten Phasen bereitstellen.

Konzeptionell:

```text
Phase N Writes
      ↓
Barrier
      ↓
Phase N+1 Reads
```

Nach erfolgreicher Barrier-Freigabe müssen relevante Speicheränderungen der vorherigen Phase gemäß dem Nova Memory Model sichtbar sein.

## Parallel Computing

Barriers eignen sich insbesondere für phasenbasierte parallele Algorithmen.

```text
Phase 1
├── Task A
├── Task B
└── Task C
      ↓
Barrier
      ↓
Phase 2
├── Task A
├── Task B
└── Task C
```

Eine Barrier sollte nicht verwendet werden, wenn Tasks unabhängig fortfahren können.

## Task Groups

Barriers dürfen mit Task Groups kombiniert werden.

```text
Task Group
├── Worker A
├── Worker B
├── Worker C
└── Barrier
```

Die Barrier-Teilnehmer müssen dabei nicht zwingend allen Tasks der Gruppe entsprechen.

## Realtime

Bei Realtime-Ausführung bestimmt der langsamste erforderliche Teilnehmer die Barrier-Latenz.

```text
Fast Tasks ─────┐
Slow Task ──────┼→ Barrier Release
```

Hard-Realtime-Barriers benötigen daher begrenzte Teilnehmerlaufzeiten und kontrollierte Blockierungszeiten.

## Normative Anforderungen

1. NovaOS MUSS Barrier-Synchronisation für mehrere Tasks unterstützen.
2. Eine Barrier MUSS die erforderliche Teilnehmerzahl eindeutig verwalten.
3. Kein Teilnehmer DARF eine erfolgreiche Barrier vor vollständiger Synchronisation passieren.
4. Wiederverwendbare Barriers MÜSSEN Synchronisationsrunden über Generationen trennen.
5. Generationen MÜSSEN gegen verspätete Wakeups vorheriger Runden geschützt sein.
6. Wartende Tasks SOLLEN blockiert werden, statt dauerhaft aktiv zu warten.
7. Änderungen dynamischer Teilnehmerzahlen MÜSSEN kontrolliert erfolgen.
8. Cancellation und Teilnehmerausfall MÜSSEN eine definierte Barrier-Policy besitzen.
9. Eine unerfüllbare Barrier MUSS in einen erkennbaren Fehlerzustand wechseln können.
10. Timeout, Cancellation und erfolgreiche Freigabe MÜSSEN unterscheidbar sein.
11. Erfolgreiche Barrier-Synchronisation MUSS definierte Memory-Ordering-Garantien bereitstellen.
12. Generation, Teilnehmer, Waiter und Barrier-Zustand SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-CONDITION-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-SCHEDULER-0001`
- `ADR-SYNC-0006`

## Ergebnis

```text
Parallel Phase N
      ↓
Participants Arrive
      ↓
Barrier
      ↓
All Required Participants
      ↓
Generation Release
      ↓
Parallel Phase N+1
```

NovaOS erhält damit einen generationssicheren Synchronisationsmechanismus für phasenbasierte parallele Ausführung, der Blocking, Cancellation, Fehlerfälle und definierte Speicherordnung kontrolliert miteinander verbindet.