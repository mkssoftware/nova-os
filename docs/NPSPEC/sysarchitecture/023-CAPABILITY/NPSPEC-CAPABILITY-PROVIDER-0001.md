# NPSPEC-CAPABILITY-PROVIDER-0001 – Nova Capability Provider

## Status

Angenommen

## Kategorie

Capability / Provider / Security

## Zweck

NovaOS definiert Capability Provider als Komponenten, die eine klar definierte Fähigkeit bereitstellen und diese über ein versioniertes Capability Interface anbieten.

```text
Provider
   ↓
Capability Interface
   ↓
Registry
   ↓
Discovery
   ↓
Negotiation
   ↓
Capability Instance
```

Provider können ausgetauscht werden, ohne dass Verbraucher von einer konkreten Implementierung abhängig sein müssen.

## Grundprinzipien

```text
Provider ≠ Capability
Provider Identity ≠ Authority
Registered Provider ≠ Trusted Provider
Trusted Provider ≠ Authorized Provider
Provider Location ≠ Capability Identity
Provider Failure ≠ Capability Success
Provider Replacement ≠ Authority Expansion
```

## Provider-Modell

Ein Provider beschreibt mindestens:

```text
CapabilityProvider
├── ProviderID
├── Provider Identity
├── CapabilityTypeID
├── Interface Version
├── Security Domain
└── State
```

Optional:

```text
Provider InstanceID
Trust State
Location
Supported Constraints
Resource Properties
Sovereignty Properties
Execution Properties
Health State
```

## Provider-Typen

NovaOS kann unterschiedliche Provider verwenden:

```text
Kernel Provider
System Service
Driver
Application
Hardware Provider
Remote Service
Distributed Workload
Compatibility Provider
```

Die Capability-Semantik bleibt unabhängig von der konkreten Provider-Art.

## Registrierung

Provider müssen sich kontrolliert in der Capability Registry registrieren.

```text
Provider Start
     ↓
Identity Validation
     ↓
Interface Validation
     ↓
Policy Validation
     ↓
Registry Entry
```

Registrierung erzeugt keine Autorität für Clients.

## Discovery

Clients suchen primär nach benötigten Fähigkeiten statt konkreten Implementierungen.

```text
Required:
Document.Render

       ↓

Provider A
Provider B
Provider C
```

Die Registry und Discovery-Schicht bestimmt zulässige Kandidaten.

## Provider-Auswahl

Die Auswahl kann anhand eines `Nova.ExecutionContract` erfolgen.

```text
Candidates
    ↓
Hard Constraints
    ↓
Security + Trust
    ↓
Sovereignty
    ↓
Resource Requirements
    ↓
Soft Preferences
    ↓
Selected Provider
```

Die Priorität folgt:

```text
Safety
→ Security
→ Sovereignty / Trust
→ Hard System Constraints
→ Explicit User Decisions
→ Soft Preferences
→ Adaptive Optimization
```

## Autorität

Ein Provider erhält nur die Autorität, die zur Erfüllung seiner Capability erforderlich ist.

```text
Client Capability
       ↓
Attenuation
       ↓
Provider Operation Authority
```

Der Provider darf keine vollständige Client-Autorität übernehmen.

## Provider Isolation

Provider sollen in eigenen Security Domains ausführbar sein.

```text
Client Domain
     ↓
Capability IPC
     ↓
Provider Domain
```

Ein kompromittierter Provider soll dadurch auf seine expliziten Capabilities begrenzt bleiben.

## Trust

Provider Trust wird unabhängig von Provider Identity und Capability Authority bewertet.

```text
Provider Identity
      +
Signature
      +
Provenance
      +
Attestation
      ↓
Trust Evaluation
```

Ein vertrauenswürdiger Provider erhält dadurch keine zusätzlichen Rechte.

## Lokale und entfernte Provider

Provider können ortsunabhängig angeboten werden.

```text
CapabilityTypeID
├── Local Provider
├── Remote Provider
└── Migrated Provider
```

Dabei gilt:

```text
Transparent Location ≠ Transparent Authority
```

Remote Provider müssen weiterhin lokale Security-, Trust- und Sovereignty-Regeln erfüllen.

## Provider-Ausfall

Capability-Nutzung muss Provider-Ausfälle berücksichtigen.

```text
Provider Failure
      ↓
Health State Update
      ↓
Capability Re-Evaluation
      ↓
Fail / Retry / Alternate Provider
```

Ein alternativer Provider darf nur verwendet werden, wenn dessen Auswahl alle Hard Requirements erfüllt.

## Live Replacement

Provider sollen ohne unnötige Unterbrechung ersetzbar sein.

```text
Old Provider
     ↓
Load New Provider
     ↓
Validate
     ↓
Transfer State
     ↓
Switch
     ↓
Verify
     ↓
Retire Old
```

Bestehende Autorität darf dabei nicht erweitert werden.

## Mehrere Provider

Mehrere Provider derselben Capability können parallel existieren.

```text
Image.Decode
├── CPU Provider
├── GPU Provider
└── Remote Provider
```

NovaOS kann anhand von ExecutionContract, Ressourcenlage und Policy einen geeigneten Provider wählen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ProviderID
Provider Identity
CapabilityTypeID
Interface Version
Security Domain
Trust State
Location Class
Health State
Supported Constraints
Active Capability Instances
```

## Normative Anforderungen

1. NovaOS MUSS Capability Provider eindeutig identifizieren können.
2. Provider Identity MUSS von Capability Identity getrennt bleiben.
3. Provider MÜSSEN angebotene Capability Interfaces explizit deklarieren.
4. Provider Registration MUSS kontrolliert und validiert erfolgen.
5. Registrierung DARF keine Client-Autorität erzeugen.
6. Provider MÜSSEN mit minimal erforderlicher Autorität ausführbar sein.
7. Mehrere Provider derselben Capability MÜSSEN unterstützt werden können.
8. Provider-Auswahl MUSS Security-, Trust-, Sovereignty- und Hard Requirements berücksichtigen.
9. Provider Replacement DARF keine zusätzliche Autorität erzeugen.
10. Provider-Zustände MÜSSEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-TRUST-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-CAPABILITY-0017`

## Ergebnis

```text
Capability Requirement
        ↓
Registry + Discovery
        ↓
Eligible Providers
        ↓
Policy + ExecutionContract
        ↓
Selected Provider
        ↓
Minimal Authority
        ↓
Capability Execution
```

NovaOS erhält damit ein providerunabhängiges Capability-Modell, in dem Fähigkeiten dynamisch von lokalen, entfernten oder austauschbaren Komponenten bereitgestellt werden können, ohne Capability-Semantik, Provider-Identität und Autorität miteinander zu vermischen.