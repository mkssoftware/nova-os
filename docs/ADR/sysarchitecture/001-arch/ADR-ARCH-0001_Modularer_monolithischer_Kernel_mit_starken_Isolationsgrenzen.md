# ADR-ARCH-0001 – Modularer monolithischer Kernel mit starken Isolationsgrenzen

## Status

Angenommen

## Kategorie

Systemarchitektur / Kernelarchitektur

## Kontext

NovaOS benötigt eine Kernelarchitektur, die hohe Performance, geringe Latenzen und effiziente Datenpfade mit starken Sicherheits-, Fehler- und Vertrauensgrenzen verbindet.

Ein klassischer monolithischer Kernel bietet eine enge Integration von Scheduling, Speicherverwaltung, I/O, Interruptbehandlung und Treibern. Dadurch können sehr effiziente Systempfade realisiert werden. Gleichzeitig führt die gemeinsame privilegierte Ausführungsumgebung zu einer großen Trust- und Failure-Domain.

Ein strikter Microkernel reduziert diese Domain, indem möglichst viele Systemkomponenten in getrennte Adressräume ausgelagert werden. Dies verbessert die Isolation, führt aber bei hochfrequenten Systempfaden zu zusätzlichen IPC-, Scheduling- und Context-Switch-Kosten.

NovaOS übernimmt deshalb nicht unverändert eine historische Kernelarchitektur, sondern kombiniert die Performance eines modularen monolithischen Kernels mit expliziten starken Isolationsgrenzen.

Die Architektur muss insbesondere folgende NovaOS-Prinzipien unterstützen:

- Capability-basierte Sicherheit
- starke Driver Isolation
- Zero-Copy
- Structured Concurrency
- systemweite Ressourcenökonomie
- Semantic Types
- transaktionale Systemoperationen
- Deterministic Mode
- Hot Replacement / Live Evolution
- Self-Healing
- Architecture Introspection
- Execution Contracts
- Echtzeit- und Deadline-Anforderungen
- Container
- MicroVMs
- Compatibility Domains

Die native Kernelarchitektur darf außerdem nicht durch POSIX-, Linux- oder Win32-Kompatibilität bestimmt werden.

## Entscheidung

NovaOS verwendet einen **modularen monolithischen Kernel mit starken Isolationsgrenzen**.

Der Kernel Core enthält ausschließlich fundamentale Mechanismen, bei denen eine privilegierte und eng gekoppelte Ausführung technisch gerechtfertigt ist.

Dazu gehören insbesondere:

- Interrupt- und Exception-Handling
- grundlegendes Scheduling
- Thread- und Task-Grundmechanismen
- physische Speicherverwaltung
- virtuelle Speicherverwaltung
- Address-Space-Verwaltung
- Kernel-Synchronisation
- grundlegende IPC-Mechanismen
- Capability Enforcement
- Kernel Object Management
- Timer- und Clock-Grundmechanismen
- HAL-Kernfunktionen
- fundamentale Resource-Economy-Mechanismen
- fundamentale Isolation-Domain-Mechanismen

Nicht jede Betriebssystemfunktion wird automatisch Bestandteil des Kernel Core.

Komponenten können abhängig von ihren Anforderungen innerhalb unterschiedlicher Ausführungs- und Isolationsdomänen betrieben werden.

## Grundstruktur

```text
Applications
    │
    ▼
Nova APIs / Capabilities
    │
    ▼
System Services
    │
    ▼
Execution Domains
    │
    ▼
Kernel Interfaces
    │
    ▼
┌─────────────────────────────────────┐
│             Nova Kernel             │
├─────────────────────────────────────┤
│ Scheduler                           │
│ Memory Management                   │
│ IPC Primitives                      │
│ Capability Enforcement              │
│ Interrupt / Exception Core          │
│ Timer / Clock Core                  │
│ HAL Core                            │
│ Resource Economy Core               │
│ Kernel Object Management            │
└─────────────────────────────────────┘
    │
    ▼
Hardware
```

## Kernelmodularität

Der Kernel MUSS intern modular aufgebaut sein.

Kernelkomponenten MÜSSEN klar definierte Verantwortlichkeiten und Schnittstellen besitzen.

