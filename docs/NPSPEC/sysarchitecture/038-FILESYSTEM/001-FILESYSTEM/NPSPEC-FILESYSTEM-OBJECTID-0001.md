# NPSPEC-FILESYSTEM-OBJECTID-0001 – Nova Filesystem Object Identity

## Status

Angenommen

## Kategorie

Filesystem / Object Identity / Namespace

## Zweck

NovaOS verwendet stabile `ObjectID`s zur eindeutigen Identifikation persistenter Filesystem-Objekte unabhängig von Pfad, Name, Projection, Volume oder physischem Speicherort.

```text
Object
  ↓
ObjectID
  ↓
Namespace / Projection / Handle
```

Der Pfad dient der Navigation. Die `ObjectID` definiert die logische Identität.

## Grundprinzipien

```text
ObjectID ≠ Path
ObjectID ≠ Filename
ObjectID ≠ ContentID
ObjectID ≠ VolumeID
ObjectID ≠ Handle
ObjectID ≠ Capability
ObjectID ≠ Physical Location
```

Insbesondere gilt:

```text
Rename ≠ New Object
Move ≠ New Object
Projection ≠ New Object
Content Change ≠ New Object
```

## ObjectID Model

Jedes persistente NovaOS-Filesystem-Objekt kann eine dauerhaft eindeutige `ObjectID` besitzen.

```text
FilesystemObject
├── ObjectID
├── ObjectType
├── Version
├── ContentID
├── Metadata
└── Provenance
```

Die `ObjectID` identifiziert das logische Objekt über dessen Lebenszeit.

## Pfadunabhängigkeit

Beispiel:

```text
/Benutzer/Dokumente/Bericht.nf
              ↓
         ObjectID: 42
```

Nach einem Move:

```text
/Solutions/Nova/Bericht.nf
              ↓
         ObjectID: 42
```

Die Identität bleibt unverändert.

## Projection Integration

Mehrere Projections können dieselbe `ObjectID` darstellen.

```text
/Benutzer/Bilder/a.nf ─────┐
                           │
/Solutions/X/Bilder/a.nf ──┼──→ ObjectID 42
                           │
/Daten/Bilder/a.nf ────────┘
```

Damit erkennt NovaOS, dass alle sichtbaren Einträge dasselbe zugrunde liegende Objekt darstellen.

## ObjectID und ContentID

`ObjectID` und `ContentID` erfüllen unterschiedliche Aufgaben.

```text
ObjectID
→ Welches logische Objekt?

ContentID
→ Welcher konkrete Inhalt?
```

Beispiel:

```text
ObjectID: 42

Version 1 → ContentID A
Version 2 → ContentID B
Version 3 → ContentID C
```

Eine Inhaltsänderung erzeugt daher nicht automatisch eine neue `ObjectID`.

## ObjectID und Version

Versionen gehören zu einem Objekt.

```text
ObjectID
├── Version 1
├── Version 2
└── Version 3
```

Rollback erzeugt keinen Identitätswechsel.

```text
Rollback
→ New State Version
→ Same ObjectID
```

## ObjectID und Volume

Die Identität darf nicht vom Volume abhängen.

```text
Volume A
   ↓
ObjectID 42
```

Nach kontrollierter Migration:

```text
Volume B
   ↓
ObjectID 42
```

Damit unterstützt NovaOS Location Transparency auch im Filesystem.

## Kopieren

Eine echte Kopie erzeugt grundsätzlich ein neues logisches Objekt.

```text
ObjectID 42
   ↓
Copy
   ↓
ObjectID 91
```

Die Inhalte können zunächst identisch sein:

```text
ObjectID 42 ──→ ContentID A
ObjectID 91 ──→ ContentID A
```

```text
Same ContentID
≠
Same ObjectID
```

Dadurch bleibt Content-Deduplication möglich, ohne Objektidentitäten zusammenzuführen.

## Export

Ein Export erzeugt ebenfalls eine eigenständige Ressource.

```text
ObjectID 42
   ↓
Export
   ↓
ObjectID 117
```

Eine Projection dagegen behält die ursprüngliche Identität.

```text
Projection → Same ObjectID
Export     → New ObjectID
```

## Hard References

Interne dauerhafte Beziehungen sollen bevorzugt `ObjectID`s statt Pfade verwenden.

```text
Object A
   ↓
ObjectID Reference
   ↓
Object B
```

Dadurch überleben Beziehungen:

```text
Rename
Move
Projection Change
Volume Migration
```

## Handles

Eine `ObjectID` ist kein geöffneter Zugriff.

```text
ObjectID
   ↓
Resolve
   ↓
Capability Check
   ↓
Handle
```

Ein Handle repräsentiert einen konkreten autorisierten Zugriff auf das Objekt.

```text
Know ObjectID
≠
Possess Handle
≠
Possess Authority
```

## Capability Integration

Kenntnis einer `ObjectID` gewährt keine Berechtigung.

