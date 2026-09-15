# ADR-SEMANTIC-0011 – Semantic Discovery

## Status

Angenommen

## Kategorie

Semantic Architecture / Discovery

## Kontext

NovaOS kennt Semantic Types, Typed Resources und Typed Capabilities.

Damit diese automatisch miteinander verbunden werden können, benötigt das System einen Mechanismus, um passende Ressourcen, Capabilities und Konvertierungen anhand ihrer semantischen Eigenschaften zu finden.

## Entscheidung

NovaOS unterstützt **Semantic Discovery**.

```text
Semantic Requirement
        ↓
Discovery
        ↓
Matching Resources / Capabilities
```

Die Suche erfolgt anhand semantischer Eigenschaften und nicht ausschließlich anhand von Namen, Pfaden oder konkreten Implementierungen.

## Discovery

Eine Anfrage kann unter anderem enthalten:

```text
Semantic Type
Required Metadata
Capability
Relationship
Contract
```

NovaOS liefert nur semantisch kompatible Kandidaten.

## Ressourcen

Typed Resources können anhand ihrer Semantic Types gefunden werden.

```text
Find:
    Image

Result:
    Resource<Image.JPEG>
    Resource<Image.PNG>
```

Dabei bleibt die Resource Identity unabhängig von ihrem Speicherort.

## Capabilities

Capabilities können anhand ihrer semantischen Ein- und Ausgaben gefunden werden.

```text
Input:
    Image.RGBA

Required Output:
    Image.RGBA
        ↓
Compatible Capabilities
```

Die konkrete Provider-Auswahl erfolgt anschließend durch den Capability Resolver.

## Conversions

Existiert keine direkte Typkompatibilität, kann Semantic Discovery geeignete Conversion Capabilities finden.

```text
Type A
  ↓
Discovery
  ↓
Conversion
  ↓
Type B
```

## Relationships

Semantic Relationships können für Discovery verwendet werden.

```text
Document
   ↓ references
Dataset
```

Dadurch können zusammenhängende Ressourcen unabhängig von ihrer physischen Ablage gefunden werden.

## Security

Semantic Discovery darf nur Informationen offenlegen, die der anfragende Kontext sehen darf.

```text
Discoverable
     ≠
Globally Visible
```

Permissions und Capability-Regeln bleiben erhalten.

## Normative Anforderungen

1. NovaOS MUSS Semantic Discovery unterstützen.
2. Ressourcen MÜSSEN anhand ihrer Semantic Types auffindbar sein können.
3. Capabilities MÜSSEN anhand ihrer semantischen Contracts auffindbar sein können.
4. Semantic Metadata MUSS als Discovery-Kriterium verwendbar sein.
5. Semantic Relationships MÜSSEN für Discovery nutzbar sein.
6. Conversion Capabilities MÜSSEN auffindbar sein.
7. Discovery DARF Zugriffs- und Sichtbarkeitsregeln nicht umgehen.
8. Provider-Auswahl MUSS von Semantic Discovery getrennt bleiben.

## Konsequenzen

### Positive Konsequenzen

- Ressourcen können unabhängig von Pfaden gefunden werden.
- passende Capabilities können automatisch ermittelt werden.
- semantische Pipelines lassen sich dynamisch zusammensetzen.
- Beziehungen zwischen Daten werden für Discovery nutzbar.

### Negative Konsequenzen

- semantische Indizes müssen gepflegt werden.
- Discovery muss Berechtigungen effizient berücksichtigen.

## Abhängigkeiten

- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0007_Semantic_Metadata`
- `ADR-SEMANTIC-0008_Semantic_Relationships`
- `ADR-SEMANTIC-0009_Semantic_Conversion`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-INDEX-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-QUERY-0001`

## Ergebnis

NovaOS kann Systemobjekte anhand ihrer Bedeutung finden:

```text
Semantic Requirement
        ↓
Semantic Discovery
        ↓
Compatible Candidates
```

Der zentrale Grundsatz lautet:

```text
NovaOS sucht nicht nur danach,
wo etwas liegt oder wie es heißt,
sondern danach,
was es semantisch ist.
```