# NPSPEC-EVIDENCE-0002 – Source & Provenance References

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Quellen und Herkunftsinformationen innerhalb von `Nova.Evidence` referenziert werden.

Ziel ist, nachvollziehen zu können, welche Informationen zu einem Ergebnis beigetragen haben und woher diese stammen.

## Grundprinzip

```text
Source
    ↓
Processing
    ↓
Intermediate Data
    ↓
Result
```

Jede relevante Stufe muss über stabile Referenzen nachvollziehbar sein können.

## Source Reference

Eine Quelle kann logisch beschrieben werden als:

```text
SourceReference {
    id
    type
    origin
    version
    integrity
}
```

Beispiele:

```text
object:document:42
dataset:weather:2026
sensor:temperature:3
```

## Provenance Reference

Provenance beschreibt die Herkunft und Verarbeitungsgeschichte eines Ergebnisses.

Beispiel:

```text
Dataset
    ↓
Filter
    ↓
Analysis
    ↓
Report
```

Das Evidence Bundle darf dafür auf bestehende Causality- und Lineage-Daten verweisen.

## Stabile Referenzen

Evidence darf nicht von kurzlebigen Kennungen abhängen.

Nicht geeignet:

```text
pointer
process_id
temporary_handle
```

Bevorzugt:

```text
object_id
content_id
versioned_resource
causal_event_id
```

## Versionen

Veränderbare Quellen müssen mit ihrem tatsächlich verwendeten Zustand referenziert werden können.

Beispiel:

```text
source:
    object:dataset:17

version:
    42
```

## Externe Quellen

Externe Quellen müssen ebenfalls eindeutig referenzierbar sein.

Soweit möglich sollen gespeichert werden:

```text
origin
version
retrieval_time
integrity
```

## Abgeleitete Daten

Zwischenergebnisse dürfen selbst zu Quellen werden.

```text
Source A
    ↓
Intermediate B
    ↓
Result C
```

Dabei muss die Verbindung zur ursprünglichen Provenance erhalten bleiben.

## Beispiel

```text
EvidenceBundle {
    result:
        object:report:81

    sources {
        object:dataset:17@42
        object:document:31@7
    }

    provenance:
        causal_graph:991
}
```

## Normative Anforderungen

1. Evidence Bundles MÜSSEN verwendete Quellen stabil referenzieren können.
2. Veränderbare Quellen MÜSSEN mit ihrem tatsächlich verwendeten Zustand oder ihrer Version identifizierbar sein.
3. Kurzlebige Prozess- oder Speicherkennungen DÜRFEN nicht als alleinige Source Reference verwendet werden.
4. Externe Quellen SOLLEN Herkunfts-, Versions- und Integritätsinformationen enthalten.
5. Abgeleitete Daten MÜSSEN mit ihrer vorherigen Provenance verknüpfbar bleiben.
6. Source References MÜSSEN mit `Nova.Causality` und Object Lineage verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Source References
- Provenance References
- stabile Quellenidentität
- Versionsbezug

Nicht Bestandteil sind:

- Algorithmus- und Modellidentität
- Evidence-Signaturen
- Evidence Queries
- Causality Graph selbst

## Zugehörige NPSPECs

- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`
- `NPSPEC-EVIDENCE-0003 – Algorithm, Model & Tool Identity`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`
- `NPSPEC-EVIDENCE-0006 – Evidence Retention & Privacy`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`