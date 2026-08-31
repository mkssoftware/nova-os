# ADR-ARCH-0007 – Location Transparency mit sichtbaren Kosten und Fehlern

## Status

Angenommen

## Kategorie

Systemarchitektur / Verteilte Systeme / Execution / Ressourcen / Datenlokalität

## Kontext

NovaOS soll Fähigkeiten, Daten, Dienste und Rechenressourcen nicht ausschließlich an einen festen physischen Ausführungsort binden.

Eine Capability kann beispielsweise durch unterschiedliche Provider bereitgestellt werden:

```text
Local CPU
Local GPU
Local NPU
System Service
Driver Domain
MicroVM
Remote Device
Remote NovaOS Node
Edge Node
Cloud Provider
```

Für eine Anwendung soll die semantische Bedeutung einer Operation möglichst unabhängig davon bleiben, wo die konkrete Implementierung ausgeführt wird.

Beispiel:

```text
Image.Filter.Gaussian(image)
```

soll semantisch dieselbe Operation darstellen, unabhängig davon, ob sie ausgeführt wird auf:

```text
CPU
GPU
NPU
Remote Accelerator
```

Dieses Prinzip wird als:

```text
Location Transparency
```

bezeichnet.

Eine naive Form von Location Transparency birgt jedoch erhebliche Risiken.

Ein lokaler Funktionsaufruf und eine entfernte Operation unterscheiden sich fundamental hinsichtlich:

- Latenz
- Bandbreite
- Verfügbarkeit
- Energieverbrauch
- Ressourcenverbrauch
- Fehlerverhalten
- Datenschutz
- Data Sovereignty
- Trust
- Sicherheit
- Kosten
- Determinismus
- Konsistenz
- Partitionierung
- Datenbewegung

Ein lokaler Aufruf kann beispielsweise eine Latenz von:

```text
50 µs
```

besitzen.

Dieselbe semantische Capability auf einem entfernten System kann dagegen:

```text
50 ms
```

oder mehr benötigen.

Ein lokaler Fehler kann beispielsweise sein:

```text
OutOfMemory
```

während bei entfernter Ausführung zusätzlich Fehler auftreten können wie:

```text
NetworkUnavailable
Timeout
RemoteNodeUnavailable
ConnectionLost
RemoteProviderFailure
SovereigntyViolation
TrustViolation
```

NovaOS darf diese Unterschiede nicht vollständig hinter einer scheinbar identischen lokalen Abstraktion verstecken.

Deshalb verwendet NovaOS eine kontrollierte Form der Location Transparency:

```text
Semantic Location Transparency
+
Operational Location Awareness
```

Die semantische Operation bleibt ortsunabhängig.

Die relevanten Kosten, Risiken und Fehler des tatsächlichen Ausführungsortes bleiben jedoch für:

- Runtime
- Planner
- Resource Economy
- Policies
- Anwendungen
- Entwickler
- Architecture Introspection

sichtbar.

## Entscheidung

NovaOS verwendet **Location Transparency mit sichtbaren Kosten und Fehlern** als systemweites Architekturprinzip.

Capabilities, Datenobjekte und Services SOLLEN grundsätzlich so beschrieben werden, dass ihre semantische Identität nicht von einem bestimmten physischen Ausführungsort abhängt.

Das bedeutet:

```text
What
```

wird getrennt von:

```text
Where
```

beschrieben.

Beispiel:

```text
Capability:
    de.nova.image.filter.gaussian
```

ist nicht identisch mit:

```text
GPU0
```

oder:

```text
RemoteNode42
```

Stattdessen können mehrere Provider dieselbe Capability bereitstellen:

```text
de.nova.image.filter.gaussian
    │
    ├── CPUProvider
    ├── GPUProvider
    ├── NPUProvider
    └── RemoteProvider
```

NovaOS kann anhand von:

- Execution Contract
- Resource Economy
- Trust
- Data Sovereignty
- Determinism
- Performance
- Energie
- Thermal Headroom
- Verfügbarkeit
- Datenlokalität
- Netzwerkbedingungen
- Benutzer- und System-Policies

entscheiden, welcher Provider verwendet wird.

Location Transparency darf dabei niemals bedeuten:

```text
Location does not matter.
```

Das verbindliche Prinzip lautet:

```text
Location does not define semantics,
but location affects execution.
```

## Grundprinzip

NovaOS trennt:

```text
Semantic Identity
```

von:

```text
Execution Location
```

und:

```text
Resource Location
```

Beispiel:

```text
Capability
    de.nova.ai.inference.object_detection
```

kann ausgeführt werden auf:

```text
Local NPU
Local GPU
Remote Edge Node
Cloud Accelerator
```

Die Capability bleibt dieselbe.

Die konkreten Eigenschaften der Ausführung unterscheiden sich.

## Semantic Location Transparency

Semantic Location Transparency bedeutet:

Eine Anwendung beschreibt primär:

```text
Was soll ausgeführt werden?
```

und nicht zwingend:

```text
Auf welchem konkreten Gerät muss es ausgeführt werden?
```

Beispiel:

```text
result = DetectObjects(image)
```

statt:

```text
result = SendImageToGpu0AndRunModel(image)
```

Die Runtime kann einen geeigneten Provider bestimmen.

## Operational Location Awareness

Die tatsächliche Ausführungslocation darf jedoch nicht unsichtbar sein.

NovaOS muss relevante Eigenschaften des gewählten Providers berücksichtigen können.

Beispiel:

```text
Provider {
    location
    latency
    bandwidth
    trust
    sovereignty
    availability
    cost
    failureDomain
}
```

## Location

Eine Location beschreibt den logischen beziehungsweise physischen Ort einer Ressource oder Ausführung.

Mögliche Location-Klassen sind:

```text
SameThread
SameProcess
SameExecutionDomain
SameKernel
SameDevice
SameMachine
SameLAN
Edge
RemoteNode
Cloud
ExternalProvider
```

Diese Klassen sind keine vollständige Netzwerkadressierung.

Sie beschreiben die semantisch relevante Nähe beziehungsweise Ausführungsdomäne.

## Location Identity

Konkrete Locations können eindeutig identifizierbar sein.

Beispiele:

```text
Local.CPU.Package0
Local.GPU0
Local.NPU0
Node.Workstation01
Node.Edge01
Cloud.Region.EU-Central
```

Die konkrete Syntax wird durch separate NPSPECs definiert.

## Location Hierarchy

Locations können hierarchisch organisiert sein.

Beispiel:

```text
System
└── Machine
    ├── CPU
    ├── GPU
    └── NPU
```

oder:

```text
Network
└── Site
    ├── Node01
    ├── Node02
    └── Node03
```

Eine hierarchische Location-Beschreibung ermöglicht Policies wie:

```text
SameMachineOnly
SameSiteOnly
GermanyOnly
EUOnly
```

## Provider Location

Jeder Provider SOLL seine relevante Ausführungslocation beschreiben können.

Beispiel:

```text
Provider {
    capability = de.nova.image.filter.gaussian
    location   = Local.GPU0
}
```

oder:

```text
Provider {
    capability = de.nova.image.filter.gaussian
    location   = Node.Edge01.GPU0
}
```

## Data Location

Auch Daten besitzen eine Location.

Beispiel:

```text
DataObject {
    location = Local.Memory
}
```

oder:

```text
DataObject {
    location = GPU0.VRAM
}
```

oder:

```text
DataObject {
    location = Node.Storage01
}
```

Provider Location und Data Location müssen nicht identisch sein.

## Compute-to-Data

NovaOS SOLL bevorzugt prüfen, ob Compute zu den Daten gebracht werden kann, bevor große Datenmengen zum Compute verschoben werden.

Beispiel:

```text
10 GiB Dataset:
    Node.Storage01

Provider A:
    Local CPU

Provider B:
    Node.Storage01.Compute
```

Auch wenn Provider A nominell schneller rechnet, kann Provider B insgesamt effizienter sein, weil keine 10 GiB übertragen werden müssen.

Das Planungsprinzip lautet daher:

```text
Compute-to-Data
```

wenn dies unter Berücksichtigung aller anderen Constraints sinnvoll ist.

## Data-to-Compute

Data-to-Compute bleibt zulässig.

Beispiel:

```text
Dataset
    ↓ Transfer
GPU
    ↓
Compute
```

Dies kann sinnvoll sein, wenn:

- Daten klein sind,
- Compute erheblich schneller ist,
- Daten mehrfach wiederverwendet werden,
- die Transferkosten akzeptabel sind.

Die Entscheidung erfolgt durch Planner und Resource Economy.

## Data Gravity

Große oder häufig verwendete Daten erzeugen Data Gravity.

NovaOS SOLL Datenlokalität deshalb als Planungsfaktor berücksichtigen.

Beispiel:

```text
Provider A:
    Compute = 2 ms
    Transfer = 500 ms

Provider B:
    Compute = 50 ms
    Transfer = 0 ms
```

Provider B ist für die Gesamtoperation günstiger.

## Execution Location Constraint

Ein Execution Contract kann Location Constraints enthalten.

Beispiel:

```text
ExecutionContract {
    location {
        allowed = LocalOnly
    }
}
```

oder:

```text
ExecutionContract {
    location {
        allowed = [
            SameMachine,
            SameLAN
        ]
    }
}
```

## Preferred Location

Ein Contract kann bevorzugte Locations definieren.

Beispiel:

```text
ExecutionContract {
    location {
        preferred = Local
        remoteAllowed = true
    }
}
```

Dies ist ein Soft Constraint.

## Forced Location

Entwickler oder autorisierte Anwendungen dürfen eine konkrete Location erzwingen, wenn die Policy dies erlaubt.

Beispiel:

```text
Execute(
    capability,
    location = GPU0
)
```

Dies entspricht dem NovaOS-Prinzip:

```text
Automatic by default.
Explicit override when requested.
```

Ein expliziter Override darf jedoch keine:

- Security Policy
- Data-Sovereignty Policy
- Safety Requirement
- Capability Restriction

umgehen.

## Location Policy

Policies können bestimmen, welche Locations verwendet werden dürfen.

Beispiele:

```text
LocalOnly
SameMachine
SameSite
PrivateNetworkOnly
EUOnly
GermanyOnly
TrustedNodesOnly
CloudAllowed
CloudForbidden
```

## Location Policy Scope

Location Policies können gelten für:

- System
- User
- Session
- Application
- Capability
- Data Object
- Transaction
- Execution Contract

Untergeordnete Policies dürfen übergeordnete harte Einschränkungen nicht abschwächen.

## Execution Contract

Location ist Bestandteil von `Nova.ExecutionContract`.

Beispiel:

```text
ExecutionContract {
    latency <= 20 ms

    location {
        localPreferred = true
        remoteAllowed  = true
    }

    sovereignty {
        allowedRegions = [
            Germany
        ]
    }

    trust {
        minimum = SystemVerified
    }
}
```

## Hard Constraints

Harte Constraints werden vor Provideroptimierung geprüft.

Beispiel:

```text
Provider A:
    latency = 5 ms
    location = US

Provider B:
    latency = 12 ms
    location = Germany

Contract:
    sovereignty = GermanyOnly
```

Provider A ist ungültig.

Seine bessere Performance darf den Sovereignty Constraint nicht überschreiben.

## Soft Constraints

Soft Constraints können gegeneinander abgewogen werden.

Beispiel:

```text
Prefer:
    Local
    LowEnergy
    LowLatency
```

Wenn kein Provider alle Präferenzen optimal erfüllt, darf NovaOS einen gewichteten Trade-off bestimmen.

## Kosten

Location Transparency muss die Kosten einer Location sichtbar halten.

Kosten können umfassen:

```text
Latency
Bandwidth
DataMovement
Energy
Thermal
CPU
Memory
Network
MonetaryCost
Serialization
Encryption
Synchronization
```

## Latency

Remote-Ausführung besitzt zusätzliche Latenz.

Die Gesamtzeit kann vereinfacht beschrieben werden als:

```text
TotalLatency =
    QueueDelay
  + Serialization
  + TransferToProvider
  + Execution
  + TransferFromProvider
  + Synchronization
```

Eine Providerentscheidung darf deshalb nicht ausschließlich die reine Compute-Zeit betrachten.

## Bandwidth

Remote Provider benötigen Netzwerkbandbreite.

Beispiel:

```text
Input:
    2 GiB

Network:
    100 MiB/s

Minimum Transfer Time:
    ~20 s
```

Ein nominell schneller Remote Provider kann dadurch ungeeignet sein.

## Data Movement

Location Planning ist direkt mit ADR-ARCH-0006 verbunden.

Der Planner SOLL Datenbewegung minimieren.

Beispiel:

```text
CPU → GPU
```

kann günstiger sein als:

```text
CPU → Network → Remote GPU
```

selbst wenn die Remote GPU eine höhere Compute-Leistung besitzt.

## Zero-Copy

Location Transparency und Zero-Copy ergänzen sich.

Innerhalb derselben Maschine können Daten über:

- Shared Memory
- DMA
- Shared Buffers
- Memory Mapping

weitergegeben werden.

Über Netzwerkgrenzen ist echtes Shared Physical Memory häufig nicht möglich.

Die semantische API bleibt dennoch identisch.

## Remote Data Transfer

Remote Transfers können:

- serialisieren
- komprimieren
- verschlüsseln
- fragmentieren
- streamen

müssen.

Diese Operationen sind Teil der tatsächlichen Ausführungskosten.

Sie dürfen nicht durch Location Transparency unsichtbar gemacht werden.

## Serialization Cost

Serialization ist ein expliziter Kostenfaktor.

Beispiel:

```text
Execution:
    4 ms

Serialization:
    10 ms

Network:
    20 ms

Total:
    34 ms
```

Die Providerwahl muss den Gesamtpfad bewerten.

## Compression

NovaOS DARF Daten vor einem Remote Transfer komprimieren.

Beispiel:

```text
Compression Cost:
    5 ms

Transfer Saving:
    40 ms
```

Dann kann Kompression sinnvoll sein.

Bei kleinen Daten kann sie dagegen unnötige Kosten verursachen.

Die Entscheidung ist Policy- und Planner-gesteuert.

## Encryption

Remote-Datenübertragung kann Verschlüsselung erfordern.

Encryption Cost ist Teil der Resource Economy.

Security Requirements dürfen jedoch nicht aufgrund von Performanceoptimierung deaktiviert werden.

## Monetary Cost

Remote Provider können monetäre Kosten besitzen.

Beispiel:

```text
Provider {
    monetaryCost = 0.02 EUR / request
}
```

oder:

```text
Provider {
    monetaryCost = 0.50 EUR / GPU-hour
}
```

NovaOS DARF solche Kosten als Policy-Faktor berücksichtigen.

## Cost Budget

Ein Execution Contract kann monetäre Budgets enthalten.

Beispiel:

```text
ExecutionContract {
    monetaryCost {
        maximum = 0.05 EUR
    }
}
```

Eine automatische Providerwahl darf dieses Hard Budget nicht überschreiten.

## Resource Economy

Location Planning ist Bestandteil der systemweiten Ressourcenökonomie.

Ein Remote Provider verbraucht nicht nur Compute-Ressourcen.

Er kann zusätzlich benötigen:

```text
Network
Energy
Serialization CPU
Encryption CPU
Local Buffer Memory
Remote Resources
```

Diese Kosten müssen gemeinsam betrachtet werden können.

## Resource Profile

Provider SOLLEN Location-bezogene Resource Profiles veröffentlichen können.

Beispiel:

```text
ResourceProfile {
    provider = RemoteGPU

    execution = 4 ms
    network   = 40 MiB
    energy    = 2 J
    location  = Edge01
}
```

## Dynamic Costs

Location Costs sind nicht zwingend konstant.

Sie können sich verändern durch:

- Netzwerkbelastung
- Routing
- Thermal Pressure
- Provider Load
- Battery State
- Cloud Capacity
- Remote Node Load

NovaOS muss dynamische Kosten berücksichtigen können.

## Cost Prediction

NovaOS DARF zukünftige Location Costs vorhersagen.

Beispiel:

```text
Predicted Network Latency:
    12 ms

Observed:
    25 ms
```

Prediction Error kann zur Anpassung zukünftiger Modelle verwendet werden.

## Prediction Error

Das allgemeine adaptive Modell gilt auch für Location Planning:

```text
Prediction
    ↓
Execution
    ↓
Observation
    ↓
Prediction Error
    ↓
Model Correction
```

Harte Constraints dürfen nicht auf bloßen Vorhersagen beruhen, wenn keine entsprechende Garantie existiert.

## Fehler

Location Transparency darf Fehler nicht semantisch verstecken.

NovaOS unterscheidet zwischen:

```text
Semantic Failure
```

und:

```text
Location / Transport Failure
```

## Semantic Failure

Ein Semantic Failure entsteht durch die eigentliche Operation.

Beispiele:

```text
InvalidInput
UnsupportedFormat
InsufficientPrecision
```

Diese Fehler können unabhängig vom Ausführungsort auftreten.

## Location Failure

Ein Location Failure entsteht durch den Ort beziehungsweise den Kommunikationspfad.

Beispiele:

```text
LocationUnavailable
NodeUnavailable
ProviderUnavailable
NetworkUnavailable
ConnectionLost
Partitioned
Timeout
RemoteReset
```

Diese Fehler müssen erkennbar bleiben.

## Partial Failure

Verteilte Systeme können Partial Failures besitzen.

Beispiel:

```text
Local System:
    Healthy

Remote Node:
    Unreachable
```

NovaOS darf einen Remote Failure nicht automatisch als lokalen Systemfehler behandeln.

## Network Partition

Eine Network Partition ist ein normal zu berücksichtigender Fehlerzustand verteilter Ausführung.

Beispiel:

```text
Node A
   X
Node B
```

Das System muss definieren können, ob eine Operation:

- wartet
- fehlschlägt
- lokal erneut ausgeführt wird
- einen anderen Provider verwendet
- degradiert

## Timeout

Remote Operationen benötigen definierte Timeout-Semantik.

Ein Timeout bedeutet nicht zwingend:

```text
Operation wurde nicht ausgeführt.
```

Es kann bedeuten:

```text
Ergebnis wurde nicht rechtzeitig beobachtet.
```

Dies ist insbesondere bei mutierenden Operationen kritisch.

## Unknown Outcome

Für entfernte mutierende Operationen muss der Zustand:

```text
UnknownOutcome
```

modellierbar sein.

Beispiel:

```text
Request sent
    ↓
Remote operation executes
    ↓
Connection lost before acknowledgement
```

Der lokale Caller weiß nicht sicher, ob die Operation durchgeführt wurde.

## Transaktionale Systemoperationen

ADR-ARCH-0004 ist deshalb besonders wichtig für Location Transparency.

Remote mutierende Operationen SOLLEN:

- Transaction IDs
- Idempotency Keys
- Commit Protocols
- Durable Operation IDs

verwenden können.

Dadurch können Wiederholungen sicherer behandelt werden.

## Idempotency

Operationen SOLLEN deklarieren können, ob sie idempotent sind.

Beispiel:

```text
GetStatus()
```

ist typischerweise idempotent.

Dagegen kann:

```text
ChargeAccount(10 EUR)
```

