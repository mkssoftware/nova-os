# ADR-ARCH-0004 – Transaktionale Systemoperationen

## Status

Angenommen

## Kategorie

Systemarchitektur / Zustandsmanagement / Systemtransaktionen

## Kontext

NovaOS soll Systemänderungen nicht als lose Folge voneinander unabhängiger Einzeloperationen behandeln, wenn diese gemeinsam einen konsistenten neuen Systemzustand bilden.

In klassischen Betriebssystemen werden viele komplexe Änderungen imperativ und schrittweise durchgeführt.

Beispiel:

```text
StopService()
ReplaceFiles()
UpdateConfiguration()
MigrateData()
RestartService()
```

Wenn während dieser Sequenz ein Fehler auftritt, kann ein inkonsistenter Zwischenzustand entstehen.

Beispiele:

- neue Binärdateien mit alter Konfiguration
- aktualisierte Anwendung mit altem Datenbankschema
- neue Capability-Version mit inkompatiblem Provider
- teilweise aktualisierte Systemdienste
- veränderte Berechtigungen ohne zugehörige Policy
- Storage-Migration ohne vollständige Metadatenaktualisierung
- Service-Neustart mit nur teilweise aktiviertem Update

Solche Zwischenzustände erschweren:

- Recovery
- Self-Healing
- Updates
- Hot Replacement
- Softwareinstallation
- Systemkonfiguration
- Capability-Rebinding
- Storage-Operationen
- Security-Änderungen
- deklarative Reconciliation

NovaOS benötigt deshalb ein systemweites Modell für **transaktionale Systemoperationen**.

Das Ziel ist nicht, jede einzelne Kerneloperation wie eine klassische Datenbanktransaktion zu behandeln.

Stattdessen sollen logisch zusammengehörige Systemänderungen als definierte Transaktionen ausgeführt werden können.

## Entscheidung

NovaOS verwendet **transaktionale Systemoperationen als grundlegendes Architekturprinzip**.

Mehrere logisch zusammengehörige Zustandsänderungen SOLLEN zu einer Systemtransaktion zusammengefasst werden können.

Eine Systemtransaktion beschreibt:

```text
Alter gültiger Zustand
        │
        ▼
Geplante Änderungen
        │
        ▼
Validierung
        │
        ▼
Staging
        │
        ▼
Aktivierung
        │
        ▼
Verifikation
        │
        ▼
Commit
        │
        ▼
Neuer gültiger Zustand
```

Schlägt eine notwendige Phase fehl, soll NovaOS nach Möglichkeit:

```text
Rollback
```

oder:

```text
Forward Recovery
```

verwenden.

Der zentrale Grundsatz lautet:

```text
Keine dauerhaft sichtbaren
inkonsistenten Zwischenzustände,
wenn die Operation semantisch atomar sein soll.
```

## Transaktionsmodell

Eine Systemtransaktion besitzt mindestens folgende logische Bestandteile:

```text
SystemTransaction {
    Identity
    Scope
    Participants
    Preconditions
    Operations
    ResourceRequirements
    SecurityRequirements
    CommitConditions
    RollbackPlan
    RecoveryPlan
    Verification
}
```

Nicht jede Transaktion muss sämtliche Felder explizit deklarieren.

Das Modell muss jedoch alle notwendigen Informationen zur sicheren Durchführung repräsentieren können.

## Transaktionsphasen

NovaOS verwendet für komplexe Systemtransaktionen ein mehrstufiges Modell.

Ein grundlegender Ablauf lautet:

```text
Plan
    ↓
Acquire
    ↓
Validate
    ↓
Stage
    ↓
Prepare
    ↓
Activate
    ↓
Verify
    ↓
Commit
```

Im Fehlerfall:

```text
Failure
    ↓
Contain
    ↓
Rollback
```

oder:

```text
Failure
    ↓
Contain
    ↓
Forward Recovery
```

## Plan

In der Planungsphase wird bestimmt, welche Änderungen notwendig sind.

Beispiel:

```text
Transaction:
    Upgrade MediaService v4 → v5
```

Plan:

```text
Download MediaService v5
Verify package
Prepare new capability providers
Migrate configuration
Migrate persistent state
Start new service instance
Verify health
Switch bindings
Retire old instance
```

Die Planung kann durch:

- deklarativen Desired State
- Execution Contracts
- Policies
- Abhängigkeitsgraphen
- Capability Graphs
- Resource Economy
- Trust
- aktuelle Systemzustände

beeinflusst werden.

## Acquire

Benötigte Ressourcen SOLLEN vor kritischen Änderungen reserviert oder zumindest auf Verfügbarkeit geprüft werden.

Dazu können gehören:

- Speicher
- Storage Capacity
- CPU Budget
- temporäre Dateien
- Locks
- Capabilities
- Provider
- Netzwerkzugriff
- Update-Artefakte
- Snapshots

Eine Transaktion soll nicht erst nach mehreren irreversiblen Schritten feststellen, dass eine essentielle Ressource fehlt.

## Validate

Vor der Aktivierung müssen relevante Voraussetzungen geprüft werden.

Beispiele:

- Signaturen gültig
- Provenance akzeptiert
- Dependencies verfügbar
- Schema kompatibel
- ausreichender Storage vorhanden
- benötigte Capabilities vorhanden
- keine widersprüchliche Policy
- Hardware kompatibel
- Zielversion zulässig
- Rollback grundsätzlich möglich

Validierung reduziert das Risiko, dass eine Transaktion erst während der Aktivierung scheitert.

## Stage

Änderungen SOLLEN soweit möglich zunächst außerhalb des aktiven Zustands vorbereitet werden.

