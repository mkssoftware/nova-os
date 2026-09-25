# NPSPEC-INTERRUPT-0001 – Nova Interrupt Architecture

## Status

Angenommen

## Kategorie

Interrupts / Kernel / HAL / Event Handling

## Zweck

NovaOS definiert eine einheitliche Interrupt-Architektur für Hardware-Interrupts, CPU-Ausnahmen und Inter-Processor Interrupts.

```text
Interrupt Source
      ↓
HAL / CPU
      ↓
Interrupt Core
      ↓
Handler
      ↓
Deferred Work
```

## Grundprinzip

Interrupt-Behandlung wird in einen möglichst kurzen unmittelbaren Teil und nachgelagerte Verarbeitung getrennt.

```text
Interrupt
   ↓
Acknowledge
   ↓
Minimal Handler
   ↓
Deferred Work
```

Zeitaufwendige Verarbeitung soll nicht im direkten Interrupt-Kontext stattfinden.

## Interrupt-Klassen

NovaOS unterscheidet mindestens:

```text
CPU Exceptions
Hardware Interrupts
Inter-Processor Interrupts
Software-generated Interrupts
```

Die konkrete Hardwaredarstellung wird durch die HAL abstrahiert.

## Interrupt Descriptor

Ein Interrupt besitzt mindestens:

```text
InterruptID
Type
Vector
Source
Target
Priority
Handler
State
```

## Dispatch

Der Interrupt Core ordnet eingehende Ereignisse dem zuständigen Handler zu.

```text
Vector
  ↓
Interrupt Core
  ↓
Registered Handler
```

Nicht registrierte oder unerwartete Interrupts müssen kontrolliert behandelt werden.

## Handler

Ein Handler soll nur unmittelbar notwendige Arbeit durchführen:

```text
Read Status
Acknowledge Hardware
Capture Event Data
Schedule Deferred Work
Return
```

Blockierende Operationen sind im direkten Interrupt-Kontext zu vermeiden.

## Deferred Work

Aufwendige Verarbeitung wird in einen geeigneten Kernel-Ausführungskontext verschoben.

```text
Interrupt Handler
      ↓
Deferred Work
      ↓
Scheduler
      ↓
Kernel Task
```

Damit bleibt die Interrupt-Latenz begrenzt.

## Priorität und Verschachtelung

NovaOS muss Interrupt-Prioritäten und plattformspezifische Verschachtelung kontrolliert unterstützen können.

Priorität ist dabei ein Mechanismus der Interrupt-Verarbeitung und darf nicht automatisch mit Scheduling-Priorität gleichgesetzt werden.

## Multiprocessing

Interrupts können gezielt CPUs zugeordnet werden.

```text
Interrupt
   ↓
Routing
   ↓
CPU N
```

Inter-Processor Interrupts ermöglichen definierte Kommunikation zwischen CPUs.

## Fehlerbehandlung

Fehlerhafte Interrupt-Zustände müssen diagnostizierbar sein.

Beispiele:

```text
Unknown Vector
Spurious Interrupt
Handler Failure
Interrupt Storm
Invalid Routing
```

Interrupt Storms sollen erkannt und begrenzt werden können.

## Normative Anforderungen

1. NovaOS MUSS eine zentrale Interrupt-Architektur bereitstellen.
2. CPU-Ausnahmen, Hardware-Interrupts und IPIs MÜSSEN unterscheidbar sein.
3. Interrupts MÜSSEN eindeutig identifizierbar und dispatchbar sein.
4. Direkte Interrupt-Handler SOLLEN möglichst kurz bleiben.
5. Aufwendige Verarbeitung SOLL in Deferred Work verschoben werden.
6. Handler DÜRFEN den Interrupt-Kontext nicht unkontrolliert blockieren.
7. Interrupt-Routing MUSS Multiprozessorsysteme unterstützen.
8. Interrupt-Prioritäten MÜSSEN kontrolliert behandelt werden.
9. Unerwartete und spurious Interrupts MÜSSEN sicher behandelt werden.
10. Interrupt Storms SOLLEN erkannt und begrenzt werden können.
11. Interrupt-Ereignisse SOLLEN für Diagnose und Observability nachvollziehbar sein.
12. Hardwareabhängige Interrupt-Mechanismen MÜSSEN hinter der HAL gekapselt bleiben.

## Abhängigkeiten

- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-INTERRUPT-0001`
- `ADR-INTERRUPT-0002`
- `ADR-INTERRUPT-0003`
- `ADR-INTERRUPT-0004`
- `ADR-INTERRUPT-0005`

## Ergebnis

```text
Hardware / CPU Event
        ↓
HAL Interrupt Layer
        ↓
Nova Interrupt Core
        ↓
Minimal Handler
        ↓
Deferred Processing
```

NovaOS erhält damit eine einheitliche und skalierbare Interrupt-Verarbeitung mit kurzen Handlerpfaden und klarer Trennung zwischen unmittelbarer Ereignisbehandlung und nachgelagerter Arbeit.