ohne zusätzliche Semantik nicht sicher wiederholt werden.

Der Retry-Mechanismus muss diese Unterschiede kennen.

## Retry

NovaOS darf fehlgeschlagene Operationen automatisch erneut versuchen.

Automatische Retries sind nur zulässig, wenn:

- die Operation idempotent ist,
- eine Transaction ID Doppelwirkungen verhindert,
- der Contract Retry erlaubt.

## Retry Policy

Eine Retry Policy kann definieren:

```text
maxAttempts
backoff
deadline
allowedFailureClasses
```

Beispiel:

```text
RetryPolicy {
    maxAttempts = 3
    backoff = Exponential
}
```

## Deadline

Retries dürfen die übergeordnete Deadline nicht ignorieren.

Beispiel:

```text
Total Deadline:
    100 ms

Attempt 1:
    70 ms

Remaining:
    30 ms
```

Ein zweiter Versuch mit erwarteten 80 ms ist dann nicht sinnvoll.

## Fallback

NovaOS kann bei Location Failure einen alternativen Provider verwenden.

Beispiel:

```text
RemoteGPU
    ↓ Failure
LocalGPU
```

Ein Fallback ist nur zulässig, wenn der alternative Provider weiterhin alle Hard Constraints erfüllt.

## Fallback Chain

Capabilities können mehrere geeignete Provider besitzen.

Beispiel:

```text
Preferred:
    Local NPU

Fallback:
    Local GPU

Fallback:
    Local CPU

Optional:
    Remote NPU
```

Die Reihenfolge kann dynamisch durch Policies bestimmt werden.

## Failover

Für langlebige Services kann NovaOS Failover unterstützen.

Beispiel:

```text
Service@NodeA
      ↓ Failure
Service@NodeB
```

State und Transaction Semantics müssen dabei separat definiert werden.

## Duplicate Execution

Retries und Failover können zu Duplicate Execution führen.

NovaOS muss deshalb zwischen:

```text
AtMostOnce
AtLeastOnce
EffectivelyOnce
```

unterscheiden können.

Ein generelles echtes `ExactlyOnce` über beliebige Failure Domains darf nicht leichtfertig versprochen werden.

## Delivery Semantics

Remote Execution Contracts können Delivery Semantics definieren.

Beispiel:

```text
Delivery {
    semantics = AtMostOnce
}
```

oder:

```text
Delivery {
    semantics = AtLeastOnce
}
```

Effektiv einmalige Ausführung kann über:

- Transaction IDs
- Deduplication
- Idempotency

realisiert werden.

## State

Location Transparency für zustandsbehaftete Komponenten ist komplexer als für stateless Operationen.

State kann:

- lokal
- repliziert
- verteilt
- persistent
- migratable

sein.

Die Location des Codes und die Location des State müssen getrennt modelliert werden.

## State Affinity

Ein Provider kann eine Affinität zu einem bestimmten State besitzen.

Beispiel:

```text
Database State:
    Node01

Provider:
    Node01.Compute
```

Dieser Provider kann trotz geringerer nomineller Compute-Leistung effizienter sein.

## Stateful Migration

Stateful Services dürfen zwischen Locations migriert werden.

Dies erfordert:

- State Transfer
- Quiescence
- Version Compatibility
- Capability Rebinding
- Transaction Handling
- Failure Recovery

Hot Replacement und Live Evolution müssen damit integriert werden.

## Stateless Operations

Stateless Capabilities sind besonders einfach location-transparent ausführbar.

Beispiel:

```text
Hash(data)
```

oder:

```text
ResizeImage(image)
```

wenn sämtliche benötigten Inputs explizit vorhanden sind.

## Caching

NovaOS darf Ergebnisse oder Daten an mehreren Locations cachen.

Caching kann:

- Latenz reduzieren
- Netzwerkverkehr reduzieren
- Offline-Fähigkeit verbessern

erzeugt jedoch zusätzliche Fragen zu:

- Konsistenz
- Lifetime
- Invalidierung
- Data Sovereignty
- Storage Cost

## Cache Location

Caches besitzen eine explizite Location.

Beispiel:

```text
Cache {
    location = Edge01
}
```

Data-Sovereignty-Policies gelten auch für Cache Copies.

## Replication

Daten oder Services dürfen repliziert werden.

Beispiel:

```text
Data
    ├── NodeA
    ├── NodeB
    └── NodeC
```

Replication verbessert Verfügbarkeit, erzeugt jedoch Konsistenz- und Ressourcenfragen.

## Consistency

Location Transparency darf Konsistenzmodelle nicht verstecken.

NovaOS muss für verteilte Daten unterschiedliche Modelle unterstützen können.

Beispiele:

```text
Strong
Causal
Session
Eventual
ApplicationDefined
```

Die genaue Definition erfolgt in separaten ADRs und NPSPECs.

## Consistency Contract

Ein Execution Contract beziehungsweise Data Contract kann Konsistenzanforderungen definieren.

Beispiel:

```text
consistency = Strong
```

Ein Provider, der nur Eventual Consistency anbieten kann, ist dann ungültig.

## CAP-Aspekte

Bei verteilten zustandsbehafteten Systemen kann eine Network Partition nicht ignoriert werden.

NovaOS darf nicht suggerieren, dass gleichzeitig beliebige:

- Consistency
- Availability
- Partition Tolerance

ohne Trade-offs garantiert werden können.

Diese Trade-offs müssen explizit modellierbar bleiben.

## Data Sovereignty

Location Transparency ist direkt mit Data Sovereignty verbunden.

Daten dürfen nur an Locations übertragen werden, die ihre Sovereignty Policy erlauben.

Beispiel:

```text
DataObject {
    sovereignty {
        allowed = Germany
    }
}
```

Ein Provider in einer nicht zulässigen Region ist kein gültiger Kandidat.

## Data Residency

NovaOS unterscheidet zwischen:

```text
Execution Location
```

und:

```text
Data Residency
```

Ein Service kann beispielsweise in Deutschland ausgeführt werden, aber Daten auf einem Storage Provider in einer anderen Region speichern.

Beide Aspekte müssen getrennt geprüft werden.

## Information Flow

Location Constraints können Teil der systemweiten Information-Flow-Policy sein.

Beispiel:

```text
MedicalData
    ↓
GermanyOnly
```

Ein Informationsfluss zu einem nicht zulässigen Remote Provider muss blockiert werden.

## Trust

Location allein bestimmt nicht Trust.

Ein lokaler Provider kann untrusted sein.

Ein Remote Provider kann stark attestiert sein.

NovaOS bewertet deshalb getrennt:

```text
Location
Trust
Identity
Provenance
Attestation
```

## Nova.Trust

Remote Provider müssen in Nova.Trust integrierbar sein.

Die Vertrauenskette kann beispielsweise umfassen:

```text
Identity
    ↓
Signature
    ↓
Provenance
    ↓
Attestation
    ↓
Permission
    ↓
Capability
    ↓
Audit
```

## Remote Attestation

Für bestimmte Workloads kann Remote Attestation erforderlich sein.

Beispiel:

```text
ExecutionContract {
    trust {
        remoteAttestation = Required
    }
}
```

Ein Provider ohne gültige Attestation ist dann ungültig.

## Capability Security

Remote Location Transparency ändert nicht das Capability-Modell.

Ein Remote Provider erhält nur die Capabilities, die für seine Aufgabe erforderlich sind.

Es existiert keine implizite Vollmacht allein aufgrund der Remote-Verbindung.

## Remote Capabilities

Capabilities über Node-Grenzen benötigen eine sichere Repräsentation.

Sie müssen insbesondere:

- authentifiziert
- autorisiert
- scoped
- revokierbar
- gegen Replay geschützt

sein.

Die konkrete Protokollspezifikation erfolgt separat.

## Capability Delegation

Eine lokale Capability darf nicht automatisch vollständig an einen Remote Provider übertragen werden.

Stattdessen soll Capability Attenuation verwendet werden.

Beispiel:

```text
Local Capability:
    Read + Write + Delete

Remote Delegation:
    Read
```

## Capability Lifetime

Remote Capabilities benötigen definierte Lifetimes.

Mögliche Modelle:

```text
Lease
Session
Transaction
Execution
Persistent
```

Kurzlebige Execution Capabilities sollen nach Abschluss automatisch ungültig werden können.

## Revocation

Remote Capability Revocation ist komplexer als lokale Revocation.

Ein Remote Node kann temporär nicht erreichbar sein.

Deshalb können Mechanismen wie:

- Short Leases
- Epochs
- Revocation Lists
- Session Keys

verwendet werden.

## Authentication

Remote Nodes und Provider müssen authentifiziert werden.

Location Transparency darf niemals bedeuten:

```text
Any network endpoint can act as a provider.
```

Provideridentität ist Teil von Nova.Trust.

## Transport Security

Remote Kommunikation MUSS angemessen geschützt werden.

Je nach Contract kann dies umfassen:

- Encryption
- Integrity
- Mutual Authentication
- Replay Protection
- Forward Secrecy

## Confidential Computing

NovaOS DARF Confidential-Computing-Provider verwenden.

Beispiel:

```text
Remote Confidential VM
```

Ein Execution Contract kann verlangen:

```text
confidentialExecution = Required
```

## Privacy

Remote Provider können zusätzliche Privacy-Risiken erzeugen.

Der Planner muss deshalb nicht nur Performance, sondern auch:

- Trust
- Sovereignty
- Information Flow
- User Policy

berücksichtigen.

## Benutzerkontrolle

Benutzer sollen Policies definieren können wie:

```text
NeverUseCloud
PreferLocal
AllowTrustedEdge
AllowCloudForPublicData
```

Solche Präferenzen sind Teil des Policy-Modells.