Ein Kernelmodul SOLL mindestens folgende Eigenschaften beschreiben können:

```text
KernelModule {
    Identity
    Version
    Interfaces
    Dependencies
    Capabilities
    ResourceRequirements
    Lifecycle
    Provenance
    TrustRequirements
}
```

Die Ausführung innerhalb des Kerneladressraums bedeutet NICHT automatisch, dass ein Modul uneingeschränkte semantische Autorität besitzt.

Direkte undokumentierte Abhängigkeiten zwischen Kernelmodulen SOLLEN vermieden werden.

Stabile Interfaces MÜSSEN versioniert werden.

Interne Implementierungsdetails DÜRFEN evolvieren.

## Mechanism / Policy Separation

Der Kernel stellt fundamentale Mechanismen bereit.

Höherwertige Policies SOLLEN soweit möglich von diesen Mechanismen getrennt bleiben.

Der Kernel kann beispielsweise Mechanismen bereitstellen für:

- Scheduling
- Prioritäten
- Deadlines
- CPU-Affinität
- Ressourcenbudgets
- Isolation

Eine höhere Policy entscheidet beispielsweise:

- welche Schedulerklasse verwendet wird,
- welcher Workload bevorzugt wird,
- welcher Compute Provider verwendet wird,
- welche Energiepolicy gilt,
- welche Isolationsebene erforderlich ist.

Dadurch können Policies weiterentwickelt werden, ohne fundamentale Kernelmechanismen neu entwerfen zu müssen.

## Isolation Domains

NovaOS unterstützt unterschiedliche Isolation Domains.

Mindestens folgende Klassen sind vorgesehen:

```text
Kernel Core
Trusted Kernel Module
Driver Domain
System Service Domain
Application Domain
Compatibility Domain
Container Domain
MicroVM
Full VM
Confidential Domain
```

Eine Komponente ist nicht allein aufgrund ihrer funktionalen Kategorie an eine bestimmte Domain gebunden.

Die geeignete Domain wird anhand ihrer Anforderungen bestimmt.

Dabei können insbesondere berücksichtigt werden:

- Trust Level
- Provenance
- Hardwarezugriff
- Sicherheitsanforderungen
- Fehlerauswirkungen
- Latenz
- Determinismus
- Echtzeit
- Ressourcenbedarf
- Recovery-Anforderungen
- Compatibility Requirements

## Isolation Planner

NovaOS SOLL einen Isolation Planner unterstützen.

Dieser kann anhand eines Execution Contracts bestimmen, welche Ausführungsdomäne für eine Komponente geeignet ist.

Beispiel:

```text
Component {
    latency     = Low
    trust       = Verified
    isolation   = Strong
    restartable = True
}
```

Eine mögliche Entscheidung lautet:

```text
ExecutionDomain = UserDriverDomain
```

Eine andere Komponente könnte dagegen erhalten:

```text
ExecutionDomain = KernelModule
```

oder:

```text
ExecutionDomain = MicroVM
```

Harte Security-, Safety-, Realtime- und Determinism-Anforderungen dürfen durch die automatische Auswahl NICHT verletzt werden.

## Capability-basierte Sicherheitsgrenzen

Privilegien werden nicht ausschließlich über CPU-Ringe oder Prozessgrenzen definiert.

NovaOS verwendet Capability-basierte Autorisierung.

Eine Komponente erhält nur Zugriff auf Ressourcen, für die sie entsprechende Capabilities besitzt.

Beispiel:

```text
DriverCapability {
    device = GPU0

    rights = [
        MapRegisters,
        SubmitDMA,
        ReceiveInterrupt
    ]
}
```

Privilegierte Ausführung bedeutet NICHT automatisch Zugriff auf:

- alle Geräte
- alle Speicherbereiche
- alle Prozesse
- alle Dateien
- alle Netzwerkverbindungen
- alle Secrets
- alle Systemfähigkeiten

Capabilities MÜSSEN nach dem Least-Privilege-Prinzip vergeben werden.

## Keine universelle Root-Autorität

NovaOS verwendet keine einzelne universelle Root-Autorität als fundamentales natives Sicherheitsmodell.

Administrative und privilegierte Operationen werden über explizite Capabilities autorisiert.

