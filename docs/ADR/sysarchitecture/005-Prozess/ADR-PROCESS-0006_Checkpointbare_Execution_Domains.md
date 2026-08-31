# ADR-PROCESS-0006 – Checkpointbare Execution Domains

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Execution Domains / Checkpoint / Restore / Migration / Recovery / Lifecycle

## Kontext

NovaOS definiert mit `ADR-PROCESS-0001_Natives_Execution_Domain_Modell` das Execution Domain als native systemweite Ausführungseinheit.

Darauf aufbauend behandeln die bisherigen Process-ADRs insbesondere:

- Isolation,
- Ressourcen,
- Spawn,
- Kompatibilität,
- Supervision.

Für Recovery, Suspendierung, Live Evolution, Migration und langlebige Anwendungen wird zusätzlich ein standardisiertes Modell benötigt, mit dem der relevante Zustand eines Execution Domains erfasst und später wiederhergestellt werden kann.

Ein klassisches Prozessabbild besteht nicht nur aus Speicher.

Zu einem realen Execution Domain können unter anderem gehören:

- virtueller Adressraum,
- Threads,
- Registerzustände,
- Handles,
- Shared Memory,
- IPC-Verbindungen,
- Resource Budgets,
- Scheduling State,
- Capabilities,
- Trust Context,
- Data-Sovereignty-Policy,
- offene Dateien,
- Netzwerkverbindungen,
- Gerätebindungen,
- GPU-/NPU-Kontexte,
- ausstehende asynchrone Operationen.

Nicht jeder dieser Zustände ist automatisch serialisierbar oder wiederherstellbar.

NovaOS benötigt deshalb keine naive „Speicherabbild speichern und später laden“-Semantik, sondern ein explizites Checkpoint-Modell.

---

## Entscheidung

NovaOS macht Execution Domains grundsätzlich **checkpointfähig**, sofern ihre verwendeten Ressourcen und Provider eine entsprechende Checkpoint-Semantik besitzen.

Checkpointing wird als koordinierter Lifecycle-Vorgang behandelt.

Das Grundmodell lautet:

```text
Running Domain
      ↓
Checkpoint Request
      ↓
Quiesce
      ↓
Capture State
      ↓
External Resource Coordination
      ↓
Validate
      ↓
Persist / Transfer Checkpoint
      ↓
Resume or Terminate
```

Restore erfolgt entsprechend:

```text
Checkpoint
    ↓
Validate
    ↓
Resolve Dependencies
    ↓
Create Execution Domain
    ↓
Restore Memory / State
    ↓
Rebind External Resources
    ↓
Verify
    ↓
Resume
```

---

## Grundprinzip

```text
Checkpoint execution state explicitly.

Do not assume every resource is serializable.

Restore contracts, not accidental machine state.
```

---

## Checkpointfähigkeit ist eine Eigenschaft

Nicht jedes Execution Domain muss vollständig checkpointfähig sein.

Ein Domain kann eine Checkpoint Class besitzen.

Beispielsweise:

```text
None
MemoryOnly
Restartable
Checkpointable
Migratable
```

Die konkrete Klassifikation kann später erweitert werden.

---

## Checkpointable

`Checkpointable` bedeutet, dass der für eine definierte Wiederherstellung notwendige Zustand vollständig und konsistent erfasst werden kann.

Dies bedeutet nicht zwingend:

```text
bit-identical physical machine state
```

sondern:

```text
semantically equivalent execution state
```

---

## Migratable

Ein Domain ist nur dann `Migratable`, wenn sein Checkpoint auf einer anderen zulässigen Ausführungsumgebung wiederhergestellt werden kann.

Damit gilt:

```text
Migratable
    ⇒
Checkpointable
```

aber nicht zwingend:

```text
Checkpointable
    ⇒
Migratable
```

---

## Checkpoint Scope

Checkpointing muss explizit auf einen Scope angewendet werden.

Mögliche Scopes:

```text
Execution Domain
Process
Process Group
Application
Execution Scope
```

Diese ADR definiert primär das Execution Domain als Basiseinheit.

---

## Checkpoint Object

Ein Checkpoint wird als versioniertes Systemobjekt behandelt.

Konzeptionell:

```text
ExecutionCheckpoint {
    formatVersion
    domainIdentity
    domainType

    executableIdentity
    architecture
    abi

    memoryState
    executionState
    resourceState

    capabilityState
    securityContext
    trustContext
    dataPolicy

    dependencyManifest
    provenance

    integrityMetadata
}
```

Die konkrete Binärstruktur wird separat spezifiziert.

---

## Kein rohes Memory Dump

Ein Checkpoint ist nicht lediglich:

```text
RAM dump
```

Ein Memory Dump allein reicht nicht aus, um:

- Handles,
- externe Ressourcen,
- Kernelobjekte,
- Geräte,
- IPC-Verbindungen

sicher wiederherzustellen.

Deshalb ist der Checkpoint ein strukturiertes Format.

---

## Quiescing

Vor einer konsistenten Zustandserfassung wird das Domain kontrolliert quiesziert.

Konzeptionell:

```text
Running
   ↓
CheckpointRequested
   ↓
Quiescing
   ↓
CheckpointReady
```

Während `Quiescing` sollen neue nicht notwendige Operationen verhindert oder kontrolliert abgeschlossen werden.

---

## Safe Point

Ein Domain kann einen Checkpoint Safe Point erreichen.

Dieser Zustand bedeutet, dass der relevante Laufzeitzustand konsistent erfasst werden kann.

Mögliche Bedingungen:

- definierte Threadzustände,
- keine kritische halbfertige Runtime-Transition,
- bekannte Ownership externer Ressourcen,
- synchronisierte Checkpoint Provider.

---

## Cooperative Checkpoint

Runtimes und Anwendungen dürfen bei der Checkpoint-Vorbereitung kooperieren.

Beispiele:

```text
BeforeCheckpoint
AfterCheckpoint
AfterRestore
```

Damit können sie:

- Caches flushen,
- JIT-Zustände stabilisieren,
- externe Verbindungen kennzeichnen,
- nicht serialisierbare Ressourcen vorbereiten.

---

## Forced Checkpoint

NovaOS darf für bestimmte Domains einen erzwungenen Checkpoint unterstützen.

Dieser ist jedoch nur zulässig, wenn die beteiligten Kernel- und Resource-Provider eine konsistente Zustandserfassung garantieren können.

Ein beliebiges Suspendieren aller CPUs und Kopieren des Speichers gilt nicht automatisch als sicherer Checkpoint.

---

## Thread State

Der Checkpoint muss für wiederherzustellende Threads mindestens den notwendigen logischen Ausführungszustand erfassen können.

Dazu können gehören:

```text
instruction state
register state
stack state
thread-local state
scheduler-visible state
```

Architekturabhängige Details werden durch entsprechende Backend-Formate gekapselt.

---

## Thread Identity

Thread-Identitäten dürfen beim Restore:

- erhalten,
- logisch rekonstruiert,
- neu abgebildet

werden, sofern die zugesicherte Semantik erhalten bleibt.

Rohe hardware- oder bootabhängige IDs dürfen nicht als dauerhaft portable Identität vorausgesetzt werden.

---

## Memory State

Der virtuelle Speicherzustand eines Domains muss strukturiert erfasst werden können.

Dazu gehören:

- private Pages,
- Mapping Metadata,
- Protection Flags,
- Shared Regions,
- Copy-on-Write-Beziehungen,
- Memory Object References.

---

## Lazy Checkpoint

NovaOS darf Speicherzustand inkrementell oder lazy sichern.

Mögliche Mechanismen:

```text
Copy-on-Write snapshot
dirty page tracking
incremental checkpoint
deduplicated backing store
```

Die Optimierung darf die Konsistenz des Checkpoints nicht verändern.

---

## Incremental Checkpoints

Nach einem vollständigen Basis-Checkpoint dürfen spätere Checkpoints nur Änderungen speichern.

Beispiel:

```text
Checkpoint A
    ↓
Delta B
    ↓
Delta C
```

Restore muss die Abhängigkeitskette validieren.

Beschädigte oder fehlende Basen dürfen nicht stillschweigend ignoriert werden.

---

## Shared Memory

Shared Memory benötigt eine explizite Checkpoint-Semantik.

Mögliche Fälle:

```text
checkpoint together
reference external shared object
recreate shared object
reject checkpoint
```

Mehrere Domains, die denselben Shared-Memory-Zustand konsistent benötigen, können als gemeinsame Checkpoint Group behandelt werden.

---

## Handles

Kernel-Handles dürfen nicht einfach als numerische Werte gespeichert und später wiederverwendet werden.

Stattdessen wird ihre semantische Ressource beschrieben.

Beispiel:

```text
Checkpoint Handle Entry {
    objectType
    resourceIdentity
    rights
    restorePolicy
}
```

Beim Restore wird daraus ein neuer gültiger Handle erzeugt.

---

## Handle Restore Policies

Ressourcen können unterschiedliche Restore Policies besitzen.

Beispiele:

```text
Restore
Reopen
Recreate
Rebind
Drop
Fail
```

Die Policy muss mit der Semantik der Ressource vereinbar sein.

---

## Dateien

Offene Dateien können wiederhergestellt werden, wenn ihre Identität und der benötigte Zustand eindeutig rekonstruierbar sind.

