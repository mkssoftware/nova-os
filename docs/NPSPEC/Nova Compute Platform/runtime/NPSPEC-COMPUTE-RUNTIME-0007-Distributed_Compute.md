# NPSPEC-COMPUTE-RUNTIME-0007 — Distributed Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS Distributed Compute Runtime  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0007`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`  
**Kurzname:** Distributed Compute  

---

## 1. Zweck

Diese Spezifikation definiert **Distributed Compute** innerhalb der NovaOS Compute Runtime.

Distributed Compute erweitert das lokale Compute-Modell auf mehrere physische oder virtuelle Systeme.

Ein Compute Workload DARF dadurch auf Ressourcen ausgeführt werden, die sich nicht im lokalen System befinden.

Mögliche Ausführungsziele sind:

```text
Local CPU
Local GPU
Local NPU

Remote CPU
Remote GPU
Remote NPU

Compute Server
Workstation
Cluster Node
Edge Node
Virtual Machine
Cloud Instance
```

Die Anwendung SOLL dabei möglichst dasselbe Compute-Modell verwenden wie für lokale Compute Devices.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Entfernte Compute-Ressourcen werden als kontrollierte Erweiterung des lokalen Compute-Modells behandelt, ohne Netzwerk-, Sicherheits-, Konsistenz- und Fehlereigenschaften zu verbergen.

Distributed Compute DARF Remote-Ausführung nicht so darstellen, als hätte sie dieselben Eigenschaften wie lokale Ausführung.

Insbesondere MÜSSEN berücksichtigt werden:

```text
Network Latency
Bandwidth
Connection Loss
Node Failure
Partial Failure
Authentication
Authorization
Data Locality
Data Confidentiality
Remote Memory Cost
Result Integrity
```

---

## 3. Ziele

Distributed Compute MUSS folgende Ziele verfolgen:

1. einheitliche lokale und entfernte Compute-Abstraktion,
2. Unterstützung heterogener Remote Devices,
3. automatische oder explizite Node-Auswahl,
4. sichere Node-Authentifizierung,
5. verschlüsselte Kommunikation,
6. Workload-Isolation,
7. Data-Locality-aware Scheduling,
8. Network-aware Scheduling,
9. automatische Fehlererkennung,
10. definierte Retry-Semantik,
11. Node-Ausfallbehandlung,
12. Ressourcenlimits,
13. Remote-Capability-Discovery,
14. skalierbare Cluster-Ausführung,
15. Unterstützung mehrerer Nodes,
16. CPU/GPU/NPU-übergreifende Ausführung,
17. effiziente Datenübertragung,
18. verteilte Synchronisation,
19. Diagnose und Observability,
20. Erweiterbarkeit für zukünftige Compute-Infrastrukturen.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- ein allgemeines verteiltes Dateisystem,
- eine vollständige Cloud-Plattform,
- einen Container-Orchestrator,
- ein allgemeines RPC-System,
- eine globale Datenbank,
- eine vollständige Clusterverwaltung,
- einen konkreten Netzwerktransport,
- eine konkrete Cloud-Provider-API.

Diese Systeme DÜRFEN jedoch durch Distributed Compute verwendet werden.

---

## 5. Grundarchitektur

Die grundlegende Architektur lautet:

```text
Application
    |
    v
Nova Compute API
    |
    v
Compute Runtime
    |
    v
Distributed Compute Runtime
    |
    +-- Node Discovery
    |
    +-- Node Registry
    |
    +-- Capability Manager
    |
    +-- Distributed Scheduler
    |
    +-- Transport Layer
    |
    +-- Remote Memory Manager
    |
    +-- Distributed Synchronization
    |
    +-- Security Manager
    |
    +-- Failure Manager
    |
    +-- Diagnostics
    |
    v
Remote Compute Runtime
    |
    +-- CPU Runtime
    |
    +-- GPU Runtime
    |
    +-- NPU Runtime
    |
    v
Remote Hardware
```

---

## 6. Compute Node

Ein physisches oder virtuelles System, das Compute-Ressourcen bereitstellt, wird als:

```text
Compute Node
```

bezeichnet.

Ein Node kann beispielsweise sein:

```text
Desktop
Laptop
Server
Workstation
Edge Device
Virtual Machine
Cloud Instance
Cluster Node
```

---

## 7. Node Identifier

Jeder Compute Node MUSS einen stabilen logischen Identifier besitzen.

Beispiel:

```text
NodeID
```

Ein Node Name DARF zusätzlich für menschenlesbare Darstellung verwendet werden.

Beispiel:

```text
NodeName = "Nova-Workstation-01"
```

---

## 8. Node Descriptor

Ein Compute Node SOLL mindestens folgende Eigenschaften beschreiben können:

```text
NodeID
NodeName
Architecture
OperatingEnvironment
RuntimeVersion
ProtocolVersion
SecurityIdentity
Capabilities
Devices
MemoryCapacity
NetworkCapabilities
CurrentLoad
HealthState
PowerState
AvailabilityState
```

---

## 9. Node Classes

Die Runtime SOLL mindestens folgende logische Node-Klassen unterstützen können:

```text
LOCAL
LAN
EDGE
DATACENTER
CLOUD
VIRTUAL
```

Diese Klassen sind Hinweise und DÜRFEN nicht allein für Sicherheitsentscheidungen verwendet werden.

---

## 10. Local Node

Das lokale System MUSS als eigener Compute Node modelliert werden können.

Beispiel:

```text
NodeID = LOCAL
```

---

## 11. Remote Node

Ein Remote Node ist jeder Compute Node, dessen Compute Devices nicht direkt durch die lokale Kernel-/Treiberinstanz kontrolliert werden.

---

## 12. Node Device Model

Ein Remote Node kann mehrere Compute Devices bereitstellen.

Beispiel:

```text
Node A
 |
 +-- CPU0
 |
 +-- GPU0
 |
 +-- GPU1
 |
 +-- NPU0
```

---

## 13. Remote Device

Ein Remote Device MUSS in das allgemeine Compute Device Model integriert werden können.

Beispiel:

```text
RemoteDevice
 |
 +-- NodeID
 |
 +-- DeviceID
 |
 +-- DeviceType
 |
 +-- Capabilities
```

---

## 14. Global Device Identity

Ein Remote Device MUSS innerhalb eines Distributed-Compute-Domains eindeutig adressierbar sein.

Konzeptionell:

```text
GlobalDeviceID =
    NodeID
    +
    DeviceID
```

---

## 15. Device Types

Distributed Compute SOLL mindestens folgende Remote Device Types unterstützen:

```text
CPU
GPU
NPU
```

Zukünftige Typen DÜRFEN ergänzt werden.

---

## 16. Node Discovery

Die Runtime MUSS Compute Nodes entdecken oder explizit registrieren können.

Mögliche Mechanismen:

```text
Static Configuration
Trusted Registry
Local Network Discovery
Cluster Controller
Administrative Provisioning
Explicit Address
```

---

## 17. Discovery ist keine Vertrauensentscheidung

Das Auffinden eines Nodes DARF nicht automatisch bedeuten, dass diesem Node vertraut wird.

Discovery und Trust MÜSSEN getrennte Schritte sein.

---

## 18. Node Registration

Ein Node MUSS registriert werden, bevor er reguläre Distributed-Compute-Workloads erhalten kann.

---

## 19. Node Registry

Die Runtime SOLL eine Node Registry verwalten können.

Beispiel:

```text
NodeRegistry
 |
 +-- Node A
 |
 +-- Node B
 |
 +-- Node C
```

---

## 20. Registry Entry

Ein Registry Entry SOLL mindestens enthalten:

```text
NodeID
Endpoint
Identity
TrustState
Capabilities
LastSeen
HealthState
```

---

## 21. Trust State

Ein Node MUSS einen definierten Trust State besitzen können.

Mindestens:

```text
UNKNOWN
UNTRUSTED
TRUSTED
BLOCKED
REVOKED
```

---

## 22. Unknown Node

Ein `UNKNOWN` Node DARF keine geschützten Compute Workloads erhalten.

---

## 23. Untrusted Node

Ein `UNTRUSTED` Node DARF nur Workloads erhalten, deren Policy dies ausdrücklich erlaubt.

---

## 24. Trusted Node

Ein `TRUSTED` Node wurde durch einen gültigen Trust-Prozess autorisiert.

---

## 25. Blocked Node

Ein `BLOCKED` Node DARF keine neuen Workloads erhalten.

---

## 26. Revoked Node

Ein `REVOKED` Node gilt als nicht mehr vertrauenswürdig.

Bestehende Sessions MÜSSEN beendet oder isoliert werden.

---

## 27. Authentication

Remote Nodes MÜSSEN kryptographisch authentifiziert werden.

Eine reine IP-Adresse oder ein Hostname DARF nicht als Identitätsnachweis gelten.

---

## 28. Mutual Authentication

Distributed Compute SOLL gegenseitige Authentifizierung unterstützen.

```text
Client verifies Node
Node verifies Client
```

---

## 29. Secure Channel

Distributed-Compute-Kommunikation MUSS über einen authentifizierten und verschlüsselten Kanal erfolgen, sofern nicht eine explizit definierte lokale Trusted-Transport-Ausnahme existiert.

---

## 30. Transport Security

Der Transport MUSS mindestens schützen gegen:

```text
Eavesdropping
Tampering
Replay
Endpoint Impersonation
```

