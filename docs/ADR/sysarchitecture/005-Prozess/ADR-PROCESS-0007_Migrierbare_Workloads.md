# ADR-PROCESS-0007 – Migrierbare Workloads

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Execution Domains / Migration / Scheduling / Ressourcen / Location Transparency / Recovery

## Kontext

`ADR-PROCESS-0006_Checkpointbare_Execution_Domains` definiert Checkpoint und Restore als strukturierte Mechanismen zur Erfassung und Wiederherstellung von Execution Domains.

Darauf aufbauend benötigt NovaOS ein Modell für Workloads, die ihren Ausführungsort wechseln können.

Mögliche Ziele sind:

- andere CPU,
- anderer NUMA-Knoten,
- anderer Accelerator,
- anderes Gerät,
- andere physische Maschine,
- andere vertrauenswürdige NovaOS-Instanz.

Migration soll dabei nicht als Spezialfunktion einzelner Anwendungen implementiert werden, sondern als systemweit kontrollierbare Fähigkeit.

Gleichzeitig darf NovaOS Location Transparency nicht so interpretieren, dass Kosten und Auswirkungen einer Migration unsichtbar werden.

Migration kann unter anderem beeinflussen:

- Latenz,
- Speicherlokalität,
- Netzwerkverkehr,
- Energieverbrauch,
- Trust,
- Data Sovereignty,
- Ressourcengarantie,
- Verfügbarkeit externer Geräte,
- deterministisches Verhalten.

Nicht jeder Prozess und nicht jede Ressource ist migrierbar.

---

## Entscheidung

NovaOS führt **migrierbare Workloads** als explizite Eigenschaft von Execution Domains und ihren Execution Contracts ein.

Migration ist eine koordinierte Systemoperation auf Basis von:

```text
Checkpoint
+
Transfer
+
Restore
+
Rebind
+
Verification
+
Ownership Transfer
```

Das Grundmodell lautet:

```text
Source Execution Domain
        ↓
Migration Planning
        ↓
Target Validation
        ↓
Checkpoint / State Transfer
        ↓
Target Restore
        ↓
Resource Rebinding
        ↓
Verification
        ↓
Ownership Switch
        ↓
Source Termination
```

Migration ist keine implizite Eigenschaft jedes Prozesses.

---

## Grundprinzip

```text
Move workloads only when their contracts can move.

Migration changes location,
not authority or correctness guarantees.
```

---

## Beziehung zu Checkpointing

Migration baut grundsätzlich auf der Checkpoint-/Restore-Semantik auf.

Es gilt:

```text
Migratable
    ⇒
Checkpointable
```

Für Live Migration dürfen Checkpoint- und Transfermechanismen überlappen.

---

## Migration Capability

Ein Execution Domain kann eine Migration Capability besitzen.

Beispielsweise:

```text
NonMigratable
LocalMigratable
HostMigratable
LiveMigratable
```

Diese Klassifikation beschreibt Möglichkeiten.

Die tatsächliche Migration bleibt eine Policyentscheidung.

---

## Workload

Der Begriff Workload bezeichnet die logisch zusammengehörige Ausführungseinheit, die migriert werden soll.

Dies kann sein:

```text
Execution Domain
Process
Process Group
Application Group
Service Instance
```

Die kleinste native Basiseinheit bleibt das Execution Domain.

---

## Migration Scope

Der Migration Scope muss explizit sein.

Ein einzelner Prozess darf nicht unabhängig migriert werden, wenn seine semantische Konsistenz zwingend andere Domains einschließt.

Beispiel:

```text
Application
    ├── Process A
    ├── Process B
    └── Shared Memory
```

Wenn A und B gemeinsamen nicht extern rekonstruierbaren Zustand besitzen, kann die gesamte Gruppe der erforderliche Migration Scope sein.

---

## Migration Contract

Migrationseigenschaften können Teil des `Nova.ExecutionContract` sein.

Konzeptionell:

```text
MigrationContract {
    migratable
    allowedLocations
    requiredArchitecture
    maximumDowntime
    maximumMigrationTime
    requiredResources
    localityRequirements
    trustRequirements
    dataPolicy
}
```

Hard Constraints dürfen nicht während der Migration abgeschwächt werden.

---

## Desired Location

Ein Workload kann eine gewünschte Ausführungsposition besitzen.

Beispiel:

```text
Preferred:
    NUMA node 2

Allowed:
    local host

Forbidden:
    remote host
```

oder:

```text
Preferred:
    GPU-capable node

Allowed:
    trusted cluster
```

---

## Location ist mehrdimensional

Location bedeutet nicht nur physischer Rechner.

Sie kann umfassen:

```text
Machine
NUMA Domain
CPU Set
Memory Domain
Accelerator
Security Domain
Network Zone
Geographic Region
```

Migration kann daher auch vollständig innerhalb eines Rechners stattfinden.

---

## Lokale Migration

Lokale Migration verändert den Ausführungsort innerhalb desselben Systems.

Beispiele:

```text
CPU Set A
    ↓
CPU Set B

NUMA Node 0
    ↓
NUMA Node 1

GPU 0
    ↓
GPU 1
```

Nicht jede lokale Migration benötigt vollständiges Checkpoint-to-Storage.

---

## Host Migration

Host Migration bewegt einen Workload zwischen NovaOS-Systemen.

Beispiel:

```text
Host A
    ↓
Host B
```

Dabei müssen zusätzliche Eigenschaften geprüft werden:

- Netzwerkpfad,
- Architektur,
- ABI,
- Trust,
- Ressourcen,
- Data Sovereignty,
- externe Abhängigkeiten.

---

## Cold Migration

Bei Cold Migration wird die Ausführung auf der Quelle gestoppt, bevor der Zustand übertragen wird.

```text
Quiesce Source
      ↓
Checkpoint
      ↓
Transfer
      ↓
Restore Target
      ↓
Activate Target
```

Vorteile:

- einfachere Konsistenz,
- geringere Split-Brain-Gefahr.

Nachteil:

- längere Downtime.

---

## Live Migration

Live Migration versucht, die Unterbrechung zu minimieren.

Konzeptionell:

```text
Source continues running
      ↓
Initial state transfer
      ↓
Dirty-state tracking
      ↓
Iterative synchronization
      ↓
Short final quiesce
      ↓
Transfer remainder
      ↓
Target activation
```

Live Migration ist eine Optimierung und keine allgemeine Korrektheitsvoraussetzung.

---

## Pre-Copy

NovaOS darf Pre-Copy-Migration unterstützen.

```text
Copy memory
    ↓
Track dirty pages
    ↓
Copy changed pages
    ↓
Repeat
    ↓
Final stop-and-copy
```

Pre-Copy eignet sich, wenn der Dirtying Rate beherrschbar ist.

---

## Post-Copy

Post-Copy darf ebenfalls unterstützt werden.

```text
Minimal execution state transferred
      ↓
Target starts
      ↓
Pages fetched on demand
```

Da Post-Copy stärkere Netzwerk- und Fehlertoleranzanforderungen besitzt, darf es nicht als universeller Standard vorausgesetzt werden.

---

## Hybrid Migration

NovaOS darf Pre-Copy und Post-Copy kombinieren.

Die Wahl ist Policy und hängt unter anderem von:

- Memory Size,
- Dirty Rate,
- Network Bandwidth,
- Downtime Requirement,
- Failure Risk

ab.

---

## Migration Planning

Vor jeder Migration wird ein Migration Plan erzeugt.

Konzeptionell:

```text
MigrationPlan {
    source
    target
    workload
    strategy
    requiredResources
    transferableResources
    reboundResources
    constraints
    rollbackPlan
}
```

Die Ausführung beginnt erst nach Validierung des Plans.

---

## Target Discovery

Migration darf nicht voraussetzen, dass jedes erreichbare System geeignet ist.

Ziele müssen anhand ihrer Eigenschaften bewertet werden.

Mindestens relevant:

```text
architecture
abi
resources
topology
providers
trust
data policy
network reachability
```

---

## Target Admission Control

Das Ziel muss den Workload akzeptieren.

Hard Requirements werden vor Migration geprüft beziehungsweise reserviert.

Beispiel:

```text
Required:
    4 GiB memory
    AVX-compatible CPU
    GPU capability X

Target:
    2 GiB memory

Result:
    MigrationRejected
```

---

## Ressourcenreservierung

Für migrationskritische Ressourcen soll das Ziel vor dem Ownership Switch Reservierungen durchführen können.

Dadurch wird verhindert:

```text
Source stopped
    ↓
Target lacks resources
```

---

## Architekturkompatibilität

Native Execution State ist grundsätzlich ISA-abhängig.

Ein Workload mit nativen x86_64-Threads kann nicht transparent als derselbe rohe Registerzustand auf ARM64 fortgesetzt werden.

