# ADR-ARCH-0013 – Architecture Introspection

## Status

Angenommen

## Kategorie

Systemarchitektur / Observability / Introspection / Explainability / Diagnostics / Runtime Architecture

## Kontext

Moderne Betriebssysteme bestehen aus einer großen Anzahl miteinander verbundener Komponenten:

- Kernel-Subsystemen,
- Treibern,
- Diensten,
- Prozessen,
- Threads,
- Ressourcen,
- Namespaces,
- Sicherheitsdomänen,
- IPC-Verbindungen,
- Hardwaregeräten,
- Dateisystemen,
- Netzwerkverbindungen.

In NovaOS wird diese Komplexität zusätzlich durch dynamische und adaptive Architekturkonzepte erweitert.

Dazu gehören insbesondere:

- Capabilities,
- dynamische Capability Provider,
- automatische Algorithmusauswahl,
- Execution Contracts,
- systemweite Ressourcenökonomie,
- Semantic Types,
- Daten- und Objekt-Pipelines,
- Structured Concurrency,
- Location Transparency,
- Data Sovereignty,
- Nova.Trust,
- Self-Healing,
- Hot Replacement,
- Live Evolution,
- Deterministic Mode,
- adaptive Systementscheidungen.

Dadurch entsteht ein System, dessen konkrete Runtime-Architektur nicht vollständig statisch ist.

Beispielsweise kann derselbe abstrakte Aufruf:

```text
Image.Resize(image)
```

zu unterschiedlichen Zeitpunkten über unterschiedliche Ausführungspfade laufen:

```text
CPU Provider
```

oder:

```text
GPU Provider
```

oder:

```text
NPU Provider
```

oder theoretisch:

```text
Remote Provider
```

abhängig von:

- Hardware,
- Systemlast,
- Resource Budgets,
- Execution Contract,
- Trust,
- Data Sovereignty,
- Determinism,
- Energieverbrauch,
- thermischer Situation,
- Provider Health,
- Algorithmuswahl.

Eine solche Architektur darf für Entwickler, Administratoren und das Betriebssystem selbst nicht zu einer Black Box werden.

NovaOS benötigt deshalb eine systemweite Möglichkeit, seine eigene aktuelle Architektur zu beschreiben und zu untersuchen.

Diese Fähigkeit wird als:

```text
Architecture Introspection
```

bezeichnet.

Architecture Introspection beantwortet nicht nur:

```text
What is running?
```

sondern insbesondere:

```text
What exists?

How is it connected?

Why is it connected this way?

Which implementation is currently active?

Which resources are being used?

Which contracts apply?

Which security boundaries exist?

Where is data located?

Which decisions created this architecture?

How did the architecture change?

What would happen if a component failed?
```

Architecture Introspection ist damit mehr als klassisches Monitoring.

---

## Problem

Klassische Observability-Systeme zeigen häufig:

- CPU-Auslastung,
- Speicherverbrauch,
- Prozesse,
- Netzwerktraffic,
- Logs,
- Traces.

Diese Informationen sind notwendig, reichen für NovaOS jedoch nicht aus.

Ein NovaOS-System kann beispielsweise folgenden logischen Aufbau besitzen:

```text
Application
    │
    ▼
Image.Resize Capability
    │
    ▼
Execution Contract
    │
    ▼
Provider Selection
    │
    ▼
GPU Provider
    │
    ▼
Shared Tensor Buffer
    │
    ▼
GPU Device
```

Ein klassischer Prozessmonitor würde möglicherweise lediglich anzeigen:

```text
Process A
GPU Usage: 12 %
Memory: 240 MiB
```

Er würde jedoch nicht erklären:

- welche Capability verwendet wird,
- welcher Provider gewählt wurde,
- warum dieser Provider gewählt wurde,
- welcher Algorithmus verwendet wird,
- welcher Execution Contract gilt,
- welche Semantic Types transportiert werden,
- welche Shared Buffers beteiligt sind,
- welche Security Capabilities existieren,
- welche Trust-Anforderungen gelten,
- ob Daten lokal bleiben müssen,
- welche Alternative verworfen wurde,
- welche Kosten die Entscheidung verursacht,
- ob die Implementierung kürzlich hot-replaced wurde.

NovaOS benötigt daher ein wesentlich reichhaltigeres Architekturmodell.

---

## Entscheidung

NovaOS führt **Architecture Introspection als grundlegendes systemweites Architekturprinzip** ein.

Alle relevanten Systemsubsysteme sollen ihre logische und physische Runtime-Struktur über ein gemeinsames semantisches Introspection-Modell beschreibbar machen.

Das System soll dadurch seine eigene Architektur als dynamischen Graphen darstellen können.

Das grundlegende Modell lautet:

```text
Runtime System
      │
      ▼
Introspection Model
      │
      ▼
Architecture Graph
      │
      ├── Components
      ├── Capabilities
      ├── Providers
      ├── Execution Scopes
      ├── Resources
      ├── Data Objects
      ├── Pipelines
      ├── Trust Relationships
      ├── Security Boundaries
      ├── Locations
      ├── Generations
      └── Decisions
```

Dieser Graph soll:

- maschinenlesbar,
- typisiert,
- versioniert,
- filterbar,
- sicher zugreifbar,
- teilweise historisierbar

sein.

---

## Grundprinzip

Das zentrale Prinzip lautet:

```text
If NovaOS can make an architectural decision,
NovaOS should be able to explain that decision.
```

Zusätzlich:

```text
If NovaOS creates a runtime relationship,
that relationship should be introspectable
when security and privacy permit it.
```

---

## Architecture Introspection ist keine Debug-only-Funktion

Architecture Introspection ist kein ausschließlich für Entwickler vorgesehenes Debugging-System.

Sie ist Bestandteil der regulären NovaOS-Architektur.

Sie wird unter anderem verwendet für:

- Administration,
- Diagnose,
- Self-Healing,
- Security,
- Performanceanalyse,
- Ressourcenplanung,
- Hot Replacement,
- Live Evolution,
- Developer Tools,
- Systemvisualisierung,
- Audit,
- Testing,
- Support,
- adaptive Optimierung.

---

## Architecture Graph

Die Runtime-Architektur wird logisch als Graph modelliert.

```text
Node ──Relationship──> Node
```

Beispiel:

```text
Application
    │ uses
    ▼
Capability
    │ provided-by
    ▼
Provider
    │ executes-on
    ▼
GPU
```

Ein komplexerer Graph kann beispielsweise sein:

```text
Application
    │
    │ requests
    ▼
Capability
    │
    │ constrained-by
    ▼
ExecutionContract
    │
    │ resolved-to
    ▼
Provider
    │
    ├── uses ────────> Algorithm
    │
    ├── executes-on ─> GPU
    │
    └── accesses ────> DataObject
                           │
                           └── located-on ─> LocalMemory
```

---

## Architecture Node

Ein allgemeiner Architecture Node kann konzeptionell beschrieben werden als:

```text
ArchitectureNode {
    Identity
    Type
    Name
    Version
    Generation
    State
    Location
    Trust
    Metadata
}
```

Nicht jedes Feld ist für jeden Node erforderlich.

---

## Node Identity

Jeder introspektierbare Node benötigt eine stabile Identität innerhalb seines relevanten Scopes.

Identität darf nicht ausschließlich aus:

- Speicheradresse,
- Prozess-ID,
- Thread-ID

bestehen.

Diese Werte können zusätzliche Runtime-Attribute sein.

---

## Node Types

Mindestens folgende Node-Klassen sollen unterstützt werden:

```text
System
Node
HardwareDevice
KernelSubsystem
ExecutionDomain
Process
Task
Thread
StructuredScope
Service
Capability
Provider
Algorithm
ExecutionContract
Resource
ResourceBudget
DataObject
Buffer
Pipeline
PipelineStage
SemanticType
Policy
TrustIdentity
SecurityDomain
Namespace
Volume
FileObject
NetworkEndpoint
RemoteNode
Generation
Transaction
RecoveryAction
```

Weitere Typen können ergänzt werden.

---

## Architecture Relationship

Beziehungen sind ebenfalls typisiert.

Konzeptionell:

```text
ArchitectureRelationship {
    Identity
    Type
    Source
    Target
    State
    Metadata
}
```

---

## Relationship Types

Beispiele:

```text
contains
owns
uses
provides
requires
depends-on
calls
communicates-with
reads
writes
maps
shares
executes-on
scheduled-on
located-on
constrained-by
authorized-by
trusted-by
derived-from
replaced-by
member-of
parent-of
child-of
flows-to
```

---

## Typisierte Beziehungen

Relationships dürfen nicht nur unstrukturierte Strings sein.

Tools sollen beispielsweise unterscheiden können zwischen:

```text
uses
```

und:

```text
owns
```

oder:

```text
reads
```

und:

```text
writes
```

---

## Hierarchische Sicht

Obwohl das interne Modell graphbasiert ist, muss eine hierarchische Darstellung möglich sein.

Beispiel:

```text
System
├── Kernel
├── Services
│   ├── Nova.Index
│   └── Nova.Trust
├── Applications
│   ├── App A
│   └── App B
├── Devices
└── Resources
```

---

## Capability View

Architecture Introspection muss Capability-Beziehungen darstellen können.

Beispiel:

```text
Application
    │
    ▼
de.nova.image.filter.gaussian
    │
    ├── Provider CPU
    ├── Provider GPU
    └── Provider NPU
```

Zusätzlich muss sichtbar sein, welcher Provider aktuell verwendet wird.

```text
Selected:
    GPU Provider
```

---

## Capability Identity

Capability-Identitäten verwenden das NovaOS Capability-Modell.

Beispiel:

```text
de.nova.image.filter.gaussian
```

mit:

```text
domain    = de
authority = nova
namespace = image.filter
name      = gaussian
```

Der physische Installationspfad bestimmt nicht die Capability Identity.

---

## Provider View

Für einen Provider sollen unter anderem darstellbar sein:

```text
Provider {
    Identity
    Capability
    Version
    Generation
    ExecutionLocation
    TrustLevel
    ResourceProfile
    Health
    State
}
```

---

## Provider Selection

Wenn NovaOS automatisch einen Provider auswählt, muss die Entscheidung introspektierbar sein.

Beispiel:

```text
Capability:
    de.nova.image.resize

Candidates:
    CPU Provider
    GPU Provider
    NPU Provider

Selected:
    GPU Provider
```

---

## Decision Trace

Architecture Introspection integriert ein systemweites:

```text
Decision Tracing
```

Ein Decision Trace erklärt, warum eine Entscheidung getroffen wurde.

Beispiel:

```text
Decision:
    Select GPU Provider

Reason:
    latency requirement = 8 ms

Candidates:

CPU
    estimated latency = 14 ms
    rejected

GPU
    estimated latency = 4 ms
    selected

NPU
    unsupported semantic input type
    rejected
```

---

## Decision Record

