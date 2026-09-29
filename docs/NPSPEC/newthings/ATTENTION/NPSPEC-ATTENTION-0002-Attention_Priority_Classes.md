# NPSPEC-ATTENTION-0002 – Attention Priority Classes

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Prioritätsklassen für `Nova.Attention`.

Sie bestimmen, wie wichtig ein Attention Request im Verhältnis zu anderen Anfragen ist.

Priorität beschreibt die Bedeutung einer Meldung.

Sie ist getrennt von:

- Dringlichkeit
- Deadline
- Unterbrechbarkeit
- Attention Cost

## Grundprinzip

```text
Attention Request
    ↓
Priority Class
    ↓
Attention Policy
    ↓
Darstellung / Verzögerung / Eskalation
```

## Prioritätsklassen

NovaOS verwendet folgende grundlegende Klassen:

```text
BACKGROUND
LOW
NORMAL
HIGH
CRITICAL
```

## `BACKGROUND`

Für Ereignisse ohne unmittelbaren Bedarf an Nutzeraufmerksamkeit.

Beispiele:

```text
Synchronisierung abgeschlossen
Index aktualisiert
Hintergrundtask beendet
```

Typisches Verhalten:

```text
silent
log
optional aggregation
```

## `LOW`

Für relevante, aber nicht wichtige Informationen.

Beispiele:

```text
Update verfügbar
optionale Empfehlung
nicht dringender Statushinweis
```

Solche Requests dürfen lange verzögert oder gebündelt werden.

## `NORMAL`

Standardklasse für reguläre Nutzerinformationen.

Beispiele:

```text
Download abgeschlossen
Aufgabe abgeschlossen
neue Nachricht
```

Die Meldung darf sichtbar werden, soll aber den aktuellen Arbeitsfluss grundsätzlich nicht unterbrechen.

## `HIGH`

Für wichtige Ereignisse, auf die der Nutzer zeitnah reagieren sollte.

Beispiele:

```text
Speicher fast voll
Task benötigt Entscheidung
wichtige Operation blockiert
```

`HIGH` bedeutet nicht automatisch sofortige Unterbrechung.

Dringlichkeit und Kontext bleiben zusätzlich relevant.

## `CRITICAL`

Für Ereignisse, bei denen eine verzögerte Reaktion erheblichen Schaden verursachen kann.

Beispiele:

```text
unmittelbarer Datenverlust droht
kritische Sicherheitsentscheidung erforderlich
Hardwarezustand gefährdet laufende Daten
```

`CRITICAL` darf eine unmittelbare Unterbrechung rechtfertigen.

Die Verwendung dieser Klasse muss restriktiv erfolgen.

## Prioritätsbestimmung

Die Priorität soll anhand semantischer Eigenschaften bestimmt werden.

Mögliche Faktoren:

```text
potential_damage
user_goal_impact
recoverability
time_sensitivity
security_impact
data_loss_risk
```

Eine Quelle darf nicht allein deshalb hohe Priorität erhalten, weil sie sich selbst als wichtig betrachtet.

## Priorität und Dringlichkeit

Beide Werte müssen getrennt behandelt werden.

Beispiel:

```text
priority:
    HIGH

urgency:
    LOW
```

Dies kann eine wichtige Aufgabe darstellen, die erst später Aufmerksamkeit benötigt.

Umgekehrt kann ein kurzfristiges Ereignis geringe fachliche Bedeutung besitzen.

## Priorität und Unterbrechbarkeit

Eine hohe Priorität erzwingt nicht automatisch `INTERRUPT`.

Beispiel:

```text
priority:
    HIGH

interruptibility:
    DEFERABLE
```

Erst die Attention Policy bestimmt das konkrete Verhalten.

## Prioritätsvererbung

Child-Requests dürfen die Priorität eines Parent-Requests berücksichtigen.

Sie dürfen die Priorität jedoch nicht automatisch erhöhen.

Beispiel:

```text
Parent:
    NORMAL

Child:
    HIGH
```

Eine Hochstufung muss durch einen eigenen semantischen Grund gerechtfertigt sein.

## Dynamische Neubewertung

Priorität darf sich ändern, wenn sich die Bedeutung des Ereignisses verändert.

Beispiel:

```text
Speicher frei:
    15 %

priority:
    NORMAL

später:
    2 %

priority:
    HIGH
```

oder:

```text
0.2 %

priority:
    CRITICAL
```

Die Neubewertung muss nachvollziehbar sein.

## Prioritätsmissbrauch

Capabilities dürfen nicht uneingeschränkt selbst `CRITICAL` setzen.

NovaOS darf:

- Prioritäten begrenzen
- Quellen herunterstufen
- ungerechtfertigte Eskalation blockieren
- systemweite Regeln anwenden

Damit wird verhindert, dass einzelne Komponenten dauerhaft Aufmerksamkeit erzwingen.

## Beispiel

```text
AttentionRequest {
    type:
        storage.capacity_warning

    priority:
        HIGH

    urgency:
        MEDIUM

    interruptibility:
        DEFERABLE
}
```

Ein anderes Ereignis:

```text
AttentionRequest {
    type:
        imminent_data_loss

    priority:
        CRITICAL

    urgency:
        IMMEDIATE

    interruptibility:
        INTERRUPT
}
```

## Normative Anforderungen

1. NovaOS MUSS mindestens `BACKGROUND`, `LOW`, `NORMAL`, `HIGH` und `CRITICAL` unterscheiden können.
2. Priorität MUSS getrennt von Dringlichkeit und Unterbrechbarkeit behandelt werden.
3. `CRITICAL` MUSS auf tatsächlich schwerwiegende Ereignisse begrenzt werden.
4. Capabilities DÜRFEN kritische Priorität nicht uneingeschränkt selbst erzwingen.
5. Prioritäten MÜSSEN während der Laufzeit neu bewertet werden können.
6. Eine Hochstufung MUSS semantisch begründbar sein.
7. Priorität allein DARF keine unmittelbare Nutzerunterbrechung garantieren.

## Abgrenzung

Diese NPSPEC definiert:

- Attention Priority Classes
- grundlegende Prioritätssemantik
- Neubewertung
- Schutz vor Prioritätsmissbrauch

Nicht Bestandteil sind:

- Attention Budgets
- konkrete Notification Policies
- Batching
- Deferral
- Escalation
- lernende Anpassung

## Zugehörige NPSPECs

- `NPSPEC-ATTENTION-0001 – Attention Resource Model`
- `NPSPEC-ATTENTION-0003 – Attention Budget`
- `NPSPEC-ATTENTION-0004 – Interruption & Notification Policy`
- `NPSPEC-ATTENTION-0005 – Batching, Deferral & Escalation`
- `NPSPEC-ATTENTION-0006 – Attention Feedback & Adaptation`