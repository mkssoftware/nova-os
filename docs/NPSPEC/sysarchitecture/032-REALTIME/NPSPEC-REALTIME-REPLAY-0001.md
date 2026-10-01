# NPSPEC-REALTIME-REPLAY-0001 – Nova Realtime Record & Replay

## Status

Angenommen

## Kategorie

Realtime / Determinism / Record & Replay / Diagnostics

## Zweck

NovaOS definiert ein Record-&-Replay-Modell, mit dem zeitkritische Executions und ihre relevanten nichtdeterministischen Einflüsse aufgezeichnet und später kontrolliert reproduziert werden können.

```text
Realtime Execution
       ↓
Record Events
       ↓
Execution Record
       ↓
Replay Environment
       ↓
Deterministic Replay
       ↓
Analysis / Verification
```

Record & Replay dient insbesondere Diagnose, Debugging, Fehleranalyse und Verifikation.

## Grundprinzipien

```text
Replay ≠ Re-execution
Replay ≠ Realtime Guarantee
Recorded ≠ Complete
Same Input ≠ Same Execution
Timestamp ≠ Causality
Replay ≠ Authority
```

Eine Replay-Ausführung darf keine zusätzlichen Berechtigungen erhalten.

## Replay Model

```text
RealtimeReplay
├── ReplayID
├── ExecutionID
├── RecordID
├── ReplayMode
├── EventStream
└── State
```

Optional:

```text
ExecutionContractID
RealtimeProfile
AlgorithmID
ProviderID
PolicyVersion
SystemVersion
ResourceTopology
InitialState
TraceID
ProvenanceID
```

## Aufzuzeichnende Einflüsse

Abhängig vom Execution Contract können aufgezeichnet werden:

```text
External Inputs
Interrupts
Device Events
IPC Messages
Scheduling Decisions
Timing Events
Network Inputs
Random Values
Resource Decisions
Provider Selection
Algorithm Selection
Cancellation
Failures
```

Nur für die Reproduktion relevante Informationen sollen aufgezeichnet werden.

## Initial State

Replay benötigt einen definierten Ausgangszustand.

```text
Checkpoint / Snapshot
        +
Execution Record
        ↓
Replay
```

Der Ausgangszustand muss eindeutig einer kompatiblen System- und Objektversion zugeordnet werden können.

## Event Ordering

Aufgezeichnete Ereignisse benötigen eine reproduzierbare Ordnung.

```text
Event
├── EventID
├── Source
├── Sequence
├── Timestamp
└── Causal Context
```

Zeitstempel allein dürfen bei konkurrierenden oder verteilten Ereignissen nicht als vollständige Kausalitätsinformation betrachtet werden.

## Scheduling Replay

Scheduler-Entscheidungen können aufgezeichnet werden:

```text
Runnable Set
     ↓
Recorded Decision
     ↓
Selected Task
```

Beim Replay kann NovaOS diese Entscheidungen reproduzieren, sofern die Replay-Umgebung kompatibel ist.

## External Events

Externe Ereignisse dürfen während Replay nicht unkontrolliert erneut in die Execution einfließen.

```text
Live External Input
        X

Recorded Input
        ↓
Replay Execution
```

Ausnahmen müssen explizit durch den Replay-Modus definiert werden.

## Replay Modes

NovaOS soll mindestens unterstützen können:

```text
Diagnostic Replay
Deterministic Replay
Partial Replay
Simulation Replay
```

### Diagnostic Replay

Reproduziert genügend Kontext für Fehleranalyse.

### Deterministic Replay

Versucht alle relevanten aufgezeichneten Entscheidungen und Eingaben reproduzierbar wiederzugeben.

### Partial Replay

Reproduziert einen definierten Teil der Execution.

### Simulation Replay

Führt aufgezeichnete Ereignisse in einer isolierten Analyseumgebung aus.

## Timing

Replay kann zwei Zeitmodelle verwenden:

```text
Logical Timing
Recorded Timing
```

Logical Timing erhält Ereignisreihenfolge und Abhängigkeiten.

Recorded Timing versucht zusätzlich, aufgezeichnete zeitliche Abstände nachzubilden.

```text
Recorded Timing ≠ Original Realtime Guarantee
```

## Realtime Verhalten

Record-Funktionalität darf zugesicherte Realtime-Garantien nicht unkontrolliert beeinträchtigen.

Aufzeichnung muss daher:

```text
Bounded
Buffered
Resource Accounted
Backpressure Aware
```

sein.

Bei Ressourcenknappheit muss entsprechend der Policy entschieden werden, ob Daten reduziert, verworfen oder die Record-Funktion deaktiviert wird.