Beispiele:

- neue Dateien in Staging-Bereich
- neue Service-Version parallel installieren
- neues Datenbankschema vorbereiten
- neue Capability Provider registrieren, aber noch nicht aktivieren
- neue Konfiguration vorladen
- neue Policy validieren
- Snapshot erzeugen

Der aktive Zustand bleibt währenddessen unverändert.

## Prepare

In der Prepare-Phase bestätigen beteiligte Komponenten, dass sie für die Umschaltung bereit sind.

Beispiel:

```text
StorageService   = Prepared
MediaService     = Prepared
CapabilityRouter = Prepared
PolicyEngine     = Prepared
```

Erst wenn die notwendigen Teilnehmer bereit sind, erfolgt die Aktivierung.

## Activate

Die Aktivierung verändert den für das System wirksamen Zustand.

Diese Phase SOLL möglichst kurz gehalten werden.

Beispiel:

```text
OldProvider
    ↓
Atomic Binding Switch
    ↓
NewProvider
```

Die lange Vorbereitung erfolgt vorher.

Dadurch wird das Zeitfenster eines inkonsistenten aktiven Zustands minimiert.

## Verify

Nach der Aktivierung muss der neue Zustand überprüft werden.

Beispiele:

- Dienst antwortet
- Daten sind lesbar
- Capability Provider erfüllt Contract
- neue Konfiguration ist gültig
- Integrity Checks erfolgreich
- Performance innerhalb Mindestanforderungen
- Health Checks erfolgreich
- benötigte Dependencies funktionieren

Ein erfolgreicher Aktivierungsschritt allein bedeutet noch keinen erfolgreichen Commit.

## Commit

Erst nach erfolgreicher Verifikation wird die Transaktion endgültig bestätigt.

Nach dem Commit darf der alte Zustand abhängig von der Retention Policy:

- gelöscht
- archiviert
- als Rollback-Punkt behalten

werden.

## Rollback

Wenn eine Transaktion vor dem Commit fehlschlägt, SOLL der vorherige gültige Zustand wiederhergestellt werden können.

Rollback kann beinhalten:

- alte Bindings wieder aktivieren
- alten Service starten
- neue Dateien entfernen
- vorherige Konfiguration wiederherstellen
- Storage Snapshot zurücksetzen
- Capability-Rebinding zurücknehmen
- Ressourcen freigeben

Rollback selbst muss ebenfalls fehlerrobust sein.

## Forward Recovery

Nicht jede Operation kann sinnvoll rückgängig gemacht werden.

Beispiele:

- Nachricht wurde extern gesendet
- Hardware wurde physisch verändert
- Daten wurden an externes System übertragen
- irreversible Firmwareoperation
- externe Zahlung oder Transaktion

In solchen Fällen muss Forward Recovery möglich sein.

Beispiel:

```text
Old state cannot be restored
        ↓
Find next valid state
        ↓
Repair / Compensate
        ↓
Verify
```

NovaOS darf deshalb nicht voraussetzen, dass jede Systemtransaktion vollständig reversibel ist.

## Kompensierende Operationen

Für nicht direkt reversible Änderungen können kompensierende Operationen definiert werden.

Beispiel:

```text
Operation:
    RegisterRemoteResource

Compensation:
    UnregisterRemoteResource
```

Eine Kompensation ist semantisch nicht zwingend identisch mit einem echten Rollback.

Sie stellt einen definierten gültigen Zustand wieder her.

## Atomarität

NovaOS unterstützt semantische Atomarität.

Dies bedeutet:

```text
Andere Komponenten sollen
entweder den alten
oder den neuen konsistenten Zustand sehen.
```

Absolute globale Atomarität über beliebig viele Geräte, Maschinen und externe Systeme ist nicht immer technisch möglich.

Daher unterscheidet NovaOS zwischen verschiedenen Transaktionsgraden.

## Transaktionsgrade

Mögliche Transaktionsklassen:

```text
LocalAtomic
ProcessAtomic
DomainAtomic
SystemAtomic
DistributedCoordinated
BestEffortCompensated
```

### LocalAtomic

Atomare Änderung innerhalb eines einzelnen lokalen Objekts oder Subsystems.

### ProcessAtomic

Atomare Sichtbarkeit innerhalb einer Prozess- oder Service-Domain.

### DomainAtomic

Atomare Änderung innerhalb einer Execution Domain.

### SystemAtomic

Koordinierte atomare Umschaltung mehrerer lokaler Systemkomponenten.

### DistributedCoordinated

Koordinierte Änderung über mehrere Nodes.

### BestEffortCompensated

Keine vollständige Atomarität möglich; Fehler werden durch Kompensationen behandelt.

## Transaktionsscope

Jede Transaktion MUSS einen definierten Scope besitzen.

Beispiel:

```text
TransactionScope {
    System
}
```

oder:

```text
TransactionScope {
    Application = MediaEditor
}
```

oder:

```text
TransactionScope {
    Capability = de.nova.image.decode.jpeg
}
```

Der Scope beeinflusst:

- beteiligte Komponenten
- Locking
- Isolation
- Recovery
- Logging
- Berechtigungen

## Nested Transactions

NovaOS DARF verschachtelte Transaktionen unterstützen.

Beispiel:

```text
SystemUpdateTransaction
    ├── KernelUpdateTransaction
    ├── DriverUpdateTransaction
    └── ServiceUpdateTransaction
```

Die genaue Semantik verschachtelter Transaktionen muss explizit definiert werden.

Ein Child Commit bedeutet nicht automatisch, dass dessen Änderungen unabhängig vom Parent dauerhaft sichtbar werden.

