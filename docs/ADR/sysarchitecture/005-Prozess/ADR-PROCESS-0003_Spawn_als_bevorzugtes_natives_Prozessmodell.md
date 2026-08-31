# ADR-PROCESS-0003 – Spawn als bevorzugtes natives Prozessmodell

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Prozessstart / Execution Domains / Ressourcen / Sicherheit / Lifecycle

## Kontext

NovaOS benötigt ein natives Modell zur Erzeugung neuer Prozesse.

Historische Betriebssysteme verwenden hierfür unterschiedliche Ansätze.

Ein bekanntes Modell ist:

```text
fork()
    ↓
dupliziert bestehenden Prozess
    ↓
exec()
    ↓
ersetzt Prozessabbild
```

Dieses Modell ist für klassische Unix-Systeme leistungsfähig und kompatibel, bringt jedoch semantische Altlasten mit sich.

Beim Fork-Modell entsteht zunächst logisch eine Kopie des bestehenden Prozesses, einschließlich Teilen seines Zustands:

- Adressraum,
- Handles beziehungsweise Deskriptoren,
- Prozesskontext,
- Environment,
- Signalzustand,
- teilweise Security- und Runtime-Zustand.

Anschließend wird dieser Zustand häufig unmittelbar durch `exec()` ersetzt.

Für moderne Systeme mit:

- vielen Threads,
- großen Adressräumen,
- GPU-/NPU-Ressourcen,
- Capability-Sicherheit,
- komplexen Runtime-Zuständen,
- Structured Concurrency,
- expliziten Ressourcenbudgets,
- Data-Sovereignty-Regeln

ist dieses Modell als native Grundlage unnötig indirekt.

NovaOS soll deshalb einen neuen Prozess direkt aus einer expliziten Beschreibung erzeugen.

---

## Entscheidung

NovaOS verwendet **Spawn als bevorzugtes natives Prozessstartmodell**.

Ein neuer Prozess wird direkt aus einer deklarativen `SpawnRequest` beziehungsweise äquivalenten Startbeschreibung erzeugt.

Das grundlegende Modell lautet:

```text
Spawn Request
     ↓
Validate
     ↓
Resolve Executable
     ↓
Evaluate Trust
     ↓
Derive Authority
     ↓
Reserve Resources
     ↓
Create Process Domain
     ↓
Create Address Space
     ↓
Load Image
     ↓
Create Initial Execution Context
     ↓
Activate
```

Es entsteht keine vorherige logische Kopie des aufrufenden Prozesses.

---

## Grundprinzip

```text
Create the process you want.

Do not clone a process
only to replace it immediately.
```

---

## Beziehung zu ADR-PROCESS-0001

`ADR-PROCESS-0001_Natives_Execution_Domain_Modell` definiert Execution Domains als native Ausführungsprimitive.

Spawn erzeugt eine neue Process Execution Domain.

Konzeptionell:

```text
Spawn
   ↓
New Execution Domain
   ↓
Process Domain
```

---

## Beziehung zu ADR-PROCESS-0002

`ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen` definiert Prozesse als:

```text
Isolation Domain
+
Resource Domain
```

Spawn muss deshalb bereits bei der Prozesserstellung folgende Eigenschaften bestimmen können:

- Speicherisolation,
- Capability Set,
- Ressourcenbudget,
- Scheduling Policy,
- Trust Context,
- Data Policy,
- Lifecycle-Verknüpfung.

---

## SpawnRequest

Der Prozessstart wird über eine explizite Startbeschreibung modelliert.

Konzeptionell:

```text
SpawnRequest {
    executable
    arguments
    environment

    capabilities
    handles
    ipcEndpoints

    resourceContract
    schedulingPolicy
    affinity

    securityContext
    trustRequirements
    dataPolicy

    lifecyclePolicy
}
```

Nicht jedes Feld muss bei jedem Aufruf explizit gesetzt werden.

Defaults werden durch Systempolicy bestimmt.

---

## Explizite Semantik

Ein Spawn-Aufruf beschreibt den gewünschten neuen Prozess.

Nicht:

```text
copy parent
then modify copy
```

sondern:

```text
construct child from explicit specification
```

