# ADR-ARCH-0005 – Systemweite Ressourcenökonomie

## Status

Angenommen

## Kategorie

Systemarchitektur / Ressourcenmanagement / Scheduling / Execution

## Kontext

Klassische Betriebssysteme verwalten Ressourcen häufig in weitgehend getrennten Subsystemen.

Beispiele:

- CPU-Zeit durch den Scheduler
- Arbeitsspeicher durch die Speicherverwaltung
- Storage-I/O durch Block- und Dateisystemschichten
- Netzwerkbandbreite durch den Netzwerkstack
- GPU-Ressourcen durch Grafik- und Compute-Treiber
- NPU-Ressourcen durch separate Accelerator-Runtimes
- Energie durch Power Management
- thermische Grenzen durch Thermal Management

Diese Subsysteme verfügen häufig über eigene:

- Prioritäten
- Limits
- Quoten
- Scheduler
- Heuristiken
- Telemetrie
- Policies

Dadurch fehlt dem Gesamtsystem eine einheitliche Vorstellung davon, welche Ressourcen eine Operation tatsächlich benötigt und welche systemweiten Kosten ihre Ausführung verursacht.

Ein Workload kann beispielsweise wenig CPU benötigen, aber gleichzeitig:

- sehr viel GPU-Zeit,
- mehrere GiB Arbeitsspeicher,
- hohe Storage-Bandbreite,
- erhebliche Netzwerkbandbreite,
- hohe Energie,
- thermischen Headroom

beanspruchen.

Eine rein CPU-basierte Schedulingentscheidung kann diesen Workload deshalb nicht vollständig bewerten.

NovaOS soll Ressourcen nicht nur als voneinander unabhängige technische Größen betrachten.

Stattdessen wird eine **systemweite Ressourcenökonomie** eingeführt.

Diese bildet ein gemeinsames Modell für:

```text
Demand
Supply
Reservation
Budget
Priority
Cost
Pressure
Allocation
Consumption
Reclamation
Accounting
```

Die Ressourcenökonomie soll insbesondere mit folgenden NovaOS-Konzepten zusammenarbeiten:

- Declarative System Model
- Execution Contracts
- Mechanism / Policy Separation
- Structured Concurrency
- Capability-System
- automatische Providerwahl
- automatische Algorithmuswahl
- Deterministic Mode
- Realtime
- Power Management
- Thermal Management
- Self-Healing
- Architecture Introspection

## Entscheidung

NovaOS verwendet eine **systemweite Ressourcenökonomie** als grundlegendes Architekturprinzip.

Alle relevanten endlichen Systemressourcen SOLLEN über ein gemeinsames Ressourcenmodell beschreibbar, budgetierbar und beobachtbar sein.

Dazu gehören mindestens:

```text
CPU
Memory
Storage I/O
Network
GPU
NPU
Accelerators
Energy
Thermal Headroom
Latency
Deadline Capacity
```

Weitere Ressourcentypen können ergänzt werden.

Das grundlegende Modell lautet:

```text
Resource Supply
       │
       ▼
Available Capacity
       │
       ▼
Resource Economy
       │
       ├── Reservations
       ├── Budgets
       ├── Priorities
       ├── Policies
       ├── Execution Contracts
       └── Current Pressure
       │
       ▼
Allocation
       │
       ▼
Execution
       │
       ▼
Accounting
       │
       ▼
Feedback
```

Ressourcenmanagement wird dadurch nicht zu einem einzigen zentralen Scheduler.

Die einzelnen Subsysteme behalten spezialisierte Mechanismen.

Die Resource Economy stellt darüber ein gemeinsames semantisches Modell für Anforderungen, Budgets, Kosten und Entscheidungen bereit.

## Ziele

Die systemweite Ressourcenökonomie verfolgt insbesondere folgende Ziele:

- Ressourcen systemweit sichtbar machen
- Ressourcenanforderungen deklarativ beschreiben
- Ressourcenbudgets definieren
- Ressourcen reservieren
- Ressourcenverbrauch messen
- Überlastung frühzeitig erkennen
- Admission Control ermöglichen
- Prioritäten systemweit berücksichtigen
- Realtime-Garantien unterstützen
- Energie- und Thermalbudgets integrieren
- Providerwahl verbessern
- Algorithmuswahl verbessern
- Ressourcenmissbrauch begrenzen
- Self-Healing unterstützen
- Ressourcenentscheidungen introspektierbar machen

## Nicht-Ziele

Die Resource Economy ist nicht:

- ein einzelner globaler Scheduler,
- eine universelle monetäre Währung,
- ein Ersatz für CPU-Scheduling,
- ein Ersatz für Memory Management,
- ein Ersatz für GPU-Scheduling,
- ein Ersatz für Netzwerk-QoS,
- ein Ersatz für Power Management.

Sie verbindet diese Subsysteme über ein gemeinsames Ressourcenmodell.

## Ressourcenmodell

Eine Ressource besitzt mindestens:

```text
Resource {
    Identity
    Type
    Capacity
    Availability
    Unit
    Scope
    Provider
    Constraints
}
```

Beispiel:

```text
Resource {
    identity     = CPU.Package0
    type         = Compute.CPU
    capacity     = 16 cores
    availability = Dynamic
}
```

oder:

```text
Resource {
    identity     = GPU0.Memory
    type         = Memory.VRAM
    capacity     = 8 GiB
}
```

## Ressourcentypen

NovaOS soll mindestens folgende Ressourcendomänen modellieren können:

```text
Compute
Memory
Storage
Network
Accelerator
Energy
Thermal
Time
Latency
Deadline
Device
```

Diese können weiter spezialisiert werden.

Beispiel:

```text
Compute.CPU
Compute.GPU
Compute.NPU
Compute.DSP
Compute.FPGA

Memory.RAM
Memory.VRAM
Memory.Persistent

Storage.Capacity
Storage.ReadBandwidth
Storage.WriteBandwidth
Storage.IOPS

Network.Bandwidth
Network.Latency

Energy.Power
Energy.Total

Thermal.Headroom
```

## Ressourcenidentität

Ressourcen müssen eindeutig identifizierbar sein.

Beispiele:

```text
CPU.Package0
CPU.Core3
GPU0
GPU0.Memory
NPU0
Storage.NVMe0
Network.WiFi0
```

Ressourcen können hierarchisch organisiert werden.

Beispiel:

```text
System
└── CPU.Package0
    ├── Core0
    ├── Core1
    ├── Core2
    └── Core3
```

## Ressourcenhierarchie

Ressourcenbudgets SOLLEN hierarchisch delegierbar sein.

Beispiel:

```text
System
    │
    ├── User
    │   └── Application
    │       └── Task
    │
    └── SystemServices
```

Ein Parent-Budget kann auf Child-Budgets verteilt werden.

Beispiel:

```text
Application:
    CPU <= 20 %

Children:
    Decoder <= 8 %
    Renderer <= 10 %
    Background <= 2 %
```

Die Summe garantierter Child-Ressourcen darf die verfügbare Parent-Garantie nicht unkontrolliert überschreiten.

## Resource Contract

Ressourcenanforderungen können über einen Resource Contract beschrieben werden.

Beispiel:

```text
ResourceContract {
    cpu {
        minimum   = 1 core
        preferred = 4 cores
        maximum   = 8 cores
    }

    memory {
        minimum = 256 MiB
        maximum = 2 GiB
    }

    gpu {
        preferred = true
    }

    energy {
        mode = PreferEfficient
    }
}
```

Der Resource Contract kann Bestandteil eines `Nova.ExecutionContract` sein.

## Execution Contract

Die Resource Economy ist direkt mit `Nova.ExecutionContract` verbunden.

Beispiel:

```text
ExecutionContract {
    latency      <= 16 ms
    deadline     = 16 ms
    determinism  = Preferred

    resources {
        cpu      <= 20 %
        memory   <= 512 MiB
        gpu      = Preferred
        energy   = PreferLow
    }
}
```

Der Execution Contract beschreibt Anforderungen.

