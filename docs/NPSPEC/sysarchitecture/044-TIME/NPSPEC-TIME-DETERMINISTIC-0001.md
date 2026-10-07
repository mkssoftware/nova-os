# NPSPEC-TIME-DETERMINISTIC-0001 – Nova Deterministic Time

## Status

Angenommen

## Kategorie

Time / Deterministic Time

## Zweck

NovaOS definiert deterministische Zeit als kontrollierte Zeitdomäne für reproduzierbare Ausführung.

Der Zeitfortschritt wird dabei nicht direkt durch reale Ausführungsdauer, Hardware-Timing oder Wall Clock bestimmt, sondern durch definierte Ereignisse und kontrollierte Fortschrittsregeln.

## Grundprinzipien

```text
Deterministic Time ≠ Wall Clock
Deterministic Time ≠ Host Time
Deterministic Time ≠ Execution Duration
Deterministic Time ≠ Scheduler Latency
Same Inputs + Same Events + Same Time Rules
→ Same Timeline
```

## Modell

```text
DeterministicClock
├── ClockDomainID
├── Epoch
├── CurrentTime
├── StepPolicy
├── EventSequence
├── State
└── Generation
```

## Architektur

```text
Initial State
     ↓
Deterministic Clock Domain
     ↓
Event / Execution Step
     ↓
Controlled Time Advance
     ↓
Due Timers / Deadlines
     ↓
Next State
```

Reale Zeit darf weiterhin parallel existieren, beeinflusst die deterministische Domain jedoch nicht automatisch.

## Zeitfortschritt

Deterministische Zeit darf fortschreiten durch:

```text
Explicit Advance
Execution Step
Defined Event
Next Deadline
Simulation Step
Replay Event
```

Beispiel:

```text
T = 100
   ↓
Advance +20
   ↓
T = 120
```

Ohne definierten Fortschritt bleibt die Zeit unverändert.

## Reproduzierbarkeit

Bei identischem:

```text
Initial State
Input
Event Order
Time Advance
Configuration
```

muss dieselbe deterministische Zeitfolge erzeugbar sein.

Reale CPU-Geschwindigkeit oder Systemlast dürfen das Ergebnis nicht verändern.

## Timer und Deadlines

Timer verwenden die deterministische Clock Domain:

```text
Deterministic Time
       ↓
Deadline Queue
       ↓
Advance
       ↓
Due Events
```

Das System darf direkt zur nächsten relevanten Deadline springen, wenn keine dazwischenliegende Ausführung erforderlich ist.

## Replay

Aufgezeichnete Zeitereignisse dürfen reproduziert werden:

```text
Event Log
   ↓
Deterministic Replay
   ↓
Original Event Order
   ↓
Reconstructed Timeline
```

Zeitwerte und Ereignisreihenfolge müssen dabei eindeutig zuordenbar sein.

## Parallelität

Nebenläufige Ereignisse benötigen eine deterministische Ordnungsregel, wenn ihre Reihenfolge das Ergebnis beeinflussen kann.

```text
Timestamp
+
Sequence / Ordering Rule
```

Reale Race-Timings dürfen nicht unkontrolliert in die deterministische Zeitfolge einfließen.

## Virtual Time

Deterministische Zeit darf auf der Virtual-Time-Infrastruktur aufbauen.

```text
Virtual Time
     ↓
Deterministic Policy
     ↓
Deterministic Time
```

Nicht jede virtuelle Clock ist automatisch deterministisch.

## Isolation

Änderungen deterministischer Zeit dürfen nicht verändern:

```text
System Wall Clock
System Monotonic Time
RTC
Andere Clock Domains
```

## Externe Ereignisse

Nichtdeterministische externe Eingaben müssen entweder:

```text
Recorded
Ordered
Normalized
Injected
Rejected
```

werden, bevor sie Teil einer reproduzierbaren Ausführung werden.

## Normative Anforderungen

1. NovaOS MUSS deterministische Clock Domains unterstützen können.
2. Deterministische Zeit MUSS von realer Ausführungsdauer unabhängig sein können.
3. Zeitfortschritt MUSS kontrolliert und reproduzierbar erfolgen.
4. Ohne definierten Fortschritt DARF deterministische Zeit nicht automatisch weiterlaufen.
5. Timer und Deadlines MÜSSEN deterministische Clock Domains verwenden können.
6. Das Springen zur nächsten relevanten Deadline MUSS möglich sein.
7. Reale Scheduling-Verzögerungen DÜRFEN die deterministische Timeline nicht automatisch verändern.
8. Nebenläufige Ereignisse MÜSSEN deterministisch ordnungsfähig sein.
9. Externe nichtdeterministische Ereignisse MÜSSEN kontrolliert eingebracht werden.
10. Deterministische Zeit MUSS Replay unterstützen können.
11. Änderungen DÜRFEN globale Clock Domains nicht beeinflussen.
12. ClockDomainID, aktuelle Zeit, Fortschrittsregel, Event-Reihenfolge und Generation MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-TIME-VIRTUAL-0001`
- `NPSPEC-TIME-NAMESPACE-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine kontrollierte deterministische Zeitbasis für Tests, Simulation, Replay und reproduzierbare Ausführung. Zeit wird durch definierte Ereignisse und Fortschrittsregeln bestimmt und bleibt unabhängig von realer CPU-Geschwindigkeit, Scheduling-Verzögerungen und globaler Systemzeit.