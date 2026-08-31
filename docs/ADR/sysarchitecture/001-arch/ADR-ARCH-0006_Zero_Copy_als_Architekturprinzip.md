# ADR-ARCH-0006 – Zero-Copy als Architekturprinzip

## Status

Angenommen

## Kategorie

Systemarchitektur / Datenpfade / Speicher / I/O / Performance

## Kontext

Moderne Betriebssysteme verarbeiten große Datenmengen über zahlreiche Subsystemgrenzen hinweg.

Typische Datenpfade umfassen beispielsweise:

- Storage → Dateisystem → Anwendung
- Netzwerk → Protokollstack → Anwendung
- Kamera → Bildverarbeitung → GPU
- Audioeingang → DSP → Anwendung
- Video → Decoder → Renderer
- Sensor → Analyse → Speicherung
- GPU → NPU → Anwendung
- Anwendung → Storage
- Anwendung → Netzwerk
- Prozess → Prozess
- Driver Domain → System Service
- lokale → entfernte Compute Provider

In klassischen Systemarchitekturen werden Daten auf solchen Pfaden häufig mehrfach kopiert.

Beispiel:

```text
Device Buffer
    ↓ Copy
Kernel Buffer
    ↓ Copy
Service Buffer
    ↓ Copy
Application Buffer
    ↓ Copy
GPU Buffer
```

Jede Kopie kann Ressourcen verbrauchen:

- CPU-Zeit
- Speicherbandbreite
- Cache-Kapazität
- Energie
- Latenz
- temporären Speicher
- Busbandbreite

Bei großen Datenströmen können Kopieroperationen einen erheblichen Anteil des gesamten Ressourcenverbrauchs verursachen.

Besonders relevant ist dies für:

- Video
- Audio
- KI
- Machine Learning
- Scientific Computing
- Imaging
- Medizinische Bildverarbeitung
- Storage
- Netzwerk
- Multimedia
- Grafik
- High Performance Computing
- große Dateien
- Remote Compute
- Sensorverarbeitung

NovaOS soll deshalb Daten nicht unnötig von einer Komponente zur nächsten kopieren.

Stattdessen wird **Zero-Copy beziehungsweise Copy Avoidance als systemweites Architekturprinzip** eingeführt.

Zero-Copy bedeutet dabei nicht, dass im gesamten Betriebssystem niemals Daten kopiert werden dürfen.

Kopien können weiterhin notwendig oder sinnvoll sein, beispielsweise wegen:

- Sicherheitsgrenzen
- Datenformatkonvertierung
- Hardwareanforderungen
- Alignment
- Isolation
- Lebensdauer
- Verschlüsselung
- Kompression
- Ownership-Wechsel
- fehlender Hardwareunterstützung
- Debugging
- Recovery

Das eigentliche Architekturprinzip lautet daher:

```text
Avoid unnecessary copies.
```

und nicht:

```text
Never copy data.
```

## Entscheidung

NovaOS verwendet **Zero-Copy als grundlegendes Architekturprinzip für Datenpfade**.

Systemkomponenten SOLLEN Daten nach Möglichkeit über:

- Shared Memory
- Shared Buffers
- Memory Mapping
- DMA Buffers
- Scatter/Gather
- Buffer Views
- Capability-geschützte Shared Objects
- Page Remapping
- Ownership Transfer
- Reference Passing

weitergeben, anstatt sie unnötig vollständig zu kopieren.

Das bevorzugte Modell lautet:

```text
Producer
    │
    ▼
Shared Data Object
    │
    ├── View → Consumer A
    ├── View → Consumer B
    └── View → Hardware
```

anstatt:

```text
Producer Buffer
    ↓ Copy
Consumer A Buffer
    ↓ Copy
Consumer B Buffer
    ↓ Copy
Hardware Buffer
```

Zero-Copy ist dabei ein Optimierungs- und Architekturziel.

Security, Correctness, Safety, Data Sovereignty, Determinism und definierte Ownership-Regeln besitzen Vorrang.

## Grundprinzip

NovaOS unterscheidet zwischen:

```text
Logical Data Transfer
```

und:

```text
Physical Data Copy
```

Eine logische Übergabe eines Objekts muss nicht automatisch eine physische Kopie seines Payloads bedeuten.

Beispiel:

```text
Send(Image)
```

kann intern bedeuten:

```text
Transfer Capability to Shared Buffer
```

anstatt:

```text
Copy all pixels into receiver memory
```

## Copy Avoidance

Der normative Architekturbegriff ist:

```text
Copy Avoidance
```

Zero-Copy beschreibt den optimalen Fall.

Copy Avoidance bedeutet:

```text
Eine Datenkopie wird nur durchgeführt,
wenn sie technisch, semantisch oder sicherheitlich
begründet ist.
```

Damit bleibt NovaOS flexibel genug für unterschiedliche Hardware- und Sicherheitsanforderungen.

## Datenobjektmodell

Daten sollen soweit möglich als Objekte betrachtet werden, deren Payload unabhängig von ihrer logischen Referenz verwaltet werden kann.

Ein vereinfachtes Modell lautet:

```text
DataObject {
    Identity
    Type
    Payload
    Metadata
    Ownership
    AccessRights
    Lifetime
    Provenance
}
```

Mehrere Komponenten können kontrollierte Views auf denselben Payload besitzen.

## Shared Data Object

Ein Shared Data Object repräsentiert Daten, die von mehreren Teilnehmern verwendet werden können.

Beispiel:

```text
SharedDataObject {
    objectId
    semanticType
    backingMemory
    size
    owner
    lifetime
    permissions
}
```

Die physische Speicherregion muss dabei nicht Bestandteil der öffentlichen API sein.

## Buffer Object

Für datenintensive Pfade verwendet NovaOS Buffer Objects.

Ein Buffer Object kann unter anderem enthalten:

```text
Buffer {
    Identity
    Size
    Alignment
    MemoryClass
    CachePolicy
    Ownership
    AccessRights
    Mappings
    DeviceMappings
    Lifetime
}
```

## Buffer Views

Ein Consumer muss nicht zwingend Zugriff auf den gesamten Buffer erhalten.

Stattdessen können Views verwendet werden.

Beispiel:

```text
Buffer:
    64 MiB

View:
    offset = 8 MiB
    length = 4 MiB
```

Dadurch können Teilbereiche weitergegeben werden, ohne Daten zu kopieren.

## Semantic Views

Views können zusätzlich semantische Informationen besitzen.

Beispiel:

```text
ImageView {
    buffer
    width
    height
    stride
    format
    colorSpace
}
```

oder:

```text
TensorView {
    buffer
    shape
    strides
    dataType
}
```

Damit bleiben Daten und semantische Struktur getrennt, ohne dass der Payload kopiert werden muss.

## Semantic Types

Zero-Copy wird mit den systemweiten Semantic Types kombiniert.

Ein Buffer allein beschreibt nur Speicher.

Ein Semantic Type beschreibt, was dieser Speicher bedeutet.

Beispiel:

```text
Buffer
```

plus:

```text
Image<RGBA8>
```

oder:

```text
Tensor<Float32, Shape<1,3,224,224>>
```

Dadurch können Provider prüfen, ob ein vorhandener Buffer direkt verwendbar ist.

## Unified Data/Object Pipeline

Zero-Copy ist Grundlage des einheitlichen Daten-/Objekt-Pipelining-Modells von NovaOS.

Beispiel:

```text
Camera
   ↓
Image<RAW>
   ↓
Demosaic
   ↓
Image<RGB>
   ↓
AI Detection
   ↓
Image + Objects
   ↓
Renderer
```

Die Pipeline soll nicht automatisch zwischen jedem Schritt vollständige Payload-Kopien erzeugen.

## Ownership

Zero-Copy benötigt ein explizites Ownership-Modell.

NovaOS unterscheidet mindestens:

```text
Owner
Reader
Writer
Mapper
Borrower
Device
```

Ownership bestimmt insbesondere:

- wer Lifetime kontrolliert
- wer Größe verändern darf
- wer schreiben darf
- wer freigeben darf
- wer Rechte weiterdelegieren darf

## Ownership Transfer

Ownership darf ohne Payload-Kopie übertragen werden.

Beispiel:

