# NPSPEC-FILESYSTEM-RELATION-0001 – Nova Filesystem Relations

## Status

Angenommen

## Kategorie

Filesystem / Relations / Semantic Model

## Zweck

NovaOS unterstützt explizite, persistente Beziehungen zwischen Filesystem-Objekten.

Beziehungen werden nicht über veränderliche Pfade, sondern über stabile `ObjectID`s beschrieben.

```text
Object A
   ↓
Relation
   ↓
Object B
```

Dadurch bleiben Zusammenhänge auch bei Rename, Move, Projection oder Volume-Migration erhalten.

## Grundprinzipien

```text
Relation ≠ Path
Relation ≠ Directory Hierarchy
Relation ≠ Projection
Relation ≠ Copy
Relation ≠ Capability
Relation ≠ Ownership
Related ≠ Authorized
```

Die klassische Verzeichnisstruktur beschreibt, **wo ein Objekt sichtbar ist**.

Relations beschreiben, **wie Objekte logisch miteinander zusammenhängen**.

## Relation Model

```text
Relation
├── RelationID
├── RelationTypeID
├── SourceObjectID
├── TargetObjectID
├── Direction
├── State
└── Version
```

Optional:

```text
Metadata
OwnerID
Created
Modified
TransactionID
SecurityLabel
ProvenanceID
```

## Stabile Referenzen

Relations verwenden stabile Objektidentitäten.

```text
ObjectID A
    ↓
Relation
    ↓
ObjectID B
```

Nicht:

```text
/Pfad/A
   ↓
Relation
   ↓
/Pfad/B
```

Dadurch überlebt eine Relation:

```text
Rename
Move
Projection Change
Volume Migration
```

## Relation Types

Relation Types besitzen stabile `RelationTypeID`s.

Beispiele:

```text
belongs-to
contains
references
derived-from
depends-on
related-to
version-of
created-from
associated-with
```

Relation Types dürfen systemspezifisch oder erweiterbar sein.

Die Identität eines Relation Types darf nicht ausschließlich aus seiner sichtbaren Bezeichnung abgeleitet werden.

## Richtung

Relations können gerichtet oder symmetrisch sein.

Gerichtet:

```text
A ──derived-from──→ B
```

Symmetrisch:

```text
A ←──related-to──→ B
```

Die Semantik der Richtung wird durch den Relation Type definiert.

## Mehrere Relations

Ein Objekt darf beliebig viele Beziehungen besitzen.

```text
Object A
├── belongs-to ──→ Project
├── references ──→ Document B
├── derived-from ─→ Dataset C
└── related-to ──→ Image D
```

Dadurch kann dasselbe Objekt gleichzeitig in mehreren fachlichen Zusammenhängen existieren.

## Relation Graph

Relations bilden einen Graphen über Filesystem-Objekten.

```text
      Object B
      ↑      \
 references  \
      |       \
Object A ─────→ Object C
      \
       \
        → Object D
```

Das Filesystem bleibt damit hierarchisch navigierbar, erhält zusätzlich jedoch eine graphbasierte semantische Ebene.

## Directory Hierarchy

Directory Parent/Child und semantische Relation bleiben getrennt.

```text
Directory Hierarchy
≠
Semantic Relation Graph
```

Ein Objekt muss nicht physisch innerhalb eines Projektverzeichnisses liegen, um semantisch zu diesem Projekt zu gehören.

## Relation Projection

Relations können Filesystem-Projections erzeugen.

```text
Project Object
      ↓
Relation Query
      ↓
Related ObjectIDs
      ↓
Projection Engine
      ↓
/Solutions/Projekt/
```

Beispiel:

```text
Project
├── Dokument.nf
├── Bild.nf
└── Daten.nf
```

Die dargestellten Objekte können physisch an völlig unterschiedlichen Orten gespeichert sein.

```text
Relation Projection
≠
Object Copy
```

## Semantische Suche

Relations müssen durch das semantische Filesystem abfragbar sein.

Beispiele:

```text
Alle Objekte von Projekt X

Alle von Objekt Y abgeleiteten Dateien

Alle Dokumente, die Objekt Z referenzieren

Alle Abhängigkeiten einer Solution
```

Ergebnis einer Relation Query sind grundsätzlich stabile Objektidentitäten.

## Reverse Relations

NovaOS soll Beziehungen auch rückwärts auflösen können.

Aus:

```text
A ──references──→ B
```

kann abgefragt werden:

```text
Welche Objekte referenzieren B?
```

Dafür dürfen Indizes verwendet werden.

## Relation Metadata

Eine Relation kann eigene Metadaten besitzen.

Beispiel:

```text
Relation
├── Created
├── Creator
├── Description
├── Priority
└── Properties
```

Relation Metadata gehört zur Beziehung und nicht automatisch zu Source oder Target.

## NovaFile Integration

NovaFile kann Relations direkt mitführen.

```text
NovaFile
├── ObjectID
├── Payload
├── Metadata
└── Relationships
```

Die gespeicherten Beziehungen referenzieren andere Ressourcen über deren stabile Identitäten.

Dadurch können Relations gemeinsam mit dem Objekt versioniert und nachvollzogen werden.

## Versionierung

Relations müssen versionierbar sein.

```text
Relation Version 1
      ↓
Relation Version 2
```

Änderungen können sein:

```text
Create
Modify
Remove
Retarget
```

Ein Retarget muss als explizite Änderung behandelt werden.

## Gelöschte Ziele

Wird ein referenziertes Objekt gelöscht, darf eine Relation nicht stillschweigend auf ein anderes Objekt zeigen.

