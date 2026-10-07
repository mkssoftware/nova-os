# NPSPEC-TIME-CLOCKDOMAIN-0001 – Nova Clock Domain

## Status

Angenommen

## Kategorie

Time / Clock Domain

## Zweck

NovaOS definiert Clock Domains als logisch getrennte Zeiträume mit eigener Zeitbasis, Semantik und Fortschrittsregel.

Dadurch können Systemzeit, monotone Zeit, Boot-Zeit, Prozesszeit, virtuelle Zeit und deterministische Zeit unabhängig voneinander verwendet werden.

## Grundprinzipien

```text
Clock Domain ≠ Clock Source
Clock Domain ≠ Timer
Clock Domain ≠ Time Zone
Clock Domain ≠ RTC
Same Clock Source ≠ Same Clock Domain
Same Instant ≠ Same Domain Value
```

## Modell

```text
ClockDomain
├── ClockDomainID
├── Type
├── ClockSourceID
├── Epoch
├── Rate
├── Offset
├── SuspendBehavior
├── AdjustmentPolicy
└── State
```

Eine Clock Source darf mehrere Clock Domains versorgen.

## Standard-Domänen

NovaOS unterstützt mindestens:

```text
Monotonic
Boot
Wall Clock
Process
Task
Virtual
Deterministic
```

Weitere Domänen dürfen registriert werden.

## Monotonic Domain

```text
Clock Source
     ↓
Monotonic Domain
```

Sie läuft monoton vorwärts und dient insbesondere:

```text
Timeouts
Deadlines
Durations
Scheduling
```

## Boot Domain

Die Boot Domain repräsentiert die seit Systemstart vergangene Zeit und darf im Gegensatz zu einer Active-Monotonic-Domain auch Suspend-Zeit einschließen.

```text
Boot Time = Active Time + Suspend Time
```

## Wall-Clock Domain

Die Wall-Clock-Domain bildet die kanonische reale Systemzeit.

Sie darf durch Synchronisation korrigiert werden und ist daher nicht für monotone Deadlines geeignet.

## Process- und Task-Domänen

Prozesse und Tasks dürfen eigene Zeitdomänen für verbrauchte Ausführungszeit besitzen:

```text
Process Time
Task Time
```

Diese Zeit schreitet nur entsprechend der definierten Ausführungssemantik fort.

## Virtuelle Domänen

Virtualisierung, Simulation und Compatibility Environments dürfen eigene Clock Domains besitzen.

Eine virtuelle Domain darf:

```text
Offset Time
Scale Time
Pause Time
Resume Time
```

sofern ihre Semantik dies explizit erlaubt.

## Deterministische Domänen

Deterministische Ausführung darf eine kontrollierte Clock Domain verwenden:

```text
Execution Step
      ↓
Controlled Time Advance
      ↓
Deterministic Clock
```

Der Zeitfortschritt muss reproduzierbar sein und darf nicht von zufälligen Host-Timing-Effekten abhängen.

## Rate und Offset

Eine Clock Domain darf aus einer zugrunde liegenden Zeitbasis abgeleitet werden:

```text
DomainTime =
(SourceTime × Rate)
+ Offset
```

Für normale Systemdomänen gilt typischerweise:

```text
Rate = 1
```

Abweichende Raten sind nur für ausdrücklich dafür vorgesehene Domains zulässig.

## Suspend-Verhalten

Jede Clock Domain definiert explizit:

```text
Continue
Pause
Reconstruct
Provider Defined
```

Damit bleibt eindeutig, ob während Suspend vergangene Zeit Bestandteil der Domain ist.

## Domain-Konvertierung

Konvertierungen zwischen Clock Domains müssen explizit erfolgen:

```text
Domain A
   ↓
Conversion Context
   ↓
Domain B
```

Werte verschiedener Domains dürfen nicht ohne definierte Transformation miteinander verglichen werden.

## Isolation

Virtuelle oder anwendungsspezifische Clock Domains dürfen die globale Systemzeit nicht verändern.

```text
Virtual Clock Adjustment
        ≠
System Clock Adjustment
```

Eine Clock Domain erzeugt keine zusätzliche Authority.

## Fehlerverhalten

Wird die zugrunde liegende Clock Source ungültig:

```text
Source Failure
     ↓
Domain Preservation
     ↓
Alternative Source
     ↓
Recalibration
```

Soweit möglich muss die Semantik der Clock Domain erhalten bleiben.

## Normative Anforderungen

1. NovaOS MUSS mehrere unabhängige Clock Domains unterstützen.
2. Clock Domain und Clock Source MÜSSEN getrennte Konzepte bleiben.
3. Eine Clock Source DARF mehrere Clock Domains versorgen.
4. Werte unterschiedlicher Domains DÜRFEN nicht implizit miteinander verglichen werden.
5. Jede Clock Domain MUSS ihr Suspend-Verhalten definieren.
6. Monotone Domains DÜRFEN nicht rückwärts laufen.
7. Wall-Clock-Domains DÜRFEN kontrolliert korrigiert werden.
8. Prozess- und Task-Zeit MÜSSEN als eigene Domains darstellbar sein.
9. Virtuelle Domains MÜSSEN Offset, Pause und kontrollierte Skalierung unterstützen können.
10. Deterministische Domains MÜSSEN reproduzierbaren Zeitfortschritt ermöglichen.
11. Domain-Anpassungen DÜRFEN globale Zeitdomänen nicht ohne Authority verändern.
12. ClockDomainID, Typ, Quelle, Rate, Offset und Suspend-Verhalten MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CIVIL-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann unterschiedliche Zeitsemantiken als getrennte Clock Domains abbilden. System-, Boot-, Prozess-, virtuelle und deterministische Zeit können dadurch unabhängig voneinander fortschreiten, während gemeinsame Hardware-Zeitquellen genutzt werden können, ohne ihre jeweilige Semantik miteinander zu vermischen.