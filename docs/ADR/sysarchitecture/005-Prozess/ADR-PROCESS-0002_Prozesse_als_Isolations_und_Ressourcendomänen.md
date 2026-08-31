# ADR-PROCESS-0002 – Prozesse als Isolations- und Ressourcendomänen

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Isolation / Ressourcen / Scheduling / Sicherheit

## Kontext

`ADR-PROCESS-0001_Natives_Execution_Domain_Modell` definiert das Execution Domain als native systemweite Ausführungseinheit.

Darauf aufbauend benötigt NovaOS eine klare Definition dessen, was ein klassischer Prozess innerhalb dieses Modells bedeutet.

In traditionellen Betriebssystemen ist ein Prozess häufig vor allem:

- ein Adressraum,
- eine Sammlung von Threads,
- eine PID,
- ein Handle-Container.

Für NovaOS reicht diese Sicht nicht aus.

Ein Prozess muss zusätzlich als klar kontrollierbare Grenze für folgende Eigenschaften dienen:

- Speicherisolation,
- Ressourcenverbrauch,
- Capability-Zuweisung,
- Scheduling,
- Fehlerbegrenzung,
- Lifecycle,
- Accounting,
- Audit,
- Data-Sovereignty-Regeln,
- Trust-Kontext.

Dabei soll ein Prozess nicht zur universellen Abstraktion für jede Art von Ausführung werden.

Das Execution Domain bleibt die allgemeinere primitive Architektur.

---

## Entscheidung

NovaOS definiert einen nativen Prozess als **spezialisierte Execution Domain mit eigener Isolations- und Ressourcengrenze**.

Konzeptionell:

```text
Execution Domain
      ↓
Process Domain
      │
      ├── Virtual Address Space
      ├── Resource Account
      ├── Capability Set
      ├── Security Context
      ├── Trust Context
      ├── Process Lifecycle
      └── Threads / Tasks
```

Ein Prozess ist damit nicht nur ein Container für Threads, sondern eine explizite Grenze für:

```text
Isolation
Authority
Resources
Failure Containment
Lifecycle
```

---

## Grundprinzip

```text
A process is an execution domain
with strong isolation and explicit resource ownership.
```

---

## Beziehung zum Execution Domain

Jeder native Prozess ist ein Execution Domain.

Nicht jedes Execution Domain ist jedoch zwingend ein Prozess.

```text
Execution Domain
    ├── Process
    ├── Driver Domain
    ├── Kernel Service Domain
    ├── Sandbox Domain
    └── weitere Domain-Typen
```

Damit bleiben Prozesssemantik und allgemeine Ausführungsarchitektur getrennt.

---

## Prozess als Isolationsdomäne

Ein Prozess bildet standardmäßig eine eigene Hardware-gestützte Speicherisolation.

Typischerweise:

```text
Process A
    ↓
Virtual Address Space A

Process B
    ↓
Virtual Address Space B
```

Ein Prozess darf nicht ohne explizite Autorisierung auf privaten Speicher eines anderen Prozesses zugreifen.

---

## Virtueller Adressraum

Jeder normale User-Mode-Prozess besitzt einen eigenen virtuellen Adressraum.

Dieser enthält mindestens logisch getrennte Bereiche für:

- ausführbaren Code,
- Daten,
- Heap,
- Thread Stacks,
- Shared Mappings,
- Kernel Interface Mappings.

Die genaue Speicherarchitektur wird durch die Memory-ADRs spezifiziert.

---

## Kernel Address Space

Der Kernel darf in Prozessadressräume über einen geschützten gemeinsamen oder getrennten Kernelbereich eingebunden werden.

Die konkrete Strategie kann architekturabhängig sein.

Mögliche Implementierungen:

```text
shared kernel mapping
separate kernel page tables
hybrid mapping
```

Die Prozessabstraktion darf davon unabhängig bleiben.

---

## Speicherfreigabe

Bei Prozessbeendigung müssen alle prozesseigenen privaten Speicherressourcen freigegeben werden.

Shared Memory folgt seiner eigenen Ownership- und Lifetime-Semantik.

---

## Prozess als Ressourcendomäne

Jeder Prozess besitzt einen expliziten Resource Account.

Dieser kann mindestens enthalten:

```text
CPU usage
Memory usage
I/O usage
Network usage
GPU usage
NPU usage
Handle count
IPC usage
```

Der Prozess wird damit zu einer natürlichen Accounting-Grenze.

