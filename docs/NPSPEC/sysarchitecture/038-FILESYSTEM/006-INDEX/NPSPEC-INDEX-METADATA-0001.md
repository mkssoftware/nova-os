# NPSPEC-INDEX-METADATA-0001 – Nova Metadata Index

## Status

Angenommen

## Kategorie

Index / Metadata

## Zweck

NovaOS definiert einen systemweiten Index für strukturierte Metadaten von Dateien, NovaFiles und anderen indexierbaren Objekten.

Der Metadata Index beschleunigt Suche, Filterung, Sortierung, semantische Abfragen und dynamische Projektionen, ohne selbst zur maßgeblichen Quelle der Metadaten zu werden.

## Grundprinzipien

```text
Metadata Index ≠ Metadata Source
Index Entry ≠ Object
Indexed Metadata ≠ Authority
Index Loss ≠ Metadata Loss
Metadata Visibility ≠ Object Access
```

## Indexmodell

Ein Eintrag kann enthalten:

```text
MetadataIndexEntry
├── ObjectID
├── MetadataKey
├── MetadataType
├── IndexedValue
├── ObjectVersion
└── IndexVersion
```

Mehrere Metadatenfelder desselben Objekts können unabhängig indexiert werden.

## Unterstützte Metadaten

Indexierbar sind insbesondere:

```text
Name
SemanticTypeID
Tags
Creation Time
Modification Time
Author
Dimensions
Duration
Media Properties
Custom Metadata
Provenance References
```

Welche Metadaten tatsächlich indexiert werden, wird durch Typ, Policy und Sicherheitskontext bestimmt.

## Aktualisierung

```text
Metadata Change
      ↓
Index Event
      ↓
Update Entry
      ↓
Index Available
```

Die Aktualisierung darf asynchron erfolgen.

Der Index muss deshalb erkennen können, wenn ein Eintrag nicht mehr der aktuellen Objektversion entspricht.

## Abfragen

```text
Metadata Query
      ↓
Metadata Index
      ↓
Candidate ObjectIDs
      ↓
Permission Filter
      ↓
Result
```

Abfragen können mehrere Bedingungen kombinieren.

Beispiel:

```text
SemanticType = Image
AND
CreationYear = 2026
AND
Tag = NovaOS
```

## Semantische Integration

Der Metadata Index kann Grundlage für semantische Projektionen sein.

```text
Metadata Query
      ↓
ObjectIDs
      ↓
Projection
```

Dadurch können dynamische Ansichten entstehen, ohne Dateien zu kopieren oder physisch umzusortieren.

## Rekonstruktion

Der Index muss aus den maßgeblichen Metadaten der Objekte rekonstruierbar sein.

```text
Metadata Source
      ↓
Reindex
      ↓
Metadata Index
```

Beschädigte oder verlorene Indexdaten dürfen die eigentlichen Metadaten nicht verändern.

## Sicherheit

Sensible Metadaten dürfen nur indexiert oder ausgegeben werden, wenn die jeweilige Policy dies erlaubt.

Eine Suchanfrage darf keine geschützten Metadaten oder die Existenz nicht sichtbarer Objekte offenlegen.

## Normative Anforderungen

1. Der Metadata Index MUSS als abgeleitete Datenstruktur behandelt werden.
2. Indexeinträge MÜSSEN auf stabile `ObjectID`s verweisen können.
3. Metadatenwerte MÜSSEN typisiert indexierbar sein.
4. Metadatenänderungen MÜSSEN eine Indexaktualisierung auslösen können.
5. Veraltete Einträge MÜSSEN erkennbar oder validierbar sein.
6. Der Index MUSS vollständig rekonstruierbar sein.
7. Indexverlust DARF keinen Verlust maßgeblicher Metadaten verursachen.
8. Metadata Queries MÜSSEN mehrere Kriterien kombinieren können.
9. Ergebnisse MÜSSEN den aktuellen Sicherheitskontext berücksichtigen.
10. Sensible Metadaten DÜRFEN Sicherheitsgrenzen nicht umgehen.
11. Der Metadata Index MUSS für semantische Projektionen nutzbar sein.
12. Indexierung DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

NovaOS erhält einen rekonstruierbaren Metadata Index für schnelle strukturierte Abfragen. Metadaten können unabhängig vom physischen Speicherort durchsucht, kombiniert und für dynamische Projektionen verwendet werden, ohne Objektidentität oder Sicherheitsmodell zu verändern.