Konzeptionell:

```text
DecisionRecord {
    Identity
    Timestamp
    Subject
    Candidates
    Constraints
    SelectedOption
    RejectedOptions
    Reasons
    PolicyVersion
    ModelVersion
}
```

---

## Keine vollständige interne Reasoning-Offenlegung

Decision Tracing bedeutet nicht, dass beliebige interne Implementierungsdetails oder interne KI-Reasoning-Zustände offengelegt werden müssen.

Das System soll stattdessen strukturierte, überprüfbare Entscheidungsgründe bereitstellen.

Beispiel:

```text
Selected GPU because:
    deadline requirement
    available memory
    provider trust
    local-only sovereignty policy
```

---

## Execution Contract View

Architecture Introspection muss relevante Execution Contracts darstellen können.

Beispiel:

```text
ExecutionContract {
    latency      <= 5 ms
    memory       <= 128 MiB
    determinism  = Required
    sovereignty  = LocalOnly
    trust        >= Verified
    algorithm    = Automatic
}
```

---

## Contract Binding

Es muss sichtbar sein, welche Ausführung welchem Contract unterliegt.

```text
Execution Scope
      │
      ▼
Execution Contract
```

---

## Contract Satisfaction

Das System soll anzeigen können, ob ein Contract aktuell erfüllt wird.

```text
Latency:
    required <= 5 ms
    observed = 3.8 ms
    state = Satisfied
```

oder:

```text
Latency:
    required <= 5 ms
    observed = 7.2 ms
    state = Violated
```

---

## Contract Violation

Contract Violations müssen als Architekturereignisse sichtbar sein können.

---

## Resource Economy View

Architecture Introspection integriert:

```text
ADR-ARCH-0005
Systemweite Ressourcenökonomie
```

Darstellbar sein sollen unter anderem:

- Resource Supply,
- Capacity,
- Reservations,
- Budgets,
- Consumption,
- Pressure,
- Throttling,
- Resource Ownership.

---

## Resource Graph

Beispiel:

```text
System Memory
├── Kernel
│   └── 512 MiB
├── Services
│   └── 1.2 GiB
└── Applications
    ├── App A
    │   └── 620 MiB
    └── App B
        └── 340 MiB
```

---

## Resource Attribution

Ressourcen müssen möglichst dem verursachenden logischen Objekt zugeordnet werden.

Beispiel:

```text
Application
    ↓
Capability
    ↓
Provider
    ↓
GPU Allocation
```

Dadurch kann beantwortet werden:

```text
Why is this GPU memory allocated?
```

---

## Shared Resource Accounting

Shared Resources dürfen nicht fälschlich mehrfach als physischer Verbrauch gezählt werden.

Architecture Introspection muss unterscheiden können zwischen:

```text
Physical Allocation
```

und:

```text
Logical Mapping
```

---

## Zero-Copy View

Architecture Introspection integriert:

```text
ADR-ARCH-0006
Zero-Copy als Architekturprinzip
```

Datenpfade sollen physische Kopien sichtbar machen können.

Beispiel:

```text
Camera
  │ DMA
  ▼
Shared Buffer
  │ view
  ▼
GPU
  │
  ▼
Compositor
```

Ergebnis:

```text
Physical Copies:
    0
```

---

## Copy Reason

Wenn eine Kopie stattfindet, soll der Grund sichtbar sein können.

Beispiel:

```text
Copy:
    8 MiB

Reason:
    FormatConversion
```

Mögliche Gründe:

```text
SecurityCopy
FormatConversion
AlignmentCopy
LifetimeCopy
RemoteSerialization
CopyOnWrite
SmallPayloadOptimization
HardwareLimitation
```

---

## Data Movement View

Architecture Introspection soll Datenbewegungen darstellen können.

Beispiel:

```text
NVMe
  ↓
RAM
  ↓
GPU VRAM
```

mit:

```text
NVMe → RAM
    120 MiB

RAM → VRAM
    120 MiB
```

---

## Semantic Types View

Architecture Introspection integriert:

```text
ADR-ARCH-0008
Semantic Types als systemweites Typsystem
```

Datenobjekte sollen nicht nur als Bytebereiche erscheinen.

Beispiel:

```text
Buffer
    Size = 24 MiB
```

kann zusätzlich semantisch beschrieben werden als:

```text
Image<RGBA8>
    Width  = 3840
    Height = 2160
```

---

## Semantic Pipeline View

Ein Pipeline-Graph kann dadurch darstellen:

```text
Image<RAW>
    ↓
Decode
    ↓
Image<LinearRGB>
    ↓
ColorTransform
    ↓
Image<sRGB>
    ↓
Display
```

---

## Data and Object Pipeline View

Architecture Introspection integriert:

```text
ADR-ARCH-0009
Einheitliches Daten- und Objekt-Pipelining
```

Pipelines sollen als Graph sichtbar sein.

Beispiel:

```text
Source
  ↓
Decoder
  ↓
Filter
  ↓
AI Enhancer
  ↓
Encoder
  ↓
Sink
```

---

## Pipeline Stage

Ein Pipeline Stage kann darstellen:

```text
PipelineStage {
    Capability
    Provider
    InputType
    OutputType
    ResourceUsage
    Latency
    Location
    State
}
```

---

## Pipeline Bottleneck

Tools sollen erkennen können:

```text
Stage 1 = 1 ms
Stage 2 = 2 ms
Stage 3 = 14 ms
Stage 4 = 1 ms
```

und Stage 3 als möglichen Bottleneck identifizieren.

---

## Structured Concurrency View

Architecture Introspection integriert:

```text
ADR-ARCH-0010
Structured Concurrency
```

Task-Strukturen sollen als Baum beziehungsweise Graph darstellbar sein.

Beispiel:

```text
Application Scope
├── Document Scope
│   ├── Load Task
│   ├── Render Task
│   └── Autosave Task
└── UI Scope
    ├── Input Task
    └── Animation Task
```

---

## Task Ownership

Es muss sichtbar sein:

```text
Who owns this task?
```

und:

```text
Which scope will cancel it?
```

---

## Cancellation Propagation

Cancellation-Beziehungen sollen darstellbar sein.

```text
Parent Scope
    ↓ cancel
Child Scope
    ↓ cancel
Task
```

---

## Deadline Propagation

Deadlines können ebenfalls sichtbar sein.

```text
Parent Deadline
      ↓
Child Deadline
      ↓
Task Deadline
```

---

## Orphan Detection

Architecture Introspection soll helfen, unstrukturierte beziehungsweise verwaiste Tasks zu erkennen.

---

## Deterministic Mode View

Architecture Introspection integriert:

```text
ADR-ARCH-0011
Deterministic Mode
```

Für einen deterministischen Scope soll sichtbar sein:

```text
Determinism = Required

CPU Set:
    2,3

Provider:
    GPU0

Algorithm:
    FFT.Radix4

Generation:
    42

Adaptive Decisions:
    Disabled
```

---

## Determinism Violations

Abweichungen müssen sichtbar sein.

Beispiel:

```text
Violation:
    Provider generation changed during deterministic scope.
```

---

## Hot Replacement View

Architecture Introspection integriert:

```text
ADR-ARCH-0012
Hot Replacement und Live Evolution
```

Generationen sollen sichtbar sein.

```text
Capability
    │
    ├── Generation 41
    │      State = Retiring
    │
    └── Generation 42
           State = Active
```

---

## Replacement State

Darstellbar sein sollen:

```text
Current Generation
Target Generation
Replacement Phase
State Migration
Grace Period
Rollback State
```

---

## Architecture Evolution

Architecture Introspection muss nicht nur den aktuellen Zustand, sondern auch Architekturänderungen darstellen können.

---

## Architecture Snapshot

Ein Architecture Snapshot beschreibt einen konsistenten beziehungsweise ausreichend kohärenten Blick auf die Architektur zu einem Zeitpunkt.

Konzeptionell:

```text
ArchitectureSnapshot {
    Identity
    Timestamp
    Nodes
    Relationships
    SystemGeneration
}
```

---

## Snapshot Consistency

Ein Snapshot muss nicht zwangsläufig das gesamte System global stoppen.

Je nach Anwendungsfall können unterschiedliche Konsistenzgrade verwendet werden.

Mindestens:

```text
BestEffort
Coherent
Transactional
Deterministic
```

---

## Best-Effort Snapshot

Für Monitoring genügt häufig ein ungefähr konsistenter Snapshot.

---

## Transactional Snapshot

Für kritische Diagnose oder Recovery kann ein stärker konsistenter Snapshot erforderlich sein.

---

## Architecture Diff

Zwei Snapshots können verglichen werden.

```text
Snapshot A
    ↓
Architecture Diff
    ↓
Snapshot B
```

Beispiel:

```text
+ Provider Generation 42
- Provider Generation 41

Changed:
    Binding:
        Generation 41 → 42
```

---

## Time Travel Diagnostics

Wenn historische Daten verfügbar sind, können Tools Architekturzustände rückwirkend untersuchen.

Beispiel:

```text
Architecture at 14:31:20
```

gegen:

```text
Architecture at 14:31:25
```

Dies bedeutet nicht, dass NovaOS zwingend vollständiges System-Time-Travel-Debugging implementieren muss.

---

## Event Stream

Architekturänderungen können als Events veröffentlicht werden.

Beispiele:

```text
NodeCreated
NodeRemoved
RelationshipCreated
RelationshipRemoved
BindingChanged
ProviderSelected
ResourceReserved
ContractViolated
GenerationActivated
TaskCreated
TaskCancelled
TrustChanged
LocationChanged
PipelineReconfigured
```

---

## Event Ordering

Events sollen geeignete:

- timestamps,
- sequence numbers,
- causal identifiers

besitzen können.

---

## Causality

Architecture Introspection soll kausale Beziehungen darstellen können.

Beispiel:

```text
User Action
    ↓
Capability Request
    ↓
Provider Selection
    ↓
Resource Reservation
    ↓
GPU Execution
```

---

## Correlation Identity

Zusammengehörige Ereignisse sollen über Correlation IDs verbunden werden können.

---

## Distributed Tracing

Bei Remote Execution muss Architecture Introspection über Node-Grenzen hinweg funktionieren können.

Beispiel:

```text
Local Application
      ↓
Remote Capability Provider
      ↓
Remote GPU
```

---

## Location Transparency View

Architecture Introspection integriert:

```text
ADR-ARCH-0007
Location Transparency mit sichtbaren Kosten und Fehlern
```

Ein logischer Aufruf kann lokal oder remote ausgeführt werden.

Die tatsächliche Location muss introspektierbar bleiben.

---

## Location

Beispiel:

```text
Provider:
    de.nova.ai.inference

Location:
    Node = workstation-01
    Device = GPU0
```

oder:

```text
Location:
    Node = remote-node-17
    Region = eu-central
```

abhängig von Berechtigungen.

---

## Location Costs