Damit kann NovaOS schon vor der Prozesserstellung prüfen, ob der gewünschte Zustand zulässig ist.

---

## Kein impliziter Parent-Klon

Der Child-Prozess übernimmt nicht automatisch den vollständigen Zustand des Parent-Prozesses.

Insbesondere werden nicht automatisch vollständig dupliziert:

- Adressraum,
- Handles,
- Capabilities,
- Execution Scopes,
- Ressourcenreservierungen,
- Threadzustände,
- Runtime-Zustände.

Übertragung muss explizit definiert sein.

---

## Executable

Die SpawnRequest referenziert das auszuführende Objekt.

Konzeptionell:

```text
ExecutableReference
```

Dies kann beispielsweise sein:

- nativer Executable Handle,
- Capability,
- Package Entry Point,
- System Service Entry,
- Compatibility Entry Point.

Die Referenz muss vor Aktivierung aufgelöst und validiert werden.

---

## Executable Identity

Der gestartete Code wird nicht ausschließlich über einen Pfad identifiziert.

Eine robuste Identität kann zusätzlich enthalten:

```text
package identity
artifact identity
version
hash
signature
provenance
```

Damit kann Trust unabhängig von einem veränderlichen Dateipfad bewertet werden.

---

## Trust Evaluation

Vor Aktivierung wird der Trust-Kontext des ausführbaren Objekts bestimmt.

Beispiel:

```text
Executable
    ↓
Identity
    ↓
Signature
    ↓
Provenance
    ↓
Trust Evaluation
```

Trust bestimmt nicht automatisch die Berechtigungen des Prozesses.

---

## Capability Delegation

Der Parent kann nur Capabilities übertragen, die:

- delegierbar sind,
- innerhalb seiner Authority liegen,
- von der Systempolicy zugelassen werden.

Beispiel:

```text
Parent:
    File.Read
    File.Write
    Network.Connect

Spawn Request:
    File.Read

Child:
    File.Read
```

Der Child erhält keine implizite vollständige Capability-Kopie.

---

## Kein Authority Amplification

Spawn darf keine neue Authority erzeugen, nur weil ein Child-Prozess erstellt wird.

Grundregel:

```text
Child Authority
    ⊆
Authorized Delegation
    +
Explicit System Grants
```

System Grants können beispielsweise durch eine vertrauenswürdige Service- oder Package-Policy entstehen.

---

## Handle Transfer

Handles müssen explizit in die SpawnRequest aufgenommen werden.

Konzeptionell:

```text
HandleTransfer {
    sourceHandle
    targetRights
    transferMode
}
```

Mögliche Transfermodi können sein:

```text
Duplicate
Move
RestrictedDuplicate
```

Die genaue Semantik wird separat spezifiziert.

---

## Restricted Handle Transfer

Ein Child darf bei einer Handle-Übertragung geringere Rechte als der Parent erhalten.

Beispiel:

```text
Parent:
    File Handle = Read + Write

Child:
    File Handle = Read
```

Dies unterstützt Least Authority.

---

## Environment

Environment-Daten können explizit übertragen werden.

Sie sind Konfigurationsdaten und keine Sicherheitsautorität.

```text
Environment Variable
    ≠
Capability
```

---

## Arguments

Argumente werden als definierter Bestandteil der SpawnRequest übergeben.

NovaOS soll intern keine unnötige Abhängigkeit von einer einzigen textbasierten Command-Line-Repräsentation besitzen.

High-Level-Kompatibilitäts-APIs können weiterhin:

```text
"program --option value"
```

unterstützen.

Native APIs dürfen strukturierte Argumente verwenden.

---

## Structured Arguments

Eine native API kann Argumente beispielsweise als:

```text
ArgumentList
```

beziehungsweise typisierte Werte übergeben.

Damit muss NovaOS nicht sämtliche Startparameter durch Shell-Parsing rekonstruieren.

---

## Resource Contract

Die SpawnRequest kann ein Ressourcenprofil beziehungsweise einen Execution Contract enthalten.

Beispiel:

```text
Resources {
    maxMemory
    cpuWeight
    cpuBudget
    ioBudget
    gpuBudget
}
```

Vor Prozessstart kann geprüft werden, ob die Anforderungen grundsätzlich erfüllbar sind.