---

## Resource Budget

Ein Prozess kann explizite Ressourcenbudgets besitzen.

Beispiel:

```text
ProcessResourceBudget {
    cpu
    memory
    storageIo
    network
    gpu
    npu
}
```

Budgets können:

- hart,
- weich,
- reserviert,
- gewichtet

sein.

Die genaue Semantik wird durch die systemweite Ressourcenökonomie definiert.

---

## Ressourcenvererbung

Ein neu gestarteter Prozess übernimmt nicht automatisch unbegrenzte Ressourcenrechte seines Elternprozesses.

Ressourcenbudgets müssen explizit:

- zugewiesen,
- reserviert,
- geteilt,
- delegiert

werden.

---

## Hierarchische Ressourcen

Prozesse dürfen Teil hierarchischer Ressourcengruppen sein.

Beispiel:

```text
Application
    ↓
Process Group
    ├── Main Process
    ├── Renderer Process
    └── Worker Process
```

Ein gemeinsames Anwendungsbudget kann auf einzelne Prozesse verteilt werden.

---

## CPU Accounting

CPU-Zeit aller Threads eines Prozesses wird dem Prozess zugerechnet.

Konzeptionell:

```text
Thread 1 CPU
Thread 2 CPU
Thread 3 CPU
      ↓
Process CPU Account
```

Dies gilt unabhängig davon, auf welchen CPUs die Threads ausgeführt werden.

---

## Speicher-Accounting

Dem Prozess werden seine privaten und verantworteten Speicherressourcen zugerechnet.

Shared Memory benötigt eine definierte Accounting-Policy.

Mögliche Modelle sind:

```text
Owner pays
Shared proportional
Shared resource domain
```

Die konkrete Policy wird separat spezifiziert.

---

## I/O Accounting

I/O-Aktivitäten müssen dem verursachenden Prozess beziehungsweise Resource Scope zugeordnet werden können.

Beispiele:

```text
Storage Reads
Storage Writes
Network Traffic
Device Operations
```

Async I/O verliert seine ursprüngliche Ressourcenverantwortung nicht dadurch, dass die Verarbeitung später durch Kernelworker erfolgt.

---

## Resource Attribution

Deferred Work oder Kernelarbeit im Auftrag eines Prozesses soll, soweit sinnvoll, dem verursachenden Prozess zugerechnet werden.

Beispiel:

```text
Process
   ↓
Async Storage Request
   ↓
Kernel Worker
   ↓
Device I/O
```

Die Resource Economy soll diese Arbeit nicht fälschlicherweise vollständig dem Kernelworker zuschreiben.

---

## Scheduling Domain

Ein Prozess kann Scheduling-Eigenschaften besitzen.

Beispiele:

```text
priority class
CPU weight
CPU budget
CPU set
latency class
deadline constraints
```

Threads können diese Werte verfeinern, dürfen harte Prozessgrenzen jedoch nicht überschreiten.

---

## CPU Sets

Ein Prozess kann auf ein bestimmtes CPU Set eingeschränkt werden.

```text
Process Allowed CPUs:
    {2,3,4,5}
```

Alle normalen Threads dieses Prozesses müssen diese Grenze respektieren.

---

## Threads

Threads gehören genau einem Prozess beziehungsweise dem zugrunde liegenden Process Domain.

Sie teilen standardmäßig:

- Adressraum,
- Capability-Kontext,
- Prozessressourcen,
- Prozessidentität.

Threads besitzen dennoch eigene:

- Registerzustände,
- Stacks,
- Schedulingzustände,
- lokale Execution Scopes.

---

## Prozess und Capability Set

Ein Prozess besitzt ein explizites Capability Set.

Beispiel:

```text
Process
  ↓
Capabilities
  ├── File.Read
  ├── Network.Connect
  └── Graphics.Render
```

Ein Prozess darf nur Operationen ausführen, für die ausreichende Authority vorhanden ist.

---

## Keine implizite Vollmacht

Die Tatsache, dass ein Prozess:

- vom System gestartet wurde,
- eine hohe Priorität besitzt,
- signiert ist,
- einem privilegierten Benutzer gehört

darf nicht automatisch unbegrenzte Systemautorität erzeugen.

Authority bleibt capability-basiert.

---

## Process Creation

Prozesserstellung folgt dem Domain-Erstellungsmodell.

Konzeptionell:

```text
Create Process Request
        ↓
Validate Executable
        ↓
Evaluate Trust
        ↓
Create Process Domain
        ↓
Create Address Space
        ↓
Assign Capabilities
        ↓
Assign Resource Budget
        ↓
Create Root Execution Scope
        ↓
Create Initial Thread
        ↓
Run
```

---

## Initial Thread

Ein Prozess kann mit einem Initial Thread gestartet werden.

Der Prozess selbst ist jedoch nicht mit diesem Thread identisch.

```text
Process
    ↓
Initial Thread
```

Das Ende des Initial Threads muss nicht zwingend automatisch das Ende des gesamten Prozesses bedeuten, sofern weitere Threads vorhanden sind und die Prozesssemantik dies erlaubt.

---

## Process Spawn

Ein Prozess kann einen neuen Prozess starten, wenn er dafür die notwendige Capability besitzt.

Beispiel:

```text
Process.Spawn
```

Beim Spawn werden explizit festgelegt:

- ausführbares Objekt,
- Argumente,
- Environment,
- Capabilities,
- Ressourcen,
- IPC-Verbindungen,
- Startmodus.

---

## Prozessvererbung

NovaOS soll keine unkontrollierte implizite Vererbung sämtlicher Handles, Capabilities und Ressourcen verwenden.

Vererbung muss explizit sein.

Konzeptionell:

```text
Parent
    ↓
Spawn Description
    ↓
Explicit Delegation
    ↓
Child
```

---

## Environment

Ein Prozess kann ein Environment besitzen.

Dieses ist eine Laufzeitkonfiguration und keine Sicherheitsgrenze.

Environment-Werte dürfen keine Capabilities ersetzen.

Beispiel:

```text
ENV_ALLOW_NETWORK=true
```

erteilt keine Netzwerkberechtigung.

---

## Handles

Ein Prozess besitzt einen kontrollierten Handle Namespace.

Handles referenzieren beispielsweise:

- Dateien,
- IPC Endpoints,
- Shared Memory,
- Devices,
- Events,
- Prozesse.

Ein Handle ist nur innerhalb seines gültigen Authority-Kontexts nutzbar.

---

## Handle Lifetime

Handles werden beim Prozessende geschlossen beziehungsweise entsprechend ihrer Ownership-Semantik freigegeben.

Ein Handle darf nicht auf bereits zerstörte Kernelobjekte verweisen.

---

## IPC

Prozesse kommunizieren über kontrollierte IPC-Mechanismen.

IPC darf nicht voraussetzen, dass beide Prozesse denselben Adressraum teilen.

Beispiel:

```text
Process A
   ↓
IPC Endpoint
   ↓
Process B
```

IPC-Endpunkte selbst können Capability-geschützt sein.

---

## Shared Memory

Shared Memory ist eine explizite Ausnahme von vollständiger Speichertrennung.

Es muss genau festgelegt werden:

```text
Who may map?
Read?
Write?
Execute?
How long?
Who owns?
```

Shared Memory hebt die restliche Prozessisolation nicht auf.

---

## Zero-Copy

Shared Memory und Ownership Transfer können für Zero-Copy-Kommunikation genutzt werden.

Beispiel:

```text
Process A
   ↓
Buffer Ownership Transfer
   ↓
Process B
```

Die Übertragung muss Security-, Data- und Lifetime-Regeln einhalten.

---

## Prozessgruppen

NovaOS soll logische Gruppen mehrerer Prozesse unterstützen können.

Mögliche Verwendungen:

- eine Anwendung mit mehreren Prozessen,
- Job,
- Service Group,
- Sandbox,
- Resource Group.

Prozessgruppen ersetzen jedoch nicht die Identität einzelner Prozesse.

---

## Application Boundary

Eine Anwendung darf aus mehreren Prozessen bestehen.

Beispiel:

```text
Application
    ├── UI Process
    ├── Content Process
    ├── Worker Process
    └── Extension Process
```

Daher gilt:

```text
Application != Process
```

Die Application-Architektur wird separat definiert.

---

## Failure Containment

Ein Prozessabsturz soll primär innerhalb seiner Isolationsdomäne bleiben.

Ein User-Mode-Prozess darf durch:

- ungültige Speicherzugriffe,
- Stackfehler,
- fehlerhafte Instruktionen

keinen fremden Prozess oder Kernelzustand direkt beschädigen können.

---

## Process Fault

