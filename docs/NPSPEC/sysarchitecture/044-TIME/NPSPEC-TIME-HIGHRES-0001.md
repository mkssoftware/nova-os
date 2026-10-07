# NPSPEC-TIME-HIGHRES-0001 – Nova High-Resolution Time

## Status

Angenommen

## Kategorie

Time / High Resolution

## Zweck

NovaOS definiert eine hochauflösende Zeitbasis für präzise Zeitmessungen, Timer und Deadlines.

High-Resolution Time erweitert die allgemeine Time Architecture um eine möglichst feingranulare Zeitdarstellung, ohne eine höhere Genauigkeit vorzutäuschen als die zugrunde liegende Hardware tatsächlich bereitstellt.

## Grundprinzipien

```text
High Resolution ≠ High Accuracy
Resolution ≠ Precision
Precision ≠ Accuracy
High-Resolution Time ≠ Wall Clock
High-Resolution Timer ≠ Busy Waiting
Requested Precision ≠ Guaranteed Precision
```

## Modell

```text
HighResolutionClock
├── ClockID
├── ClockSourceID
├── Resolution
├── Precision
├── Accuracy
├── Frequency
├── Stability
├── ReadLatency
└── State
```

## Zeitbasis

High-Resolution Time basiert grundsätzlich auf einer monotonen Zeitdomäne:

```text
Hardware Counter
      ↓
Clock Source
      ↓
Calibration
      ↓
Monotonic Time
      ↓
High-Resolution Interface
```

Wall-Clock- oder Civil-Time-Korrekturen dürfen diese Zeitbasis nicht beeinflussen.

## Verwendung

High-Resolution Time ist vorgesehen für:

```text
High-Resolution Timers
Fine-Grained Deadlines
Performance Measurement
Profiling
Realtime Scheduling
Media Synchronization
Device Timing
Latency Measurement
```

Normale Anwendungen sollen keine hohe Auflösung anfordern, wenn eine gröbere Zeitbasis ausreichend ist.

## High-Resolution Timer

```text
Deadline
   ↓
High-Resolution Timer
   ↓
Timer Scheduler
   ↓
Hardware Timer
   ↓
Event
```

Die Timer-Infrastruktur darf mehrere logische Timer auf wenigen Hardware-Timern multiplexen.

## Genauigkeit

NovaOS muss unterscheiden zwischen:

```text
Requested Deadline
Actual Wake Time
Timer Error
Scheduling Delay
```

Ein hochauflösender Timer garantiert keinen exakt zum Zielzeitpunkt ausgeführten Task.

## Clock Sources

Geeignete Quellen können beispielsweise sein:

```text
Invariant TSC
HPET
Architecture Timer
High-Resolution SoC Counter
Paravirtualized Counter
Registered Clock Provider
```

NovaOS darf keine bestimmte Quelle voraussetzen.

## Energieeffizienz

Hohe Timerauflösung kann Energieverbrauch erhöhen.

NovaOS darf deshalb Timer zusammenfassen oder gröber behandeln, sofern deren Constraints dies erlauben.

```text
Timer Deadline
      +
Tolerance
      ↓
Timer Coalescing
```

Hard Deadlines dürfen dadurch nicht verletzt werden.

## Counter-Konvertierung

Hardwarecounter müssen kontrolliert in NovaOS-Zeiteinheiten umgerechnet werden.

```text
Counter Delta
     ×
Conversion Factor
     ↓
Time Duration
```

Umrechnung und Skalierung müssen Überläufe und unnötigen Präzisionsverlust vermeiden.

## Virtualisierung

Virtuelle Systeme dürfen paravirtualisierte oder emulierte High-Resolution Clock Sources bereitstellen.

Deterministische Umgebungen dürfen eine virtuelle hochauflösende Zeit verwenden.

## Fehlerverhalten

Wird die aktive Quelle instabil oder unzuverlässig:

```text
Detect
  ↓
Degrade Source
  ↓
Select Alternative
  ↓
Recalibrate
  ↓
Preserve Monotonicity
```

Falls keine geeignete High-Resolution-Quelle verfügbar ist, muss NovaOS auf eine gröbere sichere Zeitbasis zurückfallen können.

## Normative Anforderungen

1. NovaOS MUSS eine optionale hochauflösende monotone Zeitbasis unterstützen.
2. High-Resolution Time MUSS von Wall Clock und Civil Time getrennt bleiben.
3. Resolution, Precision und Accuracy MÜSSEN getrennte Eigenschaften bleiben.
4. Hohe Auflösung DARF nicht als Garantie hoher Genauigkeit dargestellt werden.
5. High-Resolution Timer MÜSSEN Deadlines mit der verfügbaren Hardwareauflösung planen können.
6. Angeforderte Deadline und tatsächlicher Ausführungszeitpunkt MÜSSEN unterscheidbar sein.
7. Mehrere logische Timer MÜSSEN auf Hardware-Timern multiplexbar sein.
8. Timer Coalescing DARF verwendet werden, sofern Constraints dies erlauben.
9. Hard Deadlines DÜRFEN durch Coalescing nicht verletzt werden.
10. Instabile Clock Sources MÜSSEN ersetzt oder degradiert werden können.
11. Ein sicherer Fallback auf gröbere Zeitauflösung MUSS möglich sein.
12. Clock Source, Auflösung, Genauigkeit, Timerfehler und tatsächliche Wake-Zeit MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS stellt eine hochauflösende monotone Zeitbasis für präzise Timer, Deadlines und Messungen bereit. Die Architektur nutzt die bestmögliche verfügbare Hardwareauflösung, ohne Auflösung mit Genauigkeit gleichzusetzen, und kann Energieeffizienz, Echtzeitanforderungen und Hardwaregrenzen kontrolliert gegeneinander abwägen.