# ADR-ARCH-0012 – Hot Replacement und Live Evolution

## Status

Angenommen

## Kategorie

Systemarchitektur / Lifecycle / Runtime Evolution / Hot Replacement / Live Update / Resilience

## Kontext

Klassische Betriebssysteme behandeln Software, Treiber, Systemdienste und teilweise sogar große Teile der Systemarchitektur als weitgehend statische Komponenten.

Änderungen erfolgen typischerweise über:

```text
Install
   ↓
Stop
   ↓
Replace
   ↓
Restart
```

oder bei systemnahen Komponenten:

```text
Update
   ↓
Reboot
   ↓
New Version
```

Dieses Modell ist einfach, besitzt jedoch erhebliche Nachteile:

- laufende Arbeit wird unterbrochen,
- Dienste müssen neu gestartet werden,
- offene Zustände können verloren gehen,
- Updates verursachen Downtime,
- Sicherheitsupdates werden verzögert,
- Treiberupdates können Neustarts verlangen,
- lang laufende Berechnungen werden unterbrochen,
- verteilte Dienste müssen aufwendig koordiniert werden,
- Systemkomponenten können nicht schrittweise weiterentwickelt werden.

NovaOS ist als langfristig evolvierendes Betriebssystem konzipiert.

Die Architektur soll nicht davon ausgehen, dass alle Komponenten während der gesamten Lebensdauer einer Systeminstanz unverändert bleiben.

Stattdessen sollen geeignete Komponenten während des laufenden Betriebs:

- ersetzt,
- aktualisiert,
- migriert,
- neu gebunden,
- erweitert,
- zurückgerollt

werden können.

Diese Fähigkeit wird unter dem Begriff:

```text
Hot Replacement
```

zusammengefasst.

Darüber hinaus soll NovaOS langfristig eine kontrollierte:

```text
Live Evolution
```

unterstützen.

Live Evolution geht über das bloße Ersetzen einer Binärdatei hinaus.

Sie umfasst die kontrollierte Evolution von:

- Implementierungen,
- Fähigkeiten,
- Providern,
- Diensten,
- Datenmodellen,
- Zuständen,
- Protokollen,
- Schnittstellen,
- Pipelines,
- Execution Domains,
- System Policies.

Dabei dürfen jedoch zentrale Garantien von NovaOS nicht verletzt werden.

Insbesondere:

- Safety,
- Security,
- Trust,
- Data Sovereignty,
- Information Flow,
- Determinism,
- Realtime,
- Resource Contracts,
- Transactional Consistency.

Hot Replacement darf deshalb nicht als simples Überschreiben laufender Binärdateien implementiert werden.

Es benötigt ein systemweites Lifecycle- und Evolutionsmodell.

---

## Entscheidung

NovaOS führt **Hot Replacement und Live Evolution als grundlegendes Architekturprinzip** ein.

Geeignete Systemkomponenten sollen während des laufenden Betriebs durch kompatible neue Versionen ersetzt werden können.

Der grundlegende Ablauf lautet:

```text
Current Version
      │
      ▼
Replacement Candidate
      │
      ▼
Validate
      │
      ▼
Prepare
      │
      ▼
Stage
      │
      ▼
Quiesce / Handover
      │
      ▼
State Transfer
      │
      ▼
Rebind
      │
      ▼
Activate
      │
      ▼
Verify
      │
      ▼
Commit
```

Bei Fehlern:

```text
Failure
   │
   ▼
Contain
   │
   ├── Rollback
   │
   ├── Forward Recovery
   │
   └── Safe Degraded State
```

Hot Replacement wird eng mit transaktionalen Systemoperationen verbunden.

---

## Grundprinzip

Das zentrale Prinzip lautet:

```text
Replace implementation without unnecessarily
destroying the logical system state.
```

Eine logische Fähigkeit soll möglichst unabhängig von ihrer konkreten Implementierungsinstanz betrachtet werden.

Beispiel:

```text
Capability
    de.nova.image.decode.jpeg
            │
            ▼
      Provider v3.1
```

wird zu:

```text
Capability
    de.nova.image.decode.jpeg
            │
            ▼
      Provider v3.2
```

Die logische Capability-Identität bleibt bestehen.

Die konkrete Implementierung kann wechseln.

---

## Capability und Implementation

NovaOS unterscheidet strikt zwischen:

```text
Capability Identity
```

und:

```text
Capability Implementation
```

Beispiel:

```text
Capability:
    de.nova.image.filter.gaussian
```

kann bereitgestellt werden durch:

```text
Provider A
Provider B
Provider C
```

oder unterschiedliche Versionen:

```text
Provider A v1
Provider A v2
Provider A v3
```

Diese Trennung bildet eine wesentliche Grundlage für Hot Replacement.

---

## Logical Identity

Eine laufende Systemfunktion soll nicht unnötig an eine konkrete Speicheradresse, Prozess-ID oder Binärdatei gebunden sein.

Stattdessen werden stabile logische Identitäten verwendet.

Konzeptionell:

```text
Logical Identity
       │
       ▼
Binding
       │
       ▼
Current Implementation
```

Das Binding kann kontrolliert geändert werden.

---

## Indirection

Hot Replacement benötigt kontrollierte Indirection.

Beispiel:

```text
Caller
   │
   ▼
Capability Reference
   │
   ▼
Binding
   │
   ▼
Provider
```

nicht:

```text
Caller
   │
   ▼
Hardcoded Provider Address
```

Indirection darf jedoch nicht bedeuten, dass jeder Funktionsaufruf zwingend einen teuren globalen Lookup durchführen muss.

Bindings können:

- gecacht,
- lokalisiert,
- versioniert,
- inline optimiert

werden.

---

## Fast Path

Nach erfolgreicher Bindung darf ein Fast Path verwendet werden.

Beispiel:

```text
Initial Resolution
       ↓
Validated Binding
       ↓
Cached Handle
       ↓
Fast Invocation
```

Bei Replacement wird das Binding kontrolliert invalidiert oder aktualisiert.

---

## Replacement Scope

Hot Replacement kann unterschiedliche Scopes besitzen.

Mindestens:

```text
Function
Capability Implementation
Library
Service
Driver
Provider
Execution Domain
Pipeline Node
Policy Module
Model
System Component
```

Nicht jede Komponente muss jede Replacement-Stufe unterstützen.

---

## Replacement Classes

NovaOS unterscheidet konzeptionell mehrere Replacement-Klassen.

### Stateless Replacement

Komponente besitzt keinen relevanten persistenten Runtime-State.

```text
Old
 ↓
Replace
 ↓
New
```

Dies ist die einfachste Form.

### Restartable Replacement

Komponente besitzt State, kann diesen jedoch aus persistentem oder rekonstruierbarem Zustand wiederherstellen.

```text
Old
 ↓
Stop
 ↓
New
 ↓
Restore
```

### Stateful Replacement

Runtime-State muss direkt übertragen werden.

```text
Old State
    │
    ▼
State Export
    │
    ▼
State Migration
    │
    ▼
State Import
    │
    ▼
New Version
```

### Parallel Replacement

Alte und neue Version laufen kurzzeitig parallel.

```text
          ┌── Old Version
Input ────┤
          └── New Version
```

Danach erfolgt kontrolliertes Umschalten.

### Rolling Replacement

Mehrere Instanzen werden schrittweise ersetzt.

```text
Instance 1 → New
Instance 2 → New
Instance 3 → New
...
```

### Transactional Replacement

Replacement wird als Systemtransaktion ausgeführt.

```text
Prepare
Stage
Validate
Switch
Verify
Commit
```

### Reboot-Requiring Replacement

Bestimmte fundamentale Komponenten können nicht sicher live ersetzt werden.

Dann gilt:

```text
Replacement Prepared
       ↓
Reboot Required
       ↓
Atomic Activation During Boot
```

Hot Replacement ist daher kein Dogma.

---

## Replacement Eligibility

Jede ersetzbare Komponente muss deklarieren können, welche Replacement-Fähigkeiten sie besitzt.

Konzeptionell:

```text
ReplacementProfile {
    supported
    stateful
    parallel
    rollback
    migration
    quiescence
    compatibility
}
```

---

## Lifecycle Model

Hot-replace-fähige Komponenten besitzen einen expliziten Lifecycle.

Beispiel:

```text
Discovered
    ↓
Validated
    ↓
Loaded
    ↓
Prepared
    ↓
Active
    ↓
Quiescing
    ↓
Quiescent
    ↓
Replaced
    ↓
Retired
    ↓
Unloaded
```

Fehlerzustände können sein:

```text
Failed
Degraded
RollbackPending
RecoveryPending
```

---

## Component Generations

NovaOS behandelt unterschiedliche aktive Versionen einer logischen Komponente als Generationen.

Beispiel:

```text
Generation 41
    ↓
Generation 42
```

Eine Generation erhält eine stabile Generation Identity.

Konzeptionell:

```text
ComponentGeneration {
    LogicalIdentity
    Version
    GenerationID
    Provider
    StateSchema
    TrustState
}
```

---

## Generation Binding

Laufende Operationen können weiterhin an eine ältere Generation gebunden bleiben.

Beispiel:

```text
Existing Execution
        │
        ▼
Generation 41

New Execution
        │
        ▼
Generation 42
```

Dadurch muss ein Replacement nicht zwangsläufig laufende Operationen abbrechen.

---

## Generation Retirement

Eine alte Generation darf erst vollständig entfernt werden, wenn keine relevanten Bindungen mehr existieren oder diese sicher migriert wurden.

Beispiel:

```text
Generation 41
    │
    ├── Active Reference A
    ├── Active Reference B
    └── Active Reference C
```

Nach Abschluss:

```text
Reference Count / Lease State = 0
             ↓
          Retire
```

Ein einfacher Reference Count ist nicht für alle Subsysteme ausreichend.

NovaOS kann zusätzlich verwenden:

- Leases,
- epochs,
- quiescent-state detection,
- generation tracking,
- dependency tracking.

---

## Quiescence

Ein zentraler Begriff ist:

```text
Quiescence
```

Eine Komponente ist quiescent, wenn sie einen Zustand erreicht hat, in dem sie sicher ersetzt oder migriert werden kann.

Beispiel:

```text
No active critical operation
No mutable state transition in progress
No unsafe external transaction
No untransferable resource ownership
```

---

## Quiescence Protocol

Der grundlegende Ablauf lautet:

```text
Request Quiescence
       ↓
Stop Accepting New Work
       ↓
Drain Existing Work
       ↓
Complete / Cancel / Transfer
       ↓
Reach Safe Point
       ↓
Quiescent
```

---

## Safe Points

Komponenten können explizite Safe Points definieren.

Beispiel:

```text
Processing
    ↓
Safe Point
    ↓
Processing
    ↓
Safe Point
```

Replacement darf dann bevorzugt an einem Safe Point stattfinden.

---

## Long-Running Operations

Lange Operationen dürfen nicht zwingend vollständig abgewartet werden.

Mögliche Strategien:

```text
Drain
Cancel
Checkpoint
Migrate
Continue on Old Generation
```

Die Strategie wird durch:

- Execution Contract,
- Replacement Policy,
- Component Capability,
- Resource Situation

bestimmt.

---

## Structured Concurrency

Hot Replacement integriert sich mit:

```text
ADR-ARCH-0010
Structured Concurrency
```

Ein Execution Scope macht sichtbar, welche Tasks zu einer Komponente oder Generation gehören.

Beispiel:

```text
Provider Generation 41
└── Execution Scope
    ├── Task A
    ├── Task B
    └── Task C
```

Quiescence kann dadurch strukturiert erfolgen.

---

## Cancellation

Structured Cancellation kann verwendet werden, wenn laufende Operationen nicht migrierbar sind.

Dabei muss der jeweilige Execution Contract berücksichtigt werden.

