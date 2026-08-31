# ADR-CONCURRENCY-0001 – Structured Concurrency

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Tasks / Threads / Execution Scopes / Lifecycle / Scheduling / Ressourcen

## Kontext

NovaOS verwendet Execution Domains als grundlegende Einheit kontrollierter Ausführung.

Innerhalb solcher Domains entstehen zahlreiche parallele oder asynchrone Aktivitäten:

- Threads,
- Tasks,
- Worker,
- I/O-Operationen,
- Timer,
- RPC-Aufrufe,
- Deferred Work,
- GPU-/NPU-Jobs,
- Hintergrundoperationen,
- parallele Pipelines.

In klassischen Systemen entstehen dabei häufig unabhängige oder schwer nachvollziehbare Lebenszyklen.

Typische Probleme sind:

- verwaiste Threads,
- vergessene Tasks,
- unkontrollierte Hintergrundarbeit,
- fehlende Cancellation,
- unklare Fehlerweitergabe,
- nicht deterministisches Shutdown-Verhalten,
- Ressourcen, deren Besitzer nicht mehr existieren,
- Workloads, die ihre erzeugende Operation überleben,
- unübersichtliche Abhängigkeiten zwischen Parent und Child Tasks.

NovaOS benötigt deshalb ein systemweit einheitliches Concurrency-Modell, das Lebensdauer, Fehler, Ressourcen und Cancellation explizit strukturiert.

---

## Entscheidung

NovaOS verwendet **Structured Concurrency als systemweites Standardmodell für nebenläufige Ausführung**.

Nebenläufige Arbeit wird grundsätzlich innerhalb expliziter `Execution Scopes` erzeugt.

Das Modell lautet:

```text
Execution Domain
      ↓
Root Execution Scope
      ↓
Child Scope
      ├── Task A
      ├── Task B
      └── Child Scope
             ├── Task C
             └── Task D
```

Ein Scope besitzt eine klar definierte Lebensdauer.

Von ihm gestartete Child-Aktivitäten müssen grundsätzlich vor Abschluss des Scopes:

```text
complete
cancel
fail
or transfer ownership explicitly
```

werden.

---

## Grundprinzip

```text
Concurrency must have structure.

Every task has an owner.

Every lifetime has a scope.

Every cancellation has a path.
```

---

## Execution Scope

Das zentrale Modell ist der `Execution Scope`.

Konzeptionell:

```text
ExecutionScope {
    id
    parent
    state

    cancellation
    deadline

    resourceBudget
    schedulingPolicy
    executionContract

    children
    failurePolicy
}
```

Ein Scope ist keine reine syntaktische Sprachabstraktion.

Er ist ein systemweit relevantes Ausführungs- und Lifecycle-Konzept.

---

## Root Execution Scope

Jedes Execution Domain besitzt mindestens einen Root Execution Scope.

```text
Execution Domain
      ↓
Root Scope
```

Der Root Scope definiert die oberste Lebensdauer der zum Domain gehörenden strukturierten Arbeit.

Bei Domain-Beendigung muss dieser Scope beendet werden.

---

## Hierarchie

Execution Scopes bilden eine Hierarchie.

```text
Root
 ├── Scope A
 │    ├── Task A1
 │    └── Task A2
 │
 └── Scope B
      └── Scope C
           └── Task C1
```

Diese Hierarchie bildet:

- Ownership,
- Cancellation,
- Fehlerweitergabe,
- Ressourcenvererbung,
- Lifecycle

ab.

---

## Ownership

Jede strukturierte Task gehört genau einem Scope.

```text
Task
    ↓
Owning Execution Scope
```

Ein Task darf nicht ohne bekannten Owner im System existieren.

---

## Child Lifecycle

Ein Child Scope oder Task darf seinen Parent Scope grundsätzlich nicht unkontrolliert überleben.

Vor Ende des Parent müssen seine Children:

```text
Complete
Cancel
Join
or
Explicitly Detach
```

werden.

---

## Kein implizites Detached Work

Detached Work ist kein Standardfall.

Eine Operation wie:

```text
StartBackgroundTask()
```

darf nicht automatisch eine unbegrenzt unabhängige Task erzeugen.

Wenn unabhängige Arbeit gewünscht ist, muss Ownership explizit übertragen werden.

---

## Ownership Transfer

Ein Task kann kontrolliert in einen anderen Scope übertragen werden, sofern:

- der Zielscope existiert,
- die Operation erlaubt ist,
- Resource- und Security-Constraints eingehalten werden.

Konzeptionell:

```text
Scope A
   ↓
Task
   ↓ transfer
Scope B
```

Nach erfolgreichem Transfer besitzt Scope A keine Lifecycle-Verantwortung mehr für diese Task.

