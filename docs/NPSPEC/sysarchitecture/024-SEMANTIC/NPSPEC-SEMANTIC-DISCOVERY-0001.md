# NPSPEC-SEMANTIC-DISCOVERY-0001 – Nova Semantic Discovery

## Status

Angenommen

## Kategorie

Semantic / Discovery / Architecture

## Zweck

NovaOS definiert eine systemweite semantische Discovery-Schicht, über die Daten, Ressourcen, Capabilities, Services und Provider anhand ihrer Bedeutung und Anforderungen gefunden werden können.

```text
Semantic Requirement
        ↓
Discovery
        ↓
Semantic Candidates
        ↓
Compatibility + Policy
        ↓
Usable Candidates
```

Discovery beantwortet primär die Frage:

```text
"Was kann diese Anforderung erfüllen?"
```

statt:

```text
"Wo befindet sich eine bestimmte Implementierung?"
```

## Grundprinzipien

```text
Discovery ≠ Authority
Discoverable ≠ Accessible
Found ≠ Compatible
Compatible ≠ Authorized
Registered ≠ Trusted
Known Identity ≠ Permission
Discovery Result ≠ Provider Selection
Location ≠ Semantic Identity
```

## Discovery Request

Eine Anfrage beschreibt die benötigte Semantik.

```text
SemanticDiscoveryRequest
├── Required Semantic Type
├── Required Operation
├── Required Capability Type
└── Constraints
```

Optional:

```text
Input Semantic Types
Output Semantic Types
Resource Requirements
Trust Requirements
Security Requirements
Sovereignty Requirements
Location Preference
Version Requirements
ExecutionContract
```

Nicht benötigte Felder können entfallen.

## Discovery-Ziele

Semantic Discovery kann unterschiedliche Entitäten finden:

```text
Semantic Files
Semantic Resources
Semantic Capabilities
Services
Providers
Conversion Paths
Compatible Interfaces
```

Alle verwenden dieselbe semantische Grundlage, bleiben aber unterschiedliche Entitätstypen.

## Discovery-Prozess

```text
Requirement
    ↓
Semantic Resolution
    ↓
Registry / Index / Graph
    ↓
Candidate Set
    ↓
Compatibility
    ↓
Security + Trust + Sovereignty
    ↓
Eligible Candidates
```

Discovery selbst erzeugt keine Capability.

## Semantic Types

Semantic Types dienen als zentrale Discovery-Eigenschaft.

Beispiel:

```text
Need:
Nova.Image.Raster

Operation:
Edit
```

NovaOS kann daraus passende Capabilities und Provider bestimmen.

## Resource Discovery

Ressourcen können semantisch gesucht werden.

```text
Need:
Compute.GPU

Constraints:
Memory >= 2 GiB
Location = Local
```

Die physische Geräteidentität muss dem Requester nicht vorab bekannt sein.

## Capability Discovery

Eine Capability kann anhand ihrer Semantik gesucht werden.

```text
Input:
Nova.Document.Text

Operation:
Render

Output:
Nova.Image.Raster
```

Mögliche Ergebnisse:

```text
Provider A
Provider B
Provider C
```

Die endgültige Auswahl erfolgt separat.

## Conversion Discovery

Wenn keine direkte Kompatibilität besteht:

```text
Source Type
     ↓
Compatibility Check
     ↓
Conversion Discovery
     ↓
Conversion Path
     ↓
Target Type
```

Mehrstufige Conversion Paths dürfen berücksichtigt werden.

## Relationship Discovery

Der Semantic Relationship Graph kann Discovery unterstützen.

```text
Project
   ↓ Contains
Documents
   ↓ References
Datasets
```

Graph-Traversierung muss Security- und Privacy-Grenzen beachten.

## Registry und Query

Semantic Discovery kann mehrere Informationsquellen verwenden:

```text
Capability Registry
Resource Registry
Semantic Metadata Index
Relationship Graph
Semantic Query Engine
```

Die Discovery-Schnittstelle bleibt von deren physischer Implementierung getrennt.

## ExecutionContract

Ein `Nova.ExecutionContract` kann als Discovery-Anforderung dienen.

```text
ExecutionContract
       ↓
Semantic Requirements
       ↓
Discovery
       ↓
Eligible Candidates
```

Dabei können berücksichtigt werden:

```text
Semantic Types
Capabilities
Resources
Deadline
Resource Budget
Determinism
Trust
Sovereignty
Location
```

## Location Transparency

Discovery kann lokale und entfernte Kandidaten finden.

```text
Semantic Requirement
├── Local Provider
├── Local Hardware
├── Remote Provider
└── Distributed Resource
```

Dabei gilt:

```text
Transparent Location ≠ Transparent Authority
```

Remote Discovery darf lokale Trust-, Security- oder Sovereignty-Regeln nicht umgehen.

## Dynamische Discovery

Discovery muss Änderungen der Systemlandschaft berücksichtigen.

```text
Provider Added
Resource Removed
Capability Revoked
Device Hotplug
Service Migrated
Trust Changed
```

Gecachte Discovery-Ergebnisse müssen invalidierbar oder erneut prüfbar sein.

## Datenschutz und Sicherheit

Discovery darf keine unautorisierte Enumeration ermöglichen.

Nicht sichtbare Entitäten dürfen nicht unnötig über:

```text
Names
Metadata
Counts
Relationships
Locations
Timing
```

offengelegt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
DiscoveryID
Semantic Requirement
Candidate Count
Compatibility State
Applied Constraints
Selected Sources
Filtered Candidates
Discovery State
```

Sensible oder nicht autorisierte Kandidaten dürfen dabei nicht sichtbar werden.

## Normative Anforderungen

1. NovaOS MUSS semantische Discovery systemweit unterstützen können.
2. Discovery MUSS unabhängig von konkreten Providern und Speicherorten formulierbar sein.
3. Semantic Types, Capabilities und Resources MÜSSEN als Discovery-Kriterien verwendbar sein.
4. Discovery DARF keine Autorität erzeugen.
5. Compatibility MUSS vor Nutzung eines Kandidaten überprüfbar sein.
6. Security-, Trust- und Sovereignty-Regeln MÜSSEN Discovery-Ergebnisse filtern können.
7. Conversion Paths SOLLEN über Semantic Discovery auffindbar sein.
8. Lokale und entfernte Kandidaten SOLLEN über dasselbe semantische Modell auffindbar sein.
9. Discovery-Caches MÜSSEN bei relevanten Zustandsänderungen revalidierbar sein.
10. Discovery MUSS introspektierbar sein, ohne unautorisierte Entitäten offenzulegen.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-QUERY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0019`

## Ergebnis

```text
Semantic Requirement
        ↓
System-wide Discovery
        ↓
Types + Resources + Capabilities
        ↓
Compatibility Filtering
        ↓
Security + Trust + Sovereignty
        ↓
Eligible Candidates
        ↓
Separate Authorization / Selection
```

NovaOS erhält damit eine gemeinsame Discovery-Schicht, über die Komponenten beschreiben können, **was sie benötigen**, während das System geeignete Daten, Ressourcen, Capabilities und Provider anhand ihrer Semantik findet, ohne Auffindbarkeit mit Zugriff oder Autorität gleichzusetzen.