## Transaction Ownership

Jede aktive Transaktion MUSS eine klar definierte Ownership besitzen.

Mögliche Owner:

- Kernel
- System Service
- Update Manager
- Application
- Capability Manager
- Recovery Controller

Owner müssen über die notwendigen Capabilities verfügen.

## Teilnehmer

Eine Transaktion kann mehrere Teilnehmer besitzen.

Beispiel:

```text
Transaction {
    participants = [
        StorageService,
        CapabilityRegistry,
        MediaService,
        PolicyEngine
    ]
}
```

Teilnehmer müssen ihre Rolle und ihren Status deklarieren können.

Mögliche Stati:

```text
Joined
Prepared
Activated
Verified
Committed
RolledBack
Failed
```

## Transaktionskoordinator

Komplexe Transaktionen können einen Coordinator besitzen.

Der Coordinator ist verantwortlich für:

- Plan
- Participant Coordination
- Phase Control
- Timeout Handling
- Recovery Decision
- Commit Decision

NovaOS benötigt jedoch keinen einzigen globalen Transaktionskoordinator.

Koordinatoren können subsystembezogen sein.

## Kein globaler Mega-Transaction-Manager

NovaOS SOLL nicht sämtliche Systemoperationen über einen einzigen zentralen Transaction Manager serialisieren.

Dies würde:

- Skalierbarkeit reduzieren
- Latenz erhöhen
- Failure Domain vergrößern
- Realtime-Workloads behindern

Transaktionen werden stattdessen:

- lokal
- hierarchisch
- subsystembezogen
- nur bei Bedarf koordiniert

## Isolation

Parallel ausgeführte Transaktionen dürfen sich nicht unkontrolliert gegenseitig beeinflussen.

Mögliche Isolationstechniken:

- Locks
- MVCC
- Copy-on-Write
- Snapshots
- Versioned Objects
- Immutable Staging
- Capability Leases
- Namespace Switching

Die geeignete Technik hängt vom Subsystem ab.

## MVCC

Multi-Version Concurrency Control DARF für Systemobjekte verwendet werden.

Beispiel:

```text
ConfigVersion 41
ConfigVersion 42
```

Eine laufende Operation kann weiterhin Version 41 verwenden, während Version 42 vorbereitet wird.

Nach erfolgreicher Aktivierung können neue Operationen Version 42 erhalten.

## Copy-on-Write

Copy-on-Write eignet sich besonders für:

- Storage Snapshots
- Konfigurationszustände
- Package Updates
- System Images
- Dateisystemtransaktionen

Dadurch können Änderungen vorbereitet werden, ohne den aktiven Zustand sofort zu überschreiben.

## Versionierte Objekte

Transaktionale Systemobjekte SOLLEN versioniert werden können.

Beispiel:

```text
Object:
    nova.policy.network

Version:
    17
```

Eine Transaktion kann erzeugen:

```text
Version:
    18
```

Die aktive Referenz wird erst beim Commit umgeschaltet.

## Capability Binding Transactions

Capability-Bindings SOLLEN transaktional geändert werden können.

Beispiel:

```text
Capability:
    de.nova.image.decode.jpeg

OldProvider:
    JPEGDecoderV3

NewProvider:
    JPEGDecoderV4
```

Ablauf:

```text
Install V4
    ↓
Verify V4
    ↓
Prepare Binding
    ↓
Atomic Rebind
    ↓
Health Check
    ↓
Commit
```

Bei Fehler:

```text
Rebind V3
```

## Hot Replacement

Hot Replacement verwendet transaktionale Systemoperationen.

Ein möglicher Ablauf:

```text
Prepare New Component
    ↓
Quiesce Old Component
    ↓
Transfer State
    ↓
Rebind Capabilities
    ↓
Activate New Component
    ↓
Verify
    ↓
Commit
```

Bei Fehler:

```text
Restore Old Component
```

sofern dies noch möglich ist.

## Updates

Systemupdates MÜSSEN transaktionale Aktivierung unterstützen können.

Ein Update soll nicht einfach aktive Dateien schrittweise überschreiben.

Bevorzugtes Modell:

```text
Acquire
    ↓
Verify
    ↓
Stage
    ↓
Prepare
    ↓
Activate
    ↓
Health Check
    ↓
Commit
```

Rollback muss abhängig vom Update-Typ vorgesehen werden.

## Softwareinstallation

Softwareinstallation kann ebenfalls transaktional erfolgen.

Beispiel:

```text
Install Application
Install Dependencies
Register Capabilities
Apply Permissions
Create Integration
```

Diese Schritte bilden logisch eine gemeinsame Operation.

Wenn ein kritischer Schritt fehlschlägt, soll keine halb installierte Anwendung zurückbleiben.

## Softwareentfernung

Deinstallation SOLL ebenfalls transaktional modelliert werden können.

Beispiel:

```text
Disable Application
Revoke Capabilities
Remove Registrations
Remove Files
Preserve User Data if Policy Requires
Commit Removal
```

## Systemkonfiguration

Mehrere zusammengehörige Konfigurationsänderungen SOLLEN atomar aktiviert werden können.

Beispiel:

```text
transaction {
    Firewall.Enabled = true
    RemoteAccess.Enabled = false
    Network.Profile = Public
}
```

Andere Komponenten sollen nicht unnötig einen Zwischenzustand beobachten wie:

```text
Firewall.Enabled = false
RemoteAccess.Enabled = true
Network.Profile = Public
```

wenn dieser Zustand sicherheitstechnisch unerwünscht ist.

## Security Transactions

