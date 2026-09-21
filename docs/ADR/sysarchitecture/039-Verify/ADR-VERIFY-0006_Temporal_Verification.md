# ADR-VERIFY-0006 – Temporal Verification

## Status

Angenommen

## Kategorie

Verification / Realtime / Timing / Determinism

## Kontext

Für Realtime-Komponenten reicht funktionale Korrektheit allein nicht aus.

Eine Operation kann das richtige Ergebnis liefern und trotzdem fehlerhaft sein, wenn sie zu spät abgeschlossen wird.

```text
Correct Result
+
Correct Time
=
Temporal Correctness
```

NovaOS benötigt deshalb überprüfbare Verfahren für zeitkritische Ausführungspfade.

## Entscheidung

NovaOS behandelt **Temporal Verification als eigenständige Verifikationsdimension** für zeitkritische Komponenten und Execution Contracts.

Überprüft werden insbesondere:

```text
Worst-Case Latency
Deadline Compliance
Execution Budget
Blocking Time
Jitter
I/O Latency
Scheduling Interference
```

Temporal Verification wird entsprechend dem jeweiligen Realtime-Profil angewendet.

## Verifikationsmodell

Zeitliche Garantien müssen den vollständigen relevanten Ausführungspfad berücksichtigen.

```text
Scheduling
   +
Blocking
   +
Execution
   +
Memory
   +
IPC
   +
I/O
   =
End-to-End Latency
```

Ein lokal begrenzter Teilpfad darf nicht als Garantie für den gesamten Pfad interpretiert werden.

## Hard Realtime

Für Hard-Realtime-Ausführung müssen benötigte obere Schranken vor der Annahme eines Execution Contracts bekannt oder konservativ ableitbar sein.

```text
Worst-Case Bound
      ≤
Deadline
```

Ist eine notwendige zeitliche Grenze unbekannt, darf daraus keine Hard-Realtime-Garantie abgeleitet werden.

## Soft und Firm Realtime

Soft- und Firm-Realtime dürfen statistische und empirische Verfahren ergänzend verwenden.

Beispiele:

```text
Latency Distribution
Percentiles
Jitter Analysis
Deadline Miss Rate
Runtime Measurements
```

Statistische Ergebnisse dürfen jedoch nicht als mathematische Hard-Realtime-Garantie dargestellt werden.

## Runtime Verification

NovaOS überwacht zeitkritische Ausführung auch während des Betriebs.

```text
Expected Temporal Contract
           ↓
Runtime Measurement
           ↓
Compare
           ↓
Violation / Valid
```

Runtime-Messungen ergänzen die vorherige Analyse, ersetzen sie bei Hard Realtime jedoch nicht.

## Abweichungen

Eine erkannte zeitliche Verletzung muss als explizites Systemereignis behandelt werden.

Abhängig vom Realtime-Profil kann NovaOS:

```text
Report
Degrade
Reschedule
Abort
Recover
Enter Safe State
```

Eine Deadline-Verletzung darf bei Hard Realtime nicht stillschweigend ignoriert werden.

## Determinismus

Temporal Verification und deterministische Ausführung ergänzen sich, sind jedoch nicht identisch.

```text
Deterministic Execution
≠
Guaranteed Deadline
```

Eine deterministische Operation kann konstant zu langsam sein.

## Normative Anforderungen

1. Zeitkritische Komponenten MÜSSEN zeitlich überprüfbare Contracts besitzen können.
2. Hard-Realtime-Garantien MÜSSEN auf bekannten oder konservativ bestimmten oberen Schranken basieren.
3. End-to-End-Verifikation MUSS alle relevanten Ausführungskomponenten berücksichtigen.
4. Unbekannte zeitliche Grenzen DÜRFEN nicht als garantiert behandelt werden.
5. Statistische Messwerte DÜRFEN keine Hard-Realtime-Garantie ersetzen.
6. Realtime-Ausführung MUSS zur Laufzeit auf relevante zeitliche Verletzungen überwachbar sein.
7. Deadline-Verletzungen MÜSSEN entsprechend dem Realtime-Profil behandelt werden.
8. Temporal Verification MUSS Scheduling-, Blocking- und I/O-Einflüsse berücksichtigen.
9. Verifikationsergebnisse MÜSSEN der jeweiligen Software-, Hardware- und Contract-Version zuordenbar sein.

## Konsequenzen

### Positiv

- Realtime-Garantien werden überprüfbar,
- zeitliche Fehler werden von funktionalen Fehlern getrennt betrachtet,
- Deadline- und Latenzanforderungen können systematisch validiert werden,
- Runtime-Messungen können Abweichungen früh erkennen.

### Negativ

- Worst-Case-Analysen können aufwendig sein,
- Hardware- und I/O-Verhalten erschweren feste obere Grenzen,
- Änderungen an kritischen Ausführungspfaden können erneute Verifikation erfordern.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-REALTIME-0005_Deterministic_Execution`
- `ADR-REALTIME-0006_Deterministic_IO`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`

## Ergebnis

NovaOS überprüft bei zeitkritischen Komponenten nicht nur:

```text
"Was ist das Ergebnis?"
```

sondern auch:

```text
"Wurde das Ergebnis innerhalb der garantierten Zeit erreicht?"
```

Damit wird zeitliche Korrektheit zu einer explizit überprüfbaren Systemeigenschaft.