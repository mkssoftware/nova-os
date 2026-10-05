# NPSPEC-USERSPACE-FILES-0001 – Nova Userspace Files

## Status

Angenommen

## Kategorie

Userspace / Files

## Zweck

NovaOS definiert, wie Benutzerdateien im Userspace behandelt werden.

Benutzerdateien sind eigenständige Filesystem-Objekte mit stabiler `ObjectID` und werden nicht dauerhaft über ihren Pfad identifiziert.

```text
File
├── ObjectID
├── Content
├── Metadata
├── Semantic Type
└── Relations
```

## Grundprinzipien

```text
File ≠ Path
File ≠ Filename
File ≠ Extension
ObjectID ≠ ContentID
```

Rename, Move oder eine andere Projection verändern nicht automatisch die Identität einer Datei.

## Benutzerdateien

Benutzerdateien liegen logisch im Benutzerbereich:

```text
/Benutzer/<User>/
```

Die tatsächlichen Daten können unabhängig davon auf unterschiedlichen Volumes oder Storage Devices gespeichert sein.

Programme sollen Dateien über autorisierte Handles verwenden und nicht von festen physischen Speicherorten abhängig sein.

## Dateinamen

Der Dateiname ist eine menschenlesbare Namespace-Eigenschaft.

```text
ObjectID: 42
Name: Bericht.pdf
```

Eine Umbenennung:

```text
Bericht.pdf
→
Projektbericht.pdf
```

verändert die `ObjectID` nicht.

## Dateiendungen

Dateiendungen bleiben für Benutzer und Kompatibilität unterstützt.

```text
.pdf
.jpg
.txt
.nf
.nui
.nlf
```

Sie dienen als Hinweis auf Format oder Verwendung, bestimmen jedoch nicht allein die tatsächliche Semantik einer Datei.

## NovaFile

NovaFile `.nf` ist das native Containerformat für Dateien, bei denen Payload, Metadaten und Beziehungen gemeinsam verwaltet werden sollen.

```text
NovaFile
├── Payload
├── Metadata
└── Relationships
```

Native Dateien müssen jedoch weiterhin direkt unterstützt werden.

NovaOS darf daher beispielsweise sowohl:

```text
bild.jpg
```

als auch:

```text
bild.jpg.nf
```

verarbeiten.

## Metadaten und Semantik

Dateien können typisierte Metadaten und semantische Informationen besitzen.

Diese ermöglichen:

- semantische Suche
- automatische Organisation
- Projections
- Relationships
- passende Capability-Auswahl

Der Speicherpfad bleibt davon unabhängig.

## Kopieren und Verschieben

```text
Move
→ gleiche ObjectID

Copy
→ neue ObjectID
```

Eine reine Projection erzeugt ebenfalls keine neue Datei.

## Berechtigungen

Dateizugriffe erfolgen über das Filesystem-Permission- und Capability-Modell.

```text
File Request
    ↓
ObjectID
    ↓
Permission Check
    ↓
Authorized Handle
```

Die Sichtbarkeit einer Datei gewährt keine Zugriffsberechtigung.

## Normative Anforderungen

1. Benutzerdateien MÜSSEN stabile `ObjectID`s besitzen können.
2. Dateiname und Pfad DÜRFEN NICHT die Objektidentität bestimmen.
3. Rename und Move DÜRFEN die `ObjectID` nicht verändern.
4. Copy MUSS für die Kopie eine neue `ObjectID` erzeugen.
5. Native Dateiformate MÜSSEN weiterhin unterstützt werden.
6. NovaFile MUSS Payload, Metadaten und Relationships gemeinsam verwalten können.
7. Dateiendungen DÜRFEN NICHT als alleinige semantische Identität gelten.
8. Dateien MÜSSEN unabhängig vom physischen Storage-Ort adressierbar sein.
9. Projections DÜRFEN keine unnötigen Dateikopien erzeugen.
10. Dateizugriffe MÜSSEN capability-basiert autorisierbar sein.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`

## Ergebnis

NovaOS behandelt Benutzerdateien als stabile logische Objekte statt als bloße Pfade. Dateiname, Speicherort und Darstellung können sich ändern, während Identität, Metadaten, Beziehungen und Berechtigungen erhalten bleiben.