Security-relevante Änderungen SOLLEN transaktional durchgeführt werden können.

Beispiel:

```text
Revoke old capability
Issue new capability
Update policy
Rotate key
```

Eine teilweise ausgeführte Sicherheitsänderung kann gefährlicher sein als gar keine Änderung.

Deshalb muss besonders bei Security-Operationen ein konsistenter Commit-Zustand sichergestellt werden.

## Key Rotation

Kryptografische Schlüsselrotation kann als Transaktion modelliert werden.

Beispiel:

```text
Generate NewKey
    ↓
Distribute NewKey
    ↓
Verify Participants
    ↓
Switch ActiveKey
    ↓
Retire OldKey
```

Der alte Schlüssel darf erst entfernt werden, wenn der neue gültig aktiviert wurde und alle erforderlichen Teilnehmer ihn verwenden können.

## Storage

Storage-Systeme SOLLEN transaktionale Mechanismen bereitstellen.

Beispiele:

- atomic rename
- snapshots
- journaled metadata
- copy-on-write
- transactional metadata updates
- transactional object updates

Die genaue Storage-Implementierung wird durch separate NPSPECs definiert.

## NovaFile

Komplexe NovaFile-Operationen können transaktional durchgeführt werden.

Beispiel:

```text
Update Payload
Update Metadata
Update Provenance
Update Security Information
```

Diese Informationen bilden logisch ein gemeinsames Objekt.

NovaOS sollte vermeiden, dass beispielsweise ein neuer Payload dauerhaft mit veralteter Integrity- oder Provenance-Information sichtbar wird.

## Data Sovereignty

Transaktionen müssen Data-Sovereignty-Policies respektieren.

Rollback- oder Staging-Daten dürfen nicht automatisch an Orte verschoben werden, die für die Originaldaten nicht zulässig wären.

Beispiel:

```text
Data:
    LocalOnly
```

Dann gilt dies ebenfalls für:

- Snapshots
- Staging Copies
- Transaction Logs
- Backups
- Recovery Data

## Resource Economy

Transaktionen nehmen an der Nova Resource Economy teil.

Eine Transaktion kann Ressourcen benötigen für:

- temporäre Kopien
- Snapshots
- parallele Komponenten
- Rollback-Daten
- Migration
- Verification

Diese Ressourcen müssen budgetiert werden.

Beispiel:

```text
TransactionBudget {
    memory  <= 512 MiB
    storage <= 4 GiB
    cpu     <= 10 %
}
```

## Admission Control

Eine Transaktion SOLL vor Beginn prüfen können, ob ihre Mindestressourcen verfügbar sind.

Wenn ein Update beispielsweise für sichere A/B-Aktivierung 8 GiB freien Speicher benötigt und nur 2 GiB verfügbar sind, sollte die Transaktion nicht erst nach dem Kopieren mehrerer Komponenten scheitern.

## Deadlines

Transaktionen können Deadlines besitzen.

Beispiel:

```text
Transaction {
    deadline = 2 s
}
```

Dies ist insbesondere relevant für:

- Realtime-Reconfiguration
- Failover
- Driver Replacement
- Live Migration
- Network Handover

## Realtime

Hard-Realtime-Pfade dürfen nicht durch unvorhersehbar lange globale Transaktionen blockiert werden.

Realtime-relevante Transaktionen müssen deshalb:

- begrenzte Critical Sections
- bekannte Commit-Zeiten
- vorher reservierte Ressourcen
- deterministische Aktivierung

verwenden können.

## Structured Concurrency

Transaktionsinterne Arbeit SOLL Structured Concurrency verwenden.

Beispiel:

```text
Transaction
    ├── VerifyPackage
    ├── PrepareStorage
    └── PrepareService
```

Wenn die Transaktion abgebrochen wird, müssen abhängige Child Tasks kontrolliert beendet werden.

Ungebundene Hintergrundtasks dürfen nicht unbeabsichtigt nach einem Rollback weiterarbeiten.

## Cancellation

Transaktionen SOLLEN Cancellation unterstützen, solange dies semantisch sicher ist.

Abhängig von der Phase kann gelten:

```text
Planning:
    cancellable

Staging:
    cancellable

Activation:
    limited cancellation

Commit:
    non-cancellable critical section
```

Die genaue Semantik hängt vom Transaktionstyp ab.

## Commit Point

Jede Transaktion SOLL einen klar definierten Commit Point besitzen.

Vor diesem Punkt gilt:

```text
Rollback expected
```

Nach diesem Punkt gilt:

```text
New state authoritative
```

Recovery nach dem Commit verwendet normalerweise Forward Recovery statt Rückkehr zum alten logischen Zustand.

## Point of No Return

Einige Transaktionen besitzen zusätzlich einen Point of No Return.

Beispiel:

```text
Firmware Update
```

Nach diesem Punkt kann ein klassischer Rollback technisch unmöglich sein.

Solche Transaktionen müssen den Point of No Return explizit modellieren.

Vor Erreichen dieses Punkts müssen besonders strenge Prüfungen durchgeführt werden.

## Crash Consistency

Systemtransaktionen müssen Crash Consistency berücksichtigen.

Ein Stromausfall oder Kernel-Crash darf nicht dazu führen, dass die Transaktion in einem undefinierten Zustand verbleibt.

Nach einem Neustart muss NovaOS feststellen können:

```text
Was war der letzte konsistente Zustand?
```

und:

```text
Welche Transaktion war aktiv?
```

sowie:

```text
Muss committed, rolled back
oder recovered werden?
```

## Transaction Journal

Für geeignete Transaktionen kann ein persistentes Transaction Journal verwendet werden.

