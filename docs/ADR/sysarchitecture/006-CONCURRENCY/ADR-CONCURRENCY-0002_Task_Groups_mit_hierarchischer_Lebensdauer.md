# ADR-CONCURRENCY-0002 – Task Groups mit hierarchischer Lebensdauer

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Tasks / Task Groups / Execution Scopes / Lifecycle / Ressourcen / Fehlerbehandlung

## Kontext

`ADR-CONCURRENCY-0001_Structured_Concurrency` definiert Structured Concurrency als natives Standardmodell für nebenläufige Arbeit in NovaOS.

Dabei wird jede strukturierte Arbeit einem `Execution Scope` zugeordnet.

Für reale Programme reicht jedoch eine reine Parent-/Child-Beziehung einzelner Tasks nicht aus.

Viele Operationen bestehen aus Gruppen logisch zusammengehöriger Tasks.

Beispiele:

- mehrere parallele Dateizugriffe,
- Worker eines Algorithmus,
- Pipeline-Stufen,
- parallele Netzwerkoperationen,
- Rendering-Arbeit,
- Task-Fan-Out,
- parallele Suchoperationen,
- Hintergrundarbeit einer Anwendung,
- mehrere Child-Operationen eines Requests.

Diese Aufgaben benötigen eine gemeinsame Lebensdauer.

NovaOS benötigt deshalb ein explizites Modell für Task Groups.

Eine Task Group soll nicht nur eine Sammlung von Tasks sein, sondern eine strukturierte Lifecycle-Grenze mit:

- eindeutiger Ownership,
- gemeinsamer Cancellation,
- gemeinsamen Deadlines,
- hierarchischen Ressourcenbudgets,
- definierter Fehlersemantik,
- kontrolliertem Join,
- klarer Beziehung zum Parent Scope.

---

## Entscheidung

NovaOS führt **Task Groups mit hierarchischer Lebensdauer** als zentrale Structured-Concurrency-Abstraktion ein.

Eine Task Group ist ein spezialisierter Execution Scope zur Verwaltung einer Gruppe logisch zusammengehöriger Child Tasks und Child Groups.

Das Grundmodell lautet:

```text
Execution Domain
      ↓
Execution Scope
      ↓
Task Group
      ├── Task A
      ├── Task B
      ├── Task C
      └── Child Task Group
             ├── Task D
             └── Task E
```

Eine Task Group darf erst abgeschlossen werden, wenn ihre zugehörige Arbeit entsprechend ihrer Group Policy abgeschlossen, gecancelt oder kontrolliert übernommen wurde.

---

## Grundprinzip

```text
Tasks that belong together
must live and die together
unless ownership is explicitly transferred.
```

---

## Task Group

Eine Task Group ist ein hierarchischer Container strukturierter Arbeit.

Konzeptionell:

```text
TaskGroup {
    id
    parent
    state

    tasks
    childGroups

    cancellationContext
    deadline

    resourceBudget
    schedulingConstraints

    completionPolicy
    failurePolicy
}
```

---

## Beziehung zum Execution Scope

Eine Task Group ist semantisch ein Execution Scope mit gruppenspezifischer Task-Verwaltung.

Es gilt:

```text
Task Group
    ⊆
Execution Scope
```

Damit gelten grundsätzlich dieselben systemweiten Regeln für:

- Lifecycle,
- Cancellation,
- Ressourcen,
- Security,
- Data Policy,
- Deadlines.

---

## Keine zweite konkurrierende Scope-Hierarchie

Task Groups dürfen keine separate unabhängige Lifecycle-Struktur neben Execution Scopes erzeugen.

Die Hierarchie muss Teil derselben Execution-Scope-Struktur bleiben.

Beispiel:

```text
Root Scope
   ↓
Task Group A
   ↓
Task Group B
```

nicht:

```text
Scope Tree
+
independent Task Group Tree
```

---

## Hierarchische Lebensdauer

Jede Task Group besitzt genau einen Parent Scope beziehungsweise eine Parent Task Group, mit Ausnahme eines expliziten Root-Kontexts.

Die Lebensdauer eines Childs ist an seine Parent-Struktur gebunden.

```text
Parent Group
    ↓
Child Group
    ↓
Tasks
```

Ein Child darf den Parent nicht unkontrolliert überleben.

---

## Group Creation

Eine Task Group wird innerhalb eines bestehenden Scope erzeugt.

Konzeptionell:

```text
parent.CreateTaskGroup(...)
```

Die neue Group erbt definierte Constraints des Parents.

---

## Group Ownership

Der erzeugende beziehungsweise explizit übernehmende Scope besitzt die Lifecycle-Verantwortung für die Group.

