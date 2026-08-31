# ADR-ARCH-0017 – Composability und Orthogonalität

## Status

Angenommen

## Kategorie

Systemarchitektur / Modularität / Komposition / Capabilities / Contracts / APIs / Systemdesign

## Kontext

NovaOS verfolgt bewusst keine Architektur, in der Anwendungen, Dienste und Systemkomponenten als große, weitgehend geschlossene Funktionsblöcke betrachtet werden.

Stattdessen basiert NovaOS auf kleinen, klar definierten und kombinierbaren Systemfähigkeiten.

Eine Anwendung soll beispielsweise nicht zwingend selbst implementieren:

```text
Open File
Decode Image
Resize Image
Apply Filter
Convert Color Space
Run AI Enhancement
Encode Image
Write File
```

sondern diese Funktionen können durch unabhängige Capabilities bereitgestellt und kombiniert werden:

```text
Nova.File.Read
      │
      ▼
Nova.Image.Decode
      │
      ▼
Nova.Image.Resize
      │
      ▼
Nova.Image.Filter
      │
      ▼
Nova.Image.Enhance
      │
      ▼
Nova.Image.Encode
      │
      ▼
Nova.File.Write
```

Damit dieses Modell langfristig skalieren kann, müssen zwei Eigenschaften systemweit gelten:

```text
Composability
```

und:

```text
Orthogonality
```

Composability bedeutet:

> Systemkomponenten und Fähigkeiten werden so definiert, dass sie kontrolliert zu größeren Funktionen, Pipelines und Anwendungen zusammengesetzt werden können.

Orthogonalität bedeutet:

> Unterschiedliche Architekturkonzepte sollen möglichst unabhängige Verantwortlichkeiten besitzen und nicht unnötig miteinander gekoppelt werden.

Beispielsweise sollen folgende Aspekte getrennt bleiben:

```text
What
    Capability

How
    Algorithm / Provider

Where
    Location

With what resources
    Resource Budget

Under which authority
    Capability Security

Under which trust assumptions
    Nova.Trust

With which data restrictions
    Data Sovereignty

With which execution guarantees
    Execution Contract
```

Ein monolithischer API-Aufruf wie:

```text
ProcessImageUsingLocalGpuWithTrustedProviderAndFastAlgorithm(...)
```

vermischt mehrere unabhängige Dimensionen.

NovaOS soll stattdessen beispielsweise ermöglichen:

```text
Image.Process(
    image,
    contract = {
        location = Local,
        accelerator = Preferred,
        trust >= Verified,
        latency <= 20ms
    }
)
```

Die semantische Operation bleibt:

```text
Image.Process
```

während unabhängige Anforderungen über Contracts und Policies beschrieben werden.

Diese Trennung ist entscheidend für:

- Capability-Komposition,
- automatische Providerwahl,
- automatische Algorithmuswahl,
- Hardwareunabhängigkeit,
- Location Transparency,
- Zero-Copy,
- Resource Economy,
- Graceful Degradation,
- Deterministic Mode,
- Hot Replacement,
- Architecture Introspection,
- langfristige API-Stabilität.

Ohne systemweite Composability und Orthogonalität würde die Anzahl möglicher Kombinationen schnell zu einer kombinatorischen Explosion von APIs, Komponenten und Sonderfällen führen.

---

# Entscheidung

NovaOS MUSS Composability und Orthogonalität als fundamentale systemweite Architekturprinzipien verwenden.

Systemfunktionen sollen bevorzugt aus kleinen, semantisch klar definierten und über explizite Contracts kombinierbaren Fähigkeiten aufgebaut werden.

Unabhängige Architekturentscheidungen MÜSSEN soweit sinnvoll unabhängig modelliert werden.

Das Grundmodell lautet:

```text
Small Semantic Capabilities
           │
           ▼
Explicit Contracts
           │
           ▼
Composition
           │
           ▼
Execution Planning
           │
           ▼
Larger System Behavior
```

Orthogonal dazu:

```text
Semantics ─────────────── What?
Algorithm ─────────────── How?
Provider ──────────────── Who implements it?
Location ──────────────── Where?
Resources ─────────────── With what?
Security ──────────────── Who may?
Trust ─────────────────── Whom can we trust?
Sovereignty ───────────── Where may data go?
Determinism ───────────── How reproducible?
Lifecycle ─────────────── How long?
Failure Policy ────────── What happens on failure?
```

Die zentrale Regel lautet:

```text
Separate independent concerns.

Compose capabilities,
not accidental coupling.
```

---

# Composability

Composability beschreibt die Fähigkeit, kleinere Systembausteine zu größeren Funktionen zusammenzusetzen.

Konzeptionell:

```text
A + B + C
    ↓
Composite Function
```

Dabei müssen die Komponenten nicht voneinander wissen, solange ihre Contracts kompatibel sind.

---

# Semantische Komposition

Komposition soll bevorzugt auf semantischen Contracts basieren.

Beispiel:

```text
Capability A

Input:
    Image<RGBA8>

Output:
    Image<RGBA8>
```

und:

```text
Capability B

Input:
    Image<RGBA8>

Output:
    Image<RGBA8>
```

können direkt verbunden werden:

```text
A → B
```

---

# Contract-basierte Komposition

Komponenten dürfen nicht allein aufgrund zufällig kompatibler Speicherlayouts kombiniert werden.

Kompatibilität muss anhand relevanter Contracts bestimmt werden.

Dazu gehören beispielsweise:

```text
Semantic Type
Representation
Version
Security
Trust
Ownership
Data Sovereignty
Execution Requirements
```

---

# Kompositionsprüfung

Vor einer Komposition kann NovaOS prüfen:

```text
Output Contract A
        │
        ▼
Compatibility
        │
        ▼
Input Contract B
```

Mögliche Ergebnisse:

```text
DirectlyCompatible
Convertible
Adaptable
ConditionallyCompatible
Incompatible
Unknown
```

---

# Direct Composition

Bei direkter Kompatibilität:

```text
A → B
```

---

# Conversion

Ist eine sichere Konvertierung verfügbar:

```text
A
│
▼
Converter
│
▼
B
```

---

# Adapter

Unterschiedliche technische Interfaces können über Adapter verbunden werden:

```text
A
│
▼
Adapter
│
▼
B
```

Adapter dürfen semantische Unterschiede nicht verstecken.

---

# Lossy Composition

Verlustbehaftete Konvertierungen dürfen nicht implizit erfolgen, wenn dadurch relevante Semantik verloren geht.

Beispiel:

```text
Image<HDR, Float16>
        ↓
Image<RGB8>
```

kann Information verlieren.

Der Contract muss dies erlauben.

---

# Capability Composition

Capabilities sind primäre Kompositionsbausteine von NovaOS.

Beispiel:

```text
de.nova.image.decode
de.nova.image.resize
de.nova.image.filter.gaussian
de.nova.image.encode
```

können zu einer Pipeline kombiniert werden.

---

# Composite Capability

Mehrere Capabilities können als neue zusammengesetzte Capability präsentiert werden.

```text
CompositeCapability {
    Input
    Output
    InternalGraph
    Contract
}
```

Nach außen kann sie wie eine einzelne Capability erscheinen.

---

# Hierarchische Komposition

Komposition darf rekursiv erfolgen:

```text
Capability
    │
    ├── Capability
    │      ├── Capability
    │      └── Capability
    │
    └── Capability
```

---

# Black-Box-Komposition

Eine zusammengesetzte Capability kann intern komplex sein, solange ihr externer Contract eindeutig bleibt.

---

# White-Box-Introspection

Architecture Introspection darf dennoch, sofern autorisiert, die interne Komposition sichtbar machen.

---

# Orthogonalität

Orthogonalität bedeutet nicht, dass Architekturkonzepte niemals interagieren.

Sie bedeutet:

```text
Independent concerns
should remain independently expressible.
```

Beispielsweise beeinflusst Data Sovereignty die Providerwahl.

Trotzdem ist:

```text
Data Sovereignty
```

nicht identisch mit:

```text
Provider Selection
```

---

# Semantik und Implementierung

NovaOS trennt:

```text
Semantic Capability
```

von:

```text
Implementation Provider
```

Beispiel:

```text
Capability:
    Sort<T>
```

mögliche Provider:

```text
Nova CPU Provider
GPU Provider
Specialized Hardware Provider
Remote Provider
Compatibility Provider
```

---

# Semantik und Algorithmus

Eine Capability ist nicht identisch mit einem Algorithmus.

```text
Sort(data)
```

beschreibt die gewünschte Operation.

Mögliche Algorithmen:

```text
TimSort
QuickSort
MergeSort
RadixSort
GPU Radix Sort
```

---

# Algorithm Override

Ein expliziter Algorithmus kann gewählt werden:

```text
Sort(data, algorithm = TimSort)
```

ohne dass dadurch die semantische Definition von:

```text
Sort
```

verändert wird.

---

# Provider und Algorithmus

Provider und Algorithmus sind ebenfalls unterschiedliche Dimensionen.

Ein Provider kann mehrere Algorithmen implementieren.

Ein Algorithmus kann von mehreren Providern implementiert werden.

Konzeptionell:

```text
Capability
   │
   ├── Algorithm A
   │      ├── Provider 1
   │      └── Provider 2
   │
   └── Algorithm B
          ├── Provider 1
          └── Provider 3
```

---

# Provider und Location

Provideridentität und Ausführungsort sind getrennte Konzepte.

Ein Provider kann:

```text
Local
Remote
Distributed
Accelerator
Sandboxed
```

ausgeführt werden.

---

# Location und Transport

Location Transparency bedeutet nicht, dass Transport Teil der Capability-Semantik werden muss.

Nicht:

```text
RemoteImageResizeViaQUIC
```

sondern:

```text
Image.Resize
```

mit:

```text
Provider
Location
Transport
```

als getrennten Ausführungsentscheidungen.

---

# Security und Funktion

Security soll nicht unnötig in jede funktionale API eingebaut werden.

Nicht:

```text
SecureReadFile()
```

als parallele Semantik zu:

```text
ReadFile()
```

wenn die gleiche Operation gemeint ist.

Stattdessen:

```text
File.Read
```

mit Capability-Autorisierung.

---

# Trust und Security

Trust und Authorization bleiben getrennt.

```text
Trusted
```

bedeutet nicht automatisch:

```text
Authorized
```

und:

```text
Authorized
```

bedeutet nicht automatisch:

```text
Trusted for every operation
```

---

# Security und Data Sovereignty

Eine Operation kann autorisiert sein und trotzdem aufgrund von Data Sovereignty unzulässig sein.

```text
Authorized = true
RemoteTransferAllowed = false
```

ist ein gültiger Zustand.

---

# Resource Economy und Semantik

Ressourcenanforderungen sollen nicht unnötig Bestandteil der Capability-Identität werden.

Nicht:

```text
ResizeImageWith256MBMemory
ResizeImageWith512MBMemory
ResizeImageWithGPU
```

sondern:

```text
Image.Resize
```

mit Resource Contract.

---

# Performance und Semantik

Performanceziele sollen als Constraints oder Preferences modelliert werden.

```text
latency.preferred <= 10ms
```

statt neue funktionale APIs für jede Performanceklasse zu erzeugen.

---

# Determinism und Funktion

Deterministic Mode ist orthogonal zur semantischen Operation.

```text
Sort(data)
```

kann ausgeführt werden als:

```text
Normal
Deterministic
Replayable
BitwiseDeterministic
```

sofern Provider und Algorithmus dies unterstützen.

---

# Realtime und Funktion

Realtime-Anforderungen sind ebenfalls orthogonal.

```text
Audio.Process
```

kann beispielsweise einen Contract besitzen:

```text
deadline = 2ms
```

ohne dass eine eigene:

```text
RealtimeAudioProcess
```

Capability benötigt wird.

---

# Graceful Degradation

Graceful Degradation ist eine Policy über alternative gültige Kompositionen.

Beispiel:

```text
ImageEnhancement
      │
      ├── GPU AI Enhancement
      ├── CPU AI Enhancement
      ├── Classical Enhancement
      └── PassThrough
```

Die semantische Zieloperation bleibt stabil.

---

# Fail-Safe Defaults

Fail-Safe Defaults bleiben orthogonal zur Funktion.

Die gleiche Capability kann abhängig vom Failure Contract beispielsweise:

```text
Fail
Fallback
ReadOnly
Degrade
Suspend
```

verwenden.

---

# Compositional Failure

Ein Fehler innerhalb einer Komposition muss einer Komponente oder einem Contract zugeordnet werden können.

Nicht:

```text
Pipeline failed.
```

allein, sondern beispielsweise:

```text
PipelineFailure {
    node = Image.Decode
    reason = UnsupportedRepresentation
}
```

---

# Failure Propagation

Fehler können entlang der Komposition propagiert werden.

```text
A
│
▼
B  ← Failure
│
X
C
```

Der übergeordnete Scope entscheidet anhand seiner Failure Policy über:

```text
Fail
Retry
Fallback
Degrade
Compensate
Recover
```

---

# Structured Concurrency

Diese ADR integriert:

```text
ADR-ARCH-0010
Structured Concurrency als systemweites Ausführungsmodell
```

Komposition von Funktionen und Komposition von Ausführung sind miteinander verbunden, aber nicht identisch.

---

# Pipeline Graph vs Execution Tree

NovaOS unterscheidet:

```text
Pipeline Graph
```

und:

```text
Execution Scope Tree
```

Pipeline Graph:

```text
A → B → C
    │
    └→ D
```

Execution Tree:

```text
PipelineScope
├── Task A
├── Task B
├── Task C
└── Task D
```

---

# Datenabhängigkeit vs Ownership

Eine Datenabhängigkeit bedeutet nicht automatisch Lifecycle-Ownership.

Diese Konzepte müssen getrennt modelliert werden.

---

# Declarative Composition

Diese ADR integriert:

```text
ADR-ARCH-0003
Deklaratives Systemmodell als Architekturgrundlage
```

Kompositionen können deklarativ beschrieben werden.

Beispiel:

```text
Pipeline {
    source = Camera

    stages = [
        Decode,
        Stabilize,
        Enhance,
        Display
    ]
}
```

---

# Intent-basierte Komposition

Eine Anwendung kann auch nur ein Ziel beschreiben:

```text
Convert(image, target = Image<RGBA8>)
```

NovaOS kann daraus eine gültige Kompositionskette bestimmen.

---

# Automatic Composition

Automatische Komposition ist erlaubt, wenn:

- alle Hard Contracts erfüllt werden,
- Konvertierungen zulässig sind,
- Trust-Anforderungen erfüllt werden,
- Security eingehalten wird,
- Sovereignty eingehalten wird,
- Ressourcen verfügbar sind.

---

# Composition Planning

Konzeptionell:

```text
Intent
  │
  ▼
Capability Graph
  │
  ▼
Candidate Compositions
  │
  ▼
Contract Validation
  │
  ▼
Cost Evaluation
  │
  ▼
Execution Plan
```

---

# Candidate Graph

NovaOS darf mehrere mögliche Kompositionen betrachten.

Beispiel:

```text
       ┌→ A → B ─┐
Input ─┼→ C ─────┼→ Output
       └→ D → E ─┘
```

---

# Hard Constraints

Kandidaten, die Hard Constraints verletzen, werden ausgeschlossen.

---

# Soft Constraints

Verbleibende Kandidaten können anhand von Preferences bewertet werden.

---

# Composition Cost

Kompositionskosten können umfassen:

```text
Latency
CPU
Memory
GPU
NPU
Energy
Thermal
Data Movement
Serialization
Network
Monetary Cost
Conversion Loss
Startup Cost
Reliability
```

---

# Global statt lokaler Optimierung

Komposition soll nicht ausschließlich lokal pro Capability optimiert werden.

Beispiel:

```text
A → B → C
```

Die jeweils lokal schnellste Providerwahl kann insgesamt langsamer sein, wenn zwischen jedem Schritt Daten zwischen CPU und GPU kopiert werden müssen.

NovaOS darf deshalb die gesamte Komposition optimieren.

---

# Data Gravity

Datenlokalität ist ein Kompositionsfaktor.

```text
Move Compute to Data
```

kann günstiger sein als:

```text
Move Data to Compute
```

---

# Zero-Copy

Diese ADR integriert:

```text
ADR-ARCH-0006
Zero-Copy als Architekturprinzip
```

Komponenten sollen Daten so austauschen können, dass unnötige Kopien vermieden werden.

---

# Representation Orthogonality

Semantic Type und Physical Representation bleiben getrennt.

Beispiel:

```text
Semantic:
    Image<RGBA8>

Representation:
    CPU Linear Buffer
```

oder:

```text
Semantic:
    Image<RGBA8>

Representation:
    GPU Texture
```

---

# Representation Negotiation

Komponenten können kompatible Repräsentationen aushandeln.

---

# Conversion Avoidance

Wenn:

```text
A Output Representation
```

direkt von:

```text
B Input Representation
```

verwendet werden kann, soll unnötige Konvertierung vermieden werden.

---

# Operator Fusion

Komposition ermöglicht Optimierungen wie:

```text
A → B → C
```

zu:

```text
Fused(A,B,C)
```

sofern Semantik und Contracts erhalten bleiben.

---

# Fusion ist Implementierungsdetail

Die logische Komposition bleibt introspektierbar, auch wenn physische Ausführung fusioniert wurde.

---

# Semantic Types

Diese ADR integriert:

```text
ADR-ARCH-0008
Semantic Types als systemweites Typsystem
```

Semantic Types sind ein zentraler Mechanismus für sichere Komposition.

---

# Type Compatibility

Komposition erfordert definierte Type Compatibility.

Beispiel:

```text
Exact
Subtype
LosslessConvertible
LossyConvertible
Incompatible
```

---

# Units

Orthogonale semantische Einheiten verhindern fehlerhafte Komposition.

```text
Distance<Meter>
```

darf nicht implizit mit:

```text
Duration<Second>
```

verwechselt werden.

---

# Refinement Types

Zusätzliche Constraints können Komposition weiter einschränken.

Beispiel:

```text
Image<RGBA8, Width >= 1>
```

---

# Data/Object Pipelining

Diese ADR integriert:

```text
ADR-ARCH-0009
Einheitliches Daten- und Objekt-Pipelining
```

Das Pipeline-System stellt eine zentrale Form der Capability-Komposition dar.

---

# Pipeline Node

Ein Node beschreibt primär:

```text
Input Contract
Operation
Output Contract
```

und nicht zwingend:

```text
Thread
Process
Machine
GPU
```

---

# Execution Mapping

Die physische Ausführung wird später geplant:

```text
Logical Pipeline
       │
       ▼
Execution Planner
       │
       ▼
Physical Execution Graph
```

---

# Logical vs Physical Graph

Beispiel:

```text
Logical:

Decode → Resize → Filter → Encode
```

kann physisch werden:

```text
CPU Decode
    ↓
GPU Fused Resize+Filter
    ↓
Hardware Encoder
```

---

# Resource Economy

Diese ADR integriert:

```text
ADR-ARCH-0005
Systemweite Ressourcenökonomie
```

Ressourcen bleiben orthogonal zur funktionalen Komposition.

---

# Hierarchical Budget

Eine Composite Capability kann ein Gesamtbudget besitzen.

```text
Composite Budget
├── Decode
├── Transform
└── Encode
```

---

# Budget Composition

Child Budgets müssen mit dem Parent Budget vereinbar sein.

---

# Resource Attribution

Ressourcenverbrauch muss trotz Komposition einer:

- Capability,
- Komponente,
- Pipeline,
- Anwendung,
- Session

zugeordnet werden können.

---

# Resource Sharing

Komposition darf Ressourcen gemeinsam verwenden, wenn Ownership und Isolation dies erlauben.

---

# Security Composition

Security muss kompositional sein.

Wenn:

```text
A → B → C
```

darf B keine Rechte erhalten, die für seine Aufgabe nicht erforderlich sind.

---

# Capability Delegation

Ein Parent kann attenuierte Capabilities an Child-Komponenten delegieren.

Beispiel:

```text
Parent:
    File.ReadWrite
```

Child:

```text
File.Read
```

---

# Authority Amplification

Komposition darf keine Authority Amplification erzeugen.

```text
A Rights + B Rights
```

dürfen nicht implizit zu:

```text
More Than A+B
```

werden.

---

# Confused Deputy

Composability muss Confused-Deputy-Probleme berücksichtigen.

Komponenten dürfen ihre eigenen Rechte nicht unbeabsichtigt im Auftrag unprivilegierter Aufrufer einsetzen.

---

# Explicit Authority Context

Aufrufe sollen ihren Authority Context eindeutig transportieren.

---

# Trust Composition

Trust muss ebenfalls kompositional betrachtet werden.

Eine Pipeline:

```text
Trusted A
   ↓
Untrusted B
   ↓
Trusted C
```

ist nicht automatisch vollständig trusted.

---

# Trust Boundary

Trust Boundaries müssen sichtbar bleiben.

---

# Provenance

Die Provenance eines Outputs kann aus mehreren Inputs und Verarbeitungsschritten zusammengesetzt werden.

Konzeptionell:

```text
Output Provenance {
    Inputs
    Capabilities
    Providers
    Algorithms
    Transformations
}
```

---

# Data Sovereignty Composition

Sovereignty Constraints müssen entlang einer Komposition propagiert werden.

---

# Constraint Intersection

Wenn zwei Datenobjekte kombiniert werden, muss das Ergebnis mindestens die relevanten kombinierten Hard Constraints berücksichtigen.

---

# Information Flow

Komposition darf Information-Flow-Regeln nicht umgehen.

---

# Declassification

Declassification muss eine explizite autorisierte Operation sein.

---

# Transactions

Diese ADR integriert:

```text
ADR-ARCH-0004
Transaktionale Systemoperationen
```

Composite Operations können einen gemeinsamen Transaction Scope besitzen.

---

# Transaction Composition

```text
Transaction
├── Operation A
├── Operation B
└── Operation C
```

---

# Atomic Composite Operation

Eine Composite Capability kann nach außen atomare Semantik anbieten, obwohl intern mehrere Operationen ausgeführt werden.

---

# Transaction Boundary

Transaktionsgrenzen müssen explizit bleiben.

---

# Side Effects

Capabilities sollen ihre Side-Effect-Semantik deklarieren können.

Beispiele:

```text
Pure
ReadOnly
Idempotent
Transactional
ExternalSideEffect
Irreversible
```

---

# Pure Composition

Pure Capabilities sind besonders frei kombinierbar und optimierbar.

---

# Side-Effect Ordering

Bei Side Effects kann Reihenfolge semantisch relevant sein.

