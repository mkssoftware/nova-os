# NPSPEC-DISTCOMM-DEADLINE-0001 – Nova Distributed Deadline

## Status

Angenommen

## Kategorie

Distributed Communication / Time / Deadline

## Zweck

NovaOS definiert ein gemeinsames Deadline-Modell für verteilte Operationen über Prozess-, System- und Netzwerkgrenzen hinweg.

```text
Caller
  ↓
Deadline
  ↓
RPC / Messaging / Queue
  ↓
Network
  ↓
Remote Execution
```

Eine Deadline beschreibt den spätesten sinnvollen Abschlusszeitpunkt einer Operation und soll über die gesamte Kommunikations- und Ausführungskette erhalten bleiben.

## Grundprinzip

```text
Deadline ≠ Timeout
Deadline ≠ Priority
Deadline ≠ Guaranteed Completion
Timeout Expired ≠ Remote Operation Stopped
Remaining Time ≠ Original Deadline
```

Eine Deadline beschreibt ein Ende-zu-Ende-Zeitlimit.

Ein Timeout beschreibt dagegen typischerweise die maximale Dauer eines einzelnen Schrittes.

## Deadline-Modell

Eine verteilte Operation darf mindestens besitzen:

```text
DeadlineContext
├── OperationID
├── Deadline
├── Remaining Budget
├── State
└── Clock Context
```

Optional:

```text
ExecutionContract
Network Intent
QoS
Security Context
Resource Budget
Cancellation
Retry Policy
```

## Ende-zu-Ende-Semantik

Die ursprüngliche Deadline bleibt über die gesamte Operation erhalten.

```text
Total Deadline
     ↓
Resolve
     ↓
Connect
     ↓
Transmit
     ↓
Remote Queue
     ↓
Execute
     ↓
Respond
```

Jeder Schritt verbraucht einen Teil des verbleibenden Zeitbudgets.

Eine neue Kommunikationsschicht darf die ursprüngliche Deadline nicht durch einen vollständig neuen Zeitrahmen ersetzen.

## Deadline-Propagation

Deadlines müssen über Kommunikationsgrenzen propagiert werden können.

```text
Caller
  ↓ Deadline
Service A
  ↓ Remaining Deadline
Service B
  ↓ Remaining Deadline
Service C
```

Ein nachgelagerter Dienst darf keine längere effektive Deadline erhalten als die verbleibende Deadline seines Aufrufers, sofern keine explizite unabhängige Operation erzeugt wird.

## Absolute und relative Zeit

Intern sollen Deadlines möglichst als stabiler Zeitbezug verarbeitet werden.

```text
Absolute Deadline
       ↓
Current Time
       ↓
Remaining Time
```

Über Systemgrenzen hinweg müssen Clock-Unterschiede berücksichtigt werden.

Eine entfernte Gegenstelle darf nicht blind davon ausgehen, dass ihre lokale Uhr exakt mit der Uhr des Senders synchronisiert ist.

## Deadline-Zustände

NovaOS soll mindestens unterscheiden können:

```text
Active
Expiring
Exceeded
Completed
Cancelled
```

Eine bereits überschrittene Deadline soll neue vermeidbare Arbeit verhindern können.

## Early Rejection

Eine Operation darf frühzeitig abgelehnt werden, wenn ihre Deadline offensichtlich nicht mehr erreichbar ist.

```text
Remaining Time
      <
Estimated Required Time
      ↓
DeadlineUnreachable
```

Dadurch können Ressourcen für noch sinnvoll ausführbare Operationen freigehalten werden.

Eine Schätzung darf jedoch nicht als harte Garantie behandelt werden.

## RPC

Distributed RPC muss Deadlines übertragen können.

```text
RPC Request
├── OperationID
└── Deadline
```

Die entfernte Seite muss die verbleibende Deadline bei Queueing, Ausführung und Antwort berücksichtigen können.

Eine abgelaufene RPC-Deadline bedeutet nicht automatisch, dass eine bereits gestartete Remote-Operation rückgängig gemacht wurde.

## Messaging

Asynchrone Nachrichten dürfen Deadlines oder Ablaufzeiten besitzen.

```text
Queued
  ↓
Deadline Exceeded
  ↓
Expire / Reject / Policy Action
```

