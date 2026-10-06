# NPSPEC-FILESYSTEM-SEMANTIC-0001 – Nova Filesystem Semantic Model

## Status

Angenommen

## Kategorie

Filesystem / Semantic / Metadata

## Zweck

NovaOS erweitert das klassische hierarchische Dateisystem um eine semantische Ebene.

Filesystem-Objekte werden nicht ausschließlich durch Dateiname, Endung oder Pfad beschrieben, sondern besitzen maschinenlesbare Informationen über ihre Bedeutung, Eigenschaften und Beziehungen.

```text
ObjectID
   ↓
Semantic Information
   ↓
Meaning
   ↓
Discovery / Projection / Processing
```

Der Namespace bleibt für Navigation erhalten, ist jedoch nicht die einzige Möglichkeit, Ressourcen zu organisieren oder zu finden.

## Grundprinzipien

```text
Semantic Type ≠ File Extension
Semantic Type ≠ MIME Type
Semantic Type ≠ Path
Semantic Type ≠ Application Association
Metadata ≠ Identity
Relationship ≠ Path
Semantic Knowledge ≠ Authority
```

## Semantic Object Model

Ein Filesystem-Objekt kann semantische Informationen besitzen:

```text
FilesystemObject
├── ObjectID
├── SemanticTypeID
├── Metadata
├── Relationships
├── ContentID
└── Provenance
```

Die `ObjectID` bleibt die Identität des Objekts.

Der `SemanticTypeID` beschreibt dessen Bedeutung.

## Semantic Type

Beispiele:

```text
image
document
audio
video
source-code
dataset
configuration
solution
model
archive
```

Semantische Typen dürfen spezialisiert werden:

```text
image
└── photo
    └── raw-photo
```

Die konkrete Identität eines semantischen Typs muss über eine stabile `SemanticTypeID` erfolgen.

## Dateiendungen

Dateiendungen bleiben für Benutzer, Kompatibilität und Import/Export nutzbar.

```text
.jpg
.png
.pdf
.nf
.nui
.nlf
```

Sie bestimmen jedoch nicht allein die Semantik.

```text
Extension
    ↓
Possible Type Hint

Content + Metadata
    ↓
Actual Semantic Type
```

Eine Endung darf daher als Hinweis dienen, aber nicht als alleinige Wahrheitsquelle vorausgesetzt werden.

## Metadaten

Semantische Metadaten können beispielsweise enthalten:

```text
Title
Author
Created
Modified
Language
Tags
Description
Dimensions
Duration
Location
Document Type
Project Association
Semantic Properties
```

Metadaten sollen typisiert und maschinenlesbar sein.

## Relationships

Objekte können explizite Beziehungen besitzen.

```text
Object A
   ├── belongs-to → Object B
   ├── derived-from → Object C
   ├── references → Object D
   └── related-to → Object E
```

Beziehungen sollen bevorzugt stabile `ObjectID`s verwenden.

Dadurch bleiben sie bei Rename, Move oder Projection erhalten.

## Semantische Suche

NovaOS darf Filesystem-Ressourcen anhand ihrer Bedeutung suchen.

```text
Query
 ↓
Semantic Type
Metadata
Relationships
 ↓
Matching ObjectIDs
```

Beispiele:

```text
alle Bilder

Dokumente von Projekt X

Dateien mit Bezug zu ObjectID Y

alle NovaLang-Quellen

Bilder aus 2026
```

Der physische Speicherort ist dafür nicht entscheidend.

## Semantische Projection

Semantische Abfragen können Namespace-Projections erzeugen.

```text
Semantic Query
      ↓
Matching ObjectIDs
      ↓
Projection Engine
      ↓
Filesystem View
```

Beispiel:

```text
SemanticType = image
        ↓
Daten/Bilder/
```

Die dort dargestellten Dateien können physisch auf unterschiedlichen Volumes liegen.

## Ein Objekt – mehrere Kontexte

Ein Objekt kann gleichzeitig mehreren logischen Zusammenhängen angehören.

```text
                    ObjectID 42
                   /     |     \
                  /      |      \
              Bilder   Projekt   2026
```

Es entstehen keine Kopien.

```text
Multiple Semantic Views
≠
Multiple Objects
```

## NovaFile Integration

NovaFile kann semantische Informationen direkt mit dem Objekt transportieren.

```text
NovaFile
├── ObjectID
├── Payload
├── Metadata
├── Relationships
└── Semantic Information
```

Dadurch können relevante Informationen gemeinsam mit dem Objekt erhalten bleiben.

Filesystem-Indizes dürfen daraus abgeleitete Suchstrukturen erzeugen.

```text
Embedded Metadata
        ↓
Semantic Index
        ↓
Fast Query
```

Der Index ist jedoch nicht die maßgebliche Quelle der Objektidentität.

## Semantic Discovery

Programme und Solutions können Ressourcen nach benötigter Bedeutung statt nach festen Pfaden suchen.

```text
Need:
"image"

        ↓

Semantic Discovery

        ↓

ObjectID(s)
```

Dadurch müssen Programme nicht voraussetzen, dass bestimmte Inhalte in fest definierten Verzeichnissen liegen.

## Semantic Processing

Semantische Typen können mit passenden Capabilities verbunden werden.

```text
Object
 ↓
SemanticTypeID
 ↓
Capability Discovery
 ↓
Compatible Provider
```

Beispiel:

```text
image
 ↓
Image Decode Capability
 ↓
Provider
```

