# ADR-SEMANTIC-0001 – Semantic Types zusätzlich zu primitiven Typen

## Status

Angenommen

## Kategorie

Semantic Architecture / Type System

## Kontext

Primitive Datentypen wie:

```text
int
float
string
bool
byte[]
```

beschreiben die technische Repräsentation eines Wertes, nicht dessen Bedeutung.

Beispielsweise können folgende Werte technisch alle `float` sein:

```text
Temperature
Distance
Voltage
Probability
Latitude
```

Ohne semantische Information müssen Anwendungen Bedeutung, Einheit und zulässige Operationen selbst interpretieren.

## Entscheidung

NovaOS verwendet **Semantic Types zusätzlich zu primitiven Typen**.

```text
Primitive Type
      +
Semantic Type
      ↓
Meaningful Value
```

Beispiel:

```text
float
+
Temperature.Celsius
```

Der primitive Typ beschreibt die Repräsentation.

Der Semantic Type beschreibt die Bedeutung.

## Grundprinzip

```text
Primitive Type = Wie ist der Wert gespeichert?

Semantic Type = Was bedeutet der Wert?
```

Semantic Types ersetzen primitive Typen nicht, sondern ergänzen sie.

## Beispiele

```text
float + Length.Meter
float + Temperature.Celsius
float + Voltage.Volt
string + Identity.EmailAddress
byte[] + Image.PNG
byte[] + Audio.PCM
```

Dadurch können Werte systemweit semantisch erkannt werden.

## Typidentität

Semantic Types benötigen eine eindeutige Identität.

```text
Nova.Semantic.Length.Meter
Nova.Semantic.Temperature.Celsius
Nova.Semantic.Image.RGBA
```

Die konkrete Namens- und Identitätsstruktur wird separat spezifiziert.

## Contracts

Capabilities können Semantic Types in ihren Contracts verwenden.

```text
Capability:
    image.resize

Input:
    Image.RGBA

Output:
    Image.RGBA
```

Dadurch kann NovaOS prüfen, ob Daten semantisch zu einer Capability passen.

## Konvertierung

Unterschiedliche Semantic Types dürfen nur über bekannte Konvertierungen ineinander überführt werden.

```text
Temperature.Celsius
        ↓
Known Conversion
        ↓
Temperature.Fahrenheit
```

Eine gleiche primitive Repräsentation bedeutet nicht automatisch semantische Kompatibilität.

## Type Safety

NovaOS soll inkompatible semantische Verknüpfungen erkennen können.

```text
Length.Meter
     +
Temperature.Celsius
     ↓
Invalid
```

Damit können Fehler bereits vor oder während der Datenübergabe erkannt werden.

## Erweiterbarkeit

Neue Semantic Types müssen ohne Änderung bestehender primitiver Typen ergänzt werden können.

Hersteller und Anwendungen dürfen eigene Semantic Types definieren, sofern Namensräume und Contracts eingehalten werden.

## Normative Anforderungen

1. NovaOS MUSS Semantic Types zusätzlich zu primitiven Typen unterstützen.
2. Semantic Types MÜSSEN eindeutig identifizierbar sein.
3. Primitive und semantische Typinformation MÜSSEN getrennt behandelbar sein.
4. Capabilities MÜSSEN Semantic Types in ihren Contracts verwenden können.
5. Gleiche primitive Typen DÜRFEN nicht automatisch als semantisch kompatibel gelten.
6. Konvertierungen zwischen Semantic Types MÜSSEN explizit definierbar sein.
7. Semantic Types MÜSSEN erweiterbar und namespacefähig sein.

## Konsequenzen

### Positive Konsequenzen

- Daten tragen systemweit Bedeutung.
- Capabilities können automatisch kompatible Daten erkennen.
- Fehlinterpretationen von Einheiten und Datentypen werden reduziert.
- Daten können leichter zwischen unterschiedlichen Programmen und Capabilities verbunden werden.

### Negative Konsequenzen

- zusätzliche Typmetadaten müssen verwaltet werden.
- Semantic-Type-Namespaces und Konvertierungen benötigen klare Regeln.

## Abhängigkeiten

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0017_Composability`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-NAMESPACE-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`

## Ergebnis

NovaOS unterscheidet zwischen technischer Repräsentation und fachlicher Bedeutung:

```text
Value
 ├── Primitive Type
 └── Semantic Type
```

Der zentrale Grundsatz lautet:

```text
Daten sollen nicht nur sagen,
wie sie gespeichert sind,
sondern auch,
was sie bedeuten.
```