# NPSPEC-FONT-FALLBACK-0001 – Nova Font Fallback

## Status

Angenommen

## Kategorie

Font / Fallback

## Zweck

NovaOS definiert ein systemweites Font-Fallback-Verfahren für Zeichen, Grapheme oder Shaping-Sequenzen, die vom bevorzugten Font nicht vollständig dargestellt werden können.

Fallback erfolgt automatisch, deterministisch und möglichst ohne sichtbare Brüche im Schriftbild.

## Grundprinzipien

```text
Fallback ≠ Font Replacement
Fallback ≠ Font Substitution
Fallback ≠ Shaping
Missing Glyph ≠ Missing Character
Font Coverage ≠ Script Coverage
```

## Architektur

```text
Requested Font
      ↓
Coverage Check
      ↓
Complete Coverage ──→ Use Font
      │
      └── Missing Coverage
               ↓
        Fallback Resolver
               ↓
        Candidate Fonts
               ↓
        Best Compatible Font
               ↓
             Shaping
```

## Fallback-Kontext

Die Auswahl darf berücksichtigen:

```text
Requested Font
Script
Language
Unicode Coverage
Grapheme
Shaping Sequence
Style
Weight
Width
Slant
Variation Axes
Scope
Font Policy
```

## Fallback-Kette

Für einen Textkontext wird eine geordnete Fallback-Kette aufgebaut:

```text
Preferred Font
      ↓
Family Fallback
      ↓
Script Fallback
      ↓
Language Fallback
      ↓
System Fallback
      ↓
Universal Fallback
```

Die konkrete Reihenfolge muss deterministisch sein.

## Grapheme-Integrität

Fallback darf ein Grapheme nicht unnötig auf mehrere Fonts verteilen.

```text
Base Character
      +
Combining Marks
      ↓
Prefer Same Font
```

Kann ein Font die vollständige Sequenz darstellen, soll dieser gegenüber einer fragmentierten Darstellung bevorzugt werden.

## Shaping-Sequenzen

Für komplexe Scripts muss Fallback auf einer ausreichend großen Shaping-Einheit erfolgen.

Ein einzelner fehlender Glyph darf nicht dazu führen, dass zusammengehörige Zeichen so getrennt werden, dass korrektes Shaping verloren geht.

## Stilähnlichkeit

Unter mehreren geeigneten Fonts soll der Font Manager möglichst ähnliche Eigenschaften bevorzugen:

```text
Weight
Width
Slant
Optical Size
Metrics
Design Compatibility
```

Lesbarkeit und vollständige Glyphabdeckung haben Vorrang vor visueller Ähnlichkeit.

## Emoji und Symbole

Emoji-, Symbol- und spezielle Unicode-Fonts dürfen über eigene Fallback-Regeln priorisiert werden.

```text
Text
Emoji
Symbols
Mathematics
Music
Specialized Scripts
```

Variation Selectors und zusammengehörige Emoji-Sequenzen müssen berücksichtigt werden.

## Scope

Fallback darf nur Fonts berücksichtigen, die im aktuellen Kontext sichtbar und zulässig sind.

```text
System
User
Application
Solution
Document
Temporary
```

Ein Font außerhalb des sichtbaren Scopes darf nicht allein durch Fallback verfügbar werden.

## Cache

Ermittelte Fallback-Ketten dürfen gecacht werden.

Der Cache-Schlüssel darf unter anderem enthalten:

```text
FontID
Script
Language
Style
Scope
FontGeneration
```

Änderungen an Fontregistrierung oder Fontbestand müssen betroffene Einträge invalidieren.

## Fehlerfall

Kann kein Font die benötigte Darstellung liefern, muss ein definierter Missing-Glyph-Fallback verwendet werden.

Der Fehler darf nicht zum Abbruch der gesamten Textdarstellung führen.

## Normative Anforderungen

1. NovaOS MUSS systemweiten Font Fallback bereitstellen.
2. Fallback MUSS Unicode Coverage berücksichtigen.
3. Fallback-Ketten MÜSSEN deterministisch auflösbar sein.
4. Grapheme DÜRFEN nicht unnötig über mehrere Fonts fragmentiert werden.
5. Komplexe Shaping-Sequenzen MÜSSEN möglichst als Einheit behandelt werden.
6. Script und Sprache MÜSSEN bei der Auswahl berücksichtigt werden können.
7. Stilähnlichkeit SOLL bei mehreren kompatiblen Kandidaten berücksichtigt werden.
8. Vollständige Darstellung MUSS gegenüber Stilähnlichkeit priorisiert werden.
9. Emoji-, Symbol- und Spezialfonts MÜSSEN gezielt priorisierbar sein.
10. Fallback DARF keine Font-Scope-Grenzen umgehen.
11. Fallback-Caches MÜSSEN rekonstruierbar und invalidierbar sein.
12. Fallback-Kette, Auswahlgrund, FontID, Scope und fehlende Coverage MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FONT-MANAGER-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SCRIPT-0001`
- `NPSPEC-TEXT-SHAPING-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann fehlende Glyphen automatisch durch geeignete Fonts ergänzen, ohne die logische Textstruktur zu verändern. Fallback berücksichtigt Unicode-Abdeckung, Script, Sprache, Grapheme, Shaping und Font-Scope und liefert eine deterministische, sichere und möglichst visuell konsistente Darstellung.