# NPSPEC-TIME-CIVIL-0001 – Nova Civil Time

## Status

Angenommen

## Kategorie

Time / Civil Time

## Zweck

NovaOS definiert Civil Time als menschenlesbare Darstellung eines Zeitpunkts innerhalb eines Kalenders und einer Zeitzone.

Civil Time wird aus einer kanonischen Zeitbasis abgeleitet und bleibt von monotoner Systemzeit, Hardware-Uhren und internen Timern getrennt.

## Grundprinzipien

```text
Civil Time ≠ Monotonic Time
Civil Time ≠ UTC
Civil Time ≠ Time Zone
Civil Time ≠ Calendar
Civil Time ≠ RTC
Local Date/Time ≠ Unique Instant
```

## Modell

```text
CivilTime
├── Year
├── Month
├── Day
├── Hour
├── Minute
├── Second
├── Fraction
├── CalendarID
├── TimeZoneID
├── UTCOffset
└── Fold / Ambiguity State
```

Civil Time beschreibt eine lokale Kalenderdarstellung.

## Ableitung

```text
Canonical Instant
       ↓
Time Zone Rules
       ↓
Calendar System
       ↓
Civil Time
       ↓
Locale Formatting
```

Locale beeinflusst nur die Darstellung, nicht den zugrunde liegenden Zeitpunkt.

## Kanonischer Zeitpunkt

Intern gespeicherte absolute Zeitpunkte sollen als kanonische Instants behandelt werden.

```text
Instant
   ↓
Time Zone
   ↓
Civil Time
```

Ändert sich die Zeitzone, bleibt der Instant unverändert, während sich die Civil-Time-Darstellung ändern kann.

## Mehrdeutige Zeiten

Durch Zeitzonenänderungen können lokale Zeiten mehrfach auftreten.

```text
02:30
 ↓
Instant A
oder
Instant B
```

NovaOS muss solche Zeiten als `Ambiguous` erkennen und darf nicht stillschweigend einen beliebigen Zeitpunkt wählen.

## Nicht existierende Zeiten

Bestimmte lokale Zeiten können durch Zeitzonenübergänge übersprungen werden.

```text
01:59
 ↓
03:00
```

Eine Eingabe innerhalb der übersprungenen Zeit muss als `Invalid` oder `Nonexistent` erkannt werden.

## Zeitzonen

Civil Time verwendet eine explizite `TimeZoneID`.

Ein fester UTC-Offset ist nicht gleichbedeutend mit einer Zeitzone:

```text
UTC+01:00 ≠ Europe/Berlin
```

Zeitzonen können historische und zukünftige Offset-Regeln besitzen.

## Kalender

Civil Time muss unterschiedliche Kalendersysteme unterstützen können.

```text
Instant
   ↓
Calendar Provider
   ↓
Year / Month / Day
```

Der Kalender verändert nicht den zugrunde liegenden Instant.

## Eingabe

Lokale Benutzereingaben werden validiert:

```text
Civil Input
    ↓
Calendar Validation
    ↓
Time Zone Resolution
    ↓
Ambiguity Check
    ↓
Canonical Instant
```

Mehrdeutige oder nicht existierende Zeiten dürfen eine explizite Entscheidung erfordern.

## Speicherung

Für absolute Ereignisse soll NovaOS bevorzugt speichern:

```text
Canonical Instant
+
TimeZoneID
```

Civil-Time-Felder dürfen zusätzlich gespeichert werden, wenn ihre ursprüngliche lokale Bedeutung relevant ist.

Für rein lokale Termine ohne festen globalen Zeitpunkt darf Civil Time bewusst ohne Instant existieren.

## Zeitkorrekturen

Änderungen der Wall Clock dürfen bestehende gespeicherte Instants nicht verändern.

Civil-Time-Darstellungen werden aus dem jeweiligen Instant und den gültigen Regeln neu berechnet.

## Normative Anforderungen

1. NovaOS MUSS Civil Time von monotoner Zeit und kanonischen Instants trennen.
2. Civil Time MUSS Kalender und Zeitzone explizit berücksichtigen können.
3. Locale DARF nur Darstellung und Eingabe beeinflussen.
4. Eine lokale Zeit DARF nicht grundsätzlich als eindeutiger Instant behandelt werden.
5. Mehrdeutige lokale Zeiten MÜSSEN erkannt werden.
6. Nicht existierende lokale Zeiten MÜSSEN erkannt werden.
7. Zeitzonen MÜSSEN von festen UTC-Offsets getrennt bleiben.
8. Zeitzonenänderungen DÜRFEN den zugrunde liegenden Instant nicht verändern.
9. Unterschiedliche Kalendersysteme MÜSSEN unterstützt werden können.
10. Lokale Eingaben MÜSSEN vor Umwandlung in einen Instant validiert werden.
11. Civil Time MUSS auch ohne zugeordneten globalen Instant darstellbar sein.
12. Instant, TimeZoneID, CalendarID, Offset und Ambiguity State MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-DATETIME-0001`
- `NPSPEC-GLOBALIZATION-CALENDAR-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein eindeutiges Civil-Time-Modell, das menschenlesbare lokale Zeit konsequent von absoluten Zeitpunkten trennt. Zeitzonen, Kalender, Sommerzeitübergänge und mehrdeutige lokale Zeiten können dadurch korrekt behandelt werden, ohne interne Zeitmessung oder gespeicherte Instants zu verändern.