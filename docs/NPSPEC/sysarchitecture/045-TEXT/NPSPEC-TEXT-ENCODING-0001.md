# NPSPEC-TEXT-ENCODING-0001 – Nova Text Encoding

## Status

Angenommen

## Kategorie

Text / Encoding

## Zweck

NovaOS definiert eine einheitliche Architektur zum Erkennen, Dekodieren, Kodieren und Konvertieren unterschiedlicher Textkodierungen.

Unicode bildet das interne Zeichenmodell. UTF-8 ist die bevorzugte systemweite Kodierung, während andere Kodierungen über explizite Encoding-Provider unterstützt werden.

## Grundprinzipien

```text
Encoding ≠ Unicode
Encoding ≠ Language
Encoding ≠ Locale
Encoding ≠ Normalization
Byte Sequence ≠ Text
Detection ≠ Certainty
Conversion ≠ Semantic Transformation
```

## Architektur

```text
Byte Stream
    ↓
Encoding Identification
    ↓
Decoder
    ↓
Unicode Text Model
    ↓
Encoder
    ↓
Target Byte Stream
```

Interne Textverarbeitung soll grundsätzlich auf Unicode erfolgen.

## Encoding-Modell

```text
TextEncoding
├── EncodingID
├── CanonicalName
├── Aliases[]
├── ProviderID
├── CodeUnitSize
├── BOMSupport
├── Stateful
├── RoundTripCapability
└── State
```

`EncodingID` muss unabhängig von lokalisierten Anzeigenamen sein.

## Unterstützte Kodierungen

NovaOS darf über Provider unter anderem unterstützen:

```text
UTF-8
UTF-16LE
UTF-16BE
UTF-32LE
UTF-32BE
ASCII
ISO-8859 Families
Windows Code Pages
Legacy Encodings
Protocol-Specific Encodings
```

UTF-8 bleibt die bevorzugte Kodierung für NovaOS-native Textdaten.

## Dekodierung

```text
Encoded Bytes
     ↓
Decoder
     ↓
Validation
     ↓
Unicode Code Points
```

Decoder müssen ungültige oder nicht darstellbare Sequenzen erkennen können.

## Kodierung

```text
Unicode Code Points
        ↓
Encoder
        ↓
Target Encoding
```

Kann ein Unicode-Codepoint in der Zielkodierung nicht dargestellt werden, muss eine explizite Fehlerstrategie verwendet werden.

## Fehlerstrategien

Unterstützte Strategien dürfen sein:

```text
Strict
Replace
Report
Skip
```

`Skip` und `Replace` dürfen nicht stillschweigend für sicherheitskritische Daten verwendet werden.

## Encoding-Erkennung

Kodierungen dürfen anhand folgender Informationen bestimmt werden:

```text
Explicit Metadata
Protocol Declaration
File Format
BOM
User Selection
Validated Heuristic Detection
```

Priorität soll expliziten Informationen vor heuristischer Erkennung gegeben werden.

```text
Explicit Declaration
        >
Format / Protocol
        >
BOM
        >
Heuristic Guess
```

Heuristische Erkennung muss als unsicher gekennzeichnet werden können.

## BOM

Byte Order Marks dürfen erkannt und entsprechend der jeweiligen Kodierung behandelt werden.

Ein BOM darf nicht automatisch als universelle Encoding-Angabe für Formate interpretiert werden, deren Spezifikation eine andere Semantik definiert.

## Konvertierung

Encoding-Konvertierung erfolgt grundsätzlich über Unicode:

```text
Encoding A
    ↓
Unicode
    ↓
Encoding B
```

Direkte Legacy-zu-Legacy-Konvertierungen dürfen intern optimiert werden, sofern das Ergebnis semantisch identisch bleibt.

## Round Trip

Nicht jede Konvertierung ist verlustfrei.

```text
Encoding A
    ↓
Unicode
    ↓
Encoding B
    ↓
Possible Information Loss
```

NovaOS muss erkennen können, wenn Zeichen in der Zielkodierung nicht repräsentierbar sind.

## Stateful Encodings

Provider dürfen zustandsbehaftete Kodierungen unterstützen.

Decoderzustände müssen an Stream-Grenzen explizit verwaltet und dürfen nicht unbeabsichtigt zwischen unabhängigen Textströmen geteilt werden.

## Sicherheit

Encoding-Unterschiede dürfen nicht zu unterschiedlichen Interpretationen sicherheitskritischer Daten führen.

Besonders relevant sind:

```text
Invalid Sequences
Encoding Confusion
Overlong Representations
Truncated Input
Mixed Encodings
Decoder Differences
```

Sicherheitsgrenzen sollen Text erst nach eindeutiger Dekodierung und Validierung interpretieren.

## Normative Anforderungen

1. NovaOS MUSS Unicode als internes Zeichenmodell verwenden.
2. UTF-8 SOLL die bevorzugte systemweite Textkodierung sein.
3. Weitere Kodierungen MÜSSEN über austauschbare Provider unterstützt werden können.
4. EncodingID MUSS stabil und sprachunabhängig sein.
5. Dekodierung und Kodierung MÜSSEN explizite Fehlerstrategien unterstützen.
6. Explizite Encoding-Angaben SOLLEN Vorrang vor heuristischer Erkennung besitzen.
7. Heuristisch erkannte Kodierungen MÜSSEN als unsicher kennzeichenbar sein.
8. Encoding-Konvertierungen SOLLEN über Unicode erfolgen.
9. Nicht darstellbare Zeichen MÜSSEN erkannt werden können.
10. Verlustbehaftete Konvertierungen DÜRFEN nicht stillschweigend als verlustfrei gelten.
11. Zustandsbehaftete Decoder MÜSSEN ihren Zustand isoliert verwalten.
12. Encoding, Provider, Erkennungsmethode, Confidence und Fehlerzustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine einheitliche Encoding-Schicht, die externe und historische Textkodierungen kontrolliert in das Unicode-Textmodell überführt. UTF-8 bleibt der bevorzugte Standard, während andere Kodierungen explizit, validierbar und ohne Vermischung von Encoding-, Sprach- und Textsemantik unterstützt werden.