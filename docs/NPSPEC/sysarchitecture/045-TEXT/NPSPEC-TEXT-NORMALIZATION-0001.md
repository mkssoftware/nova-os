# NPSPEC-TEXT-NORMALIZATION-0001 – Nova Unicode Normalization

## Status

Angenommen

## Kategorie

Text / Normalization

## Zweck

NovaOS definiert die kontrollierte Unicode-Normalisierung von Text.

Unicode-Sequenzen, die denselben oder kompatiblen Text repräsentieren, dürfen bei Bedarf in eine definierte Normalisierungsform überführt werden. Normalisierung erfolgt ausschließlich dort, wo die jeweilige Semantik dies verlangt.

## Grundprinzipien

```text
Normalization ≠ Encoding
Normalization ≠ Case Folding
Normalization ≠ Collation
Normalization ≠ Transliteration
Canonical Equivalent ≠ Byte Identical
Visual Equality ≠ Semantic Equality
```

## Normalisierungsformen

NovaOS unterstützt:

```text
NFC
NFD
NFKC
NFKD
```

Dabei gilt:

```text
NFC  → Canonical Composition
NFD  → Canonical Decomposition
NFKC → Compatibility Composition
NFKD → Compatibility Decomposition
```

## Architektur

```text
Unicode Text
     ↓
Normalization Policy
     ↓
Normalization Engine
     ↓
Normalized Unicode Text
```

Die Normalisierung arbeitet auf Unicode-Codepoints und nicht auf UTF-8-Bytes.

## Kanonische Äquivalenz

Unterschiedliche Codepoint-Sequenzen können kanonisch denselben Text darstellen.

Beispiel:

```text
U+00E4

und

U+0061 U+0308
```

können beide das Graphem:

```text
ä
```

repräsentieren.

Byte- oder Codepoint-Gleichheit darf daher nicht automatisch mit kanonischer Textgleichheit gleichgesetzt werden.

## NFC

NFC soll die bevorzugte Normalisierungsform sein, wenn NovaOS-native Daten eine kanonisch normalisierte Textdarstellung benötigen.

Dies bedeutet nicht, dass sämtlicher Text systemweit automatisch nach NFC konvertiert wird.

## Kompatibilitätsnormalisierung

NFKC und NFKD können Darstellungsunterschiede entfernen, die semantisch relevant sein können.

Sie dürfen daher nur verwendet werden, wenn der jeweilige Anwendungsfall dies ausdrücklich verlangt.

```text
NFKC / NFKD
      ↓
Possible Semantic Loss
```

## Identität

Normalisierung darf nicht unkontrolliert zur Bestimmung systemweiter Identitäten verwendet werden.

Insbesondere gilt:

```text
Normalized Name ≠ ObjectID
Normalized Text ≠ Principal Identity
Visual Name ≠ CapabilityID
```

Stabile NovaOS-Identitäten bleiben von Textdarstellungen getrennt.

## Vergleich

Textvergleich darf bei Bedarf eine definierte Normalisierung verlangen:

```text
Text A ─→ Normalize ─┐
                     ├→ Compare
Text B ─→ Normalize ─┘
```

Die verwendete Normalisierungsform muss Teil der Vergleichssemantik sein.

## Speicherung

Subsysteme müssen explizit festlegen, ob sie:

```text
Original Text Preserve
Normalize on Input
Normalize on Storage
Normalize on Comparison
```

verwenden.

Eine globale automatische Normalisierung ist nicht zulässig.

## Sicherheit

Normalisierungsunterschiede können sicherheitsrelevante Mehrdeutigkeiten erzeugen.

NovaOS muss insbesondere berücksichtigen:

```text
Canonical Equivalence
Compatibility Characters
Combining Marks
Confusable Characters
Identifier Spoofing
Normalization Mismatch
```

Sicherheitsgrenzen müssen dieselbe definierte Normalisierungssemantik verwenden.

## Stabilität

Normalisierungsoperationen müssen idempotent sein:

```text
Normalize(Normalize(Text)) = Normalize(Text)
```

Die verwendete Unicode-Version muss nachvollziehbar sein, wenn sie das Ergebnis beeinflussen kann.

## Normative Anforderungen

1. NovaOS MUSS NFC, NFD, NFKC und NFKD unterstützen.
2. Normalisierung MUSS auf Unicode-Semantik basieren.
3. Normalisierung und Encoding MÜSSEN getrennte Operationen bleiben.
4. NovaOS DARF Text nicht global und stillschweigend normalisieren.
5. NFC SOLL bevorzugt werden, wenn eine kanonische Normalform erforderlich ist.
6. NFKC und NFKD DÜRFEN nur bei explizit geeigneter Semantik verwendet werden.
7. Die gewählte Normalisierungsform MUSS für relevante Vergleiche eindeutig definiert sein.
8. Normalisierung DARF stabile Systemidentitäten nicht ersetzen.
9. Originaltext MUSS erhaltbar bleiben können, wenn dessen exakte Darstellung relevant ist.
10. Normalisierungsoperationen MÜSSEN idempotent sein.
11. Sicherheitskritische Vergleiche MÜSSEN eine konsistente Normalisierungsstrategie verwenden.
12. Normalisierungsform, Unicode-Version und relevanter Normalisierungszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-ENCODING-0001`
- `NPSPEC-GLOBALIZATION-COLLATION-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Unicode-Text kontrolliert in standardisierte Normalisierungsformen überführen, ohne Normalisierung mit Encoding, Vergleich oder Identität zu vermischen. Dadurch können kanonisch äquivalente Texte zuverlässig verarbeitet werden, während Originaldarstellung und sicherheitsrelevante Semantik erhalten bleiben.