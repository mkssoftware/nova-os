# NPSPEC-GLOBALIZATION-COLLATION-0001 – Nova Collation

## Status

Angenommen

## Kategorie

Globalization / Collation

## Zweck

NovaOS definiert ein einheitliches Modell für sprach- und localeabhängiges Sortieren und Vergleichen von Text.

Collation dient ausschließlich der semantisch passenden Reihenfolge und Suche für Benutzeroberflächen und Datenansichten. Sicherheits-, Identitäts- und Protokollvergleiche bleiben davon unabhängig.

## Grundprinzipien

```text
Collation ≠ Binary Comparison
Collation ≠ Identity
Sort Order ≠ Storage Order
Locale Equality ≠ Identity Equality
Display Order ≠ Canonical Order
Collation Key ≠ Object Identity
```

## Modell

```text
CollationContext
├── CollationID
├── Locale
├── Rules
├── Strength
├── Options
└── Version
```

`CollationID` identifiziert einen definierten Satz von Sortier- und Vergleichsregeln.

## Vergleich

Ein localeabhängiger Vergleich erfolgt über den effektiven Collation Context:

```text
Text A
   +
Text B
   +
Collation Context
      ↓
Compare
      ↓
Less / Equal / Greater
```

Das Ergebnis beschreibt ausschließlich die Sortierreihenfolge innerhalb dieses Kontexts.

## Vergleichsstärke

Collation darf unterschiedliche Vergleichsstärken unterstützen:

```text
Primary
Secondary
Tertiary
Identical
```

Damit kann beispielsweise bestimmt werden, ob Unterschiede bei:

```text
Grundzeichen
Akzenten
Groß-/Kleinschreibung
Unicode-Repräsentation
```

für einen Vergleich relevant sind.

## Sortierung

Benutzeroberflächen dürfen localeabhängig sortieren:

```text
Dateinamen
Kontakte
Suchergebnisse
Listen
Tabellen
Kategorien
```

Die physische Speicherreihenfolge und ObjectIDs bleiben davon unberührt.

## Optionen

Ein Collation Context darf zusätzliche Regeln definieren:

```text
Case Sensitivity
Accent Sensitivity
Numeric Ordering
Punctuation Handling
Whitespace Handling
Script Ordering
```

Beispielsweise darf Numeric Ordering:

```text
file2
file10
```

in numerisch sinnvoller Reihenfolge darstellen.

## Collation Keys

Für häufige Vergleiche dürfen optimierte Collation Keys erzeugt werden:

```text
Text
 ↓
Collation Rules
 ↓
Collation Key
```

Collation Keys sind abgeleitete Daten und dürfen nicht als dauerhafte Identität verwendet werden.

Sie müssen bei relevanten Änderungen der Collation-Version neu erzeugbar sein.

## Suche

Textsuche darf dieselben Collation-Regeln verwenden, wenn sprachabhängige Gleichheit oder Reihenfolge gewünscht ist.

Suchlogik muss jedoch zwischen exaktem und localeabhängigem Vergleich unterscheiden können.

## Sicherheitsrelevante Vergleiche

Folgende Werte dürfen nicht über localeabhängige Collation verglichen werden:

```text
ObjectID
CapabilityID
SolutionID
ServiceID
Registry Identity
Cryptographic Identifier
Protocol Identifier
Security Principal
```

Für diese gelten definierte kanonische Vergleichsregeln.

## Versionierung

Collation-Regeln müssen versionierbar sein, da sich Sprachdaten und Sortierstandards ändern können.

Persistierte Indizes müssen erkennen können, mit welcher Collation-Version sie erzeugt wurden.

## Normative Anforderungen

1. NovaOS MUSS localeabhängige Textsortierung unterstützen.
2. Collation MUSS von binären und identitätsrelevanten Vergleichen getrennt bleiben.
3. Unterschiedliche Vergleichsstärken MÜSSEN unterstützt werden können.
4. Collation-Regeln MÜSSEN localeabhängig auswählbar sein.
5. Sortierung DARF ObjectID und gespeicherte Daten nicht verändern.
6. Collation Keys MÜSSEN als abgeleitete Daten behandelt werden.
7. Collation Keys DÜRFEN nicht als stabile Identitäten verwendet werden.
8. Sicherheitsrelevante Identitäten DÜRFEN nicht localeabhängig verglichen werden.
9. Exakter und localeabhängiger Vergleich MÜSSEN unterscheidbar bleiben.
10. Collation-Regeln MÜSSEN versionierbar sein.
11. Abhängige Indizes MÜSSEN nach relevanten Regeländerungen neu erzeugbar sein.
12. Effektiver Collation Context und verwendete Version MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-QUERY-0001`
- `NPSPEC-INDEX-REBUILD-0001`

## Ergebnis

NovaOS besitzt ein localeabhängiges und versionierbares Collation-Modell für benutzerorientierte Textvergleiche, Sortierungen und Suchen. Gleichzeitig bleiben stabile Identitäten und sicherheitsrelevante Vergleiche vollständig von sprachabhängigen Sortierregeln getrennt.