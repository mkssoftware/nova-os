# NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.Synthesis` Constraints und Policies bereits während der Capability-Komposition berücksichtigt.

Ziel ist, nur solche Composition Graphs zu erzeugen, die die Anforderungen des Intents tatsächlich erfüllen können.

## Grundprinzip

```text
Intent
    +
Constraints
    +
Policies
    ↓
Capability Candidates
    ↓
Constraint-Aware Composition
    ↓
gültiger Composition Graph
```

## Constraint-Arten

Die Komposition muss mindestens folgende Anforderungen berücksichtigen können:

- Latenz
- Speicher
- Energie
- Kosten
- Präzision
- Determinismus
- Trust
- lokale oder entfernte Ausführung
- Netzwerkzugriff
- Hardwareanforderungen

Hard Constraints sind zwingend.

Soft Constraints beeinflussen die Auswahl, dürfen aber bei Bedarf verletzt werden.

## Frühes Filtern

Offensichtlich ungültige Capabilities sollen möglichst früh ausgeschlossen werden.

Beispiel:

```text
Policy:
    network: denied
```

Dann wird:

```text
RemoteSpeechService
```

bereits vor dem Graphaufbau verworfen.

Dadurch wird der Suchraum reduziert.

## Node Constraints

Einzelne Nodes dürfen eigene Anforderungen besitzen.

Beispiel:

```text
Node A:
    requires GPU

Node B:
    deterministic required

Node C:
    max_memory 256MiB
```

Alle Node-Anforderungen müssen mit dem übergeordneten Intent vereinbar sein.

## Graphweite Constraints

Einige Anforderungen gelten für den gesamten Composition Graph.

Beispiel:

```text
total_latency <= 100ms
total_memory <= 1GiB
remote_execution = denied
```

Einzelne Nodes können für sich gültig sein, während der Gesamtgraph trotzdem ungültig ist.

## Constraint Propagation

Übergeordnete Constraints müssen auf relevante Teile des Graphen propagiert werden.

Beispiel:

```text
Intent:
    local_only: true
```

Dann gilt für alle ausführenden Nodes:

```text
remote_execution:
    forbidden
```

Child- oder Teilgraphen dürfen solche Einschränkungen nicht eigenständig abschwächen.

## Precision Constraints

Numerische Anforderungen müssen mit `Nova.Precision` abgestimmt werden.

Beispiel:

```text
required_error:
    <= 0.01 %
```

Eine Capability, die nur eine schlechtere Genauigkeit garantieren kann, ist für diesen Pfad ungültig.

## Trust Constraints

Ein Intent darf Mindestanforderungen an Trust stellen.

Beispiel:

```text
required_trust:
    verified
```

Dann dürfen:

```text
untrusted
```

Capabilities nicht in den Graph aufgenommen werden.

## Ressourcenbudget

Graphweite Ressourcenbudgets dürfen auf Nodes verteilt werden.

Beispiel:

```text
total_memory:
    1GiB
```

Planung:

```text
Node A:
    300MiB

Node B:
    400MiB

Node C:
    200MiB
```

Gesamt:

```text
900MiB
```

Der Graph bleibt gültig.

## Konflikte

Widersprüchliche Anforderungen müssen erkannt werden.

Beispiel:

```text
Constraint:
    remote_only: true

Policy:
    remote_execution: denied
```

Ergebnis:

```text
CONSTRAINT_CONFLICT
```

In diesem Fall darf kein scheinbar gültiger Graph erzeugt werden.

## Alternative Pfade

Scheitert ein Pfad an Constraints, darf Nova.Synthesis alternative Kompositionen prüfen.

Beispiel:

```text
GPU Pipeline
    ↓ memory limit exceeded
    ↓
CPU Pipeline
```

Das ursprüngliche Intent-Ziel bleibt unverändert.

## Soft Preferences

Soft Constraints dürfen zur Bewertung mehrerer gültiger Graphs verwendet werden.

Beispiel:

```text
prefer:
    low_energy
```

Kandidaten:

```text
Graph A:
    faster

Graph B:
    lower energy
```

Beide bleiben gültig.

Die Präferenz beeinflusst lediglich die Auswahl.

## Laufzeitänderungen

Ändern sich relevante Constraints oder Ressourcen während der Ausführung, darf Replanning notwendig werden.

Beispiel:

```text
available_memory decreases
    ↓
current graph invalid
    ↓
Replan
```

Hard Constraints dürfen auch während der Laufzeit nicht stillschweigend verletzt werden.

## Beispiel

Intent:

```text
media.audio.transcribe
```

Constraints:

```text
local_only:
    true

max_memory:
    1GiB

required_trust:
    verified
```

Kandidaten:

```text
LocalCPU:
    memory 600MiB
    trust verified

LocalGPU:
    memory 1.4GiB
    trust verified

RemoteService:
    memory low
    trust verified
```

Bewertung:

```text
LocalCPU:
    VALID

LocalGPU:
    REJECTED_MEMORY

RemoteService:
    REJECTED_LOCAL_ONLY
```

Ergebnis:

```text
LocalCPU
```

## Normative Anforderungen

1. Hard Constraints MÜSSEN bereits während der Composition berücksichtigt werden.
2. Policies MÜSSEN ungültige Nodes und Pfade ausschließen können.
3. Graphweite Ressourcen- und Qualitätsgrenzen MÜSSEN überprüfbar sein.
4. Übergeordnete Constraints MÜSSEN auf relevante Teilgraphen propagiert werden.
5. Teilgraphen DÜRFEN geerbte Hard Constraints nicht eigenständig abschwächen.
6. Widersprüchliche Anforderungen MÜSSEN als Constraint Conflict erkannt werden.
7. Ungültige Pfade SOLLEN möglichst früh aus dem Suchraum entfernt werden.
8. Soft Preferences DÜRFEN die Gültigkeit eines ansonsten zulässigen Graphen nicht verändern.

## Abgrenzung

Diese NPSPEC definiert:

- Constraint-Aware Composition
- frühes Kandidatenfiltering
- graphweite Constraints
- Constraint Propagation
- Konflikterkennung

Nicht Bestandteil sind:

- allgemeines Intent-Policy-Modell
- Pipeline Verification
- konkrete Optimierungsalgorithmen
- Scheduling
- Composition Cache

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`
- `NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification`
- `NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation`
- `NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-PRECISION-0001 – Precision Contract`