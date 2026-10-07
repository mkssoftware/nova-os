# NPSPEC-TIME-INTROSPECTION-0001 – Nova Time Introspection

## Status

Angenommen

## Kategorie

Time / Introspection

## Zweck

NovaOS stellt einen einheitlichen Introspektionszugriff auf Zustand, Qualität, Herkunft und Entscheidungen der gesamten Time Architecture bereit.

Time Introspection dient Beobachtung, Diagnose, Debugging, Monitoring und Systemanalyse. Sie erzeugt keine zusätzliche Authority über Zeitquellen oder Zeitkonfiguration.

## Grundprinzipien

```text
Introspection ≠ Authority
Observation ≠ Modification
Reported Time ≠ Guaranteed Accuracy
Measured ≠ Estimated
Unknown ≠ Zero
Available ≠ Trusted
Synchronized ≠ Exact
```

## Architektur

```text
Clock Sources ───────┐
Clock Domains ───────┤
Timers / Deadlines ──┤
Synchronization ─────┤
RTC / NTP / PTP ─────┤
Virtual Time ────────┤
Trusted Time ────────┼─→ Time Introspection
Distributed Time ────┤
Media Time ──────────┤
Civil Schedules ─────┘
```

## Beobachtbare Bereiche

NovaOS muss mindestens introspektierbar machen können:

```text
Clock Sources
Clock Domains
Current Time Values
Resolution / Precision
Calibration
Drift
Synchronization
Uncertainty
Trust State
Timers
Deadlines
Timer Coalescing
Persistent Timers
Virtual Clocks
Time Namespaces
Civil Schedules
Media Clocks
Distributed Time
```

## Clock Sources

Für Clock Sources sollen mindestens sichtbar sein:

```text
ClockSourceID
Provider
Frequency
Resolution
Stability
Validation State
Calibration State
Active / Inactive
```

## Clock Domains

Für Clock Domains:

```text
ClockDomainID
Type
Source
Epoch
Rate
Offset
Suspend Behavior
State
```

## Timer und Deadlines

NovaOS muss zeitkritische Planung diagnostizierbar machen können:

```text
TimerID
Owner
Clock Domain
Deadline
Tolerance
Effective Wakeup
Expiration
Execution Delay
Deadline Miss
State
```

## Synchronisation

Synchronisationszustände sollen mindestens enthalten:

```text
Reference Source
Offset
Drift
Uncertainty
Confidence
Last Synchronization
Correction Mode
Holdover State
```

Bei NTP/PTP dürfen providerspezifische Detailinformationen ergänzt werden.

## Herkunft

Zeitinformationen müssen ihre Herkunft nachvollziehbar machen können:

```text
Value
 ↓
Clock Domain
 ↓
Clock Source / Reference
 ↓
Provider
 ↓
Quality / Trust
```

## Entscheidungsnachvollziehbarkeit

NovaOS soll erklären können, warum beispielsweise:

```text
Clock Source gewechselt
Timer zusammengelegt
Deadline verpasst
NTP Peer verworfen
PTP Grandmaster gewechselt
Wall Clock korrigiert
Trusted Time degradiert
```

wurde.

## Historie

Für Diagnosezwecke darf eine begrenzte Historie relevanter Ereignisse geführt werden:

```text
Clock Source Changes
Calibration Events
Time Steps
Synchronization Loss
Holdover
Deadline Misses
Leap Events
Trust Changes
```

Die Historie ist kein vollständiges Audit-Log, sofern sie nicht explizit an das Audit-System übergeben wird.

## Sicherheit und Datenschutz

Introspektionsdaten können Informationen über:

```text
User Activity
Application Activity
Network Infrastructure
Device Behavior
Execution Timing
```

offenlegen.

Der Zugriff muss daher capability- und policybasiert begrenzbar sein.

Introspektion darf keine Änderungsrechte implizieren.

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche Time-Introspection-Schnittstelle bereitstellen.
2. Clock Sources und Clock Domains MÜSSEN introspektierbar sein.
3. Timer, Deadlines und Deadline Misses MÜSSEN diagnostizierbar sein.
4. Kalibrierungs- und Synchronisationszustände MÜSSEN sichtbar sein können.
5. Gemessene und geschätzte Werte MÜSSEN unterscheidbar bleiben.
6. Unsicherheit und Confidence MÜSSEN darstellbar sein.
7. `Unknown` DARF nicht als `0`, `Valid` oder `Trusted` dargestellt werden.
8. Herkunft und Provider einer Zeitinformation MÜSSEN nachvollziehbar sein.
9. Relevante Time-Entscheidungen SOLLEN erklärbar sein.
10. Eine begrenzte Diagnosehistorie MUSS unterstützt werden können.
11. Introspektion DARF keine zusätzliche Authority erzeugen.
12. Zugriff auf sensible Zeit- und Aktivitätsinformationen MUSS durch Capability und Policy kontrollierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-CALIBRATION-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-TIME-TRUSTED-0001`
- `NPSPEC-TIME-DISTRIBUTED-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS macht seine Time Architecture vollständig beobachtbar, ohne Beobachtung mit Kontrolle zu vermischen. Zeitquellen, Clock Domains, Timer, Deadlines, Synchronisation, Drift, Unsicherheit und Trust können einheitlich diagnostiziert und ihre relevanten Entscheidungen nachvollzogen werden.