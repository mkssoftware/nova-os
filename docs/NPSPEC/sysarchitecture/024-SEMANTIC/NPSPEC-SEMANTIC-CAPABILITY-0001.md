# NPSPEC-SEMANTIC-CAPABILITY-0001 – Nova Semantic Capability

## Status

Angenommen

## Kategorie

Semantic / Capability / Architecture

## Zweck

NovaOS definiert Capabilities zusätzlich über ihre semantische Bedeutung. Komponenten fordern dadurch nicht primär eine konkrete Implementierung oder einen bestimmten Service an, sondern eine Fähigkeit mit definierten Ein- und Ausgaben, Operationen und Eigenschaften.

```text
Intent
  ↓
Semantic Capability Requirement
  ↓
Capability Discovery
  ↓
Compatible Provider
  ↓
Authorized Capability Instance
```

## Grundprinzipien

```text
Semantic Capability ≠ Capability Instance
Capability Type ≠ Provider
Capability Type ≠ Application
Capability Identity ≠ Implementation
Semantic Compatibility ≠ Authorization
Discovery ≠ Authority
Provider Selection ≠ Capability Grant
```

## Semantic Capability Type

Eine semantische Capability besitzt mindestens:

```text
SemanticCapabilityType
├── CapabilityTypeID
├── Operation Semantics
├── Input Semantic Types
├── Output Semantic Types
└── Interface Version
```

Optional:

```text
Resource Requirements
Execution Properties
Supported Constraints
Trust Requirements
Security Requirements
Determinism Properties
Sovereignty Properties
```

Beispiele:

```text
Document.Render
Image.Resize
Audio.Encode
Data.Analyze
Storage.Read
Network.Connect
Compute.Execute
```

## CapabilityTypeID

`CapabilityTypeID` identifiziert die Bedeutung einer Fähigkeit unabhängig von ihrem Provider.

```text
Document.Render
├── Provider A
├── Provider B
└── Provider C
```

Alle Provider können dieselbe semantische Capability anbieten, obwohl ihre Implementierungen unterschiedlich sind.

## Ein- und Ausgaben

Semantic Capabilities deklarieren ihre Datenanforderungen über Semantic Types.

```text
Image.Resize

Input:
Nova.Image.Raster

Output:
Nova.Image.Raster
```

Dadurch kann NovaOS überprüfen, ob Daten und Capability semantisch kompatibel sind.

## Operation Semantics

Eine Capability muss definieren, welche Wirkung ihre Operation besitzt.

Beispiel:

```text
Document.Convert
├── Input: Nova.Document
├── Output: Nova.Document
└── Effect: Representation Transformation
```

Die Semantik darf nicht still zwischen Providern oder kompatiblen Versionen verändert werden.

## Discovery

Semantic Capability Discovery sucht anhand der benötigten Fähigkeit.

```text
Required Operation
       ↓
CapabilityTypeID
       ↓
Registry
       ↓
Compatible Providers
```

Der Aufrufer muss dadurch keine konkrete Anwendung oder Provider-Implementierung kennen.

## Authorization

Die semantische Beschreibung erzeugt keine Autorität.

```text
Semantic Capability
       ↓
Provider Discovery
       ↓
Policy + Trust
       ↓
Authorization
       ↓
Capability Instance
```

Es gilt:

```text
CapabilityTypeID ≠ CapabilityID
```

`CapabilityTypeID` beschreibt die Fähigkeit.

`CapabilityID` identifiziert eine konkrete Autoritätsinstanz.

## ExecutionContract

`Nova.ExecutionContract` kann Semantic Capabilities direkt anfordern.

```text
ExecutionContract
├── Operation
├── Input Semantic Types
├── Output Semantic Types
├── Required Capability Types
├── Resource Requirements
├── Trust Requirements
└── Constraints
```

NovaOS kann daraus geeignete Provider und konkrete Capability-Instanzen bestimmen.

## Semantic Resources

Capabilities können semantische Ressourcen benötigen.

```text
Image.Render
      ↓
Compute.GPU
      +
Display.Output
```

Die konkrete Ressource wird unabhängig von der Capability-Semantik aufgelöst.

## Composition

Mehrere Semantic Capabilities können zu einer Ausführungskette kombiniert werden.

```text
Document.Load
      ↓
Document.Parse
      ↓
Document.Render
      ↓
Display.Output
```

Jeder Schritt behält seine eigenen Autoritäts- und Sicherheitsanforderungen.

## Provider-Unabhängigkeit

Provider können ausgetauscht werden, solange sie dieselbe erforderliche Semantik und alle Hard Requirements erfüllen.

```text
Semantic Capability
├── Local Provider
├── Hardware Provider
├── Application Provider
└── Remote Provider
```

Dabei gilt:

```text
Same Capability Type ≠ Same Trust
Same Capability Type ≠ Same Performance
Same Capability Type ≠ Same Authority
```

## Versionierung

Semantic Capabilities müssen versionierbar sein.

```text
CapabilityTypeID
├── v1
├── v2
└── v3
```

Inkompatible Änderungen der Operations-, Sicherheits- oder Datensemantik müssen explizit erkennbar sein.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
CapabilityTypeID
Operation Semantics
Input Semantic Types
Output Semantic Types
Interface Version
Available Providers
Supported Constraints
Resource Requirements
Trust Requirements
```

Konkrete Capability Tokens dürfen dadurch nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capabilities semantisch beschreiben können.
2. `CapabilityTypeID` MUSS von konkreten `CapabilityID`s getrennt bleiben.
3. Semantic Capabilities MÜSSEN unabhängig von konkreten Providern definierbar sein.
4. Ein- und Ausgaben SOLLEN über Semantic Types beschrieben werden.
5. Semantic Capability Discovery DARF keine Autorität erzeugen.
6. Konkrete Nutzung MUSS weiterhin durch Capability Security autorisiert werden.
7. ExecutionContracts SOLLEN Semantic Capability Requirements verwenden können.
8. Semantic Capabilities MÜSSEN mit Semantic Resources kombinierbar sein.
9. Provider-Wechsel DÜRFEN die definierte Semantik oder Hard Requirements NICHT still verändern.
10. Semantic Capabilities MÜSSEN versionierbar und introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-IPC-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0012`

## Ergebnis

```text
Semantic Intent
      ↓
Semantic Capability Type
      ↓
Semantic Inputs / Outputs
      ↓
Discovery
      ↓
Policy + Authorization
      ↓
Compatible Provider
      ↓
Concrete Capability Instance
      ↓
Execution
```

NovaOS erhält damit eine semantische Capability-Ebene, durch die Komponenten ausdrücken können, **welche Fähigkeit benötigt wird**, statt festzulegen, welche Anwendung, welcher Service oder welche konkrete Implementierung diese Fähigkeit bereitstellen muss.