Beispiel:

```text
TransactionID
State
Participants
PreparedVersions
CommitIntent
RecoveryMetadata
```

Das Journal soll nur Informationen enthalten, die zur Recovery benötigt werden.

## Write-Ahead Logging

Write-Ahead Logging DARF verwendet werden, wenn es für ein Subsystem geeignet ist.

Das bedeutet:

```text
Recovery Information persistieren
        ↓
Änderung durchführen
```

WAL ist jedoch kein universell verpflichtender Mechanismus für sämtliche NovaOS-Transaktionen.

## Idempotenz

Transaktionsoperationen SOLLEN soweit möglich idempotent sein.

Beispiel:

```text
EnsureCapabilityRegistered()
```

ist gegenüber:

```text
RegisterCapabilityWithoutChecking()
```

für Recovery besser geeignet.

Nach einem Crash kann eine Phase dadurch sicher erneut ausgeführt werden.

## Recovery nach Neustart

Nach einem Neustart kann eine Transaktion beispielsweise in folgendem Zustand gefunden werden:

```text
Prepared
```

NovaOS muss anhand des Journals und der Teilnehmerzustände entscheiden:

```text
Rollback
```

oder:

```text
Continue Commit
```

Die Entscheidung muss deterministisch und recoverbar sein.

## Health Verification

Ein Commit kann von Health Checks abhängig sein.

Beispiel:

```text
NewService {
    processAlive = true
    APIResponsive = true
    stateValid = true
}
```

Nur ein gestarteter Prozess reicht nicht notwendigerweise als erfolgreicher Health Check.

## Commit Conditions

Eine Transaktion kann explizite Commit Conditions besitzen.

Beispiel:

```text
CommitConditions {
    ServiceHealthy
    DataMigrationVerified
    CapabilityAvailable
    NoIntegrityViolation
}
```

Erst wenn alle verpflichtenden Bedingungen erfüllt sind, darf committed werden.

## Grace Period

Bestimmte Transaktionen dürfen vor endgültigem Commit eine Beobachtungsphase besitzen.

Beispiel:

```text
Activate
    ↓
Observe for 30 s
    ↓
Commit
```

Dies eignet sich beispielsweise für:

- Service Updates
- Driver Updates
- Netzwerkänderungen
- Policy Updates

## Canary Activation

Transaktionale Updates können Canary-Aktivierung unterstützen.

Beispiel:

```text
Activate for 5 %
    ↓
Observe
    ↓
Expand
    ↓
Commit Global Rollout
```

Die genaue Rollout-Steuerung wird durch Update- und Policy-Spezifikationen definiert.

## Distributed Transactions

NovaOS kann Transaktionen über mehrere Nodes koordinieren.

Dabei müssen Netzwerkpartitionen und Teilausfälle berücksichtigt werden.

Eine verteilte Transaktion darf nicht davon ausgehen, dass klassische globale ACID-Semantik in allen Fällen kostengünstig oder überhaupt verfügbar ist.

Mögliche Modelle:

- Two-Phase Commit
- Consensus-backed Commit
- Saga
- Compensation
- Versioned Desired State
- Eventual Reconciliation

Die geeignete Technik wird durch den Anwendungsfall bestimmt.

## Two-Phase Commit

Two-Phase Commit DARF verwendet werden, wenn starke koordinierte Commit-Semantik erforderlich ist.

Grundmodell:

```text
Prepare
    ↓
All Participants Ready?
    ↓
Commit
```

2PC wird nicht als universeller Standard für sämtliche verteilten Operationen vorgeschrieben.

## Saga

Für lange oder verteilte Workflows DARF das Saga-Modell verwendet werden.

Beispiel:

```text
Operation A
    ↓
Operation B
    ↓
Operation C
```

Bei Fehler:

```text
Compensate B
    ↓
Compensate A
```

Dies eignet sich insbesondere für Operationen, die keine globale atomare Sperre verwenden können.

## Declarative System Model

Transaktionale Systemoperationen bilden die Ausführungsebene des deklarativen Systemmodells.

Das Modell lautet:

```text
Desired State
    ↓
Planner
    ↓
Transaction Plan
    ↓
Transactional Execution
    ↓
Verification
    ↓
Actual State
```

Damit wird verhindert, dass Reconciliation komplexe Zustandsänderungen als unkoordinierte Einzeloperationen durchführen muss.

## Reconciliation

Ein Reconciler darf eine Systemtransaktion erzeugen, wenn mehrere Änderungen notwendig sind, um einen Desired State sicher zu erreichen.

Beispiel:

```text
Desired:
    MediaPlatform = Version 5
```

Daraus kann entstehen:

```text
Transaction {
    Update MediaService
    Update CodecCapability
    Update Configuration
    Migrate State
}
```

## Architecture Introspection

Aktive und vergangene Systemtransaktionen SOLLEN introspektierbar sein.

Beispiel:

```text
Transaction:
    tx-7F42

Type:
    ServiceUpgrade

State:
    Verify

Participants:
    MediaService
    CapabilityRegistry
    StorageService

PreviousState:
    MediaService v4

TargetState:
    MediaService v5

Rollback:
    Available
```

## Decision Tracing

NovaOS SOLL erklären können, warum eine Transaktion:

- gestartet
- blockiert
- abgebrochen
- zurückgerollt
- committed

wurde.

Beispiel:

```text
Transaction:
    DriverUpdate

Result:
    RolledBack

Reason:
    HealthCheckFailed

Failure:
    Device initialization timeout

Restored:
    Driver v8
```

## Audit

Security-relevante und administrative Transaktionen SOLLEN auditierbar sein.