---

# Reordering

NovaOS darf Operationen nur umordnen, wenn ihre Contracts dies erlauben.

---

# Parallel Composition

Unabhängige Operationen können parallel ausgeführt werden.

```text
       ┌→ B ─┐
A ─────┤     ├→ D
       └→ C ─┘
```

---

# Dependency Graph

Parallelisierung muss Daten- und Side-Effect-Abhängigkeiten respektieren.

---

# Deterministic Composition

Im Deterministic Mode müssen relevante Kompositionsentscheidungen fixiert oder reproduzierbar gemacht werden können.

Dazu können gehören:

```text
Provider
Algorithm
Conversion Path
Fusion
Parallelism
Ordering
Batching
Location
Scheduling Constraints
```

---

# Hot Replacement

Diese ADR integriert:

```text
ADR-ARCH-0012
Hot Replacement und Live Evolution
```

Komponenten sollen austauschbar sein, solange ihre Contracts kompatibel bleiben.

---

# Replacement Principle

```text
Implementation A
      ↓
Compatible Contract
      ↓
Implementation B
```

---

# Stable Semantic Boundary

Hot Replacement wird durch stabile semantische Contracts erleichtert.

---

# Version Compatibility

Neue Komponenten können alte ersetzen, wenn ihre Contract-Version kompatibel ist.

---

# State Migration

Stateful Components benötigen gegebenenfalls State Migration.

---

# Composite Hot Replacement

Auch interne Teile einer Composite Capability können ersetzt werden.

```text
A → B1 → C
```

wird:

```text
A → B2 → C
```

ohne die externe Capability zu verändern.

---

# Architecture Introspection

Diese ADR integriert:

```text
ADR-ARCH-0013
Architecture Introspection
```

Kompositionen müssen introspektierbar sein.

---

# Composition Graph

Architecture Introspection soll beispielsweise zeigen können:

```text
Composite Capability
├── Node A
│   └── Provider CPU
├── Node B
│   └── Provider GPU
└── Node C
    └── Provider HardwareEncoder
```

---

# Contract View

Für jede Verbindung sollen relevante Contracts sichtbar sein können.

---

# Decision Trace

NovaOS soll erklären können:

```text
Why was this composition selected?
```

```text
Why was this converter inserted?
```

```text
Why was this provider selected?
```

```text
Why was fusion disabled?
```

```text
Why was remote execution rejected?
```

---

# Explicit Contracts

Diese ADR integriert:

```text
ADR-ARCH-0014
Explizite Contracts zwischen Systemkomponenten
```

Composability basiert auf expliziten Contracts statt impliziten Annahmen.

---

# Contract Dimensions

Ein Contract kann orthogonale Dimensionen enthalten:

```text
Functional
Type
Representation
Resource
Timing
Security
Trust
Sovereignty
Determinism
Failure
Lifecycle
Location
```

---

# Contract Composition

Mehrere Contracts müssen kombinierbar sein.

---

# Hard Constraint Intersection

Hard Constraints werden grundsätzlich durch zulässige Schnittmengen kombiniert.

---

# Unsatisfiable Composition

Ergibt die Contract-Komposition keine gültige Lösung:

```text
UnsatisfiableComposition
```

muss dies explizit gemeldet werden.

---

# Keine Constraint-Verwässerung

NovaOS darf Konflikte nicht lösen, indem Hard Constraints stillschweigend abgeschwächt werden.

---

# Graceful Degradation

Eine alternative Komposition darf verwendet werden, wenn sie die verbleibenden Hard Constraints erfüllt.

---

# Fail-Safe Defaults

Diese ADR integriert:

```text
ADR-ARCH-0016
Fail-Safe Defaults
```

Kann die Sicherheit einer Komposition nicht bestätigt werden:

```text
Do Not Compose
```

oder ein sicherer validierter Adapter/Fallback muss verwendet werden.

---

# Unknown Compatibility

```text
Unknown Compatibility
```

ist nicht:

```text
Compatible
```

---

# Explicit Unsafe Composition

Für Low-Level-Systemprogrammierung kann eine explizite Unsafe-Komposition vorgesehen werden.

Diese muss:

- explizit,
- autorisiert,
- sichtbar,
- auditierbar,
- möglichst lokal begrenzt

sein.

---

# API Design

NovaOS APIs sollen orthogonale Dimensionen nicht unnötig in Funktionsnamen kodieren.

Schlecht:

```text
FastLocalTrustedGpuResize()
FastRemoteTrustedGpuResize()
DeterministicLocalCpuResize()
SecureLocalCpuResize()
```

Besser:

```text
Resize(image, size, contract)
```

---

# Option Explosion

Orthogonalität verhindert:

```text
Feature × Provider × Location × Security × Performance × Determinism
```

als API-Kombinationsexplosion.

---

# Feature Flags

Feature Flags dürfen verwendet werden, sollen aber nicht als Ersatz für saubere semantische Trennung dienen.

---

# Configuration Orthogonality

Konfiguration soll möglichst einzelne unabhängige Dimensionen verändern können.

---

# Policy Orthogonality

Policies sollen nicht unnötig funktionale Semantik duplizieren.

---

# Mechanism / Policy Separation

Diese ADR integriert:

```text
ADR-ARCH-0002
Mechanism / Policy Separation
```

Mechanismus und Policy sind selbst ein wichtiges Beispiel für Orthogonalität.

```text
Mechanism:
    what can be done

Policy:
    what should be done
```

---

# Policy Composition

Mehrere Policies können gleichzeitig wirken.

Beispiel:

```text
Security Policy
Resource Policy
Energy Policy
Location Policy
User Preference
```

---

# Policy Priority

Policy Composition muss definierte Prioritätsregeln besitzen.

---

# Hard vs Soft

Hard Policies dürfen von Soft Policies nicht überschrieben werden.

---

# Cross-Cutting Concerns

Nicht jede Querschnittsfunktion soll in jede Komponente eingebaut werden.

Beispiele:

```text
Logging
Tracing
Security
Resource Accounting
Trust
Metrics
Cancellation
```

sollen über systemweite Mechanismen integrierbar sein.

---

# Keine versteckte globale Magie

Cross-Cutting Concerns dürfen jedoch nicht zu unsichtbarer Semantik führen.

Relevante Auswirkungen müssen über Contracts oder Introspection nachvollziehbar bleiben.

---

# Context Propagation

Structured Context kann orthogonale Informationen transportieren:

```text
ExecutionContext {
    Security
    Trust
    Cancellation
    Deadline
    Trace
    ResourceBudget
}
```

---

# Kein Thread-Local-Zwang

Da Tasks migrieren können, dürfen solche Informationen nicht ausschließlich an physische Threads gebunden sein.

---

# UI Composition

NovaOS verwendet Capability-basierte adaptive Benutzeroberflächen.

Die UI kann aus den verfügbaren Capabilities zusammengesetzt werden.

---

# Adaptive Ribbon

Ribbon-Elemente können von Capabilities bereitgestellt werden.

Beispiel:

```text
Loaded Capabilities
       │
       ▼
UI Contributions
       │
       ▼
Ribbon Composition
```

---

# Datei-Tab

Der systemweit definierte Tab:

```text
Datei
```

bleibt eine stabile Systemkomponente.

Weitere UI-Funktionen können dynamisch aus Capabilities entstehen.

---

# UI und Logik

NovaLang trennt:

```text
.nlf
```

für Logik und:

```text
.nui
```

für deklarative UI.

Beide verwenden dasselbe NovaLang-Sprach- und Typsystem.

---

# UI Orthogonality

UI-Darstellung und Business-/Capability-Logik sollen nicht unnötig gekoppelt werden.

---

# UI Capability Contract

Eine Capability kann deklarieren:

```text
UIContribution
```

ohne dass ihre Kernfunktion von einer konkreten Oberfläche abhängig wird.

---

# Headless Execution

Capabilities sollen, soweit semantisch sinnvoll, ohne UI ausführbar sein.

---

# Automation

Die gleiche Capability kann dadurch verwendet werden von:

```text
GUI
Shell
Script
AI Agent
System Service
Remote API
Automation
```

ohne ihre Kernsemantik zu duplizieren.

---

# NovaDOS

NovaDOS kann eine reduzierte Menge derselben semantischen Konzepte verwenden.

Die Recovery-Umgebung muss jedoch nicht jede vollständige NovaOS-Kompositionsfunktion implementieren.

---

# Boot Environment

Auch Boot-Komponenten sollen orthogonal entworfen werden, soweit dies ohne unnötige Komplexität möglich ist.

Beispiel:

```text
Renderer
Input
Layout
Resources
Navigation
Recovery Logic
```

sollen keine unnötigen zyklischen Abhängigkeiten besitzen.

---

# Kernel

Der Kernel soll Mechanismen bereitstellen, nicht sämtliche High-Level-Kompositionen selbst durchführen.

---

# Kernel Composition

Kernelmodule können intern modular komponiert sein.

Kritische Fast Paths dürfen stärker spezialisiert werden.

---

# Fast Path

Composability bedeutet nicht, dass jeder einzelne Laufzeitaufruf durch einen generischen dynamischen Graphen gehen muss.

---

# Control Plane / Data Plane

NovaOS unterscheidet:

```text
Control Plane
```

und:

```text
Data Plane
```

Komposition kann im Control Plane geplant werden.

Der resultierende Data Plane kann hochoptimiert sein.

---

# Prepared Execution Plan

Beispiel:

```text
Dynamic Composition
       │
       ▼
Validated Plan
       │
       ▼
Prepared Fast Path
       │
       ▼
Repeated Execution
```

---

# Plan Cache

Validierte Kompositionspläne können gecacht werden.

---

# Cache Validity

Ein gecachter Plan bleibt nur gültig, solange relevante Bedingungen kompatibel bleiben.

Dazu können gehören:

```text
Contract Versions
Provider Versions
Security Policy
Trust State
Resource Topology
Hardware
Location
Data Representation
```

---

# Replanning

Ändern sich relevante Bedingungen, kann Replanning erforderlich sein.

---

# Adaptive Composition

NovaOS darf Kompositionsentscheidungen adaptiv verbessern.

---

# Prediction Error

Prediction Error kann verwendet werden, um vorhergesagte optimale Kompositionen mit tatsächlichen Ergebnissen zu vergleichen.

```text
Predicted Plan
      ↓
Execution
      ↓
Observed Result
      ↓
Prediction Error
      ↓
Model Adjustment
```

---

# Keine adaptive Contract-Änderung

Adaptive Optimierung darf Hard Contracts nicht verändern.

---

# Stability

Adaptive Komposition muss Hysterese oder ähnliche Mechanismen verwenden können, um ständiges Umschalten zu vermeiden.

---

# Recomposition Cost

Der Wechsel zwischen Kompositionen besitzt selbst Kosten.

Diese müssen berücksichtigt werden.

---

# Static Composition

NovaOS muss auch vollständig statische Komposition unterstützen.

Nicht jede Operation benötigt dynamische Planung.

---

# Compile-Time Composition

NovaLang oder Build-Werkzeuge können Kompatibilität bereits zur Compile-Zeit prüfen.

---

# Load-Time Composition

Weitere Bindungen können beim Laden erfolgen.

---

# Runtime Composition

Dynamische Komponenten können zur Laufzeit gebunden werden.

---

# Composition Stages

Damit existieren:

```text
Compile Time
Load Time
Activation Time
Runtime
```