---

## Resource Reservation

Für harte Ressourcenanforderungen kann Spawn Ressourcen vor Aktivierung reservieren.

Konzeptionell:

```text
Request
   ↓
Reserve
   ↓
Create
   ↓
Activate
```

Scheitert die notwendige Reservierung, kann Spawn vor dem Start fehlschlagen.

---

## Keine halbfertigen Prozesse

Ein Prozess soll nicht sichtbar als regulär laufender Prozess erscheinen, bevor seine notwendigen Startbedingungen erfüllt sind.

Der Lifecycle lautet beispielsweise:

```text
Created
   ↓
Preparing
   ↓
Prepared
   ↓
Runnable
   ↓
Running
```

Ein Fehler während `Preparing` führt zur kontrollierten Rückabwicklung.

---

## Transactional Spawn

Spawn soll soweit sinnvoll transaktional behandelt werden.

Konzeptionell:

```text
Validate
   ↓
Reserve
   ↓
Create Domain
   ↓
Create Address Space
   ↓
Load
   ↓
Bind Resources
   ↓
Verify
   ↓
Activate
```

Scheitert ein Schritt vor der Aktivierung:

```text
Rollback
```

und bereits reservierte Ressourcen werden freigegeben.

---

## Commit Point

Der Start besitzt einen definierten Commit Point.

Vor diesem Punkt ist die Operation vorbereitend.

Danach gilt der Prozess als aktiviert und erhält reguläre Ausführung.

Beispiel:

```text
Prepared
   ↓
Activate Initial Thread
   ↓
COMMIT
   ↓
Running
```

---

## Address Space Creation

Spawn erzeugt direkt einen neuen Adressraum.

Es ist keine Kopie des Parent-Adressraums notwendig.

Das Grundmodell lautet:

```text
New Process
    ↓
New Address Space
    ↓
Map Executable
    ↓
Map Runtime
    ↓
Map Explicit Shared Resources
```

---

## Copy-on-Write

Copy-on-Write bleibt ein wertvoller Speichermechanismus.

NovaOS lehnt COW nicht ab.

Es wird jedoch nicht benötigt, um eine native Fork-Semantik zum grundlegenden Prozessstartmodell zu machen.

COW kann weiterhin für:

- Snapshots,
- Shared Memory,
- Clone-artige APIs,
- Checkpoints,
- Speicheroptimierungen

verwendet werden.

---

## Initial Thread

Spawn erzeugt mindestens einen Initial Execution Context.

Typischerweise:

```text
Process
    ↓
Initial Thread
    ↓
Executable Entry Point
```

Dieser Thread wird erst nach erfolgreicher Vorbereitung ausführbar.

---

## Initial Thread State

Der Initial Thread erhält einen definierten Startzustand.

Dieser kann mindestens enthalten:

```text
entry point
stack
arguments
process runtime context
thread-local base
```

Architekturspezifische Registerdetails bleiben in der HAL-/ABI-Ebene.

---

## Spawn und Structured Concurrency

Ein gestarteter Prozess muss eine explizite Lifecycle-Beziehung besitzen.

Beispiele:

```text
Independent

BoundToParent

BoundToApplication

BoundToJob

BoundToExecutionScope
```

Damit ersetzt NovaOS implizite Parent-/Child-Annahmen durch explizite Lifecycle-Policies.

---

## Bound Spawn

Ein Child kann an einen Parent-Execution-Scope gebunden werden.

Beispiel:

```text
Parent Scope
     ↓
Spawn Child
     ↓
Child Lifecycle bound to scope
```

Wird der Scope beendet, kann auch der Child kontrolliert beendet werden.

---

## Independent Spawn

Ein Prozess darf ausdrücklich unabhängig vom Parent gestartet werden.

Dann gilt:

```text
Parent termination
    ≠
automatic child termination
```

Die Unabhängigkeit muss explizit ausgedrückt beziehungsweise durch die verwendete API-Semantik definiert sein.

---

## Application Spawn

Eine Anwendung kann mehrere Prozesse als Teil derselben Application Domain erzeugen.

Beispiel:

```text
Application
    ├── UI Process
    ├── Worker Process
    └── Renderer Process
```