Der semantische Typ bestimmt dabei nicht automatisch den konkreten Provider.

## Validierung

NovaOS darf prüfen, ob deklarierte Semantik und tatsächlicher Inhalt miteinander vereinbar sind.

```text
Declared Semantic Type
        +
Content
        +
Metadata
        ↓
Semantic Validation
```

Mögliche Zustände:

```text
Valid
Invalid
Partial
Unknown
```

`Unknown` darf nicht als `Valid` interpretiert werden.

## Änderungen

Ändert sich der Inhalt eines Objekts, müssen davon abhängige semantische Informationen invalidiert oder neu bewertet werden können.

```text
Content Changed
      ↓
Semantic Revalidation
      ↓
Metadata / Index Update
```

Die `ObjectID` bleibt dabei erhalten, solange weiterhin dasselbe logische Objekt vorliegt.

## Capability Integration

Semantische Informationen verleihen keine Berechtigung.

```text
Semantic Query
      ↓
ObjectID
      ↓
Capability Check
      ↓
Authorized Handle
```

Es gilt:

```text
Discoverable ≠ Accessible
Known Semantic Type ≠ Authority
Metadata Visibility ≠ Content Access
```

Suchergebnisse müssen den Sicherheits- und Sichtbarkeitsregeln des jeweiligen Kontexts folgen.

## Datenschutz

Semantische Metadaten können selbst sensible Informationen enthalten.

Daher müssen auch Metadaten:

```text
Protected
Filtered
Encrypted
Minimized
Access Controlled
```

werden können.

Eine verweigerte Content-Berechtigung darf nicht durch frei sichtbare Metadaten umgangen werden.

## Indexierung

NovaOS darf semantische Indizes zur Beschleunigung verwenden.

```text
Objects
 ↓
Semantic Index
 ↓
Query
```

Indizes sind abgeleitete Daten.

```text
Index ≠ Authoritative Object State
```

Änderungen an Objekten, Metadaten oder Beziehungen müssen eine Aktualisierung beziehungsweise Invalidierung relevanter Indexeinträge ermöglichen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
SemanticTypeID
Metadata Schema
Relationships
Validation State
Semantic Projections
Index State
Provenance
```

Introspection darf keine zusätzliche Authority erzeugen.

## Normative Anforderungen

1. NovaOS MUSS semantische Informationen für Filesystem-Objekte unterstützen können.
2. Semantik MUSS unabhängig von Pfad und physischem Speicherort sein.
3. Semantic Type und ObjectID MÜSSEN getrennte Konzepte sein.
4. Semantic Type und Dateiendung MÜSSEN getrennte Konzepte sein.
5. Dateiendungen DÜRFEN als Type Hint verwendet werden.
6. Dateiendungen DÜRFEN NICHT als alleinige semantische Wahrheitsquelle vorausgesetzt werden.
7. Semantische Typen MÜSSEN über stabile `SemanticTypeID`s identifizierbar sein.
8. Metadaten SOLLEN typisiert und maschinenlesbar sein.
9. Relationships SOLLEN stabile `ObjectID`s verwenden.
10. Relationships SOLLEN Rename und Move überleben.
11. Semantische Suche MUSS unabhängig von festen Pfaden möglich sein können.
12. Semantische Queries MÜSSEN als Grundlage für Filesystem-Projections verwendbar sein.
13. Mehrere semantische Projections DÜRFEN dasselbe Objekt darstellen.
14. Semantische Projections DÜRFEN keine unnötigen Kopien erzeugen.
15. NovaFile MUSS semantische Metadaten und Relationships integrieren können.
16. Programme SOLLEN Ressourcen nach semantischen Anforderungen entdecken können.
17. Semantic Discovery DARF KEINE Authority verleihen.
18. Semantic Types SOLLEN mit Capability Discovery kombinierbar sein.
19. Semantische Deklarationen MÜSSEN validierbar sein können.
20. `Unknown` DARF NICHT als semantisch `Valid` interpretiert werden.
21. Inhaltsänderungen MÜSSEN eine semantische Revalidierung auslösen können.
22. Semantische Metadaten MÜSSEN Sicherheits- und Datenschutzregeln unterliegen.
23. Suchergebnisse MÜSSEN den Namespace- und Capability-Regeln des jeweiligen Kontexts folgen.
24. Semantische Indizes MÜSSEN als abgeleitete Daten behandelt werden.
25. Semantische Indizes MÜSSEN invalidierbar beziehungsweise aktualisierbar sein.
26. Semantischer Zustand MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-QUERY-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-SEMANTIC-VALIDATION-0001`
- `NPSPEC-FSCAPABILITY-DISCOVERY-0001`

## Ergebnis

```text
Filesystem Object
       ↓
     ObjectID
       ↓
┌──────┼────────────┐
↓      ↓            ↓
Type Metadata   Relationships
└──────┼────────────┘
       ↓
 Semantic Model
       ↓
 ┌─────┼──────────┐
 ↓     ↓          ↓
Query Discovery Projection
       ↓
 Capability Check
       ↓
Authorized Access
```

NovaOS erhält damit ein semantisches Filesystem-Modell, bei dem Dateien und andere Objekte nicht nur anhand ihrer Position und ihres Namens, sondern anhand ihrer tatsächlichen Bedeutung, Metadaten und Beziehungen verstanden werden können. Die klassische Verzeichnisstruktur bleibt erhalten, wird jedoch durch semantische Suche, Discovery und dynamische Projections ergänzt.