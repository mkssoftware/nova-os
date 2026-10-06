# NPSPEC-GLOBALIZATION-LOCALE-0001 – Nova Locale Model

## Status

Angenommen

## Kategorie

Globalization / Locale

## Zweck

NovaOS definiert ein einheitliches Locale-Modell für sprach-, regions- und kulturabhängige Darstellung.

Locale beeinflusst ausschließlich die Darstellung und Interpretation lokalisierbarer Informationen. Interne Identitäten, Datenmodelle, Objektpfade und Systemlogik bleiben davon unabhängig.

## Grundprinzipien

```text
Locale ≠ Language Only
Locale ≠ System Identity
Locale ≠ Time Zone
Locale ≠ Permission
Display Value ≠ Stored Value
Localized Name ≠ Internal Identity
```

## Modell

```text
LocaleContext
├── LocaleID
├── Language
├── Region
├── Script
├── FormattingRules
├── MeasurementSystem
└── FallbackChain
```

`LocaleID` verwendet eine standardisierte, maschinenlesbare Kennung.

Beispiele:

```text
de-DE
de-AT
de-CH
en-US
en-GB
fr-FR
```

## Locale-Kontext

NovaOS unterstützt unterschiedliche Locale-Kontexte:

```text
System Locale
User Locale
Session Locale
Process Locale
Application Locale
Solution Locale
```

Ein spezifischerer Kontext darf einen allgemeineren Kontext überschreiben.

```text
System
  ↓
User
  ↓
Session
  ↓
Process / Program / Solution
```

## Darstellung

Locale kann insbesondere beeinflussen:

```text
Language
Date Format
Time Format
Number Format
Decimal Separator
Grouping Separator
Currency Format
Measurement Units
Sorting
Text Direction
```

Die zugrunde liegenden Werte bleiben unverändert.

Beispiel:

```text
Stored Value: 1234.50

de-DE → 1.234,50
en-US → 1,234.50
```

## Sprache und Region

Sprache und Region müssen unabhängig kombinierbar bleiben.

```text
Language: de
Region: DE

Language: de
Region: CH
```

Dadurch können Sprache, Formatierung und regionale Konventionen getrennt behandelt werden.

## Fallback

Fehlt eine spezifische Lokalisierung, verwendet NovaOS eine definierte Fallback-Kette.

```text
de-DE
 ↓
de
 ↓
Default Locale
 ↓
Invariant Resource
```

Fehlende Übersetzungen dürfen keine Änderung interner Identitäten verursachen.

## Systemidentitäten

Locale darf keine stabilen NovaOS-Identitäten verändern.

Dies betrifft insbesondere:

```text
ObjectID
CapabilityID
SolutionID
Program Identity
ServiceID
DeviceID
VolumeID
Registry Identity
SemanticTypeID
```

Auch systemdefinierte Namespace-Identitäten bleiben sprachneutral.

Die Benutzeroberfläche darf dafür lokalisierte Anzeigenamen darstellen.

## Laufzeitwechsel

Locale soll während einer laufenden Sitzung geändert werden können.

Locale-fähige Komponenten sollen ihre Darstellung aktualisieren können, ohne Neustart des gesamten Systems oder Migration gespeicherter Daten.

## Sortierung und Vergleich

Sprachabhängige Sortierung darf für Benutzeroberflächen verwendet werden.

Sicherheits-, Identitäts- und Protokollvergleiche dürfen jedoch nicht von sprachabhängigen Locale-Regeln abhängen.

```text
UI Sorting        → Locale-aware
Identity Compare  → Locale-independent
Protocol Compare  → Defined Canonical Rules
```

## Normative Anforderungen

1. NovaOS MUSS Locale unabhängig von internen Systemidentitäten behandeln.
2. Sprache und Region MÜSSEN getrennt modellierbar sein.
3. System-, User-, Session- und Anwendungskontexte MÜSSEN unterschiedliche Locales verwenden können.
4. Locale MUSS Zahlen-, Datums-, Zeit- und andere Darstellungsformate beeinflussen können.
5. Gespeicherte kanonische Werte DÜRFEN durch reine Locale-Wechsel nicht verändert werden.
6. Systemdefinierte Identitäten und Namespace-Identitäten DÜRFEN nicht lokalisiert werden.
7. Lokalisierte Anzeigenamen DÜRFEN von internen Namen und IDs abweichen.
8. Eine definierte Locale-Fallback-Kette MUSS unterstützt werden.
9. Locale-Wechsel SOLLEN zur Laufzeit möglich sein.
10. Sicherheitsrelevante Vergleiche DÜRFEN nicht von Locale-Regeln abhängen.
11. Locale und Time Zone MÜSSEN unabhängig konfigurierbar sein.
12. Der effektive Locale-Kontext MUSS introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-LOCALIZATION-0001`
- `NPSPEC-SYSTEM-LOCALE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS trennt regionale und sprachliche Darstellung konsequent von internen Daten und Identitäten. Nutzer, Programme und Solutions können unterschiedliche Locale-Kontexte verwenden, während Systemobjekte, Capabilities, Pfade und gespeicherte Werte stabil und sprachunabhängig bleiben.