als mögliche Kompositionszeitpunkte.

---

# Early Binding

Early Binding kann Performance und Vorhersagbarkeit erhöhen.

---

# Late Binding

Late Binding kann Flexibilität und Hot Replacement erhöhen.

---

# Hybrid Binding

NovaOS darf beide Ansätze kombinieren.

---

# Dependency Injection

Dependency Injection kann als Mechanismus verwendet werden, ist aber nicht die grundlegende Architekturdefinition.

---

# Service Locator

Ein globaler Service Locator darf nicht zum versteckten universellen Abhängigkeitsmechanismus werden.

---

# Explicit Dependencies

Komponenten sollen ihre benötigten Abhängigkeiten explizit deklarieren.

---

# Optional Dependencies

Optionale Abhängigkeiten müssen als optional markiert werden.

---

# Hidden Dependency

Versteckte Abhängigkeiten sollen vermieden werden.

---

# Global State

Global mutable State reduziert Composability und soll minimiert werden.

---

# Shared State

Notwendiger Shared State benötigt:

- Ownership,
- Synchronisation,
- Lifecycle,
- Security,
- Versioning.

---

# Cyclic Dependencies

Zyklische Abhängigkeiten sollen vermieden werden.

---

# Dependency Graph

NovaOS soll Abhängigkeiten als Graph introspektierbar machen können.

---

# Cycle Detection

Unzulässige Zyklen sollen erkannt werden.

---

# Layering

Layering darf verwendet werden, soll aber keine künstlichen Barrieren zwischen orthogonalen Konzepten erzeugen.

---

# No Universal Layer Stack

Nicht jede NovaOS-Komponente muss exakt dieselbe Schichtenstruktur verwenden.

---

# Capability Graph statt starrem Stack

Für viele High-Level-Systemfunktionen ist ein Capability Graph flexibler als ein starrer vertikaler Stack.

---

# Unix-Prinzip

NovaOS übernimmt die Stärke des Unix-Prinzips:

```text
Small tools that compose
```

aber erweitert es.

Klassisches Unix:

```text
Bytes
  │
  ▼
Pipe
  │
  ▼
Bytes
```

NovaOS:

```text
Semantic Typed Object
        │
        ▼
Typed Capability
        │
        ▼
Semantic Typed Object
```

---

# Über Unix Pipes hinaus

NovaOS-Komposition kann zusätzlich berücksichtigen:

```text
Semantic Types
Zero-Copy
Security
Trust
Resource Budgets
Location
Sovereignty
Determinism
Structured Concurrency
Transactions
```

---

# Kein Text-Only-Zwang

Komposition darf nicht auf Textstreams beschränkt sein.

---

# Native Object Composition

Bilder, Audio, Video, Tensoren, Dokumente, Tabellen, Graphen und andere Objekte sollen direkt komponierbar sein.

---

# Streams

Streaming-Objekte sind First-Class-Kompositionsobjekte.

---

# Backpressure

Komponierte Streams müssen Backpressure unterstützen können.

---

# Backpressure Policy

Backpressure bleibt orthogonal zur Capability-Semantik.

Mögliche Policies:

```text
Block
DropOldest
DropNewest
Sample
Throttle
ReduceQuality
Spill
```

---

# Error Types

Fehler sollen semantisch typisiert sein.

---

# Error Composition

Composite Errors können Child Errors enthalten.

Beispiel:

```text
CompositeError {
    operation
    failedNode
    cause
    attemptedFallbacks
}
```

---

# Retry

Retry ist Policy und nicht Teil der funktionalen Capability-Identität.

---

# Retry Scope

Retry kann gelten für:

```text
Single Operation
Node
Subgraph
Entire Composite Operation
```

---

# Retry Budget

Retries müssen Ressourcen- und Deadline-Budgets respektieren.

---

# Fallback Composition

Alternative Graphen können als Fallback dienen.

```text
Primary:
    A → B → C

Fallback:
    A → D → C
```

---

# Fallback Compatibility

Der Fallback muss den externen Contract weiterhin erfüllen oder eine explizit erlaubte Degradationsstufe darstellen.

---

# Testing

Composability verbessert isoliertes Testen.

---

# Mock Provider

Provider können durch Testprovider ersetzt werden.

---

# Deterministic Test Provider

Tests können deterministische Provider verwenden.

---

# Fault Provider

Fault Injection kann Komponenten gezielt fehlerhaft reagieren lassen.

---

# Contract Tests

Jede Capability soll gegen ihren Contract testbar sein.

---

# Composition Tests

Zusätzlich müssen Kombinationen getestet werden.

---

# Property-Based Testing

Für kompatible Komponenten können Property-Based Tests verwendet werden.

---

# Substitutability

Wenn zwei Provider denselben Contract implementieren, sollen sie bezüglich der garantierten Semantik substituierbar sein.

---

# Behavioral Compatibility

Gleiche Typen allein reichen für Substitutability nicht aus.

Auch Verhalten und garantierte Semantik müssen kompatibel sein.

---

# Liskov-artige Substitution

Ein Ersatz darf die vom Contract garantierten Eigenschaften nicht verletzen.

---

# Versioning

Komponenten müssen unabhängig versionierbar sein.

---

# Semantic Versioning

Versionsnummern allein bestimmen nicht zwingend Kompatibilität.

NovaOS soll Contract-Kompatibilität explizit modellieren.

---

# Contract Evolution

Neue Contract-Versionen können:

```text
Compatible
BackwardCompatible
ForwardCompatible
Adaptable
Incompatible
```

sein.

---

# Adapter Registry

Kompatibilitätsadapter können registriert werden.

---

# Adapter Chains

Mehrere Adapter können theoretisch kombiniert werden.

---

# Adapter Cost

Adapter Chains besitzen Kosten und sollen vom Planner berücksichtigt werden.

---

# Adapter Explosion

NovaOS soll vermeiden, für jedes Komponentenpaar individuelle Adapter zu benötigen.

Semantic Types und standardisierte Contracts reduzieren dieses Problem.

---

# Canonical Interfaces

Für häufige Domänen sollen kanonische semantische Contracts definiert werden.

---

# Keine God Interfaces

Kanonische Interfaces dürfen nicht zu gigantischen God Interfaces werden.

---

# Interface Segregation

Komponenten sollen nur die Contracts implementieren müssen, die sie tatsächlich anbieten.

---

# Capability Granularity

Capabilities sollen weder:

```text
zu grob
```

noch:

```text
extrem mikroskopisch
```

geschnitten werden.

---

# Cohesion

Eine Capability soll eine semantisch zusammengehörige Fähigkeit repräsentieren.

---

# Granularity Criterion

Eine Capability ist sinnvoll abgegrenzt, wenn sie:

- einen klaren semantischen Zweck besitzt,
- einen stabilen Contract besitzen kann,
- unabhängig ersetzt werden kann,
- unabhängig getestet werden kann,
- sinnvoll komponiert werden kann.

---

# Over-Fragmentation

Zu starke Fragmentierung kann verursachen:

- hohe Dispatch-Kosten,
- komplexe Graphen,
- schwierige Fehleranalyse,
- hohe Contract-Kosten,
- unnötige Datenübergänge.

---

# Under-Fragmentation

Zu grobe Komponenten verursachen:

- geringe Wiederverwendbarkeit,
- schlechte Austauschbarkeit,
- große Trust Domains,
- unnötige Privilegien,
- schlechte Testbarkeit.

---

# Cohesion over Arbitrary Size

NovaOS definiert daher keine feste maximale Capability-Größe.

Semantische Kohäsion ist wichtiger als Zeilenzahl oder Binärgröße.

---

# Orthogonality Matrix

Architekturkonzepte können konzeptionell als unabhängige Achsen betrachtet werden:

```text
                  Semantic
                     │
                     │
Security ───────── Operation ───────── Location
                     │
                     │
                  Resources
```

mit weiteren Dimensionen:

```text
Trust
Determinism
Lifecycle
Failure
Sovereignty
Representation
Provider
Algorithm
```

---

# Cross-Dimension Constraints

Orthogonalität bedeutet nicht, dass jede Kombination gültig ist.

Beispiel:

```text
Remote
+
LocalOnly Data
```

ist inkompatibel.

---

# Constraint Solver

Der Planner kann solche Cross-Dimension Constraints prüfen.

---

# No Cartesian Product APIs

Die API muss jedoch nicht jede gültige Kombination als eigene Funktion definieren.

---

# Systemweite Einheitlichkeit

Die gleichen Kompositionsprinzipien sollen soweit sinnvoll gelten für:

```text
Kernel Services
System Services
Applications
Data Pipelines
Media
Graphics
AI
Networking
Storage
Compute
Automation
Agents
UI
```

---

# Domain-Specific Semantics

Systemweite Einheitlichkeit bedeutet nicht, dass alle Domänen identische Contracts besitzen.

Domainspezifische Semantik bleibt erlaubt.

---

# Common Contract Infrastructure

Gemeinsam sollen insbesondere sein:

```text
Identity
Versioning
Type Description
Capability Description
Security Context
Trust Context
Resource Description
Failure Description
Lifecycle
Introspection
```

---

# No Universal Object God Model

Das einheitliche NovaObject-Modell darf nicht dazu führen, dass jedes Objekt alle denkbaren Eigenschaften physisch enthalten muss.

---

# Sparse Metadata

Metadaten sollen optional und bedarfsgerecht sein können.

---

# Extensibility

Neue orthogonale Dimensionen sollen später hinzugefügt werden können, ohne bestehende Capability-Semantik vollständig neu definieren zu müssen.

---

# Future Hardware

Neue Hardwareklassen sollen als neue Provider oder Execution Targets integrierbar sein.

Nicht jede Capability muss dafür neu entworfen werden.

---

# Future Algorithms

Neue Algorithmen sollen hinter bestehenden semantischen Capabilities integrierbar sein.

---

# Future Locations

Neue Execution Locations sollen bestehende Semantik möglichst nicht verändern.

---

# Future Security Models

Neue Security-Mechanismen sollen über Contracts und Capability Enforcement integrierbar sein, ohne sämtliche funktionalen APIs neu zu definieren.

---

# Evolution

Composability und Orthogonalität sind damit Voraussetzung für langfristige Live Evolution.

---

# Architecture Debt

Nicht orthogonale APIs erzeugen Architecture Debt.

Beispiele:

```text
Provider-specific semantics
Location-specific APIs
Security-specific duplicate APIs
Hardware-specific application logic
Implicit global state
Hidden dependencies
```

---

# Architecture Review

Neue System-APIs sollen deshalb auf unnötige Kopplung geprüft werden.

---

# Review Questions

Bei neuen Komponenten soll insbesondere gefragt werden:

```text
What is the semantic responsibility?
```

```text
Which concerns are independent?
```

```text
Can the component be replaced?
```

```text
Can it be composed?
```

```text
Are dependencies explicit?
```

```text
Are contracts explicit?
```

```text
Is provider identity leaking into semantics?
```

```text
Is location leaking into semantics?
```

```text
Is policy leaking into mechanism?
```

```text
Is security being duplicated instead of enforced orthogonally?
```

---

# Normative Festlegungen

1. NovaOS MUSS Composability als systemweites Architekturprinzip verwenden.

2. NovaOS MUSS Orthogonalität als systemweites Architekturprinzip verwenden.

3. Systemfunktionen SOLLEN aus semantisch kohärenten Komponenten zusammensetzbar sein.

