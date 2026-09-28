# NPSPEC-CAUSAL-0002 – Causal Event Records

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Ereigniseinträge von `Nova.Causality`.

Ein `Causal Event Record` beschreibt eine konkrete, kausal relevante Aktion oder Zustandsänderung, aus der später der Causality Graph aufgebaut oder erweitert werden kann.

Grundprinzip:

```text
Ereignis
    ↓
Causal Event Record
    ↓
Causality Graph
```

## Event Record

Ein Event Record besitzt mindestens:

```text
CausalEvent {
    id
    type
    timestamp
    actor
    intent
    operation
    inputs
    outputs
    metadata
}
```

Nicht jedes Feld muss bei jedem Event belegt sein.

## `id`

Jeder Event Record muss eindeutig identifizierbar sein.

Beispiel:

```text
causal:event:01K...
```

Die ID bleibt unverändert.

## `type`

`type` beschreibt die Art des Ereignisses.

Beispiele:

```text
object.created
object.modified
object.deleted
operation.started
operation.completed
operation.failed
intent.triggered
result.produced
dependency.used
```

Event-Typen müssen semantisch eindeutig definiert sein.

## `timestamp`

Jeder Event Record besitzt mindestens einen Zeitstempel.

Optional können mehrere Zeitpunkte verwendet werden:

```text
started_at
completed_at
recorded_at
```

Zeitstempel unterstützen die zeitliche Einordnung, erzeugen aber allein keine kausale Beziehung.

## `actor`

`actor` beschreibt, wodurch das Ereignis ausgelöst wurde.

Mögliche Akteure:

```text
user
system
intent
capability
service
device
```

Beispiel:

```text
actor:
    user:42
```

oder:

```text
actor:
    capability:nova.speech.local
```

## Intent-Bezug

Ein Event darf auf den auslösenden Intent verweisen.

Beispiel:

```text
intent:
    intent:01JQX7...
```

Dadurch bleibt nachvollziehbar, zu welcher Nutzerabsicht das Ereignis gehört.

## Operation-Bezug

Wenn das Ereignis durch eine konkrete Operation entstand, muss diese referenzierbar sein.

Beispiel:

```text
operation:
    causal:operation:4412
```

Mehrere Events dürfen zu derselben Operation gehören.

Beispiel:

```text
operation.started
operation.completed
```

## Inputs und Outputs

Ein Event darf kausal relevante Eingaben und Ergebnisse referenzieren.

Beispiel:

```text
inputs {
    object:audio.raw
}

outputs {
    object:audio.cleaned
}
```

Mehrere Inputs und Outputs sind zulässig.

Die Referenzen müssen eindeutig und semantisch prüfbar sein.

## Event-Typen

Mindestens folgende Ereignisklassen sollen unterstützt werden:

```text
CREATE
READ
MODIFY
TRANSFORM
DELETE
EXECUTE
PRODUCE
FAIL
TRIGGER
LINK
UNLINK
```

Die konkrete Event-Taxonomie darf erweitert werden.

## Erfolgszustand

Ausführungsbezogene Events können einen Status besitzen.

Beispiel:

```text
status:
    success
```

oder:

```text
status:
    failed
```

Bei Fehlern kann eine Fehlerreferenz angegeben werden.

```text
failure:
    error:decoder_unavailable
```

## Metadaten

Zusätzliche technische Informationen dürfen in `metadata` gespeichert werden.

Beispiele:

```text
capability_version
host
execution_attempt
transaction_id
checkpoint_id
```

Metadaten dürfen nicht die eigentliche kausale Semantik ersetzen.

## Unveränderlichkeit

Ein bestätigter Causal Event Record soll unveränderlich sein.

Fehlerhafte oder ergänzte Informationen werden durch:

- neue Events
- Korrektur-Records
- Versionierungsinformationen

dargestellt.

Historische Events dürfen nicht stillschweigend überschrieben werden.

## Reihenfolge

Events können eine logische Reihenfolge besitzen.

Beispiel:

```text
sequence:
    18372
```

Diese kann verwendet werden, wenn Zeitstempel allein keine eindeutige Reihenfolge garantieren.

Eine Event-Reihenfolge darf nicht automatisch als Kausalität interpretiert werden.

## Atomare Erfassung

Ein Event Record muss entweder vollständig gültig oder nicht sichtbar sein.

Teilweise geschriebene Events dürfen nicht in den Causality Graph übernommen werden.

## Beispiel

```text
CausalEvent {
    id: causal:event:10042

    type:
        result.produced

    timestamp:
        2026-09-28T12:30:00Z

    actor:
        capability:nova.speech.local

    intent:
        intent:01JQX7M8A4

    operation:
        causal:operation:4412

    inputs {
        object:audio.cleaned
    }

    outputs {
        object:transcript
    }

    metadata {
        capability_version: 2.1
        execution_attempt: 1
    }
}
```

Daraus kann Nova.Causality beispielsweise folgende Beziehungen erzeugen:

```text
audio.cleaned
    ↓ USED
speech_recognition
    ↓ PRODUCED
transcript
```

## Persistenz

Causal Event Records müssen persistent speicherbar sein.

Die Speicherung darf:

- append-only
- journaling-basiert
- transaktional

implementiert werden.

Die konkrete Speicherform wird separat spezifiziert.

## Normative Anforderungen

1. Jeder Causal Event Record MUSS eindeutig identifizierbar sein.
2. Jedes Event MUSS einen definierten Event-Typ besitzen.
3. Kausal relevante Inputs und Outputs MÜSSEN eindeutig referenzierbar sein.
4. Event Records MÜSSEN atomar gespeichert werden.
5. Bestätigte Event Records DÜRFEN nicht stillschweigend verändert werden.
6. Zeitliche Reihenfolge DARF nicht automatisch als Kausalität interpretiert werden.
7. Events MÜSSEN persistent speicherbar sein.
8. Event-Typen MÜSSEN erweiterbar bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- Aufbau eines Causal Event Records
- grundlegende Event-Typen
- Referenzen auf Inputs, Outputs, Intents und Operationen
- Persistenzanforderungen

Nicht Bestandteil sind:

- Lineage-Berechnung
- Causality Propagation
- Query-Schnittstellen
- Graph-Compaction
- Evidence-Bundles

## Zugehörige NPSPECs

- `NPSPEC-CAUSAL-0001 – Causality Graph Model`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`
- `NPSPEC-CAUSAL-0004 – Causality Propagation`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`
- `NPSPEC-CAUSAL-0006 – Causal Integrity & Graph Compaction`