---

## State Transfer

Stateful Replacement benötigt ein definiertes State-Transfer-Modell.

Grundablauf:

```text
Old Runtime State
       ↓
Export
       ↓
Canonical State Representation
       ↓
Migration
       ↓
Validation
       ↓
Import
       ↓
New Runtime State
```

---

## State Schema

State muss eine explizite semantische Struktur besitzen.

Beispiel:

```text
StateSchema {
    identity
    version
    semanticTypes
    fields
    invariants
}
```

---

## State Schema Version

State Schemas müssen versioniert sein.

Beispiel:

```text
StateSchema v4
       ↓
Migration
       ↓
StateSchema v5
```

---

## State Migration

Eine neue Version kann eine Migration bereitstellen.

Beispiel:

```text
Migrate(
    State<v4>
) -> State<v5>
```

---

## Migration Chain

Mehrere Versionen können über eine kontrollierte Migrationskette verbunden sein.

```text
v2
 ↓
v3
 ↓
v4
 ↓
v5
```

NovaOS soll jedoch unnötig lange Migrationsketten vermeiden können.

Direkte Migrationen dürfen bereitgestellt werden.

---

## Migration Validation

Nach State Migration müssen definierte Invarianten geprüft werden.

Beispiel:

```text
State migrated
      ↓
Schema Validation
      ↓
Invariant Validation
      ↓
Semantic Validation
```

---

## State Transfer Failure

Schlägt State Transfer fehl, darf die alte Generation nicht vorschnell zerstört werden.

Beispiel:

```text
Migration Failed
       ↓
Old Generation remains authoritative
       ↓
Rollback Replacement
```

---

## Canonical State

Wenn sinnvoll, soll ein implementationsunabhängiges State Format verwendet werden.

Dadurch kann State zwischen unterschiedlichen Providern übertragen werden.

Beispiel:

```text
Provider A
    ↓
Canonical State
    ↓
Provider B
```

---

## Semantic Types

State Transfer muss:

```text
ADR-ARCH-0008
Semantic Types
```

verwenden können.

Beispiel:

```text
Temperature<Celsius>
```

darf nicht unkontrolliert als:

```text
Float
```

migriert werden, wenn dadurch Semantik verloren geht.

---

## State Ownership

Vor Replacement muss klar sein, wer State besitzt.

Mögliche Kategorien:

```text
Component-Owned
Shared
External
Persistent
Reconstructable
Ephemeral
```

---

## Shared State

Shared State benötigt besondere Koordination.

Eine neue Generation darf nicht gleichzeitig unkontrolliert mit einer alten Generation denselben mutierbaren State verändern.

Mögliche Techniken:

- MVCC,
- versioned objects,
- copy-on-write,
- ownership transfer,
- transactional locking,
- immutable snapshots.

---

## Zero-Copy

Hot Replacement muss mit:

```text
ADR-ARCH-0006
Zero-Copy
```

kompatibel sein.

Aktive Shared Buffers dürfen nicht ungültig werden, nur weil ein Provider ersetzt wurde.

---

## Buffer Ownership Transfer

Beispiel:

```text
Old Provider
    │
    ▼
Shared Buffer
    │
    ▼
Ownership Transfer
    │
    ▼
New Provider
```

Der physische Buffer muss nicht kopiert werden, wenn:

- Security,
- Lifetime,
- Synchronisation,
- Memory Domain

dies erlauben.

---

## DMA

Treiber- und Hardware-Replacement muss aktive DMA-Operationen berücksichtigen.

Vor Device- oder Driver-Rebinding müssen relevante DMA-Operationen:

```text
Complete
Cancel
Drain
Transfer
```

werden.

---

## Capability Binding

Capability-Aufrufe erfolgen über kontrollierte Bindings.

Beispiel:

```text
Capability
       ↓
Binding
       ↓
Provider Generation
```

Hot Replacement kann das Binding ändern.

---

## Atomic Rebinding

Das Umschalten eines Bindings muss aus Sicht neuer Aufrufe atomar erfolgen können.

Beispiel:

```text
Before Commit:
    Binding → Generation 41

After Commit:
    Binding → Generation 42
```

Es darf keinen sichtbaren undefinierten Zwischenzustand geben.

---

## Existing Calls

Bereits laufende Calls können:

```text
ContinueOld
Migrate
Cancel
Restart
```

werden.

Die konkrete Semantik wird durch den Capability Contract definiert.

---

## New Calls

Nach dem Commit sollen neue Calls standardmäßig die neue Generation verwenden.

---

## Versioned Interfaces

Hot Replacement benötigt versionierte Interfaces.

Beispiel:

```text
Capability Interface v3
```

Neue Implementierung:

```text
supports Interface v3
```

kann direkt gebunden werden.

---

## Interface Compatibility

NovaOS unterscheidet mindestens:

```text
Compatible
Adaptable
Migratable
Incompatible
```

---

## Backward Compatibility

Eine neue Implementierung kann ältere Interface-Versionen weiterhin unterstützen.

Beispiel:

```text
Provider v5 {
    supports Interface v3
    supports Interface v4
    supports Interface v5
}
```

---

## Compatibility Adapter

Wenn direkte Kompatibilität fehlt, kann ein Adapter verwendet werden.

```text
Caller v3
    ↓
Adapter
    ↓
Provider v4
```

Adapter müssen denselben:

- Security,
- Trust,
- Resource,
- Determinism,
- Sovereignty

Constraints unterliegen.

---

## ABI

NovaABI muss Hot Replacement unterstützen.

Dies bedeutet insbesondere:

- stabile Handles statt unnötiger direkter Pointer-Abhängigkeiten,
- versionierte Interfaces,
- kontrollierte Object Lifetimes,
- Capability-basierte Bindings.

---

## ABI Stability

Hot Replacement bedeutet nicht, dass jedes interne ABI für immer unveränderlich sein muss.

Stattdessen soll Evolution über:

```text
Versioning
Negotiation
Adapters
Migration
```

ermöglicht werden.

---

## Data Structure Evolution

Interne Datenstrukturen dürfen sich zwischen Generationen ändern.

State Migration muss diese Unterschiede behandeln.

---

## Kernel Objects

Kernel Objects benötigen besondere Vorsicht.

Fundamentale Kernelobjekte dürfen nicht beliebig live ersetzt werden.

---

## Kernel Replacement Classes

Kernel-Komponenten können beispielsweise klassifiziert werden als:

```text
HotReplaceable
QuiescenceReplaceable
BootReplaceable
RebootRequired
ImmutableForBootGeneration
```

---

## Kernel Generation

NovaOS kann langfristig bestimmte Kernel-Subsysteme generationenbasiert ersetzen.

Dies ist jedoch keine Voraussetzung dafür, dass der gesamte Kernel live austauschbar sein muss.

---

## Kein vollständiger Live-Kernel-Zwang

Diese ADR verlangt ausdrücklich nicht:

```text
Every kernel instruction must be replaceable live.
```

Fundamentale Komponenten dürfen einen Neustart verlangen.

---

## Drivers

Treiber sind ein wichtiger Hot-Replacement-Anwendungsfall.

Beispiel:

```text
Driver v7
   ↓
Quiesce Device
   ↓
Export Device State
   ↓
Load Driver v8
   ↓
Import State
   ↓
Rebind Device
   ↓
Resume
```

---

## User-Mode Driver Domains

User-Mode beziehungsweise isolierte Driver Domains erleichtern Hot Replacement.

Ein Driver Domain kann ersetzt werden, ohne den Kernel selbst auszutauschen.

---

## Device State

Nicht jedes Gerät unterstützt vollständigen State Transfer.

Mögliche Strategien:

```text
Preserve
Reinitialize
Reset
Reenumerate
```

---

## Device Reset

Wenn ein Reset erforderlich ist, muss die dadurch entstehende Unterbrechung sichtbar und contractkonform sein.

---

## Storage Drivers

Storage Driver Replacement benötigt besondere Sicherheitsanforderungen.

Ein Replacement darf keine:

- uncommitted writes,
- filesystem corruption,
- lost flushes,
- invalid DMA

verursachen.

---

## Network Drivers

Netzwerkverbindungen sollen, wenn technisch möglich, über Driver Replacement hinweg erhalten bleiben.

Dies ist jedoch abhängig von:

- Hardware,
- Network Stack,
- State Transfer,
- Connection Ownership.

---

## GPU Provider

GPU Runtime und GPU Provider können ersetzt werden.

Aktive GPU Workloads müssen:

```text
Drain
Checkpoint
Complete
Cancel
```

werden.

---

## NPU Provider

Dasselbe gilt für NPU- und andere Accelerator Provider.

---

## Services

Systemdienste sollen bevorzugt hot-replace-fähig entworfen werden.

Beispiel:

```text
Nova.Indexer v4
       ↓
Nova.Indexer v5
```

ohne vollständigen Systemneustart.

---

## Service Endpoints

Service Clients sollen stabile logische Endpoints verwenden.

```text
Client
  ↓
Service Capability
  ↓
Current Service Generation
```

---

## Connection Handover

Lang laufende Verbindungen können übergeben werden, wenn Protokoll und Runtime dies unterstützen.

---

## Session Preservation

Ein Service Replacement soll Benutzersitzungen möglichst erhalten.

---

## Application Hot Replacement

Auch Anwendungen können ihre Capability-Implementierungen aktualisieren.

Dies kann insbesondere für adaptive NovaOS-Programme relevant sein.

---

## Adaptive Applications

Da NovaOS-Anwendungen aus Fähigkeiten zusammengesetzt werden können, kann eine einzelne Fähigkeit ersetzt werden, ohne die gesamte Anwendung neu zu starten.

Beispiel:

```text
Document Workspace
├── Text Editing
├── Spell Checking v2
├── PDF Export
└── Image Processing
```

wird zu:

```text
Document Workspace
├── Text Editing
├── Spell Checking v3
├── PDF Export
└── Image Processing
```

---

## Adaptive Ribbon

Wenn sich die verfügbaren Fähigkeiten ändern, kann sich auch die deklarativ erzeugte Ribbon-Oberfläche aktualisieren.

Diese Änderung muss jedoch UI-State und Benutzerkontext berücksichtigen.

---

## UI Live Evolution

`.nui`-beschriebene Oberflächen können aktualisiert werden.

Dabei sollen relevante UI-Zustände erhalten bleiben.

Beispiele:

- Fokus,
- Auswahl,
- Scrollposition,
- offene Dokumente,
- Fensterzustand.

---

## UI State Migration

UI State kann wie anderer Runtime-State versioniert und migriert werden.

---

## Declarative System Model

Hot Replacement integriert sich direkt mit:

```text
ADR-ARCH-0003
Deklaratives Systemmodell
```

Das deklarative Modell beschreibt den gewünschten Zustand.

Beispiel:

```text
Desired:
    ProviderVersion = 5

Actual:
    ProviderVersion = 4
```

Der Reconciler plant:

```text
Replacement 4 → 5
```

---

## Reconciliation

Nach Replacement wird geprüft:

```text
Desired State
    ==
Actual State
```

Wenn nicht:

```text
Reconcile
```

---

## Transactional System Operations

Hot Replacement integriert sich mit:

```text
ADR-ARCH-0004
Transaktionale Systemoperationen
```

Ein Replacement soll, wenn sinnvoll, als Transaktion behandelt werden.

---

## Replacement Transaction

Konzeptionell:

```text
ReplacementTransaction {
    currentGeneration
    targetGeneration
    participants
    stateMigration
    resourceReservation
    rebindingPlan
    verification
    rollbackPlan
}
```

---

## Transaction Phases

```text
Plan
 ↓
Validate
 ↓
Reserve
 ↓
Stage
 ↓
Prepare
 ↓
Quiesce
 ↓
Migrate
 ↓
Activate
 ↓
Rebind
 ↓
Verify
 ↓
Commit
```

