# NPSPEC-TEXT-SHAPING-0001 – Nova Text Shaping

## Status

Angenommen

## Kategorie

Text / Shaping

## Zweck

NovaOS definiert Text Shaping als Umwandlung von Unicode-Text in positionierte Glyphen für die visuelle Darstellung.

Shaping berücksichtigt Script, Sprache, Schreibrichtung, Font und typografische Regeln, ohne den zugrunde liegenden Unicode-Text zu verändern.

## Grundprinzipien

```text
Character ≠ Glyph
Code Point ≠ Glyph
Grapheme ≠ Glyph
Script ≠ Font
Shaping ≠ BiDi
Shaping ≠ Rasterization
Visual Glyph Order ≠ Stored Text Order
```

## Architektur

```text
Unicode Text
     ↓
Segmentation / BiDi
     ↓
Script Runs
     ↓
Font Selection
     ↓
Shaping Engine
     ↓
Glyph IDs + Positions
     ↓
Text Layout
     ↓
Rendering
```

## Eingaben

Ein Shaping-Vorgang erhält mindestens:

```text
Unicode Text
Script
Language
Direction
Font
Font Features
Variation Parameters
```

Fehlende optionale Informationen dürfen durch definierte Defaults ergänzt werden.

## Ausgabe

Das Ergebnis besteht aus einer Glyphensequenz:

```text
GlyphRun
├── GlyphIDs[]
├── Advances[]
├── Offsets[]
├── Clusters[]
├── Direction
├── FontID
└── State
```

Die Zuordnung zwischen Glyphen und ursprünglichem Text muss erhalten bleiben.

## Kontextabhängiges Shaping

Ein Codepoint kann abhängig vom Kontext unterschiedliche Glyphen erzeugen.

Dies ist insbesondere relevant für:

```text
Arabic Joining
Indic Scripts
Ligatures
Combining Marks
Contextual Forms
Reordering
```

NovaOS darf daher Glyphen nicht durch eine einfache `CodePoint → Glyph`-Tabelle bestimmen.

## Ligaturen

Mehrere Zeichen dürfen zu einer gemeinsamen Glyphe geformt werden:

```text
f + i
  ↓
Ligature Glyph
```

Die logischen Textpositionen müssen trotzdem erhalten bleiben.

## Combining Marks

Kombinierende Zeichen müssen relativ zu ihrer Basisglyphe korrekt positionierbar sein.

```text
Base Glyph
    +
Combining Mark
    ↓
Positioned Glyph Cluster
```

## BiDi-Integration

Der BiDi-Algorithmus bestimmt gerichtete Runs.

Das Shaping verarbeitet diese Runs anschließend mit der aufgelösten Richtung.

```text
BiDi Resolution
      ↓
Directional Run
      ↓
Script Shaping
```

BiDi und Shaping bleiben getrennte Verarbeitungsschritte.

## Font Fallback

Wenn ein Font benötigte Glyphen nicht bereitstellt, darf Font Fallback verwendet werden.

Fallback darf Grapheme oder Shaping-Sequenzen nicht unnötig zerlegen.

```text
Missing Glyph
     ↓
Fallback Font
     ↓
Reshape Affected Run
```

## Typografische Features

Shaping Provider dürfen Features unterstützen wie:

```text
Ligatures
Kerning
Contextual Alternates
Small Caps
Fractions
Stylistic Sets
Variable Font Axes
```

Features müssen explizit steuerbar sein.

## Cluster Mapping

Glyphen müssen auf ihre logischen Textcluster zurückführbar bleiben.

Dies wird benötigt für:

```text
Cursor
Selection
Hit Testing
Editing
Accessibility
Text Extraction
```

Visuelle Glyphengrenzen dürfen nicht automatisch als Textgrenzen interpretiert werden.

## Provider-Modell

NovaOS darf austauschbare Shaping Provider verwenden.

```text
Text Shaping API
      ↓
Shaping Provider
      ↓
Glyph Run
```

Provider müssen dieselbe grundlegende Nova-Text-Semantik einhalten.

## Normative Anforderungen

1. NovaOS MUSS komplexes Unicode Text Shaping unterstützen.
2. Unicode-Text und Glyphensequenz MÜSSEN getrennte Repräsentationen bleiben.
3. Shaping MUSS Script, Sprache, Richtung und Font berücksichtigen können.
4. Kontextabhängige Glyphenformen MÜSSEN unterstützt werden.
5. Ligaturen und Combining Marks MÜSSEN korrekt abbildbar sein.
6. Shaping DARF den zugrunde liegenden Unicode-Text nicht verändern.
7. BiDi und Shaping MÜSSEN getrennte Verarbeitungsschritte bleiben.
8. Font Fallback MUSS mit Shaping integrierbar sein.
9. Glyphen MÜSSEN auf logische Textcluster zurückführbar bleiben.
10. Typografische Features MÜSSEN kontrollierbar sein.
11. Austauschbare Shaping Provider MÜSSEN unterstützt werden können.
12. Script, Sprache, Richtung, Font, Features, Cluster-Mapping und Provider MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SEGMENTATION-0001`
- `NPSPEC-TEXT-BIDI-0001`
- `NPSPEC-TEXT-SCRIPT-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine einheitliche Shaping-Schicht, die Unicode-Text unter Berücksichtigung von Script, Sprache, Richtung, Font und typografischen Regeln in korrekt positionierte Glyphen überführt. Die logische Textstruktur bleibt dabei vollständig erhalten und von der visuellen Darstellung getrennt.