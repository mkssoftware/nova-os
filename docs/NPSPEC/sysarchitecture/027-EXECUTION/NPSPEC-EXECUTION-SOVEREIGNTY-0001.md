# NPSPEC-EXECUTION-SOVEREIGNTY-0001 – Nova Execution Sovereignty

## Status

Angenommen

## Kategorie

Execution / Sovereignty / Execution Model

## Zweck

NovaOS definiert Sovereignty-Anforderungen als expliziten Bestandteil des Execution Contracts.

Sie bestimmen, **wo**, **durch wen** und **unter welcher administrativen oder technischen Kontrolle** eine Operation und ihre Daten verarbeitet werden dürfen.

```text
ExecutionContract
      ↓
Sovereignty Requirements
      ↓
Provider + Location Filtering
      ↓
Execution Planning
      ↓
Controlled Execution
```

## Grundprinzipien

```text
Sovereignty ≠ Location
Sovereignty ≠ Trust
Sovereignty ≠ Security
Sovereignty ≠ Privacy
Sovereignty ≠ Ownership

Local ≠ Automatically Sovereign
Remote ≠ Automatically Non-Sovereign
Trusted ≠ Sovereignty-Compliant
Encrypted ≠ Sovereignty-Compliant
```

## Sovereignty Requirement

Ein Execution Contract kann enthalten:

```text
ExecutionSovereigntyRequirement
├── Sovereignty Domain
├── Processing Policy
└── Requirement Class
```

Optional:

```text
Allowed Locations
Forbidden Locations
Allowed Providers
Allowed Organizations
LocalOnly
DeviceOnly
RemoteAllowed
Data Residency
Jurisdiction Constraints
Administrative Domain
Migration Policy
Fallback Policy
```

## Requirement Classes

NovaOS unterscheidet:

```text
Required
Preferred
NotRequired
```

Bei `Required` dürfen ausschließlich Ausführungspfade verwendet werden, die alle Sovereignty Constraints erfüllen.

```text
Requirement not satisfied
        ↓
Replan / Reject / Fail
```

Eine automatische Abschwächung ist nicht zulässig.

## Sovereignty Domains

NovaOS kann Ausführungsumgebungen logischen Sovereignty Domains zuordnen.

Beispiele:

```text
ThisDevice
UserDevices
LocalNetwork
Organization
PrivateInfrastructure
TrustedCloud
SpecificRegion
SpecificJurisdiction
```

Die konkrete Policy bestimmt, welche Ressourcen und Provider zu einer Domain gehören.

## Local Execution

Ein Contract kann verlangen:

```text
LocalOnly
```

Dann dürfen Operation und relevante Daten das erlaubte lokale Ausführungssystem nicht verlassen.

```text
Operation
   ↓
Local Providers Only
```

Remote Provider werden bereits bei der Discovery ausgeschlossen.

## Device-Only Execution

Strenger als `LocalOnly` kann gelten:

```text
DeviceOnly
```

Dabei muss die Verarbeitung auf dem aktuellen physischen Gerät stattfinden.

Dies kann insbesondere für sensible Daten oder sicherheitskritische Operationen verwendet werden.

## Data Residency

Sovereignty kann auch Datenbewegungen begrenzen.

```text
Input Data
   ↓
Execution
   ↓
Temporary Data
   ↓
Output Data
```

Für jede Phase können Residency Constraints gelten.

Beispiel:

```text
Input: DeviceOnly
Temporary Data: DeviceOnly
Output: UserDevices
```

## Provider Selection

Provider Discovery muss Sovereignty Constraints vor der Optimierung anwenden.

```text
Candidate Providers
       ↓
Sovereignty Filter
       ↓
Trust Filter
       ↓
Security Filter
       ↓
Resource Evaluation
       ↓
Optimization
```

Ein schnellerer oder energieeffizienterer Provider darf einen Required Sovereignty Constraint nicht verletzen.

## Location Transparency

NovaOS behält Location Transparency bei.

```text
Operation ≠ Location
Provider Identity ≠ Location
```

Sovereignty begrenzt jedoch, welche Locations für eine konkrete Ausführung zulässig sind.

```text
Location Transparency
        +
Sovereignty Constraints
        ↓
Controlled Location Resolution
```

## Trust Integration

Trust und Sovereignty werden getrennt geprüft.

```text
Trusted Provider
      +
Allowed Sovereignty Domain
      ↓
Eligible Provider
```

Ein vertrauenswürdiger Provider außerhalb der erlaubten Sovereignty Domain bleibt unzulässig.

