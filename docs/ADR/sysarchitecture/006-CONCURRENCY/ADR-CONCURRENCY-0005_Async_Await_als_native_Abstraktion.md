# ADR-CONCURRENCY-0005 – Async/Await als native Abstraktion

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Async / Await / Structured Concurrency / Runtime / NovaABI

## Kontext

NovaOS definiert mit:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`

ein systemweites Modell für strukturierte nebenläufige Arbeit.

Viele Operationen verbringen einen erheblichen Teil ihrer Lebensdauer wartend:

- Storage-I/O,
- Netzwerk-I/O,
- IPC,
- Timer,
- Geräteoperationen,
- Benutzerinteraktion,
- GPU-/NPU-Completion.

Ein Thread pro wartender Operation wäre unnötig teuer und würde die Skalierbarkeit begrenzen.

NovaOS benötigt deshalb eine native asynchrone Ausführungsabstraktion, die direkt mit Structured Concurrency, Cancellation und Deadlines zusammenarbeitet.

---

## Entscheidung

NovaOS behandelt **Async/Await als native High-Level-Abstraktion für suspendierbare asynchrone Ausführung**.

`async` beschreibt eine Operation, deren Ausführung suspendiert und später fortgesetzt werden kann.

`await` wartet logisch auf deren Ergebnis, ohne zwingend einen physischen Thread zu blockieren.

```text
Async Task
    ↓
Execute
    ↓
Await Operation
    ↓
Suspend
    ↓
Thread becomes available
    ↓
Completion
    ↓
Resume Task
```

Async/Await baut auf dem nativen Task-, Execution-Scope- und Completion-Modell von NovaOS auf.

---

## Grundprinzip

```text
Await suspends work,
not necessarily a thread.

Async work remains structured.

Suspension does not break ownership.
```

---

## Async Task

Eine asynchrone Operation wird als Task innerhalb eines Execution Scope beziehungsweise einer Task Group ausgeführt.

```text
Execution Scope
      ↓
Async Task
      ↓
Await
```

Eine Async Task besitzt weiterhin:

- Owner,
- Lifecycle,
- Cancellation Context,
- Deadline,
- Resource Attribution,
- Execution Contract.

---

## Task ≠ Thread

Async/Await verstärkt die bereits definierte Trennung:

```text
Task
    ≠
Thread
```

Ein Task kann während seiner Lebensdauer:

```text
Thread A
   ↓
Suspend
   ↓
Thread B
```

verwenden.

Die logische Task-Identität bleibt erhalten.

---

## Suspension

`await` darf die aktuelle Task suspendieren, wenn das erwartete Ergebnis noch nicht verfügbar ist.

Der ausführende Thread wird dadurch für andere Arbeit verfügbar.

Ist das Ergebnis bereits vorhanden, darf die Ausführung ohne Suspension fortgesetzt werden.

---

## Continuation

Nach Abschluss der erwarteten Operation wird die Task über eine Continuation fortgesetzt.

Die Continuation gehört weiterhin zum selben strukturierten Lifecycle.

Sie ist keine neue ownerlose Hintergrundoperation.

---

## Scheduler-Integration

Der Scheduler beziehungsweise die Runtime entscheidet, auf welchem geeigneten Execution Context eine suspendierte Task fortgesetzt wird.

Async/Await darf keine feste Thread-Affinität voraussetzen.

Explizite Affinity- oder Realtime-Constraints können diese Freiheit einschränken.

---

## Kein impliziter Thread

Das Starten einer Async Task erzeugt nicht automatisch einen neuen Kernelthread.

Runtimes dürfen viele Async Tasks auf einer kleineren Zahl von Threads multiplexen.

```text
Many Async Tasks
      ↓
Runtime / Scheduler
      ↓
Worker Threads
```

---

## Blocking und Awaiting

NovaOS unterscheidet:

```text
Blocking
```

von:

```text
Awaiting
```

Blocking bindet einen Execution Context während des Wartens.

Awaiting kann die Task suspendieren und den Execution Context freigeben.

Für geeignete asynchrone Systemoperationen soll Awaiting bevorzugt werden.

---

## Native Completion

Asynchrone Kernel- und I/O-Mechanismen sollen Completion-basierte Nutzung unterstützen.

Konzeptionell:

```text
Submit Operation
      ↓
Pending
      ↓
Completion Event
      ↓