---

## Spawn von Tasks

Tasks werden innerhalb eines Scope gestartet.

Konzeptionell:

```text
scope.Spawn(task)
```

oder eine semantisch äquivalente API.

Der neue Task erbt definierte Eigenschaften seines Scopes.

---

## Vererbung

Ein Child kann standardmäßig Teile folgender Eigenschaften ableiten:

```text
Cancellation Context
Deadline
Resource Budget
Scheduling Constraints
Execution Contract
Security Context
Data Policy
```

Diese Vererbung darf durch strengere Child-Constraints eingeschränkt werden.

---

## Keine Constraint-Eskalation

Ein Child darf harte Parent-Grenzen nicht erweitern.

Beispiel:

```text
Parent Deadline:
    100 ms

Child Deadline:
    500 ms
```

ist als harte Deadline nicht sinnvoll.

Der effektive Wert ist höchstens durch die Parent-Grenze bestimmt.

Analog gilt für Ressourcen und Authority.

---

## Cancellation

Cancellation ist ein First-Class-Konzept.

Ein Scope besitzt einen Cancellation State.

Konzeptionell:

```text
Active
    ↓
CancelRequested
    ↓
Cancelling
    ↓
Cancelled
```

Cancellation wird hierarchisch an Child Scopes und Tasks propagiert.

---

## Cancellation Propagation

```text
Parent Cancel
    ↓
Child Scope A
    ↓
Task A1

    ↓
Child Scope B
    ↓
Task B1
```

Children dürfen die Parent-Cancellation nicht dauerhaft ignorieren.

---

## Cooperative Cancellation

Der bevorzugte Weg ist cooperative Cancellation.

Tasks prüfen regelmäßig einen Cancellation State beziehungsweise erhalten entsprechende Cancellation Points.

Typische Cancellation Points sind:

```text
await
blocking wait
I/O
sleep
channel receive
resource acquisition
```

---

## Forced Cancellation

Nicht jede Task kann kooperativ beendet werden.

NovaOS darf abhängig von Ausführungskontext und Policy stärkere Abbruchmechanismen besitzen.

Diese müssen unterscheiden zwischen:

```text
request cancellation
force stop
terminate process/domain
```

Ein erzwungener Threadabbruch innerhalb beliebiger kritischer Zustände darf nicht leichtfertig als universelles Standardverfahren verwendet werden.

---

## Cancellation Safety

APIs müssen definieren, ob eine Operation:

- cancel-safe,
- deferred-cancel,
- non-cancellable

ist.

Beispiel:

```text
Transaction Commit
```

kann einen kurzen nicht unterbrechbaren Abschnitt benötigen.

Solche Bereiche müssen begrenzt und explizit sein.

---

## Deadlines

Execution Scopes können Deadlines besitzen.

Beispiel:

```text
deadline = T
```

Alle Child-Aktivitäten müssen diese Deadline berücksichtigen.

Ein Child darf eine strengere Deadline wählen.

---

## Deadline Propagation

Die effektive Child-Deadline lautet konzeptionell:

```text
effectiveDeadline =
    min(parentDeadline, childDeadline)
```

falls beide definiert sind.

---

## Timeout vs Deadline

NovaOS unterscheidet:

```text
Timeout
    = relative duration

Deadline
    = absolute temporal constraint
```

Intern sollen Deadlines bevorzugt werden, da verschachtelte relative Timeouts sonst Zeitbudgets unbeabsichtigt verlängern können.

---

## Resource Budgets

Execution Scopes können Ressourcenbudgets besitzen.

Beispiel:

```text
ScopeBudget {
    cpu
    memory
    io
    network
    gpu
}
```

Children teilen beziehungsweise erhalten Subbudgets dieses Parent-Budgets.

---

## Hierarchisches Accounting

Ressourcenverbrauch wird hierarchisch zurechenbar.

```text
Execution Domain
      ↓
Scope A
      ↓
Task A1
```

Der Verbrauch von Task A1 zählt mindestens zu:

```text
Task A1
Scope A
Execution Domain
```

---

## Budget Delegation

Ein Parent Scope kann einem Child ein Teilbudget geben.

Beispiel:

```text
Parent:
    Memory 1 GiB

Child A:
    256 MiB

Child B:
    512 MiB
```

Restliche Ressourcen verbleiben beim Parent beziehungsweise anderen Children.

---

## Budget Exhaustion

Bei Überschreiten eines Scope-Budgets können definierte Reaktionen erfolgen:

```text
Throttle
Deny
Cancel
Fail Scope
Escalate
```

Die Policy hängt von Ressource und Execution Contract ab.

---

