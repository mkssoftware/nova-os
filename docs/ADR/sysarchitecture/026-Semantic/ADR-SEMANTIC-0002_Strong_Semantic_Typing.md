# ADR-SEMANTIC-0002 – Strong Semantic Typing

## Status

Angenommen

## Kategorie

Semantic Architecture / Type Safety

## Kontext

Semantic Types besitzen nur dann einen echten Sicherheits- und Architekturwert, wenn ihre Bedeutung bei Datenübergaben tatsächlich geprüft wird.

Würde NovaOS semantische Typinformationen lediglich als optionale Metadaten behandeln, könnten inkompatible Werte trotz unterschiedlicher Bedeutung miteinander verbunden werden.

```text
Length.Meter
Temperature.Celsius
```

Beide können technisch als `float` gespeichert sein, sind semantisch jedoch nicht kompatibel.

## Entscheidung

NovaOS verwendet **Strong Semantic Typing**.

Semantic Types sind Bestandteil von Contracts und werden bei semantisch typisierten Schnittstellen verbindlich geprüft.

```text
Value
  ↓
Primitive Type
  +
Semantic Type
  ↓
Contract Check
```

Primitive Kompatibilität allein reicht nicht aus.

## Typkompatibilität

Eine Übergabe ist nur zulässig, wenn die Semantic Types:

```text
identisch
kompatibel
oder explizit konvertierbar
```

sind.

Beispiel:

```text
Length.Meter
     ↓
Length.Meter
     ↓
Valid
```

Dagegen:

```text
Length.Meter
     ↓
Time.Second
     ↓
Invalid
```

## Explizite Konvertierung

Unterschiedliche Semantic Types dürfen über definierte Konvertierungen verbunden werden.

```text
Length.Meter
     ↓
Defined Conversion
     ↓
Length.Kilometer
```

NovaOS darf keine implizite Konvertierung allein aufgrund gleicher primitiver Repräsentation durchführen.

## Capability Contracts

Capabilities deklarieren ihre semantischen Ein- und Ausgabetypen.

```text
Capability:
    image.resize

Input:
    Image.RGBA

Output:
    Image.RGBA
```

Der Capability Resolver kann dadurch semantisch inkompatible Provider bereits vor der Ausführung ausschließen.

## Grenzen

Strong Semantic Typing gilt an semantisch typisierten Systemgrenzen.

Intern dürfen Implementierungen weiterhin primitive Typen verwenden, solange beim Eintritt und Austritt die definierten Semantic Contracts eingehalten werden.

## Normative Anforderungen

1. NovaOS MUSS Strong Semantic Typing unterstützen.
2. Semantic Types MÜSSEN an semantisch typisierten Schnittstellen verbindlich geprüft werden.
3. Primitive Typgleichheit DARF keine semantische Kompatibilität implizieren.
4. Inkompatible Semantic Types MÜSSEN zurückgewiesen werden.
5. Konvertierungen MÜSSEN explizit definiert sein.
6. Capability Contracts MÜSSEN Semantic Types verbindlich verwenden können.
7. Implementierungsinterne primitive Repräsentationen DÜRFEN erhalten bleiben.

## Konsequenzen

### Positive Konsequenzen

- semantisch falsche Datenverknüpfungen werden verhindert,
- Capability Contracts werden zuverlässiger,
- Einheiten- und Bedeutungsfehler werden reduziert,
- automatische Datenverknüpfung wird sicherer.

### Negative Konsequenzen

- Typprüfung verursacht zusätzlichen Aufwand,
- Semantic-Type- und Conversion-Regeln müssen gepflegt werden.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-STRONG-TYPING-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`

## Ergebnis

NovaOS behandelt semantische Typen nicht nur als Beschreibung, sondern als überprüfbaren Contract:

```text
Primitive compatible
        +
Semantic compatible
        ↓
Valid Data Transfer
```

Der zentrale Grundsatz lautet:

```text
Gleiche Datenrepräsentation
bedeutet nicht
gleiche Bedeutung.
```