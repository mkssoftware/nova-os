# ADR-SYNC-0004 – Futex-artige User-Kernel-Synchronisation

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / IPC / Userspace / Scheduler

## Kontext

Synchronisationsprimitive wie Mutexes, Semaphores, Condition Variables und Wait Queues werden sehr häufig verwendet.

Würde jede Synchronisationsoperation einen Syscall erfordern, entstünden unnötige:

- User-/Kernel-Transitions,
- Scheduler-Aufrufe,
- Context-Switches,
- Cache-Effekte.

Viele Synchronisationsfälle können vollständig im Userspace entschieden werden.

Der Kernel wird erst benötigt, wenn ein Execution Context tatsächlich warten oder aufgeweckt werden muss.

NovaOS benötigt deshalb einen Mechanismus nach dem Grundprinzip von Linux-Futexes, jedoch eingebettet in das eigene NovaOS-Synchronisations-, Scheduler- und Execution-Domain-Modell.

---

## Entscheidung

NovaOS führt eine **futex-artige User-Kernel-Synchronisationsschnittstelle** als fundamentales Primitive für effiziente Userspace-Synchronisation ein.

Das Grundmodell lautet:

```text
Userspace Atomic State
        ↓
Fast Path
        ↓
No Kernel Transition

Contention
        ↓
Kernel Wait Primitive
        ↓
Scheduler blocks Task
        ↓
Wake
        ↓
Task becomes runnable
```

Der gemeinsame Synchronisationszustand verbleibt grundsätzlich im Userspace.

Der Kernel verwaltet nur die notwendigen Wait-/Wake-Operationen und deren Integration mit Scheduler, Deadlines, Cancellation und Priority-Inversion-Mechanismen.

Die Architektur übernimmt damit das **Futex-Prinzip**, nicht zwingend die konkrete Linux-Futex-ABI.

---

## Architekturprinzip

```text
Uncontended synchronization stays in userspace.

The kernel participates only when waiting is required.
```

Der typische Ablauf ist:

```text
Atomic User State
      │
      ├── uncontended
      │       ↓
      │   Userspace only
      │
      └── contended
              ↓
          Kernel Wait
              ↓
           Scheduler
              ↓
             Wake
```

---

## Fast Path

Der uncontended Fast Path darf vollständig über Atomics im Userspace ausgeführt werden.

Beispiel:

```text
Unlocked
   ↓
Atomic CAS
   ↓
Locked
```

Ist die Operation erfolgreich, erfolgt kein Syscall.

Dies ist der bevorzugte Pfad für häufig verwendete Synchronisationsprimitive.

---

## Slow Path

Kann die Synchronisation nicht unmittelbar abgeschlossen werden, darf die Runtime den Kernel aufrufen.

Der Kernel kann den aktuellen Task daraufhin:

```text
Running
   ↓
Waiting
```

setzen und aus der CPU-Scheduling-Menge entfernen.

Nach einem passenden Wake-Ereignis wird der Task wieder runnable.

---

## Wait Semantik

Eine Wait-Operation muss an einen atomar überprüfbaren Userspace-Zustand gekoppelt sein.

Konzeptionell:

```text
Wait(address, expected)
```

Der Kernel blockiert nur, wenn der beobachtete Zustand weiterhin der erwarteten Bedingung entspricht.

Damit wird das klassische Race verhindert:

```text
Check state
    ↓
Other task changes state
    ↓
Sleep
```

Die konkrete ABI und Vergleichssemantik werden in den NPSPECs definiert.

---

## Wake Semantik

Eine Wake-Operation signalisiert dem Kernel, wartende Tasks für einen Synchronisationszustand wieder ausführbar zu machen.

Konzeptionell:

```text
Wake(address, count)
```

Dabei kann eine begrenzte Anzahl oder eine definierte Gruppe von Waitern aktiviert werden.

Wake bedeutet grundsätzlich:

```text
Waiting → Runnable
```

und nicht:

```text
Waiting → Running immediately
```

Die tatsächliche CPU-Zuteilung bleibt Aufgabe des Schedulers.

---

## Keine Kernel-Ownership im Fast Path

Der Kernel muss für uncontended Synchronisation keinen permanenten Mutex- oder Semaphore-Zustand verwalten.

Dadurch bleibt die häufige Operation:

```text
Userspace
    ↓
Atomic
    ↓
Userspace
```

