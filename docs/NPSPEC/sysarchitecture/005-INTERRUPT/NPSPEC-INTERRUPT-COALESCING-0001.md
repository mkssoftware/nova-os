# NPSPEC-INTERRUPT-COALESCING-0001 – Nova Interrupt Coalescing

## Status

Entwurf

## Kategorie

Interrupts / Coalescing / Performance / Latency

## Zweck

NovaOS kann mehrere gleichartige Hardwareereignisse zu weniger Interrupts zusammenfassen, um Interrupt-Last und CPU-Overhead zu reduzieren.

```text
Event
Event
Event
Event
  ↓
Coalescing
  ↓
Interrupt
```

## Grundprinzip

Interrupt Coalescing ist eine Optimierung zwischen Latenz und Verarbeitungseffizienz.

```text
Low Coalescing
    ↓
Low Latency
High Interrupt Rate

High Coalescing
    ↓
Higher Latency
Low Interrupt Rate
```

Die gewählte Strategie muss zu den Anforderungen der jeweiligen Interrupt-Quelle passen.

## Coalescing-Modell

Eine Interrupt-Quelle kann mindestens folgende Parameter besitzen:

```text
Enabled
TimeThreshold
EventThreshold
LatencyLimit
Policy
```

Hardware- und softwarebasiertes Coalescing dürfen unterstützt werden.

## Zeitbasiertes Coalescing

Ereignisse werden für ein begrenztes Zeitfenster gesammelt.

```text
First Event
    ↓
Start Timer
    ↓
Collect Events
    ↓
Time Threshold
    ↓
Interrupt
```

Das Zeitfenster darf definierte Latenzgrenzen nicht überschreiten.

## Ereignisbasiertes Coalescing

Ein Interrupt wird nach einer bestimmten Anzahl von Ereignissen ausgelöst.

```text
Events
  ↓
Counter
  ↓
Threshold
  ↓
Interrupt
```

Zeit- und Ereignisschwellen dürfen kombiniert werden.

## Adaptive Steuerung

NovaOS darf Coalescing dynamisch anpassen.

Berücksichtigt werden können:

```text
Interrupt Rate
CPU Load
Device Load
Latency Requirements
Realtime Requirements
Power State
```

Adaptive Optimierung darf harte Latenz- oder Realtime-Anforderungen nicht verletzen.

## Realtime

Für zeitkritische Interrupts kann Coalescing eingeschränkt oder deaktiviert werden.

```text
Hard Latency Requirement
          ↓
Coalescing Limit
          ↓
Bounded Delay
```

Coalescing darf keine zugesicherte Deadline ungültig machen.

## Deferred Processing

Interrupt Coalescing und Deferred Processing ergänzen sich:

```text
Multiple Events
      ↓
Coalescing
      ↓
Interrupt
      ↓
Minimal Handler
      ↓
Deferred Work
```

Coalescing reduziert die Anzahl der Interrupt-Aufrufe, während Deferred Processing die Arbeit im Interrupt-Kontext reduziert.

## Ereigniserhalt

Coalescing darf die Semantik der zugrunde liegenden Ereignisse nicht zerstören.

Falls jedes Ereignis einzeln relevant ist, müssen die einzelnen Ereignisdaten trotz zusammengefasstem Interrupt erhalten bleiben.

```text
Coalesced Interrupt ≠ Lost Events
```

## Überlastung

Bei hoher Ereignisrate kann NovaOS Coalescing verstärken, sofern dies zulässig ist.

Reicht dies nicht aus, müssen zusätzliche Mechanismen wie Backpressure, Throttling oder kontrollierte Degradation verwendet werden.

## Normative Anforderungen

1. NovaOS SOLL Interrupt Coalescing unterstützen.
2. Coalescing MUSS pro Interrupt-Quelle steuerbar sein.
3. Zeit- und Ereignisschwellen SOLLEN unterstützt werden können.
4. Coalescing DARF semantisch relevante Ereignisse nicht verlieren.
5. Definierte Latenzgrenzen MÜSSEN eingehalten werden.
6. Harte Realtime-Anforderungen MÜSSEN Vorrang vor Coalescing-Optimierungen haben.
7. Coalescing DARF dynamisch angepasst werden.
8. Adaptive Anpassungen DÜRFEN harte Constraints nicht überschreiben.
9. Hardware-Coalescing SOLL verwendet werden können, wenn die Hardware es unterstützt.
10. Coalescing-Entscheidungen und aktuelle Parameter SOLLEN diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-INTERRUPT-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `NPSPEC-INTERRUPT-DEFERRED-0001`
- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-INTERRUPT-0005`
- `ADR-REALTIME-0003_Bounded_Latency`

## Ergebnis

```text
High Event Rate
      ↓
Controlled Coalescing
      ↓
Fewer Interrupts
      ↓
Lower CPU Overhead
      ↓
Preserved Latency Constraints
```

NovaOS kann damit hohe Ereignisraten effizient verarbeiten, ohne definierte Latenz- und Realtime-Anforderungen unkontrolliert zu verletzen.