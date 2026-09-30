# NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Adapter- und Transformation-Nodes im `Nova.CompatibilityGraph`.

Sie ermöglichen Kompatibilität zwischen Komponenten, Datentypen oder Versionen, die nicht direkt miteinander kompatibel sind.

## Grundprinzip

```text
Source
    ↓
Adapter / Transformation
    ↓
Compatible Target
```

## Adapter Node

Ein Adapter verbindet unterschiedliche, aber semantisch kompatible Schnittstellen.

Beispiel:

```text
Interface@1
    ↓ Adapter
Interface@2
```

Die fachliche Bedeutung der Daten soll dabei erhalten bleiben.

## Transformation Node

Eine Transformation verändert Darstellung oder Struktur einer Information.

Beispiel:

```text
Document.Schema@1
    ↓ Transformation
Document.Schema@2
```

Transformationen können:

```text
LOSSLESS
LOSSY
```

sein.

## Node Descriptor

Ein Node kann logisch beschrieben werden als:

```text
TransformationNode {
    input
    output
    type
    requirements
    properties
}
```

`type` kann beispielsweise sein:

```text
ADAPTER
TRANSFORMATION
MIGRATION
```

## Anforderungen

Ein Node darf Voraussetzungen besitzen.

Beispiele:

```text
required_capability
required_schema
hardware
memory
trust
```

Nicht erfüllte zwingende Anforderungen machen den Übergang unbrauchbar.

## Verlustbehaftete Transformation

Bei einer `LOSSY`-Transformation muss der mögliche Informationsverlust erkennbar sein.

Beispiel:

```text
Image.RGBA
    ↓ LOSSY
Image.RGB
```

NovaOS darf einen solchen Pfad nur verwenden, wenn die Anforderungen des Intents dies erlauben.

## Verkettung

Mehrere Adapter und Transformationen dürfen kombiniert werden.

```text
Type A
    ↓ Adapter
Type B
    ↓ Transformation
Type C
```

Die resultierende Kompatibilität hängt von allen beteiligten Nodes ab.

## Seiteneffekte

Adapter- und Transformation-Nodes sollen bevorzugt frei von externen Seiteneffekten sein.

Vorhandene Seiteneffekte müssen explizit deklariert werden.

## Normative Anforderungen

1. Adapter und Transformationen MÜSSEN als eigene Nodes im Compatibility Graph darstellbar sein.
2. Input und Output MÜSSEN semantisch eindeutig beschrieben werden.
3. `LOSSLESS` und `LOSSY` MÜSSEN unterscheidbar sein.
4. Zwingende Anforderungen eines Nodes MÜSSEN vor Verwendung prüfbar sein.
5. Verlustbehaftete Transformationen DÜRFEN nicht verwendet werden, wenn dadurch Intent-Anforderungen verletzt werden.
6. Mehrere Nodes MÜSSEN zu einem Kompatibilitätspfad verkettbar sein.
7. Relevante Seiteneffekte MÜSSEN deklarierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Adapter Nodes
- Transformation Nodes
- verlustfreie und verlustbehaftete Übergänge
- grundlegende Node-Anforderungen

Nicht Bestandteil sind:

- Pfadsuche
- Auswahl des besten Pfads
- Compatibility Validation
- Trust-Bewertung

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0002 – Semantic Version Constraints`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-COMPAT-0005 – Compatibility Path Resolution`
- `NPSPEC-COMPAT-0006 – Compatibility Validation`
- `NPSPEC-COMPAT-0007 – Compatibility Trust & Security`