---

## 31. Protocol Versioning

Das Distributed Compute Protocol MUSS versioniert sein.

Beispiel:

```text
NovaDistributedComputeProtocol 1.0
```

---

## 32. Protocol Negotiation

Beim Verbindungsaufbau MÜSSEN beide Seiten kompatible Protokollversionen aushandeln.

---

## 33. Capability Negotiation

Nach erfolgreicher Authentifizierung MÜSSEN Compute Capabilities ausgehandelt werden können.

---

## 34. Node Capabilities

Ein Node SOLL mindestens deklarieren können:

```text
SupportedDeviceTypes
SupportedDataTypes
SupportedRuntimeFeatures
MaximumConcurrentJobs
MaximumMemoryPerJob
MaximumTransferSize
CompressionSupport
StreamingSupport
CheckpointSupport
RetrySupport
```

---

## 35. Device Capability Discovery

Die lokale Runtime MUSS Capabilities entfernter Devices abfragen können.

Beispiel:

```text
Node A
 |
 +-- GPU0
 |    +-- FP16
 |    +-- FP32
 |    +-- INT8
 |
 +-- NPU0
      +-- FP16
      +-- INT8
      +-- INT4
```

---

## 36. Capability Cache

Remote Capabilities DÜRFEN lokal gecacht werden.

---

## 37. Capability Cache Invalidation

Der Cache MUSS ungültig werden können bei:

```text
Node Reconnect
Runtime Update
Driver Update
Hardware Change
Capability Version Change
```

---

## 38. Node Availability

Ein Node MUSS einen Availability State besitzen können.

Mindestens:

```text
AVAILABLE
BUSY
DRAINING
OFFLINE
UNREACHABLE
MAINTENANCE
```

---

## 39. AVAILABLE

Ein `AVAILABLE` Node kann neue Workloads akzeptieren.

---

## 40. BUSY

Ein `BUSY` Node ist erreichbar, besitzt aber hohe Auslastung.

---

## 41. DRAINING

Ein `DRAINING` Node akzeptiert keine oder nur eingeschränkt neue Workloads.

Bestehende Workloads SOLLEN kontrolliert beendet werden.

---

## 42. OFFLINE

Ein `OFFLINE` Node wurde kontrolliert aus dem Compute Pool entfernt.

---

## 43. UNREACHABLE

Ein `UNREACHABLE` Node konnte innerhalb definierter Netzwerkgrenzen nicht erreicht werden.

---

## 44. MAINTENANCE

Ein Node im `MAINTENANCE` State DARF keine normalen Workloads erhalten.

---

## 45. Node Health

Die Runtime SOLL mindestens folgende Health States unterstützen:

```text
HEALTHY
DEGRADED
UNSTABLE
FAILED
UNKNOWN
```

---

## 46. Health und Availability

Health und Availability MÜSSEN getrennte Konzepte bleiben.

Beispiel:

```text
HealthState       = HEALTHY
AvailabilityState = DRAINING
```

ist zulässig.

---

## 47. Distributed Compute Context

Ein Distributed Compute Context MUSS auf dem allgemeinen Compute Context Model aufbauen.

Er SOLL mindestens enthalten:

```text
OwnerProcess
SecurityDomain
AllowedNodes
AllowedDevices
MemoryBudget
NetworkBudget
Priority
Deadline
DataPolicy
FallbackPolicy
RetryPolicy
```

---

## 48. Allowed Nodes

Ein Context MUSS einschränken können, auf welchen Nodes Workloads ausgeführt werden dürfen.

Beispiel:

```text
AllowedNodes =
[
    NodeA,
    NodeB
]
```

---

## 49. Node Deny List

Ein Context DARF zusätzlich bestimmte Nodes explizit ausschließen.

---

## 50. Geographic Policy

Distributed Compute DARF geografische oder administrative Ausführungsgrenzen unterstützen.

Beispiele:

```text
LOCAL_MACHINE_ONLY
LOCAL_NETWORK_ONLY
ORGANIZATION_ONLY
REGION_RESTRICTED
```

---

## 51. Data Policy

Ein Compute Job MUSS eine Data Policy besitzen können.

Beispiele:

```text
LOCAL_ONLY
TRUSTED_NODES_ONLY
ENCRYPTED_REMOTE_ALLOWED
REMOTE_ALLOWED
```

---

## 52. LOCAL_ONLY

Daten mit:

```text
LOCAL_ONLY
```

DÜRFEN das lokale System nicht verlassen.

---

## 53. TRUSTED_NODES_ONLY

Daten mit:

```text
TRUSTED_NODES_ONLY
```

DÜRFEN nur an Nodes mit ausreichendem Trust State übertragen werden.

---

## 54. Sensitive Data

Die Runtime MUSS ermöglichen, sensible Ressourcen von Remote-Ausführung auszuschließen.

---

## 55. Resource Policy

Compute Resources SOLLEN individuelle Distribution Policies besitzen können.

Beispiel:

```text
Resource A = LOCAL_ONLY
Resource B = TRUSTED_NODES_ONLY
Resource C = REMOTE_ALLOWED
```

---

## 56. Remote Job

Ein auf einem Remote Node ausgeführter Compute Job wird als:

```text
Remote Job
```

bezeichnet.

---

## 57. Remote Job Descriptor

Ein Remote Job SOLL mindestens enthalten:

```text
JobID
ContextID
RequiredCapabilities
PreferredDeviceType
Priority
Deadline
RetryPolicy
Timeout
ResourceReferences
ExecutableReference
SecurityPolicy
```

---

## 58. Global Job ID

Distributed Jobs MÜSSEN systemübergreifend eindeutig identifizierbar sein.

---

## 59. Job State

Ein Remote Job SOLL mindestens folgende Zustände besitzen:

```text
CREATED
QUEUED
TRANSFERRING
READY
RUNNING
COMPLETED
FAILED
CANCELLED
LOST
RETRYING
```

---

## 60. CREATED

Der Job wurde lokal erzeugt, aber noch nicht an einen Node gebunden.

---

## 61. QUEUED

Der Job wurde einem Node zugeordnet und wartet auf Ausführung.

---

## 62. TRANSFERRING

Benötigte Daten oder Executables werden übertragen.

---

## 63. READY

Alle notwendigen Voraussetzungen für die Remote-Ausführung sind erfüllt.

---

## 64. RUNNING

Der Remote Node führt den Workload aus.

---

## 65. COMPLETED

Der Job wurde erfolgreich abgeschlossen.

---

## 66. FAILED

Der Job ist mit einem definierten Fehler beendet worden.

---

## 67. CANCELLED

Der Job wurde kontrolliert abgebrochen.

---

## 68. LOST

Der tatsächliche Zustand des Jobs kann aufgrund eines Node- oder Verbindungsverlusts nicht sicher bestimmt werden.

---

## 69. RETRYING

Die Runtime versucht, einen zulässigen fehlgeschlagenen oder verlorenen Job erneut auszuführen.

---

## 70. Distributed Scheduling

Distributed Compute MUSS in den Compute Scheduler aus `NPSPEC-COMPUTE-RUNTIME-0002` integriert werden.

---

## 71. Scheduling Hierarchy

Ein möglicher Scheduling-Ablauf lautet:

```text
Compute Job
    |
    v
Local Scheduler
    |
    +--> Local Device
    |
    +--> Remote Node
            |
            v
      Remote Scheduler
            |
            +--> CPU
            +--> GPU
            +--> NPU
```

---

## 72. Scheduler Inputs

Bei Remote Scheduling SOLLEN mindestens berücksichtigt werden:

```text
NodeAvailability
NodeHealth
NodeLoad
DeviceCapabilities
DeviceLoad
QueueDepth
DataLocation
TransferSize
NetworkLatency
NetworkBandwidth
NetworkReliability
MemoryAvailability
PowerPolicy
TrustPolicy
Priority
Deadline
EstimatedExecutionTime
```

---

## 73. Remote Execution Cost

Die Runtime SOLL Remote-Ausführung anhand der Gesamtkosten bewerten.

Konzeptionell:

```text
RemoteCost =
    QueueWait
  + DataTransfer
  + Serialization
  + NetworkLatency
  + RemoteExecution
  + Synchronization
  + ResultTransfer
  + ReliabilityPenalty
```

---

## 74. Local-vs-Remote Selection

Ein Workload SOLL nur remote ausgeführt werden, wenn dies mit den definierten Policies vereinbar ist und gegenüber lokaler Ausführung sinnvoll erscheint.

---

## 75. Kleine Workloads

Sehr kleine Workloads SOLLEN typischerweise lokal bleiben, wenn Netzwerk- und Submission-Overhead größer als der Compute-Gewinn wären.

---

## 76. Große Workloads

Große parallelisierbare Workloads DÜRFEN auf Remote Nodes verteilt werden.

---

## 77. Data Locality

Der Scheduler MUSS berücksichtigen, wo benötigte Daten bereits vorhanden sind.

---

## 78. Data-Locality-Beispiel

```text
Dataset
  |
  v
Node B
```

Wenn Node B die Daten bereits besitzt, SOLL Node B bevorzugt werden können, sofern die übrigen Scheduling-Kriterien dies zulassen.

---

## 79. Compute-to-Data

Distributed Compute SOLL bevorzugen können:

```text
Move Compute to Data
```

statt:

```text
Move Large Data to Compute
```