## Transparenz für Benutzer

NovaOS soll dem Benutzer bei relevanten Operationen anzeigen können, ob Daten:

- lokal verarbeitet
- an ein anderes Gerät übertragen
- an Edge-Infrastruktur übertragen
- an einen Cloud Provider übertragen

werden.

Die konkrete UI wird separat spezifiziert.

## Offline Mode

NovaOS MUSS einen Betriebsmodus unterstützen können, in dem Remote Provider nicht verwendet werden.

Beispiel:

```text
Network unavailable
```

oder:

```text
Policy = LocalOnly
```

Native Kernfunktionen dürfen nicht grundsätzlich von Cloudverfügbarkeit abhängen.

## Local-First

Für fundamentale Systemfunktionen gilt grundsätzlich:

```text
Local-first where practical.
```

Remote Provider dürfen lokale Fähigkeiten erweitern.

Sie sollen jedoch nicht ohne explizite Architekturentscheidung zur zwingenden Voraussetzung fundamentaler Systemfunktionen werden.

## Cloud Independence

NovaOS ist nicht Cloud-first im Sinne einer zwingenden Cloudabhängigkeit.

Cloud ist eine mögliche Execution Location.

Sie ist nicht die fundamentale Systemarchitektur.

## Edge Computing

Edge Nodes können zwischen lokalem System und Cloud liegen.

Beispiel:

```text
Device
   ↓
Local Network
   ↓
Edge Node
   ↓
Cloud
```

Der Planner kann abhängig von:

- Latenz
- Sovereignty
- Energie
- Verfügbarkeit

zwischen diesen Ebenen wählen.

## Multi-Device

NovaOS soll mehrere persönliche Geräte als mögliche Locations integrieren können.

Beispiele:

```text
Desktop
Laptop
Tablet
Phone
Home Server
```

Eine Capability kann gegebenenfalls auf einem anderen autorisierten Gerät ausgeführt werden.

## Device Presence

Remote Geräte können dynamisch erscheinen und verschwinden.

Location Planning muss deshalb dynamische Provider Discovery unterstützen.

Ein Gerät darf nicht als dauerhaft verfügbar angenommen werden.

## Mobility

Bei mobilen Geräten ändern sich:

- Netzwerk
- Standort
- Bandbreite
- Energie
- Provider Availability

während der Laufzeit.

Location Policies müssen solche Änderungen berücksichtigen können.

## Migration während Ausführung

Lang laufende Workloads dürfen zwischen Locations migriert werden, wenn dies unterstützt wird.

Beispiel:

```text
Laptop NPU
    ↓ Battery low
Desktop GPU
```

Migration ist nur zulässig, wenn:

- State übertragbar ist
- Contract dies erlaubt
- Trust erfüllt bleibt
- Sovereignty erfüllt bleibt
- Deadline nicht verletzt wird

## Session Continuity

Location Transparency kann Session Continuity zwischen Geräten unterstützen.

Beispiel:

```text
Application Session
    Desktop
       ↓
    Tablet
```

Dies ist jedoch eine höhere Systemfunktion und nicht automatisch Bestandteil jeder Capability.

## Failure Domain

Jede Location besitzt eine Failure Domain.

Beispiele:

```text
Process
Execution Domain
Device
Machine
Network
Site
Region
Cloud Provider
```

Der Planner SOLL Failure-Domain-Diversität berücksichtigen können.

## Failure Correlation

Mehrere Provider können scheinbar unabhängig sein, aber dieselbe Failure Domain teilen.

Beispiel:

```text
Provider A → Node01
Provider B → Node01
```

oder:

```text
Provider A → Cloud Region A
Provider B → same Cloud Region A
```

Ein Failover zwischen beiden schützt nicht vor Ausfall der gemeinsamen Failure Domain.

## Redundancy

Für kritische Workloads kann ein Contract Redundanz verlangen.

Beispiel:

```text
redundancy {
    replicas = 2
    distinctFailureDomains = true
}
```

NovaOS muss dann Provider aus unterschiedlichen Failure Domains wählen können.

## Quorum

Verteilte zustandsbehaftete Services dürfen Quorum-basierte Mechanismen verwenden.

Die genaue Semantik gehört in separate Distributed-System-NPSPECs.

Location Transparency darf Quorum- und Konsistenzanforderungen nicht verstecken.

## Availability

Provider sollen ihre Verfügbarkeitsklasse beschreiben können.

Beispiel:

```text
Availability {
    class = BestEffort
}
```

oder:

```text
Availability {
    class = Guaranteed
}
```

Eine historische hohe Verfügbarkeit ist nicht automatisch eine harte Garantie.

## Health

NovaOS darf Provider Health beobachten.

Mögliche Zustände:

```text
Healthy
Degraded
Unstable
Unavailable
Unknown
```

Health beeinflusst die Providerwahl.

## Circuit Breaker

Wiederholt fehlschlagende Remote Provider können temporär aus der Auswahl entfernt werden.

Beispiel:

```text
Failures exceed threshold
        ↓
Circuit Open
        ↓
Provider temporarily unavailable
```

Dadurch werden wiederholte teure Timeouts vermieden.

## Hedged Requests

Für bestimmte idempotente und latenzkritische Operationen darf NovaOS Hedged Requests verwenden.

Beispiel:

```text
Request → Provider A

after threshold:

Request → Provider B
```

Das erste gültige Ergebnis wird verwendet.

Dies darf nur erfolgen, wenn:

- zusätzliche Ressourcen zulässig sind
- Operation semantisch sicher duplizierbar ist
- Cost Budget dies erlaubt

## Speculative Execution

Location Planning darf spekulative Ausführung verwenden.

Beispiel:

```text
Local CPU
+
Remote GPU
```

parallel.

Das schnellere Ergebnis kann verwendet werden.

Diese Technik darf nicht für mutierende Operationen verwendet werden, sofern keine geeignete Transaktionssemantik existiert.

## Deterministic Mode

Deterministic Mode muss Location Planning einschränken können.

Mögliche Regeln:

```text
Fixed Provider
Fixed Location
No Adaptive Migration
No Hedged Requests
No Dynamic Failover
```

wenn diese Maßnahmen für reproduzierbare Ausführung notwendig sind.

## Realtime

Hard-Realtime-Anforderungen sind über ungarantierte Remote-Netzwerke im Allgemeinen nicht zuverlässig erfüllbar.

Ein Hard-Realtime-Contract darf einen Remote Provider nur akzeptieren, wenn der gesamte Kommunikations- und Ausführungspfad die benötigten Garantien tatsächlich bereitstellt.

## Latency Guarantees

Gemessene durchschnittliche Latenz ist keine Garantie.

NovaOS muss unterscheiden zwischen:

```text
ObservedLatency
PredictedLatency
GuaranteedLatency
```

Ein Hard Contract darf nur auf einer geeigneten Garantie beruhen.

## Safety

Safety-kritische Funktionen dürfen Remote Locations nur verwenden, wenn:

- Failure Semantics
- Timing
- Trust
- Redundancy
- Recovery

den Safety-Anforderungen entsprechen.

Eine entfernte Verbindung darf nicht ohne entsprechende Absicherung zum Single Point of Failure einer Safety-Funktion werden.

## Resource Reservation

Remote Provider können Reservations unterstützen.

Beispiel:

```text
Reserve {
    provider = EdgeGPU0
    duration = 10 s
}
```

Eine lokale Reservation und eine Remote Reservation besitzen möglicherweise unterschiedliche Garantien.

Diese müssen explizit beschrieben werden.

## Admission Control

Remote Workloads mit harten Anforderungen müssen Admission Control durchlaufen können.

Dabei sind mindestens zu prüfen:

```text
Remote Compute Capacity
Network Capacity
Deadline
Trust
Sovereignty
Cost Budget
Failure Requirements
```

## Structured Concurrency

Remote Tasks müssen in Structured Concurrency integrierbar sein.

Beispiel:

```text
Parent Task
    ├── Local Task
    └── Remote Task
```

Wenn der Parent gecancelt wird, soll auch der Remote Task gecancelt werden können.

## Remote Cancellation

Cancellation über Netzwerk ist nicht zwangsläufig sofort.

Ein Remote Task kann bereits:

- ausgeführt
- abgeschlossen
- disconnected

sein.

Die Cancellation-Semantik muss deshalb explizit definiert werden.

## Orphaned Remote Tasks

Wenn die Verbindung zum Caller verloren geht, kann ein Remote Task verwaisen.

NovaOS muss Policies für solche Tasks unterstützen.

Beispiele:

```text
CancelOnDisconnect
ContinueUntilLeaseExpires
ContinueTransaction
PersistUntilResultRetrieved
```

## Leases

Remote Execution SOLL Leases verwenden können.

Beispiel:

```text
ExecutionLease {
    duration = 30 s
}
```

Wenn der Caller verschwindet, kann die Remote-Ausführung nach Ablauf der Lease automatisch beendet werden.

## Resource Cleanup

Remote Ressourcen müssen nach:

- Completion
- Cancellation
- Timeout
- Lease Expiration
- Node Failure

soweit möglich freigegeben werden.

## Self-Healing

Nova.Resilience muss Location Failures erkennen und behandeln können.

Beispiel:

```text
Remote Provider Failure
        ↓
Classify
        ↓
Check Contract
        ↓
Select Fallback
        ↓
Rebind Capability
        ↓
Retry if safe
        ↓
Verify
```

## Recovery

Recovery muss den Unterschied zwischen:

```text
Local Failure
```

und:

```text
Remote Failure
```

berücksichtigen.

Ein Remote Failure darf nicht automatisch einen lokalen Reboot auslösen.

## Architecture Introspection

