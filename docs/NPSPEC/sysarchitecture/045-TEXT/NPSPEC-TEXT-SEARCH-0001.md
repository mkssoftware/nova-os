# NPSPEC-TEXT-SEARCH-0001 – Nova Text Search

## Status

Angenommen

## Kategorie

Text / Search

## Zweck

NovaOS definiert eine einheitliche Unicode-basierte Textsuche für Dokumente, Metadaten, Benutzeroberflächen und andere Textquellen.

Die Suche trennt exakte Textidentität von sprachabhängiger oder toleranter Übereinstimmung.

## Grundprinzipien

```text
Search ≠ Identity Comparison
Search ≠ Collation
Search ≠ Index
Match ≠ Byte Equality
Case-Insensitive ≠ Locale-Independent
Normalized Match ≠ Original Text
```

## Modell

```text
TextSearch
├── Query
├── SearchMode
├── LocaleID
├── NormalizationPolicy
├── CasePolicy
├── DiacriticPolicy
├── BoundaryPolicy
└── Result[]
```

Ein Ergebnis enthält mindestens:

```text
SearchResult
├── SourceID
├── StartPosition
├── EndPosition
├── MatchType
└── Confidence
```

## Suchmodi

NovaOS muss unterschiedliche Suchsemantiken unterstützen können:

```text
Exact
Canonical
CaseInsensitive
DiacriticInsensitive
Word
Prefix
Substring
LocaleAware
```

Der verwendete Modus muss explizit bestimmbar sein.

## Suchpipeline

```text
Query
  ↓
Unicode Validation
  ↓
Search Policy
  ↓
Optional Normalization
  ↓
Optional Case Processing
  ↓
Segmentation
  ↓
Matching
  ↓
Result Mapping
```

Die Transformation der Suchrepräsentation darf den Originaltext nicht verändern.

## Exakte Suche

`Exact` vergleicht Text gemäß der ausdrücklich definierten Textrepräsentation.

Eine exakte Suche darf nicht automatisch:

```text
Normalize
Case Fold
Remove Diacritics
Transliterate
```

## Kanonische Suche

Eine kanonische Suche darf Unicode-kanonisch äquivalente Sequenzen als Treffer behandeln.

```text
ä

und

a + Combining Diaeresis
```

können dadurch übereinstimmen.

## Groß-/Kleinschreibung

Case-insensitive Suche muss Unicode Case Folding verwenden können.

Locale-spezifische Regeln müssen über den entsprechenden Sprach- oder Locale-Kontext berücksichtigt werden.

## Wortsuche

Wortbasierte Suche muss die Text-Segmentierung verwenden.

```text
Query
  ↓
Word Segmentation
  ↓
Boundary-Aware Match
```

Leerzeichen allein dürfen nicht als universelle Wortgrenze verwendet werden.

## Ergebnispositionen

Treffer müssen auf den ursprünglichen Text zurückführbar bleiben.

```text
Search Representation
        ↓
Match
        ↓
Original Text Range
```

Positionen müssen ihre Indexdomäne eindeutig angeben können:

```text
Byte Offset
Code Point Index
Grapheme Index
```

Für benutzersichtbare Markierungen sollen Grapheme-Grenzen verwendet werden.

## Indexintegration

Die Textsuche darf vorhandene Suchindizes verwenden:

```text
Query
  ↓
Text Search Semantics
  ↓
Search Index
  ↓
Candidate Results
  ↓
Validation
```

Der Index bleibt abgeleitet und darf die definierte Suchsemantik nicht verändern.

## Sicherheit

Suchnormalisierung darf nicht zur sicherheitskritischen Identitätsprüfung verwendet werden.

```text
SearchMatch(A, B)
       ≠
IdentityEqual(A, B)
```

Unsichtbare Zeichen, Confusables, BiDi-Steuerzeichen und Mixed-Script-Inhalte müssen bei sicherheitsrelevanten Suchkontexten erkennbar bleiben können.

## Normative Anforderungen

1. NovaOS MUSS Unicode-basierte Textsuche unterstützen.
2. Suchmodus und Vergleichssemantik MÜSSEN explizit bestimmbar sein.
3. Exakte Suche DARF keine implizite Normalisierung durchführen.
4. Kanonisch äquivalente Suche MUSS unterstützt werden können.
5. Unicode Case Folding MUSS für case-insensitive Suche verfügbar sein.
6. Wortbasierte Suche MUSS Unicode-Segmentierung verwenden können.
7. Suchtransformationen DÜRFEN den Originaltext nicht verändern.
8. Treffer MÜSSEN auf Bereiche des Originaltexts zurückführbar sein.
9. Die verwendete Indexdomäne MUSS eindeutig sein.
10. Suchindizes DÜRFEN die definierte Suchsemantik nicht verändern.
11. Suchgleichheit DARF nicht mit sicherheitskritischer Identität gleichgesetzt werden.
12. Suchmodus, Locale, Normalisierung, Case Policy und Match-Typ MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-NORMALIZATION-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SEGMENTATION-0001`
- `NPSPEC-TEXT-COLLATION-0001`
- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine gemeinsame Unicode-basierte Textsuche, die exakte, normalisierte, sprachabhängige und tolerante Suchverfahren klar voneinander trennt. Treffer bleiben auf den unveränderten Originaltext zurückführbar und können effizient mit der systemweiten Index-Infrastruktur kombiniert werden.