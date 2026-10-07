# NPSPEC-TIME-MONOTONIC-0001 – Nova Monotonic Time

## Status

Angenommen

## Kategorie

Time / Monotonic

## Zweck

NovaOS definiert Monotonic Time als kontinuierlich vorwärtslaufende Zeitbasis für interne Zeitmessung.

Sie wird für Zeitdauern, Timeouts, Deadlines, Scheduling und Performance-Messungen verwendet und bleibt unabhängig von Änderungen der realen Kalenderzeit.

## Grundprinzipien

```text
Monotonic Time ≠ Wall Clock
Monotonic Time ≠ Civil Time
Monotonic Time ≠ UTC
Monotonic Time ≠ RTC
Monotonic Time ≠ CPU Time
Clock Correction ≠ Monotonic Jump
```

## Modell

```text
MonotonicTime
├── Counter
├── Epoch
├── Resolution
├── Precision
├── ClockSourceID
├── SuspendBehavior
└── State
```

Der absolute Zahlenwert besitzt keine zivile Bedeutung.

Relevant sind insbesondere Differenzen zwischen zwei Zeitpunkten.

## Eigenschaft

Für zwei aufeinanderfolgende gültige Abfragen gilt:

```text
T₂ >= T₁
```

Die monotone Zeit darf während des normalen Betriebs niemals aufgrund einer Wall-Clock-Korrektur rückwärts springen.

## Verwendung

Monotonic Time ist die bevorzugte Zeitbasis für:

```text
Timeouts
Deadlines
Durations
Scheduler
Rate Limiting
Retry Delays
Performance Measurement
Lease Expiration
Internal Timers
```

Beispiel:

```text
Start = MonotonicNow()
Deadline = Start + 5s
```

Eine Änderung der Systemuhr beeinflusst diese Deadline nicht.

## Clock Source

```text
Hardware Clock Source
        ↓
Calibration
        ↓
Timekeeping Core
        ↓
Monotonic Time
```

Die zugrunde liegende Clock Source darf zur Laufzeit gewechselt werden, sofern die Kontinuität der monotonen Zeit erhalten bleibt.

## Clock-Source-Wechsel

```text
Source A
   ↓
Capture Current Time
   ↓
Switch / Calibrate
   ↓
Source B
   ↓
Continue Monotonic Timeline
```

Der Wechsel darf keinen sichtbaren Rücksprung erzeugen.

## Suspend

NovaOS muss mindestens unterscheiden können zwischen:

```text
Active Monotonic Time
Boot Time Including Suspend
```

Eine monotone Zeitdomäne darf Suspend-Zeit ausschließen.

Eine separate monotone Boot-Zeit darf die während Suspend vergangene Zeit einschließen.

Die Semantik der verwendeten Zeitdomäne muss eindeutig sein.

## Frequenzabweichungen

Hardwarezähler können Drift oder Frequenzabweichungen besitzen.

NovaOS darf deshalb:

```text
Calibrate
Compensate
Compare Clock Sources
Detect Drift
Replace Unstable Sources
```

Die Korrektur darf die monotone Eigenschaft nicht verletzen.

## Auflösung

Monotonic Time muss eine definierte Auflösung besitzen.

```text
Resolution ≠ Precision
Resolution ≠ Accuracy
```

Aufrufer dürfen keine höhere tatsächliche Genauigkeit annehmen als die Zeitquelle bereitstellt.

## Überlauf

Interne Repräsentationen müssen so gewählt werden, dass Counter-Überläufe während realistischer Systemlaufzeiten vermieden oder korrekt behandelt werden.

Zeitvergleiche dürfen nicht durch numerische Überläufe fehlerhaft werden.

## Determinismus

Deterministische oder simulierte Ausführungsumgebungen dürfen eine virtuelle monotone Zeit bereitstellen.

```text
Real Clock
oder
Virtual Deterministic Clock
        ↓
Monotonic Interface
```

Die Semantik der Schnittstelle bleibt dabei identisch.

## Fehlerverhalten

Wird eine Clock Source instabil:

```text
Detect Instability
      ↓
Mark Source Degraded
      ↓
Select Alternative
      ↓
Calibrate
      ↓
Continue Timeline
```

Fehlende oder unsichere Zeitinformationen dürfen nicht als gültige Zeitdifferenzen ausgegeben werden.

## Normative Anforderungen

1. NovaOS MUSS eine monotone Zeitbasis bereitstellen.
2. Monotonic Time MUSS von Wall Clock und Civil Time getrennt bleiben.
3. Wall-Clock-Korrekturen DÜRFEN Monotonic Time nicht rückwärts verändern.
4. Timeouts und interne Deadlines SOLLEN Monotonic Time verwenden.
5. Clock Sources MÜSSEN austauschbar sein können, ohne die monotone Zeitlinie zu unterbrechen.
6. Drift und instabile Clock Sources MÜSSEN erkannt werden können.
7. Suspend-Verhalten MUSS für jede monotone Zeitdomäne eindeutig definiert sein.
8. Eine monotone Zeitdomäne inklusive Suspend-Zeit MUSS bereitgestellt werden können.
9. Resolution, Precision und Accuracy MÜSSEN getrennt behandelbar sein.
10. Counter-Überläufe DÜRFEN Zeitvergleiche nicht verfälschen.
11. Virtuelle monotone Zeit MUSS für deterministische Umgebungen unterstützt werden können.
12. ClockSourceID, Zeitdomäne, Auflösung, Suspend-Verhalten und Source State MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine stabile monotone Zeitbasis für alle internen zeitabhängigen Abläufe. Timeouts, Deadlines und Zeitmessungen bleiben dadurch unabhängig von Änderungen der Kalenderzeit, Zeitzonen und Zeitsynchronisation und können selbst bei Clock-Source-Wechseln oder Suspend eindeutig behandelt werden.