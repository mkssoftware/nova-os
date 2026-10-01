# NPSPEC-EXECUTION-TRUST-0001 – Nova Execution Trust

## Status

Angenommen

## Kategorie

Execution / Trust / Execution Model

## Zweck

NovaOS definiert Trust-Anforderungen als expliziten Bestandteil des Execution Contracts.

Damit kann eine Operation festlegen, welchen Vertrauensanforderungen Provider, Code, Ressourcen, Geräte und Ausführungsumgebungen entsprechen müssen.

```text
ExecutionContract
      ↓
Trust Requirements
      ↓
Trust Evaluation
      ↓
Provider + Resource Selection
      ↓
Execution
      ↓
Trust Verification
```

## Grundprinzipien

```text
Trust ≠ Identity
Trust ≠ Authentication
Trust ≠ Authorization
Trust ≠ Capability
Trust ≠ Signature
Trust ≠ Integrity

Signed ≠ Trusted
Known ≠ Trusted
Trusted ≠ Authorized
Trusted Provider ≠ Unlimited Authority
Unknown Trust ≠ Trusted
```

Trust ist kontextabhängig und darf nicht als globale binäre Eigenschaft behandelt werden.

## Trust Requirement

Ein Execution Contract kann enthalten:

```text
ExecutionTrustRequirement
├── Trust Domain
├── Minimum Trust Level
└── Requirement Class
```

Optional:

```text
Required Attestation
Required Code Integrity
Required Signatures
Required Provenance
Allowed Providers
Allowed Software Sources
Allowed Devices
Trust Anchors
Freshness Requirement
Revocation Requirement
Fallback Policy
```

## Requirement Classes

NovaOS unterscheidet:

```text
Required
Preferred
NotRequired
```

Bei `Required` dürfen nur Ausführungspfade verwendet werden, welche die Trust-Anforderungen erfüllen.

```text
Trust Requirement
      ↓ not satisfied
Reject / Replan / Fail
```

Eine automatische Abschwächung ist nicht zulässig.

## Trust Domains

Trust wird innerhalb eines definierten Kontextes bewertet.

Beispiele:

```text
System
Kernel
Driver
Application
Provider
Device
Organization
User
Remote Service
Supply Chain
```

Vertrauen in einer Domain darf nicht automatisch auf eine andere Domain übertragen werden.

```text
Trust(A) ≠ Trust(B)
```

## Provider Trust

Vor der Provider-Auswahl kann NovaOS prüfen:

```text
Provider Identity
Code Integrity
Software Provenance
Signature
Attestation
Version
Revocation State
Trust Policy
```

Nur danach wird der Provider gegen die Anforderungen des Execution Contracts bewertet.

## Code Trust

Ausgeführter Code kann Anforderungen besitzen wie:

```text
Signed Code Required
Verified Publisher Required
Known Build Required
Measured Code Required
Attested Runtime Required
```

Eine gültige Signatur beweist lediglich die entsprechende kryptografische Aussage und stellt nicht automatisch Vertrauen her.

## Device Trust

Hardware kann Bestandteil der Trust-Anforderungen sein.

```text
CPU
GPU
NPU
Storage
Network Device
External Device
```

Beispiel:

```text
ExecutionContract
    ↓
Trusted Local Accelerator Required
    ↓
GPU A → Accepted
GPU B → Unknown Trust
Remote GPU → Rejected
```

## Attestation

Für kritische Ausführungen kann Attestation erforderlich sein.

```text
Execution Environment
      ↓
Measurement
      ↓
Attestation Evidence
      ↓
Trust Evaluation
```

Attestation liefert Evidenz über einen Zustand, entscheidet aber nicht selbst über Vertrauen.

```text
Attestation ≠ Trust Decision
```

## Provenance

NovaOS kann Herkunftsinformationen berücksichtigen.

```text
Provider
   ↓
Software Artifact
   ↓
Build
   ↓
Publisher
   ↓
Supply Chain
```

Unvollständige Provenance kann abhängig vom Contract zu:

```text
Accept
Restricted
Replan
Reject
```

führen.

## Trust und Capabilities

Trust entscheidet nicht über Autorität.

```text
Trusted Provider
      +
Required Capability
      ↓
Authorized Execution
```

Ein vertrauenswürdiger Provider ohne notwendige Capability darf die Operation nicht durchführen.

Ebenso macht eine Capability einen Provider nicht automatisch vertrauenswürdig.

## Trust und Sovereignty

