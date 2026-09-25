# NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001 – Nova Structured Concurrency

## Status

Angenommen

## Kategorie

Architecture / Concurrency / Tasks / Lifecycle

## Zweck

NovaOS verwendet Structured Concurrency, damit nebenläufige Aufgaben einen klaren Besitzer, Lebenszyklus und Abbruchpfad besitzen.

```text
Parent Task
 ├── Child Task A
 ├── Child Task B
 └── Child Task C
```

Kindaufgaben dürfen ihren definierten Ausführungskontext nicht unkontrolliert überleben.

## Grundprinzip

Nebenläufigkeit bildet eine hierarchische Task-Struktur.

```text
Scope
  ↓
Parent Task
  ├── Child
  └── Child
        ↓
Scope End
```

Ein Scope gilt erst als beendet, wenn seine relevanten Kindaufgaben beendet, abgebrochen oder kontrolliert übertragen wurden.

## Task-Modell

Eine Task besitzt mindestens:

```text
TaskID
ParentID
State
Execution Context
Cancellation Context
Result
```

Typische Zustände:

```text
Created
Running
Waiting
Completed
Cancelled
Failed
```

## Lebenszyklus

Der Lebenszyklus einer Child Task ist an ihren Parent bzw. Scope gebunden.

```text
Parent starts
    ↓
Children execute
    ↓
Join / Cancel
    ↓
Parent completes
```

Unkontrollierte verwaiste Tasks sollen vermieden werden.

## Cancellation

Abbruch wird strukturiert durch die Task-Hierarchie propagiert.

```text
Cancel Parent
     ↓
Cancel Children
```

Tasks müssen definierte Cleanup-Pfade besitzen.

## Fehlerpropagation

Fehler von Child Tasks werden an den zuständigen Scope zurückgegeben.

```text
Child Failure
     ↓
Parent Scope
     ↓
Handle / Cancel / Propagate
```

Fehler dürfen nicht unbemerkt in unabhängigen Hintergrundtasks verloren gehen.

## Ressourcen

Task-gebundene Ressourcen sollen an denselben Lebenszyklus gekoppelt werden.

```text
Task
 ├── Memory
 ├── Handles
 ├── Buffers
 └── Reservations
```

Beim Ende der Task müssen gebundene Ressourcen kontrolliert freigegeben werden.

## Detached Tasks

Explizit unabhängige Hintergrundaufgaben sind zulässig, benötigen jedoch einen neuen definierten Besitzer, beispielsweise:

```text
System Service
Session
Job Manager
```

`Detached` darf nicht als Umgehung von Structured Concurrency verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS hierarchische Task-Strukturen unterstützen.
2. Tasks SOLLEN einen eindeutig definierten Parent oder Owner besitzen.
3. Child Tasks DÜRFEN ihren Scope nicht unkontrolliert überleben.
4. Cancellation MUSS durch Task-Hierarchien propagierbar sein.
5. Fehler von Child Tasks MÜSSEN kontrolliert behandelt oder propagiert werden.
6. Task-gebundene Ressourcen MÜSSEN kontrolliert freigegeben werden.
7. Scope-Ende MUSS den Zustand relevanter Child Tasks berücksichtigen.
8. Unabhängige Hintergrundtasks MÜSSEN einen expliziten neuen Owner erhalten.
9. Structured Concurrency MUSS mit Scheduling und Execution Contracts kombinierbar sein.
10. Structured Concurrency DARF Realtime- oder Determinismus-Anforderungen nicht verletzen.

## Abhängigkeiten

- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `ADR-ARCH-0010_Structured_Concurrency`
- `ADR-CONCURRENCY-0001`

## Ergebnis

```text
Scope
 └── Task
      ├── Child
      └── Child
           ↓
     Controlled Completion
```

NovaOS erhält damit nachvollziehbare Nebenläufigkeit mit klaren Lebenszyklen, Fehlerpfaden und Ressourcenbesitz.