---

## Commit Point

Der Replacement-Prozess benötigt einen klar definierten Commit Point.

Vor dem Commit ist die alte Generation grundsätzlich weiterhin authoritative.

Nach Commit ist die neue Generation authoritative.

---

## Point of No Return

Bestimmte Hardware- oder Datenmigrationen können einen Point of No Return besitzen.

Dieser muss explizit bekannt sein.

Vor Überschreiten müssen:

- Preconditions,
- Backup,
- Recovery Path,
- Resource Guarantees

geprüft werden.

---

## Rollback

Wenn die neue Generation nicht korrekt funktioniert, soll NovaOS auf die alte Generation zurückkehren können.

Beispiel:

```text
Generation 41
      ↓
Generation 42
      ↓
Health Failure
      ↓
Rollback
      ↓
Generation 41
```

---

## Rollback State

Rollback ist nur möglich, wenn der alte State noch kompatibel beziehungsweise rekonstruierbar ist.

---

## Reverse Migration

Falls die neue Generation den State bereits verändert hat, kann eine Reverse Migration erforderlich sein.

```text
State v5
   ↓
Reverse Migration
   ↓
State v4
```

Nicht jede Migration ist reversibel.

---

## Forward Recovery

Wenn Rollback nicht möglich ist, muss Forward Recovery unterstützt werden können.

```text
Failed Generation
       ↓
Repair / Replacement
       ↓
Generation 43
```

---

## Canary Activation

Neue Generationen können zunächst nur für einen Teil der Workloads aktiviert werden.

Beispiel:

```text
95 % → Old
5 %  → New
```

Danach:

```text
50 % → Old
50 % → New
```

und schließlich:

```text
100 % → New
```

---

## Shadow Execution

Neue Implementierungen können parallel ausgeführt werden, ohne ihr Ergebnis zunächst authoritative zu machen.

```text
Input
  │
  ├── Old Provider → Authoritative Result
  │
  └── New Provider → Shadow Result
```

Ergebnisse können verglichen werden.

---

## Differential Verification

Beispiel:

```text
OldResult
    vs
NewResult
```

Bei semantischer Gleichheit kann die neue Generation freigegeben werden.

---

## Semantic Equality

Semantic Types können definieren, was als gleichwertiges Ergebnis gilt.

Nicht jede Validierung muss bitidentisch sein.

---

## Health Verification

Nach Aktivierung muss die neue Generation geprüft werden.

Mögliche Signale:

- Crash Rate,
- Error Rate,
- Latency,
- Deadline Misses,
- Memory Usage,
- Resource Violations,
- Security Violations,
- Output Validation,
- Health Checks.

---

## Grace Period

Nach Replacement kann eine Grace Period gelten.

Während dieser Zeit bleibt die alte Generation rollbackfähig.

```text
Activate New
     ↓
Grace Period
     ↓
Stable
     ↓
Retire Old
```

---

## Resource Economy

Hot Replacement integriert sich mit:

```text
ADR-ARCH-0005
Systemweite Ressourcenökonomie
```

Während eines Replacements können alte und neue Generation gleichzeitig Ressourcen benötigen.

Beispiel:

```text
Normal:
    Memory = 500 MiB

Replacement:
    Old = 500 MiB
    New = 600 MiB

Temporary Total:
    1100 MiB
```

Diese temporären Ressourcen müssen berücksichtigt werden.

---

## Replacement Resource Contract

Beispiel:

```text
ReplacementResources {
    memory {
        temporary = 600 MiB
    }

    cpu {
        preferred = 2 cores
    }

    deadline = 5 s
}
```

---

## Admission Control

Ein Replacement darf nicht begonnen werden, wenn notwendige Ressourcen für eine sichere Parallelphase fehlen.

Mögliche Ergebnisse:

```text
Accepted
Deferred
Rejected
AlternativeStrategy
```

---

## Resource Pressure

Unter Resource Pressure kann NovaOS:

- Replacement verzögern,
- Parallelphase verkürzen,
- Shadow Execution deaktivieren,
- andere Strategie wählen.

Security-kritische Updates können höhere Priorität besitzen.

---

## Emergency Replacement

Ein kompromittierter Provider kann sofort ersetzt werden müssen.

Dann kann:

```text
Security > Availability
```

gelten.

---

## Execution Contracts

Aktive:

```text
Nova.ExecutionContract
```

müssen beim Replacement berücksichtigt werden.

Beispiele:

```text
latency
deadline
determinism
resources
trust
sovereignty
algorithm
```

---

## Contract Preservation

Ein Replacement darf bestehende Hard Contracts nicht stillschweigend verletzen.

---

## Contract Revalidation

Vor Rebinding muss geprüft werden:

```text
Does New Generation satisfy
existing Execution Contract?
```

Wenn nein:

```text
Do not rebind
```

oder:

```text
Explicitly renegotiate
```

---

## Deterministic Mode

Hot Replacement integriert sich mit:

```text
ADR-ARCH-0011
Deterministic Mode
```

Ein laufender deterministischer Scope darf nicht unkontrolliert die Generation wechseln.

---

## Deterministic Generation Pinning

Beispiel:

```text
Deterministic Scope
      ↓
Generation 41 pinned
```

Neue Generation:

```text
Generation 42
```

wird installiert, aber der bestehende Scope verwendet weiterhin Generation 41.

---

## New Deterministic Scopes

Neue Scopes können nach Commit Generation 42 verwenden.

---

## Deterministic Replay

Replay muss dieselbe relevante Generation beziehungsweise eine nachweislich kompatible deterministische Implementierung verwenden.

---

## Realtime

Realtime-Workloads benötigen besondere Regeln.

Hot Replacement darf Hard Realtime nicht unkontrolliert unterbrechen.

---

## Realtime Replacement

Mögliche Strategien:

```text
Wait for Maintenance Window
Parallel Replacement
Redundant Provider
Failover
No Live Replacement
```

---

## Safety-Critical Systems

Safety-kritische Komponenten dürfen nur ersetzt werden, wenn der Replacement-Prozess entsprechend zertifizierbare Garantien besitzt.

---

## Security

Hot Replacement ist eine sicherheitskritische Operation.

Ein Angreifer darf Replacement nicht als Weg verwenden, um vertrauenswürdigen Code durch nicht vertrauenswürdigen Code zu ersetzen.

---

## Nova.Trust

Jede Replacement Candidate Generation muss durch:

```text
Nova.Trust
```

bewertet werden.

Beispiel:

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Permission
   ↓
Capability
   ↓
Audit
```

---

## Signature Validation

Neue Komponenten müssen entsprechend ihrer Trust Policy signiert beziehungsweise verifiziert sein.

---

## Provenance

NovaOS soll nachvollziehen können:

```text
Where did this generation come from?
Who built it?
Which source produced it?
Which package delivered it?
Which previous generation did it replace?
```

---

## Trust Regression

Eine neue Generation darf nicht automatisch akzeptiert werden, wenn ihr Trust-Level niedriger ist als der erforderliche Trust-Level.

---

## Revocation

Wird eine Generation widerrufen, kann NovaOS einen Replacement-Prozess initiieren.

---

## Capability Permissions

Die neue Generation erhält nicht automatisch mehr Berechtigungen als die alte.

Neue Capability-Anforderungen müssen separat autorisiert werden.

---

## Privilege Expansion

Beispiel:

```text
Old:
    File.Read

New:
    File.Read
    Network.Connect
```

Dies ist keine reine Implementierungsänderung.

NovaOS muss die neue Berechtigungsanforderung erkennen.

---

## Least Privilege

Replacement darf Least Privilege nicht umgehen.

---

## Capability Attenuation

Übertragene Capabilities dürfen während State Transfer nicht unbeabsichtigt erweitert werden.

---

## Data Sovereignty

Hot Replacement muss:

```text
Data Sovereignty
```

respektieren.

Eine neue Generation darf Daten nicht plötzlich an einen anderen unzulässigen Standort verlagern.

---

## Location Transparency

Hot Replacement kann einen lokalen Provider durch einen Remote Provider ersetzen.

Dies ist nur zulässig, wenn:

- Contract,
- Trust,
- Sovereignty,
- Latency,
- Resource Policy

dies erlauben.

---

## Sichtbare Kosten

Location Transparency darf Replacement-Kosten nicht verstecken.

Beispiel:

```text
Old Provider:
    Local GPU

New Provider:
    Remote GPU

Additional:
    Network latency
    bandwidth
    monetary cost
    failure domain
```

Diese Faktoren müssen in die Entscheidung einfließen.

---

## Information Flow

Neue Generationen müssen dieselben oder strengere Information-Flow-Regeln respektieren.

---

## Confidential State

State Transfer sensibler Daten muss geschützt erfolgen.

Mögliche Anforderungen:

- encrypted transfer,
- protected memory,
- trusted execution domain,
- capability-controlled access.

---

## Secret State

Secrets dürfen nur übertragen werden, wenn die neue Generation dazu berechtigt ist.

---

## Key Material

Kryptografisches Key Material benötigt spezielle Transfer Policies.

In bestimmten Fällen darf es nicht exportiert werden.

Dann kann Replacement nur über:

```text
Rebind to existing secure key object
```

statt über State Copy erfolgen.

---

## Hardware-Bound Secrets

Hardwaregebundene Secrets können nicht beliebig migriert werden.

---

## Architecture Introspection

Hot Replacement muss systemweit introspektierbar sein.

Beispiel:

```text
Component:
    de.nova.image.filter.gaussian

Current:
    Version = 4.2.1
    Generation = 41

Replacement:
    Version = 4.3.0
    Generation = 42

State:
    GracePeriod

Traffic:
    Old = 20 %
    New = 80 %
```

---

## Replacement History

NovaOS soll eine Replacement History führen können.

Beispiel:

```text
Generation 39
    ↓
Generation 40
    ↓
Generation 41
    ↓
Generation 42
```

mit:

- Zeit,
- Ursache,
- Version,
- Trust,
- Ergebnis,
- Rollback Status.

---

## Decision Trace

Replacement-Entscheidungen sollen erklärbar sein.

Beispiel:

```text
Replacement deferred.

Reason:
    insufficient temporary memory

Required:
    600 MiB

Available:
    420 MiB
```

oder:

```text
Replacement rejected.

Reason:
    New provider violates sovereignty policy.
```

---

## Audit

Sicherheitsrelevante Replacements müssen auditierbar sein.

---

## Observability

Mindestens folgende Ereignisse sollen beobachtbar sein:

```text
ReplacementRequested
ReplacementValidated
ReplacementStaged
QuiescenceRequested
QuiescenceReached
StateExported
StateMigrated
StateImported
GenerationActivated
BindingChanged
VerificationPassed
VerificationFailed
RollbackStarted
RollbackCompleted
GenerationRetired
```

---

## Metrics

Mögliche Metriken:

- Replacement Duration,
- Quiescence Time,
- Migration Time,
- Downtime,
- State Size,
- Temporary Memory,
- Failed Replacements,
- Rollback Rate,
- Verification Failures,
- Generation Lifetime.

---

## Downtime

Ein Ziel von Hot Replacement ist:

```text
Zero or minimal externally visible downtime
```

Dies ist jedoch kein universelles Hard Requirement.

---

## Zero-Downtime Replacement

Zero-Downtime kann durch parallele Generationen erreicht werden.

```text
Old Active
    │
    ├── New Load
    ├── New Validate
    ├── New Warmup
    └── Atomic Rebind
            ↓
        New Active
