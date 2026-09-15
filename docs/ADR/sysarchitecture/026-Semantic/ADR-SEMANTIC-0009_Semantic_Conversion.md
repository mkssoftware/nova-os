# ADR-SEMANTIC-0009 – Semantic Conversion

## Status

Angenommen

## Kategorie

Semantic Architecture / Conversion / Type System

## Kontext

Semantic Types können unterschiedliche Bedeutungen oder Repräsentationen derselben fachlichen Information beschreiben.

Beispiele:

```text
Temperature.Celsius
Temperature.Fahrenheit

Length.Meter
Length.Kilometer

Image.RGB
Image.RGBA
```

NovaOS benötigt einen definierten Mechanismus, um solche Typen sicher ineinander umzuwandeln.

## Entscheidung

NovaOS verwendet **explizite Semantic Conversions**.

```text
Source Type
    ↓
Conversion
    ↓
Target Type
```

Eine Konvertierung muss als definierte semantische Operation bekannt sein.

## Conversion Contract

Eine Semantic Conversion beschreibt mindestens:

```text
Source Type
Target Type
Conversion Capability
```

Beispiel:

```text
Temperature.Celsius
        ↓
temperature.convert
        ↓
Temperature.Fahrenheit
```

## Keine implizite Typumdeutung

Gleiche primitive Repräsentation erlaubt keine automatische semantische Umdeutung.

```text
float + Length.Meter
        ≠
float + Temperature.Celsius
```

Eine Konvertierung muss semantisch gültig sein.

## Lossless und Lossy

Konvertierungen können klassifiziert werden als:

```text
Lossless
Lossy
```

Beispiel:

```text
Image.RGBA
    ↓
Image.RGB
```

kann Informationen verlieren.

Der Conversion Contract muss dies kenntlich machen.

## Automatische Konvertierung

NovaOS darf eine Conversion automatisch einsetzen, wenn:

```text
Source und Target bekannt
Conversion erlaubt
Contract erfüllt
Policy erlaubt
```

Lossy Conversions dürfen nicht unbemerkt durchgeführt werden, wenn dadurch relevante Information verloren geht.

## Conversion Chains

Mehrere Conversions können verkettet werden.

```text
Type A
  ↓
Type B
  ↓
Type C
```

NovaOS darf solche Pfade automatisch bestimmen, wenn alle beteiligten Contracts kompatibel sind.

## Capabilities

Conversions werden bevorzugt als Capabilities bereitgestellt.

Dadurch können unterschiedliche Provider dieselbe Conversion implementieren.

```text
Semantic Conversion
        ↓
Capability
        ↓
Provider
```

## Normative Anforderungen

1. NovaOS MUSS explizite Semantic Conversions unterstützen.
2. Jede Conversion MUSS Source und Target Type definieren.
3. Primitive Typgleichheit DARF keine semantische Konvertierung implizieren.
4. Lossless und Lossy Conversions MÜSSEN unterscheidbar sein.
5. Lossy Conversions DÜRFEN nicht unkontrolliert durchgeführt werden.
6. Conversion Chains MÜSSEN unterstützt werden können.
7. Conversions SOLLEN als Capabilities bereitgestellt werden.
8. Strong Semantic Typing MUSS auch während Conversions erhalten bleiben.

## Konsequenzen

### Positive Konsequenzen

- unterschiedliche semantische Datenformate können sicher verbunden werden,
- automatische Capability-Pipelines werden möglich,
- Informationsverlust wird explizit sichtbar,
- Conversion Provider bleiben austauschbar.

### Negative Konsequenzen

- Conversion Contracts müssen gepflegt werden,
- komplexe Conversion Chains benötigen Resolver-Logik.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0007_Semantic_Metadata`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-SEMANTIC-CONVERSION-CONTRACT-0001`
- `NPSPEC-SEMANTIC-CONVERSION-RESOLUTION-0001`

## Ergebnis

NovaOS behandelt Typumwandlungen als explizite semantische Operationen:

```text
Typed Data A
     ↓
Semantic Conversion
     ↓
Typed Data B
```

Der zentrale Grundsatz lautet:

```text
NovaOS wandelt Bedeutung
nicht implizit um.

Jede semantische Umwandlung
besitzt einen definierten Contract.
```