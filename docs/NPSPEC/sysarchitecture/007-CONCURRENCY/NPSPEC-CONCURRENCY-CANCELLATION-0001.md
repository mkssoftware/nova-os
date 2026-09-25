# NPSPEC-CONCURRENCY-CANCELLATION-0001 – Nova Cancellation

## Status

Angenommen

## Kategorie

Concurrency / Cancellation / Structured Concurrency / Lifecycle

## Zweck

NovaOS definiert Cancellation als kontrollierten Mechanismus zum Beenden nicht mehr benötigter oder nicht mehr zulässiger nebenläufiger Arbeit.

```text
Cancellation Request
        ↓
Propagation
        ↓
Task Cooperation
        ↓
Cleanup
        ↓
Cancelled
```

## Grundprinzip

Cancellation ist ein regulärer Bestandteil des Task-Lebenszyklus und kein unkontrollierter Abbruch.

```text
Cancellation ≠ Forced Termination
```

Tasks erhalten ein Cancellation-Signal und müssen dieses an definierten sicheren Punkten verarbeiten können.

## Cancellation Context

Jeder cancellable Task besitzt einen Cancellation Context.

```text
CancellationContext
├── State
├── Parent
├── Deadline
└── Reason
```

Mindestens folgende Zustände werden unterschieden:

```text
Active
CancellationRequested
Cancelled
```

## Propagation

Cancellation propagiert standardmäßig entlang der strukturierten Task-Hierarchie.

```text
Parent
  ↓
Cancel
  ↓
Child A
Child B
Child C
```

Child Tasks dürfen Cancellation nicht unbegrenzt ignorieren.

## Cancellation Points

Tasks prüfen Cancellation an definierten Punkten.

Beispiele:

```text
Await
Blocking Operation
IPC Wait
Timer Wait
Explicit Check
Resource Acquisition
```

Lang laufende Operationen sollen geeignete Cancellation Points bereitstellen.

## Cleanup

Nach einer Cancellation-Anforderung muss ein Task seine Ressourcen kontrolliert bereinigen können.

```text
Cancellation Requested
        ↓
Stop New Work
        ↓
Cleanup
        ↓
Release Resources
        ↓
Cancelled
```

Cleanup darf keine neuen unkontrollierten Hintergrundaufgaben erzeugen.

## Blocking Operations

Blockierende Operationen sollen durch Cancellation unterbrechbar sein.

```text
Task
 ↓
Wait for I/O
 ↓
Cancellation
 ↓
Abort / Wake
 ↓
Cleanup
```

Falls eine Operation technisch nicht sofort abbrechbar ist, muss ihr Zustand eindeutig bleiben.

## Task Groups

Cancellation einer Task Group propagiert zu ihren aktiven Child Tasks.

```text
Cancel Group
    ↓
Cancel Children
    ↓
Wait for Resolution
    ↓
Group Cancelled
```

Die Gruppe gilt erst als beendet, wenn ihre abhängigen Tasks kontrolliert aufgelöst wurden.

## Deadlines

Eine Deadline darf automatisch eine Cancellation auslösen.

```text
Deadline Reached
      ↓
Cancellation Request
```

Dabei gilt:

```text
Deadline Expired ≠ Task Immediately Terminated
```

Die tatsächliche Beendigung folgt weiterhin dem definierten Cancellation-Verfahren.

## Nicht abbrechbare Bereiche

Kurze kritische Bereiche dürfen Cancellation temporär verzögern.

```text
Enter Critical Region
        ↓
Defer Cancellation
        ↓
Leave Critical Region
        ↓
Process Cancellation
```

Solche Bereiche müssen möglichst kurz bleiben.

## Eskalation

Reagiert ein Task nicht innerhalb definierter Grenzen, darf eine höhere Ebene eskalieren.

```text
Cancellation
    ↓
Timeout
    ↓
Supervisor
    ↓
Escalation
```

Eine erzwungene Terminierung ist nur zulässig, wenn die daraus entstehenden Zustands- und Ressourcenfolgen kontrolliert behandelt werden können.

## Normative Anforderungen

1. NovaOS MUSS Cancellation als natives Concurrency-Konzept unterstützen.
2. Cancellation MUSS von erzwungener Terminierung unterschieden werden.
3. Cancellable Tasks MÜSSEN einen definierten Cancellation Context besitzen.
4. Cancellation MUSS durch strukturierte Task-Hierarchien propagierbar sein.
5. Lang laufende Operationen SOLLEN geeignete Cancellation Points besitzen.
6. Blockierende Operationen SOLLEN Cancellation unterstützen.
7. Cancellation MUSS kontrolliertes Cleanup ermöglichen.
8. Taskgebundene Ressourcen MÜSSEN bei Cancellation behandelt werden.
9. Task Groups DÜRFEN erst nach Auflösung ihrer abhängigen Tasks als cancelled gelten.
10. Kritische Bereiche DÜRFEN Cancellation nur kontrolliert und zeitlich begrenzt verzögern.
11. Nicht reagierende Tasks MÜSSEN eskaliert werden können.
12. Cancellation-Zustand und Ursache SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-STRUCTURED-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-CONCURRENCY-0004`

## Ergebnis

```text
 Cancellation Request
         ↓
Structured Propagation
         ↓
  Cooperative Stop
         ↓
      Cleanup
         ↓
Defined Final State
```

NovaOS erhält damit ein kontrolliertes Cancellation-Modell, bei dem nebenläufige Arbeit beendet werden kann, ohne Ressourcen, Task-Hierarchien oder Systemzustände unkontrolliert zurückzulassen.