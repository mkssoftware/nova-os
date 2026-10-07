# NPSPEC-TEXT-BIDI-0001 – Nova Bidirectional Text

## Status

Angenommen

## Kategorie

Text / Bidirectional

## Zweck

NovaOS definiert die Verarbeitung bidirektionaler Unicode-Texte, in denen links-nach-rechts und rechts-nach-links geschriebene Inhalte gemeinsam auftreten.

Die logische Zeichenreihenfolge bleibt von der visuellen Darstellungsreihenfolge getrennt.

## Grundprinzipien

```text
Logical Order ≠ Visual Order
BiDi ≠ Text Reversal
RTL ≠ Mirrored String
Writing Direction ≠ Language
Storage Order ≠ Display Order
Visual Position ≠ Logical Index
```

## Architektur

```text
Unicode Text
     ↓
Logical Order
     ↓
BiDi Properties
     ↓
Unicode BiDi Algorithm
     ↓
Directional Runs
     ↓
Text Layout
     ↓
Visual Order
```

Text wird weiterhin in logischer Reihenfolge gespeichert.

## Schreibrichtungen

NovaOS unterstützt:

```text
LTR
RTL
Mixed Direction
```

Beispiele für RTL-Schriften sind Arabisch und Hebräisch.

Zahlen, Satzzeichen, technische Bezeichner und eingebettete LTR-Texte können innerhalb eines RTL-Kontexts eigene Richtungen besitzen.

## Base Direction

Jeder Textkontext muss eine Basisrichtung bestimmen können:

```text
LTR
RTL
Auto
```

`Auto` darf die Richtung anhand definierter Unicode-Regeln bestimmen.

Locale oder Sprache können einen Standard liefern, ersetzen aber nicht die eigentliche BiDi-Verarbeitung.

## Directional Runs

Der Unicode-BiDi-Algorithmus zerlegt Text in gerichtete Bereiche:

```text
Logical Text
    ↓
BiDi Resolution
    ↓
LTR Run
RTL Run
LTR Run
    ↓
Visual Layout
```

Diese Runs dürfen anschließend durch die Text-Layout- und Shaping-Schicht verarbeitet werden.

## Cursor und Auswahl

Cursorbewegung muss zwischen:

```text
Logical Navigation
Visual Navigation
```

unterscheiden können.

Auswahlbereiche werden logisch auf Textpositionen abgebildet, auch wenn ihre visuelle Darstellung nicht kontinuierlich erscheint.

## Editing

Textbearbeitung muss die logische Reihenfolge erhalten.

Operationen wie:

```text
Insert
Delete
Selection
Copy
Paste
Cursor Movement
```

dürfen nicht auf einer künstlich umgedrehten Zeichenfolge arbeiten.

## Mirroring

Bestimmte Zeichen dürfen in RTL-Kontexten visuell gespiegelt werden.

```text
( )
[ ]
{ }
```

Mirroring ist eine Darstellungsoperation und verändert nicht automatisch den gespeicherten Unicode-Codepoint.

## BiDi-Steuerzeichen

Unicode-BiDi-Steuerzeichen und Isolates müssen unterstützt werden.

Besonders relevant sind:

```text
LRI
RLI
FSI
PDI
LRE
RLE
LRO
RLO
PDF
```

Isolates sollen bevorzugt werden, wenn eingebettete Inhalte voneinander getrennt werden müssen.

## Sicherheit

Unsichtbare BiDi-Steuerzeichen können die visuelle Interpretation von Text verändern.

NovaOS muss dies insbesondere berücksichtigen bei:

```text
Source Code
File Names
URLs
Identifiers
Security Dialogs
Logs
Commands
```

Sicherheitsrelevante Oberflächen dürfen verdächtige Richtungssteuerung sichtbar machen, markieren oder ablehnen.

```text
Displayed Text ≠ Trusted Identity
```

## Integration

BiDi-Verarbeitung muss mit folgenden Schichten zusammenspielen:

```text
Unicode
Grapheme Segmentation
Text Segmentation
Shaping
Font Fallback
Text Layout
Globalization
Accessibility
```

Die BiDi-Schicht selbst erzeugt keine Glyphen.

## Normative Anforderungen

1. NovaOS MUSS den Unicode Bidirectional Algorithm unterstützen.
2. Text MUSS grundsätzlich in logischer Reihenfolge gespeichert werden.
3. Logische und visuelle Reihenfolge MÜSSEN getrennte Konzepte bleiben.
4. LTR-, RTL- und gemischte Texte MÜSSEN unterstützt werden.
5. Textkontexte MÜSSEN eine explizite oder automatisch bestimmte Basisrichtung besitzen können.
6. Cursor und Auswahl MÜSSEN bidirektionale Texte korrekt behandeln können.
7. Textbearbeitung DARF keine physisch umgedrehten Strings voraussetzen.
8. Unicode BiDi Isolates MÜSSEN unterstützt werden.
9. Zeichen-Mirroring MUSS von der gespeicherten Zeichenidentität getrennt bleiben.
10. Unsichtbare BiDi-Steuerzeichen MÜSSEN sicherheitsrelevant erkennbar sein.
11. Sicherheitskritische Oberflächen MÜSSEN BiDi-Spoofing kontrolliert behandeln können.
12. Basisrichtung, Directional Runs, BiDi-Level und relevante Steuerzeichen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SEGMENTATION-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-RTL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS verarbeitet links-nach-rechts, rechts-nach-links und gemischte Unicode-Texte über eine gemeinsame bidirektionale Textarchitektur. Logische Speicherung, visuelle Reihenfolge, Cursorverhalten und Sicherheitsbehandlung bleiben dabei klar getrennt, sodass auch komplexe mehrsprachige Texte konsistent und sicher dargestellt werden können.