# ADR-ARCH-0009 – Einheitliches Daten- und Objekt-Pipelining

## Status

Angenommen

## Kategorie

Systemarchitektur / Datenmodell / Objektmodell / Pipelines / Capabilities / Execution

## Kontext

Klassische Betriebssysteme besitzen häufig mehrere voneinander getrennte Modelle für den Transport und die Verarbeitung von Daten.

Typische Mechanismen sind:

- Dateien
- Streams
- Pipes
- Sockets
- Shared Memory
- IPC-Nachrichten
- Datenbankobjekte
- GPU-Buffer
- Audio-Buffer
- Video-Frames
- Tensoren
- Gerätebuffer
- Netzwerkpakete
- temporäre Dateien
- anwendungsspezifische Objektmodelle

Diese Mechanismen entstanden historisch für unterschiedliche Anwendungsfälle und besitzen deshalb häufig unterschiedliche:

- APIs,
- Ownership-Modelle,
- Synchronisationsmodelle,
- Fehlersemantiken,
- Metadatenmodelle,
- Sicherheitsmodelle,
- Lebenszyklen,
- Typinformationen.

Eine Anwendung, die beispielsweise ein Kamerabild:

1. empfängt,
2. dekodiert,
3. transformiert,
4. durch ein KI-Modell verarbeitet,
5. annotiert,
6. auf dem Bildschirm darstellt,
7. optional speichert,

muss in klassischen Systemen häufig mehrere inkompatible Datenmodelle miteinander verbinden.

Ein möglicher traditioneller Datenpfad lautet:

```text
Camera Driver
    ↓
Kernel Buffer
    ↓ copy
Application Buffer
    ↓ copy
Decoder Buffer
    ↓ copy
GPU Upload
    ↓
GPU Texture
    ↓ readback
CPU Buffer
    ↓
AI Runtime Tensor
    ↓
Result Object
    ↓
Application Object
    ↓
Renderer
```

Dabei entstehen:

- unnötige Kopien,
- unnötige Serialisierung,
- Formatkonvertierungen,
- Speicherallokationen,
- API-Brücken,
- Kontextwechsel,
- Synchronisationspunkte.

Zusätzlich geht häufig semantische Information verloren.

Ein:

```text
Image<RGBA8>
```

wird beispielsweise zwischen Komponenten nur noch als:

```text
void*
```

oder:

```text
byte[]
```

behandelt.

NovaOS besitzt bereits beziehungsweise plant:

- Semantic Types,
- Capabilities,
- Execution Contracts,
- Zero-Copy,
- Location Transparency,
- Resource Economy,
- Structured Concurrency,
- Data Sovereignty,
- Trust,
- transaktionale Systemoperationen,
- Architecture Introspection.

Diese Architekturkonzepte entfalten ihren größten Nutzen, wenn Daten nicht an jeder Systemgrenze in ein anderes Modell übersetzt werden müssen.

NovaOS benötigt deshalb ein gemeinsames systemweites Modell für Daten- und Objektflüsse.

## Entscheidung

NovaOS verwendet ein **einheitliches Daten- und Objekt-Pipelining als systemweites Architekturprinzip**.

Daten sollen zwischen kompatiblen Systemkomponenten, Capabilities und Providern möglichst als semantisch typisierte Objekte durch Pipelines fließen können.

Das grundlegende Modell lautet:

```text
Source
   │
   ▼
Typed Object
   │
   ▼
Capability
   │
   ▼
Typed Object
   │
   ▼
Capability
   │
   ▼
Typed Object
   │
   ▼
Sink
```

Eine Pipeline beschreibt damit primär:

```text
Object<T>
    ↓
Capability<A>
    ↓
Object<U>
    ↓
Capability<B>
    ↓
Object<V>
```

und nicht zwingend:

```text
File
→ Buffer
→ Byte Stream
→ Temporary File
→ Buffer
→ Application Object
```

Das Pipeline-System verbindet:

```text
Semantic Types
Capabilities
Execution Contracts
Object Ownership
Zero-Copy
Location Transparency
Resource Economy
Structured Concurrency
Security
Trust
Data Sovereignty
Observability
```

zu einem gemeinsamen Datenflussmodell.

## Architekturprinzip

Das zentrale Prinzip lautet:

```text
Data should flow by meaning,
not by accidental representation.
```

Oder technisch:

```text
Semantic Object
      │
      ▼
Capability Contract
      │
      ▼
Execution Planning
      │
      ▼
Provider
      │
      ▼
Semantic Object
```

Die physische Repräsentation wird nur dort geändert, wo dies tatsächlich notwendig ist.

## Einheitliches Objektmodell

NovaOS verwendet ein gemeinsames abstraktes Objektmodell für Daten, die durch systemweite Pipelines verarbeitet werden.

Ein Pipeline-Objekt kann konzeptionell folgende Eigenschaften besitzen:

```text
NovaObject {
    Identity
    SemanticType
    Representation
    Metadata
    Payload
    Ownership
    Capabilities
    Location
    Provenance
    Trust
    SecurityLabels
    Sovereignty
    Lifecycle
    Version
}
```

Nicht jedes Objekt muss alle Eigenschaften physisch enthalten.

Das Modell beschreibt die logisch verfügbaren Eigenschaften.

## Objektidentität

Ein NovaOS-Objekt KANN eine stabile Objektidentität besitzen.

Beispiel:

```text
ObjectId = 8d2e...
```

Objektidentität und Speicheradresse sind nicht identisch.

Ein Objekt kann:

- verschoben,
- repliziert,
- serialisiert,
- remote verarbeitet,
- in eine andere Representation überführt

werden, ohne dass seine logische Identität zwingend verloren geht.

## Value Objects

Nicht jedes Objekt benötigt eine dauerhafte Identität.

Kleine immutable Werte können als Value Objects behandelt werden.

Beispiele:

```text
Temperature<Celsius>
Duration<Millisecond>
Probability
Coordinate<WGS84>
```

Für solche Werte kann semantische Gleichheit wichtiger sein als Objektidentität.

## Entity Objects

Objekte mit eigenständigem Lebenszyklus können eine stabile Identity besitzen.

Beispiele:

```text
Document
ImageAsset
Device
Model
Dataset
Session
```

## Payload

Der Payload enthält die eigentlichen Daten.

Beispiele:

```text
CPU Memory
Shared Memory
GPU Memory
NPU Memory
Device Memory
Persistent Storage
Remote Object
Stream
```

Der Payload muss nicht zwingend im Adressraum des Consumers liegen.

## Object Descriptor

Pipeline-Komponenten können mit einem Object Descriptor arbeiten.

Beispiel:

```text
ObjectDescriptor {
    objectId      = ...
    semanticType  = Image<RGBA8>
    representation = RGBA8
    dimensions    = 3840x2160
    location      = GPU0.Memory
    size          = 33177600
}
```

Der Descriptor kann wesentlich kleiner als der Payload sein.

## Payload Handle

Große Daten werden bevorzugt über Handles beziehungsweise Capabilities referenziert.

Beispiel:

```text
ObjectDescriptor {
    semanticType = VideoFrame<NV12>
    payload      = Capability<Buffer42>
}
```

Damit kann ein Objekt durch IPC übertragen werden, ohne seinen Payload zu kopieren.

## Semantic Types

Pipeline-Objekte MÜSSEN Semantic Types verwenden können.

Beispiel:

```text
Image<RGBA8>
```

statt ausschließlich:

```text
byte[]
```

Semantic Types bilden die primäre Kompatibilitätsebene zwischen Pipeline-Stufen.

## Representation

Semantic Type und Representation bleiben getrennt.

Beispiel:

```text
Semantic Type:
    Image

Representation:
    RGBA8
```

oder:

```text
Semantic Type:
    Image

Representation:
    NV12
```

Dadurch kann der Planner entscheiden, ob eine direkte Übergabe oder Konvertierung erforderlich ist.

## Metadata

Objekte können typisierte Metadaten besitzen.

Beispiel:

```text
Image {
    width       = 3840
    height      = 2160
    colorSpace  = sRGB
    orientation = Landscape
}
```

Metadaten sollen nicht unnötig vom Payload getrennt und wieder rekonstruiert werden.

## Provenance

Objekte können ihre Herkunft dokumentieren.

Beispiel:

```text
Camera
    ↓
Decoder
    ↓
ImageFilter
    ↓
AIEnhancer
```

Provenance kann nachvollziehbar machen, welche Komponenten ein Objekt erzeugt oder verändert haben.

## Trust

Trust-Informationen können an Objekte gekoppelt sein.

Beispiel:

```text
Object {
    semanticType = Document
    provenance   = SignedSource
    trust        = Verified
}
```

Pipeline-Operationen können Trust-Anforderungen besitzen.

## Security Labels

Security- und Information-Flow-Labels werden über Pipeline-Grenzen erhalten.

Beispiel:

```text
MedicalImage<CT>
+
SecurityLabel = PatientConfidential
```

Eine Capability darf diese Daten nur erhalten, wenn ihre Autorisierung und Policy dies erlauben.

## Data Sovereignty

Sovereignty-Informationen begleiten Objekte durch die Pipeline.

Beispiel:

```text
DataSovereignty {
    location = GermanyOnly
}
```

Ein Planner darf dann keinen Remote Provider außerhalb des erlaubten Bereichs auswählen.

## Immutable-by-default

Pipeline-Objekte SOLLEN bevorzugt immutable oder logisch immutable behandelt werden.

Das bedeutet:

```text
Input Object
    ↓ Transform
Output Object
```

anstatt:

```text
Input Object
    ↓ arbitrary mutation
same object
```

Dies vereinfacht:

- Parallelität,
- Caching,
- Provenance,
- Rollback,
- Reproducibility,
- Zero-Copy Sharing.

## Mutable Objects

Mutable Objects bleiben erlaubt.

Sie benötigen jedoch klar definierte:

- Ownership,
- Synchronisation,
- Write Rights,
- Versionierung.

## Views

NovaOS SOLL Views auf bestehende Objekte unterstützen.

Beispiel:

```text
Image<RGBA8>
    │
    ├── Full View
    ├── Region View
    └── Channel View
```

Ein View besitzt keinen zwingend eigenen Payload.

## Zero-Copy Views

Views können denselben zugrunde liegenden Buffer referenzieren.

Beispiel:

```text
Tensor
    ↓
Slice
    ↓
TensorView
```

ohne Datenkopie.

## Typed Views

Views müssen ihren Semantic Type beziehungsweise ihre semantische Einschränkung korrekt beschreiben.

Ein View darf nicht so typisiert werden, dass dadurch ungültige Annahmen entstehen.

## Object Graph

Objekte können andere Objekte referenzieren.

Beispiel:

```text
Document
├── Text
├── Image
├── Table
└── Metadata
```

Dadurch entsteht ein semantischer Object Graph.

## Pipeline Graph

Pipelines müssen nicht linear sein.

Beispiel:

```text
                 ┌──► AI Analysis
                 │
Camera ─► Decode ┼──► Preview
                 │
                 └──► Recording
```

Das Pipeline-System unterstützt deshalb Graphen.

## Directed Graph

Eine Pipeline wird grundsätzlich als gerichteter Graph modelliert.

```text
Node
Edge
Node
Edge
Node
```

Nodes repräsentieren beispielsweise:

- Sources
- Capabilities
- Converters
- Filters
- Aggregators
- Sinks.

Edges repräsentieren typisierte Objektflüsse.

## Pipeline Node

Ein Pipeline Node besitzt mindestens konzeptionell:

```text
PipelineNode {
    capability
    inputContracts
    outputContracts
    executionContract
}
```

## Pipeline Edge

Eine Pipeline Edge kann beschreiben:

```text
PipelineEdge {
    semanticType
    representation
    ownership
    buffering
    backpressure
    ordering
    security
}
```

## Source

Sources erzeugen Objekte.

Beispiele:

```text
File
Camera
Microphone
Network
Sensor
Database
Generator
Application
Device
```

## Sink

Sinks konsumieren Objekte.

Beispiele:

```text
Display
File
Network
Printer
Database
AudioDevice
Application
```

