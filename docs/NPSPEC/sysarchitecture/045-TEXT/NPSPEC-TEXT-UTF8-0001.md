# NPSPEC-TEXT-UTF8-0001 – Nova UTF-8 Encoding

## Status

Angenommen

## Kategorie

Text / Encoding

## Zweck

NovaOS verwendet UTF-8 als bevorzugte systemweite Bytekodierung für Unicode-Text.

UTF-8 bildet Unicode-Codepoints auf Bytefolgen ab, ohne selbst Textsemantik, Grapheme, Sprache oder Darstellung zu definieren.

## Grundprinzipien

```text
UTF-8 ≠ Unicode
Byte ≠ Character
Code Unit ≠ Code Point
Code Point ≠ Grapheme
Valid Bytes ≠ Valid Application Data
Encoding ≠ Text Semantics
```

## Architektur

```text
Byte Stream
    ↓
UTF-8 Validation
    ↓
UTF-8 Decoding
    ↓
Unicode Code Points
    ↓
Unicode Text Model
```

Beim Schreiben gilt entsprechend:

```text
Unicode Code Points
        ↓
UTF-8 Encoding
        ↓
Byte Stream
```

## Kodierung

Ein Unicode-Codepoint wird durch ein bis vier Bytes dargestellt:

```text
U+0000   – U+007F   → 1 Byte
U+0080   – U+07FF   → 2 Bytes
U+0800   – U+FFFF   → 3 Bytes
U+10000  – U+10FFFF → 4 Bytes
```

ASCII bleibt dadurch direkt kompatibel:

```text
UTF-8 ASCII Range = ASCII Byte Representation
```

## Systemweite Verwendung

UTF-8 soll standardmäßig verwendet werden für:

```text
Dateinamen
Konfigurationsdaten
Manifeste
APIs
IPC
Logs
Shell
NovaLang
.nlf
.nui
Metadaten
Netzwerkformate
```

Protokolle oder Dateiformate mit vorgeschriebener anderer Kodierung bleiben davon unberührt.

## Validierung

Eingehende UTF-8-Daten müssen validierbar sein.

Ungültig sind insbesondere:

```text
Invalid Continuation Bytes
Truncated Sequences
Overlong Encodings
Surrogate Code Points
Code Points > U+10FFFF
Structurally Invalid Sequences
```

Overlong Encodings dürfen nicht akzeptiert werden.

## Fehlerbehandlung

Je nach Schnittstelle darf ein Decoder:

```text
Reject
Replace
Report
Preserve Raw Input Separately
```

verwenden.

Fehlerhafte Eingaben dürfen nicht stillschweigend als gültiger UTF-8-Text interpretiert werden.

## Byte-Länge

APIs müssen zwischen verschiedenen Längen unterscheiden können:

```text
ByteLength
CodePointCount
GraphemeCount
DisplayWidth
```

Beispiel:

```text
ByteLength ≠ CharacterCount
```

Byte-Indizes dürfen nicht automatisch als Zeichenpositionen interpretiert werden.

## Random Access

UTF-8 ist variabel lang kodiert.

Direkter Zugriff auf das `n`-te Byte entspricht daher nicht dem Zugriff auf das `n`-te Unicode-Zeichen.

Textsysteme dürfen bei Bedarf zusätzliche Indizes oder Datenstrukturen verwenden.

## Normalisierung

UTF-8 definiert keine Unicode-Normalisierung.

```text
Same Visual Text
      ↓
Different Valid UTF-8 Sequences
```

NFC, NFD, NFKC und NFKD werden durch das Unicode-Textmodell behandelt.

## BOM

Eine UTF-8-BOM darf erkannt werden, ist für NovaOS-interne UTF-8-Daten jedoch grundsätzlich nicht erforderlich.

Neue NovaOS-native Textformate sollen UTF-8 ohne BOM verwenden, sofern ihre jeweilige Spezifikation nichts anderes definiert.

## Sicherheit

Decoder müssen strikt gegen fehlerhafte oder mehrdeutige Bytefolgen arbeiten.

Unterschiedliche Systemkomponenten dürfen dieselbe ungültige Bytefolge nicht widersprüchlich als verschiedene gültige Zeichen interpretieren.

Dadurch werden unter anderem Parser-, Pfad- und Sicherheitsabweichungen reduziert.

## Normative Anforderungen

1. NovaOS SOLL UTF-8 als bevorzugte systemweite Unicode-Bytekodierung verwenden.
2. UTF-8 und Unicode MÜSSEN getrennte Konzepte bleiben.
3. UTF-8-Decoder MÜSSEN strukturell ungültige Sequenzen erkennen.
4. Overlong Encodings DÜRFEN nicht als gültiges UTF-8 akzeptiert werden.
5. Surrogate Codepoints DÜRFEN nicht als gültige UTF-8-Codepoints akzeptiert werden.
6. Codepoints oberhalb `U+10FFFF` DÜRFEN nicht akzeptiert werden.
7. Fehlerbehandlung MUSS für die jeweilige Schnittstelle eindeutig definiert sein.
8. ByteLength und Textlänge DÜRFEN nicht gleichgesetzt werden.
9. UTF-8-Dekodierung DARF keine implizite Unicode-Normalisierung durchführen.
10. NovaOS-native Textformate SOLLEN UTF-8 ohne BOM verwenden.
11. Fremdformate MÜSSEN ihre vorgeschriebene Kodierung weiterhin verwenden können.
12. Encoding-, Validierungs- und Fehlerzustände MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt mit UTF-8 eine einheitliche, kompakte und Unicode-kompatible Standardkodierung für Text. Die Architektur trennt Bytekodierung konsequent von Unicode-Semantik und verhindert, dass Bytepositionen, Codepoints, Grapheme oder visuelle Zeichen miteinander verwechselt werden.