```

---

## Warmup

Neue Generationen können vor Aktivierung:

- Code laden,
- Caches vorbereiten,
- Modelle laden,
- Ressourcen reservieren,
- Verbindungen herstellen.

---

## Cache Migration

Caches müssen nicht immer migriert werden.

Sie können klassifiziert werden als:

```text
Transferable
Reconstructable
Disposable
Persistent
```

Reconstructable Caches sollen häufig neu aufgebaut werden statt komplex migriert zu werden.

---

## Prediction Models

Adaptive Prediction Models können hot-replaced werden.

Dabei muss die Modellidentität versioniert werden.

---

## Model State

Online gelernter State kann getrennt vom Modellcode behandelt werden.

Beispiel:

```text
Model Architecture
Model Weights
User Adaptation State
Runtime Cache
```

Diese Bestandteile können unterschiedliche Lifecycle-Regeln besitzen.

---

## Prediction Error

Prediction-Error-Historie darf bei Model Replacement erhalten bleiben, sofern:

- semantisch kompatibel,
- datenschutzkonform,
- sinnvoll.

---

## AI Providers

AI Provider können ebenfalls ersetzt werden.

Execution Contracts bestimmen, ob laufende Inference-Aufträge:

- auf alter Generation fertiglaufen,
- migriert,
- neu gestartet

werden.

---

## Algorithm Implementations

Auch einzelne Algorithmusimplementierungen können ersetzt werden.

Beispiel:

```text
Sort Implementation v3
        ↓
Sort Implementation v4
```

---

## Automatic Algorithm Selection

Nach Replacement kann die automatische Algorithmusauswahl die neue Implementierung berücksichtigen.

Bestehende deterministische Scopes bleiben gegebenenfalls gepinnt.

---

## Policy Hot Replacement

Da NovaOS:

```text
Mechanism / Policy Separation
```

verwendet, können Policy Module unabhängig von Mechanismen ersetzt werden.

---

## Policy Safety

Eine neue Policy muss vor Aktivierung validiert werden.

Sie darf höhere Hard Constraints nicht umgehen.

---

## Policy Versioning

Policy Decisions sollen die verwendete Policy-Version nachvollziehbar machen können.

---

## Configuration Evolution

Konfigurationsschemas können sich ebenfalls weiterentwickeln.

Beispiel:

```text
ConfigSchema v2
      ↓
Migration
      ↓
ConfigSchema v3
```

---

## Declarative Configuration

Da Systemkonfiguration deklarativ ist, kann NovaOS nach Schema-Migration den Desired State erneut reconciliieren.

---

## Database Evolution

Datenbanken und persistente Stores benötigen eigene Migrationsmechanismen.

Diese ADR schreibt keine universelle Datenbankmigrationstechnik vor.

---

## Persistent Data

Persistente Datenmigrationen müssen besonders vorsichtig behandelt werden, weil Rollback schwieriger sein kann.

---

## Backup / Snapshot

Vor irreversiblen persistenten Migrationen soll NovaOS:

- Snapshot,
- Backup,
- Journal,
- Recovery Point

verwenden können.

---

## Filesystem Snapshots

Copy-on-Write-Dateisysteme können für Rollback genutzt werden.

---

## Btrfs / Native NovaFS

Die Architektur ist nicht an Btrfs gebunden.

Ein zukünftiges natives NovaOS-Dateisystem soll entsprechende Snapshot-/Transaction-Primitiven bereitstellen können.

---

## Distributed Replacement

In verteilten Systemen kann Replacement über mehrere Nodes erfolgen.

Beispiel:

```text
Node A → v5
Node B → v5
Node C → v5
```

---

## Rolling Upgrade

Rolling Upgrade soll unterstützt werden können.

Währenddessen können mehrere Generationen gleichzeitig aktiv sein.

---

## Protocol Compatibility

Bei Rolling Upgrades müssen unterschiedliche Versionen miteinander kommunizieren können.

---

## Version Negotiation

Protokolle sollen Version Negotiation unterstützen können.

---

## Mixed-Version Window

Der Zeitraum mit mehreren Versionen muss explizit modelliert werden.

---

## Cluster Commit

Für bestimmte Updates kann ein koordinierter Cluster Commit erforderlich sein.

---

## Distributed Transactions

Nicht jeder verteilte Replacement-Prozess muss eine globale ACID-Transaktion verwenden.

Mögliche Strategien:

- rolling update,
- staged rollout,
- saga,
- consensus-controlled switch,
- desired-state reconciliation.

---

## Failure Domains

Hot Replacement muss Failure Domains berücksichtigen.

Beispiel:

```text
Component
Service
Driver Domain
Execution Domain
Device
Node
Cluster
```

Ein Fehler soll möglichst auf den kleinsten sicheren Scope begrenzt bleiben.

---

## Nova.Resilience

Hot Replacement ist ein wichtiger Mechanismus für:

```text
Nova.Resilience
```

Self-Healing kann eine defekte Komponente durch eine bekannte funktionierende Generation ersetzen.

---

## Self-Healing Replacement

Beispiel:

```text
Detect Failure
      ↓
Classify
      ↓
Select Known-Good Generation
      ↓
Replace
      ↓
Verify
```

---

## Known-Good Generation

NovaOS kann eine:

```text
Last Known Good
```

Generation speichern.

---

## Automatic Rollback

Wenn Health Verification fehlschlägt, kann Nova.Resilience automatisch zurückrollen.

---

## Replacement Loop Prevention

Ein System darf nicht endlos zwischen zwei fehlerhaften Generationen wechseln.

Beispiel:

```text
v4 → v5 → v4 → v5 → ...
```

Daher benötigt NovaOS:

- Retry Limits,
- Backoff,
- failure history,
- quarantine.

---

## Quarantine

Fehlerhafte Generationen können quarantänisiert werden.

```text
Generation 42
    ↓
Verification Failure
    ↓
Quarantine
```

Sie werden nicht erneut automatisch aktiviert, bis eine Policy dies erlaubt.

---

## Update System

Das NovaOS Update System verwendet Hot Replacement, wenn möglich.

Beispiel:

```text
Update Package
     ↓
Determine Replacement Strategy
     │
     ├── Live
     ├── Rolling
     ├── Restart Component
     └── Reboot
```

---

## Reboot Minimization

Ziel ist:

```text
Reboot only when architecturally necessary.
```

Nicht:

```text
Never reboot.
```

---

## Boot Generation

Nach einem vollständigen Systemupdate kann NovaOS eine neue Boot Generation erzeugen.

Beispiel:

```text
System Generation 124
        ↓
System Generation 125
```

---

## Atomic Boot Activation

Updates, die einen Neustart erfordern, sollen möglichst atomar aktiviert werden.

---

## Boot Rollback

Wenn die neue Boot Generation fehlschlägt:

```text
Boot Generation 125
       ↓
Failure
       ↓
Rollback
       ↓
Boot Generation 124
```

---

## NovaDOS

NovaDOS bleibt als unabhängige Offline-Recovery-Umgebung verfügbar.

Wenn Hot Replacement und Nova.Resilience selbst nicht mehr funktionieren, kann NovaDOS:

- Komponenten prüfen,
- Generationen auswählen,
- Updates zurückrollen,
- Systemdateien reparieren.

NovaDOS darf nicht vom vollständigen Live-Evolution-Stack abhängig sein.

---

## Legacy Applications

Legacy-Anwendungen können Hot Replacement nur begrenzt unterstützen.

POSIX-/Linux-/Win32-Kompatibilität darf das native NovaOS-Lifecycle-Modell nicht bestimmen.

---

## Legacy Libraries

Klassische dynamische Bibliotheken können gegebenenfalls nur nach Prozessneustart ersetzt werden.

Dies ist akzeptabel.

---

## Native Nova Applications

Native Nova-Anwendungen sollen stärker auf:

- Capability Bindings,
- versionierte Interfaces,
- Structured Concurrency,
- Semantic State

ausgerichtet werden und dadurch bessere Hot-Replacement-Eigenschaften besitzen.

---

## NovaLang

NovaLang soll Hot Replacement sprachlich unterstützen können.

Die konkrete Syntax wird separat spezifiziert.

Mögliche Konzepte:

```text
replaceable
state
migration
version
```

---

## NovaLang State

NovaLang soll State deklarativ kennzeichnen können, sodass Runtime und Tooling erkennen können:

- persistent,
- transferable,
- reconstructable,
- ephemeral.

---

## Developer Tooling

Entwicklungswerkzeuge sollen Replacement lokal testen können.

Beispiel:

```text
Run Application
      ↓
Modify Capability
      ↓
Compile
      ↓
Hot Replace
      ↓
Continue Session
```

---

## Development Hot Reload

Development Hot Reload ist eine spezielle, weniger strenge Form von Hot Replacement.

Es darf nicht mit Production Hot Replacement gleichgesetzt werden.

---

## Production Replacement

Production Replacement benötigt:

- Trust Validation,
- Transactionality,
- Resource Planning,
- State Validation,
- Rollback,
- Audit.

---

## Testing

Jede hot-replace-fähige Komponente soll automatisierte Replacement-Tests besitzen können.

---

## Compatibility Testing

Zu testen sind mindestens:

```text
Old State → New Code
Old Client → New Provider
New Client → Old Provider
Mixed Versions
Rollback
Forward Recovery
```

---

## State Migration Testing

Migrationen müssen mit realistischen State-Samples getestet werden.

---

## Fuzz Testing

State Migration Parser und Compatibility Adapter sollen fuzzbar sein.

---

## Failure Injection

Replacement-Prozesse müssen Failure Injection unterstützen können.

Beispiele:

```text
Fail during State Export
Fail during Migration
Fail during Rebind
Fail during Verification
Fail during Commit
```

---

## Crash Consistency

Ein Systemcrash während Replacement darf keinen undefinierten permanenten Zustand erzeugen.

---

## Recovery Record

NovaOS soll genügend Informationen speichern, um nach einem Crash erkennen zu können:

```text
Which replacement?
Which phase?
Which generation authoritative?
Can rollback?
Can continue?
```

---

## Replacement Journal

Ein Replacement Journal kann verwendet werden.

Konzeptionell:

```text
ReplacementID
CurrentGeneration
TargetGeneration
Phase
CommitState
RecoveryState
```

---

## Idempotence

Recovery-Schritte sollen soweit möglich idempotent sein.

---

## Garbage Collection

Alte Generationen dürfen nicht unbegrenzt gespeichert werden.

Eine Retention Policy bestimmt, welche Generationen erhalten bleiben.

---

## Generation Retention

Beispiel:

```text
Current
Previous
LastKnownGood
Recovery
```

Zusätzliche historische Generationen können abhängig von Storage Policy erhalten bleiben.

---

## Storage Economy

Generation Retention muss Storage Budgets berücksichtigen.

---

## Dependency Graph

Replacement muss Abhängigkeiten berücksichtigen.

Beispiel:

```text
Capability A
    ↓
Capability B
    ↓
Capability C
```

Wenn B ersetzt wird, müssen A und C hinsichtlich Kompatibilität geprüft werden.

---

## Dependency Version Constraints

Komponenten können Anforderungen deklarieren.

Beispiel:

```text
requires:
    Capability B >= 4
    Capability B < 6
```

---

## Replacement Set

Manchmal müssen mehrere Komponenten gemeinsam ersetzt werden.

```text
A v2
B v4
C v7
```

werden atomar zu:

```text
A v3
B v5
C v8
```

---

## Coordinated Replacement

Ein Replacement Set kann als gemeinsame Transaktion behandelt werden.

---

## Dependency Cycles

Zyklische Abhängigkeiten müssen erkannt werden.

NovaOS darf keinen undefinierten Replacement-Plan erzeugen.

---

## Graph Planning

Das deklarative Systemmodell kann einen Replacement Plan aus dem Dependency Graph erzeugen.

---

## Replacement Planner

Konzeptionell:

```text
Desired Versions
      ↓
Dependency Graph
      ↓
Compatibility
      ↓
