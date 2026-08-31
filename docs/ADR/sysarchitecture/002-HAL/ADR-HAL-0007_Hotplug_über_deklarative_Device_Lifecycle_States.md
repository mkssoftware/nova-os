# ADR-HAL-0007 – Hotplug über deklarative Device Lifecycle States

## Status

Angenommen

## Kategorie

HAL / Hotplug / Device Lifecycle / Declarative System Model / Treiber / Ressourcenverwaltung / Resilienz

## Kontext

Moderne Hardware ist nicht statisch.

Geräte können während des laufenden Betriebs:

- erkannt,
- initialisiert,
- aktiviert,
- deaktiviert,
- entfernt,
- ersetzt,
- zurückgesetzt,
- fehlerhaft,
- vorübergehend nicht verfügbar

werden.

Typische Beispiele sind:

```text
USB
PCIe Hotplug
Thunderbolt
NVMe Hotplug
Docking Stations
Network Devices
External GPUs
Virtual Devices
CXL Devices
CPU Hotplug
Memory Hotplug
```

Ein klassisches Hotplug-Modell behandelt solche Änderungen häufig primär als Folge imperativer Events:

```text
device added
    ↓
run handler
    ↓
load driver
    ↓
initialize device
```

Dieses Modell wird problematisch, sobald mehrere Subsysteme gleichzeitig beteiligt sind.

Ein Gerät kann beispielsweise Beziehungen besitzen zu:

- Driver Binding,
- DMA und IOMMU,
- Interrupt Routing,
- Resource Economy,
- Power Management,
- Security,
- Nova.Trust,
- Device Namespaces,
- User Sessions,
- Filesystems,
- Pipelines,
- laufenden Execution Scopes.

Das tatsächliche Entfernen eines Geräts ist deshalb keine einzelne Operation, sondern eine koordinierte Zustandsänderung.

NovaOS soll Hotplug daher nicht als Sammlung lose gekoppelter Callback-Ketten modellieren.

---

## Entscheidung

NovaOS modelliert Hotplug über **deklarative Device Lifecycle States**.

Für jedes relevante Device existieren mindestens:

```text
Desired State
```

und:

```text
Actual State
```

Das System versucht, den tatsächlichen Gerätezustand kontrolliert mit dem gewünschten Zustand zu reconciliieren.

Das Grundmodell lautet:

```text
Hardware Event
      │
      ▼
Hardware System Graph
      │
      ▼
Device Actual State
      │
      │
      ├──────────────┐
      ▼              │
Desired State        │
      │              │
      ▼              │
Lifecycle Reconciler │
      │              │
      ▼              │
Validated Transition │
      │              │
      ▼              │
Driver / DMA / IRQ / Resources
      │              │
      └──── Verify ──┘
```

Damit wird Hotplug Teil des deklarativen Systemmodells von NovaOS.

---

## Grundprinzip

Es gilt:

```text
A hardware event reports what changed.

It does not define
the complete system reaction.
```

und:

```text
Hotplug is a state transition problem,
not a callback problem.
```

---

## Device Lifecycle Model

Ein Gerät besitzt einen expliziten Lebenszyklus.

Das Basismodell umfasst:

```text
Discovered
Available
Binding
Initializing
Ready
Active
Quiescing
Suspended
Offline
Removing
Removed
Failed
```

Weitere spezialisierte Zustände dürfen ergänzt werden.

---

## Discovered

`Discovered` bedeutet:

Das Gerät wurde erkannt, ist jedoch noch nicht vollständig validiert oder an einen Provider beziehungsweise Treiber gebunden.

Beispiel:

```text
PCIe enumeration
      ↓
Device discovered
```

In diesem Zustand dürfen noch keine allgemeinen Annahmen über Betriebsbereitschaft getroffen werden.

---

## Available

`Available` bedeutet:

Das Gerät wurde soweit erkannt und validiert, dass eine Bindung oder Aktivierung grundsätzlich möglich ist.

Es muss noch nicht aktiv verwendet werden.

---

## Binding

`Binding` bedeutet:

NovaOS bestimmt und bindet einen geeigneten:

- Driver,
- Device Provider,
- Capability Provider

an das Gerät.

Die Bindung unterliegt dabei unter anderem:

- Compatibility Contracts,
- Security Policy,
- Trust Requirements,
- Resource Requirements.

---

## Initializing

`Initializing` bedeutet:

Gerätespezifische Ressourcen werden vorbereitet.

Beispiele:

```text
MMIO mapping
Interrupt setup
DMA domain
Queues
Firmware initialization
Device reset
```

Das Gerät gilt währenddessen noch nicht als allgemein nutzbar.

---

## Ready

`Ready` bedeutet:

Die Initialisierung war erfolgreich und das Gerät kann aktiviert beziehungsweise konsumiert werden.

`Ready` und `Active` werden bewusst getrennt.