Bei Remote Execution sollen relevante Kosten sichtbar sein können.

Beispiel:

```text
Network Latency:
    18 ms

Transferred:
    24 MiB

Estimated Energy:
    ...

Monetary Cost:
    ...
```

wenn diese Informationen verfügbar und autorisiert sind.

---

## Location Failure

Fehler dürfen nicht durch Location Transparency versteckt werden.

Beispiel:

```text
Execution Failed

Cause:
    RemoteNodeUnavailable
```

---

## Data Sovereignty View

Architecture Introspection muss Data-Sovereignty-Constraints darstellen können.

Beispiel:

```text
Data:
    PersonalDocument

Sovereignty:
    LocalOnly
```

und:

```text
Current Location:
    Local
```

---

## Information Flow View

Informationsflüsse können als Graph dargestellt werden.

```text
DataObject A
    ↓
Service B
    ↓
DataObject C
```

Security Policies bestimmen, wie detailliert diese Informationen sichtbar sind.

---

## Security Architecture View

Architecture Introspection muss Security Boundaries darstellen können.

Beispiel:

```text
Kernel
────────────────────────────

Driver Domain A
────────────────────────────

Application Domain
────────────────────────────
```

---

## Capability Security View

Capability Grants können introspektierbar sein.

Beispiel:

```text
Application A
    │
    └── File.Read
```

oder:

```text
Application A
    │
    └── Network.Connect
            Scope:
                example.org
```

---

## Capability Provenance

Es soll nachvollziehbar sein können:

```text
Who granted this capability?
```

---

## Capability Delegation

Delegationsketten können dargestellt werden.

```text
System
  ↓
Service A
  ↓
Service B
  ↓
Task C
```

---

## Capability Attenuation

Es muss sichtbar sein können, wenn Rechte eingeschränkt wurden.

```text
Parent:
    File.ReadWrite

Child:
    File.Read
```

---

## Nova.Trust View

Architecture Introspection integriert Nova.Trust.

Beispiel:

```text
Provider:
    Nova.Image.Decoder

Trust:
    Identity   = Verified
    Signature  = Valid
    Provenance = Verified
    Permission = Granted
    Capability = Active
```

---

## Provenance Graph

Provenance kann als Graph dargestellt werden.

```text
Source
  ↓
Build
  ↓
Package
  ↓
Installation
  ↓
Runtime Generation
```

---

## Trust Changes

Änderungen des Trust-Zustands sollen Events erzeugen können.

---

## Revocation

Wird eine Komponente widerrufen, muss dies im Architecture Graph sichtbar sein.

---

## Security Boundaries

Architecture Introspection darf Security Boundaries nicht selbst schwächen.

Das Wissen über Systemarchitektur kann sicherheitsrelevant sein.

Beispiele:

- Speicheradressen,
- Prozessstrukturen,
- Netzwerkendpunkte,
- Security Capabilities,
- Hardwaretopologie,
- Trust Relationships.

Daher benötigt Introspection ein eigenes Autorisierungsmodell.

---

## Introspection Capability

Zugriff auf Introspection erfolgt capability-basiert.

Beispiel:

```text
System.Introspection.Read
```

Feinere Rechte können existieren.

Beispiel:

```text
System.Introspection.Process.Read
System.Introspection.Resource.Read
System.Introspection.Security.Read
System.Introspection.Trust.Read
System.Introspection.History.Read
```

Die endgültige Capability-Namensgebung wird separat spezifiziert.

---

## Scoped Introspection

Anwendungen sollen standardmäßig nur ihren eigenen relevanten Architektur-Scope sehen.

Beispiel:

```text
Application
    ├── Own Tasks
    ├── Own Resources
    ├── Own Capabilities
    └── Authorized Shared Objects
```

---

## Administrative Introspection

Administratoren können abhängig von Security Policy eine umfassendere Sicht erhalten.

---

## Kernel Introspection

Kernel und Nova.Resilience benötigen gegebenenfalls privilegierte vollständige Introspection.

---

## Privacy

Introspection darf keine unnötigen personenbezogenen Daten offenlegen.

---

## Redaction

Nicht autorisierte Informationen können redigiert werden.

Beispiel:

```text
Remote Provider:
    Location = Restricted
```

statt einer genauen Adresse.

---

## Aggregation

Statt einzelner Details können aggregierte Werte bereitgestellt werden.

Beispiel:

```text
Other Applications:
    Memory = 3.4 GiB
```

ohne einzelne Anwendungen aufzulisten.

---

## Side Channels

Introspection kann Side Channels erzeugen.

Beispielsweise könnten hochauflösende Ressourcenmetriken Rückschlüsse auf andere Prozesse erlauben.

Deshalb können:

- Auflösung,
- Frequenz,
- Genauigkeit,
- Sichtbarkeit

durch Security Policy eingeschränkt werden.

---

## Observability vs Introspection

NovaOS unterscheidet:

```text
Observability
```

und:

```text
Architecture Introspection
```

Observability beantwortet beispielsweise:

```text
CPU usage = 42 %
```

Architecture Introspection beantwortet zusätzlich:

```text
Which architecture objects caused the 42 %?
```

---

## Logs

Logs bleiben Bestandteil der Observability-Infrastruktur.

Architecture Introspection ersetzt Logs nicht.

---

## Metrics

Metrics bleiben Bestandteil der Observability-Infrastruktur.

Architecture Introspection verknüpft Metrics mit Architekturidentitäten.

---

## Traces

Distributed und lokale Traces können Architecture Nodes referenzieren.

---

## Unified Identity

Logs, Metrics, Traces und Architecture Introspection sollen möglichst dieselben stabilen Identitäten referenzieren.

Beispiel:

```text
ProviderGenerationID = 42
```

kann in:

- Logs,
- Metrics,
- Traces,
- Architecture Graph

verwendet werden.

---

## Telemetry Schema

Telemetry soll strukturierte Semantic Types verwenden.

Unstrukturierte Textlogs allein reichen nicht aus.

---

## Introspection API

NovaOS stellt eine native Introspection API bereit.

Konzeptionell:

```text
Nova.Introspection
```

Die endgültige API-Struktur wird durch NPSPECs definiert.

---

## Query Model

Die API soll Abfragen gegen den Architecture Graph ermöglichen.

Beispiel:

```text
Find all providers
for capability X
```

oder:

```text
Find all tasks
owned by application Y
```

oder:

```text
Show all physical copies
in pipeline Z
```

---

## Query

Konzeptionell:

```text
ArchitectureQuery {
    Scope
    NodeFilter
    RelationshipFilter
    Time
    DetailLevel
}
```

---

## Streaming Query

Tools können Architekturänderungen abonnieren.

```text
Subscribe {
    NodeType = Provider
    Event = BindingChanged
}
```

---

## Snapshot Query

Tools können einen Snapshot anfordern.

---

## Historical Query

Wenn historische Daten vorhanden sind:

```text
Query at Time T
```

---

## Graph Traversal

Abfragen sollen Relationship Traversal unterstützen.

Beispiel:

```text
Application
    → uses
Capability
    → provided-by
Provider
    → executes-on
GPU
```

---

## Bounded Queries

Introspection Queries müssen Ressourcenlimits besitzen.

Eine Anwendung darf nicht durch eine komplexe Graph Query das System überlasten.

---

## Resource Economy

Auch Introspection selbst unterliegt der systemweiten Ressourcenökonomie.

---

## Introspection Budget

Mögliche Ressourcen:

```text
Query CPU
Query Memory
History Storage
Event Bandwidth
Trace Bandwidth
```

---

## Sampling

Hochfrequente Informationen dürfen gesampelt werden.

---

## Backpressure

Introspection Event Streams müssen Backpressure unterstützen.

---

## Event Loss

Wenn Events aufgrund von Resource Pressure verloren gehen dürfen, muss dies erkennbar sein.

Beispiel:

```text
EventsDropped = 128
```

---

## Persistent History

Nicht jede Architecture Information muss dauerhaft gespeichert werden.

Retention Policies bestimmen:

- welche Events,
- welche Snapshots,
- welche Decision Records

persistiert werden.

---

## History Levels

Mögliche Stufen:

```text
None
Minimal
Operational
Diagnostic
Audit
FullDebug
```

---

## Audit History

Security-relevante Events können strengere Retention Policies besitzen.

---

## Architecture Introspection und Self-Healing

Nova.Resilience verwendet Architecture Introspection zur Diagnose.

Beispiel:

```text
Service Failure
      ↓
Inspect Dependency Graph
      ↓
Identify Failed Provider
      ↓
Inspect Generation
      ↓
Inspect Resource State
      ↓
Select Recovery Action
```

---

## Failure Propagation

Der Architecture Graph kann zeigen:

```text
Failed Device
    ↓
Provider
    ↓
Capability
    ↓
Applications
```

Dadurch kann NovaOS abschätzen, welche Komponenten betroffen sind.

---

## Blast Radius

Architecture Introspection soll die Bestimmung des:

```text
Blast Radius
```

eines Fehlers unterstützen.

---

## Recovery Planning

Self-Healing kann anhand des Graphen bestimmen:

- welche Komponenten neu gestartet werden müssen,
- welche Capabilities neu gebunden werden können,
- welche Daten betroffen sind,
- welche Ressourcen neu reserviert werden müssen.

---

## Dependency Diagnostics

Ein Fehler:

```text
Capability unavailable
```

kann beispielsweise zurückverfolgt werden zu:

```text
Capability
    ↓
Provider
    ↓
Driver
    ↓
Device
    ↓
Hardware Failure
```

---

## Architecture Introspection und Hot Replacement

Vor Replacement kann geprüft werden:

```text
Who depends on this generation?
```

Nach Replacement:

```text
Who still references old generation?
```

---

## Stale Binding Detection

Veraltete Bindings sollen erkannt werden können.

---

## Generation Leak Detection

Wenn eine alte Generation nicht retired werden kann:

```text
Generation 41
    State = Retiring
    Age = 4h
```

kann Architecture Introspection zeigen:

```text
Remaining references:
    Task A
    Shared Buffer B
    Service C
```

---

## Resource Leak Detection

Dasselbe gilt für Ressourcen.

```text
Allocation
    Owner = terminated task
```

kann als möglicher Leak erkannt werden.

---

## Buffer Leak Detection

Zero-Copy Buffers benötigen Lifetime Introspection.

---

## Deadlock Diagnostics

Wait-Relationships können optional in den Architecture Graph integriert werden.

Beispiel:

```text
Task A
  waits-for
Lock B

Task C
  owns
Lock B
```

---

## Wait Graph

Ein Wait Graph kann Deadlocks sichtbar machen.

---

## Priority Inversion

Scheduler- und Resource-Informationen können Priority Inversion diagnostizierbar machen.

---

## Scheduling View

Architecture Introspection kann anzeigen:

```text
Task
    Priority
    SchedulingClass
    CPUAffinity
    Deadline
    Runtime
```

