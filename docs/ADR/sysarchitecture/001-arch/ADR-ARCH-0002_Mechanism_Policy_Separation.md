# ADR-ARCH-0002 – Mechanism / Policy Separation

## Status

Angenommen

## Kategorie

Systemarchitektur / Kernelarchitektur

## Kontext

NovaOS soll als langfristig evolvierbares Betriebssystem aufgebaut werden, dessen grundlegende Systemmechanismen nicht unnötig an konkrete, kurzfristig veränderliche Policies gekoppelt sind.

Viele klassische Betriebssystemkomponenten vermischen Mechanismus und Policy.

Ein Mechanismus beschreibt, **was das System technisch ermöglichen kann**.

Eine Policy beschreibt, **wie, wann und unter welchen Bedingungen ein Mechanismus verwendet wird**.

Beispiele:

```text
Mechanismus:
    CPU-Task priorisieren

Policy:
    Interaktive Tasks bevorzugen
```

oder:

```text
Mechanismus:
    Speicherseite auslagern

Policy:
    Welche Seite wird wann ausgelagert?
```

oder:

```text
Mechanismus:
    Komponente auf GPU ausführen

Policy:
    Soll GPU, CPU, NPU oder ein Remote Provider verwendet werden?
```

Eine starke Kopplung beider Ebenen erschwert:

- Weiterentwicklung
- Austausch von Algorithmen
- adaptive Optimierung
- unterschiedliche Nutzungsszenarien
- Deterministic Mode
- Echtzeitbetrieb
- Energieoptimierung
- Sicherheitsrichtlinien
- neue Hardware
- unterschiedliche Geräteklassen
- Hot Replacement
- Architecture Introspection

NovaOS benötigt deshalb eine systemweite Trennung zwischen Mechanismen und Policies.

Diese Trennung gilt nicht nur für den Kernel, sondern für die gesamte Systemarchitektur.

## Entscheidung

NovaOS verwendet **Mechanism / Policy Separation als systemweites Architekturprinzip**.

Fundamentale Komponenten stellen möglichst allgemeine, sichere und klar definierte Mechanismen bereit.

Die konkrete Auswahl, Gewichtung und Steuerung dieser Mechanismen erfolgt durch explizite Policies.

Das Grundmodell lautet:

```text
Intent / Requirement
        │
        ▼
Policy
        │
        ▼
Decision
        │
        ▼
Mechanism
        │
        ▼
Execution
```

Ein Mechanismus SOLL möglichst wenig Wissen darüber enthalten, warum er verwendet wird.

Eine Policy SOLL dagegen keine fundamentalen Hardware- oder Kernelmechanismen selbst implementieren müssen.

## Definition Mechanismus

Ein Mechanismus ist eine technische Fähigkeit des Systems.

Beispiele:

- Thread erzeugen
- Task blockieren
- Task aufwecken
- CPU-Affinität setzen
- Deadline registrieren
- Speicher reservieren
- Speicher mappen
- Speicher schützen
- Shared Memory erzeugen
- Capability delegieren
- Nachricht übertragen
- DMA-Buffer bereitstellen
- Gerät in Power State versetzen
- Netzwerkpaket übertragen
- Snapshot erzeugen
- Transaktion committen
- Prozess isolieren
- MicroVM starten
- Provider aktivieren
- Komponente migrieren

Mechanismen definieren primär:

```text
Was ist technisch möglich?
```

## Definition Policy

Eine Policy entscheidet, wie ein vorhandener Mechanismus verwendet wird.

Beispiele:

- welcher Task zuerst ausgeführt wird
- welche CPU verwendet wird
- wie viel CPU-Zeit ein Workload erhält
- wann Speicher ausgelagert wird
- welcher Algorithmus verwendet wird
- welcher Capability Provider gewählt wird
- welcher Netzwerkpfad verwendet wird
- welche Isolation Domain erforderlich ist
- wann ein Gerät schlafen darf
- wann eine Komponente migriert wird
- welche Recovery-Aktion durchgeführt wird
- wann ein Update aktiviert wird

Policies beantworten primär:

```text
Welche der technisch möglichen Aktionen
ist unter den aktuellen Anforderungen
die geeignete?
```

## Grundprinzip

NovaOS trennt:

```text
Mechanism
    ≠
Policy
```

und gleichzeitig:

```text
Policy
    ≠
Implementation
```

Eine Policy beschreibt eine Entscheidung oder Entscheidungsregel.

Sie soll nicht unnötig an eine konkrete Implementierung gebunden sein.

Beispiel:

```text
Policy:
    LowestEnergyProvider
```