Ein Gerät kann vollständig vorbereitet sein, ohne aktuell benutzt oder freigegeben zu sein.

---

## Active

`Active` bedeutet:

Das Gerät ist operational und darf entsprechend seiner Contracts verwendet werden.

Beispiele:

```text
Network interface transmitting
Storage device mounted/used
GPU accepting workloads
```

---

## Suspended

`Suspended` bedeutet:

Das Gerät bleibt logisch vorhanden, ist aber temporär nicht aktiv.

Mögliche Gründe:

- Energieeinsparung,
- System Suspend,
- Resource Policy,
- User Policy,
- Thermal Pressure.

---

## Quiescing

`Quiescing` bedeutet:

Das Gerät wird kontrolliert in einen Zustand gebracht, in dem keine neuen Operationen mehr gestartet werden und laufende Operationen beendet oder abgebrochen werden.

Beispiel:

```text
Stop accepting work
      ↓
Drain queues
      ↓
Wait for in-flight operations
      ↓
Safe device state
```

Dieser Zustand ist zentral für sicheres Hot-Unplug.

---

## Offline

`Offline` bedeutet:

Das Gerät ist logisch weiterhin bekannt, steht aber für normale Nutzung nicht zur Verfügung.

Ein Offline-Gerät kann möglicherweise später wieder aktiviert werden.

---

## Removing

`Removing` bedeutet:

NovaOS führt die kontrollierte Entfernung durch.

Typische Schritte:

```text
Quiesce
    ↓
Detach consumers
    ↓
Revoke capabilities
    ↓
Stop DMA
    ↓
Remove interrupt routes
    ↓
Release resources
    ↓
Unbind driver
```

---

## Removed

`Removed` bedeutet:

Das Gerät ist nicht mehr Teil der aktiven Hardwaretopologie.

Historische Identität und relevante Audit-/Diagnosedaten dürfen weiterhin erhalten bleiben.

---

## Failed

`Failed` bedeutet:

Das Gerät oder ein notwendiger Lifecycle-Schritt hat einen Fehlerzustand erreicht.

Dieser Zustand muss Ursache und Failure Information enthalten können.

Beispiel:

```text
Failed {
    reason
    phase
    recoverability
}
```

---

## Desired State

Das gewünschte Device State Model beschreibt, welchen Zustand NovaOS erreichen möchte.

Beispiele:

```text
DesiredState = Active
```

oder:

```text
DesiredState = Offline
```

oder:

```text
DesiredState = Removed
```

Der Desired State kann sich durch mehrere Quellen ändern:

- physische Hardwareänderung,
- Benutzeraktion,
- Administratorpolicy,
- Security Policy,
- Power Management,
- Driver Failure,
- Resource Pressure,
- Recovery.

---

## Actual State

Der Actual State beschreibt den aktuell bestätigten Systemzustand des Geräts.

Dabei gilt:

```text
Desired State != Actual State
```

während Transitionen ausdrücklich als normaler Zustand.

Beispiel:

```text
Desired:
    Removed

Actual:
    Quiescing
```

ist ein gültiger Zwischenzustand.

---

## Lifecycle Reconciliation

Ein Device Lifecycle Reconciler vergleicht:

```text
Desired State
```

mit:

```text
Actual State
```

und plant zulässige Transitionen.

Beispiel:

```text
Actual = Active
Desired = Removed

Plan:
    Active
      ↓
    Quiescing
      ↓
    Offline
      ↓
    Removing
      ↓
    Removed
```

---

## Keine beliebigen Zustandswechsel

Nicht jede State Transition ist zulässig.

Beispielsweise soll nicht direkt erfolgen:

```text
Active → Removed
```

wenn dadurch noch aktive DMA- oder I/O-Operationen bestehen bleiben.

Transitions müssen über einen definierten State Graph validiert werden.

---

## Lifecycle Contracts

Ein Device oder Driver Provider kann angeben, welche Lifecycle-Transitionen unterstützt werden.

Beispiel:

```text
Suspend:
    Supported

HotRemove:
    Supported

Reset:
    Supported

LiveRebind:
    Unsupported
```

Dadurch kann NovaOS die passende Strategie wählen.

---

## Device Graph Generation

Hotplug verändert den Hardware System Graph.

Eine Geräteänderung darf nicht als inkonsistente Teilmutation sichtbar werden.

Stattdessen:

```text
Topology Generation N
        ↓
Validated Change
        ↓
Topology Generation N+1
```

Lifecycle State und Graph Generation müssen miteinander konsistent gehalten werden.

---

## Device Discovery

Discovery und Activation sind getrennte Schritte.

Das bloße Erkennen eines Geräts darf nicht automatisch:

- Treiber laden,
- DMA freigeben,
- Gerät aktivieren,
- Datenzugriff erlauben.

Zuerst erfolgt:

```text
Discover
    ↓
Identify
    ↓
Validate
    ↓
Trust / Security Evaluation
    ↓
Bind
    ↓
Initialize
```

---

## Driver Binding

Driver Binding ist eine deklarative Zuordnungsentscheidung.

Beispiel:

```text
Device
    ↓
Compatible Driver Candidates
    ↓
Contract Validation
    ↓
Selected Driver
```

Ein Treiber wird nicht allein anhand einer Device ID ausgewählt.

Zusätzlich können berücksichtigt werden:

- Architektur,
- ABI,
- Trust,
- Capabilities,
- Isolation Requirement,
- Resource Cost,
- Version,
- Policy.

---

## Hot-Unplug

Hot-Unplug wird als kontrollierter Lifecycle-Wechsel behandelt.

Das Ziel ist:

```text
Active
    ↓
Quiescing
    ↓
Offline
    ↓
Removing
    ↓
Removed
```

Nicht jede Hardware erlaubt eine vollständig geordnete physische Entfernung.

Daher muss NovaOS zwischen:

```text
Planned Removal
```

und:

```text
Surprise Removal
```

unterscheiden.

---

## Planned Removal

Bei geplanter Entfernung kann NovaOS das Gerät vorab quiescen.

Beispiel:

```text
User requests eject
        ↓
Stop new work
        ↓
Drain
        ↓
Flush
        ↓
Revoke resources
        ↓
Safe-to-remove
```

---

## Surprise Removal

Bei unerwarteter physischer Entfernung kann der normale Transition Path nicht vollständig ausgeführt werden.

Beispiel:

```text
Active
    ↓
hardware disappears
    ↓
Failed / Removing
```

NovaOS muss dann:

- neue Zugriffe stoppen,
- DMA-Rechte widerrufen,
- Interruptbindungen entfernen,
- offene Operationen fehlschlagen lassen,
- abhängige Komponenten informieren,
- Recovery einleiten.

---

## In-Flight Operations

Laufende Operationen müssen explizit berücksichtigt werden.

Mögliche Strategien:

```text
Drain
Cancel
Abort
Complete
Fail
UnknownOutcome
```

Welche Strategie zulässig ist, hängt vom Device Contract und Operationstyp ab.

---

## Structured Concurrency

Geräteoperationen sollen soweit möglich an Execution Scopes gebunden sein.

Beim Entfernen eines Geräts kann NovaOS relevante Scopes gezielt abbrechen oder auslaufen lassen.

Beispiel:

```text
Device Scope
├── I/O Request A
├── I/O Request B
└── Background Worker
```

Dadurch werden verwaiste Geräteoperationen vermieden.

---

## DMA

Vor endgültiger Entfernung eines Geräts müssen dessen DMA-Rechte kontrolliert behandelt werden.

Gemäß `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen` gilt:

```text
Quiesce Device
      ↓
Stop DMA
      ↓
Revoke DMA Mappings
      ↓
Release IOMMU Domain
```

Bei Surprise Removal muss die IOMMU soweit möglich sofort als Schutzgrenze genutzt werden.

---

## Interrupts

Interruptzuordnungen eines entfernten Geräts müssen deaktiviert und entfernt werden.

Späte Interrupts dürfen nicht versehentlich an bereits freigegebene Device Objects gebunden werden.

---

## MMIO

MMIO-Mappings dürfen nach Device Removal nicht weiterhin als gültige Hardwarezugriffe betrachtet werden.

Bestehende Handles müssen invalidiert oder kontrolliert beendet werden.

---

## Device Handles

Handles auf Geräte benötigen Lifecycle Awareness.

Ein Handle auf ein entferntes Gerät soll beispielsweise einen typisierten Fehler liefern:

```text
DeviceRemoved
```

und nicht auf ein neues Gerät zeigen, das zufällig dieselbe Hardwareadresse erhalten hat.

---

## Stabile Device Identity

Geräteidentität und momentane Busposition werden getrennt behandelt.

Beispiel:

```text
DeviceIdentity
```

ist nicht automatisch identisch mit:

```text
PCI Bus/Device/Function
```

oder:

```text
USB Port Number
```

Ein neu angeschlossenes anderes Gerät darf nicht versehentlich alte Autorität übernehmen.

---

## Ressourcenfreigabe

Device Removal muss alle gebundenen Ressourcen berücksichtigen.

Dazu gehören beispielsweise:

```text
MMIO
I/O Ports
DMA Mappings
Pinned Memory
Interrupts
Queues
Power References
Driver Objects
Capabilities
Device Names
```

Freigabe erfolgt kontrolliert und nachvollziehbar.

---

## Capability Revocation

Capabilities, die auf ein Gerät oder Device Provider verweisen, müssen bei Removal oder sicherheitsrelevantem Offline-Status widerrufbar sein.

Ein bestehendes Capability Handle darf nicht automatisch ewige Verfügbarkeit garantieren.

