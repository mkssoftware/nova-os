# NPSPEC-CAUSAL-0006 – Causal Integrity & Graph Compaction

## Status

Angenommen

## Zweck

Diese Spezifikation definiert:

- Integritätsanforderungen für den `Nova.Causality`-Graph
- Erkennung beschädigter oder widersprüchlicher Beziehungen
- kontrollierte Verdichtung großer Graphbereiche
- Erhaltung wesentlicher Provenienz trotz Compaction

Ziel ist ein langfristig konsistenter und beherrschbarer Causality Graph.

## Grundprinzip

```text
Causality Graph
    ↓
Integrity Validation
    ↓
gültiger Graph
    ↓
Compaction
    ↓
kompakter Graph
```

Compaction darf die wesentliche kausale Bedeutung nicht zerstören.

## Integritätsregeln

NovaOS muss mindestens folgende Fehler erkennen können:

```text
missing_node
invalid_reference
invalid_relation
duplicate_identity
broken_lineage
illegal_cycle
corrupted_event
version_conflict
```

Nicht jede zyklische Struktur ist automatisch ungültig.

Explizit modellierte Iterationen dürfen zulässig sein.

## Referenzintegrität

Jede Graphkante muss auf gültige oder bewusst als nicht mehr verfügbar markierte Nodes zeigen.

Beispiel:

```text
object:A
    DERIVED_FROM
object:B
```

Wurde `object:B` gelöscht, muss die Beziehung beispielsweise als:

```text
source_missing
```

erhalten bleiben können.

Eine fehlende Quelle darf nicht stillschweigend aus der Historie verschwinden.

## Konsistenz

Direkte Beziehungen müssen mit gespeicherten Causal Events vereinbar sein.

Beispiel:

```text
Operation X
    USED A
    PRODUCED B
```

darf nicht gleichzeitig ohne erklärbare Versionierung behaupten:

```text
B
    DERIVED_FROM C
```

wenn `C` an der Operation nicht beteiligt war.

Widersprüche müssen erkannt und markiert werden.

## Integritätsprüfung

Integritätsprüfungen dürfen:

- beim Schreiben
- inkrementell
- beim Laden
- periodisch
- bei Recovery

erfolgen.

Nicht jede Prüfung muss den vollständigen Graph traversieren.

## Beschädigte Bereiche

Beschädigte oder unvollständige Graphbereiche müssen kenntlich bleiben.

Beispiel:

```text
integrity:
    degraded
```

oder:

```text
lineage:
    incomplete
```

NovaOS darf einen beschädigten Graphbereich nicht als vollständig darstellen.

## Graph Compaction

Compaction reduziert Speicher- und Traversierungsaufwand, ohne die wesentliche Herkunft zu verlieren.

Beispiel:

Vorher:

```text
A
↓
Operation 1
↓
B
↓
Operation 2
↓
C
↓
Operation 3
↓
D
```

Nach Compaction:

```text
A
↓
Compacted Segment
↓
D
```

Der kompaktierte Abschnitt muss weiterhin referenzierbar sein.

## Compaction-Ebenen

NovaOS darf unterschiedliche Detailstufen verwenden.

Beispiel:

```text
FULL
SUMMARY
ARCHIVED
```

### `FULL`

Alle relevanten Nodes und Events bleiben direkt verfügbar.

### `SUMMARY`

Technische Zwischenschritte werden zusammengefasst.

### `ARCHIVED`

Detailinformationen werden ausgelagert, bleiben aber referenzierbar.

## Erhaltungsregeln

Compaction darf nicht entfernen:

- ursprüngliche Quellen
- wesentliche Result-Lineage
- sicherheitsrelevante Ereignisse
- Audit-relevante Informationen
- Evidence-Referenzen
- notwendige Undo- oder StateTime-Bezüge

Rein technische Zwischenschritte dürfen zusammengefasst werden, wenn ihre Entfernung die kausale Bedeutung nicht verändert.

## Compaction Record

Eine Verdichtung muss nachvollziehbar sein.

Beispiel:

```text
CompactionRecord {
    id
    replaced_nodes
    summary_node
    timestamp
    policy
}
```

Dadurch bleibt erkennbar, dass ein Graphbereich verdichtet wurde.

## Query-Verhalten

Abfragen müssen erkennen können, ob ein Ergebnis aus einem kompaktierten Bereich stammt.

Beispiel:

```text
completeness:
    compacted
```

Falls archivierte Details verfügbar sind, darf die Query-Schnittstelle diese bei Bedarf nachladen.

## Wiederherstellung

Compaction muss mit Recovery vereinbar sein.

Wenn Detaildaten archiviert wurden, müssen Referenzen auf diese Archive erhalten bleiben.

Eine irreversible Löschung darf nur erfolgen, wenn Retention- und Datenschutzregeln dies erlauben.

## Datenschutz

Graph-Compaction darf zur gezielten Entfernung nicht mehr benötigter Detailinformationen verwendet werden.

Dabei müssen jedoch verbleibende Beziehungen korrekt darstellen, dass frühere Details entfernt wurden.

Beispiel:

```text
upstream_lineage:
    retained_summary
```

statt eine vollständige Herkunft vorzutäuschen.

## Beispiel

Vor Compaction:

```text
recording
    ↓ decode
pcm
    ↓ normalize
normalized_pcm
    ↓ noise_reduction
clean_audio
    ↓ speech_recognition
transcript
```

Nach Compaction:

```text
recording
    ↓ audio_preprocessing
clean_audio
    ↓ speech_recognition
transcript
```

`audio_preprocessing` repräsentiert die verdichteten Schritte:

```text
decode
normalize
noise_reduction
```

## Normative Anforderungen

1. Causal Graphs MÜSSEN auf ungültige Referenzen und widersprüchliche Beziehungen prüfbar sein.
2. Fehlende oder beschädigte Herkunft MUSS als solche erkennbar bleiben.
3. Compaction DARF wesentliche Provenienz nicht zerstören.
4. Verdichtete Graphbereiche MÜSSEN als verdichtet erkennbar sein.
5. Sicherheits-, Audit-, Evidence- und notwendige Recovery-Beziehungen DÜRFEN nicht unkontrolliert entfernt werden.
6. Compaction MUSS nachvollziehbar dokumentiert werden können.
7. Query-Ergebnisse MÜSSEN zwischen vollständiger und kompaktierter Herkunft unterscheiden können.
8. Archivierte Detailinformationen MÜSSEN weiterhin eindeutig referenzierbar bleiben, solange sie aufbewahrt werden.

## Abgrenzung

Diese NPSPEC definiert:

- Graph-Integrität
- Konsistenzprüfung
- beschädigte Graphbereiche
- Graph-Compaction
- Archivierung von Detailinformationen

Nicht Bestandteil sind:

- Causal Event Format
- Lineage-Erzeugung
- Query-Syntax
- konkrete Storage-Engine
- allgemeine Retention Policy

## Zugehörige NPSPECs

- `NPSPEC-CAUSAL-0001 – Causality Graph Model`
- `NPSPEC-CAUSAL-0002 – Causal Event Records`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`
- `NPSPEC-CAUSAL-0004 – Causality Propagation`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`