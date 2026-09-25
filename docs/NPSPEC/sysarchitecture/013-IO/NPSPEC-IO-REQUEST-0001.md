# NPSPEC-IO-REQUEST-0001 – Nova I/O Request Model

## Status

Angenommen

## Kategorie

I/O / Request / Asynchronous I/O

## Zweck

NovaOS definiert mit `IORequest` die einheitliche Beschreibung einer I/O-Operation.

```text
Consumer
   ↓
IORequest
   ↓
I/O Provider
   ↓
Completion
```

Ein Request beschreibt, **was** ausgeführt werden soll, ohne den aufrufenden Task an die konkrete Implementierung des Providers zu binden.

## Grundprinzip

```text
Request ≠ Execution
Request ≠ Completion
Request ≠ Device Command
```

Ein `IORequest` ist die logische Repräsentation einer I/O-Operation. Treiber und Provider dürfen daraus interne Hardware- oder Protokolloperationen erzeugen.

## Request-Struktur

Ein Request enthält mindestens:

```text
IORequest
├── RequestID
├── Operation
├── Target
├── Buffer / Data
├── State
└── Completion Target
```

Optional:

```text
Offset
Length
Priority
Deadline
Cancellation
ExecutionContract
Security Context
Provider Hints
```

## Operation

Die konkrete Operation hängt vom Zieltyp ab.

Typische Operationen sind:

```text
Read
Write
Flush
Control
Query
Open
Close
```

Subsysteme dürfen zusätzliche Operationen definieren.

Generische I/O-Schichten dürfen providerspezifische Operationen nicht unnötig interpretieren.

## Target

Das Ziel muss eindeutig referenziert werden können.

Beispiele:

```text
Device
Storage Object
Stream
Socket
Pipe
Driver Endpoint
```

Die Referenz muss die NovaOS-Security- und Capability-Regeln einhalten.

Die Kenntnis eines Targets erzeugt keine Zugriffsberechtigung.

## Buffer

Ein Request darf einen oder mehrere Buffer referenzieren.

```text
IORequest
   ↓
Buffer
├── Linear Buffer
├── Shared Buffer
└── Scatter/Gather List
```

Buffer-Eigentum, Lebensdauer und Zugriffsmodus müssen eindeutig bestimmbar sein.

Zero-Copy darf verwendet werden, wenn Provider, Security Policy und Speicherarchitektur dies erlauben.

## Request Lifecycle

```text
Created
   ↓
Submitted
   ↓
Pending
   ↓
Running
   ↓
Completed
```

Alternative Endzustände:

```text
Failed
Cancelled
Timeout
```

Ein Request darf nach Erreichen eines finalen Zustands nicht erneut ausgeführt werden.

## RequestID

Jeder aktive Request muss eindeutig identifizierbar sein.

```text
RequestID
   ↓
Request
   ↕
Completion
```

Eine Wiederverwendung von IDs darf erst erfolgen, wenn keine Verwechslung mit früheren Requests oder Completions mehr möglich ist.

## Submission

Beim Einreichen muss der Request ausreichend validiert werden.

Geprüft werden können:

```text
Target
Operation
Buffer
Bounds
Permissions
Capabilities
Resource Limits
Execution Contract
```

Ein ungültiger Request darf nicht an einen Provider weitergereicht werden.

## Cancellation

Ein Request darf ein Cancellation-Modell besitzen.

```text
Request
   ↓
Cancel
   ↓
Cancelled / Too Late / Failed
```

Cancellation ist eine Anforderung und garantiert nicht, dass eine bereits begonnene Operation rückgängig gemacht werden kann.

## Deadline und Priorität

Requests dürfen zeitliche Anforderungen enthalten.

```text
Priority
Deadline
Latency Requirement
```

Diese Angaben müssen mit Scheduler-, Realtime- und Execution-Contract-Regeln abgestimmt werden.

Eine Priorität darf Sicherheits- oder Ressourcenregeln nicht umgehen.

## Provider Mapping

Ein Provider darf einen Request in mehrere interne Operationen zerlegen.

```text
IORequest
   ↓
Provider
   ├── Operation A
   ├── Operation B
   └── Operation C
```

Für den Consumer bleibt der ursprüngliche `IORequest` die logische Einheit.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Operation
Target
State
Owner
Priority
Deadline
Transferred Bytes
Provider
Error
```

Sicherheitsrelevante Informationen dürfen nur autorisiert sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS I/O-Operationen durch ein einheitliches `IORequest`-Modell beschreiben können.
2. Jeder aktive Request MUSS eindeutig identifizierbar sein.
3. Request, Ausführung und Completion MÜSSEN getrennte Konzepte bleiben.
4. Ein Request MUSS Ziel und Operation eindeutig beschreiben.
5. Buffer-Lebensdauer und Zugriffsmodus MÜSSEN bestimmbar sein.
6. Requests MÜSSEN vor der Weitergabe an einen Provider validierbar sein.
7. I/O-Requests DÜRFEN Security- oder Capability-Prüfungen NICHT umgehen.
8. Cancellation MUSS einen eindeutig bestimmbaren Ausgang besitzen.
9. Requests SOLLEN Priorität und Deadline transportieren können.
10. Provider DÜRFEN einen Request intern in mehrere Operationen zerlegen.
11. Ein final abgeschlossener Request DARF NICHT erneut ausgeführt werden.
12. Zustand und Ergebnis eines Requests SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-IO-0003`

## Ergebnis

```text
Operation + Target + Buffer + Constraints
                 ↓
             IORequest
                 ↓
              Provider
                 ↓
             Completion
```

NovaOS erhält damit eine einheitliche Request-Abstraktion, über die unterschiedliche I/O-Provider angesprochen werden können, ohne Consumer an konkrete Geräte- oder Treiberimplementierungen zu koppeln.