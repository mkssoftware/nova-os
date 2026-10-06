# NPSPEC-GLOBALIZATION-NUMBER-0001 – Nova Number Formatting

## Status

Angenommen

## Kategorie

Globalization / Number

## Zweck

NovaOS definiert ein einheitliches Modell für die lokalisierte Darstellung und Eingabe numerischer Werte.

Numerische Daten bleiben intern in kanonischer Form gespeichert. Sprache, Locale und Region beeinflussen ausschließlich Darstellung und Interpretation von Benutzereingaben.

## Grundprinzipien

```text
Stored Number ≠ Display String
Number Value ≠ Locale
Decimal Separator ≠ Numeric Meaning
Formatting ≠ Data Conversion
Localized Input ≠ Internal Representation
```

## Modell

```text
Numeric Value
     ↓
Number Formatter
     +
Locale Context
     ↓
Localized Representation
```

Beispiel:

```text
Value: 1234567.89

de-DE → 1.234.567,89
en-US → 1,234,567.89
fr-FR → 1 234 567,89
```

Der zugrunde liegende numerische Wert bleibt identisch.

## Zahlenformate

NovaOS unterstützt mindestens:

```text
Integer
Decimal
Percentage
Scientific
Compact
Currency Amount
Custom Numeric Format
```

Spezialisierte Formate dürfen auf demselben Number-Formatting-Modell aufbauen.

## Formatierungsregeln

Ein Number Format darf unter anderem bestimmen:

```text
Decimal Separator
Grouping Separator
Grouping Pattern
Minimum Digits
Maximum Digits
Fraction Digits
Sign Representation
Digit Set
Rounding Presentation
```

Diese Eigenschaften werden aus dem effektiven Locale-Kontext und optionalen expliziten Formatierungsoptionen bestimmt.

## Eingabe

Lokalisierte Zahleneingaben müssen anhand des aktiven Eingabekontexts interpretiert werden.

```text
User Input
    ↓
Locale-aware Parser
    ↓
Validation
    ↓
Canonical Numeric Value
```

Mehrdeutige oder ungültige Eingaben dürfen nicht stillschweigend als anderer Wert interpretiert werden.

## Kanonische Darstellung

Maschinenlesbare Formate, Protokolle, persistente Daten und interne APIs dürfen nicht von benutzerspezifischen Number-Formatting-Regeln abhängig sein.

```text
UI                → Locale-aware
Stored Value      → Canonical
Protocol Value    → Protocol-defined
Identity/Data Key → Locale-independent
```

## Ziffernsysteme

NovaOS muss unterschiedliche Unicode-Ziffernsysteme darstellen können.

Die sichtbaren Ziffern dürfen lokalisiert werden, ohne den numerischen Wert zu verändern.

## Rundung

Darstellungsrundung verändert ausschließlich die sichtbare Repräsentation.

```text
Stored:    1.234567
Displayed: 1,23
```

Eine dauerhafte Änderung des numerischen Wertes muss eine explizite Datenoperation sein.

## Laufzeitwechsel

Bei Änderung des Locale-Kontexts sollen sichtbare Zahlen automatisch neu formatiert werden können.

Die zugrunde liegenden Werte dürfen dadurch nicht verändert werden.

## Normative Anforderungen

1. Numerische Werte MÜSSEN unabhängig vom Locale gespeichert werden können.
2. Number Formatting MUSS den effektiven Locale-Kontext berücksichtigen.
3. Dezimal- und Gruppierungszeichen MÜSSEN lokalisierbar sein.
4. Unterschiedliche Gruppierungsmuster MÜSSEN unterstützt werden können.
5. Lokalisierte Eingaben MÜSSEN in kanonische numerische Werte überführbar sein.
6. Mehrdeutige Eingaben DÜRFEN nicht stillschweigend fehlinterpretiert werden.
7. Maschinenlesbare Daten DÜRFEN nicht von UI-Locale-Regeln abhängen.
8. Unterschiedliche Unicode-Ziffernsysteme MÜSSEN unterstützt werden können.
9. Darstellungsrundung DARF den gespeicherten Wert nicht verändern.
10. Locale-Wechsel SOLLEN eine Neuformatierung sichtbarer Zahlen ermöglichen.
11. Explizite Formatierungsoptionen DÜRFEN Locale-Defaults überschreiben.
12. Effektiver Locale-Kontext und verwendete Formatierungsregeln MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-REGION-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`

## Ergebnis

NovaOS trennt numerische Werte konsequent von ihrer lokalisierten Darstellung. Zahlen können entsprechend Sprache, Locale und Region dargestellt und eingegeben werden, während Speicherung, Berechnung, APIs und Protokolle mit stabilen kanonischen Werten arbeiten.