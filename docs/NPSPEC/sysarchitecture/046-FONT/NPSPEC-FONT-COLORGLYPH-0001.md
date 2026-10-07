# NPSPEC-FONT-COLORGLYPH-0001 – Nova Color Glyph Fonts

## Status

Angenommen

## Kategorie

Font / Color Glyph

## Zweck

NovaOS definiert die systemweite Unterstützung mehrfarbiger Glyphen innerhalb von Fonts.

Color Glyphs werden insbesondere für Emoji, Symbole, Icons und dekorative Schriftzeichen verwendet und bleiben vollständig in Font Manager, Shaping, Fallback und Rendering integriert.

## Grundprinzipien

```text
Color Glyph ≠ Image File
Color Glyph ≠ Unicode Character
Color Glyph ≠ Emoji
Color Glyph ≠ Font
Glyph Color ≠ Text Color
Color Representation ≠ Glyph Identity
```

## Architektur

```text
Unicode Text
     ↓
Font Selection / Fallback
     ↓
Shaping
     ↓
GlyphID
     ↓
Color Glyph Resolution
     ↓
Layers / Vector / Bitmap
     ↓
Rendering
```

## Modell

```text
ColorGlyph
├── FontID
├── GlyphID
├── Format
├── Layers[]
├── Palette
├── IntrinsicColors
└── State
```

Eine Color Glyph bleibt über `FontID + GlyphID` Teil der jeweiligen Fontressource.

## Unterstützte Repräsentationen

NovaOS muss moderne Color-Font-Verfahren unterstützen können:

```text
COLR / CPAL
COLRv1
SVG Glyphs
CBDT / CBLC
sbix
```

Weitere Formate dürfen über Font- oder Rendering-Provider ergänzt werden.

## Layer-basierte Glyphen

Eine Color Glyph darf aus mehreren Glyphenebenen bestehen:

```text
Glyph
 ├── Layer 1 → Color A
 ├── Layer 2 → Color B
 └── Layer 3 → Color C
```

Die Ebenen werden als eine logische Glyphe dargestellt.

## Paletten

Fonts dürfen eine oder mehrere Farbpaletten bereitstellen.

```text
Default Palette
Light Palette
Dark Palette
Custom Palette
```

NovaOS darf passende Paletten anhand des UI-Kontexts auswählen.

Eine explizite Auswahl durch Anwendung oder Benutzer hat Vorrang vor automatischer Auswahl.

## Vektorbasierte Glyphen

Vektorbasierte Color Glyphs müssen unabhängig von der Zielauflösung skalierbar sein.

COLRv1- oder vergleichbare Darstellungen dürfen zusätzlich:

```text
Gradients
Transforms
Compositing
Transparency
```

verwenden.

## Bitmap-Glyphen

Bitmap-basierte Color Glyphs dürfen mehrere Auflösungen besitzen.

Der Renderer soll die für die Zielgröße geeignetste verfügbare Darstellung auswählen.

Fehlt eine geeignete Bitmap, darf auf eine andere unterstützte Darstellung oder einen anderen Font zurückgegriffen werden.

## Emoji

Color Glyph Fonts dürfen für Emoji verwendet werden.

Dabei müssen insbesondere zusammengehörige Unicode-Sequenzen berücksichtigt werden:

```text
Variation Selectors
Emoji Modifiers
ZWJ Sequences
Regional Indicators
Keycap Sequences
```

Font Fallback darf solche Sequenzen nicht unnötig zerlegen.

## Textfarbe

Intrinsic Colors eines Fonts und vom Benutzer definierte Textfarbe sind getrennte Konzepte.

Ein Color Font darf festgelegte Farben besitzen.

Für geeignete Glyphen darf eine monochrome oder vom UI gesteuerte Darstellung angeboten werden, sofern das Fontformat dies unterstützt.

## Fallback

Kann ein Font eine Color Glyph nicht darstellen:

```text
Color Glyph Request
       ↓
Fallback Resolution
       ↓
Color Font
       ↓
Monochrome Font
       ↓
Missing Glyph
```

Eine lesbare monochrome Darstellung ist einer fehlenden Darstellung vorzuziehen.

## Sicherheit

SVG-, Bitmap- und komplexe Color-Font-Daten müssen als nicht vertrauenswürdige Fontinhalte behandelt werden.

Parsing und Rendering dürfen keine unkontrollierte Codeausführung oder externe Ressourcenauflösung ermöglichen.

## Normative Anforderungen

1. NovaOS MUSS Color Glyph Fonts unterstützen können.
2. Color Glyphs MÜSSEN in die normale Font- und Shaping-Architektur integriert sein.
3. Layer-, Vektor- und Bitmap-basierte Color Glyphs MÜSSEN unterstützt werden können.
4. `COLR/CPAL` und moderne COLR-Darstellungen MÜSSEN unterstützt werden können.
5. Fontpaletten MÜSSEN auswählbar sein.
6. Color Glyphs DÜRFEN die logische Textstruktur nicht verändern.
7. Emoji-Sequenzen MÜSSEN beim Fallback möglichst zusammengehalten werden.
8. Monochromer Fallback MUSS möglich sein.
9. Intrinsic Font Colors und Textfarbe MÜSSEN getrennte Konzepte bleiben.
10. SVG- und Bitmap-Inhalte MÜSSEN sicher validiert und verarbeitet werden.
11. Externe Ressourcenauflösung aus Color Glyphs DARF nicht implizit erfolgen.
12. Glyphformat, Palette, Layer, Fallback und Renderingpfad MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FONT-MANAGER-0001`
- `NPSPEC-FONT-FALLBACK-0001`
- `NPSPEC-FONT-VARIABLE-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SHAPING-0001`
- `NPSPEC-TEXT-SECURITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann mehrfarbige Fontglyphen als normalen Bestandteil des Textsystems darstellen. Emoji, Symbole und andere Color Glyphs können über Layer-, Vektor- oder Bitmap-Verfahren gerendert werden, während Shaping, Fallback, Sicherheit und Textsemantik vollständig erhalten bleiben.