## Security und Capabilities

Sovereignty erzeugt keine Autorität.

```text
Sovereignty Compliance
        ≠
Capability
```

Eine Operation benötigt weiterhin alle erforderlichen Capabilities und Security Permissions.

## Remote Execution

Remote Execution ist nur zulässig, wenn der Contract dies erlaubt.

```text
RemoteAllowed
      ↓
Sovereignty Check
      ↓
Trust Check
      ↓
Security Check
      ↓
Remote Execution
```

Dabei können berücksichtigt werden:

```text
Remote Location
Provider
Administrative Domain
Data Residency
Jurisdiction
Trust State
Transport
Temporary Storage
```

## Data Movement

Sovereignty gilt auch für Zwischenschritte.

```text
Local Data
   ↓
Conversion
   ↓
Processing
   ↓
Storage
```

NovaOS darf Daten nicht vorübergehend in eine unzulässige Domain übertragen, nur weil das Endergebnis wieder lokal gespeichert wird.

## Migration

Execution Migration muss Sovereignty Constraints erhalten.

```text
Node A
  ↓
Migration
  ↓
Node B
```

Vor der Migration muss geprüft werden:

```text
Target Sovereignty Domain
Data Residency
Trust
Capabilities
Security
```

Ein unzulässiges Ziel darf nicht verwendet werden.

## Fallback

Fallback Provider müssen dieselben Hard Sovereignty Constraints erfüllen.

```text
Primary Provider unavailable
        ↓
Fallback Discovery
        ↓
Sovereignty Validation
```

Existiert kein zulässiger Provider:

```text
Reject / Defer / Fail
```

statt die Sovereignty-Anforderung stillschweigend abzuschwächen.

## Dynamic Changes

Sovereignty-relevante Eigenschaften können sich ändern.

Beispiele:

```text
Provider Location Changed
Administrative Domain Changed
Policy Changed
Remote Node Migrated
Trust Relationship Changed
```

NovaOS muss darauf reagieren können:

```text
Revalidate
Replan
Migrate Back
Restrict
Cancel
Fail
```

## Verification

Während oder nach der Ausführung kann NovaOS überprüfen:

```text
Actual Provider
Actual Location
Sovereignty Domain
Data Movement
Temporary Storage
Migration History
Remote Dependencies
```

Damit kann festgestellt werden, ob die Ausführung innerhalb der vereinbarten Sovereignty Constraints geblieben ist.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Sovereignty Requirements
Allowed Domains
Forbidden Domains
Selected Provider
Execution Location
Data Residency
Migration State
Remote Dependencies
Compliance State
Violations
```

Sensible Standort- oder Infrastrukturinformationen dürfen nur entsprechend der Security Policy offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Sovereignty Requirements als Bestandteil von Execution Contracts unterstützen.
2. Sovereignty MUSS von Location, Trust, Security, Privacy und Authority getrennt bleiben.
3. Required Sovereignty Constraints DÜRFEN NICHT stillschweigend abgeschwächt werden.
4. Provider Discovery MUSS Hard Sovereignty Constraints vor adaptiver Optimierung berücksichtigen.
5. Data Residency MUSS für Eingaben, Zwischendaten und Ergebnisse definierbar sein.
6. Remote Execution DARF nur innerhalb der erlaubten Sovereignty Domains erfolgen.
7. Migration MUSS Sovereignty Constraints erneut validieren.
8. Fallback Provider MÜSSEN dieselben Hard Sovereignty Requirements erfüllen.
9. Temporäre Datenbewegungen DÜRFEN Sovereignty Constraints NICHT umgehen.
10. Sovereignty Compliance DARF keine Capability oder Zugriffsautorität erzeugen.
11. Änderungen relevanter Sovereignty-Eigenschaften MÜSSEN Revalidation ermöglichen.
12. Sovereignty State und Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0055`

## Ergebnis

```text
ExecutionContract
      ↓
Sovereignty Requirements
      ↓
Provider + Location + Data Filtering
      ↓
Trust + Security Validation
      ↓
Sovereignty-Compliant Execution
      ↓
Continuous Verification
```

NovaOS erhält damit ein durchgängiges Execution-Sovereignty-Modell, bei dem der Nutzer oder die System-Policy kontrollieren kann, in welchen technischen und administrativen Domains eine Operation und ihre Daten verarbeitet werden dürfen, ohne diese Kontrolle mit Location, Trust, Security oder Capabilities gleichzusetzen.