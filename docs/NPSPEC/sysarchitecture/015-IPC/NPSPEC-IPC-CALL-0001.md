# NPSPEC-IPC-CALL-0001 – Nova IPC Call Model

## Status

Angenommen

## Kategorie

IPC / Call / Request-Response

## Zweck

NovaOS definiert ein einheitliches Call-Modell für gerichtete IPC-Aufrufe zwischen Prozessen, Diensten und Systemkomponenten.

```text
Caller
  ↓ Call
Callee
  ↓ Result
Caller
```

Das Call-Modell bildet die grundlegende Aufrufsemantik unterhalb höherer Mechanismen wie RPC und typisierten Service-Interfaces.

## Grundprinzip

```text
Call ≠ Local Function Call
Submitted ≠ Executed
Executed ≠ Successful
Cancelled ≠ Not Executed
```

Jeder Call besitzt eine eindeutige Identität und einen nachvollziehbaren Lifecycle.

## Call-Modell

Ein IPC-Call enthält mindestens:

```text
IPCCall
├── CallID
├── Target
├── Operation
├── Request
├── Completion Target
└── State
```

Optional:

```text
Deadline
Priority
Cancellation
ExecutionContract
Security Context
```

## Lifecycle

```text
Created
   ↓
Submitted
   ↓
Pending
   ↓
Executing
   ↓
Completed
```

Alternative Endzustände:

```text
Failed
Cancelled
Timeout
Unavailable
```

Ein unbekannter Ausführungszustand muss darstellbar bleiben.

## CallID

Jeder Call muss innerhalb seines relevanten Kommunikationskontexts eindeutig identifizierbar sein.

```text
CallID
  ├→ Request
  ├→ Completion
  ├→ Cancellation
  └→ Diagnostics
```

Die CallID ist eine Korrelationsidentität und keine Zugriffsberechtigung.

## Target

Das Ziel kann ein Endpoint, Channel oder Service sein.

```text
Call
  ↓
Target
 ├→ Endpoint
 ├→ Channel
 └→ Service
```

Die Auflösung des Targets darf dessen Capability-Prüfung nicht ersetzen.

## Synchron und Asynchron

Das Call-Modell muss asynchron ausgelegt sein.

```text
Submit
  ↓
Continue Execution
  ↓
Completion
```

Synchrone Aufrufe dürfen darauf aufgebaut werden:

```text
Submit
  ↓
Wait
  ↓
Completion
```

Ein wartender Thread ist damit eine API-Entscheidung und keine Voraussetzung des IPC-Modells.

## Completion

Jeder akzeptierte Call muss einen bestimmbaren Abschlusszustand erhalten können.

```text
CallID
  ↓
Completion
├── Success
├── Failed
├── Cancelled
├── Timeout
└── Unavailable
```

Fachliche Fehler und IPC-/Transportfehler müssen unterscheidbar bleiben.

## Cancellation

Ein ausstehender Call darf eine Cancellation-Anforderung erhalten.

```text
Caller
  ↓ Cancel(CallID)
IPC Runtime
  ↓
Callee
```

Cancellation bedeutet nicht automatisch, dass die Operation nicht bereits ausgeführt wurde.

Der tatsächliche Abschlusszustand muss separat bestimmt werden.

## Deadline

Calls dürfen eine Deadline besitzen.

```text
Call
├── Submitted At
└── Deadline
```

Ein Deadline Miss muss erkennbar sein.

Deadline und lokale Wartezeit dürfen nicht gleichgesetzt werden.

## Capability-Prüfung

Vor Ausführung muss NovaOS prüfen können:

```text
Caller Identity
Target Capability
Interface
Operation
Security Context
```

Eine gültige CallID oder Target-ID darf keine Autorität erzeugen.

## Datenübertragung

Kleine Requests dürfen direkt übertragen werden.

Größere Payloads dürfen verwenden:

```text
Shared Buffer
Shared Memory
Zero-Copy Reference
```

Dabei gelten weiterhin die jeweiligen Ownership-, Lifetime- und Capability-Regeln.

## Structured Concurrency

Calls sollen einem ausführenden Task oder Task-Kontext zugeordnet werden können.

```text
Task
 └── IPC Call
      └── Completion
```

Cancellation eines übergeordneten Tasks darf auf zugehörige Calls propagiert werden, sofern deren Semantik dies unterstützt.

## Ressourcensteuerung

NovaOS muss Calls begrenzen können hinsichtlich:

```text
Concurrent Calls
Pending Calls
Payload Size
Memory
Queue Usage
Execution Budget
```

Überlastung muss durch Backpressure oder kontrollierte Ablehnung behandelt werden können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
CallID
Caller
Target
Operation
State
Submission Time
Deadline
Completion State
Error
```

Payload-Inhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches IPC-Call-Modell bereitstellen.
2. Jeder Call MUSS eindeutig identifizierbar sein.
3. CallID und Zugriffsberechtigung MÜSSEN getrennt bleiben.
4. Calls MÜSSEN einen definierten Lifecycle besitzen.
5. Das grundlegende Call-Modell MUSS asynchrone Ausführung unterstützen.
6. Synchrone Calls SOLLEN auf dem asynchronen Modell aufbauen können.
7. Calls MÜSSEN einen eindeutig bestimmbaren Completion-Zustand besitzen können.
8. Cancellation DARF NICHT automatisch als Nichtausführung interpretiert werden.
9. Deadline und lokale Wartezeit MÜSSEN getrennt behandelt werden.
10. Die Ausführung eines Calls MUSS durch passende IPC-Capabilities autorisiert werden.
11. Calls MÜSSEN durch Ressourcenlimits und Backpressure begrenzbar sein.
12. Call- und Completion-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IPC-CHANNEL-0001`
- `NPSPEC-IPC-RPC-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-IPC-0010`

## Ergebnis

```text
Caller
  ↓
IPCCall
  ↓
Capability Check
  ↓
Target
  ↓
Execution
  ↓
Completion
```

NovaOS erhält damit ein einheitliches IPC-Call-Modell, auf dem synchrone und asynchrone Aufrufe sowie höherwertige Mechanismen wie RPC aufbauen können.