```text
Producer owns Buffer
        │
        ▼
Transfer Ownership
        │
        ▼
Consumer owns Buffer
```

Dies ist insbesondere bei Producer-Consumer-Pipelines effizient.

## Borrowing

Eine Komponente kann einen Buffer temporär verwenden, ohne dessen Ownership zu erhalten.

Beispiel:

```text
Owner
  │
  ├── Borrow ReadOnly → Consumer A
  └── Borrow ReadOnly → Consumer B
```

Die Lifetime-Regeln müssen garantieren, dass der Buffer während des Borrowing gültig bleibt.

## Read-Only Sharing

Read-only Sharing ist bevorzugt, wenn mehrere Komponenten dieselben Daten nur lesen müssen.

Beispiel:

```text
Shared Image
    ├── Thumbnail Generator
    ├── AI Analyzer
    └── Renderer
```

Alle drei können denselben Payload lesen.

## Writable Sharing

Mehrere Writer auf demselben Shared Buffer erfordern explizite Synchronisations- und Konsistenzregeln.

NovaOS DARF writable Shared Memory unterstützen.

Es darf jedoch nicht als Standardlösung für beliebige Datenpfade verwendet werden.

Bevorzugte Modelle sind:

- Single Writer
- Ownership Transfer
- Versioning
- Copy-on-Write
- Partitioned Writes

## Single-Writer-Prinzip

Für viele Zero-Copy-Pipelines SOLL gelten:

```text
One mutable owner at a time
```

Dadurch werden:

- Race Conditions
- Cache-Coherency-Probleme
- Synchronisationskosten

reduziert.

Mehrere Reader bleiben möglich.

## Immutable Objects

Immutable Objects eignen sich besonders gut für Zero-Copy.

Nach Erstellung werden sie nicht verändert.

Beispiel:

```text
Immutable Image Frame
```

Mehrere Komponenten können dasselbe Objekt ohne komplexe Schreibsynchronisation verwenden.

## Copy-on-Write

Wenn ein Consumer ein geteiltes Objekt verändern möchte, kann Copy-on-Write verwendet werden.

Beispiel:

```text
Shared Immutable Buffer
        │
        ▼
Consumer requests write
        │
        ▼
Copy only when needed
```

Damit wird eine Kopie nur im Änderungsfall erzeugt.

## Page Remapping

Große Datenbereiche können zwischen Address Spaces durch Page Mapping oder Remapping weitergegeben werden.

Beispiel:

```text
Process A Virtual Address
        │
        ▼
Physical Pages
        ▲
        │
Process B Virtual Address
```

Dadurch wird der physische Payload nicht kopiert.

## Shared Memory

Shared Memory ist ein zentraler Mechanismus für Zero-Copy über Prozess- und Domain-Grenzen.

Shared Memory muss:

- Capability-basiert autorisiert
- lifecycle-kontrolliert
- introspektierbar
- revokierbar, soweit möglich
- korrekt abgerechnet

sein.

## Capability-basierte Buffer

Zugriff auf Shared Buffers wird über Capabilities kontrolliert.

Beispiel:

```text
BufferCapability {
    object = Buffer42

    rights = [
        Read,
        Map
    ]
}
```

Ein anderer Teilnehmer kann besitzen:

```text
BufferCapability {
    object = Buffer42

    rights = [
        Read,
        Write,
        Map
    ]
}
```

## Capability Attenuation

Buffer-Rechte können bei Delegation abgeschwächt werden.

Beispiel:

```text
Owner:
    Read + Write + Delegate

Consumer:
    Read
```

Der Consumer kann dadurch keine Schreibrechte erlangen, die ihm nicht übertragen wurden.

## Capability Revocation

Zugriff auf Shared Data Objects muss widerrufbar sein, soweit die zugrunde liegende Speicher- und Hardwarearchitektur dies erlaubt.

Revocation kann beispielsweise bedeuten:

- Mapping entfernen
- Capability invalidieren
- IOMMU Mapping entfernen
- neue Version erzeugen
- Domain beenden

Revocation einer CPU-seitigen Capability allein reicht nicht aus, wenn ein Gerät weiterhin DMA-Zugriff besitzt.

## DMA

Direct Memory Access ist zentral für Zero-Copy-I/O.

Ein Gerät kann direkt auf freigegebene Speicherregionen zugreifen.

Beispiel:

```text
Storage Device
      │ DMA
      ▼
Shared Buffer
      │
      ▼
Application
```

anstatt:

```text
Storage Device
      ↓
Kernel Buffer
      ↓ Copy
Application Buffer
```

## IOMMU

DMA-fähige Geräte SOLLEN über IOMMU-Mappings auf die benötigten Speicherbereiche begrenzt werden.

Beispiel:

```text
Device
   │
   ▼
IOMMU
   │
   ▼
Allowed Buffer Pages
```

Das Gerät erhält keinen uneingeschränkten Zugriff auf den physischen Speicher.

## DMA Buffer Lifecycle

Ein DMA Buffer darf nicht freigegeben oder physisch verschoben werden, solange ein Gerät darauf zugreifen kann.

Der Lifecycle muss deshalb Zustände unterstützen wie:

```text
Allocated
Mapped
Submitted
InFlight
Completed
Unmapped
Released
```

## Scatter/Gather

NovaOS SOLL Scatter/Gather-I/O unterstützen.

Eine logische Datenstruktur kann dadurch aus mehreren physischen Speicherregionen bestehen.

Beispiel:

```text
Logical Buffer
    ├── Page A
    ├── Page B
    ├── Page C
    └── Page D
```

Ein kompatibles Gerät kann diese direkt verarbeiten.

Dadurch kann eine vorherige lineare Zusammenkopie vermieden werden.

## DMA-BUF-artiges Modell

NovaOS SOLL ein natives, providerunabhängiges Modell für gemeinsam nutzbare Device Buffers besitzen.

Es soll funktional ermöglichen:

```text
Allocate once
    ↓
Share
    ↓
Map to Device A
    ↓
Map to Device B
    ↓
Map to Process
```

Die konkrete ABI wird durch separate NPSPECs definiert.

Das Modell soll nicht an eine fremde Betriebssystem-ABI gebunden sein.

## Device-local Memory

Nicht jeder Buffer befindet sich im normalen RAM.

Mögliche Memory Classes:

```text
System RAM
Pinned RAM
GPU VRAM
NPU Memory
Device Local Memory
Unified Memory
Persistent Memory
Remote Memory
```

Das Zero-Copy-Modell muss unterschiedliche Memory Classes berücksichtigen.

## Unified Memory

Wenn Hardware Unified Memory unterstützt, darf NovaOS denselben Speicherbereich mehreren Compute Units zugänglich machen.

Dies kann CPU, GPU und NPU umfassen.

Unified Memory bedeutet jedoch nicht automatisch:

- gleiche Latenz
- gleiche Bandbreite
- gleiche Cache-Semantik
- gleiche Sicherheitsdomäne

Diese Eigenschaften bleiben Teil des Resource Models.

## Memory Placement

Der physische Speicherort eines Shared Objects darf durch Policies bestimmt werden.

Beispiel:

```text
CPU-heavy workload:
    System RAM

GPU-heavy workload:
    GPU-local memory

Mixed workload:
    Unified memory
```

Die Wahl muss Execution Contract und Resource Economy berücksichtigen.

## Migration

Ein Buffer darf zwischen Memory Classes migriert werden.

Beispiel:

```text
System RAM
    ↓
GPU VRAM
```

Eine Migration kann eine physische Kopie erfordern.

Eine solche Kopie ist zulässig, wenn sie für den optimalen Datenpfad notwendig ist.

Zero-Copy bedeutet nicht, Migrationen grundsätzlich zu verbieten.

## Cache Coherency

Shared Memory zwischen verschiedenen Prozessoren oder Geräten benötigt definierte Cache-Coherency-Regeln.

NovaOS muss unterscheiden können zwischen:

```text
Coherent
NonCoherent
ExplicitFlushRequired
DeviceManaged
```

Ein Consumer darf nicht davon ausgehen, dass alle Shared Buffers automatisch cache-coherent sind.

## Synchronisation

Zero-Copy benötigt Synchronisation zwischen Producer und Consumer.

Mögliche Mechanismen:

- Fences
- Events
- Semaphores
- Completion Objects
- Timeline Semaphores
- Sequence Numbers

