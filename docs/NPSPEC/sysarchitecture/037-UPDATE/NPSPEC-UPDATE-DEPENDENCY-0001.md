# NPSPEC-UPDATE-DEPENDENCY-0001 – Nova Update Dependency Management

## Status

Angenommen

## Kategorie

Update / Dependency Management / System Lifecycle

## Zweck

NovaOS definiert ein einheitliches Modell zur Auflösung, Validierung und Kontrolle von Abhängigkeiten zwischen Update-Paketen und Systemkomponenten.

```text
Update Request
      ↓
Dependency Graph
      ↓
Resolve + Validate
      ↓
Consistent Update Set
      ↓
Transactional Update
```

Ziel ist, dass ein Update niemals einen Systemzustand aktiviert, dessen erforderliche Abhängigkeiten fehlen, inkompatibel oder nicht vertrauenswürdig sind.

## Grundprinzipien

```text
Dependency ≠ Installation Order
Installed ≠ Compatible
Available ≠ Usable
Newer ≠ Compatible
ABI Compatible ≠ Contract Compatible
Dependency Satisfied ≠ Dependency Trusted
Optional ≠ Irrelevant
Conflict ≠ Failure
Resolved ≠ Installed
```

## Dependency Model

```text
Dependency
├── DependencyID
├── Source
├── Target
├── DependencyType
├── VersionConstraint
└── Requirement
```

Optional:

```text
ABIRequirement
APIRequirement
ContractRequirement
CapabilityRequirement
TrustRequirement
ArchitectureRequirement
StateRequirement
AlternativeProviders
Condition
```

## Dependency Types

NovaOS unterstützt mindestens:

```text
Requires
Recommends
Optional
Conflicts
Replaces
Provides
```

`Requires` beschreibt eine harte Abhängigkeit.

`Recommends` und `Optional` dürfen die Installation nicht automatisch blockieren.

## Dependency Graph

Abhängigkeiten werden als gerichteter Graph modelliert.

```text
Package A
├── requires B
│   └── requires D
└── requires C
```

Der Resolver muss den vollständigen relevanten Graphen bestimmen, bevor Änderungen angewendet werden.

## Version Constraints

Abhängigkeiten dürfen Versionsbereiche definieren.

Beispiele:

```text
B >= 3
B >= 3 && B < 5
B == 4
ABI == 2
API >= 6
```

Versionsnummern allein dürfen nicht die einzige Form der Kompatibilitätsprüfung sein.

## Semantic Dependencies

NovaOS soll Abhängigkeiten auch anhand bereitgestellter Fähigkeiten auflösen können.

```text
Package A
     ↓ requires
Capability: Image.Decode.JPEG
     ↓ provided by
Provider B
```

Damit muss eine Komponente nicht zwingend von einer konkreten Implementierung abhängig sein.

```text
Required Capability ≠ Required Provider
```

## Provider Selection

Existieren mehrere geeignete Provider:

```text
Required Capability
├── Provider A
├── Provider B
└── Provider C
```

erfolgt die Auswahl anhand von:

```text
Compatibility
Security
Trust
Sovereignty
Resource Requirements
System Policy
Explicit User Policy
```

## Transitive Dependencies

Der Resolver muss transitive Abhängigkeiten berücksichtigen.

```text
A → B → C → D
```

Eine gültige direkte Abhängigkeit reicht nicht aus, wenn eine darunterliegende harte Abhängigkeit nicht erfüllt werden kann.

## Conflicts

Konflikte müssen vor Anwendung erkannt werden.

```text
Package A
   ↓ conflicts
Package B
```

Der Resolver darf einen Konflikt nicht stillschweigend ignorieren.

Mögliche Lösungen:

```text
Alternative Provider
Compatible Version
Package Replacement
Update Set Modification
Reject Update
```

## Circular Dependencies

Zyklische Abhängigkeiten müssen erkannt werden.

```text
A → B
↑   ↓
└── C
```

Ein Zyklus darf nur akzeptiert werden, wenn für den betroffenen Update-Typ ein explizit unterstütztes gemeinsames Aktivierungsmodell existiert.

Andernfalls wird das Update blockiert.

## Update Set

Das Ergebnis der Auflösung ist ein konsistentes Update Set.

```text
UpdateSet
├── Packages
├── DependencyGraph
├── InstallationOrder
├── ActivationOrder
├── Conflicts
└── VerificationPlan
```

## Installation und Aktivierung

Installationsreihenfolge und Aktivierungsreihenfolge dürfen unterschiedlich sein.

```text
Stage B
Stage A
   ↓
Activate A + B
```

Abhängige Komponenten dürfen erst aktiviert werden, wenn ihre erforderlichen Dependencies verfügbar und gültig sind.

## Compatibility

Für kritische Abhängigkeiten müssen neben Versionen auch geprüft werden können:

```text
Architecture
ABI
API
Contract
Semantic Type
State Schema
Capability Interface
Hardware
Boot Environment
```

```text
Version Match ≠ Compatibility Proof
```

## Trust

Jede harte Dependency muss die notwendigen Trust-Anforderungen erfüllen.

