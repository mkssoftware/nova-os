# NPSPEC-STATETIME-0007 – State Retention & Garbage Collection

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie lange historische Zustände in `Nova.StateTime` erhalten bleiben und wann nicht mehr benötigte Zustände entfernt werden dürfen.

Ziel ist, Wiederherstellbarkeit und Historie zu erhalten, ohne Speicher unbegrenzt wachsen zu lassen.

## Grundprinzip

```text
Historical States
    ↓
Retention Policy
    ↓
KEEP / COMPACT / DELETE
```

## Retention-Klassen

Zustände dürfen unterschiedlichen Aufbewahrungsklassen zugeordnet werden.

Beispiele:

```text
TEMPORARY
SHORT_TERM
RECOVERY
HISTORICAL
PINNED
```

`PINNED` Zustände dürfen nicht automatisch entfernt werden.

## Abhängigkeiten

Ein Zustand darf nur gelöscht werden, wenn keine notwendige Abhängigkeit mehr auf ihn verweist.

Zu prüfen sind insbesondere:

```text
snapshots
branches
checkpoints
TaskCapsules
Evidence
Causality
```

## Garbage Collection

Garbage Collection darf Zustände entfernen, die:

```text
expired
unreferenced
superseded
invalid
```

sind.

Die Entfernung darf keine verbleibende Historie inkonsistent machen.

## Kompaktierung

Statt vollständiger Löschung dürfen historische Zustände verdichtet werden.

Beispiel:

```text
State 100
State 101
State 102
State 103
```

kann zu:

```text
Base State 100
    +
Compacted History
    +
State 103
```

verdichtet werden, sofern notwendige Rekonstruktion erhalten bleibt.

## Branches

Zustände, die Ursprung eines aktiven Branches sind, dürfen nicht entfernt werden, solange sie für dessen Historie benötigt werden.

```text
state:100
    ├── branch:A
    └── branch:B
```

Gemeinsam benötigte Zustände müssen erhalten bleiben.

## Retention Policy

Die Aufbewahrung darf abhängig sein von:

```text
age
storage_cost
importance
recovery_value
reference_count
user_policy
system_policy
```

Höher priorisierte Policies dürfen eine automatische Löschung verhindern.

## Beispiel

```text
state:100
    expired
    unreferenced

state:101
    referenced by snapshot

state:102
    pinned
```

Ergebnis:

```text
state:100 → DELETE
state:101 → KEEP
state:102 → KEEP
```

## Normative Anforderungen

1. Historische Zustände MÜSSEN einer definierbaren Retention Policy unterliegen.
2. Referenzierte oder für aktive Branches benötigte Zustände DÜRFEN nicht vorzeitig entfernt werden.
3. `PINNED` Zustände DÜRFEN nicht automatisch gelöscht werden.
4. Garbage Collection MUSS Abhängigkeiten vor der Löschung prüfen.
5. Zustände DÜRFEN kompaktierbar sein, sofern notwendige Historie und Rekonstruktion erhalten bleiben.
6. Retention MUSS Speicherverbrauch und Wiederherstellungswert berücksichtigen können.
7. Garbage Collection DARF keine verbleibende Timeline inkonsistent machen.

## Abgrenzung

Diese NPSPEC definiert:

- State Retention
- Garbage Collection
- Retention-Klassen
- Kompaktierung
- Schutz referenzierter Zustände

Nicht Bestandteil sind:

- allgemeine Storage-Garbage-Collection
- Snapshot-Erstellung
- historische Abfragen
- Restore und Branching

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0001 – Temporal State Model`
- `NPSPEC-STATETIME-0002 – System State Timeline`
- `NPSPEC-STATETIME-0003 – Temporal Object Identity`
- `NPSPEC-STATETIME-0004 – State Snapshot Coordination`
- `NPSPEC-STATETIME-0005 – Historical State Query`
- `NPSPEC-STATETIME-0006 – Temporal Restore & Branching`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`