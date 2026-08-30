# ADR-ARCH-0003 – Deklaratives Systemmodell als Architekturgrundlage

## Status

Angenommen

## Kategorie

Systemarchitektur / Deklarative Systemarchitektur

## Kontext

NovaOS soll nicht primär als Sammlung imperativer Einzeloperationen aufgebaut werden, bei denen Anwendungen und Systemkomponenten jeden technischen Schritt selbst anstoßen und koordinieren müssen.

Stattdessen soll ein wesentlicher Teil der Systemarchitektur beschreiben können, **welcher Zustand oder welches Ergebnis gewünscht ist**, während NovaOS selbst bestimmt, wie dieser Zustand unter den aktuellen Bedingungen erreicht wird.

Das klassische imperative Modell sieht beispielsweise so aus:

```text
AllocateGPU()
CreateThread()
LoadModel()
SetPriority(HIGH)
ConnectNetwork()
ReserveMemory()
StartExecution()
```

Dabei muss der Aufrufer technische Details kennen und Entscheidungen treffen, die eigentlich besser durch das Betriebssystem getroffen werden können.

NovaOS verwendet stattdessen ein deklaratives Modell.

Beispiel:

```text
ExecutionRequirement {
    workload     = AIInference
    latency      <= 10 ms
    memory       <= 256 MiB
    energy       = PreferEfficient
    determinism  = Preferred
    dataLocation = LocalOnly
}
```

Der Aufrufer beschreibt Anforderungen.

NovaOS entscheidet anschließend unter Berücksichtigung von:

- verfügbarer Hardware
- Systemlast
- Ressourcenbudgets
- Trust
- Security
- Data Sovereignty
- Energiezustand
- Thermal Headroom
- Deadlines
- Determinism
- Provider Health
- Benutzerpräferenzen
- administrativen Policies

über die konkrete Umsetzung.

Das deklarative Systemmodell ist eng mit folgenden NovaOS-Konzepten verbunden:

- Desired State
- Actual State
- State Reconciliation
- Execution Contracts
- Resource Contracts
- Security Contracts
- Capability Contracts
- Semantic Types
- Policy Engines
- Constraint Solving
- Intent-Based APIs
- transaktionale Systemoperationen
- Self-Healing
- Architecture Introspection

## Entscheidung

NovaOS verwendet ein **deklaratives Systemmodell als grundlegendes Architekturprinzip**.

Anwendungen, Dienste und Systemkomponenten SOLLEN soweit sinnvoll nicht jeden technischen Umsetzungsschritt imperativ vorgeben.

Stattdessen beschreiben sie:

```text
Was soll gelten?
```

NovaOS bestimmt daraus:

```text
Wie kann dieser Zustand
unter den aktuellen Bedingungen
korrekt erreicht werden?
```

Das grundlegende Modell lautet:

```text
Desired State
      │
      ▼
Constraints
      │
      ▼
Policies
      │
      ▼
Planning
      │
      ▼
Execution
      │
      ▼
Actual State
      │
      ▼
Verification
      │
      ▼
Reconciliation
```

Das deklarative Modell ersetzt imperative APIs nicht vollständig.

Imperative Operationen bleiben dort zulässig, wo sie semantisch sinnvoll oder technisch notwendig sind.

Die native NovaOS-Architektur soll jedoch bevorzugt deklarative Anforderungen, Verträge und Zustände verwenden.

## Desired State

Der Desired State beschreibt den gewünschten Systemzustand.

Beispiele:

```text
Service {
    state = Running
}
```

```text
Package {
    version = 4.2
    state   = Installed
}
```

```text
Network {
    connectivity = Internet
    trust        >= Trusted
}
```

```text
StorageObject {
    replicas >= 2
    encrypted = True
}
```

Der Desired State beschreibt nicht zwingend die einzelnen Schritte, mit denen dieser Zustand erreicht wird.

## Actual State

Der Actual State beschreibt den tatsächlich beobachteten Zustand des Systems.

Beispiel:

```text
Desired:
    Service = Running

Actual:
    Service = Stopped
```

Diese Differenz erzeugt einen Reconciliation-Bedarf.

## State Reconciliation

NovaOS SOLL Desired State und Actual State kontinuierlich oder ereignisgesteuert vergleichen können.

Das Grundprinzip lautet:

```text
Desired State
      │
      ▼
Compare
      │
      ▼
Actual State
      │
      ▼
Difference
      │
      ▼
Reconciliation Plan
      │
      ▼
Execution
      │
      ▼
Verification
```

Das System versucht den Actual State mit dem Desired State in Einklang zu bringen.

