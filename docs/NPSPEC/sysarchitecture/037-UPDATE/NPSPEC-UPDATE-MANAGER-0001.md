# NPSPEC-UPDATE-MANAGER-0001 – Nova Update Manager

## Status

Angenommen

## Kategorie

Update / Lifecycle / System Management

## Zweck

NovaOS definiert den Nova Update Manager als zentrale Orchestrierungsinstanz für das sichere Erkennen, Planen, Validieren, Installieren und Verifizieren von System-, Kernel-, Treiber-, Modul- und Capability-Updates.

```text
Discover
   ↓
Resolve
   ↓
Validate
   ↓
Prepare
   ↓
Apply
   ↓
Verify
   ↓
Commit
```

Der Update Manager koordiniert Updates, ohne Sicherheits-, Trust-, Capability-, Transaktions- oder Recovery-Mechanismen zu umgehen.

## Grundprinzipien

```text
Update Available ≠ Update Required
Downloaded ≠ Trusted
Signed ≠ Authorized
Compatible ≠ Safe
Installed ≠ Active
Active ≠ Healthy
Updated ≠ Verified
Failed Update ≠ Broken System
Rollback ≠ Downgrade of Security State
```

## Update Manager

```text
Nova.UpdateManager
├── Discovery
├── Dependency Resolution
├── Compatibility Validation
├── Trust Verification
├── Update Planning
├── Staging
├── Transaction Coordination
├── Activation
├── Verification
├── Rollback Coordination
└── Introspection
```

## Update Model

```text
Update
├── UpdateID
├── Version
├── Target
├── UpdateType
├── Dependencies
├── Compatibility
├── Integrity
├── TrustRequirements
└── ActivationPolicy
```

Optional:

```text
RequiredCapabilities
ResourceRequirements
RebootRequirement
MigrationPlan
RollbackPlan
VerificationPlan
SecurityClassification
ProvenanceID
```

## Update Types

Der Manager muss unterschiedliche Update-Klassen unterstützen können:

```text
Kernel
Driver
System Module
Service
Runtime
Capability Provider
Library
Firmware Package
Security Policy
Configuration
Application Component
Recovery Component
```

## Lifecycle

```text
Discovered
   ↓
Resolved
   ↓
Validated
   ↓
Downloaded
   ↓
Staged
   ↓
Prepared
   ↓
Applied
   ↓
Activated
   ↓
Verified
   ↓
Committed
```

Fehlerzustände:

```text
Blocked
Failed
RolledBack
RecoveryRequired
Unknown
```

## Discovery

Update Discovery darf keine Installation auslösen.

```text
Discovery ≠ Authority to Install
```

Gefundene Updates müssen zunächst gegen aktuelle Systemanforderungen geprüft werden.

## Dependency Resolution

Der Update Manager muss Abhängigkeiten vor der Anwendung auflösen.

```text
Update A
├── requires B >= 3
├── conflicts C
└── requires ABI X
```

Unauflösbare Abhängigkeiten müssen den Update-Vorgang blockieren.

## Compatibility

Zu prüfen sind je nach Update:

```text
Architecture
ABI
API
Contract
State Schema
Capabilities
Drivers
Dependencies
Boot Environment
Hardware Requirements
```

```text
ABI Compatible ≠ Contract Compatible
```

## Trust Verification

Vor Anwendung muss die Herkunft und Integrität des Updates überprüft werden.

```text
Package
   ↓
Integrity
   ↓
Signature
   ↓
Trust Policy
   ↓
Authorized Update
```

Eine gültige Signatur allein reicht nicht aus, wenn die Trust Policy den Herausgeber oder das Artefakt nicht akzeptiert.

## Update Plan

Vor Änderungen wird ein expliziter Plan erstellt.

```text
UpdatePlan
├── Targets
├── CurrentVersions
├── TargetVersions
├── Dependencies
├── RequiredResources
├── MigrationSteps
├── ActivationSteps
├── VerificationSteps
└── RecoveryStrategy
```

Der Plan muss vor der Ausführung validierbar sein.

## Staging

Updates sollen vor Aktivierung vorbereitet werden.

```text
Download
   ↓
Validate
   ↓
Stage
   ↓
Ready for Activation
```

Der aktive Systemzustand soll während des Staging möglichst unverändert bleiben.

## Transactional Update

Systemkritische Updates müssen transaktional behandelt werden.

```text
Begin
 ↓
Validate
 ↓
Stage
 ↓
Prepare
 ↓
Apply
 ↓
Verify
 ↓
Commit
```

Bei Fehler:

```text
Failure
  ↓
Abort / Rollback / Recovery
```

## A/B Integration

Bootkritische Updates sollen A/B-Systemzustände verwenden können.

```text
Active A
   ↓
Update B
   ↓
Boot B
   ↓
Health Verification
   ↓
Mark B Healthy
```

Bei fehlgeschlagener Verifikation:

```text
B Failed
   ↓
Boot A
```

## Live Update

Komponenten dürfen ohne Neustart aktualisiert werden, wenn Live Evolution unterstützt wird.

```text
Old Component
      ↓
Load New
      ↓
Validate
      ↓
Transfer State
      ↓
Switch
      ↓
Verify
      ↓
Retire Old
```

```text
Hot Reload Supported ≠ Safe Live Replacement
```

## State Migration

Updates mit State-Änderungen benötigen einen expliziten Migration Contract.

```text
State v1
   ↓
Migration
   ↓
State v2
   ↓
Verification
```