Beispiel:

```text
Producer writes
    ↓
Signal Fence
    ↓
Consumer waits
    ↓
Consumer reads
```

## Completion-basierte Ausführung

Asynchrone Zero-Copy-Pipelines sollen bevorzugt Completion-basierte Mechanismen verwenden.

Beispiel:

```text
Submit(Buffer)
    ↓
Continue Execution
    ↓
Completion Event
```

anstatt den Thread während der gesamten Hardwareoperation zu blockieren.

## Buffer State

Ein Buffer kann Zustände besitzen wie:

```text
Free
Writable
Submitted
InFlight
Readable
Recyclable
```

Ungültige Übergänge sollen erkannt werden.

## Ring Buffers

Für kontinuierliche Datenströme SOLLEN Ring Buffers unterstützt werden.

Beispiele:

- Audio
- Video Capture
- Netzwerk
- Telemetrie
- Sensoren

Ein Ring Buffer vermeidet wiederholte Allokation und Kopien.

## Buffer Pools

Häufig wiederverwendete Buffer SOLLEN aus Pools bereitgestellt werden können.

Beispiel:

```text
Pool
    ├── Buffer 1
    ├── Buffer 2
    ├── Buffer 3
    └── Buffer 4
```

Nach Abschluss werden Buffer zurückgegeben und wiederverwendet.

Dies reduziert:

- Allocation Overhead
- Fragmentierung
- Cache Misses
- Page Mapping Overhead

## Backpressure

Zero-Copy-Pipelines müssen Backpressure unterstützen.

Wenn ein Consumer langsamer ist als der Producer, dürfen Buffer nicht unbegrenzt akkumulieren.

Beispiel:

```text
Producer
   ↓
Queue full
   ↓
Backpressure
   ↓
Producer slows / drops / waits
```

Die konkrete Reaktion wird durch Policy und Execution Contract bestimmt.

## Bounded Queues

Zwischen Pipeline-Komponenten SOLLEN begrenzte Queues verwendet werden können.

Dadurch wird verhindert, dass ein schneller Producer unbegrenzt Speicher belegt.

## Frame Dropping

Bei Echtzeit-Multimedia kann Frame Dropping eine zulässige Degradation sein.

Beispiel:

```text
Video:
    freshness > completeness
```

Eine Policy kann dann alte Frames verwerfen statt sie zu kopieren oder zu puffern.

Bei wissenschaftlichen oder sicherheitskritischen Daten kann dies dagegen unzulässig sein.

## Zero-Copy IPC

NovaOS SOLL Zero-Copy IPC unterstützen.

Das Grundmodell lautet:

```text
Sender
   │
   │ Capability + Metadata
   ▼
Shared Object
   ▲
   │
Receiver
```

Die IPC-Nachricht enthält nicht zwingend den vollständigen Payload.

Sie kann stattdessen Referenzen beziehungsweise Capabilities auf Datenobjekte enthalten.

## Typed IPC

Typed IPC und Zero-Copy werden kombiniert.

Beispiel:

```text
Message {
    frame: VideoFrame<NV12>
}
```

Intern kann `frame` ein Capability-geschützter View auf einen Shared Buffer sein.

Dadurch bleibt die API semantisch typisiert.

## Serialization Avoidance

Wenn Sender und Empfänger kompatible interne Repräsentationen verwenden, SOLL unnötige Serialisierung vermieden werden.

Dies gilt insbesondere lokal.

Bei:

- Remote IPC
- Persistierung
- inkompatiblen ABI-Versionen
- Security Boundaries

kann Serialisierung notwendig bleiben.

## Zero-Copy über Isolation Domains

Starke Isolation und Zero-Copy schließen sich nicht aus.

Das bevorzugte Modell lautet:

```text
Isolated Domain A
      │
      │ Capability
      ▼
Shared Buffer
      ▲
      │ Capability
Isolated Domain B
```

Die Domains teilen nicht ihren gesamten Address Space.

Sie teilen ausschließlich explizit autorisierte Objekte.

## Driver Domains

User-Mode und isolierte Driver Domains müssen Zero-Copy-fähige Datenpfade unterstützen.

Beispiel:

```text
Device
   │ DMA
   ▼
Shared Buffer
   │
   ├── Driver Domain
   └── Application Domain
```

Dadurch muss Driver Isolation nicht automatisch mehrere Kopien verursachen.

## Kernel

Der Kernel stellt fundamentale Mechanismen bereit für:

- Shared Memory Objects
- Page Mapping
- Capability Enforcement
- DMA Mapping
- IOMMU Mapping
- Synchronisation
- Lifetime Tracking

Der Kernel soll Payloads nicht unnötig selbst transformieren oder kopieren.

## Kernel Crossing

Ein Syscall oder IPC-Übergang muss nicht bedeuten, dass Daten in einen Kernelbuffer kopiert werden.

Für große Payloads sollen Descriptor-, Mapping- oder Shared-Object-Modelle verwendet werden.

Kleine Kontrollnachrichten dürfen weiterhin direkt kopiert werden, wenn dies effizienter ist.

## Small Data Optimization

Zero-Copy ist nicht automatisch schneller für sehr kleine Datenmengen.

Für kleine Nachrichten kann eine direkte Kopie günstiger sein als:

- Mapping
- Capability-Erzeugung
- Page Management
- Synchronisation

NovaOS darf deshalb eine größenabhängige Policy verwenden.

Beispiel:

```text
Small payload:
    Copy

Large payload:
    Shared Buffer
```

## Copy Threshold

Subsysteme DÜRFEN einen dynamischen Copy Threshold verwenden.

Dieser kann abhängig sein von:

- Payload-Größe
- CPU
- Memory Pressure
- Cache
- Hardware
- Isolation Boundary
- Current Load

Der Threshold ist Policy und nicht Teil der semantischen API.

## Netzwerk

Netzwerkpfade SOLLEN möglichst wenige Kopien verwenden.

Beispiel:

```text
NIC
  │ DMA
  ▼
Packet Buffer
  │
  ▼
Network Stack
  │
  ▼
Application View
```

Bei geeigneter Hardware kann der gleiche Datenbereich entlang mehrerer Netzwerkstufen weitergereicht werden.

## Packet Buffers

Netzwerkdaten sollen über referenzierbare Packet Buffers repräsentiert werden können.

Headeroperationen sollen nach Möglichkeit nicht den vollständigen Payload kopieren.

## Scatter/Gather Networking

Sendepfade SOLLEN Scatter/Gather unterstützen.

Beispiel:

```text
Header Buffer
Payload Buffer
Trailer Buffer
```

können gemeinsam als eine logische Nachricht übertragen werden.

## Storage

Storage-I/O SOLL direkte Transfers zwischen Storage Device und autorisierten Zielbuffern ermöglichen.

Beispiel:

```text
NVMe
  │ DMA
  ▼
Application Buffer
```

sofern:

- Alignment
- Security
- IOMMU
- Filesystem Semantics
- Encryption
- Integrity

dies zulassen.

## Direct I/O

NovaOS DARF Direct-I/O-Modi bereitstellen.

Direct I/O ist jedoch nicht automatisch der Standard.

Page Cache und andere Cachingmechanismen können weiterhin sinnvoll sein.

Das Ziel lautet nicht:

```text
Always bypass caches
```

sondern:

```text
Avoid redundant copies.
```

## Page Cache

Page Cache und Zero-Copy sollen zusammenarbeiten können.

Ein File Mapping kann beispielsweise direkt auf gecachte Pages verweisen.

Dadurch kann eine zusätzliche Kopie in einen Anwendungsbuffer vermieden werden.

## Memory-Mapped Files

Memory Mapping SOLL für geeignete Dateizugriffe unterstützt werden.

Beispiel:

```text
File
  ↓
Page Cache
  ↓
Mapped into Application
```

Die Anwendung arbeitet direkt auf den gemappten Pages.

## Sendfile-artige Operationen

NovaOS SOLL Datenübertragungen zwischen Storage und Netzwerk ohne unnötige Übergabe durch Anwendungsbuffer ermöglichen.

Beispiel:

```text
File
  ↓
Storage/Page Cache
  ↓
Network
```

Die konkrete Implementierung kann:

- DMA
- Page References
- Scatter/Gather