Execution Contracts
      ↓
Resource Economy
      ↓
Trust / Security
      ↓
Replacement Planner
      ↓
Replacement Plan
```

---

## Replacement Plan

Beispiel:

```text
ReplacementPlan {
    targets
    order
    parallelGroups
    resourceReservations
    stateMigrations
    quiescencePoints
    bindings
    verification
    rollback
}
```

---

## Mechanism / Policy Separation

Hot Replacement folgt:

```text
ADR-ARCH-0002
Mechanism / Policy Separation
```

Mechanismen stellen bereit:

- Load,
- Quiesce,
- State Export,
- State Import,
- Rebind,
- Rollback.

Policies entscheiden:

- wann ersetzt wird,
- welche Version verwendet wird,
- wie lange Grace Period dauert,
- ob Canary verwendet wird,
- ob automatisch zurückgerollt wird.

---

## Replacement Policy

Beispiel:

```text
ReplacementPolicy {
    securityUpdates = Immediate
    featureUpdates  = MaintenanceWindow
    canary          = Enabled
    rollback        = Automatic
}
```

---

## User Control

Benutzer beziehungsweise Administratoren können abhängig von System Policy Einfluss auf Update- und Replacement-Zeitpunkte erhalten.

Hard Security Requirements können Benutzerpräferenzen überstimmen.

---

## Maintenance Windows

Bestimmte Replacements können auf Wartungsfenster warten.

---

## Deferred Replacement

Ein Replacement kann als:

```text
Pending
```

markiert werden.

---

## Security Deadline

Kritische Security Updates können eine maximale Aufschubzeit besitzen.

---

## Live Evolution

Hot Replacement ersetzt eine Implementierung.

Live Evolution geht darüber hinaus.

Sie ermöglicht kontrollierte Veränderungen des laufenden Systemmodells.

Beispiel:

```text
Capability Graph v1
        ↓
Capability Graph v2
```

---

## Evolution of System Structure

Live Evolution kann verändern:

- Capability Graphs,
- Provider Graphs,
- Pipeline Graphs,
- Policies,
- Data Schemas,
- Service Topology,
- Resource Topology,
- UI Composition.

---

## Evolution Transaction

Größere Evolutionen sollen als koordinierte Systemtransaktionen geplant werden.

---

## Schema Evolution

Systemweite Semantic Types und Schemas müssen versioniert evolvieren können.

---

## Capability Evolution

Capabilities können neue Versionen erhalten.

Ihre logische Identität bleibt erhalten, solange ihre semantische Bedeutung kompatibel bleibt.

---

## Semantic Identity

Eine inkompatibel veränderte Capability darf nicht einfach dieselbe Identität behalten, wenn dadurch bestehende Contracts falsch interpretiert würden.

---

## Evolution Boundaries

Nicht jede Änderung ist ein Upgrade.

Manche Änderungen erzeugen eine neue semantische Capability.

---

## Evolution Compatibility

NovaOS muss zwischen:

```text
Implementation Change
Compatible Evolution
Migratable Evolution
Breaking Evolution
```

unterscheiden können.

---

## Breaking Evolution

Breaking Evolution benötigt:

- neue Version,
- Adapter,
- Migration,
- neue Capability Identity

oder eine Kombination daraus.

---

## Long-Term Evolution

Die Architektur soll ermöglichen, dass NovaOS über viele Jahre intern weiterentwickelt werden kann, ohne alte Designentscheidungen dauerhaft in jedem neuen Subsystem festzuschreiben.

---

## Compatibility Layer Isolation

Legacy Compatibility wird deshalb in klaren Schichten isoliert.

Alte ABI-/API-Anforderungen dürfen die native Architektur nicht unbegrenzt einfrieren.

---

## Evolution Debt

Kompatibilitätsadapter erzeugen technische Schuld.

NovaOS soll deren Nutzung introspektierbar machen.

---

## Adapter Retirement

Veraltete Adapter können nach definierten Deprecation-Zyklen entfernt werden.

---

## Deprecation

Capabilities und Interfaces können als:

```text
Deprecated
```

markiert werden.

---

## Deprecation Metadata

Beispiel:

```text
Deprecated {
    since = 5.0
    replacement = de.nova.example.new
    removalAfter = 7.0
}
```

Die konkrete Versionspolitik wird separat definiert.

---

## No Permanent Compatibility Promise

NovaOS garantiert nicht, dass jede interne Implementierung für immer unverändert bleibt.

Stattdessen garantiert die Architektur kontrollierte Evolution über explizite Contracts.

---

## Architecture Introspection für Live Evolution

NovaOS soll darstellen können:

```text
Current Architecture
Pending Changes
Active Generations
Deprecated Interfaces
Compatibility Adapters
Migration State
```

---

## Architecture Diff

Ein geplantes Replacement kann als Architecture Diff dargestellt werden.

Beispiel:

```text
- Provider A v4
+ Provider A v5

- Interface Adapter v2

State:
    Schema v7 → v8
