# NPSPEC-EXECUTION-DETERMINISM-0001 – Nova Execution Determinism

## Status

Angenommen

## Kategorie

Execution / Determinism / Execution Model

## Zweck

NovaOS definiert Determinismus als explizite Eigenschaft eines Execution Contracts.

Eine Operation kann festlegen, ob identische Eingaben unter definierten Bedingungen reproduzierbare Ausführung und Ergebnisse erfordern.

```text
ExecutionContract
      ↓
Determinism Requirement
      ↓
Provider + Resource Selection
      ↓
Controlled Execution
      ↓
Determinism Verification
```

## Grundprinzipien

```text
Determinism ≠ Correctness
Determinism ≠ Reliability
Determinism ≠ Realtime
Determinism ≠ Identical Performance
Determinism ≠ Guaranteed Completion

Same Input ≠ Same Result
wenn relevante Ausführungsbedingungen unterschiedlich sind
```

Determinismus muss daher immer zusammen mit seinem definierten Kontext betrachtet werden.

## Determinism Requirement

Ein Execution Contract kann deklarieren:

```text
Determinism:
├── Required
├── Preferred
└── NotRequired
```

Optional:

```text
Determinism Scope
Input State
Environment State
Randomness Policy
Concurrency Policy
Time Policy
Provider Constraints
Precision Constraints
External Dependency Policy
```

## Required

Bei:

```text
Determinism = Required
```

dürfen ausschließlich Ausführungspfade verwendet werden, die die geforderte Determinismusklasse erfüllen.

Ist dies nicht möglich:

```text
No Deterministic Provider
        ↓
Replan / Reject / Fail
```

NovaOS darf die Anforderung nicht stillschweigend abschwächen.

## Preferred

Bei:

```text
Determinism = Preferred
```

wird deterministische Ausführung bevorzugt.

Andere Provider dürfen verwendet werden, wenn der Contract dies zulässt.

## NotRequired

Bei:

```text
Determinism = NotRequired
```

darf NovaOS stärker auf:

```text
Performance
Energy
Latency
Throughput
Hardware Utilization
```

optimieren.

## Determinism Scope

Der Contract muss definieren können, welche Eigenschaft deterministisch sein soll.

Beispiele:

```text
Result Determinism
Execution Order Determinism
State Transition Determinism
Scheduling Determinism
I/O Ordering Determinism
Replay Determinism
```

Ein deterministisches Ergebnis erfordert nicht zwingend identische interne Ausführungsschritte.

## Deterministic Inputs

Alle relevanten Eingaben müssen identifizierbar sein.

```text
Explicit Input
System State
Configuration
Environment
Time
Randomness
External Data
Provider State
```

Versteckte Eingaben sollen bei Required Determinism vermieden oder explizit kontrolliert werden.

## Zeit

Direkter Zugriff auf unkontrollierte Zeitquellen kann Determinismus zerstören.

NovaOS kann deshalb eine kontrollierte Zeitquelle bereitstellen:

```text
Real Time
Recorded Time
Virtual Time
Fixed Time
```

Der Execution Contract bestimmt die zulässige Semantik.

## Zufall

Zufällige Operationen müssen bei deterministischer Ausführung kontrolliert werden.

```text
Random Input
    ↓
Explicit Seed
    ↓
Deterministic Generator
```

Ein implizit wechselnder Seed ist bei Required Determinism nicht zulässig.

## Concurrency

Nebenläufigkeit kann unterschiedliche Ausführungsreihenfolgen erzeugen.

```text
Task A ─┐
        ├→ Shared State
Task B ─┘
```

Deterministische Ausführung kann deshalb verlangen:

```text
Defined Ordering
Deterministic Synchronization
Controlled Scheduling
Deterministic Reduction
Race-Free State
```

Structured Concurrency liefert hierfür kontrollierte Lebenszyklen, garantiert aber allein noch keinen Determinismus.

## Numerische Ausführung

CPU, GPU und NPU können bei numerischen Operationen unterschiedliche Ergebnisse erzeugen.

Zu berücksichtigen sind:

```text
Floating-Point Precision
Operation Ordering
Rounding
Fused Operations
Parallel Reduction
Hardware Instructions
Compiler Optimization
```

Der Contract kann deshalb festlegen:

```text
Required Precision
Allowed Tolerance
Required Algorithm
Allowed Providers
```