Eine Nachricht, deren Ergebnis nach Ablauf keinen Nutzen mehr besitzt, soll nicht unnötig verarbeitet werden.

## Queues

Queue-Einträge dürfen eine Deadline besitzen.

Scheduling darf berücksichtigen:

```text
Priority
Deadline
Queue Time
Resource Availability
```

Eine Queue darf abgelaufene Einträge kontrolliert entfernen oder als `Expired` markieren.

## Retry

Retries müssen die ursprüngliche Deadline respektieren.

```text
Failure
   ↓
Remaining Time
   ↓
Retry useful?
```

Ein Retry darf keine neue vollständige Deadline erzeugen.

Wenn die verbleibende Zeit für einen sinnvollen Retry nicht ausreicht, soll der Retry unterbleiben.

## Backpressure

Deadline und Backpressure müssen zusammenarbeiten.

```text
Queue Pressure
      +
Remaining Deadline
      ↓
Wait / Reject / Execute
```

Bei hoher Last dürfen Operationen frühzeitig abgelehnt werden, wenn weiteres Warten nur zu einem späteren Deadline-Fehler führen würde.

## QoS

Deadlines dürfen als Eingangssignal für Scheduling und QoS dienen.

```text
Deadline
   ↓
Scheduler / QoS
   ↓
Execution Priority
```

Eine kurze Deadline erzeugt jedoch nicht automatisch unbegrenzte Priorität.

```text
Deadline ≠ Unlimited Priority
```

Fairness, Sicherheit und Resource Budgets bleiben gültig.

## Cancellation

Deadline-Überschreitung darf Cancellation auslösen.

```text
Deadline Exceeded
       ↓
Cancellation Request
       ↓
Propagation
```

Cancellation bleibt kooperativ.

Bereits irreversible externe Effekte können dadurch nicht automatisch rückgängig gemacht werden.

## Network Intent

Network Intent darf Deadline-Anforderungen berücksichtigen.

```text
Deadline
   ↓
Network Intent
   ↓
Path Selection
```

Routing und Multipath dürfen einen geeigneteren Pfad auswählen, sofern Security-, Trust- und Sovereignty-Anforderungen weiterhin erfüllt bleiben.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Pending Deadline Operations
Timers
Deadline Queues
Remote Calls
Retries
Deadline Metadata
```

Abgelaufene Operationen sollen Ressourcen möglichst früh freigeben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
OperationID
Original Deadline
Remaining Time
State
Queue Time
Execution Time
Network Time
Deadline Miss
Failure Reason
```

Damit soll nachvollziehbar sein, wo das Zeitbudget einer verteilten Operation verbraucht wurde.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Distributed-Deadline-Modell bereitstellen.
2. Deadlines MÜSSEN über Kommunikationsgrenzen propagierbar sein.
3. Eine Deadline MUSS von lokalen Timeouts unterschieden werden.
4. Nachgelagerte Operationen DÜRFEN die ursprüngliche Deadline NICHT implizit verlängern.
5. Überschrittene Deadlines MÜSSEN explizit erkennbar sein.
6. Offensichtlich nicht mehr erfüllbare Operationen SOLLEN frühzeitig ablehnbar sein.
7. RPC, Messaging und Queues SOLLEN Deadlines transportieren können.
8. Retries MÜSSEN die verbleibende ursprüngliche Deadline berücksichtigen.
9. Deadline-Überschreitung DARF Cancellation auslösen.
10. Deadlines DÜRFEN Security-, Sovereignty- oder Resource-Regeln NICHT umgehen.
11. Deadline-Verarbeitung MUSS ressourcenbegrenzt sein.
12. Deadline-Zustände und Zeitverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0014`

## Ergebnis

```text
Original Deadline
        ↓
Distributed Operation
        ↓
Remaining Time Budget
        ↓
RPC / Queue / Network / Execution
        ↓
Complete Before Deadline
        │
        └── or → Expire / Cancel / Reject
```

NovaOS erhält damit ein durchgängiges Deadline-Modell, bei dem das Zeitbudget einer Operation über System- und Netzwerkgrenzen erhalten bleibt und von RPC, Messaging, Queues, Retries, Backpressure, Scheduling und Netzwerkpfadwahl gemeinsam berücksichtigt werden kann.