## Reconciliation ist kein blindes Wiederholen

Reconciliation darf nicht bedeuten, dass dieselbe fehlgeschlagene Aktion unbegrenzt erneut ausgeführt wird.

Es müssen berücksichtigt werden:

- Retry Budgets
- Failure Classification
- Backoff
- Circuit Breaker
- Resource Availability
- Trust Changes
- Policy Changes
- Hardware Failure
- Recovery Scope
- User Intervention

Beispiel:

```text
Desired:
    GPUProvider = Active

Actual:
    GPU = Failed

Result:
    Do not restart forever
```

Stattdessen könnte NovaOS entscheiden:

```text
Fallback to CPUProvider
Mark GPU degraded
Schedule hardware diagnostics
Notify user
```

## Deklarative Ressourcen

Systemressourcen SOLLEN deklarativ beschrieben werden können.

Beispiel:

```text
ResourceRequirement {
    cpu {
        minimum = 1 core
        preferred = 4 cores
    }

    memory {
        maximum = 2 GiB
    }

    gpu {
        preferred = True
    }

    energy {
        mode = PreferEfficient
    }
}
```

Das System entscheidet über die konkrete Ressourcenzuweisung.

## Deklarative Dienste

Dienste SOLLEN über Desired State beschrieben werden.

Beispiel:

```text
Service {
    identity = nova.media.indexer
    state    = Running

    restart {
        policy = OnFailure
    }
}
```

NovaOS kann selbst entscheiden:

- wann der Dienst gestartet wird
- auf welcher CPU er läuft
- welche Ressourcen er erhält
- in welcher Isolation Domain er läuft
- ob er nach einem Fehler neu gestartet wird

## On-Demand Services

Ein deklarierter Service muss nicht permanent aktiv sein.

Beispiel:

```text
Service {
    state = Available
    startup = OnDemand
}
```

Der Desired State kann damit zwischen:

```text
Installed
Available
Running
Required
```

unterscheiden.

Das verhindert die Gleichsetzung von:

```text
Soll verfügbar sein
```

und:

```text
Muss permanent laufen
```

## Deklarative Security

Sicherheitsanforderungen SOLLEN deklarativ formuliert werden können.

Beispiel:

```text
SecurityRequirement {
    trust      >= Verified
    isolation  >= Strong
    network    = Denied
}
```

Das System entscheidet anschließend beispielsweise zwischen:

- Process Domain
- Sandbox
- Driver Domain
- MicroVM
- Confidential VM

Die konkrete Isolationstechnik ist damit Mechanismus.

Die gewünschte Sicherheit ist Policy bzw. Constraint.

## Deklarative Netzwerkarchitektur

Netzwerkanforderungen können als Intent beschrieben werden.

Beispiel:

```text
NetworkIntent {
    destination = Service<MediaServer>

    requirements {
        encrypted   = Required
        latency     <= 30 ms
        sovereignty = GermanyOnly
    }
}
```

Die Anwendung muss nicht zwingend vorgeben:

```text
Interface = wlan0
Protocol  = TCP
Address   = 192.168.1.20
```

NovaOS darf selbst geeignete:

- Interfaces
- Routen
- Transportprotokolle
- Multipath-Strategien
- VPN-Provider

auswählen.

## Deklarativer Storage

Storage-Anforderungen SOLLEN über gewünschte Eigenschaften beschrieben werden können.

Beispiel:

```text
StorageRequirement {
    durability  = High
    encryption  = Required
    replicas    >= 2
    latency     <= 20 ms
}
```

Das System darf entscheiden:

- welches physische Medium verwendet wird
- ob repliziert wird
- ob komprimiert wird
- welches Storage Tier verwendet wird
- welche Verschlüsselungsimplementierung verwendet wird

## Deklarative Compute-Anforderungen

Compute-Workloads SOLLEN über Anforderungen beschrieben werden können.

Beispiel:

```text
ComputeRequirement {
    operation   = MatrixMultiply
    latency     <= 5 ms
    energy      = PreferLow
    determinism = Required
}
```

NovaOS kann danach einen geeigneten Provider wählen:

```text
CPU
GPU
NPU
DSP
FPGA
RemoteCompute
```

Harte Constraints müssen dabei eingehalten werden.

## Deklarative Capability-Anforderungen

Anwendungen sollen bevorzugt Capabilities statt konkreter Implementierungen anfordern.

Beispiel:

```text
RequireCapability {
    capability = de.nova.image.filter.gaussian
}
```

anstatt:

```text
LoadLibrary("gaussian_gpu_v4.dll")
```

NovaOS kann anschließend einen passenden Capability Provider auswählen.