Ein Sink kann gleichzeitig Outputs erzeugen.

## Transform

Transforms wandeln ein Objekt in ein anderes um.

Beispiel:

```text
Image<RGBA8>
    ↓ Resize
Image<RGBA8>
```

oder:

```text
Audio
    ↓ SpeechRecognition
Transcript<Text>
```

## Filter

Filter verändern beziehungsweise selektieren Daten.

Beispiel:

```text
Image
    ↓ GaussianFilter
Image
```

## Converter

Converter ändern Representation oder Semantic Type.

Beispiel:

```text
Image<RGBA8>
    ↓
Image<NV12>
```

## Aggregator

Aggregator kombinieren mehrere Inputs.

Beispiel:

```text
Audio
Video
Subtitles
   │
   ▼
Mux
   │
   ▼
MediaContainer
```

## Splitter

Splitter erzeugen mehrere Outputs.

Beispiel:

```text
Video
   │
   ▼
Demux
├── VideoStream
├── AudioStream
└── SubtitleStream
```

## Join

Pipeline-Branches können synchronisiert und wieder zusammengeführt werden.

Beispiel:

```text
Image ─► Detection ─┐
                    ├──► Overlay
Image ──────────────┘
```

## Conditional Node

Pipeline Nodes dürfen bedingt aktiviert werden.

Beispiel:

```text
if HDR:
    ToneMap
else:
    PassThrough
```

Die deklarative Pipeline kann solche Bedingungen als Policy beziehungsweise Planungsbedingung ausdrücken.

## Stream Pipeline

Pipelines können kontinuierliche Streams verarbeiten.

Beispiel:

```text
Camera
   ↓
Stream<VideoFrame>
   ↓
Decoder
   ↓
Stream<Image>
   ↓
Renderer
```

## Batch Pipeline

Pipelines können Batch-Daten verarbeiten.

Beispiel:

```text
Dataset<Image>
    ↓
Resize
    ↓
Inference
    ↓
Dataset<ClassificationResult>
```

## Single Object Pipeline

Auch einzelne Objekte können dieselbe Architektur verwenden.

```text
Document
    ↓
Convert
    ↓
PDF
```

## Lazy Pipeline

Pipeline-Ausführung darf lazy erfolgen.

Eine deklarierte Pipeline muss nicht sofort ausgeführt werden.

Beispiel:

```text
Pipeline Definition
      │
      ▼
Demand
      │
      ▼
Execution
```

Dies ermöglicht Optimierungen über mehrere Stufen.

## Eager Pipeline

Eager Execution bleibt möglich, wenn:

- geringe Latenz,
- Side Effects,
- Echtzeit

dies erfordern.

## Pipeline Fusion

NovaOS SOLL benachbarte Pipeline-Stufen fusionieren können, wenn Semantik und Contracts dies erlauben.

Beispiel:

```text
Decode
    ↓
Resize
    ↓
ColorConvert
```

kann möglicherweise durch einen Provider als:

```text
DecodeResizeColorConvert
```

ausgeführt werden.

## Fusion Requirements

Pipeline Fusion darf nur erfolgen, wenn:

- Output semantisch äquivalent bleibt,
- Security Policies erfüllt bleiben,
- Determinism Requirements erfüllt bleiben,
- Provenance korrekt bleibt,
- Debugging beziehungsweise Audit nicht unzulässig verloren geht.

## Operator Fusion

Compute- und AI-Pipelines können Operator Fusion verwenden.

Beispiel:

```text
MatrixMultiply
    ↓
Bias
    ↓
Activation
```

kann als einzelner GPU/NPU-Kernel ausgeführt werden.

## Pipeline Optimization

Der Planner darf Pipelines optimieren.

Mögliche Optimierungen:

- Node Fusion
- Copy Elimination
- Conversion Elimination
- Buffer Reuse
- Provider Co-location
- Parallel Execution
- Batching
- Prefetching
- Caching
- Lazy Evaluation
- Stream Fusion
- Hardware Offload.

## Semantic Preservation

Optimierungen dürfen die deklarierte Semantik nicht verändern.

Das fundamentale Prinzip lautet:

```text
Optimization may change execution.

Optimization must not silently change meaning.
```

## Execution Contract

Jeder Pipeline Node beziehungsweise jede relevante Pipeline kann einen `Nova.ExecutionContract` besitzen.

Beispiel:

```text
ExecutionContract {
    latency      <= 16 ms
    memory       <= 256 MiB
    determinism  = Preferred
    sovereignty  = LocalOnly
    trust        >= Verified
}
```

## Pipeline Contract

Zusätzlich kann die gesamte Pipeline Anforderungen besitzen.

Beispiel:

```text
PipelineContract {
    endToEndLatency <= 33 ms
    memory          <= 512 MiB
    sovereignty     = GermanyOnly
}
```

## End-to-End Constraints

Pipeline-Planung darf nicht ausschließlich einzelne Nodes lokal optimieren.

Ein End-to-End Contract kann wichtiger sein.

Beispiel:

```text
Node A = 5 ms
Node B = 8 ms
Node C = 10 ms
```

plus:

```text
Transfers = 15 ms
```

ergibt:

```text
38 ms
```

und verletzt damit möglicherweise:

```text
EndToEndLatency <= 33 ms
```

## Constraint Propagation

Pipeline Constraints sollen durch den Graph propagiert werden können.

Beispiel:

```text
Pipeline Deadline
        ↓
Per-Node Budget
        ↓
Provider Selection
```

## Budget Distribution

Die Resource Economy darf End-to-End-Budgets auf Nodes verteilen.

Beispiel:

```text
Total Latency Budget = 16 ms

Decode      = 4 ms
Transform   = 3 ms
Inference   = 6 ms
RenderPrep  = 3 ms
```

Diese Verteilung kann dynamisch angepasst werden.

## Resource Economy

Pipeline Nodes und Edges sind Teil der systemweiten Resource Economy.

Zu berücksichtigen sind:

- CPU
- Memory
- GPU
- NPU
- I/O
- Network
- Energy
- Thermal Budget
- Latency
- Bandwidth.

## Edge Cost

Nicht nur Nodes besitzen Kosten.

Auch Datenbewegung zwischen Nodes besitzt Kosten.

Beispiel:

```text
CPU → GPU
GPU → CPU
Node A → Network → Node B
```

Der Planner muss Edge Costs berücksichtigen.

## Data Movement as Cost

Datenbewegung wird als explizite Ressourcenkostenklasse behandelt.

Beispiel:

```text
Compute Cost
+
Transfer Cost
+
Conversion Cost
+
Synchronization Cost
```

## Locality

Provider sollen bevorzugt dort gewählt werden, wo Daten bereits liegen, sofern andere Constraints dies erlauben.

Beispiel:

```text
Image located on GPU0
```

Ein GPU0-Provider kann trotz geringfügig langsamerer Compute-Performance insgesamt schneller sein als ein CPU-Provider, wenn dadurch ein großer Readback vermieden wird.

## Location Transparency

Pipeline Nodes müssen nicht grundsätzlich wissen, wo ihre Inputs physisch gespeichert sind.

Sie beschreiben:

```text
Input:
    Image<RGBA8>
```

und nicht zwingend:

```text
Input:
    GPU0Buffer<Image<RGBA8>>
```

Der Planner berücksichtigt Location.

## Visible Location Cost

Location Transparency darf Kosten nicht verstecken.

Architecture Introspection und Decision Tracing müssen beispielsweise sichtbar machen können:

```text
Transfer:
    GPU0 → CPU0

Size:
    32 MiB

Estimated Cost:
    2.1 ms
```

## Remote Pipeline

Pipeline Nodes dürfen remote ausgeführt werden.

Beispiel:

```text
Local Camera
      ↓
Local Preprocessing
      ↓
Remote AI
      ↓
Local Rendering
```

nur wenn:

- Sovereignty,
- Security,
- Trust,
- Network Budget,
- Latency

dies erlauben.

## Location Failure

Remote Nodes können ausfallen.

Pipeline-Fehler müssen Location Failure explizit modellieren.

Beispiel:

```text
RemoteProviderUnavailable
NetworkPartition
TransferTimeout
```

## Fallback

Eine Pipeline kann alternative Provider oder Pfade besitzen.

Beispiel:

```text
           ┌──► NPU
Inference ─┤
           ├──► GPU
           └──► CPU
```

## Graceful Degradation

Wenn erlaubt, kann die Pipeline degradieren.

Beispiel:

```text
4K AI Enhancement
      ↓ resource pressure
1080p AI Enhancement
      ↓
720p Basic Enhancement
```

Dies muss durch Contract oder Policy erlaubt sein.

## Zero-Copy

Zero-Copy ist ein fundamentales Pipeline-Ziel.

Wenn zwei Nodes:

- kompatiblen Semantic Type,
- kompatible Representation,
- kompatibles Layout,
- kompatible Security Rights

besitzen, soll derselbe Payload möglichst direkt weitergegeben werden.

## Zero-Copy Pipeline

Beispiel:

```text
Camera DMA Buffer
      ↓
Decoder
      ↓
GPU Surface
      ↓
Renderer
```

ohne unnötige CPU-Kopie.

## Copy-on-Write

Bei immutable Sharing kann Copy-on-Write verwendet werden.

Beispiel:

```text
Object
├── Consumer A read
├── Consumer B read
└── Consumer C write
        ↓
      private copy
```

## Buffer Reuse

Der Planner darf Buffer wiederverwenden, wenn:

- Lebenszeiten bekannt,
- Ownership geklärt,
- Security Labels kompatibel

sind.

## Buffer Pooling

Für häufig verwendete Formate dürfen Pools verwendet werden.

Beispiel:

```text
VideoFramePool<NV12>
```

Dies reduziert Allokationskosten.

## Memory Pressure

Bei Memory Pressure darf der Planner:

- Buffer Pools verkleinern,
- Caches leeren,
- Pipeline Parallelism reduzieren,
- Batching ändern,
- Spill-to-Storage verwenden,

wenn Contracts dies erlauben.

## Ownership

Jedes Pipeline-Objekt benötigt ein definiertes Ownership-Modell.

Mögliche Modelle:

```text
Exclusive
SharedRead
SharedMutable
Transferred
Borrowed
```

## Ownership Transfer

Ownership kann zwischen Pipeline-Stufen übertragen werden.

Beispiel:

```text
Producer
    │ transfer
    ▼
Consumer
```

Nach erfolgreichem Transfer darf der Producer nicht weiter unautorisiert mutieren.

## Borrowing

Temporärer Zugriff darf über Borrowing erfolgen.

Beispiel:

```text
Borrow<Image>
```

Die genaue Sprachintegration wird durch NovaLang spezifiziert.

## Lifetime

Objekt-Lifetimes müssen mit Pipeline-Lifetimes koordinierbar sein.

Ein Payload darf nicht freigegeben werden, solange noch ein gültiger Consumer darauf zugreift.

## Reference Tracking

NovaOS darf für Pipeline-Objekte geeignete Mechanismen verwenden wie:

- Reference Counting
- Ownership Tracking
- Region Lifetime
- Scope Lifetime
- Capability Lifetime.

Es wird kein einzelnes universelles Memory-Management-Verfahren vorgeschrieben.

## Capability Security

Zugriff auf Pipeline-Objekte erfolgt capability-basiert.

Ein Object Handle allein darf keine Rechte implizieren.

Beispiel:

```text
Capability<Object42, Read>
```

oder:

```text
Capability<Object42, Transform>
```

## Rights

Objektrechte können umfassen:

```text
Read
Write
Map
Share
Transfer
Transform
Serialize
Export
```

## Least Privilege

Pipeline Nodes erhalten nur die Rechte, die sie für ihre Operation benötigen.

Ein Decoder benötigt beispielsweise nicht automatisch das Recht, das ursprüngliche Objekt zu löschen.

## Capability Propagation

Capabilities dürfen nicht unkontrolliert entlang einer Pipeline weitergegeben werden.

Jeder Übergang muss Rights Propagation beziehungsweise Derivation berücksichtigen.

