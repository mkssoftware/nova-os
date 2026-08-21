# NPSPEC-COMPUTE-RUNTIME-0004 — Object Model

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS Compute Runtime Object Model  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0004`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`  
**Kurzname:** Compute Object Model  

---

## 1. Zweck

Diese Spezifikation definiert das **Object Model** der NovaOS Compute Runtime.

Das Compute Object Model beschreibt die gemeinsamen Regeln für alle durch die Compute Runtime verwalteten Objekte.

Dazu gehören insbesondere:

- Runtime-Instanzen,
- Compute Devices,
- Compute Contexts,
- Queues,
- Jobs,
- Commands,
- Programme,
- Module,
- Kernels,
- Speicherressourcen,
- Buffer,
- Images,
- Tensoren,
- Events,
- Fences,
- Timelines,
- Graphen,
- Pipelines,
- Sampler,
- Memory Pools,
- importierte Ressourcen,
- exportierte Ressourcen,
- zukünftige Compute Objects.

Das Object Model definiert insbesondere:

- Objektidentität,
- Objekttypen,
- Handles,
- Objektzustände,
- Lebensdauer,
- Ownership,
- Referenzen,
- Parent-Child-Beziehungen,
- Abhängigkeiten,
- Zugriffsrechte,
- Thread Safety,
- Prozessisolation,
- Context-Isolation,
- Objektfreigabe,
- Deferred Destruction,
- Resource Sharing,
- Import und Export,
- Recovery,
- Introspection,
- Diagnose,
- ABI-Erweiterbarkeit.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Alle Compute-Komponenten werden als klar typisierte und kontrolliert verwaltete Runtime-Objekte behandelt, deren Identität, Lebensdauer, Ownership und Zugriffsrechte unabhängig von der zugrunde liegenden Hardware definiert sind.

Anwendungen DÜRFEN keine internen Runtime-Strukturen direkt manipulieren.

Der öffentliche Zugriff SOLL grundsätzlich über:

```text
Opaque Handles
```

erfolgen.

---

## 3. Ziele

Das Object Model MUSS folgende Ziele verfolgen:

1. einheitliche Objektsemantik,
2. typsichere Handles,
3. kontrollierte Lebensdauer,
4. sichere Ownership,
5. sichere Prozessisolation,
6. sichere Context-Isolation,
7. Thread Safety,
8. effiziente Handle-Auflösung,
9. geringe Runtime-Kosten,
10. sichere asynchrone Nutzung,
11. definierte Parent-Child-Beziehungen,
12. kontrolliertes Resource Sharing,
13. robuste Fehlerbehandlung,
14. Device-Recovery,
15. Debugging und Introspection,
16. ABI-Stabilität,
17. Erweiterbarkeit für zukünftige Objekttypen.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- die konkrete öffentliche C-API,
- eine C++-Klassenhierarchie,
- NovaLang-Sprachobjekte,
- allgemeine Garbage Collection,
- Kernel Object Internals,
- konkrete Gerätetreiberstrukturen,
- die vollständige Serialisierung aller Runtime Objects.

Sie definiert stattdessen die gemeinsame semantische Grundlage aller Compute Objects.

---

## 5. Grundlegende Objektstruktur

Jedes Compute Object MUSS logisch mindestens folgende Eigenschaften besitzen können:

```text
ObjectID
ObjectType
ObjectState
Owner
SecurityDomain
RuntimeGeneration
```

Je nach Objekttyp SOLLEN zusätzlich vorhanden sein können:

```text
DebugName
ParentObject
CreationFlags
AccessRights
BackendObject
UserMetadata
ReferenceState
```

---

## 6. Object ID

Jedes aktive Compute Object MUSS innerhalb seiner Runtime-Domäne eindeutig identifizierbar sein.

Beispiel:

```text
ObjectID = 0x000000000001A42F
```

Eine `ObjectID` ist nicht automatisch mit einem öffentlichen Handle identisch.

---

## 7. Object ID und Handle

Folgende Konzepte MÜSSEN getrennt betrachtet werden:

```text
ObjectID
Handle
BackendID
DeviceID
```

Eine Object ID identifiziert ein Runtime Object intern.

Ein Handle ermöglicht kontrollierten Zugriff durch eine Anwendung.

---

## 8. Object-ID-Wiederverwendung

Object IDs DÜRFEN nach der vollständigen Zerstörung eines Objekts wiederverwendet werden.

Die Runtime MUSS jedoch verhindern, dass alte Handles dadurch versehentlich auf neu erzeugte Objekte zeigen.

---

## 9. Handle Generation

Zur Erkennung veralteter Handles SOLL ein Generationsmechanismus verwendet werden.

Konzeptionell:

```text
Handle
 |
 +-- Object Index
 |
 +-- Generation
 |
 +-- Object Type
```

Wird ein Object Slot erneut verwendet:

```text
Generation = Generation + 1
```

Ein Handle mit einer alten Generation MUSS anschließend als ungültig erkannt werden.

---

## 10. Opaque Handles

Öffentliche Compute Objects SOLLEN über opaque Handles angesprochen werden.

Beispiele:

```text
NovaComputeRuntimeHandle
NovaComputeDeviceHandle
NovaComputeContextHandle
NovaComputeQueueHandle
NovaComputeJobHandle
NovaComputeModuleHandle
NovaComputeKernelHandle
NovaComputeBufferHandle
NovaComputeImageHandle
NovaComputeTensorHandle
NovaComputeEventHandle
NovaComputeFenceHandle
NovaComputeGraphHandle
```

Die interne Struktur eines Handles DARF nicht Bestandteil des öffentlichen ABI sein.

---

## 11. Handle-Anforderungen

Ein Handle MUSS mindestens folgende Prüfungen ermöglichen:

```text
Validity
Object Type
Generation
Ownership
Access Rights
Security Domain
```

---

## 12. Keine direkten Runtime Pointer

Normale Anwendungen DÜRFEN keine direkten Pointer auf interne Runtime Objects erhalten.

Unzulässig:

```text
Application
    |
    v
InternalRuntimeObject*
```

Zulässig:

```text
Application
    |
    v
Opaque Handle
    |
    v
Handle Resolver
    |
    v
Runtime Object
```

---

## 13. Handle-Typisierung

Handles SOLLEN logisch typisiert sein.

Beispiel:

```text
BufferHandle -> Buffer
QueueHandle  -> Queue
KernelHandle -> Kernel
```

Ein Buffer Handle DARF nicht als Queue Handle verwendet werden.

---

## 14. Falscher Handle-Typ

Wird ein gültiger Handle mit einem falschen erwarteten Objekttyp verwendet, MUSS die Runtime einen definierten Fehler zurückgeben.

Beispiel:

```text
INVALID_OBJECT_TYPE
```

---

## 15. Null Handle

Für optionale Referenzen SOLL ein definierter Null Handle existieren.

Beispiel:

```text
NOVA_COMPUTE_NULL_HANDLE
```

Ein Null Handle DARF niemals auf ein reales Objekt zeigen.

---

## 16. Invalid Handle

Ein ungültiger Handle MUSS sicher erkannt werden.

Er DARF nicht:

- einen Speicherfehler verursachen,
- einen Kernel Panic verursachen,
- auf fremde Objekte zeigen,
- interne Runtime-Daten offenlegen.

---

## 17. Stale Handle

Ein Handle auf ein bereits zerstörtes oder wiederverwendetes Objekt MUSS erkannt werden können.

Fehler:

```text
STALE_HANDLE
```

---

## 18. Handle Namespace

Handles MÜSSEN mindestens gegenüber unterschiedlichen Prozessen oder Security Domains geschützt sein.

Ein numerisch identischer Handle in zwei unterschiedlichen Prozessen MUSS nicht dasselbe Objekt bezeichnen.

---

## 19. Handle Forgery Protection

Eine Anwendung DARF durch das Erraten numerischer Handle-Werte keinen Zugriff auf fremde Compute Objects erhalten.

---

## 20. Object Types

Die Runtime MUSS mindestens folgende grundlegende Objekttypen modellieren können:

```text
RUNTIME
DEVICE
CONTEXT
QUEUE
JOB
COMMAND
PROGRAM
MODULE
KERNEL
RESOURCE
BUFFER
EVENT
FENCE
GRAPH
```

Eine vollständige Runtime SOLL zusätzlich unterstützen können:

```text
IMAGE
TENSOR
SAMPLER
PIPELINE
MEMORY_POOL
TIMELINE
EXTERNAL_RESOURCE
PROFILING_SESSION
```

---

## 21. Objekttyp-Hierarchie

Das Object Model DARF logische Typ-Hierarchien definieren.

Beispiel:

```text
OBJECT
 |
 +-- DEVICE
 |
 +-- CONTEXT
 |
 +-- QUEUE
 |
 +-- RESOURCE
 |    |
 |    +-- BUFFER
 |    |
 |    +-- IMAGE
 |    |
 |    +-- TENSOR
 |
 +-- SYNC_OBJECT
      |
      +-- EVENT
      |
      +-- FENCE
      |
      +-- TIMELINE
```

Diese Hierarchie schreibt keine objektorientierte Implementierung vor.

---

## 22. RESOURCE

`RESOURCE` ist ein logischer Basistyp für speicherbasierte Compute Objects.

Mindestens:

```text
BUFFER
IMAGE
TENSOR
```

SOLLen auf diesem Modell aufbauen können.

---

## 23. SYNC_OBJECT

Synchronisationsobjekte DÜRFEN logisch unter einem gemeinsamen Basistyp zusammengefasst werden.

Beispiel:

```text
SYNC_OBJECT
 |
 +-- EVENT
 |
 +-- FENCE
 |
 +-- TIMELINE
```

---

## 24. Object Descriptor

Objekte SOLLEN über versionierte Descriptor-Strukturen erzeugt werden.

Beispiele:

```text
ComputeContextDescriptor
ComputeQueueDescriptor
ComputeBufferDescriptor
ComputeKernelDescriptor
ComputeGraphDescriptor
```

---

## 25. Descriptor Header

Ein Descriptor SOLL mindestens folgende Felder besitzen können:

```text
StructureType
StructureSize
Version
Flags
```

---

## 26. Structure Type

`StructureType` MUSS den konkreten Descriptor-Typ eindeutig kennzeichnen.

---

## 27. Structure Size

`StructureSize` SOLL der Runtime ermöglichen, unterschiedliche Versionen derselben Struktur zu erkennen.

---

## 28. Descriptor Version

Descriptoren SOLLEN explizit versioniert werden können.

Beispiel:

```text
Version = 1
```

---

## 29. Erweiterbare Descriptoren

Descriptoren MÜSSEN so gestaltet werden, dass spätere Runtime-Versionen neue optionale Felder ergänzen können.

---

## 30. Extension Chain

Komplexe Descriptoren DÜRFEN eine Erweiterungskette unterstützen.

Beispiel:

```text
Base Descriptor
      |
      v
Extension A
      |
      v
Extension B
```

---

## 31. Unbekannte Extensions

Eine unbekannte optionale Extension DARF ignoriert werden, wenn ihre Semantik dies erlaubt.

Eine unbekannte verpflichtende Extension MUSS zu einem definierten Fehler führen.

---

## 32. Objektlebenszyklus

Ein generisches Compute Object SOLL mindestens folgende Zustände besitzen können:

```text
CREATING
READY
IN_USE
RELEASE_PENDING
DESTROYING
DESTROYED
```

Objektspezifische Zustände DÜRFEN zusätzlich existieren.

---

## 33. Objekterstellung

Die allgemeine Objekterstellung lautet:

```text
Descriptor
    |
    v
Validation
    |
    v
Permission Check
    |
    v
Resource Allocation
    |
    v
Backend Creation
    |
    v
Object Registration
    |
    v
Handle Creation
    |
    v
READY
```

---

## 34. Atomare Objekterstellung

Eine fehlgeschlagene Objekterstellung DARF kein teilweise nutzbares öffentliches Objekt hinterlassen.

Das Ergebnis MUSS entweder sein:

```text
SUCCESS
+
Valid Handle
```

oder:

```text
ERROR
+
No Valid Handle
```

---

## 35. Rollback

Intern bereits erzeugte Teilressourcen MÜSSEN bei fehlgeschlagener Objekterstellung wieder freigegeben werden.

---

## 36. READY

Ein Objekt im Zustand:

```text
READY
```

DARF entsprechend seiner Typspezifikation verwendet werden.

---

## 37. IN_USE

`IN_USE` bezeichnet ein Objekt, das aktuell durch mindestens eine Compute Operation verwendet wird.

Ein Objekt im Zustand `IN_USE` kann weiterhin einen gültigen öffentlichen Handle besitzen.

---

## 38. RELEASE_PENDING

Wird ein noch intern verwendetes Objekt öffentlich freigegeben, DARF die Runtime es in:

```text
RELEASE_PENDING
```

überführen.

---

## 39. Deferred Destruction

Die physische Zerstörung eines Objekts DARF verzögert werden, bis keine laufende Operation mehr darauf zugreift.

---

## 40. DESTROYING

Während:

```text
DESTROYING
```

dürfen keine neuen normalen Operationen auf dem Objekt gestartet werden.

---

## 41. DESTROYED

Ein Objekt im Zustand:

```text
DESTROYED
```

DARF nicht mehr verwendet werden.

Der zugehörige öffentliche Handle MUSS ungültig sein.

---

## 42. Asynchrone Lebensdauer

Das Object Model MUSS asynchrone Ausführung korrekt berücksichtigen.

Beispiel:

```text
Buffer
  |
  +--> Job submitted
  |
  +--> Application releases Buffer
  |
  +--> Job still running
  |
  +--> Job completes
  |
  +--> Buffer physically destroyed
```

---

## 43. Public Lifetime

Die öffentliche Lebensdauer wird durch die Anwendung kontrolliert.

Beispiel:

```text
Create
 |
 v
Application owns Handle
 |
 v
Release
```

---

## 44. Internal Lifetime

Die Runtime DARF unabhängig vom öffentlichen Handle interne Referenzen halten.

Beispiele:

```text
Job -> Buffer
Queue -> Context
Kernel -> Module
Graph -> Kernel
Command -> Resource
```

---

## 45. Referenzmodell

Eine Implementierung DARF Reference Counting verwenden.

Beispiel:

```text
PublicReferences
InternalReferences
```

---

## 46. Reference Counting nicht verpflichtend

Diese Spezifikation schreibt keine konkrete Lifetime-Implementierung vor.

Mögliche Verfahren:

- Reference Counting,
- Deferred Destruction,
- Object Table Ownership,
- Epoch-Based Reclamation,
- Hazard Pointers,
- RCU,
- Kombinationen dieser Verfahren.

Die öffentliche Semantik MUSS identisch bleiben.

---

## 47. Strong Reference

Eine Strong Reference verhindert die physische Zerstörung des referenzierten Objects.

---

## 48. Weak Reference

Eine Weak Reference verhindert die Zerstörung nicht.

Der Nutzer einer Weak Reference MUSS mit dem Verschwinden des Objekts umgehen können.

---

## 49. Referenzzyklen

Die Runtime MUSS verhindern, dass starke Referenzzyklen dauerhaft Objekte leaken.

Beispiel:

```text
A -> B
^    |
|    v
+----C
```

---

## 50. Cycle Prevention

Ownership-Strukturen SOLLEN so gestaltet werden, dass starke Referenzzyklen möglichst vermieden werden.

---

## 51. Ownership

Jedes Compute Object MUSS einen logischen Owner besitzen können.

Typischerweise:

```text
Runtime
```

oder:

```text
Compute Context
```

---

## 52. Owner Process

Compute Objects MÜSSEN einem Prozess oder einer entsprechenden Security Domain zugeordnet werden können.

---

## 53. Context Ownership

Der Compute Context SOLL die primäre Ownership Domain normaler Compute Objects darstellen.

Beispiel:

```text
Context
 |
 +-- Queue
 |
 +-- Job
 |
 +-- Module
 |
 +-- Kernel
 |
 +-- Buffer
 |
 +-- Image
 |
 +-- Tensor
 |
 +-- Event
 |
 +-- Fence
 |
 +-- Graph
```

---

## 54. Parent-Child-Beziehungen

Compute Objects DÜRFEN Parent-Child-Beziehungen besitzen.

Beispiel:

```text
Runtime
 |
 +-- Device
 |
 +-- Context
      |
      +-- Queue
      |
      +-- Buffer
      |
      +-- Kernel
      |
      +-- Event
```

---

## 55. Parent Lifetime

Die Zerstörung eines Parent Objects DARF nicht zu Use-After-Free in noch laufenden Child Operations führen.

---

## 56. Context Destruction

Wird ein Context zerstört, MUSS die Runtime seine Child Objects kontrolliert behandeln.

Ein möglicher Ablauf lautet:

```text
Stop New Submissions
        |
        v
Cancel / Drain Jobs
        |
        v
Release Child Objects
        |
        v
Release Backend State
        |
        v
Destroy Context
```

---

## 57. Implizites Child Cleanup

Beim Context-Abbau DARF die Runtime noch existierende Child Objects automatisch freigeben.

Dies MUSS sicher und deterministisch erfolgen.

---

## 58. Device Object

Ein Device Object repräsentiert eine physische, virtuelle oder logische Compute-Einheit.

Es SOLL mindestens enthalten können:

```text
DeviceID
DeviceClass
Capabilities
Topology
MemoryDomains
Backend
HealthState
Availability
```

---

## 59. Device Lifetime

Device Objects werden typischerweise durch die Runtime verwaltet und können länger leben als einzelne Contexts.

---

## 60. Device States

Ein Device Object SOLL mindestens folgende Zustände besitzen können:

```text
AVAILABLE
BUSY
QUIESCING
RESETTING
LOST
REMOVED
```

---

## 61. Device Removal

Nach:

```text
REMOVED
```

DARF ein Device Handle keine neuen normalen Compute Operationen zulassen.

---

## 62. Device Lost

Bei einem ungeplanten Geräteverlust MUSS der Zustand:

```text
LOST
```

oder eine äquivalente Semantik verwendet werden.

---

## 63. Context Object

Ein Context Object definiert mindestens:

- Ownership,
- Security Domain,
- Device Access,
- Scheduling Domain,
- Memory Limits,
- Compute Limits,
- Resource Limits.

---

## 64. Queue Object

Eine Queue gehört mindestens zu einem:

```text
Context
```

und typischerweise zu einem:

```text
Device
```

---

## 65. Queue States

Eine Queue SOLL mindestens folgende Zustände besitzen können:

```text
READY
ACTIVE
DRAINING
STOPPED
LOST
DESTROYED
```

---

## 66. Job Object

Ein Job Object repräsentiert eine logisch zusammengehörige Compute-Ausführung.

Es kann Referenzen enthalten auf:

```text
Queue
Kernel
Resources
Events
Dependencies
```

---

## 67. Job States

Ein Job Object SOLL mindestens folgende Zustände besitzen können:

```text
CREATED
QUEUED
WAITING
RUNNING
COMPLETED
FAILED
CANCEL_REQUESTED
CANCELLED
DEVICE_LOST
```

---

## 68. Job Lifetime

Ein Job MUSS mindestens solange existieren, bis sein beobachtbarer Abschlusszustand feststeht.

---

## 69. Command Object

Commands DÜRFEN als:

- eigenständige Runtime Objects,
- interne Job-Strukturen,
- Command-List-Einträge

implementiert werden.

---

## 70. Program Object

Ein Program Object DARF eine Sammlung oder Beschreibung von Compute Code repräsentieren.

---

## 71. Module Object

Ein Module Object repräsentiert geladenen oder kompilierten Compute Code.

Beispiele:

```text
Nova Compute IR
Portable Compute Binary
Native Device Binary
```

---

## 72. Module Lifetime

Kernel Objects DÜRFEN ihr zugrunde liegendes Module intern referenzieren.

Wird der öffentliche Module Handle freigegeben, DARF das Module nicht physisch zerstört werden, solange ein Kernel darauf angewiesen ist.

---

## 73. Kernel Object

Ein Kernel Object repräsentiert eine ausführbare Compute-Funktion.

---

## 74. Kernel Immutability

Ein erfolgreich erstellter Kernel SOLL weitgehend immutable sein.

Dies erleichtert:

- parallele Nutzung,
- Caching,
- Thread Safety,
- Wiederverwendung.

---

## 75. Resource Object

Resource Objects repräsentieren Compute Memory gemäß:

```text
NPSPEC-COMPUTE-RUNTIME-0003
```

Sie MÜSSEN mindestens integrieren:

```text
Owner
Handle
Type
Access Rights
Lifetime
Residency
Usage State
```

---

## 76. Buffer Object

Ein Buffer ist eine lineare Compute Resource.

Beispiel:

```text
Buffer
 |
 +-- Size
 +-- Alignment
 +-- Usage
 +-- MemoryClass
 +-- AccessRights
 +-- Residency
```

---

## 77. Image Object

Ein Image Object SOLL strukturierte Bilddaten repräsentieren können.

Mögliche Eigenschaften:

```text
Width
Height
Depth
Format
MipLevels
ArrayLayers
Usage
```

---

## 78. Tensor Object

Ein Tensor Object SOLL mathematische oder AI-Datenstrukturen repräsentieren können.

Mögliche Eigenschaften:

```text
ElementType
Rank
Shape
Strides
Layout
Quantization
```

---

## 79. Resource Views

Die Runtime SOLL Views auf Ressourcen unterstützen können.

Beispiel:

```text
Buffer
 |
 +-- Tensor View A
 |
 +-- Tensor View B
```

---

## 80. View Lifetime

Eine View MUSS ihre zugrunde liegende Resource intern referenzieren.

Die Resource DARF nicht physisch zerstört werden, solange eine gültige View existiert.

---

## 81. View Ownership

Eine View kann einen eigenen Handle besitzen, ohne eigenen physischen Speicher zu besitzen.

---

## 82. Sampler Object

Sampler Objects DÜRFEN Abtastparameter für Image Resources kapseln.

Sampler SOLLEN vorzugsweise immutable sein.

---

## 83. Event Object

Events repräsentieren Synchronisations- oder Abschlusszustände.

Mögliche Zustände:

```text
UNSIGNALED
SIGNALED
ERROR
```

---

## 84. Fence Object

Fences repräsentieren Synchronisationsfortschritt.

Ein Fence SOLL mindestens folgende Operationen unterstützen können:

```text
WAIT
SIGNAL
QUERY
```

---

## 85. Timeline Object

Eine vollständige Runtime SOLL monotone Timeline Objects unterstützen können.

Beispiel:

```text
0
1
2
3
4
5
...
```

---

## 86. Graph Object

Ein Graph Object repräsentiert einen Compute-Abhängigkeitsgraphen.

Beispiel:

```text
A
|
+--> B
|
+--> C
     |
     v
     D
```

---

## 87. Graph States

Ein Graph kann beispielsweise folgende Zustände besitzen:

```text
BUILDING
FINALIZED
EXECUTABLE
INVALID
```

---

## 88. Graph Immutability

Ein finalisierter Graph SOLL immutable sein können.

Dadurch kann die Runtime:

- Scheduling vorberechnen,
- Speicherlebenszeiten analysieren,
- Resource Aliasing planen,
- Device Placement vorbereiten,
- Optimierungen cachen.

---

## 89. Pipeline Object

Eine Pipeline DARF vorkonfigurierte oder vorkompilierte Compute-Ausführungszustände kapseln.

Sie SOLL wiederverwendbar sein.

---

## 90. Memory Pool Object

Ein Memory Pool Object DARF Compute Allocations bündeln.

Beispiele:

```text
HOST_POOL
DEVICE_POOL
TEMPORARY_POOL
UPLOAD_POOL
DOWNLOAD_POOL
```

---

## 91. External Resource Object