Besonders kritische Operationen können zusätzliche Anforderungen besitzen, beispielsweise:

- Step-Up Authentication
- physische Bestätigung
- Recovery Authorization
- Trust Requirements
- Audit Requirements

## Treibermodell

Treiber müssen nicht grundsätzlich innerhalb des Kernel Core ausgeführt werden.

NovaOS unterstützt:

- Kernel-Mode Driver
- User-Mode Driver
- isolierte Driver Domains
- virtualisierte Driver
- Legacy Driver Domains

Treiber mit hoher Angriffsfläche oder niedrigerem Trust Level SOLLEN bevorzugt außerhalb der zentralen Kernel-Trust-Domain ausgeführt werden.

Kernel-Mode Driver sind insbesondere dann gerechtfertigt, wenn Anforderungen wie:

- sehr niedrige Latenz
- deterministische Reaktionszeit
- fundamentale Bootabhängigkeit
- technische Hardwarebeschränkungen

eine privilegierte Ausführung rechtfertigen.

## IOMMU

DMA-fähige Geräte SOLLEN nach Möglichkeit über eine IOMMU isoliert werden.

Ein Gerät beziehungsweise eine Driver Domain darf ausschließlich auf explizit freigegebene Speicherbereiche zugreifen.

Die Architektur DARF NICHT davon ausgehen, dass ein vertrauenswürdiger CPU-Ausführungskontext automatisch einen vertrauenswürdigen DMA-Zugriff garantiert.

## Shared Objects

Isolation bedeutet nicht automatisch Datenkopien.

NovaOS verwendet kontrollierte Shared Objects für effiziente domainübergreifende Kommunikation.

Beispiele:

- Shared Buffers
- Shared Memory Objects
- DMA Buffers
- Ring Buffers
- Shared Queues
- Memory-Mapped Objects

Der Zugriff auf diese Objekte wird über Capabilities kontrolliert.

## Zero-Copy

Die Kernelarchitektur MUSS Zero-Copy-Datenpfade ermöglichen.

Ein möglicher Datenpfad lautet:

```text
Storage Device
      │
      ▼
Storage Driver
      │
      ▼
Shared Buffer
      │
      ▼
Decoder
      │
      ▼
Shared Buffer
      │
      ▼
GPU
```

Die Daten müssen dabei nicht zwischen jeder beteiligten Domain vollständig kopiert werden.

Stattdessen erhalten die beteiligten Komponenten kontrollierte Rechte auf entsprechende Bufferobjekte.

Zero-Copy ist ein Architekturziel, jedoch kein absoluter Zwang.

Wenn Kopieren aus Gründen wie Sicherheit, Isolation, Alignment, Hardwarekompatibilität, Lebensdauer oder Datenformatkonvertierung sinnvoller ist, darf NovaOS einen Copy-Pfad verwenden.

Das übergeordnete Prinzip lautet daher:

```text
Copy Avoidance
```

und nicht:

```text
Copy Prohibition
```

## IPC

Domainübergreifende Kommunikation MUSS über definierte IPC-Mechanismen erfolgen.

NovaOS bevorzugt:

- Typed IPC
- Capability IPC
- Zero-Copy IPC
- Shared Objects
- asynchrone Kommunikation
- Completion-basierte Kommunikation
- Structured Concurrency
- Deadline Propagation
- Cancellation Propagation

IPC-Schnittstellen SOLLEN versioniert und introspektierbar sein.

## Structured Concurrency

Systemdienste und Kernel-nahe Tasks SOLLEN dem systemweiten Structured-Concurrency-Modell folgen.

Child Tasks SOLLEN einer klaren Parent-Lifetime zugeordnet werden.

Cancellation und Deadlines SOLLEN propagiert werden können.

Ungebundene Hintergrundarbeit MUSS explizit als solche deklariert werden.

## Resource Economy

Alle Ausführungsdomänen sind Bestandteil der systemweiten Nova Resource Economy.

Dies gilt auch für privilegierte Komponenten.

Ressourcen umfassen mindestens:

```text
CPU
Memory
I/O
Network
GPU
NPU
Energy
Thermal Headroom
Latency
Deadline
```