## Information Flow

Security Labels müssen entlang der Pipeline propagiert werden.

Beispiel:

```text
Confidential Input
      ↓
Transform
      ↓
Confidential Output
```

Eine Capability darf Labels nicht ohne Autorisierung entfernen.

## Derived Data

Aus vertraulichen Daten abgeleitete Daten können weiterhin vertraulich sein.

Beispiel:

```text
MedicalImage
      ↓ AI
DiagnosisResult
```

Das Ergebnis kann dieselbe oder eine strengere Security Classification benötigen.

## Declassification

Eine Herabstufung von Security Labels muss explizit autorisiert sein.

Beispiel:

```text
Confidential
      ↓ AuthorizedDeclassification
Public
```

## Data Sovereignty Propagation

Sovereignty Constraints müssen durch Ableitungen propagierbar sein.

Ein aus:

```text
GermanyOnly
```

erzeugtes Derivat darf nicht automatisch:

```text
Unrestricted
```

werden.

## Trust Propagation

Trust kann entlang einer Pipeline verändert werden.

Beispiel:

```text
Untrusted Input
      ↓ Verified Parser
Validated Object
```

oder:

```text
Trusted Object
      ↓ Untrusted Plugin
Output Trust = Reduced
```

Die genauen Trust-Regeln werden durch Nova.Trust definiert.

## Provenance Chain

Pipeline-Ausführung kann Provenance erzeugen.

Beispiel:

```text
Source Object
      ↓
Capability A
      ↓
Capability B
      ↓
Output Object
```

Provenance kann mindestens referenzieren:

- Input Objects
- Capability
- Provider
- Version
- Execution Time
- Trust State.

## Provenance Cost

Nicht jede Pipeline muss vollständige hochgranulare Provenance dauerhaft speichern.

Policy entscheidet über:

- Detailgrad,
- Aufbewahrung,
- Persistenz.

## Side Effects

Nicht alle Pipeline Nodes sind pure Transformations.

Beispiele:

```text
SaveFile
SendNetwork
Print
Delete
ActuateDevice
```

besitzen Side Effects.

## Side-Effect Classification

Nodes SOLLEN deklarieren:

```text
Pure
Idempotent
Transactional
ExternalSideEffect
Irreversible
```

Diese Information ist wichtig für:

- Retry,
- Recovery,
- Transactions,
- Deterministic Mode.

## Pure Nodes

Pure Nodes erzeugen bei gleichem Input und gleichem definiertem Environment denselben semantischen Output.

Sie sind besonders geeignet für:

- Caching,
- Fusion,
- Parallelisierung,
- Reexecution.

## Idempotent Nodes

Idempotente Nodes können mehrfach ausgeführt werden, ohne zusätzliche semantische Side Effects zu erzeugen.

## Non-Idempotent Nodes

Nicht-idempotente Nodes benötigen besondere Retry-Regeln.

Beispiel:

```text
SendPayment
```

darf nach Timeout nicht blind erneut ausgeführt werden.

## Transactional Nodes

Pipeline Nodes können Teil transaktionaler Systemoperationen sein.

Beispiel:

```text
Transform
    ↓
Validate
    ↓
Persist
    ↓
Activate
```

## Pipeline Transaction

Bestimmte Pipelines dürfen als Transaktion ausgeführt werden.

Beispiel:

```text
BEGIN

Read
Transform
Validate
Store

COMMIT
```

Bei Fehler:

```text
ROLLBACK
```

## Transaction Boundary

Nicht jede Pipeline muss vollständig transaktional sein.

Transaktionsgrenzen sollen explizit definierbar sein.

## External Side Effects

Externe Side Effects können nicht immer rollbackfähig sein.

Das System muss dies im Pipeline Contract berücksichtigen.

## Compensation

Für nicht rollbackfähige Operationen können Compensation Actions definiert werden.

Beispiel:

```text
ReserveResource
      ↓ failure
ReleaseResource
```

## Structured Concurrency

Pipeline-Ausführung wird mit Structured Concurrency integriert.

Alle Tasks einer Pipeline gehören zu definierten Scopes.

Beispiel:

```text
PipelineScope
├── DecodeTask
├── InferenceTask
└── RenderTask
```

## Cancellation

Cancellation propagiert durch die Pipeline.

Beispiel:

```text
User cancels operation
        ↓
Pipeline Scope Cancel
        ↓
Pending Nodes Cancel
        ↓
Resources Release
```

## Deadline Propagation

Deadlines werden durch Pipeline Nodes propagiert.

Beispiel:

```text
End-to-End Deadline = 20 ms
```

führt zu lokalen Budgets.

## Error Propagation

Fehler müssen strukturiert durch Pipeline-Scopes propagiert werden.

Beispiel:

```text
DecoderError
    ↓
Pipeline Failure
```

oder bei erlaubtem Fallback:

```text
Decoder A failed
    ↓
Decoder B
```

## Partial Failure

Graph-Pipelines können teilweise ausfallen.

Beispiel:

```text
Preview branch = success
Recording branch = failure
AI branch = success
```

Pipeline Policy bestimmt, ob:

- Gesamtpipeline fehlschlägt,
- nur Branch beendet wird,
- Fallback aktiviert wird.

## Backpressure

Streaming-Pipelines MÜSSEN Backpressure unterstützen.

Wenn ein Consumer langsamer ist als der Producer, darf das System nicht unbegrenzt Daten puffern.

## Backpressure Policies

Mögliche Strategien:

```text
Block
DropOldest
DropNewest
Sample
ReduceQuality
ThrottleProducer
SpillToStorage
```

## Policy Selection

Die Strategie muss zur Semantik passen.

Beispiel:

Bei Live-Video kann:

```text
DropOldest
```

sinnvoll sein.

Bei Finanztransaktionen wäre dies unzulässig.

## Queue Bounds

Queues SOLLEN standardmäßig bounded sein.

Unbounded Queues sind nur zulässig, wenn dies bewusst konfiguriert und ressourcenseitig abgesichert ist.

## Buffering

Edges können Buffering-Strategien besitzen.

Beispiel:

```text
buffering = 3 frames
```

oder:

```text
buffering = Adaptive
```

## Ordering

Pipelines müssen Ordering-Anforderungen ausdrücken können.

Mögliche Modelle:

```text
Strict
PerKey
BestEffort
Unordered
```

## Reordering

Parallelisierung darf Daten nur umordnen, wenn der Contract dies erlaubt.

## Sequence Numbers

Streams können Sequence Numbers verwenden, um Reihenfolge zu erhalten beziehungsweise wiederherzustellen.

## Time Semantics

Media- und Sensordaten können Timestamps besitzen.

Beispiel:

```text
VideoFrame {
    timestamp = MediaTime
}
```

Synchronisation muss Clock Domains berücksichtigen.

## Multi-Stream Synchronization

Pipelines können mehrere Streams synchronisieren.

Beispiel:

```text
Audio
Video
Subtitles
```

Synchronisation erfolgt anhand definierter Time Semantics.

## Scheduling

Pipeline Scheduling berücksichtigt:

- Dependencies
- Deadlines
- Priorities
- Resource Budgets
- Locations
- Data Availability.

## Parallelism

Unabhängige Nodes dürfen parallel ausgeführt werden.

Beispiel:

```text
           ┌──► FaceDetection
Image ─────┼──► OCR
           └──► SceneAnalysis
```

## Pipeline Parallelism

Streaming-Pipelines dürfen verschiedene Objekte gleichzeitig in unterschiedlichen Stufen verarbeiten.

Beispiel:

```text
Frame 1 → Stage C
Frame 2 → Stage B
Frame 3 → Stage A
```

## Data Parallelism

Ein Batch darf auf mehrere Provider verteilt werden.

Beispiel:

```text
Dataset
├── GPU0
├── GPU1
└── NPU0
```

wenn Determinism und Contracts dies erlauben.

## Adaptive Parallelism

Parallelism darf dynamisch an:

- System Load,
- Thermal State,
- Energy Budget,
- Deadline

angepasst werden.

Hard Constraints haben Vorrang.

## Deterministic Mode

Deterministic Mode kann Pipeline-Optimierungen einschränken.

Beispielsweise können fixiert werden:

- Provider
- Algorithm
- Node Order
- Parallelism
- Buffering
- Batch Size
- Conversion Path
- Scheduling Policy.

## Deterministic Pipeline

Eine deterministische Pipeline muss bei definierten Inputs und definiertem Systemzustand reproduzierbare Ergebnisse liefern können.

Nondeterministische Quellen müssen explizit modelliert werden.

## Randomness

Zufallsabhängige Nodes müssen ihre Randomness deklarieren.

Für Deterministic Mode kann ein definierter Seed beziehungsweise Random Source erforderlich sein.

## Caching

Pure Pipeline Nodes dürfen Outputs cachen.

Cache Keys können berücksichtigen:

```text
Input Identity/Content
Semantic Type
Capability Version
Provider Version
Parameters
Execution Contract
```

## Semantic Cache

Ein Cache darf nicht ausschließlich rohe Speicheradressen verwenden.

Die Cache-Identität muss zur Semantik des Nodes passen.

## Cache Validity

Caching ist nur zulässig, wenn der Output unter den relevanten Bedingungen wiederverwendbar ist.

## Cache Security

Caches müssen Security Labels und Tenant-/User-Isolation berücksichtigen.

Vertrauliche Ergebnisse dürfen nicht über Cache-Sharing an unautorisierte Consumer gelangen.

## Memoization

Deterministische pure Capabilities können memoisiert werden.

## Materialization

Lazy Pipeline Results können bei Bedarf materialisiert werden.

Beispiel:

```text
LazyImage
    ↓ Materialize
Image Buffer
```

## Virtual Objects

Ein Objekt kann virtuell existieren, bevor sein Payload vollständig berechnet wurde.

Beispiel:

```text
VirtualObject {
    semanticType = Image
    producer = PipelineNode42
}
```

## Demand-driven Execution

Ein Consumer kann die tatsächliche Berechnung auslösen.

Dies erlaubt:

- Lazy Evaluation
- Fusion
- Dead-Code-Elimination
- Partial Materialization.

## Dead Pipeline Elimination

Wenn ein Pipeline-Output nie konsumiert wird und keine relevanten Side Effects besitzt, darf der Planner seine Berechnung entfernen.

## Partial Materialization

Bei geeigneten Datenstrukturen darf nur der benötigte Teil berechnet werden.

Beispiel:

```text
HugeImage
    ↓
RegionView
```

muss nicht zwingend das gesamte Bild materialisieren.

## Streaming Materialization

Große Objekte dürfen inkrementell erzeugt werden.

Beispiel:

```text
Video
Dataset
Archive
```

## Files

Dateien sind eine mögliche Source beziehungsweise Sink im einheitlichen Pipeline-Modell.

Sie sind nicht das universelle Zwischenformat.

Beispiel:

```text
File<JPEG>
    ↓ Decode
Image
    ↓ Filter
Image
    ↓ Encode
File<AVIF>
```

## NovaFile

NovaFile integriert sich direkt in das Pipeline-Modell.

Ein NovaFile kann:

```text
Payload
SemanticType
Metadata
Provenance
Security
```

bereitstellen.

Dadurch muss beim Laden nicht jede Semantik neu rekonstruiert werden.

## Legacy Files

Legacy-Dateien werden über Decoder-/Parser-Capabilities in Semantic Objects überführt.

Beispiel:

```text
JPEG File
    ↓ JPEG Decoder
Image<RGB8>
```

## Storage Pipeline

Persistierung kann als Pipeline dargestellt werden.

```text
Object
    ↓ Serialize
    ↓ Compress
    ↓ Encrypt
    ↓ Store
```

Der Planner kann Schritte optimieren, wenn Contracts dies erlauben.

## Network Pipeline

Netzwerkübertragung kann ebenfalls Teil der Pipeline sein.

```text
Object
    ↓ Serialize
    ↓ Compress
    ↓ Encrypt
    ↓ Transport
    ↓ Decrypt
    ↓ Deserialize
    ↓ Object
```

## Transparent Optimization