Die einzelnen Prozesse bleiben separate Isolationsdomänen.

Ressourcenbudgets können jedoch einer gemeinsamen übergeordneten Application Resource Domain zugeordnet werden.

---

## IPC Bootstrap

Spawn kann initiale IPC-Endpunkte direkt an den Child-Prozess übergeben.

Beispiel:

```text
Parent
   ↓
Spawn
   ↓
Child
   ↓
preconfigured IPC endpoint
```

Damit ist kein globaler Namensraum erforderlich, nur um Parent und Child nach dem Start miteinander zu verbinden.

---

## Service Spawn

Systemdienste können ebenfalls über dieselbe native Spawn-Infrastruktur erzeugt werden.

Service Manager oder System Model liefern dabei die gewünschte Startbeschreibung.

Konzeptionell:

```text
Declarative Service Definition
        ↓
Spawn Request
        ↓
Service Process
```

---

## Declarative System Model

Spawn integriert sich in das Declarative System Model.

Beispiel:

```text
Desired State:
    service X running

Actual State:
    service X absent

Reconciler:
    ↓
Spawn service X
```

Spawn ist dabei der Mechanismus.

Die Entscheidung, einen Dienst zu starten, bleibt Policy beziehungsweise Reconciliation.

---

## Sandbox Spawn

Ein Sandbox-Prozess kann direkt mit reduzierter Authority erzeugt werden.

```text
Spawn
   ↓
restricted capabilities
restricted handles
restricted network
restricted resources
   ↓
Sandbox Process
```

Es ist kein nachträgliches Entfernen umfangreicher Parent-Rechte erforderlich.

---

## Driver Spawn

User-Mode- und isolierte Treiber können ebenfalls über kontrollierte Spawn-Mechanismen gestartet werden.

Dabei können bereits während der Vorbereitung:

- Device Capabilities,
- DMA-Rechte,
- Interrupt Endpoints,
- Memory Regions

kontrolliert gebunden werden.

---

## Agent Spawn

KI-Agenten und autonome Komponenten werden mit derselben Spawn-Semantik erzeugt.

Ein Agent erhält explizite:

```text
Capabilities
Resource Budget
Data Policy
Trust Requirements
Lifecycle
```

Agenten benötigen kein separates privilegiertes Prozessmodell.

---

## Compatibility Spawn

POSIX-, Linux- oder Win32-Kompatibilitätsschichten können ihre jeweilige Prozess-API auf den nativen Spawn-Mechanismus abbilden.

Beispiel:

```text
Win32 CreateProcess
        ↓
Compatibility Translation
        ↓
Nova Spawn
```

---

## fork()-Kompatibilität

NovaOS kann für POSIX-Kompatibilität eine `fork()`-Semantik bereitstellen.

Diese ist jedoch:

```text
compatibility mechanism
```

und nicht:

```text
native preferred process creation model
```

Die Implementierung kann beispielsweise:

- Copy-on-Write,
- Compatibility Runtime,
- spezielle Process Clone Mechanismen

verwenden.

---

## Native Process Clone

Falls NovaOS später einen echten Process-Clone-Mechanismus benötigt, kann dieser separat bereitgestellt werden.

Beispiel:

```text
CloneProcess()
```

Ein Clone ist semantisch von Spawn zu unterscheiden.

```text
Spawn:
    construct new process

Clone:
    derive process from existing process state
```

---

## Multithreaded Parent

Spawn ist besonders für multithreaded Prozesse geeignet.

Bei einem klassischen Fork stellt sich die Frage, welcher Zustand anderer Threads im Child sichtbar ist.

Spawn vermeidet diese semantische Komplexität.

```text
Multithreaded Parent
      ↓
Spawn Request
      ↓
Clean Child
```

Der Child beginnt mit einem neu definierten Startzustand.

---

## Runtime-Zustand

Runtime-spezifischer Parent-Zustand wird nicht implizit kopiert.

Beispiele:

- Garbage Collector State,
- JIT State,
- Runtime Locks,
- Thread Pools,
- Event Loops.

Nur explizit übergebene Daten und Ressourcen erreichen den Child.

---

## GPU- und NPU-Zustand