verwenden.

## Grafik

Grafikdaten sollen möglichst ohne unnötige Kopien zwischen:

- Application
- Compositor
- GPU
- Display Engine

übergeben werden.

Beispiel:

```text
Application Surface
      │
      ▼
Compositor
      │
      ▼
Display
```

Dabei können Buffer Handles beziehungsweise Capabilities statt Pixelkopien übertragen werden.

## Compositor

Der Compositor SOLL Shared Surface Buffers unterstützen.

Anwendungen übergeben:

- Surface
- Damage Regions
- Synchronisation

anstatt vollständige Frames über IPC zu kopieren.

## Video

Video-Pipelines sind ein primärer Zero-Copy-Anwendungsfall.

Beispiel:

```text
Storage / Network
        ↓
Compressed Buffer
        ↓
Hardware Decoder
        ↓
Decoded Surface
        ↓
GPU
        ↓
Compositor
        ↓
Display
```

Idealerweise wird der große Video-Payload nicht zwischen jeder Stufe vollständig kopiert.

## Kamera

Kamera-Pipelines können beispielsweise lauten:

```text
Camera Sensor
      ↓ DMA
Capture Buffer
      ↓
Image Processing
      ↓
AI
      ↓
Renderer / Storage
```

Die gleichen Frame Buffers können kontrolliert zwischen Komponenten weitergegeben werden.

## Audio

Audio verwendet häufig Ring Buffers.

Beispiel:

```text
Audio Device
     ↓
Ring Buffer
     ↓
DSP
     ↓
Mixer
     ↓
Output
```

Die Buffergröße muss Latenz- und Realtime-Anforderungen berücksichtigen.

## KI und Machine Learning

Tensoren können sehr groß sein.

NovaOS SOLL deshalb Zero-Copy beziehungsweise Copy Avoidance zwischen:

- CPU
- GPU
- NPU
- AI Runtime
- Application

unterstützen.

Beispiel:

```text
Image Buffer
    ↓
Tensor View
    ↓
NPU
```

Eine Konvertierung darf vermieden werden, wenn der bestehende Speicher bereits das erforderliche Layout besitzt.

## Tensor Layout

Provider können bestimmte Layouts verlangen.

Beispiele:

```text
NCHW
NHWC
Blocked
Quantized
```

Wenn eine Layoutkonvertierung erforderlich ist, kann eine Kopie notwendig werden.

Der Planner SOLL solche Kosten berücksichtigen.

## Layout-aware Planning

Providerwahl kann berücksichtigen, ob Daten bereits im passenden Format und Memory Space vorliegen.

Beispiel:

```text
GPU Provider:
    execution = 2 ms
    conversion = 8 ms

CPU Provider:
    execution = 6 ms
    conversion = 0 ms
```

Dann kann CPU insgesamt schneller sein.

Zero-Copy ist deshalb mit der Resource Economy und Providerwahl zu verbinden.

## Format Conversion

Eine Formatkonvertierung ist keine unnötige Kopie, wenn der Consumer ein anderes Format zwingend benötigt.

NovaOS soll jedoch prüfen können, ob:

- ein anderer Provider das bestehende Format direkt unterstützt,
- eine View ausreicht,
- eine In-Place-Konvertierung möglich ist.

## In-Place Operations

Algorithmen DÜRFEN In-Place-Verarbeitung unterstützen.

Beispiel:

```text
Transform(buffer, inPlace = true)
```

Dies kann Speicherverbrauch und Kopien reduzieren.

In-Place-Operationen sind jedoch nur zulässig, wenn:

- Ownership eindeutig ist,
- keine Reader die alte Version benötigen,
- Datenabhängigkeiten dies erlauben.

## Pipeline Fusion

NovaOS DARF mehrere Verarbeitungsschritte fusionieren, um Zwischenbuffer zu vermeiden.

Beispiel:

```text
Decode
    ↓
Resize
    ↓
Normalize
```

kann möglicherweise als:

```text
FusedDecodeResizeNormalize
```

ausgeführt werden.

Pipeline Fusion ist eine Optimierung und darf semantische Anforderungen nicht verändern.

## Provider Fusion

Ein Provider darf mehrere Capabilities gemeinsam implementieren.

Dadurch können Daten innerhalb derselben Hardware verbleiben.

Beispiel:

```text
GPU:
    Decode
    Filter
    Scale
    Render
```

anstatt nach jedem Schritt zwischen CPU und GPU zu transferieren.

## Execution Contract

Zero-Copy-Anforderungen können Bestandteil des Execution Contracts sein.

Beispiel:

```text
ExecutionContract {
    latency <= 5 ms

    dataMovement {
        preferZeroCopy = true
    }
}
```

Für spezielle Anwendungen kann ein stärkeres Constraint existieren:

```text
dataMovement {
    hostCopy = Forbidden
}
```

Ein solches hartes Constraint darf nur akzeptiert werden, wenn die Plattform es tatsächlich erfüllen kann.

## Data Movement Cost

Die Resource Economy SOLL Datenbewegung als Kostenfaktor berücksichtigen.

Dazu gehören:

- Bytes Copied
- Memory Bandwidth
- PCIe Traffic
- Network Traffic
- Energy
- Latency

Providerentscheidungen sollen nicht nur Compute-Zeit betrachten.

## Memory Bandwidth

Speicherbandbreite ist eine endliche Ressource.

Unnötige Kopien können andere Workloads beeinträchtigen.

Zero-Copy reduziert daher nicht nur die Latenz des eigenen Workloads, sondern kann systemweite Bandbreite freisetzen.

## Energie

Datenbewegung benötigt Energie.

Insbesondere auf mobilen und heterogenen Systemen kann Memory Movement einen erheblichen Energieanteil verursachen.

Copy Avoidance ist deshalb Bestandteil der systemweiten Energieoptimierung.

## Thermal Effects

Weniger Speicher- und Busverkehr kann thermische Belastung reduzieren.

Zero-Copy kann damit indirekt Thermal Headroom erhöhen.

## Resource Accounting

Zero-Copy-Objekte müssen korrekt abgerechnet werden.

Ein physischer Buffer darf nicht für jeden Mapping-Teilnehmer vollständig als zusätzlicher physischer Speicher gezählt werden.

NovaOS unterscheidet daher mindestens:

```text
Physical Consumption
Virtual Mapping
Reservation
Ownership
```

## Shared Resource Charging

Für Shared Objects muss eine Policy bestimmen, wem Ressourcenverbrauch zugerechnet wird.

Mögliche Modelle:

- Owner pays
- Producer pays
- Consumer pays
- proportional sharing
- capability-specific accounting

Die physische Nutzung darf dabei nicht mehrfach erzeugt werden.

## Memory Pressure

Shared Buffers müssen am Memory-Pressure-System teilnehmen.

Ein Buffer darf nicht allein deshalb unreclaimable bleiben, weil mehrere veraltete Referenzen existieren.

Lifetime und Ownership müssen deshalb streng verwaltet werden.

## Buffer Reclamation

Ein Buffer kann freigegeben werden, wenn:

```text
No Owner
AND
No Active Borrow
AND
No Device Access
AND
No Mapping requiring persistence
```

Die genaue Semantik wird durch die Memory-NPSPECs definiert.

## Leaks

Zero-Copy erhöht die Bedeutung korrekter Lifetime-Verwaltung.

Mögliche Fehler:

- verlorene Buffer Capabilities
- nicht freigegebene Device Mappings
- Zombie Buffers
- dauerhaft gepinnter Speicher
- stale Views

NovaOS SOLL solche Zustände introspektieren und erkennen können.

## Pinned Memory

Pinned Memory darf nicht unkontrolliert verwendet werden.

Da gepinnter Speicher:

- nicht migrierbar,
- häufig nicht reclaimbar

ist, muss er in der Resource Economy besonders berücksichtigt werden.

Pinned Memory Budgets sollen begrenzt werden können.

## Security

Zero-Copy darf Sicherheitsgrenzen nicht schwächen.

Ein Shared Buffer zwischen zwei Domains darf nicht automatisch andere Speicherbereiche offenlegen.

NovaOS MUSS:

- Range Checks
- Access Rights
- Mapping Permissions
- Capability Enforcement

durchsetzen.

## Information Flow