Auditdaten können enthalten:

- Transaction ID
- Initiator
- Authorized Capability
- Zielzustand
- betroffene Objekte
- Commit Result
- Rollback Result
- Zeit
- Trust Context

Sensitive Daten müssen dabei entsprechend geschützt werden.

## Trust

Transaktionsartefakte müssen Trust- und Provenance-Prüfungen durchlaufen können.

Beispiele:

- Update-Paket
- neue Policy
- neuer Driver
- neue Capability
- Konfiguration
- Firmware

Ein Transaktionscommit darf keine nicht vertrauenswürdige Komponente aktivieren, wenn die geltende Trust Policy dies verbietet.

## Capability Security

Das Starten oder Steuern einer Systemtransaktion benötigt entsprechende Capabilities.

Beispiel:

```text
nova.system.update.execute
```

oder:

```text
nova.service.media.configure
```

Eine Capability für eine einzelne Operation bedeutet nicht automatisch das Recht, beliebige Systemtransaktionen zu erzeugen.

## Least Privilege

Transaktionskoordinatoren und Teilnehmer erhalten nur die Rechte, die für die konkrete Transaktion erforderlich sind.

Temporäre Capabilities SOLLEN nach Abschluss widerrufen oder automatisch ablaufen.

## Timeouts

Transaktionsphasen müssen Timeouts definieren können.

Ein Teilnehmer darf eine globale Operation nicht unbegrenzt im Zustand `Prepared` blockieren.

Bei Timeout muss eine definierte Policy entscheiden:

- Retry
- Abort
- Rollback
- Participant Isolation
- Forward Recovery

## Deadlock Prevention

Transaktionen müssen Deadlocks berücksichtigen.

Mögliche Strategien:

- globale Lock Ordering Rules
- optimistic concurrency
- timeout-based abort
- version validation
- lock-free staging
- resource pre-acquisition

Ein globales Lock über das gesamte System ist nicht vorgesehen.

## Transaction Conflicts

Konkurrierende Transaktionen müssen Konflikte erkennen können.

Beispiel:

```text
Transaction A:
    Update NetworkPolicy

Transaction B:
    Delete NetworkPolicy
```

Mögliche Reaktionen:

- serialize
- reject one transaction
- merge if semantically valid
- replan
- retry against new version

## Optimistic Concurrency

NovaOS DARF optimistische Concurrency verwenden.

Beispiel:

```text
Read Version 10
Prepare Change
Commit only if CurrentVersion == 10
```

Ist inzwischen Version 11 aktiv, muss die Transaktion:

- neu planen
- abbrechen
- oder semantisch mergen

## Pessimistic Locking

Pessimistisches Locking DARF für kritische oder kurzlebige Operationen verwendet werden.

Locks müssen jedoch:

- Scope besitzen
- Lifecycle besitzen
- Timeout unterstützen
- Deadlock-Regeln beachten

## Performance

Transaktionalität darf nicht bedeuten, dass jede hochfrequente Operation einen persistenten Journal-Eintrag erzeugt.

NovaOS unterscheidet zwischen:

```text
Fine-Grained Runtime Operations
```

und:

```text
State-Changing System Transactions
```

Beispiele für normalerweise nicht transaktionale Fast-Path-Operationen:

- jeder Scheduler Tick
- jeder Netzwerkpacket-Transfer
- jeder Memory Read
- jeder GPU Command

Transaktionalität wird dort eingesetzt, wo mehrere Zustandsänderungen einen logisch konsistenten Commit benötigen.

## Transaktionskosten

Eine Transaktion kann zusätzliche Kosten verursachen durch:

- Staging
- Snapshots
- Versioning
- Logging
- Health Checks
- parallele Instanzen
- Rollback State

Diese Kosten müssen durch Resource Economy sichtbar und budgetierbar sein.

## Deterministic Mode

Im Deterministic Mode müssen Transaktionsabläufe soweit erforderlich reproduzierbar sein.

Dazu können gehören:

- feste Participant Order
- feste Policyversionen
- deterministische Timeout-Regeln
- deterministische Commit Decisions
- deaktivierte adaptive Heuristiken

Externe nichtdeterministische Systeme können vollständige Reproduzierbarkeit weiterhin begrenzen.

## Self-Healing

Nova.Resilience nutzt Systemtransaktionen für komplexe Recovery-Vorgänge.

Beispiel:

```text
Detect corruption
    ↓
Create recovery transaction
    ↓
Isolate component
    ↓
Restore snapshot
    ↓
Rebind capabilities
    ↓
Verify
    ↓
Commit recovery
```

Recovery soll dadurch nicht selbst neue inkonsistente Zustände erzeugen.

## Transaction Failure Domains

Fehler einer Transaktion SOLLEN auf ihren Scope begrenzt werden.

Eine fehlgeschlagene Anwendungstransaktion darf nicht automatisch eine systemweite Recovery auslösen.

Eskalation erfolgt nur, wenn der Fehler tatsächlich größere Invarianten betrifft.

## Invarianten

Subsysteme können Invarianten definieren, die vor einem Commit erfüllt sein müssen.

Beispiel:

```text
Invariant:
    ExactlyOneActiveProvider
```

oder:

```text
Invariant:
    AllActiveSystemComponentsAreTrusted
```

oder:

```text
Invariant:
    DatabaseSchemaCompatibleWithApplication
```

Ein Commit darf eine verpflichtende Invariante NICHT verletzen.

## Normative Festlegungen

1. NovaOS MUSS transaktionale Systemoperationen als grundlegendes Architekturprinzip unterstützen.