## Scheduling

Ein Scope kann Scheduling Constraints besitzen.

Beispiele:

```text
priority class
CPU set
latency class
deadline
weight
```

Child Tasks dürfen diese verfeinern, harte Grenzen jedoch nicht überschreiten.

---

## Scheduler Integration

Der Scheduler muss die Scope-Struktur nicht für jede einzelne Entscheidung vollständig traversieren.

Er muss jedoch die daraus resultierenden effektiven Scheduling Constraints berücksichtigen können.

Structured Concurrency ist ein semantisches Modell, keine Vorschrift für einen ineffizienten Scheduler.

---

## Fehlerweitergabe

Fehler werden strukturiert entlang der Scope-Hierarchie behandelt.

Ein Child Failure besitzt einen klaren Owner.

```text
Task Failure
    ↓
Owning Scope
    ↓
Failure Policy
```

Der Fehler darf nicht ohne bekannte Behandlung im System verschwinden.

---

## Failure Policy

Ein Scope besitzt eine explizite Failure Policy.

Mögliche Strategien:

```text
FailFast
Collect
Isolate
Supervise
IgnoreExplicitly
```

`IgnoreExplicitly` bedeutet, dass das Ignorieren bewusst und sichtbar konfiguriert wurde.

---

## FailFast

Bei `FailFast` führt der Fehler eines relevanten Childs zur Cancellation der übrigen Children.

```text
Task A fails
    ↓
Cancel B
Cancel C
    ↓
Scope fails
```

Dies eignet sich für parallel ausgeführte Teile derselben Gesamtoperation.

---

## Collect

Bei `Collect` dürfen mehrere Children bis zu einem definierten Join Point weiterlaufen.

Anschließend werden mehrere Fehler gemeinsam zurückgegeben.

Dies ist beispielsweise für unabhängige parallele Berechnungen sinnvoll.

---

## Isolate

Bei `Isolate` bleibt ein Child-Fehler auf seinen Teilbereich begrenzt.

Dies ist nur sinnvoll, wenn die Child-Arbeit semantisch unabhängig ist.

---

## Supervise

Bei langlebigen Worker-Scopes kann eine Supervision Policy verwendet werden.

Dies integriert Structured Concurrency mit:

`ADR-PROCESS-0005_Systemweite_Process_Supervision`.

---

## Task Result

Eine Task besitzt ein typisiertes Ergebnis.

Konzeptionell:

```text
TaskResult<T> =
    Success<T>
    Failure<Error>
    Cancelled
```

Cancellation ist kein unspezifizierter Ausnahmezustand.

---

## Join

Ein Scope muss auf seine strukturierten Children warten beziehungsweise sie joinen können.

```text
Spawn children
    ↓
Work
    ↓
Join
    ↓
Scope complete
```

Ein Scope gilt erst als vollständig abgeschlossen, wenn seine verpflichtenden Children abgeschlossen sind.

---

## Join Semantik

Join darf nicht bedeuten:

```text
busy wait
```

Der Scheduler kann den wartenden Execution Context blockieren beziehungsweise suspendieren, bis relevante Children abgeschlossen sind.

---

## Async/Await

Sprachruntimes können Structured Concurrency über `async` / `await` abbilden.

NovaOS schreibt keine bestimmte Programmiersprachen-Syntax vor.

NovaLang kann beispielsweise eine native Sprachabstraktion dafür bereitstellen.

Die Systemsemantik bleibt jedoch unabhängig von der Oberflächensyntax.

---

## Threads

Threads bleiben grundlegende ausführbare Scheduling-Kontexte.

Structured Concurrency ersetzt Threads nicht.

Das Modell lautet:

```text
Execution Scope
    ↓
Tasks
    ↓
Execution Contexts / Threads
```

Eine Runtime kann viele Tasks auf weniger Threads multiplexen.

---

## Task ≠ Thread

Eine Task ist eine logische Arbeitseinheit.

Ein Thread ist ein ausführbarer Kontext.

Daher gilt:

```text
Task
    ≠
Thread
```

Ein Task kann:

- auf einem Thread laufen,
- Threads wechseln,
- suspendiert sein,
- später auf anderem Thread fortgesetzt werden.

---

## Kernel Work

Auch Kernelarbeit soll, soweit sinnvoll, einem Execution Scope zugeordnet werden.

Beispiel:

```text
Process Request
      ↓
Kernel Async I/O
      ↓
Deferred Worker
```

Die Operation behält ihre logische Scope- und Ressourcenverantwortung.

---

## Deferred Work

Deferred Work darf nicht automatisch seine ursprüngliche Ownership verlieren.

Ein Interrupt Handler kann beispielsweise Arbeit in einen Deferred Scope übergeben.