## Providerwahl

Das deklarative Systemmodell trennt:

```text
Capability
```

von:

```text
Provider
```

Beispiel:

```text
Capability:
    nova.image.decode.jpeg
```

Mögliche Provider:

```text
SoftwareJPEGDecoder
SIMDJPEGDecoder
GPUJPEGDecoder
HardwareJPEGDecoder
```

Eine Anwendung beschreibt die benötigte Fähigkeit.

Das System bestimmt den Provider.

## Execution Contracts

Das deklarative Systemmodell wird durch `Nova.ExecutionContract` ergänzt.

Der Desired State beschreibt typischerweise den gewünschten Zustand.

Ein Execution Contract beschreibt Anforderungen an eine konkrete Ausführung.

Beispiel:

```text
ExecutionContract {
    input        = Image<RGBA>
    output       = Image<RGBA>

    latency      <= 16 ms
    memory       <= 128 MiB

    determinism  = Preferred
    sovereignty  = LocalOnly
    trust        >= Signed

    algorithm    = Automatic
}
```

Der Contract beschreibt nicht die konkrete technische Umsetzung.

## Intent-Based APIs

NovaOS SOLL Intent-Based APIs unterstützen.

Eine Intent-Based API beschreibt das Ziel einer Operation.

Beispiel:

```text
Render(scene)
```

anstatt:

```text
SelectGPU(0)
CreateCommandQueue()
AllocateBuffers()
CompileShaders()
SubmitCommands()
WaitFence()
```

Dies bedeutet nicht, dass Low-Level APIs verschwinden.

Für spezialisierte Anwendungen können weiterhin explizite APIs verfügbar sein.

Die native High-Level-Systemarchitektur soll jedoch Intent-basierte Operationen ermöglichen.

## Deklarativ und imperativ

Deklarative und imperative Modelle schließen sich nicht gegenseitig aus.

NovaOS verwendet beide.

Das bevorzugte Modell lautet:

```text
Declarative by default
Explicit imperative control when required
```

Beispiel:

```text
Automatic:
    Render(scene)
```

Explizit:

```text
Render(
    scene,
    provider = GPU0,
    algorithm = RasterizerV4
)
```

Ein explizites Override darf keine höher priorisierten Security-, Safety- oder Sovereignty-Constraints verletzen.

## Constraint Solving

Das deklarative Systemmodell kann mehrere Anforderungen gleichzeitig enthalten.

Beispiel:

```text
Requirements {
    latency      <= 10 ms
    memory       <= 256 MiB
    energy       = PreferLow
    sovereignty  = LocalOnly
    trust        >= Verified
}
```

NovaOS kann daraus ein Constraint-Problem erzeugen.

Mögliche Kandidaten:

```text
CPU0
GPU0
NPU0
CloudGPU
```

Wenn `CloudGPU` gegen `LocalOnly` verstößt, wird dieser Kandidat ausgeschlossen.

Unter den verbleibenden Kandidaten kann anschließend optimiert werden.

## Hard Constraints

Harte Constraints MÜSSEN erfüllt werden.

Beispiele:

- Security
- Safety
- Data Sovereignty
- Hard Realtime
- erforderliche Trust Levels
- notwendige Capabilities
- explizite Verbote
- Determinism Requirements

Kann kein gültiger Zustand erzeugt werden, MUSS NovaOS:

- die Operation ablehnen,
- eine explizit erlaubte Degradation verwenden,
- oder eine Intervention anfordern.

Das System DARF einen harten Constraint NICHT stillschweigend ignorieren.

## Soft Constraints

Weiche Anforderungen dienen zur Optimierung.

Beispiele:

- PreferLowEnergy
- PreferFast
- PreferLocal
- PreferSilent
- PreferLowCost

Falls nicht alle weichen Ziele gleichzeitig erreicht werden können, darf NovaOS eine gewichtete Entscheidung treffen.

## Deklarative Systemkonfiguration

Systemkonfiguration SOLL bevorzugt als gewünschter Zustand repräsentiert werden.

Beispiel:

```text
SystemConfiguration {
    firewall = Enabled
    remoteAccess = Disabled
    updateChannel = Stable
}
```

NovaOS ist dafür verantwortlich, den tatsächlichen Zustand mit dieser Konfiguration in Einklang zu bringen.

## Configuration Transactions

Mehrere Konfigurationsänderungen SOLLEN transaktional zusammengefasst werden können.

Beispiel:

```text
transaction {
    set Firewall.Enabled = true
    set RemoteAccess.Enabled = false
    set Network.Profile = Trusted
}
```