kann auf unterschiedlichen Geräten zu unterschiedlichen Entscheidungen führen:

```text
Desktop:
    CPU

Notebook:
    NPU

Server:
    RemoteAccelerator
```

Der zugrunde liegende Mechanismus zur Provideraktivierung bleibt gleich.

## Architekturmodell

Die systemweite Struktur lautet:

```text
Application / System Component
            │
            ▼
        Intent
            │
            ▼
    Execution Contract
            │
            ▼
     Policy Engine
            │
            ▼
      Decision Plan
            │
            ▼
        Mechanism
            │
            ▼
         Runtime
            │
            ▼
        Hardware
```

Policies können dabei mehrere Systemdimensionen gleichzeitig berücksichtigen.

## Policy Inputs

Eine Policy darf unter anderem folgende Informationen verwenden:

- Execution Contract
- Semantic Types
- Capability Requirements
- Trust Requirements
- Security Policy
- Data-Sovereignty Policy
- Resource Budget
- aktuelle Systemlast
- Hardwaretopologie
- Energiezustand
- Thermal Headroom
- Netzwerkzustand
- Deadline
- Latenzanforderungen
- Determinism Requirement
- Realtime Requirement
- historische Messwerte
- Benutzerpräferenzen
- administrative Vorgaben
- Recovery State
- Provider Health

Nicht jede Policy muss alle Informationen verwenden.

## Policy Hierarchie

Policies können auf unterschiedlichen Ebenen existieren.

Beispiel:

```text
System Policy
    │
    ├── Security Policy
    ├── Resource Policy
    ├── Power Policy
    ├── Scheduling Policy
    ├── Storage Policy
    ├── Network Policy
    └── Application Policy
```

Eine untergeordnete Policy DARF eine höher priorisierte harte Policy nicht verletzen.

## Policy-Prioritäten

NovaOS unterscheidet zwischen harten und weichen Anforderungen.

Eine mögliche Priorisierung lautet:

```text
Safety
    ↓
Security
    ↓
Data Sovereignty
    ↓
Hard Realtime
    ↓
Determinism
    ↓
Correctness
    ↓
Explicit User Requirement
    ↓
Resource Budget
    ↓
Performance
    ↓
Energy Efficiency
    ↓
Adaptive Optimization
    ↓
Preference
```

Diese Reihenfolge ist kein universeller statischer Scheduler.

Sie beschreibt das Prinzip, dass eine Optimierung keine höher priorisierte harte Anforderung verletzen darf.

Beispiel:

Eine energieeffizientere Ausführung darf nicht gewählt werden, wenn dadurch eine Hard-Realtime-Deadline verletzt würde.

## Kernel

Im Kernel stellt Mechanism / Policy Separation sicher, dass fundamentale Mechanismen nicht unnötig mit konkreten Strategien verschmolzen werden.

Beispiel Scheduling:

```text
Mechanismen:
    EnqueueTask()
    DequeueTask()
    BlockTask()
    WakeTask()
    SetAffinity()
    SetDeadline()
    ContextSwitch()
```

Darüber können unterschiedliche Scheduling Policies arbeiten:

```text
Fair
Realtime
Deadline
EnergyAware
LatencyOptimized
Deterministic
```

Die genaue Implementierung darf aus Performancegründen optimierte gemeinsame Datenstrukturen verwenden.

Die logische Trennung zwischen Mechanismus und Policy bleibt trotzdem erhalten.

## Speicherverwaltung

Die Speicherverwaltung stellt Mechanismen bereit wie:

```text
Allocate
Map
Unmap
Protect
Pin
Share
Reclaim
Migrate
Evict
```

Eine Policy entscheidet beispielsweise:

```text
Welche Seite darf verdrängt werden?
Welche NUMA-Node soll verwendet werden?
Darf Speicher komprimiert werden?
Darf eine Seite migriert werden?
Wie viel Speicher erhält eine Domain?
```

Dadurch können unterschiedliche Memory Policies verwendet werden, ohne die grundlegende virtuelle Speicherarchitektur zu ersetzen.

## I/O

Die I/O-Architektur stellt Mechanismen bereit wie:

```text
Submit
Cancel
Complete
Prioritize
Batch
MapBuffer
ShareBuffer
```

Policies entscheiden beispielsweise:

- Reihenfolge von Requests
- Prioritäten
- Deadline-Behandlung
- Batching
- Energieoptimierung
- Geräteauswahl
- Queue-Auswahl

## Netzwerk

Die Netzwerkarchitektur stellt Mechanismen bereit für:

- Verbindungen
- Streams
- Datagramme
- Routing
- Multipath
- QoS
- Traffic Shaping
- Verschlüsselung
- Connection Migration

Policies entscheiden beispielsweise:

```text
Wi-Fi oder Ethernet?
IPv4 oder IPv6?
TCP oder QUIC?
Welcher Netzwerkpfad?
Welche QoS-Klasse?
Darf ein souveränitätskritischer Datenstrom diese Route verwenden?
```

## Storage

Storage-Mechanismen umfassen beispielsweise:

- Lesen
- Schreiben
- Flush
- Snapshot
- Clone
- Transaction
- Replication
- Encryption
- Integrity Verification

Policies entscheiden beispielsweise:

- Replikationsgrad
- Cache-Verhalten
- Storage Tier
- Kompression
- Verschlüsselungsprofil
- Snapshot-Frequenz
- Retention
- Recovery-Verhalten

## Capability-System

Das Capability-System stellt Mechanismen bereit für:

- Capability-Erzeugung
- Delegation
- Attenuation
- Revocation
- Lifetime
- Scope
- Transfer

Security Policies entscheiden, ob eine Capability ausgegeben oder delegiert werden darf.

Das Modell lautet:

```text
Policy
    ↓
Authorization Decision
    ↓
Capability Issuance
    ↓
Fast Capability Enforcement
```

Dadurch müssen komplexe Policies nicht bei jeder einzelnen Operation vollständig neu ausgewertet werden.

## Providerwahl

NovaOS abstrahiert viele Funktionen über Capabilities und Provider.

Beispiel:

```text
Capability:
    nova.compute.matrix.multiply
```

Mögliche Provider:

```text
CPUProvider
GPUProvider
NPUProvider
RemoteProvider
```

Der Mechanismus ermöglicht:

- Provider Discovery
- Provider Binding
- Provider Invocation
- Provider Rebinding

Eine Policy entscheidet, welcher Provider verwendet wird.

## Automatische Algorithmuswahl

NovaOS darf Algorithmen automatisch auswählen.

Beispiel:

```text
Sort(data)
```

Der Mechanismus stellt mehrere Implementierungen bereit.

Die Policy kann anhand von:

- Datengröße
- Datentyp
- Sortierzustand
- Speicherbudget
- Hardware
- Deadline
- Determinism Requirement

eine Implementierung auswählen.

Beispiel:

```text
Small data:
    InsertionSort

Mostly sorted:
    TimSort

Parallel workload:
    ParallelMergeSort
```

Eine explizite Entwicklerentscheidung darf die automatische Auswahl überschreiben:

```text
Sort(data, algorithm = TimSort)
```

Eine solche explizite Vorgabe wird Bestandteil des Execution Contracts beziehungsweise der Operation Requirements.

## Execution Contract

`Nova.ExecutionContract` bildet eine zentrale Verbindung zwischen Anforderungen und Policies.

Beispiel:

```text
ExecutionContract {
    latency       <= 5 ms
    determinism   = Required
    memory        <= 64 MiB
    energy        = PreferLow
    trust         >= Verified
    sovereignty   = LocalOnly
}
```

Der Contract beschreibt Anforderungen.

Er implementiert nicht selbst die Mechanismen.

Policies interpretieren den Contract und erzeugen daraus einen ausführbaren Plan.

## Decision Plan

Komplexe Policyentscheidungen SOLLEN als expliziter Decision Plan repräsentiert werden können.

Beispiel:

```text
DecisionPlan {
    provider      = NPU0
    algorithm     = QuantizedInferenceV3
    cpuBudget     = 5 %
    memoryBudget  = 128 MiB
    isolation     = ServiceDomain
    network       = Disabled
}
```

Der Plan kann anschließend durch die zugrunde liegenden Mechanismen umgesetzt werden.

## Policy Engine

NovaOS SOLL eine gemeinsame Policy-Infrastruktur bereitstellen.

Dies bedeutet nicht, dass sämtliche Entscheidungen durch einen einzelnen globalen Policy-Prozess ausgeführt werden müssen.

Stattdessen kann es spezialisierte Policy Engines geben.

Beispiele:

```text
Nova.Policy.Security
Nova.Policy.Resource
Nova.Policy.Compute
Nova.Policy.Network
Nova.Policy.Storage
Nova.Policy.Power
Nova.Policy.Resilience
```

Diese verwenden ein gemeinsames Modell für:

- Identität
- Priorität
- Scope
- Version
- Constraints
- Decision Tracing
- Introspection

## Policy Scope

Eine Policy MUSS einen definierten Scope besitzen.

