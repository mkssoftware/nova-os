# NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Prüfung eines durch `Nova.Synthesis` erzeugten Capability Composition Graph vor seiner Ausführung.

Ziel ist sicherzustellen, dass eine synthetisierte Pipeline semantisch korrekt, vollständig, zulässig und ausführbar ist.

## Grundprinzip

```text
Synthesized Pipeline
    ↓
Verification
    ↓
VALID / INVALID / CONDITIONAL
    ↓
Execution
```

Nur ausreichend verifizierte Pipelines dürfen ausgeführt werden.

## Prüfbereiche

Die Verifikation muss mindestens prüfen:

- semantische Typkompatibilität
- vollständige Inputs und Outputs
- Capability-Abhängigkeiten
- Constraints
- Policies
- Trust-Anforderungen
- Seiteneffekte
- Ressourcenanforderungen
- Graphstruktur

## Semantische Prüfung

Alle Verbindungen zwischen Nodes müssen typkompatibel sein.

Beispiel:

```text
Capability A:
    produces Media.Audio

Capability B:
    accepts Media.Audio
```

ist gültig.

Dagegen:

```text
Capability A:
    produces Document.Text

Capability B:
    accepts Media.Audio
```

ist ohne definierte Transformation ungültig.

## Graphprüfung

Der Composition Graph muss strukturell gültig sein.

Zu prüfen sind insbesondere:

```text
missing_nodes
broken_edges
unresolved_inputs
unreachable_outputs
illegal_cycles
invalid_dependencies
```

Explizit definierte Iterationen dürfen zulässig sein.

## Vollständigkeit

Alle zwingenden Inputs müssen bereitgestellt werden können.

Ebenso muss mindestens der vom Intent geforderte Output erreichbar sein.

Beispiel:

```text
Intent Output:
    Document.Transcript
```

Die Pipeline ist unvollständig, wenn kein gültiger Pfad diesen Typ erzeugt.

## Constraint Verification

Hard Constraints müssen für die gesamte Pipeline erfüllt sein.

Beispiele:

```text
local_only
max_memory
deadline
required_precision
deterministic
required_trust
```

Ein einzelner ungültiger Node kann den gesamten Graph ungültig machen.

## Policy Verification

Alle Nodes und Datenflüsse müssen mit den geltenden Policies vereinbar sein.

Beispiel:

```text
Policy:
    network: denied
```

Pipeline:

```text
LocalCapability
    ↓
RemoteService
```

Ergebnis:

```text
INVALID
```

## Seiteneffekte

Seiteneffekte müssen vollständig bekannt und zulässig sein.

Beispiele:

```text
filesystem_write
network_access
device_control
external_publish
```

Nicht deklarierte relevante Seiteneffekte machen die betroffene Capability für die verifizierte Pipeline unzulässig.

## Trust

Die Pipeline darf keine Capability verwenden, deren Trust-Level unter den Anforderungen des Intents liegt.

Beispiel:

```text
required_trust:
    verified
```

Eine:

```text
untrusted
```

Capability ist damit unzulässig.

## Verification Result

Die Prüfung muss mindestens folgende Ergebnisse unterscheiden:

```text
VALID
INVALID
CONDITIONAL
```

### `VALID`

Die Pipeline erfüllt alle bekannten zwingenden Anforderungen.

### `INVALID`

Mindestens eine zwingende Anforderung ist verletzt.

### `CONDITIONAL`

Die Pipeline ist grundsätzlich gültig, benötigt aber vor Ausführung noch eine Bedingung.

Beispiele:

```text
permission_required
resource_pending
device_required
user_authorization
```

## Verification Record

Das Ergebnis kann logisch beschrieben werden als:

```text
PipelineVerification {
    graph_id
    result
    checks
    assumptions
    failures
}
```

Damit bleibt nachvollziehbar, warum eine Pipeline akzeptiert oder verworfen wurde.

## Änderungen nach Verifikation

Wird der Graph nach erfolgreicher Prüfung verändert, verliert die bestehende Verifikation ihre Gültigkeit.

Beispiel:

```text
Graph v4
    ↓ VERIFIED

Graph changed
    ↓
Graph v5
    ↓
REVERIFY
```

Dasselbe gilt für relevante Änderungen an:

- Capability-Versionen
- Policies
- Constraints
- Semantic Types
- Trust-Informationen

## Laufzeitprüfung

Einige Annahmen dürfen erst unmittelbar vor oder während der Ausführung überprüft werden.

Beispiele:

```text
available_memory
device_available
network_state
runtime_permission
```

Wird eine zwingende Annahme ungültig, muss die Pipeline:

```text
pause
replan
fail
```

können.

## Beispiel

Pipeline:

```text
Media.Video
    ↓
VideoAudioExtractor
    ↓
Media.Audio
    ↓
SpeechRecognizer
    ↓
Document.Transcript
```

Prüfung:

```text
Semantic Types:
    PASS

Required Inputs:
    PASS

Output:
    PASS

Policy:
    local_only → PASS

Trust:
    PASS

Constraints:
    PASS
```

Ergebnis:

```text
VALID
```

## Normative Anforderungen

1. Jeder synthetisierte Composition Graph MUSS vor Ausführung verifizierbar sein.
2. Semantische Typverbindungen MÜSSEN vollständig geprüft werden.
3. Alle zwingenden Inputs und Outputs MÜSSEN auflösbar sein.
4. Hard Constraints und Policies MÜSSEN eingehalten werden.
5. Relevante Seiteneffekte MÜSSEN vor Ausführung bekannt sein.
6. NovaOS MUSS `VALID`, `INVALID` und `CONDITIONAL` unterscheiden können.
7. Änderungen am verifizierten Graph MÜSSEN eine erneute Prüfung auslösen.
8. Ungültig gewordene Laufzeitannahmen MÜSSEN Replanning oder Abbruch ermöglichen.

## Abgrenzung

Diese NPSPEC definiert:

- Verifikation synthetisierter Pipelines
- semantische und strukturelle Prüfung
- Constraint- und Policy-Prüfung
- Verification Results

Nicht Bestandteil sind:

- Synthesis Planning
- konkrete Optimierung
- Capability Isolation
- Composition Cache
- Scheduling

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`
- `NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition`
- `NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation`
- `NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`