Eine privilegierte Komponente besitzt nicht automatisch unbegrenzte Ressourcen.

Kernel- und Systemdienste dürfen jedoch reservierte oder garantierte Ressourcen erhalten, wenn dies für Systemstabilität oder Echtzeit erforderlich ist.

## Execution Contracts

Komponenten können Anforderungen über einen `Nova.ExecutionContract` beschreiben.

Beispiel:

```text
ExecutionContract {
    latency       <= 1 ms
    determinism   = Required
    memory        <= 32 MiB
    trust         = SystemVerified
    isolation     = Strong
}
```

Diese Anforderungen können Einfluss darauf haben:

- wo eine Komponente ausgeführt wird
- welche Schedulerklasse verwendet wird
- welche Ressourcen reserviert werden
- welche Provider verwendet werden
- welche Isolation Domain verwendet wird

## Echtzeit

NovaOS MUSS Echtzeit-Workloads unterstützen können.

Realtime Domains können besondere Regeln erhalten.

Beispiele:

- reservierte CPU-Zeit
- CPU-Affinität
- Interrupt-Affinität
- gepinnter Speicher
- eingeschränkte Page Faults
- garantierte Ressourcen
- deterministische Schedulerklassen
- begrenzte Migration
- eingeschränkte Hot-Replacement-Mechanismen
- kontrollierte DVFS-Policies

Hard-Realtime-Anforderungen haben Vorrang vor adaptiven Optimierungszielen.

## Deterministic Mode

NovaOS unterstützt einen Deterministic Mode.

In diesem Modus können dynamische Optimierungen eingeschränkt werden.

Dazu können gehören:

- Work Stealing
- Task Migration
- adaptive Providerwahl
- adaptive Algorithmuswahl
- opportunistische Ressourcenverteilung
- DVFS
- speculative Prefetching

Eine adaptive Optimierung DARF eine explizite Determinism Requirement NICHT verletzen.

## Fehlerdomänen

Die Kernelarchitektur MUSS explizite Failure Domains unterstützen.

Beispiele:

```text
Task
Component
Service
Driver Domain
Execution Domain
Device
Session
Node
System
```

Fehler sollen auf die kleinstmögliche sichere Failure Domain begrenzt werden.

Ein Fehler in einem User-Mode Driver soll beispielsweise nicht automatisch den Kernel kompromittieren.

## Self-Healing

Die Kernelarchitektur MUSS die systemweite Nova.Resilience-Architektur unterstützen.

Der grundlegende Recovery-Ablauf lautet:

```text
Detect
    ↓
Classify
    ↓
Contain
    ↓
Preserve Evidence
    ↓
Plan
    ↓
Recover
    ↓
Verify
    ↓
Restore
    ↓
Learn
```

Ein Driver Domain kann beispielsweise neu gestartet werden, ohne das gesamte System neu zu starten.

## Recovery Scope

NovaOS SOLL immer den kleinsten sicheren Recovery Scope wählen.

Beispiel:

```text
Restart Task
    ↓
Restart Component
    ↓
Restart Service
    ↓
Restart Driver Domain
    ↓
Restart Execution Domain
    ↓
Restart Subsystem
    ↓
Restart System
```

Eine Eskalation erfolgt nur, wenn eine kleinere Recovery Domain nicht ausreichend ist.

## Hot Replacement

Austauschbare Komponenten SOLLEN Hot Replacement unterstützen können.

Dafür sind mindestens erforderlich:

- versionierte Interfaces
- definierte Lifecycle States
- Quiescence Points
- State Transfer
- Capability Rebinding
- Dependency Tracking
- Recovery Path
- Verification

Hot Replacement bedeutet nicht, dass jede Kernelkomponente jederzeit ersetzt werden können muss.

Fundamentale Kernelstrukturen können weiterhin einen Neustart erfordern.

## Live Evolution

Die Architektur MUSS langfristige Weiterentwicklung unterstützen.

Neue Implementierungen können parallel zu bestehenden Implementierungen vorhanden sein.

Beispiel:

```text
Capability:
    nova.storage.block

Providers:
    BlockProviderV1
    BlockProviderV2
```

