# NPSPEC-FILESYSTEM-PROJECTION-0001 – Nova Filesystem Projection

## Status

Ersetzt

## Ersetzt durch

- `NPSPEC-FILESYSTEM-PROJECTION-0002`

## Hinweis

Diese Fassung bleibt als historische Grundlage erhalten. Maßgeblich für Implementierung und spätere Spezifikationen ist `NPSPEC-FILESYSTEM-PROJECTION-0002`, weil dort ProjectionID, Konfliktauflösung, Schreibverhalten, Authority-Trennung und die Beziehung zum globalen Namespace präzisiert sind.

## Kategorie

Filesystem / Namespace / Projection

## Zweck

NovaOS definiert Filesystem Projections als logische Sichten auf vorhandene Dateien, Objekte, Volumes oder andere Ressourcen.

Eine Projection verändert weder die Identität noch zwingend den physischen Speicherort einer Ressource.

```text
Resource
   ↓
Stable Identity
   ↓
Projection
   ↓
Namespace Entry
```

Damit können Ressourcen an einer für Benutzer, Programme, Solutions oder Systemkomponenten sinnvollen Stelle erscheinen, ohne kopiert oder physisch verschoben werden zu müssen.

## Grundprinzipien

```text
Projection ≠ Copy
Projection ≠ Object
Projection ≠ Physical Location
Projection ≠ Capability
Projection Path ≠ Object Identity
Multiple Projections ≠ Multiple Objects
Visible ≠ Authorized
```

## Projection Model

```text
FilesystemProjection
├── ProjectionID
├── TargetID
├── NamespaceID
├── ProjectionPath
├── Scope
├── ProjectionType
└── State
```

Optional:

```text
OwnerID
SourceID
SemanticTypeID
MetadataQuery
RelationshipQuery
Priority
OverlayPolicy
AccessPolicy
ProvenanceID
```

## Zielressource

Eine Projection referenziert eine bestehende Ressource über eine stabile Identität.

```text
Projection
    ↓
TargetID
```

Der Target kann beispielsweise sein:

```text
ObjectID
VolumeID
DirectoryID
ResourceID
```

Der sichtbare Pfad ist nicht die Identität des Targets.

## Ein Objekt – mehrere Projektionen

Dieselbe Ressource darf an mehreren Stellen erscheinen.

```text
                ObjectID 42
                /        \
               /          \
/Benutzer/Bilder/a.nf   /Projekt/Bilder/a.nf
```

Beide Einträge referenzieren dasselbe Objekt.

Änderungen am Objekt sind daher grundsätzlich über alle Projektionen desselben Objekts sichtbar.

## Projection Types

NovaOS kann unterschiedliche Projection-Arten unterstützen:

```text
Direct
Overlay
Semantic
Relationship
Contextual
Virtual
```

### Direct

Direkte Darstellung einer Ressource an einer anderen Namespace-Position.

### Overlay

Kombination mehrerer Namespace-Quellen zu einer effektiven Sicht.

### Semantic

Erzeugung einer Sicht anhand semantischer Typen oder Metadaten.

### Relationship

Darstellung anhand expliziter Objektbeziehungen.

### Contextual

Nur innerhalb eines bestimmten User-, Process-, Program-, Solution- oder Workspace-Kontexts sichtbar.

### Virtual

Namespace-Eintrag ohne direkte physische Entsprechung als Verzeichnisstruktur.

## Semantische Projection

NovaOS kann Ressourcen anhand ihrer Bedeutung projizieren.

```text
Objects
   ↓
SemanticType + Metadata
   ↓
Projection Query
   ↓
Filesystem View
```

Beispiel:

```text
SemanticType = image
        ↓
/Benutzer/Bilder/
```

Der physische Speicherort muss dafür nicht `/Benutzer/Bilder/` entsprechen.

## Relationship Projection

Objektbeziehungen können ebenfalls eine Namespace-Sicht erzeugen.

```text
Project
  ├──relates-to→ Document
  ├──relates-to→ Image
  └──relates-to→ Dataset
```

kann projiziert werden als:

```text
/Projekt/
├── Dokument.nf
├── Bild.nf
└── Daten.nf
```