```text
ABI Compatible ≠ State Compatible
```

## Security Updates

Sicherheitskritische Updates dürfen besondere Priorität besitzen.

Dabei bleiben Benutzerkontrolle und Systemschutz getrennt:

```text
Update Priority ≠ Unlimited Authority
```

Aktuelle Security- und Revocation-Zustände dürfen durch ein Update oder Rollback nicht abgeschwächt werden.

## Rollback

Vor kritischen Updates muss eine Recovery-Strategie existieren, soweit technisch möglich.

Mögliche Mechanismen:

```text
A/B Rollback
Snapshot
Checkpoint
Previous Version
Transactional Rollback
Component Replacement
Recovery Environment
```

Rollback erzeugt einen neuen gültigen Systemzustand und schreibt Historie nicht um.

## Verification

Nach Aktivierung muss der tatsächliche Zustand geprüft werden.

```text
Update Applied
     ↓
Boot / Start
     ↓
Health Checks
     ↓
Contract Verification
     ↓
Integrity Verification
     ↓
Operational Verification
     ↓
Commit
```

```text
Started ≠ Healthy
```

## Self-Healing

Der Update Manager integriert sich mit NovaOS Self-Healing.

```text
Update
  ↓
Failure Detected
  ↓
Contain
  ↓
Rollback / Repair
  ↓
Verify
```

Wiederholte fehlerhafte Updates müssen erkannt und begrenzt werden.

## Resource Management

Updates dürfen Ressourcenbudgets berücksichtigen:

```text
Storage
Memory
CPU
Network
Energy
Downtime
```

Update-Aktivitäten sollen normale Systemarbeit nicht unnötig beeinträchtigen.

## User Control

NovaOS muss Update-Policies unterstützen können:

```text
Automatic
Notify
Manual
Scheduled
Security-Only
Managed
```

Explizite Benutzerentscheidungen dürfen nicht durch adaptive Optimierung überschrieben werden, sofern keine höherrangige Safety- oder Security-Anforderung besteht.

## Provenance

Update-Vorgänge müssen nachvollziehbar sein.

```text
UpdateID
Source
Publisher
PreviousVersion
TargetVersion
Plan
Result
Timestamp
BuildID
VerificationResult
```

Sensitive Informationen dürfen nicht unnötig protokolliert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Installed Version
Available Updates
Update State
Update Plan
Dependencies
Trust State
Compatibility
Rollback Availability
Verification State
Last Successful Update
Last Failure
```

## Normative Anforderungen

1. NovaOS MUSS einen zentralen Update Manager für systemrelevante Updates bereitstellen.
2. Update Discovery DARF NICHT automatisch Installations-Authority erzeugen.
3. Updates MÜSSEN vor Anwendung auf Integrität geprüft werden.
4. Updates MÜSSEN gegen die aktuelle Trust Policy geprüft werden.
5. Kritische Abhängigkeiten MÜSSEN vor Anwendung aufgelöst werden.
6. Inkompatible Updates MÜSSEN blockiert werden.
7. Systemkritische Updates MÜSSEN einen validierbaren Update Plan besitzen.
8. Kritische Updates SOLLEN vor Aktivierung gestaged werden.
9. Systemkritische Updates MÜSSEN transaktional ausführbar sein.
10. Bootkritische Updates SOLLEN A/B-Mechanismen unterstützen.
11. Ein aktiviertes Update DARF NICHT automatisch als gesund gelten.
12. Kritische Updates MÜSSEN nach Aktivierung verifiziert werden.
13. State Migration MUSS explizit definiert und überprüfbar sein.
14. ABI Compatibility DARF NICHT als State- oder Contract-Compatibility interpretiert werden.
15. Live Updates MÜSSEN den Nova-Live-Evolution-Prozess einhalten.
16. Rollback DARF aktuelle Security- oder Revocation-Zustände NICHT abschwächen.
17. Fehlgeschlagene Updates MÜSSEN kontrolliert zurückgesetzt oder repariert werden können.
18. Update Recovery MUSS mit NovaOS Self-Healing integrierbar sein.
19. Wiederholte fehlerhafte Update-Versuche MÜSSEN begrenzbar sein.
20. Updates SOLLEN Resource Budgets berücksichtigen.
21. Update Policies MÜSSEN Benutzerkontrolle unterstützen.
22. Adaptive Update-Optimierung DARF explizite Benutzerentscheidungen NICHT überschreiben.
23. Update-Vorgänge MÜSSEN nachvollziehbare Provenance besitzen.
24. Update-Ergebnisse MÜSSEN mit Build- und Verification-Artefakten verknüpfbar sein.
25. Der Update-Zustand MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-VERIFY-CONTRACT-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `ADR-ARCH-0163`

## Ergebnis

```text
Update Available
      ↓
Resolve Dependencies
      ↓
Verify Integrity + Trust
      ↓
Check Compatibility
      ↓
Create Update Plan
      ↓
Stage
      ↓
Transactional Apply
      ↓
Activate
      ↓
Verify Health
      ↓
Healthy?
├── Yes → Commit
└── No  → Rollback / Recovery
```

NovaOS erhält damit einen zentralen, transaktionalen und verifizierbaren Update Manager, der Updates sicher vorbereitet, ihre Vertrauenswürdigkeit und Kompatibilität prüft, Änderungen kontrolliert aktiviert und bei Fehlern automatisch auf einen bekannten funktionsfähigen Zustand zurückkehren kann.