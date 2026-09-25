# NPSPEC-CAPABILITY-VERSIONING-0001 – Nova Capability Versioning

## Status

Angenommen

## Kategorie

Capability / Versioning / Compatibility / Security

## Zweck

NovaOS definiert ein einheitliches Versionierungsmodell für Capability-Typen, Interfaces und Semantik.

```text
CapabilityTypeID
      ↓
Interface Version
      ↓
Negotiation
      ↓
Compatible Capability
```

Versionierung ermöglicht die Weiterentwicklung von Capabilities, ohne Sicherheitssemantik still zu verändern oder bestehende Komponenten unnötig zu brechen.

## Grundprinzipien

```text
Capability Identity ≠ Capability Version
Newer ≠ More Trusted
Compatible ≠ Identical
Version Upgrade ≠ Authority Expansion
Version Downgrade ≠ Automatically Safe
Interface Compatibility ≠ Security Compatibility
Unknown Version ≠ Compatible Version
```

## Versionsmodell

Ein versionierter Capability-Typ besitzt mindestens:

```text
CapabilityInterface
├── CapabilityTypeID
├── Major Version
├── Minor Version
└── Interface Schema
```

Optional:

```text
Feature Set
Security Semantics
Deprecated Features
Required Constraints
Provider Extensions
Minimum Compatible Version
```

## Major Version

Eine neue Major Version wird verwendet, wenn inkompatible Änderungen auftreten.

Beispiele:

```text
Changed Operation Semantics
Removed Operations
Changed Security Model
Changed Rights Semantics
Changed Constraint Semantics
```

Beispiel:

```text
Storage.Read v1
        ↓ incompatible change
Storage.Read v2
```

Eine Major-Version darf nicht automatisch als kompatibel angenommen werden.

## Minor Version

Minor-Versionen dürfen rückwärtskompatible Erweiterungen enthalten.

```text
v2.0
 ↓
v2.1
 ↓
v2.2
```

Beispiele:

```text
Optional Operation
Optional Metadata
Additional Constraint
New Introspection Field
```

Eine Minor-Version darf bestehende Sicherheitsgarantien nicht abschwächen.

## Capability Identity

Die semantische Capability bleibt über kompatible Versionen hinweg identifizierbar.

```text
CapabilityTypeID: Document.Render

Versions:
├── 1.0
├── 1.1
└── 2.0
```

Konkrete Capability-Instanzen behalten weiterhin ihre eigene `CapabilityID`.

```text
CapabilityTypeID ≠ CapabilityID
```

## Negotiation

Requester und Provider handeln eine kompatible Version aus.

```text
Requester:
v2.0 – v2.3

Provider:
v1.5
v2.1
v3.0

Result:
v2.1
```

Hard Requirements dürfen dabei nicht verletzt werden.

## Sicherheitssemantik

Versionierung darf nicht verwendet werden, um Sicherheitsanforderungen zu umgehen.

```text
Current:
Strong Isolation

Legacy:
Weak Isolation
```

Ein automatischer Fallback auf die schwächere Version ist unzulässig, wenn die aktuelle Security Policy dies nicht ausdrücklich erlaubt.

## Downgrade-Schutz

Downgrades müssen kontrolliert werden.

```text
Preferred Version
       ↓ unavailable
Policy Check
       ↓
Allowed Older Version
       ↓
Explicit Negotiation
```

Es gilt:

```text
Negotiation Failure ≠ Silent Downgrade
```

## Registry

Die Capability Registry kann mehrere Versionen parallel verwalten.

```text
Capability Registry
└── Document.Render
    ├── Provider A → v1.2
    ├── Provider B → v2.0
    └── Provider C → v2.3
```

Discovery kann dadurch passende Provider bestimmen.

## Deprecation

Versionen können Lebenszykluszustände besitzen:

```text
Preferred
Supported
Legacy
Deprecated
Forbidden
```

`Forbidden` Versionen dürfen nicht neu ausgehandelt werden.

Bestehende Instanzen müssen gemäß Security Policy beendet, ersetzt oder migriert werden können.

## Live Evolution

Capability-Versionen sollen mit Live Replacement integrierbar sein.

```text
Provider v2
    ↓
Load Provider v3
    ↓
Validate
    ↓
Negotiate / Migrate
    ↓
Switch
    ↓
Retire v2
```

Bestehende Capability-Instanzen dürfen nicht still eine veränderte Autoritätssemantik erhalten.

## Serialisierung und IPC

Versionierte Capabilities müssen auch über IPC und Remote-Kommunikation eindeutig interpretierbar sein.

```text
CapabilityTypeID
Interface Version
Schema Version
```

Capability-Version und Nachrichten-/Serialisierungsversion bleiben getrennte Konzepte.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
CapabilityTypeID
CapabilityID
Interface Version
Supported Versions
Negotiated Version
Lifecycle State
Compatibility Information
```

## Normative Anforderungen

1. NovaOS MUSS Capability-Interfaces explizit versionieren können.
2. Capability Identity und Interface Version MÜSSEN getrennt bleiben.
3. Inkompatible semantische Änderungen MÜSSEN eine neue Major-Version erzeugen.
4. Minor-Versionen DÜRFEN bestehende Sicherheitsgarantien NICHT abschwächen.
5. Unbekannte Versionen DÜRFEN NICHT automatisch als kompatibel gelten.
6. Version Negotiation MUSS Hard Requirements berücksichtigen.
7. Sicherheitsrelevante Downgrades DÜRFEN NICHT still erfolgen.
8. Mehrere Capability-Versionen MÜSSEN parallel registrierbar sein.
9. Deprecated und Forbidden Versionen MÜSSEN durch Policy kontrollierbar sein.
10. Live Evolution DARF bestehende Autoritätssemantik NICHT still verändern.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-NEGOTIATION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `ADR-CAPABILITY-0016`

## Ergebnis

```text
Stable Capability Type
        ↓
Versioned Interfaces
        ↓
Compatibility + Security Policy
        ↓
Explicit Negotiation
        ↓
Compatible Capability Instance
```

NovaOS erhält damit ein evolvierbares Capability-Modell, bei dem Interfaces weiterentwickelt und parallel betrieben werden können, ohne Kompatibilität, Versionierung oder Downgrades zur unkontrollierten Veränderung von Autorität und Sicherheitssemantik werden zu lassen.