---

## Hardware Topology

Hardwaretopologie ist Teil des Architecture Graph.

Beispiel:

```text
System
├── NUMA Node 0
│   ├── CPU 0
│   ├── CPU 1
│   └── Memory
├── NUMA Node 1
│   ├── CPU 2
│   ├── CPU 3
│   └── Memory
└── PCIe
    └── GPU0
```

---

## Resource Locality

Dadurch können Daten- und Execution-Locations gemeinsam analysiert werden.

Beispiel:

```text
Task
    executes-on CPU Node 0

Buffer
    located-on NUMA Node 1
```

NovaOS kann dies als potenziell suboptimale Platzierung erkennen.

---

## Energy View

Wenn Hardwaredaten verfügbar sind, kann Introspection Energieinformationen darstellen.

---

## Thermal View

Thermal Headroom kann ebenfalls dargestellt werden.

---

## Adaptive Systems

Adaptive Entscheidungen müssen introspektierbar bleiben.

Beispiel:

```text
Prediction:
    User likely opens File A

Action:
    Prefetch File A

Result:
    User opened File B

Prediction Error:
    High
```

---

## Prediction Error

Prediction Error kann als strukturiertes Feedback dargestellt werden.

```text
Prediction
    ↓
Actual Outcome
    ↓
Deviation
    ↓
Model Update
```

---

## Model Version

Adaptive Entscheidungen sollen die verwendete Modellversion referenzieren können.

---

## Policy Version

Policy-basierte Entscheidungen sollen die verwendete Policy-Version referenzieren können.

---

## Reproducibility

Wenn eine Entscheidung untersucht wird, sollen relevante Inputs möglichst rekonstruierbar sein.

Beispiel:

```text
Decision Inputs:
    Resource Pressure = Normal
    GPU Load = 24 %
    Deadline = 8 ms
    Sovereignty = LocalOnly
    Trust >= Verified
```

---

## Deterministic Decision Record

Für Deterministic Mode können Decision Records stärker eingeschränkt und reproduzierbar gestaltet werden.

---

## Explainability

NovaOS soll nicht nur technische Rohdaten liefern.

Es soll daraus verständliche Erklärungen erzeugen können.

Beispiel:

```text
Warum wurde die GPU verwendet?

Die GPU wurde gewählt, weil die CPU den geforderten
8-ms-Latenzvertrag voraussichtlich nicht erfüllen konnte.
Die GPU war lokal verfügbar, erfüllte die Trust-Anforderung
und benötigte keine zusätzliche Datenübertragung.
```

---

## Unterschiedliche Zielgruppen

Erklärungen können für unterschiedliche Zielgruppen aufbereitet werden.

### Benutzer

```text
Diese Aufgabe wird auf deiner lokalen GPU ausgeführt,
weil sie dort schneller verarbeitet werden kann.
```

### Entwickler

```text
Provider GPU0 selected.
Estimated latency: 3.8 ms.
CPU estimate: 11.2 ms.
```

### Administrator

```text
GPU0 selected due to latency SLA.
Resource reservation: 128 MiB VRAM.
```

### Security Analyst

```text
Remote providers rejected because sovereignty=LocalOnly.
```

---

## Rohdaten bleiben verfügbar

Erklärungen dürfen strukturierte Rohdaten nicht vollständig ersetzen.

---

## Developer Tools

Architecture Introspection bildet die Grundlage für NovaOS-Entwicklerwerkzeuge.

Mögliche Tools:

```text
Architecture Explorer
Capability Explorer
Pipeline Explorer
Resource Explorer
Trust Explorer
Execution Explorer
Dependency Explorer
Generation Explorer
```

---

## Architecture Explorer

Ein Architecture Explorer kann den laufenden Architecture Graph visualisieren.

Beispiel:

```text
Application
    │
    ├── Capability A
    │      └── Provider A
    │
    └── Capability B
           └── Provider C
```

---

## Pipeline Explorer

Zeigt:

- Pipeline Stages,
- Semantic Types,
- Provider,
- Buffer,
- Copies,
- Latency.

---

## Resource Explorer

Zeigt:

- Budgets,
- Reservations,
- Consumption,
- Pressure,
- Ownership.

---

## Trust Explorer

Zeigt:

- Identity,
- Signature,
- Provenance,
- Permissions,
- Capabilities,
- Audit State.

---

## Execution Explorer

Zeigt Structured Concurrency und Execution Contracts.

---

## Generation Explorer

Zeigt:

- aktive Generationen,
- alte Generationen,
- Replacement State,
- Rollback State.

---

## CLI

Architecture Introspection muss nicht ausschließlich grafisch verfügbar sein.

CLI-basierte Werkzeuge sollen möglich sein.

Konzeptionell:

```text
nova inspect ...
```

Die konkrete CLI-Syntax wird separat spezifiziert.

---

## Programmatic Access

Anwendungen und Systemdienste können autorisierte Introspection Queries programmatisch ausführen.

---

## Machine-Readable First

Das primäre Modell soll maschinenlesbar sein.

Textuelle oder grafische Darstellung wird daraus erzeugt.

---

## Kein Screen-Scraping

Systemtools dürfen nicht darauf angewiesen sein, Textausgaben anderer Tools zu parsen.

---

## Stable Semantic Schema

Introspection Schemas müssen versioniert sein.

---

## Schema Evolution

Architecture Introspection selbst muss mit NovaOS evolvieren können.

---

## Unknown Fields

Clients sollen unbekannte optionale Felder tolerieren können.

---

## Version Negotiation

Introspection Clients und Provider sollen Schema-Versionen aushandeln können.

---

## Extensions

Subsysteme dürfen eigene Introspection-Erweiterungen bereitstellen.

Diese dürfen das gemeinsame Kernmodell ergänzen.

---

## Vendor Extensions

Hardware- oder Drittanbieter können zusätzliche Felder bereitstellen.

Sie müssen namespaced sein.

---

## No Unstructured Extension Pollution

Vendor Extensions dürfen das Kernschema nicht mit unkoordinierten globalen Namen verschmutzen.

---

## Performance

Architecture Introspection darf Fast Paths nicht unnötig verlangsamen.

---

## Lazy Materialization

Teure Introspection-Daten können bei Bedarf erzeugt werden.

---

## Cached Metadata

Statische oder selten veränderte Metadaten können gecacht werden.

---

## Event-Driven Updates

Dynamische Architekturänderungen sollen bevorzugt Events erzeugen, statt den gesamten Graph permanent neu zu scannen.

---

## Lock-Free / Low-Contention

Fast-Path-Metriken und Events sollen, wenn sinnvoll, mit Low-Contention-Techniken erfasst werden.

---

## Debug Detail Levels

Der Detailgrad kann konfigurierbar sein.

Beispiel:

```text
Minimal
Normal
Detailed
Debug
Trace
```

---

## Production Safety

Debug-Level darf Production-Systeme nicht unkontrolliert destabilisieren.

---

## Failure Independence

Ein Fehler im Introspection-System darf nicht automatisch die introspektierte Komponente zum Absturz bringen.

---

## Out-of-Band Diagnostics

Für kritische Komponenten können minimale unabhängige Diagnosepfade existieren.

---

## Kernel Panic

Bei schweren Kernel-Fehlern kann ein reduzierter Architecture Snapshot für Crash Dumps erzeugt werden.

---

## Crash Dump

Ein Crash Dump kann referenzieren:

- aktive Tasks,
- relevante Generationen,
- Resource State,
- Device State,
- Dependency Graph,
- letzte Decision Records.

---

## Boot Introspection

Auch der Bootprozess kann Introspection Events erzeugen.

Beispiel:

```text
Firmware
   ↓
Bootloader
   ↓
Kernel
   ↓
Core Services
   ↓
System Ready
```

---

## Boot Failure

Bei Boot Failure kann sichtbar sein:

```text
Failed Component:
    Storage Provider

Cause:
    Device initialization timeout
```

---

## NovaDOS

NovaDOS soll einen reduzierten Offline-Introspection-Modus unterstützen können.

Es muss nicht den vollständigen Live Architecture Graph implementieren.

NovaDOS soll jedoch mindestens relevante persistierte Informationen lesen können, beispielsweise:

- Boot Generation,
- Last Known Good,
- Update State,
- Recovery Records,
- Crash Records,
- relevante Hardwareinformationen.

---

## Introspection Persistence

Ein Teil der Architekturinformationen kann persistent gespeichert werden.

Dies muss jedoch bewusst erfolgen.

---

## Sensitive Persistence

Security-sensitive Introspection-Daten dürfen nicht ungeschützt persistent gespeichert werden.

---

## Data Retention

Retention Policies gelten auch für Architecture History.

---

## Data Sovereignty für Telemetrie

Introspection- und Telemetriedaten unterliegen selbst Data Sovereignty.

Sie dürfen nicht automatisch an externe Systeme übertragen werden.

---

## Local-First Diagnostics

NovaOS soll Diagnose grundsätzlich lokal durchführen können.

Remote Telemetry ist eine separate autorisierte Policy-Entscheidung.

---

## Export

Architecture Snapshots können exportierbar sein.

Export muss:

- Security,
- Privacy,
- Data Sovereignty,
- Redaction

berücksichtigen.

---

## Support Bundle

NovaOS kann ein Support Bundle erzeugen.

Beispiel:

```text
SupportBundle {
    ArchitectureSnapshot
    Logs
    Metrics
    RelevantTraces
    CrashData
    SystemVersion
}
```

Sensitive Informationen müssen redigiert werden können.

---

## Architecture Fingerprint

Ein System kann einen abstrakten Architecture Fingerprint besitzen.

Dieser kann beispielsweise relevante:

- Komponenten,
- Versionen,
- Generationen,
- Policies

zusammenfassen.

Er darf keine unnötigen personenbezogenen Informationen enthalten.

---

## Testing

Architecture Introspection soll auch für automatisierte Tests verwendet werden.

Beispiel:

```text
Assert:
    no remote provider used

Assert:
    zero physical copies

Assert:
    deterministic provider pinned

Assert:
    no stale generation
```

---

## Architecture Assertions

NovaOS soll langfristig Architecture Assertions unterstützen können.

Beispiel:

```text
assert Pipeline.Copies <= 1
```

oder:

```text
assert Data.Location == Local
```

Die konkrete Syntax wird separat spezifiziert.

---

## Continuous Verification

Architecture Assertions können zur kontinuierlichen Verifikation verwendet werden.

---

## Policy Verification

Security- und Resource Policies können gegen den tatsächlichen Architecture Graph geprüft werden.

---

## Desired vs Actual Architecture

Das deklarative Systemmodell beschreibt:

```text
Desired Architecture
```

Architecture Introspection beschreibt:

```text
Actual Architecture
```

Der Vergleich ermöglicht:

```text
Desired
   ↓
Diff
   ↓
Actual
```

---

## Architecture Drift

