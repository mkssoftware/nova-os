# NPSPEC-SEMANTIC-RELATIONSHIP-0001 – Nova Semantic Relationship

## Status

Angenommen

## Kategorie

Semantic / Relationship / Data Model

## Zweck

NovaOS definiert ein systemweites Modell für semantische Beziehungen zwischen Objekten, Ressourcen, Identitäten und anderen semantischen Entitäten.

```text
Entity A
   ↓
Relationship
   ↓
Entity B
```

Beziehungen beschreiben, **wie Entitäten fachlich miteinander verbunden sind**, unabhängig von Pfad, Speicherort oder konkreter Anwendung.

## Grundprinzipien

```text
Relationship ≠ Authority
Relationship ≠ Ownership
Relationship ≠ Storage Location
Relationship ≠ Hard Link
Relationship ≠ Capability
Known Relationship ≠ Access
Related Object ≠ Accessible Object
```

## Relationship-Modell

Eine Beziehung besitzt mindestens:

```text
SemanticRelationship
├── RelationshipID
├── RelationshipTypeID
├── SourceID
├── TargetID
└── State
```

Optional:

```text
Direction
Metadata
Provenance
Timestamp
Confidence
Trust State
Security Label
Privacy Label
Constraints
Version
```

## Beziehungstypen

Beispiele:

```text
DerivedFrom
Contains
References
RelatedTo
DependsOn
CreatedBy
OwnedBy
PartOf
VersionOf
GeneratedFrom
AssociatedWith
```

Beziehungstypen erhalten stabile semantische Identitäten.

```text
RelationshipTypeID
```

Die Bedeutung darf nicht ausschließlich durch frei gewählte Namen definiert werden.

## Gerichtete Beziehungen

Beziehungen können gerichtet sein.

```text
Document A
   ↓ DerivedFrom
Document B
```

Die inverse Beziehung kann separat definiert werden:

```text
Document B
   ↓ SourceOf
Document A
```

NovaOS darf inverse Beziehungen nur ableiten, wenn dies durch die Semantik des Beziehungstyps erlaubt ist.

## Bidirektionale Beziehungen

Bestimmte Beziehungen können symmetrisch sein.

```text
Object A
   ↕ RelatedTo
Object B
```

Dies muss explizit im Relationship Type definiert sein.

## Stabile Identitäten

Beziehungen sollen stabile IDs verwenden.

```text
Source ObjectID
      ↓
Relationship
      ↓
Target ObjectID
```

Pfadänderungen beeinflussen dadurch die Beziehung nicht.

```text
Identity ≠ Location
```

## Relationship Graph

Semantische Beziehungen bilden einen Graphen.

```text
        Project
       /       \
   Contains   Contains
     ↓           ↓
Document A → References → Dataset
     ↓
DerivedFrom
     ↓
Document B
```

Dieser Graph kann für Navigation, Discovery, Verarbeitung und Introspection genutzt werden.

## Semantic File Integration

Semantic Files können Beziehungen direkt besitzen.

```text
SemanticFile
├── ObjectID
├── SemanticType
├── Metadata
└── Relationships
```

NovaFile kann diese Beziehungen in seiner nativen Containerstruktur speichern.

## Capability Integration

Eine Beziehung erzeugt niemals automatisch Zugriff.

```text
Object A
   ↓ References
Object B
```

bedeutet nicht:

```text
Read Capability(Object B)
```

Für den Zugriff auf das Zielobjekt ist weiterhin eine entsprechende Capability erforderlich.

## Provenance

Beziehungen können Teil der Provenance sein.

```text
Source Object
     ↓
Transformation
     ↓
Derived Object
```

Beispiel:

```text
Image B
DerivedFrom
Image A
```

NovaOS kann dadurch Herkunftsketten rekonstruieren.

## Lebenszyklus

Beziehungen besitzen einen eigenen Lebenszyklus.

```text
Created
   ↓
Active
   ↓
Updated
   ↓
Removed
```

Das Löschen einer Beziehung darf die beteiligten Objekte nicht automatisch löschen.

## Transitivität

Beziehungen dürfen nur dann transitiv ausgewertet werden, wenn der Relationship Type dies ausdrücklich erlaubt.

```text
A → B
B → C
```

bedeutet nicht automatisch:

```text
A → C
```

Eigenschaften wie:

```text
Directional
Symmetric
Transitive
Inverse
```

müssen Teil der Typdefinition sein.

## Sicherheit und Datenschutz

Beziehungen können sensible Informationen offenlegen.

Beispiele:

```text
Person → MemberOf → Organization
Document → CreatedBy → Identity
Image → CapturedAt → Location
```

Deshalb müssen Beziehungen Security- und Privacy-Regeln unterliegen.

```text
Relationship Visibility ≠ Object Access
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
RelationshipID
RelationshipTypeID
SourceID
TargetID
Direction
Properties
Provenance
Trust State
Security Label
Privacy Label
State
```

Graph-Abfragen müssen Capability- und Privacy-Grenzen berücksichtigen.

## Normative Anforderungen

1. NovaOS MUSS semantische Beziehungen zwischen Systementitäten darstellen können.
2. Beziehungstypen MÜSSEN stabile `RelationshipTypeID`s besitzen können.
3. Beziehungen SOLLEN stabile Objekt- und Ressourcenidentitäten verwenden.
4. Beziehungen DÜRFEN keine Autorität erzeugen.
5. Gerichtete, symmetrische und inverse Beziehungen MÜSSEN explizit modellierbar sein.
6. Transitivität DARF NICHT automatisch angenommen werden.
7. Beziehungen MÜSSEN unabhängig vom physischen Speicherort bleiben.
8. Relationship-Daten MÜSSEN Security- und Privacy-Regeln unterliegen.
9. Beziehungen SOLLEN Provenance und Semantic Metadata integrieren können.
10. Relationship Graphs MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROVENANCE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0014`

## Ergebnis

```text
Semantic Entities
       ↓
Typed Relationships
       ↓
Relationship Graph
       ↓
Metadata + Provenance
       ↓
Authorized Discovery
       ↓
Semantic System Context
```

NovaOS erhält damit einen systemweiten semantischen Beziehungsgraphen, in dem Dateien, Ressourcen, Identitäten und andere Objekte über stabile und maschinenlesbare Beziehungen miteinander verknüpft werden können, ohne Beziehungen mit Speicherstruktur oder Zugriffsrechten gleichzusetzen.