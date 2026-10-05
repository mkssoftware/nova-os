# NPSPEC-SYSTEM-LOCALE-0001 – Nova System Locale

## Status

Angenommen

## Kategorie

System / Localization / Locale

## Zweck

NovaOS definiert ein einheitliches Locale-Modell für Sprache, Region und regionale Darstellungsregeln.

Locale beeinflusst ausschließlich Darstellung und Interpretation benutzernaher Daten und verändert keine internen Identitäten, Objektstrukturen oder Sicherheitsentscheidungen.

## Grundprinzipien

```text
Locale ≠ Language Only
Locale ≠ System Identity
Display Format ≠ Stored Value
Localized Name ≠ Object Identity
Locale Change ≠ Data Migration
```

## Locale-Modell

Ein Locale-Kontext kann enthalten:

```text
LocaleContext
├── Language
├── Region
├── NumberFormat
├── DateFormat
├── TimeFormat
├── CurrencyFormat
├── MeasurementSystem
├── Calendar
└── TimeZone
```

Einzelne Einstellungen dürfen unabhängig überschrieben werden.

## Scopes

Locale-Einstellungen können auf unterschiedlichen Ebenen gelten:

```text
System Default
      ↓
User
      ↓
Program / Solution
```

Benutzerspezifische Einstellungen überschreiben grundsätzlich Systemdefaults, sofern keine zwingende Policy entgegensteht.

## Sprache und Region

Sprache und Region werden getrennt behandelt.

Beispiel:

```text
Language = de
Region   = DE
```

oder:

```text
Language = en
Region   = DE
```

Dadurch kann die UI englisch dargestellt werden, während deutsche Datums-, Zahlen- oder Maßeinheiten verwendet werden.

## Formatierung

Interne Werte bleiben sprach- und regionsneutral.

```text
Stored Value
     ↓
Locale Formatting
     ↓
Displayed Value
```

Beispiel:

```text
Internal Date → 2026-10-05

de-DE → 05.10.2026
en-US → 10/05/2026
```

Die Darstellung verändert den gespeicherten Wert nicht.

## Lokalisierte Systemnamen

Systemdefinierte Anzeigenamen dürfen lokalisiert werden:

```text
Internal Identity: User
de-DE: Benutzer
en-US: Users
```

Die interne Namespace- oder Objektidentität bleibt unverändert.

Benutzerdefinierte Namen werden nicht automatisch übersetzt.

## Laufzeitwechsel

Locale-Einstellungen sollen ohne Neustart des gesamten Systems geändert werden können.

```text
Locale Change
     ↓
Update Context
     ↓
Notify Components
     ↓
Refresh Presentation
```

Komponenten müssen nicht benötigte Locale-Daten nicht dauerhaft zwischenspeichern.

## Fallback

Fehlt eine Übersetzung oder regionale Definition, verwendet NovaOS eine definierte Fallback-Kette.

```text
Requested Locale
      ↓
Language Fallback
      ↓
System Default
      ↓
Neutral Representation
```

Fehlende Lokalisierung darf keine Systemfunktion verhindern.

## Sicherheit

Locale darf keine sicherheitsrelevante Identität oder Authority beeinflussen.

Berechtigungen, Capability-IDs, ObjectIDs und andere stabile Identitäten bleiben locale-unabhängig.

## Normative Anforderungen

1. NovaOS MUSS Sprache und Region getrennt behandeln können.
2. Locale MUSS pro Benutzer konfigurierbar sein.
3. Interne Werte MÜSSEN locale-neutral speicherbar sein.
4. Formatierung DARF gespeicherte Werte nicht verändern.
5. Systemdefinierte Anzeigenamen DÜRFEN lokalisiert werden.
6. Lokalisierte Namen DÜRFEN keine stabile Identität darstellen.
7. Benutzerdefinierte Namen DÜRFEN nicht automatisch übersetzt werden.
8. Locale-Änderungen SOLLEN zur Laufzeit wirksam werden können.
9. Fehlende Lokalisierungen MÜSSEN einen definierten Fallback besitzen.
10. Locale-Wechsel DARF keine Datenmigration erfordern.
11. Locale DARF Berechtigungen oder Authority nicht verändern.
12. Locale-Zustand und effektive Einstellungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-FRAMEWORK-0001`
- `NPSPEC-FILESYSTEM-LOCALIZATION-0001`
- `NPSPEC-USERSPACE-SETTINGS-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Locale-Modell, das Sprache und regionale Darstellung unabhängig von internen Daten und Identitäten behandelt. Benutzer können Sprache, Region und Formatierung flexibel wählen, ohne Dateisystemstrukturen, Objektidentitäten oder Sicherheitsmechanismen zu verändern.