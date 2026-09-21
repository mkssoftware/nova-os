# ADR-REALTIME-0003 – Bounded Latency

## Status

Angenommen

## Kategorie

Realtime / Latency / Scheduling / Execution Contracts

## Kontext

Realtime-Systeme benötigen nicht nur niedrige durchschnittliche Latenzen, sondern vor allem **bekannte obere Grenzen**.

Eine Operation mit:

```text
Average Latency: 1 ms
Worst Case: 500 ms
```

ist für viele Realtime-Anwendungen ungeeignet.

Entscheidend ist deshalb nicht nur:

```text
How fast is it usually?
```

sondern:

```text
How long can it take at most?
```

NovaOS benötigt ein systemweites Modell für begrenzbare Latenzen entlang vollständiger Execution Paths.

## Entscheidung

NovaOS führt **Bounded Latency** als explizite Eigenschaft von Realtime-Executions und Ressourcen ein.

```text
Request
   ↓
Scheduling
   ↓
Execution
   ↓
Dependencies
   ↓
I/O
   ↓
Result

Total Latency ≤ Defined Bound
```

Eine Latenzgarantie darf nur zugesichert werden, wenn eine belastbare obere Grenze für den relevanten Ausführungspfad existiert.

## Latency Bound

Ein Execution Contract kann eine maximale zulässige Latenz definieren.

Beispiel:

```text
Maximum Latency: 5 ms
```

Dies bedeutet:

```text
Start → Completion ≤ 5 ms
```

sofern der Contract erfolgreich angenommen wurde.

## End-to-End Latency

NovaOS betrachtet die gesamte relevante Latenz.

```text
Total Latency =
    Scheduling Delay
  + Blocking Time
  + Execution Time
  + Memory Delay
  + I/O Delay
  + Communication Delay
```

Nur die reine CPU-Ausführungszeit zu begrenzen reicht nicht aus.

## Worst-Case statt Durchschnitt

Für garantierte Realtime-Pfade ist die obere Grenze maßgeblich.

```text
Average Latency
    ≠
Latency Guarantee
```

Messwerte wie Durchschnitt, Median oder P99 dürfen zur Optimierung verwendet werden, ersetzen aber keine erforderliche Worst-Case-Grenze.

## Scheduling Latency

Die Zeit zwischen Ausführungsbereitschaft und tatsächlichem Start muss begrenzbar sein.

```text
Ready
  ↓
Scheduling Delay
  ↓
Running
```

Für Hard Realtime muss diese Verzögerung Bestandteil der Garantie sein.

## Blocking Latency

Blockierende Operationen auf einem Realtime-Pfad benötigen bekannte Grenzen.

Beispiele:

```text
Locks
IPC
Driver Calls
Memory Allocation
Filesystem Operations
Device Access
```

Unbounded Blocking ist mit garantierter Hard-Realtime-Ausführung nicht vereinbar.

## Interrupt Latency

Interrupts müssen innerhalb definierter Grenzen verarbeitet werden können.

```text
Interrupt
    ↓
Dispatch
    ↓
Handler
```

Unbegrenzte Interrupt-Deaktivierung oder unbounded Interrupt Handler sind auf Hard-Realtime-Pfaden nicht zulässig.

## Memory Latency

Realtime-Ausführungen dürfen nicht von unvorhersehbaren Speicheroperationen abhängig sein.

Kritische Pfade sollen daher möglichst vermeiden:

```text
Page Faults
Swap
Unbounded Allocation
Memory Reclamation
Unexpected Copying
```

Erforderlicher Speicher wird bevorzugt vorab reserviert.

## I/O Latency

Geräte und Treiber müssen ihre zeitlichen Eigenschaften beschreiben können.

```text
Execution
   ↓
Driver
   ↓
Device
```

Besitzt ein notwendiges Gerät keine belastbare obere Latenzgrenze, kann der gesamte Pfad keine stärkere Garantie erhalten.

## IPC Latency

Auch Interprozesskommunikation kann Teil eines Realtime-Pfades sein.

```text
Task A
  ↓
IPC
  ↓
Service B
```

Für garantierte Pfade müssen Queueing, Dispatch und Verarbeitung entsprechend begrenzbar sein.

## Capability Chains