Wenn beide NovaOS-Nodes denselben Typ und ein kompatibles Protokoll unterstützen, dürfen unnötige Zwischenrepräsentationen entfallen.

## Serialization Avoidance

Lokale Pipeline-Stufen sollen nicht serialisieren, wenn ein Shared-Object- beziehungsweise Shared-Buffer-Pfad möglich ist.

## Remote Serialization

Remote Transport kann Serialisierung benötigen.

Der Planner darf geeignete Representation und Codec auswählen.

## Compression Decision

Kompression darf automatisch gewählt werden, wenn:

```text
CompressionCost
<
TransferSavings
```

und Contract/Policy dies erlauben.

## Hardware Pipelines

Geräte sollen in dasselbe logische Pipeline-Modell integrierbar sein.

Beispiel:

```text
Camera
    ↓ DMA
GPU
    ↓
Display
```

oder:

```text
NVMe
    ↓ DMA
GPU
    ↓ Compute
```

wenn Hardware und Security dies erlauben.

## DMA

DMA wird als möglicher Payload-Transfermechanismus behandelt.

DMA-Zugriffe müssen durch IOMMU beziehungsweise entsprechende Plattformmechanismen abgesichert werden.

## GPU

GPU-Objekte bleiben semantisch typisierte NovaOS-Objekte.

Beispiel:

```text
Image<RGBA8>
Location = GPU0
```

statt ausschließlich:

```text
VkImage
```

oder einem vendor-spezifischen Handle auf Systemarchitekturebene.

Native Provider können intern weiterhin solche APIs verwenden.

## NPU

Dasselbe gilt für NPU-Objekte.

Beispiel:

```text
Tensor<Float16>
Location = NPU0
```

## Device-neutral Semantics

Hardware-spezifische Handles dürfen die öffentliche semantische Systemarchitektur nicht dominieren.

## Provider Abstraction

Pipeline Nodes referenzieren primär Capabilities.

Beispiel:

```text
Image.Resize
```

nicht zwingend:

```text
GPUVendorXResizeKernel42
```

Der Provider wird separat aufgelöst.

## Automatic Provider Selection

Der Planner darf für jeden Node automatisch einen Provider auswählen.

Beispiel:

```text
Resize
├── CPU
├── GPU
└── NPU
```

Die Entscheidung berücksichtigt den gesamten Pipeline-Graphen.

## Global versus Local Optimization

Die lokal schnellste Node-Ausführung ist nicht zwingend global optimal.

Beispiel:

```text
CPU Resize = 1 ms
GPU Resize = 2 ms
```

Wenn das Bild bereits auf GPU liegt, können zusätzliche Transfers CPU insgesamt langsamer machen.

Der Pipeline Planner SOLL deshalb nach Möglichkeit End-to-End-Kosten berücksichtigen.

## Algorithm Selection

Auch Algorithmen können pipelinebezogen ausgewählt werden.

Beispiel:

```text
Resize
├── Nearest
├── Bilinear
├── Bicubic
└── Lanczos
```

Semantic Requirements und Quality Contracts bestimmen gültige Kandidaten.

## Explicit Algorithm Override

Entwickler können einen Algorithmus explizit erzwingen.

Beispiel:

```text
Resize(
    image,
    algorithm = Lanczos
)
```

Höhere Hard Constraints bleiben bindend.

## Pipeline Definition

Pipelines sollen deklarativ beschreibbar sein.

Konzeptionelles Beispiel:

```text
pipeline PhotoEnhancement {

    input Image

    Decode
    AutoEnhance
    Denoise
    Sharpen

    output Image
}
```

Die endgültige NovaLang-Syntax wird separat definiert.

## Typed Pipeline Definition

Eine präzisere Form kann Input- und Outputtypen enthalten.

```text
pipeline Detection {

    input:
        Image<RGB8>

    node preprocess:
        Image.Normalize

    node inference:
        AI.ObjectDetection

    output:
        DetectionResult
}
```

## Pipeline Parameters

Nodes können typisierte Parameter besitzen.

Beispiel:

```text
GaussianBlur {
    radius = Length<Pixel>(5)
}
```

## Pipeline Templates

Wiederverwendbare Pipeline-Templates dürfen definiert werden.

Beispiel:

```text
ImageProcessingPipeline<TInput, TOutput>
```

## Dynamic Pipelines

Pipelines dürfen zur Laufzeit zusammengesetzt werden.

Dies ist insbesondere relevant für das NovaOS-Capability-Modell.

Beispiel:

```text
Loaded Capabilities
        ↓
Required Goal
        ↓
Pipeline Construction
```

## Capability Composition

Capabilities können automatisch zusammengesetzt werden, wenn Output- und Inputtypen kompatibel sind.

Beispiel:

```text
Capability A:
    Image → TextRegions

Capability B:
    TextRegions → Text

Capability C:
    Text → Translation
```

daraus:

```text
Image
    ↓ OCR Detection
TextRegions
    ↓ Recognition
Text
    ↓ Translation
TranslatedText
```

## Goal-based Pipeline Construction

NovaOS DARF Pipelines aus einem Ziel konstruieren.

Beispiel:

```text
Input:
    Audio

Goal:
    GermanText
```

möglicher Plan:

```text
Audio
    ↓ SpeechRecognition
EnglishText
    ↓ Translation
GermanText
```

## Planning Constraints

Automatische Pipeline Construction muss berücksichtigen:

- Semantic Compatibility
- Security
- Trust
- Sovereignty
- Resources
- Latency
- Determinism
- Quality
- Location
- Conversion Loss.

## Search Space

Automatische Pipeline Construction kann einen großen Suchraum erzeugen.

Der Planner muss deshalb:

- Heuristics,
- Cost Models,
- Cached Plans,
- Bounded Search

verwenden können.

## AI-assisted Planning

KI darf bei komplexer Pipeline-Planung unterstützen.

Sie darf jedoch nicht allein die Einhaltung harter:

- Security,
- Safety,
- Sovereignty,
- Correctness

Constraints garantieren.

## Declarative System Model

Pipelines können Teil des Desired State sein.

Beispiel:

```text
DesiredState {
    CameraPipeline = Running
}
```

Der Reconciler stellt die benötigten Nodes und Verbindungen her.

## Pipeline Actual State

Actual State kann enthalten:

```text
Pipeline {
    state = Running
    activeProviders = [...]
    latency = 12 ms
    droppedFrames = 0
}
```

## Reconciliation

Wenn ein Provider ausfällt, kann der Reconciler einen alternativen Plan herstellen.

Beispiel:

```text
Desired:
    VideoPipeline = Running

Actual:
    GPU Provider Failed

Action:
    Rebind to CPU Provider
```

wenn Contracts dies erlauben.

## Hot Replacement

Pipeline Nodes sollen hot replaceable sein, wenn:

- Type Contracts kompatibel,
- State Migration möglich,
- Security erfüllt

ist.

## Live Rebinding

Beispiel:

```text
DecoderProviderV1
      ↓
quiesce
      ↓
DecoderProviderV2
```

Der Datenstrom kann anschließend weiterlaufen.

## Stateful Nodes

Nicht jeder Node ist stateless.

Beispiele:

```text
VideoDecoder
AudioResampler
AIConversationContext
Compressor
```

Stateful Nodes benötigen explizite State Contracts.

## Node State

Ein Stateful Node kann besitzen:

```text
NodeState {
    semanticType
    version
    migration
}
```

## State Migration

Hot Replacement kann State Migration durchführen.

Beispiel:

```text
DecoderStateV1
      ↓ migrate
DecoderStateV2
```

## Checkpointing

Lange Pipelines dürfen Checkpoints erzeugen.

Beispiel:

```text
Stage A
    ↓
Checkpoint
    ↓
Stage B
```

Nach Fehler muss Stage A möglicherweise nicht erneut ausgeführt werden.

## Recovery

Pipeline Recovery kann abhängig vom Node-Typ unterschiedliche Strategien verwenden.

Beispiele:

```text
Retry
RestartNode
RestartBranch
RebindProvider
RestoreCheckpoint
ReconstructPipeline
Abort
```

## Smallest Failure Domain

Recovery soll bevorzugt den kleinsten sicheren Scope betreffen.

Ein einzelner Decoderfehler soll nicht automatisch die gesamte Anwendung oder das System neu starten.

## Self-Healing

Das Pipeline-Modell integriert sich mit Nova.Resilience.

Beispiel:

```text
Detect
    ↓
Classify
    ↓
Contain
    ↓
Replan Pipeline
    ↓
Rebind
    ↓
Verify
```

## Health

Pipeline Nodes können Health States besitzen.

Beispiel:

```text
Healthy
Degraded
Recovering
Failed
Unavailable
```

## Health-aware Provider Selection

Provider Health wird bei Replanning berücksichtigt.

## Pipeline Lifecycle

Eine Pipeline kann mindestens folgende Zustände besitzen:

```text
Defined
Resolving
Prepared
Starting
Running
Degraded
Pausing
Paused
Recovering
Stopping
Stopped
Failed
```

Die genaue State Machine wird separat spezifiziert.

## Node Lifecycle

Nodes besitzen ebenfalls definierte Lifecycle States.

## Preparation Phase

Vor Start kann NovaOS:

- Types prüfen,
- Capabilities auflösen,
- Provider auswählen,
- Ressourcen reservieren,
- Buffer vorbereiten,
- Security prüfen.

## Activation

Nach erfolgreicher Preparation wird die Pipeline aktiviert.

Dies kann transaktional erfolgen.

## Deactivation

Beim Stoppen werden:

- Tasks beendet,
- Buffers freigegeben,
- Capabilities zurückgegeben,
- Reservations aufgehoben.

## Pause

Pipelines dürfen pausierbar sein.

Stateful Nodes müssen definieren, wie State erhalten wird.

## Resume

Resume kann denselben oder einen neuen Provider verwenden, sofern State kompatibel ist.

## Architecture Introspection

Das vollständige Pipeline-Modell muss introspektierbar sein.

Beispiel:

```text
Camera
  │ VideoFrame<NV12>
  ▼
Decoder [GPU0]
  │ Image<RGBA8>
  ▼
AIEnhancer [NPU0]
  │ Image<RGBA8>
  ▼
Renderer [GPU0]
```

## Pipeline Introspection

Mindestens sichtbar sein sollen können:

```text
Nodes
Edges
Semantic Types
Providers
Locations
Resource Usage
Latency
Buffers
Security Labels
Health
```

abhängig von Berechtigungen.

## Decision Tracing

Automatische Planungsentscheidungen sollen erklärbar sein.

Beispiel:

```text
CPU Provider rejected:
    requires GPU → CPU transfer
    estimated transfer = 4.2 ms

GPU Provider selected:
    input already located on GPU0
    no conversion required
```

## Data Movement Visualization

Entwicklertools sollen Datenbewegung sichtbar machen können.

Beispiel:

```text
CPU0
 │
 │ 32 MiB copy
 ▼
GPU0
 │
 │ zero-copy
 ▼
Display
```

## Copy Tracing

NovaOS SOLL unnötige Kopien diagnostizierbar machen.

Beispiel:

```text
Copy #42

Reason:
    incompatible layout

Source:
    Image<RGBA8>

Target:
    Image<RGBA8>

Source Layout:
    Linear

Target Requirement:
    Tiled
```

## Conversion Tracing

Konvertierungen sollen ebenfalls sichtbar sein.

```text
RGBA8
    ↓ ColorConvert
NV12

Cost:
    0.8 ms
```

## Performance Metrics

Pipeline Metrics können umfassen:

- End-to-End Latency
- Node Latency
- Edge Latency
- Throughput
- Queue Depth
- Buffer Count
- Copy Count
- Conversion Count
- Dropped Objects
- Stall Time
- Resource Usage
- Energy Cost.

## Critical Path

Der Planner beziehungsweise Profiler soll den Critical Path einer Pipeline bestimmen können.

Beispiel:

```text
Decode → Inference → Render
```

mit:

```text
Critical Path = 14.8 ms
```

## Bottleneck Detection

NovaOS darf Bottlenecks automatisch erkennen.

