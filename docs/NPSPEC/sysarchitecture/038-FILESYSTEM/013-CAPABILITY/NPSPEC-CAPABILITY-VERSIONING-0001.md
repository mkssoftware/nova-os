# NPSPEC-CAPABILITY-VERSIONING-0001 – Nova Capability Versioning

## Status

Angenommen

## Kategorie

Capability / Versioning

## Zweck

NovaOS definiert die unabhängige Versionierung von Capabilities, Interfaces, Packages und Implementierungen.

Versionierung ermöglicht Weiterentwicklung und parallele Nutzung verschiedener Versionen, ohne die stabile Identität einer Capability zu verändern.

## Grundprinzipien

```text
CapabilityID ≠ Version
CapabilityVersion ≠ InterfaceVersion
CapabilityVersion ≠ PackageVersion
CapabilityVersion ≠ ImplementationVersion
New Version ≠ New CapabilityID
Compatible Update ≠ Permission Reset
```

## Versionsmodell

NovaOS unterscheidet mindestens:

```text
CapabilityID
├── CapabilityVersion
├── InterfaceVersion
├── PackageVersion
└── ImplementationVersion
```

Zusätzlich können abhängige Komponenten eigene Versionen besitzen:

```text
RuntimeVersion
FrameworkVersion
LibraryVersion
SemanticTypeVersion
```

Diese Versionen dürfen nicht miteinander gleichgesetzt werden.

## Capability-Version

Die `CapabilityVersion` beschreibt die Version des öffentlichen semantischen Capability-Vertrags.

```text
CapabilityID:
de.nova.image.filter.gaussian

CapabilityVersion:
1
2
3
```

Die `CapabilityID` bleibt erhalten, solange die grundlegende semantische Bedeutung der Capability erhalten bleibt.

Eine grundlegend andere Bedeutung erfordert eine neue `CapabilityID`.

## Interface-Version

Die `InterfaceVersion` beschreibt den technischen Aufrufvertrag:

```text
Operations
Inputs
Outputs
Parameters
Errors
Execution Semantics
```

Inkompatible Änderungen am Interface müssen eindeutig versioniert werden.

## Package-Version

Die `PackageVersion` beschreibt die Version des installierbaren Capability-Pakets.

Ein neues Package kann dieselbe Capability- und Interface-Version mit einer verbesserten Implementierung bereitstellen.

## Implementation-Version

Die `ImplementationVersion` beschreibt eine konkrete Implementierung.

Beispiel:

```text
CapabilityVersion:     2
InterfaceVersion:      3
PackageVersion:        8
ImplementationVersion: 12
```

## Kompatibilität

Versionsanforderungen müssen explizit ausdrückbar sein:

```text
Exact
Minimum
Maximum
Range
Compatible
```

Die Auflösung erfolgt gegen die tatsächlich verfügbaren Versionen und deren deklarierte Kompatibilität.

## Parallele Versionen

NovaOS muss mehrere Versionen parallel unterstützen können:

```text
CapabilityID
├── v1
├── v2
└── v3
```

Dadurch dürfen ältere Programme und Solutions weiterhin kompatible Versionen verwenden, während neue Komponenten neuere Versionen nutzen.

## Auflösung

```text
Capability Request
      ↓
Version Constraint
      ↓
Compatible Capability Versions
      ↓
Compatible Interfaces
      ↓
Compatible Implementations
      ↓
Trust + Policy
      ↓
Execution Contract
      ↓
Selected Version
```

Die Auflösung muss deterministisch und nachvollziehbar sein.

## Berechtigungen

Eine Versionsänderung erzeugt keine zusätzliche Authority.

Sicherheitsrelevante Änderungen, insbesondere neue Operationen oder erweiterte Capability-Anforderungen, müssen jedoch eine erneute Permission- und Policy-Bewertung auslösen können.

## Deprecation

Versionen dürfen Zustände besitzen wie:

```text
Active
Deprecated
Legacy
Unsupported
Revoked
```

Deprecation darf bestehende kompatible Nutzung nicht ohne definierte Policy unmittelbar brechen.

## Normative Anforderungen

1. `CapabilityID` und Version MÜSSEN getrennt bleiben.
2. Capability-, Interface-, Package- und Implementation-Version MÜSSEN unabhängig versionierbar sein.
3. Kompatible Weiterentwicklungen DÜRFEN dieselbe `CapabilityID` behalten.
4. Eine grundlegend andere semantische Bedeutung MUSS eine neue `CapabilityID` erhalten.
5. Versionsanforderungen MÜSSEN deklarativ ausdrückbar sein.
6. Mehrere Versionen MÜSSEN parallel unterstützt werden können.
7. Versionsauflösung MUSS deterministisch erfolgen.
8. Inkompatible Interface-Änderungen MÜSSEN eindeutig versioniert werden.
9. Versionsänderungen DÜRFEN keine zusätzliche Authority erzeugen.
10. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Permission- und Policy-Bewertung auslösen können.
11. Deprecated, Legacy, Unsupported und Revoked MÜSSEN unterscheidbar sein.
12. Registry und Resolver MÜSSEN verfügbare Versionen und deren Kompatibilität erkennen können.
13. Gewählte Version und Auflösungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`

## Ergebnis

NovaOS besitzt ein unabhängiges Versionsmodell für Capability, Interface, Package und Implementierung. Dadurch können Capabilities langfristig weiterentwickelt, mehrere Versionen parallel betrieben und kompatible Implementierungen deterministisch ausgewählt werden, ohne stabile Capability-Identitäten oder Sicherheitsgrenzen unnötig zu verändern.