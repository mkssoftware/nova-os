# NPSPEC-INTERRUPT-DEFERRED-0001 – Nova Deferred Interrupt Processing

## Status

Entwurf

## Kategorie

Interrupts / Deferred Work / Latency / Kernel

## Zweck

NovaOS trennt zeitkritische Interrupt-Behandlung von aufwendiger Verarbeitung, um Interrupt-Latenzen kurz und vorhersehbar zu halten.

```text
Interrupt
   ↓
Minimal Handler
   ↓
Deferred Work
   ↓
Kernel Execution Context
```

## Grundprinzip

Im direkten Interrupt-Kontext wird nur unmittelbar notwendige Arbeit ausgeführt.

```text
Immediate:
- Hardwarestatus erfassen
- Interrupt bestätigen
- minimale Ereignisdaten sichern
- Deferred Work auslösen

Deferred:
- Daten verarbeiten
- Protokolle ausführen
- weitere Kernelarbeit
```

## Ablauf

```text
Hardware Interrupt
      ↓
Interrupt Handler
      ↓
Capture State
      ↓
Acknowledge
      ↓
Queue Deferred Work
      ↓
Return
      ↓
Deferred Execution
```

Der direkte Handler soll so schnell wie möglich beendet werden.

## Deferred Work Item

Ein Deferred Work Item beschreibt mindestens:

```text
WorkID
Source
Handler
Priority
State
Context
```

Typische Zustände:

```text
Pending
Running
Completed
Cancelled
Failed
```

## Ausführungskontext

Deferred Work wird außerhalb des direkten Interrupt-Kontexts ausgeführt.

Dadurch können – abhängig vom gewählten Kontext – Funktionen verwendet werden, die im Interrupt-Handler nicht zulässig sind.

```text
Interrupt Context
      ↓
Deferred Context
      ↓
Scheduler / Kernel Task
```

## Priorität

Deferred Work kann eine eigene Ausführungspriorität besitzen.

```text
Interrupt Priority
        ≠
Deferred Work Priority
```

Die ursprüngliche Interrupt-Priorität darf nicht automatisch unverändert auf nachgelagerte Arbeit übertragen werden.

## Coalescing

Mehrere gleichartige Ereignisse dürfen zusammengefasst werden.

```text
Event
Event
Event
  ↓
Coalescing
  ↓
Deferred Work
```

Dies reduziert unnötige Scheduling- und Verarbeitungskosten.

Ereignisse, deren Semantik eine Einzelverarbeitung verlangt, dürfen nicht verloren gehen.

## Backpressure

Entsteht Deferred Work schneller als es verarbeitet werden kann, muss NovaOS kontrolliert reagieren.

Mögliche Maßnahmen:

```text
Coalesce
Throttle
Prioritize
Drop only if semantically allowed
Signal Overload
```

Unbegrenzte Warteschlangen sind zu vermeiden.

## CPU-Affinität

Deferred Work kann auf derselben CPU wie der Interrupt oder auf einer anderen geeigneten CPU ausgeführt werden.

```text
Interrupt CPU
     ↓
Affinity / Locality
     ↓
Deferred CPU
```

NUMA-, Cache- und Realtime-Anforderungen können berücksichtigt werden.

## Fehlerbehandlung

Fehler in Deferred Work dürfen den Interrupt-Kontext nicht nachträglich inkonsistent machen.

Fehler müssen kontrolliert gemeldet und diagnostizierbar sein.

## Normative Anforderungen

1. NovaOS MUSS Deferred Interrupt Processing unterstützen.
2. Direkte Interrupt-Handler SOLLEN auf unmittelbar notwendige Arbeit begrenzt bleiben.
3. Aufwendige Verarbeitung SOLL außerhalb des Interrupt-Kontexts erfolgen.
4. Deferred Work MUSS eindeutig identifizierbar und verwaltbar sein.
5. Interrupt- und Deferred-Work-Priorität MÜSSEN getrennt behandelt werden.
6. Gleichartige Ereignisse DÜRFEN zusammengefasst werden, wenn ihre Semantik dies erlaubt.
7. Ereignisse DÜRFEN durch Coalescing nicht semantisch verloren gehen.
8. Deferred-Work-Warteschlangen MÜSSEN gegen unkontrolliertes Wachstum geschützt werden.
9. Backpressure MUSS kontrolliert behandelt werden.
10. CPU-Affinität und Lokalität SOLLEN berücksichtigt werden können.
11. Realtime-Anforderungen DÜRFEN durch Deferred Processing nicht unkontrolliert verletzt werden.
12. Fehler in Deferred Work MÜSSEN diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-INTERRUPT-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `NPSPEC-INTERRUPT-AFFINITY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-INTERRUPT-0004`

## Ergebnis

```text
Interrupt
   ↓
Minimal Immediate Work
   ↓
Fast Return
   ↓
Controlled Deferred Processing
```

NovaOS hält damit direkte Interrupt-Pfade kurz und verschiebt aufwendige Verarbeitung in kontrollierte Kernel-Ausführungskontexte.