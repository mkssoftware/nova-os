# NPSPEC-GLOBALIZATION-LANGUAGE-0001 – Nova Language Model

## Status

Angenommen

## Kategorie

Globalization / Language

## Zweck

NovaOS definiert ein einheitliches Sprachmodell für Benutzeroberflächen, Systemtexte, Programme, Solutions und andere lokalisierbare Inhalte.

Die gewählte Sprache beeinflusst die textuelle Darstellung, jedoch keine internen Identitäten, Datenmodelle oder Systemlogik.

## Grundprinzipien

```text
Language ≠ Locale
Language ≠ Region
Language ≠ Script
Translation ≠ Identity
Display Text ≠ Internal Name
Missing Translation ≠ Missing Function
```

## Modell

```text
LanguageContext
├── LanguageID
├── Script
├── Variants[]
├── FallbackChain
└── ResourceSet
```

Sprachen werden durch standardisierte maschinenlesbare Kennungen identifiziert.

Beispiele:

```text
de
en
fr
es
ja
```

Sprachvarianten dürfen bei Bedarf zusätzlich spezifiziert werden.

## Sprachkontexte

NovaOS unterstützt unterschiedliche Sprachkontexte:

```text
System Language
User Language
Session Language
Program Language
Solution Language
```

Ein spezifischer Kontext darf einen allgemeineren Kontext überschreiben.

```text
System
  ↓
User
  ↓
Session
  ↓
Program / Solution
```

## Sprachressourcen

Lokalisierbare Inhalte werden von ihrer internen Identität getrennt gespeichert.

```text
ResourceID
├── de → "Einstellungen"
├── en → "Settings"
└── fr → "Paramètres"
```

Systemlogik referenziert `ResourceID` und nicht den übersetzten Text.

## Fallback

Fehlt eine Übersetzung, wird eine definierte Fallback-Kette verwendet.

```text
Requested Language
       ↓
Language Variant
       ↓
Base Language
       ↓
Default Language
       ↓
Invariant Resource
```

Eine fehlende Übersetzung darf die zugrunde liegende Funktion nicht unzugänglich machen.

## Sprache und Locale

Sprache und Locale bleiben getrennte Konzepte.

Beispiel:

```text
Language: de
Locale: de-CH
```

Dadurch kann eine deutschsprachige Oberfläche mit schweizerischen Zahlen-, Datums-, Währungs- und Formatierungsregeln verwendet werden.

## Script

Sprachen dürfen unterschiedliche Schriftsysteme verwenden.

```text
Language
   ↓
Script
   ↓
Text Rendering
```

NovaOS muss Unicode-basierte Textverarbeitung unterstützen und unterschiedliche Schreibrichtungen ermöglichen.

## Laufzeitwechsel

Die Sprache soll während einer laufenden Benutzersitzung gewechselt werden können.

Sprachfähige Systemkomponenten, Programme und Solutions sollen ihre sichtbaren Texte anschließend aktualisieren können, ohne ihre internen Zustände oder Identitäten zu verändern.

## Systemidentitäten

Übersetzungen dürfen keine internen Identitäten verändern.

Dies betrifft insbesondere:

```text
ObjectID
CapabilityID
SolutionID
Program Identity
ServiceID
SemanticTypeID
Registry Identity
Namespace Identity
ResourceID
```

## Normative Anforderungen

1. NovaOS MUSS Sprache unabhängig von Locale und Region modellieren.
2. Sprachkennungen MÜSSEN standardisiert und maschinenlesbar sein.
3. Lokalisierte Texte MÜSSEN von internen Identitäten getrennt bleiben.
4. System-, User-, Session-, Program- und Solution-Sprachen MÜSSEN getrennt konfigurierbar sein können.
5. Sprachressourcen MÜSSEN über stabile ResourceIDs referenzierbar sein.
6. Eine definierte Sprach-Fallback-Kette MUSS unterstützt werden.
7. Fehlende Übersetzungen DÜRFEN Funktionen nicht deaktivieren.
8. Sprachwechsel SOLLEN zur Laufzeit möglich sein.
9. Unicode MUSS die Grundlage der Textverarbeitung bilden.
10. Unterschiedliche Schriftsysteme und Schreibrichtungen MÜSSEN unterstützt werden können.
11. Übersetzte Anzeigenamen DÜRFEN keine internen Systemidentitäten verändern.
12. Effektive Sprache, Fallback und verwendete Sprachressourcen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-FILESYSTEM-LOCALIZATION-0001`
- `NPSPEC-SYSTEM-LOCALE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein von Locale, Region und internen Identitäten getrenntes Sprachmodell. System, Nutzer, Programme und Solutions können unabhängig lokalisiert werden, während stabile ResourceIDs und definierte Fallback-Regeln eine konsistente und zur Laufzeit wechselbare Benutzeroberfläche ermöglichen.