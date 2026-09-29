# NPSPEC-ATTENTION-0006 – Attention Feedback & Adaptation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.Attention` aus dem tatsächlichen Nutzerverhalten lernt und seine zukünftige Behandlung von Attention Requests anpasst.

Ziel ist eine adaptive Attention-Steuerung, ohne Prioritäten, Sicherheitsregeln oder Nutzerentscheidungen unkontrolliert zu verändern.

## Grundprinzip

```text
Attention Decision
    ↓
Presentation
    ↓
User Reaction
    ↓
Feedback
    ↓
Prediction Error
    ↓
Adaptation
```

NovaOS vergleicht dabei seine erwartete Reaktion mit dem tatsächlichen Verhalten.

## Feedback

Feedback kann explizit oder implizit entstehen.

Explizite Beispiele:

```text
dismiss
snooze
mute
mark_important
change_priority
disable_source
```

Implizite Beispiele:

```text
immediately_opened
ignored
dismissed_quickly
acted_on_later
repeatedly_deferred
```

Ein einzelnes Verhalten darf nicht automatisch als dauerhafte Präferenz interpretiert werden.

## Feedback Record

Ein Feedback-Eintrag kann logisch enthalten:

```text
AttentionFeedback {
    request_id
    decision
    user_response
    response_delay
    context
    timestamp
}
```

Der Eintrag muss auf den ursprünglichen Attention Request referenzieren können.

## Prediction Error

NovaOS darf vorhersagen, welche Attention-Behandlung wahrscheinlich geeignet ist.

Beispiel:

```text
Prediction:
    NOTIFY

Actual:
    user immediately dismisses
```

Daraus entsteht ein Prediction Error.

Wiederholte Abweichungen dürfen zur Anpassung des Attention-Modells führen.

Grundmodell:

```text
Vorhersage
    ↓
Nutzerreaktion
    ↓
Abweichung
    ↓
Modellanpassung
```

## Adaptive Eigenschaften

Das System darf unter anderem lernen:

- bevorzugte Notification-Zeiten
- häufig ignorierte Quellen
- bevorzugte Deferral-Zeitpunkte
- sinnvolle Batch-Größen
- kontextabhängige Unterbrechbarkeit
- typische Reaktionszeiten

Nicht automatisch verändert werden dürfen:

- zwingende Sicherheitsregeln
- harte Policies
- technische Gefahrenbewertung
- ausdrücklich festgelegte Nutzerregeln

## Kontextabhängigkeit

Anpassungen sollen kontextbezogen erfolgen können.

Beispiel:

```text
work_context:
    defer social notifications

private_context:
    normal notification
```

Eine Präferenz aus einem Kontext darf nicht automatisch auf alle anderen Kontexte übertragen werden.

## Stabilität

NovaOS muss kurzfristige Ausnahmen von langfristigen Präferenzen unterscheiden können.

Beispiel:

```text
einmalige Ablehnung
    ≠
dauerhafte Präferenz
```

Anpassungen sollen deshalb mehrere Beobachtungen oder ausreichend starke explizite Signale berücksichtigen.

## Nutzerkontrolle

Gelernte Attention-Präferenzen müssen durch den Nutzer übersteuerbar sein.

Der Nutzer muss insbesondere:

```text
adaptation disable
adaptation reset
source preference change
explicit rule define
```

können.

Explizite Nutzerregeln haben Vorrang vor gelernten Präferenzen.

## Schutz vor Fehlanpassung

NovaOS darf aus fehlender Interaktion nicht automatisch schließen, dass ein Ereignis unwichtig ist.

Mögliche Gründe für fehlende Reaktion sind beispielsweise:

```text
user absent
device locked
presentation active
notification not noticed
task concentration
```

Der Kontext muss deshalb bei der Bewertung berücksichtigt werden.

## Kritische Ereignisse

Gelernte Präferenzen dürfen kritische Attention Requests nicht unzulässig unterdrücken.

Beispiel:

```text
user often dismisses storage warnings
```

darf nicht dazu führen, dass:

```text
imminent_data_loss
```

unterdrückt wird.

Systemische Sicherheitsgrenzen bleiben bestehen.

## Modelltransparenz

Adaptive Entscheidungen sollen begründbar sein.

Beispiel:

```text
Deferred because:
    similar requests were repeatedly postponed
    during focus sessions
```

NovaOS soll auf Anfrage erklären können, welche Präferenz eine Entscheidung beeinflusst hat.

## Datenschutz

Attention-Feedback kann sensibles Nutzungsverhalten offenbaren.

Daher sollen:

- nur notwendige Daten gespeichert werden
- Daten möglichst lokal verarbeitet werden
- Retention begrenzt sein
- Policies und Nutzerrechte gelten

Eine externe Übertragung darf nicht Voraussetzung für Attention Adaptation sein.

## Beispiel

Wiederholtes Verhalten:

```text
NORMAL notifications
during focus_mode

System:
    NOTIFY

User:
    repeatedly snoozes
```

Gelernte Anpassung:

```text
focus_mode
+ NORMAL
    ↓
DEFER
```

Ausnahme:

```text
CRITICAL
    ↓
INTERRUPT
```

Die Sicherheitssemantik bleibt unverändert.

## Normative Anforderungen

1. NovaOS MUSS explizites und implizites Attention-Feedback unterscheiden können.
2. Einzelne Nutzerreaktionen DÜRFEN nicht automatisch als dauerhafte Präferenz gelten.
3. Prediction Error SOLL als Lernsignal verwendet werden können.
4. Explizite Nutzerregeln MÜSSEN Vorrang vor gelernten Präferenzen haben.
5. Adaptive Regeln DÜRFEN harte Policies oder Sicherheitsanforderungen nicht abschwächen.
6. Kontext MUSS bei der Bewertung impliziten Feedbacks berücksichtigt werden können.
7. Gelernte Entscheidungen MÜSSEN übersteuerbar und zurücksetzbar sein.
8. Attention-Feedback SOLL möglichst lokal und datensparsam verarbeitet werden.

## Abgrenzung

Diese NPSPEC definiert:

- Attention Feedback
- Prediction Error
- adaptive Präferenzen
- Schutz vor Fehlanpassung
- Nutzerkontrolle

Nicht Bestandteil sind:

- konkrete ML-Algorithmen
- Priority-Klassen
- Attention Budget
- Notification UI
- allgemeine Nutzerprofilierung

## Zugehörige NPSPECs

- `NPSPEC-ATTENTION-0001 – Attention Resource Model`
- `NPSPEC-ATTENTION-0002 – Attention Priority Classes`
- `NPSPEC-ATTENTION-0003 – Attention Budget`
- `NPSPEC-ATTENTION-0004 – Interruption & Notification Policy`
- `NPSPEC-ATTENTION-0005 – Batching, Deferral & Escalation`