---

## Resource Economy

Hotplug verändert verfügbare Ressourcen.

Beispiel:

```text
GPU removed
    ↓
Compute capacity changes
```

oder:

```text
CXL memory added
    ↓
Memory capacity changes
```

Resource Economy muss solche Änderungen als neue Resource State Generation behandeln können.

---

## Execution Planning

Wenn ein Provider oder Accelerator entfernt wird, müssen abhängige Execution Plans neu bewertet werden.

Mögliche Reaktionen:

```text
Continue existing work
Fallback provider
Replan
Degrade
Fail
```

Die Entscheidung hängt vom Execution Contract ab.

---

## Graceful Degradation

Hotplug soll soweit möglich Graceful Degradation unterstützen.

Beispiel:

```text
GPU removed
    ↓
Fallback to CPU
```

wenn:

- Capability Contract kompatibel,
- Performanceanforderung weiterhin erfüllbar,
- Determinism dies erlaubt,
- kein Forced Provider vorliegt.

---

## Hard Provider Binding

Wenn ein Execution Contract explizit verlangt:

```text
Provider = GPU0
```

darf der Verlust von `GPU0` nicht stillschweigend durch `CPU0` ersetzt werden.

Das Ergebnis ist ein expliziter Contract Failure.

---

## Storage Devices

Storage Hotplug benötigt besondere Behandlung.

Vor geplanter Entfernung können erforderlich sein:

```text
Flush
Unmount
Commit transactions
Close device
```

Bei Surprise Removal müssen Dateisystem und Storage Stack den Verlust explizit verarbeiten.

Ein physisch entferntes Storage Device darf nicht als erfolgreich schreibend behandelt werden.

---

## Network Devices

Network Device Removal kann:

- Verbindungen abbrechen,
- Routing ändern,
- Network Namespaces beeinflussen,
- laufende Streams degradieren.

Höhere Netzwerkkomponenten erhalten strukturierte Device Lifecycle Events.

---

## GPU und NPU

Bei Accelerator Removal können laufende Jobs in Zuständen enden wie:

```text
Completed
Aborted
DeviceLost
UnknownOutcome
```

Der Runtime Provider muss diese Zustände explizit an Execution Planning und Anwendungen propagieren.

---

## CPU Hotplug

CPU Hotplug verwendet dasselbe deklarative Grundprinzip, besitzt aber spezielle Constraints.

Beispiel:

```text
Desired CPU State = Offline
        ↓
Stop scheduling new tasks
        ↓
Migrate movable tasks
        ↓
Migrate interrupts
        ↓
Drain CPU-local work
        ↓
Platform CPU Off
```

Kernelkritische CPU-Rollen dürfen nicht ohne vorherige Übergabe entfernt werden.

---

## Memory Hotplug

Memory Hotplug muss bestehende Memory Ownership berücksichtigen.

Das Entfernen eines Memory Nodes kann Schritte erfordern wie:

```text
Drain allocations
Migrate pages
Reject new allocations
Update NUMA topology
Offline memory
```

Nicht migrierbarer belegter Speicher kann die Entfernung verhindern.

---

## CXL und zukünftige Ressourcen

Das Lifecycle-Modell muss auch zukünftige Hotplug-fähige Ressourcen unterstützen.

Beispiele:

```text
CXL Memory
CXL Accelerator
Disaggregated Device
Remote Hardware Resource
```

Der State Graph darf deshalb nicht ausschließlich auf klassische USB-Geräte zugeschnitten sein.

---

## Reset

Ein Device Reset wird als kontrollierte Lifecycle-Operation modelliert.

Beispiel:

```text
Active
   ↓
Quiescing
   ↓
Resetting
   ↓
Initializing
   ↓
Ready
   ↓
Active
```

`Resetting` darf als spezialisierter zusätzlicher State definiert werden.

---

## Driver Restart

Ein Treiber kann neu gestartet werden, ohne das physische Gerät zu entfernen.

Device State und Driver State müssen daher unterscheidbar bleiben.

Beispiel:

```text
Physical Device:
    Present

Driver:
    Restarting
```

---

## Driver Hot Replacement

Wenn ein neuer kompatibler Treiber verfügbar ist, kann der Lifecycle mit Hot Replacement kombiniert werden.

Beispiel:

```text
Active
  ↓
Quiescing
  ↓
Driver Handover
  ↓
Reinitialize / State Transfer
  ↓
Active
```

Dies ist nur zulässig, wenn Device- und Driver-Contracts die Transition unterstützen.

---

## Transaktionale Lifecycle-Transitionen

Komplexe Zustandswechsel sollen nach dem transaktionalen NovaOS-Modell erfolgen.

Beispiel:

```text
Plan
  ↓
Validate
  ↓
Quiesce
  ↓
Stage changes
  ↓
Activate
  ↓
Verify
  ↓
Commit
```

