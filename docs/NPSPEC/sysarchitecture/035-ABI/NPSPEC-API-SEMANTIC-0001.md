# NPSPEC-API-SEMANTIC-0001 – Nova Semantic API

## Status

Angenommen

## Kategorie

API / Semantics / System Architecture

## Zweck

NovaOS definiert APIs primär über ihre Bedeutung und Fähigkeiten statt über konkrete Implementierungen, Prozesse, Libraries oder Provider.

```text
Consumer Intent
      ↓
Semantic API
      ↓
Capability + Contract Resolution
      ↓
Provider Selection
      ↓
Execution
```

Eine Anwendung fordert damit beispielsweise eine Fähigkeit wie `Image.Decode`, `Document.Render` oder `Math.Solve` an, ohne einen bestimmten Provider kennen zu müssen.

## Grundprinzipien

```text
Semantic API ≠ Provider API
Semantic API ≠ ABI
Semantic API ≠ Implementation
Semantic API ≠ File Extension
Semantic API ≠ Process Endpoint
Semantic Compatibility ≠ Binary Compatibility
API Discovery ≠ Authority
```

## Semantic API Model

```text
SemanticAPI
├── SemanticAPIID
├── Version
├── Operations
├── InputTypes
├── OutputTypes
├── Contract
└── RequiredCapabilities
```

Optional:

```text
QualityRequirements
ResourceRequirements
TrustRequirements
SovereigntyRequirements
Determinism
Latency
Deadline
ProviderConstraints
```

## Semantic Identity

Eine API erhält eine stabile semantische Identität.

```text
SemanticAPIID
      ↓
Meaning
```

Beispiele:

```text
Nova.Image.Decode
Nova.Document.Render
Nova.Math.Solve
Nova.Storage.Store
Nova.Audio.Encode
```

Die Identität bleibt unabhängig von:

```text
Provider
Process
Library
Binary
Machine
Location
Implementation Language
```

## Semantic Operations

Operationen werden über ihre Bedeutung beschrieben.

```text
Operation
├── OperationID
├── Input Semantics
├── Output Semantics
├── Preconditions
├── Postconditions
└── Side Effects
```

Beispiel:

```text
Nova.Image.Decode

Input:
    EncodedImage

Output:
    PixelImage
```

Der Consumer muss nicht wissen, welche Decoder-Library verwendet wird.

## Semantic Types

Ein- und Ausgaben verwenden nach Möglichkeit Nova Semantic Types.

```text
Semantic API
├── Input → SemanticTypeID
└── Output → SemanticTypeID
```

Darstellung und Bedeutung bleiben getrennt.

```text
JPEG Representation
PNG Representation
NovaFile Representation
        ↓
Semantic Type: Image
```

## Provider Resolution

Mehrere Provider können dieselbe Semantic API implementieren.

```text
Semantic API
    ↓
Provider Discovery
├── Provider A
├── Provider B
└── Provider C
```

NovaOS kann anhand des Execution Contracts einen geeigneten Provider auswählen.

## Provider Selection

Auswahlkriterien können sein:

```text
Compatibility
Capabilities
Security
Trust
Sovereignty
Latency
Deadline
Resources
Energy
Determinism
Hardware Availability
User Policy
```

```text
Fastest Provider ≠ Automatically Best Provider
```

Harte Constraints haben Vorrang vor Optimierung.

## API Contract

Jede Semantic API besitzt einen expliziten Contract.

```text
Semantic Meaning
      +
API Contract
      ↓
Expected Behavior
```

Provider müssen denselben ausgehandelten semantischen Vertrag erfüllen.

## Semantic Compatibility

Zwei Provider gelten nicht allein deshalb als kompatibel, weil ihre Funktionssignaturen identisch sind.

Kompatibilität berücksichtigt:

```text
Input Meaning
Output Meaning
Side Effects
Error Semantics
Ownership
Ordering
Transaction Semantics
Security Semantics
```

```text
Same Signature ≠ Same Semantic API
```

## Conversion

Wenn ein Provider einen anderen kompatiblen Semantic Type benötigt:

```text
Input Type A
    ↓
Semantic Conversion
    ↓
Provider Type B
```

Conversion muss explizit erfolgen.

Verlustbehaftete Konvertierung darf nicht stillschweigend erfolgen, wenn dadurch Anforderungen verletzt werden.

## Capability Integration