Eine Hard-Realtime-Execution darf nicht wegen optionaler Aufzeichnung ihre Deadline verletzen.

## Overflow

Record Buffer können überlaufen.

```text
Buffer Full
   ↓
Policy
├── Drop Optional Events
├── Reduce Detail
├── Stop Recording
└── Reserved Critical Path
```

Verlorene Daten müssen im Record kenntlich gemacht werden.

```text
Incomplete Record ≠ Complete Replay
```

## Distributed Replay

Bei verteilter Execution können mehrere Event Streams beteiligt sein.

```text
Node A ─┐
Node B ─┼→ Causal Event Graph → Replay
Node C ─┘
```

Eine perfekte globale Uhr darf nicht vorausgesetzt werden.

Kausale Beziehungen und Distributed Trace Context sollen zur Rekonstruktion verwendet werden können.

## Security

Replay Records können sensible Informationen enthalten.

Es gelten:

```text
Capability Checks
Privacy Labels
Data Minimization
Encryption
Retention Policy
Sovereignty Rules
```

Secrets und Credentials dürfen nicht unnötig aufgezeichnet werden.

## Isolation

Replay soll standardmäßig in einer kontrollierten Umgebung stattfinden.

```text
Replay
  ↓
Isolation
  ↓
Controlled Capabilities
```

Aufgezeichnete Aktionen dürfen nicht automatisch erneut reale externe Effekte erzeugen.

Beispiele:

```text
Payment
Message Send
Device Control
File Modification
Network Request
```

## Provenance

Replay Records müssen ihre Herkunft nachvollziehbar machen können.

```text
Record
├── Source Execution
├── System Version
├── Policy Version
├── Provider Version
├── Timestamp
└── Integrity Information
```

## Verification

Replay kann verwendet werden, um:

```text
Failure Reproduction
Race Analysis
Deadline Analysis
Scheduling Analysis
Regression Detection
Determinism Verification
Self-Healing Diagnosis
```

zu unterstützen.

Replay selbst ist jedoch kein Beweis für vollständige Korrektheit.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
RecordID
ReplayID
ExecutionID
Record Completeness
Replay Mode
Event Count
Dropped Events
System Version
Policy Version
Replay State
Divergence Point
```

## Normative Anforderungen

1. NovaOS SOLL Record & Replay für zeitkritische Executions unterstützen.
2. Replay DARF NICHT mit einer Realtime-Garantie gleichgesetzt werden.
3. Relevante nichtdeterministische Eingaben MÜSSEN für deterministisches Replay erfassbar sein.
4. Replay MUSS einen definierten Ausgangszustand referenzieren können.
5. Ereignisordnung DARF NICHT ausschließlich aus Zeitstempeln abgeleitet werden.
6. Scheduler-Entscheidungen MÜSSEN bei Bedarf aufzeichnungsfähig sein.
7. Externe Live-Ereignisse DÜRFEN deterministisches Replay NICHT unkontrolliert beeinflussen.
8. Record-Operationen MÜSSEN ressourcenbegrenzt sein.
9. Optionale Aufzeichnung DARF Hard-Realtime-Garantien NICHT verletzen.
10. Buffer Overflow MUSS explizit behandelt werden.
11. Verlorene Record-Daten MÜSSEN als solche erkennbar sein.
12. Ein unvollständiger Record DARF NICHT als vollständig reproduzierbar dargestellt werden.
13. Distributed Replay DARF keine perfekte globale Uhr voraussetzen.
14. Replay MUSS bestehende Capability-, Privacy- und Sovereignty-Regeln respektieren.
15. Replay DARF aufgezeichnete externe Seiteneffekte NICHT automatisch erneut ausführen.
16. Replay Records SOLLEN versionierte Provenance enthalten.
17. Replay-Divergenzen MÜSSEN erkennbar sein.
18. Record- und Replay-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-SCHEDULING-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-REALTIME-TEMPORALISOLATION-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-0001`
- `NPSPEC-REALTIME-IO-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-DISTRIBUTEDTRACE-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0107`

## Ergebnis

```text
Realtime Execution
       ↓
Bounded Recording
       ↓
Versioned Event Record
       ↓
Checkpoint + Provenance
       ↓
Isolated Replay
       ↓
Divergence Detection
       ↓
Diagnosis / Verification
```

NovaOS erhält damit ein systemweites Record-&-Replay-Modell, das Realtime-Fehler, Race Conditions, Scheduling-Entscheidungen und nichtdeterministische Ereignisse reproduzierbar analysierbar macht, ohne Replay mit einer Realtime-Garantie oder zusätzlicher Autorität gleichzusetzen.