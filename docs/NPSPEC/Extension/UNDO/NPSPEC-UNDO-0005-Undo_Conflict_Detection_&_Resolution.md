# NPSPEC-UNDO-0005 – Undo Conflict Detection & Resolution

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Konflikte erkennt und behandelt, die beim systemweiten Undo entstehen.

Ein Konflikt liegt vor, wenn eine geplante Compensation nicht mehr sicher auf den aktuellen Systemzustand angewendet werden kann.

## Grundprinzip

```text
Undo Request
    ↓
Precondition Check
    ↓
Conflict Detection
    ↓
Resolution
    ↓
Compensation
```

NovaOS darf bei einem Konflikt keine potenziell zerstörerische Gegenoperation blind ausführen.

## Typische Konflikte

Mindestens folgende Konfliktarten müssen unterscheidbar sein:

```text
STATE_CHANGED
TARGET_MISSING
TARGET_REPLACED
NAME_COLLISION
DEPENDENCY_CHANGED
PERMISSION_CHANGED
POLICY_CHANGED
VERSION_DIVERGED
EXTERNAL_STATE_CHANGED
```

Beispiel:

```text
Original:
    A.txt → B.txt

Undo:
    B.txt → A.txt
```

Wenn inzwischen bereits eine neue `A.txt` existiert:

```text
NAME_COLLISION
```

## Konflikterkennung

Vor jeder Compensation müssen die relevanten Preconditions mit dem aktuellen Zustand verglichen werden.

Beispiel:

```text
Expected:
    current_name = B.txt

Actual:
    current_name = C.txt

Result:
    STATE_CHANGED
```

Die Prüfung soll semantisch erfolgen und nicht nur über rohe Speicherzustände.

## Versionskonflikte

Objekte sollen über Versionen oder vergleichbare Zustandsmarker überprüfbar sein.

Beispiel:

```text
expected_version: 12
current_version: 15
```

Das bedeutet nicht automatisch, dass Undo unmöglich ist.

NovaOS muss prüfen können, ob die zwischenzeitlichen Änderungen:

- unabhängig
- kompatibel
- überschneidend
- widersprüchlich

sind.

## Resolution Strategies

NovaOS muss mindestens folgende Strategien unterstützen können:

```text
ABORT
RETRY
REBASE
MERGE
ALTERNATIVE_COMPENSATION
USER_DECISION
```

### `ABORT`

Undo wird sicher abgebrochen.

### `RETRY`

Die Voraussetzungen werden erneut geprüft und die Compensation später erneut versucht.

### `REBASE`

Die ursprüngliche Compensation wird auf den aktuellen Zustand angepasst.

Beispiel:

```text
Original:
    value 10 → 20

Current:
    value 25
```

Statt blind auf `10` zurückzusetzen kann eine semantisch passende Delta-Korrektur möglich sein.

### `MERGE`

Die ursprüngliche Änderung und spätere Änderungen werden zusammengeführt.

Dies eignet sich insbesondere für strukturierte Daten oder Dokumente.

### `ALTERNATIVE_COMPENSATION`

Eine andere Gegenoperation stellt die gewünschte semantische Wirkung sicher.

### `USER_DECISION`

Wenn NovaOS keine sichere automatische Entscheidung treffen kann, wird eine Nutzerentscheidung benötigt.

## Automatische Resolution

Eine automatische Konfliktauflösung ist nur zulässig, wenn die semantische Wirkung eindeutig und sicher bestimmt werden kann.

Beispiel:

```text
Original:
    Datei von Ordner A nach B verschoben

Später:
    Datei in B umbenannt

Undo:
    Datei mit aktuellem Namen zurück nach A
```

Dies kann zulässig sein, wenn Objektidentität und Abhängigkeiten eindeutig erhalten geblieben sind.

## Nutzerentscheidung

Bei nicht sicher lösbaren Konflikten muss NovaOS die relevanten Unterschiede verständlich darstellen.

Beispiel:

```text
Das ursprüngliche Undo möchte:

report.md → draft.md

Der Name draft.md wird inzwischen von einer anderen Datei verwendet.
```

Mögliche Optionen können sein:

```text
anderen Namen wählen
bestehendes Objekt behalten
Undo abbrechen
manuell zusammenführen
```

NovaOS darf keine riskante Standardentscheidung vortäuschen.

## Cross-Capability-Konflikte

Ein Konflikt innerhalb einer Capability kann Auswirkungen auf die gesamte Undo Transaction haben.

Beispiel:

```text
Reference.restore → success
Metadata.restore  → conflict
Storage.restore   → pending
```

Der Undo Coordinator entscheidet abhängig von den Transaktionsregeln über:

```text
pause
continue_safe_parts
abort
partial
```

## Partial Undo

Kann ein Konflikt nicht aufgelöst werden, darf eine Transaktion teilweise rückgängig gemacht worden sein.

Dieser Zustand muss ausdrücklich sichtbar bleiben:

```text
PARTIAL
```

NovaOS muss dokumentieren:

- welche Actions kompensiert wurden
- welche offen sind
- welche Konflikte bestehen

## Causality

Konflikte und ihre Auflösung sollen mit `Nova.Causality` verknüpft werden.

Beispiel:

```text
Original Action
    ↓
Undo Attempt
    ↓
Conflict
    ↓
Resolution
    ↓
Compensation
```

Dadurch bleibt nachvollziehbar, warum der resultierende Zustand entstanden ist.

## Beispiel

Ausgangslage:

```text
Action:
    notes.txt → archive.txt
```

Spätere Änderung:

```text
neue Datei:
    notes.txt
```

Undo-Versuch:

```text
archive.txt → notes.txt
```

Konflikt:

```text
NAME_COLLISION
```

Mögliche Resolution:

```text
USER_DECISION

Option:
    archive.txt → notes-restored.txt
```

Die ursprüngliche Datei wird erhalten, ohne die neue `notes.txt` zu überschreiben.

## Normative Anforderungen

1. Preconditions MÜSSEN unmittelbar vor einer Compensation erneut geprüft werden.
2. Konflikte MÜSSEN eindeutig als solche erkennbar sein.
3. NovaOS DARF bei ungeklärten Konflikten keine destruktive Compensation blind ausführen.
4. Automatische Konfliktauflösung DARF nur bei eindeutig sicherer Semantik erfolgen.
5. Teilweise ausgeführtes Undo MUSS als `PARTIAL` erkennbar bleiben.
6. Versions- und Zustandsänderungen MÜSSEN bei der Konflikterkennung berücksichtigt werden können.
7. Konfliktauflösungen MÜSSEN nachvollziehbar bleiben.
8. Nicht automatisch lösbare Konflikte MÜSSEN eine kontrollierte Entscheidung ermöglichen.

## Abgrenzung

Diese NPSPEC definiert:

- Undo-Konflikterkennung
- Konfliktklassifikation
- Resolution-Strategien
- Partial Undo

Nicht Bestandteil sind:

- Definition von Semantic Actions
- Compensation-Implementierung
- Cross-Capability-Koordination
- Behandlung grundsätzlich irreversibler Aktionen

## Zugehörige NPSPECs

- `NPSPEC-UNDO-0001 – Semantic Action Model`
- `NPSPEC-UNDO-0002 – Compensation Operations`
- `NPSPEC-UNDO-0003 – Undo Transaction Boundaries`
- `NPSPEC-UNDO-0004 – Cross-Capability Undo Coordination`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`
- `NPSPEC-CAUSAL-0001 – Causality Graph Model`