Eine Task Group ohne Owner ist im nativen Modell nicht zulässig.

---

## Task Creation

Tasks werden innerhalb einer Group erzeugt.

```text
group.Spawn(task)
```

Jede Task gehört während ihrer strukturierten Lebensdauer genau einer Group beziehungsweise einem owning Scope.

---

## Nested Groups

Task Groups dürfen beliebig logisch verschachtelt werden.

Beispiel:

```text
Image Processing Group
    ├── Decode Group
    │     ├── Task 1
    │     └── Task 2
    │
    ├── Filter Group
    │     ├── Task 3
    │     └── Task 4
    │
    └── Encode Group
          └── Task 5
```

Die Implementierung darf technische Grenzen gegen pathologische Verschachtelung besitzen.

---

## Parent Completion

Eine Parent Group darf nicht erfolgreich abgeschlossen werden, solange verpflichtende Child Tasks oder Groups noch aktiv sind.

Es gilt:

```text
Parent Complete
    requires
Required Children Complete
```

---

## Group Close

Eine Group kann für neue Tasks geschlossen werden.

Konzeptionell:

```text
Open
   ↓
Closing
   ↓
Closed
```

Nach `Closing` dürfen keine neuen normalen Child Tasks mehr aufgenommen werden.

---

## Join

Das Schließen einer Group und das Warten auf ihre Children sind semantisch getrennte Operationen.

Typischer Ablauf:

```text
Create Group
    ↓
Spawn Tasks
    ↓
Close Group
    ↓
Join
    ↓
Completed
```

High-Level-APIs dürfen diese Schritte kombinieren.

---

## Implicit Join

Sprach- oder Runtime-Abstraktionen dürfen beim Verlassen eines strukturierten Blocks automatisch joinen.

Beispiel konzeptionell:

```text
with task_group:
    spawn A
    spawn B

# A and B completed or were cancelled here
```

Die konkrete NovaLang-Syntax wird separat definiert.

---

## Dynamic Task Creation

Tasks innerhalb einer offenen Group dürfen weitere Tasks in derselben oder einer Child Group erzeugen, sofern die Group Policy dies erlaubt.

Dies darf nicht nach dem endgültigen Schließen der Group erfolgen.

---

## Race zwischen Close und Spawn

Group Close und Task Spawn müssen atomar beziehungsweise race-sicher definiert sein.

Es darf kein Zustand entstehen, bei dem eine Task nach erfolgreichem Group-Abschluss unbemerkt hinzugefügt wird.

---

## Completion Policy

Eine Task Group besitzt eine Completion Policy.

Mögliche Grundformen sind:

```text
All
Any
FirstSuccess
Quorum
Explicit
```

Nicht jede Policy muss in der ersten Implementierung vollständig unterstützt werden.

`All` bildet die normative Baseline.

---

## All

Bei `All` müssen alle verpflichtenden Child Tasks erfolgreich beziehungsweise entsprechend ihrer individuellen Semantik abgeschlossen sein.

```text
A complete
B complete
C complete
    ↓
Group complete
```

---

## Any

Bei `Any` kann das Ergebnis der ersten abgeschlossenen Task die Gruppenoperation erfüllen.

Verbleibende Tasks werden anschließend abhängig von Policy kontrolliert gecancelt oder weitergeführt.

---

## FirstSuccess

Bei `FirstSuccess` wird die Group erfolgreich, sobald eine Task erfolgreich abgeschlossen wurde.

Fehlgeschlagene Tasks allein beenden die Group nicht, solange noch eine mögliche erfolgreiche Task existiert.

Beispiel:

```text
Search Provider A
Search Provider B
Search Provider C
```

Die erste erfolgreiche Antwort kann genügen.

---

## Quorum

Eine zukünftige Quorum Policy kann verlangen:

```text
N successful results out of M
```

Dies ist insbesondere für verteilte oder redundante Operationen geeignet.

Die Group muss dabei weiterhin eine klare Rest-Task-Policy besitzen.

---

## Required und Optional Tasks

Tasks können als:

```text
Required
Optional
```

klassifiziert werden.

Required Tasks bestimmen die Korrektheit der Group.

Optional Tasks können bei:

- Deadline-Druck,
- Ressourcenmangel,
- Graceful Degradation

beendet werden.

---

## Cancellation

Eine Task Group besitzt einen eigenen Cancellation Context.

Cancellation einer Group wird an alle nicht abgeschlossenen Child Tasks und Child Groups propagiert.

```text
Cancel Group
    ↓
Task A
Task B
Child Group
```

---

## Parent Cancellation

Wird der Parent Scope gecancelt, muss die Cancellation grundsätzlich an die Child Group weitergegeben werden.

Eine Child Group darf Parent Cancellation nicht dauerhaft blockieren.