und benötigt keine Kernelobjektoperation.

---

## Shared Memory

Futex-artige Synchronisation kann mit Shared Memory zwischen mehreren Execution Domains verwendet werden, sofern diese den betreffenden Speicher ausdrücklich gemeinsam nutzen dürfen.

Der Kernel muss dabei die Identität des Synchronisationsortes eindeutig bestimmen können.

Virtuelle Adressen allein dürfen bei Shared-Memory-Synchronisation nicht als globale physische Identität angenommen werden.

---

## Execution-Domain-Isolation

Ein Execution Domain darf ausschließlich auf Synchronisationszustände warten beziehungsweise diese aufwecken, für die es die notwendige Speicher- und Capability-Autorisierung besitzt.

Der Mechanismus darf keine Isolation zwischen Execution Domains umgehen.

---

## Scheduler-Integration

Kernel-Waiter werden direkt mit dem Scheduler integriert.

Ein wartender Task:

- verbraucht keine aktive CPU-Zeit,
- bleibt seinem Execution Scope zugeordnet,
- behält seine Scheduling Class,
- behält relevante Deadline- und QoS-Informationen,
- bleibt der Resource Economy zurechenbar.

Wakeup verwendet die normalen Scheduler-Regeln für CPU Placement und Dispatch.

---

## Structured Concurrency

Futex-artiges Warten verändert nicht die Ownership eines Tasks.

Der Task bleibt Bestandteil seiner Structured-Concurrency-Hierarchie.

```text
Execution Scope
      ↓
Task
      ↓
Synchronization Wait
```

Cancellation und Scope-Lebensdauer müssen weiterhin durchsetzbar bleiben.

---

## Cancellation

Kernel-Wait-Operationen müssen mit Cancellation integrierbar sein.

Ein wartender Task kann beispielsweise durch:

- Synchronisationsereignis,
- Cancellation,
- Deadline-Ablauf,
- Domain-Termination

wieder aktiviert werden.

Die Runtime muss unterscheiden können, aus welchem Grund der Wait beendet wurde.

---

## Deadlines und Timeouts

Wait-Operationen müssen zeitlich begrenzbar sein.

Dabei sollen die systemweiten Deadline-Mechanismen verwendet werden.

```text
Wait
 ├── Wake
 ├── Cancellation
 └── Deadline Expired
```

Relative Timeouts sollen intern möglichst früh auf die geeignete Deadline-Semantik abgebildet werden.

---

## Priority Inversion

Futex-artige Synchronisation muss mit den NovaOS-Mechanismen gegen Priority Inversion integrierbar sein.

Für geeignete Mutex-Primitive kann der Kernel Informationen über:

```text
Waiter
Owner
Scheduling Urgency
```

verwenden.

Priority-Inheritance- oder vergleichbare Mechanismen werden separat spezifiziert.

Nicht jede Futex-artige Wait-Adresse besitzt automatisch Owner-Semantik.

---

## Realtime

Realtime-fähige Synchronisationsprimitive können den Futex-artigen Mechanismus verwenden, sofern:

- Wait-/Wake-Kosten ausreichend kontrollierbar sind,
- Priority Inversion behandelt wird,
- notwendige Ressourcen reserviert sind,
- die verwendete Synchronisationspolicy für die Realtime-Klasse geeignet ist.

Ein gewöhnlicher Futex-artiger Wait erzeugt allein keine Realtime-Garantie.

---

## Spurious Wakeups

Höhere Synchronisationsprimitive dürfen nicht voraussetzen, dass jeder Wake automatisch bedeutet, dass ihre gewünschte Bedingung erfüllt ist.

Das bevorzugte Muster bleibt:

```text
while condition not satisfied:
    wait
```

Die genaue Spurious-Wakeup-Semantik wird durch die jeweilige NPSPEC festgelegt.

---

## Memory Ordering

Wait/Wake ersetzt keine korrekte Atomic- und Memory-Ordering-Semantik.

Der eigentliche Synchronisationszustand wird durch die Userspace-Atomics definiert.

```text
Atomic State
    +
Memory Ordering
    +
Kernel Wait/Wake
```

bilden gemeinsam das vollständige Primitive.

Der Kernel darf fehlendes Acquire-/Release-Verhalten einer fehlerhaften Userspace-Implementierung nicht implizit reparieren.

---

