# NPSPEC-UNDO-0001 – Semantic Action Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das semantische Aktionsmodell für systemweites Undo in NovaOS.

Eine `Semantic Action` beschreibt eine fachlich verständliche Änderung am Systemzustand.

Beispiele:

```text
Datei umbenennen
Dokument bearbeiten
Einstellung ändern
Bild konvertieren
Capability installieren
Objekt verschieben
```

Undo arbeitet damit nicht nur auf Speicherblöcken oder Dateiversionen, sondern auf der Bedeutung einer ausgeführten Aktion.

## Grundprinzip

```text
Intent
    ↓
Semantic Action
    ↓
Execution
    ↓
State Change
    ↓
Undo Information
```

Eine Semantic Action verbindet:

- Nutzerabsicht
- ausgeführte Operation
- betroffene Objekte
- Zustandsänderung
- mögliche Gegenoperation

## Action Object

Die logische Grundstruktur lautet:

```text
SemanticAction {
    id
    type
    intent
    actor
    targets
    inputs
    effects
    reversibility
    compensation
    metadata
}
```

## `type`

`type` beschreibt die semantische Bedeutung der Aktion.

Beispiele:

```text
storage.file.rename
storage.object.move
document.content.edit
system.setting.change
media.image.convert
capability.install
```

Der Typ soll unabhängig von einer konkreten Anwendung oder Implementierung sein.

## Betroffene Objekte

`targets` referenziert die Objekte, deren Zustand durch die Aktion verändert wurde.

Beispiel:

```text
targets {
    object:report.md
}
```

Mehrere Ziele sind zulässig.

```text
targets {
    object:fileA
    object:fileB
}
```

## Inputs und Effects

`inputs` beschreibt relevante Ausgangsdaten.

`effects` beschreibt die semantisch erzeugten Änderungen.

Beispiel:

```text
type:
    storage.file.rename

inputs {
    old_name: report-old.md
}

effects {
    new_name: report.md
}
```

Undo muss aus diesen Informationen eine gültige Rücknahme ableiten können.

## Reversibility

Jede Aktion muss ihre Rücknehmbarkeit beschreiben können.

Mindestens:

```text
REVERSIBLE
COMPENSATABLE
CONDITIONAL
IRREVERSIBLE
```

### `REVERSIBLE`

Der vorherige Zustand kann direkt wiederhergestellt werden.

Beispiel:

```text
rename
move
setting.change
```

### `COMPENSATABLE`

Die ursprüngliche Aktion kann nicht technisch ungeschehen gemacht werden, aber eine semantische Gegenaktion existiert.

Beispiel:

```text
message.send
```

Mögliche Compensation:

```text
message.recall
```

sofern das Zielsystem dies unterstützt.

### `CONDITIONAL`

Undo ist nur unter bestimmten Bedingungen möglich.

Beispiel:

```text
object.delete
```

nur solange die Daten noch wiederherstellbar sind.

### `IRREVERSIBLE`

Es existiert keine sichere Rücknahme.

Beispiele können externe physische oder bereits dauerhaft wirksame Aktionen sein.

## Compensation Reference

Eine Aktion darf eine passende Gegenoperation referenzieren.

Beispiel:

```text
compensation {
    type: storage.file.rename
    old_name: report.md
    new_name: report-old.md
}
```

Die eigentliche Ausführung der Compensation wird separat spezifiziert.

## Action Boundaries

Eine Semantic Action muss klar definieren, welche Änderung zu ihr gehört.

Beispiel:

```text
"Dokument speichern"
```

darf intern aus mehreren technischen Operationen bestehen:

```text
temporary_write
metadata_update
atomic_replace
index_update
```

Für Undo kann dies trotzdem eine einzige semantische Aktion darstellen.

## Zusammengesetzte Aktionen

Eine Benutzeraktion darf mehrere Semantic Actions enthalten.

Beispiel:

```text
"Projekt umbenennen"
```

kann erzeugen:

```text
directory.rename
metadata.update
reference.update
shortcut.update
```

Diese Aktionen können später zu einer Undo-Transaktion gruppiert werden.

## Causality-Bezug

Semantic Actions sollen mit `Nova.Causality` verknüpft werden.

Beispiel:

```text
Intent
    ↓
Semantic Action
    ↓
Causal Operations
    ↓
Changed Objects
```

Dadurch kann NovaOS nachvollziehen:

```text
Was wurde geändert?
Warum wurde es geändert?
Welche Aktion muss rückgängig gemacht werden?
```

## Zustandsabhängigkeit

Eine Undo-Aktion darf nur ausgeführt werden, wenn ihre Voraussetzungen noch gültig sind.

Beispiel:

```text
A.txt → B.txt
```

Ein späteres Undo darf nicht blind:

```text
B.txt → A.txt
```

ausführen, wenn inzwischen bereits eine andere Datei `A.txt` existiert.

Solche Fälle müssen als Konflikt behandelt werden.

## Persistenz

Semantic Actions müssen persistent gespeichert werden können.

Mindestens:

```text
action_id
type
targets
effects
reversibility
compensation_reference
timestamp
intent_reference
```

Dadurch kann systemweites Undo auch über Prozessgrenzen hinweg funktionieren.

## Beispiel

```text
SemanticAction {
    id: action:1042

    type:
        storage.file.rename

    intent:
        intent:771

    actor:
        user:42

    targets {
        object:file:12
    }

    inputs {
        old_name: notes.txt
    }

    effects {
        new_name: archive.txt
    }

    reversibility:
        REVERSIBLE

    compensation {
        type: storage.file.rename
        old_name: archive.txt
        new_name: notes.txt
    }
}
```

## Normative Anforderungen

1. Jede undo-fähige Änderung MUSS als semantisch identifizierbare Aktion darstellbar sein.
2. Semantic Actions MÜSSEN von konkreten Prozessen und Anwendungen unabhängig beschreibbar sein.
3. Betroffene Objekte und relevante Zustandsänderungen MÜSSEN referenzierbar sein.
4. Jede Aktion MUSS ihre Rücknehmbarkeit klassifizieren können.
5. Irreversible Aktionen DÜRFEN nicht als sicher rücknehmbar dargestellt werden.
6. Undo-Voraussetzungen MÜSSEN vor einer Rücknahme erneut geprüft werden.
7. Semantic Actions MÜSSEN persistent gespeichert werden können.
8. Mehrere technische Operationen DÜRFEN zu einer gemeinsamen semantischen Aktion zusammengefasst werden.

## Abgrenzung

Diese NPSPEC definiert:

- Semantic Actions
- Action-Typen
- Reversibility
- Effects
- grundlegende Compensation-Referenzen

Nicht Bestandteil sind:

- konkrete Compensation-Ausführung
- Undo-Transaktionsgrenzen
- Cross-Capability-Koordination
- Konfliktauflösung
- Behandlung irreversibler Aktionen

## Zugehörige NPSPECs

- `NPSPEC-UNDO-0002 – Compensation Operations`
- `NPSPEC-UNDO-0003 – Undo Transaction Boundaries`
- `NPSPEC-UNDO-0004 – Cross-Capability Undo Coordination`
- `NPSPEC-UNDO-0005 – Undo Conflict Detection & Resolution`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`
- `NPSPEC-CAUSAL-0001 – Causality Graph Model`