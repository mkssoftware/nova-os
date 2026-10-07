# NPSPEC-TIME-VIRTUAL-0001 – Nova Virtual Time

## Status

Angenommen

## Kategorie

Time / Virtual Time

## Zweck

NovaOS definiert virtuelle Zeit als kontrollierte Clock Domain, deren Zeitverlauf von der physischen Systemzeit entkoppelt werden kann.

Virtuelle Zeit ermöglicht Virtualisierung, Simulation, Tests, Compatibility Environments, Replay und deterministische Ausführung, ohne globale Zeitdomänen zu verändern.

## Grundprinzipien

```text
Virtual Time ≠ Wall Clock
Virtual Time ≠ Host Time
Virtual Time ≠ Clock Source
Virtual Time ≠ Time Zone
Virtual Time Adjustment ≠ System Time Adjustment
Virtual Clock ≠ Additional Authority
```

## Modell

```text
VirtualClock
├── VirtualClockID
├── ClockDomainID
├── ParentClockDomainID
├── Epoch
├── Offset
├── Rate
├── State
├── SuspendBehavior
└── Policy
```

## Architektur

```text
Parent Clock Domain
        ↓
Virtual Time Mapping
        ↓
Virtual Clock Domain
        ↓
Timers / Deadlines / Runtime
```

Eine virtuelle Clock Domain darf von einer realen oder einer anderen virtuellen Clock Domain abgeleitet werden.

## Zeitabbildung

Grundsätzlich kann gelten:

```text
VirtualTime =
((ParentTime - ParentEpoch) × Rate)
+ VirtualEpoch
+ Offset
```

Typischer Normalbetrieb:

```text
Rate = 1.0
```

## Operationen

Virtuelle Zeit darf kontrolliert:

```text
Start
Pause
Resume
Advance
Offset
Scale
Reset
```

werden.

Nicht jede Umgebung muss alle Operationen erlauben.

## Pause

Eine virtuelle Clock darf angehalten werden:

```text
Running
   ↓
Pause
   ↓
Virtual Time frozen
   ↓
Resume
```

Währenddessen kann die reale Zeit unabhängig weiterlaufen.

## Skalierung

Virtuelle Zeit darf schneller oder langsamer als ihre Parent Domain laufen:

```text
Rate < 1 → slower
Rate = 1 → normal
Rate > 1 → faster
Rate = 0 → paused
```

Negative Raten sind standardmäßig nicht zulässig.

## Kontrollierter Fortschritt

Simulationen und deterministische Ausführung dürfen Zeit explizit fortschalten:

```text
Current Virtual Time
        ↓
Advance Δt
        ↓
Process Due Events
        ↓
New Virtual Time
```

Dadurch kann ein System ohne Abhängigkeit von realer Ausführungsdauer simuliert werden.

## Timer und Deadlines

Timer innerhalb einer virtuellen Clock Domain verwenden ausschließlich deren Zeitbasis.

```text
Virtual Clock
     ↓
Virtual Deadline
     ↓
Virtual Timer
```

Eine Pause der virtuellen Zeit pausiert damit auch entsprechende Deadlines, sofern deren Semantik nichts anderes definiert.

## Hierarchie

Virtuelle Domains dürfen hierarchisch aufgebaut sein:

```text
Host Monotonic
      ↓
VM Time
      ↓
Simulation Time
      ↓
Test Time
```

Änderungen einer Parent Domain müssen gemäß der definierten Mapping-Regeln auf abhängige Domains wirken.

## Isolation

Eine virtuelle Umgebung darf ihre eigene Zeit verändern, ohne:

```text
Host Wall Clock
Host Monotonic Time
Andere Virtual Domains
RTC
```

zu verändern.

## Determinismus

Virtuelle Zeit darf als Grundlage reproduzierbarer Ausführung dienen.

```text
Same Initial State
+ Same Events
+ Same Time Advances
        ↓
Same Virtual Timeline
```

Reale Scheduling-Verzögerungen dürfen dabei nicht automatisch die virtuelle Zeit verändern.

## Sicherheit

Das Recht zur Veränderung einer virtuellen Clock Domain muss getrennt von deren Leserecht kontrolliert werden.

Eine virtuelle Clock erzeugt selbst keine Authority.

## Normative Anforderungen

1. NovaOS MUSS virtuelle Clock Domains unterstützen können.
2. Virtuelle Zeit MUSS von globaler Systemzeit isoliert sein.
3. Jede virtuelle Clock MUSS eine stabile `VirtualClockID` besitzen.
4. Parent Domain, Epoch, Offset und Rate MÜSSEN explizit modellierbar sein.
5. Virtuelle Zeit MUSS pausierbar und fortsetzbar sein können.
6. Kontrollierter manueller Zeitfortschritt MUSS unterstützt werden können.
7. Zeitskalierung MUSS unterstützt werden können.
8. Negative Zeitraten SOLLEN standardmäßig verboten sein.
9. Timer und Deadlines MÜSSEN virtuelle Clock Domains verwenden können.
10. Virtuelle Clock Domains MÜSSEN hierarchisch ableitbar sein können.
11. Änderungen virtueller Zeit DÜRFEN globale Clock Domains nicht verändern.
12. VirtualClockID, Parent Domain, Rate, Offset, Zustand und aktuelle virtuelle Zeit MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann unabhängige virtuelle Zeiträume erzeugen, pausieren, skalieren und kontrolliert fortschalten. Dadurch werden Virtualisierung, Simulation, Tests und deterministische Ausführung möglich, ohne die reale Systemzeit oder andere Clock Domains zu beeinflussen.