```

---

## Explainability

NovaOS soll erklären können:

```text
Why is this component being replaced?
Why now?
Why this provider?
Why is reboot required?
Why is rollback unavailable?
```

---

## Safety Invariants

Während jeder Live Evolution müssen definierte Systeminvarianten erhalten bleiben.

Beispiele:

```text
No unauthorized capability gain
No invalid state exposure
No broken transaction
No sovereignty violation
No unprotected secret transfer
```

---

## Verification Gates

Ein Replacement kann mehrere Verification Gates besitzen.

```text
Signature Gate
Compatibility Gate
Resource Gate
State Gate
Security Gate
Health Gate
```

Nur wenn alle notwendigen Gates erfolgreich sind, erfolgt Commit.

---

## Fail Closed

Bei sicherheitsrelevanten Unsicherheiten muss NovaOS konservativ reagieren.

---

## No Blind Replacement

Eine neue Version darf nicht allein deshalb aktiviert werden, weil ihre Versionsnummer höher ist.

---

## Version Number Is Not Trust

Es gilt:

```text
Newer != Safer
Newer != Compatible
Newer != Better
```

Die Entscheidung basiert auf Contracts und verifizierbaren Eigenschaften.

---

## Normative Festlegungen

1. NovaOS MUSS Hot Replacement als grundlegendes Architekturprinzip unterstützen.

2. Hot Replacement MUSS auf geeignete Komponenten beschränkt werden können.

3. Nicht jede Komponente MUSS live ersetzbar sein.

4. Fundamentale Komponenten DÜRFEN einen Reboot verlangen.

5. NovaOS MUSS zwischen logischer Capability Identity und konkreter Implementation unterscheiden.

6. Capability Bindings MÜSSEN kontrolliert änderbar sein.

7. Hot Replacement DARF keine unnötige direkte Kopplung an konkrete Speicheradressen voraussetzen.

8. Bindings SOLLEN stabile logische Handles verwenden.

9. Binding Indirection DARF Fast Paths nicht unnötig belasten.

10. Bindings DÜRFEN gecacht werden.

11. Cached Bindings MÜSSEN kontrolliert invalidierbar beziehungsweise aktualisierbar sein.

12. Replacement MUSS unterschiedliche Scopes unterstützen können.

13. Stateless Replacement MUSS unterstützt werden können.

14. Restartable Replacement SOLL unterstützt werden.

15. Stateful Replacement SOLL unterstützt werden.

16. Parallel Replacement SOLL unterstützt werden.

17. Rolling Replacement SOLL für Multi-Instance-Systeme unterstützt werden.

18. Transactional Replacement MUSS unterstützt werden können.

19. Reboot-Requiring Replacement MUSS als legitime Strategie unterstützt werden.

20. Komponenten MÜSSEN ihre Replacement-Fähigkeiten deklarieren können.

21. Hot-replace-fähige Komponenten MÜSSEN einen definierten Lifecycle besitzen.

22. Generationen MÜSSEN eindeutig unterscheidbar sein.

23. Laufende Operationen DÜRFEN an ältere Generationen gebunden bleiben.

24. Neue Operationen SOLLEN nach Commit die neue Generation verwenden.

25. Alte Generationen DÜRFEN erst entfernt werden, wenn ihre sichere Retirement-Bedingung erfüllt ist.

26. NovaOS MUSS Quiescence unterstützen können.

27. Komponenten SOLLEN Safe Points deklarieren können.

28. Quiescence MUSS neue Arbeit kontrolliert blockieren können.

29. Bestehende Arbeit MUSS drainbar, abbrechbar, migrierbar oder auf alter Generation fortsetzbar sein.

30. Structured Concurrency MUSS mit Replacement integrierbar sein.

31. Cancellation MUSS Replacement Contracts respektieren.

32. Stateful Replacement MUSS State Export unterstützen können.

33. Stateful Replacement MUSS State Import unterstützen können.

34. State Schemas MÜSSEN versionierbar sein.

35. State Migration MUSS explizit modellierbar sein.

36. State Migration MUSS validierbar sein.

37. Migration Failure DARF die alte authoritative Generation NICHT vorschnell zerstören.

38. Canonical State Representations SOLLEN unterstützt werden, wenn sie Providerwechsel erleichtern.

39. Semantic Types MÜSSEN bei State Migration erhalten bleiben.

40. State Ownership MUSS vor Migration eindeutig bestimmbar sein.

41. Shared State MUSS kontrolliert migriert beziehungsweise geteilt werden.

42. Unkontrollierte konkurrierende Mutation durch alte und neue Generation MUSS verhindert werden.

43. Zero-Copy MUSS mit Hot Replacement kompatibel sein.

44. Shared Buffer Lifetimes MÜSSEN Generationenwechsel überleben können, wenn der Contract dies verlangt.

45. Buffer Ownership Transfer MUSS unterstützt werden können.

46. DMA MUSS vor relevantem Driver-/Device-Replacement kontrolliert werden.

47. Capability Rebinding MUSS atomar erfolgen können.

48. Es DARF keinen undefinierten sichtbaren Binding-Zwischenzustand geben.

49. Laufende Calls MÜSSEN eine definierte Replacement-Semantik besitzen.

50. Neue Calls MÜSSEN nach Commit konsistent geroutet werden.

51. Interfaces MÜSSEN versionierbar sein.

52. Interface Compatibility MUSS überprüfbar sein.

53. Compatibility Adapter DÜRFEN verwendet werden.

54. Adapter MÜSSEN dieselben Hard Constraints wie native Provider respektieren.

55. NovaABI SOLL Hot Replacement durch stabile Handles und versionierte Interfaces unterstützen.

56. Hot Replacement DARF NICHT verlangen, dass jedes interne ABI dauerhaft eingefroren wird.

57. Datenstrukturen DÜRFEN zwischen Generationen evolvieren.

58. Kernel-Komponenten MÜSSEN nach Replacement-Fähigkeit klassifizierbar sein.

59. Der gesamte Kernel MUSS NICHT live austauschbar sein.

60. Driver Hot Replacement SOLL unterstützt werden.

61. Isolierte Driver Domains SOLLEN bevorzugt hot-replace-fähig sein.

62. Device State MUSS bei Driver Replacement berücksichtigt werden.

63. Device Reset MUSS als mögliche Replacement-Strategie unterstützt werden.

64. Storage Driver Replacement MUSS Datenintegrität garantieren.

65. Uncommitted Storage Writes DÜRFEN durch Replacement NICHT verloren gehen, wenn der zugrunde liegende Contract Persistenz garantiert.

66. Aktive DMA-Zugriffe DÜRFEN NICHT auf ungültige Speicherbereiche zeigen.

67. Network Driver Replacement SOLL bestehende Verbindungen erhalten, wenn technisch möglich.

68. GPU Provider Replacement MUSS aktive GPU Workloads berücksichtigen.

69. NPU Provider Replacement MUSS aktive NPU Workloads berücksichtigen.

70. Systemdienste SOLLEN hot-replace-fähig entworfen werden, wenn dies sinnvoll ist.

71. Service Clients SOLLEN stabile logische Endpoints verwenden.

72. Connection Handover SOLL unterstützt werden können.

73. Sessions SOLLEN bei Service Replacement möglichst erhalten bleiben.

74. Native Nova-Anwendungen SOLLEN Capability-Level Replacement unterstützen können.

75. Adaptive Anwendungen SOLLEN einzelne Fähigkeiten unabhängig aktualisieren können.

76. UI-Komposition MUSS auf Capability Replacement reagieren können.

77. UI State SOLL bei UI Replacement erhalten bleiben.

78. `.nui`-Strukturen SOLLEN Live Evolution unterstützen können.

79. Hot Replacement MUSS mit dem deklarativen Systemmodell integriert sein.

80. Desired und Actual Generation MÜSSEN unterscheidbar sein.

81. Reconciliation MUSS Replacement planen können.

82. Hot Replacement MUSS mit transaktionalen Systemoperationen integriert sein.

83. Replacement SOLL als Systemtransaktion ausgeführt werden, wenn mehrere atomare Zustandsänderungen beteiligt sind.

84. Replacement MUSS einen definierten Commit Point besitzen.

85. Vor Commit SOLL die alte Generation authoritative bleiben.

86. Nach Commit SOLL die neue Generation authoritative sein.

87. Points of No Return MÜSSEN explizit modellierbar sein.

88. Irreversible Migrationen MÜSSEN vorab validiert werden.

89. Rollback MUSS unterstützt werden, wenn technisch möglich.

90. Reverse Migration SOLL unterstützt werden, wenn State Evolution dies erlaubt.

91. Forward Recovery MUSS möglich sein, wenn Rollback nicht möglich ist.

92. Canary Activation SOLL unterstützt werden.

93. Shadow Execution SOLL unterstützt werden können.

94. Differential Verification SOLL unterstützt werden.

95. Semantic Equality MUSS für Differential Verification verwendbar sein.

96. Neue Generationen MÜSSEN Health Verification durchlaufen können.

97. Grace Periods SOLLEN unterstützt werden.

98. Alte Generationen SOLLEN während einer Grace Period rollbackfähig bleiben.

99. Hot Replacement MUSS Resource Economy integrieren.

100. Temporärer Ressourcenmehrbedarf MUSS geplant werden.

101. Replacement MUSS Resource Reservations verwenden können.

102. Admission Control MUSS sichere Replacement-Ausführung verhindern können, wenn Ressourcen fehlen.

103. Resource Pressure MUSS Replacement Policies beeinflussen können.

104. Security-kritische Replacements DÜRFEN höhere Priorität erhalten.

105. Aktive Execution Contracts MÜSSEN beim Replacement berücksichtigt werden.

106. Hard Execution Contracts DÜRFEN NICHT stillschweigend verletzt werden.

107. Neue Generationen MÜSSEN vor Rebinding gegen relevante Contracts validiert werden.

108. Contract Renegotiation MUSS explizit erfolgen.

109. Deterministic Mode MUSS Hot Replacement kontrollieren können.

110. Deterministic Scopes MÜSSEN Generation Pinning unterstützen können.

111. Ein Required-Deterministic-Scope DARF NICHT unkontrolliert die Generation wechseln.

112. Neue deterministische Scopes DÜRFEN nach Commit die neue Generation verwenden.

113. Replay MUSS relevante Generationen identifizieren können.

114. Realtime Contracts MÜSSEN Replacement beeinflussen.

115. Hard Realtime DARF NICHT unkontrolliert unterbrochen werden.

116. Realtime Replacement DARF auf Maintenance Windows warten.

117. Redundante Provider DÜRFEN für Realtime Replacement verwendet werden.

118. Safety-kritische Systeme DÜRFEN strengere Replacement Policies verlangen.

119. Hot Replacement MUSS als sicherheitskritische Operation behandelt werden.

120. Jede neue Generation MUSS Nova.Trust unterliegen.

121. Signaturen MÜSSEN entsprechend der Trust Policy validiert werden.

122. Provenance SOLL für Generationen nachvollziehbar sein.

123. Ein höherer Versionswert DARF NICHT automatisch höheres Vertrauen bedeuten.

124. Trust Regression MUSS erkannt werden.

125. Revocation MUSS Replacement auslösen können.

126. Eine neue Generation DARF NICHT automatisch zusätzliche Capabilities erhalten.

127. Privilege Expansion MUSS explizit erkannt werden.

128. Least Privilege MUSS erhalten bleiben.

129. Capability Attenuation MUSS bei State Transfer erhalten bleiben.

130. Data Sovereignty MUSS während Replacement erhalten bleiben.

131. Remote Provider Replacement MUSS Location Constraints berücksichtigen.

132. Location Transparency DARF Remote-Kosten und Fehler NICHT verstecken.

133. Information Flow MUSS während Replacement erhalten bleiben.

134. Confidential State MUSS geschützt übertragen werden.

135. Secrets DÜRFEN nur an autorisierte Generationen übertragen werden.

136. Hardwaregebundene Secrets DÜRFEN NICHT unkontrolliert migriert werden.

137. Secure Object Rebinding MUSS anstelle von Secret Copy möglich sein.

138. Architecture Introspection MUSS aktive Generationen darstellen können.

139. Architecture Introspection MUSS laufende Replacement-Prozesse darstellen können.

140. Replacement History SOLL verfügbar sein.

141. Decision Tracing SOLL Replacement-Entscheidungen erklären können.

142. Sicherheitsrelevante Replacements MÜSSEN auditierbar sein.

143. Replacement Lifecycle Events SOLLEN beobachtbar sein.

144. Replacement Metrics SOLLEN verfügbar sein.

145. Zero-Downtime Replacement SOLL unterstützt werden, wenn technisch möglich.

146. Zero Downtime DARF NICHT als universelle Garantie angenommen werden.

147. Warmup neuer Generationen SOLL unterstützt werden.

148. Caches MÜSSEN nach Transferability klassifizierbar sein.

149. Reconstructable Caches SOLLEN nicht unnötig migriert werden.

150. Prediction Models MÜSSEN versionierbar sein.

151. Adaptive Model State MUSS vom ausführbaren Modellcode getrennt behandelbar sein.

152. Prediction Error History DARF bei kompatiblen Modellwechseln erhalten bleiben.

153. AI Provider MÜSSEN Hot Replacement unterstützen können.

154. Algorithmusimplementierungen MÜSSEN generationenbasiert ersetzt werden können.

155. Automatische Algorithmusauswahl DARF neue Generationen nach Commit berücksichtigen.

156. Deterministic Scopes MÜSSEN weiterhin gepinnte Algorithmusgenerationen verwenden können.

157. Policy Module SOLLEN hot-replace-fähig sein.

158. Mechanism / Policy Separation MUSS beim Replacement erhalten bleiben.

159. Neue Policies DÜRFEN höhere Hard Constraints NICHT umgehen.

160. Policy-Versionen SOLLEN in Decision Traces sichtbar sein.

161. Konfigurationsschemas MÜSSEN evolvierbar sein.

162. Konfigurationsmigrationen SOLLEN transaktional erfolgen.

163. Persistente Datenmigrationen MÜSSEN besonders abgesichert werden.

164. Irreversible persistente Migrationen SOLLEN Snapshot- oder Recovery-Mechanismen verwenden.

165. Die Architektur DARF NICHT an ein bestimmtes Dateisystem gebunden sein.

166. Distributed Replacement MUSS möglich sein.

167. Rolling Upgrades SOLLEN unterstützt werden.

168. Mixed-Version Windows MÜSSEN explizit modellierbar sein.

169. Protokolle SOLLEN Version Negotiation unterstützen.

170. Distributed Replacement MUSS NICHT zwingend globale ACID-Transaktionen verwenden.

171. Failure Domains MÜSSEN bei Replacement berücksichtigt werden.

172. Fehler SOLLEN auf den kleinsten sicheren Scope begrenzt werden.

173. Nova.Resilience MUSS Hot Replacement als Recovery-Mechanismus verwenden können.

174. Last-Known-Good-Generationen SOLLEN unterstützt werden.

175. Automatic Rollback SOLL unterstützt werden.

176. Replacement Loops MÜSSEN verhindert werden.

177. Retry Limits SOLLEN existieren.

178. Backoff SOLL unterstützt werden.

179. Fehlerhafte Generationen SOLLEN quarantänisiert werden können.

180. Das NovaOS Update System MUSS die passende Replacement-Strategie bestimmen können.

181. Reboot SOLL nur verwendet werden, wenn architektonisch notwendig oder sicherer.

182. Reboot-Replacements SOLLEN atomare Boot-Aktivierung unterstützen.

183. Boot Rollback SOLL unterstützt werden.

184. NovaDOS MUSS als unabhängiger Offline-Recovery-Pfad erhalten bleiben.

185. NovaDOS DARF NICHT vom vollständigen Hot-Replacement-Stack abhängig sein.

186. Legacy-Anwendungen DÜRFEN eingeschränkte Replacement-Fähigkeiten besitzen.

187. Legacy Compatibility DARF das native Lifecycle-Modell NICHT definieren.

188. Native Nova-Anwendungen SOLLEN auf versionierte Capability Bindings ausgelegt sein.

189. NovaLang SOLL Hot Replacement sprachlich unterstützen.

190. NovaLang SOLL State Transfer und Migration ausdrücken können.

191. Developer Tooling SOLL Hot Reload unterstützen.

192. Development Hot Reload DARF NICHT mit Production Hot Replacement gleichgesetzt werden.

193. Production Replacement MUSS Trust, Transactionality, Resource Planning und Rollback berücksichtigen.

194. Hot-replace-fähige Komponenten SOLLEN automatisierte Replacement-Tests besitzen.

195. State Migration MUSS testbar sein.

196. Compatibility Adapter MÜSSEN testbar sein.

197. Migration Parser SOLLEN fuzzbar sein.

198. Failure Injection SOLL für Replacement-Prozesse unterstützt werden.

199. Crash während Replacement DARF keinen undefinierten persistenten Systemzustand erzeugen.

200. Recovery Records MÜSSEN den autoritativen Zustand nach Crash bestimmbar machen.

201. Replacement Journaling SOLL unterstützt werden.

202. Recovery-Schritte SOLLEN idempotent sein.

203. Alte Generationen DÜRFEN NICHT unbegrenzt ohne Policy gespeichert werden.

204. Generation Retention MUSS Storage Budgets berücksichtigen.

205. Dependency Graphs MÜSSEN bei Replacement berücksichtigt werden.

206. Dependency Version Constraints MÜSSEN geprüft werden.

207. Mehrere Komponenten MÜSSEN als Replacement Set behandelbar sein.

208. Coordinated Replacement MUSS möglich sein.

209. Dependency Cycles MÜSSEN erkannt werden.

210. Replacement Planning SOLL graphbasiert erfolgen können.

211. Replacement Plans MÜSSEN explizit darstellbar sein.

212. Replacement Plans SOLLEN Reihenfolge und Parallelgruppen enthalten können.

213. Replacement Plans SOLLEN Resource Reservations enthalten können.

214. Replacement Plans SOLLEN State Migrations enthalten können.

215. Replacement Plans SOLLEN Rollback Paths enthalten können.

216. Mechanismen und Policies MÜSSEN getrennt bleiben.

217. Replacement Policies MÜSSEN konfigurierbar sein.

218. Benutzerpräferenzen DÜRFEN Replacement-Zeitpunkte beeinflussen.

219. Benutzerpräferenzen DÜRFEN Hard Security Requirements NICHT überschreiben.

220. Maintenance Windows MÜSSEN unterstützt werden können.

221. Deferred Replacement MUSS unterstützt werden.

222. Kritische Security Updates DÜRFEN maximale Aufschubzeiten besitzen.

223. Live Evolution MUSS über reines Binärreplacement hinausgehen.

224. Capability Graphs MÜSSEN evolvierbar sein.

225. Provider Graphs MÜSSEN evolvierbar sein.

226. Pipeline Graphs MÜSSEN evolvierbar sein.

227. Data Schemas MÜSSEN evolvierbar sein.

228. Service Topologien MÜSSEN evolvierbar sein.

229. System Policies MÜSSEN evolvierbar sein.

230. Größere Evolutionen SOLLEN transaktional geplant werden.

231. Semantic Types MÜSSEN versionierbar evolvieren können.

232. Capability Identity DARF nur bei erhaltener semantischer Bedeutung beibehalten werden.

233. Breaking Evolution MUSS explizit behandelt werden.

234. Breaking Evolution DARF NICHT als transparente kompatible Änderung ausgegeben werden.

235. Compatibility Adapter SOLLEN langfristig retirebar sein.

236. Deprecation MUSS ausdrückbar sein.

237. Deprecation SOLL Replacement-Informationen enthalten können.

238. Interne Implementierungen MÜSSEN NICHT dauerhaft eingefroren werden.

239. Architecture Introspection SOLL Compatibility Debt sichtbar machen.

240. Architecture Diff SOLL geplante Evolution darstellen können.

241. NovaOS SOLL erklären können, warum ein Replacement durchgeführt wird.

242. NovaOS SOLL erklären können, warum ein Replacement verzögert wird.

243. NovaOS SOLL erklären können, warum ein Reboot erforderlich ist.

244. NovaOS SOLL erklären können, warum Rollback nicht verfügbar ist.

245. Systeminvarianten MÜSSEN während Live Evolution erhalten bleiben.

246. Unautorisierte Capability Expansion MUSS verhindert werden.

247. Inkonsistente State Exposure MUSS verhindert werden.

248. Offene Systemtransaktionen MÜSSEN berücksichtigt werden.

249. Sovereignty Violations MÜSSEN verhindert werden.

250. Secret Leakage MUSS verhindert werden.

251. Replacement MUSS Verification Gates unterstützen können.

252. Trust Validation MUSS vor Activation erfolgen.

253. Compatibility Validation MUSS vor Activation erfolgen.

254. Resource Validation MUSS vor sicherheitskritischer Activation erfolgen.

255. State Validation MUSS vor Commit erfolgen.

256. Health Verification MUSS nach Activation möglich sein.

257. Sicherheitskritische Unsicherheit MUSS konservativ behandelt werden.

258. NovaOS DARF keine neue Generation blind aufgrund ihrer Versionsnummer aktivieren.

259. Neue Generationen MÜSSEN anhand ihrer tatsächlichen Contracts und Eigenschaften bewertet werden.

260. KI-basierte Entscheidungen DÜRFEN Replacement optimieren, aber NICHT allein Hard Safety oder Security Guarantees begründen.

261. Adaptive Replacement Policies MÜSSEN Prediction Error verwenden können.

262. Adaptive Replacement Policies DÜRFEN Hard Constraints NICHT verletzen.

263. Replacement Failure MUSS als lernbares Signal verwendet werden können.

264. Prediction darf entscheiden helfen, wann ein Replacement günstig ist.

265. Prediction DARF einen zwingenden Security Replacement NICHT unzulässig verzögern.

266. Hot Replacement MUSS sprachunabhängig als Systemmechanismus verfügbar sein.

267. Hot Replacement MUSS providerunabhängig abstrahierbar sein.

268. Provider-spezifische Replacement-Mechanismen DÜRFEN darunter implementiert werden.

269. Komponenten MÜSSEN ihre Replacement Constraints deklarieren können.

270. Komponenten SOLLEN ihre State Migration Capabilities deklarieren können.

271. Komponenten SOLLEN ihre Rollback Capabilities deklarieren können.

272. Komponenten SOLLEN ihre Quiescence Capabilities deklarieren können.

273. Komponenten SOLLEN ihre Parallel-Generation-Fähigkeit deklarieren können.

274. Komponenten SOLLEN ihre maximale Mixed-Version-Dauer deklarieren können.

275. Replacement Policies MÜSSEN diese Deklarationen berücksichtigen.

276. Unbekannte Replacement-Eigenschaften DÜRFEN NICHT automatisch als sicher angenommen werden.

277. Hot Replacement MUSS fail-safe ausgelegt sein.

278. Ein fehlerhafter Replacement Manager DARF nicht automatisch alle laufenden Generationen zerstören.

279. Alte authoritative Generationen SOLLEN bis zum sicheren Commit erhalten bleiben.

280. Hot Replacement MUSS mit Architecture Introspection, Nova.Trust, Nova.Resilience, Resource Economy und Execution Contracts als gemeinsames Systemmodell integriert werden.

---

## Konsequenzen

### Positive Konsequenzen

- deutlich weniger notwendige Systemneustarts,
- Systemdienste können ohne vollständigen Neustart aktualisiert werden,
- Treiber können häufiger isoliert ersetzt werden,
- Sicherheitsupdates können schneller aktiviert werden,
- laufende Anwendungen können Zustand behalten,
- einzelne Capabilities können unabhängig evolvieren,
- adaptive Anwendungen können modular aktualisiert werden,
- NovaOS kann sich langfristig weiterentwickeln,
- alte Implementierungen müssen die native Architektur nicht dauerhaft einfrieren,
- Self-Healing erhält einen starken Recovery-Mechanismus,
- Last-Known-Good-Rollback wird systematisch möglich,
- Canary und Shadow Execution verbessern Update-Sicherheit,
- State Migration wird zu einem expliziten Architekturkonzept,
- Versionierung und Compatibility werden systemweit vereinheitlicht,
- Reboots werden auf tatsächlich notwendige Fälle reduziert,
- Live Evolution wird mit Security und Trust verbunden,
- deterministische Workloads können Generationen pinnen,
- Realtime-Workloads können Replacement kontrollieren,
- Resource Economy kann temporäre Updatekosten planen,
- Architecture Introspection kann aktive und historische Generationen darstellen,
- NovaOS kann schrittweise über viele Jahre evolvieren.

### Negative Konsequenzen

- Lifecycle Management wird komplexer,
- Komponenten benötigen explizite Versionierung,
- State Migration muss entwickelt und getestet werden,
- parallele Generationen benötigen temporär zusätzliche Ressourcen,
- Compatibility Adapter erhöhen Komplexität,
- Quiescence kann schwierig sein,
- Treiber- und Hardware-State ist teilweise nicht migrierbar,
- Rollback ist nach irreversiblen State-Änderungen nicht immer möglich,
- zusätzliche Metadaten und Journale werden benötigt,
- Generation Retention benötigt Speicherplatz,
- Security-Prüfungen erhöhen Replacement-Aufwand,
- verteilte Rolling Upgrades erhöhen Protokollkomplexität,
- Live Kernel Evolution ist nur begrenzt realistisch,
- falsche Migrationen können schwerwiegende Fehler verursachen,
- Testaufwand steigt erheblich.

---

## Verworfene Alternativen

### Jeder Update benötigt einen Reboot

Nicht übernommen.

Dies würde die langfristige Evolutionsfähigkeit und Verfügbarkeit von NovaOS unnötig einschränken.

### Niemals rebooten

Nicht übernommen.

Bestimmte fundamentale Änderungen sind live nicht sicher durchführbar.

NovaOS bevorzugt:

```text
Live when safe.
Reboot when necessary.
```

### Binärdatei einfach überschreiben

Verworfen.

Dies berücksichtigt nicht:

- aktive Calls,
- State,
- Capabilities,
- Trust,
- Ressourcen,
- Rollback.

### Direkte Pointer auf Implementierungen

Nicht als grundlegendes Binding-Modell übernommen.

Sie erschweren kontrolliertes Rebinding und Lifetime Management.

### Alle Komponenten über eine globale Proxy-Schicht

Nicht übernommen.

Dies würde unnötige Fast-Path-Kosten verursachen.

### State immer vollständig kopieren

Nicht übernommen.

Zero-Copy, Shared Objects und Capability Rebinding sollen verwendet werden können.

### State niemals migrieren

Nicht übernommen.

Dies würde Stateful Replacement stark einschränken.

### Immer State migrieren

Nicht übernommen.

Reconstructable und Disposable State soll häufig neu erzeugt werden.

### Alte Generation sofort löschen

Verworfen.

Dies verhindert Grace Period und zuverlässigen Rollback.

### Unbegrenzte Speicherung aller Generationen

Nicht übernommen.

Storage Budgets und Retention Policies müssen berücksichtigt werden.

### Neue Version automatisch vertrauen

Verworfen.

Version ist kein Trust-Beweis.

### Security zugunsten von Verfügbarkeit umgehen

Verworfen.

Kompromittierte oder nicht vertrauenswürdige Generationen dürfen nicht allein zur Vermeidung von Downtime verwendet werden.

### Vollständige ABI-Unveränderlichkeit

Nicht übernommen.

Versionierung und Adapter erlauben Evolution ohne permanente interne Erstarrung.

### Hot Replacement nur für Anwendungen

Nicht übernommen.

Das Modell soll auch:

- Services,
- Driver,
- Provider,
- Policies,
- Capability Implementations

umfassen.

### Hot Replacement nur als Entwickler-Hot-Reload

Nicht übernommen.

Production Replacement benötigt deutlich stärkere Garantien.

### Globaler zentraler Replacement-Monolith

Nicht übernommen.

NovaOS verwendet subsystemfähige Mechanismen mit einem gemeinsamen semantischen Lifecycle- und Contract-Modell.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`