Entweder wird ein konsistenter neuer Zustand erzeugt oder der vorherige Zustand bleibt erhalten.

## System Transactions

Deklarative Zustandsänderungen SOLLEN mit dem transaktionalen NovaOS-Systemmodell kombinierbar sein.

Beispiel:

```text
DesiredChange {
    install Application
    enable Service
    migrate Data
    update Capability
}
```

NovaOS erzeugt daraus einen Plan.

Die Ausführung kann beispielsweise lauten:

```text
Acquire
    ↓
Verify
    ↓
Stage
    ↓
Apply
    ↓
Validate
    ↓
Activate
    ↓
Observe
    ↓
Commit
```

Bei einem Fehler kann ein Rollback erfolgen.

## State Machines

Deklarative Systemobjekte SOLLEN definierte State Machines verwenden.

Beispiel:

```text
Installed
    ↓
Configured
    ↓
Starting
    ↓
Running
    ↓
Stopping
    ↓
Stopped
```

Ungültige Zustandsübergänge sollen verhindert werden.

## State Versioning

Wichtige deklarative Zustände SOLLEN versioniert werden können.

Dadurch können:

- History
- Rollback
- Audit
- Reconciliation
- Recovery

unterstützt werden.

## State History

NovaOS SOLL nachvollziehen können, wie ein Systemzustand entstanden ist.

Beispiel:

```text
Version 120
    Firewall = Disabled

Version 121
    Firewall = Enabled

Reason:
    SecurityPolicyUpdate
```

## State Snapshots

Für geeignete Systembereiche SOLLEN State Snapshots erzeugt werden können.

Snapshots können als Recovery- oder Rollback-Punkte dienen.

Sie ersetzen nicht zwangsläufig Storage Snapshots.

Ein State Snapshot kann beispielsweise nur deklarative Systemobjekte und deren Versionen enthalten.

## Event Sourcing

Event Sourcing DARF dort verwendet werden, wo es semantisch sinnvoll ist.

NovaOS schreibt jedoch nicht vor, sämtliche Systemzustände ausschließlich aus vollständigen Event Logs zu rekonstruieren.

Event Sourcing ist eine mögliche Implementierungstechnik und kein universeller Zwang.

## Reconciliation Controller

Unterschiedliche Subsysteme können eigene Reconciliation Controller besitzen.

Beispiele:

```text
Service Reconciler
Network Reconciler
Storage Reconciler
Update Reconciler
Security Reconciler
Capability Reconciler
```

Diese verwenden gemeinsame Systemprinzipien, müssen aber nicht durch einen einzelnen globalen Controller implementiert werden.

## Kein zentraler Mega-Reconciler

NovaOS SOLL keinen einzelnen allwissenden globalen Reconciliation-Prozess voraussetzen.

Das würde:

- Skalierbarkeit reduzieren
- Ausfallwirkungen vergrößern
- starke Kopplung erzeugen
- Echtzeitpfade behindern

Stattdessen sollen Reconciliation-Aufgaben hierarchisch und subsystembezogen organisiert werden können.

## Ownership

Für deklarative Zustände MUSS klar sein, welche Komponente deren authoritative Desired State kontrolliert.

Mehrere Komponenten dürfen nicht unkontrolliert denselben Zustand schreiben.

Mögliche Modelle:

- Single Owner
- Hierarchical Ownership
- Transactional Multi-Owner
- Policy Composition

Ownership muss explizit sein.

## Conflict Resolution

Konflikte zwischen Desired States müssen erkannt werden.

Beispiel:

```text
Application:
    NetworkAccess = Required

SecurityPolicy:
    NetworkAccess = Forbidden
```

Die Security Policy besitzt in diesem Fall Vorrang.

Der resultierende Zustand lautet:

```text
NetworkAccess = Denied
```

Die Anwendung muss eine entsprechende Fehlermeldung oder Degradation erhalten.

## Declarative UI

Auch Benutzeroberflächen können deklarativ beschrieben werden.

NovaLang verwendet hierfür `.nui`.

Beispiel:

```text
Window {
    title = "Editor"

    Content {
        TextEditor {
            document = CurrentDocument
        }
    }
}
```

Das deklarative Systemmodell und die deklarative UI folgen damit demselben grundlegenden Architekturprinzip:

```text
Describe desired structure and semantics
instead of manually orchestrating every step.
```

## Capability-Driven UI

Anwendungsoberflächen können sich aus verfügbaren Capabilities zusammensetzen.

Der deklarative UI-Zustand kann damit dynamisch aus:

- geladenen Capabilities
- Dokumentkontext
- Benutzerkontext
- verfügbaren Providern
- Policies