---

## Child Cancellation

Die Cancellation einer einzelnen Child Task muss nicht automatisch die gesamte Group canceln.

Die Reaktion bestimmt die Failure beziehungsweise Completion Policy.

---

## Cancellation State

Eine Group besitzt mindestens Zustände analog zu:

```text
Active
CancellationRequested
Cancelling
Joining
Cancelled
```

---

## Deadline

Eine Task Group kann eine eigene Deadline besitzen.

Ihre effektive Deadline darf die Parent Deadline nicht überschreiten.

Konzeptionell:

```text
effectiveDeadline =
    min(parentDeadline, groupDeadline)
```

---

## Deadline Propagation

Die effektive Group Deadline wird wiederum an Children propagiert.

```text
Parent Deadline
      ↓
Group Deadline
      ↓
Task Deadline
```

Damit entstehen keine unkontrollierten längeren Child-Lifetimes.

---

## Deadline Expiry

Bei Ablauf einer Deadline wird abhängig vom Contract eine definierte Aktion ausgelöst.

Typisch:

```text
Deadline Exceeded
    ↓
Cancel Group
```

oder für Graceful Degradation:

```text
Cancel Optional Tasks
    ↓
Continue Required Work if still valid
```

---

## Resource Budget

Eine Task Group kann ein hierarchisches Ressourcenbudget erhalten.

Beispiel:

```text
TaskGroupBudget {
    cpu
    memory
    io
    network
    gpu
    npu
}
```

Dieses Budget ist Teil des Parent-Budgets.

---

## Keine Ressourcenerzeugung durch Verschachtelung

Child Groups dürfen durch Verschachtelung keine zusätzlichen Ressourcen erzeugen.

Grundregel:

```text
Child Budget
    ≤
Available Parent Budget
```

für harte reservierte Ressourcen.

---

## Shared Group Budget

Mehrere Tasks können ein gemeinsames Group Budget dynamisch verwenden.

Beispiel:

```text
Group Memory Budget:
    1 GiB

Task A uses:
    300 MiB

Task B uses:
    200 MiB

Remaining:
    500 MiB
```

---

## Reserved Subbudgets

Eine Group kann Ressourcen auch explizit auf Children verteilen.

```text
Group:
    CPU 100 units

Task A:
    30

Task B:
    20

Shared remainder:
    50
```

---

## Resource Exhaustion

Wenn eine Group ihr Budget ausschöpft, kann ihre Policy:

```text
Throttle
Reject new work
Cancel Optional Tasks
Fail Group
Escalate
```

auslösen.

Hard Resource Contracts dürfen nicht still überschritten werden.

---

## Hierarchisches Accounting

Der Ressourcenverbrauch jeder Task wird zur Group und allen Parent Scopes aggregiert.

```text
Task
 ↓
Task Group
 ↓
Parent Group
 ↓
Execution Domain
```

Damit bleibt systemweit erkennbar, welche Gesamtoperation Ressourcen verursacht.

---

## Scheduling Constraints

Eine Group kann Scheduling Constraints definieren.

Beispiele:

```text
CPU Set
Priority Class
Latency Class
Deadline
Weight
Locality Preference
```

Child Tasks dürfen diese Werte nur innerhalb der Parent-Grenzen verfeinern.

---

## Parallelism Limit

Eine Task Group kann einen maximalen Parallelitätsgrad besitzen.

Beispiel:

```text
maxParallelism = 8
```

Dies begrenzt die gleichzeitig ausführbaren Tasks, ohne die Anzahl logisch vorhandener Tasks zwingend zu begrenzen.

---

## Concurrency Limit vs Task Count

NovaOS unterscheidet:

```text
Task Count
```

von:

```text
Active Parallelism
```

Eine Group kann beispielsweise 100 Tasks enthalten, aber maximal 8 gleichzeitig ausführen.

---

## Adaptive Parallelism

Der Parallelitätsgrad darf bei Soft Constraints adaptiv angepasst werden.

Signale können sein:

- CPU-Auslastung,
- Cache Pressure,
- Memory Bandwidth,
- Energie,
- Thermal State,
- Deadline.

Hard Limits bleiben autoritativ.

---

## Locality

Eine Group kann ein gemeinsames Locality-Ziel besitzen.

Beispiel:

```text
prefer same NUMA node
```

oder:

```text
prefer same accelerator
```

Dadurch können eng zusammenarbeitende Tasks lokal zusammengehalten werden.

---

## Work Distribution

Der Scheduler beziehungsweise eine Runtime darf Tasks einer Group über mehrere Execution Contexts verteilen.

Die Task Group schreibt keine feste Worker-Implementierung vor.

Mögliche Mechanismen:

```text
thread pool
work stealing
dedicated workers
kernel workers
accelerator queues
```

---

## Work Stealing

Work Stealing darf verwendet werden, solange:

- Scope Ownership erhalten bleibt,
- Ressourcenattribution erhalten bleibt,
- Locality-/Determinism-Constraints eingehalten werden.

Eine Task verliert durch Worker-Wechsel nicht ihre Group-Zugehörigkeit.

---

## Failure Model

Task-Fehler werden zunächst der owning Group gemeldet.

Die Group entscheidet nach ihrer Failure Policy.

Konzeptionell:

```text
Task Failure
     ↓
Task Group
     ↓
Failure Policy
```

---

## FailFast

Bei `FailFast` führt ein relevanter Child-Fehler zu:

```text
Failure detected
    ↓
Cancel remaining Children
    ↓
Join
    ↓
Group Failed
```

Dies ist die bevorzugte Semantik für Tasks, die gemeinsam ein einziges Ergebnis erzeugen.

---

## CollectErrors

Bei `CollectErrors` dürfen mehrere Child Tasks unabhängig bis zum Join weiterlaufen.

Anschließend liefert die Group eine strukturierte Fehleraggregation.

Beispiel:

```text
Task A → Error A
Task B → Success
Task C → Error C

Group Result:
    AggregateError(A, C)
```

---

## Error Aggregation

Aggregierte Fehler müssen ihre Herkunft erhalten.

Mindestens sollen erkennbar sein:

```text
task identity
error type
scope/group identity
```

Die interne Fehlerrepräsentation darf kompakter implementiert werden.

---

## Isolated Child

Ein Child kann ausdrücklich als isoliert markiert werden.

Sein Failure führt dann nicht automatisch zum Failure der Group, sofern die Group-Semantik dies erlaubt.

Das Ignorieren eines Fehlers muss explizit sein.

---

## Task Result Collection

Groups dürfen Resultate ihrer Tasks sammeln.

Beispiel:

```text
TaskGroup<Result<T>>
```

Die High-Level-API kann unterschiedliche Resultatmodelle anbieten.

---

## Result Ordering

Resultat-Reihenfolge muss semantisch definiert sein.

Mögliche Modelle:

```text
SubmissionOrder
CompletionOrder
Unordered
```

Im Deterministic Mode muss die gewählte Semantik reproduzierbar sein.

---

## Early Result

Bei Policies wie `Any` oder `FirstSuccess` kann ein Ergebnis vor Abschluss aller gestarteten Tasks verfügbar werden.

Die restlichen Tasks müssen trotzdem einen kontrollierten Lifecycle erhalten.

Es gilt nicht:

```text
return result
and forget remaining tasks
```

---

## Cleanup nach Early Completion

Nach frühem Abschluss gilt typischerweise:

```text
Select Result
    ↓
Cancel Remaining
    ↓
Join Remaining
    ↓
Release Resources
    ↓
Return
```

sofern die Policy nichts anderes explizit vorsieht.

---

## Detached Child

Eine Task darf nicht durch bloßes Verlassen einer Group automatisch detached werden.

Wenn Arbeit weiterleben soll, muss Ownership vor Group-Ende übertragen werden.

---

## Ownership Transfer

Eine Task oder Child Group kann in einen anderen gültigen Scope übertragen werden.

Der Transfer muss:

- atomar,
- autorisiert,
- ressourcenkonsistent

sein.

Nach dem Transfer ist die neue Group beziehungsweise der neue Scope verantwortlich.

---

## Transfer Constraints

Ein Transfer darf keine Parent-Constraints umgehen.

Beispiel:

Eine Task mit `LocalOnly`-Data Policy darf nicht in einen Remote Scope übertragen werden, der diese Policy verletzt.

---

## Scoped Capabilities

Eine Group darf temporäre Capabilities besitzen.

Beispiel:

```text
Task Group:
    File.Write(document X)
```

Alle Child Tasks können abhängig von Policy auf diese Capability zugreifen.

Nach Group-Ende wird die temporäre Delegation beendet.

---

## Capability Restriction

Child Groups dürfen Capabilities weiter einschränken.

```text
Parent Group:
    Directory.Read

Child Group:
    File.Read(A)
```

Authority darf nicht durch Hierarchie erweitert werden.

---

## Data Sovereignty

Data-Sovereignty-Constraints werden von Parent zu Child vererbt.

Ein Child darf stärkere Einschränkungen hinzufügen.

Es darf Parent Constraints nicht abschwächen.

---

## Trust Context

Task Groups erzeugen keinen eigenen unabhängigen Trust allein durch ihre Existenz.

