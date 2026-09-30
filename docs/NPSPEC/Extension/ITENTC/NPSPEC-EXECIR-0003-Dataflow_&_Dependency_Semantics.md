# NPSPEC-EXECIR-0003 – Dataflow & Dependency Semantics

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Semantik von Datenflüssen und Abhängigkeiten innerhalb der `Nova Execution IR`.

Ziel ist, eindeutig festzulegen, wann ein Node ausgeführt werden darf und welche Daten dafür verfügbar sein müssen.

## Grundprinzip

```text
Producer
    ↓ Value
Consumer
```

Ein Consumer darf erst ausgeführt werden, wenn seine erforderlichen Inputs verfügbar und alle zwingenden Abhängigkeiten erfüllt sind.

## Dataflow

Daten werden über IR Values zwischen Nodes übertragen.

```text
Node A
    ↓ value:1
Node B
```

Ein Value besitzt einen definierten Producer und darf von mehreren Consumern verwendet werden.

## Dependency-Typen

Mindestens folgende Abhängigkeiten müssen unterstützt werden:

```text
DATA
CONTROL
ORDER
RESOURCE
```

### DATA

Ein Node benötigt einen erzeugten Value.

```text
A → value → B
```

### CONTROL

Die Ausführung hängt von einer Kontrollentscheidung ab.

### ORDER

Eine Operation muss vor oder nach einer anderen stattfinden.

### RESOURCE

Die Ausführung hängt von einer Ressource oder deren Zustand ab.

## Readiness

Ein Node gilt als ausführungsbereit, wenn:

```text
required inputs available
+
required dependencies satisfied
+
required conditions satisfied
```

sind.

## Parallelität

Bestehen keine relevanten Abhängigkeiten, dürfen Nodes parallel ausgeführt werden.

```text
        ┌── B
A ──────┤
        └── C
```

Eine gemeinsame Herkunft erzeugt keine automatische Reihenfolge zwischen B und C.

## Datenidentität

Values müssen unabhängig von konkreten Speicheradressen identifizierbar sein.

```text
value:42
```

Die spätere Ausführung darf diesen Value auf unterschiedliche Speicher- oder Transportmechanismen abbilden.

## Seiteneffekte

Operationen mit Seiteneffekten müssen entsprechende Abhängigkeiten explizit ausdrücken können.

Beispiel:

```text
write file
    ↓ ORDER
send notification
```

Dadurch darf die IR keine implizite Reihenfolge voraussetzen.

## Fehlende Abhängigkeiten

Kann eine zwingende Abhängigkeit nicht erfüllt werden, darf der betroffene Node nicht normal ausgeführt werden.

Mögliche Zustände:

```text
READY
WAITING
BLOCKED
FAILED_DEPENDENCY
```

## Beispiel

```text
node:decode
    ↓ audio

        ┌──────────────┐
        ↓              ↓
node:transcribe    node:analyze
        ↓              ↓
      text           metadata
```

`transcribe` und `analyze` dürfen parallel laufen, sobald `audio` verfügbar ist.

## Normative Anforderungen

1. Datenflüsse MÜSSEN über explizite Values darstellbar sein.
2. Zwingende Abhängigkeiten MÜSSEN vor der Ausführung eines Nodes erfüllt sein.
3. `DATA`, `CONTROL`, `ORDER` und `RESOURCE` MÜSSEN unterscheidbar sein.
4. Unabhängige Nodes DÜRFEN parallel ausgeführt werden.
5. Seiteneffektabhängigkeiten MÜSSEN explizit darstellbar sein.
6. Values DÜRFEN nicht von konkreten Speicheradressen als Identität abhängen.
7. Nicht erfüllbare zwingende Abhängigkeiten MÜSSEN als blockiert oder fehlgeschlagen erkennbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Dataflow-Semantik
- Dependency-Typen
- Node Readiness
- Parallelität
- Seiteneffektabhängigkeiten

Nicht Bestandteil sind:

- Scheduling-Algorithmen
- konkrete Speicherübertragung
- Execution Contracts
- Backend-Lowering

## Zugehörige NPSPECs

- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-EXECIR-0005 – IR Optimization & Transformation`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`