Location-Entscheidungen müssen introspektierbar sein.

Beispiel:

```text
Capability:
    de.nova.ai.inference

Selected Provider:
    NPU0

Location:
    Local

Reason:
    latency = 8 ms
    data already local
    energy = low

Rejected Provider:
    EdgeGPU01

Reason:
    transfer cost = 24 ms
```

## Remote Introspection

Für Remote-Ausführung sollen mindestens sichtbar sein können:

```text
Provider Identity
Location
Trust
Current Health
Latency
Bandwidth
Failure Domain
Sovereignty Region
Execution State
```

sofern Security- und Privacy-Policies dies erlauben.

## Decision Tracing

Automatische Location-Entscheidungen SOLLEN erklärbar sein.

Beispiel:

```text
Selected:
    EdgeNode01

Reasons:
    Local GPU unavailable
    Deadline <= 30 ms
    Edge predicted total latency = 18 ms
    Sovereignty = Germany
    Trust = Verified
```

## Observability

Location-bezogene Observability kann Metriken umfassen:

- Local Executions
- Remote Executions
- Provider Selection
- Provider Rejection
- Network Latency
- Transfer Volume
- Serialization Cost
- Encryption Cost
- Remote Execution Time
- Timeout Count
- Retry Count
- Failover Count
- Unknown Outcomes
- Remote Cancellation
- Provider Health
- Monetary Cost

## Audit

Security-relevante Remote-Ausführungen SOLLEN auditierbar sein.

Beispiel:

```text
Data:
    MedicalImage

Provider:
    EdgeMedicalAI01

Location:
    Germany

Trust:
    Verified

Result:
    Success
```

Auditdaten müssen selbst Datenschutz- und Retention-Policies unterliegen.

## Developer API

Anwendungs-APIs sollen nicht unnötig zwischen lokalen und entfernten Providern unterscheiden.

Beispiel:

```text
result = Capability.Execute(input)
```

statt separater APIs wie:

```text
ExecuteLocal()
ExecuteRemote()
ExecuteCloud()
```

wenn die Semantik identisch ist.

## Explizite Kontrolle

Wenn der Entwickler die Location bewusst kontrollieren muss, soll dies über Contracts oder Options erfolgen.

Beispiel:

```text
Execute(
    input,
    location = LocalOnly
)
```

Die semantische Capability bleibt dieselbe.

## Error API

Die Fehler-API muss Location-bezogene Fehler ausdrücken können.

Beispiel:

```text
ExecutionError {
    category = LocationFailure
    reason   = Timeout
    provider = EdgeNode01
    retryable = true
}
```

## Error Causality

Fehler sollen ihre Ursache erhalten.

Beispiel:

```text
CapabilityExecutionFailed
    caused by NetworkTimeout
```

anstatt alle Fehler auf:

```text
OperationFailed
```

zu reduzieren.

## Retryability

Fehler können klassifiziert werden als:

```text
Retryable
NonRetryable
Unknown
```

Die Klassifikation darf nicht allein anhand des Transportfehlers erfolgen.

Auch die Semantik der Operation muss berücksichtigt werden.

## Error Aggregation

Wenn mehrere Provider versucht wurden, soll NovaOS die Failure Chain erhalten können.

Beispiel:

```text
Provider A:
    Timeout

Provider B:
    TrustRejected

Provider C:
    ResourceUnavailable
```

Dies verbessert Debugging und Decision Tracing.

## Error Transparency

Location Transparency bedeutet nicht Error Transparency im Sinne des Versteckens aller Remote-Fehler.

Das Prinzip lautet:

```text
Hide unnecessary location-specific API complexity,
not meaningful failure semantics.
```

## Performance Transparency

Ebenso bedeutet Location Transparency nicht:

```text
All locations have equivalent performance.
```

Performanceunterschiede müssen Planner und Introspection sichtbar bleiben.

## Security Transparency

Location darf niemals Security- oder Trust-Unterschiede unsichtbar machen.

Ein Remote Provider wird nicht allein deshalb vertrauenswürdig, weil er dieselbe Capability implementiert.

## Persistence

Persistente Daten können location-transparent adressiert werden.

Die tatsächliche Storage Location bleibt jedoch für:

- Sovereignty
- Availability
- Cost
- Failure Recovery

relevant.

## Object Identity

Ein Datenobjekt kann dieselbe logische Identität über mehrere Locations behalten.

Beispiel:

```text
Object42
    ├── Local Cache
    ├── Edge Replica
    └── Persistent Primary
```

Die Replikate müssen Version und Konsistenzstatus beschreiben können.

## Versioning

Location-transparent replizierte Objekte benötigen Versionierungsinformationen.

Beispiel:

```text
Object42 {
    version = 17
}
```

Dadurch können stale Replicas erkannt werden.

## Stale Data

Ein lokaler Cache kann schneller, aber veraltet sein.

Der Execution Contract muss bestimmen können, ob stale Data akzeptabel ist.

Beispiel:

```text
freshness {
    maximumAge = 5 s
}
```

## Freshness

Freshness ist damit ein möglicher semantischer Contract für verteilte Daten.

Ein Provider oder Cache, der die Freshness-Anforderung nicht erfüllt, ist ungültig.

## Transactions

Verteilte Transaktionen dürfen unterstützt werden.

Sie sind jedoch nicht für jede Operation verpflichtend.

NovaOS soll je nach Semantik zwischen:

- Local Transaction
- Distributed Transaction
- Saga
- Compensating Action
- Idempotent Operation

wählen können.

## Distributed Transaction Cost

Verteilte Transaktionen besitzen zusätzliche Kosten und Failure Modes.

Sie dürfen nicht allein zur Erzeugung scheinbar perfekter Location Transparency erzwungen werden.

## Sagas

Lang laufende verteilte Operationen dürfen Saga-artige Modelle verwenden.

Beispiel:

```text
Step A
    ↓
Step B
    ↓
Step C
```

Bei Fehler:

```text
Compensate B
Compensate A
```

Die genaue Semantik wird separat spezifiziert.

## Hot Replacement

Provider können während des Betriebs ersetzt werden.

Beispiel:

```text
Provider A @ Node01
        ↓
Provider B @ Node02
```

Location Transparency erleichtert Rebinding, da die semantische Capability nicht an den ursprünglichen Provider gebunden ist.

## Live Evolution

Neue Provider-Versionen können parallel auf unterschiedlichen Locations existieren.

Die Auswahl kann berücksichtigen:

- Version
- Compatibility
- Trust
- Resource Profile
- Location
- Execution Contract

## Provider Discovery

NovaOS benötigt einen Mechanismus zur Provider Discovery.

Provider können:

- lokal registriert
- dynamisch erkannt
- über vertrauenswürdige Nodes angeboten
- über konfigurierte Remote Provider verfügbar

sein.

Discovery allein bedeutet noch keine Autorisierung zur Verwendung.

## Provider Registry

Die Registry beschreibt unter anderem:

```text
Capability
Provider
Version
Location
Trust
Resource Profile
Availability
```

Die Registry ist nicht zwingend ein einzelner zentraler Dienst.

## Discovery Trust

Remote Discovery Results müssen verifizierbar sein.

Ein nicht vertrauenswürdiger Netzwerkteilnehmer darf keine beliebigen Provider in die Auswahl einschleusen.

## Naming

Capability Naming bleibt location-unabhängig.

Beispiel:

```text
de.nova.image.filter.gaussian
```

ist korrekt.

Ein Name wie:

```text
de.nova.image.filter.gaussian.cloud
```

soll nicht erforderlich sein, wenn `cloud` lediglich die Ausführungslocation beschreibt.

Location ist Provider-Metadatum, nicht Capability-Identität.

## Stable Semantics

Alle Provider derselben Capability müssen die spezifizierte semantische Funktion erfüllen.

Location darf keine versteckten semantischen Unterschiede erzeugen.

Wenn ein Remote Provider andere Semantik besitzt, handelt es sich entweder um:

- einen anderen Provider-Modus,
- eine andere Capability-Version,
- oder eine andere Capability.

## Approximate Providers

Approximate Computing darf unterstützt werden.

Ein Provider kann beispielsweise:

```text
accuracy = Approximate
```

besitzen.

Dies ist jedoch eine semantische Eigenschaft und darf nicht allein wegen seiner Location implizit angenommen werden.

## Provider Equivalence

Provider müssen nicht identische:

- Performance
- Hardware
- Implementierung
- Energieeffizienz

besitzen.

Sie müssen jedoch die für die Capability und den Contract erforderliche Semantik erfüllen.

## Locality Hints

Anwendungen dürfen Locality Hints bereitstellen.

Beispiel:

```text
PreferDataLocality
PreferSameDevice
PreferSameNUMANode
```

Hints sind keine Hard Constraints.

## Affinity

Ein Workload kann Location Affinity besitzen.

Beispiel:

```text
Affinity {
    data = Dataset42
}
```

oder:

```text
Affinity {
    device = GPU0
}
```

Der Planner darf diese Information zur Optimierung verwenden.

## Anti-Affinity

Für Redundanz können Workloads Anti-Affinity besitzen.

Beispiel:

```text
Replica A
Replica B

Constraint:
    DifferentFailureDomains
```

Damit werden beide Replikate nicht auf derselben Maschine platziert.

## Resource Topology

Location Planning muss mit Resource Topology zusammenarbeiten.

Innerhalb einer Maschine können relevante Locations beispielsweise sein:

```text
NUMA Node
CPU Package
GPU
NPU
PCIe Root Complex
Memory Domain
```

Location Transparency gilt deshalb nicht nur für Netzwerkverteilung.

## NUMA

Auch NUMA ist eine Form von Location.

Ein Task auf:

```text
NUMA Node 0
```