Sie laufen im Trust-/Security-Kontext des Execution Domains beziehungsweise in explizit delegierten Teilkontexten.

---

## Request Groups

Ein Service kann pro eingehender Anfrage eine Task Group erzeugen.

```text
Request
    ↓
Request Task Group
    ├── Authenticate
    ├── Read Data
    ├── Compute
    └── Response
```

Wird der Request abgebrochen, kann die gesamte Group gecancelt werden.

---

## Parallel I/O

Eine Group eignet sich für parallele I/O-Operationen.

Beispiel:

```text
Load Document Group
    ├── Read Metadata
    ├── Read Content
    └── Read Preview
```

Falls Content zwingend benötigt wird, Preview aber optional ist, kann diese Semantik explizit abgebildet werden.

---

## Algorithmische Parallelität

NovaOS-Bibliotheken dürfen Task Groups zur internen Parallelisierung abstrakter Algorithmen verwenden.

Beispiel:

```text
Sort(data)
    ↓
Algorithm selected
    ↓
Task Group
       ├── Partition A
       ├── Partition B
       └── Merge
```

Die Group bleibt dabei Implementierungsdetail der Capability.

---

## Parallel For

Eine Runtime kann `parallel for` auf eine Task Group abbilden.

```text
Task Group
    ├── Range 0
    ├── Range 1
    ├── Range 2
    └── Range 3
```

Der Parallelitätsgrad kann kleiner als die Zahl der Partitionen sein.

---

## Pipeline Groups

Pipelines können aus hierarchischen Groups bestehen.

```text
Pipeline Group
    ├── Decode Group
    ├── Transform Group
    └── Encode Group
```

Die konkrete Datenflusssemantik wird durch die Pipeline-Architektur definiert.

---

## I/O Cancellation

Werden I/O-Tasks gecancelt, muss zwischen logischem Task-Ende und physischer I/O-Completion unterschieden werden.

Die Group darf erst dann vollständig bereinigt gelten, wenn alle relevanten Ressourcen sicher behandelt wurden.

---

## Kernel Task Groups

Das Konzept darf auch innerhalb geeigneter Kernelbereiche verwendet werden.

Beispiel:

```text
Filesystem Operation Group
    ├── Metadata Read
    ├── Data Read
    └── Verification
```

Die Kernelimplementierung darf dafür optimierte interne Repräsentationen verwenden.

---

## Deferred Work Groups

Deferred Work kann einer gemeinsamen Group zugeordnet werden.

Damit kann beispielsweise ein komplexer Gerätetransfer als eine zusammenhängende Operation verwaltet werden.

---

## Driver Operations

Treiber können pro Geräteoperation Task Groups verwenden.

```text
Device Request Group
    ├── Prepare DMA
    ├── Submit
    ├── Wait Completion
    └── Cleanup
```

Bei Hot-Unplug kann die gesamte Group gecancelt beziehungsweise drained werden.

---

## Group Quiescing

Eine Task Group kann einen Quiescing-Zustand besitzen.

Dabei gilt:

- keine neue normale Arbeit,
- bestehende Arbeit wird abgeschlossen oder gecancelt,
- Group bewegt sich zu einem stabilen Zustand.

Dies ist für:

- Checkpoint,
- Migration,
- Hot Replacement

relevant.

---

## Checkpoint

Checkpointing kann eine Group gezielt quieszieren.

Dadurch ist bekannt, welche Tasks zum Capture-Zeitpunkt:

- abgeschlossen,
- wartend,
- aktiv,
- gecancelt

sind.

---

## Migration

Bei Migration kann eine Workload Group optionale Tasks abbrechen und Required Tasks bis zu einem definierten Punkt drainen.

Damit wird der zu übertragende Ausführungszustand reduziert.

---

## Hot Replacement

Service- oder Application-Task-Groups können beim Hot Replacement kontrolliert geschlossen werden.

Neue Arbeit wird dann an eine neue Group beziehungsweise neue Domain geleitet.

---

## Process Termination

Bei Prozessbeendigung werden Root Scope und alle enthaltenen Task Groups hierarchisch beendet.

```text
Terminate Process
      ↓
Cancel Root Scope
      ↓
Cancel Task Groups
      ↓
Join / Drain
      ↓
Release Resources
```

---

## Process Supervision

Task Groups ersetzen Process Supervision nicht.

Die Ebenen sind:

```text
Process Supervisor
      ↓
Process / Execution Domain
      ↓
Task Groups
      ↓
Tasks
```

Process Supervision behandelt Domain-Lifecycle.

Task Groups behandeln interne strukturierte Arbeit.

---

## Group Supervision

Für langlebige interne Worker kann eine Group eine begrenzte Supervision Policy besitzen.