Beispiele:

- slow Node
- transfer bottleneck
- queue congestion
- memory pressure
- provider saturation.

## Adaptive Optimization

Pipeline-Pläne dürfen dynamisch optimiert werden.

Beispiel:

```text
Current:
    CPU Decoder

Observed:
    CPU saturation

Alternative:
    GPU Decoder

Action:
    Replan
```

nur wenn Contracts dies erlauben.

## Prediction Error

Adaptive Pipeline-Optimierung kann das NovaOS-Prediction-Error-Modell verwenden.

```text
Predicted Best Plan
        ↓
Actual Performance
        ↓
Deviation
        ↓
Model Correction
```

## User Decisions

Wenn ein Nutzer beziehungsweise Entwickler wiederholt automatische Entscheidungen überschreibt, kann dies als Lernsignal verwendet werden.

Hard Constraints bleiben unveränderlich.

## Stability

Adaptive Pipeline-Optimierung darf keine permanente Plan-Fluktuation verursachen.

NovaOS muss Mechanismen gegen:

- oscillation,
- thrashing,
- excessive rebinding

besitzen.

## Hysteresis

Providerwechsel können Hysteresis verwenden.

Ein minimaler erwarteter Vorteil kann erforderlich sein, bevor eine laufende Pipeline umgeplant wird.

## Replanning Cost

Die Kosten des Replanning selbst müssen berücksichtigt werden.

Beispiel:

```text
New Provider saves:
    0.5 ms/frame

Migration costs:
    500 ms
```

Bei kurzer Restlaufzeit ist ein Wechsel möglicherweise nicht sinnvoll.

## Stable Plan

Ein stabiler leicht suboptimaler Plan kann einem ständig wechselnden theoretisch optimalen Plan vorgezogen werden.

## QoS

Pipeline Contracts können Quality-of-Service-Anforderungen besitzen.

Beispiele:

```text
FrameRate >= 60 Hz
Latency <= 16 ms
Resolution >= 1080p
AudioDrop = Forbidden
```

## Quality Levels

Capabilities dürfen mehrere Qualitätsstufen anbieten.

Beispiel:

```text
ImageEnhancement {
    Fast
    Balanced
    HighQuality
}
```

## Quality Adaptation

Bei Resource Pressure darf Quality angepasst werden, wenn der Contract dies erlaubt.

## Realtime

Realtime Pipelines benötigen reservierbare Ressourcen.

Beispiele:

```text
Audio
Video
Industrial Control
Medical Imaging
```

## Hard Realtime

Hard-Realtime-Pipelines dürfen nicht von:

- unbounded allocation,
- uncontrolled paging,
- arbitrary remote providers,
- unpredictable converters

abhängen.

## Admission Control

Vor Aktivierung einer Hard-Realtime-Pipeline muss geprüft werden, ob benötigte Ressourcen verfügbar beziehungsweise reservierbar sind.

## Deadline Miss

Deadline Misses müssen sichtbar sein.

Je nach Pipeline können sie:

- toleriert,
- geloggt,
- kompensiert,
- als Fehler behandelt

werden.

## Energy

Pipeline Planning berücksichtigt Energie.

Beispiel:

```text
CPU Plan:
    10 ms
    4 J

NPU Plan:
    12 ms
    0.8 J
```

Bei ausreichendem Latency Budget kann NPU bevorzugt werden.

## Thermal

Thermal Pressure kann Replanning auslösen.

Beispiel:

```text
GPU thermal limit
      ↓
Move inference to NPU
```

wenn erlaubt.

## Background Pipelines

Background Pipelines dürfen zugunsten interaktiver Pipelines gedrosselt werden.

## Priority

Pipelines und Nodes können Prioritäten besitzen.

Priority allein darf jedoch keine Security- oder Hard-Realtime-Regeln umgehen.

## Fairness

Resource Economy muss Fairness zwischen Pipelines berücksichtigen.

Eine einzelne Pipeline darf nicht ohne explizite Autorisierung alle Systemressourcen monopolieren.

## Multi-Tenant

Pipeline-Objekte und Ressourcen müssen Tenant-/User-Grenzen respektieren.

## Sandboxing

Untrusted Pipeline Nodes können in isolierten Execution Domains ausgeführt werden.

Beispiel:

```text
Trusted Pipeline
      ↓
Untrusted Plugin
      ↓
Sandbox Domain
```

## Plugin Nodes

Plugins müssen ihre:

- Input Types
- Output Types
- Capabilities
- Side Effects
- Resource Requirements

deklarieren.

## Trust Boundaries

Pipeline Edges über Trust Boundaries können zusätzliche:

- Validation
- Copy
- Sanitization
- Serialization

erfordern.

Zero-Copy darf Security Boundaries nicht umgehen.

## Validation Nodes

Validation kann expliziter Pipeline Node sein.

Beispiel:

```text
UntrustedData
      ↓ Validate
ValidatedData
```

## Sanitization Nodes

Ebenso:

```text
UntrustedHTML
      ↓ Sanitize
SafeHTML
```

## Audit

Security-relevante Pipeline-Operationen können auditiert werden.

Audit kann beispielsweise enthalten:

```text
Object
Capability
Provider
Action
Timestamp
Result
```

## Privacy

Architecture Introspection darf nicht automatisch vertrauliche Payload-Inhalte offenlegen.

Metadatenzugriff muss capability-basiert kontrolliert werden.

## Payload Inspection

Tools benötigen separate Rechte, um Payloads zu inspizieren.

Pipeline-Topologie und Payload-Zugriff sind unterschiedliche Berechtigungen.

## Serialization Boundary

Pipeline Edges müssen sichtbar machen können, wann eine Serialization Boundary existiert.

Beispiel:

```text
Local Object
      ↓ Serialize
Network
      ↓ Deserialize
Remote Object
```

## Copy Boundary

Ebenso müssen Copy Boundaries erkennbar sein.

## Trust Boundary

Ebenso Trust Boundaries.

## Transaction Boundary

Ebenso Transaction Boundaries.

## Location Boundary

Ebenso Location Boundaries.

## Unified Developer Model

Entwickler sollen möglichst dasselbe logische Modell verwenden können für:

```text
Files
Streams
Images
Audio
Video
AI
Network
Devices
Databases
```

Das bedeutet nicht, dass alle Subsysteme intern identisch implementiert werden.

Es bedeutet, dass sie in ein gemeinsames semantisches Pipeline-Modell integrierbar sind.

## Abstraktionsgrenze

Das einheitliche Pipeline-Modell darf nicht dazu führen, dass spezialisierte High-Performance-APIs verschwinden.

Entwickler können bei Bedarf:

- Low-Level GPU APIs,
- direkte Device APIs,
- spezialisierte Compute APIs

verwenden.

Das Prinzip lautet:

```text
Unified by default,
specialized when necessary.
```

## Escape Hatch

Low-Level Escape Hatches bleiben ausdrücklich erlaubt.

Sie müssen jedoch Security, Capability und Resource Policies respektieren.

## No Universal Runtime Requirement

Das Pipeline-Modell bedeutet nicht, dass jede einzelne Datenoperation durch einen zentralen Pipeline-Daemon laufen muss.

Dies wäre ein:

- Performance Bottleneck,
- Single Point of Failure,
- Skalierungsproblem.

Das Modell ist logisch einheitlich, die Ausführung kann verteilt sein.

## Distributed Execution

Pipeline Nodes können in unterschiedlichen:

- Prozessen,
- Services,
- Driver Domains,
- CPUs,
- GPUs,
- NPUs,
- Maschinen

ausgeführt werden.

## Control Plane

Das Control Plane übernimmt unter anderem:

- Pipeline Resolution
- Type Checking
- Capability Resolution
- Policy Evaluation
- Resource Planning
- Provider Selection
- Security Validation.

## Data Plane

Das Data Plane transportiert beziehungsweise verarbeitet die eigentlichen Daten.

Es soll möglichst:

- direkt,
- zero-copy,
- prevalidated,
- low-overhead

arbeiten.

## Control/Data Plane Separation

Das bevorzugte Modell lautet:

```text
                CONTROL PLANE

Pipeline Declaration
        ↓
Type Resolution
        ↓
Capability Resolution
        ↓
Policy Evaluation
        ↓
Resource Planning
        ↓
Provider Selection
        ↓
Execution Plan

----------------------------------------

                  DATA PLANE

Source
   ↓
Object
   ↓
Provider
   ↓
Object
   ↓
Provider
   ↓
Sink
```

## Fast Path

Nach erfolgreicher Planung sollen Fast Paths ohne permanente komplexe Policy-Auswertung möglich sein.

## Prepared Pipeline

Eine vorbereitete Pipeline kann enthalten:

```text
PreparedPipeline {
    nodes
    providers
    objectLayouts
    buffers
    routes
    permissions
    budgets
}
```

## Pipeline Handle

Anwendungen können einen vorbereiteten Plan über ein Pipeline Handle referenzieren.

## Plan Cache

Häufig verwendete Pipeline-Pläne dürfen gecacht werden.

## Plan Invalidation

Ein Plan muss invalidiert beziehungsweise neu geprüft werden, wenn relevante Bedingungen sich ändern.

Beispiele:

- Provider unavailable
- Type version changed
- Security policy changed
- Sovereignty changed
- Resource topology changed.

## Versioning

Pipeline-Definitionen müssen versionierbar sein.

## Capability Versioning

Pipeline-Pläne müssen Capability- und Provider-Versionen berücksichtigen.

## Schema Evolution

Pipeline-Verbindungen müssen Type Schema Evolution berücksichtigen.

## Pipeline Migration

Eine laufende Pipeline kann auf eine neue Definition migriert werden, wenn dies sicher möglich ist.

## Live Evolution

Dies unterstützt das NovaOS-Prinzip:

```text
Hot Replacement / Live Evolution
```

## Declarative Upgrade

Eine neue Desired Pipeline Definition kann bereitgestellt werden.

Der Reconciler plant den Übergang.

Beispiel:

```text
Pipeline V1
    ↓
Prepare V2
    ↓
Quiesce
    ↓
State Transfer
    ↓
Activate V2
    ↓
Verify
    ↓
Retire V1
```

## Rollback

Wenn Verifikation fehlschlägt, kann auf den vorherigen Plan zurückgekehrt werden.

## Architecture Graph Integration

Pipeline Nodes und Objects werden Teil des Architecture Graph.

Beispiel:

```text
Application
    │
    ▼
Pipeline
    ├── Decoder
    ├── Filter
    ├── AI
    └── Renderer
```

## Global System State Graph

Pipeline State kann in den globalen System State Graph integriert werden.

Dadurch werden sichtbar:

- Dependencies
- Providers
- Objects
- Resources
- Locations
- Health.

## Explainability

Automatische Pipeline Construction und Optimization müssen nachvollziehbar sein.

Beispiel:

```text
Plan #14 selected.

Reasons:
    no lossy conversion
    2 fewer memory copies
    satisfies 16 ms deadline
    data remains local
    NPU provider verified

Rejected Plan #11:
    requires cloud provider
    violates LocalOnly
```

## Normative Festlegungen

1. NovaOS MUSS ein einheitliches Daten- und Objekt-Pipelining als systemweites Architekturprinzip unterstützen.

2. Das Pipeline-Modell MUSS semantisch typisierte Objekte zwischen Systemkomponenten transportieren können.

3. Pipeline-Kompatibilität MUSS Semantic Types berücksichtigen.

4. Semantic Type und physische Representation MÜSSEN getrennt modellierbar sein.

5. Pipeline-Objekte SOLLEN typisierte Metadaten besitzen können.

6. Pipeline-Objekte SOLLEN Provenance besitzen können.

7. Pipeline-Objekte MÜSSEN Security- und Information-Flow-Metadaten tragen können.

8. Pipeline-Objekte MÜSSEN Data-Sovereignty-Constraints tragen beziehungsweise referenzieren können.

9. Große Payloads SOLLEN über Handles beziehungsweise Capabilities referenzierbar sein.

10. Objektidentität MUSS von Speicheradresse unterscheidbar sein.