Schlägt die Transition fehl, erfolgt je nach Zustand:

```text
Rollback
Forward Recovery
Safe Offline
Failed
```

---

## Known-Good State

Für kritische Device-Transitionen soll ein zuletzt bestätigter stabiler Zustand bekannt sein.

Beispiele:

```text
Ready
Active
Offline
```

Nicht jede Hardware erlaubt ein vollständiges Rollback.

In solchen Fällen ist Forward Recovery erforderlich.

---

## Fail-Safe Defaults

Bei unklaren Lifecycle-Zuständen gilt:

```text
Unknown device state
    ↓
Do not grant new authority
```

Bei unklarer Hardwarepräsenz darf NovaOS nicht davon ausgehen, dass weitere I/O-Operationen sicher möglich sind.

---

## Device Failure

Hardwarefehler und Removal sind getrennte Konzepte.

Ein Gerät kann:

```text
Present + Failed
```

sein.

Der Lifecycle darf deshalb Failure nicht automatisch als physische Entfernung behandeln.

---

## Recovery

Ein `Failed` Device kann je nach Contract folgende Recovery Paths besitzen:

```text
Retry initialization
Reset
Rebind driver
Offline
Replace provider
Remove
```

Recovery darf nicht unbegrenzt oder ohne Policy wiederholt werden.

---

## Backoff

Wiederholte Initialisierungs- oder Recovery-Fehler sollen Backoff oder Failure Thresholds unterstützen.

Dadurch werden Endlosschleifen wie:

```text
Initialize
Fail
Initialize
Fail
Initialize
Fail
```

vermieden.

---

## Trust

Neu erkannte Geräte dürfen vor vollständiger Vertrauens- und Sicherheitsbewertung nur minimale Autorität erhalten.

Nova.Trust kann beispielsweise berücksichtigen:

```text
Device Identity
Firmware
Driver Signature
Provenance
Policy
```

Trust-Analyse darf die deterministische Baseline-Validierung ergänzen, aber nicht ersetzen.

---

## Security

Ein neu angeschlossenes Gerät darf nicht allein durch physische Anwesenheit Privilegien erhalten.

Insbesondere:

```text
Device Present
```

ist nicht gleich:

```text
Device Authorized
```

Device Lifecycle und Authorization State müssen getrennt modellierbar sein.

---

## Device Authorization

Zwischen Discovery und Activation darf ein Autorisierungsschritt liegen.

Beispiel:

```text
Discovered
    ↓
Validated
    ↓
Authorized
    ↓
Binding
```

`Authorized` kann entweder eigener Lifecycle State oder orthogonale Security-Eigenschaft sein.

Bevorzugt wird eine orthogonale Eigenschaft, damit Lifecycle und Security nicht unnötig gekoppelt werden.

---

## Orthogonalität

Folgende Dimensionen müssen getrennt bleiben:

```text
Physical Presence
Lifecycle State
Driver Binding
Authorization
Trust
Power State
Resource State
Health State
```

Ein einziger riesiger Device State Enum soll vermieden werden.

Der Lifecycle State beschreibt primär betriebliche Übergänge.

---

## Device State Model

Konzeptionell:

```text
DeviceState {
    lifecycle
    presence
    health
    power
    binding
    authorization
    trust
}
```

Diese Eigenschaften können gemeinsam ausgewertet werden, ohne semantisch vermischt zu werden.

---

## Power Management

Power State und Lifecycle State sind verwandt, aber getrennt.

Beispiel:

```text
Lifecycle:
    Ready

Power:
    LowPower
```

oder:

```text
Lifecycle:
    Suspended

Power:
    Off
```

Nicht jeder Power-State-Wechsel ist ein vollständiger Device-Lifecycle-Wechsel.

---

## Architecture Introspection

Der vollständige Device Lifecycle muss introspektierbar sein.

Beispiel:

```text
Device:
    NVMe0

Presence:
    Present

Lifecycle:
    Active

Driver:
    Nova.NVMe

DMA:
    Protected

Power:
    Active

Health:
    Healthy
```

Während einer Transition:

```text
Desired:
    Removed

Actual:
    Quiescing

Reason:
    User requested safe removal
```

---

## Transition History

Relevante Lifecycle-Transitionen sollen historisch nachvollziehbar sein.

Beispiel:

```text
09:14:22 Discovered
09:14:22 Available
09:14:23 Binding
09:14:23 Initializing
09:14:24 Ready
09:14:24 Active
10:48:11 Quiescing
10:48:12 Removed
```

Dies unterstützt Diagnose und Audit.

---

## Decision Trace

Wenn eine Transition blockiert wird, soll NovaOS den Grund darstellen können.

Beispiel:

```text
Desired:
    Removed

Blocked by:
    2 active non-cancellable storage operations
```

oder:

```text
Desired:
    Active

Blocked by:
    Driver trust requirement not satisfied
```

