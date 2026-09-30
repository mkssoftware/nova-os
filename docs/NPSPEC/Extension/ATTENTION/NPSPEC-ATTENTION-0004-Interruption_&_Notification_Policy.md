# NPSPEC-ATTENTION-0004 – Interruption & Notification Policy

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS entscheidet, wann und auf welche Weise ein `Attention Request` dem Nutzer präsentiert wird.

Ziel ist, notwendige Informationen sichtbar zu machen, ohne den Arbeitsfluss unnötig zu unterbrechen.

## Grundprinzip

```text
Attention Request
    ↓
Priority
Urgency
Attention Cost
Context
Budget
    ↓
Attention Policy
    ↓
SILENT
NOTIFY
DEFER
BATCH
INTERRUPT
```

Die erzeugende Capability bestimmt nicht selbst die endgültige Darstellungsform.

## Policy Input

Die Entscheidung kann mindestens berücksichtigen:

```text
priority
urgency
interruptibility
deadline
attention_cost
attention_budget
user_context
source
current_intent
```

Zusätzliche System- und Sicherheitsrichtlinien dürfen einbezogen werden.

## Präsentationsmodi

NovaOS muss mindestens folgende Modi unterstützen:

```text
SILENT
NOTIFY
DEFER
BATCH
INTERRUPT
```

### `SILENT`

Keine unmittelbare sichtbare Unterbrechung.

Der Request kann beispielsweise im Notification Center oder Verlauf erscheinen.

### `NOTIFY`

Der Nutzer erhält eine sichtbare Information, ohne dass die aktuelle Tätigkeit blockiert wird.

### `DEFER`

Die Meldung wird auf einen geeigneteren Zeitpunkt verschoben.

### `BATCH`

Mehrere zusammengehörige Requests werden gemeinsam dargestellt.

### `INTERRUPT`

Die aktuelle Tätigkeit darf aktiv unterbrochen werden.

`INTERRUPT` ist auf Fälle zu begrenzen, bei denen eine zeitnahe Reaktion tatsächlich erforderlich ist.

## Kontext

Die Policy darf den aktuellen Nutzungskontext berücksichtigen.

Beispiele:

```text
focus_mode
presentation
fullscreen
active_call
active_intent
idle
lock_screen
```

Beispiel:

```text
NORMAL + presentation
    → DEFER

CRITICAL + presentation
    → INTERRUPT
```

Kontext darf die semantische Priorität eines Requests nicht verfälschen.

## Deadline

Ein aufgeschobener Request darf eine Deadline besitzen.

Beispiel:

```text
deadline:
    30min
```

Nähert sich die Deadline, darf die Präsentationsstrategie neu bewertet werden.

Beispiel:

```text
DEFER
    ↓ deadline approaches
NOTIFY
    ↓ deadline critical
INTERRUPT
```

Die konkrete Eskalation wird in `NPSPEC-ATTENTION-0005` definiert.

## Nutzerpräferenzen

Der Nutzer darf beeinflussen:

- welche Quellen sichtbar melden dürfen
- welche Kategorien stumm bleiben
- Focus- und Ruhemodi
- bevorzugte Darstellungsformen
- erlaubte Unterbrechungsstärke

Nutzerpräferenzen dürfen zwingende Sicherheits- und Systemanforderungen nicht unbemerkt außer Kraft setzen.

## Quellenkontrolle

Capabilities dürfen einen gewünschten Präsentationsmodus angeben.

Beispiel:

```text
preferred_mode:
    NOTIFY
```

Dieser Wert ist nur eine Anfrage.

Die zentrale Attention Policy darf ihn:

```text
upgrade
downgrade
defer
batch
reject
```

wenn systemweite Regeln dies erfordern.

## Deduplizierung

Mehrfach auftretende gleichartige Requests sollen nicht als identische Einzelmeldungen präsentiert werden.

Beispiel:

```text
Download failed
Download failed
Download failed
```

kann zu:

```text
3 Downloads fehlgeschlagen
```

zusammengefasst werden.

## Aktive Entscheidungen

Benötigt eine Operation zwingend eine Nutzerentscheidung, muss dies explizit gekennzeichnet sein.

Beispiel:

```text
requires_response:
    true
```

Die Policy darf die Anfrage verzögern, solange dadurch:

- keine Deadline verletzt wird
- kein Schaden entsteht
- die Operation sicher blockiert bleiben kann

## Sperrende Dialoge

Modale oder sperrende Dialoge sollen vermieden werden.

Sie sind nur zulässig, wenn die aktuelle Aktion ohne Entscheidung nicht sicher fortgesetzt werden kann.

Bevorzugt sind nicht-blockierende Interaktionen.

## Security Override

Sicherheitskritische Systemzustände dürfen normale Nutzer- oder Budgetregeln übersteuern.

Beispiel:

```text
imminent_data_loss
    → INTERRUPT
```

Ein Security Override muss begründbar und nachvollziehbar sein.

## Policy Result

Eine Entscheidung soll mindestens enthalten:

```text
AttentionDecision {
    mode
    reason
    effective_priority
    scheduled_time
}
```

Beispiel:

```text
mode:
    DEFER

reason:
    focus_mode_active

scheduled_time:
    after_current_intent
```

## Beispiel

Request:

```text
priority:
    HIGH

urgency:
    LOW

interruptibility:
    DEFERABLE

deadline:
    2h
```

Aktueller Kontext:

```text
focus_mode:
    true
```

Entscheidung:

```text
DEFER
```

Ein anderer Request:

```text
priority:
    CRITICAL

urgency:
    IMMEDIATE

type:
    imminent_data_loss
```

Entscheidung:

```text
INTERRUPT
```

## Normative Anforderungen

1. Die endgültige Präsentationsentscheidung MUSS durch eine zentrale Attention Policy bestimmbar sein.
2. Capabilities DÜRFEN eine Unterbrechung nicht eigenmächtig erzwingen.
3. NovaOS MUSS mindestens `SILENT`, `NOTIFY`, `DEFER`, `BATCH` und `INTERRUPT` unterstützen.
4. `INTERRUPT` MUSS auf tatsächlich zeitkritische oder sicherheitsrelevante Situationen begrenzt werden.
5. Nutzerkontext MUSS bei der Präsentationsentscheidung berücksichtigt werden können.
6. Wiederholte gleichartige Requests SOLLEN dedupliziert oder gebündelt werden.
7. Zwingende Nutzerentscheidungen MÜSSEN als solche erkennbar sein.
8. Policy-Overrides MÜSSEN begründbar und nachvollziehbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Unterbrechungsentscheidungen
- Präsentationsmodi
- Kontextauswertung
- grundlegende Notification Policy

Nicht Bestandteil sind:

- Attention Priority Classes
- Attention Budget
- konkrete Batching- und Eskalationsalgorithmen
- lernende Anpassung
- konkrete UI-Darstellung

## Zugehörige NPSPECs

- `NPSPEC-ATTENTION-0001 – Attention Resource Model`
- `NPSPEC-ATTENTION-0002 – Attention Priority Classes`
- `NPSPEC-ATTENTION-0003 – Attention Budget`
- `NPSPEC-ATTENTION-0005 – Batching, Deferral & Escalation`
- `NPSPEC-ATTENTION-0006 – Attention Feedback & Adaptation`