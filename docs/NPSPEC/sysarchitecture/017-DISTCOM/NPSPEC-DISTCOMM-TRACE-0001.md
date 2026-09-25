# NPSPEC-DISTCOMM-TRACE-0001 – Nova Distributed Tracing

## Status

Angenommen

## Kategorie

Distributed Communication / Observability / Distributed Tracing

## Zweck

NovaOS definiert ein gemeinsames Distributed-Tracing-Modell zur Nachverfolgung logischer Operationen über Prozess-, Dienst-, System- und Netzwerkgrenzen hinweg.

```text
Operation
   ↓
Service A
   ↓
RPC
   ↓
Service B
   ↓
Queue
   ↓
Service C
```

Alle beteiligten Schritte können zu einem gemeinsamen Trace zusammengeführt werden.

## Grundprinzip

```text
Trace ≠ Log
TraceID ≠ Permission
Span ≠ Process
Correlation ≠ Causality Proof
Observability ≠ Authority
```

Tracing dient der Beobachtung und Diagnose verteilter Abläufe und darf keine zusätzlichen Berechtigungen erzeugen.

## Trace-Modell

Ein Trace besitzt mindestens:

```text
DistributedTrace
├── TraceID
├── RootOperation
├── Start Time
├── State
└── Spans
```

Optional:

```text
Security Context
ExecutionContract Reference
Network Intent Reference
Resource Context
Sampling Policy
Baggage
```

Die `TraceID` bleibt über alle beteiligten Kommunikations- und Ausführungsschritte stabil.

## Span-Modell

Ein einzelner Verarbeitungsschritt wird als Span dargestellt.

```text
TraceSpan
├── SpanID
├── ParentSpanID
├── Operation
├── Start Time
├── Duration
├── State
└── Component
```

Optional:

```text
ServiceID
ProcessID
FlowID
ConnectionID
QueueID
MessageID
Network Path
Retry Attempt
Deadline
Error
```

`SpanID` identifiziert einen einzelnen Abschnitt innerhalb eines Trace.

## Hierarchie

Spans bilden eine kausale Verarbeitungshierarchie.

```text
Trace
└── Span A
    ├── Span B
    │   └── Span C
    └── Span D
```

Asynchrone oder parallele Verarbeitung darf mehrere abhängige Spans erzeugen.

## Trace Context

Trace Context muss über Kommunikationsgrenzen propagierbar sein.

```text
Caller
  ↓ TraceID + Span Context
RPC
  ↓
Remote Service
```

Dies gilt ebenfalls für:

```text
Message Bus
Pub/Sub
Queues
Streaming
Remote Capabilities
```

Die empfangende Seite erzeugt daraus einen neuen lokalen Span, ohne die logische Trace-Identität zu verlieren.

## RPC

Distributed RPC soll Trace Context automatisch propagieren können.

```text
Client Span
    ↓
RPC Transport
    ↓
Server Span
```

Mindestens folgende Phasen dürfen unterscheidbar sein:

```text
Resolve
Connect
Send
Remote Queue
Remote Execute
Receive
```

## Messaging

Bei asynchroner Kommunikation muss zwischen Senden und Verarbeitung unterschieden werden.

```text
Producer Span
     ↓
Message
     ↓
Queue Time
     ↓
Consumer Span
```

`MessageID` und `TraceID` bleiben getrennte Identitäten.

```text
MessageID ≠ TraceID
```

Eine Nachricht darf Teil eines größeren Trace sein.

## Retry

Retries müssen innerhalb desselben logischen Trace nachvollziehbar bleiben.

```text
Operation
   ↓
Attempt 1 → Failed
   ↓
Attempt 2 → Failed
   ↓
Attempt 3 → Success
```

Jeder Versuch darf einen eigenen Span besitzen.

Die logische Operation behält ihre Trace-Zuordnung.

## Deadline

Trace-Daten dürfen Deadline-Informationen enthalten.

```text
Original Deadline
Remaining Time
Deadline Miss
```

Dadurch kann analysiert werden, an welcher Stelle das Zeitbudget verbraucht wurde.

## Netzwerkpfad

Tracing darf relevante Netzwerkbeziehungen referenzieren.

