# NPSPEC-CAUSAL-0005 – Causal Query Interface

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Abfrageschnittstelle von `Nova.Causality`.

Sie ermöglicht es, Herkunft, Auswirkungen und kausale Beziehungen von Objekten, Intents und Operationen gezielt abzufragen.

Typische Fragen sind:

```text
Woher stammt dieses Objekt?
Was hat dieses Ergebnis erzeugt?
Welche Objekte wurden daraus abgeleitet?
Welche Operation hat diese Änderung verursacht?
Welche Intents hängen damit zusammen?
```

## Grundprinzip

```text
Query
    ↓
Causality Graph
    ↓
Filter / Traversierung
    ↓
Query Result
```

## Query-Ziele

Abfragen müssen mindestens auf folgende Entitäten möglich sein:

```text
Object
Result
Operation
Intent
Capability
Source
```

Beispiel:

```text
causal.query(object:transcript)
```

## Grundlegende Abfragen

Mindestens folgende Query-Arten müssen unterstützt werden:

```text
ANCESTORS
DESCENDANTS
DIRECT_CAUSES
DIRECT_EFFECTS
OPERATIONS
INTENTS
SOURCES
PATH
```

### `ANCESTORS`

Liefert die vorgelagerte Herkunft.

```text
ANCESTORS(object:summary)
```

Ergebnis:

```text
summary
← transcript
← audio.cleaned
← audio.raw
```

### `DESCENDANTS`

Liefert daraus entstandene Ergebnisse.

```text
DESCENDANTS(object:audio.raw)
```

### `DIRECT_CAUSES`

Liefert nur direkte Ursachen.

```text
DIRECT_CAUSES(object:transcript)
```

### `DIRECT_EFFECTS`

Liefert nur direkt daraus entstandene Ergebnisse.

### `OPERATIONS`

Liefert beteiligte Operationen.

```text
OPERATIONS(object:transcript)
```

### `INTENTS`

Liefert relevante auslösende oder übergeordnete Intents.

### `SOURCES`

Liefert ursprüngliche Quellen eines Objekts.

### `PATH`

Liefert einen kausalen Pfad zwischen zwei Entitäten.

```text
PATH(
    object:audio.raw,
    object:summary
)
```

## Traversierung

Abfragen müssen die maximale Traversierungstiefe begrenzen können.

Beispiel:

```text
depth: 3
```

Optional muss vollständige transitive Traversierung möglich sein.

```text
depth: unlimited
```

Unbegrenzte Traversierung darf durch Ressourcen- oder Sicherheitsrichtlinien eingeschränkt werden.

## Filter

Queries dürfen gefiltert werden.

Beispiele:

```text
type
time_range
operation_type
intent_id
capability_id
relation_type
```

Beispiel:

```text
ANCESTORS(object:summary) {
    relation: DERIVED_FROM
    depth: 5
}
```

## Query Result

Ein Ergebnis soll mindestens enthalten können:

```text
CausalQueryResult {
    nodes
    edges
    completeness
}
```

`completeness` beschreibt, ob die Antwort vollständig ist.

Mögliche Werte:

```text
complete
partial
restricted
compacted
```

Damit wird verhindert, dass fehlende oder verborgene Graphbereiche fälschlich als nicht existent interpretiert werden.

## Pfadabfragen

NovaOS muss einen oder mehrere kausale Pfade zwischen zwei Entitäten bestimmen können.

Beispiel:

```text
audio.raw
    ↓
noise_reduction
    ↓
audio.cleaned
    ↓
speech_recognition
    ↓
transcript
```

Bei mehreren möglichen Pfaden dürfen alle oder ein begrenzter Teil zurückgegeben werden.

## Berechtigungen

Query-Ergebnisse müssen bestehende Sicherheits- und Datenschutzregeln beachten.

Ein Nutzer darf durch eine Causality-Abfrage keine Informationen erhalten, auf die er sonst keinen Zugriff besitzt.

Statt vertrauliche Details offenzulegen, darf NovaOS beispielsweise liefern:

```text
restricted_node
```

Die Existenz einer Einschränkung soll erkennbar bleiben, sofern die Policy dies erlaubt.

## Performance

Queries dürfen:

- gecacht
- indexiert
- inkrementell berechnet

werden.

Der Cache darf keine veralteten Graphzustände als aktuell darstellen.

Große Abfragen müssen begrenzbar sein durch:

```text
depth
max_nodes
max_edges
time_range
```

## Beispiel

```text
query {
    target: object:summary
    direction: ancestors
    relation: DERIVED_FROM
    depth: unlimited
}
```

Ergebnis:

```text
summary
    ← transcript
    ← clean_audio
    ← recording
```

Mit Operationen:

```text
recording
    ↓ noise_reduction
clean_audio
    ↓ speech_recognition
transcript
    ↓ summarization
summary
```

## Normative Anforderungen

1. NovaOS MUSS direkte und transitive Causality-Abfragen unterstützen.
2. Vorwärts- und Rückwärts-Traversierung MÜSSEN möglich sein.
3. Abfragen MÜSSEN in Tiefe und Ergebnisgröße begrenzbar sein.
4. Query-Ergebnisse MÜSSEN unvollständige oder eingeschränkte Graphbereiche kenntlich machen.
5. Sicherheits- und Datenschutzregeln MÜSSEN auch für Causality-Abfragen gelten.
6. Pfade zwischen kausal verbundenen Entitäten MÜSSEN abfragbar sein.
7. Query-Caches DÜRFEN keine ungültigen oder veralteten Ergebnisse als aktuell ausgeben.

## Abgrenzung

Diese NPSPEC definiert:

- Causality-Abfragen
- Traversierung
- Filter
- Pfadabfragen
- Ergebnisgrenzen

Nicht Bestandteil sind:

- Speicherung des Graphen
- Event-Erzeugung
- Lineage-Propagation
- Graph-Compaction
- Benutzeroberfläche zur Visualisierung

## Zugehörige NPSPECs

- `NPSPEC-CAUSAL-0001 – Causality Graph Model`
- `NPSPEC-CAUSAL-0002 – Causal Event Records`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`
- `NPSPEC-CAUSAL-0004 – Causality Propagation`
- `NPSPEC-CAUSAL-0006 – Causal Integrity & Graph Compaction`