```text
ObjectID
   ↓
Requested Operation
   ↓
Capability / Policy Check
   ↓
Authorized Handle
```

Die `ObjectID` ist ausschließlich Identität.

## Lebenszyklus

```text
Created
   ↓
Active
   ↓
Versioned
   ↓
Archived
   ↓
Deleted
```

Eine gelöschte `ObjectID` darf nicht stillschweigend einem neuen, unabhängigen Objekt zugewiesen werden.

```text
Deleted ObjectID
≠
Reusable Identity
```

Dadurch werden stale References und Identitätsverwechslungen vermieden.

## NovaFile Integration

NovaFile verwendet die `ObjectID` als Identität des logischen Containers.

```text
NovaFile
├── ObjectID
├── Payload
├── Metadata
├── Relationships
└── ContentID
```

Änderungen an Payload oder Metadaten dürfen neue Versionen erzeugen, ohne automatisch die `ObjectID` zu ändern.

## Beziehungen

Relationships sollen stabile Objektidentitäten verwenden.

```text
ObjectID A
    │
    ├── relates-to → ObjectID B
    └── derived-from → ObjectID C
```

Pfade können zusätzlich für Darstellung und Navigation gespeichert werden, sind jedoch nicht die maßgebliche Referenz.

## Transaktionen

Object-Erzeugung und Identitätszuweisung müssen transaktional integrierbar sein.

```text
Begin
 ↓
Allocate ObjectID
 ↓
Create Object
 ↓
Validate
 ↓
Commit
```

Eine nicht erfolgreich committed Objekterzeugung darf kein scheinbar existierendes Objekt hinterlassen.

## Wiederherstellung

Snapshots, Recovery und Rollback müssen vorhandene Objektidentitäten erhalten können.

Wird dagegen bewusst ein neues unabhängiges Objekt erzeugt, erhält dieses eine neue `ObjectID`.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
ObjectType
Current Version
ContentID
Known Projections
Volume Location
Lifecycle State
Relationships
Provenance
```

Die Abfrage einer `ObjectID` darf keine zusätzliche Authority erzeugen.

## Normative Anforderungen

1. NovaOS MUSS persistente Filesystem-Objekte über stabile `ObjectID`s identifizieren können.
2. `ObjectID` MUSS unabhängig von Dateiname und Pfad sein.
3. `ObjectID` MUSS von `ContentID`, `VolumeID`, Handle und Capability getrennt sein.
4. Rename DARF die `ObjectID` NICHT verändern.
5. Move DARF die `ObjectID` NICHT verändern.
6. Projection DARF die `ObjectID` NICHT verändern.
7. Inhaltsänderungen DÜRFEN die `ObjectID` NICHT automatisch verändern.
8. Objektversionen MÜSSEN derselben `ObjectID` zugeordnet werden können.
9. Mehrere Projections DÜRFEN dieselbe `ObjectID` referenzieren.
10. Eine echte Kopie MUSS grundsätzlich eine neue `ObjectID` erhalten.
11. Ein Export MUSS als neues unabhängiges Objekt eine neue `ObjectID` erhalten.
12. Gleiche `ContentID`s DÜRFEN NICHT automatisch gleiche `ObjectID`s bedeuten.
13. Object Migration zwischen Volumes SOLL die `ObjectID` erhalten können.
14. Dauerhafte interne Referenzen SOLLEN `ObjectID`s statt veränderlicher Pfade verwenden.
15. Relationships SOLLEN auf stabilen `ObjectID`s basieren.
16. Kenntnis einer `ObjectID` DARF KEINE Capability verleihen.
17. Zugriff auf ein Objekt MUSS separat autorisiert werden.
18. Autorisierte Zugriffe SOLLEN über Handles repräsentiert werden.
19. Gelöschte `ObjectID`s DÜRFEN NICHT stillschweigend für unabhängige neue Objekte wiederverwendet werden.
20. NovaFile MUSS seine logische Identität über eine stabile `ObjectID` darstellen können.
21. Snapshot und Rollback SOLLEN bestehende Objektidentitäten erhalten.
22. Object-Erzeugung und Identitätszuweisung MÜSSEN transaktional integrierbar sein.
23. ObjectID-basierte Referenzen MÜSSEN Rename und Move überleben können.
24. ObjectID-basierte Referenzen SOLLEN Volume-Migration überleben können.
25. Object Identity MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`

## Ergebnis

```text
                    ObjectID
                       │
          ┌────────────┼────────────┐
          ↓            ↓            ↓
       Version      Projection   Relationship
          ↓            ↓            ↓
      ContentID       Path       Other ObjectID
          │
          ↓
   Physical Storage

ObjectID
   ↓
Capability Check
   ↓
Authorized Handle
```

NovaOS erhält damit eine stabile, pfad- und speicherortunabhängige Objektidentität für das Filesystem. Dateien und andere persistente Ressourcen können verschoben, umbenannt, versioniert, auf andere Volumes migriert oder mehrfach projiziert werden, ohne ihre logische Identität zu verlieren.