abgeleitet werden.

## Resource Economy

Das deklarative Systemmodell integriert sich in die Nova Resource Economy.

Ein Workload beschreibt beispielsweise:

```text
ResourceRequirements {
    CPU     <= 20 %
    Memory  <= 512 MiB
    Energy  = PreferLow
}
```

NovaOS plant daraus eine zulässige Ressourcenzuweisung.

## Adaptive Systeme

Adaptive Systeme dürfen den Desired State nicht ohne definierte Autorität beliebig verändern.

Adaptive Optimierungen können beispielsweise beeinflussen:

- Providerwahl
- Cache-Größe
- Prefetching
- Scheduling
- Power Policy

Ein expliziter Desired State oder eine harte Policy bleibt jedoch bindend.

## Prediction Error

Adaptive Policies können Prediction Error als Lernsignal verwenden.

Beispiel:

```text
Prediction
    ↓
System Decision
    ↓
Actual User Decision
    ↓
Deviation
    ↓
Model Correction
```

Das gelernte Modell beeinflusst zukünftige Optimierungsentscheidungen.

Es verändert jedoch nicht automatisch harte Sicherheits- oder Systeminvarianten.

## Self-Healing

Das deklarative Modell bildet eine wesentliche Grundlage für Self-Healing.

NovaOS kennt:

```text
Desired State
```

und beobachtet:

```text
Actual State
```

Eine Abweichung kann deshalb automatisch erkannt werden.

Beispiel:

```text
Desired:
    StorageService = Running

Actual:
    StorageService = Failed
```

Der Reconciler kann einen Recovery Plan erzeugen.

## Recovery

Recovery verändert den Actual State wieder in Richtung eines gültigen Desired State.

Beispiel:

```text
Detect Failure
    ↓
Contain
    ↓
Restart Service
    ↓
Verify
    ↓
Reconcile
```

Der Desired State selbst muss dabei nicht verändert werden.

## Graceful Degradation

Kann der ursprüngliche Desired State nicht erreicht werden, darf eine explizit definierte Degradation verwendet werden.

Beispiel:

```text
Desired:
    GPUAcceleration = Required
```

Wenn GPUAcceleration tatsächlich `Required` ist, darf kein CPU-Fallback erfolgen.

Bei:

```text
GPUAcceleration = Preferred
```

kann dagegen ein CPU-Fallback zulässig sein.

Die deklarative Semantik muss deshalb zwischen:

- Required
- Preferred
- Optional
- Forbidden

unterscheiden können.

## Deterministic Mode

Im Deterministic Mode müssen deklarative Entscheidungen reproduzierbar planbar sein.

Dies kann bedeuten:

- feste Provider
- feste Algorithmen
- versionierte Policies
- eingefrorene Optimierungsmodelle
- feste Ressourcenpläne

Der Desired State allein reicht für vollständige Reproduzierbarkeit nicht aus.

Auch die relevanten Constraints und Policyversionen müssen bekannt sein.

## Architecture Introspection

NovaOS MUSS deklarative Zustände introspektierbar machen.

Beispiel:

```text
Object:
    nova.service.media.indexer

Desired State:
    Running

Actual State:
    Running

Provider:
    MediaIndexerV3

Isolation:
    ServiceDomain

Resources:
    CPU    = 4 %
    Memory = 128 MiB

Policy:
    BackgroundServicePolicyV2

Health:
    Healthy
```

## Explainability

Wenn Desired State und Actual State voneinander abweichen, SOLL NovaOS den Grund erklären können.

Beispiel:

```text
Desired:
    GPUProvider

Actual:
    CPUProvider

Reason:
    GPU temperature above policy threshold

Fallback:
    CPUProvider

Contract:
    GPU = Preferred
```

Bei:

```text
GPU = Required
```

würde die Operation dagegen fehlschlagen.

## Decision Tracing

Planungs- und Reconciliation-Entscheidungen SOLLEN über Decision Tracing nachvollziehbar sein.

Erfasst werden können:

- Desired State
- Actual State
- erkannte Abweichung
- relevante Constraints
- angewendete Policies
- verfügbare Kandidaten
- ausgewählter Plan
- ausgeführte Aktionen
- Verifikationsergebnis

Sensitive Daten müssen gemäß Security- und Privacy-Policies geschützt werden.

## Performance

Das deklarative Systemmodell darf kritische Fast Paths nicht unnötig verlangsamen.

Nicht jede einzelne Kerneloperation muss einen vollständigen Desired-State-Reconciliation-Prozess durchlaufen.

Das Modell unterscheidet deshalb zwischen:

```text
Control Plane
```

