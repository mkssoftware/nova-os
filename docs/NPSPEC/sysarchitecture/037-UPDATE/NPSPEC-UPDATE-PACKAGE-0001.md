# NPSPEC-UPDATE-PACKAGE-0001 – Nova Update Package

## Status

Angenommen

## Kategorie

Update / Package / Distribution

## Zweck

NovaOS definiert ein standardisiertes Update-Paketformat für die sichere, reproduzierbare und transaktionale Verteilung von System-, Kernel-, Treiber-, Modul-, Capability- und Recovery-Updates.

```text
Update Package
├── Manifest
├── Payload
├── Metadata
├── Dependencies
├── Migration
├── Verification
└── Signature
```

Ein Update-Paket beschreibt nicht nur Dateien, sondern die vollständigen Bedingungen für Validierung, Installation, Aktivierung, Verifikation und gegebenenfalls Rollback.

## Grundprinzipien

```text
Package ≠ Installed State
Downloaded ≠ Valid
Valid ≠ Trusted
Signed ≠ Authorized
Compatible ≠ Installable
Installed ≠ Activated
Activated ≠ Verified
Package Version ≠ Component Version
```

## Package Model

```text
UpdatePackage
├── PackageID
├── PackageVersion
├── UpdateID
├── Target
├── TargetVersion
├── Manifest
├── Payload
├── Integrity
└── Signature
```

Optional:

```text
Dependencies
Conflicts
Capabilities
HardwareRequirements
MigrationPlan
RollbackMetadata
VerificationPlan
ActivationPolicy
ResourceRequirements
Provenance
```

## Package Identity

Jedes Paket besitzt eine stabile Identität.

```text
PackageID ≠ Filename
PackageID ≠ Download URL
PackageID ≠ Storage Location
```

Der tatsächliche Paketinhalt soll zusätzlich über eine kryptografische ContentID identifizierbar sein.

## Manifest

Das Manifest beschreibt das Paket deklarativ.

```text
Manifest
├── PackageID
├── Version
├── Target
├── Architecture
├── Dependencies
├── Compatibility
├── PayloadDescription
├── InstallationPolicy
└── VerificationPolicy
```

Das Manifest muss vor der Anwendung des Payloads vollständig validiert werden.

## Payload

Der Payload enthält die eigentlichen Update-Artefakte.

Beispiele:

```text
Kernel Image
Driver
System Module
Library
Service
Runtime
Capability Provider
Configuration
Recovery Image
Firmware Payload
```

Mehrere Artefakte dürfen gemeinsam in einem Paket enthalten sein.

## Integrity

Alle sicherheitsrelevanten Paketbestandteile müssen kryptografisch gegen Manipulation geschützt sein.

```text
Package
   ↓
Hash Verification
   ↓
Signature Verification
   ↓
Trust Validation
```

Manifest und Payload müssen kryptografisch miteinander verbunden sein.

## Signaturen

Pakete müssen signierbar sein.

Die Signatur muss mindestens relevante Identitäten binden:

```text
PackageID
PackageVersion
Manifest
Payload ContentIDs
Publisher
```

```text
Valid Signature ≠ Trusted Publisher
```

Die Vertrauensentscheidung erfolgt über Nova Trust Policy.

## Dependencies

Pakete dürfen Abhängigkeiten definieren.

```text
Requires
Recommends
Conflicts
Replaces
Provides
```

Beispiel:

```text
Kernel >= 4
ABI = 3
DriverFramework >= 2
Conflicts LegacyDriver-X
```

Dependency Resolution erfolgt vor der Installation.

## Compatibility

Ein Paket kann Anforderungen definieren für:

```text
Architecture
Hardware
Firmware
Kernel
ABI
API
Contract
State Schema
Capability Interface
Boot Environment
```

Nicht erfüllte harte Anforderungen blockieren die Anwendung.

## Delta Updates

NovaOS darf Delta-Pakete unterstützen.

```text
Version A
   +
Delta
   ↓
Version B
```

Delta-Pakete müssen ihre erwartete Ausgangsversion eindeutig definieren.

Kann diese nicht bestätigt werden, muss ein vollständiges Paket verwendet werden können.

## State Migration

Benötigt ein Update eine State Migration, muss diese explizit beschrieben werden.

```text
Old State
   ↓
Migration
   ↓
New State
   ↓
Verification
```

Migration Code unterliegt denselben Security- und Trust-Anforderungen wie andere systemkritische Update-Komponenten.

## Installation

Pakete werden nicht direkt selbstständig installiert.

```text
Package
   ↓
Nova Update Manager
   ↓
Validated Update Plan
   ↓
Transaction
```

Das Paket beschreibt Anforderungen und Artefakte; der Update Manager kontrolliert die Ausführung.

## Activation

Das Paket muss angeben können, wie eine neue Version aktiviert wird.

Beispiele:

```text
Immediate
Service Restart
Live Replacement
Next Boot
A/B Switch
Recovery Boot
Manual Activation
```

## Rollback

Pakete sollen Informationen für einen sicheren Rollback bereitstellen können.

```text
Previous Version
Snapshot Reference
A/B State
Reverse Migration
Compatibility Information
```