```text
IRQ
 ↓
Capture work
 ↓
Deferred Execution Scope
```

Lifecycle und Resource Attribution bleiben explizit.

---

## I/O

Asynchrone I/O-Operationen gehören zu einem Scope.

Wird der Scope beendet, muss die Operation abhängig von ihrer Semantik:

```text
cancel
drain
complete
or transfer
```

werden.

---

## Non-Cancellable I/O

Ein Hardware-I/O-Vorgang kann möglicherweise nicht physisch sofort abgebrochen werden.

Dann muss zwischen:

```text
logical cancellation
```

und:

```text
physical operation completion
```

unterschieden werden.

Das Ergebnis darf nach Cancellation nicht unkontrolliert in bereits zerstörten State geschrieben werden.

---

## Completion Ownership

Auch nach logischer Cancellation muss eindeutig sein, wer eine verspätete Completion verarbeitet und Ressourcen freigibt.

Es darf keine Completion ohne gültige Lifetime geben.

---

## IPC

Request-basierte IPC kann an Execution Scopes gebunden werden.

Beispiel:

```text
Client Scope
    ↓
RPC Request
    ↓
Server Child Scope
```

Cancellation oder Deadline des Requests kann semantisch an die Serverarbeit weitergegeben werden.

---

## Distributed Cancellation

Bei Remote-Ausführung kann Cancellation über Prozess- oder Hostgrenzen propagiert werden.

Diese Propagation ist best effort, sofern kein stärkerer Contract besteht.

Ein lokales Scope darf trotzdem kontrolliert beenden, auch wenn ein Remote Peer nicht erreichbar ist.

---

## Data/Object Pipelines

Structured Concurrency integriert sich in das einheitliche Daten-/Objekt-Pipelining.

Beispiel:

```text
Pipeline Scope
    ├── Decode
    ├── Transform
    └── Encode
```

Wenn eine notwendige Stage fehlschlägt, kann die gesamte Pipeline kontrolliert beendet werden.

---

## Zero-Copy Buffer Lifetime

Zero-Copy erfordert besonders klare Lifetimes.

Ein Buffer darf nur solange verwendet werden, wie sein Ownership Contract gültig ist.

Scope-Lifetimes helfen dabei:

```text
Scope owns buffer
    ↓
children may borrow
    ↓
scope ends
    ↓
buffer release
```

---

## Borrowed Resources

Children dürfen Ressourcen vom Parent leihen.

Eine geliehene Ressource darf den Parent Scope nicht überleben, sofern Ownership nicht explizit übertragen wurde.

---

## Structured Resource Cleanup

Scope-Ende ist ein natürlicher Cleanup-Punkt.

Dabei werden scopegebundene Ressourcen:

- freigegeben,
- geschlossen,
- zurückgegeben,
- revokiert

entsprechend ihrer Lifecycle-Semantik.

---

## Scope Guards

Sprach- und Runtime-Abstraktionen dürfen Scope Guards beziehungsweise automatische Cleanup-Mechanismen verwenden.

Die Systemarchitektur setzt jedoch nicht voraus, dass jede Sprache RAII verwendet.

---

## Execution Contract

Ein Scope kann einen `Nova.ExecutionContract` besitzen oder von seinem Parent ableiten.

Darin können unter anderem enthalten sein:

- Determinism Requirement,
- Deadline,
- Resource Budget,
- Trust Requirement,
- Data-Sovereignty Policy,
- Locality Constraint,
- Algorithm Policy.

Children dürfen Hard Constraints nicht verletzen.

---

## Deterministic Mode

Structured Concurrency ist eine wichtige Grundlage für Deterministic Mode.

Da Child-Arbeit explizit bekannt ist, kann NovaOS unter anderem:

- Spawn-Reihenfolge,
- Join-Punkte,
- Schedulinggrenzen,
- Ressourcen,
- Cancellation

kontrollierter behandeln.

Determinismus bedeutet jedoch nicht zwingend rein serielle Ausführung.

---

## Deterministische Parallelität

Parallelität kann deterministisch sein, wenn:

- Work Partitioning definiert ist,
- Synchronisationspunkte stabil sind,
- Ergebnisreduktion definiert ist,
- Race Conditions ausgeschlossen beziehungsweise kontrolliert sind.

Structured Concurrency liefert dafür die Lifecycle-Struktur.

---

## Realtime

Realtime Scopes können harte Deadlines und Ressourcenreservierungen besitzen.

Child Tasks dürfen diese Constraints nicht verwässern.

Bei erkennbarer Deadline-Verletzung kann die Policy:

```text
cancel optional work
degrade quality
fail
```

entscheiden.