und:

```text
Fast Data / Execution Path
```

Beispiel:

```text
Desired State
    ↓
Policy / Planning
    ↓
Configuration
    ↓
Fast Runtime State
    ↓
High-Frequency Execution
```

Der deklarative Control Plane kann damit komplex sein, während der eigentliche Fast Path effizient bleibt.

## Control Plane und Data Plane

NovaOS SOLL deklarative Planung und hochfrequente Ausführung voneinander trennen können.

Beispiel Netzwerk:

```text
Control Plane:
    Desired connectivity
    Routing policy
    Security policy
    QoS policy

Data Plane:
    Packet forwarding
```

Beispiel Scheduling:

```text
Control Plane:
    Resource contract
    Scheduling policy
    CPU reservation

Execution Path:
    Enqueue
    Select
    Context switch
```

## Security

Nicht jede Anwendung darf beliebige Desired States setzen.

Deklarative Zustandsänderungen benötigen entsprechende Capabilities.

Beispiel:

```text
Capability:
    nova.service.media.configure
```

oder:

```text
Capability:
    nova.system.network.policy.modify
```

Read-, Modify- und Admin-Rechte können getrennt sein.

## Trust

Deklarative Ressourcen und Policies besitzen Identität und Provenance.

NovaOS muss unterscheiden können zwischen:

- System Policy
- Administrator Policy
- User Policy
- Application Request
- untrusted external declaration

Eine Deklaration wird nicht allein deshalb autoritativ, weil sie syntaktisch gültig ist.

## Fail-Safe Defaults

Wenn kein gültiger Desired State oder keine gültige Policy ermittelt werden kann, müssen kritische Subsysteme sichere Fallback-Zustände verwenden können.

Beispiel:

```text
Network Security Policy unavailable

Fallback:
    Deny untrusted inbound traffic
```

und nicht:

```text
Allow everything
```

## Transaktionale Reconciliation

Reconciliation-Aktionen SOLLEN transaktional ausgeführt werden, wenn mehrere zusammengehörige Änderungen einen konsistenten Zustand bilden.

Beispiel:

```text
Desired:
    ApplicationVersion = 4
    DatabaseSchema     = 7
```

Die Aktivierung darf nicht zu folgendem Zustand führen:

```text
ApplicationVersion = 4
DatabaseSchema     = 6
```

wenn diese Kombination inkompatibel ist.

## Idempotenz

Reconciliation-Aktionen SOLLEN soweit möglich idempotent sein.

Das wiederholte Anwenden derselben gewünschten Änderung sollte nicht zu unerwarteten zusätzlichen Seiteneffekten führen.

Beispiel:

```text
Ensure Service Running
```

soll nicht bei jeder Auswertung einen weiteren Serviceprozess erzeugen.

## Convergence

Deklarative Subsysteme SOLLEN auf einen stabilen gültigen Zustand konvergieren.

Ein Reconciler darf nicht dauerhaft zwischen zwei Zuständen oszillieren.

NovaOS soll solche Zustände erkennen können.

Beispiel:

```text
Policy A:
    Move workload to GPU

Policy B:
    Move workload to CPU
```

Ein solcher Konflikt muss durch Policy Composition oder Constraint Resolution behoben werden.

## Normative Festlegungen

1. NovaOS MUSS ein deklaratives Systemmodell als grundlegendes Architekturprinzip verwenden.

2. Das deklarative Modell MUSS Desired State und Actual State unterscheiden.

3. NovaOS MUSS Zustandsabweichungen grundsätzlich erkennen können.

4. Deklarative Subsysteme SOLLEN Reconciliation unterstützen.

5. Reconciliation DARF NICHT als unbegrenztes blindes Retry implementiert werden.

6. Reconciliation MUSS Failure-, Retry- und Recovery-Policies berücksichtigen können.

7. Anwendungen SOLLEN bevorzugt Anforderungen und Intents statt unnötiger technischer Umsetzungsschritte beschreiben.

8. Imperative APIs DÜRFEN weiterhin angeboten werden.

9. NovaOS SOLL `Declarative by default, explicit control when required` verwenden.

10. Harte Constraints MÜSSEN bei der Planung eingehalten werden.

11. Harte Security-Anforderungen DÜRFEN NICHT zugunsten einer Optimierung verletzt werden.

12. Data-Sovereignty-Anforderungen DÜRFEN NICHT stillschweigend verletzt werden.

13. Hard-Realtime-Anforderungen MÜSSEN bei deklarativer Planung berücksichtigt werden.

14. Determinism Requirements MÜSSEN die Menge zulässiger Ausführungspläne einschränken können.