Möglicher Zustand:

```text
file identity
open mode
current offset
locks
transaction state
```

Ein Dateipfad allein reicht nicht zwingend als stabile Identität aus.

---

## Veränderte externe Dateien

Hat sich eine referenzierte Datei seit dem Checkpoint inkompatibel verändert, darf Restore nicht blind fortgesetzt werden.

Mögliche Reaktionen:

```text
Fail Restore
Use Versioned Snapshot
Revalidate
Explicit Compatibility Policy
```

---

## IPC

IPC-Endpunkte benötigen eine definierte Checkpoint-Semantik.

Mögliche Strategien:

```text
checkpoint both endpoints
reconnect
rebind to service
invalidate
fail restore
```

Eine alte rohe Endpoint-ID darf nicht als dauerhaft gültig angenommen werden.

---

## Service Rebinding

Verbindungen zu systemweiten Diensten sollen, wenn semantisch möglich, beim Restore neu gebunden werden.

Beispiel:

```text
Old service endpoint
      ↓
restore
      ↓
resolve service capability
      ↓
new endpoint
```

Dies erhöht die Portabilität von Checkpoints.

---

## Netzwerkverbindungen

Aktive Netzwerkverbindungen sind nicht grundsätzlich checkpointfähig.

Eine TCP- oder andere externe Verbindung kann unter bestimmten Bedingungen:

- erhalten,
- proxybasiert weitergeführt,
- rekonstruiert,
- abgebrochen

werden.

Checkpointfähigkeit darf nicht universell für externe Peers vorausgesetzt werden.

---

## Netzwerk-Reconnect

Anwendungen können eine semantische Reconnect Policy angeben.

Beispiel:

```text
NetworkResource {
    restorePolicy = Reconnect
}
```

Dann wird nicht der rohe Transportzustand restauriert, sondern die logische Verbindung neu aufgebaut.

---

## Geräte

Exklusive Gerätebindungen benötigen explizite Provider-Unterstützung.

Ein Device Provider kann deklarieren:

```text
CheckpointSupported
RestoreSupported
MigrationSupported
```

Fehlt die Unterstützung, kann das gesamte Domain entsprechend seiner Policy nicht vollständig checkpointfähig sein.

---

## DMA

Aktive DMA-Operationen müssen vor einem konsistenten Checkpoint:

```text
complete
cancel
drain
or snapshot through explicit provider support
```

werden.

Ein Checkpoint darf keine unbekannten weiterhin aktiven DMA-Zugriffe hinterlassen.

---

## Interrupts

Vor Checkpointing gerätegebundener Domains müssen relevante Interrupt- und Deferred-Work-Zustände synchronisiert werden.

Beim Restore werden Interrupt-Ressourcen neu gebunden.

Rohe Interrupt-Vektoren sind nicht portable Checkpoint-Identitäten.

---

## GPU und NPU

GPU-/NPU-Kontexte sind nur checkpointfähig, wenn der entsprechende Provider dies unterstützt.

Mögliche Strategien:

```text
Native Context Snapshot
Reconstruct from Higher-Level State
Drain and Recreate
Unsupported
```

NovaOS darf keine universelle transparente Accelerator-Migration vortäuschen.

---

## Resource Provider Contract

Ressourcen, die Checkpointing unterstützen, implementieren ein standardisiertes Checkpoint Provider Contract.

Konzeptionell:

```text
CheckpointProvider {
    Prepare()
    Capture()
    Validate()
    Restore()
    Rebind()
    Abort()
}
```

Nicht jede Ressource muss alle Operationen unterstützen.

Capabilities werden explizit gemeldet.

---

## Provider Capabilities

Konzeptionell:

```text
CheckpointCapabilities {
    checkpoint
    restore
    migrate
    incremental
    liveCheckpoint
}
```

Die Planungslogik prüft diese Fähigkeiten vor Beginn der Operation.

---

## Capability State

Capabilities werden nicht als rohe Kernelreferenzen serialisiert.

Der Checkpoint speichert ihre semantisch relevante Authority und Herkunft, soweit dies zulässig ist.

Beim Restore müssen Capabilities erneut validiert werden.

---

## Keine automatische Authority-Wiederherstellung

Ein alter Checkpoint ist keine dauerhafte Berechtigungsgarantie.

Beispiel:

```text
Checkpoint:
    Network.Connect

Current Policy:
    Network denied
```

Resultat:

```text
Network capability not restored
```

oder:

```text
restore rejected
```

abhängig vom Execution Contract.

---

## Security Context

Der Security Context wird beim Restore nicht blind übernommen.

Er muss gegen den aktuellen Systemzustand validiert werden.

Änderungen an:

- Benutzerrechten,
- Systempolicy,
- Revocation,
- Security Level

