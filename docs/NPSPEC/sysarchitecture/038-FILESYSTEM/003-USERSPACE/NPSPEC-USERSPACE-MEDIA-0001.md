# NPSPEC-USERSPACE-MEDIA-0001 – Nova Userspace Media

## Status

Angenommen

## Kategorie

Userspace / Media

## Zweck

NovaOS behandelt Medien als semantische Benutzerobjekte und nicht als zwingend festgelegte Ordnerstruktur.

Medien können unabhängig von ihrem Speicherort über Typ, Metadaten und Beziehungen gefunden und dargestellt werden.

## Grundprinzipien

```text
Media ≠ Directory
Media Type ≠ File Extension
Media View ≠ Copy
Media Library ≠ Physical Storage
```

Klassische Ordner bleiben weiterhin vollständig nutzbar.

## Medientypen

Mindestens folgende semantische Medienklassen sollen unterstützt werden:

```text
Image
Photo
Audio
Music
Video
Animation
```

Weitere Typen können über das semantische Typsystem ergänzt werden.

## Medienansichten

NovaOS kann logische Ansichten erzeugen:

```text
Medien/
├── Bilder/
├── Musik/
└── Videos/
```

Diese Ansichten werden über Projections aus vorhandenen Objekten erzeugt.

Ein Bild muss daher nicht physisch in einem Ordner `Bilder` liegen.

## Metadaten

Medien können typisierte Metadaten besitzen, beispielsweise:

```text
Title
Artist
Album
Duration
Dimensions
Date
Codec
Tags
```

Welche Metadaten gültig sind, wird durch den jeweiligen semantischen Typ bestimmt.

## Organisation

Medien dürfen automatisch nach ihren Eigenschaften organisiert werden.

Beispiele:

```text
Bilder → Jahr → Monat
Musik → Künstler → Album
Videos → Kategorie
```

Mehrere Ansichten dürfen dasselbe `ObjectID` darstellen, ohne die Mediendatei zu duplizieren.

## Zugriff

Programme und Solutions sollen Medien über:

```text
Semantic Query
      ↓
ObjectID
      ↓
Capability Check
      ↓
Authorized Handle
```

finden und verwenden können.

Feste Pfade wie `/Benutzer/.../Bilder` dürfen nicht Voraussetzung für den Zugriff sein.

## Externe Medien

Medien auf:

```text
zusätzlichen Volumes
USB-Speichern
Remote Storage
```

können in dieselben Medienansichten integriert werden.

Das Entfernen einer Quelle muss die betroffenen Objekte als nicht verfügbar behandeln, ohne andere Medienansichten zu beschädigen.

## Sicherheit

Semantische Medienansichten erzeugen keine zusätzliche Authority.

Metadaten können ebenfalls geschützt sein, insbesondere wenn sie sensible Informationen wie Standortdaten enthalten.

## Normative Anforderungen

1. NovaOS MUSS Medien semantisch klassifizieren können.
2. Medienorganisation DARF NICHT von festen Ordnern abhängig sein.
3. Klassische Benutzerordner MÜSSEN weiterhin unterstützt werden.
4. Mehrere Medienansichten DÜRFEN dasselbe `ObjectID` darstellen.
5. Medien-Projections DÜRFEN keine unnötigen Kopien erzeugen.
6. Medientyp und Dateiendung MÜSSEN getrennte Konzepte bleiben.
7. Medien-Metadaten SOLLEN typisiert sein.
8. Medien auf unterschiedlichen Volumes MÜSSEN gemeinsam dargestellt werden können.
9. Programme SOLLEN Medien über semantische Discovery finden können.
10. Medienansichten DÜRFEN keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-USERSPACE-DATA-0001`
- `NPSPEC-USERSPACE-FILES-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`

## Ergebnis

NovaOS behandelt Bilder, Musik, Videos und andere Medien als semantisch organisierbare Objekte. Benutzer können klassische Ordner weiterhin verwenden, während NovaOS zusätzlich dynamische Medienansichten unabhängig vom tatsächlichen Speicherort bereitstellen kann.