Mögliche Scopes:

```text
System
User
Session
Application
Capability
ExecutionDomain
Device
Operation
```

Eine Policy darf nur innerhalb ihres autorisierten Scopes wirken.

## Policy Composition

Mehrere Policies können gleichzeitig auf dieselbe Operation wirken.

Beispiel:

```text
Security Policy:
    Remote execution allowed

Data Sovereignty Policy:
    Data must remain in Germany

Performance Policy:
    Prefer fastest provider

Energy Policy:
    Prefer lowest-energy provider
```

Die resultierende Entscheidung muss alle harten Constraints erfüllen.

Erst danach dürfen Optimierungsziele gegeneinander abgewogen werden.

## Constraint Solver

Komplexe Policyentscheidungen DÜRFEN als Constraint-Problem behandelt werden.

Beispiel:

```text
Candidates:
    CPU0
    GPU0
    CloudGPU

Constraints:
    latency <= 20 ms
    sovereignty = LocalOnly
    trust >= Verified

Optimization:
    minimize energy
```

Wenn `CloudGPU` die Sovereignty-Anforderung verletzt, wird dieser Provider bereits vor der Optimierung ausgeschlossen.

## Harte Constraints

Harte Constraints dürfen nicht aufgrund einer Optimierung verletzt werden.

Beispiele:

- Security
- Safety
- Data Sovereignty
- Hard Realtime
- explizite Verbote
- notwendige Trust Levels
- notwendige Capabilities
- Determinism Requirement

Kann kein gültiger Plan erzeugt werden, MUSS die Operation fehlschlagen oder eine explizit definierte Degradation verwenden.

NovaOS DARF nicht stillschweigend eine harte Anforderung ignorieren.

## Weiche Ziele

Sind mehrere gültige Lösungen vorhanden, dürfen weiche Ziele zur Auswahl verwendet werden.

Beispiele:

- Performance maximieren
- Energieverbrauch minimieren
- Netzwerkverkehr minimieren
- thermische Belastung minimieren
- Kosten minimieren
- Cache-Lokalität maximieren
- Benutzerpräferenz berücksichtigen

## Expliziter Override

NovaOS verwendet standardmäßig automatische Policyentscheidungen.

Entwickler und Benutzer können dort, wo dies vorgesehen ist, explizite Entscheidungen treffen.

Beispiel:

```text
Automatic:
    Sort(data)

Override:
    Sort(data, algorithm = TimSort)
```

oder:

```text
Automatic:
    Compute(task)

Override:
    Compute(task, provider = GPU0)
```

Ein Override darf jedoch keine höhere Sicherheits-, Safety- oder Sovereignty-Policy umgehen.

Das Modell lautet:

```text
Automatic Policy
        ↓
Explicit Preference
        ↓
Hard Constraints
        ↓
Final Decision
```

Harte Constraints bleiben bindend.

## Deterministic Mode

Im Deterministic Mode müssen Policyentscheidungen reproduzierbar sein.

Nichtdeterministische adaptive Entscheidungen können deshalb:

- deaktiviert
- eingefroren
- versioniert
- durch einen festen Plan ersetzt

werden.

Beispiel:

```text
Normal Mode:
    provider = Auto

Deterministic Mode:
    provider = CPUProviderV3
```

Auch Policyversionen und relevante Eingaben müssen bei deterministischer Reproduktion berücksichtigt werden.

## Realtime

Realtime Policies müssen harte Deadline-Anforderungen respektieren.

Eine Policy darf beispielsweise keine energiesparende CPU auswählen, wenn dadurch eine garantierte Deadline verletzt wird.

Realtime Requirements können außerdem:

- CPU-Reservierung
- Memory Pinning
- Interrupt-Affinität
- Provider Pinning
- feste Algorithmen
- feste Clock Policies

erzwingen.

## Resource Economy

Die systemweite Resource Economy verwendet Mechanism / Policy Separation.

Mechanismen erfassen und kontrollieren:

- CPU
- Memory
- I/O
- Network
- GPU
- NPU
- Energy
- Thermal Headroom

Policies entscheiden:

- Budgetverteilung
- Priorisierung
- Admission Control
- Throttling
- Reservation
- Reclamation
- Degradation

Dadurch kann die Ressourcenpolitik weiterentwickelt werden, ohne die fundamentalen Accounting-Mechanismen zu ersetzen.

## Power Management

Power-Mechanismen umfassen:

- C-States
- P-States
- DVFS
- Device Power States
- Core Parking
- Suspend
- Hibernate

Power Policies entscheiden:

- wann Zustände gewechselt werden
- welche Performanceziele gelten
- welche Energieziele gelten
- welche Geräte schlafen dürfen
- welche Wake Sources aktiv bleiben

Realtime- und Safety-Anforderungen können bestimmte Power-Optimierungen blockieren.

## Resilience

Nova.Resilience trennt Recovery-Mechanismen von Recovery-Policies.

Mechanismen umfassen beispielsweise:

- Restart
- Rollback
- Failover
- Restore
- Quarantine
- Rebind
- Reinitialize

Policies entscheiden:

- welche Recovery-Aktion geeignet ist
- welcher Recovery Scope verwendet wird
- wie oft ein Retry zulässig ist
- wann eskaliert wird
- wann ein Systemteil quarantänisiert wird

Beispiel:

```text
Fault:
    DriverTimeout

Possible Mechanisms:
    Retry
    RestartDriver
    ResetDevice
    FailoverDevice

Policy Decision:
    RestartDriver
```

## Updates

Das Update-System stellt Mechanismen bereit für:

- Download
- Verification
- Staging
- Activation
- Snapshot
- Rollback
- A/B Switching

Update Policies bestimmen:

- wann Updates installiert werden
- welche Rollout-Gruppe verwendet wird
- ob Canary Deployment notwendig ist
- welche Health Criteria gelten
- wann Rollback ausgelöst wird

## Data Sovereignty

Data-Sovereignty-Regeln werden als harte Policy Constraints behandelt.

Eine Compute- oder Netzwerkpolicy darf keine Route oder keinen Provider auswählen, der eine bindende Sovereignty Policy verletzt.

Beispiel:

```text
DataPolicy:
    LocalOnly

Candidates:
    LocalGPU
    GermanEdge
    USCloud

Allowed:
    LocalGPU

Rejected:
    GermanEdge
    USCloud
```

Falls die Policy tatsächlich `LocalOnly` fordert, ist auch ein externer Provider innerhalb desselben Landes nicht zulässig.

## Trust

Trust-Anforderungen können Policyentscheidungen einschränken.

Beispiel:

```text
ExecutionContract:
    trust >= SystemVerified
```

Provider mit niedrigerem Trust Level werden nicht als gültige Kandidaten betrachtet.

Eine Policy darf Trust-Anforderungen nicht zugunsten besserer Performance reduzieren.

## Policy Versioning

Policies MÜSSEN versionierbar sein.

Eine Policyänderung kann das Verhalten des Systems verändern, obwohl sich der zugrunde liegende Mechanismus nicht geändert hat.

Daher sollen relevante Entscheidungen folgende Informationen erfassen können:

```text
PolicyID
PolicyVersion
InputConstraints
SelectedDecision
Reason
Timestamp
```

## Policy Lifecycle

Policies besitzen einen Lifecycle.

Ein mögliches Modell lautet:

```text
Installed
    ↓
Validated
    ↓
Active
    ↓
Superseded
    ↓
Retired
```

Ungültige oder nicht vertrauenswürdige Policies dürfen nicht aktiviert werden.

## Policy Security

Nicht jede Anwendung darf System-Policies verändern.

Das Ändern einer Policy erfordert eine entsprechende Capability.

Beispiel:

```text
Capability:
    nova.policy.power.modify
```

oder:

```text
Capability:
    nova.policy.security.read
```

Lesen und Ändern können getrennte Rechte darstellen.

## Policy Sandboxing

Komplexe oder programmierbare Policies SOLLEN isoliert ausgeführt werden.

Eine fehlerhafte Policy darf nicht unmittelbar Kernel-Speicher verändern.

Die Policy liefert eine Entscheidung beziehungsweise einen Plan.

Die eigentliche Operation wird durch kontrollierte Mechanismen ausgeführt.

## Fail-Safe Policy

Für kritische Subsysteme MÜSSEN sichere Fallback-Policies definiert werden können.

Wenn eine höhere Policy Engine ausfällt, darf ein fundamentales System nicht automatisch handlungsunfähig werden.

Beispiel:

```text
Normal:
    AdaptivePowerPolicy

Failure:
    ConservativeSafePowerPolicy
```

oder:

```text
Normal:
    AdaptiveSchedulerPolicy

Failure:
    StaticSafeSchedulerPolicy
```

## Policy Caching

Policyentscheidungen dürfen gecacht werden, wenn ihre Gültigkeitsbedingungen bekannt sind.

Ein Cache-Eintrag muss invalidiert werden können, wenn sich relevante Bedingungen ändern.

Beispiele:

- Capability revoked
- Trust changed
- Hardware removed
- Resource budget changed
- Policy version changed
- Thermal state changed

## Fast Path

Mechanism / Policy Separation darf nicht dazu führen, dass jede Kerneloperation eine komplexe Policy Engine aufrufen muss.

NovaOS unterscheidet deshalb zwischen:

```text
Policy Decision
        ↓
Authorization / Plan
        ↓
Cached State / Capability
        ↓
Fast Mechanism
```

Beispielsweise kann eine komplexe Security Policy einmal entscheiden, ob eine Capability ausgegeben werden darf.

Danach kann der Kernel die Capability mit einem schnellen Enforcement-Pfad prüfen.

## Adaptive Policies

Policies dürfen adaptiv sein.

Adaptive Policies können historische Messwerte verwenden, um zukünftige Entscheidungen zu verbessern.

NovaOS verwendet dafür unter anderem Prediction Error als Lernsignal:

```text
Prediction
    ↓
User / System Decision
    ↓
Observed Result
    ↓
Deviation
    ↓
Model Correction
```

Adaptive Policies dürfen jedoch keine harten Constraints umgehen.

## KI-basierte Policies

KI darf zur Policyoptimierung verwendet werden.

Beispiele:

- Workload Prediction
- Prefetch Prediction
- Provider Selection
- Energy Optimization
- Failure Prediction

KI-basierte Entscheidungen dürfen jedoch nicht alleinige Grundlage fundamentaler Sicherheits- oder Korrektheitsgarantien sein.

Für kritische Entscheidungen müssen deterministische Regeln und überprüfbare Constraints vorhanden sein.

## Architecture Introspection

Policies und ihre Entscheidungen MÜSSEN introspektierbar sein.

NovaOS soll beispielsweise darstellen können:

```text
Operation:
    nova.compute.image.upscale

Candidates:
    CPU0
    GPU0
    NPU0

Selected:
    NPU0

Policies:
    PerformancePolicy
    EnergyPolicy
    TrustPolicy

Reasons:
    Deadline satisfied
    Lowest estimated energy
    Required trust level satisfied

Rejected:
    GPU0
        Higher estimated energy

    CPU0
        Deadline risk
```

## Decision Tracing

Automatische Entscheidungen SOLLEN nachvollziehbar sein.

Decision Tracing soll mindestens erfassen können:

- welche Policy beteiligt war
- welche Policyversion verwendet wurde
- welche Kandidaten existierten
- welche Constraints angewendet wurden
- welche Kandidaten ausgeschlossen wurden
- welche Entscheidung getroffen wurde
- warum die Entscheidung getroffen wurde

Sensitive Informationen müssen dabei gemäß Security- und Privacy-Policy geschützt werden.

## Observability

Policyentscheidungen können in die systemweite Observability integriert werden.

Mögliche Metriken:

- Decision Count
- Decision Latency
- Cache Hit Rate
- Constraint Failure
- Override Count
- Fallback Count
- Policy Error
- Prediction Error

Diese Daten können zur Optimierung und Fehlerdiagnose verwendet werden.

## Hot Replacement

Policies SOLLEN unabhängig von fundamentalen Mechanismen aktualisierbar sein.

Eine neue Scheduling-, Power- oder Provider-Policy soll nach Möglichkeit aktiviert werden können, ohne den zugrunde liegenden Kernelmechanismus auszutauschen.

Policywechsel müssen jedoch:

- transaktional
- validiert
- versioniert
- rollbackfähig

sein, sofern ihre Kritikalität dies erfordert.

## Transaktionale Policyänderungen

Mehrere zusammengehörige Policyänderungen SOLLEN atomar aktiviert werden können.

Beispiel:

```text
Transaction {
    Update SecurityPolicy
    Update NetworkPolicy
    Update DataSovereigntyPolicy
}
```

Entweder wird der konsistente neue Policyzustand aktiviert oder der vorherige Zustand bleibt erhalten.

Ein teilweise aktivierter widersprüchlicher Policyzustand soll vermieden werden.

## Policy-Konflikte

Policy-Konflikte müssen explizit behandelt werden.

Beispiel:

```text
Policy A:
    Use fastest provider

Policy B:
    Use lowest-energy provider
```

Sind beide weiche Ziele, kann eine Gewichtung verwendet werden.

Beispiel:

```text
Performance = 0.7
Energy      = 0.3
```

Bei harten Constraints ist keine solche Gewichtung zulässig.

Beispiel:

```text
Security:
    RemoteProvider forbidden
```

Diese Regel kann nicht durch eine Performancegewichtung aufgehoben werden.