GPU-/NPU-Kontexte werden nicht implizit durch Spawn dupliziert.

Soll ein Child Zugriff auf einen Accelerator erhalten, muss dieser explizit delegiert beziehungsweise neu gebunden werden.

Dies verhindert implizite komplexe Hardwarezustandskopien.

---

## Security Context

Der Security Context des Child-Prozesses wird explizit abgeleitet.

Er muss nicht identisch mit dem Parent sein.

Mögliche Transformation:

```text
Parent Authority
     ↓
Policy
     ↓
Reduced Child Authority
```

Ein Child mit stärkerer Authority benötigt eine separate autorisierte Elevation beziehungsweise Systementscheidung.

---

## Privilege Elevation

Spawn selbst ist keine Privilege-Escalation-API.

Soll ein Prozess mit stärkeren Rechten gestartet werden, benötigt dies einen expliziten autorisierten Mechanismus.

Beispiel:

```text
User Process
    ↓
Request Elevated Operation
    ↓
Authorization Authority
    ↓
Spawn privileged service/process
```

Der Parent kann sich nicht selbst durch Spawn zusätzliche Rechte geben.

---

## Data Sovereignty

Data-Sovereignty-Policy kann beim Spawn an den Child gebunden werden.

Beispiel:

```text
Child:
    allowed data class = Medical
    execution location = LocalOnly
    network export = Forbidden
```

Eine delegierte Datei-Capability darf diese Data Policy nicht automatisch aufheben.

---

## Deterministic Spawn

Für deterministische Ausführung müssen Startparameter stabilisierbar sein.

Dies kann umfassen:

```text
fixed executable generation
fixed capabilities
fixed resource contract
fixed CPU set
fixed environment
fixed runtime configuration
```

Damit wird die Prozesserstellung reproduzierbarer.

---

## Startzeitpunkt

Die Aktivierung eines Prozesses muss kontrollierbar sein.

Ein Prozess kann zunächst vollständig vorbereitet und anschließend zu einem definierten Zeitpunkt aktiviert werden.

Dies ist relevant für:

- Realtime,
- Transaktionen,
- Gruppenstarts,
- Hot Replacement.

---

## Suspended Start

NovaOS darf einen Prozess in vorbereitetem, noch nicht laufendem Zustand erzeugen.

Beispiel:

```text
SpawnPrepared
      ↓
Process exists
but initial thread not running
      ↓
Activate
```

Dies darf nicht mit einem normalen vollständig gestarteten Prozess verwechselt werden.

---

## Gruppenstart

Mehrere Prozesse können zukünftig als gemeinsame transaktionale Startgruppe vorbereitet werden.

Beispiel:

```text
Prepare A
Prepare B
Prepare C
    ↓
Validate
    ↓
Activate Group
```

Diese ADR verlangt keine vollständige Gruppenstartimplementierung, verhindert sie jedoch ausdrücklich nicht.

---

## Startup Failure

Ein Spawn kann vor der Aktivierung typisiert fehlschlagen.

Mögliche Fehler:

```text
ExecutableNotFound
ExecutableInvalid
TrustRejected
PermissionDenied
CapabilityDenied
ResourceUnavailable
AddressSpaceCreationFailed
ImageLoadFailed
InvalidConfiguration
```

Ein fehlgeschlagener Spawn darf keinen scheinbar laufenden Restprozess hinterlassen.

---

## Post-Activation Failure

Fehler nach dem Commit Point sind normale Prozess-Lifecycle- beziehungsweise Runtime-Fehler.

Beispiel:

```text
Spawn succeeded
    ↓
process runs
    ↓
process crashes
```

Dies ist kein Spawn-Fehler mehr.

---

## Return Value

Ein erfolgreicher Spawn liefert eine sichere Prozessreferenz.

Beispiel:

```text
ProcessHandle
```

oder:

```text
ProcessReference
```

Eine rohe PID allein ist für sichere Lifetime-Verfolgung nicht ausreichend.

---

## Parent Visibility

Der Parent muss nicht automatisch uneingeschränkte Kontrolle über den Child erhalten.

Welche Operationen möglich sind, hängt von den beim Spawn erhaltenen Prozess-Capabilities ab.

Beispiel:

```text
Observe
Wait
Signal
Terminate
Debug
```

können getrennte Rechte sein.

---

## Wait

Ein Parent oder anderer autorisierter Prozess kann auf das Ende eines Child-Prozesses warten, wenn er eine entsprechende Referenz besitzt.

Die Fähigkeit zu warten bedeutet nicht automatisch die Fähigkeit zur Terminierung oder zum Debugging.

---

## Exit Information

Beim Prozessende können autorisierte Beobachter strukturierte Exit-Informationen erhalten.

Beispiel:

```text
ExitResult {
    reason
    code
    failureType
}
```

---

## Namespace-Unabhängigkeit

Spawn soll nicht zwingend einen global sichtbaren Prozessnamen erfordern.

Die primäre Referenz ist ein Process Handle beziehungsweise eine Domain Identity.

Menschlich lesbare Namen dienen:

- Administration,
- Diagnose,
- Introspection.

---

## Location Transparency

Langfristig kann dieselbe abstrakte Startanforderung auch eine Ausführung außerhalb des lokalen Prozesskontexts ermöglichen.

Beispiel:

```text
Execute capability X
```

könnte abhängig von höherer Policy lokal oder remote ausgeführt werden.

Diese ADR definiert jedoch den **lokalen nativen Prozess-Spawn**.

Remote Execution muss sichtbar andere Kosten- und Fehlermodelle besitzen.

---

## Resource Economy

Spawn ist in die systemweite Ressourcenökonomie integriert.

Vor Aktivierung können mindestens geprüft werden:

```text
memory availability
CPU budget
handle limits
required device resources
accelerator availability
```

Der erfolgreiche Prozessstart darf nicht als Zusage unbegrenzter zukünftiger Ressourcen interpretiert werden.

---

## Admission Control

Für harte Ressourcenanforderungen kann Admission Control den Spawn ablehnen.

Beispiel:

```text
Process requires:
    2 GiB guaranteed memory

Available:
    1 GiB

Result:
    ResourceUnavailable
```

Statt den Prozess mit unerfüllbaren Garantien zu starten.

---

## Auditing

Sicherheitsrelevante Spawn-Ereignisse sollen auditierbar sein.

Beispiele:

```text
SpawnRequested
SpawnDenied
ProcessCreated
CapabilityDelegated
ElevatedSpawnAuthorized
```

Auditdaten müssen den geltenden Datenschutz- und Data-Sovereignty-Regeln entsprechen.

---

## Introspection

Ein laufender Prozess soll nachvollziehbar machen können, wie er gestartet wurde, soweit dies sicher offengelegt werden darf.

Beispiel:

```text
Process:
    nova.editor.worker

Spawned by:
    nova.editor

Lifecycle:
    BoundToApplication

Capabilities:
    File.Read
    Graphics.Render

Resource Budget:
    CPU 10%
    Memory 512 MiB

Executable:
    Verified
```

---

## Spawn Decision Record

Für komplexe Systemstarts kann ein strukturierter Spawn Decision Record verwendet werden.

Konzeptionell:

```text
SpawnDecision {
    executable
    requester
    grantedCapabilities
    rejectedCapabilities
    resourceAllocation
    lifecyclePolicy
    trustResult
}
```

Dies unterstützt Architecture Introspection.

---

## Fail-Safe Defaults

Unklare Authority wird nicht übertragen.

Unklare Ressourcenanforderungen gelten nicht als garantiert.

Nicht validierbare Executables werden nicht gestartet.

Nicht erfüllbare Hard Constraints führen zu:

```text
Spawn Failure
```

statt zu einem stillschweigend schwächeren Prozess.

---

## Keine KI-Abhängigkeit

Spawn ist grundlegende Kernel- und Prozessinfrastruktur.

Die vollständige Prozesserstellung muss ohne KI funktionieren.

KI darf optional:

- Ressourcengrößen prognostizieren,
- Schedulingpräferenzen vorschlagen,
- ungewöhnliche Starts erkennen.

KI darf niemals die autoritative Entscheidung über grundlegende:

- Capabilities,
- Trust,
- Isolation,
- Hard Resource Contracts

ersetzen.

---

## Normative Anforderungen

