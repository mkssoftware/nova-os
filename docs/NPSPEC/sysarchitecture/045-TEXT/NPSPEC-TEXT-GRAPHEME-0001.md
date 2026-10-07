# NPSPEC-TEXT-GRAPHEME-0001 – Nova Grapheme Model

## Status

Angenommen

## Kategorie

Text / Grapheme

## Zweck

NovaOS definiert Grapheme Cluster als primäre Einheit für benutzersichtbare Zeichenoperationen.

Ein sichtbares Zeichen kann aus einem oder mehreren Unicode-Codepoints bestehen. Cursorbewegung, Auswahl, Löschen und ähnliche UI-Operationen dürfen daher nicht grundsätzlich auf einzelnen Codepoints oder Bytes basieren.

## Grundprinzipien

```text
Grapheme ≠ Byte
Grapheme ≠ Code Unit
Grapheme ≠ Code Point
Grapheme ≠ Glyph
Grapheme ≠ Display Width
One Grapheme ≠ One Code Point
```

## Architektur

```text
UTF-8 Bytes
    ↓
Unicode Code Points
    ↓
Grapheme Segmentation
    ↓
Extended Grapheme Clusters
    ↓
Text Layout
    ↓
Glyphs
```

## Grapheme Cluster

NovaOS verwendet Extended Grapheme Clusters entsprechend den Unicode-Segmentierungsregeln als Grundlage für benutzersichtbare Zeichen.

Beispiele:

```text
"a"                         → 1 Grapheme
"a" + Combining Mark        → 1 Grapheme
Emoji + Modifier            → 1 Grapheme
Emoji ZWJ Sequence          → 1 Grapheme
Regional Indicator Sequence → 1 Grapheme
```

Die Anzahl der Codepoints kann größer als die Anzahl der Grapheme sein.

## Textoperationen

Benutzerorientierte Operationen sollen Grapheme-Grenzen berücksichtigen:

```text
Cursor Left / Right
Selection
Backspace
Delete
Text Truncation
Character Counting
Editing
```

Eine Operation darf einen Grapheme Cluster nicht unbeabsichtigt in einen ungültigen oder unerwarteten sichtbaren Zustand zerlegen.

## Indizes

NovaOS unterscheidet explizit:

```text
Byte Offset
Code Point Index
Grapheme Index
Display Position
```

Diese Werte dürfen nicht implizit gegeneinander ausgetauscht werden.

## Grapheme-Segmentierung

Die Segmentierung erfolgt anhand der Unicode Grapheme-Break-Regeln.

```text
Code Point Stream
       ↓
Unicode Properties
       ↓
Grapheme Break Algorithm
       ↓
Grapheme Boundaries
```

Die verwendete Unicode-Version muss eindeutig bestimmbar sein.

## Normalisierung

Grapheme-Segmentierung und Unicode-Normalisierung bleiben getrennte Operationen.

```text
Normalization ≠ Segmentation
```

Text muss nicht normalisiert werden, um grundsätzlich in Grapheme Cluster segmentiert werden zu können.

## Rendering

Ein Grapheme Cluster kann:

```text
One Glyph
Multiple Glyphs
Ligature
Combined Glyph Sequence
```

erzeugen.

Die endgültige Glyphenbildung ist Aufgabe der Text-Layout- und Rendering-Schicht.

## Display Width

Die visuelle Breite eines Graphems ist nicht aus seiner Codepoint-Anzahl ableitbar.

```text
Grapheme Count ≠ Column Width
Grapheme Count ≠ Pixel Width
```

Terminal-, UI- und Layoutsysteme müssen die jeweilige Darstellungsbreite separat bestimmen.

## Performance

Subsysteme dürfen Grapheme-Grenzen cachen oder indizieren.

```text
Text
 ↓
Grapheme Index
 ↓
Fast Navigation
```

Caches müssen bei Textänderungen invalidiert oder aktualisiert werden.

## Sicherheit

Textvalidierung darf Grapheme-Darstellung nicht mit Identität verwechseln.

Visuell ähnliche oder identische Grapheme können unterschiedliche Unicode-Sequenzen besitzen.

Sicherheitskritische Identitäten dürfen daher nicht allein anhand sichtbarer Grapheme verglichen werden.

## Normative Anforderungen

1. NovaOS MUSS Extended Grapheme Clusters unterstützen.
2. Benutzerorientierte Textoperationen SOLLEN Grapheme Cluster als Zeicheneinheit verwenden.
3. Byte-, Codepoint- und Grapheme-Indizes MÜSSEN getrennte Konzepte bleiben.
4. Cursorbewegungen SOLLEN Grapheme-Grenzen berücksichtigen.
5. Löschen und Auswahl SOLLEN Grapheme Cluster nicht unbeabsichtigt zerlegen.
6. Emoji-, Combining-Mark- und ZWJ-Sequenzen MÜSSEN korrekt segmentierbar sein.
7. Grapheme-Segmentierung MUSS auf Unicode-Regeln basieren.
8. Segmentierung und Normalisierung MÜSSEN getrennte Operationen bleiben.
9. Grapheme und Glyph MÜSSEN getrennte Konzepte bleiben.
10. Grapheme-Anzahl DARF nicht mit Display-Breite gleichgesetzt werden.
11. Grapheme-Indizes DÜRFEN zur Performanceoptimierung gecacht werden.
12. Unicode-Version und verwendete Segmentierungsregeln MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-ENCODING-0001`
- `NPSPEC-TEXT-NORMALIZATION-0001`
- `NPSPEC-GLOBALIZATION-RTL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt benutzersichtbare Zeichen als Unicode Grapheme Cluster statt als einzelne Bytes oder Codepoints. Dadurch funktionieren Cursorbewegung, Auswahl, Löschen und Textbearbeitung auch bei kombinierten Zeichen, Emoji-Sequenzen und komplexen Schriftsystemen konsistent.