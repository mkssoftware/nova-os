# NPSPEC-TEXT-COLLATION-0001 – Nova Text Collation

## Status

Angenommen

## Kategorie

Text / Collation

## Zweck

NovaOS definiert eine einheitliche Architektur für sprach- und kontextabhängiges Vergleichen und Sortieren von Unicode-Text.

Collation bestimmt die relative Sortierreihenfolge von Text und bleibt von Encoding, Normalisierung, Codepoint-Reihenfolge und stabiler Systemidentität getrennt.

## Grundprinzipien

```text
Collation ≠ Binary Comparison
Collation ≠ Code Point Order
Collation ≠ Normalization
Collation ≠ Case Folding
Collation ≠ Identity
Equal Collation ≠ Identical Text
```

## Architektur

```text
Unicode Text
     ↓
Collation Context
     ↓
Collation Provider
     ↓
Collation Elements
     ↓
Sort Key / Comparison
```

## Modell

```text
CollationContext
├── LocaleID
├── CollationID
├── Strength
├── CasePolicy
├── NumericPolicy
├── PunctuationPolicy
└── ProviderID
```

## Vergleichsstärken

NovaOS muss unterschiedliche Vergleichsstärken unterstützen können:

```text
Primary
Secondary
Tertiary
Identical
```

Typische Bedeutung:

```text
Primary   → Grundbuchstaben
Secondary → Akzente
Tertiary  → Groß-/Kleinschreibung und Varianten
Identical → zusätzliche eindeutige Unterscheidung
```

Die konkrete Semantik wird durch die verwendete Collation definiert.

## Locale-Abhängigkeit

Sortierreihenfolgen können sprachabhängig sein.

```text
Text
 +
Locale
 +
Collation Rules
 ↓
Ordering
```

Derselbe Textbestand darf deshalb unter unterschiedlichen Locales unterschiedliche korrekte Sortierungen besitzen.

Ein Locale darf einen Standard liefern, die konkrete Collation muss jedoch explizit bestimmbar bleiben.

## Sort Keys

Für wiederholte Vergleiche dürfen Sort Keys erzeugt werden:

```text
Unicode Text
     ↓
Collation Provider
     ↓
Sort Key
```

Sort Keys sind abgeleitete Daten und keine stabile Textidentität.

Sie dürfen verworfen und neu erzeugt werden.

## Numerische Sortierung

Optional muss numerische Collation unterstützt werden können.

Beispiel:

```text
file2
file10
```

Numerische Sortierung kann:

```text
file2 < file10
```

ergeben, während eine rein lexikalische Sortierung davon abweichen kann.

## Normalisierung

Collation und Unicode-Normalisierung bleiben getrennte Konzepte.

Ein Collation Provider darf kanonisch äquivalente Texte entsprechend seiner Regeln gleich behandeln, ohne den gespeicherten Originaltext zu verändern.

## Stabilität

Collation-Regeln können sich mit Unicode-, Locale- oder Provider-Versionen ändern.

Persistente Daten dürfen deshalb nicht davon ausgehen, dass ein Sort Key über beliebige Versionen hinweg stabil bleibt.

```text
Sort Key ≠ Persistent Identity
```

Bei relevanten Regeländerungen müssen Sort Keys neu erzeugbar sein.

## Sicherheit

Collation-Gleichheit darf nicht als Sicherheitsidentität verwendet werden.

```text
CollationEqual(A, B)
        ≠
IdentityEqual(A, B)
```

Berechtigungen, CapabilityIDs, ObjectIDs und andere stabile Identitäten dürfen nicht ausschließlich über sprachabhängige Collation verglichen werden.

## Normative Anforderungen

1. NovaOS MUSS Unicode-basierte Collation unterstützen.
2. Collation und binärer Vergleich MÜSSEN getrennte Vergleichsarten bleiben.
3. Locale-abhängige Sortierreihenfolgen MÜSSEN unterstützt werden können.
4. Vergleichsstärken MÜSSEN explizit auswählbar sein.
5. Numerische Collation MUSS optional unterstützt werden können.
6. Case- und Interpunktionsregeln MÜSSEN konfigurierbar sein können.
7. Collation DARF den gespeicherten Originaltext nicht verändern.
8. Sort Keys DÜRFEN als Performanceoptimierung verwendet werden.
9. Sort Keys MÜSSEN als abgeleitete und rekonstruierbare Daten behandelt werden.
10. Collation-Gleichheit DARF nicht mit stabiler Identität gleichgesetzt werden.
11. Änderungen von Unicode-, Locale- oder Provider-Regeln MÜSSEN behandelbar sein.
12. LocaleID, CollationID, Stärke, Optionen, Provider und Regelversion MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-NORMALIZATION-0001`
- `NPSPEC-TEXT-SEGMENTATION-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-COLLATION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine einheitliche Unicode-basierte Collation-Schicht für sprachgerechtes Vergleichen und Sortieren von Text. Sortierreihenfolge, Vergleichsstärke und Locale können explizit bestimmt werden, während Collation konsequent von Textidentität, Speicherung und sicherheitskritischen Vergleichen getrennt bleibt.