Ein External Resource Object repräsentiert eine aus einem anderen Subsystem importierte Resource.

Beispiele:

- Grafikresource,
- Videobuffer,
- IPC-Speicher,
- Kameraresource.

---

## 92. Immutable Objects

Wo möglich SOLLEN Objekte nach Erstellung immutable sein.

Geeignete Beispiele:

```text
Kernel
Sampler
Finalized Graph
Pipeline
```

---

## 93. Mutable Objects

Objekte mit Laufzeitzustand sind typischerweise mutable.

Beispiele:

```text
Queue
Job
Event
Fence
Resource Residency
Device Health
```

---

## 94. Mutability-Klassen

Für Objekttypen SOLL definiert werden können:

```text
IMMUTABLE
MUTABLE
RUNTIME_MANAGED
```

---

## 95. Object State Validation

Jede Operation MUSS prüfen, ob das Objekt im aktuellen Zustand für die Operation geeignet ist.

Beispiel:

```text
QueueState = DESTROYED

Submit(Job)
```

MUSS fehlschlagen.

---

## 96. Invalid Object State

Für unzulässige Zustände MUSS ein definierter Fehler vorhanden sein.

Beispiel:

```text
INVALID_OBJECT_STATE
```

---

## 97. Zustandsübergänge

Objektzustände DÜRFEN nur über definierte Übergänge verändert werden.

---

## 98. Thread Safety

Jeder öffentliche Objekttyp MUSS eine definierte Thread-Safety-Semantik besitzen.

Mögliche Klassen:

```text
THREAD_SAFE
EXTERNALLY_SYNCHRONIZED
IMMUTABLE
```

---

## 99. Handle Thread Safety

Die Übergabe und grundlegende Validierung eines gültigen Handles MUSS aus mehreren Threads sicher möglich sein.

---

## 100. Immutable Thread Safety

Immutable Objects SOLLEN ohne zusätzliche Anwendungssynchronisation parallel verwendet werden können.

---

## 101. Mutable Thread Safety

Mutable Objects MÜSSEN:

- intern synchronisiert

oder:

- ausdrücklich als externally synchronized

definiert sein.

---

## 102. Locking

Diese Spezifikation schreibt keine konkrete Synchronisationsstrategie vor.

Mögliche Implementierungen:

```text
Mutex
Spinlock
RW Lock
Atomics
Lock-Free
RCU
Epoch Reclamation
```

---

## 103. Hot Paths

Häufige Handle-Auflösung SOLL keine unnötigen globalen Locks verwenden.

---

## 104. Object Table

Eine Implementierung DARF eine Object Table verwenden.

Beispiel:

```text
Handle
  |
  v
+-------------------+
| Object Table      |
+-------------------+
| Slot 0            |
| Slot 1            |
| Slot 2            |
| ...               |
+-------------------+
  |
  v
Runtime Object
```

---

## 105. Object Table Entry

Ein interner Eintrag kann beispielsweise enthalten:

```text
ObjectPointer
Generation
ObjectType
Owner
AccessFlags
State
```

Dies ist nicht ABI-verbindlich.

---

## 106. Per-Process Object Table

Object Tables SOLLEN pro Prozess oder Security Domain partitionierbar sein.

---

## 107. Per-Context Object Table

Zusätzliche Context-spezifische Tabellen DÜRFEN verwendet werden.

---

## 108. Fast Handle Lookup

Handle-Auflösung SOLL möglichst konstante oder annähernd konstante Laufzeit besitzen.

Bevorzugt:

```text
O(1)
```

---

## 109. Handle Validation Fast Path

Ein Fast Path kann prüfen:

```text
Index
Generation
Type
Owner
Access
```

---

## 110. Slow Validation Path

Komplexere Prüfungen DÜRFEN über einen Slow Path erfolgen.

Beispiele:

- Cross-Process Import,
- Revocation,
- Security Policy,
- Device Recovery,
- externe Resource Ownership.

---

## 111. Object Access Rights

Objekte MÜSSEN Zugriffsrechte besitzen können.

Mindestens:

```text
READ
WRITE
EXECUTE
SUBMIT
WAIT
SIGNAL
EXPORT
IMPORT
DESTROY
QUERY
```

---

## 112. Ownership und Zugriff

Ownership und Access Rights sind getrennte Konzepte.

Ein fremder Context kann beispielsweise nur:

```text
READ
```

auf eine geteilte Resource erhalten.

---

## 113. Capability Handles

Handles DÜRFEN intern capability-basierte Rechte repräsentieren.

Beispiel:

```text
Buffer Handle A:
READ

Buffer Handle B:
READ + WRITE

Buffer Handle C:
READ + EXPORT
```

---

## 114. Object Sharing

Bestimmte Objekttypen DÜRFEN explizit geteilt werden.

Typische Beispiele:

```text
BUFFER
IMAGE
TENSOR
EVENT
FENCE
TIMELINE
```

---

## 115. Shareable Flag

Ein Objekt SOLL bei Erstellung als teilbar markiert werden können.

Beispiel:

```text
SHAREABLE
```

---

## 116. Nicht teilbare Objekte

Ein Objekt ohne entsprechende Freigabe DARF nicht exportiert werden.

---

## 117. Export Object

Ein exportierbares Objekt kann einen externen Handle erzeugen.

Ablauf:

```text
Compute Object
      |
      v
Export Validation
      |
      v
External Handle
```

---

## 118. Import Object

Ein gültiger External Handle kann in einem anderen autorisierten Context importiert werden.

---

## 119. Import erzeugt lokale Referenz

Ein Import SOLL einen lokalen Compute Handle erzeugen.

Die zugrunde liegende Resource kann physisch geteilt bleiben.

---

## 120. Import Rights

Beim Import MÜSSEN die gewährten Rechte erhalten oder weiter eingeschränkt werden.

Eine Rechteausweitung DARF nicht stattfinden.

---

## 121. Export Lifetime

Das Freigeben des ursprünglichen Handles DARF eine noch gültig exportierte Resource nicht zwangsläufig sofort zerstören.

---

## 122. Revocation

Die Runtime SOLL für geeignete Sharing-Szenarien Revocation unterstützen können.

---

## 123. Cross-Process Sharing

Cross-Process-Sharing MUSS über kontrollierte NovaOS-IPC- oder Handle-Transfermechanismen erfolgen.

Direkte interne Pointer DÜRFEN nicht übertragen werden.

---

## 124. Cross-Context Sharing

Objekte DÜRFEN zwischen Contexts desselben Prozesses geteilt werden, sofern:

- der Objekttyp dies unterstützt,
- die erforderlichen Rechte bestehen.

---

## 125. Cross-Device Objects

Ein logisches Resource Object DARF auf mehreren Devices verwendet werden.

Die logische Objektidentität bleibt dabei erhalten.

---

## 126. Residency und Object Identity

Folgendes Objekt:

```text
BufferHandle A
```

bleibt dasselbe logische Object, auch wenn seine Daten von:

```text
CPU RAM
```

nach:

```text
GPU VRAM
```

migriert werden.

---

## 127. Device Affinity

Objekte DÜRFEN Device Affinity besitzen.

Beispiele:

```text
PreferredDevice = GPU0
```

```text
RequiredDevice = NPU0
```

---

## 128. Multi-Device Objects

Bestimmte Objekte DÜRFEN mehrere Devices umfassen.

Beispiele:

```text
Context
Graph
Resource
```

---

## 129. Backend Object

Ein öffentliches Runtime Object DARF ein oder mehrere interne Backend Objects besitzen.

Beispiel:

```text
Compute Buffer
     |
     +--> CPU Backend State
     |
     +--> GPU Backend Allocation
     |
     +--> NPU Backend Mapping
```

---

## 130. Backend Isolation

Backend Objects DÜRFEN normalen Anwendungen nicht direkt zugänglich sein.

---

## 131. Lazy Backend Creation

Backend Objects DÜRFEN erst bei tatsächlicher Nutzung erzeugt werden.

Beispiel:

```text
Logical Buffer
     |
     v
No GPU Allocation
     |
     v
First GPU Use
     |
     v
Create GPU Allocation
```