## Prozess- und Speicherlebensdauer

Wird ein Speicherbereich entfernt, während Waiter darauf registriert sind, muss NovaOS diesen Zustand kontrolliert behandeln.

Dies betrifft insbesondere:

- Unmapping,
- Shared-Memory-Destruction,
- Process Termination,
- Execution-Domain-Termination.

Es dürfen keine dauerhaft verwaisten Kernel-Waiter entstehen.

---

## Fairness

Die Low-Level-Wake-Schnittstelle garantiert nicht automatisch eine universelle Fairnessreihenfolge.

Höhere Synchronisationsprimitive können stärkere Fairnessanforderungen definieren.

Der Kernel muss jedoch starvation-fördernde interne Implementierungen vermeiden, soweit dies mit der jeweiligen Policy vereinbar ist.

---

## Thundering Herd

Wake-Operationen sollen nicht unnötig große Mengen von Tasks aktivieren.

Wenn nur ein Task Fortschritt machen kann, soll grundsätzlich eine begrenzte Wake-Operation möglich sein.

Damit werden unnötige:

- Scheduler-Aktivierungen,
- Context Switches,
- Cache-Contention

reduziert.

---

## Private und Shared Synchronisation

Die Implementierung darf zwischen:

```text
Execution-Domain-private synchronization

und

Cross-Domain shared synchronization
```

unterscheiden.

Private Synchronisation kann effizientere Lookup- und Wait-Queue-Mechanismen verwenden, solange die äußere Semantik identisch bleibt.

---

## Deterministic Mode

Deterministic Mode kann die Reihenfolge der Auswahl mehrerer Waiter einschränken.

Dadurch können reproduzierbare Wakeup-Reihenfolgen bereitgestellt werden, wenn der Execution Contract dies verlangt.

Die normale Implementierung muss nicht grundsätzlich eine globale deterministische Wake-Reihenfolge erzwingen.

---

## Introspection

NovaOS muss Futex-artige Waits ausreichend introspektierbar machen.

Relevante Informationen können sein:

```text
Waiting Task
Execution Domain
Wait Object Identity
Wait Duration
Deadline
Wake Reason
Contention
```

Debugging-Werkzeuge sollen dadurch insbesondere:

- Deadlocks,
- ungewöhnlich lange Waits,
- hohe Contention,
- Priority Inversion

analysieren können.

---

## Sicherheit

Userspace-Werte sind nicht vertrauenswürdig.

Der Kernel darf:

- Pointer,
- Zustände,
- Ownership-Angaben,
- Wait-Adressen

nicht ungeprüft übernehmen.

Ungültige Speicheradressen oder nicht autorisierte Shared-Memory-Operationen müssen sicher abgewiesen werden.

---

## Normative Anforderungen

1. NovaOS MUSS eine futex-artige User-Kernel-Synchronisationsschnittstelle bereitstellen.
2. Uncontended Synchronisation SOLL vollständig im Userspace möglich sein.
3. Der Kernel SOLL nur bei tatsächlichem Wait-/Wake-Bedarf beteiligt werden.
4. Der Synchronisationszustand MUSS über das NovaOS-Atomic- und Memory-Ordering-Modell abbildbar sein.
5. Wait MUSS den erwarteten Userspace-Zustand atomar beziehungsweise race-sicher mit dem Blocking-Vorgang koordinieren.
6. Wake MUSS wartende Tasks in den Runnable-Zustand überführen können.
7. Wake DARF keine sofortige CPU-Ausführung garantieren.
8. Futex-artige Synchronisation MUSS mit Shared Memory verwendbar sein.
9. Shared-Memory-Wait-Objekte MÜSSEN unabhängig von zufällig identischen virtuellen Adressen identifizierbar sein.
10. Execution-Domain-Isolation MUSS erhalten bleiben.
11. Waiter MÜSSEN in den normalen Scheduler integriert werden.
12. Wartende Tasks MÜSSEN ihrer Structured-Concurrency-Hierarchie zugeordnet bleiben.
13. Wait MUSS mit Cancellation integrierbar sein.
14. Wait MUSS mit Deadlines beziehungsweise Timeouts integrierbar sein.
15. Der Grund für die Beendigung eines Waits MUSS unterscheidbar sein.
16. Owner-basierte Synchronisationsprimitive MÜSSEN mit Priority-Inversion-Mechanismen integrierbar sein.
17. Nicht-owner-basierte Waits DÜRFEN NICHT künstlich als Mutex-Ownership interpretiert werden.
18. Realtime-Verwendung MUSS die jeweiligen Realtime-Synchronisationsanforderungen erfüllen.
19. Wait/Wake DARF korrekte Atomic-Memory-Ordering-Semantik NICHT ersetzen.
20. Höhere Primitive MÜSSEN mit zulässigen Spurious Wakeups umgehen können, sofern die jeweilige Schnittstelle diese erlaubt.
21. Unmapping und Domain-Termination MÜSSEN wartende Kernelzustände kontrolliert bereinigen.
22. Begrenzte Wake-Operationen MÜSSEN möglich sein.
23. Die Implementierung SOLL unnötige Thundering-Herd-Wakeups vermeiden.
24. Private Synchronisation DARF intern optimiert werden, ohne die öffentliche Semantik zu verändern.
25. Deterministic Mode MUSS Wakeup-Auswahl bei Bedarf einschränken können.
26. Waits, Wake-Gründe und Contention SOLLEN introspektierbar sein.
27. Userspace-Adressen und Synchronisationszustände MÜSSEN als nicht vertrauenswürdige Eingaben behandelt werden.
28. Futex-artige Synchronisation DARF keine Capability-, Speicher- oder Execution-Domain-Grenzen umgehen.