Hard Safety- und Correctness-Anforderungen bleiben bestehen.

---

## Optional Work

Scopes können unterscheiden zwischen:

```text
Required Child
Optional Child
```

Bei Ressourcen- oder Zeitdruck kann optionale Arbeit zuerst beendet werden.

Dies unterstützt Graceful Degradation.

---

## Background Work

Länger laufende Hintergrundarbeit wird nicht unstrukturiert detached.

Stattdessen erhält sie einen passenden langlebigen Owner.

Beispiele:

```text
Application Background Scope
Session Scope
System Service Scope
Supervisor Scope
```

---

## Application Lifecycle

Eine Anwendung kann ihre strukturierten Aufgaben unter einem Application Scope organisieren.

```text
Application
    ↓
Application Scope
    ├── UI Scope
    ├── Document Scope
    └── Background Scope
```

Beim Beenden der Anwendung können alle zugehörigen Scopes kontrolliert beendet werden.

---

## Request Scope

Server und Services können pro Request einen Child Scope erzeugen.

```text
Service
   ↓
Request Scope
   ├── Database Query
   ├── File Read
   └── Remote Call
```

Endet der Request, kann verbleibende Request-Arbeit automatisch beendet werden.

---

## Driver Work

Treiber können Scope-basierte Operationen verwenden.

Beispiel:

```text
Device Operation Scope
      ├── DMA
      ├── Wait IRQ
      └── Deferred Completion
```

Bei Device Hot-Unplug kann der entsprechende Scope kontrolliert gecancelt und drainiert werden.

---

## Hotplug

Structured Concurrency integriert sich mit dem deklarativen Device Lifecycle.

Beispiel:

```text
Device Removing
    ↓
Cancel Device Scopes
    ↓
Drain
    ↓
Revoke DMA / IRQ
    ↓
Remove
```

Damit werden aktive Operationen nicht unabhängig vom Device Lifecycle weitergeführt.

---

## Checkpointing

Checkpointbare Execution Domains benötigen bekannte aktive Arbeit.

Structured Concurrency vereinfacht die Bestimmung:

- welche Tasks existieren,
- welche Scopes quiesziert werden müssen,
- welche Operationen noch aktiv sind.

Ein Checkpoint kann einen Scope gezielt quieszieren.

---

## Migration

Migrierbare Workloads profitieren ebenfalls von klaren Scope-Lifetimes.

Vor Migration kann NovaOS:

```text
quiesce
cancel optional work
drain required work
capture remaining state
```

---

## Hot Replacement

Bei Hot Replacement können relevante Service-Scopes gequiesziert werden.

Neue Requests werden beispielsweise an den Candidate geleitet, während alte Scopes kontrolliert auslaufen.

---

## Prozessbeendigung

Bei Prozessbeendigung gilt:

```text
Process Termination
      ↓
Cancel Root Scope
      ↓
Propagate Cancellation
      ↓
Drain / Join
      ↓
Release Resources
      ↓
Terminate
```

Dies verhindert verwaiste prozessinterne Arbeit.

---

## Scope State

Ein Execution Scope besitzt mindestens Zustände analog zu:

```text
Created
Active
Cancelling
Joining
Completed
Failed
Cancelled
```

Die genaue interne Repräsentation wird separat spezifiziert.

---

## Scope Identity

Scopes erhalten stabile Laufzeitidentitäten für:

- Introspection,
- Accounting,
- Tracing,
- Debugging.

Diese IDs sind keine globale dauerhafte Sicherheitsidentität.

---

## Architecture Introspection

Die Scope-Hierarchie muss introspektierbar sein.

Beispiel:

```text
Process:
    nova.editor

Root Scope
    ├── UI Scope
    │    └── Render Task
    │
    └── Document Scope
         ├── Save Task
         └── Thumbnail Task
```

Zusätzlich können sichtbar sein:

```text
state
deadline
resource usage
failure
cancellation state
```

soweit Berechtigungen dies erlauben.

---

## Tracing

Tracing darf Scope-Beziehungen nutzen.

Ein Trace kann dadurch Ursache und Zugehörigkeit einer asynchronen Operation nachvollziehen.

```text
Request
   ↓
Task
   ↓
Async I/O
   ↓
Completion
```

bleibt als zusammenhängende Operation erkennbar.

---

## Debugging

Debugger sollen Child Scopes, Tasks und ihre Parent-Beziehungen darstellen können.

Dies ist insbesondere bei stark asynchronen Programmen wichtiger als eine reine Threadliste.

---

## Audit

Nicht jede Task-Erzeugung muss dauerhaft sicherheitsrelevant auditiert werden.

Sicherheitsrelevante Scope-Operationen können jedoch auditierbar sein.