---

## Events

Subsysteme erhalten strukturierte Lifecycle-Events.

Beispiele:

```text
DeviceDiscovered
DeviceReady
DeviceActivated
DeviceQuiescing
DeviceOffline
DeviceRemoved
DeviceFailed
```

Events informieren über Zustandsänderungen.

Sie ersetzen nicht den deklarativen State als Quelle der Wahrheit.

---

## Event Delivery

Event-Verlust darf nicht dazu führen, dass ein Subsystem dauerhaft einen falschen Gerätezustand annimmt.

Ein Empfänger muss bei Bedarf den aktuellen Zustand erneut aus dem Systemmodell lesen können.

Daher gilt:

```text
State is authoritative.

Events are notifications.
```

---

## Idempotenz

Reconciliation-Operationen sollen soweit möglich idempotent sein.

Beispiel:

```text
EnsureDeviceOffline(device)
```

soll auch dann sicher aufrufbar sein, wenn das Gerät bereits offline ist.

---

## Concurrency

Mehrere Lifecycle-Anforderungen können gleichzeitig eintreffen.

Beispiel:

```text
User requests removal
+
Device reports failure
+
Power manager requests suspend
```

Der Device Reconciler muss solche konkurrierenden Zustandswünsche deterministisch auflösen.

---

## Priorität

Lifecycle-Wünsche müssen höhere Systemconstraints respektieren.

Beispielhafte Priorität:

```text
Safety
    ↓
Security
    ↓
Hardware Failure
    ↓
Explicit Removal
    ↓
Power Policy
    ↓
Performance Optimization
```

Die konkrete Policy wird separat definiert.

---

## Deterministic Mode

Deterministic Mode kann automatische Device-Rebindings oder Providerwechsel einschränken.

Ein deterministischer Workload darf nicht unbemerkt auf ein neu hinzugefügtes Gerät migriert werden.

---

## Graceful Degradation

Wenn ein Device verschwindet, kann das System eine alternative Capability-Implementierung verwenden.

Beispiel:

```text
NPU removed
    ↓
GPU provider
```

oder:

```text
GPU removed
    ↓
CPU provider
```

Nur sofern alle Hard Contracts weiterhin erfüllt werden.

---

## Architecture Evolution

Neue Lifecycle-States oder zusätzliche orthogonale State-Dimensionen müssen versionierbar ergänzt werden können.

Ältere Komponenten sollen unbekannte neue States sicher behandeln.

Ein unbekannter kritischer State darf nicht als `Active` interpretiert werden.

---

## KI-Unabhängigkeit

Device Discovery, Lifecycle-Reconciliation, Removal, Resource Revocation und Recovery-Baseline müssen ohne KI funktionieren.

KI darf unterstützen bei:

- Failure Prediction,
- Device Health Analysis,
- Recovery Recommendation,
- Provider Selection,
- Power Optimization.

KI darf nicht entscheiden, dass ein tatsächlich nicht validiertes Gerät sicher aktiv ist.

---

## Normative Anforderungen

