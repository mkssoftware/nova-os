# NPSPEC-STORAGE-OBJECT-0001 – Nova Storage Object Model

## Status

Entwurf

## Kategorie

Storage / Object Model / Identity / Metadata

## Zweck

NovaOS definiert gespeicherte Daten primär als Objekte mit stabiler Identität, Typ, Metadaten und Beziehungen.

```text
Storage Object
├── ObjectID
├── Type
├── Data
├── Metadata
└── Relationships
```

Das Modell bildet die gemeinsame Grundlage für Dateien, Verzeichnisse, NovaFile-Container und weitere persistente Objekte.

## Grundprinzip

```text
Object Identity ≠ Name
Object Identity ≠ Path
Object Identity ≠ Physical Location
```

Ein Objekt bleibt dasselbe Objekt, wenn es umbenannt oder innerhalb des Storage-Namespace verschoben wird.

## ObjectID

Jedes persistente Nova-Objekt muss eine stabile `ObjectID` besitzen.

```text
ObjectID
   ↓
Storage Object
```

Die `ObjectID` darf nicht von Dateiname, Pfad, Volume-Bezeichnung oder physischem Speicherort abgeleitet sein.

## Objekttypen

Das Storage-Modell muss mindestens unterstützen können:

```text
File
Directory
Container
Stream
Link
System Object
```

Weitere semantische Typen dürfen ergänzt werden.

## Daten und Metadaten

Payload und Metadaten werden logisch getrennt betrachtet.

```text
Object
├── Payload
└── Metadata
```

Metadaten können unter anderem enthalten:

```text
Name
Semantic Type
Size
Timestamps
Permissions
Owner
Version
Relationships
Attributes
```

Das konkrete Dateisystem entscheidet über die physische Speicherung.

## Beziehungen

Objekte dürfen explizite Beziehungen zu anderen Objekten besitzen.

```text
Object A
   ↓ relationship
Object B
```

Beziehungen sollen stabile `ObjectID`s statt Pfade verwenden.

Dadurch bleiben sie nach Umbenennen oder Verschieben erhalten.

## Verzeichnisse

Ein Verzeichnis stellt eine strukturierte Zuordnung zwischen Namen und Objekten bereit.

```text
Directory
├── "Dokument" → ObjectID A
├── "Bild"     → ObjectID B
└── "Projekt"  → ObjectID C
```

Der Verzeichniseintrag ist nicht die Identität des enthaltenen Objekts.

## Move und Rename

Für native Nova-Objekte gilt:

```text
Rename → Same ObjectID
Move   → Same ObjectID
```

Eine Kopie erzeugt dagegen grundsätzlich ein neues Objekt.

```text
Copy
 ↓
New ObjectID
```

## Versionierung

Versionierte Storage-Objekte dürfen mehrere Zustände besitzen.

```text
ObjectID
├── Version 1
├── Version 2
└── Version 3
```

Dabei bleibt die Objektidentität erhalten, während einzelne Versionen eindeutig identifizierbar sein müssen.

## NovaFile

Das Objektmodell muss den NovaFile-Container unterstützen können.

```text
NovaFile
├── Payload
├── Metadata
├── Semantic Type
└── Relationships
```

Damit können Daten und zugehörige Metadaten als gemeinsame logische Einheit gespeichert werden.

## VFS-Projektion

Das VFS projiziert Storage-Objekte in einen für Anwendungen navigierbaren Namespace.

```text
Storage Objects
      ↓
     VFS
      ↓
Files / Directories / Paths
```

Legacy-Dateisysteme dürfen auf das Nova-Objektmodell abgebildet werden, auch wenn sie nicht alle Eigenschaften nativ unterstützen.

## Sicherheit

Berechtigungen müssen an das Objekt beziehungsweise dessen autorisierte Referenz gebunden sein und dürfen nicht allein vom aktuellen Pfad abhängen.

```text
ObjectID
   +
Capability
   ↓
Authorized Access
```

## Lifecycle

Ein Objekt besitzt einen definierten Lifecycle.

```text
Create
  ↓
Use
  ↓
Modify / Move / Version
  ↓
Delete
  ↓
Reclaim
```

Löschen eines Namenseintrags bedeutet nicht zwingend sofortige physische Freigabe, solange gültige Referenzen oder Versionen bestehen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
Type
Name
Volume
Metadata
Version
Relationships
References
Lifecycle State
```

## Normative Anforderungen

1. NovaOS MUSS persistente Storage-Objekte mit stabiler Identität unterstützen.
2. Eine `ObjectID` DARF NICHT von Name, Pfad oder physischer Position abhängen.
3. Rename und Move SOLLEN die Objektidentität nicht verändern.
4. Copy MUSS grundsätzlich eine neue Objektidentität erzeugen.
5. Payload und Metadaten MÜSSEN logisch getrennt modelliert werden.
6. Beziehungen zwischen Objekten SOLLEN stabile Objektidentitäten verwenden.
7. Verzeichnisse MÜSSEN als Zuordnung zwischen Namen und Objekten modellierbar sein.
8. Das Objektmodell MUSS Versionierung unterstützen können.
9. Berechtigungen DÜRFEN NICHT ausschließlich an Pfade gebunden sein.
10. Das VFS MUSS Storage-Objekte in einen navigierbaren Namespace projizieren können.
11. Legacy-Dateisysteme DÜRFEN eingeschränkte Projektionen des Objektmodells verwenden.
12. Identität, Typ, Version und Beziehungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0002`

## Ergebnis

```text
Stable ObjectID
      ↓
Storage Object
├── Payload
├── Metadata
├── Version
└── Relationships
      ↓
VFS Projection
      ↓
User-visible Namespace
```

NovaOS erhält damit ein pfadunabhängiges Storage-Objektmodell, bei dem Identität, Daten, Metadaten und Beziehungen unabhängig von der sichtbaren Dateisystemstruktur erhalten bleiben.