wenn dies effizienter und policy-konform ist.

---

## 80. Network Latency

Die Runtime SOLL aktuelle oder geschätzte Netzwerk-Latenz berücksichtigen.

---

## 81. Network Bandwidth

Die Runtime SOLL verfügbare oder geschätzte Bandbreite berücksichtigen.

---

## 82. Network Reliability

Ein instabiler Link SOLL einen Scheduling Penalty erhalten können.

---

## 83. Network Cost

Die Runtime DARF weitere Netzwerkkosten berücksichtigen.

Beispiele:

```text
Metered Connection
Data Cap
Cloud Egress Cost
Energy Cost
```

---

## 84. Node Affinity

Ein Job DARF einen bevorzugten Node angeben.

Beispiel:

```text
PreferredNode = NodeA
```

---

## 85. Required Node

Ein Job DARF zwingend einen bestimmten Node verlangen.

Beispiel:

```text
RequiredNode = NodeB
```

Ist dieser Node nicht verfügbar, MUSS der Job fehlschlagen oder entsprechend seiner expliziten Policy warten.

---

## 86. Device Affinity

Ein Remote Job DARF zusätzlich ein bevorzugtes Device auf einem Node angeben.

Beispiel:

```text
PreferredNode   = NodeA
PreferredDevice = GPU1
```

---

## 87. Node Groups

Nodes DÜRFEN zu Gruppen zusammengefasst werden.

Beispiele:

```text
WORKSTATIONS
GPU_NODES
NPU_NODES
EDGE_NODES
TRUSTED_CLUSTER
```

---

## 88. Node Group Scheduling

Ein Job DARF auf eine Node Group beschränkt werden.

---

## 89. Resource Reservation

Distributed Compute SOLL Ressourcenreservierungen unterstützen können.

Beispiele:

```text
CPU Cores
GPU Memory
NPU Memory
System Memory
Network Bandwidth
```

---

## 90. Admission Control

Ein Node MUSS einen Job ablehnen können, wenn dessen Ressourcenanforderungen nicht sicher erfüllt werden können.

---

## 91. Remote Memory

Remote Memory DARF nicht wie lokaler RAM behandelt werden.

---

## 92. Remote Resource

Eine auf einem Remote Node gespeicherte Compute Resource SOLL als Remote Resource modelliert werden.

Beispiel:

```text
RemoteBuffer
RemoteTensor
RemoteModel
RemoteDataset
```

---

## 93. Resource Location

Die Runtime MUSS den bekannten Standort einer Resource verfolgen können.

Beispiel:

```text
Resource A -> LOCAL
Resource B -> NodeA
Resource C -> NodeB/GPU0
```

---

## 94. Resource Replication

Immutable oder replizierbare Resources DÜRFEN auf mehreren Nodes vorhanden sein.

Beispiel:

```text
Model Weights
 |
 +--> Node A
 |
 +--> Node B
 |
 +--> Node C
```

---

## 95. Replication State

Die Runtime SOLL Replikationszustände verfolgen können.

Beispiele:

```text
VALID
STALE
TRANSFERRING
INVALID
```

---

## 96. Mutable Resources

Veränderliche Resources MÜSSEN eine definierte Konsistenzsemantik besitzen.

---

## 97. Keine implizite globale Kohärenz

Distributed Compute DARF keine vollständig kohärente globale Shared-Memory-Semantik vortäuschen, sofern diese nicht tatsächlich bereitgestellt wird.

---

## 98. Explicit Data Movement

Die Runtime MUSS explizite oder runtime-gesteuerte Datenbewegung zwischen Nodes unterstützen.

Beispiel:

```text
Node A
  |
  v
Node B
```

---

## 99. Remote Copy

Ein Remote Copy MUSS mindestens Quelle, Ziel und Datenbereich definieren.

---

## 100. Direct Node-to-Node Transfer

Wenn möglich, SOLL ein Transfer direkt zwischen zwei Remote Nodes erfolgen können.

Bevorzugt:

```text
Node A
  |
  v
Node B
```

anstatt:

```text
Node A
  |
  v
Local System
  |
  v
Node B
```

---

## 101. Transfer Authorization

Node-to-Node Transfers MÜSSEN durch die relevante Security Policy autorisiert sein.

---

## 102. Transfer Integrity

Übertragene Daten MÜSSEN gegen unbeabsichtigte oder bösartige Veränderung geschützt werden.

---

## 103. Transfer Verification

Die Runtime SOLL Datenintegrität nach Übertragung verifizieren können.

Mögliche Mechanismen:

```text
Authenticated Transport
Chunk Authentication
Cryptographic Hash
Content Identifier
```

---

## 104. Chunked Transfer

Große Resources SOLLEN in Chunks übertragen werden können.

---

## 105. Transfer Resume

Unterbrochene große Transfers SOLLEN fortgesetzt werden können, sofern Integrität und Identität der bereits übertragenen Daten sicher geprüft werden können.

---

## 106. Compression

Die Runtime DARF Daten vor Netzwerkübertragung komprimieren.

---

## 107. Compression Cost

Kompression SOLL nur verwendet werden, wenn:

```text
CompressionCost
+
TransferCompressed
+
DecompressionCost
```

voraussichtlich günstiger ist als die unkomprimierte Übertragung.

---

## 108. Streaming

Distributed Compute SOLL Streaming von Daten unterstützen können.

Beispiele:

```text
Video Frames
Audio
Sensor Data
Token Streams
Dataset Chunks
```

---

## 109. Backpressure

Streaming-Pipelines MÜSSEN Backpressure unterstützen können.

---

## 110. Flow Control

Ein schneller Produzent DARF einen langsameren Remote Consumer nicht unkontrolliert mit Daten überlasten.

---

## 111. Remote Executable

Ein Remote Job benötigt eine auf dem Zielsystem ausführbare Compute-Repräsentation.

Diese DARF bestehen aus:

```text
Portable IR
Compute Graph
Kernel IR
Native Binary
Model Graph
Signed Package
```

---

## 112. Portable Representation

Portable Compute-Repräsentationen SOLLEN bevorzugt werden, wenn heterogene Nodes unterstützt werden müssen.

---

## 113. Remote Compilation

Ein Remote Node DARF portable Compute-Repräsentationen lokal für seine Hardware kompilieren.

Beispiel:

```text
Portable IR
    |
    v
Remote Compiler
    |
    v
GPU Binary
```

---

## 114. Local Compilation

Alternativ DARF das lokale System zielgerätespezifische Binaries vorbereiten, wenn die Zielarchitektur eindeutig bekannt und kompatibel ist.

---

## 115. Compilation Cache

Remote Nodes SOLLEN kompilierte Compute-Artefakte cachen können.

---

## 116. Compilation Cache Key

Ein Cache Key SOLL mindestens berücksichtigen:

```text
WorkloadHash
TargetArchitecture
DeviceArchitecture
RuntimeVersion
BackendVersion
DriverVersion
CompilerVersion
CompileOptions
```

---

## 117. Executable Integrity

Remote Executables MÜSSEN vor Ausführung auf Integrität geprüft werden.

---

## 118. Executable Authorization

Ein Node MUSS prüfen, ob der anfragende Security Principal zur Ausführung des Workloads berechtigt ist.

---

## 119. Remote Sandbox

Remote Workloads MÜSSEN in einer geeigneten Isolation ausgeführt werden.

Sie DÜRFEN nicht automatisch Zugriff auf:

```text
Host Files
Host Devices
Other Processes
Other Jobs
Kernel Memory
Unrelated Network Resources
```

erhalten.

---

## 120. Least Privilege

Ein Remote Job MUSS nur die Ressourcen und Fähigkeiten erhalten, die für seine Ausführung erforderlich sind.

---

## 121. Capability-based Access

Der Zugriff auf Remote Ressourcen SOLL über explizite Handles oder Capabilities erfolgen.

---

## 122. Remote Handles

Ein lokaler Remote Handle DARF keine direkt verwendbare Remote-Speicheradresse enthalten müssen.

Er SOLL eine sichere logische Referenz repräsentieren.

---

## 123. Handle Validation

Jede Remote Handle Operation MUSS serverseitig validiert werden.

---

## 124. Handle Lifetime

Remote Handles MÜSSEN eine definierte Lebensdauer besitzen.

---

## 125. Lease

Remote Resources DÜRFEN über Leases verwaltet werden.

Beispiel:

```text
LeaseDuration = 60 s
```

---

## 126. Lease Renewal

Aktiv verwendete Resources DÜRFEN ihre Lease verlängern.

---

## 127. Lease Expiration

Nach Ablauf einer Lease DARF der Remote Node nicht mehr benötigte Resources freigeben.

---

## 128. Client Crash Cleanup

Ein abgestürzter Client DARF keine unbegrenzt persistenten Remote Resources hinterlassen.

---

## 129. Session

Eine Verbindung zwischen Compute Runtime und Remote Node SOLL als Session modelliert werden können.

---

## 130. Session State

Mindestens:

```text
CONNECTING
AUTHENTICATING
NEGOTIATING
READY
DEGRADED
DISCONNECTED
FAILED
```

---

## 131. Session Reconnect

Nach temporärem Verbindungsverlust DARF die Runtime versuchen, eine Session wiederherzustellen.

---

## 132. Session Identity

Eine wiederhergestellte Session MUSS sicher derselben autorisierten Identität zugeordnet werden.

---