können einen Restore beeinflussen.

---

## Trust

Der Checkpoint selbst und die referenzierten Executables müssen einer Trust-Prüfung unterliegen.

Mindestens müssen geprüft werden:

```text
checkpoint integrity
checkpoint provenance
executable identity
required runtime generation
```

Ein manipulierter Checkpoint darf nicht ausgeführt werden.

---

## Checkpoint Signing

Persistente Checkpoints können signiert beziehungsweise durch Nova.Trust geschützt werden.

Dabei soll nachvollziehbar sein:

- wer den Checkpoint erstellt hat,
- von welchem Domain er stammt,
- auf welcher Systemgeneration er erzeugt wurde.

---

## Data Sovereignty

Checkpoints können sensitive Nutzdaten enthalten.

Deshalb sind sie Bestandteil der Data-Sovereignty-Architektur.

Mögliche Regeln:

```text
LocalOnly
EncryptedAtRest
NoCloudStorage
SpecificRegionOnly
NoMigration
```

Die Checkpoint-Infrastruktur muss diese Regeln berücksichtigen.

---

## Verschlüsselung

Checkpoints mit sensitiven Daten müssen abhängig von Policy verschlüsselbar sein.

Schlüssel dürfen nicht ungeschützt gemeinsam mit dem Checkpoint gespeichert werden.

TPM beziehungsweise andere Trust Provider können für geeignete Sealing-Mechanismen verwendet werden.

---

## Restore auf anderem System

Migration beziehungsweise Restore auf anderer Hardware ist nur erlaubt, wenn:

- Architektur kompatibel ist,
- ABI kompatibel ist,
- erforderliche Provider vorhanden sind,
- Trust erfüllt ist,
- Data-Sovereignty dies erlaubt.

---

## Architecture Compatibility

Der Checkpoint muss seine relevante Architekturidentität angeben.

Beispiel:

```text
x86_64
arm64
riscv64
```

Ein nativer Register-/Codezustand kann nicht automatisch zwischen verschiedenen ISAs übertragen werden.

Cross-ISA-Migration benötigt eine separate Runtime- oder Übersetzungsebene.

---

## ABI Compatibility

Ein Checkpoint muss die benötigte NovaABI- beziehungsweise Runtime-Kompatibilität beschreiben.

Eine neuere Systemgeneration darf einen alten Checkpoint nur laden, wenn die erforderlichen Contracts kompatibel sind oder ein definierter Upgrade-Pfad existiert.

---

## Versioned Checkpoint Format

Das Checkpoint-Format muss versioniert sein.

NovaOS darf die interne Kernelstruktur nicht ungefiltert als dauerhaftes On-Disk-Format verwenden.

Es gilt:

```text
Kernel Internal Representation
    ≠
Persistent Checkpoint ABI
```

---

## Evolvability

Checkpointdaten sollen semantisch versionierte Felder und Providerzustände verwenden.

Dadurch können:

- Kerneländerungen,
- Runtimeupdates,
- Hot Replacement

besser unterstützt werden.

---

## Checkpoint Upgrade

NovaOS darf definierte Checkpoint-Migrationspfade zwischen Format- oder Runtimeversionen unterstützen.

Konzeptionell:

```text
Checkpoint v3
    ↓
Upgrade Adapter
    ↓
Checkpoint v4
```

Ein Upgrade muss explizit und validierbar sein.

---

## Execution Contract

Checkpointfähigkeit kann Bestandteil eines `Nova.ExecutionContract` sein.

Beispiele:

```text
checkpointRequired = true
migrationRequired = false
maximumCheckpointPause = 20 ms
```

Damit kann die Ausführungsplanung Provider auswählen, die diese Anforderungen erfüllen.

---

## Checkpoint Deadline

Realtime- oder latenzkritische Domains können eine maximal zulässige Checkpoint-Pause definieren.

Wenn diese nicht eingehalten werden kann, muss die Operation:

- abgelehnt,
- als Live Checkpoint ausgeführt,
- verschoben

werden.

Sie darf nicht still die Hard Deadline verletzen.

---

## Live Checkpoint

NovaOS darf Live-Checkpointing unterstützen.

Möglicher Ablauf:

```text
Begin Snapshot
    ↓
Domain continues
    ↓
Track modified pages
    ↓
Iterative copy
    ↓
Short final quiesce
    ↓
Capture remaining state
```

Dies reduziert die Pause, erhöht jedoch die Implementierungskomplexität.

---

## Stop-and-Copy

Für einfache Domains darf ein Stop-and-Copy-Verfahren verwendet werden.

```text
Quiesce
   ↓
Capture
   ↓
Resume
```

Die Policy entscheidet abhängig von:

- Domain-Typ,
- Datenmenge,
- Deadline,
- Providerfähigkeiten.

---

## Checkpoint und Supervision

Process Supervision kann Checkpoints für Recovery nutzen.

Beispiel:

```text
Service
   ↓
Periodic Checkpoint
   ↓
Crash
   ↓
Supervisor
   ↓
Restore from Checkpoint
```

Dies ist nur zulässig, wenn der Checkpoint zur aktuellen Recovery Policy passt.

---

## Restart vs Restore

Supervisoren müssen zwischen Restart und Restore unterscheiden.

```text
Restart
    → clean process state

Restore
    → previously captured execution state
```

Ein Restore ist nicht grundsätzlich sicherer oder besser als ein Restart.

---

## Persistenter Zustand

Checkpoint und persistenter Anwendungszustand sind getrennte Konzepte.

Ein Prozesscheckpoint darf nicht als Ersatz für:

- Datenbanktransaktionen,
- Journaling,
- Dateisystemkonsistenz

angesehen werden.

Beide Zustände müssen gegebenenfalls koordiniert werden.

---

## Application-Consistent Checkpoint

Anwendungen können einen applikationskonsistenten Checkpoint erzeugen.

Beispiel:

```text
Pause transactions
    ↓
Flush durable state
    ↓
Checkpoint domain
    ↓
Resume
```

Dies bietet stärkere Garantien als ein rein systemseitiger Crash-Consistent Checkpoint.

---

## Checkpoint Consistency Classes

NovaOS soll mindestens zwischen folgenden Konzepten unterscheiden können:

```text
CrashConsistent
ApplicationConsistent
```

Weitere Klassen dürfen ergänzt werden.

---

## Checkpoint Groups

Mehrere voneinander abhängige Domains können gemeinsam checkpointet werden.

Beispiel:

```text
Application
    ├── UI Process
    ├── Worker
    └── Local Database Service
```

Die Gruppe benötigt einen gemeinsamen Konsistenzpunkt.

---

## Group Checkpoint

Konzeptionell:

```text
Prepare all
    ↓
Quiesce all
    ↓
Capture shared state
    ↓
Validate
    ↓
Commit checkpoint
    ↓
Resume all
```

Scheitert ein erforderlicher Teilnehmer vor Commit, wird der Gruppencheckpoint verworfen.

---

## Transactional Checkpoint Creation

Checkpoint-Erstellung soll transaktional sein.

Ein persistenter Checkpoint erhält erst nach vollständiger erfolgreicher Erfassung den Zustand:

```text
Valid
```

Zwischenstände gelten als:

```text
Incomplete
```

und dürfen nicht als Restore-Quelle verwendet werden.

---

## Checkpoint States

Beispielsweise:

```text
Preparing
Capturing
Validating
Committed
Failed
Invalid
```

---

## Atomic Publication

Ein Checkpoint soll erst nach erfolgreichem Commit sichtbar als nutzbare Restore-Version veröffentlicht werden.

Dies verhindert Restore von teilweise geschriebenen Daten.

---

## Storage

Checkpointdaten können abhängig von Policy gespeichert werden in:

- lokalen Dateien,
- Snapshot Storage,
- dediziertem System Store,
- verschlüsseltem persistentem Speicher.

Remote Storage ist nur zulässig, wenn Data-Sovereignty- und Security-Policy dies erlauben.

---

## Deduplication

Checkpoint-Speicher darf identische immutable Pages oder Objekte deduplizieren.

Dies ist eine Storage-Optimierung.

Isolation und Zugriffskontrolle müssen trotzdem erhalten bleiben.

---

## Resource Economy

Checkpointing verbraucht selbst Ressourcen.

Dazu gehören:

```text
CPU
Memory
Storage I/O
Storage Capacity
Network
```

Checkpoint-Vorgänge müssen daher in die Resource Economy integriert werden.

---

## Checkpoint Budget

Ein Checkpoint kann ein eigenes Ressourcenbudget erhalten.

Beispiel:

```text
CheckpointBudget {
    cpu
    memory
    ioBandwidth
    storage
}
```

Dadurch darf Checkpointing das Gesamtsystem nicht unkontrolliert überlasten.

---

## Priorisierung

Systemkritische Recovery-Checkpoints können höhere Priorität als opportunistische Anwendungssnapshots besitzen.

Mechanismus und Policy bleiben getrennt.

---

## Cancellation

Eine noch nicht committete Checkpoint-Operation muss abbrechbar sein, sofern dadurch keine externe Inkonsistenz entsteht.

Nach Abbruch werden temporäre Ressourcen freigegeben.

---

## Structured Concurrency

Checkpoint-Operationen werden in kontrollierten Execution Scopes durchgeführt.