Der Provider kann abhängig von Version, Compatibility, Trust, Hardware, Execution Contract und Policy ausgewählt werden.

## Versionierte Kernelinterfaces

Austauschbare oder extern verwendete Kernelinterfaces MÜSSEN versioniert sein.

Interne Implementierungsdetails dürfen dagegen verändert werden, sofern keine stabile Schnittstelle zugesichert wurde.

NovaOS unterscheidet:

```text
Stable Contract
Internal Interface
Implementation Detail
```

Nicht jede interne Kernelstruktur wird Bestandteil einer stabilen ABI.

## Native NovaABI

Die native NovaABI SOLL möglichst klein bleiben.

Höherwertige Systemfunktionen SOLLEN bevorzugt über:

- Nova APIs
- Capability Contracts
- Semantic APIs
- Execution Contracts

bereitgestellt werden.

Die native Kernelarchitektur darf nicht durch Anforderungen fremder ABIs bestimmt werden.

## Kompatibilität

POSIX-, Linux- und Win32-Kompatibilität werden als Kompatibilitätsschichten behandelt.

Sie definieren nicht die native NovaOS-Kernelarchitektur.

Beispiele:

```text
Nova Native
    │
    ├── POSIX Personality
    ├── Linux Personality
    ├── Win32 Personality
    ├── Compatibility Domain
    ├── MicroVM
    └── Full VM
```

Dadurch können Kompatibilitätsschichten unabhängig vom Kernel weiterentwickelt werden.

## Legacy-Treiber

Legacy-Treiber SOLLEN nicht ungeprüft direkt in den nativen NovaOS-Kernel geladen werden.

Mögliche Ausführungsformen sind:

- Compatibility Driver Domain
- isolierter User-Mode Driver
- MicroVM
- Full VM

Direkte Kernelintegration ist nur zulässig, wenn ein expliziter vertrauenswürdiger NovaOS-Port vorliegt.

## Virtualisierung

NovaOS ist kein Hypervisor-First-System.

Virtualisierung bleibt jedoch eine zentrale Isolationstechnologie.

NovaOS kann abhängig von den Anforderungen zwischen verschiedenen Ausführungsformen wählen:

```text
Native Process
System Service Domain
Container
Driver Domain
MicroVM
Full VM
Confidential VM
```

Die Wahl der Isolationsebene darf automatisiert erfolgen, sofern alle harten Anforderungen eingehalten werden.

## Trust

Privilegierte Komponenten werden über Nova.Trust bewertet.

Das grundlegende Trust-Modell lautet:

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

Eine gültige digitale Signatur allein reicht nicht aus, um Kernel-Level-Ausführung zu rechtfertigen.

Zusätzlich können erforderlich sein:

- bekannte Provenance
- autorisierter Publisher
- reproduzierbarer Build
- gültige Policy
- ausreichende Attestation
- geeigneter Trust Anchor
- keine bekannte Revocation

## Code Integrity

Kernelcode und privilegierte Module SOLLEN Code-Integrity-Prüfungen unterstützen.

Dazu gehören abhängig von Plattform und Buildkonfiguration:

- Signaturprüfung
- Hash-Verifikation
- Provenance-Prüfung
- Secure Boot
- Measured Boot
- Runtime Integrity
- Control-Flow-Schutz

## Memory Protection

Der Kernel SOLL verfügbare Hardware- und Compilermechanismen zur Speicherabsicherung verwenden.

Dazu gehören insbesondere:

- W^X
- NX/DEP
- Guard Pages
- Stack Canaries
- ASLR
- Control-Flow Integrity
- Shadow Stack, sofern verfügbar
- Memory Protection Keys, sofern geeignet
- IOMMU für DMA-Isolation

## Memory Safety

Neue Kernelkomponenten SOLLEN nach Möglichkeit speichersicher implementiert werden.

Speicherunsichere Implementierungen müssen auf Bereiche begrenzt werden, in denen sie technisch erforderlich sind.

Unsichere Operationen SOLLEN:

- klein
- lokal
- dokumentiert
- überprüfbar
- testbar

gehalten werden.

## Architecture Introspection

NovaOS MUSS seine tatsächliche Architektur zur Laufzeit introspektierbar machen.

