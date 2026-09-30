# NPSPEC-COMPAT-0003 – Compatibility Graph Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert den `Compatibility Graph` von NovaOS.

Der Graph beschreibt bekannte Kompatibilitätsbeziehungen zwischen Versionen, Semantic Types, Capabilities, Schemas und Formaten.

## Grundprinzip

```text
Source
    ↓
Compatibility Graph
    ↓
Compatibility Path
    ↓
Target
```

Kompatibilität kann direkt oder über Adapter, Transformationen und Migrationen hergestellt werden.

## Graphmodell

Der Compatibility Graph besteht aus:

```text
Nodes
Edges
```

Nodes repräsentieren kompatibilitätsrelevante Zustände.

Edges beschreiben bekannte Übergänge zwischen ihnen.

## Nodes

Nodes können beispielsweise darstellen:

```text
Capability
Semantic Type
Schema
Format
Interface
Version
Execution IR
```

Beispiel:

```text
Media.Audio@1
Media.Audio@2
Media.Audio@3
```

## Edges

Eine Edge beschreibt eine bekannte Kompatibilitätsbeziehung.

Mögliche Typen:

```text
COMPATIBLE
ADAPTER
TRANSFORMATION
MIGRATION
```

Beispiel:

```text
Media.Audio@1
    ↓ ADAPTER
Media.Audio@2
```

## Gerichtete Beziehungen

Kompatibilitätsbeziehungen dürfen gerichtet sein.

```text
Schema@1
    → Schema@2
```

bedeutet nicht automatisch:

```text
Schema@2
    → Schema@1
```

Rückwärtskompatibilität muss separat beschrieben werden.

## Mehrstufige Kompatibilität

Direkte Kompatibilität ist nicht notwendig, wenn ein gültiger Pfad existiert.

```text
Type A
    ↓
Adapter
    ↓
Type B
    ↓
Migration
    ↓
Type C
```

Die eigentliche Pfadsuche wird separat spezifiziert.

## Edge-Eigenschaften

Eine Beziehung darf zusätzliche Eigenschaften besitzen.

Beispiele:

```text
lossless
lossy
trusted
cost
requirements
```

Dadurch kann NovaOS unterschiedliche Kompatibilitätspfade bewerten.

## Dynamische Erweiterung

Neue Capabilities, Adapter oder Versionen dürfen den Graph erweitern.

Entfernte oder ungültige Komponenten müssen entsprechende Beziehungen invalidieren können.

## Beispiel

```text
Document.Schema@1
    ↓ MIGRATION
Document.Schema@2
    ↓ COMPATIBLE
Document.Schema@2.1
```

NovaOS kann daraus erkennen, dass Daten aus Version 1 über eine Migration mit Version 2.1 verwendbar sind.

## Normative Anforderungen

1. Kompatibilitätsbeziehungen MÜSSEN als gerichteter Graph darstellbar sein.
2. Nodes MÜSSEN kompatibilitätsrelevante Komponenten oder Zustände eindeutig referenzieren können.
3. Direkte Kompatibilität, Adapter, Transformationen und Migrationen MÜSSEN unterscheidbar sein.
4. Mehrstufige Kompatibilitätspfade MÜSSEN darstellbar sein.
5. Gerichtete Beziehungen DÜRFEN nicht automatisch als bidirektional interpretiert werden.
6. Beziehungen MÜSSEN zusätzliche Eigenschaften und Anforderungen tragen können.
7. Änderungen an Komponenten MÜSSEN betroffene Graphbeziehungen invalidierbar machen.

## Abgrenzung

Diese NPSPEC definiert:

- Compatibility Graph
- Nodes und Edges
- gerichtete Kompatibilitätsbeziehungen
- mehrstufige Verbindungen

Nicht Bestandteil sind:

- Adapterausführung
- Pfadauflösung
- Compatibility Validation
- Trust-Prüfung

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0002 – Semantic Version Constraints`
- `NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes`
- `NPSPEC-COMPAT-0005 – Compatibility Path Resolution`
- `NPSPEC-COMPAT-0006 – Compatibility Validation`
- `NPSPEC-COMPAT-0007 – Compatibility Trust & Security`