Ein schwerwiegender Fehler kann den Prozess in einen Failure State überführen.

Beispiele:

```text
AccessViolation
IllegalInstruction
UnhandledException
ResourceViolation
SecurityViolation
```

Der Fehler muss dem betroffenen Prozess eindeutig zugeordnet werden.

---

## Crash Reporting

NovaOS kann bei einem Prozessabsturz strukturierte Diagnosedaten erfassen.

Beispiele:

```text
process identity
executable identity
fault type
thread
instruction location
resource state
relevant audit data
```

Sensitive Inhalte dürfen dabei nicht unkontrolliert offengelegt werden.

---

## Prozessbeendigung

Ein Prozess kann enden durch:

```text
NormalExit
RequestedTermination
UnhandledFailure
ResourceEnforcement
SecurityEnforcement
SystemShutdown
```

Der Beendigungsgrund muss typisiert sein.

---

## Kontrollierte Terminierung

Der bevorzugte Ablauf lautet:

```text
Mark Terminating
      ↓
Reject new external work
      ↓
Cancel execution scopes
      ↓
Wake / stop threads
      ↓
Drain required operations
      ↓
Revoke capabilities
      ↓
Release resources
      ↓
Destroyed
```

Nicht jeder Fehlerfall erlaubt sämtliche Schritte.

---

## Forced Termination

Bei nicht kooperierendem oder beschädigtem Prozess muss der Kernel eine erzwungene Beendigung durchführen können.

Dabei gilt:

```text
process safety
<
system safety
```

Der Kernel darf nicht unbegrenzt auf einen fehlerhaften Prozess warten.

---

## External Resources

Prozesse können Operationen ausgelöst haben, die außerhalb ihres eigenen Adressraums weiterlaufen.

Beispiele:

- Storage I/O,
- Netzwerkoperationen,
- GPU Commands,
- Remote Requests.

Bei Prozessende muss definiert werden, ob diese:

```text
cancel
complete
detach with explicit ownership transfer
```

werden.

Unkontrollierte verwaiste Operationen sind nicht zulässig.

---

## Structured Concurrency

Prozessinterne Aufgaben sollen über einen Root Execution Scope hierarchisch organisiert sein.

```text
Process
   ↓
Root Scope
   ↓
Child Scopes
   ↓
Tasks
```

Dadurch kann Prozessbeendigung systematisch auf untergeordnete Arbeit propagiert werden.

---

## Resource Limit Enforcement

Bei Überschreiten eines Ressourcenlimits muss die Policy explizit reagieren.

Mögliche Reaktionen:

```text
Throttle
Deny allocation
Degrade
Suspend
Terminate
```

Die konkrete Reaktion hängt von Ressource und Policy ab.

---

## Memory Exhaustion

Ein Prozess, der sein Speicherbudget überschreitet, soll nicht automatisch das gesamte System in einen globalen Out-of-Memory-Zustand bringen.

Mögliche Maßnahmen:

```text
deny new allocation
reclaim process-local resources
notify process
terminate process
```

Systemweite Mechanismen werden separat spezifiziert.

---

## CPU Overuse

Bei Überschreiten eines weichen CPU-Budgets kann der Scheduler den Prozess drosseln.

Hard-Realtime- oder reservierte Ressourcen folgen eigenen Policies.

---

## Resource Reservation

Ein Prozess kann Ressourcen reservieren, wenn:

- entsprechende Capability vorhanden ist,
- Resource Economy zustimmt,
- die Reservierung verfügbar ist.

Eine angeforderte Reservierung ist nicht automatisch garantiert.

---

## Deterministic Mode

Prozesse können in einem deterministischen Execution Scope betrieben werden.

Dabei können stabilisiert werden:

- CPU-Zuordnung,
- Schedulingparameter,
- Ressourcenlimits,
- Zeitquellen,
- erlaubte externe Einflüsse.

Die Prozessgrenze bietet dafür einen klaren Kontrollbereich.

---

## Data Sovereignty

Data-Sovereignty-Regeln können auf Prozessebene wirksam sein.

Beispiel:

```text
Process:
    may process medical data

Policy:
    local device only
    no external network
```

Netzwerk- oder IPC-Zugriff allein darf diese Regeln nicht umgehen.

---

## Trust

Der Prozess kann Trust-Informationen des ausgeführten Artefakts tragen.

Beispiel:

```text
Executable:
    signed
    provenance verified

Process Trust:
    Verified
```