Shared Memory kann einen direkten Informationsfluss erzeugen.

Data-Sovereignty- und Information-Flow-Policies müssen deshalb auch auf Zero-Copy-Verbindungen angewendet werden.

Ein Buffer darf nicht an eine Domain delegiert werden, wenn diese den enthaltenen Datentyp nicht erhalten darf.

## Data Sovereignty

Zero-Copy über Remote Memory oder Remote Compute muss Data-Sovereignty-Regeln berücksichtigen.

Beispiel:

```text
Buffer:
    sovereignty = LocalOnly
```

Dann darf dieser Buffer nicht allein aus Performancegründen an einen RemoteGPU-Provider übertragen werden.

## Confidential Data

Sensitive Buffer können zusätzliche Anforderungen besitzen.

Beispiele:

- encrypted memory
- confidential domain
- restricted mapping
- no CPU mapping
- device-only access

Das Zero-Copy-Modell muss solche Constraints unterstützen können.

## Encryption

Verschlüsselung kann Zero-Copy erschweren.

Wenn ein Consumer Plaintext benötigt, kann eine Transformation notwendig sein.

NovaOS soll hardwaregestützte In-Place- oder Direct-Decrypt-Pfade nutzen können, sofern Security und Hardware dies erlauben.

## Trust

Shared Buffers dürfen nur an Provider delegiert werden, deren Trust Level den Anforderungen entspricht.

Beispiel:

```text
Buffer:
    trustRequirement = SystemVerified
```

Ein untrusted Provider darf keine Capability auf diesen Buffer erhalten.

## Provenance

Datenprovenance muss bei Zero-Copy erhalten bleiben können.

Wenn mehrere Views auf denselben Payload erzeugt werden, verlieren die Daten dadurch nicht ihre Provenance-Information.

## NovaFile

NovaFile kann Payload und Metadaten getrennt repräsentieren.

Große Payloads SOLLEN ohne unnötige Kopien in Verarbeitungspipelines eingebunden werden können.

Beispiel:

```text
NovaFile
   │
   ├── Metadata
   └── Payload Object
           │
           ▼
       Shared View
```

## Persistierung

Ein Shared In-Memory Object ist nicht automatisch persistent.

Wenn Persistierung notwendig ist, muss ein Storage Commit erfolgen.

Zero-Copy darf die Unterscheidung zwischen:

```text
Memory Lifetime
```

und:

```text
Persistent Lifetime
```

nicht verwischen.

## Transaktionen

Transaktionale Systemoperationen können Zero-Copy-Objekte verwenden.

Eine Transaktion darf beispielsweise eine neue Version eines großen Objekts über Copy-on-Write erzeugen, anstatt sofort den gesamten Payload zu duplizieren.

## Snapshotting

Snapshots SOLLEN Page Sharing und Copy-on-Write verwenden können.

Beispiel:

```text
Version 1
    │
    ├── Shared Pages
    │
Version 2
```

Nur geänderte Pages müssen dupliziert werden.

## Hot Replacement

Hot Replacement kann State Transfer über Shared State Objects durchführen.

Beispiel:

```text
Old Service
     │
     │ Shared State Capability
     ▼
State Object
     ▲
     │
New Service
```

Dadurch kann ein großer Zustand ohne vollständige Serialisierung und Kopie übergeben werden.

## Live Evolution

Versionierte Komponenten müssen vereinbaren können, welche Shared-Object-Layouts kompatibel sind.

Falls Layouts inkompatibel sind, kann eine Transformation notwendig werden.

Eine solche Transformation muss explizit sein.

## Compatibility Domains

Legacy APIs können traditionell Copy-based arbeiten.

NovaOS darf intern dennoch Zero-Copy-Optimierungen einsetzen.

Kompatibilität darf jedoch keine sichtbare Semantik brechen.

## ABI

Die native NovaABI SOLL Zero-Copy-fähige Objekt- und Buffer-Handles unterstützen.

Große Datenobjekte sollen nicht grundsätzlich als inline kopierte Syscall-Argumente modelliert werden.

## FFI

Foreign Function Interfaces können zusätzliche Kopien verursachen.

NovaLang- und NovaOS-FFI-Spezifikationen SOLLEN daher Möglichkeiten für:

- borrowed memory
- pinned views
- foreign buffer handles
- ownership transfer

vorsehen.

## Language Runtime

NovaLang und andere unterstützte Runtimes sollen Zero-Copy-Objekte sicher repräsentieren können.

Die Sprachintegration muss Lifetime- und Ownership-Regeln respektieren.

## Memory Safety

Memory-safe Sprachen dürfen Shared Buffers nicht durch unkontrollierte rohe Pointerzugriffe unsicher machen.

Sichere Abstraktionen sollen:

- Range
- Lifetime
- Mutability
- Ownership

repräsentieren.

## Unsafe Access

Low-Level-Komponenten dürfen unsafe Buffer Access verwenden, wenn technisch erforderlich.

Solche Bereiche SOLLEN:

- minimal
- lokal
- explizit
- testbar
- überprüfbar

sein.

## Deterministic Mode

Zero-Copy ist grundsätzlich mit Deterministic Mode kompatibel.

Allerdings können adaptive:

- Buffer Placement
- Migration
- Copy Thresholds
- Provider Fusion

im Deterministic Mode eingeschränkt werden.

Reproduzierbare Ausführung kann feste:

- Memory Classes
- Buffer Pools
- Provider
- Synchronisationspfade

erfordern.

## Realtime

Zero-Copy ist besonders für Realtime-Pfade relevant, weil zusätzliche Kopien:

- Latenz
- Jitter
- Memory Pressure

erzeugen können.

Realtime Buffer können:

- vorallokiert
- gepinnt
- aus festen Pools
- ohne dynamische Migration

bereitgestellt werden.

## Hard Realtime

Hard-Realtime-Pipelines dürfen nicht von unbeschränkter dynamischer Buffer-Allokation abhängen.

Benötigte Buffer SOLLEN vor Ausführung reserviert werden können.

## Structured Concurrency

Buffer Lifetimes SOLLEN an strukturierte Task Lifetimes gekoppelt werden können.

Beispiel:

```text
Pipeline Task
    ├── Decode Task
    ├── Filter Task
    └── Render Task
```

Beim Ende des Parent-Kontexts können nicht mehr benötigte temporäre Buffer kontrolliert freigegeben werden.

## Cancellation

Bei Cancellation müssen In-Flight Buffer korrekt behandelt werden.

Ein Buffer darf nicht freigegeben werden, solange:

- Hardwarezugriff aktiv ist,
- ein anderer Task eine gültige Borrow besitzt.

Cancellation muss deshalb mit Completion und Lifetime Management koordiniert werden.

## Self-Healing

Nova.Resilience muss Zero-Copy-Ressourcen berücksichtigen.

Wenn eine Driver Domain abstürzt, muss NovaOS beispielsweise:

- IOMMU-Mappings entfernen
- Buffer-Rechte widerrufen
- In-Flight-Operationen klassifizieren
- Buffer zurückgewinnen
- Provider neu binden

können.

## Failure Domains

Ein Shared Buffer darf Failure Domains nicht unkontrolliert miteinander koppeln.

Ein Crash eines Consumers darf den Owner des Buffers nicht zwangsläufig kompromittieren.

## Corruption

Wenn ein Writer einen Shared Buffer beschädigt, können alle Reader betroffen sein.

Deshalb müssen:

- Write Permissions
- Ownership
- Integrity
- Versioning

klar definiert sein.

Für untrusted Domains können Copy- oder Validation-Boundaries bewusst sinnvoller sein als writable Sharing.

## Integrity

Shared Objects können Integrity-Metadaten besitzen.

Beispiele:

- hash
- checksum
- authenticated metadata
- version

Die Verwendung hängt vom Datentyp und Trust Model ab.

## When Copy Is Required

NovaOS MUSS Kopien zulassen, wenn sie technisch oder semantisch notwendig sind.

Beispiele:

### Security Boundary

Ein untrusted Consumer soll nur bereinigte Daten erhalten.

### Format Conversion

Ein Consumer benötigt anderes Pixel-, Tensor- oder Audioformat.

### Alignment

Hardware benötigt anderes Alignment.

### Lifetime Decoupling