---

## Zugehörige NPSPECs

- `NPSPEC-ARCH-HOTREPLACE-0001`
- `NPSPEC-HOTREPLACE-MODEL-0001`
- `NPSPEC-HOTREPLACE-LIFECYCLE-0001`
- `NPSPEC-HOTREPLACE-GENERATION-0001`
- `NPSPEC-HOTREPLACE-BINDING-0001`
- `NPSPEC-HOTREPLACE-QUIESCENCE-0001`
- `NPSPEC-HOTREPLACE-SAFEPOINT-0001`
- `NPSPEC-HOTREPLACE-STATE-0001`
- `NPSPEC-HOTREPLACE-STATE-SCHEMA-0001`
- `NPSPEC-HOTREPLACE-STATE-MIGRATION-0001`
- `NPSPEC-HOTREPLACE-STATE-TRANSFER-0001`
- `NPSPEC-HOTREPLACE-OWNERSHIP-0001`
- `NPSPEC-HOTREPLACE-BUFFER-0001`
- `NPSPEC-HOTREPLACE-COMPATIBILITY-0001`
- `NPSPEC-HOTREPLACE-INTERFACE-0001`
- `NPSPEC-HOTREPLACE-ADAPTER-0001`
- `NPSPEC-HOTREPLACE-ABI-0001`
- `NPSPEC-HOTREPLACE-DRIVER-0001`
- `NPSPEC-HOTREPLACE-DEVICE-0001`
- `NPSPEC-HOTREPLACE-SERVICE-0001`
- `NPSPEC-HOTREPLACE-CAPABILITY-0001`
- `NPSPEC-HOTREPLACE-APPLICATION-0001`
- `NPSPEC-HOTREPLACE-UI-0001`
- `NPSPEC-HOTREPLACE-TRANSACTION-0001`
- `NPSPEC-HOTREPLACE-COMMIT-0001`
- `NPSPEC-HOTREPLACE-ROLLBACK-0001`
- `NPSPEC-HOTREPLACE-FORWARDRECOVERY-0001`
- `NPSPEC-HOTREPLACE-CANARY-0001`
- `NPSPEC-HOTREPLACE-SHADOW-0001`
- `NPSPEC-HOTREPLACE-VERIFICATION-0001`
- `NPSPEC-HOTREPLACE-GRACEPERIOD-0001`
- `NPSPEC-HOTREPLACE-RESOURCE-0001`
- `NPSPEC-HOTREPLACE-DETERMINISTIC-0001`
- `NPSPEC-HOTREPLACE-REALTIME-0001`
- `NPSPEC-HOTREPLACE-SECURITY-0001`
- `NPSPEC-HOTREPLACE-TRUST-0001`
- `NPSPEC-HOTREPLACE-SOVEREIGNTY-0001`
- `NPSPEC-HOTREPLACE-INTROSPECTION-0001`
- `NPSPEC-HOTREPLACE-AUDIT-0001`
- `NPSPEC-HOTREPLACE-JOURNAL-0001`
- `NPSPEC-HOTREPLACE-RECOVERY-0001`
- `NPSPEC-HOTREPLACE-RETENTION-0001`
- `NPSPEC-HOTREPLACE-DEPENDENCY-0001`
- `NPSPEC-HOTREPLACE-PLANNER-0001`
- `NPSPEC-HOTREPLACE-POLICY-0001`
- `NPSPEC-HOTREPLACE-DISTRIBUTED-0001`
- `NPSPEC-HOTREPLACE-ROLLING-0001`
- `NPSPEC-HOTREPLACE-PROTOCOL-0001`
- `NPSPEC-HOTREPLACE-AI-0001`
- `NPSPEC-HOTREPLACE-TESTING-0001`
- `NPSPEC-LIVEEVOLUTION-MODEL-0001`
- `NPSPEC-LIVEEVOLUTION-SCHEMA-0001`
- `NPSPEC-LIVEEVOLUTION-CAPABILITY-0001`
- `NPSPEC-LIVEEVOLUTION-ARCHDIFF-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`