---

## 132. Lazy Materialization

Ein logisches Compute Object DARF intern erst bei Bedarf vollständig materialisiert werden.

---

## 133. Object Caching

Immutable oder wiederverwendbare interne Objects DÜRFEN gecacht werden.

Beispiele:

```text
Compiled Kernel
Pipeline
Sampler
Graph Plan
Descriptor Translation
```

---

## 134. Object Deduplication

Identische immutable Objects DÜRFEN intern denselben Backend State verwenden.

Beispiel:

```text
Sampler A
Sampler B
```

können intern dedupliziert werden.

Die öffentlichen Handles bleiben logisch unabhängig.

---

## 135. Copy-on-Write

Geteilte interne Zustände DÜRFEN bei Mutation über Copy-on-Write getrennt werden.

---

## 136. Debug Name

Entwickler SOLLEN Compute Objects einen Debug-Namen geben können.

Beispiel:

```text
"MainInferenceInput"
```

Ein Debug-Name DARF keinen Einfluss auf die Programmlogik haben.

---

## 137. Debug Labels

Jobs oder Command-Bereiche SOLLEN optionale Debug Labels unterstützen können.

Beispiel:

```text
BeginLabel("Image Preprocessing")

...

EndLabel()
```

---

## 138. User Metadata

Objekte DÜRFEN begrenzte benutzerdefinierte Metadaten besitzen.

Diese Metadaten DÜRFEN keine Sicherheits- oder Scheduling-Regeln umgehen.

---

## 139. Object Introspection

Die Runtime SOLL kontrollierte Introspection ermöglichen.

Mindestens abfragbar SOLLEN sein können:

```text
ObjectType
ObjectState
OwnerContext
CreationFlags
DebugName
```

---

## 140. Device Introspection

Device Objects SOLLEN unter anderem abfragbar machen:

```text
DeviceClass
Capabilities
HealthState
Availability
MemoryDomains
Topology
```

---

## 141. Resource Introspection

Resource Objects SOLLEN unter anderem abfragbar machen:

```text
Size
Type
Usage
Access
MemoryClass
Residency
```

---

## 142. Immutable Query Results

Eigenschaften, die bei Erstellung unveränderlich festgelegt wurden, SOLLEN stabile Query-Ergebnisse liefern.

---

## 143. Dynamic Query Results

Dynamische Eigenschaften DÜRFEN sich verändern.

Beispiele:

```text
JobState
QueueDepth
Residency
DeviceHealth
FenceValue
```

---

## 144. Query Snapshot

Eine dynamische Objektabfrage liefert nur einen Snapshot.

Der Zustand DARF sich unmittelbar nach der Abfrage ändern.

---

## 145. Object Dependencies

Die Runtime MUSS Objektabhängigkeiten verwalten können.

Beispiel:

```text
Job
 |
 +--> Kernel
 |
 +--> Buffer A
 |
 +--> Buffer B
 |
 +--> Event
```

---

## 146. Dependency Lifetime

Objekte, die durch einen laufenden Job benötigt werden, MÜSSEN intern gültig bleiben.

---

## 147. Zerstörungsreihenfolge

Bei komplexen Abhängigkeiten MUSS eine sichere Destruction Order existieren.

Beispiel:

```text
Stop Jobs
   |
   v
Drain Queues
   |
   v
Release Graphs
   |
   v
Release Kernels
   |
   v
Release Resources
   |
   v
Destroy Context
```

---

## 148. Explicit Release

Öffentliche Objekte SOLLEN explizit freigegeben werden können.

Beispiel:

```text
Release(handle)
```

---

## 149. Double Release

Ein doppeltes Release eines bereits ungültigen Handles MUSS kontrolliert fehlschlagen.

Es DARF keine doppelte physische Freigabe verursachen.

---

## 150. Release eines Null Handles

Das Verhalten von:

```text
Release(NOVA_COMPUTE_NULL_HANDLE)
```

MUSS API-weit einheitlich definiert werden.

Empfohlen ist:

```text
No Operation
```

---

## 151. Deferred Release

Ein Release beendet die öffentliche Nutzbarkeit eines Handles.

Es MUSS nicht notwendigerweise die sofortige physische Objektzerstörung auslösen.

---

## 152. Forced Destruction

Privilegierte Runtime-Komponenten DÜRFEN bei folgenden Ereignissen eine forcierte Bereinigung einleiten:

- Prozessabbruch,
- Runtime Shutdown,
- Device Loss,
- Sicherheitsverletzung.

---

## 153. Process Exit

Beim Prozessende MUSS die Runtime sämtliche diesem Prozess zugeordneten Compute Objects erfassen können.

---

## 154. Process Cleanup

Ein möglicher Cleanup-Ablauf lautet:

```text
Process Exit
    |
    v
Block New Compute Calls
    |
    v
Cancel / Detach Jobs
    |
    v
Release Shares
    |
    v
Destroy Contexts
    |
    v
Release Objects
    |
    v
Destroy Process Object Table
```

---

## 155. Crash Cleanup

Auch bei einem ungeplanten Prozessabsturz MUSS die Compute Runtime alle zugehörigen Objekte bereinigen können.

---

## 156. Prozessisolierte Objektlebensdauer

Ein Prozessabsturz DARF keine dauerhaft belegten Compute Objects hinterlassen.

Ausgenommen sind ausdrücklich unabhängige, persistente oder systemweit geteilte Objekte.

---

## 157. Device Loss und Objects

Objekte, die von einem verlorenen Device abhängen, MÜSSEN entsprechend markiert werden können.

Beispiele:

```text
DEVICE_LOST
RESOURCE_LOST
OBJECT_INVALIDATED
```

---

## 158. Object Invalidation

Ein Handle kann weiterhin syntaktisch gültig sein, obwohl das zugehörige Object nicht mehr normal verwendbar ist.

Beispiel:

```text
Handle = VALID

ObjectState = RESOURCE_LOST
```

---

## 159. Handle Validity vs Object Usability

Die Runtime MUSS unterscheiden zwischen:

```text
Handle Validity
Object State
Backend Availability
Access Rights
```

Ein gültiger Handle bedeutet nicht automatisch, dass jede Operation zulässig ist.

---

## 160. Recoverable Object

Objekte DÜRFEN nach einem Device Reset wiederhergestellt werden, wenn ihre logische Beschreibung erhalten geblieben ist.

---

## 161. RECREATABLE

Objekte SOLLEN optional mit:

```text
RECREATABLE
```

markiert werden können.

Typische Beispiele:

- Pipeline Backend State,
- Kernel Backend State,
- temporäre Caches,
- regenerierbare Resources.

---

## 162. Non-Recreatable Object

Objekte mit nicht rekonstruierbarem Zustand MÜSSEN nach Verlust als ungültig oder verloren gemeldet werden.

---

## 163. Runtime Generation

Die Runtime SOLL eine:

```text
RuntimeGeneration
```

besitzen.

Nach vollständigem Runtime Reset MUSS eine neue Generation verwendet werden können.

---

## 164. Runtime Restart

Handles aus einer alten Runtime Generation MÜSSEN nach einem Runtime-Neustart ungültig sein.

---

## 165. Keine persistente Handle-Identität

Compute Handles DÜRFEN nicht als persistente IDs gespeichert und nach Neustart wiederverwendet werden.

---

## 166. Stable IDs

Wenn persistente Identität benötigt wird, MUSS ein davon getrenntes stabiles ID-Modell verwendet werden.

---

## 167. Error Model

Objektoperationen MÜSSEN definierte Result Codes liefern können.

Mindestens:

```text
SUCCESS

INVALID_HANDLE
STALE_HANDLE
INVALID_OBJECT_TYPE
INVALID_OBJECT_STATE

INVALID_ARGUMENT
ACCESS_DENIED

OUT_OF_MEMORY
RESOURCE_LIMIT

DEVICE_LOST
RESOURCE_LOST

BUSY
TIMEOUT

NOT_SUPPORTED
INTERNAL_ERROR
```

---

## 168. Fehleratomarität