Beispiele:

```text
privileged detached execution
authority transfer
cross-domain cancellation
resource limit override
```

---

## Fail-Safe Defaults

Wenn die Ownership einer Task unklar ist, darf sie nicht als unbegrenzt detached weiterlaufen.

Wenn ein Scope endet und eine Child-Task nicht sauber abschließbar ist, muss eine definierte Eskalation erfolgen.

Unbekannte Child-Zustände dürfen den Lifecycle nicht still blockieren.

---

## Graceful Degradation

Bei Zeit- oder Ressourcenmangel kann der Scope optionale Children kontrolliert abbrechen.

Beispiel:

```text
Image Processing Scope
    ├── Required: Decode
    ├── Required: Resize
    └── Optional: AI Enhancement
```

Unter Ressourcenknappheit kann zuerst:

```text
AI Enhancement
```

entfallen.

---

## Priority Inversion

Scope-Hierarchie allein verhindert keine Priority Inversion.

Synchronisationsmechanismen müssen entsprechende Verfahren wie Priority Inheritance oder andere geeignete Policies separat unterstützen.

Structured Concurrency liefert lediglich die Ownership- und Lifecycle-Struktur.

---

## Synchronisation

Structured Concurrency ersetzt keine Synchronisationsprimitive.

Weiterhin benötigt werden beispielsweise:

- Mutex,
- Semaphore,
- Event,
- Channel,
- Atomic Operations,
- Wait Sets.

Diese müssen jedoch mit Cancellation und Scope-Lifetimes kompatibel sein.

---

## Locks und Cancellation

Eine Cancellation darf nicht dazu führen, dass gelockte Ressourcen dauerhaft gesperrt bleiben.

Runtime- und System-APIs müssen Cleanup beziehungsweise definierte kritische Bereiche unterstützen.

---

## Channels

Channels und Message Queues können scopegebundene Lifetimes besitzen.

Wenn der owning Scope endet, muss klar sein, ob ein Channel:

```text
close
drain
transfer
```

wird.

---

## Detached System Services

Langlebige Systemdienste sind nicht „detached“ im Sinne fehlender Ownership.

Sie gehören beispielsweise einem:

```text
System Supervisor Scope
```

und besitzen dadurch weiterhin einen kontrollierten Lifecycle.

---

## Cross-Domain Work

Ein Execution Domain kann Arbeit in einem anderen Domain anstoßen.

Die lokale Task-Ownership wird dadurch nicht automatisch auf den Remote-Prozess übertragen.

Stattdessen existieren zwei verbundene Scopes beziehungsweise Request-Lifetimes.

```text
Client Scope
    ↓ request
Server Scope
```

Die jeweilige Isolation bleibt erhalten.

---

## Capability Security

Ein Child erhält nicht automatisch neue Capabilities allein durch seine Task-Erzeugung.

Authority ist vom Domain beziehungsweise expliziter Capability Delegation abhängig.

Scope-Struktur ist keine Privilege-Struktur.

---

## Scoped Capabilities

Capabilities dürfen optional an einen Execution Scope gebunden werden.

Beispiel:

```text
Capability:
    TemporaryFile.Write

Lifetime:
    Request Scope
```

Nach Ende des Scopes wird die Capability ungültig beziehungsweise revokiert.

---

## Scoped Delegation

Ein Parent darf einer Child-Operation eine temporär eingeschränkte Capability delegieren.

Beispiel:

```text
Parent:
    File.Read.All

Child:
    File.Read(document X)
```

Die Delegation endet mit dem Child Scope.

---

## Data Sovereignty

Data Policies können an Scope-Grenzen verfeinert werden.

Ein Child darf Data-Sovereignty-Constraints seines Parents nicht abschwächen.

Beispiel:

```text
Parent:
    LocalOnly

Child:
    RemoteAllowed
```

ist nicht zulässig.

---

## Locality

Scopes können Locality Preferences besitzen.

Beispielsweise:

```text
prefer same NUMA node as data
prefer GPU-local execution
```

Dies sind Scheduling-/Placement-Constraints und keine Änderung der Structured-Concurrency-Semantik.

---

## Adaptive Scheduling

NovaOS darf Task Placement und andere Soft Policies adaptiv optimieren.

Prediction Error kann zur Modellkorrektur verwendet werden.

Beispiel:

```text
Prediction:
    worker will finish faster on CPU 3

Observed:
    CPU 6 performed better

Prediction Error
    ↓
update placement model
```

Die Scope-Hierarchie bleibt unabhängig davon unverändert.

---

## Keine KI-Abhängigkeit

Structured Concurrency ist grundlegende Systemsemantik.

