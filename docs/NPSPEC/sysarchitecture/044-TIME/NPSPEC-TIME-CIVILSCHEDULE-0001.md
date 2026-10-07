# NPSPEC-TIME-CIVILSCHEDULE-0001 – Nova Civil Schedule

## Status

Angenommen

## Kategorie

Time / Civil Scheduling

## Zweck

NovaOS definiert Civil Schedules für Ereignisse, die an menschliche Kalenderzeit statt an eine feste Zeitdauer gebunden sind.

Damit können Regeln wie „jeden Montag um 08:00 Uhr“ oder „am ersten Tag jedes Monats um 09:00 Uhr“ korrekt über Zeitzonen-, Kalender- und Sommerzeitänderungen hinweg behandelt werden.

## Grundprinzipien

```text
Civil Schedule ≠ Periodic Timer
Civil Time ≠ Duration
Local Time ≠ UTC Instant
Time Zone ≠ UTC Offset
Recurrence Rule ≠ Fixed Interval
DST Change ≠ Schedule Drift
```

## Modell

```text
CivilSchedule
├── ScheduleID
├── OwnerID
├── CalendarID
├── TimeZoneID
├── LocalTime
├── RecurrenceRule
├── AmbiguityPolicy
├── GapPolicy
├── MissPolicy
├── WakePolicy
└── State
```

## Architektur

```text
Civil Schedule
      ↓
Calendar Rules
      ↓
Time Zone Rules
      ↓
Next Civil Occurrence
      ↓
UTC Instant
      ↓
Persistent Timer
```

Die nächste konkrete Ausführung wird erst durch Kalender- und Zeitzonenregeln in einen absoluten Instant aufgelöst.

## Wiederholungsregeln

Civil Schedules dürfen Regeln ausdrücken wie:

```text
Daily
Weekly
Monthly
Yearly
Specific Weekdays
Specific Calendar Dates
Nth Weekday
Custom Calendar Rule
```

Beispiel:

```text
Every Monday
08:00
Europe/Berlin
```

Dies ist nicht identisch mit:

```text
Every 168 Hours
```

## Zeitzonen

Ein Civil Schedule soll eine stabile `TimeZoneID` verwenden.

```text
08:00 Europe/Berlin
```

bleibt semantisch 08:00 Uhr Ortszeit, auch wenn sich der UTC-Offset durch Sommerzeit oder geänderte Zeitzonenregeln verändert.

## Mehrdeutige Zeit

Bei einer Zeitumstellung kann eine lokale Uhrzeit zweimal auftreten.

```text
02:30 → first occurrence
02:30 → second occurrence
```

Die `AmbiguityPolicy` muss bestimmen:

```text
First
Second
Both
Skip
RequireResolution
```

## Nicht existente Zeit

Bei einer Vorwärtsumstellung können lokale Zeiten fehlen.

Die `GapPolicy` bestimmt beispielsweise:

```text
Skip
NextValidTime
PreviousValidTime
RequireResolution
```

NovaOS darf keine stille willkürliche Entscheidung treffen.

## Kalender

Civil Schedules müssen mit dem angegebenen `CalendarID` ausgewertet werden.

Kalenderregeln dürfen nicht implizit auf den gregorianischen Kalender reduziert werden.

## Persistenz

Civil Schedules sollen über Neustarts hinweg erhalten bleiben.

Nur die nächste konkrete Ausführung wird als Instant geplant; die ursprüngliche Civil-Schedule-Regel bleibt die Source of Truth.

```text
Schedule Rule
    ↓
Next Occurrence
    ↓
Execute
    ↓
Resolve Next Occurrence
```

Dadurch entsteht keine kumulative Drift.

## Regeländerungen

Ändern sich Zeitzonen- oder Kalenderdaten:

```text
Rule Update
    ↓
Reevaluate Future Occurrences
    ↓
Preserve Civil Intent
```

Bereits vergangene Ereignisse werden dadurch nicht rückwirkend verändert.

## Normative Anforderungen

1. NovaOS MUSS Civil Schedules getrennt von periodischen Timern behandeln.
2. Civil Schedules MÜSSEN CalendarID und TimeZoneID explizit speichern können.
3. Wiederholungsregeln DÜRFEN nicht implizit in feste Zeitintervalle umgewandelt werden.
4. Die nächste Ausführung MUSS über Kalender- und Zeitzonenregeln bestimmt werden.
5. Mehrdeutige lokale Zeiten MÜSSEN über eine AmbiguityPolicy behandelt werden.
6. Nicht existente lokale Zeiten MÜSSEN über eine GapPolicy behandelt werden.
7. NovaOS DARF bei DST-Konflikten keine stille willkürliche Auflösung vornehmen.
8. Zeitzonenänderungen MÜSSEN zukünftige Ausführungen neu auflösbar machen.
9. Die ursprüngliche Civil-Schedule-Regel MUSS die Source of Truth bleiben.
10. Civil Schedules MÜSSEN persistent speicherbar sein.
11. Verpasste Ausführungen MÜSSEN über eine definierte MissPolicy behandelbar sein.
12. ScheduleID, Regel, Kalender, Zeitzone, nächste Ausführung und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CIVIL-0001`
- `NPSPEC-TIME-PERIODIC-0001`
- `NPSPEC-TIME-PERSISTENTTIMER-0001`
- `NPSPEC-GLOBALIZATION-CALENDAR-0001`
- `NPSPEC-GLOBALIZATION-DATETIME-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann wiederkehrende Ereignisse anhand menschlicher Kalender- und Ortszeitregeln planen, ohne sie fälschlich als feste Zeitintervalle zu behandeln. Zeitzonenänderungen, Sommerzeit, Kalenderregeln und mehrdeutige oder nicht existente lokale Zeiten bleiben dabei explizit und deterministisch behandelbar.