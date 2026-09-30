# NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Ressourcenverbrauch und Aufbewahrung von `MicroCheckpoints` steuert.

Ziel ist, ausreichend Wiederherstellungspunkte bereitzuhalten, ohne Speicher, I/O oder Energie unnötig zu belasten.

## Grundprinzip

```text
Checkpoint
    ↓
Resource Evaluation
    ↓
Keep / Consolidate / Archive / Delete
```

## Ressourcen

Die Policy darf mindestens berücksichtigen:

- Speicherplatz
- Arbeitsspeicher
- Schreibaufwand
- I/O-Bandbreite
- Energieverbrauch
- Restore-Zeit
- Checkpoint-Erzeugungszeit

Nicht jeder Checkpoint muss persistent gespeichert werden.

## Checkpoint-Klassen

Checkpoints dürfen unterschiedliche Aufbewahrungsklassen besitzen.

Beispiel:

```text
EPHEMERAL
SHORT_TERM
RECOVERY
MIGRATION
PINNED
```

### `EPHEMERAL`

Kurzlebiger Checkpoint, typischerweise nur im RAM.

### `SHORT_TERM`

Für kurzfristiges Pause/Resume oder lokale Recovery.

### `RECOVERY`

Persistenter Zustand für Crash Recovery.

### `MIGRATION`

Checkpoint mit den für Systemwechsel notwendigen Informationen.

### `PINNED`

Darf nicht automatisch gelöscht werden, solange die Pinning-Bedingung gilt.

## Retention Policy

Eine Policy kann logisch enthalten:

```text
CheckpointRetentionPolicy {
    max_count
    max_storage
    max_age
    minimum_recovery_points
    consolidation_policy
}
```

Diese Werte dürfen je nach Intent oder Task unterschiedlich sein.

## Aufbewahrungspriorität

Bei Ressourcenknappheit sollen Checkpoints bevorzugt erhalten bleiben, wenn sie:

- letzter gültiger Recovery Point sind
- vor einer irreversiblen Aktion liegen
- einen wichtigen Intent sichern
- für Migration benötigt werden
- vom Nutzer oder System gepinnt wurden

Rein redundante Zwischenstände dürfen früher entfernt werden.

## Delta-Ketten

Inkrementelle Checkpoints müssen gemeinsam mit ihren Abhängigkeiten betrachtet werden.

Beispiel:

```text
Full A
↓
Delta B
↓
Delta C
```

`Full A` darf nicht gelöscht werden, solange `B` oder `C` davon abhängen und kein anderer gültiger Basiszustand existiert.

## Konsolidierung

Lange oder teure Delta-Ketten dürfen zusammengeführt werden.

Beispiel:

```text
Full A
↓
Delta B
↓
Delta C
↓
Delta D
```

wird zu:

```text
Full E
```

Nach erfolgreicher Konsolidierung dürfen nicht mehr benötigte Vorgänger gemäß Retention Policy entfernt werden.

## Mindestschutz

Für aktive wiederaufnehmbare Aufgaben soll mindestens ein gültiger Recovery Point erhalten bleiben.

Wenn dies wegen Ressourcenmangel nicht möglich ist, muss NovaOS den Zustand kenntlich machen.

Beispiel:

```text
checkpoint_protection:
    degraded
```

NovaOS darf nicht stillschweigend alle Recovery-Möglichkeiten entfernen.

## Alterung

Checkpoints dürfen nach Alter entfernt werden.

Beispiel:

```text
max_age:
    24h
```

Alter allein darf jedoch keinen noch benötigten Basis-Checkpoint löschen.

Abhängigkeiten und Recovery-Relevanz haben Vorrang.

## Ressourcenbudget

Ein Intent oder eine TaskCapsule darf ein Checkpoint-Budget besitzen.

Beispiel:

```text
checkpoint_budget {
    max_storage: 512MiB
    max_count: 8
}
```

Bei Überschreitung kann NovaOS:

```text
consolidate
remove_oldest_safe
reduce_frequency
switch_to_memory_only
```

## Checkpoint-Frequenz

Die Policy darf beeinflussen, wie häufig Checkpoints erzeugt werden.

Beispiel:

```text
high_failure_risk:
    frequent

low_failure_risk:
    sparse
```

Auch Kosten der Wiederholung dürfen berücksichtigt werden.

Eine lange, teure Berechnung kann häufiger gesichert werden als eine sehr kurze Operation.

## Irreversible Operationen

Vor einem `Point of No Return` soll ein geeigneter gültiger Checkpoint nach Möglichkeit vor automatischer Löschung geschützt werden.

Beispiel:

```text
Checkpoint A
    ↓ PIN
irreversible operation
    ↓
confirmed completion
    ↓
UNPIN
```

## Speicherknappheit

Bei starkem Ressourcenmangel darf NovaOS Checkpoints entfernen oder reduzieren.

Die Reihenfolge muss policy-gesteuert erfolgen.

Beispiel:

```text
1. expired ephemeral
2. redundant deltas
3. old short-term checkpoints
4. superseded recovery points
```

Geschützte oder notwendige Checkpoints dürfen erst entfernt werden, wenn keine sicherere Alternative existiert oder eine übergeordnete Policy dies verlangt.

## Completed Tasks

Nach erfolgreichem Abschluss eines Intents dürfen dessen Recovery-Checkpoints reduziert oder gelöscht werden.

Erforderliche Informationen für:

- Undo
- Causality
- Evidence
- StateTime

dürfen dadurch nicht unbeabsichtigt verloren gehen.

Diese Systeme verwalten ihre eigene Retention.

## Datenschutz

Checkpoints können sensible temporäre Daten enthalten.

Retention muss daher auch:

- Datenklassifikation
- Verschlüsselung
- Löschanforderungen
- Information-Flow-Policies

berücksichtigen.

Ein abgelaufener Checkpoint darf nicht allein wegen Recovery-Komfort unbegrenzt erhalten bleiben.

## Beispiel

```text
RetentionPolicy {
    max_count:
        6

    max_storage:
        1GiB

    minimum_recovery_points:
        2

    consolidation:
        after_4_deltas

    max_age:
        12h
}
```

Vorhanden:

```text
Full A
Delta B
Delta C
Delta D
Delta E
Delta F
```

NovaOS kann:

```text
A + B + C + D
    ↓ consolidate
Full D
```

und nicht mehr benötigte ältere Zustände entfernen.

## Normative Anforderungen

1. NovaOS MUSS Ressourcen- und Retention-Grenzen für MicroCheckpoints verwalten können.
2. Abhängige Delta-Checkpoints DÜRFEN ihre erforderlichen Basiszustände nicht verlieren.
3. Lange Checkpoint-Ketten MÜSSEN konsolidierbar sein.
4. Für aktive wiederaufnehmbare Aufgaben SOLL mindestens ein gültiger Recovery Point erhalten bleiben.
5. Notwendige Checkpoints DÜRFEN bei Ressourcenknappheit nicht stillschweigend entfernt werden.
6. Retention MUSS geschützte, gepinnte und irreversible Übergänge berücksichtigen können.
7. Checkpoint-Frequenz DARF an Wiederholungskosten und Ressourcenlage angepasst werden.
8. Datenschutz- und Information-Flow-Regeln MÜSSEN auch für gespeicherte Checkpoints gelten.

## Abgrenzung

Diese NPSPEC definiert:

- Ressourcenbudgets
- Retention
- Checkpoint-Klassen
- Konsolidierung
- automatische Bereinigung

Nicht Bestandteil sind:

- Checkpoint-Inhalt
- Safe Points
- Crash-Recovery-Ablauf
- konkrete Storage-Implementierung
- Migration

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`
- `NPSPEC-CHECKPOINT-0005 – Crash Recovery`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`