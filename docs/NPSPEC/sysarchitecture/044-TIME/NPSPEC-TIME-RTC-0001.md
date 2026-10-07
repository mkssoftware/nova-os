# NPSPEC-TIME-RTC-0001 – Nova Real-Time Clock

## Status

Angenommen

## Kategorie

Time / RTC

## Zweck

NovaOS definiert den Umgang mit hardwaregestützten Real-Time Clocks (RTC).

Die RTC dient primär dazu, eine persistente Zeitreferenz über Neustarts und ausgeschaltete Systemzustände hinweg bereitzustellen. Sie ist weder die primäre Laufzeit-Zeitquelle noch die monotone Systemzeit.

## Grundprinzipien

```text
RTC ≠ Wall Clock
RTC ≠ Monotonic Time
RTC ≠ Civil Time
RTC ≠ High-Resolution Clock
RTC Value ≠ Trusted Time
Hardware Clock ≠ Time Zone
```

## Modell

```text
RTCDevice
├── RTCID
├── Provider
├── CurrentValue
├── Resolution
├── Accuracy
├── Drift
├── AlarmCapabilities[]
├── BatteryState
└── State
```

Mehrere RTC-Provider dürfen gleichzeitig vorhanden sein.

## Architektur

```text
RTC Hardware
    ↓
RTC Provider
    ↓
Validation
    ↓
Timekeeping Core
    ↓
Initial Wall Clock
```

Nach Initialisierung übernimmt das Timekeeping Core die laufende Zeitverwaltung.

Die RTC darf nicht für normale Deadlines, Timeouts oder Scheduling verwendet werden.

## Systemstart

Beim Boot kann NovaOS eine RTC als initiale persistente Zeitquelle verwenden:

```text
Boot
 ↓
Read RTC
 ↓
Validate
 ↓
Establish Initial Wall Clock
 ↓
External Synchronization
```

Ein RTC-Wert darf nicht ungeprüft als korrekt oder vertrauenswürdig betrachtet werden.

## RTC-Zeitbasis

NovaOS soll RTC-Werte intern bevorzugt als UTC interpretieren und speichern.

```text
RTC
 ↓
UTC
 ↓
Time Zone
 ↓
Civil Time
```

Lokale Zeitzonen und Sommerzeit gehören nicht in die Hardware-Zeitbasis.

Für Plattformkompatibilität dürfen abweichende RTC-Konventionen über Provider behandelt werden.

## Synchronisation

Die Wall Clock darf bei geeigneten Ereignissen zurück in die RTC geschrieben werden:

```text
Validated Wall Clock
       ↓
RTC Synchronization
       ↓
Persistent Time Reference
```

Schreibvorgänge müssen kontrolliert erfolgen und unnötige Hardwarezugriffe vermeiden.

## Drift

RTC-Hardware kann Zeitabweichungen besitzen.

NovaOS darf:

```text
Measure Drift
Estimate Drift
Compensate Drift
Record Calibration
```

Eine Drift-Kompensation verändert nicht die physische Genauigkeit der RTC.

## RTC Alarm

Unterstützte RTCs dürfen Alarmfunktionen bereitstellen:

```text
RTC Alarm
    ↓
Wake Source
    ↓
Power Wake Management
```

RTC Alarm und allgemeine Timer bleiben getrennte Mechanismen.

Ein RTC Alarm darf als Wake Source für unterstützte Power States registriert werden.

## Mehrere RTCs

Sind mehrere RTCs vorhanden, darf NovaOS anhand von:

```text
Availability
Accuracy
Stability
Platform Preference
Trust
```

eine bevorzugte RTC auswählen.

Abweichungen zwischen mehreren RTCs müssen diagnostizierbar sein.

## Fehlerverhalten

Ungültige RTC-Werte müssen erkannt werden können:

```text
Invalid Value
Battery Failure
Clock Stopped
Implausible Date
Read Failure
Write Failure
```

Fallback:

```text
RTC unavailable
      ↓
Alternative RTC
      ↓
Persisted Time Hint
      ↓
External Time Synchronization
```

Fehlende RTC-Unterstützung darf den normalen Systembetrieb nicht verhindern.

## Sicherheit

Die RTC ist keine vertrauenswürdige Sicherheitsquelle.

Eine manipulierte RTC darf nicht allein verwendet werden für sicherheitskritische Entscheidungen wie:

```text
Certificate Validity
Credential Expiration
Rollback Protection
Trust Decisions
Security Token Validity
```

Solche Entscheidungen benötigen geeignete zusätzliche Vertrauens- und Zeitmechanismen.

## Normative Anforderungen

1. NovaOS MUSS RTC und laufende Systemzeit getrennt behandeln.
2. RTC DARF nicht als monotone Zeitquelle für Deadlines verwendet werden.
3. RTC-Werte MÜSSEN vor Verwendung validiert werden.
4. NovaOS SOLL RTC intern bevorzugt als UTC behandeln.
5. Zeitzonen DÜRFEN nicht Bestandteil der allgemeinen RTC-Semantik sein.
6. Abweichende Plattformkonventionen MÜSSEN über Provider behandelbar sein.
7. RTC-Drift MUSS erkennbar und kompensierbar sein können.
8. Mehrere RTC-Provider MÜSSEN unterstützt werden können.
9. RTC-Alarme MÜSSEN mit Wake Management integrierbar sein.
10. Fehlende oder fehlerhafte RTC-Hardware DARF den normalen Systembetrieb nicht verhindern.
11. RTC DARF nicht allein als vertrauenswürdige Sicherheitszeit verwendet werden.
12. RTCID, Wert, Quelle, Drift, Alarmfähigkeit, Batteriezustand und Fehlerstatus MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CIVIL-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-POWER-WAKE-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt die RTC als persistente Hardware-Zeitreferenz für Boot und ausgeschaltete Systemzustände. Laufende Zeitmessung, monotone Zeit, Civil Time und Sicherheitsentscheidungen bleiben davon getrennt, während RTC-Drift, mehrere Hardwarequellen und RTC-basierte Wake-Alarme kontrolliert unterstützt werden.