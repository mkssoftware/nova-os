# NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Evidence-Daten gegen unbemerkte Veränderung geschützt und kryptografisch signiert werden können.

Ziel ist, Integrität, Herkunft und Unverändertheit eines Evidence Bundles prüfbar zu machen.

## Grundprinzip

```text
Evidence Bundle
    ↓
Integrity Protection
    ↓
Hash / Signature
    ↓
Verification
```

Eine gültige Signatur bestätigt Integrität und kryptografisch nachweisbare Herkunft, nicht automatisch die fachliche Richtigkeit des Ergebnisses.

## Integritätsprüfung

Ein Evidence Bundle muss relevante Bestandteile auf Veränderung prüfen können.

Dazu gehören insbesondere:

```text
sources
provenance
execution_identity
verification_results
result_reference
```

## Hashing

Evidence-Daten dürfen durch kryptografische Hashes abgesichert werden.

Beispiel:

```text
integrity {
    algorithm:
        SHA-256

    digest:
        ...
}
```

Große Strukturen dürfen über Teil-Hashes oder Merkle-Strukturen abgesichert werden.

## Signaturen

Evidence Bundles dürfen digital signiert werden.

```text
signature {
    signer
    algorithm
    key_reference
    value
}
```

Der verwendete Signaturschlüssel muss eindeutig referenzierbar sein.

## Signaturumfang

Es muss eindeutig definiert sein, welche Daten von einer Signatur abgedeckt werden.

Beispiel:

```text
signed {
    result
    sources
    provenance
    execution
}
```

Nicht signierte Bestandteile dürfen nicht als kryptografisch bestätigt dargestellt werden.

## Verification Status

Mindestens folgende Zustände müssen unterscheidbar sein:

```text
VALID
INVALID
UNSIGNED
UNVERIFIABLE
```

`UNVERIFIABLE` bedeutet beispielsweise, dass der benötigte Schlüssel oder Algorithmus nicht verfügbar ist.

## Änderungen

Wird ein signierter Bestandteil verändert, muss die bestehende Signatur ungültig werden.

Eine autorisierte Änderung erfordert anschließend eine neue Signatur oder Evidence-Version.

## Beispiel

```text
EvidenceBundle {
    result:
        object:analysis:81

    integrity {
        digest:
            ...
    }

    signature {
        signer:
            system:nova-01

        key_reference:
            key:42

        value:
            ...
    }
}
```

## Normative Anforderungen

1. Evidence Bundles MÜSSEN auf Integritätsverletzungen prüfbar sein.
2. Der Umfang einer Signatur MUSS eindeutig definiert sein.
3. Änderungen an signierten Daten MÜSSEN die zugehörige Signatur ungültig machen.
4. NovaOS MUSS mindestens `VALID`, `INVALID`, `UNSIGNED` und `UNVERIFIABLE` unterscheiden können.
5. Signaturgültigkeit DARF nicht automatisch mit fachlicher Richtigkeit oder Trust gleichgesetzt werden.
6. Signatur- und Hashverfahren MÜSSEN algorithmus- und versionsidentifizierbar sein.
7. Autorisierte Änderungen MÜSSEN eine neue Integritätsabsicherung ermöglichen.

## Abgrenzung

Diese NPSPEC definiert:

- Evidence-Integrität
- Hashing
- digitale Signaturen
- Signaturprüfung

Nicht Bestandteil sind:

- allgemeines Schlüsselmanagement
- PKI
- Source Provenance
- fachliche Bewertung eines Ergebnisses

## Zugehörige NPSPECs

- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`
- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`
- `NPSPEC-EVIDENCE-0003 – Algorithm, Model & Tool Identity`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`
- `NPSPEC-EVIDENCE-0006 – Evidence Retention & Privacy`