Abweichungen werden als:

```text
Architecture Drift
```

bezeichnet.

---

## Drift Detection

NovaOS soll Architecture Drift erkennen können.

Beispiel:

```text
Desired:
    Provider = Local

Actual:
    Provider = Remote
```

Dies kann eine Policy-Verletzung darstellen.

---

## Reconciliation

Das deklarative Systemmodell kann auf Grundlage der Introspection-Daten eine Reconciliation auslösen.

---

## Closed Control Loop

Dadurch entsteht:

```text
Desired State
      ↓
Planning
      ↓
Execution
      ↓
Actual State
      ↓
Introspection
      ↓
Verification
      ↓
Reconciliation
```

---

## Architecture Introspection als Feedback-System

Architecture Introspection ist damit ein zentraler Sensor des deklarativen NovaOS-Control-Loops.

---

## Normative Festlegungen

1. NovaOS MUSS Architecture Introspection als systemweites Architekturprinzip unterstützen.

2. Architecture Introspection MUSS Bestandteil der regulären Systemarchitektur sein.

3. Architecture Introspection DARF NICHT ausschließlich als Debug-Funktion betrachtet werden.

4. Relevante Runtime-Komponenten SOLLEN introspektierbar sein.

5. Die Runtime-Architektur MUSS als typisierter Graph darstellbar sein.

6. Der Architecture Graph MUSS Nodes unterstützen.

7. Der Architecture Graph MUSS typisierte Relationships unterstützen.

8. Architecture Nodes MÜSSEN stabile logische Identitäten besitzen können.

9. Speicheradressen DÜRFEN NICHT die einzige Node Identity darstellen.

10. Prozess-IDs DÜRFEN NICHT die einzige logische Identität darstellen.

11. Architecture Nodes MÜSSEN typisierbar sein.

12. Architecture Relationships MÜSSEN typisierbar sein.

13. Relationship Types DÜRFEN NICHT ausschließlich unstrukturierte Strings ohne definierte Semantik sein.

14. Hierarchische Sichten MÜSSEN aus dem Graph erzeugbar sein.

15. Capability Relationships MÜSSEN introspektierbar sein.

16. Capability Provider MÜSSEN introspektierbar sein.

17. Aktuell ausgewählte Provider MÜSSEN bestimmbar sein.

18. Provider-Versionen SOLLEN introspektierbar sein.

19. Provider-Generationen MÜSSEN introspektierbar sein, wenn Hot Replacement unterstützt wird.

20. Provider Locations MÜSSEN introspektierbar sein, soweit autorisiert.

21. Provider Trust State SOLL introspektierbar sein.

22. Provider Health SOLL introspektierbar sein.

23. Automatische Providerentscheidungen MÜSSEN erklärbar sein.

24. NovaOS MUSS strukturierte Decision Records unterstützen können.

25. Decision Records SOLLEN Kandidaten darstellen können.

26. Decision Records SOLLEN ausgewählte Optionen darstellen.

27. Decision Records SOLLEN verworfene Optionen darstellen können.

28. Decision Records SOLLEN Ablehnungsgründe darstellen können.

29. Decision Records SOLLEN relevante Hard Constraints darstellen können.

30. Decision Records SOLLEN relevante Policy-Versionen referenzieren.

31. Adaptive Entscheidungen SOLLEN relevante Modellversionen referenzieren.

32. Decision Tracing MUSS NICHT beliebige interne Reasoning-Zustände offenlegen.

33. Decision Tracing SOLL strukturierte überprüfbare Entscheidungsgründe bereitstellen.

34. Execution Contracts MÜSSEN introspektierbar sein.

35. Die Bindung zwischen Execution Scope und Execution Contract MUSS bestimmbar sein.

36. Contract Satisfaction SOLL introspektierbar sein.

37. Contract Violations MÜSSEN beobachtbar sein können.

38. Resource Economy MUSS mit Architecture Introspection integriert sein.

39. Resource Capacity SOLL introspektierbar sein.

40. Resource Reservations MÜSSEN introspektierbar sein können.

41. Resource Budgets MÜSSEN introspektierbar sein können.

42. Resource Consumption SOLL introspektierbar sein.

43. Resource Pressure MUSS introspektierbar sein.

44. Resource Throttling SOLL introspektierbar sein.

45. Ressourcen SOLLEN ihren logischen Verursachern zugeordnet werden können.

46. Shared Resource Accounting MUSS zwischen physischem Verbrauch und logischen Mappings unterscheiden können.

47. Zero-Copy-Datenpfade MÜSSEN introspektierbar sein können.

48. Physische Kopien SOLLEN messbar sein.

49. Copy Reasons SOLLEN klassifizierbar sein.

50. Data Movement SOLL introspektierbar sein.

51. Semantic Types MÜSSEN im Architecture Graph referenzierbar sein.

52. Datenobjekte SOLLEN semantisch statt ausschließlich als Bytebereiche dargestellt werden.

53. Daten- und Objekt-Pipelines MÜSSEN introspektierbar sein.

54. Pipeline Stages MÜSSEN einzeln darstellbar sein.

55. Pipeline Provider MÜSSEN darstellbar sein.

56. Pipeline Semantic Types MÜSSEN darstellbar sein.

57. Pipeline Latencies SOLLEN darstellbar sein.

58. Pipeline Copies SOLLEN darstellbar sein.

59. Pipeline Locations SOLLEN darstellbar sein.

60. Pipeline Bottlenecks SOLLEN diagnostizierbar sein.

61. Structured Concurrency MUSS mit Architecture Introspection integriert sein.

62. Execution Scope Hierarchien MÜSSEN darstellbar sein.

63. Task Ownership MUSS bestimmbar sein.

64. Cancellation Relationships SOLLEN darstellbar sein.

65. Deadline Propagation SOLL darstellbar sein.

66. Orphaned Tasks SOLLEN erkennbar sein.

67. Deterministic Mode MUSS introspektierbar sein.

68. Deterministische Provider Bindings MÜSSEN darstellbar sein.

69. Deterministische Algorithmus Bindings MÜSSEN darstellbar sein.

70. Deterministische Generation Bindings MÜSSEN darstellbar sein.

71. Determinism Violations MÜSSEN beobachtbar sein.

72. Hot Replacement MUSS mit Architecture Introspection integriert sein.

73. Aktive Generationen MÜSSEN darstellbar sein.

74. Retiring Generations MÜSSEN darstellbar sein.

75. Replacement Phases SOLLEN darstellbar sein.

76. State Migration SOLL darstellbar sein.

77. Grace Period State SOLL darstellbar sein.

78. Rollback State SOLL darstellbar sein.

79. Architecture Snapshots MÜSSEN unterstützt werden können.

80. Architecture Snapshots MÜSSEN einen definierten Konsistenzgrad besitzen.

81. Best-Effort Snapshots MÜSSEN unterstützt werden können.

82. Stärkere Snapshot-Konsistenz SOLL für Diagnose- und Recovery-Fälle unterstützt werden.

83. Architecture Diffs MÜSSEN zwischen Snapshots erzeugbar sein können.

84. Historische Architekturabfragen SOLLEN unterstützt werden, wenn History verfügbar ist.

85. Architecture Events MÜSSEN unterstützt werden können.

86. Architecture Events SOLLEN Sequence Numbers besitzen können.

87. Architecture Events SOLLEN Zeitinformationen besitzen.

88. Architecture Events SOLLEN Correlation IDs besitzen können.

89. Kausale Beziehungen SOLLEN darstellbar sein.

90. Distributed Introspection MUSS Remote Execution unterstützen können.

91. Location Transparency MUSS mit Introspection integriert sein.

92. Tatsächliche Execution Location MUSS introspektierbar bleiben.

93. Location Transparency DARF reale Fehler NICHT verstecken.

94. Remote Failure Causes MÜSSEN darstellbar sein.

95. Remote Execution Costs SOLLEN darstellbar sein, wenn verfügbar.

96. Data Sovereignty Constraints MÜSSEN introspektierbar sein.

97. Aktuelle Data Locations SOLLEN mit Sovereignty Constraints vergleichbar sein.

98. Information Flow SOLL graphbasiert darstellbar sein.

99. Security Boundaries MÜSSEN introspektierbar sein.

100. Architecture Introspection DARF Security Boundaries NICHT schwächen.

101. Capability Grants SOLLEN introspektierbar sein.

102. Capability Delegation SOLL introspektierbar sein.

103. Capability Attenuation SOLL introspektierbar sein.

104. Capability Provenance SOLL introspektierbar sein.

105. Nova.Trust MUSS mit Architecture Introspection integriert sein.

106. Trust Identity SOLL introspektierbar sein.

107. Signature State SOLL introspektierbar sein.

108. Provenance SOLL introspektierbar sein.

109. Revocation State MUSS introspektierbar sein können.

110. Provenance Graphs SOLLEN unterstützt werden.

111. Introspection MUSS capability-basiert autorisiert werden.

112. Anwendungen DÜRFEN NICHT automatisch vollständige Systemintrospection erhalten.

113. Anwendungen SOLLEN standardmäßig ihren eigenen Architecture Scope sehen.

114. Administrative Introspection MUSS separat autorisierbar sein.

115. Security-sensitive Introspection MUSS separat einschränkbar sein.

116. Introspection MUSS Privacy berücksichtigen.

117. Introspection MUSS Redaction unterstützen können.

118. Introspection SOLL Aggregation statt Detailoffenlegung unterstützen können.

119. Hochauflösende Metrics DÜRFEN zur Side-Channel-Vermeidung eingeschränkt werden.

120. Introspection Query Frequency DARF durch Policy begrenzt werden.

121. Observability und Architecture Introspection MÜSSEN miteinander integrierbar sein.

122. Architecture Introspection DARF Logs NICHT vollständig ersetzen.

123. Architecture Introspection DARF Metrics NICHT vollständig ersetzen.

124. Architecture Introspection DARF Traces NICHT vollständig ersetzen.

125. Logs SOLLEN Architecture Identities referenzieren können.

126. Metrics SOLLEN Architecture Identities referenzieren können.

127. Traces SOLLEN Architecture Identities referenzieren können.

128. Gemeinsame stabile Identitäten SOLLEN systemweit verwendet werden.

129. Telemetry SOLL strukturierte Schemas verwenden.

130. Unstrukturierte Textlogs DÜRFEN NICHT die einzige Introspection-Schnittstelle sein.

131. NovaOS MUSS eine native Introspection API bereitstellen.

132. Die Introspection API MUSS maschinenlesbar sein.

133. Die Introspection API SOLL Graph Queries unterstützen.

134. Die Introspection API SOLL Node Filter unterstützen.

135. Die Introspection API SOLL Relationship Filter unterstützen.

136. Die Introspection API SOLL Scope Filter unterstützen.

137. Die Introspection API SOLL Snapshot Queries unterstützen.

138. Die Introspection API SOLL Streaming Queries unterstützen.