4. Komponenten SOLLEN klar definierte Verantwortlichkeiten besitzen.

5. Unabhängige Architekturbelange SOLLEN unabhängig ausdrückbar bleiben.

6. Capability-Semantik MUSS von Provideridentität getrennt werden.

7. Capability-Semantik MUSS soweit sinnvoll von Ausführungsort getrennt werden.

8. Capability-Semantik SOLL von konkreten Transportmechanismen getrennt werden.

9. Capability-Semantik SOLL von konkreten Algorithmen getrennt werden.

10. Capability-Semantik SOLL von Hardwareimplementierungen getrennt werden.

11. Resource Requirements SOLLEN über Contracts statt über Capability-Duplikation beschrieben werden.

12. Performance Requirements SOLLEN über Contracts oder Policies beschrieben werden.

13. Security MUSS als orthogonaler Autorisierungsaspekt integrierbar sein.

14. Trust MUSS von Authorization unterscheidbar bleiben.

15. Data Sovereignty MUSS von Authorization unterscheidbar bleiben.

16. Location MUSS von Provideridentität unterscheidbar bleiben.

17. Algorithmus MUSS von Provideridentität unterscheidbar bleiben.

18. Representation MUSS von Semantic Type unterscheidbar bleiben.

19. Execution Ownership MUSS von Data Dependency unterscheidbar bleiben.

20. Pipeline Graph MUSS vom Execution Scope Tree unterscheidbar bleiben.

21. Mechanism MUSS von Policy unterscheidbar bleiben.

22. Hard Constraints MÜSSEN von Soft Preferences unterscheidbar sein.

23. Side Effects MÜSSEN deklarierbar sein.

24. Pure Operations SOLLEN identifizierbar sein.

25. Idempotente Operationen SOLLEN identifizierbar sein.

26. Transactional Operations SOLLEN identifizierbar sein.

27. Irreversible Operations MÜSSEN identifizierbar sein können.

28. Capability Inputs SOLLEN semantisch typisiert sein.

29. Capability Outputs SOLLEN semantisch typisiert sein.

30. Komposition MUSS Contract-Kompatibilität berücksichtigen.

31. Komposition DARF NICHT allein auf physischer Layout-Kompatibilität beruhen.

32. Direct Compatibility MUSS darstellbar sein.

33. Convertible Compatibility MUSS darstellbar sein.

34. Incompatibility MUSS darstellbar sein.

35. Unknown Compatibility MUSS darstellbar sein.

36. Unknown Compatibility DARF NICHT automatisch als Compatible gelten.

37. Lossless Conversion SOLL von Lossy Conversion unterscheidbar sein.

38. Lossy Conversion MUSS explizit zulässig sein.

39. Converter SOLLEN als eigenständige Komponenten modellierbar sein.

40. Adapter SOLLEN als eigenständige Komponenten modellierbar sein.

41. Adapter DÜRFEN semantische Inkompatibilität NICHT verstecken.

42. Composite Capabilities MÜSSEN möglich sein.

43. Composite Capabilities SOLLEN einen eigenen externen Contract besitzen.

44. Composite Capabilities DÜRFEN intern hierarchisch aufgebaut sein.

45. Interne Komplexität DARF hinter stabilen semantischen Contracts verborgen werden.

46. Architecture Introspection SOLL interne Komposition autorisiert sichtbar machen können.

47. Kompositionen SOLLEN deklarativ beschreibbar sein.

48. Intent-basierte automatische Komposition DARF unterstützt werden.

49. Automatische Komposition MUSS Hard Contracts respektieren.

50. Automatische Komposition MUSS Security Constraints respektieren.

51. Automatische Komposition MUSS Trust Constraints respektieren.

52. Automatische Komposition MUSS Data Sovereignty respektieren.

53. Automatische Komposition MUSS Resource Constraints respektieren.

54. Automatische Komposition MUSS Determinism Requirements respektieren.

55. Automatische Komposition MUSS Location Constraints respektieren.

56. Der Planner DARF mehrere Candidate Compositions betrachten.

57. Kandidaten mit verletzten Hard Constraints MÜSSEN ausgeschlossen werden.

58. Soft Preferences DÜRFEN zur Bewertung verbleibender Kandidaten verwendet werden.

59. Kompositionskosten SOLLEN Latency berücksichtigen können.

60. Kompositionskosten SOLLEN CPU-Kosten berücksichtigen können.

61. Kompositionskosten SOLLEN Memory-Kosten berücksichtigen können.

62. Kompositionskosten SOLLEN GPU-Kosten berücksichtigen können.

63. Kompositionskosten SOLLEN NPU-Kosten berücksichtigen können.

64. Kompositionskosten SOLLEN Data Movement berücksichtigen können.

65. Kompositionskosten SOLLEN Network-Kosten berücksichtigen können.

66. Kompositionskosten SOLLEN Energy berücksichtigen können.

67. Kompositionskosten SOLLEN Thermal Effects berücksichtigen können.

68. Kompositionskosten SOLLEN Conversion Loss berücksichtigen können.

69. Kompositionskosten DÜRFEN Monetary Cost berücksichtigen.

70. Kompositionskosten SOLLEN Reliability berücksichtigen können.

71. NovaOS DARF Kompositionen global statt nur lokal optimieren.

72. Providerwahl DARF End-to-End-Datenbewegung berücksichtigen.

73. Data Gravity SOLL bei relevanten Workloads berücksichtigt werden.

74. Zero-Copy SOLL durch kompatible Komposition ermöglicht werden.

75. Zero-Copy DARF Ownership NICHT verletzen.

76. Zero-Copy DARF Security NICHT verletzen.

77. Representation Negotiation SOLL unterstützt werden.

78. Unnötige Representation Conversions SOLLEN vermieden werden.

79. Operator Fusion DARF verwendet werden.

80. Operator Fusion MUSS semantische Äquivalenz erhalten.

81. Operator Fusion MUSS Hard Contracts erhalten.

82. Physische Fusion DARF logische Introspection NICHT vollständig zerstören.

83. Semantic Types MÜSSEN für sichere Komposition verwendet werden können.

84. Type Compatibility MUSS explizit modellierbar sein.

85. Units MÜSSEN semantisch berücksichtigt werden können.

86. Refinement Constraints SOLLEN Komposition einschränken können.

87. Data/Object Pipelines MÜSSEN Capability-Komposition unterstützen.

88. Pipeline Nodes SOLLEN primär semantische Operationen beschreiben.

89. Pipeline Nodes MÜSSEN NICHT an einen bestimmten Thread gebunden sein.

90. Pipeline Nodes MÜSSEN NICHT an einen bestimmten Prozess gebunden sein.

91. Pipeline Nodes MÜSSEN NICHT an eine bestimmte Maschine gebunden sein.

92. Logical Pipeline und Physical Execution Graph MÜSSEN unterscheidbar sein.

93. Execution Planner DARF Logical Pipelines auf Hardware abbilden.

94. Resource Budgets MÜSSEN hierarchisch komponierbar sein.

95. Child Budgets DÜRFEN Parent Budgets NICHT unautorisiert überschreiten.

96. Ressourcenverbrauch MUSS Composite Operations zurechenbar bleiben.

97. Ressourcenverbrauch SOLL einzelnen Nodes zurechenbar sein.

98. Security MUSS kompositional erhalten bleiben.

99. Capability Delegation MUSS Attenuation unterstützen können.

100. Child Components SOLLEN nur notwendige Capabilities erhalten.

101. Komposition DARF keine Authority Amplification erzeugen.

102. Confused-Deputy-Risiken MÜSSEN berücksichtigt werden.

103. Authority Context MUSS über relevante Kompositionsgrenzen erhalten bleiben.

104. Trust Boundaries MÜSSEN sichtbar bleiben.

105. Trusted Inputs und Untrusted Processing DÜRFEN NICHT automatisch Trusted Output erzeugen.

106. Provenance SOLL über Komposition erhalten werden können.

107. Output Provenance SOLL mehrere Inputs referenzieren können.

108. Output Provenance SOLL relevante Transformationsschritte referenzieren können.

109. Data Sovereignty MUSS entlang relevanter Kompositionen propagiert werden.

110. Information Flow Policies MÜSSEN entlang relevanter Kompositionen propagiert werden.

111. Declassification MUSS explizit autorisiert werden.

112. Composite Operations DÜRFEN gemeinsame Transaction Scopes verwenden.

113. Transaktionsgrenzen MÜSSEN explizit bleiben.

114. Composite Capabilities DÜRFEN atomare externe Semantik anbieten.

115. Side-Effect Ordering MUSS respektiert werden.

116. Operation Reordering DARF nur bei semantischer Zulässigkeit erfolgen.

117. Unabhängige Operationen DÜRFEN parallel ausgeführt werden.

118. Parallelisierung MUSS Data Dependencies respektieren.

119. Parallelisierung MUSS Side-Effect Dependencies respektieren.

120. Structured Concurrency MUSS mit Capability Composition integrierbar sein.

121. Task Ownership DARF NICHT allein aus Pipeline Edges abgeleitet werden.

122. Failure Propagation MUSS über Composite Operations möglich sein.

123. Failed Node MUSS identifizierbar sein.

124. Failure Reason MUSS identifizierbar sein.

125. Fallback Attempts SOLLEN introspektierbar sein.

126. Retry MUSS als Policy und nicht als Capability-Semantik modellierbar sein.

127. Retry Scope MUSS definierbar sein.

128. Retry Budget MUSS Resource Budgets respektieren.

129. Retry MUSS Deadlines respektieren.

130. Fallback Graphs MÜSSEN unterstützt werden können.

131. Fallback Graphs MÜSSEN externe Hard Contracts erfüllen.

132. Degraded Fallbacks MÜSSEN explizit erlaubt sein.

133. Deterministic Mode MUSS relevante Kompositionsentscheidungen fixieren können.

134. Deterministic Mode MUSS Provider Selection kontrollieren können.

135. Deterministic Mode MUSS Algorithm Selection kontrollieren können.

136. Deterministic Mode MUSS Conversion Paths kontrollieren können.

137. Deterministic Mode MUSS Ordering kontrollieren können.

138. Deterministic Mode MUSS relevante Parallelism Decisions kontrollieren können.

139. Hot Replacement MUSS durch stabile Contracts unterstützt werden.

140. Provider Replacement MUSS Contract Compatibility prüfen.

141. Stateful Replacement MUSS State Migration unterstützen können.

142. Teile einer Composite Capability DÜRFEN unabhängig ersetzt werden.

143. Replacement DARF externe Semantik NICHT unzulässig verändern.

144. Composition Graphs MÜSSEN introspektierbar sein.

145. Provider Mapping SOLL introspektierbar sein.

146. Conversion Nodes SOLLEN introspektierbar sein.

147. Adapter SOLLEN introspektierbar sein.

148. Fusion SOLL introspektierbar sein.

149. Composition Decisions SOLLEN Decision Traces besitzen.

150. NovaOS SOLL erklären können, warum eine Komposition gewählt wurde.

151. NovaOS SOLL erklären können, warum eine Komposition verworfen wurde.

152. NovaOS SOLL erklären können, warum ein Converter eingefügt wurde.

153. NovaOS SOLL erklären können, warum ein Provider gewählt wurde.

154. NovaOS SOLL erklären können, warum Fusion deaktiviert wurde.

155. Explizite Component Contracts MÜSSEN Grundlage systemweiter Komposition sein.