Trust kann gemeinsam mit Sovereignty Constraints verwendet werden.

```text
Trusted Provider
      +
Allowed Sovereignty Domain
      +
Allowed Location
      ↓
Eligible Provider
```

Alle Hard Constraints müssen gleichzeitig erfüllt sein.

## Remote Execution

Bei Remote Execution können zusätzliche Anforderungen gelten:

```text
Remote Identity
Remote Attestation
Transport Security
Provider Trust
Software Provenance
Data Sovereignty
Execution Environment
```

Eine verschlüsselte Verbindung macht den Remote Provider nicht automatisch vertrauenswürdig.

```text
Encryption ≠ Trust
```

## Dynamic Trust

Trust kann sich während einer Ausführung ändern.

Beispiele:

```text
Certificate Revoked
Provider Revoked
Attestation Expired
Integrity Violation
Software Changed
Trust Policy Changed
```

NovaOS muss darauf reagieren können:

```text
Continue
Restrict
Revalidate
Replan
Migrate
Cancel
Fail
```

Die erlaubte Reaktion wird durch den Execution Contract bestimmt.

## Trust Freshness

Trust Evidence kann zeitlich begrenzt sein.

```text
Evidence
├── Created
├── Valid Until
└── Last Verified
```

Veraltete Evidenz darf bei erforderlicher aktueller Verifikation nicht als gültiger Trust-Nachweis behandelt werden.

## Revocation

Trust Evaluation muss Revocation berücksichtigen können.

```text
Valid
Revoked
Expired
Unknown
```

Dabei gilt:

```text
Unknown ≠ Valid
```

Für `Required Trust` darf ein unbekannter Revocation State nicht stillschweigend akzeptiert werden.

## Fallback

Fallback Provider müssen dieselben Hard Trust Requirements erfüllen.

```text
Primary Provider
      ↓ unavailable
Fallback Provider
      ↓
Trust Evaluation
```

Ein schneller oder lokal verfügbarer Provider darf nicht verwendet werden, wenn er die erforderliche Trust Policy verletzt.

## Verification

Während oder nach der Ausführung kann NovaOS prüfen:

```text
Expected Provider
Actual Provider
Provider Version
Code Measurement
Attestation State
Trust Domain
Trust Evidence
Revocation State
Provenance
```

Damit kann festgestellt werden, ob die Ausführung innerhalb der vereinbarten Trust Constraints stattgefunden hat.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Trust Requirements
Trust Domain
Selected Provider
Trust State
Trust Evidence
Attestation State
Provenance
Revocation State
Evidence Freshness
Trust Violations
```

Sensible Trust Evidence darf dabei nur entsprechend ihrer Security Policy offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Trust Requirements als Bestandteil von Execution Contracts unterstützen.
2. Trust MUSS von Identity, Authentication, Authorization und Capabilities getrennt bleiben.
3. Required Trust DARF nicht stillschweigend abgeschwächt werden.
4. Trust MUSS kontext- und domainabhängig bewertet werden können.
5. Provider Selection MUSS erforderliche Trust Constraints berücksichtigen.
6. Signaturen, Provenance und Attestation DÜRFEN nicht automatisch mit Trust gleichgesetzt werden.
7. Remote Execution MUSS dieselben Hard Trust Requirements wie lokale Ausführung erfüllen.
8. Trust Evidence MUSS Freshness und Revocation berücksichtigen können.
9. `Unknown` DARF bei Required Trust NICHT automatisch als vertrauenswürdig gelten.
10. Fallback und Replanning DÜRFEN Required Trust Constraints NICHT umgehen.
11. Trust-Änderungen während einer Ausführung MÜSSEN kontrollierte Reaktionen ermöglichen.
12. Trust State und relevante Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-SEMANTICTYPES-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-TRUST-ARCH-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-TRUST-REVOCATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0054`

## Ergebnis

```text
ExecutionContract
      ↓
Trust Requirements
      ↓
Identity + Provenance + Attestation
      ↓
Trust Evaluation
      ↓
Eligible Providers
      ↓
Controlled Execution
      ↓
Continuous Trust Verification
```

NovaOS erhält damit ein durchgängiges Execution-Trust-Modell, bei dem Vertrauen bereits vor der Provider-Auswahl als explizite Ausführungsanforderung berücksichtigt und während der Ausführung überprüft wird, ohne Trust mit Identität, Autorität oder kryptografischer Signatur gleichzusetzen.