Schlägt eine Objektoperation fehl, SOLL das Object entweder:

- unverändert bleiben

oder:

- in einen klar definierten Fehlerzustand übergehen.

Halbdefinierte Zustände DÜRFEN nicht öffentlich sichtbar werden.

---

## 169. Object Poisoning

Nach schwerwiegenden internen Fehlern DARF ein Object als:

```text
POISONED
```

markiert werden.

Normale Operationen MÜSSEN anschließend kontrolliert abgelehnt werden.

---

## 170. Security Domain

Jedes sicherheitsrelevante Compute Object MUSS einer Security Domain zugeordnet werden können.

---

## 171. Context Isolation

Ein Context DARF standardmäßig keine Objects eines anderen Contexts verwenden.

---

## 172. Process Isolation

Ein Prozess DARF keine Compute Objects eines anderen Prozesses verwenden, sofern diese nicht explizit geteilt wurden.

---

## 173. Privileged Objects

Bestimmte Objects oder Flags DÜRFEN nur privilegierten Systemkomponenten zugänglich sein.

Beispiele:

```text
SYSTEM_QUEUE
PROTECTED_MEMORY
DEVICE_ADMIN
EXCLUSIVE_DEVICE
SYSTEM_CRITICAL
```

---

## 174. Object Limits

Die Runtime MUSS Limits setzen können.

Beispiele:

```text
MaximumContexts
MaximumQueues
MaximumJobs
MaximumBuffers
MaximumImages
MaximumTensors
MaximumEvents
MaximumFences
MaximumGraphs
MaximumObjects
```

---

## 175. Resource Exhaustion Protection

Eine Anwendung DARF durch massenhafte Objekterstellung nicht unbegrenzt Runtime- oder Kernel-Ressourcen verbrauchen.

---

## 176. Quotas

Objektlimits DÜRFEN mit weiteren Quotas kombiniert werden.

Beispiele:

```text
MemoryQuota
JobQuota
QueueQuota
ObjectQuota
PinnedMemoryQuota
```

---

## 177. Object Accounting

Die Runtime MUSS Objekte mindestens ihrem Owner zuordnen können.

Diagnosewerte SOLLEN umfassen können:

```text
ObjectCount
ObjectCountByType
PeakObjectCount
PendingDestructions
```

---

## 178. Object Telemetry

Optional können folgende Werte erfasst werden:

```text
ObjectsCreated
ObjectsDestroyed
HandleLookups
InvalidHandleAttempts
StaleHandleAttempts
DeferredDestructions
```

---

## 179. Object Trace Events

Die Runtime SOLL mindestens folgende Trace Events erzeugen können:

```text
COMPUTE_OBJECT_CREATE
COMPUTE_OBJECT_READY
COMPUTE_OBJECT_RETAIN
COMPUTE_OBJECT_RELEASE
COMPUTE_OBJECT_DESTROY
COMPUTE_OBJECT_INVALIDATE
COMPUTE_OBJECT_IMPORT
COMPUTE_OBJECT_EXPORT
```

---

## 180. Debug Object Registry

Debug Builds SOLLEN eine Registry aktiver Compute Objects bereitstellen können.

Beispiel:

```text
Context 42

BUFFER    27
QUEUE      3
KERNEL     8
EVENT     14
GRAPH      2
```

---

## 181. Leak Detection

Beim Context- oder Prozessende SOLL die Runtime nicht explizit freigegebene Objects erkennen können.

---

## 182. Leak Report

Ein Entwicklerbericht kann beispielsweise enthalten:

```text
Leaked Object

Type: BUFFER
DebugName: MainInferenceInput
Size: 64 MiB
Context: 42
```

---

## 183. Object Dependency Diagnostics

Entwicklerwerkzeuge SOLLEN Object Dependencies darstellen können.

Beispiel:

```text
Context
 |
 +-- Queue
 |
 +-- Graph
      |
      +-- Kernel
      |
      +-- Buffer A
      |
      +-- Buffer B
```

---

## 184. Object Serialization

Normale Runtime Objects MÜSSEN nicht serialisierbar sein.

---

## 185. Serializable Descriptors

Descriptoren und bestimmte immutable Objektbeschreibungen DÜRFEN serialisierbar sein.

Beispiele:

```text
Pipeline Description
Graph Description
Kernel Metadata
Tensor Description
```

---

## 186. ABI-Stabilität

Interne Object-Strukturen DÜRFEN sich zwischen Runtime-Versionen verändern.

Das öffentliche Handle- und Descriptor-Verhalten MUSS der definierten ABI-Versionierung folgen.

---

## 187. Unknown Object Type

Eine Runtime MUSS unbekannte verpflichtende Objekttypen kontrolliert ablehnen.

---

## 188. Extension Object Types

Zukünftige Compute Extensions DÜRFEN neue Object Types definieren.

Diese MÜSSEN die grundlegenden Regeln dieser Spezifikation einhalten.

---

## 189. Vendor Objects

Hersteller DÜRFEN Erweiterungsobjekte bereitstellen.

Diese DÜRFEN nicht:

- Context-Isolation umgehen,
- Handle-Sicherheit umgehen,
- Process-Isolation umgehen,
- Sicherheitsrechte erweitern.

---

## 190. Object Type Namespace

Object-Type-IDs SOLLEN Namensräume besitzen können.

Beispiel:

```text
CORE
EXTENSION
VENDOR
EXPERIMENTAL
```

---

## 191. Experimental Objects

Experimentelle Object Types MÜSSEN keine langfristige ABI-Stabilität garantieren.

---

## 192. Object Factory

Intern DARF die Runtime eine Object Factory verwenden.

Beispiel:

```text
Descriptor
    |
    v
Object Factory
    |
    +--> Validation
    |
    +--> Security
    |
    +--> Allocation
    |
    +--> Backend
    |
    +--> Registration
    |
    v
Handle
```

---

## 193. Object Pooling

Häufig erzeugte kleine interne Objects DÜRFEN gepoolt werden.

Beispiele:

```text
Events
Commands
Job Records
Temporary Descriptors
```

---

## 194. Pool Reuse

Wiederverwendete Object Slots MÜSSEN vollständig neu initialisiert werden.

Insbesondere DÜRFEN alte:

- Owner,
- Rechte,
- Referenzen,
- Zustände,
- Debug-Daten

nicht auf neue Objects übergehen.

---

## 195. Generation bei Pooling

Bei Wiederverwendung eines öffentlichen Object Slots MUSS die Handle Generation aktualisiert werden.

---

## 196. Object Locality

Interne Object Tables SOLLEN CPU-Cache-Lokalität berücksichtigen können.

---

## 197. Object Table Sharding

Große Object Tables DÜRFEN partitioniert werden.

Beispiele:

```text
per Process
per Context
per Device
per CPU
```

---

## 198. Object Pinning

Die Runtime DARF interne Objects temporär gegen physische Zerstörung schützen.

Dies kann für lock-arme Handle-Auflösung erforderlich sein.

---

## 199. Hazard Protection

Lock-free Implementierungen DÜRFEN Mechanismen verwenden wie:

```text
Hazard Pointers
Epoch Reclamation
RCU
```

---

## 200. Reentrancy

Callbacks oder Completion Handler DÜRFEN Runtime-Funktionen erneut aufrufen, sofern dies für die jeweilige API erlaubt ist.

---

## 201. Callback Lifetime

Ein Object, das einem Callback übergeben wird, MUSS mindestens für die definierte Callback-Dauer gültig bleiben.

---

## 202. Completion Objects

Asynchrone Operationen SOLLEN Completion Objects liefern können.

Beispiele:

```text
Event
Fence
Timeline Value
```

---

## 203. Completion Lifetime

Ein Completion Object DARF unabhängig vom öffentlichen Job Handle weiterexistieren, wenn interne Referenzen bestehen.

---

## 204. Scheduler-Integration

Der Compute Scheduler gemäß:

```text
NPSPEC-COMPUTE-RUNTIME-0002
```

MUSS mit dem Object Model integriert sein.

Der Scheduler DARF keine:

- ungültigen,
- zerstörten,
- nicht autorisierten