## Normative Festlegungen

1. NovaOS MUSS Mechanism / Policy Separation als systemweites Architekturprinzip verwenden.

2. Fundamentale Mechanismen SOLLEN möglichst unabhängig von konkreten Policies implementiert werden.

3. Policies SOLLEN keine fundamentalen Kernel- oder Hardwaremechanismen duplizieren.

4. Mechanismen MÜSSEN klar definierte Schnittstellen bereitstellen.

5. Policies MÜSSEN einen definierten Scope besitzen.

6. Policies SOLLEN versionierbar sein.

7. Kritische Policyänderungen SOLLEN transaktional aktivierbar sein.

8. Policyentscheidungen DÜRFEN harte Security Constraints NICHT verletzen.

9. Policyentscheidungen DÜRFEN harte Safety Constraints NICHT verletzen.

10. Policyentscheidungen DÜRFEN Data-Sovereignty-Anforderungen NICHT verletzen.

11. Hard-Realtime-Anforderungen MÜSSEN gegenüber weichen Optimierungszielen Vorrang besitzen.

12. Determinism Requirements MÜSSEN adaptive oder nichtdeterministische Policyentscheidungen einschränken können.

13. Explizite Benutzer- oder Entwickler-Overrides DÜRFEN automatische Auswahlmechanismen überschreiben, sofern dadurch keine höher priorisierten harten Constraints verletzt werden.

14. NovaOS SOLL automatische Providerwahl unterstützen.

15. NovaOS SOLL automatische Algorithmuswahl unterstützen.

16. Automatische Provider- und Algorithmuswahl MUSS explizit überschreibbar sein, sofern die betreffende API dies zulässt.

17. Execution Contracts SOLLEN als Eingabe für Policyentscheidungen verwendet werden können.

18. Policies SOLLEN Resource Budgets berücksichtigen können.

19. Policies SOLLEN Trust Requirements berücksichtigen können.

20. Policies SOLLEN Data-Sovereignty-Regeln berücksichtigen können.

21. Policies SOLLEN Energie- und Thermalzustände berücksichtigen können.

22. Policies SOLLEN Deadline- und Latenzanforderungen berücksichtigen können.

23. Komplexe Policyentscheidungen SOLLEN als Decision Plan repräsentierbar sein.

24. Policy Engines DÜRFEN verteilt und subsystembezogen implementiert werden.

25. NovaOS MUSS NICHT für jede Operation eine zentrale globale Policy Engine aufrufen.

26. Häufige Mechanismusoperationen SOLLEN Fast Paths verwenden können.

27. Policyentscheidungen DÜRFEN gecacht werden, wenn ihre Gültigkeitsbedingungen bekannt sind.

28. Policy-Caches MÜSSEN bei relevanten Zustandsänderungen invalidierbar sein.

29. Kritische Subsysteme SOLLEN sichere Fallback-Policies besitzen.

30. Programmierbare Policies SOLLEN isoliert ausgeführt werden.

31. Policies DÜRFEN Kernelzustand NICHT außerhalb definierter Mechanismen manipulieren.

32. Policyänderungen MÜSSEN Capability-basiert autorisiert werden können.

33. Policies und ihre Versionen MÜSSEN über Architecture Introspection sichtbar gemacht werden können.

34. Automatische Entscheidungen SOLLEN über Decision Tracing erklärbar sein.

35. Decision Tracing SOLL relevante Kandidaten, Constraints und Entscheidungsgründe erfassen können.

36. Sensitive Policyinformationen MÜSSEN durch Security- und Privacy-Regeln geschützt werden.

37. Adaptive Policies DÜRFEN Prediction Error als Lernsignal verwenden.

38. Adaptive Policies DÜRFEN harte Constraints NICHT aufgrund gelernter Wahrscheinlichkeiten ignorieren.

39. KI-basierte Policyoptimierung DARF NICHT alleinige Grundlage fundamentaler Sicherheits- oder Korrektheitsgarantien sein.

40. Policies SOLLEN unabhängig von den zugrunde liegenden Mechanismen weiterentwickelt und ausgetauscht werden können.

## Konsequenzen

### Positive Konsequenzen

- hohe Evolvierbarkeit der Systemarchitektur
- austauschbare Policies
- geringere Kopplung zwischen Subsystemen
- bessere Testbarkeit
- bessere Introspektion
- adaptive Optimierung ohne Austausch fundamentaler Mechanismen
- Unterstützung unterschiedlicher Geräteklassen
- bessere Realtime-Unterstützung
- bessere Determinismus-Unterstützung
- flexible Energieoptimierung
- flexible Providerwahl
- flexible Algorithmuswahl
- kontrollierte Benutzer- und Entwickler-Overrides
- einfacheres Hot Replacement von Policies
- klare Trennung zwischen Constraints und Optimierungszielen