1. NovaOS MUSS Spawn als bevorzugtes natives Prozessstartmodell verwenden.
2. Spawn MUSS einen neuen Process Domain direkt erzeugen können.
3. Spawn DARF NICHT die vollständige Duplizierung des Parent-Prozesses voraussetzen.
4. Der neue Prozess MUSS einen eigenen definierten Adressraum erhalten.
5. Der Prozessstart MUSS über eine explizite Startbeschreibung modellierbar sein.
6. Executable, Argumente und Environment MÜSSEN explizit festlegbar sein.
7. Capability-Übertragung MUSS explizit erfolgen.
8. Spawn DARF KEINE Authority Amplification ermöglichen.
9. Handle-Übertragung MUSS explizit und einschränkbar sein.
10. Child-Prozesse MÜSSEN mit reduzierten Handle-Rechten erzeugbar sein.
11. Ressourcenbudgets MÜSSEN beim Spawn angegeben werden können.
12. Hard Resource Requirements MÜSSEN vor Aktivierung validiert beziehungsweise reserviert werden können.
13. Spawn MUSS einen definierten Commit Point besitzen.
14. Fehler vor dem Commit Point MÜSSEN kontrolliert rückabwickelbar sein.
15. Ein fehlgeschlagener Spawn DARF KEINEN regulär laufenden Restprozess hinterlassen.
16. Initial Threads DÜRFEN erst nach erfolgreicher Vorbereitung ausführbar werden.
17. Lifecycle-Beziehungen zwischen Parent und Child MÜSSEN explizit modellierbar sein.
18. Unabhängige und Parent-gebundene Prozesse MÜSSEN unterstützt werden können.
19. Initiale IPC-Endpunkte MÜSSEN direkt beim Spawn übertragbar sein.
20. Spawn MUSS mit Structured Concurrency integrierbar sein.
21. Sandbox-, Service-, Driver- und Agent-Prozesse MÜSSEN über dasselbe grundlegende Spawn-Modell erzeugbar sein.
22. Kompatibilitäts-APIs MÜSSEN auf den nativen Spawn-Mechanismus abbildbar sein.
23. POSIX-`fork()` DARF als Kompatibilitätsfunktion existieren, MUSS jedoch NICHT das native Startmodell bestimmen.
24. Multithreaded Parent-Prozesse DÜRFEN beim Spawn keinen impliziten vollständigen Thread-/Runtime-Zustand übertragen.
25. GPU-, NPU- und vergleichbarer externer Zustand DARF NICHT implizit dupliziert werden.
26. Prozessstart MUSS typisierte Fehler liefern können.
27. Erfolgreicher Spawn MUSS eine sichere Prozessreferenz liefern können.
28. Spawn-Vorgänge und relevante Authority-Transfers SOLLEN auditierbar und introspektierbar sein.
29. Nicht erfüllbare Hard Constraints DÜRFEN NICHT stillschweigend abgeschwächt werden.
30. Die vollständige native Spawn-Funktionalität MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- klarere Prozessstartsemantik,
- keine unnötige Parent-Prozessduplizierung,
- bessere Eignung für multithreaded Anwendungen,
- Capability- und Ressourcenvergabe erfolgt von Anfang an explizit,
- leichter transaktional implementierbarer Prozessstart,
- gute Integration mit Structured Concurrency,
- einfachere Sandbox-Erstellung,
- bessere Unterstützung großer Adressräume und moderner Runtime-Systeme,
- GPU-/NPU- und andere externe Zustände müssen nicht künstlich forkfähig gemacht werden,
- native Architektur bleibt unabhängig von POSIX-Historie.

### Negative Konsequenzen

- POSIX-Software mit `fork()` benötigt eine Kompatibilitätsschicht,
- Programme, die stark von Fork-Semantik abhängen, sind nicht direkt auf die native API übertragbar,
- SpawnRequest und Resource-/Capability-Beschreibung benötigen eine reichhaltigere API,
- Child-Erzeugung erfordert explizitere Entscheidungen durch Runtime und Anwendung.

---

## Verworfene Alternativen

### `fork()` + `exec()` als natives Standardmodell

Verworfen.

Das Modell erzeugt zunächst einen abgeleiteten Prozesszustand, obwohl häufig direkt ein anderes Executable gestartet werden soll.

