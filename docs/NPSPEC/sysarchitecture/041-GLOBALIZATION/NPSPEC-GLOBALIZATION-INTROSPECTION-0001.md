# NPSPEC-GLOBALIZATION-INTROSPECTION-0001 – Nova Globalization Introspection

## Status

Angenommen

## Kategorie

Globalization / Introspection

## Zweck

NovaOS definiert eine einheitliche Introspection-Schnittstelle für den aktuellen Globalization-Kontext.

Systemkomponenten, Programme, Solutions, Diagnosewerkzeuge und autorisierte Agenten können damit erkennen, welche Sprache, Locale, Region, Kalender-, Formatierungs-, Collation- und Schreibrichtungsregeln tatsächlich verwendet werden und wie diese bestimmt wurden.

## Grundprinzipien

```text
Introspection ≠ Authority
Configured Value ≠ Effective Value
Preference ≠ Effective Context
Fallback ≠ Error
Display Value ≠ Internal Identity
Globalization Metadata ≠ Location Proof
```

## Modell

```text
GlobalizationInfo
├── Language
├── Locale
├── Region
├── Calendar
├── NumberFormat
├── DateTimeFormat
├── Collation
├── TextDirection
├── Fallbacks[]
└── Sources[]
```

## Effektiver Kontext

NovaOS muss zwischen konfigurierten und tatsächlich wirksamen Einstellungen unterscheiden können.

```text
System Defaults
      ↓
User Preferences
      ↓
Session Overrides
      ↓
Program / Solution Requirements
      ↓
Negotiation
      ↓
Effective Globalization Context
```

Introspection liefert primär den effektiven Kontext.

## Herkunft

Für einen Wert soll nachvollziehbar sein, wodurch er bestimmt wurde:

```text
System
User
Session
Program
Solution
Negotiation
Fallback
Explicit Override
```

Beispiel:

```text
Language = de
Source   = User

Locale   = de-CH
Source   = Session

Calendar = Gregorian
Source   = Locale Default
```

## Fallbacks

Wurde ein gewünschter Wert nicht unterstützt, muss der tatsächlich verwendete Fallback erkennbar sein.

```text
Requested: de-CH
    ↓
Fallback: de
    ↓
Effective: de
```

Dies gilt insbesondere für Sprache, Ressourcen, Kalender und andere lokalisierbare Komponenten.

## Ressourcen

Für lokalisierte Ressourcen darf nachvollziehbar sein:

```text
ResourceID
Requested Language
Resolved Language
Resource Version
Fallback Path
```

Der übersetzte Text selbst ist nicht Teil der Identität.

## Formatierung

Introspection darf die aktuell verwendeten Regeln für folgende Bereiche bereitstellen:

```text
Numbers
Date / Time
Calendar
Collation
Plural Rules
Text Direction
```

Dabei sollen stabile Regel- oder Versionsinformationen verwendet werden, sofern vorhanden.

## Datenschutz

Globalization Introspection darf nicht als Standortbestimmung verwendet werden.

Insbesondere gilt:

```text
Region ≠ Physical Location
Language ≠ Nationality
Locale ≠ Location
Time Zone ≠ Location Proof
```

Nur die für den jeweiligen Aufrufer autorisierten Informationen dürfen sichtbar sein.

## Änderungen

Änderungen des effektiven Globalization-Kontexts sollen beobachtbar sein.

```text
Context Change
      ↓
Globalization Event
      ↓
Affected Components
      ↓
Re-evaluation / Re-render
```

Programme und Solutions können dadurch ihre Darstellung aktualisieren, ohne ihre interne Identität oder Daten zu verändern.

## Normative Anforderungen

1. NovaOS MUSS den effektiven Globalization-Kontext introspektierbar machen.
2. Konfigurierte und effektive Werte MÜSSEN unterscheidbar sein.
3. Die Herkunft effektiver Einstellungen SOLL nachvollziehbar sein.
4. Verwendete Fallbacks MÜSSEN ermittelbar sein.
5. Sprache, Locale und Region MÜSSEN getrennt introspektierbar sein.
6. Calendar, Number Format, DateTime Format, Collation und Text Direction MÜSSEN ermittelbar sein können.
7. Verwendete Regel- und Ressourcenversionen SOLLEN nachvollziehbar sein.
8. Änderungen des effektiven Kontexts SOLLEN beobachtbar sein.
9. Introspection DARF keine Globalization-Einstellung selbst verändern.
10. Introspection DARF keine Authority erzeugen oder erweitern.
11. Globalization-Daten DÜRFEN nicht als Nachweis eines physischen Standorts behandelt werden.
12. Sicherheits- und Datenschutzgrenzen MÜSSEN bei der Bereitstellung von Introspection-Daten erhalten bleiben.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-REGION-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-GLOBALIZATION-NUMBER-0001`
- `NPSPEC-GLOBALIZATION-DATETIME-0001`
- `NPSPEC-GLOBALIZATION-CALENDAR-0001`
- `NPSPEC-GLOBALIZATION-COLLATION-0001`
- `NPSPEC-GLOBALIZATION-RESOURCE-0001`
- `NPSPEC-GLOBALIZATION-PLURAL-0001`
- `NPSPEC-GLOBALIZATION-RTL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS macht den effektiven Globalization-Kontext strukturiert nachvollziehbar. Sprache, Locale, Region, Formatierungsregeln, Ressourcen, Fallbacks und deren Herkunft können diagnostiziert und beobachtet werden, ohne interne Identitäten, Berechtigungen oder Datenschutzgrenzen zu beeinflussen.