Objects dispatchen.

---

## 205. Scheduler Object Dependencies

Vor dem Dispatch MUSS sichergestellt werden, dass alle für den Job erforderlichen Objects intern gültig bleiben.

Beispiel:

```text
Job
 |
 +--> Kernel
 |
 +--> Buffer A
 |
 +--> Buffer B
 |
 +--> Event
```

---

## 206. Memory-Model-Integration

Das Memory Model gemäß:

```text
NPSPEC-COMPUTE-RUNTIME-0003
```

verwendet Resource Objects des Object Models.

---

## 207. Logische vs physische Identität

Die physische Speicherplatzierung einer Resource DARF sich ändern.

Die logische Objektidentität bleibt erhalten.

Beispiel:

```text
BufferHandle
     |
     v
Buffer Object
     |
     +--> CPU RAM
     |
     +--> GPU VRAM
```

---

## 208. Device-Loss-Beispiel

Gegeben:

```text
Buffer Object
     |
     v
GPU0 Allocation
```

Dann:

```text
GPU0 Lost
```

Die Runtime prüft:

```text
Host Copy Available?
        |
   +----+----+
   |         |
  YES        NO
   |         |
   v         v
Recover    RESOURCE_LOST
```

---

## 209. Public und Internal Lifetime

Das allgemeine Modell lautet:

```text
Application
    |
    v
Public Handle
    |
    v
Runtime Object
    |
    +--> Internal Job Reference
    |
    +--> Queue Reference
    |
    +--> Backend Reference
```

Nach:

```text
Release(PublicHandle)
```

kann das Runtime Object weiterhin intern existieren.

---

## 210. Physische Zerstörung

Die physische Zerstörung darf erst erfolgen, wenn keine notwendigen Referenzen mehr existieren.

Konzeptionell:

```text
Public References   = 0
Internal References = 0
```

Dann:

```text
Destroy Object
```

Die Implementierung MUSS kein Reference Counting verwenden, solange die Semantik erhalten bleibt.

---

## 211. Handle-Modell

Eine mögliche interne Handle-Struktur lautet:

```text
+----------------+----------------+----------------+
| Object Type    | Generation     | Object Index   |
+----------------+----------------+----------------+
```

Zusätzliche Validierung kann berücksichtigen:

```text
Process
Context
Security Domain
Access Rights
```

Die konkreten Bitbreiten sind nicht Bestandteil dieser Spezifikation.

---

## 212. Sicherheitsprüfung

Ein Compute Handle wird als nicht vertrauenswürdige Eingabe behandelt.

Ablauf:

```text
Untrusted Handle
      |
      v
Index Validation
      |
      v
Generation Validation
      |
      v
Type Validation
      |
      v
Owner Validation
      |
      v
Access Validation
      |
      v
Object State Validation
      |
      v
Trusted Runtime Object
```

---

## 213. Testanforderungen — Handle Validity

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
Valid Handle
Null Handle
Random Handle
Stale Handle
Wrong Type
Wrong Process
Wrong Context
Old Generation
```

---

## 214. Testanforderungen — Object Lifecycle

Zu testen sind:

```text
Create
Use
Release
Deferred Release
Destroy
Double Release
Use After Release
```

---

## 215. Testanforderungen — Async Lifetime

Folgender Test MUSS vorhanden sein:

```text
Create Buffer
     |
     v
Submit Job using Buffer
     |
     v
Release Buffer immediately
     |
     v
Job continues
     |
     v
Job completes
```

Der Buffer DARF nicht vorzeitig physisch zerstört werden.

---

## 216. Testanforderungen — Parent Lifetime

Zu testen sind:

```text
Context with active Queue
Context with active Job
Context with active Resources
Context destruction during execution
```

---

## 217. Testanforderungen — Process Exit

Zu testen sind:

```text
Normal Process Exit
Process Crash
Forced Termination
Exit with Active Jobs
Exit with Shared Resources
```

---

## 218. Testanforderungen — Sharing

Mindestens:

```text
Export Buffer
Import Buffer
Read-Only Share
Read-Write Share
Unauthorized Import
Revocation
```

---

## 219. Testanforderungen — Security

Zu testen sind:

```text
Handle Forgery
Cross-Process Handle
Cross-Context Handle
Privilege Escalation
Invalid Capability
Stale External Handle
```

---

## 220. Testanforderungen — Device Loss

Zu testen sind:

```text
Device Loss with Queue
Device Loss with Kernel
Device Loss with Resource
Device Reset
Object Recovery
Object Invalidation
```

---

## 221. Testanforderungen — Thread Safety

Mehrere Threads MÜSSEN gleichzeitig folgende Operationen durchführen können:

```text
Create Objects
Query Objects
Submit Objects
Release Objects
```

Dabei DARF keine Object Table Corruption entstehen.

---

## 222. Release Race

Folgende Race Condition MUSS getestet werden:

```text
Thread A:
Release(Buffer)

Thread B:
Submit(Job using Buffer)
```

Das Ergebnis MUSS definiert sein.

Ein Use-After-Free DARF nicht entstehen.

---

## 223. Device-Lost Race

Folgende Race Condition MUSS getestet werden:

```text
Thread A:
Query(Resource)

Thread B:
DeviceLost()
```

Die Runtime MUSS einen konsistenten Zustand liefern.

---

## 224. Handle Generation Wrap

Falls der Generation Counter eine endliche Breite besitzt, MUSS dessen Überlauf sicher behandelt werden.

Ein alter stale Handle DARF dadurch nicht versehentlich wieder gültig werden.

---

## 225. Object Limit Tests

Zu testen sind:

```text
Maximum Objects
Maximum Contexts
Maximum Buffers
Maximum Queues
Maximum Jobs
Maximum Events
Maximum Graphs
```

---

## 226. Stress Tests

Stress Tests SOLLEN mindestens umfassen:

```text
Millions of Object Creations
Millions of Handle Lookups
Rapid Create / Destroy
Multiple Threads
Multiple Processes
Multiple Contexts
Device Reset under Load
Process Crash under Load
```

---

## 227. Fuzzing

Folgende Eingaben SOLLTEN fuzz-getestet werden:

```text
Handles
Object Descriptors
Object Types
Flags
Extension Chains
Import Descriptors
Export Descriptors
```

---

## 228. Fault Injection

Die Testumgebung SOLL mindestens folgende Fehler injizieren können:

```text
OUT_OF_MEMORY
BACKEND_CREATE_FAILED
DEVICE_LOST
RESOURCE_LOST
HANDLE_TABLE_FULL
ACCESS_DENIED
```

---

## 229. Performance Tests

Mindestens folgende Operationen SOLLEN gemessen werden:

```text
ObjectCreateLatency
ObjectDestroyLatency
HandleLookupLatency
HandleValidationLatency
ReferenceAcquireLatency
ReferenceReleaseLatency
```

---

## 230. Skalierungstests

Die Runtime SOLL mit steigender Objektzahl getestet werden.

Beispiel:

```text
1,000 Objects
10,000 Objects
100,000 Objects
1,000,000 Objects
```

Die Handle-Auflösung DARF nicht auf einer linearen Suche durch alle Objects basieren.

---

## 231. Leak Tests

Nach vollständigem Context-Abbau MUSS gelten können:

```text
ContextOwnedObjectCount == 0
```

Global verwaltete Runtime Objects sind davon ausgenommen.

---

## 232. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens folgende Objekttypen unterstützen:

```text
RUNTIME
DEVICE
CONTEXT
QUEUE
JOB
KERNEL
BUFFER
EVENT
```

Zusätzlich MUSS sie unterstützen:

```text
Opaque Handles
Type Validation
Ownership
Context Isolation
Process Isolation
Lifetime Management
Deferred Destruction
Handle Generation
Context Cleanup
Process Cleanup
```

---

## 233. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
PROGRAM
MODULE
IMAGE
TENSOR
SAMPLER
FENCE
TIMELINE
GRAPH
PIPELINE
MEMORY_POOL
EXTERNAL_RESOURCE
PROFILING_SESSION

Cross-Process Sharing
Object Export
Object Import
Revocation
Introspection
Debug Names
Leak Detection
Object Dependency Diagnostics
Lazy Materialization
Object Deduplication
```