2. Logisch zusammengehörige Systemänderungen SOLLEN als gemeinsame Systemtransaktion ausführbar sein.

3. Systemtransaktionen MÜSSEN einen definierten Scope besitzen.

4. Systemtransaktionen MÜSSEN eindeutig identifizierbar sein.

5. Kritische Transaktionen MÜSSEN Preconditions definieren können.

6. Transaktionen SOLLEN Änderungen soweit möglich vor der Aktivierung stagen.

7. Kritische Voraussetzungen SOLLEN vor irreversiblen Schritten validiert werden.

8. Transaktionen SOLLEN benötigte Mindestressourcen vor kritischen Änderungen prüfen oder reservieren.

9. Die aktive Umschaltphase SOLL möglichst kurz gehalten werden.

10. Ein erfolgreicher Aktivierungsschritt DARF NICHT automatisch als erfolgreicher Commit betrachtet werden.

11. Kritische Transaktionen SOLLEN nach Aktivierung Health- oder Integrity-Verifikation durchführen.

12. Transaktionen SOLLEN einen definierten Commit Point besitzen.

13. Vor dem Commit SOLL Rollback verwendet werden können, sofern die Operation reversibel ist.

14. Nicht reversible Operationen MÜSSEN Forward-Recovery- oder Kompensationsmechanismen definieren können.

15. NovaOS DARF NICHT voraussetzen, dass jede Systemtransaktion vollständig reversibel ist.

16. Transaktionen SOLLEN semantische Atomarität bereitstellen.

17. Unterschiedliche Transaktionsgrade MÜSSEN unterstützt werden können.

18. Transaktionskoordinatoren MÜSSEN Capability-basiert autorisiert werden.

19. Transaktionsteilnehmer SOLLEN ihren aktuellen Transaktionsstatus melden können.

20. NovaOS SOLL keinen einzelnen globalen Transaction Manager als zwingende Architektur voraussetzen.

21. Transaktionskoordination SOLL subsystembezogen und hierarchisch erfolgen können.

22. Parallele Transaktionen MÜSSEN Konflikte erkennen können.

23. Transaktionen DÜRFEN optimistische oder pessimistische Concurrency Control verwenden.

24. Deadlocks MÜSSEN durch definierte Mechanismen verhindert, erkannt oder aufgelöst werden können.

25. Transaktionen SOLLEN Timeouts unterstützen.

26. Timeouts MÜSSEN definierte Recovery- oder Abort-Semantik besitzen.

27. Systemupdates MÜSSEN transaktional aktivierbar sein können.

28. Softwareinstallationen SOLLEN transaktional durchgeführt werden können.

29. Softwaredeinstallationen SOLLEN transaktional durchgeführt werden können.

30. Capability-Rebinding SOLL transaktional durchgeführt werden können.

31. Hot Replacement SOLL transaktionale Umschaltung verwenden.

32. Security-relevante Konfigurationsänderungen SOLLEN transaktional aktiviert werden.

33. Key Rotation SOLL transaktionale Aktivierung unterstützen.

34. Storage-Metadatenänderungen SOLLEN geeignete Crash-Consistency-Mechanismen verwenden.

35. Transaktionszustände MÜSSEN nach einem Crash recoverbar sein, wenn eine teilweise Ausführung persistente Auswirkungen haben kann.

36. Persistente Transaktionen SOLLEN ein Recovery Journal verwenden können.

37. Write-Ahead Logging DARF verwendet werden, ist aber KEIN universeller Zwang.

38. Transaktionsoperationen SOLLEN soweit möglich idempotent sein.

39. Nach einem Neustart MUSS NovaOS aktive persistente Transaktionen klassifizieren können.

40. Recovery MUSS zwischen Commit-Fortsetzung, Rollback und Forward Recovery unterscheiden können.

41. Transaktionen DÜRFEN Health-Grace-Periods verwenden.

42. Canary-Aktivierung DARF Bestandteil transaktionaler Rollouts sein.

43. Verteilte Transaktionen DÜRFEN 2PC, Consensus, Saga oder Compensation verwenden.

44. NovaOS MUSS NICHT überall globale ACID-Semantik erzwingen.

45. Data-Sovereignty-Regeln MÜSSEN auch für Staging-, Snapshot-, Journal- und Rollback-Daten gelten.

46. Transaktionen MÜSSEN in die Nova Resource Economy integrierbar sein.

47. Transaktionsressourcen SOLLEN budgetiert werden können.

48. Hard-Realtime-Transaktionen MÜSSEN begrenzte und planbare Critical Sections unterstützen können.

49. Transaktionsinterne Tasks SOLLEN Structured Concurrency verwenden.

50. Cancellation SOLL unterstützt werden, sofern die jeweilige Phase dies sicher zulässt.

51. Points of No Return MÜSSEN für irreversible kritische Operationen explizit modellierbar sein.

52. Vor einem Point of No Return MÜSSEN alle technisch möglichen kritischen Prüfungen abgeschlossen sein.

53. Transaktionscommits MÜSSEN definierte Systeminvarianten respektieren.

54. Ein Commit DARF verpflichtende Security-, Safety-, Trust- oder Consistency-Invarianten NICHT verletzen.

55. Aktive Systemtransaktionen SOLLEN über Architecture Introspection sichtbar sein.

56. Commit-, Rollback- und Failure-Entscheidungen SOLLEN über Decision Tracing nachvollziehbar sein.

57. Security-relevante Transaktionen SOLLEN auditierbar sein.

58. Sensitive Transaktionsinformationen MÜSSEN gemäß Security- und Privacy-Policies geschützt werden.

59. Transaktionalität DARF Fast Paths NICHT unnötig belasten.