Dies darf jedoch nicht zu unkontrollierten ewigen Restart-Loops innerhalb einer Task Group führen.

Langlebige systemkritische Arbeit gehört gegebenenfalls in ein eigenes supervisiertes Execution Domain.

---

## Bounded Lifetime

Eine normale Task Group besitzt eine begrenzte semantische Lebensdauer.

Extrem langlebige Groups sind möglich, benötigen aber weiterhin einen klaren übergeordneten Owner.

---

## Persistent Background Group

Eine Anwendung kann beispielsweise besitzen:

```text
Application Scope
    ↓
Background Task Group
```

Diese Group darf solange leben wie die Anwendung, aber nicht länger als deren owning Scope.

---

## Deterministic Mode

Task Groups müssen Deterministic Mode unterstützen.

Deterministisch kontrollierbar sein müssen insbesondere:

- Task-Erzeugungsreihenfolge,
- Result Ordering,
- Failure Selection,
- Join-Semantik,
- Parallelism Limit,
- Scheduling Constraints.

Parallelität darf trotzdem erhalten bleiben, sofern Ergebnissemantik reproduzierbar ist.

---

## Race Semantik

Structured Concurrency verhindert nicht automatisch Data Races.

Tasks einer Group, die gemeinsamen veränderlichen Speicher verwenden, benötigen weiterhin:

- Synchronisation,
- Ownership,
- atomare Datenstrukturen,
- immutable Daten

oder andere definierte Mechanismen.

---

## First Failure Determinism

Bei parallelen Tasks können mehrere Fehler nahezu gleichzeitig auftreten.

Im Deterministic Mode darf nicht zufällig der zeitlich zuerst beobachtete Hardwarefehler als alleiniger semantischer Fehler gelten, wenn dies die Reproduzierbarkeit verletzt.

Die Group kann stattdessen:

- mehrere Fehler sammeln,
- deterministische Priorität verwenden,
- einen stabilen Auswahlmechanismus definieren.

---

## Realtime

Realtime Task Groups können besitzen:

```text
Group Deadline
Reserved CPU Budget
Maximum Parallelism
CPU Set
```

Optional Work darf bei Deadline-Druck zuerst gecancelt werden.

Required Work darf seine zugesicherte Deadline nicht still verlieren.

---

## Graceful Degradation

Task Groups bieten eine natürliche Einheit für Graceful Degradation.

Beispiel:

```text
Render Group
    ├── Required: geometry
    ├── Required: compositing
    ├── Optional: supersampling
    └── Optional: AI enhancement
```

Bei Ressourcen- oder Zeitdruck können optionale Tasks wegfallen.

---

## Architecture Introspection

Task Groups müssen introspektierbar sein.

Beispiel:

```text
Task Group:
    document.open

State:
    Active

Parent:
    application.document

Tasks:
    6

Running:
    3

Waiting:
    2

Completed:
    1

Deadline:
    120 ms

Budget:
    Memory 256 MiB
```

---

## Group Tree

Architecture Introspection soll die Hierarchie darstellen können.

```text
Application Root
    ├── UI Group
    ├── Document Group
    │      ├── Load Group
    │      └── Save Group
    └── Background Group
```

---

## Tracing

Task Group IDs können als strukturierende Trace-Kontexte verwendet werden.

Dadurch können mehrere Tasks derselben logischen Operation gemeinsam analysiert werden.

---

## Debugging

Debugger und Profiler sollen Gruppen statt nur einzelne Threads darstellen können.

Dies erlaubt Aussagen wie:

```text
Save Document Group:
    12 Tasks
    2 blocked
    1 failed
```

---

## Resource Introspection

Ressourcen sollen gruppiert dargestellt werden können.

Beispiel:

```text
Task Group:
    image.render

CPU:
    148 ms

Memory Peak:
    220 MiB

GPU:
    31 ms

I/O:
    4 MiB
```

---

## Audit

Normale Task-Group-Erzeugung benötigt kein vollständiges persistentes Security Audit.

Sicherheitsrelevante Ereignisse können jedoch auditierbar sein.

Beispiele:

```text
privileged capability delegated to group
cross-domain ownership transfer
resource hard-limit override
security-sensitive cancellation
```

---

## Fail-Safe Defaults

Wenn eine Group endet, dürfen unbekannte Child Tasks nicht einfach vergessen werden.

Wenn eine Task nicht rechtzeitig beendet werden kann, muss eine definierte Eskalation erfolgen.

Mögliche Eskalation:

```text
Cancel
    ↓
Drain timeout
    ↓
Escalate to owning scope
    ↓
possibly terminate domain
```

---

## Kein globaler Task Garbage Collector