Trust ist jedoch nur ein Eingang in Policyentscheidungen.

Er ist kein Ersatz für Capabilities.

---

## Sandboxing

Eine Sandbox kann einen oder mehrere Prozesse umfassen.

Ein Sandboxprozess bleibt dennoch ein normaler Process Domain mit besonders eingeschränkten:

- Capabilities,
- Ressourcen,
- IPC-Verbindungen,
- Data Policies.

---

## Kompatibilität

POSIX- und Win32-Prozesssemantik können auf das native Prozessmodell abgebildet werden.

Beispiel:

```text
POSIX Process API
       ↓
Compatibility Layer
       ↓
Native Nova Process Domain
```

Native Kernelstrukturen werden nicht nach historischen Fremd-ABI-Annahmen entworfen.

---

## Prozess-ID

Native Prozesse erhalten eine Process ID für Administration und API-Nutzung.

Zusätzlich besitzt das zugrunde liegende Execution Domain seine systemweite Domain Identity.

Die IDs dürfen intern unterschiedlich sein.

---

## PID-Wiederverwendung

Process IDs dürfen nach Prozessende wiederverwendet werden.

Eine PID allein darf deshalb nicht als dauerhafte globale Identität dienen.

Für robuste Referenzen muss eine generations- oder objektbasierte Identität verwendet werden.

Beispiel:

```text
ProcessReference {
    pid
    generation
}
```

oder eine äquivalente native Domain-ID.

---

## Prozessreferenzen

Kernelobjekte dürfen nicht ausschließlich über rohe numerische PIDs auf Prozesse verweisen.

Referenzen müssen gegen PID-Reuse geschützt sein.

---

## Parent / Child

NovaOS kann Parent-/Child-Beziehungen für Prozesse darstellen.

Diese Beziehung ist jedoch:

- Lifecycle-Metadatum,
- Verwaltungsinformation,
- mögliche Ressourcenzuordnung.

Sie erzeugt nicht automatisch Sicherheitsautorität.

---

## Orphan-Prozesse

Das Ende eines Parent-Prozesses erzwingt nicht automatisch das Ende sämtlicher Child-Prozesse.

Die gewünschte Lifecycle-Beziehung muss explizit sein.

Beispiele:

```text
Independent
BoundToParent
BoundToApplication
BoundToJob
```

---

## Prozesszustände

Ein Prozess besitzt einen expliziten Lifecycle.

Mindestens:

```text
Created
Prepared
Runnable
Running
Suspended
Terminating
Terminated
Failed
```

Die interne Schedulerzustände einzelner Threads bleiben davon getrennt.

---

## Suspend

Ein Prozess kann kontrolliert suspendiert werden.

Dabei werden seine Threads nicht mehr regulär ausgeführt.

Externe Ressourcenoperationen müssen eine definierte Suspend-Semantik besitzen.

Suspend ist nicht automatisch gleichbedeutend mit vollständigem Geräte- oder I/O-Stopp.

---

## Resume

Beim Resume muss geprüft werden, ob:

- benötigte Capabilities noch gültig sind,
- Ressourcen noch vorhanden sind,
- externe Abhängigkeiten noch existieren.

Ein Prozess darf nicht blind in einen ungültigen Zustand zurückkehren.

---

## Checkpoint und zukünftige Migration

Das Prozessmodell soll zukünftige Checkpoint-, Restore- oder Migrationstechniken nicht verhindern.

Dazu müssen prozessrelevante Zustände klar identifizierbar sein.

Dies bedeutet nicht, dass jeder Prozess automatisch migrierbar ist.

---

## Hot Replacement

Ein laufender Prozess kann zukünftig Teil von Hot-Replacement-Mechanismen sein.

Der Austausch erfolgt jedoch über kontrollierte:

```text
Quiesce
State Transfer
Rebind
Activate
Verify
```

Mechanismen und nicht durch unkontrollierten Austausch von Adressräumen.

---

## Architecture Introspection

Prozesse müssen systemweit introspektierbar sein.

Beispiel:

```text
Process:
    1842

Domain:
    domain:7f21...

Executable:
    de.nova.editor

State:
    Running

Threads:
    12

Memory:
    310 MiB

CPU:
    3.2 %

Capabilities:
    24

Trust:
    Verified

Resource Group:
    nova.editor
```

Sichtbarkeit einzelner Informationen hängt von Berechtigungen ab.