## Provider Selection

Provider Discovery muss Determinismus berücksichtigen.

```text
Compatible Providers
       ↓
Determinism Filter
       ↓
CPU / GPU / NPU / Service
       ↓
Selected Provider
```

Ein schnellerer Provider darf nicht gewählt werden, wenn er `Required` Determinism nicht erfüllen kann.

## External Dependencies

Externe Dienste oder Datenquellen können nichtdeterministische Zustände erzeugen.

Bei Required Determinism können deshalb erforderlich sein:

```text
Pinned Version
Snapshot
Recorded Input
ContentID
Fixed Provider Version
Offline Data
```

Nicht kontrollierbare externe Abhängigkeiten können eine deterministische Ausführung unmöglich machen.

## Record / Replay

NovaOS kann relevante nichtdeterministische Eingaben aufzeichnen.

```text
Execution
   ↓
Record External Inputs
   ↓
Execution Record
   ↓
Replay
```

Aufgezeichnet werden können:

```text
Events
Timing Inputs
Random Seeds
External Responses
Scheduling Decisions
I/O Results
```

Record/Replay ist jedoch nicht automatisch gleichbedeutend mit vollständigem Determinismus.

## Transactions

Deterministische Operationen können mit Transactions kombiniert werden.

```text
Deterministic Execution
        ↓
Transaction
        ↓
Defined State Transition
```

Dadurch können reproduzierbare Zustandsänderungen unterstützt werden.

## Live Evolution

Provider- oder Implementierungswechsel können Determinismus beeinflussen.

```text
Provider Version A
       ↓
Live Replacement
       ↓
Provider Version B
```

Bei Required Determinism muss NovaOS prüfen, ob die neue Implementierung weiterhin den Contract erfüllt.

## Verification

Nach oder während der Ausführung können geprüft werden:

```text
Provider
Provider Version
Algorithm
Input Versions
Random Seed
Precision
Environment
Execution Record
Output
```

Optional können wiederholte Ausführungen oder Referenzergebnisse verglichen werden.

## Security

Determinismus darf Sicherheitsmechanismen nicht abschwächen.

Insbesondere dürfen Anforderungen an reproduzierbare Ausführung nicht automatisch deaktivieren:

```text
ASLR
Cryptographic Randomness
Security Isolation
Secret Generation
Nonce Requirements
```

Sicherheitskritische Zufälligkeit besitzt Vorrang vor allgemeinem Execution Determinism.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Determinism Requirement
Determinism Scope
Selected Provider
Provider Version
Algorithm
Precision
Randomness Policy
Time Policy
Replay State
Verification State
Determinism Violations
```

## Normative Anforderungen

1. NovaOS MUSS Determinismus als Bestandteil von Execution Contracts unterstützen.
2. `Required`, `Preferred` und `NotRequired` MÜSSEN unterscheidbar sein.
3. Required Determinism DARF nicht stillschweigend abgeschwächt werden.
4. Determinismus MUSS unabhängig von Correctness, Realtime und Reliability modelliert werden.
5. Relevante Zeit-, Zufalls- und externe Eingaben MÜSSEN kontrollierbar sein.
6. Provider Selection MUSS Required Determinism berücksichtigen.
7. Numerische Precision und Operation Ordering MÜSSEN bei relevanten Workloads berücksichtigt werden.
8. Provider- oder Implementierungswechsel DÜRFEN Required Determinism NICHT unbemerkt verletzen.
9. Record/Replay SOLL für geeignete deterministische Ausführungen unterstützt werden.
10. Determinismus DARF kryptografische oder sicherheitskritische Zufälligkeit NICHT schwächen.
11. Deterministische Ausführung MUSS mit Structured Concurrency und Transactions integrierbar sein.
12. Determinism State und Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-SEMANTICTYPES-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `ADR-ARCH-0053`

## Ergebnis

```text
ExecutionContract
      ↓
Determinism Requirement
      ↓
Control Inputs + Environment
      ↓
Deterministic Provider Selection
      ↓
Controlled Execution
      ↓
Verification / Record / Replay
```

NovaOS erhält damit ein durchgängiges Determinismusmodell, mit dem reproduzierbare Ausführung explizit angefordert, geplant und überprüft werden kann, ohne Determinismus mit Korrektheit, Realtime-Verhalten oder Sicherheit gleichzusetzen.