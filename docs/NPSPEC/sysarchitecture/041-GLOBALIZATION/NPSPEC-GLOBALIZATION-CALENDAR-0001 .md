# NPSPEC-GLOBALIZATION-CALENDAR-0001 – Nova Calendar Model

## Status

Angenommen

## Kategorie

Globalization / Calendar

## Zweck

NovaOS definiert ein einheitliches Modell für unterschiedliche Kalendersysteme.

Kalender dienen als Projektion kanonischer Zeitwerte in eine kulturell oder fachlich geeignete Datumsdarstellung. Das verwendete Kalendersystem verändert nicht den zugrunde liegenden Zeitpunkt.

## Grundprinzipien

```text
Calendar ≠ Time
Calendar ≠ Time Zone
Calendar ≠ Locale
Calendar Date ≠ Canonical Instant
Calendar Conversion ≠ Time Conversion
Display Calendar ≠ Storage Format
```

## Modell

```text
CalendarContext
├── CalendarID
├── CalendarSystem
├── Era
├── Rules
├── LocaleContext
└── Version
```

`CalendarID` identifiziert ein Kalendersystem unabhängig von Sprache, Locale und Darstellung.

## Kalenderprojektion

```text
Canonical Time
      ↓
Time Zone
      ↓
Local Date
      ↓
Calendar Projection
      ↓
Localized Representation
```

Der Kalender bestimmt die Interpretation von Jahr, Monat, Woche und Tag.

## Kalendersysteme

NovaOS muss unterschiedliche Kalendersysteme unterstützen können.

Beispiele:

```text
Gregorian
ISO Week Calendar
Julian
Islamic
Hebrew
Buddhist
Japanese Era Calendar
Registered Custom Calendar
```

Der gregorianische Kalender kann als systemweiter Standard verwendet werden, ist jedoch nicht Bestandteil der internen Zeitidentität.

## Kalenderregeln

Ein Kalender Provider definiert mindestens:

```text
Era
Year
Month
Day
Leap Rules
Month Length
Year Length
Valid Range
```

Zusätzliche Regeln dürfen kalenderabhängig bereitgestellt werden.

## Locale

Locale darf bestimmen, welcher Kalender standardmäßig für die Darstellung bevorzugt wird.

```text
Locale Preference
      ↓
Calendar Selection
      ↓
Localized Date
```

Ein Nutzer darf einen anderen unterstützten Kalender explizit auswählen.

## Konvertierung

Kalenderkonvertierungen erfolgen über eine gemeinsame kanonische Zeitbasis:

```text
Calendar A
    ↓
Canonical Date/Time
    ↓
Calendar B
```

Direkte kalenderabhängige Umrechnungen sollen vermieden werden, wenn eine eindeutige kanonische Abbildung verfügbar ist.

## Eingabe

Kalenderbasierte Datumseingaben müssen vor ihrer Übernahme validiert werden.

```text
Calendar Input
      ↓
Calendar Validation
      ↓
Canonical Representation
```

Ungültige Tage, Monate, Schaltregeln oder Epochen dürfen nicht stillschweigend korrigiert werden.

## Versionierung

Kalenderregeln dürfen versioniert werden, wenn sich externe oder historische Definitionen ändern.

Gespeicherte Daten müssen weiterhin eindeutig interpretierbar bleiben.

## Programme und Solutions

Programme und Solutions dürfen:

```text
System Calendar verwenden
CalendarID anfordern
Eigenen registrierten Calendar Provider verwenden
Mehrere Kalender parallel darstellen
```

Die Auswahl eines Kalenders erzeugt keine zusätzliche Authority.

## Normative Anforderungen

1. NovaOS MUSS Kalendersysteme von der kanonischen Zeitrepräsentation trennen.
2. Jeder Calendar Provider MUSS eine stabile `CalendarID` besitzen.
3. Mehrere Kalendersysteme MÜSSEN parallel unterstützt werden können.
4. Locale und Calendar MÜSSEN getrennt konfigurierbar bleiben.
5. Calendar und Time Zone MÜSSEN getrennte Konzepte bleiben.
6. Kalenderkonvertierungen MÜSSEN den zugrunde liegenden Zeitpunkt erhalten.
7. Kalenderbasierte Eingaben MÜSSEN validiert werden.
8. Ungültige Kalenderdaten DÜRFEN nicht stillschweigend korrigiert werden.
9. Kalenderregeln MÜSSEN versionierbar sein können.
10. Explizite Nutzerauswahl MUSS Locale-Defaults überschreiben können.
11. Persistente Zeitwerte DÜRFEN nicht von der aktuell ausgewählten Kalenderdarstellung abhängig sein.
12. CalendarID, Version und effektiver Calendar Context MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-REGION-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-GLOBALIZATION-DATETIME-0001`

## Ergebnis

NovaOS behandelt Kalendersysteme als austauschbare Projektionen einer gemeinsamen kanonischen Zeitbasis. Unterschiedliche kulturelle und fachliche Kalender können parallel verwendet und lokalisiert dargestellt werden, ohne gespeicherte Zeitpunkte oder interne Systemidentitäten zu verändern.