```text
Relation
   ↓
Deleted ObjectID
   ↓
Unresolved
```

Mögliche Zustände:

```text
Valid
Unresolved
Invalid
Unavailable
Unknown
```

`Unknown` darf nicht als `Valid` interpretiert werden.

## Zyklen

Relation Graphs dürfen grundsätzlich Zyklen enthalten.

```text
A → B → C → A
```

Algorithmen zur Traversierung müssen deshalb Zyklenerkennung und Begrenzungen unterstützen.

Eine Relation darf keine unbegrenzte Rekursion verursachen.

## Transaktionen

Änderungen an Relations müssen mit NovaOS-Transaktionen integrierbar sein.

```text
Begin
 ↓
Create / Modify Relation
 ↓
Validate
 ↓
Commit
```

Objekt- und Relation-Änderungen dürfen innerhalb derselben Transaktion konsistent durchgeführt werden.

## Capability Integration

Kenntnis einer Relation gewährt keine Authority über deren Objekte.

```text
Relation Query
      ↓
ObjectID
      ↓
Capability Check
      ↓
Authorized Handle
```

Es gilt:

```text
Related ≠ Accessible
Relation Visible ≠ Target Visible
Know ObjectID ≠ Authority
```

Relation Queries müssen Security-, Privacy- und Namespace-Regeln berücksichtigen.

## Datenschutz

Auch Beziehungen selbst können sensible Informationen offenlegen.

Beispiel:

```text
Document
   ↓ belongs-to
Confidential Project
```

Deshalb müssen Relations:

```text
Access Controlled
Filtered
Protected
Encrypted
```

werden können.

## Indexierung

NovaOS darf Relation-Indizes verwenden:

```text
SourceObjectID → Relations
TargetObjectID → Reverse Relations
RelationTypeID → Matching Relations
```

Indizes sind abgeleitete Daten.

```text
Relation Index ≠ Authoritative Relation State
```

Sie müssen invalidierbar und rekonstruierbar sein.

## Provenance

NovaOS soll nachvollziehen können:

```text
RelationID
RelationTypeID
SourceObjectID
TargetObjectID
Creator
Creation Time
Changes
Version
ProvenanceID
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
RelationID
RelationTypeID
SourceObjectID
TargetObjectID
Direction
Version
State
Metadata
Provenance
```

Introspection darf keine zusätzliche Authority auf Source oder Target übertragen.

## Normative Anforderungen

1. NovaOS MUSS persistente Relations zwischen Filesystem-Objekten unterstützen können.
2. Relations MÜSSEN über stabile `RelationID`s identifizierbar sein.
3. Source und Target MÜSSEN über stabile `ObjectID`s referenzierbar sein.
4. Pfade DÜRFEN NICHT die primäre Identität einer Relation bilden.
5. Relations MÜSSEN Rename und Move überleben können.
6. Relations SOLLEN Volume-Migration überleben können.
7. Relation Types MÜSSEN über stabile `RelationTypeID`s identifizierbar sein.
8. Gerichtete und symmetrische Relations MÜSSEN unterstützt werden können.
9. Ein Objekt MUSS mehrere Relations besitzen können.
10. Relations MÜSSEN einen Graphen unabhängig von der Directory-Hierarchie bilden können.
11. Directory-Hierarchie und semantische Relations MÜSSEN getrennte Konzepte bleiben.
12. Relations MÜSSEN für Filesystem-Projections verwendbar sein.
13. Relation Projections DÜRFEN keine unnötigen Objektkopien erzeugen.
14. Relations MÜSSEN semantisch abfragbar sein können.
15. Reverse Relation Queries SOLLEN unterstützt werden.
16. Relation Metadata MUSS von Object Metadata unterscheidbar sein.
17. NovaFile MUSS Relations integrieren können.
18. Relations MÜSSEN versionierbar sein.
19. Retargeting MUSS als explizite Relation-Änderung behandelt werden.
20. Gelöschte Targets DÜRFEN NICHT stillschweigend durch andere Objekte ersetzt werden.
21. Ungültige oder nicht auflösbare Relations MÜSSEN explizite Zustände besitzen.
22. `Unknown` DARF NICHT als `Valid` interpretiert werden.
23. Relation Traversal MUSS Zyklen sicher behandeln können.
24. Relation-Änderungen MÜSSEN transaktional integrierbar sein.
25. Relation-Sichtbarkeit DARF KEINE Capability verleihen.
26. Relation Queries MÜSSEN Security- und Privacy-Regeln berücksichtigen.
27. Relation-Indizes MÜSSEN als abgeleitete Daten behandelt werden.
28. Relation-Indizes MÜSSEN rekonstruierbar und invalidierbar sein.
29. Relations MÜSSEN nachvollziehbare Provenance besitzen können.
30. Relation-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-QUERY-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`

## Ergebnis

```text
             Filesystem Objects
                    ↓
                 ObjectID
                    ↓
              Relation Graph
          ┌─────────┼─────────┐
          ↓         ↓         ↓
      belongs-to references derived-from
          │         │         │
          └─────────┼─────────┘
                    ↓
           Semantic Queries
                    ↓
             Projections
                    ↓
            Namespace View
                    ↓
           Capability Check
                    ↓
          Authorized Access
```

NovaOS erhält damit neben der klassischen hierarchischen Verzeichnisstruktur einen persistenten graphbasierten Beziehungsraum. Dateien und andere Objekte können unabhängig von ihrem Speicherort miteinander verknüpft werden, wobei stabile `ObjectID`s dafür sorgen, dass diese Beziehungen auch bei Rename, Move, Projection und Volume-Migration erhalten bleiben.