mit Speicher auf:

```text
NUMA Node 1
```

besitzt andere Kosten als lokale Speicherzugriffe.

Der Planner SOLL diese Unterschiede berücksichtigen.

## Device Topology

Datenbewegung zwischen:

```text
GPU0
```

und:

```text
GPU1
```

kann abhängig von:

- PCIe
- NVLink-artigen Interconnects
- Shared Memory
- NUMA

unterschiedliche Kosten besitzen.

Diese Topologie ist Teil der Location Cost.

## Locality Levels

NovaOS kann Locality Levels verwenden.

Beispiel:

```text
L0 = Same Core
L1 = Same NUMA Node
L2 = Same Machine
L3 = Same LAN
L4 = Same Site
L5 = Edge
L6 = Remote Region
```

Die genaue Klassifikation wird durch NPSPEC definiert.

## Scheduling

Scheduler und Execution Planner sollen Location gemeinsam berücksichtigen.

Der CPU Scheduler entscheidet beispielsweise über lokale CPU-Ausführung.

Der Execution Planner entscheidet darüber, welche Execution Domain beziehungsweise welcher Provider verwendet wird.

## Mechanism / Policy Separation

Mechanismen für:

- Remote Execution
- Data Transfer
- Discovery
- Capability Delegation

werden von Policies getrennt.

Policies bestimmen beispielsweise:

- ob Cloud erlaubt ist
- welche Region zulässig ist
- welcher Provider bevorzugt wird
- welche Kosten akzeptabel sind

## Declarative System Model

Location Policies und Providerzuordnungen sind Teil des deklarativen Systemmodells.

Beispiel:

```text
CapabilityPolicy {
    capability = de.nova.ai.inference

    location {
        prefer = Local
        allow  = TrustedEdge
    }
}
```

NovaOS kann den tatsächlichen Zustand mit dem gewünschten Zustand vergleichen.

## Desired State

Ein deklarativer Desired State kann beispielsweise lauten:

```text
Service {
    replicas = 2

    placement {
        distinctFailureDomains = true
        region = Germany
    }
}
```

Das System versucht diesen Zustand aufrechtzuerhalten.

## Reconciliation

Wenn ein Node ausfällt:

```text
Desired:
    2 replicas

Actual:
    1 replica
```

kann der Reconciler einen neuen Provider beziehungsweise eine neue Instanz starten.

## Architecture Introspection

NovaOS soll jederzeit beantworten können:

```text
Wo wird diese Capability ausgeführt?
```

```text
Wo befinden sich die Daten?
```

```text
Warum wurde diese Location gewählt?
```

```text
Welche Kosten entstehen?
```

```text
Welche Failure Domain wird verwendet?
```

```text
Welche Sovereignty Policy gilt?
```

## Explainability

Automatische Location Transparency darf nicht zu unsichtbarer Magie werden.

Entscheidungen müssen bei Bedarf erklärbar sein.

Beispiel:

```text
Remote provider selected because:

Local GPU:
    unavailable

Local CPU:
    predicted deadline miss

Edge GPU:
    predicted latency = 18 ms
    sovereignty = compliant
    trust = verified
```

## Normative Festlegungen

1. NovaOS MUSS Location Transparency als systemweites Architekturprinzip unterstützen.

2. Location Transparency MUSS semantische Identität von physischer beziehungsweise logischer Ausführungslocation trennen.

3. Capability-Identitäten DÜRFEN NICHT von einer konkreten Execution Location abhängig sein, sofern Location nicht selbst Bestandteil der Capability-Semantik ist.

4. Mehrere Provider DÜRFEN dieselbe Capability an unterschiedlichen Locations bereitstellen.

5. Location Transparency DARF relevante Ausführungskosten NICHT vor Planner, Resource Economy oder Introspection verbergen.

6. Location Transparency DARF relevante Failure Modes NICHT semantisch verschleiern.

7. NovaOS MUSS zwischen Semantic Failure und Location beziehungsweise Transport Failure unterscheiden können.

8. Remote-Ausführung MUSS zusätzliche Fehler wie Timeout, Network Failure und Remote Provider Failure darstellen können.

9. Der Zustand `UnknownOutcome` MUSS für geeignete entfernte mutierende Operationen darstellbar sein.

10. NovaOS DARF NICHT generell annehmen, dass ein Timeout bedeutet, dass eine Remote Operation nicht ausgeführt wurde.

11. Provider SOLLEN ihre Execution Location beschreiben können.

12. Datenobjekte SOLLEN ihre relevante Data Location beschreiben können.

13. Provider Location und Data Location MÜSSEN getrennt modellierbar sein.

14. NovaOS SOLL Compute-to-Data als Optimierung unterstützen.

15. NovaOS DARF Data-to-Compute verwenden, wenn dies insgesamt günstiger oder erforderlich ist.

16. Data Gravity SOLL bei Providerentscheidungen berücksichtigt werden können.

17. `Nova.ExecutionContract` MUSS Location Constraints ausdrücken können.

18. Execution Contracts MÜSSEN harte und weiche Location Constraints unterscheiden können.

19. Entwickler DÜRFEN eine Location explizit erzwingen, sofern übergeordnete Policies dies erlauben.

20. Ein Location Override DARF Security-, Safety-, Trust- oder Sovereignty-Constraints NICHT umgehen.

21. Location Policies MÜSSEN hierarchische Scopes unterstützen können.

22. Untergeordnete Location Policies DÜRFEN übergeordnete Hard Constraints NICHT abschwächen.

23. Providerwahl MUSS Hard Constraints vor Soft Optimization prüfen.

24. Data Sovereignty MUSS vor der Auswahl eines Remote Providers geprüft werden.

25. Data Residency und Execution Location MÜSSEN getrennt modellierbar sein.

26. Information-Flow-Policies MÜSSEN Location Transfers kontrollieren können.

27. Location allein DARF NICHT als Trust-Nachweis gelten.

28. Remote Provider MÜSSEN in Nova.Trust integrierbar sein.

29. Remote Provider MÜSSEN authentifizierbar sein.

30. Remote Attestation MUSS als optionales Trust Requirement unterstützt werden können.

31. Remote Capability Delegation MUSS Least Privilege verwenden.

32. Remote Capabilities SOLLEN attenuierbar sein.

33. Remote Capabilities MÜSSEN definierte Lifetimes besitzen können.

34. Remote Capability Revocation SOLL durch geeignete Mechanismen wie Leases, Epochs oder Revocation State unterstützt werden können.

35. Remote Kommunikation MUSS angemessene Integrity- und Authentication-Mechanismen unterstützen.

36. Vertrauliche Remote-Kommunikation MUSS Verschlüsselung unterstützen.

37. NovaOS DARF Confidential-Computing-Provider integrieren.

38. Cloud MUSS als optionale Execution Location behandelbar sein.

39. Fundamentale NovaOS-Funktionen DÜRFEN NICHT ohne explizite Architekturentscheidung zwingend von Cloudverfügbarkeit abhängen.

40. NovaOS MUSS Local-Only-Betrieb unterstützen können.

41. Remote Provider DÜRFEN lokale Fähigkeiten erweitern.

42. Location Costs MÜSSEN mindestens Latenz und Datenbewegung berücksichtigen können.

43. Netzwerkbandbreite MUSS als Resource Cost einer Remote-Ausführung berücksichtigt werden können.

44. Serialization Cost SOLL berücksichtigt werden können.

45. Encryption Cost SOLL berücksichtigt werden können.

46. Monetary Cost DARF als Provider Cost modelliert werden.

47. Execution Contracts DÜRFEN Monetary Cost Budgets definieren.

48. Hard Cost Budgets DÜRFEN NICHT automatisch überschritten werden.

49. Location Planning MUSS in die systemweite Resource Economy integrierbar sein.

50. Provider Resource Profiles SOLLEN Location-bezogene Kosten beschreiben können.

51. Dynamische Location Costs MÜSSEN aktualisierbar sein.

52. NovaOS DARF Location Costs prognostizieren.

53. Prediction Error DARF zur Verbesserung von Location-Cost-Modellen verwendet werden.

54. Vorhersagen DÜRFEN garantierte Eigenschaften NICHT ersetzen.

55. Gemessene Durchschnittslatenz DARF NICHT automatisch als harte Latenzgarantie behandelt werden.

56. NovaOS MUSS Observed, Predicted und Guaranteed Properties unterscheiden können.

57. Remote Operations SOLLEN definierte Timeout-Semantik besitzen.

58. Automatische Retries DÜRFEN nur durchgeführt werden, wenn die Operationssemantik dies erlaubt.

59. Idempotency MUSS für Retry-Entscheidungen beschreibbar sein.

60. Transaction IDs beziehungsweise vergleichbare Deduplication-Mechanismen SOLLEN für relevante mutierende Remote-Operationen unterstützt werden.

61. Retry Policies MÜSSEN übergeordnete Deadlines berücksichtigen.

62. Fallback Provider DÜRFEN nur verwendet werden, wenn sie weiterhin alle Hard Constraints erfüllen.

63. NovaOS SOLL Provider Failover unterstützen können.

64. Duplicate Execution MUSS bei Retry und Failover berücksichtigt werden.

65. Delivery Semantics wie AtMostOnce und AtLeastOnce MÜSSEN ausdrückbar sein können.

66. NovaOS DARF NICHT pauschal echtes ExactlyOnce über beliebige Failure Domains garantieren.

67. Effektiv einmalige Ausführung DARF über Idempotency, Deduplication und Transactions realisiert werden.

68. Network Partition MUSS als normaler möglicher Fehlerzustand verteilter Ausführung behandelt werden.

69. Partial Failure MUSS modellierbar sein.