## 133. Connection Multiplexing

Mehrere Jobs DÜRFEN dieselbe sichere Node-Verbindung verwenden.

---

## 134. Multiple Connections

Für hohe Bandbreite oder Isolation DÜRFEN mehrere Verbindungen zu demselben Node aufgebaut werden.

---

## 135. Distributed Synchronization

Distributed Compute MUSS Synchronisation zwischen lokalen und entfernten Jobs ermöglichen.

---

## 136. Remote Event

Ein Remote Event repräsentiert ein Ereignis auf einem entfernten Node.

---

## 137. Remote Fence

Ein Remote Fence DARF einen monotonen Wert besitzen.

Beispiel:

```text
FenceValue = 42
```

---

## 138. Distributed Dependency

Ein Job auf Node B DARF von einem Job auf Node A abhängen.

Beispiel:

```text
Job A @ Node A
      |
      v
Job B @ Node B
```

---

## 139. Dependency Transfer

Wenn Job B Daten von Job A benötigt, MUSS die Datenabhängigkeit mit der Ausführungsabhängigkeit gekoppelt werden.

---

## 140. No Global Clock Assumption

Distributed Compute DARF keine perfekt synchronisierte globale Uhr voraussetzen.

---

## 141. Timestamp Domains

Zeitstempel verschiedener Nodes MÜSSEN als unterschiedliche Clock Domains behandelt werden, sofern keine definierte Synchronisationsgarantie besteht.

---

## 142. Timeline Reconstruction

Tracing DARF synchronisierte oder korrelierte Zeitquellen verwenden, um verteilte Abläufe näherungsweise zu rekonstruieren.

---

## 143. Timeout

Remote Operationen MÜSSEN definierte Timeouts unterstützen.

---

## 144. Timeout ist kein Ausführungsbeweis

Ein lokaler Timeout bedeutet nicht automatisch, dass der Remote Job nicht ausgeführt wurde.

---

## 145. Ambiguous Completion

Bei Verbindungsverlust kann folgender Zustand entstehen:

```text
Request sent
    |
    v
Connection lost
    |
    ?
    +-- Job never started
    |
    +-- Job running
    |
    +-- Job completed
```

Die Runtime MUSS diesen Zustand explizit behandeln.

---

## 146. Exactly-Once

Eine allgemeine Exactly-Once-Ausführung DARF nicht ohne entsprechende Protokoll- und Workload-Garantien behauptet werden.

---

## 147. At-Most-Once

Für nicht wiederholbare Workloads SOLL eine At-Most-Once-Policy unterstützt werden können.

---

## 148. At-Least-Once

Retry-safe und idempotente Workloads DÜRFEN mit At-Least-Once-Semantik erneut ausgeführt werden.

---

## 149. Idempotent Job

Ein Job DARF als:

```text
IDEMPOTENT
```

markiert werden.

---

## 150. Retry-Safe Job

Ein Job DARF als:

```text
RETRY_SAFE
```

markiert werden.

---

## 151. Side-Effect-Free Job

Ein reiner Compute Job ohne externe Seiteneffekte SOLL als:

```text
SIDE_EFFECT_FREE
```

markiert werden können.

---

## 152. Retry Policy

Mindestens folgende Policies SOLLEN unterstützt werden:

```text
NO_RETRY
RETRY_SAME_NODE
RETRY_OTHER_NODE
RETRY_ANY_COMPATIBLE_DEVICE
```

---

## 153. Retry Limit

Eine Retry Policy MUSS ein Limit besitzen können.

Beispiel:

```text
MaximumRetries = 3
```

---

## 154. Retry Backoff

Wiederholte Verbindungsversuche SOLLEN Backoff verwenden können.

---

## 155. Node Failure

Ein Node Failure MUSS von einem normalen Job Failure unterschieden werden.

---

## 156. Failure Classes

Mindestens folgende Fehlerklassen SOLLEN unterschieden werden:

```text
NETWORK_FAILURE
AUTHENTICATION_FAILURE
AUTHORIZATION_FAILURE
NODE_FAILURE
DEVICE_FAILURE
OUT_OF_MEMORY
TIMEOUT
EXECUTION_FAILURE
PROTOCOL_FAILURE
DATA_INTEGRITY_FAILURE
POLICY_VIOLATION
```

---

## 157. Failure Propagation

Fehler MÜSSEN mit ausreichend Kontext an den lokalen Compute Scheduler zurückgegeben werden.

---

## 158. Node Loss

Wird ein Node während eines laufenden Jobs unerreichbar, MUSS der Job zunächst als potenziell:

```text
LOST
```

behandelt werden, bis sein tatsächlicher Zustand bestimmt werden kann.

---

## 159. Recovery Query

Nach Wiederverbindung SOLL die Runtime den Status bekannter Job IDs abfragen können.

---

## 160. Completed Job Recovery

Wenn ein Job während eines Verbindungsverlusts erfolgreich abgeschlossen wurde, SOLL das Resultat nach Wiederverbindung abrufbar sein können.

---

## 161. Result Retention

Remote Nodes SOLLEN Jobresultate für eine begrenzte Zeit aufbewahren können.

---

## 162. Result Lease

Resultate DÜRFEN über eine definierte Retention oder Lease verwaltet werden.

---

## 163. Checkpointing

Lange verteilte Workloads SOLLEN Checkpointing unterstützen können.

---

## 164. Checkpoint

Ein Checkpoint SOLL genügend Zustand enthalten, um einen geeigneten Workload fortsetzen zu können.

---

## 165. Checkpoint Location

Checkpoints DÜRFEN gespeichert werden auf:

```text
Local Node
Remote Node
Shared Storage
Trusted Distributed Storage
```

---

## 166. Checkpoint Security

Checkpoints MÜSSEN denselben Datenschutz- und Sicherheitsregeln wie die ursprünglichen Workload-Daten unterliegen.

---

## 167. Migration

Checkpoint-fähige Workloads DÜRFEN zwischen Nodes migriert werden.

Beispiel:

```text
Node A
  |
  v
Checkpoint
  |
  v
Node B
  |
  v
Resume
```

---

## 168. Live Migration

Live Migration ist optional und DARF nur angeboten werden, wenn die benötigten Runtime- und Device-Garantien vorhanden sind.

---

## 169. Distributed Workload Splitting

Teilbare Workloads DÜRFEN auf mehrere Nodes verteilt werden.

Beispiel:

```text
Workload
 |
 +--> Node A
 |
 +--> Node B
 |
 +--> Node C
```

---

## 170. Split Safety

Ein Workload DARF nur automatisch geteilt werden, wenn die Runtime seine Teilbarkeit sicher bestimmen kann oder der Workload diese explizit deklariert.

---

## 171. Partitioning

Ein Compute Graph DARF zwischen Nodes partitioniert werden.

Beispiel:

```text
Input
  |
  v
Node A / GPU
  |
  v
Node B / NPU
  |
  v
Node C / CPU
  |
  v
Output
```

---

## 172. Partition Cost

Graph Partitioning MUSS mindestens berücksichtigen:

```text
ExecutionCost
TransferCost
SerializationCost
SynchronizationCost
NetworkLatency
MemoryCost
FailureRisk
```

---

## 173. Fine-Grained Partitioning

Sehr feingranulare Remote-Partitionierung SOLL vermieden werden, wenn Netzwerk-Overhead die Compute-Vorteile übersteigt.

---

## 174. Coarse-Grained Remote Execution

Distributed Compute SOLL bevorzugt ausreichend grobe Workload-Einheiten über Node-Grenzen verteilen.

---

## 175. Map-Style Workloads

Unabhängige Datenpartitionen SOLLEN effizient auf mehrere Nodes verteilt werden können.

Beispiel:

```text
Dataset
 |
 +--> Partition A -> Node A
 |
 +--> Partition B -> Node B
 |
 +--> Partition C -> Node C
```

---

## 176. Reduction

Teilresultate DÜRFEN anschließend zusammengeführt werden.

Beispiel:

```text
Node A Result --+
                |
Node B Result --+--> Reduce --> Final Result
                |
Node C Result --+
```

---

## 177. Tree Reduction

Bei vielen Nodes SOLL eine hierarchische Reduction möglich sein.

---

## 178. Collective Operations

Eine vollständige Distributed Compute Runtime SOLL Collective Operations unterstützen können.

Beispiele:

```text
BROADCAST
REDUCE
ALL_REDUCE
GATHER
ALL_GATHER
SCATTER
```

---

## 179. Collective Capability

Collectives DÜRFEN durch optimierte Netzwerk- oder Accelerator-Backends implementiert werden.

---

## 180. Collective Fallback

Falls kein optimiertes Backend vorhanden ist, DARF die Runtime Collectives über allgemeine Transfers implementieren.

---

## 181. Barrier

Eine Distributed Barrier DARF verwendet werden, um mehrere Teilnehmer zu synchronisieren.

---

## 182. Barrier Failure

Der Ausfall eines Teilnehmers MUSS als definierter Barrier-Fehler behandelt werden.

---

## 183. Elastic Execution

Eine vollständige Runtime SOLL Workloads unterstützen können, die mit einer variablen Anzahl von Nodes arbeiten können.

---

## 184. Node Join

Elastic Workloads DÜRFEN zusätzliche Nodes während der Ausführung aufnehmen, wenn die Workload-Semantik dies erlaubt.

---

## 185. Node Leave