11. Value Objects MÜSSEN ohne dauerhafte Object Identity möglich sein.

12. Entity Objects SOLLEN stabile Object Identity besitzen können.

13. Pipeline-Objekte SOLLEN bevorzugt immutable beziehungsweise logisch immutable sein.

14. Mutable Pipeline Objects MÜSSEN klar definiertes Ownership besitzen.

15. NovaOS SOLL Views auf bestehende Payloads unterstützen.

16. Views SOLLEN Zero-Copy möglich machen.

17. Views MÜSSEN semantisch korrekt typisiert sein.

18. Pipelines MÜSSEN als gerichtete Graphen modellierbar sein.

19. Pipelines DÜRFEN linear, verzweigt oder zusammengeführt sein.

20. Pipeline Nodes MÜSSEN typisierte Input- und Output-Contracts besitzen können.

21. Pipeline Edges MÜSSEN Semantic Type und relevante Transfersemantik beschreiben können.

22. Sources und Sinks MÜSSEN in dasselbe Pipeline-Modell integrierbar sein.

23. Transforms, Filters, Converters, Aggregators und Splitters SOLLEN als Pipeline Nodes modellierbar sein.

24. Streaming-, Batch- und Single-Object-Pipelines MÜSSEN unterstützt werden können.

25. Lazy Execution SOLL unterstützt werden.

26. Eager Execution MUSS möglich bleiben.

27. NovaOS SOLL Pipeline Fusion unterstützen können.

28. Pipeline Fusion DARF die deklarierte Semantik NICHT verändern.

29. Pipeline Fusion MUSS Hard Security Constraints erhalten.

30. Pipeline Fusion MUSS Determinism Requirements erhalten.

31. Operator Fusion DARF für Compute- und AI-Pipelines verwendet werden.

32. Pipeline Optimization SOLL Copy Elimination berücksichtigen.

33. Pipeline Optimization SOLL Conversion Elimination berücksichtigen.

34. Pipeline Optimization SOLL Buffer Reuse berücksichtigen.

35. Pipeline Optimization SOLL Provider Co-location berücksichtigen.

36. Pipeline Optimization SOLL End-to-End-Kosten berücksichtigen.

37. Jeder relevante Pipeline Node MUSS einen Execution Contract besitzen beziehungsweise ableiten können.

38. Eine Pipeline MUSS End-to-End-Contracts besitzen können.

39. End-to-End Hard Constraints MÜSSEN gegenüber lokalen Optimierungszielen Vorrang besitzen.

40. Pipeline Constraints SOLLEN auf Nodes propagiert werden können.

41. Die Resource Economy MUSS Pipeline Nodes und Edges berücksichtigen.

42. Datenbewegung MUSS als explizite Ressourcenkostenklasse berücksichtigt werden.

43. Transfer Cost MUSS bei Provider Selection berücksichtigt werden können.

44. Conversion Cost MUSS bei Pipeline Planning berücksichtigt werden können.

45. Synchronization Cost SOLL bei Pipeline Planning berücksichtigt werden.

46. Data Locality SOLL bei Provider Selection berücksichtigt werden.

47. Location Transparency DARF Transferkosten NICHT unsichtbar machen.

48. Pipeline Nodes DÜRFEN remote ausgeführt werden, wenn Hard Constraints dies erlauben.

49. Location Failures MÜSSEN als explizite Fehlerklasse behandelbar sein.

50. Pipeline-Pläne SOLLEN Fallback Provider beziehungsweise alternative Pfade besitzen können.

51. Graceful Degradation DARF nur erfolgen, wenn Contract oder Policy dies erlaubt.

52. Zero-Copy SOLL der bevorzugte Datenpfad zwischen kompatiblen Pipeline Nodes sein.

53. Zero-Copy DARF Security- oder Isolation Boundaries NICHT umgehen.

54. Copy-on-Write SOLL für geeignete Shared Objects unterstützt werden können.

55. Buffer Reuse DARF nur bei sicheren Lifetime- und Ownership-Verhältnissen erfolgen.

56. Buffer Pooling SOLL für häufig verwendete Datenrepräsentationen unterstützt werden können.

57. Memory Pressure MUSS in Pipeline Planning integrierbar sein.

58. Pipeline-Objekte MÜSSEN definierte Ownership-Semantik besitzen.

59. Ownership Transfer MUSS explizit modellierbar sein.

60. Borrowing SOLL unterstützt werden können.

61. Objekt-Lifetimes MÜSSEN mit Pipeline-Lifetimes koordinierbar sein.

62. Es MUSS kein einzelnes universelles Memory-Management-Verfahren für alle Pipeline-Objekte verwendet werden.

63. Objektzugriff MUSS capability-basiert autorisierbar sein.

64. Pipeline Nodes SOLLEN nach Least Privilege nur benötigte Objektrechte erhalten.

65. Capability Propagation MUSS kontrolliert erfolgen.

66. Security Labels MÜSSEN durch Pipeline-Operationen propagierbar sein.

67. Derived Data MUSS Security- und Sovereignty-Policy berücksichtigen.

68. Declassification MUSS explizit autorisiert sein.

69. Data-Sovereignty-Constraints MÜSSEN durch relevante Datenableitungen propagierbar sein.

70. Trust State MUSS entlang einer Pipeline veränderbar und nachvollziehbar sein.

71. Pipeline-Ausführung SOLL Provenance Chains erzeugen können.

72. Der Detailgrad der Provenance DARF durch Policy gesteuert werden.

73. Pipeline Nodes SOLLEN Side Effects deklarieren.

74. Nodes SOLLEN als Pure, Idempotent, Transactional oder ExternalSideEffect klassifizierbar sein.

75. Pure Nodes SOLLEN für Caching und Fusion optimierbar sein.

76. Nicht-idempotente Nodes DÜRFEN NICHT blind erneut ausgeführt werden.

77. Pipeline Nodes DÜRFEN Teil transaktionaler Systemoperationen sein.

78. Transaktionsgrenzen MÜSSEN explizit modellierbar sein.

79. Nicht rollbackfähige externe Side Effects MÜSSEN im Execution Model berücksichtigt werden.

80. Compensation Actions SOLLEN unterstützt werden können.

81. Pipeline-Ausführung MUSS mit Structured Concurrency integrierbar sein.

82. Pipeline Tasks MÜSSEN definierte Scopes besitzen.

83. Cancellation MUSS durch Pipeline Scopes propagierbar sein.

84. Deadline Propagation MUSS unterstützt werden.

85. Fehler MÜSSEN strukturiert durch Pipeline Scopes propagierbar sein.

86. Partial Failure MUSS modellierbar sein.

87. Streaming-Pipelines MÜSSEN Backpressure unterstützen.

88. Backpressure MUSS semantisch geeignete Policies verwenden können.

89. Queues SOLLEN standardmäßig bounded sein.

90. Unbounded Queues DÜRFEN NICHT unbeabsichtigt entstehen.

91. Pipeline Edges MÜSSEN Buffering Policies besitzen können.

92. Pipelines MÜSSEN Ordering Requirements ausdrücken können.

93. Parallelisierung DARF Ordering NICHT verletzen.

94. Multi-Stream Synchronization MUSS Time Semantics berücksichtigen.

95. Pipeline Scheduling MUSS Dependencies berücksichtigen.

96. Pipeline Scheduling MUSS Deadlines berücksichtigen können.

97. Pipeline Scheduling MUSS Resource Budgets berücksichtigen können.

98. Unabhängige Nodes SOLLEN parallel ausführbar sein.

99. Pipeline Parallelism SOLL unterstützt werden.

100. Data Parallelism SOLL unterstützt werden können.

101. Adaptive Parallelism DARF Hard Constraints NICHT verletzen.

102. Deterministic Mode MUSS Pipeline Provider, Algorithms und Scheduling einschränken können.

103. Deterministic Mode MUSS Conversion Paths fixieren können.

104. Zufallsabhängige Nodes MÜSSEN Randomness deklarieren können.

105. Pure deterministische Nodes DÜRFEN gecacht beziehungsweise memoisiert werden.

106. Pipeline Caches MÜSSEN Security- und Isolation-Grenzen berücksichtigen.

107. Lazy Results MÜSSEN materialisierbar sein.

108. Virtuelle Objekte SOLLEN unterstützt werden können.

109. Demand-driven Execution SOLL möglich sein.

110. Nicht benötigte side-effect-freie Pipeline-Branches DÜRFEN eliminiert werden.

111. Partial Materialization SOLL unterstützt werden können.

112. Dateien MÜSSEN als Sources und Sinks integrierbar sein.

113. Dateien DÜRFEN NICHT als universelles internes Zwischenformat vorausgesetzt werden.

114. NovaFile MUSS direkt in das semantische Pipeline-Modell integrierbar sein.

115. Legacy-Dateien SOLLEN über Parser-/Decoder-Capabilities in Semantic Objects überführt werden.

116. Persistierung SOLL als Pipeline modellierbar sein.

117. Netzwerktransport SOLL als Pipeline-Edge beziehungsweise Pipeline-Stufe modellierbar sein.

118. Lokale Pipeline-Stufen SOLLEN unnötige Serialisierung vermeiden.

119. Remote Pipeline-Stufen DÜRFEN Serialisierung verwenden.

120. Compression DARF automatisch geplant werden, wenn Contract und Policy dies erlauben.

121. Hardware Devices SOLLEN in das gemeinsame Pipeline-Modell integrierbar sein.

122. DMA DARF als Transfermechanismus verwendet werden.

123. DMA MUSS durch geeignete Hardware-Isolation abgesichert werden.

124. GPU- und NPU-Objekte SOLLEN semantisch typisierte Systemobjekte bleiben.

125. Hardware-spezifische Handles DÜRFEN die öffentliche semantische Pipeline-Architektur NICHT dominieren.

126. Pipeline Nodes SOLLEN primär Capabilities statt konkrete Provider referenzieren.

127. Provider Selection SOLL automatisch erfolgen können.

128. Provider Selection MUSS den gesamten relevanten Pipeline-Kontext berücksichtigen können.

129. Die lokal schnellste Node-Implementierung MUSS NICHT gewählt werden, wenn ein anderer End-to-End-Plan besser ist.

130. Automatische Algorithmusauswahl SOLL pipelinebezogene Constraints berücksichtigen.

131. Explizite Algorithmus-Overrides MÜSSEN möglich sein, sofern höhere Hard Constraints erfüllt bleiben.

132. Pipelines SOLLEN deklarativ definierbar sein.

133. Pipeline Definitions MÜSSEN typisierte Inputs und Outputs besitzen können.

134. Pipeline Parameters SOLLEN Semantic Types verwenden.

135. Wiederverwendbare Pipeline Templates SOLLEN unterstützt werden.

136. Pipelines MÜSSEN dynamisch zur Laufzeit zusammensetzbar sein können.

137. Capability Composition MUSS Type Compatibility berücksichtigen.

138. NovaOS DARF zielbasierte Pipeline Construction unterstützen.

139. Automatische Pipeline Construction MUSS Security Constraints einhalten.

140. Automatische Pipeline Construction MUSS Trust Requirements einhalten.

141. Automatische Pipeline Construction MUSS Data-Sovereignty-Constraints einhalten.

142. Automatische Pipeline Construction MUSS Resource Budgets einhalten.

143. Automatische Pipeline Construction MUSS Determinism Requirements einhalten.

144. Pipeline Planning MUSS bounded beziehungsweise kontrollierbar sein.

145. KI DARF Pipeline Planning unterstützen.

146. KI DARF NICHT allein die Einhaltung fundamentaler Hard Constraints garantieren.

147. Pipelines MÜSSEN in das Declarative System Model integrierbar sein.

148. Desired State MUSS den gewünschten Pipeline-Zustand beschreiben können.

149. Actual State MUSS den tatsächlichen Pipeline-Zustand beschreiben können.

150. Pipeline Reconciliation SOLL unterstützt werden.

151. Provider Failure SOLL Replanning auslösen können.

152. Hot Replacement MUSS Type Contract Compatibility berücksichtigen.