Eine Capability darf keine stärkere Latenzgarantie anbieten als ihre Abhängigkeiten ermöglichen.

```text
Capability A
    ↓
Capability B
    ↓
Driver C
    ↓
Device D
```

Es gilt:

```text
End-to-End Bound
≥
Summe der relevanten Teilgrenzen
```

inklusive notwendiger Scheduling- und Kommunikationsreserven.

## Latency Budgets

Ein Gesamtbudget kann auf Subsysteme verteilt werden.

Beispiel:

```text
Total Budget:        10 ms

Scheduling:           1 ms
Processing:           4 ms
IPC:                  1 ms
Device I/O:           3 ms
Reserve:              1 ms
```

Dadurch können einzelne Komponenten prüfen, ob der End-to-End-Contract erfüllbar bleibt.

## Admission Control

Vor Annahme einer garantierten Latenz prüft NovaOS:

```text
Requested Bound
      ↓
Execution Path
      ↓
Known Component Bounds
      ↓
Available Resources
      ↓
Guarantee Possible?
```

Ist die Garantie nicht nachweisbar:

```text
Reject
```

oder:

```text
Renegotiate Contract
```

Eine unbekannte Latenz darf nicht als ausreichend interpretiert werden.

## Unknown Latency

Es gilt:

```text
Unknown Bound
    ≠
Bounded Latency
```

Ein Subsystem ohne bekannte obere Grenze kann nicht Bestandteil eines garantierten Hard-Realtime-Pfades sein, sofern keine andere Isolation oder nachweisbare Begrenzung existiert.

## Temporal Isolation

Bounded Latency baut auf Temporal Isolation auf.

```text
Bounded Latency
      requires
Temporal Isolation
```

Ohne Schutz vor konkurrierenden Workloads kann eine zugesicherte obere Latenzgrenze nicht zuverlässig eingehalten werden.

## Jitter

Neben der maximalen Latenz kann ein Contract maximale zeitliche Schwankung definieren.

```text
Maximum Latency
Maximum Jitter
```

Latenz und Jitter werden getrennt behandelt.

## Soft und Firm Realtime

Bei Soft und Firm Realtime dürfen statistische Latenzmodelle verwendet werden.

Beispielsweise:

```text
Expected Latency
P95
P99
Observed Maximum
```

Diese Werte dürfen jedoch nicht als Hard-Realtime-Garantie dargestellt werden.

## Hard Realtime

Hard Realtime benötigt belastbare obere Grenzen.

```text
Hard Realtime
     ↓
Bounded Scheduling
Bounded Blocking
Bounded Execution
Bounded Dependencies
     ↓
Guaranteed Deadline
```

Kann eine dieser Grenzen nicht eingehalten werden, darf die Garantie nicht zugesichert werden.

## Adaptive Systeme

Adaptive Systeme dürfen Latenzen vorhersagen und optimieren.

```text
Predicted Latency
      ↓
Execution
      ↓
Measured Latency
      ↓
Prediction Error
```

Eine Vorhersage ersetzt jedoch keine Hard-Realtime-Garantie.

Adaptive Entscheidungen dürfen garantierte Bounds nicht verletzen.

## Distributed Execution

Bei Remote Execution müssen zusätzlich berücksichtigt werden:

```text
Network Delay
Remote Queueing
Remote Scheduling
Remote Execution
Response Transfer
```

Kann keine End-to-End-Obergrenze garantiert werden, darf der Pfad nicht für entsprechende Hard-Realtime-Contracts verwendet werden.

## Overrun

Wird eine zugesicherte Grenze überschritten:

```text
Observed Latency
      >
Guaranteed Bound
```

liegt eine `Latency Bound Violation` vor.

Diese wird entsprechend dem Realtime Profile behandelt.

```text
Soft → Record / Adapt

Firm → Result may be discarded

Hard → Realtime Failure
```

## Observability

NovaOS muss Latenzen entlang relevanter Execution Paths messen können.

Mindestens:

```text
Execution ID
Latency Bound
Observed Latency
Scheduling Delay
Blocking Time
Execution Time
I/O Delay
IPC Delay
Jitter
Bound Violation
```

Messung darf den zeitkritischen Pfad selbst nicht unkontrolliert verändern.

## Normative Anforderungen