Cross-Architecture Migration ist nur möglich, wenn eine höhere Ausführungsschicht dies unterstützt.

Beispiele:

```text
portable bytecode runtime
interpreter
managed runtime
architecture-independent checkpoint
```

---

## ABI-Kompatibilität

Das Ziel muss die vom Workload benötigten ABI- und Contract-Versionen unterstützen.

Eine Migration zwischen inkompatiblen NovaOS-Generationen benötigt einen expliziten State-/ABI-Adapter.

---

## Executable Identity

Das Ziel muss das benötigte Executable entweder:

- bereits besitzen,
- vertrauenswürdig beziehen,
- zusammen mit dem Workload übertragen dürfen.

Die ausführbare Identität muss verifiziert werden.

---

## Kein blindes Code Transfer

Migration darf nicht automatisch beliebigen ausführbaren Code in ein Zielsystem einschleusen.

Codeübertragung unterliegt:

```text
Identity
Signature
Provenance
Trust
Policy
```

---

## Capability Reevaluation

Capabilities werden beim Ziel neu validiert.

Eine Capability, die auf Host A gültig war, ist nicht automatisch auf Host B gültig.

Beispiel:

```text
Source:
    Device.Camera.Use

Target:
    no camera authority

Result:
    capability cannot be restored
```

Je nach Contract muss die Migration:

- fehlschlagen,
- degradieren,
- alternative Capability verwenden.

---

## Keine Authority Amplification

Migration darf niemals zusätzliche Authority allein durch einen Ortswechsel erzeugen.

Grundregel:

```text
Target Authority
    ⊆
Authorized Migrated Authority
    +
Explicit Target Grants
```

---

## Trust

Quelle, Ziel und Transferweg müssen entsprechend des Migration Contracts vertrauenswürdig sein.

Ein mögliches Modell lautet:

```text
Source Trust
    +
Target Trust
    +
Transport Trust
    +
Artifact Trust
```

Der geringste relevante Trust Level kann die Migration begrenzen.

---

## Data Sovereignty

Data Sovereignty besitzt höhere Priorität als Performancevorteile einer Migration.

Beispiele:

```text
Data = LocalOnly
```

verbietet:

```text
Remote Host Migration
```

oder:

```text
AllowedRegion = EU
```

verbietet Ziele außerhalb dieses Bereichs.

---

## Datenklassifikation

Der Migration Planner muss die für den Workload relevante Datenklassifikation berücksichtigen.

Dazu gehören nicht nur Dateien, sondern auch:

- RAM-Inhalte,
- Shared Memory,
- Checkpoints,
- Cache-Inhalte,
- IPC-Nachrichten.

---

## Transfer Security

Hostübergreifender Zustandstransfer muss geschützt werden.

Abhängig von Policy können erforderlich sein:

```text
encryption
mutual authentication
integrity protection
anti-replay
target attestation
```

---

## Checkpoint Storage vermeiden

Für Live Migration darf der Checkpoint direkt zwischen Quelle und Ziel übertragen werden.

```text
Source
    ↓
Encrypted State Stream
    ↓
Target
```

Persistente Zwischenspeicherung ist nicht zwingend erforderlich.

---

## Zero-Copy

Wo möglich, können lokale Migrationspfade Zero-Copy- oder Page-Remapping-Mechanismen verwenden.

Beispiel:

```text
NUMA Migration
    ↓
page migration / remap
```

Sicherheits- und Ownership-Regeln bleiben erhalten.

---

## Memory Migration

Speicher kann je nach Migrationstyp behandelt werden durch:

```text
page copy
page remap
COW
remote page fetch
reallocation
```

Die virtuelle Semantik für den Workload muss erhalten bleiben.

---

## NUMA Migration

Bei Migration zwischen NUMA-Domänen kann NovaOS nicht nur Threads, sondern auch relevante Memory Pages verschieben.

Beispiel:

```text
Threads on Node 0
Memory on Node 0

        ↓ migrate

Threads on Node 2
Memory on Node 2
```

Dies reduziert langfristig Remote-Memory-Kosten.

---

## Locality Cost

Migration besitzt selbst Kosten.

Ein Planner muss mindestens berücksichtigen können:

```text
state size
memory distance
network bandwidth
expected execution duration
dependency locality
migration overhead
```

Eine theoretisch lokalere Zielposition kann unvorteilhaft sein, wenn die Migrationskosten höher sind als der erwartete Nutzen.

---

## Migration Hysteresis