Für eine Komponente sollen beispielsweise folgende Informationen abrufbar sein:

```text
Component:
    WiFi.Driver

ExecutionDomain:
    UserDriverDomain

Identity:
    de.vendor.wifi.driver

Trust:
    Verified

Isolation:
    Process
    IOMMU

Capabilities:
    PCI.Device<WiFi0>
    DMA.Region<WiFiBuffers>
    Interrupt<WiFiIRQ>

Resources:
    CPUBudget    = 2 %
    MemoryBudget = 64 MiB

Recovery:
    Restartable

HotReplacement:
    Supported
```

## Decision Tracing

Wenn NovaOS automatisch eine Ausführungsdomäne auswählt, SOLL die Entscheidung nachvollziehbar sein.

Beispiel:

```text
Decision:
    UserDriverDomain selected

Reasons:
    Kernel latency not required
    Strong isolation requested
    Driver is restartable
    IOMMU available
    Execution contract satisfied
```

Automatische Architekturentscheidungen dürfen keine vollständig undurchsichtige Black Box bilden.

## Performance

Isolation darf nicht blind maximiert werden.

NovaOS verwendet das Prinzip:

```text
Wähle die stärkste sinnvolle Isolation,
die alle harten Execution Contracts erfüllt.
```

Dabei können insbesondere berücksichtigt werden:

- Latency
- Throughput
- Determinism
- Security
- Trust
- Energy
- Thermal Budget
- Memory
- Recovery Requirements

Performanceoptimierung darf harte Sicherheitsgrenzen NICHT stillschweigend aufheben.

## Boot und Recovery

Der NovaOS-Kernel muss über die definierten BIOS- und UEFI-Bootpfade gestartet werden können.

UEFI ist auf moderner unterstützter Hardware der bevorzugte Bootpfad.

BIOS bleibt als Kompatibilitätsbootpfad erhalten.

Beide Bootpfade sollen letztlich eine gemeinsame definierte Kernel-Startumgebung erzeugen.

NovaDOS bildet eine separate Offline-Recovery-Umgebung.

NovaDOS kann verwendet werden, wenn der normale NovaOS-Kernel oder dessen Systemumgebung nicht ausreichend vertrauenswürdig oder funktionsfähig ist, um eine Reparatur selbst durchzuführen.

## Sicherheitsgrenze des Kernel Core

Der Kernel Core ist die stärkste native privilegierte Domain des Systems.

Ein Fehler innerhalb des Kernel Core kann weiterhin systemweite Auswirkungen besitzen.

Deshalb gilt:

```text
Nur Mechanismen gehören in den Kernel Core,
deren privilegierte Ausführung technisch gerechtfertigt ist.
```

Komfortfunktionen, komplexe Parser, Netzwerkdienste, Dateiformatdecoder und ähnliche Komponenten SOLLEN nicht allein aus Bequemlichkeit im Kernel Core implementiert werden.

## Normative Festlegungen