Die Resource Economy bestimmt gemeinsam mit Policies und Providern, ob und wie diese Anforderungen erfüllt werden können.

## Minimum, Maximum und Preferred

Ressourcenanforderungen SOLLEN mindestens folgende Semantik unterstützen:

```text
Minimum
Maximum
Preferred
Guaranteed
BestEffort
```

### Minimum

Die Operation kann unterhalb dieses Werts nicht sinnvoll ausgeführt werden.

### Maximum

Die Operation darf diesen Wert nicht überschreiten.

### Preferred

Dieser Wert ist wünschenswert, aber nicht zwingend erforderlich.

### Guaranteed

Die Ressource muss für einen definierten Zeitraum garantiert werden.

### BestEffort

Es existiert keine harte Garantie.

## Ressourcenbudgets

Ein Budget definiert, wie viel einer Ressource eine Entität verwenden darf.

Beispiel:

```text
ResourceBudget {
    CPU      <= 10 %
    Memory   <= 512 MiB
    Network  <= 20 MiB/s
    GPU      <= 15 %
}
```

Budgets können gelten für:

- System
- User
- Session
- Application
- Service
- Execution Domain
- Capability
- Task
- Transaction

## Harte und weiche Budgets

NovaOS unterscheidet zwischen harten und weichen Budgets.

### Hard Budget

Ein Hard Budget darf nicht überschritten werden.

Beispiel:

```text
Memory <= 512 MiB
```

### Soft Budget

Ein Soft Budget darf kurzfristig überschritten werden, wenn:

- freie Ressourcen verfügbar sind,
- keine höher priorisierten Anforderungen verletzt werden,
- die Policy dies erlaubt.

Beispiel:

```text
CPU preferred <= 20 %
```

## Reservations

NovaOS MUSS Ressourcen reservieren können.

Eine Reservation sichert eine Ressource für einen definierten Scope und Zeitraum.

Beispiel:

```text
Reservation {
    resource = CPU.Core3
    amount   = 100 %
    duration = 5 ms
}
```

oder:

```text
Reservation {
    resource = Memory.RAM
    amount   = 256 MiB
}
```

Reservations sind insbesondere wichtig für:

- Realtime
- Deterministic Mode
- Transaktionen
- Hot Replacement
- Multimedia
- Hardwaresteuerung
- Safety-Critical Workloads

## Reservation Lifecycle

Eine Reservation kann folgenden Lifecycle besitzen:

```text
Requested
    ↓
Validated
    ↓
Granted
    ↓
Active
    ↓
Released
```

Fehlerzustände können sein:

```text
Rejected
Expired
Revoked
```

## Admission Control

Bevor eine Operation mit garantierten Anforderungen gestartet wird, SOLL Admission Control prüfen, ob die notwendigen Ressourcen verfügbar sind.

Beispiel:

```text
Request:
    CPU = 2 cores guaranteed
    Memory = 1 GiB guaranteed
    Deadline = 10 ms
```

Wenn diese Anforderungen nicht erfüllt werden können, darf NovaOS die Operation nicht einfach starten und anschließend die Garantie verletzen.

Mögliche Ergebnisse:

```text
Accepted
Rejected
Deferred
Degraded
```

`Degraded` ist nur zulässig, wenn der Execution Contract eine Degradation ausdrücklich erlaubt.

## Ressourcenknappheit

NovaOS modelliert Resource Pressure.

Mögliche Zustände:

```text
Normal
Elevated
High
Critical
Exhausted
```

Diese Zustände können pro Ressource unterschiedlich sein.

Beispiel:

```text
CPU:
    Normal

Memory:
    Critical

GPU:
    High

Thermal:
    Elevated
```

Damit kann das System gezielter reagieren als mit einem einzelnen globalen Lastwert.

## Pressure Signals

Subsysteme SOLLEN Resource Pressure melden können.

Beispiele:

- Memory Pressure
- CPU Saturation
- Storage Queue Pressure
- Network Congestion
- GPU Saturation
- Energy Pressure
- Thermal Pressure

Diese Signale können von Policies verwendet werden.

## Resource Accounting

NovaOS MUSS Ressourcenverbrauch messen können.

Accounting kann mindestens umfassen:

```text
Allocated
Reserved
Consumed
Peak
Average
Duration
Throttled
Denied
```

Nicht jede Ressource muss alle Messgrößen unterstützen.

## Hierarchisches Accounting

Verbrauch soll auf mehreren Ebenen aggregiert werden können.

Beispiel:

```text
Task
    ↓
Application
    ↓
User
    ↓
System
```

Dadurch kann NovaOS beantworten:

```text
Welche Anwendung verbraucht GPU-Zeit?
```

oder:

```text
Welche Capability verursacht den Netzwerkverkehr?
```

oder:

```text
Welche Systemfunktion verursacht den Energieverbrauch?
```

## Capability-basiertes Accounting

Ressourcenverbrauch SOLL einer Capability zugeordnet werden können.

Beispiel:

```text
Capability:
    de.nova.image.filter.gaussian

Provider:
    GPUProvider

Resources:
    GPU    = 18 ms
    VRAM   = 64 MiB
    Energy = 2.4 J
```

Damit können Ressourcen nicht nur nach Prozess, sondern auch nach semantischer Funktion analysiert werden.

## Ressourcenvererbung

Child Tasks SOLLEN Ressourcenbudgets vom Parent-Kontext ableiten können.

Beispiel:

```text
Application
    CPU Budget = 20 %
        │
        ├── Task A
        ├── Task B
        └── Task C
```

Child Tasks dürfen das Parent-Budget nicht unkontrolliert umgehen.

Dies verbindet Resource Economy mit Structured Concurrency.

## Structured Concurrency

Structured Concurrency und Resource Economy werden miteinander integriert.

Ein Task Tree kann gleichzeitig einen Resource Tree bilden.

Beispiel:

```text
Application
│
├── DecodeTask
│   └── WorkerTasks
│
└── RenderTask
    └── GPUCommands
```

Ressourcen können dadurch:

- vererbt
- budgetiert
- aggregiert
- gecancelt
- freigegeben

werden.

Wenn ein Parent Task beendet wird, müssen zugehörige Reservations und temporäre Ressourcen kontrolliert freigegeben werden können.

## CPU

CPU-Ressourcen können beschrieben werden durch:

- CPU Time
- Core Count
- Utilization
- Priority
- Affinity
- Frequency Capacity
- Scheduler Class
- Deadline Capacity

Beispiel:

```text
CPURequirement {
    minimum = 1 core
    preferred = 4 cores
    affinity = PerformanceCores
}
```

## Memory

Memory-Ressourcen umfassen unter anderem:

- Physical RAM
- Virtual Memory
- Pinned Memory
- Shared Memory
- Huge Pages
- VRAM
- Accelerator Memory

Memory Budgets müssen insbesondere zwischen:

```text
Reserved
Committed
Resident
Pinned
Shared
```

unterscheiden können.

## Shared Memory Accounting

Shared Memory darf nicht unkontrolliert mehrfach vollständig abgerechnet werden.

NovaOS muss zwischen:

```text
Ownership
Reservation
Mapping
Actual Physical Consumption
```

unterscheiden können.

Dies ist besonders für Zero-Copy relevant.

## Storage

Storage-Ressourcen umfassen mindestens:

- Capacity
- Read Bandwidth
- Write Bandwidth
- IOPS
- Queue Capacity
- Latency

Beispiel:

```text
StorageRequirement {
    readBandwidth >= 500 MiB/s
    latency       <= 2 ms
}
```

## Netzwerk

Netzwerkressourcen umfassen:

- Bandwidth
- Latency
- Packet Rate
- Queue Capacity
- Connection Capacity

Netzwerkbudgets können nach:

- Application
- Capability
- User
- Service
- Traffic Class

definiert werden.

## GPU

GPU-Ressourcen umfassen unter anderem:

- Compute Time
- Graphics Time
- VRAM
- Command Queue Capacity
- Memory Bandwidth
- Copy Engine Capacity