Sie muss vollständig ohne KI funktionieren.

KI darf optional Scheduling, Ressourcenzuteilung oder Task Placement optimieren.

Sie darf nicht bestimmen, ob eine unbekannte verwaiste Task ohne Owner weiterlaufen darf.

---

## Native ABI

Die native NovaOS-ABI muss Mechanismen bereitstellen, auf denen Runtime- und Sprachabstraktionen für Structured Concurrency aufgebaut werden können.

Dies kann umfassen:

- Cancellation Tokens,
- Wait/Join,
- Task-/Thread-Lifecycle,
- Deadlines,
- Resource Scope IDs.

Nicht jede High-Level-Abstraktion muss direkt als einzelner Syscall implementiert werden.

---

## NovaLang

NovaLang soll Structured Concurrency als bevorzugtes High-Level-Concurrency-Modell unterstützen.

Die konkrete Sprachsyntax wird in NovaLang-ADRs/NPSPECs spezifiziert.

Die Sprache soll vermeiden, unstrukturierte Fire-and-Forget-Arbeit zum bequemsten Standard zu machen.

---

## Kompatibilität

POSIX-, Win32- und andere APIs dürfen unstrukturiertere Concurrency-Semantik emulieren.

Diese Kompatibilitätsmodelle dürfen die native NovaOS-Architektur jedoch nicht bestimmen.

Ein klassischer Thread, der über eine Compatibility API erzeugt wird, muss intern trotzdem einem definierten Domain- und Lifecycle-Kontext zugeordnet werden.

---

## Normative Anforderungen

1. NovaOS MUSS Structured Concurrency als natives Standardmodell für nebenläufige Arbeit verwenden.
2. Jedes Execution Domain MUSS mindestens einen Root Execution Scope besitzen.
3. Strukturierte Tasks MÜSSEN genau einem owning Scope zugeordnet sein.
4. Child Scopes MÜSSEN hierarchisch einem Parent zugeordnet werden können.
5. Child-Arbeit DARF einen Parent Scope NICHT unkontrolliert überleben.
6. Detached Work MUSS explizite Ownership besitzen.
7. Ownership Transfer zwischen Scopes MUSS explizit erfolgen.
8. Cancellation MUSS hierarchisch propagierbar sein.
9. Cooperative Cancellation SOLL der bevorzugte Abbruchmechanismus sein.
10. Nicht cancelbare Bereiche MÜSSEN explizit und begrenzt sein.
11. Deadlines MÜSSEN von Parent zu Child propagierbar sein.
12. Child Deadlines DÜRFEN Parent Deadlines NICHT erweitern.
13. Ressourcenbudgets MÜSSEN hierarchisch auf Scopes und Tasks abbildbar sein.
14. Child Scopes DÜRFEN harte Parent-Ressourcengrenzen NICHT überschreiten.
15. Ressourcenverbrauch MUSS entlang der Scope-Hierarchie zurechenbar sein.
16. Fehler MÜSSEN einem owning Scope eindeutig zugeordnet werden.
17. Scopes MÜSSEN explizite Failure Policies besitzen können.
18. Cancellation MUSS als eigener Task-Ausgangszustand unterscheidbar sein.
19. Scope Completion MUSS verpflichtende Child-Arbeit berücksichtigen.
20. Asynchrone I/O- und Deferred-Work-Operationen SOLLEN ihre ursprüngliche Scope-Ownership behalten.
21. Verspätete Completions DÜRFEN NICHT auf bereits ungültige Scope-Ressourcen zugreifen.
22. Scoped Capabilities und temporäre Delegation MÜSSEN unterstützt werden können.
23. Data-Sovereignty-Constraints DÜRFEN durch Child Scopes NICHT abgeschwächt werden.
24. Structured Concurrency MUSS mit Process Termination, Supervision, Checkpointing und Migration integrierbar sein.
25. Realtime- und Determinism-Constraints MÜSSEN über Scope-Hierarchien erhalten bleiben.
26. Optionale Arbeit MUSS für Graceful Degradation explizit kennzeichenbar sein.
27. Scope-Hierarchie und relevante Zustände MÜSSEN introspektierbar sein.
28. Kompatibilitäts-Threads und -Tasks MÜSSEN intern eine definierte Lifecycle-Ownership erhalten.
29. Unbekannte beziehungsweise ownerlose Hintergrundarbeit DARF NICHT der native Standardfall sein.
30. Die vollständige grundlegende Structured-Concurrency-Funktionalität MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- deutlich weniger verwaiste Arbeit,
- klarer Prozess- und Task-Lifecycle,
- systemweite Cancellation-Semantik,
- saubere Fehlerweitergabe,
- hierarchische Ressourcenbudgets,
- bessere Integration mit Realtime und Determinism,
- Checkpoint und Migration werden einfacher,
- Hotplug und Prozessbeendigung können aktive Arbeit kontrolliert stoppen,
- bessere Observability und Debugbarkeit,
- gute Grundlage für NovaLang-Async-APIs.

