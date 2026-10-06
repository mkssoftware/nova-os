# NPSPEC-GLOBALIZATION-DATETIME-0001 – Nova Date and Time Formatting

## Status

Angenommen

## Kategorie

Globalization / DateTime

## Zweck

NovaOS definiert ein einheitliches Modell für die lokalisierte Darstellung und Eingabe von Datum und Uhrzeit.

Zeitwerte werden intern in kanonischer Form verarbeitet. Locale, Sprache, Region und Zeitzone beeinflussen ausschließlich die Darstellung und Interpretation, nicht die zugrunde liegende Zeitidentität.

## Grundprinzipien

```text
Instant ≠ Display Time
DateTime ≠ Time Zone
Time Zone ≠ Locale
Locale ≠ Calendar
Displayed Time ≠ Stored Time
Formatting ≠ Time Conversion
```

## Modell

```text
Canonical Time Value
        ↓
Time Zone
        ↓
Calendar
        ↓
Locale Context
        ↓
Localized Date/Time
```

Die einzelnen Ebenen müssen getrennt bleiben.

## Darstellung

NovaOS unterstützt mindestens:

```text
Date
Time
Date + Time
Short Format
Long Format
Relative Time
Duration
Weekday
Month
Year
```

Beispiel:

```text
de-DE → 06.10.2026 18:30
en-US → 10/6/2026 6:30 PM
```

Der zugrunde liegende Zeitpunkt bleibt identisch.

## Formatierungsregeln

Ein DateTime-Format darf unter anderem bestimmen:

```text
Date Order
Separator
Month Representation
Weekday Representation
12/24 Hour Format
AM/PM Representation
First Day of Week
Week Numbering
```

Die Regeln werden aus Locale, Region und expliziten Nutzerpräferenzen bestimmt.

## Zeitzone

Zeitzonen werden unabhängig vom Locale behandelt.

```text
Instant
   +
Time Zone
   ↓
Local DateTime
```

Ein Locale-Wechsel darf die Zeitzone nicht verändern.

Ein Zeitzonenwechsel verändert die lokale Darstellung eines Zeitpunkts, nicht dessen kanonische Identität.

## Kalender

NovaOS muss Kalenderdarstellung von der zugrunde liegenden Zeitrepräsentation trennen.

Neben dem systemweiten Standardkalender dürfen weitere registrierte Kalendersysteme verwendet werden.

```text
Canonical Instant
      ↓
Calendar Projection
      ↓
Localized Representation
```

## Eingabe

Lokalisierte Datum-/Zeiteingaben werden anhand des aktiven Kontexts interpretiert:

```text
User Input
    ↓
Locale-aware Parser
    ↓
Calendar / Time Zone
    ↓
Validation
    ↓
Canonical Value
```

Mehrdeutige Eingaben dürfen nicht stillschweigend falsch interpretiert werden.

## Sommerzeit und Zeitänderungen

NovaOS muss Zeitzonenregeln einschließlich Zeitumstellungen berücksichtigen können.

Nicht existente oder mehrdeutige lokale Uhrzeiten müssen erkannt werden.

```text
Local Time
├── Unique
├── Ambiguous
└── Invalid
```

Eine mehrdeutige lokale Zeit darf nicht ohne definierte Regel oder explizite Entscheidung einem Zeitpunkt zugeordnet werden.

## Relative Zeit

Benutzeroberflächen dürfen relative Darstellungen verwenden:

```text
vor 5 Minuten
gestern
in 2 Stunden
```

Intern bleibt der zugrunde liegende absolute oder relative Zeitwert erhalten.

## Kanonische Daten

Persistente Daten, APIs und Protokolle dürfen nicht von der aktuellen UI-Formatierung abhängig sein.

```text
UI Display      → Locale-aware
Stored Instant  → Canonical
Protocol Time   → Protocol-defined
```

## Normative Anforderungen

1. Zeitwerte MÜSSEN unabhängig von ihrer lokalisierten Darstellung gespeichert werden können.
2. Locale und Zeitzone MÜSSEN getrennt behandelt werden.
3. DateTime Formatting MUSS den effektiven Locale-Kontext berücksichtigen können.
4. 12- und 24-Stunden-Darstellung MÜSSEN unterstützt werden.
5. Kalenderdarstellung MUSS von der kanonischen Zeitrepräsentation getrennt bleiben.
6. Lokalisierte Eingaben MÜSSEN validiert werden.
7. Mehrdeutige oder ungültige lokale Zeiten MÜSSEN erkennbar sein.
8. Zeitzonenregeln und Zeitumstellungen MÜSSEN berücksichtigt werden können.
9. Locale-Wechsel DÜRFEN gespeicherte Zeitwerte nicht verändern.
10. Zeitzonenwechsel DÜRFEN die Identität eines gespeicherten Zeitpunkts nicht verändern.
11. Maschinenlesbare Zeitwerte DÜRFEN nicht von UI-Formatierungsregeln abhängen.
12. Effektiver Locale-, Kalender- und Zeitzonenkontext MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-REGION-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-GLOBALIZATION-NUMBER-0001`

## Ergebnis

NovaOS trennt Zeitpunkte konsequent von ihrer sprachlichen, regionalen und zeitzonenabhängigen Darstellung. Datum und Uhrzeit können lokalisiert dargestellt und eingegeben werden, während intern stabile kanonische Zeitwerte erhalten bleiben.