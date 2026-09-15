# ADR-SEMANTIC-0006 – Typed Capabilities

## Status

Angenommen

## Kategorie

Semantic Architecture / Capabilities / Type Safety

## Kontext

Capabilities beschreiben in NovaOS verfügbare Fähigkeiten unabhängig von ihrer konkreten Implementierung.

Damit Capabilities sicher kombiniert und automatisch aufgelöst werden können, müssen ihre Ein- und Ausgaben semantisch eindeutig beschrieben sein.

Primitive Typen allein reichen dafür nicht aus.

## Entscheidung

NovaOS verwendet **Typed Capabilities**.

Capabilities definieren ihre Schnittstellen mit Semantic Types.

```text
Capability
 ├── Input Types
 ├── Output Types
 └── Contract
```

Beispiel:

```text
Capability:
    image.resize

Input:
    Image.RGBA
    Size.Pixels

Output:
    Image.RGBA
```

## Typprüfung

Vor der Ausführung wird geprüft, ob die bereitgestellten Daten semantisch mit dem Capability Contract kompatibel sind.

```text
Typed Input
    ↓
Capability Contract
    ↓
Type Check
    ↓
Execution
```

Inkompatible Eingaben werden zurückgewiesen.

## Provider

Mehrere Provider können dieselbe typisierte Capability implementieren.

```text
Capability<Image.RGBA → Image.RGBA>
              ↑
        ┌─────┴─────┐
        ↓           ↓
    Provider A   Provider B
```

Ein Provider gilt nur dann als kompatibel, wenn er den geforderten semantischen Contract erfüllt.

## Komposition

Typed Capabilities können automatisch miteinander verbunden werden, wenn ihre Ein- und Ausgabetypen kompatibel sind.

```text
Capability A
Output: Image.RGBA
        ↓
Capability B
Input: Image.RGBA
```

Dadurch können sichere Capability-Pipelines entstehen.

## Konvertierung

Sind Typen nicht direkt kompatibel, kann eine explizit definierte Conversion Capability verwendet werden.

```text
Image.RGB
    ↓
Conversion
    ↓
Image.RGBA
```

Implizite Konvertierungen aufgrund gleicher primitiver Repräsentation sind nicht zulässig.

## Versionierung

Änderungen an den Semantic Types einer Capability sind Bestandteil ihres Contracts und müssen bei der Kompatibilitätsprüfung berücksichtigt werden.

Breaking Type Changes dürfen nicht stillschweigend als kompatibel behandelt werden.

## Normative Anforderungen

1. NovaOS MUSS typisierte Capability Contracts unterstützen.
2. Capabilities MÜSSEN Semantic Types für Ein- und Ausgaben definieren können.
3. Provider MÜSSEN den semantischen Contract der angebotenen Capability erfüllen.
4. Inkompatible Eingaben MÜSSEN zurückgewiesen werden.
5. Capabilities DÜRFEN automatisch komponiert werden, wenn ihre Semantic Types kompatibel sind.
6. Typkonvertierungen MÜSSEN explizit definiert sein.
7. Breaking Type Changes MÜSSEN als inkompatible Contract-Änderungen erkennbar sein.

## Konsequenzen

### Positive Konsequenzen

- Capabilities können sicher automatisch kombiniert werden.
- Provider werden anhand ihrer tatsächlichen Datensemantik geprüft.
- semantische Fehler werden vor der Ausführung erkannt.
- Capability-Pipelines werden systemweit beschreibbar.

### Negative Konsequenzen

- Capability Contracts benötigen präzise Typdefinitionen.
- Typkompatibilität und Konvertierungen müssen verwaltet werden.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-SEMANTIC-0005_Typed_IPC`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-TYPED-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-CONTRACT-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-COMPOSITION-0001`

## Ergebnis

NovaOS beschreibt Capabilities durch semantisch typisierte Contracts:

```text
Typed Input
    ↓
Capability
    ↓
Typed Output
```

Der zentrale Grundsatz lautet:

```text
Eine Capability beschreibt nicht nur,
was sie kann,
sondern auch eindeutig,
welche Daten sie versteht
und welche Daten sie erzeugt.
```