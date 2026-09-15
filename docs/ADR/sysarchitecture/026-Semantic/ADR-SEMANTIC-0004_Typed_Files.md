# ADR-SEMANTIC-0004 – Typed Files

## Status

Angenommen

## Kategorie

Semantic Architecture / Filesystem / Type System

## Kontext

Dateiendungen wie `.jpg`, `.md` oder `.pdf` geben Hinweise auf den Inhalt einer Datei, sind jedoch nur Namensbestandteile und können falsch, mehrdeutig oder nicht vorhanden sein.

NovaOS benötigt eine zuverlässigere Möglichkeit, Dateien anhand ihrer tatsächlichen semantischen Bedeutung zu behandeln.

## Entscheidung

NovaOS verwendet **Typed Files**.

Dateien können einen oder mehrere Semantic Types besitzen, die unabhängig von Dateiname und Pfad gespeichert werden.

```text
File
 ├── Object ID
 ├── Semantic Type
 └── Payload
```

Beispiele:

```text
Document.Markdown
Document.PDF
Image.JPEG
Audio.FLAC
Video.H264
SourceCode.Python
```

## Dateiendungen

Dateiendungen bleiben für Kompatibilität und Benutzerfreundlichkeit erhalten, sind aber nicht die primäre Typidentität.

```text
photo.jpg
    ↓
Image.JPEG
```

Eine Umbenennung verändert den Semantic Type nicht automatisch.

```text
photo.jpg → holiday.data

Semantic Type:
Image.JPEG
```

## Typerkennung

Beim Import unbekannter Dateien kann NovaOS den Typ anhand verfügbarer Informationen bestimmen.

```text
Declared Metadata
File Format
Content Signature
Importer
User Decision
```

Das Ergebnis wird als semantische Typinformation mit dem Dateiobjekt verbunden.

## Mehrere Typen

Eine Datei kann mehrere kompatible Typinformationen besitzen.

```text
File
 ├── Document
 ├── Markdown
 └── Text
```

Dadurch können sowohl allgemeine als auch spezialisierte Capabilities mit derselben Datei arbeiten.

## Capabilities

Capabilities deklarieren, welche File Types sie verarbeiten können.

```text
File<Image.JPEG>
       ↓
Compatible Capability
       ↓
Decode / Edit / Convert
```

Die Auswahl einer Funktion erfolgt damit nicht ausschließlich über die Dateiendung.

## NovaFile

Typed Files müssen mit dem NovaFile-Modell kompatibel sein.

Semantische Typinformationen können Bestandteil der Metadaten eines NovaFile-Containers sein, ohne den eigentlichen Payload zu verändern.

## Normative Anforderungen

1. NovaOS MUSS Dateien Semantic Types zuordnen können.
2. Semantic File Types MÜSSEN unabhängig von Dateiname und Pfad sein.
3. Dateiendungen DÜRFEN nicht als alleinige Typidentität verwendet werden.
4. Eine Datei MUSS mehrere kompatible Semantic Types besitzen können.
5. Capabilities MÜSSEN Semantic File Types in ihren Contracts verwenden können.
6. Umbenennen oder Verschieben einer Datei DARF ihren Semantic Type nicht automatisch verändern.
7. Typinformationen MÜSSEN mit NovaFile und externen Dateiformaten verwendbar sein.

## Konsequenzen

### Positive Konsequenzen

- Dateien werden nach ihrem tatsächlichen Inhalt behandelt.
- Dateiendungen verlieren ihre Rolle als alleinige Typinformation.
- Capabilities können automatisch passende Dateien erkennen.
- Umbenennen und Verschieben beeinflusst die Dateibedeutung nicht.

### Negative Konsequenzen

- Typinformationen müssen gespeichert und gepflegt werden.
- importierte Dateien benötigen gegebenenfalls eine Typerkennung.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-TYPED-FILE-0001`
- `NPSPEC-SEMANTIC-FILE-TYPE-DETECTION-0001`

## Ergebnis

NovaOS behandelt den Dateityp als Eigenschaft des Dateiobjekts und nicht nur seines Namens:

```text
File Object
    ↓
Semantic Type
    ↓
Compatible Capabilities
```

Der zentrale Grundsatz lautet:

```text
Der Dateiname sagt,
wie eine Datei heißt.

Der Semantic Type sagt,
was sie ist.
```