Adaptive Migration darf nicht zu ständigem Hin- und Herschieben führen.

Daher müssen Hysteresis beziehungsweise Mindestnutzen berücksichtigt werden können.

Beispiel:

```text
Expected Benefit
    >
Migration Cost + Hysteresis Margin
```

---

## Thrashing Prevention

Workloads dürfen nicht bei kleinen Lastschwankungen ständig migriert werden.

Policy kann verwenden:

- Mindestaufenthaltsdauer,
- Cooldown,
- Migration Budget,
- Benefit Threshold.

---

## Resource Economy

Migration ist Bestandteil der systemweiten Resource Economy.

Sie verbraucht selbst:

```text
CPU
Memory
Network
Storage
Energy
```

Migration muss daher als eigene Systemoperation budgetiert werden.

---

## Migration Budget

Ein System kann Limits definieren.

Beispiele:

```text
maximum concurrent migrations
network bandwidth budget
CPU budget
maximum migration frequency
```

Damit darf Optimierung nicht die normale Arbeit verdrängen.

---

## Energy Policy

Workloads können aus Energiegründen konsolidiert werden.

Beispiel:

```text
Low load
    ↓
migrate workloads
    ↓
free host
    ↓
power down host
```

Dies ist nur zulässig, wenn stärkere Constraints erfüllt bleiben.

---

## Thermal Policy

Auf einem lokalen System kann Migration dazu dienen, thermische Hotspots zu reduzieren.

Beispiel:

```text
GPU 0 thermal limit
    ↓
move compatible workload to GPU 1
```

---

## Scheduling

Scheduler und Migration Planner arbeiten zusammen.

Der Scheduler entscheidet kurzfristig:

```text
where runnable work executes
```

Migration entscheidet langfristiger:

```text
where the workload and its state should reside
```

Beide Mechanismen dürfen nicht unnötig gegeneinander arbeiten.

---

## Process Supervision

Migration wird mit Process Supervision integriert.

Ein Supervisor kann beispielsweise entscheiden:

```text
Host degrading
    ↓
migrate supervised workload
```

anstatt:

```text
wait for crash
```

---

## Predictive Migration

Adaptive Systeme dürfen bevorstehende Engpässe prognostizieren.

Beispiel:

```text
Prediction:
    host likely overloaded soon

Action proposal:
    migrate workload
```

Prediction Error darf zur Verbesserung der Policy genutzt werden.

Die Migration selbst muss weiterhin Hard Constraints erfüllen.

---

## Kein KI-Zwang

Migration muss vollständig regelbasiert funktionieren.

KI ist nur Advisor beziehungsweise Optimierer.

Sie darf keine Trust-, Authority- oder Sovereignty-Regeln überschreiben.

---

## External Resources

Externe Ressourcen müssen pro Ressource klassifiziert werden.

Beispielsweise:

```text
Transferable
Rebindable
Recreatable
Proxyable
Pinned
NonMigratable
```

Diese Information fließt in den Migration Plan ein.

---

## Rebindable Resource

Ein Diensthandle kann am Ziel neu aufgelöst werden.

```text
Source Endpoint
    ↓
Migration
    ↓
Resolve Service Capability
    ↓
Target Endpoint
```

Damit muss kein identischer physischer Endpoint existieren.

---

## Transferable Resource

Eine Ressource kann Ownership direkt übertragen.

Beispiel:

```text
Distributed object ownership
Source
    ↓
Target
```

Nach Commit darf die Quelle diese Ressource nicht mehr verwenden.

---

## Proxyable Resource

Bestimmte Ressourcen können vorübergehend über die Quelle oder einen Proxy erreichbar bleiben.

Beispiel:

```text
Migrated Workload
      ↓
Remote Proxy
      ↓
Original Resource
```

Dies macht die Ressource nicht lokal.

Latenz und Ausfallabhängigkeit müssen sichtbar bleiben.

---

## Pinned Resource

Eine Ressource kann einen Workload an einen Ort binden.

Beispiele:

```text
local-only hardware
non-migratable accelerator context
exclusive physical device
```

Ein Workload mit einer Required Pinned Resource ist nicht hostmigrierbar, solange diese Bindung besteht.

---

## Dependency Graph

Migration muss den Dependency Graph berücksichtigen.

Beispiel:

```text
Process
   ↓
Local Database
   ↓
Local Device
```

Ein einzelner Prozess kann technisch checkpointfähig sein, aber aufgrund seiner Abhängigkeiten praktisch nicht migrierbar.

