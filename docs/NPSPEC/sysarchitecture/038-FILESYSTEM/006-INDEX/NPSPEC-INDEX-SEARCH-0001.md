# NPSPEC-INDEX-SEARCH-0001 – Nova Search Index

## Status

Angenommen

## Kategorie

Index / Search

## Zweck

NovaOS definiert einen systemweiten Suchindex für schnelle Suche über Dateien, NovaFiles, Metadaten, semantische Typen und Beziehungen.

Der Index ist eine abgeleitete Datenstruktur und niemals die maßgebliche Quelle eines Objekts.

## Grundprinzipien

```text
Index ≠ Source of Truth
Search Result ≠ Authority
Indexed Object ≠ Copied Object
Search Visibility ≠ Access Permission
Index Loss ≠ Data Loss
```

Der Index muss vollständig rekonstruierbar sein.

## Indexmodell

Ein Indexeintrag kann enthalten:

```text
IndexEntry
├── ObjectID
├── Name
├── SemanticTypeID
├── Metadata
├── Relationships
├── ContentIndex
└── IndexVersion
```

Nicht jedes Objekt muss vollständig indexiert werden.

Indexierungsumfang und Inhalt richten sich nach Datentyp, Policy und Berechtigungen.

## Suche

```text
Search Query
     ↓
Query Processing
     ↓
Search Index
     ↓
Candidate ObjectIDs
     ↓
Permission Filter
     ↓
Search Results
```

Suchergebnisse sollen bevorzugt über stabile `ObjectID`s auf die tatsächlichen Ressourcen verweisen.

## Sucharten

Der Index kann unterstützen:

```text
Name Search
Full-Text Search
Metadata Search
Semantic Search
Relationship Search
Type Search
Combined Queries
```

Sucharten dürfen miteinander kombiniert werden.

## Semantische Suche

NovaOS kann semantische Informationen verwenden:

```text
SemanticTypeID
Metadata
Relations
Content Properties
```

Dadurch kann beispielsweise nach Dokumenten, Bildern, Projekten oder miteinander verbundenen Objekten gesucht werden, ohne ausschließlich Pfade oder Dateiendungen auszuwerten.

## Aktualisierung

Änderungen an Objekten müssen eine Aktualisierung des Index auslösen können.

```text
Object Change
     ↓
Index Event
     ↓
Update / Remove Entry
```

Indexaktualisierung darf asynchron erfolgen.

NovaOS muss daher zwischen aktuellem Objektzustand und möglicherweise veraltetem Indexzustand unterscheiden können.

## Rekonstruktion

Der Suchindex muss aus den maßgeblichen Objekten, Metadaten und Beziehungen neu aufgebaut werden können.

```text
Index Lost
    ↓
Scan / Discovery
    ↓
Rebuild
    ↓
Search Available
```

Ein beschädigter Index darf keine Beschädigung der eigentlichen Benutzerdaten bedeuten.

## Sicherheit

Der Index darf keine Sicherheitsgrenzen umgehen.

Ein Benutzer oder Programm darf durch die Suche keine Informationen über Ressourcen erhalten, die im jeweiligen Sicherheitskontext nicht sichtbar sein dürfen.

Sensible Metadaten und Inhalte müssen entsprechend ihrer Berechtigungen behandelt werden.

## Normative Anforderungen

1. Der Suchindex MUSS als abgeleitete Datenstruktur behandelt werden.
2. Indexeinträge SOLLEN stabile `ObjectID`s referenzieren.
3. Der Index MUSS rekonstruierbar sein.
4. Indexverlust DARF keinen Verlust der Originaldaten verursachen.
5. Suchergebnisse MÜSSEN gegen den aktuellen Sicherheitskontext gefiltert werden.
6. Indexierung DARF keine zusätzliche Authority erzeugen.
7. Metadaten, semantische Typen und Beziehungen MÜSSEN indexierbar sein.
8. Volltextinhalte DÜRFEN abhängig von Policy und Berechtigung indexiert werden.
9. Objektänderungen MÜSSEN Indexaktualisierungen auslösen können.
10. Veraltete Indexeinträge MÜSSEN erkennbar oder validierbar sein.
11. Gelöschte Objekte DÜRFEN nicht dauerhaft als gültige Suchergebnisse erscheinen.
12. Ein beschädigter Index MUSS ohne Veränderung der Originalobjekte neu aufgebaut werden können.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-TEMP-CACHE-0001`

## Ergebnis

NovaOS erhält eine schnelle systemweite Suche, die Namen, Inhalte, Metadaten, semantische Typen und Beziehungen indexieren kann. Der Index bleibt vollständig von den eigentlichen Objekten getrennt, rekonstruierbar und an das Capability- und Berechtigungsmodell von NovaOS gebunden.