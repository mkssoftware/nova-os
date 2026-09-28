# NPSPEC-INTENT-0001 – Intent Object Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das grundlegende Objektmodell von `Nova.Intent`.

Ein Intent beschreibt, **welches Ergebnis erreicht werden soll**, ohne festzulegen, welches Programm, welche Capability, welcher Algorithmus oder welche Hardware dafür verwendet werden muss.

Die Nutzerabsicht wird damit zu einer eigenständigen, systemweit verständlichen und persistent referenzierbaren Einheit.

## Grundprinzip

```text
Nutzerabsicht
    ↓
Nova.Intent
    ↓
Intent Resolution
    ↓
Intent Compiler
    ↓
Execution IR
    ↓
Capabilities
    ↓
Ausführung
    ↓
Ergebnis
```

Ein Intent ist deklarativ.

Er beschreibt primär:

- Ziel
- Eingaben
- erwartetes Ergebnis
- Kontext
- Constraints
- Policies
- Metadaten

Ein Intent beschreibt grundsätzlich nicht:

- konkrete Anwendungen
- konkrete Prozesse
- konkrete Threads
- konkrete Hardware
- feste Algorithmen
- feste Ausführungsreihenfolgen

Diese Entscheidungen werden durch nachgelagerte NovaOS-Komponenten getroffen.

## Intent Object

Die logische Grundstruktur eines Intents lautet:

```text
Intent {
    id
    type
    goal
    inputs
    expected_output
    context
    constraints
    policies
    metadata
}
```

Nicht jedes optionale Feld muss bei jedem Intent belegt sein.

Die semantische Bedeutung der Felder bleibt jedoch systemweit einheitlich.

## `id`

`id` identifiziert eine konkrete Intent-Instanz eindeutig.

Anforderungen:

- systemweit eindeutig
- nach Erstellung unveränderlich
- persistent referenzierbar
- unabhängig vom ausführenden Prozess
- unabhängig von einer konkreten Capability

Beispiel:

```text
intent:01JQX7M8A4...
```

Das konkrete ID-Format wird separat spezifiziert.

## `type`

`type` beschreibt die semantische Klasse des Intents.

Beispiele:

```text
media.audio.transcribe
media.image.optimize
document.convert
document.summarize
data.analyze
project.build
system.repair
system.configure
```

Intent-Typen sind keine Programm-, Paket- oder Prozessnamen.

Mehrere unterschiedliche Capabilities dürfen denselben Intent-Typ erfüllen.

## `goal`

`goal` beschreibt das gewünschte Endergebnis.

Beispiel:

```text
goal {
    action: transcribe
    subject: audio
    output: document
}
```

Optional kann eine menschenlesbare Beschreibung ergänzt werden:

```text
description:
    "Erzeuge aus der Audioaufnahme ein durchsuchbares Transkript."
```

Maschinenrelevante Eigenschaften dürfen nicht ausschließlich als Freitext gespeichert werden.

## `inputs`

`inputs` beschreibt die für den Intent verfügbaren Eingangsdaten.

Eingaben werden über semantische Typen referenziert.

Beispiel:

```text
inputs {
    source {
        type: Media.Audio
        ref: object:9827...
    }
}
```

Mehrere Eingaben sind zulässig:

```text
inputs {
    audio: object:9827...
    vocabulary: object:1281...
    language_profile: object:7712...
}
```

Inputs können unter anderem referenzieren:

- Dateien
- NovaFile-Objekte
- Datenströme
- Geräte
- Sensoren
- Services
- andere Intents
- Ergebnisse vorheriger Intents
- Capabilities

Große Datenobjekte sollen referenziert und nicht unnötig in das Intent-Objekt kopiert werden.

## `expected_output`

`expected_output` beschreibt das gewünschte Ergebnis semantisch.

Beispiel:

```text
expected_output {
    type: Document.Transcript
    language: de
    timestamps: true
}
```

Das Ergebnis soll möglichst über semantische Typen und nicht über konkrete Anwendungen beschrieben werden.

Bevorzugt:

```text
Document.Transcript
```

statt:

```text
SpecificApplicationDocument
```

