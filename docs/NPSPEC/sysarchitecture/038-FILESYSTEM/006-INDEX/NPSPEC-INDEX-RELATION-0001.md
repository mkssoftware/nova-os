# NPSPEC-INDEX-RELATION-0001 – Nova Relation Index

## Status

Angenommen

## Kategorie

Index / Relations

## Zweck

NovaOS definiert einen rekonstruierbaren Index für schnelle Abfragen von Beziehungen zwischen Objekten.

Der Relation Index beschleunigt die Navigation im Beziehungsgraphen, ohne selbst die maßgebliche Quelle der Beziehungen zu sein.

## Grundprinzipien

```text
Relation Index ≠ Relation Source
RelationID ≠ ObjectID
Indexed Relation ≠ Authority
Index Loss ≠ Relation Loss
Relation Visibility ≠ Target Access
```

## Indexmodell

Ein Eintrag kann enthalten:

```text
RelationIndexEntry
├── RelationID
├── RelationTypeID
├── SourceObjectID
├── TargetObjectID
├── Direction
├── State
└── Version
```

Optional können für die Suche relevante Metadaten indexiert werden.

## Abfragen

Der Index unterstützt insbesondere:

```text
ObjectID → Outgoing Relations
ObjectID → Incoming Relations
RelationTypeID → Relations
SourceObjectID + RelationTypeID → Targets
TargetObjectID + RelationTypeID → Sources
```

Damit können Beziehungen effizient in beide Richtungen untersucht werden.

## Reverse Index

Gerichtete Beziehungen sollen automatisch rückwärts auffindbar sein.

```text
Object A
   │
   └── references → Object B

Index:

A → B
B ← A
```

Hierfür muss keine zusätzliche Relation erzeugt werden.

## Graph-Abfragen

Der Relation Index darf mehrstufige Traversierungen unterstützen:

```text
Object
  ↓
Relations
  ↓
Related Objects
  ↓
Relations
  ↓
Further Objects
```

Zyklen müssen erkannt und Traversierungen durch Tiefe, Ressourcenbudget oder Policy begrenzbar sein.

## Projektionen und Suche

Relationen können Grundlage dynamischer Projektionen und Suchabfragen sein:

```text
Relation Query
      ↓
Relation Index
      ↓
ObjectIDs
      ↓
Projection / Search Result
```

Dabei entstehen keine Objektkopien.

## Aktualisierung

Änderungen am maßgeblichen Relationsmodell müssen den Index aktualisieren können:

```text
Create Relation
Update Relation
Remove Relation
Object Removal
Relation State Change
```

Asynchrone Aktualisierung ist zulässig, sofern veraltete Einträge erkannt oder validiert werden können.

## Rekonstruktion

Der Relation Index muss vollständig aus den maßgeblichen Relationsdaten rekonstruierbar sein.

```text
Relations
   ↓
Reindex
   ↓
Relation Index
```

Ein Verlust des Index darf keine Beziehungen zwischen Objekten zerstören.

## Sicherheit

Das Auffinden einer Relation erzeugt keine Authority auf Source- oder Target-Objekte.

```text
Relation Result
      ↓
ObjectID
      ↓
Capability Check
      ↓
Authorized Handle
```

Nicht sichtbare Beziehungen dürfen keine geschützten Informationen offenlegen.

## Normative Anforderungen

1. Der Relation Index MUSS als abgeleitete Datenstruktur behandelt werden.
2. Beziehungen MÜSSEN über stabile `ObjectID`s indexierbar sein.
3. `RelationTypeID` MUSS als Suchkriterium verwendbar sein.
4. Eingehende und ausgehende Beziehungen MÜSSEN effizient auffindbar sein.
5. Reverse Lookups DÜRFEN keine zusätzliche Relation erzeugen müssen.
6. Graph-Traversierungen MÜSSEN gegen Zyklen und unbegrenzte Ausführung geschützt sein.
7. Relationsänderungen MÜSSEN Indexaktualisierungen auslösen können.
8. Veraltete Indexeinträge MÜSSEN erkennbar oder validierbar sein.
9. Der Index MUSS rekonstruierbar sein.
10. Indexverlust DARF keine maßgeblichen Relationsdaten verändern.
11. Ergebnisse MÜSSEN den aktuellen Sicherheitskontext berücksichtigen.
12. Der Relation Index DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

NovaOS erhält einen schnellen und rekonstruierbaren Relation Index für gerichtete und bidirektionale Abfragen im Objektgraphen. Beziehungen können effizient durchsucht, traversiert und für Projektionen verwendet werden, ohne das maßgebliche Relationsmodell oder die Capability-Sicherheitsgrenzen zu ersetzen.