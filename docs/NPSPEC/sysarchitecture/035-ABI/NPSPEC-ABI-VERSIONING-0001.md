# NPSPEC-ABI-VERSIONING-0001 – Nova ABI Versioning

## Status

Angenommen

## Kategorie

ABI / Versioning / Compatibility

## Zweck

NovaOS definiert ein einheitliches Versionierungsmodell für öffentliche und interne ABIs.

```text
ABI Definition
     ↓
Version
     ↓
Compatibility Check
     ↓
Bind / Adapt / Reject
```

ABI-Versionierung ermöglicht kontrollierte Weiterentwicklung, ohne bestehende Binärsoftware unnötig zu brechen.

## Grundprinzipien

```text
ABI Version ≠ OS Version
ABI Version ≠ API Version
Newer ≠ Compatible
Same Major ≠ Automatically Compatible
Version Match ≠ Semantic Compatibility
Compatibility ≠ Authority
```

## Version Model

```text
ABIVersion
├── ABI_ID
├── Major
├── Minor
├── Architecture
├── FeatureSet
└── CompatibilityLevel
```

Optional:

```text
Revision
ProfileID
MinimumVersion
MaximumVersion
DeprecatedFeatures
RequiredFeatures
```

## Versionsformat

Grundsätzlich:

```text
Major.Minor
```

Beispiel:

```text
Nova ABI 1.0
Nova ABI 1.1
Nova ABI 2.0
```

### Major

Eine Änderung der Major-Version erlaubt inkompatible Änderungen.

```text
1.x → 2.0
```

Programme müssen Kompatibilität explizit prüfen oder über einen Compatibility Layer ausgeführt werden.

### Minor

Eine Minor-Version kennzeichnet kompatible Erweiterungen.

```text
1.0 → 1.1
```

Bestehende garantierte Semantik darf dadurch nicht verändert werden.

## ABI-ID

Versionen gelten immer für eine konkrete ABI.

```text
ABI_ID + Version
```

Beispiele:

```text
NovaNativeABI      1.0
NovaSyscallABI     1.2
NovaDriverABI      3.1
NovaHALABI         2.0
```

Eine Versionsnummer ohne ABI-ID ist nicht ausreichend.

## Public ABI

Für stabile öffentliche ABIs gilt:

```text
Same Major
    ↓
Backward Compatibility Expected
```

Kompatible Minor-Versionen sollen ältere Consumer weiterhin unterstützen.

Inkompatible Änderungen erfordern grundsätzlich eine neue Major-Version.

## Internal ABI

Interne ABIs dürfen schneller evolvieren.

```text
Internal ABI
     ↓
Explicit Compatibility Policy
```

Mögliche Regeln:

```text
Exact Match
Same Major
Version Range
Feature Match
```

Internal ABI Versioning darf nicht automatisch die langfristigen Garantien der Stable ABI übernehmen.

## Feature Versioning

Nicht jede Erweiterung benötigt eine neue ABI-Version.

Optionale Fähigkeiten sollen über Features erkennbar sein:

```text
ABI 1.2
├── FEATURE_ASYNC_IO
├── FEATURE_ZEROCOPY
└── FEATURE_EXTENDED_HANDLES
```

Consumer sollen benötigte Features explizit prüfen.

## Compatibility Check

Vor Bindung:

```text
Consumer Requirements
        ↓
ABI_ID
Version
Architecture
Required Features
        ↓
Provider Capabilities
        ↓
Compatible?
├── Yes → Bind
└── No  → Adapt / Fallback / Reject
```

## Version Ranges

Komponenten können unterstützte Bereiche deklarieren:

```text
MinimumVersion = 1.0
MaximumVersion = 1.x
```

Unbegrenzte Annahmen über zukünftige Versionen sind zu vermeiden.

## Structure Versioning

ABI-Strukturen können unabhängig versioniert werden.

```text
struct NovaInfo {
    uint32_t size;
    uint32_t version;
    ...
};
```

Damit können Strukturen erweitert werden, ohne die gesamte ABI-Version erhöhen zu müssen.

## Syscall Versioning

Syscall IDs bleiben innerhalb ihres Stability Contracts stabil.

Neue Syscalls können additiv ergänzt werden.

```text
ABI 1.0
├── syscall 1
└── syscall 2

ABI 1.1
├── syscall 1
├── syscall 2
└── syscall 3
```

Eine bestehende Syscall ID darf nicht mit inkompatibler Semantik neu belegt werden.

## Semantic Versioning

Binäre Kompatibilität reicht nicht aus.

Folgende Änderungen können einen ABI-Bruch darstellen:

```text
Changed Meaning
Changed Ownership
Changed Lifetime
Changed Error Semantics
Changed Concurrency Contract
Changed Security Semantics
Changed Calling Convention
```

```text
Same Binary Layout ≠ Same ABI Contract
```

## Deprecation

ABI-Elemente können einen Lifecycle besitzen:

```text
Experimental
    ↓
Stable
    ↓
Deprecated
    ↓
Removed in Future Major Version
```

