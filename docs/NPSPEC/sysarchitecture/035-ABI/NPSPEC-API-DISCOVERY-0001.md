# NPSPEC-API-DISCOVERY-0001 – Nova API Discovery

## Status

Angenommen

## Kategorie

API / Discovery / Resolution

## Zweck

NovaOS definiert einen einheitlichen Mechanismus zur dynamischen Ermittlung verfügbarer APIs, Versionen, Funktionen und Provider.

```text
Consumer
   ↓
API Discovery
   ↓
Compatible APIs
   ↓
Provider Resolution
   ↓
Contract + Security Validation
   ↓
Bind
```

Consumer müssen konkrete Provider, Prozesse oder Implementierungen nicht fest einprogrammieren.

## Grundprinzipien

```text
Discovery ≠ Authority
Discovery ≠ Binding
Discovery ≠ Compatibility
Discovery ≠ Availability Guarantee
Provider Found ≠ Provider Trusted
API Found ≠ Operation Authorized
```

Discovery liefert Informationen über mögliche Schnittstellen.

Die tatsächliche Nutzung benötigt weitere Validierung.

## Discovery Model

```text
APIDiscoveryRequest
├── API_ID
├── VersionRequirements
├── RequiredFeatures
├── SemanticRequirements
└── Constraints
```

Optional:

```text
RequiredCapabilities
ExecutionContractID
TrustRequirements
SovereigntyRequirements
ProviderConstraints
LocationConstraints
```

Ergebnis:

```text
APIDiscoveryResult
├── API_ID
├── Version
├── ProviderID
├── Features
├── ContractID
└── AvailabilityState
```

## Discovery Sources

APIs können aus unterschiedlichen Quellen ermittelt werden:

```text
Local Registry
System Services
Loaded Modules
Capability Registry
Remote Registry
Cluster Services
Distributed Discovery
```

Die Herkunft muss nachvollziehbar bleiben.

## Semantic Discovery

Discovery soll nicht ausschließlich über Namen erfolgen.

```text
Required Capability
       ↓
Semantic API Discovery
       ↓
Matching APIs
```

Beispiel:

```text
Intent: Image anzeigen
        ↓
Semantic Requirement: Image.Render
        ↓
Compatible API
```

## Version Discovery

Provider veröffentlichen unterstützte API-Versionen.

```text
Provider
├── API 1.0
├── API 1.1
└── API 2.0
```

Consumer können Bereiche anfordern:

```text
Required:
API 1.0–1.x
```

Discovery liefert nur Kandidaten; die endgültige Kompatibilität wird anschließend validiert.

## Feature Discovery

Optionale Funktionen müssen explizit ermittelbar sein.

```text
API
├── Async
├── ZeroCopy
├── Transactions
└── Streaming
```

Consumer dürfen optionale Features nicht allein aus Versionsnummern ableiten.

## Provider Discovery

Mehrere Provider können dieselbe API anbieten.

```text
API_ID
  ↓
Provider A
Provider B
Provider C
```

Discovery darf Provider finden, aber nicht eigenständig den endgültigen Provider auswählen.

Die Auswahl erfolgt durch Provider Resolution und Execution Policy.

## Constraints

Discovery kann Kandidaten anhand expliziter Anforderungen filtern:

```text
Architecture
Version
Features
Location
Trust
Sovereignty
Resources
Latency
Determinism
```

Harte Constraints dürfen nicht stillschweigend abgeschwächt werden.

## Capability Integration

Discovery darf keine Capability erzeugen.

```text
API Found
   ↓
Required Capability
   ↓
Capability Validation
   ↓
Authorized Use
```

Das Wissen über eine API oder einen Provider stellt keine Authority dar.

## Trust

Provider müssen getrennt von Discovery bewertet werden.

```text
Discovered
   ↓
Identity Validation
   ↓
Trust Validation
   ↓
Eligible Provider
```

```text
Discoverable ≠ Trusted
```

## Dynamic Registration

Provider können APIs dynamisch registrieren und entfernen.

```text
Provider Start
    ↓
Register API

Provider Stop
    ↓
Withdraw API
```

Registrierung muss authentifiziert und autorisiert sein.

## Lifecycle

Discovery muss Änderungen der Providerlandschaft berücksichtigen.

```text
Available
Unavailable
Starting
Stopping
Degraded
Unknown
```

Ein zuvor gefundener Provider darf bei späterer Verwendung nicht automatisch als weiterhin verfügbar angenommen werden.

## Caching