15. Weiche Anforderungen DÜRFEN für Optimierungsentscheidungen verwendet werden.

16. NovaOS SOLL Required, Preferred, Optional und Forbidden semantisch unterscheiden können.

17. Execution Contracts SOLLEN deklarative Ausführungsanforderungen beschreiben.

18. Capability-Anforderungen SOLLEN unabhängig von konkreten Providern formulierbar sein.

19. NovaOS SOLL Provider automatisch auswählen können.

20. Explizite Provider-Overrides DÜRFEN unterstützt werden.

21. Ein Override DARF höher priorisierte harte Constraints NICHT verletzen.

22. Resource Requirements SOLLEN deklarativ formulierbar sein.

23. Netzwerk-Intents SOLLEN deklarativ formulierbar sein.

24. Storage Requirements SOLLEN deklarativ formulierbar sein.

25. Security Requirements SOLLEN deklarativ formulierbar sein.

26. Compute Requirements SOLLEN deklarativ formulierbar sein.

27. Systemkonfiguration SOLL bevorzugt als Desired State modelliert werden.

28. Zusammengehörige Zustandsänderungen SOLLEN transaktional aktiviert werden können.

29. Deklarative Objekte SOLLEN definierte Lifecycle State Machines besitzen.

30. Kritische Systemzustände SOLLEN versionierbar sein.

31. State Snapshots SOLLEN für geeignete Subsysteme unterstützt werden.

32. State History SOLL für Audit, Recovery und Introspection nutzbar sein.

33. Event Sourcing DARF eingesetzt werden, ist jedoch KEIN universeller Zwang.

34. Reconciliation SOLL subsystembezogen und hierarchisch implementierbar sein.

35. NovaOS SOLL keinen einzelnen zentralen Mega-Reconciler voraussetzen.

36. Für deklarative Zustände MUSS ein eindeutiges Ownership-Modell existieren.

37. Konflikte zwischen Desired States MÜSSEN erkannt und aufgelöst werden.

38. Höher priorisierte Security-, Safety- und Systempolicies MÜSSEN gegenüber untergeordneten Anwendungsanforderungen Vorrang besitzen.

39. Adaptive Optimierung DARF explizite harte Desired-State-Anforderungen NICHT überschreiben.

40. Prediction Error DARF für adaptive Optimierungsentscheidungen verwendet werden.

41. KI-basierte Planung DARF NICHT alleinige Grundlage fundamentaler Systemkorrektheit sein.

42. Self-Healing SOLL Desired State und Actual State zur Recovery verwenden können.

43. Graceful Degradation MUSS explizit durch die Semantik des Contracts oder der Policy erlaubt sein.

44. Deklarative Planung MUSS von hochfrequenten Fast Paths getrennt werden können.

45. Der Control Plane DARF komplexere Planung durchführen, ohne jede Fast-Path-Operation zu belasten.

46. Deklarative Zustandsänderungen MÜSSEN Capability-basiert autorisierbar sein.

47. Deklarationen MÜSSEN Trust- und Provenance-Informationen zugeordnet werden können.

48. Kritische Subsysteme SOLLEN Fail-Safe-Fallback-Zustände besitzen.

49. Reconciliation-Aktionen SOLLEN soweit möglich idempotent sein.

50. Deklarative Subsysteme SOLLEN auf einen stabilen Zustand konvergieren.

51. Reconciliation-Oszillationen SOLLEN erkannt werden können.

52. Desired State, Actual State, Policies und Planungsentscheidungen MÜSSEN über Architecture Introspection sichtbar gemacht werden können.

53. Automatische Planungsentscheidungen SOLLEN über Decision Tracing erklärbar sein.

54. Sensitive State- und Policyinformationen MÜSSEN gemäß Security- und Privacy-Policies geschützt werden.

## Konsequenzen

### Positive Konsequenzen

- geringere Kopplung zwischen Anforderungen und Implementierungen
- automatische Providerwahl
- bessere Hardwareunabhängigkeit
- einfachere Weiterentwicklung
- bessere Self-Healing-Fähigkeit
- transaktionale Systemänderungen
- kontrollierte adaptive Optimierung
- bessere Resource-Economy-Integration
- bessere Data-Sovereignty-Integration
- flexible Security- und Trust-Policies
- bessere Architecture Introspection
- nachvollziehbare Systementscheidungen
- weniger hardcodierte Implementierungsdetails in Anwendungen
- bessere Portabilität
- bessere Unterstützung zukünftiger Hardware
- vereinfachte Capability-Komposition
- klare Desired-/Actual-State-Semantik

### Negative Konsequenzen

