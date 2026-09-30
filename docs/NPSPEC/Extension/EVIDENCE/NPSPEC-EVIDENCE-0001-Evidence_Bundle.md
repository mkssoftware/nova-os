# NPSPEC-EVIDENCE-0001 – Evidence Bundle

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das `Evidence Bundle` von NovaOS.

Ein Evidence Bundle fasst die wichtigsten Nachweise zu einem Ergebnis zusammen, damit nachvollziehbar bleibt, wie und unter welchen Bedingungen es entstanden ist.

## Grundprinzip

```text
Input
    ↓
Processing
    ↓
Result
    +
Evidence Bundle
```

Das Evidence Bundle gehört logisch zum erzeugten Ergebnis.

## Inhalt

Ein Bundle kann mindestens enthalten:

```text
EvidenceBundle {
    result
    sources
    provenance
    execution
    assumptions
    verification
}
```

Nicht jedes Feld muss für jedes Ergebnis vorhanden sein.

## Quellen

Verwendete Quellen müssen referenzierbar sein.

Beispiele:

```text
object:document:42
object:dataset:17
sensor:temperature:3
```

Dabei sollen stabile Referenzen statt kurzlebiger Prozess- oder Speicherkennungen verwendet werden.

## Herkunft

Das Bundle kann beschreiben, aus welchen Daten und Verarbeitungsschritten das Ergebnis hervorgegangen ist.

Beispiel:

```text
Dataset
    ↓
Filter
    ↓
Analysis
    ↓
Result
```

Die detaillierte Abstammung kann über `Nova.Causality` referenziert werden.

## Ausführung

Relevante Informationen zur Ausführung dürfen enthalten sein:

```text
capability
algorithm
model
version
execution_contract
precision
```

Dadurch kann später nachvollzogen werden, welche technische Verarbeitung verwendet wurde.

## Annahmen

Für das Ergebnis relevante Annahmen oder Einschränkungen sollen erfasst werden können.

Beispiel:

```text
assumption:
    input data complete

limitation:
    confidence < 100 %
```

## Verification

Das Bundle darf Prüfresultate referenzieren.

Beispiele:

```text
precision_verified
pipeline_verified
integrity_verified
```

Ein fehlender Nachweis darf nicht automatisch als erfolgreiche Verifikation interpretiert werden.

## Bundle Identity

Jedes Evidence Bundle muss eindeutig referenzierbar sein.

Beispiel:

```text
evidence:01K...
```

Ein Ergebnis darf mehrere Evidence Bundles besitzen, wenn unterschiedliche Nachweisbereiche getrennt geführt werden.

## Beispiel

```text
EvidenceBundle {
    result:
        object:analysis:81

    sources {
        object:dataset:17
    }

    execution {
        capability:
            nova.statistics.analyze

        version:
            3
    }

    verification {
        precision:
            PASS
    }
}
```

## Normative Anforderungen

1. Ergebnisse MÜSSEN ein Evidence Bundle referenzieren können.
2. Verwendete Quellen MÜSSEN stabil referenzierbar sein.
3. Relevante Verarbeitungsschritte und Ausführungsinformationen MÜSSEN dokumentierbar sein.
4. Annahmen und Einschränkungen SOLLEN im Bundle erfassbar sein.
5. Fehlende Verifikation DARF nicht als erfolgreiche Verifikation interpretiert werden.
6. Evidence Bundles MÜSSEN eindeutig identifizierbar sein.
7. Evidence-Daten MÜSSEN mit Causality- und Provenance-Informationen verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Evidence Bundle
- Quellenreferenzen
- Ausführungsnachweise
- Annahmen
- Verification References

Nicht Bestandteil sind:

- detaillierte Provenance-Struktur
- Algorithmus- und Modellidentität
- kryptografische Signaturen
- Evidence-Abfragen

## Zugehörige NPSPECs

- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`
- `NPSPEC-EVIDENCE-0003 – Algorithm, Model & Tool Identity`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`
- `NPSPEC-EVIDENCE-0006 – Evidence Retention & Privacy`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`