Elastic Workloads DÜRFEN den Verlust eines Nodes überleben, wenn ihre Ausführungssemantik dies erlaubt.

---

## 186. Distributed AI

Distributed Compute SOLL AI-Workloads über mehrere Nodes unterstützen können.

Beispiele:

```text
Distributed Inference
Batch Inference
Model Parallelism
Tensor Parallelism
Pipeline Parallelism
```

---

## 187. Distributed Inference

Unabhängige Inferenzanforderungen DÜRFEN auf mehrere Nodes verteilt werden.

---

## 188. Model Replication

Ein Modell DARF auf mehreren Nodes repliziert werden.

Beispiel:

```text
Model
 |
 +--> Node A / NPU
 |
 +--> Node B / NPU
 |
 +--> Node C / GPU
```

---

## 189. Model Parallelism

Große Modelle DÜRFEN über mehrere Nodes partitioniert werden.

---

## 190. Tensor Parallelism

Tensoroperationen DÜRFEN über mehrere Remote Devices verteilt werden.

---

## 191. Pipeline Parallelism

Ein Modell DARF in Pipeline-Stufen aufgeteilt werden.

Beispiel:

```text
Stage 1 -> Node A
Stage 2 -> Node B
Stage 3 -> Node C
```

---

## 192. Distributed Training

Die Architektur SOLL zukünftiges Distributed Training ermöglichen.

Eine vollständige Trainingsspezifikation ist jedoch nicht Bestandteil dieses Dokuments.

---

## 193. Parameter Synchronization

Zukünftige Training-Runtimes DÜRFEN Distributed Collectives zur Parametersynchronisation verwenden.

---

## 194. Distributed GPU Compute

Remote GPUs MÜSSEN über die GPU Runtime abstrahiert werden können.

Konzeptionell:

```text
Distributed Runtime
      |
      v
Remote Node
      |
      v
GPU Runtime
      |
      v
GPU
```

---

## 195. Distributed NPU Compute

Remote NPUs MÜSSEN über die NPU Runtime abstrahiert werden können.

---

## 196. Heterogeneous Cluster

Ein Cluster DARF heterogene Nodes enthalten.

Beispiel:

```text
Node A
CPU + GPU

Node B
CPU + NPU

Node C
CPU

Node D
CPU + GPU + NPU
```

---

## 197. Heterogeneous Scheduling

Der Scheduler MUSS Workloads anhand tatsächlicher Capabilities und nicht nur anhand von Device-Namen verteilen.

---

## 198. Architecture Compatibility

Native Compute Binaries DÜRFEN nur auf kompatiblen Zielarchitekturen ausgeführt werden.

---

## 199. Portable IR

Bei heterogenen Nodes SOLL eine portable Intermediate Representation bevorzugt werden können.

---

## 200. Remote Compilation Security

Remote Compilation MUSS in einer geeigneten Sicherheitsdomäne erfolgen.

Compilerfehler DÜRFEN den Node nicht kompromittieren.

---

## 201. Resource Quotas

Remote Nodes MÜSSEN Ressourcenquoten durchsetzen können.

Beispiele:

```text
MaximumCPUTime
MaximumMemory
MaximumGPUMemory
MaximumNPUMemory
MaximumConcurrentJobs
MaximumNetworkTraffic
```

---

## 202. Per-Principal Quotas

Quotas SOLLEN pro Security Principal oder Compute Context definierbar sein.

---

## 203. Fairness

Ein einzelner Client DARF einen gemeinsam genutzten Compute Node nicht unbegrenzt monopolisieren.

---

## 204. Priority

Distributed Jobs MÜSSEN Prioritäten unterstützen können.

---

## 205. Priority Boundaries

Ein Remote Node DARF lokale oder administrative Priority Policies auf Client-Prioritäten anwenden.

Ein Client DARF nicht eigenständig administrative Höchstpriorität erzwingen.

---

## 206. Preemption

Remote Nodes DÜRFEN Jobs preempten, sofern Runtime und Workload dies unterstützen.

---

## 207. Preemption Notification

Wenn möglich, SOLL ein preemptierter Workload über seinen Zustand informiert werden.

---

## 208. Cancellation

Ein Client MUSS versuchen können, einen Remote Job abzubrechen.

---

## 209. Cancellation Semantics

Eine erfolgreiche Cancel-Anforderung MUSS klar von einer bloß gesendeten Cancel-Anforderung unterschieden werden.

---

## 210. Cancellation Result

Mögliche Ergebnisse:

```text
CANCELLED
ALREADY_COMPLETED
NOT_FOUND
CANNOT_CANCEL
CONNECTION_LOST
```

---

## 211. Deadline

Ein Distributed Job DARF eine Deadline besitzen.

---

## 212. Deadline-aware Scheduling

Der Scheduler SOLL Netzwerk- und Queue-Latenz in die Deadline-Bewertung einbeziehen.

---

## 213. Deadline Miss

Kann eine Deadline voraussichtlich nicht eingehalten werden, DARF der Scheduler:

- einen schnelleren Node wählen,
- lokal ausführen,
- einen bereits warmen Device Context wählen,
- den Job entsprechend seiner Policy ablehnen.

---

## 214. Power Awareness

Distributed Scheduling DARF Energieeffizienz berücksichtigen.

---

## 215. Carbon Awareness

Eine zukünftige Runtime DARF optional externe Energie- oder Emissionsinformationen berücksichtigen.

Dies DARF niemals Sicherheits-, Datenschutz- oder Deadline-Regeln außer Kraft setzen.

---

## 216. Thermal Awareness

Lokale thermische Belastung DARF ein Grund sein, einen geeigneten Workload remote auszuführen.

---

## 217. Cost Awareness

Bei kostenpflichtigen Compute Nodes DARF ein monetäres Budget berücksichtigt werden.

---

## 218. Compute Budget

Ein Context DARF ein Compute Budget besitzen.

Beispiele:

```text
MaximumRemoteCost
MaximumRuntime
MaximumTransferredBytes
```

---

## 219. Budget Enforcement

Ein Job DARF nicht automatisch ein explizites Budget überschreiten.

---

## 220. Metered Resources

Die Runtime SOLL kosten- oder mengenbegrenzte Ressourcen als solche kennzeichnen können.

---

## 221. Observability

Distributed Compute MUSS ausreichende Diagnoseinformationen bereitstellen.

---

## 222. Trace Context

Ein Distributed Job SOLL einen Trace Context über Node-Grenzen transportieren können.

---

## 223. Trace ID

Ein vollständiger verteilter Ausführungspfad SOLL über eine gemeinsame Trace ID korrelierbar sein.

---

## 224. Trace Events

Mindestens folgende Events SOLLEN verfügbar sein:

```text
NODE_DISCOVERED
NODE_CONNECTED
NODE_AUTHENTICATED
NODE_DISCONNECTED

REMOTE_JOB_CREATED
REMOTE_JOB_ASSIGNED
REMOTE_JOB_TRANSFER_START
REMOTE_JOB_TRANSFER_END
REMOTE_JOB_START
REMOTE_JOB_END
REMOTE_JOB_FAILED
REMOTE_JOB_RETRY

REMOTE_RESOURCE_CREATE
REMOTE_RESOURCE_TRANSFER
REMOTE_RESOURCE_RELEASE

NODE_HEALTH_CHANGE
NODE_LOST
```

---

## 225. Distributed Metrics

Mindestens folgende Metriken SOLLEN verfügbar sein:

```text
ConnectedNodes
AvailableNodes
RunningRemoteJobs
QueuedRemoteJobs
FailedRemoteJobs
TransferredBytes
AverageNetworkLatency
AverageRemoteExecutionTime
RetryCount
NodeFailureCount
```

---

## 226. Per-Node Metrics

Pro Node SOLLEN mindestens verfügbar sein:

```text
HealthState
AvailabilityState
CurrentLoad
QueueDepth
MemoryAvailable
NetworkLatency
NetworkBandwidth
RunningJobs
```

---

## 227. Per-Job Metrics

Pro Remote Job SOLLEN mindestens erfassbar sein:

```text
QueueWaitTime
TransferTime
RemoteExecutionTime
ResultTransferTime
RetryCount
SelectedNode
SelectedDevice
```

---

## 228. Privacy of Diagnostics

Diagnosedaten DÜRFEN keine unnötigen:

- Tensorinhalte,
- Dateiinhalte,
- Modellinhalte,
- Nutzerdaten,
- Secrets

enthalten.

---

## 229. Audit

Sicherheitsrelevante Distributed-Compute-Aktionen SOLLEN auditierbar sein.

Beispiele:

```text
Node Trust Change
Authentication Failure
Policy Violation
Unauthorized Resource Access
Executable Rejection
```

---

## 230. Credentials

Authentifizierungs-Credentials DÜRFEN nicht als normale Compute Resources behandelt oder unverschlüsselt übertragen werden.

---

## 231. Secret Handling

Secrets SOLLEN über dedizierte sichere Mechanismen bereitgestellt werden.

---

## 232. Secret Scope

Ein Secret MUSS auf den minimal notwendigen:

```text
Job
Context
Node
Lifetime
```

beschränkt werden können.

---

## 233. Secret Cleanup

Temporäre Secrets MÜSSEN nach Ende ihrer Verwendung aus Remote Runtime State entfernt werden.

---

## 234. Untrusted Workloads