Es passt schlechter zu:

- Capability Security,
- Structured Concurrency,
- großen multithreaded Runtimes,
- expliziten Ressourcenbudgets,
- GPU-/NPU-Kontexten.

### Vollständiges Prozess-Cloning als einzige API

Verworfen.

Cloning ist für bestimmte Spezialfälle sinnvoll, soll aber nicht die Semantik jeder Prozesserstellung bestimmen.

### Automatische vollständige Parent-Vererbung

Verworfen.

Dies widerspricht Least Authority und expliziter Ressourcenverwaltung.

### Prozess starten und anschließend Rechte entfernen

Verworfen.

Der Prozess könnte bereits während des Übergangs zu starke Authority besitzen.

Bevorzugt wird:

```text
construct with correct authority
```

statt:

```text
start privileged
then restrict
```

### Jede Prozessart besitzt eigene Startmechanismen

Verworfen.

Anwendungen, Services, Driver, Sandboxes und Agents sollen auf derselben grundlegenden Spawn-Architektur aufbauen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-SPAWN-0001`
- `NPSPEC-PROCESS-SPAWN-REQUEST-0001`
- `NPSPEC-PROCESS-SPAWN-LIFECYCLE-0001`
- `NPSPEC-PROCESS-SPAWN-CAPABILITY-0001`
- `NPSPEC-PROCESS-SPAWN-HANDLE-0001`
- `NPSPEC-PROCESS-SPAWN-RESOURCE-0001`
- `NPSPEC-PROCESS-SPAWN-EXECUTABLE-0001`
- `NPSPEC-PROCESS-SPAWN-IPC-0001`
- `NPSPEC-PROCESS-SPAWN-SECURITY-0001`
- `NPSPEC-PROCESS-SPAWN-DETERMINISTIC-0001`
- `NPSPEC-PROCESS-CLONE-0001`
- `NPSPEC-PROCESS-FORK-COMPAT-0001`
- `NPSPEC-PROCESS-SPAWN-INTROSPECTION-0001`
- `NPSPEC-PROCESS-SPAWN-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- einfacher Spawn eines nativen Prozesses,
- neuer unabhängiger Adressraum,
- strukturierte Argumentübergabe,
- Environment-Übergabe,
- explizite Capability-Delegation,
- reduzierte Child-Capabilities,
- Versuch einer Authority Amplification,
- explizite Handle-Übertragung,
- eingeschränkter Handle-Transfer,
- Resource Budget beim Spawn,
- fehlgeschlagene Hard Resource Reservation,
- Trust-Rejection des Executables,
- Image-Load-Fehler vor Aktivierung,
- vollständiger Rollback nach fehlgeschlagenem Spawn,
- keine sichtbar laufende Prozessleiche,
- Initial Thread erst nach Commit,
- Independent Child,
- Parent-bound Child,
- Application-bound Child,
- IPC Bootstrap beim Spawn,
- Sandbox Spawn,
- System Service Spawn,
- User-Mode-Driver Spawn,
- Agent Spawn,
- Spawn aus multithreaded Parent,
- keine implizite GPU-/NPU-Kontextkopie,
- Deterministic Spawn,
- sichere ProcessReference,
- POSIX-Fork-Kompatibilität unabhängig vom nativen Spawn,
- Audit und Introspection.

---

## Ergebnis

NovaOS verwendet Spawn als natives Standardmodell für die Prozesserstellung.

Ein Prozess wird direkt aus einer expliziten Beschreibung konstruiert:

```text
Spawn Request
     ↓
Executable
Authority
Resources
Lifecycle
Security
     ↓
Validate
     ↓
Prepare
     ↓
Create Process Domain
     ↓
Activate
```

Dadurch werden Prozesse von Beginn an mit genau dem Zustand erzeugt, den sie tatsächlich benötigen.

NovaOS muss nicht zuerst einen bestehenden Prozess kopieren, um diese Kopie anschließend wieder umzubauen.

Die zentrale Architekturregel lautet:

```text
Spawn explicitly.

Delegate only what is required.

Reserve before activation.

Construct the intended process directly.

Clone only when cloning is actually intended.
```