```text
TraceSpan
├── FlowID
├── PathID
├── InterfaceID
└── MigrationID
```

Damit können beispielsweise Pfadwechsel, VPN-Nutzung oder Netzwerkprobleme einer Operation zugeordnet werden.

## Fehler

Fehler müssen einem Span zugeordnet werden können.

```text
Span
├── State = Failed
└── Error
    ├── Category
    ├── Code
    └── Cause
```

Fehlerketten sollen über Systemgrenzen nachvollziehbar bleiben, ohne interne vertrauliche Informationen unnötig offenzulegen.

## Sampling

Nicht jeder Trace muss vollständig gespeichert werden.

NovaOS darf Sampling verwenden:

```text
Always
Probabilistic
Adaptive
Error-Based
Policy-Based
```

Sicherheitskritische oder diagnostisch relevante Ereignisse dürfen unabhängig von normalem Sampling erfasst werden, sofern Policy und Datenschutz dies erlauben.

## Baggage

Trace Context darf begrenzte zusätzliche Metadaten transportieren.

```text
Trace Context
├── TraceID
├── SpanID
└── Baggage
```

Baggage muss streng größenbegrenzt sein.

Sensible Daten, Credentials oder Capabilities dürfen nicht als normales Trace-Baggage transportiert werden.

## Sicherheit und Datenschutz

Tracing darf keine Sicherheitsgrenzen umgehen.

Insbesondere dürfen folgende Daten nicht ungefiltert aufgezeichnet werden:

```text
Passwords
Private Keys
Capability Secrets
Authentication Tokens
Sensitive Payloads
Personal Data
```

Trace-Sichtbarkeit muss anhand von Security Context, Namespace und Berechtigungen eingeschränkt werden können.

## Performance

Tracing darf den normalen Kommunikationspfad nicht unnötig dominieren.

Implementierungen sollen unterstützen:

```text
Buffered Recording
Batch Export
Sampling
Bounded Metadata
Asynchronous Processing
```

Tracing muss kontrolliert deaktiviert oder reduziert werden können.

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
Active Traces
Span Count
Trace Depth
Baggage Size
Trace Buffers
Retention
Export Bandwidth
Persistent Storage
```

Tracing darf bei Überlastung nicht selbst zur Ursache eines Systemausfalls werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
TraceID
SpanID
ParentSpanID
Operation
ServiceID
Duration
State
Retry Attempt
Deadline State
Network Path
Error
```

Zusätzlich sollen Beziehungen zu anderen NovaOS-Objekten über stabile IDs nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS Distributed Tracing unterstützen können.
2. Eine `TraceID` MUSS über Prozess- und Netzwerkgrenzen propagierbar sein.
3. Einzelne Verarbeitungsschritte MÜSSEN als Spans darstellbar sein.
4. Parent-/Child-Beziehungen zwischen Spans SOLLEN erhalten bleiben.
5. RPC, Messaging, Pub/Sub und Queues SOLLEN Trace Context propagieren können.
6. Retries SOLLEN als getrennte Versuche innerhalb desselben logischen Trace sichtbar sein.
7. `TraceID` und `MessageID` MÜSSEN getrennte Identitäten bleiben.
8. Trace Context DARF NICHT als Berechtigung verwendet werden.
9. Credentials, Capability Secrets und Schlüssel DÜRFEN NICHT ungefiltert aufgezeichnet werden.
10. Sampling und Retention MÜSSEN policygesteuert sein können.
11. Trace-Ressourcen MÜSSEN begrenzt sein.
12. Trace-Daten SOLLEN mit stabilen NovaOS-Objektidentitäten korrelierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `NPSPEC-NETWORK-INTROSPECTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0015`

## Ergebnis

```text
Logical Operation
       ↓
TraceID
       ↓
Span A
 ├── RPC → Span B
 ├── Queue → Span C
 └── Retry → Span D
       ↓
Distributed Trace
       ↓
Diagnosis / Introspection
```

NovaOS erhält damit ein durchgängiges Distributed-Tracing-Modell, das verteilte Operationen über Dienste, Nachrichten, Netzwerkpfade und Retries hinweg nachvollziehbar macht, ohne Beobachtbarkeit mit Autorität oder Zugriffskontrolle zu vermischen.