1. NovaOS MUSS Bounded Latency als explizite Realtime-Eigenschaft unterstützen.
2. Latenzgrenzen MÜSSEN über Execution Contracts ausdrückbar sein.
3. Garantierte Latenz MUSS den relevanten End-to-End-Pfad berücksichtigen.
4. Durchschnittliche Latenz DARF nicht als garantierte obere Grenze behandelt werden.
5. Scheduling Latency MUSS für Hard Realtime begrenzbar sein.
6. Blocking Time auf Hard-Realtime-Pfaden MUSS begrenzbar sein.
7. Interrupt Latency MUSS auf entsprechenden Realtime-Pfaden begrenzbar sein.
8. Unbounded Memory Operations DÜRFEN nicht Teil eines garantierten Hard-Realtime-Pfades sein.
9. I/O-Abhängigkeiten MÜSSEN ihre erforderlichen Latenzeigenschaften bereitstellen können.
10. IPC-Latenzen MÜSSEN bei End-to-End-Garantien berücksichtigt werden.
11. Capability Chains DÜRFEN keine stärkere Garantie anbieten als ihre Abhängigkeiten.
12. End-to-End-Latenz SOLL in Teilbudgets zerlegbar sein.
13. Garantierte Latenzanforderungen MÜSSEN einem Admission Control unterliegen.
14. `Unknown Latency` DARF nicht als `Bounded Latency` behandelt werden.
15. Hard-Realtime-Latenzgarantien MÜSSEN auf ausreichender Temporal Isolation basieren.
16. Jitter MUSS unabhängig von der maximalen Latenz ausdrückbar sein.
17. Statistische Latenzmodelle DÜRFEN Hard-Realtime-Garantien nicht ersetzen.
18. Adaptive Systeme DÜRFEN garantierte Latenzgrenzen nicht verändern oder abschwächen.
19. Remote Execution DARF nur dann für garantierte Realtime-Pfade verwendet werden, wenn eine ausreichende End-to-End-Grenze besteht.
20. Überschreitungen garantierter Bounds MÜSSEN als explizite `Latency Bound Violation` erfasst werden.
21. Hard-Realtime-Bound-Verletzungen MÜSSEN als Realtime Failure behandelt werden.
22. Relevante Latenzkomponenten MÜSSEN beobachtbar sein.

## Konsequenzen

### Positive Konsequenzen

- Realtime wird anhand belastbarer oberer Grenzen statt Durchschnittswerten geplant,
- Engpässe können entlang kompletter Execution Paths erkannt werden,
- Latenzbudgets können systematisch auf Subsysteme verteilt werden,
- Hard-Realtime-Garantien werden technisch überprüfbar,
- adaptive Optimierung bleibt möglich, ohne Garantien zu ersetzen.

### Negative Konsequenzen

- Subsysteme und Treiber müssen ihre Worst-Case-Eigenschaften beschreiben können,
- manche Hardware oder Remote Provider können keine garantierten Bounds anbieten,
- End-to-End-Latenzanalyse erhöht den Planungsaufwand.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0003_Latency_und_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-AI-0013_KI_darf_harte_Systemregeln_nicht_überschreiben`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-BOUNDED-LATENCY-0001`
- `NPSPEC-REALTIME-LATENCY-BUDGET-0001`
- `NPSPEC-REALTIME-SCHEDULING-LATENCY-0001`
- `NPSPEC-REALTIME-BLOCKING-LATENCY-0001`
- `NPSPEC-REALTIME-IO-LATENCY-0001`
- `NPSPEC-REALTIME-LATENCY-VIOLATION-0001`

## Ergebnis

NovaOS bewertet Realtime-Latenz als vollständigen zeitlichen Pfad:

```text
Request
   ↓
Scheduling
   ↓
Blocking
   ↓
Execution
   ↓
IPC / I/O
   ↓
Result

───────────────
End-to-End Bound
```

Der zentrale Grundsatz lautet:

```text
Schnell im Durchschnitt
ist nicht Realtime.

Für eine echte Garantie
muss NovaOS wissen,
wie lange eine Ausführung
im zulässigen Worst Case
höchstens dauern kann.

Was keine bekannte obere Grenze hat,
kann keine garantierte
Realtime-Latenz besitzen.
```