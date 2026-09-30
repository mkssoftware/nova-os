# NPSPEC-STATETIME-0006 – Temporal Restore & Branching

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie frühere Zustände wiederhergestellt und daraus neue Zustandszweige erzeugt werden.

Ziel ist, historische Zustände nutzbar zu machen, ohne bestehende Historie zu überschreiben.

## Grundprinzip

```text
State@t0
    ↓
State@t1
    ↓
State@t2
     \
      ↓ Restore
     State@t1
        ↓
     New Branch
```

Ein Restore erzeugt neue Historie, statt bestehende Zustände nachträglich zu verändern.

## Restore

Als Restore-Punkt dürfen verwendet werden:

```text
state_id
snapshot_id
timeline_position
checkpoint
```

Vor der Wiederherstellung müssen Zustand, Abhängigkeiten und Berechtigungen geprüft werden.

## Branching

Wird von einem historischen Zustand weitergearbeitet, entsteht ein neuer Branch.

```text
state:100
    ↓
state:101
    ├── state:102
    └── state:200
```

Beide Zweige bleiben mit ihrem gemeinsamen Ursprung verknüpft.

## Identität

Ein Branch benötigt eine eigene Identität.

Beispiel:

```text
branch:
    branch:42

parent:
    state:101
```

Dadurch bleiben parallele Entwicklungen eindeutig unterscheidbar.

## Restore Scope

Ein Restore darf unterschiedliche Bereiche betreffen:

```text
OBJECT
TASK
INTENT
WORKSPACE
SYSTEM
```

Der Scope muss vor der Wiederherstellung eindeutig bestimmt sein.

## Abhängigkeiten

Historische Zustände können von Ressourcen oder Capabilities abhängen, die nicht mehr verfügbar sind.

NovaOS darf dann:

```text
restore
replan
substitute
block
```

verwenden.

Der ursprüngliche historische Zustand darf dadurch nicht verändert werden.

## Seiteneffekte

Bereits ausgeführte externe oder nicht-idempotente Aktionen dürfen durch einen Restore nicht unbeabsichtigt erneut ausgeführt werden.

Beispiel:

```text
payment completed
    ↓ restore older state
    ↓
payment must not repeat automatically
```

Hierfür müssen Causality- und Effect-Informationen berücksichtigt werden.

## Restore-Ergebnis

Mindestens folgende Ergebnisse müssen unterscheidbar sein:

```text
RESTORED
PARTIAL
REPLAN_REQUIRED
INCOMPATIBLE
BLOCKED
```

`PARTIAL` darf nicht als vollständige Wiederherstellung dargestellt werden.

## Beispiel

```text
Timeline:

state:100
    ↓
state:101
    ↓
state:102
```

Restore auf:

```text
state:101
```

Neue Entwicklung:

```text
state:101
    ├── state:102
    └── state:200
            ↓
        state:201
```

Die ursprüngliche Timeline bleibt erhalten.

## Normative Anforderungen

1. Historische Zustände MÜSSEN als Restore-Punkte verwendet werden können.
2. Ein Restore DARF bestehende historische Zustände nicht überschreiben.
3. Weiterarbeit nach einem Restore MUSS als neuer Branch darstellbar sein.
4. Branches MÜSSEN ihren Ursprung eindeutig referenzieren.
5. Restore-Abhängigkeiten MÜSSEN vor Aktivierung geprüft werden.
6. Nicht-idempotente externe Aktionen DÜRFEN durch Restore nicht unbeabsichtigt wiederholt werden.
7. NovaOS MUSS mindestens `RESTORED`, `PARTIAL`, `REPLAN_REQUIRED`, `INCOMPATIBLE` und `BLOCKED` unterscheiden können.

## Abgrenzung

Diese NPSPEC definiert:

- Temporal Restore
- Branching
- Restore Scope
- Branch-Identität
- Umgang mit historischen Seiteneffekten

Nicht Bestandteil sind:

- historische Abfragen
- Snapshot-Erstellung
- Retention
- Garbage Collection

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0001 – Temporal State Model`
- `NPSPEC-STATETIME-0002 – System State Timeline`
- `NPSPEC-STATETIME-0003 – Temporal Object Identity`
- `NPSPEC-STATETIME-0004 – State Snapshot Coordination`
- `NPSPEC-STATETIME-0005 – Historical State Query`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`
- `NPSPEC-CAUSAL-0002 – Causal Event Records`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`