Consumer benötigt Daten länger als der ursprüngliche Owner.

### Immutable Snapshot

Eine unveränderliche historische Version wird benötigt.

### Remote Transfer

Daten müssen über ein Netzwerk serialisiert werden.

### Encryption Boundary

Daten müssen verschlüsselt oder entschlüsselt werden.

### Isolation

Gemeinsamer Speicher würde die gewünschte Isolation verletzen.

### Small Payload

Eine direkte Kopie ist günstiger als Shared-Memory-Management.

## Copy Reason

Systemkomponenten SOLLEN bei relevanten Datenpfaden Gründe für notwendige Kopien klassifizieren können.

Beispiele:

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

Dies verbessert Architecture Introspection und Performanceanalyse.

## Copy Accounting

NovaOS SOLL Datenkopien messbar machen können.

Mögliche Metriken:

```text
BytesCopied
CopyCount
CopyLatency
MemoryBandwidthUsed
CopyReason
```

Dadurch kann sichtbar werden, wo Zero-Copy-Ziele nicht erreicht werden.

## Data Movement Introspection

Architecture Introspection soll Datenpfade darstellen können.

Beispiel:

```text
Camera
  │ DMA
  ▼
Buffer #71
  │ ZeroCopy
  ▼
AI Provider
  │ ZeroCopy
  ▼
GPU Renderer
```

oder:

```text
Decoder
  │
  │ Copy: FormatConversion
  ▼
Renderer
```

## Decision Tracing

Wenn NovaOS zwischen Copy und Zero-Copy entscheidet, SOLL die Entscheidung nachvollziehbar sein.

Beispiel:

```text
Operation:
    Send Buffer to GPU0

Decision:
    Copy

Reason:
    Source memory not GPU-addressable

Alternative:
    Shared mapping unavailable
```

## Resource Economy Integration

Copy-Kosten sollen in Resource Planning berücksichtigt werden.

Beispiel:

```text
Provider A:
    compute = 2 ms
    copy    = 8 ms

Provider B:
    compute = 5 ms
    copy    = 0 ms

Selected:
    Provider B
```

Eine reine Compute-Benchmark reicht deshalb nicht für Providerwahl.

## Adaptive Optimization

NovaOS darf lernen, welche Datenpfade auf einer bestimmten Hardware am effizientesten sind.

Beispielsweise kann Prediction Error für Copy-Cost-Modelle verwendet werden.

Beispiel:

```text
Predicted transfer:
    2 ms

Observed:
    6 ms

Update model
```

Adaptive Modelle dürfen jedoch harte Constraints nicht verletzen.

## Architecture Introspection

Zero-Copy-Objekte müssen introspektierbar sein.

Beispiel:

```text
Buffer:
    Buffer42

SemanticType:
    VideoFrame<NV12>

Size:
    8 MiB

Owner:
    VideoDecoder

Readers:
    AIAnalyzer
    Renderer

MemoryClass:
    GPUShared

Mappings:
    GPU0
    NPU0

PhysicalCopies:
    0

State:
    ReadOnly
```

## Observability

Mögliche Zero-Copy-Metriken umfassen:

- Shared Buffer Count
- Buffer Pool Utilization
- Physical Bytes
- Virtual Mappings
- Pinned Bytes
- DMA Mappings
- Copies Avoided
- Copies Performed
- Copy Reasons
- Data Movement Latency
- Buffer Lifetime
- Buffer Leaks

## Debugging

Debugging darf Zero-Copy nicht grundsätzlich deaktivieren müssen.

Für Diagnosezwecke können jedoch Debug Copies oder Snapshot Copies erlaubt sein.

Solche Kopien müssen als Diagnosekosten sichtbar sein.

## Validation

Untrusted Daten können trotz Zero-Copy validiert werden.

Validation muss nicht zwingend eine vollständige Kopie erfordern.

Beispiele:

- bounds checking
- metadata validation
- hash verification
- structural validation

Wenn Sanitization eine neue Repräsentation benötigt, ist eine Kopie zulässig.

## Boot

Frühe Bootphasen dürfen einfachere Copy-basierte Datenpfade verwenden.

Die vollständige Zero-Copy-Infrastruktur ist keine Voraussetzung für den allerersten Bootcode.

Sobald die entsprechenden Kernel- und Memory-Subsysteme aktiv sind, sollen reguläre NovaOS-Datenpfade das Zero-Copy-Modell verwenden können.

## Recovery

NovaDOS muss nicht die vollständige Zero-Copy-Infrastruktur des normalen NovaOS implementieren.

Recovery Correctness besitzt Vorrang vor maximaler Datenpfadperformance.

## Normative Festlegungen

1. NovaOS MUSS Zero-Copy beziehungsweise Copy Avoidance als systemweites Architekturprinzip für datenintensive Pfade verwenden.

2. Das normative Ziel lautet Copy Avoidance und NICHT absolutes Copy Prohibition.

3. Datenkopien MÜSSEN zulässig bleiben, wenn sie technisch, semantisch oder sicherheitlich notwendig sind.

4. Große Payloads SOLLEN bevorzugt über Shared Objects, Buffer Handles oder Capabilities weitergegeben werden.

5. Ein logischer Datentransfer DARF NICHT automatisch eine physische Payload-Kopie erfordern.

6. NovaOS MUSS Shared Memory als Zero-Copy-Mechanismus unterstützen können.

7. NovaOS MUSS Capability-geschützte Shared Data Objects unterstützen können.

8. Zugriffsrechte auf Shared Buffers MÜSSEN Least Privilege folgen.

9. Buffer Capabilities SOLLEN attenuierbar sein.

10. Buffer-Zugriff SOLL widerrufbar sein, soweit die zugrunde liegende Hardware- und Speichersemantik dies erlaubt.

11. DMA-fähige Geräte SOLLEN auf explizit autorisierte Buffer beschränkt werden.

12. IOMMU MUSS genutzt werden können, um DMA-Zugriffe zu isolieren.

13. Ein Buffer DARF NICHT freigegeben werden, solange ein Gerät noch darauf zugreifen kann.

14. NovaOS SOLL Scatter/Gather-I/O unterstützen.

15. NovaOS SOLL Buffer Views unterstützen.

16. Views MÜSSEN Offset, Länge und Zugriffsrechte sicher begrenzen können.

17. Semantic Types SOLLEN mit Buffer Views kombinierbar sein.

18. Zero-Copy-Pipelines SOLLEN Semantic Types erhalten.

19. NovaOS SOLL Ownership Transfer ohne Payload-Kopie unterstützen.

20. NovaOS SOLL Borrowing beziehungsweise temporäre Nutzungsrechte unterstützen können.

21. Read-only Sharing SOLL bevorzugt werden, wenn mehrere Consumer keine Mutation benötigen.

22. Writable Sharing MUSS explizite Synchronisations- und Ownership-Regeln besitzen.

23. Single-Writer-Modelle SOLLEN bevorzugt werden, wenn sie semantisch geeignet sind.

24. Immutable Shared Objects SOLLEN unterstützt werden.

25. Copy-on-Write SOLL für geteilte veränderbare Daten unterstützt werden können.

26. Page Mapping und Remapping SOLLEN für große lokale Datenobjekte verwendbar sein.

27. Shared Objects MÜSSEN klare Lifetimes besitzen.

28. Buffer Lifetimes SOLLEN mit Structured Concurrency integrierbar sein.

29. Cancellation DARF In-Flight-DMA- oder Borrow-Lifetimes NICHT verletzen.

30. NovaOS SOLL Buffer Pools unterstützen.

31. NovaOS SOLL Ring Buffers für kontinuierliche Datenströme unterstützen.

32. Zero-Copy-Pipelines MÜSSEN Backpressure unterstützen können.

33. Queues SOLLEN begrenzbar sein.

34. Degradation wie Frame Dropping DARF nur verwendet werden, wenn der Execution Contract beziehungsweise die Policy dies erlaubt.

35. Zero-Copy IPC SOLL unterstützt werden.

36. Typed IPC SOLL Shared Object Capabilities transportieren können.

37. IPC über Isolation Domains DARF explizit autorisierte Shared Buffers verwenden.

38. Starke Isolation DARF NICHT automatisch vollständige Datenkopien zwischen Domains erzwingen.

