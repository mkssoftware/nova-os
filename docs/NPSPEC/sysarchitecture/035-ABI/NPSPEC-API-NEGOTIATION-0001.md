# NPSPEC-API-NEGOTIATION-0001 – Nova API Negotiation

## Status

Angenommen

## Kategorie

API / Negotiation / Compatibility

## Zweck

NovaOS definiert API Negotiation als kontrollierten Prozess zur Ermittlung eines gemeinsamen API-Profils zwischen Consumer und Provider.

```text
Consumer Requirements
        ↓
API Negotiation
        ↑
Provider Capabilities
        ↓
Effective API Contract
```

Dabei werden Version, Features, semantische Anforderungen und System-Constraints gemeinsam berücksichtigt.

## Grundprinzipien

```text
Negotiation ≠ Discovery
Negotiation ≠ Authority
Negotiation ≠ Provider Selection
Negotiation ≠ Security Downgrade
Compatible ≠ Authorized
Supported ≠ Selected
Preferred ≠ Required
```

## Negotiation Model

```text
APINegotiation
├── NegotiationID
├── API_ID
├── ConsumerRequirements
├── ProviderCapabilities
├── RequiredFeatures
├── OptionalFeatures
├── Constraints
└── State
```

Optional:

```text
ExecutionContractID
RequiredCapabilities
TrustRequirements
SovereigntyRequirements
CompatibilityPolicy
FallbackPolicy
Deadline
ProvenanceID
```

## Zustände

```text
Created
Validating
Negotiating
Compatible
Incompatible
Selected
Rejected
Expired
Unknown
```

## Ablauf

```text
Consumer Requirements
        ↓
Provider Capabilities
        ↓
Version Intersection
        ↓
Feature Intersection
        ↓
Semantic Validation
        ↓
Constraint Validation
        ↓
Security Validation
        ↓
Effective API Contract
```

## Version Negotiation

Beispiel:

```text
Consumer:
1.1 – 1.4

Provider:
1.2 – 2.0

Intersection:
1.2 – 1.4
```

Aus dem gemeinsamen Bereich wird entsprechend der Compatibility Policy eine konkrete Version ausgewählt.

```text
Newest ≠ Automatically Best
```

## Feature Negotiation

Features werden in erforderliche und optionale Eigenschaften getrennt.

```text
Required:
Transactions
Async

Optional:
ZeroCopy
Streaming
```

Fehlt ein Required Feature, ist der Provider für diesen Contract nicht kompatibel.

Fehlende optionale Features dürfen einen definierten Fallback verwenden.

## Semantic Negotiation

Gleiche Funktionsnamen oder Datentypen reichen nicht aus.

Geprüft werden müssen relevante:

```text
Input Semantics
Output Semantics
Error Semantics
Ownership
Lifetime
Side Effects
Concurrency
Transaction Semantics
```

```text
Binary Compatible ≠ Semantically Compatible
```

## Contract Negotiation

Aus Consumer- und Provider-Vertrag entsteht der effektive Contract.

```text
Consumer Requirements
        ∩
Provider Guarantees
        ∩
System Constraints
        ↓
Effective Contract
```

Verpflichtende Anforderungen dürfen dabei nicht stillschweigend abgeschwächt werden.

## Capability Integration

Negotiation erzeugt keine Authority.

```text
Negotiated API
      ↓
Capability Validation
      ↓
Authorized Execution
```

```text
Negotiated ≠ Authorized
```

## Security

Security-Anforderungen sind harte Constraints.

Eine ältere oder schwächere API-Version darf nicht gewählt werden, wenn dadurch aktuelle Sicherheitsanforderungen verletzt werden.

```text
Compatibility
     ↓
Security Policy
     ↓
Eligible Contract
```

## Trust und Sovereignty

Negotiation muss Anforderungen berücksichtigen können wie:

```text
Minimum Trust
Required Attestation
Allowed Provider
Allowed Location
Allowed Jurisdiction
Data Residency
```

Diese Anforderungen dürfen nicht zugunsten höherer Performance abgeschwächt werden.

## Execution Contract

Der Execution Contract kann zusätzliche Anforderungen liefern:

```text
Latency
Deadline
Resource Budget
Determinism
Trust
Sovereignty
Preferred Provider
Forced Provider
```

API Negotiation muss diese Constraints berücksichtigen.

## Preference Handling

Consumer können Präferenzen angeben:

```text
Prefer ZeroCopy
Prefer Async
Prefer Local Provider
Prefer Newest Compatible Version
```

Präferenzen gelten nur innerhalb der zulässigen Lösungsmenge.

```text
Hard Constraint
    >
Explicit Requirement
    >
Preference
```

## Fallback

Negotiation kann definierte Fallbacks verwenden.

```text
Preferred Profile
      ↓ unavailable
Fallback Profile
```

Beispiel:

```text
ZeroCopy
   ↓ unavailable
Safe Copy
```