Discovery-Ergebnisse dürfen gecacht werden.

```text
Discovery
   ↓
Cache
   ↓
Reuse
```

Cache-Einträge benötigen Gültigkeitsinformationen.

```text
Cached ≠ Current
```

Vor kritischen Operationen kann Revalidation erforderlich sein.

## Local und Remote Discovery

NovaOS kann APIs lokal oder remote finden.

```text
Discovery
├── Local
├── Node
├── Cluster
└── Remote
```

Location Transparency darf Trust-, Security- oder Sovereignty-Regeln nicht umgehen.

## Intent Integration

Intent Resolution kann API Discovery verwenden:

```text
Intent
   ↓
Semantic Requirement
   ↓
API Discovery
   ↓
Provider Candidates
```

Dadurch muss ein Intent keine konkrete API-Implementierung kennen.

## Fallback

Ist kein geeigneter Provider verfügbar:

```text
Discovery
   ↓
No Compatible Provider
   ↓
Fallback Policy
```

Mögliche Ergebnisse:

```text
Alternative API
Alternative Provider
Local Fallback
Deferred Execution
Degraded Function
Unsupported
```

Fallback darf harte Anforderungen nicht verletzen.

## Security

Discovery-Daten können sicherheitsrelevante Informationen enthalten.

Zugriff auf detaillierte Informationen kann Capability-basiert eingeschränkt werden.

Nicht autorisierte Consumer dürfen keine unnötigen Informationen über interne:

```text
Kernel Services
Security Services
Devices
Providers
Endpoints
```

erhalten.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Registered APIs
API Versions
Features
Providers
Provider State
Contracts
Locations
Compatibility State
Discovery Source
```

## Normative Anforderungen

1. NovaOS MUSS dynamische API Discovery unterstützen können.
2. Discovery MUSS von Binding und Execution getrennt bleiben.
3. Discovery DARF NICHT als Authority interpretiert werden.
4. APIs MÜSSEN über stabile API IDs auffindbar sein können.
5. Semantic API Discovery SOLL unterstützt werden.
6. Version Requirements MÜSSEN bei Discovery berücksichtigt werden können.
7. Optionale Features MÜSSEN explizit discoverable sein.
8. Consumer DÜRFEN optionale Features NICHT ausschließlich aus Versionsnummern ableiten.
9. Mehrere Provider MÜSSEN für dieselbe API discoverable sein können.
10. Discovery DARF Provider Selection NICHT mit Authority gleichsetzen.
11. Harte Constraints DÜRFEN bei Discovery NICHT stillschweigend abgeschwächt werden.
12. Gefundene Provider MÜSSEN separat auf Compatibility, Security und Trust validierbar sein.
13. Provider Registration MUSS authentifizierbar und autorisierbar sein.
14. Provider MÜSSEN APIs dynamisch registrieren und zurückziehen können.
15. Discovery MUSS Provider-Lifecycle-Zustände darstellen können.
16. Cached Discovery Results DÜRFEN NICHT automatisch als aktuell betrachtet werden.
17. Kritische Operationen MÜSSEN Discovery-Ergebnisse revalidieren können.
18. Local und Remote Discovery MÜSSEN unterstützt werden können.
19. Remote Discovery DARF Sovereignty- und Trust-Regeln NICHT umgehen.
20. Intent Resolution MUSS API Discovery verwenden können.
21. Fallback DARF harte Execution Constraints NICHT verletzen.
22. Sensitive Discovery-Informationen MÜSSEN Capability-basiert geschützt werden können.
23. Discovery-Herkunft und Provider-Zustand SOLLEN nachvollziehbar sein.
24. API Discovery MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-API-VERSIONING-0001`
- `NPSPEC-API-CONTRACT-0001`
- `NPSPEC-API-SEMANTIC-0001`
- `NPSPEC-API-INTENT-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0150`

## Ergebnis

```text
Consumer Requirement
        ↓
API Discovery
        ↓
API + Version + Feature Candidates
        ↓
Provider Candidates
        ↓
Compatibility
Security
Trust
Sovereignty
Capability
        ↓
Eligible Providers
        ↓
Provider Resolution
        ↓
Bind / Execute
```

NovaOS erhält damit eine dynamische API-Discovery-Schicht, über die Software benötigte Fähigkeiten und Schnittstellen unabhängig von konkreten Providern finden kann, während Kompatibilität, Authority, Trust und tatsächliche Provider-Auswahl weiterhin explizit getrennte Schritte bleiben.