1. NovaOS MUSS Hotplug über deklarative Device Lifecycle States modellieren.
2. Jedes relevante Device MUSS einen Actual Lifecycle State besitzen.
3. Jedes steuerbare Device SOLL einen Desired Lifecycle State besitzen.
4. Desired State und Actual State MÜSSEN getrennt modelliert werden.
5. Lifecycle-Transitionen MÜSSEN über definierte Zustandsübergänge erfolgen.
6. Geräte DÜRFEN NICHT unkontrolliert direkt von `Active` nach `Removed` wechseln, wenn kontrollierte Ressourcen bestehen.
7. `Discovered` MUSS von betriebsbereiten Zuständen unterschieden werden.
8. Discovery DARF NICHT automatisch vollständige Device Authority erzeugen.
9. Driver Binding MUSS als expliziter Lifecycle-Schritt oder explizite orthogonale Bindung modellierbar sein.
10. Initialisierung MUSS von allgemeiner Nutzbarkeit getrennt sein.
11. `Ready` und `Active` SOLLEN unterscheidbar sein.
12. `Quiescing` MUSS für kontrollierte Stilllegung unterstützt werden.
13. `Offline` MUSS von `Removed` unterscheidbar sein.
14. `Failed` MUSS von physischer Entfernung unterscheidbar sein.
15. Hardware Events DÜRFEN NICHT alleinige Quelle des Device State sein.
16. Der deklarative State MUSS die autoritative Zustandsquelle darstellen.
17. Lifecycle Events SOLLEN Zustandsänderungen signalisieren.
18. Eventverlust DARF NICHT dauerhafte Zustandsinkonsistenz erzeugen.
19. Hotplug MUSS den Hardware System Graph aktualisieren.
20. Topologieänderungen SOLLEN generationenbasiert konsistent sichtbar werden.
21. Planned Removal MUSS kontrolliertes Quiescing unterstützen.
22. Surprise Removal MUSS als eigener Failure-/Removal-Fall behandelt werden.
23. Neue Operationen MÜSSEN bei beginnender Entfernung blockierbar sein.
24. In-Flight Operations MÜSSEN während Removal explizit behandelt werden.
25. DMA-Mappings MÜSSEN bei Device Removal widerrufbar sein.
26. IOMMU-Domains MÜSSEN bei Removal kontrolliert aktualisiert oder entfernt werden.
27. Interruptbindungen MÜSSEN bei Device Removal deaktiviert werden.
28. MMIO-Zugriffe auf entfernte Geräte DÜRFEN NICHT als weiterhin gültig behandelt werden.
29. Device Handles MÜSSEN Removal erkennen können.
30. Alte Device Handles DÜRFEN NICHT versehentlich auf neu erkannte andere Geräte zeigen.
31. Device Identity MUSS von temporärer Busposition unterscheidbar sein.
32. Alle zu einem Device gehörenden kontrollierten Ressourcen MÜSSEN beim Lifecycle berücksichtigt werden.
33. Capabilities auf entfernte Geräte MÜSSEN widerrufbar sein.
34. Resource Economy MUSS Device Addition und Removal berücksichtigen können.
35. Execution Planning MUSS Providerverlust erkennen können.
36. Graceful Degradation DARF bei Device Removal verwendet werden, wenn Contracts dies erlauben.
37. Forced Provider Bindings DÜRFEN NICHT stillschweigend ersetzt werden.
38. Storage Removal MUSS offene und persistente Operationen berücksichtigen.
39. CPU Hotplug MUSS Scheduling und Interrupt Affinity berücksichtigen.
40. Memory Hotplug MUSS bestehende Speicherbelegung berücksichtigen.
41. Nicht migrierbarer Speicher DARF Memory Removal verhindern.
42. Device Reset SOLL als kontrollierte Lifecycle-Transition modellierbar sein.
43. Driver Restart MUSS von physischem Device Removal unterscheidbar sein.
44. Driver Hot Replacement SOLL bei kompatiblen Contracts integrierbar sein.
45. Komplexe Lifecycle-Transitionen SOLLEN transaktional ausgeführt werden.
46. Nicht rückrollbare Transitionen MÜSSEN Forward-Recovery unterstützen können.
47. Unbekannter Device State DARF NICHT neue Device Authority erzeugen.
48. Wiederholte Recovery-Fehler SOLLEN Backoff unterstützen.
49. Device Presence DARF NICHT mit Device Authorization gleichgesetzt werden.
50. Lifecycle, Presence, Health, Power, Binding, Authorization und Trust SOLLEN orthogonal modellierbar sein.
51. Power State DARF NICHT unnötig mit Lifecycle State verschmolzen werden.
52. Device Lifecycle MUSS introspektierbar sein.
53. Desired und Actual Device State SOLLEN introspektierbar sein.
54. Lifecycle Transition History SOLL nachvollziehbar sein.
55. Blockierte Transitionen SOLLEN ihren Grund erklären können.
56. Reconciliation-Operationen SOLLEN soweit möglich idempotent sein.
57. Konkurrierende Lifecycle-Anforderungen MÜSSEN kontrolliert aufgelöst werden.
58. Deterministic Mode MUSS automatische Device- und Providerwechsel einschränken können.
59. Neue Lifecycle-States MÜSSEN evolvierbar ergänzt werden können.
60. Ein unbekannter Lifecycle State DARF NICHT als sicher aktiv interpretiert werden.
61. Device Lifecycle Management MUSS ohne KI funktionieren.
62. KI DARF Lifecycle-Optimierungen unterstützen, aber keine fundamentale Autorisierung oder Zustandskorrektheit ersetzen.
63. NovaOS MUSS Hardwareänderungen als kontrollierte Zustandsentwicklung behandeln.
64. Hotplug MUSS in das Declarative System Model integriert sein.

---

## Konsequenzen

### Positive Konsequenzen

- Hotplug erhält ein einheitliches Systemmodell.
- Driver, DMA, IRQ, Security und Resource Economy können koordiniert reagieren.
- Device Removal wird robuster.
- Surprise Removal kann explizit behandelt werden.
- Device Lifecycle wird introspektierbar und diagnostizierbar.
- Hardware Hotplug integriert sich direkt in das Declarative System Model.
- CPU-, Memory-, CXL- und Accelerator-Hotplug können nach demselben Grundprinzip modelliert werden.
- Eventverlust verursacht weniger leicht dauerhafte Zustandsfehler.
- spätere Hot Replacement- und Self-Healing-Mechanismen erhalten eine klare Grundlage.

### Negative Konsequenzen