---

## Audit

Relevante Prozessereignisse sollen auditierbar sein.

Beispiele:

```text
Created
Started
CapabilityGranted
ResourceLimitChanged
Suspended
Terminated
Crashed
```

Audit dient Nachvollziehbarkeit und Sicherheit, nicht der Offenlegung interner Reasoning-Prozesse.

---

## Fail-Safe Defaults

Ist unklar, ob ein Prozess eine Ressource oder Capability erhalten darf, wird sie nicht gewährt.

Ist eine sichere Isolation nicht herstellbar, darf der Prozess nicht mit stärkeren Rechten als vorgesehen gestartet werden.

---

## KI-Unabhängigkeit

Prozessisolation und Ressourcenverwaltung sind grundlegende Kernelmechanismen.

Sie müssen vollständig ohne KI funktionieren.

KI darf optional:

- Ressourcenbedarf prognostizieren,
- Anomalien erkennen,
- Optimierungen vorschlagen.

Sie darf niemals die notwendige Isolation oder Capability-Prüfung ersetzen.

---

## Normative Anforderungen

1. Ein nativer NovaOS-Prozess MUSS als spezialisierte Execution Domain modelliert werden.
2. Jeder normale User-Mode-Prozess MUSS eine eigene Speicherisolationsdomäne besitzen.
3. Prozesse MÜSSEN eine explizite Ressourcendomäne besitzen.
4. CPU-, Speicher- und I/O-Verbrauch MÜSSEN einem Prozess zurechenbar sein.
5. Async Kernelarbeit SOLL soweit möglich dem verursachenden Prozess zugerechnet werden.
6. Prozesse MÜSSEN explizite Ressourcenbudgets unterstützen können.
7. Threads MÜSSEN einem Prozess eindeutig zugeordnet sein.
8. Prozessgrenzen MÜSSEN Capability-Grenzen unterstützen.
9. Trust DARF Capability-Prüfungen NICHT ersetzen.
10. Parent-Prozesse DÜRFEN Capabilities NICHT automatisch vollständig an Child-Prozesse vererben.
11. Prozess-Spawn MUSS explizite Capability- und Ressourcenübertragung unterstützen.
12. Prozesshandles MÜSSEN kontrollierte Lifetime-Regeln besitzen.
13. Shared Memory MUSS explizit autorisiert und geschützt werden.
14. Shared Memory DARF die übrige Prozessisolation NICHT implizit aufheben.
15. Prozesse MÜSSEN CPU-Affinity und Schedulinggrenzen unterstützen können.
16. Prozessinterne Threads DÜRFEN harte Prozessressourcengrenzen NICHT umgehen.
17. Prozessbeendigung MUSS zugehörige private Ressourcen freigeben.
18. Ausstehende externe Operationen MÜSSEN bei Prozessende explizit behandelt werden.
19. Structured Concurrency MUSS die kontrollierte Beendigung untergeordneter Arbeit ermöglichen.
20. Prozesse MÜSSEN typisierte Failure- und Exit-Zustände besitzen.
21. Ein User-Mode-Prozessfehler DARF fremde Prozesse NICHT direkt beschädigen.
22. Ressourcenlimitüberschreitungen MÜSSEN durch definierte Policies behandelt werden.
23. PIDs DÜRFEN NICHT als einzige dauerhaft sichere Prozessidentität verwendet werden.
24. Prozessreferenzen MÜSSEN gegen PID-Wiederverwendung geschützt sein.
25. Parent-/Child-Beziehungen DÜRFEN NICHT automatisch Sicherheitsautorität erzeugen.
26. Prozesszustand, Ressourcenverbrauch und relevante Authority MÜSSEN introspektierbar sein.
27. Sensitive Introspection-Daten MÜSSEN berechtigungsabhängig geschützt werden.
28. Prozess-Lifecycle-Ereignisse SOLLEN auditierbar sein.
29. Kompatibilitäts-APIs MÜSSEN auf das native Prozessmodell abbildbar sein.
30. Prozessisolation und Ressourcenverwaltung MÜSSEN vollständig ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- klare Sicherheits- und Speichergrenzen,
- zuverlässige Ressourcenabrechnung,
- Prozesse lassen sich kontrolliert begrenzen,
- gute Grundlage für Multi-Prozess-Anwendungen,
- bessere Fehlerisolation,
- direkte Integration mit Capabilities und Resource Economy,
- POSIX- und Win32-Kompatibilität bleiben überlagerbar,
- Prozessmodell bleibt konsistent mit dem Execution-Domain-Konzept.

