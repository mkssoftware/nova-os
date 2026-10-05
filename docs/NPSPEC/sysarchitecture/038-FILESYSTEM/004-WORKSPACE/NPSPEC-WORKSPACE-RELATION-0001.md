# NPSPEC-WORKSPACE-RELATION-0001 – Nova Workspace Relations

## Status

Angenommen

## Kategorie

Workspace / Relations

## Zweck

NovaOS definiert Beziehungen zwischen einem Workspace und den Ressourcen, die zu seinem Arbeitskontext gehören.

Workspace Relations verbinden Ressourcen logisch mit einem Workspace, ohne deren Speicherort, Besitz oder Berechtigungen zu verändern.

## Grundprinzipien

```text
Relation ≠ Copy
Relation ≠ Ownership
Relation ≠ Permission
Relation ≠ Directory Membership
Relation ≠ Physical Location
```

Ein Objekt kann mit mehreren Workspaces verbunden sein.

## Relationsmodell

Workspace-Beziehungen verwenden das allgemeine Filesystem-Relationsmodell:

```text
WorkspaceID
    ↓
Relation
    ↓
ObjectID
```

Mögliche Beziehungstypen sind beispielsweise:

```text
contains
references
uses
created-from
related-to
output-of
```

Workspace-spezifische Relation Types dürfen ergänzt werden.

## Ressourcen

Ein Workspace kann Beziehungen zu unterschiedlichen Ressourcen besitzen:

```text
Files
Directories
NovaFiles
Solutions
Programs
Semantic Objects
Remote Resources
```

Die Beziehungen referenzieren stabile Identitäten und keine zwingenden physischen Pfade.

## Mehrfachzuordnung

Dasselbe Objekt darf mehreren Workspaces zugeordnet sein:

```text
Workspace A ─┐
             ├── ObjectID
Workspace B ─┘
```

Dadurch entstehen keine Kopien des Objekts.

Änderungen am eigentlichen Objekt bleiben Änderungen desselben logischen Objekts.

## Semantische Nutzung

Workspace Relations können für:

```text
Navigation
Discovery
Projections
Semantic Queries
Workspace Restore
Context Generation
```

verwendet werden.

Sie bilden damit den logischen Zusammenhang zwischen den Ressourcen eines Arbeitskontexts.

## Persistenz

Persistente Workspace Relations müssen unabhängig von aktuellen Pfaden erhalten bleiben.

Rename, Move, Projection oder Storage-Migration eines Zielobjekts dürfen die Relation nicht automatisch zerstören.

Wird ein Zielobjekt entfernt oder ist nicht verfügbar, wird die Relation als nicht auflösbar behandelt und nicht automatisch auf ein anderes Objekt umgebunden.

## Sicherheit

Eine Workspace Relation erzeugt keine Authority über das Zielobjekt.

```text
Workspace Relation
       ↓
ObjectID
       ↓
Capability Check
       ↓
Authorized Handle
```

Auch beim Wiederherstellen eines Workspace müssen die erforderlichen Berechtigungen weiterhin gültig sein.

## Normative Anforderungen

1. Workspace Relations MÜSSEN stabile Identitäten referenzieren können.
2. Beziehungen DÜRFEN NICHT von aktuellen Dateipfaden abhängig sein.
3. Ein Objekt DARF mit mehreren Workspaces verbunden sein.
4. Eine Relation DARF keine Kopie des Zielobjekts erzeugen.
5. Workspace Relations DÜRFEN keine Ownership implizieren.
6. Workspace Relations DÜRFEN keine zusätzliche Authority erzeugen.
7. Rename, Move oder Storage-Migration DÜRFEN persistente Relations nicht automatisch zerstören.
8. Nicht verfügbare Ziele MÜSSEN kontrolliert als nicht auflösbar behandelt werden.
9. Relations MÜSSEN für Workspace-Projections und Wiederherstellung nutzbar sein.
10. Änderungen an mehreren zusammengehörigen Relations SOLLEN transaktional erfolgen.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS kann Ressourcen über stabile Beziehungen zu Workspaces zusammenführen. Dadurch bleiben Arbeitszusammenhänge unabhängig von Pfaden und Speicherorten erhalten, ohne Daten zu duplizieren oder durch die Beziehung zusätzliche Berechtigungen zu erzeugen.