# NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie bereits erzeugte und verifizierte Capability-Kompositionen zwischengespeichert und erneut verwendet werden können.

Ziel ist, wiederkehrende Intents schneller auszuführen, ohne jedes Mal eine vollständige Synthesis-Planung durchführen zu müssen.

## Grundprinzip

```text
Intent
    ↓
Synthesis
    ↓
Verified Composition
    ↓
Composition Cache
    ↓
Reuse
```

Ein gecachter Composition Graph darf nur wiederverwendet werden, wenn seine Voraussetzungen weiterhin gültig sind.

## Cache Entry

Ein Cache-Eintrag kann logisch enthalten:

```text
CompositionCacheEntry {
    id
    intent_signature
    graph
    dependencies
    constraints
    policies
    verification
    environment_requirements
    version
}
```

## Intent Signature

Die Wiederverwendung darf nicht ausschließlich anhand des Intent-Namens erfolgen.

Die Signatur muss relevante Eigenschaften berücksichtigen können:

```text
intent_type
semantic_inputs
expected_outputs
constraints
policies
```

Nur ausreichend kompatible Intents dürfen denselben Composition Graph verwenden.

## Wiederverwendung

Vor der Wiederverwendung muss NovaOS prüfen, ob weiterhin gültig sind:

- Capability-Versionen
- Semantic Types
- Constraints
- Policies
- Trust-Anforderungen
- Hardwareanforderungen
- relevante Abhängigkeiten

Sind diese Bedingungen erfüllt, darf die erneute vollständige Synthesis entfallen.

## Cache Validity

Ein Cache-Eintrag kann mindestens folgende Zustände besitzen:

```text
VALID
STALE
INVALID
```

### `VALID`

Der Graph darf erneut verwendet werden.

### `STALE`

Der Graph könnte noch verwendbar sein, benötigt aber erneute Prüfung.

### `INVALID`

Der Graph darf nicht verwendet werden.

## Invalidierung

Ein Cache-Eintrag muss invalidierbar sein, wenn sich relevante Voraussetzungen ändern.

Beispiele:

```text
capability_updated
capability_removed
schema_changed
policy_changed
trust_changed
dependency_changed
graph_version_changed
```

Hardwareänderungen können ebenfalls eine erneute Planung notwendig machen.

## Teilweise Wiederverwendung

NovaOS darf auch Teilgraphen wiederverwenden.

Beispiel:

```text
Graph A:

Video
    ↓
ExtractAudio
    ↓
Audio
    ↓
SpeechRecognition
```

Ein späterer Intent benötigt:

```text
Video
    ↓
ExtractAudio
    ↓
Audio
    ↓
AudioAnalysis
```

Der bereits bekannte Teil:

```text
Video
    ↓
ExtractAudio
    ↓
Audio
```

darf wiederverwendet werden.

## Parametrisierte Graphen

Ein gecachter Graph darf Parameter enthalten.

Beispiel:

```text
Image.Resize {
    target_width
    target_height
}
```

Dadurch kann dieselbe Composition-Struktur für verschiedene konkrete Werte genutzt werden.

Parameter dürfen die semantische Gültigkeit des Graphen nicht verändern.

## Verification

Ein gecachter Graph muss nicht zwangsläufig vollständig neu verifiziert werden.

NovaOS darf eine schnellere Revalidierung durchführen, wenn:

- Graph unverändert ist
- Abhängigkeiten unverändert sind
- Policies kompatibel bleiben
- Verification Record weiterhin gültig ist

Ändert sich eine sicherheits- oder semantikrelevante Voraussetzung, muss eine vollständige Prüfung möglich sein.

## Optimierte Varianten

Für denselben Intent dürfen mehrere gecachte Varianten existieren.

Beispiel:

```text
Composition A:
    CPU optimized

Composition B:
    GPU optimized

Composition C:
    low energy
```

NovaOS kann abhängig vom aktuellen Systemzustand die passende gültige Variante auswählen.

## Keine Ergebnis-Caches

Der Composition Cache speichert primär den Lösungsweg.

Er ist von einem Ergebnis-Cache zu unterscheiden.

```text
Composition Cache:
    Wie wird die Aufgabe ausgeführt?

Result Cache:
    Was war das Ergebnis?
```

Beide Mechanismen dürfen unabhängig voneinander existieren.

## Sicherheit

Ein gecachter Graph darf keine früheren Berechtigungen als dauerhaft gültig behandeln.

Berechtigungen und relevante Policies müssen bei erneuter Ausführung erneut geprüft werden.

Beispiel:

```text
cached:
    network capability

current policy:
    network denied
```

Ergebnis:

```text
cache entry unusable
```

## Cache Scope

Cache-Einträge dürfen unterschiedliche Gültigkeitsbereiche besitzen.

Beispiele:

```text
system
user
workspace
task
session
```

Ein benutzerspezifischer Graph darf nicht automatisch für einen anderen Sicherheitskontext verwendet werden.

## Retention

Nicht mehr benötigte Cache-Einträge dürfen entfernt werden.

Mögliche Kriterien:

```text
age
usage_count
storage_cost
invalid_state
dependency_removed
```

Cache-Bereinigung darf keine laufenden Execution Graphs beschädigen.

## Beispiel

Erster Intent:

```text
Media.Video
    ↓
ExtractAudio
    ↓
SpeechRecognition
    ↓
Document.Transcript
```

Nach erfolgreicher Verification:

```text
Cache:
    composition:video_to_transcript
```

Später:

```text
Intent:
    Video → Transcript
```

NovaOS prüft:

```text
Capabilities:
    unchanged

Policies:
    compatible

Semantic Types:
    unchanged

Verification:
    valid
```

Ergebnis:

```text
REUSE CACHED COMPOSITION
```

Eine erneute vollständige Graphsuche ist nicht notwendig.

## Normative Anforderungen

1. Verifizierte Composition Graphs MÜSSEN zwischenspeicherbar sein.
2. Wiederverwendung MUSS relevante Intent-, Policy- und Constraint-Eigenschaften berücksichtigen.
3. Cache-Einträge MÜSSEN invalidierbar sein.
4. Veraltete oder ungültige Graphen DÜRFEN nicht ungeprüft ausgeführt werden.
5. Teilgraphen SOLLEN wiederverwendbar sein.
6. Mehrere optimierte Varianten desselben Lösungswegs MÜSSEN unterstützt werden können.
7. Gecachte Berechtigungen DÜRFEN nicht als dauerhaft gültige Autorisierung gelten.
8. Composition Cache und Result Cache MÜSSEN logisch getrennt behandelbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Composition Cache
- Wiederverwendung
- Invalidierung
- Teilgraph-Reuse
- parametrisierte und optimierte Varianten

Nicht Bestandteil sind:

- Result Caching
- eigentliche Synthesis-Planung
- Pipeline-Ausführung
- konkrete Cache-Speicherstrategie
- Scheduling

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`
- `NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition`
- `NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification`
- `NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation`
- `NPSPEC-INTENT-0004 – Intent Resolution`