70. Ein Remote Failure DARF NICHT automatisch als lokaler Systemfehler behandelt werden.

71. State Location und Code Location MÜSSEN getrennt modellierbar sein.

72. State Affinity SOLL bei Providerwahl berücksichtigt werden können.

73. Stateful Migration DARF unterstützt werden.

74. Stateful Migration MUSS State, Transactions, Trust und Compatibility berücksichtigen.

75. Caches MÜSSEN eine explizite Location besitzen können.

76. Data-Sovereignty-Regeln MÜSSEN auch für Cache Copies und Replicas gelten.

77. NovaOS DARF Daten replizieren.

78. Replizierte Daten MÜSSEN Version beziehungsweise Konsistenzzustand beschreiben können.

79. Konsistenzanforderungen MÜSSEN explizit ausdrückbar sein.

80. Location Transparency DARF Konsistenz-Trade-offs NICHT verstecken.

81. Freshness Requirements SOLLEN ausdrückbar sein.

82. Stale Replicas DÜRFEN nur verwendet werden, wenn der Contract dies erlaubt.

83. Remote Tasks MÜSSEN in Structured Concurrency integrierbar sein.

84. Parent Cancellation SOLL auf Remote Child Tasks propagierbar sein.

85. Remote Cancellation DARF NICHT als garantiert sofortige Beendigung interpretiert werden.

86. NovaOS MUSS Policies für orphaned Remote Tasks unterstützen können.

87. Remote Execution SOLL Leases unterstützen können.

88. Remote Ressourcen SOLLEN nach Completion, Cancellation, Timeout oder Lease Expiration freigegeben werden können.

89. Nova.Resilience MUSS Remote Provider Failures klassifizieren können.

90. Self-Healing DARF alternative Locations verwenden, wenn der Contract dies erlaubt.

91. Failure Domains MÜSSEN als Provider-Eigenschaft modellierbar sein.

92. Failure-Domain-Korrelation SOLL bei Redundanz berücksichtigt werden.

93. Contracts DÜRFEN distinct Failure Domains verlangen.

94. Provider Availability und Provider Health SOLLEN getrennt modellierbar sein.

95. Wiederholt fehlschlagende Provider DÜRFEN durch Circuit-Breaker-Mechanismen temporär ausgeschlossen werden.

96. Hedged Requests DÜRFEN nur für semantisch geeignete Operationen verwendet werden.

97. Speculative Remote Execution DARF Hard Cost-, Security-, Sovereignty- und Resource-Budgets NICHT verletzen.

98. Deterministic Mode MUSS dynamische Location Selection einschränken oder deaktivieren können.

99. Deterministic Mode MUSS feste Provider beziehungsweise Locations erzwingen können.

100. Hard-Realtime-Contracts DÜRFEN Remote Provider nur verwenden, wenn der gesamte Pfad die erforderlichen Garantien erfüllt.

101. Safety-kritische Funktionen DÜRFEN Remote Locations nur verwenden, wenn ihre Safety-Anforderungen erfüllt bleiben.

102. Remote Resource Reservations MÜSSEN von lokalen Reservations semantisch unterscheidbar sein können.

103. Admission Control MUSS bei Remote Hard Contracts Netzwerk- und Remote-Compute-Ressourcen berücksichtigen können.

104. Capability Naming MUSS grundsätzlich location-unabhängig bleiben.

105. Provider Location MUSS als Provider-Metadatum modellierbar sein.

106. Unterschiedliche Locations DÜRFEN NICHT implizit unterschiedliche Capability-Semantik erzeugen.

107. Provider derselben Capability MÜSSEN die geforderte semantische Funktion erfüllen.

108. Locality Hints DÜRFEN zur Optimierung verwendet werden.

109. Locality Hints DÜRFEN Hard Constraints NICHT überschreiben.

110. Affinity und Anti-Affinity SOLLEN unterstützt werden können.

111. NUMA MUSS als relevante Form von Location modellierbar sein.

112. Hardwaretopologie SOLL in Location Costs einfließen.

113. Location Planning MUSS mit der Resource Topology integrierbar sein.

114. Mechanismen für Location Transparency MÜSSEN von Location Policies getrennt werden.

115. Location Policies MÜSSEN im Declarative System Model ausdrückbar sein.

116. Desired-State-Platzierungsregeln SOLLEN durch Reconciliation aufrechterhalten werden können.

117. Provider Discovery MUSS Discovery von Autorisierung unterscheiden.

118. Ein entdeckter Provider DARF NICHT allein aufgrund seiner Discovery automatisch verwendet werden.

119. Remote Provider Discovery MUSS gegen Spoofing beziehungsweise unautorisierte Provider geschützt werden können.

120. Architecture Introspection MUSS Execution Location sichtbar machen können.

121. Architecture Introspection MUSS relevante Data Locations sichtbar machen können.

122. Automatische Location-Entscheidungen SOLLEN durch Decision Tracing erklärbar sein.

123. Remote Failure Chains SOLLEN für Diagnosezwecke erhalten werden können.

124. Location-bezogene Observability MUSS möglich sein.

125. Security-relevante Remote-Ausführungen SOLLEN auditierbar sein.

126. Auditdaten MÜSSEN selbst Privacy-, Access- und Retention-Policies unterliegen.

127. Entwickler-APIs SOLLEN lokale und entfernte Provider nicht unnötig als unterschiedliche semantische APIs darstellen.

128. Location-spezifische Fehler MÜSSEN dennoch erkennbar bleiben.

129. Location Transparency DARF Performanceunterschiede NICHT als nicht existent darstellen.

130. Location Transparency DARF Securityunterschiede NICHT als nicht existent darstellen.

131. Location Transparency DARF Data-Sovereignty-Unterschiede NICHT als nicht existent darstellen.

132. Location Transparency DARF Failure-Domain-Unterschiede NICHT als nicht existent darstellen.

133. Zero-Copy und Location Transparency MÜSSEN gemeinsam planbar sein.

134. Remote Transfers DÜRFEN notwendige Serialization-, Compression- und Encryption-Schritte verwenden.

135. Notwendige Remote-Datenkopien SIND mit dem Zero-Copy-Prinzip vereinbar, sofern sie begründet sind.

136. NovaOS SOLL Datenbewegung minimieren, statt Remote Compute allein anhand der Compute-Leistung auszuwählen.

137. Providerwahl MUSS die End-to-End-Kosten einer Operation berücksichtigen können.

138. KI-basierte oder adaptive Location Selection DARF NICHT Voraussetzung für fundamentale Systemkorrektheit sein.

139. Adaptive Location Selection DARF Security-, Safety-, Sovereignty-, Determinism- oder Hard-Realtime-Anforderungen NICHT verletzen.

140. Das System MUSS bei Ausfall adaptiver Location Policies auf konservative, sichere Policies zurückfallen können.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen werden weniger an konkrete Hardware gebunden
- Capabilities können lokale und entfernte Provider verwenden
- automatische Nutzung heterogener Hardware wird erleichtert
- Compute-to-Data wird möglich
- Datenlokalität kann systemweit optimiert werden
- Edge- und Cloud-Ressourcen können integriert werden
- Multi-Device-Szenarien werden möglich
- Provider können dynamisch ersetzt werden
- Failover wird erleichtert
- Hot Replacement wird vereinfacht
- Capability APIs bleiben stabiler
- Resource Economy kann lokale und entfernte Ressourcen gemeinsam betrachten
- Data Sovereignty bleibt kontrollierbar
- Trust bleibt explizit
- Remote Failure Modes bleiben sichtbar
- Entwickler können Location explizit überschreiben
- automatische Entscheidungen bleiben introspektierbar
- NovaOS bleibt grundsätzlich local-first und cloud-unabhängig

### Negative Konsequenzen

- Providerplanung wird erheblich komplexer
- Remote Execution benötigt zusätzliche Fehlersemantik
- Timeouts können Unknown Outcomes erzeugen
- Retry-Semantik muss Operationssemantik berücksichtigen
- verteilte Zustände benötigen Konsistenzmodelle
- Capability Delegation über Netzwerk ist komplexer
- Remote Revocation ist schwieriger als lokale Revocation
- Data Sovereignty muss bei jeder relevanten Datenbewegung geprüft werden
- Kostenmodelle können dynamisch und ungenau sein
- Provider Discovery benötigt Trust-Mechanismen
- Debugging verteilter Ausführung ist komplexer
- Migration und Failover können State-Transfer benötigen
- Remote-Ausführung kann zusätzliche Energie-, Netzwerk- und monetäre Kosten erzeugen
- vollständige Location Transparency ist bewusst nicht möglich

## Verworfene Alternativen

### Vollständig sichtbare Location in jeder Anwendungs-API

Nicht übernommen.

Eine Architektur wie:

```text
RunOnCPU()
RunOnGPU()
RunOnRemoteGPU()
RunOnCloud()
```

würde Anwendungen stark an konkrete Ausführungsorte koppeln.

Dies erschwert:

- Hardwareevolution
- Providerwechsel
- automatische Optimierung
- Multi-Device
- Edge Computing

NovaOS trennt deshalb Capability-Semantik von Provider Location.

### Vollständige Location Transparency

Nicht übernommen.

Ein Modell, das lokale und entfernte Operationen so behandelt, als hätten sie identische:

- Latenz
- Fehler
- Kosten
- Trust
- Konsistenz

wäre irreführend und gefährlich.

NovaOS verwendet deshalb:

```text
Semantic Transparency
+
Operational Visibility
```

### Cloud-First-Architektur

Nicht übernommen.

NovaOS soll nicht grundsätzlich von einem Cloud Provider abhängig sein.

Cloud-Ressourcen sind optionale Provider innerhalb der allgemeinen Execution Architecture.

