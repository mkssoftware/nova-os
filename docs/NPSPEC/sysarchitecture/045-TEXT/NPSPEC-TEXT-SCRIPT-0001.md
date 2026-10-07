# NPSPEC-TEXT-SCRIPT-0001 – Nova Unicode Script Model

## Status

Angenommen

## Kategorie

Text / Script

## Zweck

NovaOS definiert eine einheitliche Behandlung von Unicode-Schriften (`Scripts`).

Scripts beschreiben das Schriftsystem eines Unicode-Zeichens und dienen unter anderem Textanalyse, Font-Auswahl, Shaping, Segmentierung und Sicherheitsprüfung.

## Grundprinzipien

```text
Script ≠ Language
Script ≠ Locale
Script ≠ Font
Script ≠ Writing Direction
Script ≠ Encoding
One Language → Multiple Scripts possible
One Script → Multiple Languages possible
```

## Modell

```text
ScriptInfo
├── ScriptID
├── UnicodeScript
├── ScriptExtensions[]
├── DirectionHint
├── ShapingClass
└── State
```

`ScriptID` basiert auf stabilen Unicode-Script-Eigenschaften und nicht auf lokalisierten Namen.

## Architektur

```text
Unicode Code Points
        ↓
Script Properties
        ↓
Script Resolution
        ↓
Script Runs
        ↓
Shaping / Font Selection / Layout
```

## Script-Erkennung

Unicode-Codepoints besitzen Script-Eigenschaften wie:

```text
Latin
Cyrillic
Greek
Arabic
Hebrew
Devanagari
Han
Hiragana
Katakana
Hangul
```

NovaOS muss außerdem Sonderwerte behandeln:

```text
Common
Inherited
Unknown
```

`Common` und `Inherited` dürfen nicht willkürlich einer Sprache zugeordnet werden.

## Script Extensions

Zeichen können in mehreren Schriftsystemen verwendet werden.

NovaOS muss deshalb neben der primären Script-Eigenschaft auch Unicode `Script_Extensions` berücksichtigen können.

```text
Code Point
   ↓
Script
+
Script Extensions
   ↓
Contextual Resolution
```

## Script Runs

Für Textverarbeitung darf Text in zusammenhängende Script Runs zerlegt werden:

```text
Latin Run
Arabic Run
Han Run
Latin Run
```

Script Runs dienen insbesondere als Eingabe für Shaping und Font-Auswahl.

Sie verändern den ursprünglichen Text nicht.

## Sprache

Script und Sprache bleiben getrennt.

Beispiel:

```text
Latin Script
├── Deutsch
├── Englisch
├── Französisch
└── viele weitere Sprachen
```

Ebenso kann eine Sprache in unterschiedlichen Scripts geschrieben werden.

Sprachinformationen dürfen Script-Erkennung ergänzen, aber nicht ersetzen.

## Schreibrichtung

Ein Script kann eine typische Schreibrichtung besitzen.

Diese Information ist jedoch nur ein Hinweis.

Die tatsächliche visuelle Richtung wird durch die BiDi- und Layout-Schicht bestimmt.

```text
Script Direction Hint
        ≠
Resolved BiDi Direction
```

## Shaping

Script-Informationen werden an die Shaping-Schicht weitergegeben:

```text
Text
 ↓
Script Run
 ↓
Language Context
 ↓
Shaping
 ↓
Glyph Sequence
```

Das Script-Modell selbst erzeugt keine Glyphen.

## Font-Auswahl

Font Fallback darf Script-Informationen verwenden:

```text
Script
  ↓
Required Character Coverage
  ↓
Font Selection
```

Script-Zugehörigkeit allein garantiert jedoch nicht, dass ein Font alle benötigten Zeichen enthält.

## Sicherheit

Gemischte Scripts können für visuell ähnliche Bezeichner missbraucht werden.

NovaOS muss insbesondere erkennen können:

```text
Mixed-Script Identifiers
Confusable Characters
Unexpected Script Changes
Invisible Characters
Script Spoofing
```

Ein Script-Wechsel ist nicht automatisch bösartig und darf nicht pauschal verboten werden.

Sicherheitskritische Kontexte dürfen strengere Policies verwenden.

## Normative Anforderungen

1. NovaOS MUSS Unicode Script Properties unterstützen.
2. Script und Sprache MÜSSEN getrennte Konzepte bleiben.
3. Script und Schreibrichtung MÜSSEN getrennte Konzepte bleiben.
4. `Common`, `Inherited` und `Unknown` MÜSSEN explizit behandelbar sein.
5. Unicode `Script_Extensions` MÜSSEN unterstützt werden können.
6. Text MUSS in Script Runs segmentierbar sein.
7. Script Runs DÜRFEN den ursprünglichen Text nicht verändern.
8. Script-Informationen MÜSSEN für Shaping und Font-Auswahl verfügbar sein.
9. BiDi-Verarbeitung DARF nicht durch einfache Script-Richtung ersetzt werden.
10. Mixed-Script-Inhalte MÜSSEN sicherheitsrelevant analysierbar sein.
11. Script-Wechsel DÜRFEN nicht grundsätzlich als Fehler behandelt werden.
12. Script, Script Extensions, Runs und verwendete Unicode-Version MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SEGMENTATION-0001`
- `NPSPEC-TEXT-BIDI-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Unicode-Text anhand seiner Schriftsysteme analysieren und in Script Runs aufteilen. Sprache, Schreibrichtung, Font und Script bleiben dabei getrennte Konzepte, während Shaping, Font Fallback und Sicherheitsprüfungen auf eine gemeinsame Script-Semantik zugreifen können.