Dazu gehören:

- Provider Capture,
- Storage Write,
- Validation,
- Encryption.

Ein Abbruch muss an untergeordnete Operationen propagiert werden können.

---

## Restore Transaction

Auch Restore ist transaktional.

Konzeptionell:

```text
Validate Checkpoint
      ↓
Reserve Resources
      ↓
Create Domain
      ↓
Restore State
      ↓
Rebind Resources
      ↓
Verify
      ↓
Activate
      ↓
Commit
```

Scheitert Restore vor Aktivierung, wird das teilweise erzeugte Domain wieder entfernt.

---

## Restore Verification

Vor Aktivierung muss mindestens geprüft werden:

- Checkpointintegrität,
- Architektur,
- ABI,
- Executable Identity,
- Trust,
- Capabilities,
- Ressourcen,
- Providerabhängigkeiten.

---

## Partial Restore

Ein partieller Restore ist nur erlaubt, wenn das Checkpoint Contract ihn ausdrücklich unterstützt.

Es darf nicht still ein unvollständiger Prozess gestartet werden.

---

## Graceful Degradation

Falls optionale Ressourcen nicht wiederherstellbar sind, kann eine Degradation stattfinden.

Beispiel:

```text
GPU context unavailable
    ↓
software rendering
```

Dies ist nur zulässig, wenn:

```text
GPU = Preferred
```

und nicht:

```text
GPU = Required
```

ist.

---

## Hard Dependencies

Fehlt eine als `Required` markierte Restore-Abhängigkeit, muss Restore fehlschlagen.

Hard Constraints dürfen nicht still abgeschwächt werden.

---

## Location Transparency

Checkpointing bildet eine Grundlage für spätere Migration.

Die Wiederherstellung darf jedoch Remote-Effekte nicht verstecken.

Migration muss sichtbar machen:

- neue Location,
- Netzwerkabhängigkeit,
- Latenz,
- Kosten,
- Data-Sovereignty-Auswirkungen.

---

## Migration

Eine Migration kann konzeptionell aus folgenden Schritten bestehen:

```text
Checkpoint
    ↓
Transfer
    ↓
Restore on Target
    ↓
Verify
    ↓
Switch ownership / routing
    ↓
Terminate Source
```

Die vollständige Migration wird separat spezifiziert.

---

## Split-Brain Prevention

Bei Migration darf nicht unbeabsichtigt derselbe exklusive Domainzustand gleichzeitig auf Quell- und Zielsystem aktiv werden.

Für solche Ressourcen ist ein eindeutiger Ownership Transfer notwendig.

---

## Resume after Checkpoint

Nach erfolgreicher Checkpoint-Erstellung kann das ursprüngliche Domain:

```text
Resume
```

oder:

```text
Terminate
```

werden.

Dies ist eine Policyentscheidung.

---

## Restore Identity

Beim Restore muss unterschieden werden zwischen:

```text
same logical domain
```

und:

```text
new domain restored from checkpoint
```

Die konkrete Identity-Semantik muss explizit sein.

Rohe Prozess- oder Thread-IDs werden nicht blind wiederverwendet.

---

## External Observers

Supervisoren, IPC-Peers und andere Systemkomponenten müssen bei Restore gegebenenfalls neue Referenzen erhalten oder über Rebinding informiert werden.

Stale Handles dürfen nicht still auf ein neues Domain zeigen.

---

## Architecture Introspection

Checkpointfähigkeit und Checkpoints müssen introspektierbar sein.

Beispiel:

```text
Execution Domain:
    nova.editor

Checkpoint Capability:
    Checkpointable

Last Checkpoint:
    cp-2026-08-31-001

Consistency:
    ApplicationConsistent

Memory:
    420 MiB

External Resources:
    7

Non-Migratable Resources:
    1
```

---

## Checkpoint Decision Record

Komplexe Checkpoint-Vorgänge sollen strukturiert nachvollziehbar sein.

Konzeptionell:

```text
CheckpointDecision {
    domain
    requestedMode
    selectedStrategy
    requiredProviders
    rejectedProviders
    consistencyClass
    result
}
```

---

## Audit

Sicherheitsrelevante Checkpoint- und Restore-Ereignisse sollen auditierbar sein.

Beispiele:

```text
CheckpointCreated
CheckpointExported
RestoreRequested
RestoreDenied
RestoreCompleted
MigrationStarted
```

Dies ist insbesondere relevant, da Checkpoints große Mengen sensitiven Speichers enthalten können.

---

## Fail-Safe Defaults

Kann eine Ressource nicht eindeutig und sicher checkpointet werden, gilt sie nicht automatisch als checkpointfähig.

Kann Authority beim Restore nicht sicher rekonstruiert werden, wird sie nicht gewährt.