Eine Semantic API beschreibt eine Fähigkeit, verleiht sie aber nicht.

```text
Semantic API Discovered
        ≠
Capability Granted
```

Vor Ausführung müssen erforderliche Capabilities validiert werden.

## Execution Contract

Semantic APIs integrieren sich direkt mit dem Nova Execution Contract.

```text
Semantic Operation
       +
Execution Contract
       ↓
Provider Resolution
```

Dadurch können Anforderungen wie:

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

berücksichtigt werden.

## Location Transparency

Semantic APIs können lokal oder remote bereitgestellt werden.

```text
Consumer
   ↓
Semantic API
├── Local Provider
├── Remote Provider
└── Distributed Provider
```

```text
Location Transparency ≠ Authority Transparency
```

Ein Remote Provider muss dieselben relevanten Security-, Trust- und Contract-Prüfungen erfüllen.

## Transactions

Semantic Operations können deklarieren:

```text
Transactional
Rollback-capable
Compensatable
Irreversible
Idempotent
```

Diese Semantik muss bei Providerwechsel erhalten bleiben.

## Versioning

Semantic APIs werden unabhängig von ihren Providern versioniert.

```text
Nova.Image.Decode 1.0
Nova.Image.Decode 1.1
```

Provider deklarieren die unterstützten Contract-Versionen und Features.

## Fallback

Ist ein Provider nicht verfügbar:

```text
Provider A unavailable
        ↓
Resolve Alternatives
        ↓
Provider B
```

Fallback ist nur zulässig, wenn der alternative Provider weiterhin alle harten Anforderungen erfüllt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SemanticAPIID
Version
Operations
Semantic Types
Contract
Available Providers
Selected Provider
Required Capabilities
Compatibility
Execution Requirements
```

## Normative Anforderungen

1. NovaOS MUSS semantisch identifizierbare APIs unterstützen können.
2. Semantic API Identity MUSS von Provider und Implementierung getrennt bleiben.
3. Semantic APIs SOLLEN Ein- und Ausgaben über Semantic Types beschreiben.
4. Operationen MÜSSEN ihre semantische Bedeutung eindeutig definieren können.
5. Semantic Compatibility DARF NICHT allein aus binärer Signatur abgeleitet werden.
6. Semantic APIs MÜSSEN mit API Contracts integrierbar sein.
7. Mehrere Provider MÜSSEN dieselbe Semantic API implementieren können.
8. Provider Discovery DARF NICHT als Authority interpretiert werden.
9. Provider Selection MUSS harte Security-, Trust- und Sovereignty-Constraints respektieren.
10. Provider Selection SOLL Execution Contracts berücksichtigen.
11. Semantic Conversion MUSS explizit und validierbar sein.
12. Verlustbehaftete Conversion DARF harte Anforderungen NICHT stillschweigend verletzen.
13. Semantic API Discovery DARF keine Capability erzeugen.
14. Erforderliche Capabilities MÜSSEN vor Ausführung validiert werden.
15. Semantic APIs MÜSSEN lokale und remote Provider unterstützen können.
16. Location Transparency DARF Authority-Prüfungen NICHT umgehen.
17. Transaction-, Idempotency- und Compensation-Semantik MUSS providerunabhängig beschreibbar sein.
18. Semantic APIs MÜSSEN unabhängig von Providern versionierbar sein.
19. Providerwechsel DARF den ausgehandelten Contract NICHT verletzen.
20. Fallback DARF harte Execution Constraints NICHT abschwächen.
21. Semantic APIs SOLLEN sprach- und architekturunabhängig beschreibbar sein.
22. Semantic API, Provider und Contract State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-API-VERSIONING-0001`
- `NPSPEC-API-CONTRACT-0001`
- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `ADR-ARCH-0148`

## Ergebnis

```text
Consumer Intent
      ↓
Semantic API
      ↓
Semantic Types + API Contract
      ↓
Capability Validation
      ↓
Execution Contract
      ↓
Provider Discovery
      ↓
Constraint Validation
      ↓
Provider Selection
      ↓
Execution
      ↓
Semantic Result
```

NovaOS erhält damit eine providerunabhängige semantische API-Schicht, über die Software Fähigkeiten nach ihrer Bedeutung statt nach konkreten Programmen oder Implementierungen verwendet und NovaOS den geeigneten Provider sicher, transparent und dynamisch auswählen kann.