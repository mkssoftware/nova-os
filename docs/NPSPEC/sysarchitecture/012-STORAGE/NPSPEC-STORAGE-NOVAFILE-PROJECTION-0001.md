# NPSPEC-STORAGE-NOVAFILE-PROJECTION-0001 – NovaFile Projection

## Status

Angenommen

## Kategorie

Storage / NovaFile / Projection / Compatibility

## Zweck

NovaOS definiert NovaFile Projection als Mechanismus, mit dem Inhalte und Metadaten eines NovaFile-Containers als klassische Datei- oder Verzeichnisansicht dargestellt werden können.

```text
NovaFile
   ↓
Projection
   ↓
Conventional File View
```

Damit können NovaFile-Objekte auch von Komponenten verwendet werden, die das native NovaFile-Modell nicht verstehen.

## Grundprinzip

```text
Projection ≠ Copy
Projection ≠ Object Identity
Projected Path ≠ Physical Representation
```

Eine Projektion ist eine alternative Sicht auf dasselbe logische Storage-Objekt.

## Projektionsmodell

Ein NovaFile kann unterschiedliche Sichten bereitstellen.

```text
NovaFile
├── Native View
├── Payload View
└── Metadata View
```

Die Standardprojektion soll normalerweise den ursprünglichen Payload darstellen.

Beispiel:

```text
bild.jpg.nf
      ↓
Projection
      ↓
bild.jpg
```

## Payload Projection

Der Payload darf als normale Datei projiziert werden.

```text
NovaFile
└── Payload → JPEG

Projection
└── bild.jpg
```

Die projizierte Datei soll sich für kompatible Anwendungen wie eine gewöhnliche Datei verhalten.

## Metadata Projection

Metadaten dürfen optional als strukturierte Sicht verfügbar gemacht werden.

```text
NovaFile
├── Payload
└── Metadata
      ↓
Projection
├── content
└── metadata
```

Die konkrete Darstellung darf durch VFS oder Capability bestimmt werden.

## Objektidentität

Eine Projektion erzeugt grundsätzlich kein neues Storage-Objekt.

```text
NovaFile ObjectID
       ↓
Projection
       ↓
Same ObjectID
```

Erst eine explizite Export- oder Kopieroperation darf ein neues unabhängiges Objekt erzeugen.

## Schreibzugriff

Schreibzugriffe auf eine Projektion müssen eindeutig auf den zugrunde liegenden NovaFile-Inhalt abgebildet werden können.

```text
Projected Payload
       ↓
Modify
       ↓
Transaction
       ↓
New NovaFile Version
```

Eine Änderung des Payloads soll eine neue Objektversion und gegebenenfalls eine neue `ContentID` erzeugen.

## Metadatenänderungen

Änderungen an projizierten Metadaten müssen über das NovaFile-Metadatenmodell erfolgen.

Unbekannte oder nicht darstellbare Metadaten dürfen nicht unbeabsichtigt verloren gehen.

## Legacy-Kompatibilität

Legacy-Anwendungen dürfen ausschließlich die Payload-Projektion sehen.

```text
Legacy Application
       ↓
VFS
       ↓
Projected Payload
       ↓
NovaFile
```

Die Anwendung muss das NovaFile-Containerformat dafür nicht kennen.

## Export

Eine Projektion darf als eigenständige klassische Datei exportiert werden.

```text
NovaFile
   ↓
Payload Projection
   ↓
Export
   ↓
Standalone File
```

Export erzeugt ein neues Storage-Objekt und muss von einer bloßen Projektion unterschieden werden.

## Formatzuordnung

Der projizierte Dateiname und Dateityp dürfen aus:

```text
Original Name
Payload Format
Semantic Type
Metadata
```

abgeleitet werden.

Eine Dateiendung darf dabei nicht die alleinige Quelle für die Inhaltsklassifikation sein.

## Sicherheit

Eine Projektion darf keine zusätzlichen Zugriffsrechte erzeugen.

```text
NovaFile Permission
        ↓
Projection
        ↓
Same or More Restricted
```

Geschützte Metadaten dürfen über eine Projektion nicht sichtbar werden, wenn der aufrufende Security Context dafür nicht autorisiert ist.

## Caching

NovaOS darf Projektionen cachen.

```text
NovaFile
   ↓
Projection Cache
   ↓
Consumer
```

Der Cache muss bei Änderungen des zugrunde liegenden Objekts invalidiert oder aktualisiert werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
Projection Type
Source NovaFile
Projected Format
Access Mode
VersionID
Cache State
```

## Normative Anforderungen

1. NovaOS MUSS NovaFile-Inhalte als klassische Datei-Sicht projizieren können.
2. Eine Projektion DARF grundsätzlich KEINE neue `ObjectID` erzeugen.
3. Die Payload-Projektion MUSS ohne Verständnis des NovaFile-Formats verwendbar sein.
4. Projektionspfad und physische NovaFile-Repräsentation MÜSSEN getrennt bleiben.
5. Schreibzugriffe auf Projektionen MÜSSEN kontrolliert auf das zugrunde liegende Objekt abgebildet werden.
6. Payload-Änderungen SOLLEN eine neue Objektversion erzeugen.
7. Nicht dargestellte Metadaten DÜRFEN durch Projektionsoperationen NICHT unbeabsichtigt verloren gehen.
8. Export und Projection MÜSSEN klar getrennte Operationen sein.
9. Export eines unabhängigen Objekts MUSS eine neue `ObjectID` erzeugen.
10. Projektionen DÜRFEN keine zusätzlichen Zugriffsrechte erzeugen.
11. Projektions-Caches MÜSSEN mit dem zugrunde liegenden Objekt konsistent gehalten werden.
12. Projektionsart, Quelle und Version SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `ADR-STORAGE-0019`

## Ergebnis

```text
NovaFile
   ↓
Projection Layer
   ├→ Native Nova View
   ├→ Payload File View
   └→ Metadata View
           ↓
     Compatible Consumers
```

NovaOS erhält damit eine Kompatibilitätsschicht, durch die NovaFile seine erweiterten Metadaten und Objektfunktionen behält, während klassische Anwendungen weiterhin mit normalen Datei-Sichten arbeiten können.