1. NovaOS MUSS einen modularen monolithischen Kernel als native Kernelbasis verwenden.
2. Der Kernel MUSS intern klar modularisiert sein.
3. Der Kernel DARF NICHT als unstrukturierter gemeinsamer Privilegienraum behandelt werden.
4. Kernelkomponenten MÜSSEN explizite Interfaces und Abhängigkeiten besitzen.
5. Stabile Kernelinterfaces MÜSSEN versioniert werden.
6. Interne Kernelinterfaces DÜRFEN evolvieren, sofern sie nicht als stabile Contracts veröffentlicht wurden.
7. NovaOS MUSS unterschiedliche Isolation Domains unterstützen.
8. Nicht jede Betriebssystemkomponente MUSS im Kernelraum ausgeführt werden.
9. Treiber SOLLEN abhängig von Trust-, Performance- und Echtzeitanforderungen im Kernel oder in isolierten Driver Domains ausgeführt werden können.
10. DMA-fähige isolierte Domains SOLLEN nach Möglichkeit über eine IOMMU geschützt werden.
11. NovaOS MUSS Capability-basierte Zugriffssteuerung auch für privilegierte Komponenten verwenden.
12. Privilegierte Ausführung DARF NICHT automatisch uneingeschränkte Autorität bedeuten.
13. NovaOS SOLL keine universelle Root-Autorität als grundlegendes natives Sicherheitsmodell verwenden.
14. Der Kernel MUSS Zero-Copy- und Shared-Buffer-Datenpfade ermöglichen.
15. Zero-Copy DARF NICHT die Sicherheits- oder Isolationseigenschaften des Systems verletzen.
16. Copy Avoidance ist gegenüber absolutem Copy Prohibition zu bevorzugen.
17. Kernel- und Systemkomponenten MÜSSEN in die Nova Resource Economy integrierbar sein.
18. Ressourcenbudgets MÜSSEN auch für privilegierte Komponenten anwendbar sein.
19. Realtime-, Safety- und Security-Anforderungen MÜSSEN Vorrang vor adaptiven Optimierungen erhalten.
20. Der Deterministic Mode MUSS nichtdeterministische Optimierungen einschränken können.
21. NovaOS MUSS explizite Failure Domains unterstützen.
22. Recovery SOLL auf der kleinstmöglichen sicheren Failure Domain erfolgen.
23. Austauschbare Komponenten SOLLEN definierte Lifecycle- und Hot-Replacement-Mechanismen verwenden.
24. Hot Replacement DARF NICHT erzwungen werden, wenn ein sicherer Austausch ohne Neustart nicht gewährleistet werden kann.
25. Die native NovaABI SOLL möglichst klein gehalten werden.
26. POSIX-, Linux- und Win32-Kompatibilität DÜRFEN die native Kernelarchitektur NICHT bestimmen.
27. Legacy-Treiber SOLLEN in isolierten Compatibility Domains ausgeführt werden.
28. NovaOS DARF Virtualisierung als zusätzliche Isolationstechnologie verwenden.
29. NovaOS DARF NICHT voraussetzen, dass sämtliche nativen Workloads innerhalb virtueller Maschinen ausgeführt werden.
30. Privilegierte Komponenten MÜSSEN über Nova.Trust bewertbar sein.
31. Eine gültige Signatur DARF NICHT automatisch Kernel-Trust bedeuten.
32. Kernelmodule SOLLEN Provenance-Informationen besitzen.
33. Der Kernel SOLL verfügbare Speicher- und Control-Flow-Schutzmechanismen verwenden.
34. Neue Kernelkomponenten SOLLEN soweit technisch sinnvoll speichersicher implementiert werden.
35. Unsichere Kernelbereiche SOLLEN minimal und klar abgegrenzt werden.
36. Die Kernelarchitektur MUSS über Architecture Introspection nachvollziehbar sein.
37. Automatische Domain- und Providerentscheidungen SOLLEN über Decision Tracing erklärbar sein.
38. Die stärkste Isolation, die alle harten Execution Contracts erfüllt, SOLL bevorzugt werden.
39. Sicherheitsgrenzen DÜRFEN NICHT allein aus Performancegründen stillschweigend aufgehoben werden.
40. KI-basierte Optimierungen DÜRFEN NICHT Bestandteil der fundamentalen Korrektheits- oder Sicherheitsannahmen des Kernels sein.

## Konsequenzen

### Positive Konsequenzen

- hohe native Performance
- geringe Kernel-Latenzen
- effiziente Zero-Copy-Pfade
- starke Driver Isolation
- kleinere Failure Domains
- Capability-basierte Sicherheitsgrenzen
- flexible Ausführungsdomänen
- Unterstützung für Self-Healing
- Unterstützung für Hot Replacement
- gute Echtzeiteignung
- kontrollierte Legacy-Kompatibilität
- Nutzung von MicroVMs ohne Hypervisor-First-Zwang
- bessere Evolvierbarkeit
- systemweite Introspektion
- nachvollziehbare Architekturentscheidungen

### Negative Konsequenzen

- höhere Architekturkomplexität als bei einem einfachen monolithischen Kernel
- zusätzliche IPC- und Domain-Infrastruktur
- aufwendigeres Lifecycle-Management
- komplexeres Capability Management
- komplexeres Debugging über Domain-Grenzen
- zusätzlicher Aufwand für Driver Isolation
- zusätzlicher Aufwand für State Transfer und Hot Replacement
- Kernel-Core-Fehler können weiterhin systemweite Auswirkungen haben