NovaOS darf sich nicht darauf verlassen, dass vergessene Tasks irgendwann von einem globalen Lifecycle-Garbage-Collector entdeckt und beendet werden.

Ownership muss bereits bei Task-Erzeugung definiert sein.

---

## Performance

Task Groups sind ein semantisches Modell.

Die Implementierung darf:

- kompakte IDs,
- intrusive Listen,
- lockfreie Queues,
- per-CPU Schedulerstrukturen,
- Runtime-local Metadaten

verwenden.

Es ist nicht erforderlich, jede kleine Task Group als schwergewichtiges Kernelobjekt zu implementieren.

---

## Kernel-/Runtime-Aufteilung

Ein Teil der Task-Group-Semantik kann durch Runtimes umgesetzt werden.

Der Kernel muss jedoch die notwendigen Mechanismen bereitstellen für:

- Threads,
- Cancellation-relevante Waits,
- Deadlines,
- Resource Accounting,
- Lifecycle,
- sichere I/O-Completions.

Die High-Level-Task-Abstraktion kann oberhalb davon liegen.

---

## NovaLang

NovaLang soll Task Groups als bevorzugte Struktur für parallele Child-Arbeit abbilden können.

Die Sprache soll insbesondere automatische:

- Join,
- Cancellation Propagation,
- Result Collection

unterstützen können.

Die konkrete Syntax wird separat spezifiziert.

---

## Kompatibilität

Kompatibilitäts-APIs dürfen unstrukturierte Thread- oder Future-Modelle anbieten.

Intern müssen solche Aktivitäten trotzdem einem gültigen Execution Scope beziehungsweise Compatibility Scope zugeordnet sein.

---

## Keine KI-Abhängigkeit

Task Groups und ihre Lifecycle-Regeln müssen vollständig ohne KI funktionieren.

KI darf optional:

- optimalen Parallelitätsgrad schätzen,
- Work Partitioning vorschlagen,
- Locality optimieren.

KI darf keine:

- Ownership,
- Cancellation,
- Hard Deadline,
- Capability-Grenze

aufheben.

---

## Normative Anforderungen

1. NovaOS MUSS Task Groups als strukturierte Gruppierung logisch zusammengehöriger Tasks unterstützen.
2. Task Groups MÜSSEN in die bestehende Execution-Scope-Hierarchie integriert sein.
3. Eine Task Group MUSS genau einen owning Parent Scope besitzen.
4. Child Task Groups MÜSSEN hierarchisch verschachtelbar sein.
5. Jede strukturierte Task MUSS einem gültigen Scope beziehungsweise einer Task Group zugeordnet sein.
6. Task Groups DÜRFEN ihre verpflichtenden Children NICHT unkontrolliert überleben lassen.
7. Group Close und Task Spawn MÜSSEN race-sicher definiert sein.
8. `All` MUSS als grundlegende Completion Policy unterstützt werden.
9. Alternative Completion Policies DÜRFEN unterstützt werden, müssen aber verbleibende Tasks kontrolliert behandeln.
10. Task Groups MÜSSEN Required und Optional Work unterscheiden können.
11. Cancellation MUSS an Child Tasks und Child Groups propagierbar sein.
12. Parent Cancellation MUSS Child Groups erreichen.
13. Child Cancellation DARF NICHT zwingend Parent Cancellation auslösen.
14. Deadlines MÜSSEN hierarchisch vererbbar sein.
15. Child- und Group-Deadlines DÜRFEN Parent-Deadlines NICHT erweitern.
16. Task Groups MÜSSEN hierarchische Ressourcenbudgets besitzen können.
17. Child Groups DÜRFEN harte Parent-Budgets NICHT erweitern.
18. Ressourcenverbrauch MUSS zur Group und ihren Parent Scopes zurechenbar sein.
19. Ein Parallelism Limit MUSS unabhängig von der logischen Task-Anzahl modellierbar sein.
20. Task-Fehler MÜSSEN einer klaren Group Failure Policy zugeordnet werden.
21. FailFast und strukturierte Fehleraggregation MÜSSEN unterstützt werden können.
22. Early-Completion-Policies DÜRFEN verbleibende Tasks NICHT ownerlos zurücklassen.
23. Ownership Transfer MUSS explizit, atomar und constraint-erhaltend sein.
24. Scoped Capabilities und Data Policies MÜSSEN über Task-Group-Hierarchien einschränkbar sein.
25. Task Groups MÜSSEN mit I/O, Deferred Work, Checkpoint, Migration und Process Termination integrierbar sein.
26. Realtime- und Determinism-Constraints MÜSSEN auf Group-Ebene ausdrückbar sein.
27. Task Groups MÜSSEN Graceful Degradation über optionale Arbeit unterstützen können.
28. Group-Hierarchie, Zustand und relevante Ressourcen MÜSSEN introspektierbar sein.
29. Task Groups DÜRFEN NICHT als schwergewichtiges Kernelobjekt vorausgesetzt werden, solange die definierte Semantik erhalten bleibt.
30. Die vollständige grundlegende Task-Group-Funktionalität MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- klare Lebensdauer für Gruppen paralleler Tasks,
- saubere Parent-/Child-Semantik,
- einheitliche Cancellation,
- bessere Fehleraggregation,
- strukturierter Join,
- gemeinsame Ressourcenbudgets,
- kontrollierbarer Parallelitätsgrad,
- gute Grundlage für parallele Algorithmen,
- bessere Debug- und Introspection-Möglichkeiten,
- direkte Unterstützung für Graceful Degradation.