Deprecation muss maschinenlesbar erkennbar sein können.

## Compatibility Layer

NovaOS kann mehrere ABI-Generationen parallel unterstützen.

```text
Binary ABI 1.x
      ↓
Compatibility Layer
      ↓
Current ABI 2.x
      ↓
NovaOS
```

Compatibility Layer dürfen aktuelle Sicherheits- und Capability-Regeln nicht umgehen.

## Negotiation

Bei dynamischen Komponenten kann Version Negotiation verwendet werden.

```text
Consumer
Supported: 1.0–1.3
       ↓
Negotiation
       ↑
Provider
Supported: 1.2–2.0

Result: 1.2–1.3 compatible range
```

Danach wird ein konkretes kompatibles Profil ausgewählt.

## Architecture Profiles

Versionen gelten innerhalb eines Architekturprofils.

```text
Nova ABI 1.0
├── x86-32
├── x86-64
└── ARM64
```

Gleiche Versionsnummer bedeutet nicht automatisch identisches binäres Layout zwischen Architekturen.

## Security

Version Negotiation darf keinen unsicheren Downgrade erzwingen.

```text
Preferred ABI
     ↓
Negotiation
     ↓
Minimum Security Requirements
```

```text
Compatible ≠ Secure
```

Security-, Trust- und Capability-Policy haben Vorrang vor Kompatibilität.

## ABI Manifest

NovaOS soll Versionierungsinformationen maschinenlesbar bereitstellen:

```text
ABI Manifest
├── ABI_ID
├── Version
├── Architecture
├── Features
├── Compatibility Policy
├── Deprecated Elements
└── Required Security Level
```

Toolchains und Loader können dadurch Kompatibilität vor Ausführung prüfen.

## Build Integration

Build- und Release-Systeme sollen ABI-Änderungen erkennen.

Prüfbar sind beispielsweise:

```text
Structure Layout
Calling Convention
Syscall IDs
Constants
Exported Symbols
Feature IDs
Error Codes
Semantic Contract Metadata
```

Unbeabsichtigte ABI-Breaks sollen den Build oder Release blockieren können.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ABI_ID
Current Version
Supported Versions
Architecture
Features
Deprecated Elements
Compatibility Profile
Compatibility Layers
```

## Normative Anforderungen

1. NovaOS MUSS ABIs explizit versionieren.
2. Jede versionierte ABI MUSS eine eindeutige ABI-ID besitzen.
3. ABI-Version und OS-Version MÜSSEN getrennte Konzepte bleiben.
4. Major-Versionen MÜSSEN inkompatible Änderungen ausdrücken können.
5. Minor-Versionen SOLLEN kompatible Erweiterungen darstellen.
6. Bestehende stabile Semantik DARF innerhalb kompatibler Versionen NICHT stillschweigend verändert werden.
7. Public und Internal ABI DÜRFEN unterschiedliche Compatibility Policies besitzen.
8. Optionale Fähigkeiten SOLLEN über Feature Discovery erkennbar sein.
9. Consumer MÜSSEN benötigte ABI-Versionen und Features deklarieren können.
10. Provider MÜSSEN ihre unterstützten Versionen und Features deklarieren können.
11. Version Negotiation MUSS nur tatsächlich gemeinsame Versionen auswählen.
12. Strukturversionierung MUSS unabhängig von globaler ABI-Versionierung möglich sein.
13. Bestehende Syscall IDs DÜRFEN NICHT inkompatibel wiederverwendet werden.
14. Semantic Compatibility MUSS zusätzlich zur binären Kompatibilität berücksichtigt werden.
15. Deprecated ABI-Elemente SOLLEN maschinenlesbar erkennbar sein.
16. Compatibility Layer DÜRFEN Security- und Capability-Regeln NICHT umgehen.
17. Version Negotiation DARF keinen verbotenen Security Downgrade durchführen.
18. Architekturprofile MÜSSEN getrennt berücksichtigt werden.
19. NovaOS SOLL maschinenlesbare ABI Manifeste bereitstellen.
20. ABI-Änderungen SOLLEN automatisiert gegen vorherige Versionen geprüft werden.
21. Unbeabsichtigte ABI-Breaks SOLLEN vor Release erkannt werden.
22. ABI-Versionierungszustände MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ABI-SYSCALL-0001`
- `NPSPEC-ABI-STABLE-0001`
- `NPSPEC-ABI-INTERNAL-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-DISTCOMM-NEGOTIATION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `ADR-ARCH-0145`

## Ergebnis

```text
Consumer
   ↓
ABI_ID + Version + Features
   ↓
Compatibility Validation
   ↓
Security Validation
   ↓
Compatible?
├── Yes → Bind
├── Adapt → Compatibility Layer
└── No → Reject / Fallback
```

NovaOS erhält damit ein einheitliches ABI-Versionierungsmodell, das öffentliche und interne Binärschnittstellen kontrolliert weiterentwickeln lässt, Kompatibilität explizit prüfbar macht und unbeabsichtigte ABI-Brüche verhindert.