# NPSPEC-REGISTRY-CAPABILITY-0001 – Nova Capability Registry

## Status

Angenommen

## Kategorie

Registry / Capability

## Zweck

NovaOS definiert die Capability Registry als systemweites Verzeichnis verfügbarer Capability-Typen und ihrer Provider.

Die Registry ermöglicht Discovery, Auflösung und Versionierung von Capabilities, speichert jedoch keine erteilten Capability-Tokens oder Berechtigungen.

## Grundprinzipien

```text
Registry Entry ≠ Capability Token
CapabilityID ≠ Permission
Discovery ≠ Authority
Provider ≠ Capability Identity
Registration ≠ Grant
Path ≠ Capability Identity
```

## Capability-ID

Capability-IDs besitzen das verbindliche Format:

```text
domain.authority.namespace.name
```

Beispiel:

```text
de.nova.image.filter.gaussian
```

Dabei gilt:

```text
de       → Domain
nova     → Authority
image.filter → Namespace
gaussian → Name
```

Kategorieordner oder Registry-Pfade bestimmen nicht die Identität einer Capability.

## Registry-Modell

Ein Eintrag kann enthalten:

```text
CapabilityRegistryEntry
├── CapabilityID
├── Version
├── Interface
├── ProviderIDs
├── SemanticInput
├── SemanticOutput
├── Requirements
└── State
```

Optional:

```text
Metadata
Trust Requirements
Execution Requirements
Compatibility
Documentation
```

## Registrierung

Capability Provider registrieren die von ihnen bereitgestellten Funktionen:

```text
Provider
   ↓
Validate
   ↓
Register Capability
   ↓
Publish Metadata
```

Mehrere Provider dürfen dieselbe Capability implementieren.

## Discovery

```text
Capability Requirement
        ↓
Registry Query
        ↓
CapabilityID
        ↓
Compatible Providers
        ↓
Policy / Negotiation
        ↓
Selected Provider
```

Discovery liefert ausschließlich Informationen über verfügbare Fähigkeiten und Provider.

Sie gewährt keine Ausführungsberechtigung.

## Versionierung

Eine Capability kann mehrere kompatible oder inkompatible Versionen besitzen.

```text
CapabilityID
├── Version 1
├── Version 2
└── Version 3
```

Aufrufer können Anforderungen an Version, Interface, semantische Typen oder andere Eigenschaften deklarieren.

## Provider

Provider und Capability bleiben getrennte Identitäten.

```text
CapabilityID
      ↓
Provider A
Provider B
Provider C
```

Provider dürfen abhängig von Policy, Trust, Ressourcen, Standort oder Execution Contract ausgewählt werden.

## Berechtigungen

Die Registry speichert keine aktiven Berechtigungen:

```text
Registry
   ↓
Discovery
   ↓
Capability Requirement
   ↓
Permission Evaluation
   ↓
Capability Token / Handle
```

Das Wissen über eine `CapabilityID` erzeugt keine Authority.

## Sicherheit

Registry-Einträge dürfen keine aktiven Capability-Tokens, Credentials oder delegierte Authority enthalten.

Registrierung und Änderung von Providern müssen autorisiert und bei sicherheitsrelevanten Capabilities auf Trust und Integrität prüfbar sein.

## Normative Anforderungen

1. NovaOS MUSS eine Capability Registry bereitstellen.
2. Capability-IDs MÜSSEN dem Schema `domain.authority.namespace.name` entsprechen.
3. Registry-Pfade DÜRFEN die Capability-Identität nicht bestimmen.
4. Mehrere Provider DÜRFEN dieselbe Capability bereitstellen.
5. Provider und Capability MÜSSEN getrennte Identitäten besitzen.
6. Capability-Versionen MÜSSEN unterscheidbar sein.
7. Discovery DARF keine Authority erzeugen.
8. Die Registry DARF keine aktiven Capability-Tokens speichern.
9. Capability-Anforderungen MÜSSEN gegen verfügbare Provider auflösbar sein.
10. Provider-Registrierung MUSS autorisiert und validierbar sein.
11. Sicherheitsrelevante Provider MÜSSEN auf Trust und Integrität prüfbar sein.
12. Capability-, Provider-, Versions- und Verfügbarkeitsinformationen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine zentrale Discovery- und Auflösungsschicht für Capabilities. Stabile Capability-IDs bleiben von Providern, Registry-Pfaden und Berechtigungen getrennt, während mehrere Implementierungen versioniert registriert und anhand von Anforderungen und Policies ausgewählt werden können.