39. Driver Domains SOLLEN Zero-Copy-Datenpfade unterstützen können.

40. Kleine Control Messages DÜRFEN direkt kopiert werden, wenn dies effizienter ist.

41. NovaOS DARF dynamische Copy Thresholds verwenden.

42. Copy Thresholds SIND Policy und dürfen die semantische API NICHT verändern.

43. Netzwerkpfade SOLLEN Scatter/Gather und Shared Packet Buffers unterstützen.

44. Storage-Pfade SOLLEN direkte Transfers in autorisierte Zielbuffer unterstützen können.

45. Direct I/O DARF verwendet werden, ist jedoch KEIN universeller Standard.

46. Page Cache MUSS mit Zero-Copy beziehungsweise Mapping-Modellen kombinierbar sein.

47. Memory-Mapped Files SOLLEN unterstützt werden.

48. Storage-to-Network-Transfers SOLLEN ohne unnötige Anwendungskopien möglich sein.

49. Grafik- und Compositorpfade SOLLEN Shared Surface Buffers unterstützen.

50. Video-Pipelines SOLLEN Hardware-Decoder-Surfaces direkt weiterreichen können.

51. Kamera-Pipelines SOLLEN DMA-basierte Capture Buffers weiterreichen können.

52. Audio-Pipelines SOLLEN Ring-Buffer-basierte Zero-Copy-Mechanismen unterstützen.

53. KI- und Compute-Pipelines SOLLEN Tensor Views auf bestehende Buffer verwenden können.

54. Providerwahl SOLL Datenformat und Memory Placement berücksichtigen.

55. Algorithmus- und Providerplanung SOLL Copy- und Conversion-Kosten berücksichtigen.

56. Formatkonvertierungen DÜRFEN physische Kopien erzeugen, wenn sie tatsächlich erforderlich sind.

57. In-Place-Operationen DÜRFEN verwendet werden, wenn Ownership und Datenabhängigkeiten dies erlauben.

58. Pipeline Fusion DARF zur Vermeidung von Zwischenbuffern verwendet werden.

59. Provider Fusion DARF mehrere Verarbeitungsschritte innerhalb derselben Hardware-Domain ausführen.

60. Zero-Copy-Anforderungen MÜSSEN in `Nova.ExecutionContract` ausdrückbar sein können.

61. Harte No-Copy-Anforderungen DÜRFEN nur akzeptiert werden, wenn sie tatsächlich erfüllbar sind.

62. Data Movement MUSS als Ressourcen- und Performancekostenfaktor modellierbar sein.

63. Memory Bandwidth SOLL bei Copy-Entscheidungen berücksichtigt werden.

64. Energie- und Thermal-Kosten von Datenbewegung SOLLEN berücksichtigt werden können.

65. Shared Buffers MÜSSEN korrekt in der Resource Economy abgerechnet werden.

66. Physischer Shared-Memory-Verbrauch DARF NICHT für jedes Mapping vollständig mehrfach gezählt werden.

67. Pinned Memory MUSS gesondert budgetierbar sein.

68. Unkontrolliert unbegrenzter Pinned Memory DARF NICHT erlaubt werden.

69. Shared Objects MÜSSEN am Memory-Pressure- und Reclamation-Modell teilnehmen.

70. Buffer Leaks SOLLEN erkannt werden können.

71. Zero-Copy DARF Sicherheitsgrenzen NICHT abschwächen.

72. Shared Buffer Mappings MÜSSEN Range- und Permission-Prüfungen verwenden.

73. Information-Flow-Policies MÜSSEN auch für Shared Memory gelten.

74. Data-Sovereignty-Policies MÜSSEN auch für Zero-Copy- und Remote-Memory-Pfade gelten.

75. Sensitive Buffers SOLLEN zusätzliche Mapping- und Trust-Constraints besitzen können.

76. Trust Requirements MÜSSEN vor der Delegation eines Shared Buffers geprüft werden können.

77. Provenance MUSS bei Zero-Copy-Datenobjekten erhalten bleiben können.

78. Zero-Copy DARF Persistenz- und Memory-Lifetime NICHT semantisch vermischen.

79. Snapshots SOLLEN Copy-on-Write und Page Sharing nutzen können.

80. Hot Replacement SOLL Shared-State-Objekte für effizienten State Transfer verwenden können.

81. Inkompatible State-Layouts DÜRFEN explizite Transformationskopien erfordern.

82. Die native NovaABI SOLL Zero-Copy-fähige Handles und Shared Objects unterstützen.

83. Große Payloads SOLLEN NICHT grundsätzlich inline durch Syscalls kopiert werden.

84. FFI-Schnittstellen SOLLEN Borrowing, Pinning und Ownership Transfer unterstützen können.

85. Memory-safe Sprachen SOLLEN sichere Shared-Buffer-Abstraktionen verwenden.

86. Unsafe Buffer Access SOLL auf technisch notwendige, klar abgegrenzte Bereiche beschränkt werden.

87. Deterministic Mode MUSS adaptive Buffer-Placement- und Copy-Policies einschränken können.

88. Realtime-Pipelines SOLLEN vorallokierte beziehungsweise reservierte Buffer verwenden können.

89. Hard-Realtime-Pfade SOLLEN nicht von unbeschränkter dynamischer Allokation abhängen.

90. Self-Healing MUSS DMA-, Mapping- und Buffer-Lifetimes bei Domain-Ausfällen bereinigen können.

91. Ein abgestürzter Consumer DARF Shared Objects anderer Domains NICHT automatisch beschädigen können.

92. Schreibrechte MÜSSEN stärker eingeschränkt werden können als Leserechte.

93. Kopien aus Security-, Format-, Alignment-, Lifetime-, Remote- oder Hardwaregründen SIND architekturkonform.

94. Relevante Kopien SOLLEN nach Grund klassifizierbar sein.

95. NovaOS SOLL BytesCopied, CopyCount und CopyReason messen können.

96. Datenpfade SOLLEN über Architecture Introspection sichtbar gemacht werden können.

97. Copy-versus-Zero-Copy-Entscheidungen SOLLEN über Decision Tracing erklärbar sein.

98. Resource Economy MUSS Copy- und Transferkosten berücksichtigen können.

99. Adaptive Copy-Cost-Modelle DÜRFEN Prediction Error als Lernsignal verwenden.

100. KI-basierte Zero-Copy-Optimierungen DÜRFEN harte Security-, Safety-, Realtime-, Determinism- oder Sovereignty-Anforderungen NICHT verletzen.

## Konsequenzen

### Positive Konsequenzen

- geringere CPU-Belastung
- geringere Speicherbandbreitennutzung
- niedrigere Latenz
- geringerer Energieverbrauch
- bessere Thermal-Effizienz
- effizientere Video- und Audioverarbeitung
- effizientere KI- und Compute-Pipelines
- bessere Driver-Isolation ohne zwingende Kopierkosten
- effizientere IPC
- bessere Nutzung heterogener Hardware
- weniger temporäre Speicherallokationen
- bessere Pipeline-Komposition
- effizienteres Hot Replacement
- bessere Integration von CPU, GPU und NPU
- bessere Skalierbarkeit bei großen Datenmengen

### Negative Konsequenzen

- komplexeres Ownership-Modell
- komplexeres Lifetime Management
- zusätzliche Synchronisationsanforderungen
- schwierigere Debugging-Szenarien
- höhere Gefahr von Buffer Leaks
- Pinned Memory muss streng begrenzt werden
- IOMMU- und DMA-Management wird komplexer
- Shared writable Memory kann Race Conditions verursachen
- Hardware besitzt unterschiedliche Coherency-Modelle
- nicht jede Plattform ermöglicht echte Zero-Copy-Pfade
- Format- und Memory-Layout-Unterschiede können weiterhin Kopien erfordern
- Security- und Isolation-Grenzen müssen besonders sorgfältig definiert werden

## Verworfene Alternativen

### Copy bei jeder Subsystemgrenze

Nicht übernommen.

Dieses Modell wäre einfach zu implementieren, verursacht jedoch unnötige:

- CPU-Kosten
- Speicherbandbreite
- Energieverbrauch
- Latenz

und skaliert schlecht für große Datenströme.

### Absolutes Zero-Copy

Nicht übernommen.

Ein absolutes Verbot von Kopien wäre technisch unpraktisch und könnte:

- Security verschlechtern
- Hardwarekompatibilität verhindern
- Formatkonvertierungen erschweren
- Lifetime-Isolation unmöglich machen

NovaOS verwendet deshalb Copy Avoidance.

### Global Shared Address Space

Nicht übernommen.

Zero-Copy soll nicht dadurch erreicht werden, dass sämtliche Komponenten denselben uneingeschränkten Address Space verwenden.

Starke Isolation bleibt erhalten.

Shared Memory wird explizit und Capability-basiert freigegeben.

### Ausschließlich Shared Writable Memory

Nicht übernommen.

Unkontrolliertes Shared Writable Memory erzeugt:

- Race Conditions
- Integrity-Probleme
- starke Kopplung
- schwieriges Recovery

NovaOS bevorzugt:

- Read-only Sharing
- Single Writer
- Ownership Transfer
- Copy-on-Write

### Direct I/O für sämtliche Storagezugriffe

Nicht übernommen.

Caching und Page Cache bleiben für viele Workloads sinnvoll.

Das Ziel ist die Vermeidung redundanter Kopien, nicht die Abschaffung von Caches.

### Hardware-spezifische Zero-Copy-APIs als Systemmodell

Nicht übernommen.

NovaOS verwendet eine native providerunabhängige Buffer- und Shared-Object-Abstraktion.

Hardware-spezifische Mechanismen werden durch Provider und HAL integriert.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern
- ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem
- ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-IPC-0001_Typed_IPC
- ADR-IPC-0002_Capability_Based_IPC
- ADR-IPC-0003_Zero_Copy_IPC
- ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell
- ADR-DRIVER-0002_Driver_Sandboxing
- ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell
- ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell

## Zugehörige NPSPECs

- NPSPEC-ARCH-ZEROCOPY-0001
- NPSPEC-MEMORY-SHARED-0001
- NPSPEC-MEMORY-BUFFER-0001
- NPSPEC-MEMORY-BUFFERVIEW-0001
- NPSPEC-MEMORY-OWNERSHIP-0001
- NPSPEC-MEMORY-BORROWING-0001
- NPSPEC-MEMORY-COW-0001
- NPSPEC-MEMORY-PINNED-0001
- NPSPEC-MEMORY-DMA-0001
- NPSPEC-MEMORY-IOMMU-0001
- NPSPEC-MEMORY-SCATTERGATHER-0001
- NPSPEC-MEMORY-DEVICEBUFFER-0001
- NPSPEC-MEMORY-MEMORYCLASS-0001
- NPSPEC-MEMORY-COHERENCY-0001
- NPSPEC-MEMORY-BUFFERPOOL-0001
- NPSPEC-MEMORY-RINGBUFFER-0001
- NPSPEC-IPC-ZEROCOPY-0001
- NPSPEC-IPC-SHAREDOBJECT-0001
- NPSPEC-IPC-BUFFERTRANSFER-0001
- NPSPEC-DRIVER-DMA-0001
- NPSPEC-DRIVER-IOMMU-0001
- NPSPEC-STORAGE-DIRECTIO-0001
- NPSPEC-STORAGE-MMAP-0001
- NPSPEC-NETWORK-ZEROCOPY-0001
- NPSPEC-NETWORK-SCATTERGATHER-0001
- NPSPEC-GRAPHICS-SURFACEBUFFER-0001
- NPSPEC-MEDIA-VIDEOBUFFER-0001
- NPSPEC-AUDIO-RINGBUFFER-0001
- NPSPEC-COMPUTE-TENSORBUFFER-0001
- NPSPEC-DATA-PIPELINE-0001
- NPSPEC-DATA-SEMANTICVIEW-0001
- NPSPEC-RESOURCE-DATAMOVEMENT-0001
- NPSPEC-OBSERVABILITY-DATAMOVEMENT-0001

## Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch geeignete Tests überprüft werden.

Mindestens folgende Testbereiche sind vorzusehen.

### Shared-Memory-Tests

Zu prüfen sind:

- Mapping zwischen mehreren Address Spaces
- Read-only Mapping
- Read-write Mapping
- Range Protection
- Unmapping
- Lifetime
- Revocation

### Capability-Tests

Zu prüfen sind:

- Zugriff ohne Capability wird verweigert
- Read Capability kann nicht schreiben
- attenuierte Capability kann keine stärkeren Rechte erzeugen
- widerrufene Capability ist nicht mehr verwendbar

### Ownership-Tests

Zu prüfen sind:

- Ownership Transfer
- Borrowing
- Writer Exclusion
- Lifetime während Borrow
- Cleanup nach Owner-Termination

### DMA-Tests

Zu prüfen sind:

- Device Mapping
- IOMMU Isolation
- ungültige DMA-Adressen
- Buffer-Lifetime während In-Flight-I/O
- Unmapping nach Completion
- Cleanup bei Driver Crash

### Scatter/Gather-Tests

Zu prüfen sind:

- mehrere physische Segmente
- korrekte Reihenfolge
- Alignment
- Teilsegmente
- Device Limits

### Copy-on-Write-Tests

Zu prüfen sind:

- gemeinsame Read Pages
- erste Schreiboperation erzeugt private Kopie
- unveränderte Pages bleiben geteilt
- Snapshot-Konsistenz

### IPC-Tests

Zu prüfen sind:

- große Payloads ohne Payload-Kopie
- Typed IPC mit Buffer Capability
- Capability Revocation
- Domain Crash
- Receiver Cancellation

### Performance-Tests

Zu messen sind:

- Bytes Copied
- Memory Bandwidth
- CPU Consumption
- Latency
- Energy
- Throughput

Verglichen werden sollen mindestens:

```text
Copy Path
```

gegen:

```text
Zero-Copy Path
```

### Multimedia-Tests

Zu testen sind:

- Kamera → GPU
- Video Decoder → GPU
- GPU → Compositor
- Audio Device → Ring Buffer
- Netzwerk → Decoder

### Compute-Tests

Zu testen sind:

- CPU → GPU
- GPU → NPU
- NPU → CPU
- Unified Memory
- Tensor Views
- Layout Conversion

### Security-Tests

Zu testen sind:

- Buffer Out-of-Bounds
- Unauthorized Mapping
- Use-after-revoke
- stale Device Mapping
- malicious Driver Domain
- writable-sharing attacks
- IOMMU escape attempts

### Fault-Injection

Fehler müssen künstlich erzeugt werden können.

Beispiele:

- Driver Crash während DMA
- Consumer Crash während Borrow
- Owner Crash
- Device Reset
- IOMMU Mapping Failure
- Out-of-Memory
- Cancellation während In-Flight-I/O

Nach einem Fehler dürfen keine:

- permanenten Buffer Leaks
- verwaisten DMA Mappings
- unautorisierten Zugriffsmöglichkeiten

zurückbleiben.

### Determinism-Tests

Im Deterministic Mode müssen relevante:

- Buffer Placement
- Providerwahl
- Copy Decisions

innerhalb der spezifizierten Grenzen reproduzierbar sein.

## Ergebnis

NovaOS behandelt Datenbewegung als fundamentalen Bestandteil seiner Systemarchitektur.

Das zentrale Modell lautet:

```text
Semantic Data Object
        │
        ▼
Shared / Transferable Buffer
        │
        ├── Capability
        ├── View
        ├── Ownership
        ├── Lifetime
        └── Synchronization
        │
        ▼
Consumers / Devices
```

Statt Daten an jeder Subsystemgrenze zu kopieren, werden bevorzugt:

```text
References
Capabilities
Mappings
Views
Ownership
```

übertragen.

Physische Kopien bleiben erlaubt, wenn sie für:

```text
Security
Correctness
Format Conversion
Isolation
Lifetime
Hardware
Persistence
```

notwendig sind.

Damit lautet das verbindliche NovaOS-Prinzip:

```text
Zero-Copy where practical.
Copy only where justified.
```

Die Verbindung von:

```text
Zero-Copy
    +
Semantic Types
    +
Capability Security
    +
Shared Objects
    +
Resource Economy
    +
Typed IPC
    +
Execution Contracts
    +
Architecture Introspection
```

bildet die Grundlage für effiziente, sichere und hardwareübergreifende Datenpfade in NovaOS.