Ein Node MUSS davon ausgehen, dass eingereichte Remote Workloads potenziell fehlerhaft oder bösartig sind.

---

## 235. Malformed Input

Ungültige:

```text
Descriptors
Graphs
Kernels
IR
Metadata
Resource References
Protocol Messages
```

MÜSSEN sicher abgelehnt werden.

---

## 236. Denial-of-Service Protection

Remote Nodes SOLLEN Schutz gegen Ressourcenerschöpfung besitzen.

Beispiele:

```text
Rate Limits
Memory Limits
Job Limits
Compilation Limits
Transfer Limits
Timeouts
```

---

## 237. Replay Protection

Sicherheitskritische Remote Operationen MÜSSEN gegen unzulässiges Replay geschützt werden.

---

## 238. Stale Request

Veraltete oder bereits abgeschlossene Job-Kommandos SOLLEN sicher erkannt werden können.

---

## 239. Job Ownership

Jeder Remote Job MUSS eindeutig einem autorisierten Principal und Context zugeordnet sein.

---

## 240. Resource Ownership

Jede Remote Resource MUSS einen definierten Owner oder explizite Sharing Policy besitzen.

---

## 241. Resource Sharing

Remote Resources DÜRFEN zwischen Contexts geteilt werden, wenn dies explizit autorisiert wurde.

---

## 242. Immutable Sharing

Immutable Resources SOLLEN effizient zwischen mehreren Jobs geteilt werden können.

Beispiele:

```text
Model Weights
Lookup Tables
Read-only Dataset Chunks
Compiled Kernels
```

---

## 243. Cache

Remote Nodes DÜRFEN wiederverwendbare Compute-Artefakte cachen.

---

## 244. Cache Classes

Beispiele:

```text
Executable Cache
Kernel Cache
Model Cache
Weight Cache
Dataset Cache
Compilation Cache
```

---

## 245. Cache Identity

Cache-Inhalte MÜSSEN eindeutig anhand ihres tatsächlichen Inhalts oder einer sicheren Versionierungsstrategie identifiziert werden.

---

## 246. Cache Poisoning Protection

Nicht vertrauenswürdige Clients DÜRFEN keine Cache-Einträge anderer Security Domains manipulieren.

---

## 247. Cache Eviction

Caches MÜSSEN unter Memory Pressure oder Storage Pressure bereinigt werden können.

---

## 248. Warm Node

Ein Node mit bereits geladenen Daten, Modellen oder kompilierten Artefakten DARF einen Scheduling-Vorteil erhalten.

---

## 249. Cold Node

Ein ansonsten schneller Node DARF durch hohe Initialisierungskosten für einen einzelnen Workload weniger geeignet sein.

---

## 250. Warmth Cost Model

Der Scheduler SOLL berücksichtigen:

```text
ModelAlreadyLoaded
DataAlreadyPresent
KernelAlreadyCompiled
DeviceAlreadyAwake
ContextAlreadyWarm
```

---

## 251. Node Drain

Ein Administrator oder Systemdienst MUSS einen Node kontrolliert drainen können.

---

## 252. Drain Semantik

Beim Drain:

1. werden neue normale Jobs nicht mehr angenommen,
2. laufende Jobs werden beendet oder migriert,
3. Ressourcen werden freigegeben,
4. der Node wechselt zu `OFFLINE` oder `MAINTENANCE`.

---

## 253. Graceful Shutdown

Ein Node SOLL vor geplantem Shutdown seine Compute Peers informieren können.

---

## 254. Unexpected Shutdown

Bei unerwartetem Shutdown MUSS die Failure-Recovery-Logik greifen.

---

## 255. Runtime Update

Ein Node Runtime Update DARF bestehende Sessions ungültig machen.

Die Protokollversion MUSS nach Reconnect erneut ausgehandelt werden.

---

## 256. Rolling Update

Cluster SOLLEN schrittweise aktualisiert werden können, sofern kompatible Protokollversionen verfügbar sind.

---

## 257. Mixed Runtime Versions

Mehrere Runtime-Versionen DÜRFEN gleichzeitig im Cluster existieren, solange eine kompatible Protokollmenge ausgehandelt werden kann.

---

## 258. Feature Downgrade

Ist ein neueres Feature auf einem älteren Node nicht verfügbar, MUSS die Runtime:

- das Feature deaktivieren,
- einen anderen Node wählen,
- oder den Job definiert ablehnen.

---

## 259. Split-Brain-Vermeidung

Falls ein zentraler Scheduler oder Registry-Dienst verwendet wird, MUSS dessen Design widersprüchliche Node- oder Job-Zustände kontrolliert behandeln.

---

## 260. Authority

Für jede verteilte Ressource MUSS klar definiert sein, welche Instanz autoritativ über ihren Zustand entscheidet.

---

## 261. Job Authority

Der Remote Node ist während tatsächlicher Ausführung autoritativ für den lokalen Execution State.

Der Distributed Scheduler ist autoritativ für die globale Scheduling-Entscheidung.

---

## 262. Result Authority

Ein Resultat gilt erst als erfolgreich übernommen, wenn die dafür definierte Integritäts- und Completion-Semantik erfüllt ist.

---

## 263. Distributed Runtime Backend

Die Distributed Compute Runtime MUSS eine abstrahierte Transport-/Remote-Backend-Schnittstelle besitzen.

Konzeptionell:

```text
discover_nodes()

connect_node()
disconnect_node()

authenticate_node()
negotiate_protocol()

query_node()
query_devices()

create_remote_context()
destroy_remote_context()

create_remote_resource()
transfer_resource()
release_remote_resource()

submit_job()
query_job()
cancel_job()

wait_event()
signal_event()

query_metrics()

shutdown()
```

---

## 264. Backend Independence

Die öffentliche Compute API DARF nicht von einem bestimmten Netzwerk- oder Cluster-Backend abhängen.

---

## 265. Transport Independence

Die Architektur SOLL unterschiedliche Transportmechanismen ermöglichen.

Beispiele:

```text
Local IPC
TCP-like Transport
QUIC-like Transport
RDMA-like Transport
Shared Memory Transport
High-Speed Fabric
```

---

## 266. Transport Capability

Ein Transport MUSS seine Fähigkeiten deklarieren können.

Beispiele:

```text
ReliableDelivery
OrderedDelivery
ZeroCopy
RDMA
Multiplexing
Streaming
Encryption
```

---

## 267. High-Speed Interconnect

Bei schnellen Cluster-Verbindungen SOLL die Runtime optimierte Transfermechanismen nutzen können.

---

## 268. RDMA-like Transfer

Direkter Memory Transfer DARF verwendet werden, wenn:

- Hardware dies unterstützt,
- Memory registriert wurde,
- Security Policy dies erlaubt,
- Isolation gewährleistet ist.

---

## 269. Zero-Copy Network Path

Eine vollständige Runtime SOLL unnötige Memory-Kopien im Netzwerkpfad vermeiden können.

---

## 270. Serialization

Compute Metadata MUSS in einem versionierten, robust validierbaren Format übertragen werden.

---

## 271. Endianness

Das Distributed Compute Protocol MUSS plattformübergreifende Endianness eindeutig definieren.

---

## 272. Integer Width

Übertragene Integer-Typen MÜSSEN definierte Bitbreiten besitzen.

---

## 273. Floating-Point Representation

Übertragene Floating-Point-Daten MÜSSEN ein definiertes Format besitzen oder durch Metadaten eindeutig beschrieben werden.

---

## 274. Tensor Metadata

Remote Tensor Transfers MÜSSEN mindestens beschreiben können:

```text
ElementType
Shape
Strides
Layout
ByteSize
Quantization
```

---

## 275. Graph Metadata

Remote Graphen MÜSSEN versioniert und validierbar sein.

---

## 276. Partial Compatibility

Ein Node DARF einen Graph teilweise unterstützen.

Die Runtime DARF den Graph daraufhin partitionieren.

---

## 277. Remote Fallback

Wenn ein Remote Device einen Workload nicht ausführen kann, DARF abhängig von der Policy:

```text
Remote GPU -> Remote CPU
Remote NPU -> Remote GPU
Remote Node -> Other Node
Remote -> Local
```

verwendet werden.

---

## 278. Fallback Policy

Mindestens folgende Policies SOLLEN möglich sein:

```text
NO_FALLBACK
LOCAL_FALLBACK
REMOTE_SAME_NODE_FALLBACK
REMOTE_OTHER_NODE_FALLBACK
ANY_COMPATIBLE_FALLBACK
```

---

## 279. Fallback Security

Ein Fallback DARF niemals auf einen Node erfolgen, der die Data- oder Trust-Policy des Jobs verletzt.

---

## 280. Fallback Deadline

Ein Fallback SOLL nicht durchgeführt werden, wenn dadurch eine harte Deadline sicher verletzt würde und die Job Policy dies verbietet.

---

## 281. Offline Mode

Distributed Compute MUSS erkennen, wenn keine Remote-Infrastruktur verfügbar ist.

---

## 282. Offline Fallback

Jobs mit erlaubtem Local Fallback SOLLEN dann lokal ausgeführt werden können.

---

## 283. Remote Required

Ein Job DARF Remote-Ausführung zwingend verlangen.

Beispiel:

```text
REMOTE_REQUIRED
```

Ist kein geeigneter Remote Node verfügbar, MUSS der Job definiert fehlschlagen oder warten.

---

## 284. Local Preferred