- zusätzliche State- und Reconciliation-Infrastruktur
- komplexere Policy-Komposition
- höhere Anforderungen an State Ownership
- mögliche Konflikte zwischen Desired States
- Planungs- und Reconciliation-Fehler müssen robust behandelt werden
- zusätzliche Introspection- und Tracing-Daten
- deklarative Modelle können für sehr einfache Operationen unnötig komplex sein
- Fast Paths müssen bewusst vom Control Plane getrennt werden

## Verworfene Alternativen

### Rein imperatives Systemmodell

Nicht übernommen.

Ein rein imperatives Modell würde Anwendungen und Systemkomponenten dazu zwingen, zu viele technische Implementierungsentscheidungen selbst zu treffen.

Dies würde insbesondere:

- Hardwareabstraktion
- automatische Providerwahl
- adaptive Optimierung
- Self-Healing
- Resource Economy
- Data Sovereignty
- Hot Replacement

erschweren.

### Vollständig deklaratives System ohne imperative APIs

Nicht übernommen.

Bestimmte Low-Level-, Realtime-, Debugging- oder Spezialanwendungen benötigen explizite Kontrolle.

NovaOS erlaubt deshalb weiterhin imperative APIs.

### Zentraler globaler Reconciliation Controller

Nicht übernommen.

Ein einzelner globaler Controller würde eine unnötig große Failure Domain und starke Kopplung erzeugen.

### Automatische Reconciliation ohne Grenzen

Nicht übernommen.

Unbegrenzte automatische Recovery- und Retry-Schleifen könnten:

- Ressourcen verschwenden
- Hardware beschädigen
- Fehler verschleiern
- Recovery Storms erzeugen

Reconciliation muss deshalb durch Policies und Budgets begrenzt werden.

### KI-gesteuertes Desired-State-System ohne deterministische Regeln

Nicht übernommen.

KI darf Planung optimieren, aber harte Constraints und Systeminvarianten müssen deterministisch überprüfbar bleiben.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern
- ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-EXECUTION-0001_Nova_ExecutionContract
- ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten
- ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur
- ADR-STATE-0001_Globaler_System_State_Graph
- ADR-STATE-0002_Desired_und_Actual_State

## Zugehörige NPSPECs

- NPSPEC-ARCH-SYSTEMMODEL-0001
- NPSPEC-ARCH-EXECUTIONCONTRACT-0001
- NPSPEC-STATE-GLOBAL-0001
- NPSPEC-STATE-DESIRED-0001
- NPSPEC-STATE-ACTUAL-0001
- NPSPEC-STATE-MACHINE-0001
- NPSPEC-STATE-RECONCILIATION-0001
- NPSPEC-STATE-SNAPSHOT-0001
- NPSPEC-STATE-VERSIONING-0001
- NPSPEC-STATE-HISTORY-0001
- NPSPEC-STATE-ROLLBACK-0001
- NPSPEC-STATE-TRANSACTIONAL-0001
- NPSPEC-EXECUTION-CONTRACT-0001
- NPSPEC-POLICY-MODEL-0001
- NPSPEC-POLICY-COMPOSITION-0001
- NPSPEC-POLICY-CONSTRAINT-0001
- NPSPEC-POLICY-DECISION-0001
- NPSPEC-CAPABILITY-RESOLUTION-0001
- NPSPEC-RESOURCE-ECONOMY-0001
- NPSPEC-RESILIENCE-ARCH-0001
- NPSPEC-OBSERVABILITY-DECISION-0001
- NPSPEC-OBSERVABILITY-STATEGRAPH-0001
- NPSPEC-OBSERVABILITY-INTROSPECTION-0001

## Ergebnis

NovaOS verwendet ein deklaratives Systemmodell als zentrale Grundlage für Konfiguration, Ressourcenplanung, Capability-Auflösung, Compute, Netzwerk, Storage, Security, Updates, Recovery und adaptive Systemfunktionen.

Anwendungen und Systemkomponenten beschreiben bevorzugt den gewünschten Zustand, die benötigte Fähigkeit oder die einzuhaltenden Anforderungen.

NovaOS entscheidet anhand von Execution Contracts, Policies, Constraints, Resource Economy, Trust und aktuellem Systemzustand über die konkrete Umsetzung.

Damit entsteht das grundlegende NovaOS-Modell:

```text
Declarative System Model
        +
Execution Contracts
        +
Policies
        +
Capabilities
        +
Transactional Operations
        +
Reconciliation
        +
Architecture Introspection
```

Dieses Modell bildet eine der zentralen architektonischen Grundlagen von NovaOS.