Fallback darf keine verpflichtende Anforderung verletzen.

## Multiple Providers

Mehrere Provider können erfolgreich verhandelte Contracts anbieten.

```text
API
├── Provider A → Contract A
├── Provider B → Contract B
└── Provider C → Incompatible
```

Die anschließende Provider Selection bleibt ein separater Schritt.

## Dynamic Renegotiation

Ändern sich relevante Bedingungen, kann eine erneute Negotiation erforderlich sein.

Beispiele:

```text
Provider Replacement
Capability Revocation
Feature Loss
Trust Change
Resource Change
Migration
API Upgrade
```

Bestehende aktive Operationen dürfen dadurch nicht unkontrolliert ihre Semantik ändern.

## Distributed Negotiation

Bei Remote Providern können zusätzlich ausgehandelt werden:

```text
Protocol Version
Schema Version
Serialization
Transport Features
Compression
Streaming
```

Diese bleiben von der eigentlichen API-Version getrennt.

## Downgrade Protection

Negotiation muss Downgrade-Angriffe verhindern können.

```text
Supported Secure Version
        ↓
Forced Older Version
        ↓
Reject
```

Minimum Security Requirements dürfen nicht unterschritten werden.

## Caching

Erfolgreiche Negotiation-Ergebnisse dürfen gecacht werden.

```text
Negotiation Result
      ↓
Cache
```

Änderungen an Provider, Version, Trust, Capabilities oder Policy können eine Revalidation erzwingen.

```text
Cached Contract ≠ Permanently Valid Contract
```

## Provenance

Nachvollziehbar sein sollen:

```text
NegotiationID
API_ID
Consumer Requirements
Provider Capabilities
Rejected Alternatives
Selected Version
Selected Features
Fallbacks
Effective Contract
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
NegotiationID
API_ID
State
Consumer Requirements
Provider Capabilities
Compatible Versions
Required Features
Selected Features
Effective Contract
Fallback State
```

## Normative Anforderungen

1. NovaOS MUSS API Negotiation unterstützen können.
2. Negotiation MUSS von Discovery, Provider Selection und Execution getrennt bleiben.
3. Negotiation DARF NICHT als Authority interpretiert werden.
4. Consumer MÜSSEN Version Requirements deklarieren können.
5. Provider MÜSSEN unterstützte Versionen deklarieren können.
6. Nur gemeinsam kompatible Versionen DÜRFEN ausgewählt werden.
7. Required und Optional Features MÜSSEN unterscheidbar sein.
8. Fehlende Required Features MÜSSEN zur Inkompatibilität führen.
9. Semantic Compatibility MUSS zusätzlich zur Versionskompatibilität geprüft werden können.
10. Der effektive API Contract MUSS explizit bestimmbar sein.
11. Verpflichtende Anforderungen DÜRFEN NICHT stillschweigend abgeschwächt werden.
12. Negotiation DARF keine Capabilities erzeugen.
13. Security Requirements MÜSSEN als harte Constraints behandelt werden.
14. Negotiation DARF keinen verbotenen Security Downgrade durchführen.
15. Trust- und Sovereignty-Anforderungen MÜSSEN berücksichtigt werden können.
16. Execution Contracts MÜSSEN in Negotiation einbezogen werden können.
17. Präferenzen DÜRFEN harte Constraints NICHT überschreiben.
18. Fallback MUSS explizit definiert und validiert sein.
19. Provider Selection MUSS nach Negotiation ein separater Schritt bleiben.
20. Dynamische Renegotiation MUSS unterstützt werden können.
21. Remote Negotiation MUSS API-, Schema- und Protocol-Versionen getrennt behandeln können.
22. Cached Negotiation Results MÜSSEN revalidierbar sein.
23. Negotiation Decisions SOLLEN nachvollziehbar sein.
24. Negotiation State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-API-VERSIONING-0001`
- `NPSPEC-API-CONTRACT-0001`
- `NPSPEC-API-SEMANTIC-0001`
- `NPSPEC-API-INTENT-0001`
- `NPSPEC-API-DISCOVERY-0001`
- `NPSPEC-ABI-VERSIONING-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-DISTCOMM-NEGOTIATION-0001`
- `ADR-ARCH-0151`

## Ergebnis

```text
API Discovery
     ↓
Consumer Requirements
     +
Provider Capabilities
     ↓
Version Intersection
     ↓
Feature Negotiation
     ↓
Semantic Compatibility
     ↓
Security + Trust + Sovereignty
     ↓
Execution Constraints
     ↓
Effective API Contract
     ↓
Provider Selection
```

NovaOS erhält damit eine einheitliche API-Negotiation-Schicht, die Consumer und Provider auf einen expliziten kompatiblen Contract zusammenführt, ohne dabei Authority, Sicherheitsanforderungen oder harte System-Constraints zugunsten von Kompatibilität oder Optimierung abzuschwächen.