### Negative Konsequenzen

- Runtimes und APIs müssen Scope-Semantik berücksichtigen,
- bestehende unstrukturierte Concurrency-Modelle benötigen Adapter,
- Ownership Transfer und lange Hintergrundarbeit müssen expliziter modelliert werden,
- Cancellation-Sicherheit erhöht Anforderungen an System- und Library-APIs.

---

## Verworfene Alternativen

### Unstrukturierte Threads als natives Standardmodell

Verworfen.

Threads ohne übergeordnete Lifecycle-Struktur erschweren Cancellation, Cleanup und Fehlerbehandlung.

### Fire-and-Forget als Standard

Verworfen.

Arbeit ohne definierten Owner führt zu unklaren Lifetimes und Ressourcenlecks.

### Nur sprachspezifische Structured Concurrency

Verworfen.

NovaOS benötigt die Semantik auch für Kernelarbeit, I/O, Treiber und systemweite Execution Contracts.

### Cancellation ausschließlich durch Thread-Kill

Verworfen.

Erzwungene Beendigung kann kritische Zustände und Ressourcen inkonsistent hinterlassen.

### Jeder Task besitzt ein unabhängiges Ressourcenbudget ohne Hierarchie

Verworfen.

Dies würde systemweite Accounting- und Resource-Economy-Strukturen unnötig fragmentieren.

### Parent-Ende beendet Children immer sofort hart

Verworfen.

Viele Operationen benötigen kontrolliertes Cancel, Drain oder Join.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-PROCESS-0007_Migrierbare_Workloads`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0009_Einheitliches_Daten_Objekt_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-SCOPE-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-JOIN-0001`
- `NPSPEC-CONCURRENCY-FAILURE-0001`
- `NPSPEC-CONCURRENCY-RESOURCE-0001`
- `NPSPEC-CONCURRENCY-OWNERSHIP-0001`
- `NPSPEC-CONCURRENCY-DETACHED-0001`
- `NPSPEC-CONCURRENCY-IPC-0001`
- `NPSPEC-CONCURRENCY-IO-0001`
- `NPSPEC-CONCURRENCY-REALTIME-0001`
- `NPSPEC-CONCURRENCY-DETERMINISTIC-0001`
- `NPSPEC-CONCURRENCY-INTROSPECTION-0001`
- `NPSPEC-CONCURRENCY-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Root Scope eines neuen Execution Domains,
- Child Scope,
- mehrere parallele Tasks,
- erfolgreicher Join,
- Parent-Ende mit laufenden Children,
- hierarchische Cancellation,
- Child-Cancellation ohne Parent-Cancellation,
- Cancellation während I/O,
- verspätete I/O-Completion,
- begrenzter Non-Cancellable-Bereich,
- Parent Deadline,
- strengere Child Deadline,
- Versuch einer längeren Child Deadline,
- Scope Resource Budget,
- Child Budget Exhaustion,
- hierarchisches Accounting,
- FailFast,
- Collect,
- Isolate,
- supervisierter langlebiger Worker,
- expliziter Ownership Transfer,
- abgelehntes ownerloses Detached Work,
- Application Background Scope,
- Request Scope,
- Device Operation Scope,
- Hot-Unplug während aktiver Operation,
- Prozessbeendigung mit aktivem Root Scope,
- Checkpoint während laufender Child Tasks,
- Migration eines quieszierten Scope,
- Scoped Capability,
- Capability Revocation bei Scope-Ende,
- Data-Sovereignty-Vererbung,
- Deterministic Scope,
- Realtime Scope,
- optionale Task-Degradation,
- Introspection,
- Tracing.

---

## Ergebnis

NovaOS behandelt Nebenläufigkeit nicht als lose Sammlung unabhängiger Threads und Tasks.

Stattdessen wird jede Arbeit in eine explizite Lifecycle-Hierarchie eingeordnet:

```text
Execution Domain
      ↓
Root Scope
      ↓
Child Scopes
      ↓
Tasks
      ↓
Execution Contexts
```

Dadurch werden:

```text
lifetime
ownership
cancellation
failure
resources
deadlines
```

Teil derselben strukturierten Ausführungssemantik.

Die zentrale Architekturregel lautet:

```text
No task without an owner.

No child without a lifetime.

No cancellation without propagation.

No scope completion while required work remains.

Concurrency is structured by default.
```