NovaOS SOLL GPU-Ressourcen in die allgemeine Resource Economy integrieren.

GPU-Ausführung darf nicht als vollständig separates Ressourcenuniversum behandelt werden.

## NPU und Accelerators

Dasselbe gilt für:

- NPU
- DSP
- FPGA
- AI Accelerators
- Media Engines
- Crypto Accelerators

Neue Hardwareklassen sollen in das bestehende Ressourcenmodell integrierbar sein.

## Energie

Energie ist eine systemweite Ressource.

NovaOS unterscheidet mindestens zwischen:

```text
Power
```

und:

```text
Energy
```

Power beschreibt eine momentane Rate.

Beispiel:

```text
15 W
```

Energy beschreibt Verbrauch über Zeit.

Beispiel:

```text
30 J
```

Beide Größen dürfen nicht semantisch gleichgesetzt werden.

## Energy Budget

Workloads können Energy Budgets besitzen.

Beispiel:

```text
EnergyBudget {
    maximum = 50 J
}
```

oder:

```text
EnergyPolicy {
    mode = PreferEfficient
}
```

Dies ermöglicht insbesondere auf mobilen Geräten systemweite Energieoptimierung.

## Thermal Headroom

Thermal Headroom wird als Ressource behandelt.

Beispiel:

```text
ThermalHeadroom {
    CPU = Low
    GPU = High
}
```

Ein Provider kann technisch verfügbar sein, aber aufgrund thermischer Grenzen aktuell ungeeignet sein.

Die Providerwahl kann deshalb beispielsweise entscheiden:

```text
GPU:
    Fastest
    ThermalHeadroom = Critical

NPU:
    Slightly slower
    ThermalHeadroom = High

Selected:
    NPU
```

## Zeit als Ressource

Zeitbezogene Anforderungen sind Bestandteil der Resource Economy.

Dazu gehören:

- Latency
- Deadline
- Execution Window
- Jitter Budget

Zeit ist jedoch nicht beliebig speicherbar oder zurückgewinnbar.

Sie benötigt deshalb eine eigene Semantik.

## Deadline Capacity

Ein Scheduler kann nicht unbegrenzt viele garantierte Deadlines akzeptieren.

Deadline Capacity muss deshalb bei Admission Control berücksichtigt werden.

Beispiel:

```text
Current guaranteed utilization:
    82 %

New request:
    25 %

Result:
    Rejected
```

wenn die garantierte Gesamtauslastung dadurch die zulässige Grenze überschreiten würde.

## Latency Budget

Latenzbudgets können über mehrere Komponenten propagiert werden.

Beispiel:

```text
Total Deadline:
    20 ms

Decode:
    5 ms

Process:
    7 ms

Render:
    6 ms

Reserve:
    2 ms
```

NovaOS SOLL Deadline- und Latency-Budgets entlang einer Ausführungskette propagieren können.

## End-to-End Resource Contracts

Eine Operation kann mehrere Ressourcen gleichzeitig benötigen.

Beispiel:

```text
VideoFrame {
    deadline = 16.67 ms

    resources {
        CPU    <= 3 ms
        GPU    <= 7 ms
        Memory <= 128 MiB
    }
}
```

Die Entscheidung muss deshalb multidimensional erfolgen.

## Ressourcen sind nicht vollständig austauschbar

NovaOS führt keine universelle Ressourceneinheit ein, in der beispielsweise:

```text
1 MiB RAM = 3 ms CPU = 2 Joule
```

fest umgerechnet wird.

Solche Ressourcen sind semantisch unterschiedlich.

Policies können Trade-offs bewerten, aber die ursprünglichen Dimensionen bleiben erhalten.

## Multi-Resource Scheduling

Die Resource Economy SOLL Entscheidungen über mehrere Ressourcendimensionen unterstützen.

Beispiel:

```text
Provider A:
    CPU    = Low
    Memory = High
    Energy = Medium

Provider B:
    CPU    = Medium
    Memory = Low
    Energy = Low
```

Die Auswahl hängt vom Execution Contract und der aktuellen Systemlage ab.

## Providerwahl

Providerwahl verwendet Resource Economy als wichtige Entscheidungsgrundlage.

Beispiel:

```text
Capability:
    de.nova.image.filter.gaussian
```

Provider:

```text
CPUProvider
GPUProvider
NPUProvider
```

Die Auswahl kann berücksichtigen:

- Performance
- Memory
- Energy
- Thermal Headroom
- Current Load
- Deadline
- Trust
- Determinism
- Data Sovereignty

## Algorithmuswahl

Dasselbe gilt für automatische Algorithmuswahl.

Beispiel:

```text
Sort(data)
```

NovaOS kann zwischen Algorithmen wählen, die unterschiedliche Ressourcenprofile besitzen.

Beispiel:

```text
Algorithm A:
    Fast
    High Memory

Algorithm B:
    Slower
    Low Memory
```

Bei Memory Pressure kann Algorithmus B sinnvoller sein.

Ein expliziter Algorithmus-Override bleibt möglich.

## Resource Profiles

Provider und Algorithmen SOLLEN Resource Profiles veröffentlichen können.

Beispiel:

```text
ResourceProfile {
    CPU        = Medium
    Memory     = Low
    GPU        = None
    Energy     = Low
    Parallelism = High
}
```

Profile können:

- statisch
- gemessen
- adaptiv gelernt

sein.

## Predictive Resource Management

NovaOS DARF zukünftigen Ressourcenbedarf vorhersagen.

Beispiele:

- erwartete Speichernutzung
- zukünftige CPU-Last
- GPU-Auslastung
- Energiebedarf
- Thermalentwicklung

Vorhersagen dürfen zur Optimierung verwendet werden.

Sie dürfen jedoch harte Garantien nicht ersetzen.

## Prediction Error

Vorhersagen werden mit tatsächlichen Ergebnissen verglichen.

Das Modell lautet:

```text
Prediction
    ↓
Execution
    ↓
Measured Consumption
    ↓
Prediction Error
    ↓
Model Correction
```

Wiederholte Abweichungen können Resource Profiles und zukünftige Planungen verbessern.

## Adaptive Resource Profiles

Resource Profiles dürfen aus historischen Messwerten angepasst werden.

Beispiel:

```text
Predicted Memory:
    256 MiB

Observed:
    420 MiB
```

Das System kann zukünftige Vorhersagen entsprechend korrigieren.

Harte Maximum-Grenzen dürfen dadurch nicht automatisch verändert werden.

## Resource Debt

NovaOS DARF temporäre Ressourcenüberschreitungen als Resource Debt modellieren.

Beispiel:

Eine interaktive Anwendung darf kurzfristig mehr CPU verwenden als ihr langfristiges Soft Budget vorsieht.

Diese zusätzliche Nutzung kann später ausgeglichen werden.

Resource Debt ist ausschließlich für weiche Budgets zulässig.

Hard Budgets dürfen dadurch nicht umgangen werden.

## Burst Capacity

Ressourcen können Burst Capacity besitzen.

Beispiel:

```text
CPU {
    sustained = 10 %
    burst     = 50 %
    duration  = 200 ms
}
```

Dies ermöglicht schnelle interaktive Reaktionen, ohne langfristige Fairness aufzugeben.

## Credits

NovaOS DARF Credit-basierte Mechanismen für bestimmte Soft Budgets verwenden.

Credits können beispielsweise durch geringe Nutzung aufgebaut und später für Bursts verwendet werden.

Credits sind eine Policy-Technik und keine universelle systemweite Währung.

## Fairness

Resource Policies SOLLEN Fairness berücksichtigen können.

Fairness kann abhängig vom Kontext unterschiedlich definiert werden.

Beispiele:

- per User
- per Application
- per Capability
- weighted fairness
- proportional fairness
- deadline-aware fairness

Es existiert keine universelle Fairnessdefinition für sämtliche Ressourcen.

## Prioritäten

Prioritäten dürfen Ressourcenentscheidungen beeinflussen.

Beispiel:

```text
Priority {
    Interactive
    Foreground
    Background
    Maintenance
}
```

Priorität bedeutet jedoch nicht automatisch unbegrenzten Ressourcenzugriff.

## Priority Inversion

Ressourcenmechanismen müssen Priority Inversion berücksichtigen.

Dies betrifft nicht nur CPU Locks.

Beispiel:

Ein High-Priority-Task kann von einem Low-Priority-Task abhängig sein, der:

- Speicher hält,
- einen I/O-Request blockiert,
- einen GPU-Command besitzt,
- eine Capability kontrolliert.

NovaOS SOLL geeignete Mechanismen zur Erkennung und Begrenzung solcher Situationen bereitstellen.

## Foreground und Background

Foreground-Status kann eine Policy-Eingabe sein.

Er ist jedoch kein fundamentales Sicherheitsrecht.

Eine Foreground-Anwendung kann beispielsweise bevorzugte:

- CPU-Latenz
- I/O-Priorität
- GPU-Priorität

erhalten.

Sie darf dadurch jedoch keine Hard Budgets oder Security Policies umgehen.

## Systemdienste

Systemdienste nehmen ebenfalls an der Resource Economy teil.

Sie besitzen nicht automatisch unbegrenzte Ressourcen.

Kritische Dienste können jedoch garantierte Reserven erhalten.

Beispiel:

```text
Nova.Resilience {
    CPU guaranteed = 2 %
    Memory reserved = 64 MiB
}
```

Dadurch bleibt Recovery auch bei hoher Systemlast funktionsfähig.

## Emergency Reserve

NovaOS SOLL Ressourcen für kritische Systemfunktionen reservieren können.

Beispiele:

- Kernel
- Recovery
- Storage Commit
- Security
- Thermal Emergency
- Shutdown

Eine vollständig ausgelastete Anwendungsebene darf nicht sämtliche Ressourcen verbrauchen, die zur Wiederherstellung des Systems benötigt werden.

## Memory Emergency Reserve

Insbesondere Speicherverwaltung und Recovery können einen kleinen Emergency Memory Pool benötigen.

Dieser darf nicht für normale Anwendungen verfügbar sein.

Dadurch können auch unter kritischem Memory Pressure noch:

- Fehlerberichte
- Recovery Tasks
- Termination
- Cleanup

durchgeführt werden.

## Resource Reclamation

NovaOS MUSS Ressourcen zurückfordern können.

Mögliche Maßnahmen:

```text
Release Cache
Trim Working Set
Compress Memory
Migrate Workload
Reduce Quality
Throttle
Suspend
Terminate
```

Die konkrete Auswahl ist Policy-gesteuert.

## Reclamation Hierarchie

Reclamation SOLL möglichst mit den geringsten Auswirkungen beginnen.

Beispiel:

```text
Drop Reconstructable Cache
    ↓
Reduce Prefetch
    ↓
Reduce Background Work
    ↓
Throttle BestEffort Tasks
    ↓
Request Cooperative Release
    ↓
Suspend
    ↓
Terminate
```

Harte Realtime- oder Safety-Workloads können von bestimmten Reclamation-Schritten ausgenommen sein.

## Cooperative Reclamation

Anwendungen und Services SOLLEN auf Resource Pressure reagieren können.

Beispiel:

```text
MemoryPressure {
    level = High
}
```

Eine Anwendung kann daraufhin:

- Cache reduzieren
- Vorschauen entfernen
- Hintergrunddaten freigeben
- Qualität reduzieren

Dies ist gegenüber sofortigem erzwungenem Termination-Verhalten zu bevorzugen.

## Quality of Service

Workloads können Quality-of-Service-Anforderungen besitzen.

Beispiel:

```text
QoS {
    latency = Interactive
    throughput = Medium
    reliability = High
}
```

QoS ist eine semantische Eingabe für Resource Policies.

## Graceful Degradation

Execution Contracts können erlaubte Degradationsstufen definieren.

Beispiel:

```text
VideoPlayback {
    preferredResolution = 4K
    minimumResolution   = 1080p
}
```

Bei Ressourcenknappheit kann NovaOS kontrolliert von 4K auf 1080p wechseln.

Nicht erlaubt wäre:

```text
minimumResolution = 4K
```

und trotzdem stillschweigend 1080p zu verwenden.

## Resource Negotiation

Wenn ein gewünschter Contract nicht vollständig erfüllt werden kann, darf NovaOS eine alternative Konfiguration anbieten.

Beispiel:

```text
Requested:
    4K @ 120 FPS

Available:
    4K @ 60 FPS
    1440p @ 120 FPS
```

Die Auswahl kann:

- automatisch erfolgen, wenn erlaubt,
- dem Benutzer überlassen werden,
- von der Anwendung entschieden werden.

## Deterministic Mode

Im Deterministic Mode müssen Ressourcenentscheidungen reproduzierbarer werden.

Dazu können gehören:

- feste CPU-Zuordnung
- feste Memory Reservations
- feste Provider
- deaktivierte Resource Bursts
- deaktiviertes Work Stealing
- feste Scheduler Policies
- feste DVFS-Zustände

Adaptive Ressourcenoptimierung kann eingeschränkt oder deaktiviert werden.

## Realtime

Hard-Realtime-Workloads benötigen garantierte Ressourcen.

Beispiel:

```text
RealtimeContract {
    cpu {
        reservation = 2 cores
    }

    memory {
        pinned = 128 MiB
    }

    deadline = 1 ms
}
```

Best-Effort-Workloads dürfen solche Reservierungen nicht verdrängen.

## Safety

Safety-kritische Ressourcenanforderungen besitzen Vorrang vor normalen Optimierungszielen.

Eine Safety-Domain kann beispielsweise exklusive Hardware oder reservierte Ressourcen erhalten.

Diese dürfen nicht für opportunistische Workloads verwendet werden, wenn dadurch die Safety-Garantie gefährdet würde.

## Security

Ressourcenmanagement ist auch ein Sicherheitsmechanismus.

Resource Budgets begrenzen unter anderem:

- Denial of Service
- Fork Bombs
- Memory Exhaustion
- I/O Flooding
- GPU Starvation
- Network Flooding

Ressourcenrechte können Capability-basiert vergeben werden.

## Resource Capabilities

Bestimmte Ressourcen können explizite Capabilities erfordern.

Beispiel:

```text
Capability {
    resource = GPU0
    rights = [
        Execute,
        AllocateMemory
    ]
}
```

Eine Ressource darf nicht allein deshalb verwendet werden, weil sie physisch vorhanden ist.

## Capability Attenuation

Resource Capabilities können abgeschwächt delegiert werden.

Beispiel:

```text
Parent:
    GPU <= 40 %

Child:
    GPU <= 10 %
```

Der Child-Kontext kann dadurch nicht mehr Rechte erhalten als der Parent delegiert.

## Trust

Provider mit unterschiedlichen Trust Levels können unterschiedliche Ressourcenrechte erhalten.

Ein nicht vertrauenswürdiger Provider kann beispielsweise:

- geringere Budgets,
- stärkere Isolation,
- keine direkte DMA-Berechtigung

erhalten.

Trust beeinflusst damit die Resource Policy.

## Data Sovereignty

Location-sensitive Ressourcen müssen Data-Sovereignty-Anforderungen berücksichtigen.

Beispiel:

```text
RemoteGPU:
    location = US

Contract:
    sovereignty = GermanyOnly
```

Der RemoteGPU-Provider ist damit kein gültiger Kandidat, unabhängig von seiner Performance.

## Location Transparency

Location Transparency darf Ressourcenentfernung nicht unsichtbar machen.

Ein Remote Provider besitzt andere Ressourcencharakteristiken als ein lokaler Provider.

Beispiel:

```text
LocalGPU:
    latency = 2 ms

RemoteGPU:
    latency = 30 ms
    network = 200 MiB/s
```

Location Transparency bedeutet deshalb nicht:

```text
Location has no cost.
```

sondern:

```text
Location does not change
the semantic capability interface,
but remains visible to planning.
```

## Zero-Copy

Zero-Copy reduziert Ressourcenverbrauch.

Es kann insbesondere reduzieren:

- Memory Bandwidth
- CPU Time
- Energy
- Latency

Die Resource Economy SOLL diese Effekte berücksichtigen können.

Shared Buffers müssen dabei korrekt abgerechnet werden.

## Transaktionale Systemoperationen

Transaktionen können Ressourcen reservieren.

Beispiel:

```text
UpdateTransaction {
    storage = 4 GiB
    memory  = 256 MiB
}
```

Eine Transaktion soll nicht beginnen, wenn ihre sicheren Mindestressourcen offensichtlich nicht verfügbar sind.

Nach:

- Commit
- Rollback
- Abort

müssen temporäre Reservations kontrolliert freigegeben werden.

## Hot Replacement

Hot Replacement kann temporär mehr Ressourcen benötigen, da alte und neue Komponente parallel existieren.

Beispiel:

```text
OldService:
    200 MiB

NewService:
    240 MiB

Temporary Requirement:
    440 MiB
```

Die Resource Economy muss diesen Übergang berücksichtigen.

## Self-Healing

Nova.Resilience benötigt garantierte Mindestressourcen.

Recovery darf nicht daran scheitern, dass ein fehlerhafter Workload sämtliche Ressourcen verbraucht.

Resource Economy und Self-Healing arbeiten deshalb zusammen.

Beispiel:

```text
Resource Exhaustion
    ↓
Detect
    ↓
Classify
    ↓
Reclaim
    ↓
Throttle
    ↓
Isolate offender
    ↓
Recover
```

## Resource Leaks

NovaOS SOLL Resource Leaks erkennen können.

Beispiele:

- Memory Leak
- unreleased GPU Buffer
- offene Handles
- nicht freigegebene Reservations
- orphaned Shared Memory
- verlorene Device Resources

Structured Concurrency und Capability Lifetimes sollen die automatische Bereinigung erleichtern.

## Resource Lifetime

Ressourcen sollen klare Lifetimes besitzen.

Beispiel:

```text
Task Lifetime
Application Lifetime
Session Lifetime
Transaction Lifetime
Persistent Lifetime
```

Temporäre Ressourcen sollten automatisch freigegeben werden können, wenn ihr Owner-Lifetime endet.

## Resource Ownership

Für Ressourcen MUSS ein Ownership-Modell existieren.

Ownership kann unterschieden werden von:

- Access
- Mapping
- Reservation
- Accounting Responsibility

Beispiel:

Ein Shared Buffer kann einem Service gehören, aber in mehreren Anwendungen gemappt sein.

## Resource Transfer

Ownership oder Nutzungsrechte dürfen kontrolliert übertragen werden.

Beispiel:

```text
Producer
    │
    │ Transfer Buffer Capability
    ▼
Consumer
```

Dies unterstützt Zero-Copy-Pipelines.

## Resource Revocation

Resource Capabilities und Reservations müssen widerrufbar sein, sofern ihre Semantik dies erlaubt.

Ein Hard-Realtime-Contract kann beispielsweise andere Revocation-Regeln besitzen als ein Best-Effort-Cache.

## Resource Leases

Zeitlich begrenzte Ressourcenrechte können als Leases modelliert werden.

Beispiel:

```text
Lease {
    GPU = 20 %
    duration = 5 s
}
```

Nach Ablauf wird das Recht:

- erneuert,
- reduziert,
- oder freigegeben.

## Preemption

Bestimmte Ressourcen können preemptiv neu verteilt werden.

Beispiele:

- CPU
- GPU Scheduling
- Netzwerkbandbreite

Andere Ressourcen können nicht ohne Weiteres preempted werden.

Beispiel:

- physisch belegter Storage
- gepinnter DMA Buffer

Das Ressourcenmodell muss diese Unterschiede berücksichtigen.

## Revocability

Ressourcentypen SOLLEN deklarieren können, ob sie:

```text
Preemptible
Revocable
Migratable
Shareable
Exclusive
Persistent
```

sind.

Diese Eigenschaften beeinflussen Scheduling und Recovery.

## Resource Topology

Die physische Topologie von Ressourcen muss berücksichtigt werden können.

Beispiele:

- NUMA
- CPU Cache Topology
- GPU Local Memory
- PCIe Topology
- Chiplet Topology
- Accelerator Proximity

Zwei nominell identische Ressourcen können unterschiedliche Kosten besitzen.

## NUMA

NUMA-Lokalität SOLL Teil der Ressourcenplanung sein.

Beispiel:

```text
Task:
    CPU = Node0
    Memory = Node0
```

ist häufig günstiger als:

```text
Task:
    CPU = Node0
    Memory = Node1
```

Die automatische Planung darf solche Unterschiede berücksichtigen.

## Heterogene CPUs

NovaOS muss heterogene CPU-Kerne modellieren können.

Beispiele:

```text
Performance Core
Efficiency Core
Realtime Core
```

Die Resource Economy darf CPU-Kerne nicht zwingend als identische Einheiten behandeln.

## Dynamic Capacity

Ressourcenkapazität kann sich dynamisch ändern.

Beispiele:

- Thermal Throttling
- Battery State
- CPU Frequency
- Network Conditions
- Device Removal
- VM Migration
- Hardware Failure

Die Resource Economy muss solche Änderungen berücksichtigen.

## Capacity Events

Kapazitätsänderungen SOLLEN Ereignisse erzeugen können.

Beispiel:

```text
GPU0.CapacityChanged
```

oder:

```text
MemoryPressureChanged
```

Policies können darauf reagieren.

## Rebalancing

Bei veränderter Kapazität darf NovaOS Workloads neu verteilen.

Beispiel:

```text
GPU Thermal Pressure
    ↓
Move eligible compute to NPU
```

Eine Migration ist nur zulässig, wenn:

- Execution Contract,
- Trust,
- Data Sovereignty,
- Determinism,
- Capability Requirements

weiterhin erfüllt werden.

## Resource Broker

NovaOS SOLL eine gemeinsame Resource-Broker-Abstraktion besitzen.

Der Resource Broker koordiniert:

- Resource Discovery
- Resource Contracts
- Reservations
- Budgets
- Accounting
- Pressure
- Introspection

Dies bedeutet nicht, dass sämtliche Entscheidungen durch einen einzelnen Prozess ausgeführt werden.

Der Broker kann logisch einheitlich und physisch verteilt sein.

## Verteilte Ressourcen

Remote-Ressourcen können in die Resource Economy integriert werden.

Beispiele:

- Remote Compute
- Remote GPU
- Cluster Storage
- Network Services

Remote-Ressourcen müssen zusätzliche Eigenschaften besitzen können:

```text
Location
Network Cost
Trust
Availability
Sovereignty
Failure Domain
```

## Distributed Resource Economy

Bei mehreren NovaOS-Nodes können Ressourcen über Node-Grenzen hinweg geplant werden.

Das lokale System behält jedoch seine eigenen Safety- und Recovery-Reserven.

Ein entfernter Node darf nicht als garantiert verfügbar behandelt werden, wenn keine entsprechende Garantie existiert.

## Cloud-Ressourcen

Cloud-Ressourcen sind Provider innerhalb des allgemeinen Ressourcenmodells.

Sie sind keine Sonderwelt.

Beispiel:

```text
Provider:
    RemoteGPU

Resources:
    Compute
    Network
    Cost
    Energy
```

Zusätzliche Policies können:

- Monetary Cost
- Privacy
- Sovereignty
- Trust

berücksichtigen.

## Kostenmodell

NovaOS darf Ressourcen Kostenwerte zuordnen.

Kosten können sein:

- Zeit
- Energie
- thermische Belastung
- Netzwerkverkehr
- monetäre Kosten
- Ressourcenknappheit

Es existiert jedoch keine verpflichtende universelle Einheitswährung.

## Shadow Price

Policies DÜRFEN interne Shadow Prices verwenden, um Ressourcenknappheit zu bewerten.