139. Historische Queries SOLLEN unterstützt werden, wenn History aktiviert ist.

140. Graph Traversal SOLL unterstützt werden.

141. Introspection Queries MÜSSEN Ressourcenlimits besitzen.

142. Introspection selbst MUSS der Resource Economy unterliegen.

143. Query CPU MUSS budgetierbar sein.

144. Query Memory MUSS budgetierbar sein.

145. Event Bandwidth MUSS begrenzbar sein.

146. History Storage MUSS budgetierbar sein.

147. Sampling MUSS unterstützt werden können.

148. Event Streams MÜSSEN Backpressure unterstützen.

149. Event Loss MUSS erkennbar sein, wenn Events verworfen werden dürfen.

150. Nicht alle Introspection-Daten MÜSSEN persistent gespeichert werden.

151. History Retention MUSS policygesteuert sein.

152. Security Audit History DARF strengere Retention besitzen.

153. Nova.Resilience MUSS Architecture Introspection verwenden können.

154. Dependency Graphs MÜSSEN für Recovery analysierbar sein.

155. Failure Propagation SOLL introspektierbar sein.

156. Blast Radius SOLL bestimmbar sein.

157. Recovery Planning SOLL Architecture Introspection verwenden.

158. Hot Replacement MUSS Dependency Introspection verwenden können.

159. Stale Bindings SOLLEN erkennbar sein.

160. Generation Leaks SOLLEN erkennbar sein.

161. Resource Leaks SOLLEN erkennbar sein.

162. Buffer Leaks SOLLEN erkennbar sein.

163. Wait Relationships SOLLEN optional introspektierbar sein.

164. Deadlock Diagnostics SOLLEN durch Wait Graphs unterstützt werden.

165. Priority Inversion SOLL diagnostizierbar sein.

166. Scheduling Information SOLL introspektierbar sein.

167. Hardware Topology MUSS introspektierbar sein.

168. NUMA Topology SOLL introspektierbar sein.

169. Device Topology SOLL introspektierbar sein.

170. Resource Locality SOLL introspektierbar sein.

171. Energy Information SOLL introspektierbar sein, wenn Hardwareunterstützung existiert.

172. Thermal Information SOLL introspektierbar sein, wenn Hardwareunterstützung existiert.

173. Adaptive Entscheidungen MÜSSEN introspektierbar sein.

174. Prediction Inputs SOLLEN introspektierbar sein, soweit zulässig.

175. Prediction Outcomes SOLLEN introspektierbar sein.

176. Prediction Error SOLL introspektierbar sein.

177. Adaptive Model Versions SOLLEN referenzierbar sein.

178. Policy Versions SOLLEN referenzierbar sein.

179. Relevante Decision Inputs SOLLEN rekonstruierbar sein, soweit möglich.

180. Deterministic Mode SOLL reproduzierbare Decision Records unterstützen.

181. NovaOS SOLL technische Entscheidungen verständlich erklären können.

182. Erklärungen SOLLEN zielgruppengerecht aufbereitet werden können.

183. Benutzererklärungen DÜRFEN technische Details abstrahieren.

184. Entwicklererklärungen SOLLEN detailliertere technische Daten enthalten können.

185. Security-Erklärungen SOLLEN relevante Trust- und Sovereignty-Gründe darstellen können.

186. Strukturierte Rohdaten MÜSSEN neben generierten Erklärungen verfügbar bleiben können.

187. Architecture Introspection SOLL Grundlage für Developer Tools sein.

188. Ein Architecture Explorer SOLL implementierbar sein.

189. Ein Capability Explorer SOLL implementierbar sein.

190. Ein Pipeline Explorer SOLL implementierbar sein.

191. Ein Resource Explorer SOLL implementierbar sein.

192. Ein Trust Explorer SOLL implementierbar sein.

193. Ein Execution Explorer SOLL implementierbar sein.

194. Ein Generation Explorer SOLL implementierbar sein.

195. Introspection MUSS CLI-basiert nutzbar sein können.

196. Introspection MUSS programmatisch nutzbar sein.

197. Das primäre Introspection-Modell MUSS machine-readable-first sein.

198. Systemtools DÜRFEN NICHT auf Screen-Scraping angewiesen sein.

199. Introspection Schemas MÜSSEN versionierbar sein.

200. Introspection Schemas MÜSSEN evolvierbar sein.

201. Clients SOLLEN unbekannte optionale Felder tolerieren.

202. Schema Version Negotiation SOLL unterstützt werden.

203. Subsystem Extensions MÜSSEN möglich sein.

204. Vendor Extensions MÜSSEN namespaced sein.

205. Vendor Extensions DÜRFEN das globale Kernschema NICHT unkoordiniert erweitern.

206. Architecture Introspection DARF Fast Paths NICHT unnötig belasten.

207. Teure Daten SOLLEN lazy materialisiert werden.

208. Statische Metadaten SOLLEN cachebar sein.

209. Architekturänderungen SOLLEN event-driven erfasst werden.

210. Hochfrequente Telemetrie SOLL Low-Contention-Techniken verwenden.

211. Introspection Detail Levels SOLLEN konfigurierbar sein.

212. Debug Detail DARF Production-Systeme NICHT unkontrolliert destabilisieren.

213. Fehler im Introspection-System DÜRFEN introspektierte Komponenten NICHT automatisch zum Absturz bringen.

214. Kritische Komponenten SOLLEN minimale unabhängige Diagnosepfade besitzen können.

215. Kernel Crash Dumps SOLLEN relevante Architecture Information enthalten können.

216. Bootprozesse SOLLEN Introspection Events erzeugen können.

217. Boot Failures SOLLEN Architekturursachen referenzieren können.

218. NovaDOS SOLL reduzierte Offline-Introspection unterstützen.

219. NovaDOS MUSS NICHT den vollständigen Live Architecture Graph implementieren.

220. NovaDOS SOLL persistierte Recovery- und Generation-Informationen lesen können.

221. Persistierte Introspection-Daten MÜSSEN Security Policies respektieren.

222. Sensitive Introspection-Daten DÜRFEN NICHT ungeschützt persistiert werden.

223. Introspection History MUSS Data Retention Policies respektieren.

224. Introspection Telemetry MUSS Data Sovereignty respektieren.

225. Diagnose MUSS grundsätzlich lokal möglich sein.

226. Remote Telemetry MUSS eine separate autorisierte Entscheidung sein.

227. Architecture Snapshot Export MUSS autorisiert werden.

228. Export MUSS Redaction unterstützen können.

229. Support Bundles SOLLEN Architecture Snapshots enthalten können.

230. Support Bundles MÜSSEN sensible Informationen schützen können.

231. Architecture Fingerprints SOLLEN möglich sein.

232. Architecture Fingerprints DÜRFEN keine unnötigen personenbezogenen Daten enthalten.

233. Automated Tests SOLLEN Architecture Introspection verwenden können.

234. Architecture Assertions SOLLEN unterstützt werden können.

235. Architecture Assertions SOLLEN Security Constraints prüfen können.

236. Architecture Assertions SOLLEN Resource Constraints prüfen können.

237. Architecture Assertions SOLLEN Data Location prüfen können.

238. Architecture Assertions SOLLEN Copy Counts prüfen können.

239. Architecture Assertions SOLLEN Determinism prüfen können.

240. Continuous Verification SOLL auf Architecture Introspection aufbauen können.

241. Desired Architecture MUSS mit Actual Architecture vergleichbar sein.

242. Architecture Drift MUSS erkennbar sein können.

243. Drift Detection MUSS Reconciliation auslösen können.

244. Das deklarative Systemmodell MUSS Architecture Introspection als Feedback verwenden können.

245. Architecture Introspection MUSS Teil des Desired-State-/Actual-State-Control-Loops sein.

246. Introspection Snapshots MÜSSEN ihre Systemgeneration referenzieren können.

247. Architecture Diffs MÜSSEN Generationenwechsel darstellen können.

248. Architecture Events SOLLEN Transaktionsidentitäten referenzieren können.

249. Architecture Events SOLLEN Execution Scope Identities referenzieren können.

250. Architecture Events SOLLEN Capability Identities referenzieren können.

251. Architecture Events SOLLEN Provider Generation Identities referenzieren können.

252. Introspection MUSS logische und physische Architektur unterscheiden können.

253. Eine logische Capability DARF NICHT mit ihrem aktuellen Provider gleichgesetzt werden.

254. Ein DataObject DARF NICHT mit seiner aktuellen physischen Speicherposition gleichgesetzt werden.

255. Ein Execution Scope DARF NICHT ausschließlich mit einem Thread gleichgesetzt werden.

256. Eine Resource Reservation DARF NICHT mit tatsächlichem Verbrauch gleichgesetzt werden.

257. Trust Identity DARF NICHT allein aus einer Signatur abgeleitet werden.

258. Location Transparency DARF tatsächliche Location in privilegierter Introspection NICHT verschleiern.

259. Zero-Copy DARF physische Kopien in Introspection NICHT fälschlich als logische Transfers zählen.

260. Shared Memory MUSS physisch korrekt accountingfähig sein.

261. Architecture Introspection MUSS unterschiedliche Abstraktionsebenen unterstützen.

262. Benutzer DÜRFEN eine vereinfachte Sicht erhalten.

263. Entwickler DÜRFEN bei Autorisierung detailliertere Sichten erhalten.

264. Kernel und Recovery Services DÜRFEN privilegierte Sichten erhalten.

265. Security Policies MÜSSEN jede Sicht einschränken können.

266. Introspection MUSS auch bei teilweisem Systemausfall nützliche Informationen liefern können.

267. Introspection DARF NICHT vollständig von adaptiven KI-Systemen abhängig sein.

268. Fundamentale Introspection MUSS deterministisch und ohne KI funktionieren können.

269. KI DARF Introspection-Daten analysieren und erklären.

270. KI-generierte Diagnosen MÜSSEN von den zugrunde liegenden strukturierten Fakten unterscheidbar sein.

271. KI DARF keine nicht vorhandenen Architecture Relationships als Fakten ausgeben.

272. Self-Healing DARF auf strukturierte Introspection-Fakten reagieren.

273. Self-Healing MUSS seine Recovery-Entscheidungen wiederum introspektierbar machen.

274. Dadurch entstehende Recovery-Aktionen MÜSSEN als Architecture Events darstellbar sein.

275. Architecture Introspection MUSS rekursive Beobachtbarkeit des Control Loops ermöglichen.

276. Introspection selbst SOLL introspektierbar sein.

277. Introspection Services SOLLEN ihre eigenen Resource Budgets offenlegen können.

278. Introspection Services SOLLEN ihren eigenen Health State offenlegen können.

279. Ein Ausfall von History Storage DARF Live Introspection NICHT zwingend deaktivieren.

280. Architecture Introspection MUSS als langfristig stabiles Fundament für Diagnose, Explainability, Verification und System Evolution von NovaOS dienen.