156. Contract Dimensions SOLLEN orthogonal ausdrückbar sein.

157. Functional Contracts SOLLEN von Resource Contracts unterscheidbar sein.

158. Resource Contracts SOLLEN von Security Contracts unterscheidbar sein.

159. Security Contracts SOLLEN von Trust Contracts unterscheidbar sein.

160. Trust Contracts SOLLEN von Sovereignty Contracts unterscheidbar sein.

161. Determinism Contracts SOLLEN von Timing Contracts unterscheidbar sein.

162. Lifecycle Contracts SOLLEN von Failure Contracts unterscheidbar sein.

163. Hard Constraint Composition MUSS sicher erfolgen.

164. Unsatisfiable Compositions MÜSSEN explizit fehlschlagen.

165. Hard Constraints DÜRFEN NICHT zur Konfliktlösung stillschweigend abgeschwächt werden.

166. Graceful Degradation DARF alternative Kompositionen verwenden.

167. Fail-Safe Defaults MÜSSEN bei unbekannter Kompatibilität gelten.

168. Unsichere Komposition DARF NICHT automatisch ausgeführt werden.

169. Unsafe Composition MUSS explizit sein.

170. Unsafe Composition MUSS autorisiert sein.

171. Unsafe Composition SOLL auditierbar sein.

172. APIs SOLLEN orthogonale Dimensionen nicht unnötig in Funktionsnamen kodieren.

173. NovaOS SOLL Cartesian-Product-API-Explosion vermeiden.

174. Feature Flags DÜRFEN saubere Architekturtrennung NICHT ersetzen.

175. Configuration Dimensions SOLLEN unabhängig änderbar sein.

176. Policies SOLLEN funktionale Semantik NICHT unnötig duplizieren.

177. Mechanism und Policy MÜSSEN getrennt bleiben.

178. Mehrere Policies MÜSSEN gleichzeitig wirken können.

179. Policy Priority MUSS definiert sein.

180. Hard Policies MÜSSEN Soft Policies überstimmen.

181. Cross-Cutting Concerns SOLLEN über gemeinsame Systemmechanismen integrierbar sein.

182. Cross-Cutting Concerns DÜRFEN relevante Semantik NICHT unsichtbar machen.

183. Structured Context SOLL Security Context transportieren können.

184. Structured Context SOLL Trust Context transportieren können.

185. Structured Context SOLL Cancellation transportieren können.

186. Structured Context SOLL Deadline transportieren können.

187. Structured Context SOLL Trace Context transportieren können.

188. Structured Context SOLL Resource Budget transportieren können.

189. Execution Context DARF NICHT ausschließlich Thread Local sein.

190. UI Composition MUSS Capability Contributions unterstützen können.

191. Capability Logic SOLL von konkreter UI getrennt sein.

192. `.nlf` und `.nui` MÜSSEN dasselbe NovaLang-Typsystem verwenden.

193. `.nui` MUSS deklarative UI-Komposition unterstützen.

194. UI Contributions SOLLEN von Capabilities bereitgestellt werden können.

195. Der systemweite `Datei`-Tab DARF als stabile Systemkomponente definiert werden.

196. Capabilities SOLLEN soweit sinnvoll headless ausführbar sein.

197. Dieselbe Capability SOLL von GUI, Shell, Script, Agent und Service verwendbar sein.

198. NovaDOS DARF ein reduziertes Kompositionsmodell verwenden.

199. Bootkomponenten SOLLEN unnötige zyklische Abhängigkeiten vermeiden.

200. Der Kernel SOLL High-Level-Komposition nicht vollständig selbst durchführen.

201. Kritische Kernel Fast Paths DÜRFEN spezialisiert sein.

202. Composability DARF keinen dynamischen Dispatch für jeden Fast-Path-Aufruf erzwingen.

203. Control Plane und Data Plane MÜSSEN unterscheidbar sein.

204. Composition Planning DARF im Control Plane stattfinden.

205. Prepared Fast Paths DÜRFEN im Data Plane verwendet werden.

206. Validierte Execution Plans DÜRFEN gecacht werden.

207. Plan Caches MÜSSEN relevante Versionen berücksichtigen.

208. Plan Caches MÜSSEN relevante Policy-Änderungen berücksichtigen.

209. Plan Caches MÜSSEN relevante Trust-Änderungen berücksichtigen.

210. Plan Caches MÜSSEN relevante Topology-Änderungen berücksichtigen.

211. Ungültig gewordene Plans MÜSSEN verworfen oder neu validiert werden.

212. Replanning MUSS möglich sein.

213. Adaptive Composition DARF unterstützt werden.

214. Prediction Error DARF zur Verbesserung von Planungsmodellen verwendet werden.

215. Adaptive Composition DARF Hard Contracts NICHT verändern.

216. Adaptive Composition SOLL Stabilitätsmechanismen verwenden.

217. Recomposition Cost SOLL berücksichtigt werden.

218. Statische Komposition MUSS möglich bleiben.

219. Compile-Time Composition DARF unterstützt werden.

220. Load-Time Composition DARF unterstützt werden.

221. Activation-Time Composition DARF unterstützt werden.

222. Runtime Composition DARF unterstützt werden.

223. Early Binding DARF verwendet werden.

224. Late Binding DARF verwendet werden.

225. Hybrid Binding DARF verwendet werden.

226. Dependency Injection DARF als Implementierungsmechanismus verwendet werden.

227. Ein globaler Service Locator SOLL NICHT zum universellen versteckten Abhängigkeitsmodell werden.

228. Abhängigkeiten SOLLEN explizit deklariert werden.

229. Optionale Abhängigkeiten MÜSSEN als optional erkennbar sein.

230. Hidden Dependencies SOLLEN vermieden werden.

231. Global Mutable State SOLL minimiert werden.

232. Shared State MUSS Ownership definieren.

233. Shared State MUSS Synchronisation definieren.

234. Shared State MUSS Lifecycle definieren.

235. Shared State MUSS Security berücksichtigen.

236. Shared State SOLL Versioning unterstützen.

237. Unnötige Dependency Cycles SOLLEN vermieden werden.

238. Dependency Graphs SOLLEN introspektierbar sein.

239. Unzulässige Dependency Cycles SOLLEN erkannt werden.

240. NovaOS DARF Layering verwenden.

241. NovaOS MUSS keinen universellen Layer Stack erzwingen.

242. Capability Graphs DÜRFEN für High-Level-Komposition verwendet werden.

243. NovaOS SOLL kleine kombinierbare Werkzeuge als Architekturidee übernehmen.

244. NovaOS DARF Komposition NICHT auf Byte- oder Textstreams beschränken.

245. Semantic Objects MÜSSEN direkt komponierbar sein können.

246. Streams MÜSSEN First-Class-Kompositionsobjekte sein können.

247. Streaming Composition SOLL Backpressure unterstützen.

248. Backpressure Policy SOLL orthogonal zur funktionalen Capability bleiben.

249. Fehler SOLLEN semantisch typisiert sein.

250. Composite Errors SOLLEN Child Errors referenzieren können.

251. Provider Substitutability MUSS Contract-Semantik berücksichtigen.

252. Gleiche Input-/Output-Typen allein DÜRFEN NICHT als vollständige Behavioral Compatibility gelten.

253. Provider Replacement MUSS garantierte Semantik erhalten.

254. Komponenten MÜSSEN unabhängig versionierbar sein können.

255. Contract Compatibility MUSS unabhängig von bloßen Versionsnummern modellierbar sein.

256. Backward Compatibility MUSS ausdrückbar sein.

257. Forward Compatibility SOLL ausdrückbar sein.

258. Adaptable Compatibility SOLL ausdrückbar sein.

259. Incompatibility MUSS ausdrückbar sein.

260. Adapter Registry DARF unterstützt werden.

261. Adapter Chains DÜRFEN unterstützt werden.

262. Adapter Costs SOLLEN bei Planning berücksichtigt werden.

263. Semantic Types SOLLEN die Anzahl paarweiser Adapter reduzieren.

264. Häufige Domänen SOLLEN kanonische semantische Contracts besitzen können.

265. Kanonische Contracts DÜRFEN NICHT zu God Interfaces werden.

266. Interface Segregation SOLL berücksichtigt werden.

267. Capability Granularity MUSS semantische Kohäsion berücksichtigen.

268. Capability Granularity DARF NICHT ausschließlich anhand physischer Codegröße bestimmt werden.

269. Capabilities SOLLEN unabhängig testbar sein.

270. Capabilities SOLLEN unabhängig ersetzbar sein.

271. Capabilities SOLLEN sinnvoll kombinierbar sein.

272. Over-Fragmentation SOLL vermieden werden.

273. Under-Fragmentation SOLL vermieden werden.

274. Composability DARF Performance-Anforderungen NICHT ignorieren.

275. Orthogonalität DARF reale Cross-Dimension Constraints NICHT ignorieren.

276. Orthogonalität bedeutet NICHT, dass jede theoretische Kombination zulässig ist.

277. Cross-Dimension Constraints MÜSSEN ausdrückbar sein.

278. Der Planner SOLL Cross-Dimension Constraints prüfen können.

279. APIs SOLLEN dennoch keinen Cartesian Product aller Kombinationen definieren.

280. Gemeinsame Kompositionsprinzipien SOLLEN systemweit verwendet werden.

281. Domainspezifische Semantik MUSS erhalten bleiben.

282. Common Contract Infrastructure SOLL systemweit wiederverwendet werden.

283. Objektmodelle DÜRFEN NICHT durch universelle Pflichtmetadaten unnötig aufgebläht werden.

284. Metadata SOLLEN bedarfsgerecht repräsentierbar sein.

285. Neue orthogonale Contract-Dimensionen SOLLEN später ergänzbar sein.

286. Neue Hardware SOLL als Provider oder Execution Target integrierbar sein.

287. Neue Hardware SOLL bestehende Capability-Semantik möglichst nicht verändern.

288. Neue Algorithmen SOLLEN hinter bestehenden Capabilities integrierbar sein.

289. Neue Locations SOLLEN bestehende Capability-Semantik möglichst nicht verändern.

290. Neue Security Mechanisms SOLLEN bestehende funktionale APIs möglichst nicht duplizieren.

291. Architecture Reviews SOLLEN unnötige Kopplung identifizieren.

292. Architecture Reviews SOLLEN Provider Leakage prüfen.

293. Architecture Reviews SOLLEN Location Leakage prüfen.

294. Architecture Reviews SOLLEN Policy Leakage prüfen.

295. Architecture Reviews SOLLEN versteckte Abhängigkeiten prüfen.

296. Architecture Reviews SOLLEN Contract-Klarheit prüfen.

297. Composition Tests MÜSSEN direkte kompatible Komposition prüfen.

298. Composition Tests MÜSSEN inkompatible Komposition ablehnen.

299. Composition Tests MÜSSEN Unknown Compatibility sicher behandeln.

300. Composition Tests MÜSSEN automatische Converter Insertion prüfen.

301. Composition Tests MÜSSEN Lossy Conversion Policies prüfen.

302. Composition Tests MÜSSEN Security Propagation prüfen.

303. Composition Tests MÜSSEN Trust Boundary Propagation prüfen.

304. Composition Tests MÜSSEN Data Sovereignty Propagation prüfen.

305. Composition Tests MÜSSEN Information Flow prüfen.

306. Composition Tests MÜSSEN Resource Budget Composition prüfen.