### Negative Konsequenzen

- Task-Runtimes müssen Group-Lifecycles korrekt verwalten,
- dynamische Task-Erzeugung benötigt race-sichere Close-Semantik,
- Fehlerpolitik muss für verschiedene Group-Typen explizit gewählt werden,
- Ownership Transfer erhöht die Komplexität gegenüber vollständig lexikalisch gebundenen Tasks.

---

## Verworfene Alternativen

### Task Group nur als Liste von Tasks

Verworfen.

Eine reine Sammlung besitzt keine ausreichende Lifecycle-, Cancellation- oder Fehlersemantik.

### Jede Task unabhängig joinen

Verworfen.

Dies führt bei großen parallelen Operationen zu unnötiger Lifecycle-Komplexität.

### Group-Ende ignoriert noch laufende Tasks

Verworfen.

Dies widerspricht Structured Concurrency.

### Automatisches Detached Work bei Early Return

Verworfen.

Verbleibende Arbeit benötigt weiterhin einen klaren Owner.

### Unbegrenzte Parallelität

Verworfen.

Task-Anzahl und tatsächlich sinnvoller Parallelitätsgrad sind unterschiedliche Größen.

### Task Groups als ausschließliches Kernelobjekt

Verworfen.

Viele feingranulare Groups können effizienter durch Sprachruntimes verwaltet werden.

Die Systemsemantik muss trotzdem einheitlich bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
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
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-TASKGROUP-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-LIFECYCLE-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-HIERARCHY-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-SPAWN-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-JOIN-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-FAILURE-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-RESULT-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-RESOURCE-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-PARALLELISM-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-OWNERSHIP-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-INTROSPECTION-0001`
- `NPSPEC-CONCURRENCY-TASKGROUP-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Erzeugung einer Task Group,
- mehrere Child Tasks,
- verschachtelte Child Group,
- Group Close,
- Spawn-versus-Close-Race,
- Join aller Tasks,
- Parent-Ende mit aktiver Group,
- Cancellation einer Group,
- Parent Cancellation,
- einzelne Child-Cancellation,
- FailFast,
- Fehleraggregation,
- Required und Optional Tasks,
- Deadline-Vererbung,
- Group Deadline Expiry,
- gemeinsames Resource Budget,
- Child Subbudget,
- Budget Exhaustion,
- Parallelism Limit,
- viele logische Tasks mit geringerem Parallelitätsgrad,
- Work Stealing unter Erhalt der Group-Zugehörigkeit,
- FirstSuccess mit Rest-Cancellation,
- Early Result mit sauberem Join,
- expliziter Ownership Transfer,
- unzulässiger Transfer aufgrund Data Policy,
- Scoped Capability,
- I/O-Task bei Group Cancellation,
- verspätete Completion,
- Driver Operation Group bei Hot-Unplug,
- Checkpoint einer quieszierten Group,
- Migration mit aktiver Group,
- Process Termination,
- Deterministic Result Ordering,
- Realtime Group,
- Graceful Degradation optionaler Tasks,
- Introspection,
- Tracing.

---

## Ergebnis

NovaOS erweitert Structured Concurrency um Task Groups als explizite hierarchische Lifecycle-Einheiten für logisch zusammengehörige parallele Arbeit.

Das Modell lautet:

```text
Execution Scope
      ↓
Task Group
      ├── Task
      ├── Task
      └── Child Task Group
             ├── Task
             └── Task
```

Eine Task Group bündelt:

```text
lifetime
cancellation
deadline
resources
failure
completion
```

für alle zugehörigen Child-Aktivitäten.

Damit entsteht eine robuste Grundlage für parallele Algorithmen, asynchrone I/O-Operationen, Service-Requests, Pipelines und andere zusammengesetzte Workloads.

Die zentrale Architekturregel lautet:

```text
Group related work.

Own it hierarchically.

Close before completion.

Join before leaving.

Cancel as a tree.

Never lose a child.
```