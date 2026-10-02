# NPSPEC-API-VERSIONING-0001 – Nova API Versioning

## Status

Angenommen

## Kategorie

API / Versioning / Compatibility

## Zweck

NovaOS definiert ein einheitliches Versionierungsmodell für öffentliche und interne APIs.

```text
API Consumer
     ↓
API Version + Features
     ↓
Compatibility Check
     ↓
API Provider
```

API-Versionierung ermöglicht die kontrollierte Weiterentwicklung von Systemdiensten, Libraries, Capabilities und Schnittstellen, ohne bestehende Software unnötig zu brechen.

## Grundprinzipien

```text
API Version ≠ ABI Version
API Version ≠ OS Version
API Compatibility ≠ ABI Compatibility
Newer ≠ Compatible
Deprecated ≠ Removed
Version Match ≠ Semantic Compatibility
API Access ≠ Authority
```

## API Version Model

```text
APIVersion
├── API_ID
├── Major
├── Minor
├── FeatureSet
├── CompatibilityPolicy
└── LifecycleState
```

Optional:

```text
MinimumVersion
MaximumVersion
RequiredFeatures
DeprecatedFeatures
ProviderID
SemanticTypeID
```

## Versionsformat

Grundsätzlich:

```text
Major.Minor
```

Beispiel:

```text
Nova.Storage 1.0
Nova.Storage 1.1
Nova.Storage 2.0
```

### Major

Eine neue Major-Version darf inkompatible Änderungen enthalten.

```text
1.x → 2.x
```

### Minor

Eine Minor-Version soll kompatible Erweiterungen enthalten.

```text
1.0 → 1.1
```

Bestehende garantierte Semantik darf innerhalb derselben Major-Version nicht stillschweigend verändert werden.

## API Identity

Jede versionierte API benötigt eine stabile Identität.

```text
API_ID + Version
```

Beispiele:

```text
Nova.Storage
Nova.Network
Nova.Compute
Nova.Math
Nova.Security
```

API-Identität bleibt von Provider, Prozess und Standort getrennt.

```text
API Identity ≠ Provider Identity
```

## Compatibility

Ein Consumer kann Anforderungen deklarieren:

```text
Required API
├── API_ID
├── MinimumVersion
├── MaximumVersion
└── RequiredFeatures
```

Der Provider veröffentlicht seine unterstützten Versionen und Features.

```text
Consumer Requirements
        ∩
Provider Support
        ↓
Compatible API
```

## Feature Discovery

Optionale Funktionen sollen nicht ausschließlich über Versionsnummern erkannt werden.

```text
Query API
   ↓
Discover Features
   ↓
Select Supported Feature
```

Beispiele:

```text
FEATURE_ASYNC
FEATURE_ZEROCOPY
FEATURE_TRANSACTIONS
FEATURE_STREAMING
```

## Semantic Compatibility

API-Kompatibilität umfasst mehr als Funktionsnamen und Parameter.

Änderungen an folgenden Eigenschaften können inkompatibel sein:

```text
Meaning
Ownership
Lifetime
Error Semantics
Concurrency
Ordering
Security
Transaction Semantics
Side Effects
```

```text
Same Signature ≠ Same Contract
```

## Additive Evolution

Neue Funktionalität soll bevorzugt additiv eingeführt werden.

```text
API 1.0
├── Operation A
└── Operation B

API 1.1
├── Operation A
├── Operation B
└── Operation C
```

Bestehende Consumer müssen neue optionale Funktionen nicht kennen.

## Request Versioning

Komplexe Requests sollen evolutionstauglich sein.

```text
Request
├── Size
├── Version
├── Flags
└── Fields
```

Unbekannte optionale Felder können entsprechend der API-Regel ignoriert werden.

Unbekannte erforderliche Semantik muss abgelehnt werden.

## Response Versioning

Responses müssen ebenfalls eindeutig interpretierbar bleiben.

```text
Response
├── Version
├── Status
├── Result
└── Optional Fields
```

Consumer dürfen nicht voraussetzen, dass zukünftige Provider ausschließlich bekannte Felder liefern.

## Error Stability

Fehlersemantik gehört zum API Contract.

```text
Success
InvalidArgument
AccessDenied
NotSupported
Unavailable
Timeout
Cancelled
Conflict
UnknownState
```

Bestehende Fehler dürfen nicht stillschweigend eine grundlegend andere Bedeutung erhalten.

## Deprecation

API-Elemente können einen kontrollierten Lifecycle besitzen:

```text
Experimental
    ↓
Stable
    ↓
Deprecated
    ↓
Removed in Future Major Version
```

Deprecation soll maschinenlesbar erkennbar sein.

## Parallel Versions

NovaOS darf mehrere Major-Versionen einer API gleichzeitig unterstützen.

```text
Nova.Storage 1.x
Nova.Storage 2.x
```

Dadurch können Migrationen schrittweise erfolgen.