307. Composition Tests MÜSSEN Structured Concurrency Ownership prüfen.

308. Composition Tests MÜSSEN Failure Propagation prüfen.

309. Composition Tests MÜSSEN Fallback Graphs prüfen.

310. Composition Tests MÜSSEN Graceful Degradation prüfen.

311. Composition Tests MÜSSEN Fail-Safe Defaults prüfen.

312. Composition Tests MÜSSEN Deterministic Mode prüfen.

313. Composition Tests MÜSSEN Hot Replacement prüfen.

314. Composition Tests MÜSSEN Provider Substitution prüfen.

315. Composition Tests MÜSSEN Operator Fusion prüfen.

316. Composition Tests MÜSSEN Zero-Copy prüfen.

317. Composition Tests MÜSSEN Representation Negotiation prüfen.

318. Composition Tests MÜSSEN Plan Cache Invalidation prüfen.

319. Composition Tests MÜSSEN Replanning prüfen.

320. NovaOS MUSS nach dem Prinzip arbeiten: `Compose semantics; keep independent concerns orthogonal.`

---

# Konsequenzen

## Positive Konsequenzen

- Systemfunktionen werden stärker wiederverwendbar.
- Anwendungen müssen weniger Funktionalität selbst implementieren.
- Capabilities können flexibel kombiniert werden.
- neue Provider können bestehende Funktionen implementieren.
- neue Algorithmen benötigen nicht automatisch neue APIs.
- neue Hardware kann leichter integriert werden.
- lokale und entfernte Ausführung verwenden dieselbe semantische Operation.
- Resource Economy kann unabhängig von funktionaler Semantik optimieren.
- Security wird nicht in jede funktionale API dupliziert.
- Trust bleibt von Authorization getrennt.
- Data Sovereignty bleibt unabhängig modellierbar.
- Semantic Types ermöglichen sichere automatische Komposition.
- Zero-Copy kann über Komponenten hinweg optimiert werden.
- Pipeline Fusion wird möglich.
- globale statt ausschließlich lokale Optimierung wird möglich.
- Graceful Degradation kann alternative Capability-Graphen verwenden.
- Deterministic Mode kann dieselben Funktionen unter strengeren Ausführungsbedingungen verwenden.
- Hot Replacement wird durch stabile Contracts erleichtert.
- Architecture Introspection kann vollständige Kompositionsgraphen darstellen.
- Komponenten können isolierter getestet werden.
- Provider können leichter durch Test- oder Fault-Provider ersetzt werden.
- Adaptive Planung kann neue Kombinationen ausprobieren, ohne APIs zu verändern.
- UI, Shell, Skripte und KI-Agenten können dieselben Capabilities verwenden.
- NovaLang kann dieselben semantischen Contracts in `.nlf` und `.nui` verwenden.
- die Anzahl kombinatorischer Spezial-APIs wird reduziert.
- Architekturentscheidungen bleiben langfristig austauschbarer.
- NovaOS kann zukünftige Hardware und Ausführungsmodelle integrieren, ohne seine semantische API grundlegend neu zu entwerfen.

---

# Negative Konsequenzen

- Contract-Infrastruktur wird komplexer.
- Capability Granularity muss sorgfältig gewählt werden.
- automatische Komposition benötigt leistungsfähige Planner.
- Contract Negotiation verursacht Control-Plane-Kosten.
- dynamische Komposition erschwert Debugging ohne gute Introspection.
- Adapter und Converter können zusätzliche Komplexität erzeugen.
- zu starke Fragmentierung kann Performance verschlechtern.
- zu grobe Komponenten reduzieren Wiederverwendbarkeit.
- globale Optimierung kann aufwendiger sein als lokale Auswahl.
- Plan Cache und Invalidierung werden notwendig.
- Cross-Dimension Constraints können komplex werden.
- Trust-, Security- und Sovereignty-Propagation müssen korrekt implementiert werden.
- Composite Failure Semantics benötigen sorgfältiges Design.
- Hot Replacement von stateful Komponenten benötigt State Migration.
- Legacy-Systeme passen möglicherweise schlecht in das orthogonale Modell.
- Compatibility Layers benötigen Adapter.
- Entwickler müssen zwischen Semantik, Provider, Algorithmus, Representation und Location unterscheiden.
- Tests müssen nicht nur Einzelkomponenten, sondern auch Kompositionen prüfen.
- Architecture Introspection wird für komplexe Graphen praktisch unverzichtbar.

---

# Verworfene Alternativen

## Große monolithische APIs

Verworfen.

Beispiel:

```text
ImageApplication.DoEverything()
```

reduziert Wiederverwendbarkeit, Austauschbarkeit und Testbarkeit.

---

## Provider-spezifische APIs als Standard

Verworfen.

```text
NvidiaResize()
AMDResize()
CpuResize()
```

würden Hardwareentscheidungen in Anwendungscode einbetonieren.

---

## Location-spezifische APIs als Standard

Verworfen.

```text
LocalResize()
RemoteResize()
CloudResize()
```

vermischen Semantik und Location.

---

## Algorithmus als Capability-Identität

Als allgemeines Modell verworfen.

```text
TimSort()
QuickSort()
MergeSort()
```

dürfen als explizite Algorithmen verfügbar sein, aber die übergeordnete semantische Operation bleibt:

```text
Sort()
```

---

## Security-spezifische Funktionsduplikate

Verworfen.

```text
SecureRead()
NormalRead()
TrustedRead()
```

führen zu unnötiger API-Vervielfachung.

---

## Ein universelles God Interface

Verworfen.

Ein einziges Interface für sämtliche Systemfunktionen würde Orthogonalität und Interface Segregation zerstören.

---

## Ein universelles NovaObject mit allen möglichen Feldern

Verworfen.

Ein gemeinsames semantisches Objektmodell bedeutet nicht, dass jedes Objekt physisch alle Metadaten tragen muss.

---

## Globale mutable Service Registry als einzige Kompositionsbasis

Verworfen.

Sie würde versteckte Abhängigkeiten und schwer kontrollierbaren globalen Zustand erzeugen.

---

## Nur statische Komposition

Verworfen.

Sie würde Hot Replacement, adaptive Providerwahl und dynamische Capability-Systeme unnötig einschränken.

---

## Nur dynamische Komposition

Verworfen.

Sie würde Fast Paths, Realtime, Deterministic Mode und statisch optimierbare Komponenten unnötig belasten.

---

## Jeder Capability-Aufruf durch universellen Planner

Verworfen.

Planung kann vorbereitet und gecacht werden.

Fast Paths dürfen direkt ausgeführt werden.

---

## Komposition ausschließlich über Byte Streams

Verworfen.

Dies würde semantische Typen, Zero-Copy und hardwarebeschleunigte Objekte unnötig auf Serialisierung reduzieren.

---

## Unix Pipes unverändert übernehmen

Verworfen.

Das grundlegende Kompositionsprinzip ist wertvoll, aber NovaOS benötigt zusätzlich:

```text
Semantic Types
Contracts
Capabilities
Security
Trust
Zero-Copy
Resources
Location
Transactions
Structured Concurrency
```

---

## Maximale Mikrofragmentierung

Verworfen.

Kleinere Komponenten sind nicht automatisch bessere Komponenten.

---

## Maximale Komponentenaggregation

Verworfen.

Große All-in-One-Komponenten verhindern Wiederverwendung und Austauschbarkeit.

---

## Vollständige Orthogonalität ohne Interaktionen

Verworfen.

Architekturdimensionen sind logisch getrennt, besitzen aber reale Constraints untereinander.

---

## Cartesian Product aller Optionen als API

Verworfen.

```text
Operation × Provider × Location × Algorithm × Security × QoS
```

würde zu einer nicht wartbaren API führen.

---

## Implizite Contract-Annahmen

Verworfen.

Komposition muss auf expliziten und introspektierbaren Contracts beruhen.

---

# Abhängigkeiten

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
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell`

---

# Zugehörige NPSPECs

- `NPSPEC-ARCH-COMPOSABILITY-0001`
- `NPSPEC-ARCH-ORTHOGONALITY-0001`
- `NPSPEC-COMPOSITION-MODEL-0001`
- `NPSPEC-COMPOSITION-GRAPH-0001`
- `NPSPEC-COMPOSITION-NODE-0001`
- `NPSPEC-COMPOSITION-EDGE-0001`
- `NPSPEC-COMPOSITION-CONTRACT-0001`
- `NPSPEC-COMPOSITION-COMPATIBILITY-0001`
- `NPSPEC-COMPOSITION-PLANNER-0001`
- `NPSPEC-COMPOSITION-PLAN-0001`
- `NPSPEC-COMPOSITION-PLAN-CACHE-0001`
- `NPSPEC-COMPOSITION-REPLANNING-0001`
- `NPSPEC-COMPOSITION-COST-0001`
- `NPSPEC-COMPOSITION-CONSTRAINT-0001`
- `NPSPEC-COMPOSITION-CONVERTER-0001`
- `NPSPEC-COMPOSITION-ADAPTER-0001`
- `NPSPEC-COMPOSITION-ADAPTER-REGISTRY-0001`
- `NPSPEC-COMPOSITION-FUSION-0001`
- `NPSPEC-COMPOSITION-FALLBACK-0001`
- `NPSPEC-COMPOSITION-DEGRADATION-0001`
- `NPSPEC-COMPOSITION-FAILURE-0001`
- `NPSPEC-COMPOSITION-RETRY-0001`
- `NPSPEC-COMPOSITION-TRANSACTION-0001`
- `NPSPEC-COMPOSITION-SECURITY-0001`
- `NPSPEC-COMPOSITION-TRUST-0001`
- `NPSPEC-COMPOSITION-SOVEREIGNTY-0001`
- `NPSPEC-COMPOSITION-INFORMATIONFLOW-0001`
- `NPSPEC-COMPOSITION-PROVENANCE-0001`
- `NPSPEC-COMPOSITION-RESOURCE-0001`
- `NPSPEC-COMPOSITION-LOCATION-0001`
- `NPSPEC-COMPOSITION-DETERMINISM-0001`
- `NPSPEC-COMPOSITION-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-COMPOSITION-HOTREPLACE-0001`
- `NPSPEC-COMPOSITION-INTROSPECTION-0001`
- `NPSPEC-COMPOSITION-DECISIONTRACE-0001`
- `NPSPEC-COMPOSITION-TESTING-0001`
- `NPSPEC-COMPOSITION-FAULTINJECTION-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-COMPOSITE-0001`
- `NPSPEC-CAPABILITY-GRANULARITY-0001`
- `NPSPEC-CAPABILITY-SUBSTITUTION-0001`
- `NPSPEC-CAPABILITY-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-OPTIONALDEPENDENCY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-PROVIDER-SELECTION-0001`
- `NPSPEC-CAPABILITY-ALGORITHM-0001`
- `NPSPEC-CAPABILITY-LOCATION-0001`
- `NPSPEC-CAPABILITY-UI-CONTRIBUTION-0001`
- `NPSPEC-CONTRACT-COMPOSITION-0001`
- `NPSPEC-CONTRACT-INTERSECTION-0001`
- `NPSPEC-CONTRACT-NEGOTIATION-0001`
- `NPSPEC-CONTRACT-COMPATIBILITY-0001`
- `NPSPEC-CONTRACT-SUBSTITUTABILITY-0001`
- `NPSPEC-CONTRACT-EVOLUTION-0001`
- `NPSPEC-TYPE-COMPATIBILITY-0001`
- `NPSPEC-TYPE-CONVERSION-GRAPH-0001`
- `NPSPEC-DATA-PIPELINE-0001`
- `NPSPEC-DATA-PIPELINE-GRAPH-0001`
- `NPSPEC-DATA-PIPELINE-FUSION-0001`
- `NPSPEC-DATA-PIPELINE-PLANNING-0001`
- `NPSPEC-DATA-PIPELINE-OPTIMIZATION-0001`
- `NPSPEC-DATA-REPRESENTATION-NEGOTIATION-0001`
- `NPSPEC-DATA-PROVENANCE-COMPOSITION-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-RESOURCE-COMPOSITION-0001`
- `NPSPEC-RESOURCE-HIERARCHICAL-BUDGET-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-PLAN-0001`
- `NPSPEC-EXECUTION-CONTEXT-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-EXECUTION-ALGORITHM-0001`
- `NPSPEC-EXECUTION-LOCATION-0001`
- `NPSPEC-EXECUTION-BINDING-0001`
- `NPSPEC-EXECUTION-FASTPATH-0001`
- `NPSPEC-EXECUTION-CONTROLPLANE-0001`
- `NPSPEC-EXECUTION-DATAPLANE-0001`
- `NPSPEC-UI-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-UI-RIBBON-COMPOSITION-0001`
- `NPSPEC-NOVALANG-COMPOSITION-0001`
- `NPSPEC-NOVALANG-DEPENDENCY-0001`
- `NPSPEC-NOVALANG-CONTRACT-0001`

---

# Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch Unit-, Contract-, Integration-, Composition-, Property-, Performance-, Security-, Failure- und Fault-Injection-Tests überprüft werden.

## Direct-Composition-Test

Zwei direkt kompatible Capabilities müssen ohne unnötigen Adapter kombinierbar sein.

```text
A<T> → B<T>
```

---

## Incompatibility-Test

```text
A<Image>
    ↓