---

## Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch Lifecycle-, State-, Compatibility-, Security-, Resource-, Concurrency-, Crash-, Rollback- und Recovery-Tests überprüft werden.

### Stateless-Replacement-Test

Eine zustandslose Komponente wird während des Betriebs ersetzt.

Erwartung:

```text
Old
 ↓
New
```

ohne Verlust logischer Capability-Verfügbarkeit.

### Stateful-Replacement-Test

Runtime-State wird von alter zu neuer Generation übertragen.

```text
Old State
    ↓
Migration
    ↓
New State
```

Der semantische Zustand muss erhalten bleiben.

### Schema-Migration-Test

State verschiedener Schema-Versionen muss korrekt migriert werden.

### Invalid-State-Test

Ungültiger migrierter State darf nicht aktiviert werden.

### Migration-Failure-Test

Bei Migration Failure muss die alte authoritative Generation erhalten bleiben.

### Quiescence-Test

Neue Arbeit wird gestoppt und laufende Arbeit korrekt drainiert.

### Long-Running-Task-Test

Eine lange Operation muss gemäß Contract:

- weiterlaufen,
- migriert,
- checkpointed,
- oder abgebrochen

werden.

### Atomic-Rebind-Test

Während des Binding Switch dürfen Clients ausschließlich:

```text
Old
```

oder:

```text
New
```

sehen.

Kein undefinierter Zwischenzustand ist zulässig.

### Existing-Call-Test

Ein bereits laufender Call auf alter Generation muss gemäß definierter Policy korrekt abgeschlossen werden.

### New-Call-Test

Neue Calls nach Commit müssen die neue Generation verwenden.

### Generation-Retirement-Test

Eine Generation darf nicht entfernt werden, solange gültige Bindings bestehen.

### Zero-Copy-Test

Shared Buffers müssen über Generationenwechsel hinweg korrekt weiterverwendbar sein.

### DMA-Test

Driver Replacement darf keine aktiven ungültigen DMA-Zugriffe hinterlassen.

### Driver-Test

Ein isolierter Treiber wird ersetzt und das Gerät danach weiterverwendet.

### Device-Reset-Test

Wenn State Transfer unmöglich ist, muss ein kontrollierter Device Reset funktionieren.

### Storage-Test

Storage Driver Replacement darf keine Datenkorruption verursachen.

### Network-Test

Netzwerkverbindungen sollen bei unterstütztem Handover erhalten bleiben.

### GPU-Test

Aktive GPU Workloads müssen kontrolliert behandelt werden.

### NPU-Test

Aktive NPU Workloads müssen kontrolliert behandelt werden.

### Service-Test

Ein Systemdienst wird ersetzt, ohne dass Clients ihre logische Service Capability verlieren.

### Session-Test

Benutzersitzungen werden, soweit unterstützt, erhalten.

### UI-Test

UI Replacement muss relevanten UI State erhalten.

### Transaction-Test

Ein mehrteiliger Replacement-Prozess muss entweder vollständig committen oder kontrolliert recovern.

### Crash-Before-Commit-Test

Crash vor Commit:

```text
Old Generation
```

muss weiterhin als authoritative bestimmbar sein.

### Crash-After-Commit-Test

Crash nach Commit:

```text
New Generation
```

muss als authoritative bestimmbar sein.

### Crash-During-Migration-Test

Recovery muss anhand des Journals den korrekten Zustand bestimmen.

### Rollback-Test

Eine fehlerhafte neue Generation muss auf die alte Generation zurückgerollt werden können.

### Reverse-Migration-Test

Wenn State bereits verändert wurde, muss Reverse Migration getestet werden.

### Forward-Recovery-Test

Wenn Rollback unmöglich ist, muss ein definierter Forward-Recovery-Pfad funktionieren.

### Canary-Test

Ein Teil der Workloads wird auf die neue Generation geroutet.

Fehler müssen erkannt werden, bevor vollständige Aktivierung erfolgt.

### Shadow-Test

Shadow Execution darf das authoritative Ergebnis nicht beeinflussen.

### Differential-Test

Alte und neue Ergebnisse werden semantisch verglichen.

### Grace-Period-Test

Während der Grace Period muss Rollback möglich bleiben.

### Resource-Test

Temporärer Ressourcenbedarf alter und neuer Generation muss korrekt gebucht werden.

### Resource-Pressure-Test

Bei unzureichenden Ressourcen muss Replacement:

```text
Deferred
Rejected
AlternativeStrategy
```

verwenden.

### Execution-Contract-Test

Neue Generation muss alle relevanten Hard Contracts erfüllen.

### Deterministic-Test

Ein gepinnter deterministischer Scope darf nicht unkontrolliert auf die neue Generation wechseln.

### Realtime-Test

Hard-Realtime-Workload darf durch Replacement nicht unzulässig gestört werden.

### Trust-Test

Eine nicht ausreichend vertrauenswürdige neue Generation darf nicht aktiviert werden.

### Signature-Test

Ungültig signierte Generation muss abgelehnt werden.

### Revocation-Test

Widerrufene Generation muss gemäß Security Policy ersetzt oder deaktiviert werden können.

### Privilege-Escalation-Test

Eine neue Generation mit zusätzlichen Capability-Anforderungen darf diese nicht automatisch erhalten.

### Secret-State-Test

Secrets dürfen nur an autorisierte Generationen übertragen werden.

### Data-Sovereignty-Test

Ein Replacement auf einen unzulässigen Remote Provider muss abgelehnt werden.

### Location-Test

Remote Replacement muss zusätzliche Latenz- und Netzwerkkosten berücksichtigen.

### Information-Flow-Test

Neue Generation darf keine verbotenen Datenflüsse erzeugen.

### Dependency-Test

Inkompatible Dependency-Versionen müssen Replacement verhindern.

### Replacement-Set-Test

Mehrere gekoppelte Komponenten müssen gemeinsam aktualisiert werden können.

### Dependency-Cycle-Test

Zyklische Abhängigkeiten müssen erkannt werden.

### Rolling-Upgrade-Test

Mehrere Serviceinstanzen werden schrittweise ersetzt.

### Mixed-Version-Test

Alte und neue Protokollversionen müssen innerhalb des erlaubten Fensters interoperieren.

### Protocol-Negotiation-Test

Unterschiedliche Versionen müssen korrekt aushandeln.

### Self-Healing-Test

Nova.Resilience muss eine defekte Generation durch eine Known-Good Generation ersetzen können.

### Replacement-Loop-Test

Wiederholtes automatisches Umschalten zwischen fehlerhaften Generationen muss verhindert werden.

### Quarantine-Test

Eine fehlerhafte Generation muss quarantänisiert werden können.

### Boot-Rollback-Test

Fehlerhafte neue Boot Generation muss auf Last Known Good zurückfallen können.

### NovaDOS-Recovery-Test

NovaDOS muss beschädigte beziehungsweise fehlerhafte Systemgenerationen offline zurückrollen können.

### Fuzz-Test

State Migration und Compatibility Adapter müssen mit ungültigen Inputs getestet werden.

### Failure-Injection-Test

Fehler werden gezielt injiziert bei:

```text
Validate
Stage
Quiesce
Export
Migration
Import
Rebind
Verify
Commit
Rollback
```

### Architecture-Introspection-Test

Introspection muss korrekt anzeigen:

```text
Current Generation
Target Generation
Replacement Phase
Bindings
Grace Period
Rollback State
```

### Decision-Trace-Test

NovaOS muss erklären können:

```text
Why replacement?
Why this version?
Why deferred?
Why rejected?
Why reboot?
Why rollback?
```

### Audit-Test

Sicherheitskritische Replacement-Ereignisse müssen auditierbar sein.

### Stress-Test

Zu testen sind:

- viele parallele Replacements,
- große Dependency Graphs,
- tiefe Capability Graphs,
- hoher Resource Pressure,
- viele aktive Execution Scopes,
- parallele alte und neue Generationen,
- häufige Rollbacks,
- Remote Provider,
- Driver Replacement,
- AI Model Replacement.

### Long-Running-Test

Systeme müssen über lange Laufzeiten viele Generationenwechsel durchführen können, ohne:

- Resource Leaks,
- stale Bindings,
- orphaned state,
- generation leaks,
- corrupted dependency graphs

zu erzeugen.

---

## Ergebnis

NovaOS behandelt Software und Systemkomponenten nicht als statische, während einer Systemlaufzeit unveränderliche Einheiten.

Stattdessen entsteht ein generationenbasiertes Evolutionsmodell:

```text
Logical Capability
       │
       ▼
Generation N
       │
       ▼
Replacement Transaction
       │
       ├── Validate
       ├── Stage
       ├── Quiesce
       ├── Migrate State
       ├── Rebind
       ├── Verify
       └── Commit
       │
       ▼
Generation N+1
```

Dabei bleiben die grundlegenden NovaOS-Garantien erhalten:

```text
Security
Trust
Data Sovereignty
Information Flow
Execution Contracts
Resource Economy
Structured Concurrency
Determinism
Realtime
Transactional Consistency
```

Hot Replacement ist damit nicht lediglich:

```text
replace binary
```

sondern:

```text
controlled transition
from one valid system generation
to another valid system generation
```

Live Evolution erweitert dieses Prinzip auf die gesamte Systemarchitektur:

```text
Implementation Evolution
        +
State Evolution
        +
Capability Evolution
        +
Schema Evolution
        +
Policy Evolution
        +
Architecture Evolution
```

Das langfristige Ziel lautet:

```text
NovaOS should evolve
without unnecessarily stopping.
```

Dabei gilt jedoch ausdrücklich:

```text
Live when safe.
Reboot when necessary.
Rollback when uncertain.
Recover when failure occurs.
```

Damit bildet `ADR-ARCH-0012` die Grundlage für ein NovaOS, dessen Architektur nicht nur für einen festen Releasezustand entworfen wird, sondern für kontrollierte Weiterentwicklung während seiner gesamten Lebensdauer.