Kann ein Required Provider nicht wiederhergestellt werden, schlägt Restore fehl.

---

## Deterministic Mode

Checkpoint und Restore müssen mit Deterministic Mode kompatibel sein.

Dazu können stabilisiert werden:

```text
checkpoint ordering
provider ordering
serialization order
CPU state
resource mapping
restore decisions
```

Adaptive Restore-Entscheidungen müssen in deterministischen Scopes deaktivierbar sein.

---

## KI-Unabhängigkeit

Checkpointing und Restore sind grundlegende Systemfunktionen.

Sie müssen vollständig ohne KI funktionieren.

KI darf optional:

- günstige Checkpoint-Zeitpunkte vorhersagen,
- Speicherbedarf schätzen,
- Migrationsempfehlungen geben.

KI darf nicht entscheiden, unsicheren Zustand still als gültig zu behandeln.

---

## Normative Anforderungen

1. NovaOS MUSS Execution Domains eine explizite Checkpointfähigkeit zuordnen können.
2. Checkpointfähigkeit DARF NICHT für jedes Domain automatisch vorausgesetzt werden.
3. Checkpoints MÜSSEN strukturierte versionierte Systemobjekte sein.
4. Ein Checkpoint DARF NICHT lediglich aus einem ungeprüften Memory Dump bestehen.
5. Checkpointing MUSS einen kontrollierten Quiesce- beziehungsweise Konsistenzpunkt unterstützen.
6. Thread- und Memory-Zustand MÜSSEN semantisch erfasst werden können.
7. Kernelhandles DÜRFEN NICHT als rohe numerische IDs persistiert werden.
8. Externe Ressourcen MÜSSEN eine explizite Checkpoint-/Restore-Semantik besitzen.
9. Resource Provider MÜSSEN ihre Checkpointfähigkeiten melden können.
10. Aktive DMA- und vergleichbare Hardwareoperationen MÜSSEN vor einem sicheren Checkpoint kontrolliert behandelt werden.
11. GPU-/NPU-Kontexte DÜRFEN NICHT universell als checkpointfähig angenommen werden.
12. Shared Memory MUSS konsistent über explizite Policies checkpointbar sein.
13. Mehrere abhängige Domains MÜSSEN als Checkpoint Group behandelt werden können.
14. Persistente Checkpoints MÜSSEN atomar als gültig veröffentlicht werden.
15. Unvollständige Checkpoints DÜRFEN NICHT als Restore-Quelle verwendet werden.
16. Checkpointformate MÜSSEN von internen Kernelstrukturen entkoppelt und versioniert sein.
17. Restore MUSS Architektur- und ABI-Kompatibilität prüfen.
18. Executable Identity und Trust MÜSSEN beim Restore validiert werden.
19. Capabilities MÜSSEN beim Restore erneut autorisiert werden.
20. Ein Checkpoint DARF KEINE dauerhafte Authority-Garantie darstellen.
21. Data-Sovereignty-Regeln MÜSSEN bei Speicherung, Transfer und Restore eingehalten werden.
22. Sensitive Checkpoints MÜSSEN abhängig von Policy verschlüsselbar sein.
23. Hard Restore Dependencies DÜRFEN NICHT stillschweigend weggelassen werden.
24. Restore MUSS vor Aktivierung transaktional fehlschlagen können.
25. Partielle Restore-Semantik DARF nur explizit unterstützt werden.
26. Checkpointing MUSS in die systemweite Resource Economy integriert werden.
27. Checkpoint- und Restore-Vorgänge MÜSSEN mit Structured Concurrency integrierbar sein.
28. Checkpointfähigkeit, Abhängigkeiten und Restore-Ergebnisse MÜSSEN introspektierbar sein.
29. Sicherheitsrelevante Checkpoint-/Restore-Ereignisse SOLLEN auditierbar sein.
30. Die vollständige grundlegende Checkpoint- und Restore-Funktionalität MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- Grundlage für Process Recovery,
- Vorbereitung für Live Migration,
- bessere Unterstützung langlebiger Anwendungen,
- Integration mit Process Supervision,
- kontrollierte Suspend-/Resume-Szenarien,
- systemweite statt ad-hoc Checkpoint-Semantik,
- externe Ressourcen werden ausdrücklich berücksichtigt,
- gute Grundlage für Hot Replacement und Live Evolution,
- Checkpoints können sicher mit Capability-, Trust- und Data-Sovereignty-Regeln verknüpft werden.

### Negative Konsequenzen

- Checkpointing komplexer Domains ist technisch aufwendig,
- externe Ressourcen benötigen Provider-spezifische Unterstützung,
- persistente Checkpointformate benötigen langfristige Versionierung,
- Accelerator- und Netzwerkzustände sind teilweise nicht vollständig portabel,
- Group Checkpoints erhöhen Koordinationsaufwand.

