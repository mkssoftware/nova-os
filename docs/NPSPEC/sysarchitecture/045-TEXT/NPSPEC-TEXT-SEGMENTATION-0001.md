# NPSPEC-TEXT-SEGMENTATION-0001 – Nova Text Segmentation

## Status

Angenommen

## Kategorie

Text / Segmentation

## Zweck

NovaOS definiert eine einheitliche Unicode-basierte Segmentierung von Text in logische Grenzen.

Die Segmentierung stellt gemeinsame Regeln für Grapheme, Wörter und Sätze bereit, ohne diese Einheiten mit Bytes, Codepoints, Glyphen oder visuellen Layoutgrenzen gleichzusetzen.

## Grundprinzipien

```text
Segmentation ≠ Encoding
Segmentation ≠ Normalization
Grapheme Boundary ≠ Word Boundary
Word Boundary ≠ Sentence Boundary
Code Point Boundary ≠ User-Visible Boundary
Visual Line ≠ Logical Text Segment
```

## Segmenttypen

NovaOS unterstützt mindestens:

```text
Grapheme
Word
Sentence
```

Zusätzliche spezialisierte Segmentierungen dürfen durch höhere Textdienste bereitgestellt werden.

## Architektur

```text
Unicode Code Points
        ↓
Unicode Properties
        ↓
Segmentation Engine
        ↓
Boundary Map
   ┌────┼─────┐
   ↓    ↓     ↓
Grapheme Word Sentence
```

## Grapheme-Segmentierung

Grapheme-Grenzen basieren auf dem Nova Grapheme Model.

Sie bilden die Grundlage für benutzersichtbare Zeichenoperationen.

```text
Code Points
    ↓
Extended Grapheme Clusters
```

## Wortsegmentierung

Wortgrenzen dienen unter anderem:

```text
Word Selection
Cursor Navigation
Search
Tokenization
Spell Checking
Accessibility
```

Eine Wortgrenze darf nicht ausschließlich anhand von Leerzeichen bestimmt werden.

Sprachen ohne explizite Leerzeichentrennung müssen unterstützt werden können.

## Satzsegmentierung

Satzgrenzen dienen unter anderem:

```text
Text Navigation
Accessibility
Language Processing
Search
Editing
```

Satzzeichen allein dürfen nicht grundsätzlich als eindeutige Satzgrenze interpretiert werden.

## Unicode-Regeln

Die Standardsegmentierung basiert auf den Unicode-Segmentierungsregeln.

```text
Text
 ↓
Unicode Properties
 ↓
Boundary Rules
 ↓
Segments
```

Die verwendete Unicode-Version muss eindeutig bestimmbar sein.

## Sprachspezifische Erweiterungen

Bestimmte Sprachen benötigen zusätzliche linguistische Verarbeitung.

```text
Unicode Default Rules
        ↓
Locale / Language Context
        ↓
Optional Language Provider
        ↓
Refined Segmentation
```

Sprachspezifische Segmentierung darf die allgemeine Unicode-Schicht erweitern, ohne deren Datenmodell zu ersetzen.

## Indizes

Segmentgrenzen müssen auf eindeutige Textpositionen abbildbar sein:

```text
Byte Offset
Code Point Index
Grapheme Index
Segment Boundary
```

Die jeweilige Indexdomäne muss explizit bekannt sein.

## Änderung von Text

Nach Textänderungen müssen betroffene Segmentgrenzen neu bestimmt werden können.

```text
Text Edit
   ↓
Affected Range
   ↓
Incremental Re-Segmentation
```

Eine vollständige Neuberechnung des gesamten Dokuments darf vermieden werden, sofern das Ergebnis identisch bleibt.

## Normalisierung

Segmentierung darf keine implizite Normalisierung durchführen.

```text
Segmentation ≠ Normalization
```

Falls ein Subsystem normalisierten Text benötigt, muss dies separat definiert werden.

## Sicherheit

Segmentierungsgrenzen dürfen nicht als Sicherheitsgrenzen interpretiert werden.

Visuell oder linguistisch zusammengehörige Textteile können aus unterschiedlichen Unicode-Sequenzen bestehen.

## Normative Anforderungen

1. NovaOS MUSS Unicode-basierte Textsegmentierung unterstützen.
2. Grapheme-, Wort- und Satzgrenzen MÜSSEN getrennte Segmenttypen bleiben.
3. Standardsegmentierung MUSS auf Unicode-Regeln basieren.
4. Wortsegmentierung DARF nicht ausschließlich auf Leerzeichen beruhen.
5. Sprachen ohne Leerzeichentrennung MÜSSEN unterstützt werden können.
6. Sprachspezifische Segmentierungsprovider MÜSSEN integrierbar sein.
7. Segmentierung und Normalisierung MÜSSEN getrennte Operationen bleiben.
8. Segmentgrenzen MÜSSEN eindeutig auf Textpositionen abbildbar sein.
9. Die verwendete Indexdomäne MUSS explizit sein.
10. Inkrementelle Re-Segmentierung MUSS unterstützt werden können.
11. Segmentgrenzen DÜRFEN nicht als Authority- oder Sicherheitsgrenzen behandelt werden.
12. Segmenttyp, Unicode-Version, verwendete Regeln und Provider MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-NORMALIZATION-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine gemeinsame Unicode-basierte Segmentierungsinfrastruktur für Grapheme, Wörter und Sätze. Dadurch können Texteditoren, Suche, Navigation, Accessibility und Sprachverarbeitung dieselben konsistenten Textgrenzen verwenden, während sprachspezifische Erweiterungen kontrolliert integrierbar bleiben.