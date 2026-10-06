# NPSPEC-GLOBALIZATION-PLURAL-0001 – Nova Plural Rules

## Status

Angenommen

## Kategorie

Globalization / Pluralization

## Zweck

NovaOS definiert ein einheitliches Modell für sprachabhängige Pluralformen lokalisierter Ressourcen.

Pluralformen werden anhand der Regeln der effektiven Sprache bestimmt und dürfen nicht durch fest codierte Annahmen wie Singular/Plural oder `count == 1` ersetzt werden.

## Grundprinzipien

```text
Plural ≠ Singular/Plural Only
Plural Rule ≠ Locale
Plural Category ≠ Numeric Value
Display Text ≠ Stored Number
Missing Category ≠ Missing Function
```

## Modell

```text
PluralContext
├── Language
├── RuleSet
├── RuleVersion
├── Number
└── PluralCategory
```

Die konkrete Textressource wird anschließend anhand der bestimmten Kategorie ausgewählt.

```text
Number
   ↓
Plural Rules
   ↓
Plural Category
   ↓
Localized Resource Variant
```

## Pluralkategorien

NovaOS unterstützt die standardisierten Kategorien:

```text
zero
one
two
few
many
other
```

Eine Sprache muss nicht alle Kategorien verwenden.

`other` bildet die verpflichtende Fallback-Kategorie.

## Beispiel

```text
ResourceID: nova.files.count

one   → "{count} Datei"
other → "{count} Dateien"
```

Für Sprachen mit komplexeren Regeln können zusätzliche vorhandene Kategorien genutzt werden:

```text
one
two
few
many
other
```

Die Kategorie wird aus der Sprachregel bestimmt und nicht direkt aus dem sichtbaren Text.

## Kardinal und Ordinal

Pluralregeln müssen unterschiedliche Regeltypen unterstützen können:

```text
Cardinal
Ordinal
```

Kardinalregeln werden beispielsweise für Mengen verwendet.

Ordinalregeln können für sprachabhängige Ordnungsformen verwendet werden.

Beide Regelsätze bleiben voneinander getrennt.

## Numerische Werte

Pluralregeln dürfen abhängig sein von:

```text
Integer Value
Fraction
Visible Fraction Digits
Numeric Representation
```

Daher darf eine Implementierung Pluralisierung nicht auf einfache Ganzzahlvergleiche reduzieren.

## Ressourcenauflösung

```text
ResourceID
    +
Language Context
    +
Numeric Value
      ↓
Plural Rule Evaluation
      ↓
Plural Category
      ↓
Resource Variant
```

Fehlt eine spezifische Variante, wird `other` verwendet.

Danach gelten die normalen Globalization-Resource-Fallbacks.

## Versionierung

Pluralregeln müssen versionierbar sein, da sich zugrunde liegende Sprachdaten ändern können.

Die verwendete Regelversion muss nachvollziehbar bleiben.

## Sicherheit

Pluralisierung beeinflusst ausschließlich lokalisierte Darstellung.

Sie darf keine:

```text
ObjectID
CapabilityID
SolutionID
Permission
Policy
Numeric Value
```

verändern.

## Normative Anforderungen

1. NovaOS MUSS sprachabhängige Pluralregeln unterstützen.
2. Pluralisierung MUSS von Locale und Region getrennt bleiben.
3. Die Kategorien `zero`, `one`, `two`, `few`, `many` und `other` MÜSSEN darstellbar sein.
4. `other` MUSS als Fallback-Kategorie unterstützt werden.
5. Sprachen DÜRFEN nur die für sie relevanten Kategorien verwenden.
6. Kardinal- und Ordinalregeln MÜSSEN getrennt unterstützt werden können.
7. Pluralregeln DÜRFEN nicht auf `count == 1` reduziert werden.
8. Dezimal- und Bruchwerte MÜSSEN korrekt berücksichtigt werden können.
9. Pluralisierung DARF den zugrunde liegenden numerischen Wert nicht verändern.
10. Pluralregeln MÜSSEN versionierbar sein.
11. Fehlende Pluralvarianten DÜRFEN die zugrunde liegende Funktion nicht entfernen.
12. Sprache, Regelversion, Kategorie und verwendete Ressource MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-GLOBALIZATION-NUMBER-0001`
- `NPSPEC-GLOBALIZATION-RESOURCE-0001`

## Ergebnis

NovaOS besitzt ein sprachabhängiges und versionierbares Pluralmodell, das auch komplexe Pluralregeln, Dezimalwerte sowie Kardinal- und Ordinalformen unterstützt. Lokalisierte Ressourcen können dadurch grammatikalisch korrekt ausgewählt werden, ohne numerische Werte, Systemlogik oder interne Identitäten zu verändern.