Resume Awaiting Task
```

Async/Await muss nicht durch periodisches Polling emuliert werden.

---

## Structured Concurrency

Async Tasks bleiben vollständig Bestandteil von Structured Concurrency.

Das bedeutet insbesondere:

```text
Parent Scope
      ↓
Async Child Task
```

Der Parent darf seine verpflichtende Child Task nicht vergessen, nur weil diese aktuell suspendiert ist.

---

## Cancellation

Await-Operationen müssen mit `ADR-CONCURRENCY-0003_Cancellation_Propagation` integrierbar sein.

```text
Parent Cancellation
      ↓
Async Task
      ↓
Awaited Operation
```

Kann die physische Operation nicht sofort abgebrochen werden, bleibt die sichere Completion-Behandlung bestehen.

---

## Deadlines

Async/Await muss Deadline Propagation unterstützen.

Eine erwartete Operation erhält die effektive Deadline ihres Scopes, sofern ihre Schnittstelle dies unterstützt.

```text
Scope Deadline
      ↓
Async Task
      ↓
Awaited I/O
```

Ein `await` darf das Zeitbudget seines Parents nicht neu beginnen oder verlängern.

---

## Fehler

Fehler einer erwarteten Operation werden als typisierte Task- beziehungsweise Operationsergebnisse in die Async Task zurückgeführt.

Sie bleiben Bestandteil der Structured-Concurrency-Fehlersemantik.

Ein Fehler darf nicht verloren gehen, nur weil die Operation asynchron ausgeführt wurde.

---

## Resultate

Async Tasks können typisierte Ergebnisse besitzen.

Konzeptionell:

```text
Task<T>
```

mit Ergebnissen analog zu:

```text
Success<T>
Failure<Error>
Cancelled
```

Die konkrete Sprachrepräsentation wird durch NovaLang beziehungsweise andere Runtimes definiert.

---

## Synchronisationskontext

NovaOS setzt keinen globalen impliziten UI- oder Thread-Synchronisationskontext für jede Async Task voraus.

Wenn eine Continuation auf einem bestimmten Context ausgeführt werden muss, muss dies explizit durch:

- Executor,
- Dispatcher,
- Affinity,
- Runtime Context

ausgedrückt werden.

---

## UI-Ausführung

Eine UI-Runtime kann beispielsweise verlangen:

```text
await operation
    ↓
resume on UI executor
```

Dies ist eine Eigenschaft der UI-/Runtime-Schicht und keine allgemeine Kernelregel für Async/Await.

---

## Async I/O

Native asynchrone I/O-Schnittstellen sollen bevorzugt Completion-orientiert sein.

Dies gilt insbesondere für:

- Storage,
- Netzwerk,
- IPC,
- Geräte.

Synchrone APIs dürfen darauf aufbauen, indem sie kontrolliert auf eine asynchrone Operation warten.

---

## Sync-over-Async

Unkontrolliertes synchrones Blockieren auf Async Tasks soll vermieden werden.

Insbesondere darf eine Runtime keine Deadlock-prone Semantik erzeugen, bei der:

```text
Thread waits for Task
      ↓
Task requires same blocked Thread
```

Die genaue Runtime-Policy wird separat spezifiziert.

---

## Async-over-Sync

Eine blockierende Operation wird nicht allein dadurch wirklich asynchron, dass sie auf einen Worker Thread verschoben wird.

Solche Adapter dürfen existieren, sind aber von nativer asynchroner I/O zu unterscheiden.

---

## Ressourcen

Eine suspendierte Task bleibt ihrem Execution Scope und Resource Account zugeordnet.

Suspension beendet nicht:

- Ownership,
- Capability Lifetime,
- Resource Accounting,
- Deadline,
- Cancellation Context.

---

## Zero-Copy

Async/Await muss mit Zero-Copy-Operationen kompatibel sein.

Buffer, Views und Ownership dürfen während einer Suspension nur weiterbestehen, wenn ihre Lifetime die suspendierte Operation umfasst.

---

## Structured Cleanup

Wird eine Async Task beendet oder gecancelt, müssen ihre scopegebundenen Ressourcen kontrolliert freigegeben werden.

Suspension darf Resource Cleanup nicht umgehen.

---

## Execution Contract

Async Tasks unterliegen weiterhin ihrem `Nova.ExecutionContract`.

Dazu können gehören:

- Deadline,
- Resource Budget,
- Determinism,
- Locality,
- Data Sovereignty,
- Trust Requirement.

Async/Await verändert diese Contracts nicht.

---

## Deterministic Mode

Async/Await muss mit Deterministic Mode kompatibel sein.

Deterministische Ausführung kann insbesondere kontrollieren:

- Completion-Reihenfolge,
- Continuation Scheduling,
- Task-Wakeup,
- Result Ordering.

Deterministic Mode erfordert nicht, dass alle Async Tasks seriell ausgeführt werden.

---

## Realtime

Realtime Async Tasks dürfen nur auf Mechanismen aufbauen, deren:

- Wakeup-Verhalten,
- Scheduling,
- Ressourcen,
- Completion-Latenz

mit dem jeweiligen Realtime Contract vereinbar sind.

Async/Await selbst stellt keine Realtime-Garantie dar.

---

## Kernel und Runtime

NovaOS trennt die High-Level-Abstraktion von ihrer Implementierung.

```text
NovaLang / Runtime
        ↓
