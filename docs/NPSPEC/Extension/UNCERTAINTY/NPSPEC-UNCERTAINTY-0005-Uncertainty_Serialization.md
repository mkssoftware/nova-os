# NPSPEC-UNCERTAINTY-0005 – Uncertainty Serialization

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie unsichere Werte in NovaOS gespeichert, übertragen und wiederhergestellt werden.

Ziel ist, dass Wert, Unsicherheitsform, Confidence, Einheit und Herkunft verlustfrei erhalten bleiben.

## Grundprinzip

```text
Uncertain Value
    ↓
Serialization
    ↓
Storage / IPC / TaskCapsule / Network
    ↓
Deserialization
    ↓
semantisch gleichwertiger Wert
```

## Serialisierbare Struktur

Ein unsicherer Wert muss mindestens folgende Informationen darstellen können:

```text
UncertainValue {
    semantic_type
    value
    uncertainty
    confidence
    unit
    provenance
    version
}
```

Nicht jedes Feld ist für jeden Typ verpflichtend.

## Unsicherheitsarten

Die Serialisierung muss mindestens folgende Formen unterstützen:

```text
ABSOLUTE
RELATIVE
RANGE
LOWER_UPPER
CONFIDENCE
DISTRIBUTION
UNKNOWN
```

Beispiel:

```text
uncertainty {
    kind: ABSOLUTE
    value: 0.4
}
```

## Einheit

Physikalische Werte müssen ihre Einheit gemeinsam mit Wert und Unsicherheit speichern können.

Beispiel:

```text
value:
    23.7

unit:
    celsius

uncertainty:
    0.4
```

Einheiten dürfen nicht ausschließlich aus externem Kontext abgeleitet werden müssen.

## Confidence

Falls vorhanden, muss Confidence inklusive ihrer Bedeutung serialisiert werden.

Beispiel:

```text
confidence {
    value: 0.95
    type: MEASUREMENT
    scale: 0..1
}
```

Ein nackter Zahlenwert ohne Semantik ist nicht ausreichend.

## Unbekannte Werte

Folgende Zustände müssen unterscheidbar bleiben:

```text
uncertainty_unknown
uncertainty_not_provided
uncertainty_not_applicable
zero_uncertainty
```

Diese Zustände dürfen bei Serialisierung und Deserialisierung nicht zusammenfallen.

## Mehrdimensionale Unsicherheit

Mehrdimensionale Werte müssen komponentenbezogene oder gemeinsame Unsicherheitsstrukturen speichern können.

Beispiel:

```text
value {
    x: 10
    y: 20
}

uncertainty {
    x: 0.2
    y: 0.3
}
```

Optional:

```text
covariance_matrix
```

## Distributionen

Falls Unsicherheit als Verteilung gespeichert wird, müssen mindestens Typ und Parameter erhalten bleiben.

Beispiel:

```text
distribution {
    type: normal
    mean: 100
    stddev: 2.5
}
```

Komplexere Verteilungen dürfen über erweiterbare Schemas beschrieben werden.

## Provenienz

Die Herkunft einer Unsicherheitsangabe soll referenzierbar bleiben.

Beispiel:

```text
provenance {
    source: sensor:temp_01
    method: measured
}
```

Die vollständige Provenienz kann extern über `Nova.Causality` oder `Nova.Evidence` referenziert werden.

## Versionierung

Serialisierte Unsicherheitsdaten müssen eine Version besitzen.

Beispiel:

```text
schema:
    nova.uncertainty@1
```

Neue Versionen dürfen zusätzliche optionale Felder ergänzen.

Inkompatible Änderungen benötigen eine neue Schema-Version.

## Forward Compatibility

Unbekannte optionale Felder sollen erhalten oder ignoriert werden können, sofern dies die Semantik nicht verfälscht.

Ein Empfänger darf unbekannte Pflichtsemantik nicht stillschweigend verwerfen.

In diesem Fall muss beispielsweise entstehen:

```text
unsupported_uncertainty_schema
```

## Kanonische Darstellung

NovaOS darf für Hashing, Signing oder deterministische Verarbeitung eine kanonische Serialisierungsform definieren.

Diese muss semantisch identische Daten eindeutig darstellen.

Beispiel:

```text
same semantic value
    ↓ canonical serialization
same byte representation
```

Die konkrete Binär- oder Textkodierung wird separat festgelegt.

## Verlustfreie Round-Trips

Folgende Operation muss semantisch verlustfrei sein:

```text
Value A
    ↓ serialize
Data
    ↓ deserialize
Value B
```

Dabei muss gelten:

```text
semantic(Value A) == semantic(Value B)
```

Technische interne Repräsentationen dürfen sich unterscheiden.

## Beispiel

```text
UncertainValue {
    schema:
        nova.uncertainty@1

    semantic_type:
        Measurement.Temperature

    value:
        23.7

    unit:
        celsius

    uncertainty {
        kind:
            ABSOLUTE

        value:
            0.4
    }

    confidence {
        value:
            0.95

        type:
            MEASUREMENT

        scale:
            0..1
    }

    provenance {
        source:
            sensor:temp_01

        method:
            measured
    }
}
```

## Sicherheit

Deserialisierte Unsicherheitsdaten müssen validiert werden.

Zu prüfen sind unter anderem:

```text
schema_version
semantic_type
units
numeric_ranges
distribution_parameters
references
```

Ungültige Daten dürfen nicht ungeprüft in Berechnungen übernommen werden.

## Normative Anforderungen

1. Wert und Unsicherheitsinformation MÜSSEN gemeinsam serialisierbar sein.
2. Unterschiedliche Unsicherheitsformen MÜSSEN eindeutig unterscheidbar bleiben.
3. Einheiten und Confidence-Semantik MÜSSEN erhalten bleiben können.
4. `unknown`, `not_provided`, `not_applicable` und `zero` DÜRFEN nicht zusammenfallen.
5. Serialisierte Daten MÜSSEN versionierbar sein.
6. Serialize-/Deserialize-Round-Trips MÜSSEN die semantische Bedeutung erhalten.
7. Unbekannte Pflichtsemantik DARF nicht stillschweigend verworfen werden.
8. Deserialisierte Unsicherheitsdaten MÜSSEN validierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Serialisierung unsicherer Werte
- Schema-Versionierung
- Round-Trip-Anforderungen
- grundlegende Validierung

Nicht Bestandteil sind:

- konkrete Binär- oder Textkodierung
- mathematische Unsicherheitsfortpflanzung
- Vergleichslogik
- Benutzeroberfläche

## Zugehörige NPSPECs

- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-UNCERTAINTY-0002 – Confidence & Error Representation`
- `NPSPEC-UNCERTAINTY-0003 – Uncertainty Propagation`
- `NPSPEC-UNCERTAINTY-0004 – Uncertain Comparison & Decision Semantics`
- `NPSPEC-UNCERTAINTY-0006 – Uncertainty Presentation`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`