153. Stateful Nodes MÜSSEN ihren State explizit beschreiben können.

154. Stateful Nodes SOLLEN State Migration unterstützen können.

155. Lange Pipeline-Ausführungen SOLLEN Checkpointing unterstützen können.

156. Recovery SOLL den kleinsten sicheren Failure Scope verwenden.

157. Pipeline Recovery MUSS mit Nova.Resilience integrierbar sein.

158. Pipeline Nodes SOLLEN Health States besitzen.

159. Provider Health SOLL bei Replanning berücksichtigt werden.

160. Pipelines MÜSSEN definierte Lifecycle States besitzen.

161. Pipeline Activation SOLL transaktional vorbereitet werden können.

162. Ressourcen und Capabilities MÜSSEN beim Stoppen korrekt freigegeben werden.

163. Pipelines DÜRFEN Pause und Resume unterstützen.

164. Architecture Introspection MUSS Pipeline-Graphen sichtbar machen können.

165. Architecture Introspection MUSS relevante Semantic Types anzeigen können.

166. Architecture Introspection SOLL Provider und Locations anzeigen können.

167. Architecture Introspection SOLL Resource Usage und Health anzeigen können.

168. Sensitive Payloads DÜRFEN durch Introspection NICHT ohne Berechtigung offengelegt werden.

169. Decision Tracing SOLL automatische Pipeline-Planungsentscheidungen erklären.

170. Data Movement SOLL diagnostizierbar sein.

171. Unnötige Copies SOLLEN diagnostizierbar sein.

172. Type Conversions SOLLEN diagnostizierbar sein.

173. End-to-End Latency SOLL messbar sein.

174. Node- und Edge-Latency SOLLEN messbar sein.

175. Queue Depth und Backpressure SOLLEN beobachtbar sein.

176. NovaOS SOLL Critical Paths bestimmen können.

177. NovaOS DARF Pipeline Bottlenecks automatisch erkennen.

178. Adaptive Pipeline Optimization DARF Laufzeitmetriken verwenden.

179. Adaptive Optimization DARF Prediction Error als Lernsignal verwenden.

180. Adaptive Optimization DARF Hard Constraints NICHT überschreiben.

181. Adaptive Pipeline Planning MUSS Oscillation und Thrashing begrenzen.

182. Provider Rebinding SOLL Hysteresis berücksichtigen können.

183. Replanning Cost MUSS in Replanning Decisions berücksichtigt werden können.

184. Ein stabiler Plan DARF einem minimal besseren instabilen Plan vorgezogen werden.

185. Pipelines MÜSSEN QoS Requirements ausdrücken können.

186. Quality Adaptation DARF nur innerhalb erlaubter Contracts erfolgen.

187. Realtime Pipelines MÜSSEN Ressourcenreservierung unterstützen können.

188. Hard-Realtime-Pipelines DÜRFEN NICHT von unbounded Runtime-Verhalten abhängen.

189. Hard-Realtime-Pipelines SOLLEN Admission Control verwenden.

190. Deadline Misses MÜSSEN beobachtbar sein.

191. Energie MUSS als Pipeline Planning Cost berücksichtigt werden können.

192. Thermal State MUSS Replanning beeinflussen können.

193. Background Pipelines SOLLEN zugunsten höher priorisierter Workloads gedrosselt werden können.

194. Resource Economy MUSS Fairness zwischen Pipelines berücksichtigen.

195. Multi-Tenant-Pipelines MÜSSEN Isolation respektieren.

196. Untrusted Pipeline Nodes SOLLEN in isolierten Execution Domains ausführbar sein.

197. Plugin Nodes MÜSSEN ihre Type Contracts und benötigten Capabilities deklarieren.

198. Trust Boundaries MÜSSEN im Pipeline Graph darstellbar sein.

199. Validation und Sanitization SOLLEN explizite Pipeline Nodes sein können.

200. Security-relevante Pipeline-Aktionen SOLLEN auditierbar sein.

201. Pipeline Topology Access und Payload Access MÜSSEN getrennt autorisierbar sein.

202. Serialization Boundaries SOLLEN sichtbar sein.

203. Copy Boundaries SOLLEN sichtbar sein.

204. Trust Boundaries SOLLEN sichtbar sein.

205. Transaction Boundaries SOLLEN sichtbar sein.

206. Location Boundaries SOLLEN sichtbar sein.

207. NovaOS MUSS ein logisch einheitliches Pipeline-Modell bereitstellen, ohne alle Subsysteme intern identisch implementieren zu müssen.

208. Spezialisierte Low-Level APIs MÜSSEN weiterhin möglich sein.

209. Low-Level Escape Hatches MÜSSEN Capability-, Security- und Resource-Policies respektieren.

210. Das Pipeline-Modell DARF NICHT von einem einzigen zentralen Runtime-Daemon abhängig sein.

211. Pipeline-Ausführung MUSS verteilt implementierbar sein.

212. Control Plane und Data Plane SOLLEN getrennt werden.

213. Komplexe Policy- und Planungsentscheidungen SOLLEN primär im Control Plane stattfinden.

214. Der Data Plane SOLL vorbereitete Fast Paths verwenden können.

215. Pipeline Plans SOLLEN vorbereitet und gecacht werden können.

216. Plan Caches MÜSSEN bei relevanten Änderungen invalidierbar sein.

217. Pipeline Definitions MÜSSEN versionierbar sein.

218. Pipeline Plans MÜSSEN Capability- und Provider-Versionen berücksichtigen.

219. Pipeline Migration SOLL Live Evolution unterstützen.

220. Pipeline Upgrades SOLLEN transaktional vorbereitet und aktiviert werden können.

221. Fehlgeschlagene Pipeline Upgrades SOLLEN Rollback unterstützen.

222. Pipeline State MUSS in den Architecture beziehungsweise System State Graph integrierbar sein.

223. Automatische Pipeline Construction MUSS erklärbar sein.

224. Automatische Provider Selection MUSS erklärbar sein.

225. Automatische Conversion Selection MUSS erklärbar sein.

226. Das einheitliche Pipeline-Modell MUSS langfristig erweiterbar sein, ohne Anwendungen an konkrete interne Datenrepräsentationen zu koppeln.

## Konsequenzen

### Positive Konsequenzen

- einheitliches Datenflussmodell für das gesamte System
- weniger API-Brücken
- weniger unnötige Serialisierung
- weniger Datenkopien
- bessere Zero-Copy-Nutzung
- bessere Nutzung von GPU und NPU
- geringere Speicherbandbreite
- geringere Latenz
- geringerer Energieverbrauch
- semantisch typisierte Datenflüsse
- bessere Capability-Komposition
- automatische Pipeline Construction möglich
- globale statt ausschließlich lokale Optimierung
- bessere Resource Economy
- bessere Data-Sovereignty-Kontrolle
- bessere Security- und Information-Flow-Kontrolle
- bessere Provenance
- bessere Hot-Replacement-Fähigkeit
- bessere Self-Healing-Integration
- bessere Architecture Introspection
- präzisere Performanceanalyse
- einheitliches Modell für Files, Streams, Media, AI und Devices
- langfristige Entkopplung von Datenbedeutung und Hardwareimplementation

### Negative Konsequenzen

- erhebliche Komplexität des Pipeline Planners
- komplexe Ownership- und Lifetime-Regeln
- Conversion Graph kann sehr groß werden
- globale Optimierung kann teuer sein
- Runtime-Replanning muss sorgfältig begrenzt werden
- Debugging verteilter Pipelines ist anspruchsvoll
- Security Label Propagation erhöht Komplexität
- State Migration bei Stateful Nodes ist schwierig
- Backpressure muss domänenspezifisch korrekt konfiguriert werden
- Pipeline Fusion erschwert teilweise Debugging und Provenance
- Legacy-Anwendungen profitieren nicht automatisch vollständig vom Modell
- Provider müssen präzise Contracts veröffentlichen
- falsche Cost Models können suboptimale Pläne erzeugen

## Verworfene Alternativen

### Dateien als universelles Austauschformat

Nicht übernommen.

Temporäre Dateien zwischen Verarbeitungsschritten erzeugen:

- I/O,
- Serialisierung,
- Latenz,
- Metadatenverlust.

Dateien bleiben wichtige persistente Sources und Sinks, sind aber kein universelles internes Datenmodell.

### Byte-Streams als universelle Abstraktion

Nicht übernommen.

Byte-Streams sind nützlich, verlieren aber häufig:

- Semantic Type
- Shape
- Units
- Provenance
- Security Context.

Sie bleiben als Low-Level-Mechanismus verfügbar.

### Anwendungsspezifische Pipelines ohne Systemintegration

Nicht übernommen.

Dadurch könnten Resource Economy, Security, Zero-Copy und Provider Selection nicht systemweit optimieren.

### Zentraler Pipeline-Daemon für alle Daten

Nicht übernommen.

Ein zentraler Datenpfad würde:

- Bottlenecks,
- Single Points of Failure,
- zusätzliche Context Switches

erzeugen.

NovaOS verwendet ein logisch einheitliches, aber physisch verteilbares Modell.

### Vollständig transparente Remote-Ausführung

Nicht übernommen.

Remote Execution darf nicht so transparent sein, dass:

- Netzwerkfehler,
- Latenz,
- Kosten,
- Sovereignty

unsichtbar werden.

### Zero-Copy um jeden Preis

Nicht übernommen.

Copies bleiben notwendig, wenn:

- Security Isolation,
- Representation Conversion,
- Layout Conversion,
- Hardwareanforderungen

dies verlangen.

### Globale Optimierung vor jeder Operation

Nicht übernommen.

Die Planungskosten wären zu hoch.

NovaOS verwendet:

- vorbereitete Plans,
- Caching,
- Fast Paths,
- Replanning bei relevanten Änderungen.

### Ausschließlich statische Pipelines

Nicht übernommen.

NovaOS benötigt dynamische Capability-Komposition und adaptive Providerwahl.

### Ausschließlich dynamische Pipelines

Nicht übernommen.

Statische beziehungsweise vorbereitete Pipelines sind wichtig für:

- Performance,
- Realtime,
- Determinism,
- Verifikation.

### KI als Pipeline Planner

Nicht als alleinige Architektur übernommen.

KI darf unterstützen.

Fundamentale Korrektheit und Policy Enforcement müssen deterministisch überprüfbar bleiben.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip
- ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern
- ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-EXECUTION-0001_Nova_ExecutionContract
- ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten
- ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard
- ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell
- ADR-IPC-0001_Typed_IPC
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur
- ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell
- ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur

## Zugehörige NPSPECs