```text
Dependency Found
      ↓
Integrity
      ↓
Signature
      ↓
Trust Policy
      ↓
Usable Dependency
```

Ein technisch kompatibler, aber nicht vertrauenswürdiger Provider darf eine sicherheitskritische Dependency nicht erfüllen.

## State Compatibility

Updates dürfen Abhängigkeiten zwischen State-Schemas besitzen.

```text
Component A v2
      ↓ requires
State Schema B >= 4
```

Notwendige Migrationen müssen Bestandteil des Update Plans sein.

## Dependency Changes

Ein Update kann Abhängigkeiten:

```text
Add
Remove
Replace
Upgrade
Downgrade
```

Diese Änderungen müssen vor der Aktivierung vollständig neu aufgelöst werden.

## Transaction Integration

Der gesamte aufgelöste Update Set wird als gemeinsame Update-Transaktion behandelt.

```text
Resolve
  ↓
Validate Complete Set
  ↓
Stage
  ↓
Prepare
  ↓
Activate
  ↓
Verify
  ↓
Commit
```

Schlägt eine harte Dependency fehl, darf kein inkonsistenter Teilzustand committed werden.

## Rollback

Rollback muss Dependency-Konsistenz berücksichtigen.

```text
Component A → old version
```

darf nicht erfolgen, wenn dadurch andere aktive Komponenten inkompatibel werden.

Der Resolver muss daher auch den Rollback-Zielzustand prüfen.

## Dependency Failure

Mögliche Zustände:

```text
Satisfied
Unsatisfied
Conflict
Unavailable
Untrusted
Incompatible
Unknown
```

Für harte Dependencies gilt:

```text
Unknown ≠ Satisfied
```

## Caching

Dependency Resolution darf gecacht werden.

Der Cache muss ungültig werden können bei Änderungen an:

```text
Package Set
Installed State
Trust Policy
Compatibility Rules
Provider State
System Configuration
```

## Provenance

Auflösungsentscheidungen sollen nachvollziehbar sein.

```text
Requested Update
      ↓
Dependency Requirement
      ↓
Candidate Providers
      ↓
Selected Provider
      ↓
Decision Reason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Dependency Graph
Required Dependencies
Optional Dependencies
Selected Providers
Version Constraints
Conflicts
Compatibility State
Trust State
Resolution Result
Decision Reason
```

## Normative Anforderungen

1. NovaOS MUSS Update-Abhängigkeiten vor Anwendung vollständig auflösen können.
2. Harte Dependencies MÜSSEN vor Aktivierung erfüllt sein.
3. Transitive Dependencies MÜSSEN berücksichtigt werden.
4. Dependency Cycles MÜSSEN erkannt werden.
5. Nicht unterstützte zyklische Dependencies MÜSSEN das Update blockieren.
6. Versionsbereiche MÜSSEN unterstützt werden.
7. Version Match DARF NICHT automatisch als vollständige Kompatibilität gelten.
8. ABI-, API- und Contract-Kompatibilität MÜSSEN für kritische Dependencies prüfbar sein.
9. Capability-basierte Dependencies MÜSSEN unterstützt werden können.
10. Required Capability DARF NICHT zwingend an einen konkreten Provider gebunden sein.
11. Alternative Provider MÜSSEN auflösbar sein können.
12. Provider-Auswahl MUSS Security- und Trust-Regeln einhalten.
13. Konflikte MÜSSEN vor Anwendung erkannt werden.
14. Konflikte DÜRFEN NICHT stillschweigend ignoriert werden.
15. Installation Order und Activation Order MÜSSEN getrennt modellierbar sein.
16. Abhängige Komponenten DÜRFEN NICHT vor ihren harten Dependencies aktiviert werden.
17. State-Schema-Abhängigkeiten MÜSSEN berücksichtigt werden können.
18. Notwendige Migrationen MÜSSEN Teil des Update Plans sein.
19. Der vollständige Update Set MUSS vor Commit konsistent sein.
20. Ein Dependency Failure DARF keinen inkonsistenten Teilzustand hinterlassen.
21. Rollback MUSS den resultierenden Dependency Graph erneut validieren.
22. `Unknown` DARF bei harten Dependencies NICHT als `Satisfied` gelten.
23. Dependency Cache MUSS bei relevanten Änderungen invalidierbar sein.
24. Resolution Decisions SOLLEN nachvollziehbare Provenance besitzen.
25. Dependency-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-API-COMPATIBILITY-0001`
- `NPSPEC-ABI-VERSIONING-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-VERIFY-CONTRACT-0001`
- `ADR-ARCH-0165`

## Ergebnis

```text
Requested Update
      ↓
Build Dependency Graph
      ↓
Resolve Versions + Capabilities
      ↓
Check Conflicts
      ↓
Check Compatibility + Trust
      ↓
Complete Set Valid?
├── No  → Block Update
└── Yes → Create Update Set
             ↓
       Transactional Apply
             ↓
          Verify
             ↓
           Commit
```

NovaOS erhält damit ein dependency-basiertes Update-Modell, das nicht nur Paketversionen, sondern auch ABI-, API-, Contract-, State-, Capability- und Trust-Anforderungen berücksichtigt und nur vollständig konsistente Update-Zustände aktiviert.