Beispiel:

```text
Memory abundant:
    low shadow cost

Memory critical:
    high shadow cost
```

Dadurch kann ein Planner bei Memory Pressure automatisch speichersparende Algorithmen bevorzugen.

Shadow Prices sind interne Policyinstrumente und keine Benutzerwährung.

## Resource Scoring

Provider können anhand mehrerer Dimensionen bewertet werden.

Beispiel:

```text
Score =
    Performance
  + EnergyEfficiency
  + Locality
  + Availability
  - ThermalCost
  - MemoryPressure
```

Harte Constraints werden jedoch vor einem solchen Scoring geprüft.

Ein hoher Score darf keinen ungültigen Provider legitimieren.

## Policy-Reihenfolge

Das allgemeine Entscheidungsprinzip lautet:

```text
1. Hard Constraints prüfen
2. Ungültige Kandidaten entfernen
3. Garantien prüfen
4. Ressourcenverfügbarkeit prüfen
5. Soft Goals optimieren
6. Entscheidung treffen
7. Ressourcen reservieren
8. Ausführung überwachen
```

## Resource Policy

Resource Policies bestimmen beispielsweise:

- Fairness
- Prioritäten
- Reservations
- Bursts
- Reclamation
- Throttling
- Providerwahl
- Admission Control

Mechanismen und Policies bleiben entsprechend ADR-ARCH-0002 getrennt.

## Resource Policy Scope

Policies können unterschiedliche Scopes besitzen:

```text
System
User
Session
Application
Service
Capability
ExecutionDomain
Device
```

Eine untergeordnete Policy darf eine übergeordnete harte Systemgrenze nicht umgehen.

## Benutzerpräferenzen

Benutzer können weiche Ressourcenpräferenzen definieren.

Beispiele:

```text
PreferBatteryLife
PreferPerformance
PreferQuiet
Balanced
```

Diese Einstellungen werden als Policy-Eingaben verwendet.

Sie überschreiben keine:

- Safety Requirements
- Security Requirements
- Hard Realtime Requirements
- Data-Sovereignty Requirements

## Anwendungspräferenzen

Anwendungen können Ressourcenpräferenzen angeben.

Beispiel:

```text
ApplicationPreference {
    performance = High
}
```

Dies ist keine Garantie.

Eine Anwendung erhält nur Ressourcen innerhalb ihrer zulässigen Budgets und Policies.

## Throttling

NovaOS kann Workloads throttlen.

Beispiele:

- CPU Throttling
- I/O Throttling
- Network Throttling
- GPU Throttling

Throttling soll möglichst kontrolliert und introspektierbar erfolgen.

## Suspension

Bei starkem Resource Pressure können geeignete Best-Effort-Workloads suspendiert werden.

Dabei müssen:

- Zustand
- Capabilities
- Ressourcen
- Wiederaufnahmebedingungen

klar definiert sein.

## Termination

Termination ist eine letzte mögliche Reclamation-Maßnahme.

Die Auswahl eines zu beendenden Workloads muss Policy-gesteuert erfolgen.

NovaOS soll nicht ausschließlich anhand des größten aktuellen Speicherverbrauchers entscheiden.

Zu berücksichtigen sind beispielsweise:

- Kritikalität
- Reconstructability
- User Impact
- Resource Consumption
- Trust
- Recovery Cost
- Current Activity

## OOM-Verhalten

NovaOS soll klassische unstrukturierte Out-of-Memory-Situationen soweit möglich durch:

- Admission Control
- Budgets
- Pressure Signaling
- Cooperative Reclamation
- Reservations
- Emergency Reserves

früher kontrollieren.

Ein OOM-Termination-Mechanismus bleibt als letzte Schutzmaßnahme möglich.

## Resource Starvation

NovaOS SOLL langfristige Resource Starvation erkennen können.

Ein Workload darf nicht dauerhaft keine Ressourcen erhalten, obwohl sein Contract eine Mindestversorgung vorsieht.

Best-Effort-Workloads besitzen dagegen keine solche Garantie.

## Architecture Introspection

Die Resource Economy MUSS introspektierbar sein.

NovaOS soll beispielsweise darstellen können:

```text
Application:
    MediaEditor

CPU:
    Budget      = 25 %
    Current     = 18 %
    Peak        = 24 %

Memory:
    Budget      = 2 GiB
    Current     = 1.4 GiB

GPU:
    Current     = 32 %

Energy:
    CurrentRate = 14 W

Thermal:
    GPUHeadroom = Medium
```

## Capability Introspection

Ressourcen sollen auch pro Capability analysiert werden können.

Beispiel:

```text
Capability:
    de.nova.video.decode.h265

Provider:
    HardwareDecoder0

Resources:
    CPU        = 2 %
    Memory     = 80 MiB
    Accelerator = 42 %
    Energy     = 3 W
```

## Decision Tracing

NovaOS SOLL Ressourcenentscheidungen erklären können.

Beispiel:

```text
Operation:
    AIInference

Selected:
    NPU0

Rejected:
    GPU0

Reason:
    GPU thermal pressure = Critical

Contract:
    latency <= 20 ms

NPU predicted latency:
    14 ms
```

## Observability

Resource Economy soll in die systemweite Observability integriert werden.

Mögliche Metriken:

- Allocation
- Reservation
- Consumption
- Pressure
- Reclamation
- Throttling
- Admission Failure
- Deadline Miss
- Budget Violation
- Prediction Error
- Provider Migration

## Historical Data

Historische Ressourcenwerte dürfen für:

- Capacity Planning
- Adaptive Optimization
- Anomaly Detection
- Prediction
- Debugging

verwendet werden.

Retention und Privacy müssen durch Policies begrenzt werden.

## Privacy

Ressourcenverbrauch kann Rückschlüsse auf Benutzeraktivität ermöglichen.

Deshalb müssen detaillierte Resource-Metriken Zugriffskontrollen besitzen.

Eine Anwendung darf nicht automatisch den detaillierten Ressourcenverbrauch anderer Anwendungen beobachten.

## Security gegen Resource Side Channels

Resource Introspection muss Side-Channel-Risiken berücksichtigen.

Hochauflösende Informationen über:

- CPU Timing
- Cache Pressure
- Memory Access
- GPU Activity

können sicherheitsrelevant sein.

Granularität und Zugriff müssen deshalb Capability- und Policy-gesteuert sein.

## Fail-Safe-Verhalten

Ein Ausfall höherer Resource-Policy-Komponenten darf fundamentale Ressourcenverwaltung nicht deaktivieren.

Der Kernel und kritische Runtime-Komponenten müssen konservative Fallback-Policies besitzen können.

Beispiel:

```text
AdaptiveResourcePolicy unavailable
        ↓
StaticSafeResourcePolicy
```

## Boot

Bereits während des Bootvorgangs existieren Ressourcenbudgets.

Frühe Bootphasen können ein vereinfachtes Ressourcenmodell verwenden.

Nach Initialisierung der vollständigen Resource Economy werden bestehende Ressourcen in das systemweite Modell übernommen.

## Recovery

NovaDOS und andere Recovery-Umgebungen dürfen ein vereinfachtes unabhängiges Ressourcenmodell verwenden.

Die Funktionsfähigkeit der Offline-Recovery darf nicht von der vollständigen NovaOS Resource Economy abhängen.

## Normative Festlegungen

1. NovaOS MUSS eine systemweite Ressourcenökonomie als grundlegendes Architekturprinzip verwenden.

2. Die Resource Economy MUSS mehrere Ressourcendimensionen gleichzeitig modellieren können.

3. Mindestens CPU, Memory, Storage I/O, Network, GPU, NPU, Energy und Thermal Headroom MÜSSEN integrierbar sein.

4. Zeitbezogene Anforderungen wie Latency und Deadline MÜSSEN in Ressourcenentscheidungen einbezogen werden können.

5. Neue Ressourcentypen MÜSSEN erweiterbar sein, ohne das gesamte Ressourcenmodell neu zu entwerfen.

