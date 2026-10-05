# NPSPEC-INDEX-NAMESPACE-0001 – Nova Namespace Index

## Status

Angenommen

## Kategorie

Index / Namespace

## Zweck

NovaOS definiert einen rekonstruierbaren Index für schnelle Auflösung und Suche innerhalb des Filesystem Namespace.

Der Namespace Index beschleunigt die Zuordnung von Namen, Pfaden, Projektionen und Kontexten zu stabilen Objektidentitäten, ohne selbst die maßgebliche Namespace-Struktur zu bilden.

## Grundprinzipien

```text
Namespace Index ≠ Namespace
Path ≠ ObjectID
Index Entry ≠ Object
Projection ≠ Copy
Index Loss ≠ Namespace Loss
Index Hit ≠ Authority
```

## Indexmodell

Ein Eintrag kann enthalten:

```text
NamespaceIndexEntry
├── NamespaceID
├── ParentID
├── Name
├── ObjectID
├── EntryType
├── Scope
├── ProjectionID
└── Version
```

Nicht jeder Eintrag muss alle optionalen Felder besitzen.

## Auflösung

Der Index unterstützt insbesondere:

```text
Path → ObjectID
Name → Candidate ObjectIDs
Parent + Name → ObjectID
Projection → ObjectID
ObjectID → Namespace Entries
```

Die endgültige Auflösung bleibt Aufgabe des Namespace-Subsystems.

```text
Path
 ↓
Namespace Index
 ↓
Candidate Entry
 ↓
Namespace Validation
 ↓
ObjectID
 ↓
Capability Check
 ↓
Authorized Handle
```

## Kontextabhängige Namespaces

Der Index muss unterschiedliche Namespace-Sichten berücksichtigen können:

```text
Global
User
Process
Program
Solution
Workspace
Recovery
```

Ein identischer Pfad kann abhängig vom Kontext unterschiedlich aufgelöst werden.

Programmspezifische `SYS`-Overlays müssen daher getrennt vom globalen `/System` indexierbar sein.

## Projektionen

Projizierte Namespace-Einträge können indexiert werden.

Mehrere Pfade dürfen auf dieselbe `ObjectID` zeigen:

```text
Path A ─┐
Path B ─┼─→ ObjectID
Path C ─┘
```

Dies erzeugt keine Objektkopien.

## Aktualisierung

Namespace-Änderungen müssen Indexaktualisierungen auslösen können:

```text
Create
Rename
Move
Delete
Mount
Unmount
Projection Change
Overlay Change
```

Die Aktualisierung darf asynchron erfolgen, solange veraltete Einträge erkannt und vor endgültiger Verwendung validiert werden.

## Rekonstruktion

Der Namespace Index muss vollständig aus der maßgeblichen Namespace-Struktur rekonstruierbar sein.

Ein beschädigter oder verlorener Index darf keine Namespace-Einträge oder Objekte verändern.

## Sicherheit

Der Index darf keine versteckten Namespace-Einträge oder Objektidentitäten an nicht autorisierte Kontexte offenlegen.

```text
Indexed ≠ Visible
Visible ≠ Authorized
```

Namespace-Suche und Auflösung müssen den aktuellen Sicherheitskontext berücksichtigen.

## Normative Anforderungen

1. Der Namespace Index MUSS als abgeleitete Datenstruktur behandelt werden.
2. Indexeinträge SOLLEN stabile `ObjectID`s referenzieren.
3. Kontextabhängige Namespace-Sichten MÜSSEN unterscheidbar sein.
4. Mehrere Namespace-Einträge DÜRFEN dieselbe `ObjectID` referenzieren.
5. Projektionen und Overlays MÜSSEN indexierbar sein.
6. Namespace-Änderungen MÜSSEN Indexaktualisierungen auslösen können.
7. Veraltete Einträge MÜSSEN erkannt oder validiert werden können.
8. Der Index MUSS rekonstruierbar sein.
9. Indexverlust DARF den maßgeblichen Namespace nicht verändern.
10. Such- und Auflösungsergebnisse MÜSSEN den Sicherheitskontext berücksichtigen.
11. Der Index DARF keine zusätzliche Authority erzeugen.
12. Ein Indexeintrag DARF nicht als alleiniger Beweis für die aktuelle Namespace-Zuordnung gelten.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`

## Ergebnis

NovaOS erhält einen schnellen, kontextabhängigen Namespace Index für Pfade, Namen, Projektionen und Overlays. Der Index beschleunigt die Auflösung zu stabilen Objektidentitäten, bleibt jedoch vollständig rekonstruierbar und dem maßgeblichen Namespace- und Capability-Modell untergeordnet.