Ein Paket darf jedoch keine aktuellen Security- oder Revocation-Zustände zurücksetzen.

## Verification

Ein Paket kann einen Verification Plan enthalten.

Beispiele:

```text
Integrity Check
Boot Health Check
Contract Verification
ABI Validation
State Validation
Service Health Check
Driver Probe
Self-Test
```

```text
Installation Complete ≠ Verification Complete
```

## Resource Requirements

Pakete dürfen benötigte Ressourcen deklarieren:

```text
Download Size
Installed Size
Temporary Storage
Memory
CPU
Network
Expected Downtime
```

Der Update Manager kann diese Angaben für Admission und Planung verwenden.

## Package Composition

Mehrere Updates dürfen zu einem gemeinsamen Update Set kombiniert werden.

```text
Package A
Package B
Package C
    ↓
Update Transaction
```

Abhängigkeiten und Konflikte müssen für das gesamte Set geprüft werden.

## Provenance

Pakete müssen ihre Herkunft nachvollziehbar machen können.

```text
Publisher
Source
BuildID
PackageID
ContentID
Signature
Build Provenance
Verification Evidence
```

Damit kann NovaOS die Verbindung herstellen:

```text
Source
 ↓
Build
 ↓
Package
 ↓
Installation
 ↓
Running Component
```

## Reproduzierbarkeit

Update-Pakete sollen mit reproduzierbaren Builds und Verification Evidence verknüpfbar sein.

```text
Package ContentID
        ↕
BuildID
        ↕
VerificationID
```

Dadurch kann geprüft werden, ob das ausgelieferte Artefakt tatsächlich dem verifizierten Build entspricht.

## Unbekannte Felder

Das Paketformat muss evolvierbar sein.

```text
Unknown Optional Field
→ Preserve / Ignore safely

Unknown Required Field
→ Reject Package
```

Neue Paketversionen dürfen alte Implementierungen nicht zu unsicherer Interpretation zwingen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
PackageID
PackageVersion
Target
TargetVersion
Publisher
ContentID
Dependencies
Compatibility
Signature State
Trust State
Resource Requirements
Migration Requirements
Verification Requirements
```

## Normative Anforderungen

1. NovaOS MUSS ein standardisiertes Update-Paketformat definieren.
2. Jedes Paket MUSS eine stabile PackageID besitzen.
3. Package Identity MUSS vom Dateinamen und Speicherort unabhängig sein.
4. Pakete SOLLEN kryptografische ContentIDs besitzen.
5. Jedes Paket MUSS ein validierbares Manifest enthalten.
6. Manifest und Payload MÜSSEN kryptografisch miteinander verbunden sein.
7. Systemrelevante Pakete MÜSSEN signiert sein.
8. Eine gültige Signatur DARF NICHT automatisch als Trust-Entscheidung gelten.
9. Dependencies MÜSSEN deklarativ beschreibbar sein.
10. Konflikte MÜSSEN vor Anwendung erkannt werden können.
11. Hardware-, ABI-, API- und Contract-Anforderungen MÜSSEN ausdrückbar sein.
12. Nicht erfüllte harte Compatibility Requirements MÜSSEN die Anwendung blockieren.
13. Delta Updates MÜSSEN ihre erwartete Ausgangsversion eindeutig bestimmen.
14. Bei ungeeigneter Delta-Basis MUSS ein Full-Package-Fallback möglich sein.
15. State Migration MUSS explizit deklarierbar sein.
16. Update-Pakete DÜRFEN sich NICHT selbstständig außerhalb des Update Managers installieren.
17. Activation Requirements MÜSSEN explizit beschreibbar sein.
18. Rollback-Informationen SOLLEN Bestandteil kritischer Pakete sein.
19. Rollback DARF aktuelle Security- oder Revocation-Zustände NICHT abschwächen.
20. Verification Requirements MÜSSEN im Paket deklarierbar sein.
21. Installation DARF NICHT automatisch als erfolgreiche Verifikation gelten.
22. Resource Requirements SOLLEN vor Anwendung bestimmbar sein.
23. Mehrere Pakete MÜSSEN gemeinsam transaktional planbar sein.
24. Paket-Provenance MUSS nachvollziehbar sein.
25. Pakete SOLLEN mit Build- und Verification-Evidence verknüpfbar sein.
26. Unbekannte Required Fields MÜSSEN zur Ablehnung führen.
27. Das Paketformat MUSS kontrolliert versionierbar und erweiterbar sein.
28. Paketinformationen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-VERIFY-CONTRACT-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `NPSPEC-BOOT-AB-0001`
- `ADR-ARCH-0164`

## Ergebnis

```text
Update Package
      ↓
Validate Manifest
      ↓
Verify Integrity
      ↓
Verify Signature + Trust
      ↓
Resolve Dependencies
      ↓
Check Compatibility
      ↓
Create Update Plan
      ↓
Stage + Apply
      ↓
Activate
      ↓
Verify
      ↓
Commit / Rollback
```

NovaOS erhält damit ein versioniertes, signiertes und verifizierbares Update-Paketformat, das alle notwendigen Informationen für sichere, transaktionale und reproduzierbare Systemupdates bereitstellt, ohne Installationslogik oder Authority unkontrolliert in das Paket selbst zu verlagern.