- NPSPEC-ARCH-DATAPIPELINE-0001
- NPSPEC-DATA-OBJECT-0001
- NPSPEC-DATA-OBJECT-IDENTITY-0001
- NPSPEC-DATA-OBJECT-DESCRIPTOR-0001
- NPSPEC-DATA-OBJECT-METADATA-0001
- NPSPEC-DATA-OBJECT-LIFECYCLE-0001
- NPSPEC-DATA-OBJECT-OWNERSHIP-0001
- NPSPEC-DATA-OBJECT-VIEW-0001
- NPSPEC-DATA-OBJECT-GRAPH-0001
- NPSPEC-DATA-OBJECT-SECURITY-0001
- NPSPEC-DATA-OBJECT-PROVENANCE-0001
- NPSPEC-DATA-PIPELINE-0001
- NPSPEC-DATA-PIPELINE-GRAPH-0001
- NPSPEC-DATA-PIPELINE-NODE-0001
- NPSPEC-DATA-PIPELINE-EDGE-0001
- NPSPEC-DATA-PIPELINE-SOURCE-0001
- NPSPEC-DATA-PIPELINE-SINK-0001
- NPSPEC-DATA-PIPELINE-TRANSFORM-0001
- NPSPEC-DATA-PIPELINE-CONVERTER-0001
- NPSPEC-DATA-PIPELINE-STREAM-0001
- NPSPEC-DATA-PIPELINE-BATCH-0001
- NPSPEC-DATA-PIPELINE-BACKPRESSURE-0001
- NPSPEC-DATA-PIPELINE-BUFFERING-0001
- NPSPEC-DATA-PIPELINE-ORDERING-0001
- NPSPEC-DATA-PIPELINE-SYNCHRONIZATION-0001
- NPSPEC-DATA-PIPELINE-SCHEDULING-0001
- NPSPEC-DATA-PIPELINE-PARALLELISM-0001
- NPSPEC-DATA-PIPELINE-FUSION-0001
- NPSPEC-DATA-PIPELINE-OPTIMIZATION-0001
- NPSPEC-DATA-PIPELINE-PLANNING-0001
- NPSPEC-DATA-PIPELINE-CONSTRUCTION-0001
- NPSPEC-DATA-PIPELINE-EXECUTION-0001
- NPSPEC-DATA-PIPELINE-LIFECYCLE-0001
- NPSPEC-DATA-PIPELINE-STATE-0001
- NPSPEC-DATA-PIPELINE-RECOVERY-0001
- NPSPEC-DATA-PIPELINE-CHECKPOINT-0001
- NPSPEC-DATA-PIPELINE-TRANSACTION-0001
- NPSPEC-DATA-PIPELINE-HOTREPLACE-0001
- NPSPEC-DATA-PIPELINE-REPLANNING-0001
- NPSPEC-DATA-PIPELINE-CACHE-0001
- NPSPEC-DATA-PIPELINE-QOS-0001
- NPSPEC-DATA-PIPELINE-REALTIME-0001
- NPSPEC-DATA-PIPELINE-DETERMINISM-0001
- NPSPEC-DATA-PIPELINE-SECURITY-0001
- NPSPEC-DATA-PIPELINE-TRUST-0001
- NPSPEC-DATA-PIPELINE-SOVEREIGNTY-0001
- NPSPEC-DATA-PIPELINE-INTROSPECTION-0001
- NPSPEC-DATA-PIPELINE-OBSERVABILITY-0001
- NPSPEC-DATA-PIPELINE-DECISIONTRACE-0001
- NPSPEC-DATA-PIPELINE-DATAMOVEMENT-0001
- NPSPEC-DATA-PIPELINE-ZEROCOPY-0001
- NPSPEC-DATA-PIPELINE-REMOTE-0001
- NPSPEC-DATA-PIPELINE-HARDWARE-0001
- NPSPEC-DATA-PIPELINE-PROVENANCE-0001
- NPSPEC-DATA-PIPELINE-INFORMATIONFLOW-0001
- NPSPEC-DATA-SEMANTICVIEW-0001
- NPSPEC-TYPE-COMPATIBILITY-0001
- NPSPEC-TYPE-CONVERSION-GRAPH-0001
- NPSPEC-IPC-TYPED-0001
- NPSPEC-IPC-ZEROCOPY-0001
- NPSPEC-EXECUTION-CONTRACT-0001
- NPSPEC-CAPABILITY-RESOLUTION-0001
- NPSPEC-RESOURCE-ECONOMY-0001

## Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch funktionale, semantische, Performance-, Failure- und Security-Tests überprüft werden.

### Pipeline-Graph-Tests

Zu prüfen sind:

```text
Linear Pipeline
Branching Pipeline
Joining Pipeline
Multiple Sources
Multiple Sinks
Dynamic Pipeline
```

### Type-Compatibility-Tests

Zu prüfen sind:

```text
Exact Match
Subtype Match
Lossless Conversion
Lossy Conversion
Incompatible Type
Unknown Type
```

### Object-Ownership-Tests

Zu prüfen sind:

```text
Exclusive Ownership
Shared Read
Ownership Transfer
Borrow
Copy-on-Write
Invalid Access after Transfer
```

### Lifetime-Tests

Zu prüfen sind:

- Consumer länger aktiv als Producer
- verzögerte Verarbeitung
- Branching
- Cancellation
- Provider Failure
- Pipeline Shutdown.

Es darf kein Use-after-Free entstehen.

### Zero-Copy-Tests

Zu prüfen ist mindestens:

```text
Producer:
    Image<RGBA8>

Consumer:
    Image<RGBA8>

Compatible Layout:
    yes

Security:
    compatible
```

Erwartung:

```text
Copy Count = 0
```

sofern die Hardware dies unterstützt.

### Copy-Fallback-Tests

Bei inkompatiblen Isolation Domains muss ein sicherer Copy-Pfad funktionieren.

### Conversion-Tests

Zu prüfen sind:

```text
RGBA8 → NV12
Float32 → Float16
CPU Layout → GPU Layout
```

einschließlich:

- Conversion Cost
- Loss Policy
- Error Handling.

### Pipeline-Fusion-Tests

Verglichen werden:

```text
A → B → C
```

und:

```text
FusedABC
```

Die semantischen Outputs müssen innerhalb des definierten Contracts äquivalent sein.

### Data-Movement-Tests

Der Planner muss unterschiedliche Pläne korrekt bewerten.

Beispiel:

```text
Plan A:
    CPU Compute = 1 ms
    Transfer = 5 ms

Plan B:
    GPU Compute = 3 ms
    Transfer = 0 ms
```

Erwartung:

```text
Plan B
```

wenn keine anderen Constraints dagegen sprechen.

### Backpressure-Tests

Zu prüfen sind:

```text
Fast Producer
Slow Consumer
```

für:

- Block
- DropOldest
- DropNewest
- Throttle
- SpillToStorage.

Unbounded Memory Growth darf bei bounded Pipelines nicht auftreten.

### Ordering-Tests

Zu prüfen sind:

```text
Strict
Unordered
Parallel
Reordered
```

### Cancellation-Tests

Cancellation muss:

- Tasks beenden,
- Buffers freigeben,
- Reservations freigeben,
- Capabilities freigeben.

### Deadline-Tests

Zu prüfen sind:

- Deadline Propagation
- Deadline Miss
- Provider Fallback
- Admission Failure.

### Resource-Budget-Tests

Pipelines müssen definierte:

- CPU,
- Memory,
- GPU,
- NPU,
- I/O,
- Network

Budgets respektieren.

### End-to-End-Tests

Nicht nur einzelne Node-Zeiten, sondern die vollständige Pipeline-Latenz muss gemessen werden.

### Provider-Selection-Tests

Zu prüfen sind:

```text
CPU
GPU
NPU
Remote
```

mit unterschiedlichen:

- Locations,
- Transfer Costs,
- Energy Costs,
- Trust Levels.

### Location-Tests

Remote Pipeline Execution muss:

- Network Failure
- Timeout
- Partition
- Provider Loss

korrekt behandeln.

### Sovereignty-Tests

Ein Objekt mit:

```text
GermanyOnly
```

darf nicht über einen nicht zulässigen Remote Provider verarbeitet werden.

### Security-Tests

Zu prüfen sind:

- unauthorized object access
- capability leakage
- write without right
- label stripping
- declassification without permission
- malicious plugin
- invalid shared buffer.

### Trust-Tests

Zu prüfen sind:

- trusted provider
- untrusted provider
- trust downgrade
- validation upgrade
- provenance chain.

### Transaction-Tests

Zu prüfen sind:

```text
Prepare
Apply
Validate
Commit
Rollback
```

sowie externe Side Effects.

### Retry-Tests

Nicht-idempotente Nodes dürfen nicht blind erneut ausgeführt werden.

### Recovery-Tests

Zu simulieren sind:

```text
Node Crash
Provider Crash
Device Loss
Memory Pressure
Network Failure
Invalid Output
Timeout
```

Recovery soll den kleinsten sicheren Scope verwenden.

### Hot-Replacement-Tests

Zu prüfen sind:

```text
Provider V1
    ↓
Provider V2
```

für:

- stateless node
- stateful node
- compatible state
- incompatible state
- migration failure
- rollback.

### Checkpoint-Tests

Nach Fehler muss eine Pipeline an einem gültigen Checkpoint fortsetzbar sein, sofern die Pipeline dies unterstützt.

### Determinism-Tests

Die gleiche deterministische Pipeline muss unter definierten Bedingungen reproduzierbare Outputs erzeugen.

### Replanning-Tests

Zu prüfen sind:

```text
Resource Pressure
Provider Failure
Thermal Pressure
Network Change
```

Der Planner darf nicht unkontrolliert zwischen Plänen oszillieren.

### Hysteresis-Tests

Kleine kurzfristige Performanceunterschiede dürfen nicht permanent Providerwechsel auslösen.

### Cache-Tests

Zu prüfen sind:

- Cache Hit
- Cache Miss
- invalid Capability Version
- changed Input
- Security Isolation
- Type Version Change.

### Lazy-Evaluation-Tests

Nicht konsumierte side-effect-freie Nodes sollen nicht ausgeführt werden müssen.

### Partial-Materialization-Tests

Nur benötigte Datenbereiche sollen materialisiert werden können, wenn die Datenstruktur dies unterstützt.

### Architecture-Introspection-Tests

Für eine laufende Pipeline müssen abhängig von Berechtigungen mindestens sichtbar sein können:

```text
Nodes
Edges
Semantic Types
Providers
Locations
Health
```

### Decision-Trace-Tests

Für automatische Entscheidungen muss ein Trace erklären können:

```text
Selected Provider
Rejected Providers
Conversions
Transfers
Constraint Reasons
```

### Copy-Trace-Tests

Jede relevante Copy soll einen nachvollziehbaren Grund besitzen können.

### Performance-Tests

Verglichen werden sollen mindestens:

```text
Traditional Copy Pipeline
vs.
NovaOS Zero-Copy Pipeline
```

für:

- Images
- Video
- Audio
- AI Tensor
- Storage → Compute
- Device → GPU.

### Stress-Tests

Zu testen sind:

- tausende Pipeline Nodes
- viele parallele Pipelines
- hohe Stream Rates
- große Objects
- kleine Objects
- häufiges Replanning
- Provider Churn.

### Fault-Injection

Fault-Injection SOLL mindestens simulieren:

```text
Corrupted Object Descriptor
Invalid Semantic Type
Broken Buffer
Provider Crash
Converter Crash
Network Partition
Resource Exhaustion
Security Denial
Pipeline Planner Failure
```

Fundamentale Systemstabilität darf dadurch nicht verloren gehen.

## Ergebnis

NovaOS behandelt Datenverarbeitung nicht als Folge voneinander isolierter APIs und Dateiformate.

Stattdessen entsteht ein gemeinsames semantisches Datenflussmodell:

```text
Source
   │
   ▼
Semantic Object
   │
   ▼
Capability
   │
   ▼
Semantic Object
   │
   ▼
Capability
   │
   ▼
Semantic Object
   │
   ▼
Sink
```

Die Ausführung wird durch:

```text
Semantic Types
        +
Capability Contracts
        +
Execution Contracts
        +
Resource Economy
        +
Zero-Copy
        +
Location Transparency
        +
Structured Concurrency
        +
Security / Trust
        +
Data Sovereignty
```

gesteuert.

Damit kann beispielsweise eine moderne Media-/AI-Pipeline logisch beschrieben werden als:

```text
Camera
    │
    │ VideoFrame<NV12>
    ▼
Decode
    │
    │ Image<RGBA8>
    ▼
Enhance
    │
    │ Image<RGBA8>
    ▼
AI Detection
    │
    │ DetectionResult
    ▼
Overlay
    │
    │ Image<RGBA8>
    ▼
Display
```

während NovaOS intern entscheiden kann:

```text
Camera DMA
      ↓
GPU0
      ↓ zero-copy
GPU Decoder
      ↓ zero-copy
NPU
      ↓
GPU Renderer
      ↓
Display
```

oder bei anderen Bedingungen:

```text
Camera
   ↓
CPU Decoder
   ↓
CPU Processing
   ↓
Display
```

ohne dass sich die fachliche Pipeline ändern muss.

Das verbindliche Architekturprinzip lautet:

```text
Semantic Objects flow through Capabilities.

Representations, providers and locations
are execution decisions whenever possible.
```

Damit bildet das einheitliche Daten- und Objekt-Pipelining die zentrale Verbindung zwischen dem semantischen Datenmodell und der ausführenden NovaOS-Architektur.