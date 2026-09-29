# NPSPEC-ATTENTION-0001 – Attention Resource Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert menschliche Aufmerksamkeit als verwaltbare Systemressource in NovaOS.

Ziel ist, Unterbrechungen, Rückfragen und Benachrichtigungen nicht nur technisch, sondern nach ihrer Bedeutung für den Nutzer zu behandeln.

## Grundprinzip

```text
System Event
    ↓
Attention Request
    ↓
Priorisierung
    ↓
anzeigen / bündeln / verzögern / unterdrücken
```

NovaOS verwaltet damit neben Ressourcen wie:

```text
CPU
RAM
Energy
Bandwidth
```

auch:

```text
Attention
```

## Attention Request

Jede aufmerksamkeitsrelevante Aktion wird als `Attention Request` beschrieben.

```text
AttentionRequest {
    id
    source
    type
    priority
    urgency
    interruptibility
    deadline
    context
}
```

## Quellen

Attention Requests können unter anderem stammen von:

```text
Intent
Capability
System Service
Security Component
Application
User
Device
```

Die Quelle allein bestimmt nicht die Priorität.

## Priorität

Eine Anfrage besitzt eine semantische Priorität.

Beispiel:

```text
LOW
NORMAL
HIGH
CRITICAL
```

Die genaue Priorisierung wird separat spezifiziert.

`CRITICAL` ist nur für Ereignisse vorgesehen, bei denen eine Verzögerung erheblichen Schaden verursachen kann.

## Dringlichkeit

Priorität und Dringlichkeit sind getrennt.

Beispiel:

```text
priority:
    HIGH

deadline:
    30 min
```

Eine wichtige Meldung muss nicht zwingend sofort unterbrechen.

## Unterbrechbarkeit

Eine Anfrage beschreibt, wie stark sie den aktuellen Nutzerfluss unterbrechen darf.

Mindestens:

```text
SILENT
DEFERABLE
NOTIFY
INTERRUPT
```

Beispiel:

```text
SILENT
```

kann nur im Notification Center erscheinen.

```text
INTERRUPT
```

darf den Nutzer aktiv auf eine unmittelbar notwendige Entscheidung aufmerksam machen.

## Aufmerksamkeit als begrenzte Ressource

NovaOS soll wiederholte oder unnötige Unterbrechungen begrenzen können.

Beispiel:

```text
Viele ähnliche Events
    ↓
Aggregation
    ↓
eine zusammengefasste Meldung
```

Dadurch wird verhindert, dass technisch viele Ereignisse automatisch zu vielen Nutzerunterbrechungen führen.

## Kontext

Attention Requests dürfen Kontext berücksichtigen.

Beispiele:

```text
active_task
fullscreen
presentation
focus_mode
user_activity
current_intent
```

Die eigentliche Nutzerabsicht darf dabei nicht verändert werden.

Kontext beeinflusst nur, wann und wie die Anfrage präsentiert wird.

## Attention Ownership

Eine Capability darf Aufmerksamkeit anfordern, aber nicht eigenständig erzwingen.

Beispiel:

```text
Capability
    ↓ request
Attention Manager
    ↓ decision
User Interface
```

Die zentrale Attention-Komponente entscheidet anhand systemweiter Regeln über die Darstellung.

## Attention Cost

Eine Anfrage darf einen geschätzten Attention Cost besitzen.

Beispiel:

```text
attention_cost:
    minimal
```

oder:

```text
attention_cost:
    decision_required
```

Dies kann später für Budgets und Aggregation verwendet werden.

## Zustände

Ein Attention Request kann beispielsweise folgende Zustände besitzen:

```text
PENDING
DEFERRED
PRESENTED
ACKNOWLEDGED
DISMISSED
EXPIRED
```

Die konkrete Lifecycle-Definition wird in nachfolgenden NPSPECs erweitert.

## Beispiel

```text
AttentionRequest {
    id: attention:4711

    source:
        system.storage

    type:
        storage.capacity_warning

    priority:
        HIGH

    urgency:
        MEDIUM

    interruptibility:
        DEFERABLE

    deadline:
        2h

    context {
        related_intent:
            intent:project.build
    }
}
```

NovaOS kann die Meldung zurückstellen, solange keine unmittelbare Gefahr besteht.

## Sicherheitsereignisse

Sicherheitskritische Ereignisse dürfen höhere Attention-Anforderungen besitzen.

Beispiel:

```text
unauthorized_access_detected
```

Trotzdem muss auch hier zwischen:

```text
information
warning
required_decision
immediate_action
```

unterschieden werden.

Nicht jede Sicherheitsmeldung rechtfertigt eine unmittelbare Unterbrechung.

## Persistenz

Nicht abgeschlossene Attention Requests müssen über Neustarts hinweg erhalten bleiben können, sofern sie weiterhin relevant sind.

Veraltete oder bedeutungslose Requests dürfen verworfen werden.

## Normative Anforderungen

1. Aufmerksamkeit MUSS als eigenständige systemweite Ressource modellierbar sein.
2. Aufmerksamkeit anfordernde Komponenten DÜRFEN die Darstellung nicht eigenmächtig erzwingen.
3. Priorität, Dringlichkeit und Unterbrechbarkeit MÜSSEN getrennt beschreibbar sein.
4. Ähnliche Attention Requests MÜSSEN zusammenfassbar sein können.
5. Kontext DARF die Präsentation beeinflussen, aber nicht die eigentliche Semantik eines Events verändern.
6. Kritische Unterbrechungen MÜSSEN auf tatsächlich zeitkritische oder schadensträchtige Ereignisse begrenzt werden.
7. Noch relevante Attention Requests MÜSSEN persistent speicherbar sein.
8. Veraltete Requests MÜSSEN als solche erkennbar und entfernbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Attention als Ressource
- Attention Requests
- Priorität
- Dringlichkeit
- Unterbrechbarkeit
- grundlegenden Lifecycle

Nicht Bestandteil sind:

- konkrete Priority Classes
- Attention Budgets
- Notification Policies
- Batching und Escalation
- lernende Anpassung

## Zugehörige NPSPECs

- `NPSPEC-ATTENTION-0002 – Attention Priority Classes`
- `NPSPEC-ATTENTION-0003 – Attention Budget`
- `NPSPEC-ATTENTION-0004 – Interruption & Notification Policy`
- `NPSPEC-ATTENTION-0005 – Batching, Deferral & Escalation`
- `NPSPEC-ATTENTION-0006 – Attention Feedback & Adaptation`