B<Audio>
```

muss ohne gültigen Converter als inkompatibel erkannt werden.

---

## Converter-Test

Ein registrierter zulässiger Converter muss automatisch eingefügt werden können.

```text
A
│
▼
Converter
│
▼
B
```

---

## Lossy-Conversion-Test

Eine verlustbehaftete Konvertierung darf nur verwendet werden, wenn der Contract dies erlaubt.

---

## Unknown-Compatibility-Test

```text
Compatibility = Unknown
```

darf nicht automatisch zu einer Verbindung führen.

---

## Provider-Independence-Test

Die gleiche Capability muss mit mehreren kompatiblen Providern funktionieren können.

---

## Algorithm-Independence-Test

Die gleiche semantische Operation muss mehrere kompatible Algorithmen verwenden können.

---

## Location-Independence-Test

Eine semantische Capability darf nicht unnötig von Local oder Remote abhängen.

---

## Security-Orthogonality-Test

Eine Capability muss bei unterschiedlichen Security Contexts dieselbe funktionale Semantik behalten, sofern der Zugriff erlaubt ist.

---

## Authorization-Test

Komposition darf keine Authority Amplification erzeugen.

---

## Confused-Deputy-Test

Eine privilegierte Komponente darf ihre eigenen Rechte nicht unbeabsichtigt für einen unprivilegierten Aufrufer einsetzen.

---

## Trust-Test

Eine Pipeline mit einem Untrusted Node darf nicht automatisch als vollständig Trusted gelten.

---

## Sovereignty-Test

Eine automatische Komposition darf keinen Remote Provider auswählen, wenn:

```text
DataLocation = LocalOnly
```

gilt.

---

## Information-Flow-Test

Eine Komposition darf Information Flow Restrictions nicht umgehen.

---

## Provenance-Test

Output Provenance muss relevante Inputs und Transformationsschritte referenzieren können.

---

## Resource-Budget-Test

Child Nodes dürfen das Parent Budget nicht unautorisiert überschreiten.

---

## Global-Optimization-Test

Der Planner soll eine End-to-End günstigere Komposition gegenüber lokal optimalen, aber global teureren Providerwechseln auswählen können.

---

## Zero-Copy-Test

Kompatible Representation und Ownership sollen direkte Bufferweitergabe ermöglichen.

---

## Zero-Copy-Security-Test

Unzureichende Shared-Memory-Rechte müssen Zero-Copy verhindern.

---

## Representation-Negotiation-Test

Zwei Komponenten sollen eine gemeinsame Repräsentation auswählen können.

---

## Fusion-Test

Mehrere pure kompatible Nodes dürfen fusioniert werden.

---

## Fusion-Semantics-Test

Fusion darf das beobachtbare Contract-Verhalten nicht verändern.

---

## Logical-Introspection-Test

Fusionierte Nodes müssen logisch weiterhin nachvollziehbar bleiben.

---

## Structured-Concurrency-Test

Pipeline Data Dependencies und Task Ownership müssen unabhängig korrekt modelliert werden.

---

## Parallelism-Test

Unabhängige Nodes dürfen parallel ausgeführt werden.

---

## Side-Effect-Test

Side-Effect-Abhängigkeiten müssen unzulässiges Reordering verhindern.

---

## Transaction-Test

Composite Operations müssen gemeinsame Transaction Scopes verwenden können.

---

## Determinism-Test

Deterministic Mode muss relevante Composition Decisions reproduzierbar machen können.

---

## Hot-Replacement-Test

Ein Provider muss durch einen Contract-kompatiblen Provider ersetzt werden können.

---

## Stateful-Replacement-Test

Stateful Components müssen definierte State Migration verwenden.

---

## Fallback-Test

Bei Provider Failure muss ein gültiger Fallback Graph verwendet werden können.

---

## Graceful-Degradation-Test

Ein Degraded Graph darf nur verwendet werden, wenn der Contract die reduzierte Semantik erlaubt.

---

## Fail-Safe-Test

Kann die Kompatibilität eines Adapters nicht bestätigt werden, darf er nicht verwendet werden.

---

## Plan-Cache-Test

Ein validierter Plan muss wiederverwendbar sein.

---

## Plan-Invalidation-Test

Ändert sich ein relevanter Contract oder Provider, muss der Plan invalidiert oder neu validiert werden.

---

## Trust-Invalidation-Test

Trust Revocation muss einen davon abhängigen gecachten Plan ungültig machen können.

---

## Replanning-Test

Ein nicht mehr gültiger Plan muss durch eine neue gültige Komposition ersetzt werden können.

---

## Adaptive-Test

Prediction Error darf zukünftige Planung beeinflussen.

---

## Adaptive-Hard-Constraint-Test

Prediction Error darf Hard Constraints nicht verändern.

---

## Dependency-Test

Explizite Abhängigkeiten müssen im Architecture Graph sichtbar sein.

---

## Cycle-Test

Unzulässige Dependency Cycles müssen erkannt werden.

---

## Headless-Test

Eine UI-fähige Capability soll, sofern semantisch möglich, auch ohne UI ausführbar sein.

---

## UI-Composition-Test

Capability UI Contributions müssen dynamisch in die adaptive Oberfläche integrierbar sein.

---

## Ribbon-Test

Geladene Capabilities müssen ihre Ribbon Contributions bereitstellen können.

---

## Static-Composition-Test

Eine vollständig statische Komposition muss ohne Runtime Planner ausführbar sein können.

---

## Dynamic-Composition-Test

Eine dynamische Capability muss zur Laufzeit eingebunden werden können.

---

## Fast-Path-Test

Ein vorbereiteter Composition Plan darf ohne vollständige Neuplanung wiederholt ausgeführt werden.

---

## Performance-Test

Die Abstraktionsschichten dürfen für vorbereitete Fast Paths keinen unvertretbaren Overhead verursachen.

---

## Fault-Injection-Test

Gezielt ausfallen müssen unter anderem:

```text
Provider
Converter
Adapter
Planner
Resource Reservation
Remote Node
Trust Verification
Capability Resolution
Pipeline Node
Transaction Participant
```

Dabei muss geprüft werden, dass die Composite Operation ihre Failure Contracts einhält.

---

## Substitution-Test

Zwei Provider desselben Contracts müssen bezüglich garantierter Semantik austauschbar sein.

---

## Behavioral-Test

Gleiche Input-/Output-Typen dürfen nicht ausreichen, wenn Behavioral Contracts unterschiedlich sind.

---

## Version-Test

Contract-kompatible neue Versionen müssen eingebunden werden können.

---

## Incompatible-Version-Test

Inkompatible Versionen müssen abgelehnt oder über explizite Adapter verbunden werden.

---

## Scale-Test

Der Capability Graph muss mit einer großen Anzahl registrierter:

- Capabilities,
- Provider,
- Algorithmen,
- Converter,
- Adapter

skalieren können.

---

## Composition-Explosion-Test

Der Planner darf nicht sämtliche theoretischen Kombinationen blind materialisieren müssen.

---

## Long-Running-Test

Langzeitbetrieb darf keine unbegrenzt wachsenden:

```text
Plan Caches
Adapter Chains
Dependency Graphs
Stale Providers
Stale Contracts
```

erzeugen.

---

# Ergebnis

Mit `ADR-ARCH-0017` werden Composability und Orthogonalität zu fundamentalen Architekturprinzipien von NovaOS.

Das System wird nicht nach dem Modell aufgebaut:

```text
Application
    │
    ├── own image stack
    ├── own audio stack
    ├── own AI stack
    ├── own conversion stack
    ├── own security logic
    └── own hardware logic
```

sondern:

```text
                    Application Intent
                           │
                           ▼
                   Semantic Capabilities
                           │
                           ▼
                    Explicit Contracts
                           │
                           ▼
                     Composition Graph
                           │
                           ▼
                    Execution Planner
                           │
          ┌────────────────┼────────────────┐
          ▼                ▼                ▼
      Provider          Algorithm        Location
          │                │                │
          └────────────────┼────────────────┘
                           │
          ┌────────────────┼────────────────┐
          ▼                ▼                ▼
      Resources         Security          Trust
          │                │                │
          └────────────────┼────────────────┘
                           │
          ┌────────────────┼────────────────┐
          ▼                ▼                ▼
     Sovereignty      Determinism       Failure
          │                │                │
          └────────────────┼────────────────┘
                           │
                           ▼
                   Validated Execution
```

Dadurch können kleine Fähigkeiten zu großen Systemfunktionen zusammengesetzt werden, ohne dass jede Kombination als eigene API implementiert werden muss.

Das zentrale Kompositionsprinzip lautet:

```text
Build complex behavior
from small semantic capabilities.
```

Das zentrale Orthogonalitätsprinzip lautet:

```text
Separate what can vary independently.
```

Für NovaOS bedeutet dies insbesondere:

```text
Capability != Provider
Capability != Algorithm
Capability != Location
Capability != Representation
Capability != Resource Policy
Capability != Security Policy
Capability != Trust Policy
```

und gleichzeitig:

```text
Capabilities
+
Semantic Types
+
Explicit Contracts
+
Execution Contracts
+
Resource Economy
+
Security
+
Trust
+
Data Sovereignty
+
Structured Concurrency
+
Architecture Introspection
=
Composable System Architecture
```

Die langfristige Architekturregel lautet damit:

```text
Compose semantics.

Keep independent concerns orthogonal.

Bind implementation decisions
as late as useful,
but as early as necessary.
```