Ein konkretes Dateiformat darf verlangt werden, wenn es Bestandteil der Nutzerabsicht ist.

Beispiel:

```text
expected_output {
    type: Document.Transcript
    encoding: PDF
}
```

## `context`

`context` enthält Informationen, die für Interpretation, Planung oder Ausführung des Intents relevant sind.

Beispiel:

```text
context {
    user: user:42
    workspace: workspace:novaos
    active_task: task:183
    locale: de-DE
}
```

Kontext kann unter anderem enthalten:

- aktiven Nutzer
- Workspace
- TaskCapsule
- Benutzeroberflächenkontext
- Projekt
- Sprache
- Gerätekontext
- vorhergehende Intents
- aktuelle Arbeitsphase

Kontext ist von den eigentlichen Eingabedaten zu unterscheiden.

## `constraints`

`constraints` definiert Anforderungen, die bei der Ausführung eingehalten werden müssen.

Beispiel:

```text
constraints {
    deadline: 500ms
    max_memory: 256MiB
    deterministic: true
    local_only: true
}
```

Mögliche Constraints umfassen:

- Zeitlimit
- Deadline
- Speicherlimit
- Energieverbrauch
- Kosten
- Genauigkeit
- Präzision
- Determinismus
- Trust-Level
- erlaubte Hardware
- lokale Ausführung
- Offline-Ausführung

Constraints können in einen `Nova.ExecutionContract` überführt werden.

## `policies`

`policies` definiert Regeln für die zulässige Ausführung.

Beispiel:

```text
policies {
    network: denied
    remote_execution: denied
    persistent_storage: allowed
}
```

Policies können unter anderem Regeln für folgende Bereiche enthalten:

- Netzwerkzugriff
- Datenschutz
- Datenweitergabe
- Persistenz
- Remote-Ausführung
- Privilegien
- Informationsfluss
- Trust
- Benutzerinteraktion

Policies dürfen nicht durch eine ausführende Capability stillschweigend abgeschwächt werden.

## `metadata`

`metadata` enthält ergänzende Informationen über den Intent.

Beispiel:

```text
metadata {
    created_at: ...
    created_by: user:42
    parent_intent: intent:...
    origin: ui
}
```

Metadaten können unter anderem enthalten:

- Erstellungszeitpunkt
- Ersteller
- Ursprung
- Parent-Intent
- Task-Zugehörigkeit
- Debug-Informationen
- Version des Intent-Schemas

Metadaten dürfen die eigentliche Semantik des Intents nicht ersetzen.

## Intent-Hierarchie

Ein Intent darf weitere Intents erzeugen oder enthalten.

Beispiel:

```text
Intent: "Podcast veröffentlichen"

    ├── Audio bereinigen
    ├── Sprache transkribieren
    ├── Beschreibung erzeugen
    ├── Kapitel erkennen
    └── Export erzeugen
```

Parent- und Child-Beziehungen müssen explizit nachvollziehbar sein.

Diese Beziehungen bilden später einen Intent Graph.

## Intent-Identität

Die Identität eines Intents wird durch seine `id` bestimmt.

Zwei Intents mit identischem Ziel sind nicht automatisch derselbe Intent.

Beispiel:

```text
Intent A:
    "Bild optimieren"

Intent B:
    "Bild optimieren"
```

Beide besitzen unterschiedliche Identitäten und können unterschiedliche:

- Inputs
- Constraints
- Policies
- Kontexte
- Ergebnisse

besitzen.

## Immutabilität

Der semantische Kern eines bereits gestarteten Intents soll nicht unkontrolliert verändert werden.

Ändert sich die Nutzerabsicht wesentlich, soll daraus eine neue Intent-Version oder ein neuer Intent entstehen.

Beispiel:

```text
Intent A
    ↓ modified
Intent B
```

Die Beziehung zwischen beiden Intents bleibt nachvollziehbar.

## Referenzen

Intent-Objekte dürfen andere Systemobjekte referenzieren.

Referenzen müssen:

- eindeutig
- typisiert
- überprüfbar
- soweit erforderlich persistent

sein.

Eine Referenz auf ein Objekt darf nicht automatisch Eigentum an diesem Objekt übertragen.

