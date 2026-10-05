# NPSPEC-CAPABILITY-DEPENDENCY-0001 – Nova Capability Dependency

## Status

Angenommen

## Kategorie

Capability / Dependency

## Zweck

NovaOS definiert Abhängigkeiten von Capability-Paketen und Capability-Implementierungen.

Abhängigkeiten beschreiben technische Voraussetzungen für Bereitstellung und Ausführung, ohne Capability-Identität, Authority oder Provider fest miteinander zu koppeln.

## Grundprinzipien

```text
Dependency ≠ Authority
Dependency ≠ Capability Permission
Dependency ≠ Provider Binding
CapabilityID ≠ PackageID
Dependency Resolution ≠ Capability Resolution
Installed Dependency ≠ Granted Permission
```

## Modell

Eine Abhängigkeit kann beschrieben werden durch:

```text
CapabilityDependency
├── DependencyType
├── TargetID
├── VersionConstraint
├── Required
└── Compatibility
```

Optional:

```text
Architecture
Features
TrustRequirements
ExecutionConstraints
Fallback
```

## Abhängigkeitstypen

Capability-Implementierungen dürfen abhängig sein von:

```text
System Interface
Framework
Runtime
Library
Capability Package
Capability
Device
Hardware Feature
```

Dabei muss zwischen statischen Paketabhängigkeiten und dynamisch auflösbaren Capability-Abhängigkeiten unterschieden werden.

## Capability-Abhängigkeiten

Eine Implementierung darf eine andere Capability voraussetzen:

```text
Capability A
     ↓ requires
Capability B
```

Die Abhängigkeit wird über die `CapabilityID` beschrieben und darf nicht unnötig an einen bestimmten Provider gebunden werden.

```text
Required:
de.nova.image.decode

Nicht erforderlich:
Provider X / Implementation Y
```

Eine Providerbindung ist nur zulässig, wenn sie technisch oder sicherheitsbedingt notwendig ist.

## Auflösung

```text
Implementation
      ↓
Declared Dependencies
      ↓
Version + Compatibility
      ↓
Available Providers / Components
      ↓
Trust + Policy
      ↓
Dependency Graph
      ↓
Resolved Execution Environment
```

Die Auflösung muss deterministisch nachvollziehbar sein.

## Versionsauflösung

Abhängigkeiten dürfen Versionsbereiche definieren:

```text
MinimumVersion
MaximumVersion
CompatibleVersion
ExactVersion
```

Capability-, Interface-, Package- und Implementation-Versionen müssen getrennt behandelt werden.

## Dependency Graph

NovaOS muss transitive Abhängigkeiten als Graph behandeln können:

```text
Capability A
├── Runtime X
├── Library Y
└── Capability B
    └── Library Z
```

Zyklen, fehlende Abhängigkeiten und inkompatible Versionsanforderungen müssen erkannt werden.

## Private Abhängigkeiten

Capability-Pakete dürfen private Abhängigkeiten mitbringen.

Diese bleiben dem Paket beziehungsweise der Implementierung zugeordnet und dürfen globale Systemkomponenten nicht unkontrolliert ersetzen.

## Authority

Eine technische Abhängigkeit erzeugt keine Authority.

```text
Capability A requires Capability B
              ≠
Capability A automatically owns B
```

Benötigt die Ausführung Authority für eine abhängige Capability, muss diese aus vorhandener Authority, Policy und Execution Contract abgeleitet werden.

## Fehlerbehandlung

Nicht auflösbare Abhängigkeiten müssen kontrolliert zu einem Zustand wie:

```text
Unavailable
Incompatible
Restricted
DependencyFailure
```

führen.

Alternative Implementierungen dürfen geprüft werden.

## Normative Anforderungen

1. Capability-Abhängigkeiten MÜSSEN deklarativ beschreibbar sein.
2. DependencyType, TargetID und Versionsanforderungen MÜSSEN getrennt angegeben werden können.
3. Capability-Abhängigkeiten SOLLEN über stabile `CapabilityID`s referenziert werden.
4. Abhängigkeiten SOLLEN nicht unnötig an konkrete Provider gebunden werden.
5. Capability-, Interface-, Package- und Implementation-Versionen MÜSSEN getrennt behandelt werden.
6. Transitive Abhängigkeiten MÜSSEN auflösbar sein.
7. Zyklen und Versionskonflikte MÜSSEN erkannt werden.
8. Private Abhängigkeiten DÜRFEN globale Systemabhängigkeiten nicht unkontrolliert ersetzen.
9. Dependency Resolution DARF keine Authority erzeugen.
10. Abhängige Capability-Nutzung MUSS weiterhin Capability- und Policy-Prüfungen unterliegen.
11. Nicht erfüllbare Abhängigkeiten MÜSSEN kontrolliert behandelt werden.
12. Alternative kompatible Implementierungen SOLLEN berücksichtigt werden können.
13. Aufgelöste Abhängigkeiten und Konflikte MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-PACKAGE-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`

## Ergebnis

NovaOS besitzt ein providerunabhängiges und versioniertes Abhängigkeitsmodell für Capabilities. Technische Voraussetzungen können deterministisch aufgelöst werden, ohne Abhängigkeiten mit Authority, Berechtigungen oder festen Providerbindungen zu vermischen.