6. Ressourcen MÜSSEN eindeutig identifizierbar sein.

7. Ressourcen SOLLEN hierarchisch modellierbar sein.

8. Ressourcenbudgets SOLLEN hierarchisch delegierbar sein.

9. Child-Budgets DÜRFEN Parent-Grenzen NICHT unkontrolliert umgehen.

10. Resource Contracts MÜSSEN Minimum-, Maximum- und Preferred-Anforderungen beschreiben können.

11. Guaranteed- und Best-Effort-Ressourcen MÜSSEN semantisch unterscheidbar sein.

12. Hard Budgets DÜRFEN NICHT durch Resource Debt oder Burst Mechanisms umgangen werden.

13. Soft Budgets DÜRFEN kontrollierte Bursts unterstützen.

14. NovaOS MUSS Ressourcenreservierungen unterstützen können.

15. Reservations MÜSSEN einen definierten Scope und Lifecycle besitzen.

16. Garantierte Ressourcenanforderungen SOLLEN vor Ausführungsbeginn Admission Control durchlaufen.

17. Eine Operation DARF NICHT als garantiert akzeptiert werden, wenn ihre garantierten Mindestanforderungen nicht erfüllt werden können.

18. Degradation DARF nur verwendet werden, wenn sie durch den Execution Contract oder eine autorisierte Policy erlaubt ist.

19. NovaOS MUSS Resource Pressure modellieren können.

20. Subsysteme SOLLEN Pressure Signals veröffentlichen können.

21. Ressourcenverbrauch MUSS grundsätzlich messbar und zurechenbar sein.

22. Accounting SOLL hierarchisch aggregierbar sein.

23. Ressourcenverbrauch SOLL Capability-basiert zurechenbar sein.

24. Structured Concurrency SOLL mit Ressourcenbudgets und Lifetimes integriert werden.

25. Child Tasks DÜRFEN Ressourcenlimits ihres Parent-Kontexts NICHT unkontrolliert umgehen.

26. Temporäre Ressourcen SOLLEN beim Ende ihres Owner-Lifetimes automatisch freigegeben werden können.

27. Shared Memory MUSS so abgerechnet werden können, dass physischer Verbrauch nicht unkontrolliert mehrfach gezählt wird.

28. GPU-Ressourcen MÜSSEN in das allgemeine Ressourcenmodell integrierbar sein.

29. NPU- und andere Accelerator-Ressourcen MÜSSEN integrierbar sein.

30. Power und Energy MÜSSEN semantisch getrennt behandelt werden.

31. Thermal Headroom MUSS als Planungsgröße verwendbar sein.

32. Deadline Capacity MUSS bei garantierten Realtime-Anforderungen berücksichtigt werden können.

33. Latency Budgets SOLLEN entlang von Ausführungsketten propagierbar sein.

34. Resource Planning MUSS multidimensionale Ressourcenanforderungen berücksichtigen können.

35. NovaOS DARF KEINE verpflichtende universelle Ressourceneinheit voraussetzen.

36. Providerwahl SOLL Ressourcenprofile berücksichtigen.

37. Algorithmuswahl SOLL Ressourcenprofile berücksichtigen können.

38. Provider und Algorithmen SOLLEN Resource Profiles veröffentlichen können.

39. Resource Profiles DÜRFEN adaptiv aus Messwerten verbessert werden.

40. Prediction Error DARF zur Verbesserung von Ressourcenprognosen verwendet werden.

41. Vorhersagen DÜRFEN harte Ressourcenlimits NICHT ersetzen.

42. Adaptive Resource Policies DÜRFEN Hard Constraints NICHT verletzen.

43. Realtime-Workloads MÜSSEN garantierte Ressourcen reservieren können.

44. Best-Effort-Workloads DÜRFEN garantierte Realtime-Ressourcen NICHT verdrängen.

45. Safety-kritische Ressourcenreservierungen MÜSSEN Vorrang vor opportunistischen Optimierungen besitzen.

46. Kritische Systemdienste SOLLEN garantierte Mindestressourcen erhalten können.

47. NovaOS SOLL Emergency Reserves für kritische Recovery- und Systemfunktionen bereitstellen können.

48. Normale Anwendungen DÜRFEN Emergency Reserves NICHT verbrauchen.

49. NovaOS MUSS Ressourcen zurückfordern können.

50. Reclamation SOLL möglichst mit den am wenigsten destruktiven Maßnahmen beginnen.

51. Anwendungen und Dienste SOLLEN auf Resource Pressure kooperativ reagieren können.

52. Termination SOLL als letzte Reclamation-Maßnahme betrachtet werden.

53. OOM-Situationen SOLLEN soweit möglich durch Admission Control, Budgets und Pressure Management frühzeitig kontrolliert werden.

54. Resource Capabilities MÜSSEN Least-Privilege unterstützen.

55. Resource Capabilities SOLLEN attenuierbar und delegierbar sein.

56. Ressourcenrechte DÜRFEN zeitlich begrenzte Leases verwenden.

57. Ressourcentypen SOLLEN Eigenschaften wie Preemptible, Revocable, Migratable, Shareable und Exclusive beschreiben können.

58. Ressourcenplanung SOLL Hardwaretopologie berücksichtigen können.

59. NUMA-Lokalität SOLL als Ressourcenfaktor berücksichtigt werden können.

60. Heterogene CPU-Kerne DÜRFEN NICHT zwingend als identische Ressourcen behandelt werden.

61. Dynamische Kapazitätsänderungen MÜSSEN im Ressourcenmodell abbildbar sein.

62. Kapazitätsänderungen SOLLEN Events erzeugen können.

63. NovaOS DARF Workloads bei Kapazitätsänderungen neu verteilen.

64. Migration DARF Execution Contracts, Security, Trust, Sovereignty oder Determinism NICHT verletzen.

65. Remote-Ressourcen MÜSSEN Location, Network Cost, Trust und Failure Domain beschreiben können.

66. Data-Sovereignty-Anforderungen MÜSSEN bei Remote-Ressourcen berücksichtigt werden.

67. Location Transparency DARF Ressourcenentfernung und deren Kosten NICHT vor dem Planner verbergen.

68. Zero-Copy-Effekte SOLLEN in Ressourcenentscheidungen berücksichtigt werden können.

69. Transaktionen SOLLEN Ressourcen reservieren können.

70. Transaktionsressourcen MÜSSEN nach Commit, Rollback oder Abort freigegeben werden können.

71. Hot Replacement MUSS temporären Parallelverbrauch alter und neuer Komponenten berücksichtigen können.

72. Self-Healing MUSS auch unter Resource Pressure funktionsfähig bleiben können.

73. Resource Leaks SOLLEN erkannt werden können.

74. Resource Ownership MUSS von Access, Mapping und Accounting Responsibility unterscheidbar sein.

75. Resource Rights SOLLEN kontrolliert übertragbar sein.

76. Resource Revocation MUSS dort unterstützt werden können, wo die Ressourcensemantik dies zulässt.

77. Resource Policies MÜSSEN von fundamentalen Resource Mechanisms getrennt werden.

78. Eine untergeordnete Resource Policy DARF übergeordnete harte Systemgrenzen NICHT umgehen.

79. Benutzerpräferenzen DÜRFEN Resource Policies beeinflussen.

80. Benutzerpräferenzen DÜRFEN Safety-, Security-, Realtime- oder Sovereignty-Anforderungen NICHT überschreiben.

81. Ressourcenentscheidungen MÜSSEN über Architecture Introspection sichtbar gemacht werden können.

82. Automatische Ressourcenentscheidungen SOLLEN über Decision Tracing erklärbar sein.

83. Resource Accounting MUSS in die systemweite Observability integrierbar sein.

84. Zugriff auf detaillierte Resource-Metriken MUSS Capability- und Policy-gesteuert sein.

85. Resource Introspection MUSS Side-Channel-Risiken berücksichtigen.

86. Ein Ausfall adaptiver Resource Policies DARF die fundamentale Ressourcenverwaltung NICHT funktionsunfähig machen.