### Local-Only-Architektur

Nicht übernommen.

Eine ausschließlich lokale Architektur würde:

- Edge Compute
- Remote Accelerators
- Multi-Device
- Cluster Compute
- verteilte Services

unnötig ausschließen.

Local-Only bleibt jedoch als Policy möglich.

### Remote-Ausführung als separate Capability

Nicht generell übernommen.

Eine Capability soll nicht allein deshalb eine andere Identität erhalten, weil sie remote ausgeführt wird.

Wenn die semantische Funktion identisch ist, bleibt die Capability identisch.

### Automatische Retries für alle Remote-Fehler

Nicht übernommen.

Bei nicht-idempotenten Operationen können automatische Retries doppelte Seiteneffekte erzeugen.

Retry hängt deshalb von:

- Operationssemantik
- Transaction Model
- Idempotency
- Deadline

ab.

### Exactly-Once als allgemeine Remote-Garantie

Nicht übernommen.

Über beliebige Failure Domains ist echtes Exactly-Once nicht allgemein zuverlässig garantierbar.

NovaOS verwendet explizite Delivery- und Transaction-Semantik.

### Performancebasierte Providerwahl ohne Sovereignty und Trust

Nicht übernommen.

Der schnellste Provider ist nicht automatisch ein zulässiger Provider.

Hard Constraints werden vor Performanceoptimierung geprüft.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip
- ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem
- ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-EXECUTION-0001_Nova_ExecutionContract
- ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten
- ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell
- ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur

## Zugehörige NPSPECs

- NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001
- NPSPEC-LOCATION-MODEL-0001
- NPSPEC-LOCATION-IDENTITY-0001
- NPSPEC-LOCATION-HIERARCHY-0001
- NPSPEC-LOCATION-LOCALITY-0001
- NPSPEC-LOCATION-POLICY-0001
- NPSPEC-LOCATION-CONTRACT-0001
- NPSPEC-LOCATION-AFFINITY-0001
- NPSPEC-LOCATION-FAILUREDOMAIN-0001
- NPSPEC-LOCATION-COST-0001
- NPSPEC-LOCATION-DATAGRAVITY-0001
- NPSPEC-LOCATION-COMPUTETODATA-0001
- NPSPEC-LOCATION-MIGRATION-0001
- NPSPEC-LOCATION-INTROSPECTION-0001
- NPSPEC-LOCATION-DECISIONTRACE-0001
- NPSPEC-EXECUTION-REMOTE-0001
- NPSPEC-EXECUTION-LOCATION-0001
- NPSPEC-EXECUTION-DELIVERY-0001
- NPSPEC-EXECUTION-RETRY-0001
- NPSPEC-EXECUTION-FAILOVER-0001
- NPSPEC-EXECUTION-LEASE-0001
- NPSPEC-EXECUTION-CANCELLATION-0001
- NPSPEC-PROVIDER-DISCOVERY-0001
- NPSPEC-PROVIDER-LOCATION-0001
- NPSPEC-PROVIDER-HEALTH-0001
- NPSPEC-PROVIDER-FAILOVER-0001
- NPSPEC-DATA-LOCATION-0001
- NPSPEC-DATA-RESIDENCY-0001
- NPSPEC-DATA-REPLICATION-0001
- NPSPEC-DATA-CONSISTENCY-0001
- NPSPEC-DATA-FRESHNESS-0001
- NPSPEC-DATA-SOVEREIGNTY-0001
- NPSPEC-SECURITY-REMOTE-CAPABILITY-0001
- NPSPEC-TRUST-REMOTE-PROVIDER-0001
- NPSPEC-TRUST-ATTESTATION-0001
- NPSPEC-RESOURCE-REMOTE-0001
- NPSPEC-RESOURCE-DATAMOVEMENT-0001
- NPSPEC-OBSERVABILITY-REMOTEEXECUTION-0001

## Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch Tests für lokale, entfernte und teilweise ausgefallene Ausführungsszenarien überprüft werden.

### Provider-Selection-Tests

Zu prüfen sind:

- lokale Providerwahl
- Remote Providerwahl
- Preferred Location
- Forced Location
- LocalOnly
- Provider Fallback
- ungültige Provider aufgrund harter Constraints

### Cost-Tests

Zu prüfen sind:

- Latenz
- Bandbreite
- Serialization
- Encryption
- Data Movement
- Monetary Cost
- dynamische Kostenänderungen

Der Planner muss End-to-End-Kosten berücksichtigen.

### Data-Locality-Tests

Zu prüfen sind:

- Compute-to-Data
- Data-to-Compute
- große Datenobjekte
- lokale Caches
- Remote Replicas
- NUMA
- Device-local Memory

### Sovereignty-Tests

Zu prüfen sind:

- GermanyOnly
- EUOnly
- LocalOnly
- unerlaubte Remote Region
- Cache Copy in unerlaubter Region
- Replica in unerlaubter Region

Ein schnellerer Provider darf eine Sovereignty Policy nicht überschreiben.

### Trust-Tests

Zu prüfen sind:

- unbekannter Provider
- ungültige Identität
- ungültige Signatur
- fehlende Attestation
- abgelaufene Attestation
- Capability Attenuation
- Capability Revocation

### Network-Failure-Tests

Zu simulieren sind:

- Packet Loss
- High Latency
- Connection Loss
- Network Partition
- Node Failure
- Provider Failure
- Timeout

### Unknown-Outcome-Tests

Mutierende Operationen müssen bei verlorener Bestätigung korrekt zwischen:

```text
Failed
Succeeded
UnknownOutcome
```

unterscheiden können.

### Retry-Tests

Zu prüfen sind:

- idempotente Operation
- nicht-idempotente Operation
- Transaction ID
- Duplicate Request
- Retry Deadline
- Retry Budget

### Failover-Tests

Zu prüfen sind:

- Remote → Local
- Local → Remote
- Remote A → Remote B
- unterschiedliche Failure Domains
- gemeinsamer Failure Domain Failure

### Delivery-Semantics-Tests

Zu prüfen sind:

```text
AtMostOnce
AtLeastOnce
EffectivelyOnce
```

insbesondere unter:

- Timeout
- Retry
- Duplicate Delivery
- Connection Loss

### Structured-Concurrency-Tests

Zu prüfen sind:

- Parent Cancellation
- Remote Child Cancellation
- Lease Expiration
- Orphaned Remote Task
- Cleanup nach Caller Crash

### Resource-Tests

Zu prüfen sind:

- Network Budget
- Remote Compute Budget
- Cost Budget
- Deadline Capacity
- Reservation
- Admission Control

### Determinism-Tests

Im Deterministic Mode müssen:

- Provider
- Location
- Failover Policy
- Migration Policy

innerhalb der definierten Contract-Grenzen reproduzierbar sein.

### Realtime-Tests

Ein Remote Provider darf für Hard Realtime nur akzeptiert werden, wenn die geforderte End-to-End-Garantie nachweisbar erfüllt wird.

Durchschnittswerte allein dürfen den Test nicht bestehen.

### Security-Tests

Zu prüfen sind:

- Provider Spoofing
- Replay
- Capability Forgery
- Capability Escalation
- Unauthorized Delegation
- Man-in-the-Middle
- stale Lease
- revoked Provider
- unauthorized Data Location

### Introspection-Tests

NovaOS muss für eine Ausführung mindestens bestimmen können:

```text
Capability
Provider
Location
Data Location
Trust
Failure Domain
Resource Cost
Selection Reason
```

### Fault-Injection

Fault-Injection MUSS mindestens folgende Szenarien unterstützen:

```text
Provider disappears
Network partition
Connection reset
Remote crash
Local crash
Timeout after remote commit
Duplicate request
Delayed response
Stale response
Invalid response
Lease expiration
```

### Recovery-Tests

Nach Remote Failure muss geprüft werden:

- Capability Rebinding
- Safe Retry
- Alternative Provider
- Resource Cleanup
- Capability Cleanup
- Transaction Recovery
- State Consistency

## Ergebnis

NovaOS behandelt Location Transparency nicht als Illusion, dass alle Ausführungsorte identisch wären.

Das Architekturmodell lautet:

```text
Semantic Capability
        │
        ▼
Execution Contract
        │
        ▼
Valid Provider Set
        │
        ├── Security
        ├── Trust
        ├── Sovereignty
        ├── Determinism
        ├── Realtime
        └── Capability Requirements
        │
        ▼
Location-aware Planning
        │
        ├── Data Location
        ├── Compute Location
        ├── Latency
        ├── Bandwidth
        ├── Data Movement
        ├── Energy
        ├── Monetary Cost
        ├── Availability
        └── Failure Domain
        │
        ▼
Provider Selection
        │
        ▼
Execution
        │
        ├── Local
        ├── Device
        ├── Edge
        └── Remote
        │
        ▼
Observation / Failure
        │
        ▼
Decision Tracing / Recovery
```

Die Anwendung beschreibt primär:

```text
What should happen?
```

NovaOS entscheidet, sofern nicht explizit anders vorgegeben:

```text
Where should it happen?
```

Dabei bleiben die Auswirkungen dieser Entscheidung sichtbar.

Das verbindliche Prinzip lautet:

```text
Location-independent semantics.
Location-aware execution.
```

oder ausführlicher:

```text
Hide unnecessary location coupling,
but never hide meaningful cost,
trust, sovereignty or failure semantics.
```

Damit kann NovaOS lokale CPU-, GPU- und NPU-Ressourcen ebenso wie andere Geräte, Edge-Systeme und Remote-Provider über ein gemeinsames Capability- und Execution-Modell verwenden, ohne die fundamentalen Realitäten verteilter Systeme hinter einer falschen Abstraktion zu verbergen.