---

## 234. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0004` MUSS:

1. Compute Objects eindeutig verwalten.
2. opaque Handles bereitstellen.
3. ungültige Handles erkennen.
4. stale Handles erkennen können.
5. falsche Handle-Typen erkennen.
6. Handle Generation unterstützen oder einen gleichwertigen Schutz bereitstellen.
7. Object Ownership verwalten.
8. Context-Isolation gewährleisten.
9. Process-Isolation gewährleisten.
10. Object Access Rights kontrollieren können.
11. asynchrone Object Lifetimes unterstützen.
12. vorzeitige Zerstörung verwendeter Objects verhindern.
13. Object States validieren.
14. definierte Objektzerstörung ermöglichen.
15. Double-Free verhindern.
16. Prozessende kontrolliert behandeln.
17. Context-Abbau kontrolliert behandeln.
18. Device Loss abbilden können.
19. Thread-safe Handle Validation ermöglichen.
20. Object Limits unterstützen.
21. Backend Objects kapseln.
22. Object Dependencies verwalten.
23. Resource Sharing kontrollieren.
24. zukünftige Object Types erweiterbar halten.

---

## 235. Empfohlene interne Komponenten

Eine vollständige Runtime SOLL logisch folgende Komponenten besitzen können:

```text
ComputeObjectManager
 |
 +-- HandleManager
 |
 +-- ObjectTable
 |
 +-- ObjectFactory
 |
 +-- LifetimeManager
 |
 +-- ReferenceManager
 |
 +-- OwnershipManager
 |
 +-- AccessManager
 |
 +-- SharingManager
 |
 +-- ObjectValidator
 |
 +-- ObjectDiagnostics
 |
 +-- DeferredDestroyQueue
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei- oder Klassenstruktur vor.

---

## 236. Empfohlenes Objektmodell

Eine mögliche logische Hierarchie lautet:

```text
ComputeRuntime
 |
 +-- Device
 |
 +-- Context
      |
      +-- Queue
      |
      +-- Job
      |
      +-- Program
      |
      +-- Module
      |    |
      |    +-- Kernel
      |
      +-- Resource
      |    |
      |    +-- Buffer
      |    |
      |    +-- Image
      |    |
      |    +-- Tensor
      |
      +-- SyncObject
      |    |
      |    +-- Event
      |    |
      |    +-- Fence
      |    |
      |    +-- Timeline
      |
      +-- Graph
      |
      +-- Pipeline
      |
      +-- MemoryPool
```

---

## 237. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

`NPSPEC-COMPUTE-RUNTIME-0001` definiert die übergeordnete Compute Runtime.

Diese Spezifikation konkretisiert die gemeinsame Object-Semantik.

```text
NPSPEC-COMPUTE-RUNTIME-0001
            |
            v
NPSPEC-COMPUTE-RUNTIME-0004
         Object Model
```

---

## 238. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler verwendet unter anderem:

```text
Context Objects
Queue Objects
Job Objects
Kernel Objects
Resource Objects
Sync Objects
```

Scheduling DARF die Object-Lifetime- und Security-Regeln dieser Spezifikation nicht umgehen.

---

## 239. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Compute Memory Model verwendet insbesondere:

```text
Resource
Buffer
Image
Tensor
MemoryPool
ExternalResource
```

als Runtime Objects.

Die physische Speicherplatzierung kann sich ändern, ohne die logische Objektidentität zu verändern.

---

## 240. Empfohlene Folge-Spezifikationen

Auf dieser Spezifikation SOLLEN weitere Detail-Spezifikationen aufbauen können.

Empfohlen:

```text
NPSPEC-COMPUTE-OBJECT-HANDLE-0001
Compute Handle Model

NPSPEC-COMPUTE-OBJECT-LIFETIME-0001
Object Lifetime

NPSPEC-COMPUTE-OBJECT-DEVICE-0001
Compute Device Object

NPSPEC-COMPUTE-OBJECT-CONTEXT-0001
Compute Context Object

NPSPEC-COMPUTE-OBJECT-QUEUE-0001
Compute Queue Object

NPSPEC-COMPUTE-OBJECT-JOB-0001
Compute Job Object

NPSPEC-COMPUTE-OBJECT-PROGRAM-0001
Compute Program Object

NPSPEC-COMPUTE-OBJECT-MODULE-0001
Compute Module Object

NPSPEC-COMPUTE-OBJECT-KERNEL-0001
Compute Kernel Object

NPSPEC-COMPUTE-OBJECT-RESOURCE-0001
Compute Resource Object

NPSPEC-COMPUTE-OBJECT-BUFFER-0001
Compute Buffer Object

NPSPEC-COMPUTE-OBJECT-IMAGE-0001
Compute Image Object

NPSPEC-COMPUTE-OBJECT-TENSOR-0001
Compute Tensor Object

NPSPEC-COMPUTE-OBJECT-EVENT-0001
Compute Event Object

NPSPEC-COMPUTE-OBJECT-FENCE-0001
Compute Fence Object

NPSPEC-COMPUTE-OBJECT-TIMELINE-0001
Compute Timeline Object

NPSPEC-COMPUTE-OBJECT-GRAPH-0001
Compute Graph Object

NPSPEC-COMPUTE-OBJECT-PIPELINE-0001
Compute Pipeline Object

NPSPEC-COMPUTE-OBJECT-SHARE-0001
Object Sharing

NPSPEC-COMPUTE-OBJECT-SECURITY-0001
Object Security

NPSPEC-COMPUTE-OBJECT-DEBUG-0001
Object Diagnostics

NPSPEC-COMPUTE-OBJECT-TEST-0001
Object Model Conformance Tests
```

---

## 241. Zusammenfassung

Das NovaOS Compute Object Model bildet die gemeinsame semantische Grundlage sämtlicher Compute Runtime Objects.

Der grundsätzliche Zugriff lautet:

```text
Application
    |
    v
Opaque Handle
    |
    v
Handle Validation
    |
    v
Runtime Object
    |
    +-- Identity
    |
    +-- Type
    |
    +-- State
    |
    +-- Owner
    |
    +-- Access Rights
    |
    +-- Lifetime
    |
    +-- Dependencies
    |
    +-- Backend State
    |
    v
Compute Backend
```

Die Anwendung arbeitet nicht mit internen Runtime-Pointern.

Stattdessen verwendet sie kontrollierte Handles.

Dadurch können zentral umgesetzt werden:

```text
Handle Validation
Generation Checking
Type Checking
Ownership Validation
Access Control
Process Isolation
Context Isolation
Deferred Destruction
Device Recovery
```

Ein typischer Lebenszyklus lautet:

```text
CREATE
  |
  v
READY
  |
  v
IN_USE
  |
  v
Public Release
  |
  v
RELEASE_PENDING
  |
  v
Internal References Complete
  |
  v
DESTROYING
  |
  v
DESTROYED
```

Dadurch kann beispielsweise ein Buffer direkt nach dem Einreichen eines asynchronen Jobs öffentlich freigegeben werden, während die Runtime ihn intern so lange erhält, wie der Job noch darauf zugreift.

Die logische Identität eines Compute Objects bleibt dabei unabhängig von Hardwaredetails.

Beispiel:

```text
Buffer Object
     |
     +--> CPU RAM
     |
     +--> GPU0 VRAM
     |
     +--> GPU1 VRAM
```

Die Residency kann wechseln, ohne dass die Anwendung einen neuen logischen Buffer erhalten muss.

Das grundlegende Architekturprinzip lautet:

> Ein Compute Object ist in NovaOS eine kontrollierte Runtime-Entität mit stabiler logischer Identität, typisiertem Handle, definierter Ownership, überprüften Zugriffsrechten und sicherer Lebensdauer. Physische Ressourcen, Backend-Zustände und Hardwareplatzierung dürfen sich verändern, ohne dass Anwendungen interne Implementierungsdetails kennen oder manipulieren müssen.