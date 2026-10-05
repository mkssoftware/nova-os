# NPSPEC-INDEX-WORKSPACE-0001 – Nova Workspace Index

## Status

Angenommen

## Kategorie

Index / Workspace

## Zweck

NovaOS definiert einen rekonstruierbaren Index für Workspaces und deren zugeordnete Ressourcen.

Der Workspace Index beschleunigt Suche, Navigation und Wiederherstellung, ohne Workspace Manifest, Workspace State oder Relationsmodell als maßgebliche Quellen zu ersetzen.

## Grundprinzipien

```text
Workspace Index ≠ Workspace
Index Entry ≠ Manifest
Index Entry ≠ Workspace State
Workspace Membership ≠ Ownership
Indexed Resource ≠ Authority
Index Loss ≠ Workspace Loss
```

## Indexmodell

Ein Workspace-Eintrag kann enthalten:

```text
WorkspaceIndexEntry
├── WorkspaceID
├── OwnerID
├── Name
├── State
├── ResourceObjectIDs
├── SolutionIDs
├── ProgramIDs
└── IndexVersion
```

Der Index speichert ausschließlich für Suche und Auflösung benötigte Informationen.

## Ressourcenindex

Workspace-Ressourcen werden über stabile Identitäten referenziert:

```text
WorkspaceID
     ↓
Resource Index
     ↓
ObjectID
```

Ein Objekt darf gleichzeitig mehreren Workspaces zugeordnet sein.

Dadurch entsteht keine Kopie des Objekts.

## Suche

Der Index unterstützt insbesondere:

```text
WorkspaceID → Workspace
Name → Workspaces
ObjectID → Workspaces
SolutionID → Workspaces
ProgramID → Workspaces
OwnerID → Workspaces
```

Damit kann NovaOS beispielsweise ermitteln, in welchen Workspaces ein bestimmtes Objekt verwendet wird.

## Relationsintegration

Workspace-Zuordnungen können aus dem Workspace-Relationsmodell indexiert werden:

```text
Workspace Relations
       ↓
Workspace Index
       ↓
Fast Lookup
```

Der Relation Index kann ergänzend für komplexere Graph-Abfragen verwendet werden.

## Wiederherstellung

Der Workspace Index darf die schnelle Auffindung vorhandener Workspaces unterstützen.

Die eigentliche Wiederherstellung erfolgt weiterhin aus:

```text
Workspace Manifest
Workspace State
UI State
Resource Relations
```

Ein Indexeintrag allein reicht nicht zur Wiederherstellung eines Workspaces.

## Aktualisierung

Folgende Änderungen müssen Indexaktualisierungen auslösen können:

```text
Workspace Create
Workspace Remove
Rename
Resource Add / Remove
Solution Attach / Detach
Program Attach / Detach
Owner Change
State Change
```

Asynchrone Aktualisierung ist zulässig, sofern veraltete Einträge erkannt oder validiert werden können.

## Rekonstruktion

Der Workspace Index muss aus den maßgeblichen Workspace-Daten rekonstruierbar sein.

```text
Workspace Data
      ↓
Reindex
      ↓
Workspace Index
```

Indexverlust darf keine Workspace-Daten verändern.

## Sicherheit

Das Auffinden eines Workspaces oder einer Ressourcenreferenz erzeugt keine Authority.

```text
Index Result
     ↓
WorkspaceID / ObjectID
     ↓
Permission Check
     ↓
Authorized Access
```

Nicht sichtbare Workspaces dürfen durch den Index nicht offengelegt werden.

## Normative Anforderungen

1. Der Workspace Index MUSS als abgeleitete Datenstruktur behandelt werden.
2. Workspaces MÜSSEN über stabile `WorkspaceID`s indexierbar sein.
3. Workspace-Ressourcen SOLLEN über stabile `ObjectID`s referenziert werden.
4. Ein Objekt DARF mehreren Workspaces zugeordnet sein.
5. Workspace-Zuordnungen DÜRFEN keine Objektkopien erzeugen.
6. Manifest, State und Relationsmodell MÜSSEN maßgebliche Quellen bleiben.
7. Workspace-Änderungen MÜSSEN Indexaktualisierungen auslösen können.
8. Veraltete Einträge MÜSSEN erkennbar oder validierbar sein.
9. Der Index MUSS rekonstruierbar sein.
10. Indexverlust DARF keinen Workspace oder dessen Daten zerstören.
11. Suchergebnisse MÜSSEN den aktuellen Sicherheitskontext berücksichtigen.
12. Der Workspace Index DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-RELATION-0001`
- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-WORKSPACE-STATE-0001`
- `NPSPEC-WORKSPACE-RELATION-0001`
- `NPSPEC-WORKSPACE-PERMISSION-0001`

## Ergebnis

NovaOS erhält einen schnellen und rekonstruierbaren Workspace Index. Workspaces, Ressourcen, Solutions und Programme können effizient gefunden und miteinander aufgelöst werden, während Manifest, State und Relationsmodell maßgeblich bleiben und sämtliche Zugriffe weiterhin dem NovaOS-Berechtigungsmodell unterliegen.