---

## Konsequenzen

### Positive Konsequenzen

- NovaOS wird nicht zu einer undurchsichtigen adaptiven Black Box.
- Runtime-Architektur kann systemweit untersucht werden.
- Capability-Bindings werden sichtbar.
- Providerentscheidungen werden erklärbar.
- automatische Algorithmusauswahl kann nachvollzogen werden.
- Execution Contracts können überwacht werden.
- Resource Economy erhält systemweite Attribution.
- Zero-Copy-Pfade können überprüft werden.
- unnötige Kopien können gefunden werden.
- Semantic Types bleiben in Diagnosetools sichtbar.
- Datenpipelines können vollständig analysiert werden.
- Structured Concurrency wird visuell nachvollziehbar.
- verwaiste Tasks können leichter gefunden werden.
- Deterministic Mode kann verifiziert werden.
- Hot Replacement kann generationenbasiert diagnostiziert werden.
- alte Bindings und Generation Leaks können erkannt werden.
- Location Transparency bleibt technisch transparent.
- Remote-Kosten und Fehler können sichtbar bleiben.
- Data-Sovereignty-Verletzungen können erkannt werden.
- Security Boundaries werden analysierbar.
- Capability Delegation kann nachvollzogen werden.
- Nova.Trust erhält einen sichtbaren Provenance Graph.
- Self-Healing kann den Architecture Graph für Recovery verwenden.
- Failure Blast Radius kann besser bestimmt werden.
- Hardwaretopologie und Resource Locality werden analysierbar.
- adaptive Entscheidungen können anhand von Prediction Error überprüft werden.
- Entwickler erhalten ein starkes systemweites Diagnosemodell.
- Logs, Metrics und Traces können über gemeinsame Identitäten verbunden werden.
- Desired und Actual Architecture können verglichen werden.
- Architecture Drift kann automatisch erkannt werden.
- kontinuierliche Architekturverifikation wird möglich.
- Support und Fehlerdiagnose werden erheblich verbessert.
- NovaOS kann seine eigenen Entscheidungen strukturiert erklären.

### Negative Konsequenzen

- jedes relevante Subsystem benötigt Introspection-Unterstützung,
- zusätzliche Metadaten werden benötigt,
- stabile Identitäten müssen verwaltet werden,
- Architecture Events verursachen Overhead,
- History benötigt Speicher,
- Graph Queries benötigen CPU und Speicher,
- Security und Privacy des Introspection-Systems sind komplex,
- hochauflösende Telemetrie kann Side Channels erzeugen,
- Schema-Versionierung muss langfristig gepflegt werden,
- Distributed Introspection erhöht die Komplexität,
- konsistente Snapshots können teuer sein,
- Decision Records benötigen zusätzlichen Speicher,
- Entwickler müssen Introspection Schemas pflegen,
- Vendor Extensions benötigen Governance,
- vollständige Architekturvisualisierung kann bei sehr großen Systemen komplex werden.

---

## Verworfene Alternativen

### Nur klassische Prozessliste

Nicht ausreichend.

NovaOS besitzt logische Capabilities, Provider, Pipelines, Execution Contracts und Generationen, die nicht sinnvoll ausschließlich über Prozesse dargestellt werden können.

### Nur Logs

Verworfen.

Logs sind ereignisorientiert und häufig unstrukturiert.

Sie bilden keinen vollständigen Architecture Graph.

### Nur Metrics

Verworfen.

Metrics beantworten:

```text
How much?
```

aber nicht ausreichend:

```text
Why?
How connected?
Who owns it?
```

### Nur Distributed Tracing

Verworfen.

Tracing beschreibt primär Ausführungspfade, nicht die gesamte statische und dynamische Runtime-Architektur.

### Vollständiger Systemdump bei jeder Änderung

Verworfen.

Zu hoher Ressourcenverbrauch.

NovaOS verwendet Events, Snapshots und Diffs.

### Globale unbeschränkte Introspection für jede Anwendung

Verworfen.

Dies würde Security-, Privacy- und Side-Channel-Probleme erzeugen.

### Introspection ausschließlich für Administratoren

Nicht übernommen.

Anwendungen und Entwickler benötigen autorisierte scoped Introspection.

### Introspection ausschließlich über Text

Verworfen.

Das Kernmodell muss maschinenlesbar und strukturiert sein.

### KI als primäre Introspection-Schicht

Verworfen.

Fundamentale Systemfakten müssen ohne KI verfügbar sein.

KI kann strukturierte Fakten erklären und analysieren.

### Jede Entscheidung vollständig intern offenlegen

Nicht übernommen.

NovaOS liefert strukturierte Decision Reasons, ohne unnötig interne Implementierungs- oder Reasoning-Details offenzulegen.

### Vollständige permanente History

Nicht übernommen.

Retention wird durch Policies und Resource Economy kontrolliert.

### Introspection ohne Ressourcenlimits

Verworfen.

Graph Queries und Event Streams können selbst erhebliche Ressourcen verbrauchen.

### Ein globaler monolithischer Introspection-Dienst

Nicht als zwingende Implementierung übernommen.

Das semantische Modell ist systemweit einheitlich, die Implementierung darf verteilt sein.

### Introspection als passive Diagnose

Nicht übernommen.

Architecture Introspection ist gleichzeitig Feedback für:

- Reconciliation,
- Self-Healing,
- Security,
- Resource Economy,
- Continuous Verification.

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
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell`
- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-IPC-0003_Zero_Copy_IPC`

---

## Zugehörige NPSPECs

- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-INTROSPECTION-MODEL-0001`
- `NPSPEC-INTROSPECTION-GRAPH-0001`
- `NPSPEC-INTROSPECTION-NODE-0001`
- `NPSPEC-INTROSPECTION-RELATIONSHIP-0001`
- `NPSPEC-INTROSPECTION-IDENTITY-0001`
- `NPSPEC-INTROSPECTION-SCHEMA-0001`
- `NPSPEC-INTROSPECTION-API-0001`
- `NPSPEC-INTROSPECTION-QUERY-0001`
- `NPSPEC-INTROSPECTION-TRAVERSAL-0001`
- `NPSPEC-INTROSPECTION-SNAPSHOT-0001`
- `NPSPEC-INTROSPECTION-DIFF-0001`
- `NPSPEC-INTROSPECTION-EVENT-0001`
- `NPSPEC-INTROSPECTION-HISTORY-0001`
- `NPSPEC-INTROSPECTION-STREAM-0001`
- `NPSPEC-INTROSPECTION-SECURITY-0001`
- `NPSPEC-INTROSPECTION-PRIVACY-0001`
- `NPSPEC-INTROSPECTION-REDACTION-0001`
- `NPSPEC-INTROSPECTION-RESOURCE-0001`
- `NPSPEC-INTROSPECTION-CAPABILITY-0001`
- `NPSPEC-INTROSPECTION-PROVIDER-0001`
- `NPSPEC-INTROSPECTION-EXECUTION-0001`
- `NPSPEC-INTROSPECTION-CONTRACT-0001`
- `NPSPEC-INTROSPECTION-PIPELINE-0001`
- `NPSPEC-INTROSPECTION-DATA-0001`
- `NPSPEC-INTROSPECTION-SEMANTICTYPE-0001`
- `NPSPEC-INTROSPECTION-ZEROCOPY-0001`
- `NPSPEC-INTROSPECTION-DATAMOVEMENT-0001`
- `NPSPEC-INTROSPECTION-STRUCTURED-CONCURRENCY-0001`
- `NPSPEC-INTROSPECTION-DETERMINISM-0001`
- `NPSPEC-INTROSPECTION-HOTREPLACE-0001`
- `NPSPEC-INTROSPECTION-GENERATION-0001`
- `NPSPEC-INTROSPECTION-LOCATION-0001`
- `NPSPEC-INTROSPECTION-SOVEREIGNTY-0001`
- `NPSPEC-INTROSPECTION-INFORMATIONFLOW-0001`
- `NPSPEC-INTROSPECTION-TRUST-0001`
- `NPSPEC-INTROSPECTION-PROVENANCE-0001`
- `NPSPEC-INTROSPECTION-DEPENDENCY-0001`
- `NPSPEC-INTROSPECTION-FAILURE-0001`
- `NPSPEC-INTROSPECTION-RECOVERY-0001`
- `NPSPEC-INTROSPECTION-HARDWARE-0001`
- `NPSPEC-INTROSPECTION-SCHEDULER-0001`
- `NPSPEC-INTROSPECTION-ADAPTIVE-0001`
- `NPSPEC-INTROSPECTION-PREDICTION-0001`
- `NPSPEC-INTROSPECTION-DECISION-0001`
- `NPSPEC-INTROSPECTION-EXPLAINABILITY-0001`
- `NPSPEC-INTROSPECTION-TELEMETRY-0001`
- `NPSPEC-INTROSPECTION-OBSERVABILITY-0001`
- `NPSPEC-INTROSPECTION-ASSERTION-0001`
- `NPSPEC-INTROSPECTION-DRIFT-0001`
- `NPSPEC-INTROSPECTION-EXPORT-0001`
- `NPSPEC-INTROSPECTION-SUPPORTBUNDLE-0001`
- `NPSPEC-INTROSPECTION-BOOT-0001`
- `NPSPEC-INTROSPECTION-RECOVERY-OFFLINE-0001`
- `NPSPEC-INTROSPECTION-TESTING-0001`
- `NPSPEC-DECISION-TRACE-0001`
- `NPSPEC-ARCHITECTURE-SNAPSHOT-0001`
- `NPSPEC-ARCHITECTURE-DIFF-0001`
- `NPSPEC-ARCHITECTURE-DRIFT-0001`
- `NPSPEC-ARCHITECTURE-ASSERTION-0001`
- `NPSPEC-OBSERVABILITY-IDENTITY-0001`
- `NPSPEC-OBSERVABILITY-DATAMOVEMENT-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`

---

## Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch Graph-, Security-, Performance-, Consistency-, Decision-, Resource-, Distributed-, Failure- und Recovery-Tests überprüft werden.

### Node-Test

Alle relevanten Architecture Nodes müssen mit stabilen Identitäten darstellbar sein.

### Relationship-Test

Typisierte Relationships müssen korrekt zwischen Nodes dargestellt werden.

### Graph-Traversal-Test

Eine Abfrage:

```text
Application
    → Capability
    → Provider
    → Device