Ein Job DARF lokale Ausführung bevorzugen.

```text
LOCAL_PREFERRED
```

---

## 285. Remote Preferred

Ein Job DARF Remote-Ausführung bevorzugen.

```text
REMOTE_PREFERRED
```

---

## 286. Automatic Placement

Standardmäßig SOLL der Compute Scheduler die Platzierung anhand des Cost Models bestimmen können.

---

## 287. User Policy

NovaOS DARF benutzerweite Distributed-Compute-Regeln bereitstellen.

Beispiele:

```text
Never use remote compute

Use trusted LAN nodes

Use organization nodes

Allow metered cloud compute

Allow remote compute only on AC power
```

---

## 288. Administrative Policy

Systemadministratoren DÜRFEN strengere Regeln erzwingen.

Administrative Policies MÜSSEN Vorrang vor weniger restriktiven Anwendungswünschen haben.

---

## 289. Application Policy

Anwendungen DÜRFEN innerhalb der erlaubten Systemgrenzen Anforderungen und Präferenzen definieren.

---

## 290. Policy Resolution

Eine effektive Policy SOLL aus mehreren Ebenen entstehen:

```text
System Policy
    |
    v
Administrative Policy
    |
    v
User Policy
    |
    v
Application Policy
    |
    v
Job Policy
```

Die restriktivere gültige Regel MUSS Vorrang erhalten.

---

## 291. Testing — Node Discovery

Mindestens zu testen:

```text
No Nodes
Single Node
Multiple Nodes
Duplicate Discovery
Node Reappearance
Invalid Node Advertisement
```

---

## 292. Testing — Authentication

Zu testen sind:

```text
Valid Node
Unknown Node
Invalid Identity
Expired Credential
Revoked Node
Replay Attempt
```

---

## 293. Testing — Protocol Negotiation

Mindestens:

```text
Same Version
Compatible Versions
Unsupported Version
Missing Feature
Feature Downgrade
```

---

## 294. Testing — Capability Discovery

Zu testen sind Nodes mit:

```text
CPU only
GPU only
CPU + GPU
CPU + NPU
CPU + GPU + NPU
Heterogeneous Architectures
```

---

## 295. Testing — Scheduling

Zu testen sind:

```text
Local Faster
Remote Faster
High Latency
Low Bandwidth
Busy Node
Warm Node
Cold Node
Data Already Remote
Deadline Job
```

---

## 296. Testing — Data Policy

Mindestens:

```text
LOCAL_ONLY
TRUSTED_NODES_ONLY
REMOTE_ALLOWED
Blocked Node
Revoked Node
```

---

## 297. Testing — Transfers

Zu testen sind:

```text
Small Transfer
Large Transfer
Chunked Transfer
Interrupted Transfer
Resumed Transfer
Compressed Transfer
Node-to-Node Transfer
Integrity Failure
```

---

## 298. Testing — Remote Resources

Mindestens:

```text
Create
Use
Share
Release
Lease Expiration
Client Crash
Node Crash
```

---

## 299. Testing — Job Lifecycle

Zu testen sind:

```text
Create
Queue
Transfer
Run
Complete
Cancel
Fail
Lose
Retry
Recover
```

---

## 300. Testing — Network Failure

Mindestens:

```text
Disconnect before Submit
Disconnect during Transfer
Disconnect after Submit
Disconnect during Execution
Disconnect during Result Transfer
Reconnect
```

---

## 301. Testing — Ambiguous Completion

Explizit zu testen:

```text
Submit
 |
 v
Remote Accept
 |
 v
Connection Lost
 |
 v
Unknown Completion State
```

Die Runtime DARF nicht unkontrolliert einen nicht idempotenten Job erneut ausführen.

---

## 302. Testing — Retry

Zu testen sind:

```text
NO_RETRY
RETRY_SAME_NODE
RETRY_OTHER_NODE
RETRY_ANY_COMPATIBLE_DEVICE
Retry Limit
Backoff
```

---

## 303. Testing — Node Failure

Mindestens:

```text
Node Crash
Node Reboot
Device Failure
Runtime Crash
Maintenance
Drain
Unexpected Shutdown
```

---

## 304. Testing — Checkpoint

Zu testen sind:

```text
Checkpoint Create
Checkpoint Restore
Node Migration
Corrupted Checkpoint
Unauthorized Checkpoint Access
```

---

## 305. Testing — Distributed Synchronization

Mindestens:

```text
Remote Event
Remote Fence
Cross-Node Dependency
Barrier
Barrier Participant Failure
```

---

## 306. Testing — Collectives

Falls unterstützt:

```text
Broadcast
Reduce
All-Reduce
Gather
All-Gather
Scatter
Participant Failure
```

---

## 307. Testing — Heterogeneous Compute

Mindestens:

```text
Local CPU + Remote GPU
Local GPU + Remote NPU
Remote CPU + Remote GPU
Multiple Remote GPUs
Multiple Remote NPUs
```

---

## 308. Testing — Security

Zu testen sind:

```text
Unauthorized Job
Unauthorized Resource
Cross-Context Access
Malformed Protocol Message
Malformed Graph
Malformed Kernel
Credential Theft Attempt
Replay
Cache Poisoning
```

---

## 309. Testing — Resource Exhaustion

Mindestens:

```text
Too Many Jobs
Too Much Memory
Too Many Connections
Too Many Resources
Too Much Network Traffic
Compilation Flood
```

---

## 310. Testing — Quotas

Zu testen sind:

```text
CPU Quota
Memory Quota
GPU Memory Quota
NPU Memory Quota
Network Quota
Concurrent Job Limit
```

---

## 311. Testing — Multi-Tenant

Mehrere Clients MÜSSEN gleichzeitig auf demselben Node getestet werden.

Zu prüfen sind:

```text
Isolation
Fairness
Quota Enforcement
Priority
Cleanup
```

---

## 312. Testing — Performance

Mindestens folgende Werte SOLLEN gemessen werden:

```text
ConnectionSetupLatency
AuthenticationLatency
SubmissionLatency
TransferLatency
TransferBandwidth
RemoteExecutionLatency
ResultLatency
RetryRecoveryTime
```

---

## 313. Testing — Scalability

Die Runtime SOLL getestet werden mit:

```text
1 Node
10 Nodes
100 Nodes
1000+ Nodes
```

soweit die Zielimplementierung diese Größenordnung vorsieht.

---

## 314. Stress Tests

Stress Tests SOLLEN umfassen:

```text
Thousands of Remote Jobs
Continuous Node Join/Leave
Continuous Transfers
Multiple Clients
Multiple Device Types
Repeated Disconnect/Reconnect
Node Failure under Load
```

---

## 315. Fuzzing

Mindestens folgende Inputs SOLLTEN fuzz-getestet werden:

```text
Protocol Messages
Node Descriptors
Device Descriptors
Job Descriptors
Resource Descriptors
Tensor Metadata
Graph Metadata
Remote Handles
```

---

## 316. Fault Injection

Mindestens folgende Fehler SOLLEN injizierbar sein:

```text
NETWORK_FAILURE
NODE_FAILURE
DEVICE_FAILURE
TIMEOUT
OUT_OF_MEMORY
AUTHENTICATION_FAILURE
AUTHORIZATION_FAILURE
DATA_CORRUPTION
PROTOCOL_FAILURE
```

---

## 317. Regression Tests

Änderungen an Distributed Compute SOLLEN mindestens verglichen werden hinsichtlich:

```text
SubmissionLatency
TransferLatency
Throughput
NetworkTraffic
MemoryUsage
RetryRate
FailureRecoveryTime
SchedulingQuality
SecurityIsolation
```

---

## 318. Minimalimplementierung

Eine minimale konforme Distributed Compute Runtime MUSS mindestens unterstützen:

```text
Single Remote Node
Secure Node Authentication
Protocol Negotiation
Remote Capability Query
Remote CPU or Accelerator Device
Remote Job Submission
Input Transfer
Output Transfer
Job Status Query
Cancellation
Timeout
Basic Failure Detection
Resource Cleanup
```

---

## 319. Erweiterte Implementierung

Eine vollständige Distributed Compute Runtime SOLL zusätzlich unterstützen:

```text
Multiple Nodes
CPU/GPU/NPU Nodes
Automatic Scheduling
Data Locality
Node Groups
Resource Replication
Node-to-Node Transfer
Streaming
Compression
Checkpointing
Migration
Collectives
Elastic Execution
Distributed AI
Remote Compilation
Warm Caches
Advanced Retry
Power-Aware Scheduling
Thermal-Aware Scheduling
Cost-Aware Scheduling
Detailed Profiling
```

---

## 320. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0007` MUSS:

1. Remote Compute Nodes eindeutig identifizieren können.
2. Remote Nodes authentifizieren.
3. Remote Kommunikation gegen Manipulation schützen.
4. Protokollversionen aushandeln.
5. Remote Capabilities abfragen.
6. Remote Compute Devices modellieren.
7. Remote Jobs erzeugen und einreichen können.
8. Remote Job States eindeutig modellieren.
9. Input-Daten übertragen können.
10. Resultate zurückübertragen können.
11. Remote Ressourcen sicher verwalten.
12. Context- und Process-Isolation gewährleisten.
13. Data Policies durchsetzen.
14. Node-Ausfälle erkennen.
15. Netzwerkfehler von Compute-Fehlern unterscheiden.
16. Timeouts unterstützen.
17. Retry nur gemäß expliziter Semantik durchführen.
18. unbekannte Completion States korrekt behandeln.
19. Remote Ressourcen nach Client- oder Job-Ende bereinigen.
20. Distributed Scheduling in den Compute Scheduler integrieren.
21. Remote Memory in das Compute Memory Model integrieren.
22. Remote Objects in das Compute Object Model integrieren.
23. Security Policies bei Fallback beibehalten.
24. Resource Limits und Quotas unterstützen.
25. Diagnoseinformationen bereitstellen.
26. zukünftige Transport- und Cluster-Backends erweiterbar halten.