## API Negotiation

Bei dynamischer Bindung:

```text
Consumer
Supports 1.0–1.4
      ↓
Negotiation
      ↑
Provider
Supports 1.2–2.0

Selected: Compatible 1.x Profile
```

Es darf nur ein tatsächlich gemeinsam unterstütztes Profil gewählt werden.

## Provider Replacement

Da API-Identität von Provider-Identität getrennt ist, kann ein Provider ersetzt werden.

```text
API Consumer
     ↓
Nova API Contract
     ↓
Provider A → Provider B
```

Der neue Provider muss die ausgehandelte API-Semantik erfüllen.

## Capability Integration

API-Verfügbarkeit erzeugt keine Authority.

```text
API Discovered
     ≠
API Authorized
```

Operationen benötigen weiterhin die erforderlichen Capabilities.

## Security

API-Versionierung darf keinen Security Downgrade ermöglichen.

```text
Compatible API
     +
Security Policy
     +
Trust Policy
     ↓
Usable API
```

Eine ältere API-Version darf abgelehnt werden, wenn sie aktuelle Sicherheitsanforderungen nicht erfüllt.

## API und ABI

API und ABI werden unabhängig versioniert.

```text
API Contract
    ↓
Implementation
    ↓
Nova ABI
```

Eine API kann intern verändert werden, ohne die ABI zu verändern.

Ebenso kann eine ABI intern weiterentwickelt werden, während die öffentliche API stabil bleibt.

## Distributed APIs

Remote APIs müssen zusätzlich berücksichtigen:

```text
Protocol Version
Schema Version
Serialization
Feature Negotiation
Provider Compatibility
```

Transport- oder Schema-Versionen dürfen nicht automatisch mit der API-Version gleichgesetzt werden.

## Live Evolution

API-Versionierung unterstützt parallele Migration:

```text
Provider v1
    +
Provider v2
    ↓
Migrate Consumers
    ↓
Retire v1
```

Bestehende Sessions dürfen entsprechend ihrer Verträge kontrolliert weiterlaufen oder migriert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
API_ID
Supported Versions
Selected Version
Supported Features
Deprecated Features
Provider
Compatibility State
Lifecycle State
```

## Normative Anforderungen

1. NovaOS MUSS versionierte APIs eindeutig identifizieren können.
2. API-Version und ABI-Version MÜSSEN getrennte Konzepte bleiben.
3. API-Version und OS-Version MÜSSEN getrennte Konzepte bleiben.
4. Major-Versionen MÜSSEN inkompatible Änderungen ausdrücken können.
5. Minor-Versionen SOLLEN kompatible Erweiterungen darstellen.
6. Bestehende Semantik DARF innerhalb kompatibler Versionen NICHT stillschweigend verändert werden.
7. Consumer MÜSSEN benötigte Versionen und Features deklarieren können.
8. Provider MÜSSEN unterstützte Versionen und Features veröffentlichen können.
9. Optionale Funktionen SOLLEN über Feature Discovery erkannt werden.
10. API-Kompatibilität MUSS semantische Verträge berücksichtigen.
11. Neue Funktionen SOLLEN bevorzugt additiv eingeführt werden.
12. Request- und Response-Strukturen SOLLEN evolutionstauglich sein.
13. Fehlersemantik MUSS Bestandteil des API Contracts sein.
14. Deprecated APIs SOLLEN maschinenlesbar erkennbar sein.
15. Mehrere Major-Versionen DÜRFEN parallel unterstützt werden.
16. Version Negotiation DARF nur tatsächlich kompatible Versionen auswählen.
17. API Identity MUSS von Provider Identity getrennt bleiben.
18. Provider Replacement MUSS den ausgehandelten API Contract erhalten.
19. API Discovery DARF NICHT als Authority interpretiert werden.
20. API-Versionierung DARF Security-, Trust- oder Capability-Regeln NICHT umgehen.
21. Unsichere API-Versionen MÜSSEN deaktivierbar sein.
22. Distributed APIs MÜSSEN API-, Schema- und Protocol-Versionierung getrennt behandeln können.
23. Live Evolution SOLL parallele API-Versionen und Migration unterstützen.
24. API-Versionierungszustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ABI-STABLE-0001`
- `NPSPEC-ABI-VERSIONING-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-NEGOTIATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0146`

## Ergebnis

```text
Consumer
    ↓
API_ID + Version Requirements
    ↓
Discover Provider
    ↓
Version + Feature Negotiation
    ↓
Security + Capability Validation
    ↓
Compatible?
├── Yes → Bind
├── Alternative Provider → Retry
└── No → Fallback / Reject
```

NovaOS erhält damit ein einheitliches API-Versionierungsmodell, das APIs unabhängig von ABI, Provider und Betriebssystemversion weiterentwickeln lässt und gleichzeitig Kompatibilität, Feature Negotiation, Sicherheit und langfristige Systementwicklung unterstützt.