```

muss korrekt traversierbar sein.

### Capability-Test

Alle von einer Anwendung verwendeten Capabilities müssen innerhalb ihres autorisierten Scopes darstellbar sein.

### Provider-Test

Aktive und verfügbare Provider müssen unterscheidbar sein.

### Provider-Selection-Test

Automatische Provider Selection muss einen strukturierten Decision Record erzeugen können.

### Algorithm-Selection-Test

Automatische Algorithmusauswahl muss den ausgewählten Algorithmus und relevante Auswahlgründe darstellen können.

### Execution-Contract-Test

Execution Contracts müssen mit den zugehörigen Execution Scopes verknüpft sein.

### Contract-Violation-Test

Eine absichtlich erzeugte Contract Violation muss als Event sichtbar sein.

### Resource-Attribution-Test

Eine Ressourcenallokation muss zum verursachenden Architecture Object zurückverfolgt werden können.

### Shared-Memory-Test

Ein physisch geteilter Buffer darf nicht mehrfach als physischer Speicherverbrauch gezählt werden.

### Zero-Copy-Test

Eine bekannte Zero-Copy-Pipeline muss:

```text
Physical Copies = 0
```

anzeigen.

### Copy-Reason-Test

Eine absichtlich erforderliche Formatkonvertierung muss als:

```text
FormatConversion
```

klassifiziert werden können.

### Data-Movement-Test

Transfers zwischen:

```text
Storage
RAM
GPU
```

müssen darstellbar sein.

### Semantic-Type-Test

Ein Buffer mit:

```text
Image<RGBA8>
```

muss semantisch entsprechend dargestellt werden.

### Pipeline-Test

Alle Pipeline Stages müssen mit Input-/Output-Typen und Providern darstellbar sein.

### Pipeline-Bottleneck-Test

Ein künstlich langsamer Pipeline Stage muss diagnostizierbar sein.

### Structured-Concurrency-Test

Task Hierarchien müssen korrekt dargestellt werden.

### Cancellation-Test

Cancellation Propagation muss im Graph nachvollziehbar sein.

### Orphan-Test

Ein absichtlich erzeugter unstrukturierter Task muss als möglicher Orphan erkennbar sein.

### Deterministic-Test

Ein deterministischer Scope muss:

- Provider,
- Algorithmus,
- Generation,
- CPU Set

korrekt anzeigen.

### Determinism-Violation-Test

Eine unerlaubte Änderung innerhalb eines deterministischen Scopes muss erkannt werden.

### Hot-Replacement-Test

Während Replacement müssen alte und neue Generation gleichzeitig korrekt dargestellt werden.

### Generation-Leak-Test

Eine alte Generation mit verbleibender Referenz muss die blockierende Referenz anzeigen können.

### Architecture-Snapshot-Test

Ein Snapshot muss einen definierten Architecture State liefern.

### Snapshot-Consistency-Test

Unterschiedliche Konsistenzmodi müssen ihre zugesicherten Eigenschaften erfüllen.

### Architecture-Diff-Test

Nach einem Providerwechsel muss ein Diff beispielsweise anzeigen:

```text
- Generation 41
+ Generation 42
```

### Event-Test

Architecture Changes müssen passende Events erzeugen.

### Event-Ordering-Test

Sequence Numbers müssen Event-Reihenfolgen rekonstruierbar machen.

### Correlation-Test

Zusammengehörige Events müssen über Correlation Identity verknüpfbar sein.

### Location-Test

Lokale und Remote Provider müssen unterscheidbar sein.

### Remote-Failure-Test

Ein Remote Failure muss als Remote Failure sichtbar bleiben.

### Sovereignty-Test

Ein DataObject mit:

```text
LocalOnly
```

muss auf unzulässige Remote Location geprüft werden können.

### Information-Flow-Test

Autorisierte Informationsflüsse müssen dargestellt werden können.

### Security-Boundary-Test

Isolation Domains müssen korrekt sichtbar sein.

### Capability-Delegation-Test

Eine Delegationskette muss korrekt dargestellt werden.

### Capability-Attenuation-Test

Reduzierte Rechte müssen korrekt dargestellt werden.

### Trust-Test

Trust State eines Providers muss korrekt angezeigt werden.

### Provenance-Test

Runtime Generation muss zu ihrer Provenance zurückverfolgt werden können.

### Revocation-Test

Revocation muss den Trust State im Graph aktualisieren.

### Authorization-Test

Eine normale Anwendung darf keine unautorisierten Systemdetails lesen.

### Scoped-Introspection-Test

Eine Anwendung darf ihren eigenen Scope korrekt introspektieren.

### Redaction-Test

Sensitive Informationen müssen abhängig von Policy redigiert werden.

### Side-Channel-Test

Nicht privilegierte Anwendungen dürfen keine verbotenen hochauflösenden fremden Ressourcenmetriken erhalten.

### Query-Budget-Test

Eine extrem komplexe Query muss durch Resource Limits begrenzbar sein.

### Backpressure-Test

Ein langsamer Event Consumer muss Backpressure auslösen.

### Event-Loss-Test

Wenn Events verworfen werden, muss der Verlust erkennbar sein.

### History-Test

Retention Policies müssen alte Introspection-Daten korrekt entfernen.

### Self-Healing-Test

Nova.Resilience muss einen Failure Graph analysieren können.

### Blast-Radius-Test

Ein Device Failure muss zu den betroffenen Providern und Capabilities zurückverfolgt werden können.

### Resource-Leak-Test

Eine Allocation ohne gültigen Owner muss diagnostizierbar sein.

### Buffer-Leak-Test

Ein Shared Buffer mit abgelaufenem Lifetime Owner muss erkannt werden können.

### Deadlock-Test

Ein zyklischer Wait Graph muss erkannt werden können.

### Hardware-Topology-Test

CPU-, NUMA-, PCIe- und Device-Beziehungen müssen korrekt dargestellt werden.

### Resource-Locality-Test

Remote NUMA Memory Access muss als unterschiedliche Task-/Buffer-Location sichtbar sein.

### Adaptive-Decision-Test

Eine adaptive Entscheidung muss Prediction, Outcome und Prediction Error referenzieren können.

### Decision-Reconstruction-Test

Relevante Decision Inputs müssen für Diagnose verfügbar sein.

### Explainability-Test

Aus einem strukturierten Decision Record muss eine verständliche Erklärung erzeugt werden können.

### AI-Independence-Test

Fundamentale Architecture Introspection muss auch bei deaktivierter KI vollständig funktionieren.

### False-Relationship-Test

Ein Analyse- oder KI-System darf keine nicht existierende Relationship als bestätigte Architekturbeziehung speichern.

### CLI-Test

CLI-Tools müssen Architecture Queries durchführen können.

### API-Test

Programmatic Clients müssen Architecture Queries ausführen können.

### Schema-Version-Test

Clients müssen kompatible Schema-Versionen aushandeln können.

### Unknown-Field-Test

Ein Client muss unbekannte optionale Felder ignorieren können.

### Vendor-Extension-Test

Namespaced Vendor Extensions dürfen das Kernschema nicht beschädigen.

### Performance-Test

Aktivierte Standard-Introspection darf Fast Paths nicht unvertretbar verlangsamen.

### Stress-Test

Zu testen sind:

- Millionen Architecture Nodes,
- große Relationship Graphs,
- viele parallele Queries,
- hohe Event-Raten,
- zahlreiche Execution Scopes,
- viele Capability Bindings,
- viele Shared Buffers,
- häufige Hot Replacements.

### Failure-Independence-Test

Ein Fehler im Introspection-Service darf die beobachteten Anwendungen nicht automatisch zum Absturz bringen.

### Crash-Dump-Test

Ein Crash Dump muss relevante Architecture References enthalten können.

### Boot-Test

Boot Architecture Events müssen die Startsequenz darstellen können.

### NovaDOS-Test

NovaDOS muss persistierte Recovery- und Generation-Informationen offline lesen können.

### Export-Test

Architecture Snapshot Export muss Security und Redaction respektieren.

### Support-Bundle-Test

Ein Support Bundle muss sensible Informationen entsprechend Policy entfernen können.

### Architecture-Assertion-Test

Eine Assertion:

```text
Data.Location == Local
```

muss gegen den Actual Architecture Graph geprüft werden können.

### Zero-Copy-Assertion-Test

Eine Assertion:

```text
Pipeline.PhysicalCopies == 0
```

muss überprüfbar sein.

### Determinism-Assertion-Test

Eine Assertion gegen Provider- oder Generation-Wechsel muss überprüfbar sein.

### Drift-Test

Eine Abweichung zwischen Desired und Actual Architecture muss erkannt werden.

### Reconciliation-Test

Erkannter Architecture Drift muss eine kontrollierte Reconciliation auslösen können.

### Long-Running-Test

Architecture Introspection muss über lange Laufzeiten funktionieren, ohne:

- Identity Leaks,
- Node Leaks,
- Relationship Leaks,
- unkontrolliertes History Growth,
- Event Queue Growth,
- stale Architecture State

zu erzeugen.

---

## Ergebnis

Mit `ADR-ARCH-0013` wird NovaOS zu einem System, das seine eigene laufende Architektur als explizites, maschinenlesbares Modell kennt.

Das grundlegende Prinzip lautet:

```text
Runtime Architecture
        ↓
Architecture Introspection
        ↓
Structured Architecture Graph
        ↓
Observation
        +
Explanation
        +
Verification
        +
Recovery
        +
Evolution
```

Damit kann NovaOS nicht nur feststellen:

```text
CPU usage = 42 %
```

sondern beantworten:

```text
Warum beträgt die CPU-Auslastung 42 %?

Welche Anwendung verursacht sie?

Welche Capability wurde aufgerufen?

Welcher Provider wurde gewählt?

Welcher Algorithmus läuft?

Welcher Execution Contract gilt?

Welche Ressourcen wurden reserviert?

Welche Daten werden verarbeitet?

Wo befinden sich diese Daten?

Welche Security- und Trust-Regeln gelten?

Warum wurde diese Architektur gewählt?
```

Architecture Introspection verbindet damit die zuvor definierten Architekturprinzipien:

```text
Declarative System Model
        │
        ├── Transactional Operations
        ├── Resource Economy
        ├── Zero-Copy
        ├── Location Transparency
        ├── Semantic Types
        ├── Data/Object Pipelines
        ├── Structured Concurrency
        ├── Deterministic Mode
        └── Hot Replacement
        │
        ▼
Architecture Introspection
```

Gleichzeitig bildet sie den Feedback-Kanal für das deklarative Systemmodell:

```text
Desired Architecture
        ↓
Planner
        ↓
Execution
        ↓
Actual Architecture
        ↓
Architecture Introspection
        ↓
Verification
        ↓
Architecture Diff
        ↓
Reconciliation
```

Damit entsteht ein geschlossener Architekturregelkreis:

```text
Describe
   ↓
Plan
   ↓
Execute
   ↓
Observe
   ↓
Explain
   ↓
Verify
   ↓
Adapt
```

NovaOS soll dadurch auch bei zunehmender Adaptivität und dynamischer Providerwahl nachvollziehbar bleiben.

Das abschließende Architekturprinzip lautet:

```text
No invisible architecture.
No unexplained system decision.
No adaptation without observability.
```

beziehungsweise in kompakter Form:

```text
NovaOS must be able
to inspect the architecture
it creates.
```