Async/Await
        ↓
Task / Execution Scope
        ↓
NovaABI Async Mechanisms
        ↓
Kernel / Drivers
```

Der Kernel muss nicht jede Sprach-State-Machine kennen.

Er stellt die notwendigen Scheduling-, Wait-, Completion-, Cancellation- und I/O-Mechanismen bereit.

---

## NovaABI

NovaABI soll native Mechanismen bereitstellen, auf denen unterschiedliche Sprachruntimes Async/Await effizient implementieren können.

Die ABI soll nicht ausschließlich auf NovaLang zugeschnitten sein.

Damit können auch andere Runtimes dieselbe native Async-Infrastruktur verwenden.

---

## NovaLang

NovaLang soll `Async` / `Await` als bevorzugte Sprachabstraktion für asynchrone Operationen unterstützen.

Die Sprachsemantik muss mit Structured Concurrency übereinstimmen.

Insbesondere darf eine einfache Async-Operation nicht automatisch unstrukturiertes Fire-and-Forget erzeugen.

---

## Fire-and-Forget

Fire-and-Forget ist kein impliziter Standard von Async/Await.

Soll eine Async Task den aktuellen Scope überleben, benötigt sie einen expliziten langlebigeren Owner.

```text
Current Scope
      ↓
Explicit Ownership Transfer
      ↓
Background / Service Scope
```

---

## Introspection

Async Tasks müssen als Tasks und nicht nur als aktuell laufende Threads introspektierbar sein.

Beispiel:

```text
Task:
    document.load

State:
    Awaiting

Waiting On:
    Storage.Read

Deadline:
    82 ms

Owner:
    Document Task Group
