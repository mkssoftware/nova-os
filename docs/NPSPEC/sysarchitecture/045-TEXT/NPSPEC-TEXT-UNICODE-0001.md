# NPSPEC-TEXT-UNICODE-0001 – Nova Unicode Text Model

## Status

Angenommen

## Kategorie

Text / Unicode

## Zweck

NovaOS verwendet Unicode als einheitliches systemweites Zeichenmodell.

Text darf intern nicht von Sprache, Schrift, Tastaturlayout, Dateiformat oder Darstellungssystem abhängig sein.

## Grundprinzipien

```text
Character ≠ Byte
Character ≠ Code Point
Code Point ≠ Grapheme
Grapheme ≠ Glyph
Unicode ≠ UTF-8
Text Identity ≠ Visual Appearance
```

## Textmodell

NovaOS unterscheidet:

```text
Byte Sequence
     ↓
Encoding
     ↓
Unicode Code Points
     ↓
Grapheme Clusters
     ↓
Text Layout
     ↓
Glyphs
```

Diese Ebenen dürfen nicht implizit gleichgesetzt werden.

## Interne Kodierung

UTF-8 ist die bevorzugte systemweite Unicode-Kodierung für gespeicherten und übertragenen Text.

```text
Unicode Text
     ↕
UTF-8
```

Subsysteme dürfen intern andere Repräsentationen verwenden, sofern ihre Schnittstellen Unicode-Semantik erhalten.

## Code Points

Unicode-Codepoints werden unabhängig von ihrer konkreten UTF-Kodierung behandelt.

Beispiel:

```text
U+0041 → A
U+00E4 → ä
U+1F680 → 🚀
```

APIs dürfen nicht voraussetzen, dass ein Zeichen genau einem Byte entspricht.

## Grapheme Cluster

Benutzersichtbare Zeichen können aus mehreren Codepoints bestehen.

```text
Code Point + Combining Mark
           ↓
Extended Grapheme Cluster
           ↓
User-Perceived Character
```

Cursorbewegung, Auswahl, Löschen und Textbearbeitung sollen Grapheme Cluster berücksichtigen.

## Normalisierung

NovaOS unterstützt Unicode-Normalisierungsformen:

```text
NFC
NFD
NFKC
NFKD
```

Normalisierung darf nur dort erfolgen, wo die jeweilige Semantik dies verlangt.

Text darf nicht global oder stillschweigend normalisiert werden.

## Vergleich

Textvergleich muss den jeweiligen Zweck berücksichtigen:

```text
Binary Comparison
Code Point Comparison
Normalized Comparison
Case-Insensitive Comparison
Locale-Aware Comparison
```

Es gibt keinen universellen Textvergleich für alle Anwendungsfälle.

## Groß-/Kleinschreibung

Unicode Case Mapping und Case Folding müssen unterstützt werden.

```text
Case Mapping ≠ Locale-Neutral Formatting
Case Folding ≠ Display Transformation
```

Sprachabhängige Sonderfälle müssen über die Globalization-Schicht behandelt werden.

## Ungültige Eingaben

Ungültige Bytefolgen müssen kontrolliert behandelt werden.

```text
Input Bytes
    ↓
UTF Validation
    ↓
Valid Unicode
oder
Explicit Error / Replacement
```

Ungültige Daten dürfen keine Speicher- oder Parserunsicherheit verursachen.

## Dateisystem und Identität

Unicode-Darstellung darf nicht mit Objektidentität verwechselt werden.

```text
Display Name ≠ ObjectID
Localized Name ≠ Internal Identity
```

Unterschiedliche Unicode-Sequenzen können visuell identisch erscheinen und müssen sicher behandelt werden.

## Sicherheit

NovaOS muss Unicode-bezogene Angriffsflächen berücksichtigen, insbesondere:

```text
Confusable Characters
Bidirectional Control Characters
Invisible Characters
Mixed Scripts
Normalization Differences
Malformed Encoding
```

Sicherheitskritische Identitäten dürfen nicht ausschließlich von visuell dargestelltem Text abhängen.

## Normative Anforderungen

1. NovaOS MUSS Unicode systemweit unterstützen.
2. UTF-8 SOLL die bevorzugte persistente und übertragene Textkodierung sein.
3. APIs DÜRFEN Zeichen nicht mit Bytes gleichsetzen.
4. Codepoints, Grapheme Cluster und Glyphen MÜSSEN getrennte Konzepte bleiben.
5. Textbearbeitung SOLL Extended Grapheme Clusters berücksichtigen.
6. Unicode-Normalisierung MUSS unterstützt werden.
7. Normalisierung DARF nicht ohne definierte Semantik global erzwungen werden.
8. Unterschiedliche Vergleichssemantiken MÜSSEN explizit auswählbar sein.
9. Unicode Case Folding und Case Mapping MÜSSEN unterstützt werden.
10. Ungültige Kodierungen MÜSSEN sicher behandelbar sein.
11. Unicode-Text DARF nicht als alleinige sicherheitskritische Identität verwendet werden.
12. Encoding, Normalisierung und relevante Unicode-Eigenschaften MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-COLLATION-0001`
- `NPSPEC-GLOBALIZATION-RTL-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein durchgängiges Unicode-basiertes Textmodell, das Bytes, Codepoints, Grapheme Cluster und Glyphen klar trennt. Dadurch können internationale Texte korrekt gespeichert, verarbeitet, verglichen und dargestellt werden, ohne Textdarstellung mit Identität oder Sicherheit zu vermischen.