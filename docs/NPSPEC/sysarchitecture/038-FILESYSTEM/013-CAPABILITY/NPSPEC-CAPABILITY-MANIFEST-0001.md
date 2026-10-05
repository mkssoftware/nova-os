# NPSPEC-CAPABILITY-MANIFEST-0001 – Nova Capability Manifest

## Status

Angenommen

## Kategorie

Capability / Manifest

## Zweck

NovaOS definiert das Capability Manifest als deklarative Beschreibung eines Capability Packages und der darin bereitgestellten Capability-Implementierungen.

Das Manifest beschreibt Identität, Versionen, Provider, Schnittstellen, Abhängigkeiten und Laufzeitanforderungen, erzeugt jedoch selbst keine Authority.

## Grundprinzipien

```text
Manifest ≠ Capability
Manifest ≠ Authority
Declaration ≠ Permission
CapabilityID ≠ ProviderID
PackageVersion ≠ CapabilityVersion
Declared Requirement ≠ Granted Resource
```

## Manifest-Modell

Ein Manifest enthält mindestens:

```text
CapabilityManifest
├── PackageID
├── PackageVersion
├── ProviderID
├── Capabilities[]
├── Dependencies[]
├── Compatibility
└── ManifestVersion
```

Eine Capability-Beschreibung enthält:

```text
Capability
├── CapabilityID
├── Version
├── Interface
├── SemanticInput
├── SemanticOutput
├── Implementation
└── Requirements
```

## Identitäten

Die Identitäten bleiben getrennt:

```text
PackageID
ProviderID
CapabilityID
```

Ein Package darf mehrere Capabilities enthalten und ein Provider darf mehrere Capability-Implementierungen bereitstellen.

## Capability-ID

Capabilities werden ausschließlich über ihre vollständige stabile ID referenziert:

```text
de.nova.image.filter.gaussian
```

Kategorie, Package-Pfad oder Manifest-Struktur dürfen die `CapabilityID` nicht bestimmen.

## Anforderungen

Eine Implementierung darf Anforderungen deklarieren:

```text
Runtime
Libraries
Framework Interfaces
Hardware Features
Device Capabilities
System Interfaces
Trust Level
Resource Requirements
```

Diese Angaben beschreiben Voraussetzungen und erzeugen keine entsprechenden Berechtigungen.

## Semantische Schnittstellen

Capabilities sollen ihre Ein- und Ausgaben über semantische Typen beschreiben können:

```text
SemanticInput
      ↓
Capability
      ↓
SemanticOutput
```

Dadurch können Capability Registry, Solutions und Execution Contracts kompatible Fähigkeiten automatisiert auflösen.

## Implementierung

Das Manifest referenziert die konkrete Implementierung innerhalb des Packages.

```text
CapabilityID
      ↓
Implementation
      ↓
Provider
```

Die Implementierung ist austauschbar, ohne die semantische Capability-Identität zu verändern.

## Validierung

Vor Registrierung muss das Manifest geprüft werden:

```text
Parse
 ↓
Schema Validation
 ↓
Identity Validation
 ↓
CapabilityID Validation
 ↓
Dependency Validation
 ↓
Compatibility Validation
 ↓
Trust / Integrity Validation
 ↓
Register
```

Ungültige oder widersprüchliche Manifeste dürfen nicht veröffentlicht werden.

## Versionierung

Getrennt versioniert werden:

```text
ManifestVersion
PackageVersion
CapabilityVersion
InterfaceVersion
```

Eine Änderung einer Ebene darf nicht automatisch eine Änderung aller anderen Versionen erzwingen.

## Sicherheit

Das Manifest darf keine aktiven:

```text
Capability Tokens
Credentials
Secrets
Authorized Handles
```

enthalten.

Deklarierte System- oder Geräteanforderungen müssen zur Laufzeit separat autorisiert werden.

## Normative Anforderungen

1. Jedes Capability Package MUSS ein validierbares Manifest besitzen.
2. Das Manifest MUSS `PackageID`, `PackageVersion` und `ProviderID` deklarieren.
3. Bereitgestellte Capabilities MÜSSEN über vollständige `CapabilityID`s referenziert werden.
4. Capability-, Provider- und Package-Identitäten MÜSSEN getrennt bleiben.
5. Ein Manifest MUSS mehrere Capabilities beschreiben können.
6. Capability-Versionen MÜSSEN unabhängig von Package-Versionen sein.
7. Abhängigkeiten und Laufzeitanforderungen MÜSSEN deklarierbar sein.
8. Semantische Ein- und Ausgabetypen MÜSSEN beschreibbar sein können.
9. Manifest-Anforderungen DÜRFEN keine Authority erzeugen.
10. Das Manifest DARF keine aktiven Capability-Tokens oder Secrets enthalten.
11. Das Manifest MUSS vor Registry-Veröffentlichung validiert werden.
12. Sicherheitsrelevante Manifeständerungen MÜSSEN Trust- und Policy-Neubewertungen auslösen können.
13. Manifest, Capabilities, Versionen und Anforderungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-NAMING-0001`
- `NPSPEC-CAPABILITY-PACKAGE-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS besitzt ein deklaratives und versioniertes Manifestformat für Capability Packages. Capabilities, Provider, Implementierungen, Abhängigkeiten und Anforderungen können damit eindeutig beschrieben und validiert werden, ohne Deklarationen mit tatsächlicher Authority zu vermischen.