```

Dadurch bleiben suspendierte Workloads sichtbar.

---

## Fail-Safe Defaults

Eine verlorene Continuation darf nicht dazu führen, dass eine verpflichtende Task still als erfolgreich gilt.

Unbekannte Completion-Zustände dürfen nicht als Erfolg interpretiert werden.

Eine Async Task ohne gültigen Owner darf nicht zum nativen Standardfall werden.

---

## Keine KI-Abhängigkeit

Async/Await ist grundlegende Concurrency-Infrastruktur.

Die vollständige Semantik muss ohne KI funktionieren.

Adaptive Systeme dürfen beispielsweise Worker-Zahl oder Placement optimieren, aber nicht Ownership-, Cancellation- oder Deadline-Regeln verändern.

---

## Normative Anforderungen

1. NovaOS MUSS Async/Await als native High-Level-Abstraktion für suspendierbare asynchrone Arbeit unterstützen können.
2. Async Tasks MÜSSEN Bestandteil der Structured-Concurrency-Hierarchie bleiben.
3. Eine Async Task MUSS einen eindeutigen owning Execution Scope besitzen.
4. `await` MUSS eine Task suspendieren können, ohne zwingend den ausführenden Thread zu blockieren.
5. Async Task und Kernelthread MÜSSEN getrennte Konzepte bleiben.
6. Suspension DARF die Task-Identität und Ownership NICHT aufheben.
7. Continuations MÜSSEN dem Lifecycle der ursprünglichen Task zugeordnet bleiben.
8. Das Starten einer Async Task DARF NICHT automatisch einen neuen Kernelthread erfordern.
9. NovaOS MUSS Completion-basierte asynchrone Systemoperationen unterstützen können.
10. Native Async-I/O SOLL gegenüber Worker-basiertem Blocking bevorzugt werden, wenn der Provider dies unterstützt.
11. Async Tasks MÜSSEN Cancellation Propagation unterstützen.
12. Awaited Operations SOLLEN Cancellation übernehmen können.
13. Async Tasks MÜSSEN Deadline Propagation unterstützen.
14. `await` DARF eine Parent Deadline NICHT verlängern oder neu starten.
15. Fehler asynchroner Operationen DÜRFEN NICHT verloren gehen.
16. Async Tasks MÜSSEN typisierte Resultat-, Fehler- und Cancellation-Zustände unterstützen können.
17. Continuations DÜRFEN NICHT grundsätzlich an den ursprünglich ausführenden Thread gebunden sein.
18. Explizite Affinity- und Executor-Constraints MÜSSEN eine erforderliche Continuation-Location festlegen können.
19. Suspension DARF Resource Accounting und Execution Contracts NICHT aufheben.
20. Async/Await MUSS mit Zero-Copy-Lifetime-Regeln kompatibel sein.
21. Async Tasks MÜSSEN kontrolliertes Resource Cleanup unterstützen.
22. Fire-and-Forget DARF NICHT die implizite Standardsemantik von Async Task Creation sein.
23. Länger lebende Async Tasks MÜSSEN explizite Ownership besitzen.
24. NovaABI MUSS sprachunabhängige Mechanismen für effiziente Async-Runtimes bereitstellen können.
25. Der Kernel DARF NICHT von NovaLang-spezifischen Async-State-Machines abhängig sein.
26. Async/Await MUSS mit Deterministic Mode integrierbar sein.
27. Async/Await MUSS mit Realtime Execution Contracts integrierbar sein.
28. Suspendierte Async Tasks und ihre Wait-Ursachen MÜSSEN introspektierbar sein.
29. Unbekannte Completion-Zustände DÜRFEN NICHT still als erfolgreicher Task-Abschluss gelten.
30. Die grundlegende Async/Await-Infrastruktur MUSS vollständig ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- skalierbare asynchrone Anwendungen,
- weniger blockierte Kernelthreads,
- natürliche Integration mit Structured Concurrency,
- einheitliche Cancellation- und Deadline-Semantik,
- gute Grundlage für NovaLang,
- effiziente Storage-, Netzwerk- und IPC-APIs,
- sprachübergreifend nutzbare native Async-Infrastruktur.

### Negative Konsequenzen

- Kernel, Treiber und Runtimes benötigen Completion-fähige Schnittstellen,
- Debugging muss Tasks statt nur Threads darstellen,
- Buffer- und Resource-Lifetimes über Suspensionen müssen explizit korrekt sein,
- blockierende Legacy-APIs benötigen Adapter.

---

## Verworfene Alternativen

### Ein Thread pro Async Operation

Verworfen.

Dies skaliert bei großen Mengen wartender Operationen schlecht.

### Async/Await ausschließlich als NovaLang-Feature

Verworfen.

Die zugrunde liegende Async-Infrastruktur soll systemweit und sprachunabhängig nutzbar sein.

### Await blockiert grundsätzlich den aktuellen Thread

Verworfen.

Dies würde den wesentlichen Skalierungsvorteil von Async/Await aufheben.

### Jede Continuation kehrt automatisch auf denselben Thread zurück

Verworfen.

Dies erzeugt unnötige Affinität und erschwert effizientes Scheduling.

### Async bedeutet automatisch Fire-and-Forget

Verworfen.

Dies widerspricht Structured Concurrency.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-ASYNC-0001`
- `NPSPEC-CONCURRENCY-AWAIT-0001`
- `NPSPEC-CONCURRENCY-CONTINUATION-0001`
- `NPSPEC-CONCURRENCY-ASYNC-IO-0001`
- `NPSPEC-CONCURRENCY-ASYNC-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-ASYNC-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-ASYNC-ABI-0001`
- `NPSPEC-CONCURRENCY-ASYNC-TEST-0001`

---

## Ergebnis

NovaOS behandelt Async/Await als native Abstraktion über Tasks, Execution Scopes und Completion-basierte Systemoperationen.

```text
Execution Scope
      ↓
Async Task
      ↓
Await
      ↓
Suspend
      ↓
Completion
      ↓
Resume
```

Die Task bleibt währenddessen Teil derselben strukturierten Ausführung und behält:

```text
ownership
cancellation
deadline
resources
execution contract
```

Die zentrale Architekturregel lautet:

```text
Await suspends the task,
not necessarily the thread.

Async work remains structured.

Continuations preserve ownership.

Fire-and-forget is explicit, never implicit.
```