87. Kritische Resource Mechanisms MÜSSEN konservative Fallback-Policies verwenden können.

88. Die vollständige Resource Economy DARF NICHT Voraussetzung für die grundlegende Offline-Recovery durch NovaDOS sein.

89. KI-basierte Ressourcenoptimierung DARF NICHT alleinige Grundlage für harte Ressourcen-, Safety- oder Realtime-Garantien sein.

90. Die Resource Economy MUSS mit `Nova.ExecutionContract` integrierbar sein.

## Konsequenzen

### Positive Konsequenzen

- systemweite Sicht auf Ressourcen
- bessere Ressourcenplanung
- multidimensionale Providerwahl
- bessere automatische Algorithmuswahl
- kontrollierte Ressourcenbudgets
- bessere Realtime-Garantien
- bessere Energieeffizienz
- bessere Thermalsteuerung
- frühzeitiges Admission Control
- kontrollierter Umgang mit Ressourcenknappheit
- bessere Self-Healing-Fähigkeit
- Schutz kritischer Systemdienste
- bessere Fairness
- Capability-basiertes Ressourcenmanagement
- bessere Observability
- bessere Architecture Introspection
- nachvollziehbare Ressourcenentscheidungen
- Integration lokaler und entfernter Ressourcen
- bessere Unterstützung heterogener Hardware
- kontrollierte adaptive Optimierung

### Negative Konsequenzen

- deutlich komplexeres Ressourcenmodell
- zusätzliche Accounting-Kosten
- zusätzliche Policy-Infrastruktur
- Resource Profiles müssen gepflegt oder gelernt werden
- multidimensionale Optimierung ist komplexer als isoliertes Scheduling
- hierarchische Budgets benötigen klare Ownership-Regeln
- Reservations können Ressourcen ungenutzt blockieren
- falsche Ressourcenprognosen können zu suboptimalen Entscheidungen führen
- detaillierte Telemetrie kann Privacy- und Side-Channel-Risiken erzeugen
- Interaktionen zwischen CPU-, Memory-, GPU-, Energie- und Thermal-Policies müssen sorgfältig abgestimmt werden

## Verworfene Alternativen

### Vollständig getrennte Ressourcenmanager

Nicht übernommen.

CPU, Memory, GPU, Netzwerk, Energie und Thermal ausschließlich unabhängig voneinander zu optimieren kann zu systemweit schlechten Entscheidungen führen.

Die spezialisierten Manager bleiben bestehen, werden jedoch in ein gemeinsames Ressourcenmodell eingebunden.

### Ein einziger globaler Scheduler

Nicht übernommen.

Ein zentraler Scheduler für sämtliche Ressourcen würde:

- zu starke Kopplung erzeugen,
- Skalierbarkeit reduzieren,
- spezialisierte Hardwaremechanismen schlecht abbilden,
- eine große Failure Domain erzeugen.

NovaOS verwendet ein gemeinsames Ressourcenmodell mit spezialisierten Mechanismen und Policies.

### Universelle Ressourcenwährung

Nicht übernommen.

CPU-Zeit, Speicher, Energie und Latenz besitzen unterschiedliche Semantik und können nicht allgemein verlustfrei in eine einzige Einheit umgerechnet werden.

Interne Policy-Modelle dürfen Kostenfunktionen oder Shadow Prices verwenden.

### Ausschließlich reaktives Ressourcenmanagement

Nicht übernommen.

Erst auf Ressourcenknappheit zu reagieren, nachdem sie bereits eingetreten ist, reicht insbesondere für:

- Realtime
- Updates
- Hot Replacement
- Self-Healing

nicht aus.

NovaOS verwendet zusätzlich:

- Resource Contracts
- Reservations
- Admission Control
- Prediction

### Unbegrenzte Ressourcen für Systemdienste

Nicht übernommen.

Auch privilegierte Dienste können durch Fehler oder Leaks Ressourcen erschöpfen.

Sie nehmen deshalb grundsätzlich an der Resource Economy teil.

### Rein KI-basierte Ressourcenplanung

Nicht übernommen.

KI kann Vorhersagen und Optimierung verbessern.

Harte:

- Limits
- Reservations
- Security Constraints
- Safety Constraints
- Realtime Guarantees

müssen jedoch deterministisch überprüfbar bleiben.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip
- ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-EXECUTION-0001_Nova_ExecutionContract
- ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten
- ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur
- ADR-POWER-0001_Einheitliche_Energie_und_Thermikarchitektur

## Zugehörige NPSPECs

- NPSPEC-ARCH-RESOURCEECONOMY-0001
- NPSPEC-RESOURCE-MODEL-0001
- NPSPEC-RESOURCE-TYPE-0001
- NPSPEC-RESOURCE-IDENTITY-0001
- NPSPEC-RESOURCE-HIERARCHY-0001
- NPSPEC-RESOURCE-CONTRACT-0001
- NPSPEC-RESOURCE-BUDGET-0001
- NPSPEC-RESOURCE-RESERVATION-0001
- NPSPEC-RESOURCE-ADMISSION-0001
- NPSPEC-RESOURCE-PRESSURE-0001
- NPSPEC-RESOURCE-ACCOUNTING-0001
- NPSPEC-RESOURCE-OWNERSHIP-0001
- NPSPEC-RESOURCE-LIFETIME-0001
- NPSPEC-RESOURCE-TRANSFER-0001
- NPSPEC-RESOURCE-REVOCATION-0001
- NPSPEC-RESOURCE-LEASE-0001
- NPSPEC-RESOURCE-RECLAMATION-0001
- NPSPEC-RESOURCE-EMERGENCY-0001
- NPSPEC-RESOURCE-TOPOLOGY-0001
- NPSPEC-RESOURCE-NUMA-0001
- NPSPEC-RESOURCE-CPU-0001
- NPSPEC-RESOURCE-MEMORY-0001
- NPSPEC-RESOURCE-STORAGE-0001
- NPSPEC-RESOURCE-NETWORK-0001
- NPSPEC-RESOURCE-GPU-0001
- NPSPEC-RESOURCE-NPU-0001
- NPSPEC-RESOURCE-ENERGY-0001
- NPSPEC-RESOURCE-THERMAL-0001
- NPSPEC-RESOURCE-TIME-0001
- NPSPEC-RESOURCE-DEADLINE-0001
- NPSPEC-RESOURCE-PROFILE-0001
- NPSPEC-RESOURCE-PREDICTION-0001
- NPSPEC-RESOURCE-INTROSPECTION-0001
- NPSPEC-RESOURCE-DECISIONTRACE-0001
- NPSPEC-EXECUTION-CONTRACT-0001

## Ergebnis

NovaOS behandelt Ressourcen nicht als voneinander isolierte technische Subsysteme, sondern als Bestandteile einer gemeinsamen systemweiten Ressourcenökonomie.

Das grundlegende Modell lautet:

```text
Execution Intent
      ↓
Execution Contract
      ↓
Resource Requirements
      ↓
Hard Constraints
      ↓
Admission Control
      ↓
Resource Planning
      ↓
Reservation / Allocation
      ↓
Execution
      ↓
Accounting
      ↓
Pressure / Feedback
      ↓
Adaptation
```

CPU, Memory, Storage, Netzwerk, GPU, NPU, Energie, Thermal Headroom, Latenz und Deadlines können dadurch gemeinsam in Systementscheidungen einfließen.

Spezialisierte Scheduler und Ressourcenmanager bleiben erhalten, arbeiten jedoch auf Grundlage eines gemeinsamen semantischen Ressourcenmodells.

Damit bildet die systemweite Ressourcenökonomie gemeinsam mit:

```text
Declarative System Model
        +
Execution Contracts
        +
Mechanism / Policy Separation
        +
Structured Concurrency
        +
Capability Security
        +
Architecture Introspection
```

die Grundlage für ein ressourcenbewusstes, adaptives, deterministisches und langfristig evolvierbares NovaOS.