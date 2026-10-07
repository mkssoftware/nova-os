# NPSPEC-TIME-SYNCHRONIZATION-0001 – Nova Time Synchronization

## Status

Angenommen

## Kategorie

Time / Synchronization

## Zweck

NovaOS definiert die Synchronisation der Wall Clock mit externen oder lokalen Referenzzeitquellen.

Zeitsynchronisation korrigiert die Zuordnung zwischen monotoner Zeit und realer Zeit, ohne die monotone Zeitbasis selbst zu verändern.

## Grundprinzipien

```text
Synchronization ≠ Calibration
Synchronization ≠ Clock Source
Synchronization ≠ RTC
Wall Clock Correction ≠ Monotonic Correction
External Time ≠ Automatically Trusted Time
Offset ≠ Drift
```

## Modell

```text
TimeSynchronization
├── SynchronizationID
├── ReferenceID
├── LocalTime
├── ReferenceTime
├── Offset
├── EstimatedError
├── Confidence
├── CorrectionMode
└── State
```

## Architektur

```text
Time Reference
      ↓
Reference Provider
      ↓
Validation
      ↓
Offset / Drift Estimation
      ↓
Synchronization Policy
      ↓
Wall Clock
```

Die monotone Zeit bleibt davon unabhängig.

## Referenzquellen

NovaOS darf mehrere Referenzen verwenden:

```text
Network Time Provider
Precision Time Provider
RTC
GNSS
Platform Provider
Hypervisor
Trusted Hardware
Manual Time
```

Mehrere Quellen dürfen miteinander verglichen werden.

## Synchronisationszustände

```text
Unsynchronized
Synchronizing
Synchronized
Degraded
Holdover
Failed
```

`Holdover` bedeutet, dass keine geeignete Referenz verfügbar ist und NovaOS die zuletzt bekannte Zeitqualität anhand lokaler Clock Sources weiterführt.

## Offset

NovaOS bestimmt die Abweichung zwischen lokaler und referenzierter Zeit:

```text
Offset = ReferenceTime - LocalTime
```

Kleine Abweichungen sollen bevorzugt graduell korrigiert werden.

## Slew

Bei kontrollierter Korrektur wird die Wall Clock vorübergehend leicht beschleunigt oder verlangsamt:

```text
Current Wall Clock
       ↓
Controlled Rate Adjustment
       ↓
Reference Alignment
```

Dies reduziert abrupte Zeitsprünge.

## Step

Bei großen Abweichungen darf eine direkte Korrektur notwendig sein:

```text
Old Wall Clock
      ↓
Validated Step
      ↓
New Wall Clock
```

Ein Step muss explizit erkennbar sein.

Monotone Clock Domains dürfen dadurch nicht springen.

## Drift

Langfristige Frequenzabweichungen dürfen aus Synchronisationsdaten geschätzt werden.

```text
Repeated Offset Measurements
           ↓
Drift Estimate
           ↓
Clock Discipline
```

Kalibrierung der zugrunde liegenden Clock Source bleibt dennoch ein getrenntes Konzept.

## Konflikt zwischen Quellen

Bei widersprüchlichen Referenzen muss NovaOS bewerten:

```text
Trust
Accuracy
Precision
Stability
Age
Estimated Error
Source Independence
Policy
```

Eine einzelne stark abweichende Quelle darf nicht ungeprüft die Systemzeit bestimmen.

## Verlust der Referenz

```text
Reference Lost
      ↓
Holdover
      ↓
Local Clock + Known Drift
      ↓
Reference Recovered
      ↓
Resynchronize
```

Die Unsicherheit der Wall Clock soll während Holdover mit zunehmender Dauer wachsen können.

## Sicherheit

Externe Zeitinformationen müssen entsprechend ihrer Herkunft validiert werden.

Zeitmanipulation darf nicht ungeprüft sicherheitskritische Zustände beeinflussen.

Sicherheitsrelevante Zeitentscheidungen können zusätzliche Trust-, Attestation- oder Rollback-Schutzmechanismen benötigen.

## Normative Anforderungen

1. NovaOS MUSS Wall-Clock-Synchronisation unterstützen können.
2. Synchronisation und Clock-Source-Kalibrierung MÜSSEN getrennte Mechanismen bleiben.
3. Synchronisation DARF monotone Clock Domains nicht rückwärts oder vorwärts springen lassen.
4. Mehrere Zeitreferenzen MÜSSEN unterstützt werden können.
5. Referenzen MÜSSEN hinsichtlich Qualität und Vertrauenswürdigkeit bewertbar sein.
6. Kleine Abweichungen SOLLEN bevorzugt durch Slewing korrigiert werden.
7. Große Abweichungen MÜSSEN kontrollierte Steps erlauben können.
8. Zeit-Steps MÜSSEN introspektierbar sein.
9. Verlust externer Referenzen MUSS einen Holdover-Betrieb ermöglichen können.
10. Unsicherheit MUSS während Holdover darstellbar sein.
11. Konflikte zwischen Zeitquellen MÜSSEN sicher behandelt werden.
12. Referenz, Offset, Drift, Confidence, Korrekturmodus und Synchronisationszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-CALIBRATION-0001`
- `NPSPEC-TIME-RTC-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann seine Wall Clock kontrolliert mit lokalen und externen Zeitreferenzen synchronisieren. Offset, Drift, Unsicherheit und Vertrauenswürdigkeit werden berücksichtigt, während monotone Zeitdomänen unabhängig bleiben und auch beim Verlust externer Referenzen ein definierter Holdover-Betrieb möglich ist.