### Negative Konsequenzen

- zusätzliche Policy-Infrastruktur erforderlich
- Policy-Komposition kann komplex werden
- Konfliktauflösung zwischen Policies muss formal definiert werden
- Decision Tracing erzeugt zusätzlichen Aufwand
- zu abstrakte Mechanismen können Performancekosten verursachen
- Policy Engines können selbst zu komplexen Systemkomponenten werden
- fehlerhafte Policies können trotz korrekter Mechanismen schlechtes Systemverhalten verursachen
- Versionierung und Lifecycle von Policies müssen verwaltet werden

## Verworfene Alternativen

### Fest verdrahtete Policies

Nicht übernommen.

Fest verdrahtete Policies wären einfacher zu implementieren, würden jedoch die langfristige Evolvierbarkeit von NovaOS stark einschränken.

### Vollständig anwendungsdefinierte Policies

Nicht übernommen.

Anwendungen dürfen nicht fundamentale Sicherheits-, Ressourcen- oder Systemintegritätsregeln beliebig ersetzen.

### Eine einzige globale Policy Engine

Nicht als zwingende Architektur übernommen.

Eine zentrale globale Policy Engine würde zu:

- Bottlenecks
- Single Point of Failure
- unnötiger Latenz
- starker Kopplung

führen.

NovaOS verwendet stattdessen ein gemeinsames Policy-Modell mit spezialisierten und gegebenenfalls verteilten Policy Engines.

### Rein KI-basierte Policy Engine

Nicht übernommen.

KI kann Entscheidungen optimieren, darf aber nicht die einzige Grundlage für fundamentale Systemgarantien bilden.

### Policy-Auswertung bei jeder einzelnen Operation

Nicht übernommen.

Komplexe Policies werden möglichst vorab ausgewertet und in:

- Capabilities
- Decision Plans
- Reservations
- Cached Decisions
- Execution Contracts

überführt.

Dadurch bleiben kritische Fast Paths effizient.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-EXECUTION-0001_Nova_ExecutionContract
- ADR-CAPABILITY-0001_Capability_basiertes_Systemmodell
- ADR-CAPABILITY-RESOLUTION-0001_ExecutionContract_gesteuerte_Providerwahl
- ADR-ALGORITHM-0001_Automatische_Algorithmuswahl_mit_explizitem_Override
- ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur
- ADR-POWER-0001_Einheitliche_Energie_und_Thermikarchitektur

## Zugehörige NPSPECs

- NPSPEC-ARCH-MECHANISM-POLICY-0001
- NPSPEC-POLICY-MODEL-0001
- NPSPEC-POLICY-SCOPE-0001
- NPSPEC-POLICY-COMPOSITION-0001
- NPSPEC-POLICY-PRIORITY-0001
- NPSPEC-POLICY-CONSTRAINT-0001
- NPSPEC-POLICY-DECISION-0001
- NPSPEC-POLICY-DECISION-TRACE-0001
- NPSPEC-POLICY-VERSIONING-0001
- NPSPEC-POLICY-LIFECYCLE-0001
- NPSPEC-POLICY-SECURITY-0001
- NPSPEC-POLICY-FALLBACK-0001
- NPSPEC-POLICY-CACHE-0001
- NPSPEC-POLICY-INTROSPECTION-0001
- NPSPEC-EXECUTION-CONTRACT-0001
- NPSPEC-CAPABILITY-RESOLUTION-0001
- NPSPEC-ALGORITHM-SELECTION-0001
- NPSPEC-RESOURCE-ECONOMY-0001
- NPSPEC-DETERMINISTIC-MODE-0001
- NPSPEC-PREDICTION-ERROR-0001

## Ergebnis

NovaOS trennt fundamentale technische Mechanismen konsequent von den Policies, die deren Verwendung steuern.

Damit können Scheduling, Ressourcenverteilung, Energieverwaltung, Providerwahl, Algorithmuswahl, Security, Recovery und andere Systementscheidungen weiterentwickelt werden, ohne die zugrunde liegenden Mechanismen ständig neu implementieren zu müssen.

Die Kombination aus Mechanism / Policy Separation, Execution Contracts, Capability-basiertem Enforcement, Resource Economy, Decision Tracing und Architecture Introspection bildet damit eine zentrale Grundlage für die adaptive und langfristig evolvierbare Architektur von NovaOS.