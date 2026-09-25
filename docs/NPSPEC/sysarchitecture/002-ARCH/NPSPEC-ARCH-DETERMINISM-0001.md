# NPSPEC-ARCH-DETERMINISM-0001 – Nova Deterministic Execution

## Status

Angenommen

## Kategorie

Architecture / Execution / Determinism / Reproducibility

## Zweck

NovaOS unterstützt deterministische Ausführung für Operationen, bei denen gleiche definierte Eingaben und Bedingungen reproduzierbare Ergebnisse erfordern.

```text
Input
+
Execution Conditions
+
Determinism Contract
      ↓
Execution
      ↓
Reproducible Result
```

Determinismus ist eine explizite Ausführungsanforderung und kein globaler Zwang für das gesamte System.

## Determinismusklassen

Eine Ausführung kann deklarieren:

```text
Required
Preferred
NotRequired
```

`Required` ist ein Hard Constraint.

`Preferred` darf zugunsten höher priorisierter Anforderungen verworfen werden.

## Deterministische Umgebung

Abhängig von der Operation können kontrolliert werden:

```text
Scheduling
Thread Ordering
Time Sources
Randomness
Input Ordering
Algorithm
Provider
Hardware Behaviour
External Inputs
```

Nur für die jeweilige Determinismusklasse relevante Faktoren müssen kontrolliert werden.

## Execution Contract

Determinismus wird über `Nova.ExecutionContract` angefordert.

```text
Determinism = Required
        ↓
Execution Planner
        ↓
Deterministic Providers
        ↓
Deterministic Execution
```

Nicht geeignete Provider oder Algorithmen werden ausgeschlossen.

## Nebenläufigkeit

Nebenläufigkeit ist auch bei deterministischer Ausführung erlaubt.

Sie muss jedoch so koordiniert werden, dass relevante Ergebnisse nicht von unkontrollierten Race Conditions oder Scheduling-Unterschieden abhängen.

## Zeit und Zufall

Direkter Zugriff auf nichtdeterministische Quellen kann eingeschränkt oder virtualisiert werden.

```text
Time → Controlled Time Source
Random → Seeded Random Source
```

## Record / Replay

Deterministische Ausführung kann mit Record/Replay kombiniert werden.

```text
Execution
   ↓
Record Inputs / Events
   ↓
Replay
   ↓
Reproduce Behaviour
```

Record/Replay ersetzt jedoch keine deterministische Architektur.

## Verifikation

NovaOS soll prüfen können, ob ein gewählter Ausführungspfad die angeforderte Determinismusklasse unterstützt.

Kann dies nicht nachgewiesen werden:

```text
Unknown ≠ Deterministic
```

Bei `Required` darf die Ausführung nicht als deterministisch garantiert werden.

## Normative Anforderungen

1. NovaOS MUSS deterministische Ausführung explizit anforderbar machen.
2. Determinismus MUSS über den Execution Contract ausdrückbar sein.
3. `Required` MUSS als Hard Constraint behandelt werden.
4. Nichtdeterministische Provider DÜRFEN bei `Required` nicht ausgewählt werden.
5. Relevante Zeit-, Zufalls- und Ereignisquellen MÜSSEN kontrollierbar sein.
6. Nebenläufigkeit DARF deterministische Garantien nicht verletzen.
7. Provider- und Algorithmuswahl MÜSSEN die geforderte Determinismusklasse berücksichtigen.
8. `Unknown` DARF NICHT als nachgewiesener Determinismus behandelt werden.
9. Deterministische Ausführung SOLL mit Record/Replay kombinierbar sein.
10. NovaOS DARF Determinismus nicht global erzwingen, wenn er nicht benötigt wird.

## Abhängigkeiten

- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-REALTIME-0005_Deterministic_Execution`
- `ADR-REALTIME-0006_Deterministic_IO`
- `ADR-REALTIME-0007_Record_Replay`

## Ergebnis

```text
Execution Contract
       ↓
Determinism Requirement
       ↓
Controlled Execution Environment
       ↓
Reproducible Execution
```

NovaOS kann damit deterministische Ausführung gezielt dort garantieren, wo sie benötigt wird, ohne das gesamte System unnötig auf deterministisches Verhalten zu beschränken.