---

## Verworfene Alternativen

### Checkpoint als vollständiger RAM-Dump

Verworfen.

Dies berücksichtigt Handles, Geräte, IPC, Capabilities und externe Ressourcen nicht ausreichend.

### Jedes Domain automatisch checkpointfähig

Verworfen.

Nicht alle Ressourcen besitzen eine sichere Restore-Semantik.

### Rohe Kernelobjekte persistent serialisieren

Verworfen.

Dies würde Checkpoints an interne Kernelversionen und Speicherlayouts koppeln.

### Capabilities unverändert aus dem Checkpoint übernehmen

Verworfen.

Security Policy und Revocation können sich seit der Checkpoint-Erstellung geändert haben.

### Externe Ressourcen beim Restore still entfernen

Verworfen.

Required Resources dürfen nicht unbemerkt verloren gehen.

### Checkpointing ausschließlich in Anwendungen implementieren

Verworfen.

Anwendungen können Kernel-, Speicher- und Hardwarezustand nicht vollständig kontrollieren.

### Virtuelle Maschine als einzige Checkpoint-Einheit

Verworfen.

NovaOS soll native Execution Domains checkpointen können, ohne eine Hypervisor-First-Architektur zu benötigen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-PROCESS-CHECKPOINT-FORMAT-0001`
- `NPSPEC-PROCESS-CHECKPOINT-LIFECYCLE-0001`
- `NPSPEC-PROCESS-CHECKPOINT-MEMORY-0001`
- `NPSPEC-PROCESS-CHECKPOINT-THREAD-0001`
- `NPSPEC-PROCESS-CHECKPOINT-HANDLE-0001`
- `NPSPEC-PROCESS-CHECKPOINT-PROVIDER-0001`
- `NPSPEC-PROCESS-CHECKPOINT-GROUP-0001`
- `NPSPEC-PROCESS-CHECKPOINT-SECURITY-0001`
- `NPSPEC-PROCESS-CHECKPOINT-STORAGE-0001`
- `NPSPEC-PROCESS-RESTORE-0001`
- `NPSPEC-PROCESS-CHECKPOINT-MIGRATION-0001`
- `NPSPEC-PROCESS-CHECKPOINT-INTROSPECTION-0001`
- `NPSPEC-PROCESS-CHECKPOINT-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Checkpoint eines einfachen Single-Thread-Domains,
- Checkpoint eines Multi-Thread-Domains,
- konsistente Threadzustände,
- private Memory Pages,
- Copy-on-Write-Memory,
- Shared Memory,
- inkrementeller Checkpoint,
- Restore aus Basis plus Delta,
- fehlende Delta-Basis,
- offene Datei mit Restore,
- veränderte externe Datei,
- Handle-Rebinding,
- IPC-Rebinding,
- nicht restaurierbarer Endpoint,
- ausstehendes Async I/O,
- aktive DMA-Operation,
- checkpointfähiger Device Provider,
- nicht checkpointfähiger Device Provider,
- GPU-/NPU-Kontext ohne Provider-Unterstützung,
- Capability-Neubewertung,
- widerrufene Capability,
- Trust-Prüfung,
- manipulierter Checkpoint,
- verschlüsselter Checkpoint,
- Data-Sovereignty-Verstoß beim Export,
- Group Checkpoint,
- Fehler eines Gruppenteilnehmers,
- atomare Checkpoint-Veröffentlichung,
- Restore mit fehlender Required Dependency,
- Restore mit optionaler Degradation,
- ABI-Inkompatibilität,
- Deterministic Checkpoint,
- Supervisor-Restore nach Crash,
- Introspection,
- Audit.

---

## Ergebnis

NovaOS behandelt Checkpointing nicht als rohe Kopie eines laufenden Prozesses, sondern als strukturierte Erfassung eines Execution-Domain-Zustands und seiner externen Verträge.

Das Modell lautet:

```text
Execution Domain
      ↓
Quiesce
      ↓
Capture
      ├── Memory
      ├── Threads
      ├── Handles
      ├── Capabilities
      ├── Resources
      └── External Dependencies
      ↓
Validated Checkpoint
      ↓
Restore / Migration / Recovery
```

Damit entsteht eine Grundlage für:

```text
checkpoint
restore
supervised recovery
migration
live evolution
```

ohne vorauszusetzen, dass jeder Hardware- oder Runtimezustand beliebig serialisierbar ist.

Die zentrale Architekturregel lautet:

```text
Checkpoint semantics,
not raw machine state.

Revalidate authority.

Rebind external resources.

Fail when required state cannot be restored.

Portability must be explicit.
```