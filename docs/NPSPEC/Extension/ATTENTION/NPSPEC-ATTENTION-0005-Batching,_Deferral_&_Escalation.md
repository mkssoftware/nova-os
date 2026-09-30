# NPSPEC-ATTENTION-0005 – Batching, Deferral & Escalation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Attention Requests bündelt, verschiebt und bei steigender Relevanz eskaliert.

Ziel ist, unnötige Unterbrechungen zu vermeiden, ohne wichtige oder zeitkritische Ereignisse zu verlieren.

## Grundprinzip

```text
Attention Requests
    ↓
Batching / Deferral
    ↓
Re-Evaluation
    ↓
Escalation falls erforderlich
    ↓
Presentation
```

## Batching

Mehrere semantisch zusammengehörige Requests dürfen zu einer gemeinsamen Meldung zusammengefasst werden.

Beispiel:

```text
Download A abgeschlossen
Download B abgeschlossen
Download C abgeschlossen
```

wird zu:

```text
3 Downloads abgeschlossen
```

Batching kann unter anderem nach folgenden Kriterien erfolgen:

```text
source
type
intent
workspace
time_window
semantic_relation
```

Requests unterschiedlicher Bedeutung dürfen nicht so zusammengefasst werden, dass wichtige Einzelereignisse verborgen werden.

## Batch Object

Eine Bündelung kann logisch beschrieben werden als:

```text
AttentionBatch {
    id
    requests[]
    aggregate_type
    highest_priority
    earliest_deadline
    created_at
}
```

Ein Batch muss die enthaltenen Einzel-Requests weiterhin referenzieren können.

## Deferral

Ein Request darf auf einen späteren Zeitpunkt verschoben werden, wenn keine unmittelbare Reaktion notwendig ist.

Mögliche Deferral-Ziele:

```text
after_current_intent
after_focus_mode
when_idle
at_deadline
specific_time
next_attention_window
```

Beispiel:

```text
NORMAL
+ focus_mode
+ no deadline
    ↓
DEFER until after_focus_mode
```

## Deferral Preconditions

Ein Request darf nur verschoben werden, wenn:

- seine Deadline nicht verletzt wird
- kein erheblicher Schaden entsteht
- keine zwingende sofortige Nutzerentscheidung erforderlich ist
- die zugrunde liegende Operation sicher warten kann

Kann dies nicht garantiert werden, darf der Request nicht weiter verzögert werden.

## Re-Evaluation

Aufgeschobene Requests müssen neu bewertet werden, wenn sich relevante Bedingungen ändern.

Beispiele:

```text
deadline approaches
priority changes
context changes
budget recovers
source state changes
user becomes idle
```

Ein Request darf dadurch:

```text
weiter verschoben
angezeigt
gebündelt
eskaliert
verworfen
```

werden.

## Escalation

Escalation erhöht die Präsentationsstärke eines Requests, wenn seine zeitliche oder sachliche Bedeutung zunimmt.

Beispiel:

```text
SILENT
    ↓
NOTIFY
    ↓
INTERRUPT
```

Escalation darf unter anderem ausgelöst werden durch:

```text
deadline proximity
repeated failure
growing damage risk
blocked critical intent
unacknowledged required action
```

## Escalation Levels

Eine mögliche logische Stufung lautet:

```text
LEVEL 0:
    SILENT

LEVEL 1:
    NOTIFY

LEVEL 2:
    PROMINENT_NOTIFY

LEVEL 3:
    INTERRUPT
```

Die konkrete UI-Darstellung ist nicht Bestandteil dieser Spezifikation.

## Keine künstliche Eskalation

Wiederholung allein darf nicht automatisch zu `CRITICAL` führen.

Beispiel:

```text
gleiche unwichtige Meldung × 100
```

bleibt fachlich unwichtig.

Escalation muss auf veränderter Dringlichkeit, Deadline oder Schadenswirkung beruhen.

## Acknowledgement

Ein Request kann eine Bestätigung verlangen.

Beispiel:

```text
requires_acknowledgement:
    true
```

Fehlt die Bestätigung, darf eine spätere Eskalation erfolgen, wenn dies semantisch gerechtfertigt ist.

Eine reine fehlende Interaktion darf nicht automatisch zu maximaler Eskalation führen.

## Expiration

Attention Requests dürfen verfallen.

Beispiel:

```text
expires_at:
    18:00
```

Nach Ablauf kann der Request:

```text
expire
archive
replace_with_summary
```

Ein veralteter Request darf nicht später als aktuell präsentiert werden.

## Batch-Auflösung

Ein bestehender Batch kann wieder aufgelöst werden, wenn ein enthaltenes Ereignis wesentlich wichtiger wird.

Beispiel:

```text
Batch:
    5 Storage Events

eines davon:
    imminent_data_loss
```

Dann:

```text
Critical Event
    → separate INTERRUPT

remaining 4
    → stay batched
```

## Abhängigkeit vom Attention Budget

Batching und Deferral dürfen verwendet werden, wenn das Attention Budget knapp wird.

Beispiel:

```text
LOW Requests
    ↓
budget low
    ↓
BATCH / DEFER
```

`CRITICAL` Requests dürfen nicht allein wegen Budgetmangel zurückgehalten werden.

## Beispiel

Eingehende Requests:

```text
10:00 Update available
10:02 Backup completed
10:04 Sync completed
10:05 Storage warning
```

Während Focus Mode:

```text
Update available
Backup completed
Sync completed
    ↓
BATCH + DEFER

Storage warning
    ↓
DEFER
```

Später sinkt freier Speicher stark:

```text
Storage warning
    ↓
priority rises
    ↓
NOTIFY
```

Bei unmittelbar drohendem Datenverlust:

```text
NOTIFY
    ↓
INTERRUPT
```

## Normative Anforderungen

1. Semantisch ähnliche Attention Requests MÜSSEN bündelbar sein.
2. Ein Batch MUSS seine enthaltenen Einzel-Requests referenzieren können.
3. Deferral DARF keine Deadline oder kritische Sicherheitsanforderung verletzen.
4. Aufgeschobene Requests MÜSSEN bei relevanten Kontextänderungen neu bewertet werden.
5. Escalation MUSS semantisch begründet sein.
6. Wiederholung allein DARF keine künstliche Hochstufung auf `CRITICAL` verursachen.
7. Veraltete Requests MÜSSEN als abgelaufen behandelt werden können.
8. Kritisch gewordene Einzelereignisse MÜSSEN aus einem Batch herausgelöst werden können.

## Abgrenzung

Diese NPSPEC definiert:

- Batching
- Deferral
- Re-Evaluation
- Escalation
- Expiration

Nicht Bestandteil sind:

- Priority-Klassen selbst
- Attention Budget
- konkrete Notification-UI
- lernende Nutzeranpassung

## Zugehörige NPSPECs

- `NPSPEC-ATTENTION-0001 – Attention Resource Model`
- `NPSPEC-ATTENTION-0002 – Attention Priority Classes`
- `NPSPEC-ATTENTION-0003 – Attention Budget`
- `NPSPEC-ATTENTION-0004 – Interruption & Notification Policy`
- `NPSPEC-ATTENTION-0006 – Attention Feedback & Adaptation`