---

## Dependency Closure

Der Planner muss bestimmen können, welche Abhängigkeiten:

```text
move
rebind
remain remote
block migration
```

---

## Group Migration

Mehrere Domains können gemeinsam migriert werden.

Konzeptionell:

```text
Migration Group
    ├── Domain A
    ├── Domain B
    └── Shared State
```

Die Gruppe erhält einen gemeinsamen Migration Commit.

---

## Transactional Migration

Migration ist eine transaktionale Systemoperation.

Konzeptionell:

```text
Plan
 ↓
Validate Source
 ↓
Validate Target
 ↓
Reserve Target
 ↓
Transfer State
 ↓
Restore Target
 ↓
Verify Target
 ↓
Switch Ownership
 ↓
Commit
 ↓
Release Source
```

---

## Commit Point

Vor dem Commit darf die Quelle die autoritative aktive Instanz bleiben.

Nach dem Commit ist das Ziel autoritativ.

Es darf keinen unklaren Zwischenzustand geben.

---

## Split-Brain Prevention

NovaOS muss verhindern, dass Quelle und Ziel gleichzeitig dieselbe exklusive logische Workload-Identität aktiv verwenden.

Das Ownership-Modell lautet:

```text
Source owns
     ↓
Prepared Target
     ↓
Atomic Ownership Transfer
     ↓
Target owns
```

---

## Generation Token

Für migrierbare Workloads darf ein Generation- beziehungsweise Epoch-Token verwendet werden.

Beispiel:

```text
Workload Identity:
    X

Active Generation:
    42
```

Nach Migration:

```text
Target Generation:
    43
```

Alte Instanzen dürfen mit abgelaufener Generation keine autoritativen Operationen fortsetzen.

---

## Fencing

Für kritische externe Ressourcen können Fencing-Mechanismen erforderlich sein.

Ziel:

```text
old source cannot continue writing
after ownership transfer
```

Dies ist insbesondere bei:

- Storage,
- verteilten Locks,
- exklusiven Devices

relevant.

---

## Migration Failure vor Commit

Scheitert die Migration vor dem Ownership Commit, bleibt die Quelle autoritativ, sofern sie noch funktionsfähig ist.

Konzeptionell:

```text
Target Restore Failed
      ↓
Discard Target
      ↓
Resume / Continue Source
```

---

## Migration Failure nach Commit

Ein Fehler nach Ownership Transfer wird als Target-Lifecycle- beziehungsweise Recovery-Fehler behandelt.

Die Quelle darf nicht automatisch reaktiviert werden, wenn dies Split Brain verursachen könnte.

Rollback benötigt eine explizite reverse Migration oder Recovery Policy.

---

## Rollback

Migration kann einen Rollback Plan besitzen.

Dieser muss zwischen:

```text
pre-commit rollback
```

und:

```text
post-commit recovery
```

unterscheiden.

---

## Network Failure

Bei Host Migration muss ein Verbindungsabbruch während Transfer behandelt werden.

Vor Commit:

```text
abort target
source remains authoritative
```

sofern die Quelle nicht ebenfalls ausgefallen ist.

---

## Source Failure während Migration

Live Migration muss den Fall berücksichtigen:

```text
Source crashes before commit
```

Je nach bereits übertragenem Zustand kann:

- Target Restore fortgesetzt,
- Recovery Checkpoint verwendet,
- Workload als verloren behandelt

werden.

Es darf kein falscher erfolgreicher Migrationsstatus erzeugt werden.

---

## Target Failure während Migration

Scheitert das Ziel vor Commit, wird seine vorbereitete Instanz verworfen.

Ressourcenreservierungen werden freigegeben.

---

## External Identity

Migrierte Workloads können eine stabile logische Identität behalten.

Beispiel:

```text
Service Identity
```

während sich:

```text
Process ID
Host ID
Endpoint
```

ändern können.

---

## Service Rebinding

Clients sollen wenn möglich über stabile Service- beziehungsweise Capability-Identitäten arbeiten.

Damit kann:

```text
Service X on Host A
```

zu:

```text
Service X on Host B
```

wechseln, ohne dass die logische Serviceidentität geändert werden muss.

Die Migration selbst darf trotzdem observierbare Latenz- oder Fehlereffekte besitzen.

---

## Location Transparency

NovaOS unterstützt semantische Location Transparency.

Das bedeutet:

```text
same logical service
```

kann an verschiedenen Orten ausgeführt werden.