### Negative Konsequenzen

- Prozesserstellung benötigt mehr Kontext als klassische PID-plus-Adressraum-Modelle,
- Ressourcenattribution für asynchrone Kernelarbeit ist komplex,
- Shared Resources benötigen explizite Accounting-Regeln,
- Process Groups und Application Boundaries müssen zusätzlich modelliert werden.

---

## Verworfene Alternativen

### Prozess nur als Adressraum und Thread-Container

Verworfen.

Dies bildet Ressourcen-, Capability- und Lifecycle-Anforderungen von NovaOS nicht ausreichend ab.

### Ressourcen ausschließlich pro Thread verwalten

Verworfen.

Viele Ressourcen gehören semantisch zum gesamten Prozess beziehungsweise zur Anwendung.

### Alle Prozesse teilen einen gemeinsamen Adressraum

Verworfen.

Dies würde starke Fehler- und Sicherheitsisolation verhindern.

### Prozessidentität ausschließlich über PID

Verworfen.

PID-Reuse macht dies für langlebige sichere Referenzen ungeeignet.

### Signierte Prozesse automatisch privilegieren

Verworfen.

Vertrauen und Autorität müssen getrennt bleiben.

### Kompatibilitätsprozessmodell als native Basis

Verworfen.

POSIX oder Win32 dürfen die native NovaOS-Prozessarchitektur nicht bestimmen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-PROCESS-ADDRESSSPACE-0001`
- `NPSPEC-PROCESS-RESOURCE-DOMAIN-0001`
- `NPSPEC-PROCESS-ACCOUNTING-0001`
- `NPSPEC-PROCESS-BUDGET-0001`
- `NPSPEC-PROCESS-THREAD-0001`
- `NPSPEC-PROCESS-HANDLE-0001`
- `NPSPEC-PROCESS-SPAWN-0001`
- `NPSPEC-PROCESS-GROUP-0001`
- `NPSPEC-PROCESS-SHAREDMEMORY-0001`
- `NPSPEC-PROCESS-FAULT-0001`
- `NPSPEC-PROCESS-TERMINATION-0001`
- `NPSPEC-PROCESS-PID-0001`
- `NPSPEC-PROCESS-INTROSPECTION-0001`
- `NPSPEC-PROCESS-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- zwei Prozesse mit getrennten Adressräumen,
- unerlaubter Cross-Process-Speicherzugriff,
- mehrere Threads innerhalb eines Prozesses,
- CPU-Accounting über mehrere Threads,
- Speicher-Accounting,
- Async-I/O-Attribution,
- Prozess-CPU-Budget,
- Prozess-Memory-Budget,
- CPU-Affinity,
- expliziter Child-Spawn,
- eingeschränkte Capability-Vererbung,
- Handle-Vererbung nur bei expliziter Freigabe,
- Shared Memory zwischen zwei Prozessen,
- Zero-Copy-Buffertransfer,
- Prozessabsturz durch Access Violation,
- keine Beschädigung fremder Prozesse,
- normale Prozessbeendigung,
- Forced Termination,
- laufendes Async I/O bei Prozessende,
- Root-Scope-Cancellation,
- Resource Limit Enforcement,
- PID-Reuse,
- sichere Prozessreferenz trotz PID-Reuse,
- Parent-Ende bei unabhängigem Child,
- Parent-gebundener Child-Prozess,
- Suspend und Resume,
- Introspection,
- Audit.

---

## Ergebnis

NovaOS behandelt Prozesse nicht lediglich als Container für Threads, sondern als starke Isolations- und Ressourcendomänen innerhalb des allgemeineren Execution-Domain-Modells.

Das Architekturmodell lautet:

```text
Process
   │
   ├── Isolated Address Space
   ├── Capability Boundary
   ├── Resource Account
   ├── Resource Budget
   ├── Security / Trust Context
   ├── Lifecycle
   └── Root Execution Scope
           ↓
        Threads / Tasks
```

Damit erhält NovaOS eine klare Einheit für:

```text
memory isolation
resource ownership
authority
accounting
failure containment
```

Die zentrale Architekturregel lautet:

```text
A process owns its execution context.

A process pays for its resources.

A process is isolated by default.

Sharing is explicit.

Authority is delegated,
never assumed.
```