- Device Lifecycle Management wird komplexer als einfache Add-/Remove-Callbacks.
- Driver Contracts müssen Lifecycle-Eigenschaften explizit beschreiben.
- Reconciliation und Transition Validation benötigen zusätzliche Zustandsverwaltung.
- Surprise Removal bleibt trotz sauberem Modell technisch schwierig.
- manche Geräte erlauben kein vollständiges Quiescing oder Rollback.

---

## Verworfene Alternativen

### Hotplug ausschließlich über Add-/Remove-Callbacks

Verworfen.

Callbacks beschreiben Ereignisse, aber nicht den konsistenten Gesamtzustand des Geräts.

### Device Removal als sofortige Objektlöschung

Verworfen.

Aktive DMA-, Interrupt-, I/O- und Capability-Beziehungen müssen zuvor behandelt werden.

### Ein einziger Device-State mit allen Eigenschaften

Verworfen.

Lifecycle, Power, Trust, Authorization, Health und Binding sind orthogonale Dimensionen.

### Physische Anwesenheit automatisch als Autorisierung behandeln

Verworfen.

Ein angeschlossenes Gerät ist nicht automatisch vertrauenswürdig oder erlaubt.

### Surprise Removal wie Planned Removal behandeln

Verworfen.

Bei Surprise Removal kann die Hardware bereits nicht mehr erreichbar sein.

### Hotplug nur für USB modellieren

Verworfen.

NovaOS muss auch CPU-, Memory-, PCIe-, CXL- und Accelerator-Hotplug unterstützen können.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-DEVICE-LIFECYCLE-0001`
- `NPSPEC-HAL-DEVICE-STATE-0001`
- `NPSPEC-HAL-DEVICE-DESIREDSTATE-0001`
- `NPSPEC-HAL-DEVICE-RECONCILER-0001`
- `NPSPEC-HAL-DEVICE-TRANSITION-0001`
- `NPSPEC-HAL-DEVICE-HOTPLUG-0001`
- `NPSPEC-HAL-DEVICE-HOTUNPLUG-0001`
- `NPSPEC-HAL-DEVICE-SURPRISE-REMOVAL-0001`
- `NPSPEC-HAL-DEVICE-QUIESCE-0001`
- `NPSPEC-HAL-DEVICE-RESET-0001`
- `NPSPEC-HAL-DEVICE-BINDING-0001`
- `NPSPEC-HAL-DEVICE-IDENTITY-0001`
- `NPSPEC-HAL-DEVICE-EVENT-0001`
- `NPSPEC-HAL-DEVICE-FAILURE-0001`
- `NPSPEC-HAL-DEVICE-RECOVERY-0001`
- `NPSPEC-HAL-DEVICE-RESOURCE-REVOKE-0001`
- `NPSPEC-HAL-CPU-HOTPLUG-0001`
- `NPSPEC-HAL-MEMORY-HOTPLUG-0001`
- `NPSPEC-HAL-CXL-HOTPLUG-0001`
- `NPSPEC-HAL-DEVICE-INTROSPECTION-0001`

---

## Verifikation

Die Umsetzung muss insbesondere folgende Szenarien prüfen:

- Device Discovery,
- Driver Binding,
- erfolgreiche Device Initialization,
- Planned Hot-Unplug,
- Surprise Removal,
- Removal mit aktiven DMA-Mappings,
- Removal mit aktiven Interrupts,
- Removal mit laufenden I/O-Operationen,
- Device Reset,
- Driver Crash bei physisch vorhandenem Gerät,
- Driver Restart,
- fehlgeschlagene Initialisierung,
- Recovery nach Device Failure,
- wiederholt fehlschlagende Recovery,
- CPU Hotplug,
- Memory Hotplug,
- nicht entfernbarer belegter Speicher,
- GPU-/Accelerator-Removal während laufender Jobs,
- Resource Economy Update nach Device Removal,
- Fallback auf alternativen Provider,
- Forced Provider mit verlorenem Gerät,
- konkurrierende Suspend-/Remove-/Failure-Anforderungen,
- verlorenes Lifecycle Event mit anschließender State-Neusynchronisation,
- Introspection von Desired und Actual State.

---

## Ergebnis

NovaOS behandelt Hotplug nicht als Folge unabhängiger Hardwarecallbacks.

Stattdessen wird jedes Device als deklarativ verwaltete Systemressource mit explizitem Lebenszyklus betrachtet.

Das Modell lautet:

```text
Physical Hardware Change
        ↓
Device Presence
        ↓
Desired / Actual State
        ↓
Lifecycle Reconciliation
        ↓
Validated Transition
        ↓
Driver
DMA
Interrupts
Resources
Capabilities
        ↓
Verified State
```

Damit gilt:

```text
Events report change.

State defines reality.

Reconciliation drives transition.
```

Die zentrale Architekturregel lautet:

```text
Do not react to hotplug
with uncontrolled callbacks.

Reconcile devices
toward explicit lifecycle states.
```