Es bedeutet ausdrücklich nicht:

```text
location has no cost
or cannot fail
```

Migration muss diese Unterschiede sichtbar und introspektierbar halten.

---

## Realtime

Hard-Realtime-Workloads dürfen nur migriert werden, wenn ihre Timing Contracts weiterhin garantiert werden können.

Mögliche Anforderungen:

```text
maximum downtime
target CPU reservation
memory locality
interrupt affinity
device latency
```

Kann dies nicht garantiert werden, muss Migration abgelehnt werden.

---

## Deterministic Mode

Migration eines deterministischen Workloads muss entweder:

- vollständig deterministisch spezifiziert sein,
- oder außerhalb seines deterministischen Execution Scope stattfinden.

Adaptive Zielwahl darf nicht still die deterministische Ausführung verändern.

---

## Migration und Zeit

Zeitquellen müssen beim Restore konsistent behandelt werden.

Ein migrierter Prozess darf beispielsweise keinen unerwarteten rückwärts laufenden monotonen Zeitwert beobachten.

Die genaue Virtual-Time-/Clock-Semantik wird separat spezifiziert.

---

## Migration und Randomness

Deterministische Random-State-Streams müssen als Teil des relevanten Execution State behandelt werden.

Nichtdeterministische Hardware-RNG-Zugriffe werden nach Restore entsprechend der aktuellen Policy neu bereitgestellt.

---

## Persistent State

Migration eines laufenden Prozesses ersetzt keine Datenbank- oder Storage-Replikation.

Persistenter Zustand kann:

- gemeinsam erreichbar,
- repliziert,
- ebenfalls migriert,
- remote verwendet

werden.

Die Konsistenzanforderungen müssen explizit sein.

---

## Storage Locality

Ein Workload darf nur dann aus Performancegründen migriert werden, wenn dadurch nicht unverhältnismäßig große Remote-Storage-Kosten entstehen.

Data Gravity ist Teil des Kostenmodells.

---

## Data Gravity

Große schwer bewegliche Datenmengen können einen Workload an eine Location binden.

Beispiel:

```text
Compute State:
    100 MiB

Dataset:
    4 TiB

Preferred action:
    move compute to data
```

statt:

```text
move data to compute
```

---

## Accelerator Migration

Ein Accelerator Workload kann nur migriert werden, wenn:

- Zielalgorithmus verfügbar,
- State übertragbar oder rekonstruierbar,
- semantische Anforderungen erfüllt

sind.

NovaOS darf alternativ einen anderen Provider wählen, wenn der Execution Contract dies erlaubt.

Beispiel:

```text
GPU A unavailable
    ↓
GPU B
```

oder:

```text
GPU
    ↓
CPU fallback
```

nur wenn Hard Constraints dies zulassen.

---

## Algorithmusauswahl

Migration darf zu einer neuen algorithmischen beziehungsweise Provider-Auswahl führen.

Beispiel:

```text
Source:
    CUDA-like GPU provider

Target:
    NPU provider
```

Dies ist nur zulässig, wenn die abstrakte Capability und der Execution Contract dies erlauben.

Ein explizit erzwungener Algorithmus darf nicht still ersetzt werden.

---

## Graceful Degradation

Ist das ideale Ziel nicht verfügbar, kann ein weniger optimales Ziel gewählt werden, wenn alle Required Constraints erfüllt bleiben.

Beispiel:

```text
Preferred:
    local GPU

Fallback:
    local CPU

Required:
    local-only data
```

Remote GPU wäre trotz höherer Leistung verboten.

---

## Architecture Introspection

Migration muss systemweit introspektierbar sein.

Beispiel:

```text
Workload:
    nova.render.worker

Source:
    Host-A / NUMA-0

Target:
    Host-B / NUMA-1

State:
    Transferring

Strategy:
    PreCopy

Transferred:
    72 %

Downtime Target:
    20 ms

Data Policy:
    EU-Only

Reason:
    Source thermal pressure
```

---

## Migration Decision Record

Komplexe Migrationsentscheidungen sollen als strukturierte technische Records verfügbar sein.

Konzeptionell:

```text
MigrationDecision {
    workload
    source
    candidateTargets
    selectedTarget
    strategy
    constraints
    expectedCost
    expectedBenefit
    reason
}
```

Dies dient Introspection und Audit und ist kein internes Chain-of-Thought.

---

## Audit

Sicherheits- und souveränitätsrelevante Migrationsereignisse sollen auditierbar sein.

Beispiele:

```text
MigrationRequested
MigrationApproved
MigrationDenied
StateTransferStarted
OwnershipTransferred
MigrationCompleted
MigrationFailed
```

---

## Fail-Safe Defaults

Ist nicht eindeutig bekannt, ob ein Required State migrierbar ist, gilt er als nicht migrierbar.

Ist die Trust-Eigenschaft eines Zielsystems unbekannt, darf ein Workload mit entsprechender Trust-Anforderung nicht dorthin migriert werden.

Ist die Data-Sovereignty-Zulässigkeit unklar, erfolgt keine Migration.

---

## Normative Anforderungen

1. NovaOS MUSS Migration als explizite Eigenschaft von Execution Domains und Workloads modellieren können.
2. Ein migrierbarer nativer Workload MUSS eine definierte Checkpoint-/Restore-Semantik besitzen.
3. Nicht jeder Prozess DARF automatisch als migrierbar gelten.
4. Der Migration Scope MUSS explizit bestimmbar sein.
5. Migration MUSS lokale und hostübergreifende Ziele unterscheiden können.
6. Cold Migration MUSS als Basismodell unterstützt werden können.
7. Live Migration DARF als optimierte Strategie unterstützt werden.
8. Migration MUSS einen expliziten Migration Plan besitzen.
9. Das Ziel MUSS vor dem Commit auf Hard Constraints geprüft werden.
10. Erforderliche Zielressourcen MÜSSEN reservierbar sein können.
11. ISA- und ABI-Kompatibilität MÜSSEN vor Host Migration geprüft werden.
12. Capabilities MÜSSEN am Ziel erneut autorisiert werden.
13. Migration DARF KEINE Authority Amplification erzeugen.
14. Trust-Anforderungen MÜSSEN Quelle, Ziel und Transferpfad berücksichtigen.
15. Data-Sovereignty-Regeln MÜSSEN Migration begrenzen können.
16. Zustandstransfer MUSS abhängig von Policy gegen Manipulation und Offenlegung geschützt werden.
17. Externe Ressourcen MÜSSEN als Transferable, Rebindable, Reconstructable, Proxyable, Pinned oder äquivalent klassifizierbar sein.
18. Required Pinned Resources MÜSSEN eine inkompatible Migration verhindern.
19. Dependency Closure MUSS vor Migration analysierbar sein.
20. Mehrere abhängige Domains MÜSSEN als gemeinsame Migration Group behandelt werden können.
21. Migration MUSS einen eindeutigen Ownership Commit Point besitzen.
22. Quelle und Ziel DÜRFEN nach Commit NICHT gleichzeitig dieselbe exklusive Workload-Authority besitzen.
23. Split Brain MUSS durch Ownership-, Generation- oder Fencing-Mechanismen verhindert werden.
24. Pre-Commit-Fehler MÜSSEN ohne unnötige Deaktivierung einer gültigen Quelle behandelbar sein.
25. Post-Commit-Fehler MÜSSEN als explizite Recovery-Fälle behandelt werden.
26. Migration MUSS in die systemweite Resource Economy und Locality Policy integriert sein.
27. Adaptive Migration MUSS Hysteresis beziehungsweise Thrashing-Schutz unterstützen.
28. Hard-Realtime- und Determinism-Constraints DÜRFEN durch Migration NICHT still verletzt werden.
29. Migration und ihre Entscheidungen MÜSSEN introspektierbar und sicherheitsrelevante Ereignisse SOLLEN auditierbar sein.
30. Die vollständige grundlegende Migrationsfunktionalität MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- Grundlage für Lastverschiebung,
- bessere NUMA- und Accelerator-Lokalität,
- Host-Wartung ohne zwingenden Workloadverlust,
- Grundlage für Cluster- und verteilte NovaOS-Szenarien,
- Integration mit Resource Economy,
- Integration mit Process Supervision,
- bessere Energie- und Thermaloptimierung,
- kontrollierte statt ad-hoc Migration,
- Data Sovereignty und Trust bleiben auch bei Ortswechsel autoritativ.

### Negative Konsequenzen

- Host Migration ist technisch komplex,
- externe Ressourcen können Migration blockieren,
- Live Migration benötigt aufwendige Memory- und State-Tracking-Mechanismen,
- Netzwerkfehler erzeugen zusätzliche Recovery-Fälle,
- Split-Brain-Vermeidung erfordert robuste Ownership-Protokolle,
- ABI- und Formatversionierung müssen langfristig stabil gepflegt werden.