60. Transaktionen SOLLEN nur dort verwendet werden, wo mehrere Zustandsänderungen tatsächlich einen gemeinsamen Konsistenz- oder Commit-Grenzpunkt benötigen.

## Konsequenzen

### Positive Konsequenzen

- deutlich weniger inkonsistente Zwischenzustände
- robustere Systemupdates
- bessere Softwareinstallation und -deinstallation
- sichereres Hot Replacement
- sichere Capability-Rebindings
- bessere Crash Consistency
- kontrollierte Rollbacks
- bessere Self-Healing-Fähigkeit
- klare Commit Points
- nachvollziehbare Systemänderungen
- bessere Integration mit Desired State und Reconciliation
- bessere Security bei mehrteiligen Änderungen
- kontrollierte Recovery nach Neustarts
- bessere langfristige Evolvierbarkeit

### Negative Konsequenzen

- zusätzliche Transaktionsinfrastruktur
- höherer Speicherbedarf für Staging und Snapshots
- zusätzlicher Storagebedarf für Rollback-Daten
- komplexere Fehlerbehandlung
- Konflikt- und Deadlock-Management erforderlich
- Distributed Transactions können sehr komplex werden
- nicht jede Operation ist vollständig reversibel
- zusätzliche Verifikationskosten
- längere Update- oder Aktivierungsprozesse bei umfangreichen Health Checks
- Transaktionssemantik muss für jedes Subsystem präzise definiert werden

## Verworfene Alternativen

### Rein imperative Einzeloperationen

Nicht übernommen.

Unkoordinierte Einzeloperationen können bei Fehlern dauerhaft inkonsistente Systemzustände erzeugen.

### Globale Datenbanktransaktion für das gesamte Betriebssystem

Nicht übernommen.

Eine universelle globale ACID-Transaktion würde:

- Performance reduzieren
- Fast Paths beeinträchtigen
- Skalierbarkeit begrenzen
- verteilte Systeme unnötig erschweren
- Realtime-Anforderungen behindern

NovaOS verwendet deshalb domänenspezifische und hierarchische Transaktionen.

### Rollback als einzige Fehlerstrategie

Nicht übernommen.

Nicht jede Operation kann rückgängig gemacht werden.

Forward Recovery und kompensierende Operationen sind deshalb ebenfalls Bestandteil des Modells.

### Globales Locking

Nicht übernommen.

Ein globales Systemlock würde Parallelität und Skalierbarkeit zerstören.

### Two-Phase Commit für alle verteilten Operationen

Nicht übernommen.

2PC ist für bestimmte starke Konsistenzanforderungen geeignet, aber nicht für alle verteilten Workflows.

NovaOS erlaubt je nach Semantik auch:

- Saga
- Compensation
- Reconciliation
- Consensus
- eventual convergence

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-STATE-0001_Globaler_System_State_Graph
- ADR-STATE-0002_Desired_und_Actual_State
- ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur
- ADR-UPDATE-0001_Transaktionale_Systemupdates
- ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten
- ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur

## Zugehörige NPSPECs

- NPSPEC-TRANSACTION-ARCH-0001
- NPSPEC-TRANSACTION-MODEL-0001
- NPSPEC-TRANSACTION-LIFECYCLE-0001
- NPSPEC-TRANSACTION-SCOPE-0001
- NPSPEC-TRANSACTION-PARTICIPANT-0001
- NPSPEC-TRANSACTION-COORDINATOR-0001
- NPSPEC-TRANSACTION-COMMIT-0001
- NPSPEC-TRANSACTION-ROLLBACK-0001
- NPSPEC-TRANSACTION-FORWARDRECOVERY-0001
- NPSPEC-TRANSACTION-COMPENSATION-0001
- NPSPEC-TRANSACTION-ISOLATION-0001
- NPSPEC-TRANSACTION-CONCURRENCY-0001
- NPSPEC-TRANSACTION-CONFLICT-0001
- NPSPEC-TRANSACTION-DEADLOCK-0001
- NPSPEC-TRANSACTION-TIMEOUT-0001
- NPSPEC-TRANSACTION-JOURNAL-0001
- NPSPEC-TRANSACTION-CRASHRECOVERY-0001
- NPSPEC-TRANSACTION-INVARIANT-0001
- NPSPEC-TRANSACTION-INTROSPECTION-0001
- NPSPEC-TRANSACTION-AUDIT-0001
- NPSPEC-STATE-TRANSACTIONAL-0001
- NPSPEC-STATE-SNAPSHOT-0001
- NPSPEC-STATE-VERSIONING-0001
- NPSPEC-UPDATE-TRANSACTION-0001
- NPSPEC-CAPABILITY-REBIND-0001
- NPSPEC-RESILIENCE-RECOVERY-0001

## Ergebnis

NovaOS behandelt komplexe Systemänderungen nicht als lose Folge voneinander unabhängiger Befehle, sondern kann sie als definierte transaktionale Zustandsübergänge ausführen.

Das grundlegende Modell lautet:

```text
Desired Change
      ↓
Plan
      ↓
Validate
      ↓
Stage
      ↓
Prepare
      ↓
Activate
      ↓
Verify
      ↓
Commit
```

Bei Fehlern stehen abhängig von der Operation:

```text
Rollback
```

oder:

```text
Forward Recovery
```

zur Verfügung.

Transaktionale Systemoperationen verbinden damit das deklarative Systemmodell, Nova.Resilience, Hot Replacement, Updates, Capability-Rebinding, Security, Storage und Architecture Introspection zu einem konsistenten Zustandsänderungsmodell für NovaOS.