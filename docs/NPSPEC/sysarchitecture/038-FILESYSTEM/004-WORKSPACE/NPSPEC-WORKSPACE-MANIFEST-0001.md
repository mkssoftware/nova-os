# NPSPEC-WORKSPACE-MANIFEST-0001 – Nova Workspace Manifest

## Status

Angenommen

## Kategorie

Workspace / Manifest

## Zweck

Jeder persistente NovaOS-Workspace besitzt ein Manifest, das seine Identität, Struktur und für die Wiederherstellung erforderlichen Referenzen beschreibt.

Das Manifest beschreibt den Workspace, enthält jedoch nicht zwingend die eigentlichen Nutzdaten.

## Grundprinzipien

```text
Manifest ≠ Workspace Data
Manifest ≠ Permission
Manifest ≠ Application State
Reference ≠ Copy
```

Das Manifest ist die deklarative Beschreibung des Workspace.

## Manifest-Modell

```text
WorkspaceManifest
├── ManifestVersion
├── WorkspaceID
├── Name
├── OwnerID
├── Resources
├── Solutions
├── Programs
├── UIState
├── Settings
└── Metadata
```

Optionale Bereiche dürfen ergänzt werden, solange unbekannte Erweiterungen kontrolliert behandelt werden können.

## Identität

`WorkspaceID` ist die stabile Identität des Workspace.

```text
WorkspaceID ≠ Workspace Name
WorkspaceID ≠ Manifest Path
```

Das Verschieben oder Umbenennen des Manifests darf die Workspace-Identität nicht verändern.

## Ressourcen

Ressourcen sollen über stabile Identitäten referenziert werden:

```text
Resource
├── ObjectID
├── Type
└── Optional Context
```

Pfade dürfen als Navigations- oder Fallback-Information enthalten sein, sind jedoch nicht die primäre Identität persistenter Ressourcen.

## Programme und Solutions

Das Manifest darf festhalten, welche Programme und Solutions zum Workspace gehören oder beim Wiederherstellen benötigt werden.

```text
Workspace
├── Solution References
└── Program References
```

Das Manifest enthält dabei Referenzen und keine unnötigen Kopien der jeweiligen Komponenten.

## UI-Zustand

Wiederherstellbare UI-Informationen dürfen enthalten sein, beispielsweise:

```text
geöffnete Ressourcen
Fensterzustände
Navigation
aktive Ansicht
Layout
```

Nicht wiederherstellbare oder veraltete Zustände müssen übersprungen werden können.

## Berechtigungen

Das Manifest darf benötigte Capabilities beschreiben oder referenzieren, speichert jedoch keine Authority als bloße Manifest-Eigenschaft.

```text
Manifest Requirement
        ↓
Permission Evaluation
        ↓
Authorized Capability
```

Das Laden eines Workspace darf keine Berechtigungsprüfung umgehen.

## Versionierung

Das Manifest MUSS eine Formatversion besitzen.

NovaOS soll ältere unterstützte Versionen migrieren können.

Unbekannte inkompatible Versionen dürfen nicht stillschweigend falsch interpretiert werden.

## Wiederherstellung

```text
Load Manifest
     ↓
Validate
     ↓
Resolve References
     ↓
Evaluate Permissions
     ↓
Restore Workspace
```

Fehlende Ressourcen dürfen den Workspace in einen partiell wiederherstellbaren Zustand versetzen.

## Normative Anforderungen

1. Persistente Workspaces MÜSSEN durch ein versioniertes Manifest beschreibbar sein.
2. Das Manifest MUSS die stabile `WorkspaceID` enthalten.
3. Ressourcen SOLLEN über stabile Identitäten referenziert werden.
4. Pfade DÜRFEN NICHT die alleinige Identität persistenter Ressourcen darstellen.
5. Das Manifest DARF keine Authority allein durch seinen Inhalt erzeugen.
6. Berechtigungen MÜSSEN beim Wiederherstellen erneut gültig sein.
7. Fehlende Ressourcen MÜSSEN kontrolliert behandelt werden.
8. Programme und Solutions DÜRFEN als Referenzen gespeichert werden.
9. UI-Zustand DARF für die Wiederherstellung gespeichert werden.
10. Inkompatible Manifest-Versionen MÜSSEN erkannt werden.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-USERSPACE-SETTINGS-0001`

## Ergebnis

Das Workspace Manifest bildet die portable und versionierbare Beschreibung eines NovaOS-Workspace. Es ermöglicht die zuverlässige Wiederherstellung von Ressourcen, Programmen, Solutions und UI-Zuständen, ohne Workspace-Identität, Daten und Berechtigungen miteinander zu vermischen.