# NPSPEC-TIME-LEAPSECOND-0001 – Nova Leap Second

## Status

Angenommen

## Kategorie

Time / Leap Second

## Zweck

NovaOS definiert den kontrollierten Umgang mit Schaltsekunden bei der Abbildung zwischen kontinuierlichen internen Zeitbasen und UTC.

Schaltsekunden dürfen monotone Zeit, Timer, Deadlines und Scheduling nicht beeinträchtigen.

## Grundprinzipien

```text
Leap Second ≠ Monotonic Time Adjustment
UTC ≠ Monotonic Time
Leap Second ≠ Time Zone
Leap Second ≠ DST
TAI ≠ UTC
Wall Clock Adjustment ≠ Clock Source Adjustment
```

## Architektur

```text
Monotonic Time
      ↓
Timekeeping Core
      ↓
UTC Mapping
      +
Leap Second Data
      ↓
UTC / Civil Time
```

Schaltsekunden werden in der UTC-Abbildung behandelt und nicht durch Veränderung der zugrunde liegenden Clock Source erzeugt.

## Modell

```text
LeapSecondEvent
├── EventID
├── EffectiveUTC
├── Direction
├── OffsetBefore
├── OffsetAfter
├── Source
├── TrustState
└── State
```

Unterstützte Richtungen:

```text
Positive
Negative
```

Auch wenn negative Schaltsekunden historisch nicht aufgetreten sind, muss das Modell sie grundsätzlich darstellen können.

## Positive Schaltsekunde

Eine positive Schaltsekunde fügt eine zusätzliche UTC-Sekunde ein:

```text
23:59:59
23:59:60
00:00:00
```

Die interne monotone Zeit läuft dabei normal weiter.

## Negative Schaltsekunde

Eine negative Schaltsekunde entfernt eine UTC-Sekunde.

Die daraus entstehende UTC-Abbildung muss explizit behandelt werden und darf keine monotone Zeitdiskontinuität erzeugen.

## Datenquelle

Leap-Second-Informationen dürfen aus validierten Quellen stammen:

```text
Time Synchronization Provider
NTP
PTP
Trusted Time Data
System Time Database
Administrative Update
```

Unbestätigte Informationen dürfen nicht ungeprüft übernommen werden.

## Anwendung

Vor einem bekannten Ereignis:

```text
Receive Leap Information
        ↓
Validate
        ↓
Schedule UTC Mapping Change
        ↓
Apply at Effective Instant
        ↓
Verify
```

Die Änderung betrifft ausschließlich die betroffenen UTC- und Civil-Time-Abbildungen.

## Leap Smearing

NovaOS darf Provider oder Umgebungen unterstützen, die eine Schaltsekunde über einen Zeitraum verteilen.

```text
Leap Step
oder
Leap Smear
```

Smearing muss explizit als Zeitpolitik erkennbar sein.

Unterschiedliche Smear-Verfahren dürfen nicht stillschweigend als identische UTC-Zeit behandelt werden.

## Timer und Deadlines

Interne Timer und Deadlines sollen monotone Clock Domains verwenden.

Dadurch gilt:

```text
Leap Second
    ↓
UTC changes
    ↓
Monotonic Timer unaffected
```

Eine Schaltsekunde darf keine Timer doppelt auslösen oder unbeabsichtigt überspringen.

## Persistierte Zeit

Zeitstempel müssen genügend Semantik besitzen, um relevante Unterschiede zwischen:

```text
Instant
UTC Representation
Civil Time
Clock Domain
```

eindeutig behandeln zu können.

## Fehlerverhalten

Fehlende oder widersprüchliche Leap-Second-Daten müssen sicher behandelt werden.

```text
Conflicting Data
      ↓
Mark Uncertain
      ↓
Retain Safe Mapping
      ↓
Request Better Reference
```

`Unknown` darf nicht als bestätigte Schaltsekunde interpretiert werden.

## Normative Anforderungen

1. NovaOS MUSS Schaltsekunden außerhalb monotoner Clock Domains behandeln.
2. Schaltsekunden DÜRFEN monotone Zeit nicht verändern.
3. Timer und Deadlines DÜRFEN durch Schaltsekunden nicht unbeabsichtigt doppelt ausgelöst oder übersprungen werden.
4. Positive und negative Schaltsekunden MÜSSEN modellierbar sein.
5. Leap-Second-Daten MÜSSEN vor Anwendung validiert werden.
6. UTC-Offset-Änderungen MÜSSEN explizit darstellbar sein.
7. Leap Smearing MUSS als explizite Policy unterstützt werden können.
8. Smear- und Step-Zeit DÜRFEN nicht stillschweigend gleichgesetzt werden.
9. NTP- und PTP-Provider MÜSSEN Leap-Informationen an die Time Architecture übergeben können.
10. Unsichere Leap-Informationen MÜSSEN als solche erkennbar bleiben.
11. Schaltsekunden MÜSSEN mit Civil-Time-Konvertierung integrierbar sein.
12. Ereignis, Quelle, Richtung, Offset, Policy und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CIVIL-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-TIME-NTP-0001`
- `NPSPEC-TIME-PTP-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt Schaltsekunden als kontrollierte Änderung der UTC-Abbildung und nicht als Veränderung der zugrunde liegenden Zeitbasis. Monotone Zeit, Timer, Deadlines und Scheduling bleiben kontinuierlich, während UTC, Civil Time, Leap Steps und optionale Smear-Verfahren eindeutig und introspektierbar behandelt werden.