---

## 321. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen können:

```text
NovaDistributedComputeRuntime
 |
 +-- NodeDiscoveryManager
 |
 +-- NodeRegistry
 |
 +-- NodeTrustManager
 |
 +-- NodeConnectionManager
 |
 +-- ProtocolManager
 |
 +-- RemoteCapabilityManager
 |
 +-- DistributedSchedulerAdapter
 |
 +-- RemoteJobManager
 |
 +-- RemoteResourceManager
 |
 +-- DataTransferManager
 |
 +-- DistributedSynchronizationManager
 |
 +-- CheckpointManager
 |
 +-- FailureRecoveryManager
 |
 +-- DistributedSecurityManager
 |
 +-- DistributedDiagnostics
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Modul- oder Klassenstruktur vor.

---

## 322. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

`NPSPEC-COMPUTE-RUNTIME-0001` definiert die allgemeine NovaOS Compute Runtime.

Distributed Compute erweitert deren Ausführungsmodell über die Grenzen eines einzelnen Systems hinaus.

```text
Compute Runtime
      |
      +--> Local Compute
      |
      +--> Distributed Compute
```

---

## 323. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler muss entscheiden können zwischen:

```text
Local Execution
Remote Execution
Distributed Execution
```

Dabei müssen die zusätzlichen Kosten verteilter Ausführung explizit berücksichtigt werden.

---

## 324. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Remote Resources MÜSSEN in das Compute Memory Model integriert werden.

Dabei MUSS jedoch klar zwischen:

```text
Local Memory
Device Memory
Remote Memory
```

unterschieden werden.

Remote Memory besitzt andere:

```text
Latency
Bandwidth
Failure Semantics
Consistency Semantics
Security Boundaries
```

als lokaler Speicher.

---

## 325. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Distributed Compute Objects MÜSSEN die allgemeinen Object-Model-Regeln einhalten.

Beispiele:

```text
RemoteNode
RemoteDevice
RemoteContext
RemoteJob
RemoteBuffer
RemoteTensor
RemoteEvent
RemoteFence
```

---

## 326. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Remote GPUs werden über die GPU Runtime des Ziel-Nodes ausgeführt.

```text
Local Compute Runtime
        |
        v
Distributed Runtime
        |
        v
Remote Node
        |
        v
GPU Runtime
        |
        v
GPU
```

---

## 327. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

Remote NPUs werden über die NPU Runtime des Ziel-Nodes ausgeführt.

```text
Local Compute Runtime
        |
        v
Distributed Runtime
        |
        v
Remote Node
        |
        v
NPU Runtime
        |
        v
NPU
```

---

## 328. Gesamtes heterogenes Compute-Modell

Distributed Compute erweitert das lokale heterogene Compute-Modell.

```text
                       Nova Compute Runtime
                               |
              +----------------+----------------+
              |                                 |
              v                                 v
        Local Compute                   Distributed Compute
              |                                 |
      +-------+-------+                 +-------+-------+
      |       |       |                 |       |       |
      v       v       v                 v       v       v
     CPU     GPU     NPU               Node A  Node B  Node C
                                          |       |       |
                                       CPU/GPU   NPU   CPU/GPU/NPU
```

---

## 329. Empfohlene Folge-Spezifikationen

Auf dieser Spezifikation SOLLEN weitere Detail-Spezifikationen aufbauen können.

Empfohlen:

```text
NPSPEC-COMPUTE-DIST-NODE-0001
Compute Node Model

NPSPEC-COMPUTE-DIST-DISCOVERY-0001
Node Discovery

NPSPEC-COMPUTE-DIST-REGISTRY-0001
Node Registry

NPSPEC-COMPUTE-DIST-PROTOCOL-0001
Distributed Compute Protocol

NPSPEC-COMPUTE-DIST-TRANSPORT-0001
Distributed Compute Transport

NPSPEC-COMPUTE-DIST-SECURITY-0001
Distributed Compute Security

NPSPEC-COMPUTE-DIST-TRUST-0001
Node Trust Model

NPSPEC-COMPUTE-DIST-JOB-0001
Remote Job Model

NPSPEC-COMPUTE-DIST-RESOURCE-0001
Remote Resource Model

NPSPEC-COMPUTE-DIST-MEMORY-0001
Remote Memory Model

NPSPEC-COMPUTE-DIST-TRANSFER-0001
Distributed Data Transfer

NPSPEC-COMPUTE-DIST-SCHED-0001
Distributed Scheduling

NPSPEC-COMPUTE-DIST-SYNC-0001
Distributed Synchronization

NPSPEC-COMPUTE-DIST-RETRY-0001
Distributed Retry Semantics

NPSPEC-COMPUTE-DIST-RECOVERY-0001
Distributed Failure Recovery

NPSPEC-COMPUTE-DIST-CHECKPOINT-0001
Distributed Checkpointing

NPSPEC-COMPUTE-DIST-COLLECTIVE-0001
Collective Operations

NPSPEC-COMPUTE-DIST-ELASTIC-0001
Elastic Compute

NPSPEC-COMPUTE-DIST-AI-0001
Distributed AI Compute

NPSPEC-COMPUTE-DIST-PROFILE-0001
Distributed Compute Profiling

NPSPEC-COMPUTE-DIST-TEST-0001
Distributed Compute Conformance Tests
```

---

## 330. Zusammenfassung

Distributed Compute erweitert die NovaOS Compute Runtime von einem lokalen heterogenen Compute-System zu einer verteilten Compute-Infrastruktur.

Das grundlegende Modell lautet:

```text
Application
    |
    v
Nova Compute API
    |
    v
Compute Runtime
    |
    +----------------------+
    |                      |
    v                      v
Local Compute       Distributed Compute
    |                      |
    |              +-------+-------+
    |              |       |       |
    v              v       v       v
CPU/GPU/NPU      Node A  Node B  Node C
```

Ein typischer Distributed-Compute-Ablauf lautet:

```text
Create Job
    |
    v
Analyze Requirements
    |
    v
Evaluate Local Devices
    |
    v
Discover Eligible Nodes
    |
    v
Apply Security / Data Policy
    |
    v
Estimate Execution Cost
    |
    v
Select Node + Device
    |
    v
Transfer Required Data
    |
    v
Submit Remote Job
    |
    v
Remote Execution
    |
    v
Verify Completion
    |
    v
Transfer / Reference Result
    |
    v
Complete Job
```

Die Auswahl eines Remote Nodes darf nicht allein anhand dessen theoretischer Rechenleistung erfolgen.

NovaOS muss die tatsächlichen Gesamtkosten betrachten:

```text
Compute Performance
+
Queue Delay
+
Network Latency
+
Transfer Volume
+
Data Locality
+
Memory Availability
+
Compilation Cost
+
Power
+
Thermals
+
Reliability
+
Security Policy
```

Dadurch kann beispielsweise ein langsamerer Node mit bereits lokal vorhandenen Daten effizienter sein als ein wesentlich schnellerer Node, zu dem zunächst mehrere Gigabyte übertragen werden müssten.

Gleichzeitig darf die Abstraktion die fundamentalen Eigenschaften verteilter Systeme nicht verbergen.

Insbesondere gilt:

```text
Remote != Local
```

Remote-Ausführung besitzt zusätzliche Fehlerzustände:

```text
Connection Lost
Node Lost
Unknown Completion
Partial Transfer
Authentication Failure
Remote Device Failure
```

Diese Zustände müssen explizit modelliert werden.

Die langfristige NovaOS-Compute-Architektur lautet damit:

```text
                         Nova Compute
                              |
                 +------------+------------+
                 |                         |
                 v                         v
            Local System             Remote Systems
                 |                         |
          +------+------+          +-------+-------+
          |      |      |          |       |       |
          v      v      v          v       v       v
         CPU    GPU    NPU       Node A  Node B  Node C
                                  |        |        |
                                CPU/GPU   NPU   CPU/GPU/NPU
```

Damit können Anwendungen dieselbe Compute-Infrastruktur für lokale und verteilte Berechnungen verwenden, während NovaOS Scheduling, Datenlokalität, Netzwerk, Sicherheit, Fehlerbehandlung und Hardwareauswahl übernimmt.

Das grundlegende Architekturprinzip lautet:

> Distributed Compute erweitert den NovaOS-Compute-Raum über die Grenzen eines einzelnen Rechners hinaus. Entfernte CPUs, GPUs und NPUs können als zusätzliche Compute-Ressourcen genutzt werden, ohne die fundamentalen Unterschiede zwischen lokalem und verteiltem Rechnen zu verschleiern. Sicherheit, Datenlokalität, Netzwerkaufwand und Fehlersemantik sind dabei gleichwertige Bestandteile der Scheduling-Entscheidung wie die reine Rechenleistung.