## Verworfene Alternativen

### Klassischer monolithischer Kernel

Nicht übernommen.

Ein klassischer monolithischer Kernel bietet hohe Performance, erzeugt jedoch eine zu große gemeinsame Trust- und Failure-Domain.

NovaOS übernimmt die Performancevorteile, ergänzt sie jedoch um explizite Modularität und Isolation Domains.

### Strikter Microkernel

Nicht als native Grundarchitektur übernommen.

Microkernel-Techniken wie isolierte Systemdienste, User-Mode Driver, Message Passing und kleine Trust Domains werden gezielt übernommen.

NovaOS erzwingt jedoch nicht für jeden fundamentalen Systempfad eine Prozess- oder IPC-Grenze.

### Hybridkernel

Nicht als primäre Architekturbezeichnung übernommen.

Der Begriff beschreibt nicht ausreichend präzise, welche Komponenten in NovaOS privilegiert sind und wie Isolation, Capabilities und Execution Domains funktionieren.

### Exokernel

Nicht als Grundarchitektur übernommen.

NovaOS soll weiterhin systemweite Ressourcenverwaltung, Scheduling, Security, Hardwareabstraktion und Policies bereitstellen.

### Separation Kernel

Nicht als allgemeine Kernelarchitektur übernommen.

Starke Partitionierung und Isolation können jedoch für Safety-, Security- oder Realtime-Domänen verwendet werden.

### Hypervisor-First

Nicht übernommen.

Virtualisierung ist ein Werkzeug innerhalb der NovaOS-Isolationsarchitektur und nicht die zwingende Grundlage jeder Ausführung.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer
- ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell
- ADR-DRIVER-0002_Driver_Sandboxing
- ADR-IPC-0001_Typed_IPC
- ADR-IPC-0002_Capability_Based_IPC
- ADR-IPC-0003_Zero_Copy_IPC
- ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell
- ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur
- ADR-VIRTUALIZATION-0001_Unified_Nova_Virtualization_Architecture
- ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen

## Zugehörige NPSPECs

- NPSPEC-ARCH-SYSTEMMODEL-0001
- NPSPEC-ARCH-RESOURCEECONOMY-0001
- NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001
- NPSPEC-ARCH-DETERMINISM-0001
- NPSPEC-ARCH-LIVEEVOLUTION-0001
- NPSPEC-ARCH-INTROSPECTION-0001
- NPSPEC-HAL-0001
- NPSPEC-SCHEDULER-0001
- NPSPEC-MEMORY-PMM-0001
- NPSPEC-MEMORY-VAS-0001
- NPSPEC-IPC-0001
- NPSPEC-IPC-CAPABILITY-0001
- NPSPEC-IPC-ZEROCOPY-0001
- NPSPEC-DRIVER-MODEL-0001
- NPSPEC-DRIVER-ISOLATION-0001
- NPSPEC-DRIVER-SANDBOX-0001
- NPSPEC-DRIVER-CAPABILITY-0001
- NPSPEC-RESOURCE-ECONOMY-0001
- NPSPEC-EXECUTION-CONTRACT-0001
- NPSPEC-SECURITY-CAPABILITY-0001
- NPSPEC-TRUST-0001
- NPSPEC-RESILIENCE-ARCH-0001
- NPSPEC-VIRTUALIZATION-ARCH-0001
- NPSPEC-COMPAT-ABI-0001

## Ergebnis

Mit dieser Entscheidung erhält NovaOS einen hochperformanten nativen Kernel, ohne die gesamte Betriebssystemarchitektur zu einer einzigen unkontrollierten privilegierten Failure- und Trust-Domain zu machen.

Der Kernel bleibt bewusst leistungsfähig, während Treiber, Dienste, Kompatibilitätskomponenten und andere risikobehaftete Funktionen abhängig von ihren Anforderungen in stärkere Isolation Domains verschoben werden können.

Damit bildet der modulare monolithische Kernel zusammen mit Capabilities, Execution Domains, Resource Economy, Nova.Trust, Nova.Resilience, Zero-Copy und Architecture Introspection die native Kernelgrundlage von NovaOS.