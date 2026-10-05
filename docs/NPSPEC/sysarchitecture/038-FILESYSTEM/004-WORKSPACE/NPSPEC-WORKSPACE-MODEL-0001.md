# NPSPEC-WORKSPACE-MODEL-0001 – Nova Workspace Model

## Status

Angenommen

## Kategorie

Workspace / Core Model

## Zweck

NovaOS definiert einen Workspace als aufgabenbezogenen Arbeitskontext, der Ressourcen, Zustände, Oberflächen, Solutions, Programme und Capabilities logisch zusammenführt.

Ein Workspace ist keine Anwendung und kein physischer Ordner.

## Grundprinzipien

```text
Workspace ≠ Application
Workspace ≠ Solution
Workspace ≠ Directory
Workspace ≠ User Account
Workspace ≠ Permission Boundary
```

Ein Workspace beschreibt den Kontext, in dem eine Aufgabe bearbeitet wird.

## Workspace-Modell

```text
Workspace
├── WorkspaceID
├── OwnerID
├── Name
├── State
├── Resources
├── UI State
├── Solutions
├── Programs
└── Capability Context
```

`WorkspaceID` identifiziert den Workspace dauerhaft und unabhängig von seinem Namen oder seiner Darstellung.

## Ressourcen

Ein Workspace kann Referenzen auf unterschiedliche Ressourcen enthalten:

```text
Files
NovaFiles
Directories
Solutions
Programs
Devices
Remote Resources
Semantic Objects
```

Ressourcen werden bevorzugt über stabile Identitäten wie `ObjectID` referenziert.

Das Hinzufügen einer Ressource zum Workspace erzeugt keine Kopie.

## Arbeitskontext

Ein Workspace kann den aktuellen Arbeitszustand speichern, beispielsweise:

```text
geöffnete Ressourcen
Fenster und Oberflächen
aktive Solution
Programmsitzungen
Navigation
Auswahlzustände
Workspace Settings
```

Dadurch kann ein Arbeitskontext verlassen und später wiederhergestellt werden.

## Namespace

Ein Workspace darf eine eigene logische Namespace-Sicht besitzen.

```text
Global Namespace
      ↓
Workspace Projection
      ↓
Workspace View
```

Diese Sicht kann relevante Ressourcen zusammenführen, unabhängig davon, auf welchem Volume oder Speicherort sie liegen.

## Programme und Solutions

Mehrere Programme und Solutions dürfen gleichzeitig innerhalb desselben Workspace arbeiten.

Der Workspace koordiniert ihren gemeinsamen Kontext, ersetzt sie jedoch nicht.

## Berechtigungen

Die Mitgliedschaft einer Ressource in einem Workspace erzeugt keine zusätzliche Authority.

```text
Workspace Resource
       ↓
Capability Check
       ↓
Authorized Handle
```

Capabilities können auf den Workspace-Kontext begrenzt und beim Verlassen oder Schließen des Workspace widerrufen werden.

## Persistenz

Workspaces dürfen persistent oder temporär sein.

Persistente Workspaces können ihren Zustand speichern und nach Anmeldung, Neustart oder Gerätewechsel wiederherstellen, soweit die referenzierten Ressourcen verfügbar und weiterhin autorisiert sind.

## Normative Anforderungen

1. NovaOS MUSS Workspaces als eigenständige logische Arbeitskontexte behandeln.
2. Jeder persistente Workspace MUSS eine stabile `WorkspaceID` besitzen.
3. Ein Workspace DARF NICHT an einen einzelnen Ordner oder ein einzelnes Volume gebunden sein.
4. Ressourcen SOLLEN über stabile Identitäten referenziert werden.
5. Das Hinzufügen einer Ressource DARF keine unnötige Kopie erzeugen.
6. Ein Workspace MUSS mehrere Programme und Solutions enthalten können.
7. Workspaces DÜRFEN eigene Namespace-Projections besitzen.
8. Workspace-Mitgliedschaft DARF keine zusätzliche Authority erzeugen.
9. Workspace-spezifische Capabilities MÜSSEN begrenzbar und widerrufbar sein.
10. Persistente Workspaces SOLLEN ihren Arbeitszustand wiederherstellen können.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`

## Ergebnis

NovaOS erhält mit Workspaces einen persistenten oder temporären Arbeitskontext, der Ressourcen, Programme, Solutions und Oberflächen aufgabenbezogen zusammenführt, ohne deren Identität, Speicherort oder Berechtigungsmodell miteinander zu vermischen.