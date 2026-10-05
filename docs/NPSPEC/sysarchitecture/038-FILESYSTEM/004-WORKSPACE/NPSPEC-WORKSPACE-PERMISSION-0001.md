# NPSPEC-WORKSPACE-PERMISSION-0001 – Nova Workspace Permissions

## Status

Angenommen

## Kategorie

Workspace / Security / Permissions

## Zweck

NovaOS definiert Berechtigungen für Workspaces und deren Ressourcen.

Ein Workspace bildet einen Arbeitskontext, besitzt jedoch nicht automatisch Zugriff auf alle darin referenzierten Ressourcen.

## Grundprinzipien

```text
Workspace Membership ≠ Authority
Resource Reference ≠ Permission
Visibility ≠ Authority
Permission Requirement ≠ Granted Permission
Workspace Permission ⊆ Existing Authority
```

Workspace Permissions bauen auf dem allgemeinen Capability- und Userspace-Berechtigungsmodell auf.

## Berechtigungskontext

Jeder Workspace besitzt einen eigenen Sicherheitskontext:

```text
User Authority
      ↓
Workspace Permission Context
      ↓
Program / Solution / Task
```

Dieser Kontext begrenzt, welche Ressourcen und Capabilities innerhalb des Workspace verwendet werden dürfen.

## Ressourcen

Das Hinzufügen einer Ressource zu einem Workspace erzeugt keine neue Berechtigung.

```text
Workspace Relation
       ↓
ObjectID
       ↓
Permission Check
       ↓
Authorized Handle
```

Fehlt die erforderliche Authority, bleibt die Ressource nicht zugreifbar.

## Programme und Solutions

Programme und Solutions dürfen innerhalb eines Workspace unterschiedliche Berechtigungen besitzen.

```text
Workspace
├── Solution A → Read Object X
├── Solution B → Write Object X
└── Program C  → No Access
```

Die gemeinsame Nutzung desselben Workspace hebt diese Isolation nicht auf.

## Persistenz

Das Workspace Manifest darf benötigte Berechtigungen deklarieren oder referenzieren.

Der Workspace State darf jedoch keine aktive Authority als gewöhnliche persistente Daten speichern.

```text
Stored Requirement ≠ Granted Authority
```

Beim Wiederherstellen müssen Berechtigungen erneut validiert werden.

## Änderung und Widerruf

Workspace-Berechtigungen müssen während der Laufzeit eingeschränkt oder widerrufen werden können.

Ein Widerruf kann Auswirkungen auf:

```text
Authorized Handles
Solutions
Programs
Tasks
Projections
```

haben.

Betroffene Komponenten müssen den Verlust der Authority kontrolliert behandeln.

## Workspace-Freigabe

Wird ein Workspace mit einem anderen Benutzer oder Sicherheitskontext geteilt, werden dessen Berechtigungen nicht automatisch mitübertragen.

Jeder Sicherheitskontext benötigt eigene gültige Authority für die verwendeten Ressourcen.

## Kritische Ressourcen

Zugriffe auf besonders geschützte Bereiche wie:

```text
/System
/Boot
Raw Devices
Security Configuration
```

benötigen weiterhin explizite privilegierte Capabilities.

Ein Workspace darf diese Sicherheitsgrenzen nicht umgehen.

## Normative Anforderungen

1. Jeder Workspace MUSS einen eigenen Berechtigungskontext besitzen können.
2. Workspace-Mitgliedschaft DARF keine Authority erzeugen.
3. Ressourcenreferenzen DÜRFEN keine Berechtigung implizieren.
4. Programme und Solutions MÜSSEN innerhalb desselben Workspace unterschiedliche Berechtigungen besitzen können.
5. Workspace-Berechtigungen DÜRFEN bestehende Authority nicht ohne zusätzliche Autorisierung erweitern.
6. Berechtigungen MÜSSEN widerrufbar und einschränkbar sein.
7. Persistierter Workspace State DARF keine aktive Authority als gewöhnliche Daten speichern.
8. Beim Wiederherstellen MÜSSEN benötigte Berechtigungen erneut validiert werden.
9. Das Teilen eines Workspace DARF Berechtigungen nicht automatisch übertragen.
10. Workspace Permissions DÜRFEN globale Sicherheitsgrenzen nicht umgehen.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-WORKSPACE-STATE-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-WORKSPACE-RELATION-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

NovaOS isoliert die Berechtigungen jedes Workspace und seiner Programme, Solutions und Ressourcen. Workspace-Zugehörigkeit, Referenzen oder gespeicherter Zustand erzeugen keine Authority; jeder Zugriff bleibt an explizite und widerrufbare Berechtigungen gebunden.