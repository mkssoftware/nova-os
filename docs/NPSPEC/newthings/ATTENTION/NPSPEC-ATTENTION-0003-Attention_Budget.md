# NPSPEC-ATTENTION-0003 – Attention Budget

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das `Attention Budget` von NovaOS.

Das Attention Budget begrenzt, wie viele und wie intensive Unterbrechungen ein Nutzer innerhalb eines Zeitraums erhalten soll.

Ziel ist, unnötige Aufmerksamkeitskosten zu reduzieren und wichtige Meldungen gegenüber weniger relevanten Ereignissen zu bevorzugen.

## Grundprinzip

```text
Attention Requests
    ↓
Attention Cost
    ↓
Budget Evaluation
    ↓
anzeigen / bündeln / verzögern / unterdrücken
```

Das Budget ist keine harte globale Obergrenze.

Kritische Ereignisse dürfen ein Budget überschreiten, wenn eine Verzögerung erheblichen Schaden verursachen würde.

## Budget Scope

Budgets dürfen für unterschiedliche Bereiche gelten.

Beispiele:

```text
user
session
workspace
intent
application
capability
time_window
```

Dadurch kann NovaOS verhindern, dass einzelne Quellen unverhältnismäßig viel Aufmerksamkeit beanspruchen.

## Attention Cost

Jeder Attention Request darf einen geschätzten Aufmerksamkeitsaufwand besitzen.

Beispiel:

```text
attention_cost:
    1
```

oder semantisch:

```text
MINIMAL
LOW
MEDIUM
HIGH
DECISION_REQUIRED
```

Der Cost kann unter anderem berücksichtigen:

- Unterbrechungsstärke
- notwendige Reaktion
- erwartete Entscheidungsdauer
- Kontextwechsel
- Wiederholungsfrequenz

## Budget Model

Ein Budget kann beispielsweise beschrieben werden als:

```text
AttentionBudget {
    scope
    window
    capacity
    consumed
    reserved
}
```

Beispiel:

```text
scope:
    user

window:
    30min

capacity:
    20
```

Die konkrete numerische Bewertung ist implementierungsabhängig.

## Verbrauch

Ein Budget wird nur durch tatsächlich präsentierte Aufmerksamkeit belastet.

Ein intern erzeugter, aber:

```text
batched
deferred
suppressed
```

Attention Request soll nicht automatisch denselben Cost verursachen wie eine tatsächliche Unterbrechung.

## Priorität

Priority Classes beeinflussen die Budgetbehandlung.

Beispiel:

```text
BACKGROUND
LOW
```

dürfen bei knappem Budget stark verzögert oder gebündelt werden.

```text
HIGH
```

erhält bevorzugten Zugriff.

```text
CRITICAL
```

darf das Budget überschreiten.

Ein Budget darf jedoch nicht zur künstlichen Hochstufung von Requests führen.

## Reserven

NovaOS darf einen Teil des Budgets für wichtige Ereignisse reservieren.

Beispiel:

```text
total:
    20

reserved_high_priority:
    5
```

Dadurch verhindern weniger wichtige Meldungen, dass das gesamte Budget verbraucht wird.

## Zeitfenster

Budgets müssen zeitlich begrenzt sein.

Beispiele:

```text
5 min
30 min
1 h
session
```

Verbrauch kann mit der Zeit:

```text
reset
decay
replenish
```

werden.

Das genaue Verfahren wird durch die Attention Policy bestimmt.

## Quellenbudgets

Einzelne Quellen dürfen eigene Teilbudgets besitzen.

Beispiel:

```text
Capability A:
    max_cost: 4 / 30min
```

Damit kann eine fehlerhafte oder aggressive Capability nicht unbegrenzt Benachrichtigungen erzeugen.

Systemkritische Quellen dürfen gesonderte Regeln erhalten.

## Kontextabhängigkeit

Budgets dürfen abhängig vom aktuellen Nutzungskontext angepasst werden.

Beispiele:

```text
presentation
focus_mode
active_call
gaming
critical_system_task
```

Im Focus Mode kann das reguläre Budget beispielsweise reduziert werden.

Kritische Meldungen bleiben davon unberührt.

## Budgetüberschreitung

Wenn das reguläre Budget ausgeschöpft ist, sind unter anderem folgende Reaktionen zulässig:

```text
DEFER
BATCH
SILENT
SUPPRESS_DUPLICATE
ESCALATE_LATER
```

Ein Request darf nicht ohne Policy-Grund dauerhaft verloren gehen.

## Wiederholte Requests

Wiederholungen desselben Ereignisses sollen nicht jedes Mal den vollen Attention Cost verursachen.

Beispiel:

```text
storage_warning
storage_warning
storage_warning
```

kann zu:

```text
storage_warning × 3
```

zusammengefasst werden.

Ändert sich die Dringlichkeit wesentlich, darf neu bewertet werden.

## Nutzersteuerung

Der Nutzer darf systemweite oder kontextbezogene Attention-Einstellungen beeinflussen.

Beispiele:

```text
mehr Ruhe
weniger Benachrichtigungen
Focus Mode
bestimmte Quelle bevorzugen
bestimmte Quelle stummschalten
```

Der Nutzer darf sicherheitskritische Systemmeldungen nicht zwingend vollständig deaktivieren, wenn NovaOS eine unmittelbare Warnung benötigt.

## Beispiel

```text
AttentionBudget {
    scope:
        user

    window:
        30min

    capacity:
        20

    consumed:
        16

    reserved:
        4
}
```

Neue Requests:

```text
LOW cost=3
HIGH cost=2
CRITICAL cost=5
```

Mögliche Entscheidung:

```text
LOW
    → defer

HIGH
    → present

CRITICAL
    → present even if budget exceeded
```

## Normative Anforderungen

1. NovaOS MUSS Attention Budgets für definierte Zeiträume verwalten können.
2. Attention Cost MUSS getrennt von Priority und Urgency modellierbar sein.
3. Kritische Ereignisse DÜRFEN ein reguläres Budget überschreiten.
4. Einzelne Quellen MÜSSEN durch Teilbudgets begrenzbar sein.
5. Gebündelte oder unterdrückte Duplikate SOLLEN nicht wie vollständige Einzelunterbrechungen bewertet werden.
6. Budgetüberschreitung MUSS zu einer definierten Policy-Reaktion führen.
7. Das Budget DARF sicherheitskritische Ereignisse nicht vollständig blockieren.
8. Budgetzustände MÜSSEN während der Laufzeit neu berechnet oder aufgefüllt werden können.

## Abgrenzung

Diese NPSPEC definiert:

- Attention Budgets
- Attention Cost
- Zeitfenster
- Quellenbudgets
- grundlegende Budgetüberschreitung

Nicht Bestandteil sind:

- konkrete Notification-Darstellung
- Batching-Algorithmen
- Escalation
- lernende Nutzeranpassung

## Zugehörige NPSPECs

- `NPSPEC-ATTENTION-0001 – Attention Resource Model`
- `NPSPEC-ATTENTION-0002 – Attention Priority Classes`
- `NPSPEC-ATTENTION-0004 – Interruption & Notification Policy`
- `NPSPEC-ATTENTION-0005 – Batching, Deferral & Escalation`
- `NPSPEC-ATTENTION-0006 – Attention Feedback & Adaptation`