Die Projection erzeugt keine Duplikate dieser Objekte.

## Context Scope

Eine Projection besitzt einen definierten Scope.

```text
Global
User
Process
Program
Solution
Workspace
Recovery
```

Beispiel:

```text
Global Namespace
      +
Program Projection
      ↓
Effective Program Namespace
```

Eine programmspezifische Projection darf dadurch für andere Programme unsichtbar bleiben.

## Application SYS Projection

Private Programmabhängigkeiten werden über eine Projection in die effektive Systemansicht des Programms eingebunden.

```text
/Apps/Example/SYS/
        ↓
Projection
        ↓
Effective /System View
```

Physisch bleiben die Abhängigkeiten im Programmkontext.

```text
Private SYS Projection
≠
Modification of Global /System
```

Damit kann ein Programm eigene Bibliotheken oder Runtime-Abhängigkeiten verwenden, ohne globale Systemdateien ersetzen zu müssen.

## Overlay Resolution

Überlagern sich mehrere Projection Sources, muss die Auflösung deterministisch sein.

Beispiel:

```text
Private Program SYS
        ↓
Scoped Projection
        ↓
Global /System
```

Die jeweilige Policy bestimmt Priorität und Konfliktbehandlung.

Implizite, nicht reproduzierbare Auflösungsreihenfolgen sind unzulässig.

## Capability Integration

Eine Projection verändert keine Authority.

```text
Projection
    ↓
Visible Target
    ↓
Capability Check
    ↓
Authorized Operation
```

Es gilt:

```text
Can Project ≠ Can Access
Can See ≠ Can Read
Can Resolve ≠ Can Modify
```

Die Berechtigung wird gegen die zugrunde liegende Ressource beziehungsweise den autorisierten Handle geprüft.

## Handle Semantik

Nach erfolgreicher Auflösung:

```text
Projection Path
      ↓
TargetID
      ↓
Capability Check
      ↓
Authorized Handle
```

Der Handle referenziert das zugrunde liegende Objekt und nicht lediglich dessen Projection Path.

Wird eine Projection entfernt, muss ein bereits gültiger Handle deshalb nicht automatisch ungültig werden.

## Projection Lifecycle

```text
Declared
   ↓
Validated
   ↓
Active
   ↓
Updated
   ↓
Removed
```

Zusätzliche Zustände:

```text
Blocked
Invalid
Unavailable
Unknown
```

`Unknown` darf nicht als aktive gültige Projection interpretiert werden.

## Dynamische Projections

Projections dürfen zur Laufzeit erzeugt und entfernt werden.

Beispiele:

```text
Volume Mounted
Solution Opened
Program Started
Workspace Created
Device Connected
Recovery Started
```

Änderungen sollen transaktional im Namespace sichtbar werden.

## Volume Projection

Ein Volume kann an einer logischen Position erscheinen:

```text
VolumeID
   ↓
Projection / Mount
   ↓
/Volumes/Daten/
```

Der Mountpoint verändert die VolumeID nicht.

## NovaFile Integration

NovaFile-Container können unterschiedliche Sichten bereitstellen.

```text
NovaFile Object
├── Payload
├── Metadata
└── Relationships
```

Daraus können beispielsweise entstehen:

```text
Native Projection
Payload Projection
Metadata Projection
Semantic Projection
```

Alle Sichten müssen auf dieselbe zugrunde liegende Objektidentität zurückführbar bleiben.

```text
Projection ≠ Export
```

Ein echter Export erzeugt dagegen eine eigenständige Ressource mit eigener Identität.

## Konflikte

Mehrere Projections können denselben sichtbaren Namen beanspruchen.

NovaOS muss Konflikte deterministisch behandeln.

Mögliche Policies:

```text
Priority
Explicit Overlay
Reject
Namespace Separation
User Decision
```

Ein Konflikt darf nicht stillschweigend zu einer zufälligen Zielressource führen.

## Caching

Projection-Ergebnisse dürfen gecacht werden.

```text
Projection Definition
      ↓
Resolved View
      ↓
Cache
```

Cache-Einträge müssen invalidierbar sein, wenn sich relevante:

```text
Objects
Metadata
Relationships
Volumes
Policies
Projection Definitions
```

ändern.

```text
Cached Projection ≠ Authoritative State
```

## Transaktionen

Projection-Änderungen sollen mit NovaOS-Transaktionen integrierbar sein.

```text
Begin
 ↓
Prepare Projection
 ↓
Validate
 ↓
Commit
 ↓
Namespace Visible
```

Andere Komponenten dürfen keinen teilweise aufgebauten Namespace beobachten.

## Provenance

NovaOS soll nachvollziehen können:

```text
ProjectionID
TargetID
Projection Type
Scope
Owner
Source
Creation Reason
Active Policy
Namespace Path
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Active Projections
ProjectionID
TargetID
Projection Path
Projection Type
Scope
Owner
Overlay Priority
Resolution Source
State
```

Introspection darf keine zusätzliche Authority auf die projizierten Ressourcen übertragen.

## Normative Anforderungen

1. NovaOS MUSS Filesystem Projections unterstützen.
2. Jede Projection MUSS eindeutig identifizierbar sein.
3. Eine Projection MUSS auf eine stabile Zielidentität zurückführbar sein.
4. Projection Paths DÜRFEN NICHT als Objektidentität verwendet werden.
5. Projections DÜRFEN keine unnötigen Kopien erzeugen.
6. Mehrere Projections DÜRFEN dasselbe Objekt referenzieren.
7. Mehrere Projections desselben Objekts DÜRFEN NICHT als unterschiedliche Objekte interpretiert werden.
8. Direct-, Overlay-, Semantic-, Relationship-, Contextual- und Virtual-Projections MÜSSEN unterstützt werden können.
9. Projections MÜSSEN einen definierten Scope besitzen.
10. Program- und Solution-spezifische Projections MÜSSEN unterstützt werden können.
11. Private `SYS`-Abhängigkeiten MÜSSEN über Projections in die effektive Systemansicht eingebunden werden können.
12. Eine private `SYS`-Projection DARF das globale `/System` NICHT physisch verändern.
13. Overlay-Auflösung MUSS deterministisch sein.
14. Projection-Sichtbarkeit DARF KEINE Capability verleihen.
15. Zugriffe MÜSSEN gegen die zugrunde liegende Ressource autorisiert werden.
16. Autorisierte Handles SOLLEN unabhängig vom Projection Path bleiben.
17. Das Entfernen einer Projection DARF einen ansonsten gültigen Handle NICHT automatisch widerrufen.
18. Projections MÜSSEN dynamisch aktivierbar und entfernbar sein können.
19. Projection-Änderungen SOLLEN transaktional sichtbar werden.
20. Volume Projections DÜRFEN die VolumeID NICHT verändern.
21. NovaFile-Projections MÜSSEN auf dieselbe zugrunde liegende ObjectID zurückführbar bleiben.
22. Projection und Export MÜSSEN unterschiedliche Operationen sein.
23. Namespace-Konflikte MÜSSEN deterministisch behandelt werden.
24. Projection-Caches MÜSSEN invalidierbar sein.
25. `Unknown` DARF NICHT als gültige aktive Projection interpretiert werden.
26. Projection-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-OVERLAY-0001`
- `NPSPEC-STORAGE-APP-SYS-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROJECTION-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-CAPABILITY-APPLICATION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`

## Ergebnis

```text
Physical / Logical Resource
          ↓
      Stable ID
          ↓
    Projection Engine
     ┌────┼─────┬────────┐
     ↓    ↓     ↓        ↓
 Direct Overlay Semantic Contextual
     └────┴─────┴────────┘
          ↓
   Namespace Entries
          ↓
    Path Resolution
          ↓
    Capability Check
          ↓
   Authorized Handle
```

NovaOS erhält damit eine einheitliche Projection-Schicht, durch die dieselben Ressourcen in unterschiedlichen logischen, semantischen und kontextabhängigen Sichten erscheinen können, ohne ihre Identität zu verändern oder unnötige Kopien zu erzeugen. Namespace-Darstellung, physische Speicherung und Zugriffsberechtigung bleiben dabei strikt voneinander getrennt.