## Serialisierung

Intent-Objekte müssen serialisierbar sein.

Die Serialisierung muss mindestens ermöglichen:

- Persistenz
- Wiederaufnahme
- IPC-Transport
- TaskCapsule-Integration
- Debugging
- Logging
- Versionsmigration

Die konkrete binäre oder textuelle Repräsentation wird separat spezifiziert.

## Erweiterbarkeit

Das Intent Object Model muss neue Intent-Typen und zusätzliche Felder ermöglichen, ohne bestehende Implementierungen unnötig zu brechen.

Unbekannte optionale Felder sollen ignoriert oder weitergereicht werden können, sofern dadurch keine Sicherheits- oder Semantikverletzung entsteht.

Pflichtfelder dürfen nur über versionierte Schemaänderungen verändert werden.

## Sicherheitsmodell

Ein Intent besitzt keine impliziten Rechte.

Ein Intent darf nur Operationen auslösen, die durch:

- Identität
- Capability-Rechte
- Policies
- Information-Flow-Regeln
- Execution Contracts
- Systemrichtlinien

zulässig sind.

Das Vorhandensein eines Intents stellt keine Autorisierung dar.

## Beziehung zu anderen NovaOS-Komponenten

Das Intent Object Model bildet die Grundlage für:

```text
Nova.Intent
Nova.Intent Compiler
Nova.Synthesis
Nova.ExecutionContract
Nova Execution IR
Nova.Causality
Nova.Evidence
Nova.InformationFlow
Nova.TaskCapsule
Nova.MicroCheckpoint
Nova.StateTime
```

Typischer Ablauf:

```text
Intent
    ↓
Semantic Validation
    ↓
Intent Resolution
    ↓
Intent Compiler
    ↓
Execution IR
    ↓
Capability Selection
    ↓
Execution Contract
    ↓
Execution
```

## Normative Anforderungen

1. Jeder Intent MUSS eine eindeutige `id` besitzen.
2. Jeder ausführbare Intent MUSS einen semantischen `type` besitzen.
3. Ein Intent MUSS das gewünschte Ziel beschreiben können, ohne eine konkrete Anwendung vorauszusetzen.
4. Eingaben und Ergebnisse SOLLEN über semantische Typen beschrieben werden.
5. Constraints und Policies MÜSSEN getrennt vom eigentlichen Ziel modellierbar sein.
6. Ein Intent DARF keine impliziten Berechtigungen erhalten.
7. Intent-Objekte MÜSSEN serialisierbar und persistent referenzierbar sein.
8. Parent-/Child-Beziehungen zwischen Intents MÜSSEN nachvollziehbar bleiben.
9. Wesentliche Änderungen eines laufenden Intents MÜSSEN versioniert oder als neuer Intent dargestellt werden.
10. Das Objektmodell MUSS erweiterbar und versionsfähig sein.

## Beispiel

```text
Intent {
    id: intent:01JQX7M8A4

    type:
        media.audio.transcribe

    goal {
        action: transcribe
        subject: audio
        output: document
    }

    inputs {
        source {
            type: Media.Audio
            ref: object:9827
        }
    }

    expected_output {
        type: Document.Transcript
        language: de
        timestamps: true
    }

    context {
        user: user:42
        workspace: workspace:podcast
    }

    constraints {
        local_only: true
        max_memory: 1GiB
    }

    policies {
        network: denied
        remote_execution: denied
    }

    metadata {
        origin: NovaShell
    }
}
```

NovaOS kann diesen Intent anschließend unabhängig davon erfüllen, welche konkrete Speech-to-Text-Engine oder Hardware verfügbar ist.

## Abgrenzung

Diese Spezifikation definiert ausschließlich das grundlegende Intent Object Model.

Nicht Bestandteil dieser NPSPEC sind:

- Intent-Lifecycle
- Intent-Schema-System
- Intent-Resolution
- Policy-Auflösung
- Intent-Komposition
- Persistenzstrategie
- Intent Compiler
- Execution IR

Diese werden in nachfolgenden NPSPECs spezifiziert.

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0002 – Intent Lifecycle`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-INTENT-0006 – Intent Composition`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`