---

## Konsequenzen

### Positive Konsequenzen

- uncontended Synchronisation benötigt keinen Syscall,
- geringere Kernel- und Context-Switch-Kosten,
- effiziente Grundlage für Mutexes, Condition Variables und Semaphores,
- direkte Integration mit Scheduler, Cancellation und Deadlines,
- geeignet für Thread- und Shared-Memory-Synchronisation,
- NovaOS bleibt unabhängig von der konkreten Linux-Futex-ABI.

### Negative Konsequenzen

- Race-freie Wait-Semantik ist anspruchsvoll,
- Shared-Memory-Identität erhöht die Implementierungskomplexität,
- Priority-Inversion-Unterstützung benötigt zusätzliche Kernelintegration,
- Speicher-Unmapping und Domain-Termination müssen mit Wait Queues koordiniert werden.

---

## Verworfene Alternativen

### Jede Synchronisationsoperation als Syscall

Verworfen.

Der häufige uncontended Fast Path würde unnötige Kernel-Transitions verursachen.

### Synchronisation ausschließlich im Userspace

Verworfen.

Tatsächlich wartende Tasks könnten nicht effizient aus dem Scheduler entfernt und später geweckt werden.

### Linux-Futex-ABI unverändert übernehmen

Verworfen.

NovaOS übernimmt das bewährte Grundprinzip, definiert jedoch eine eigene Semantik passend zu NovaABI, Execution Domains, Structured Concurrency und Execution Contracts.

### Kernelobjekt für jeden Mutex

Verworfen.

Dies erzeugt unnötigen Kernelzustand und Kosten für uncontended Synchronisation.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-SYNC-0002_Lock_Free_Strukturen_gezielt_einsetzen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-FUTEX-0001`
- `NPSPEC-SYNC-FUTEX-WAIT-0001`
- `NPSPEC-SYNC-FUTEX-WAKE-0001`
- `NPSPEC-SYNC-FUTEX-SHARED-0001`
- `NPSPEC-SYNC-FUTEX-PRIORITY-0001`
- `NPSPEC-SYNC-FUTEX-CANCELLATION-0001`
- `NPSPEC-SYNC-FUTEX-REALTIME-0001`
- `NPSPEC-SYNC-FUTEX-TEST-0001`

---

## Ergebnis

NovaOS verwendet eine futex-artige Architektur, bei der der häufige Synchronisationspfad im Userspace verbleibt:

```text
Atomic Userspace State
        │
        ├── success
        │      ↓
        │   Fast Path
        │
        └── contention
               ↓
          Kernel Wait
               ↓
           Scheduler
               ↓
              Wake
```

Damit entsteht ein effizientes Fundament für höherwertige Userspace-Synchronisationsprimitive, ohne für jede Operation Kernelzustand oder einen Syscall zu benötigen.

Die zentrale Architekturregel lautet:

```text
Synchronize in userspace when possible.

Enter the kernel when waiting is necessary.

Keep state cheap.

Keep blocking structured.
```