---

## Verworfene Alternativen

### Jeden Prozess automatisch migrierbar machen

Verworfen.

Viele Hardware-, Security- und externe Ressourcen besitzen keine sichere portable Semantik.

### Migration ausschließlich über virtuelle Maschinen

Verworfen.

NovaOS soll native Execution Domains migrieren können, ohne eine Hypervisor-First-Architektur vorauszusetzen.

### Migration als reine Speicherkopie

Verworfen.

Externe Ressourcen, Capabilities, Trust und Dependencies würden nicht korrekt behandelt.

### Zielsystem erhält automatisch alle Source-Rechte

Verworfen.

Authority muss am Ziel erneut validiert werden.

### Standort vollständig transparent verbergen

Verworfen.

Remote-Ausführung besitzt andere Kosten, Latenzen und Fehlermodelle.

### Migration bei jedem lokalen Lastunterschied

Verworfen.

Dies kann mehr Kosten verursachen als Nutzen und zu Migration Thrashing führen.

### KI entscheidet autonom über zulässige Migration

Verworfen.

Hard Constraints, Trust und Data Sovereignty müssen deterministisch und policybasiert durchgesetzt werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-MIGRATION-0001`
- `NPSPEC-PROCESS-MIGRATION-CONTRACT-0001`
- `NPSPEC-PROCESS-MIGRATION-PLAN-0001`
- `NPSPEC-PROCESS-MIGRATION-TARGET-0001`
- `NPSPEC-PROCESS-MIGRATION-COLD-0001`
- `NPSPEC-PROCESS-MIGRATION-LIVE-0001`
- `NPSPEC-PROCESS-MIGRATION-PRECOPY-0001`
- `NPSPEC-PROCESS-MIGRATION-RESOURCE-0001`
- `NPSPEC-PROCESS-MIGRATION-GROUP-0001`
- `NPSPEC-PROCESS-MIGRATION-OWNERSHIP-0001`
- `NPSPEC-PROCESS-MIGRATION-SECURITY-0001`
- `NPSPEC-PROCESS-MIGRATION-SOVEREIGNTY-0001`
- `NPSPEC-PROCESS-MIGRATION-INTROSPECTION-0001`
- `NPSPEC-PROCESS-MIGRATION-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- lokale CPU-Set-Migration,
- NUMA-Migration,
- Speicherverlagerung zwischen NUMA-Nodes,
- Cold Migration zwischen zwei Hosts,
- Live Migration,
- Pre-Copy mit Dirty Pages,
- Abbruch vor Commit,
- Target Failure vor Commit,
- Source Failure während Migration,
- eindeutiger Ownership Transfer,
- Split-Brain-Verhinderung,
- Generation-/Epoch-Wechsel,
- Target Admission Control,
- fehlende Hard Resource,
- ABI-Inkompatibilität,
- ISA-Inkompatibilität,
- Capability-Reevaluation,
- verweigerte Capability am Ziel,
- Trust-Rejection des Targets,
- verschlüsselter State Transfer,
- Data-Sovereignty-Verstoß,
- Required Pinned Resource,
- Rebindable Service Endpoint,
- Proxyable Resource,
- Shared-Memory-Migration als Gruppe,
- GPU-Provider-Wechsel,
- nicht migrierbarer GPU-Kontext,
- Migration mit Resource Budget,
- Thrashing Prevention,
- Deterministic Workload,
- Hard-Realtime-Workload,
- Supervision-triggered Migration,
- Introspection,
- Audit.

---

## Ergebnis

NovaOS behandelt Migration als kontrollierte Veränderung des Ausführungsortes eines Execution Domains oder einer Workload-Gruppe.

Das Modell lautet:

```text
Workload
   ↓
Migration Contract
   ↓
Source + Target Validation
   ↓
Checkpoint / Transfer
   ↓
Restore
   ↓
Rebind Resources
   ↓
Verify
   ↓
Ownership Commit
```

Damit kann NovaOS Workloads zukünftig zwischen:

```text
CPUs
NUMA Domains
Accelerators
Hosts
```

verschieben, ohne Security, Trust, Resource Contracts oder Data Sovereignty durch den Ortswechsel aufzuheben.

Die zentrale Architekturregel lautet:

```text
Migration is a contract-preserving relocation.

Move state only when dependencies can move or rebind.

Validate the target before giving up the source.

Transfer ownership exactly once.

Location may be transparent semantically,
but never economically or operationally invisible.
```