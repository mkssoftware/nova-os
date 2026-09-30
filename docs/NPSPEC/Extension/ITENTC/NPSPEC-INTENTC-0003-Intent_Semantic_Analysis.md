# NPSPEC-INTENTC-0003 – Intent Semantic Analysis

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die semantische Analyse eines normalisierten Intents.

Ziel ist zu prüfen, ob Ziel, Eingaben, Ausgaben, Constraints und Policies logisch zusammenpassen, bevor der Intent in `Execution IR` überführt wird.

## Grundprinzip

```text
Canonical Intent
    ↓
Semantic Analysis
    ↓
VALID / INVALID / AMBIGUOUS
```

## Prüfbereiche

Die Analyse berücksichtigt mindestens:

```text
goal
semantic_types
inputs
outputs
constraints
policies
dependencies
```

## Typprüfung

Eingaben und erwartete Ausgaben müssen mit den Semantic Types des Intents vereinbar sein.

Beispiel:

```text
goal:
    media.audio.transcribe

input:
    Media.Audio

output:
    Document.Transcript
```

Ein inkompatibler Typ führt zu einem semantischen Fehler.

## Vollständigkeit

Alle für den Intent zwingend erforderlichen Informationen müssen vorhanden oder später eindeutig auflösbar sein.

Mögliche Ergebnisse:

```text
COMPLETE
INCOMPLETE
AMBIGUOUS
```

## Constraint-Prüfung

Widersprüchliche Anforderungen müssen erkannt werden.

Beispiel:

```text
local_only:
    true

remote_only:
    true
```

Ergebnis:

```text
CONSTRAINT_CONFLICT
```

## Policy-Prüfung

Ein Intent darf nicht erfolgreich analysiert werden, wenn seine Anforderungen bereits bekannten zwingenden Policies widersprechen.

Beispiel:

```text
Intent:
    upload external

Policy:
    network denied
```

Ergebnis:

```text
POLICY_CONFLICT
```

## Abhängigkeiten

Semantische Voraussetzungen müssen erkannt werden können.

Beispiel:

```text
goal:
    document.translate

requires:
    Document.Text
    Language.Source
    Language.Target
```

Fehlende Abhängigkeiten müssen als solche markiert werden.

## Analyseergebnis

Mindestens folgende Ergebnisse müssen unterstützt werden:

```text
VALID
INVALID
AMBIGUOUS
INCOMPLETE
```

Zusätzlich dürfen konkrete Fehlerursachen angegeben werden.

## Beispiel

```text
Intent {
    goal:
        media.audio.transcribe

    input:
        Media.Audio

    output:
        Document.Transcript

    constraints {
        local_only:
            true
    }
}
```

Analyse:

```text
Semantic Types:
    VALID

Constraints:
    VALID

Policies:
    VALID

Dependencies:
    COMPLETE
```

Ergebnis:

```text
VALID
```

## Normative Anforderungen

1. Jeder normalisierte Intent MUSS vor dem Lowering semantisch analysierbar sein.
2. Semantic Types von Inputs und Outputs MÜSSEN geprüft werden.
3. Fehlende oder mehrdeutige Anforderungen MÜSSEN erkennbar bleiben.
4. Widersprüchliche Constraints MÜSSEN als Konflikt erkannt werden.
5. Bekannte Policy-Konflikte MÜSSEN vor erfolgreichem Lowering erkannt werden.
6. Semantische Abhängigkeiten MÜSSEN identifizierbar sein.
7. Ein semantisch ungültiger Intent DARF nicht als gültige Execution IR ausgegeben werden.

## Abgrenzung

Diese NPSPEC definiert:

- semantische Intent-Prüfung
- Typprüfung
- Vollständigkeitsprüfung
- Constraint- und Policy-Konflikte

Nicht Bestandteil sind:

- Intent Normalization
- Capability-Auswahl
- Synthesis Planning
- Execution-IR-Struktur

## Zugehörige NPSPECs

- `NPSPEC-INTENTC-0001 – Intent Compiler Architecture`
- `NPSPEC-INTENTC-0002 – Intent Normalization`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-EXECIR-0001 – Execution IR Object Model`