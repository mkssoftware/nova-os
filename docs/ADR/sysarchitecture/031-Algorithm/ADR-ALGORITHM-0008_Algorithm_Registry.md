# ADR-ALGORITHM-0008 – Algorithm Registry

## Status

Angenommen

## Kategorie

Algorithm Architecture / Registry

## Kontext

NovaOS kann für eine Algorithm Family mehrere Algorithmen und Varianten besitzen.

Diese können:

```text
Built-in
System Provided
Hardware Specific
Third Party
Dynamically Added
Remote Available
```

sein.

Damit automatische Auswahl, Discovery, Versionierung und Introspection funktionieren, benötigt NovaOS eine zentrale logische Sicht auf verfügbare Algorithmen.

## Entscheidung

NovaOS verwendet eine **Algorithm Registry** zur Registrierung und Discovery verfügbarer Algorithmen.

```text
Algorithm Provider
       ↓
Algorithm Registration
       ↓
Algorithm Registry
       ↓
Discovery
       ↓
Algorithm Selection
```

Die Registry enthält Beschreibungen und Verweise auf Algorithmen, führt die eigentliche Berechnung jedoch nicht selbst aus.

## Algorithm Descriptor

Jeder registrierte Algorithmus besitzt einen strukturierten Descriptor.

Mindestens:

```text
Algorithm ID
Algorithm Family
Version
Semantic Input Types
Semantic Output Types
Execution Properties
Compatible Compute Providers
```

Optional können weitere Eigenschaften enthalten sein:

```text
Precision
Determinism
Resource Requirements
Hardware Requirements
Cost Models
Performance Models
Energy Models
Data Characteristics
```

## Stabile Identität

Algorithmen besitzen eine stabile logische Identität.

```text
Algorithm ID
    ≠
Binary Path
Provider
Memory Address
Installation Location
```

Physische Änderungen dürfen die logische Identität nicht unnötig verändern.

## Family-Zuordnung

Jeder Algorithmus muss einer Algorithm Family zugeordnet sein.

```text
Algorithm Registry
       ↓
Algorithm Family
       ├── Algorithm A
       ├── Algorithm B
       └── Algorithm C
```

Die Registry muss Discovery sowohl nach Family als auch nach konkreter Algorithm ID ermöglichen.

## Versionierung

Mehrere Versionen eines Algorithmus können gleichzeitig bekannt sein.

```text
Algorithm A
 ├── v1
 ├── v2
 └── v3
```

Selection Contracts können dadurch:

```text
Latest Compatible
Version Range
Exact Version
```

ausdrücken.

Eine neue Version darf reproduzierbare Ausführungen nicht unkontrolliert verändern.

## Registrierung

Algorithmen können statisch oder dynamisch registriert werden.

```text
Provider
   ↓
Descriptor Validation
   ↓
Registration
   ↓
Registry
```

Eine erfolgreiche Registrierung bedeutet lediglich, dass der Algorithmus bekannt ist.

Sie bedeutet nicht automatisch:

```text
Trusted
Authorized
Selected
Executable
```

## Validierung

Vor Aufnahme in die Registry muss der Descriptor strukturell und semantisch validiert werden.

```text
Algorithm Descriptor
       ↓
Schema Validation
       ↓
Semantic Validation
       ↓
Registry Entry
```

Widersprüchliche oder unvollständige verpflichtende Angaben müssen abgelehnt werden.

## Discovery

Die Registry unterstützt semantische Discovery.

Beispiele:

```text
Find:
    Family = Image.Resize
    Deterministic = true
    Precision >= required
    GPU Compatible = true
```

Discovery liefert Kandidaten.

Die endgültige Zulässigkeit bestimmt weiterhin der Execution Contract.

## Registry und Selection

Die Registry entscheidet nicht selbst, welcher Algorithmus ausgeführt wird.

```text
Algorithm Registry
       ↓
Candidate Algorithms
       ↓
Execution Contract
       ↓
Selection Engine
       ↓
Selected Algorithm
```

Damit bleiben Discovery und Policy getrennt.

## Provider-Beziehung

Ein Algorithmus kann mehrere Compute Provider unterstützen.

```text
Algorithm A
 ├── CPU Provider
 ├── GPU Provider
 └── Remote GPU Provider
```

Ebenso kann ein Provider mehrere Algorithmen implementieren.

Algorithm Registry und Compute Provider Registry bleiben logisch getrennte Konzepte.

## Dynamische Änderungen

Algorithmen können zur Laufzeit:

```text
Added
Updated
Unavailable
Deprecated
Removed
```

werden.

Änderungen müssen für zukünftige Selection Requests sichtbar werden.

Bereits laufende Ausführungen dürfen dadurch nicht unkontrolliert verändert werden.

## Trust und Provenance

Registry-Einträge können Trust- und Provenance-Informationen referenzieren.

```text
Algorithm
    ↓
Implementation
    ↓
Signature
Provenance
Trust Evidence
```

Die Existenz eines Registry-Eintrags ist kein Trust-Nachweis.

## Introspection

Die Algorithm Registry muss introspektierbar sein.

NovaOS soll beispielsweise ermitteln können:

```text
Welche Algorithm Families existieren?
Welche Algorithmen gehören dazu?
Welche Versionen sind verfügbar?
Welche Provider werden unterstützt?
Warum wurde ein Algorithmus ausgeschlossen?
```

## Normative Anforderungen

1. NovaOS MUSS eine systemweite logische Algorithm Registry bereitstellen.
2. Jeder registrierte Algorithmus MUSS eine stabile Algorithm ID besitzen.
3. Jeder Algorithmus MUSS einer Algorithm Family zugeordnet sein.
4. Registry-Einträge MÜSSEN versionierbar sein.
5. Algorithm Descriptoren MÜSSEN vor Registrierung validiert werden.
6. Die Registry MUSS Discovery nach Family, ID und relevanten Eigenschaften ermöglichen.
7. Discovery DARF nicht mit Selection gleichgesetzt werden.
8. Registrierung DARF nicht automatisch Trust oder Authorization bedeuten.
9. Mehrere Implementierungen und Versionen eines Algorithmus MÜSSEN unterstützt werden können.
10. Dynamische Registrierung und Entfernung MÜSSEN möglich sein.
11. Registry-Änderungen DÜRFEN laufende Ausführungen nicht unkontrolliert verändern.
12. Algorithm Registry und Compute Provider Registry MÜSSEN logisch getrennt bleiben.
13. Trust- und Provenance-Informationen MÜSSEN referenzierbar sein.
14. Die Registry MUSS über Introspection auswertbar sein.

## Konsequenzen

### Positive Konsequenzen

- zentrale Discovery verfügbarer Algorithmen,
- automatische Algorithmusauswahl erhält eine definierte Kandidatenquelle,
- mehrere Versionen und Implementierungen können koexistieren,
- neue Algorithmen können dynamisch integriert werden,
- Algorithmuslandschaft wird introspektierbar.

### Negative Konsequenzen

- Registry-Zustand muss konsistent gehalten werden,
- Descriptoren benötigen ein stabiles Schema,
- dynamische Änderungen erfordern Versionierungs- und Lifecycle-Regeln.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0003_Explizites_Algorithmus_Override`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-ALGORITHM-0007_Deterministic_Algorithm_Selection`
- `ADR-SEMANTIC-0011_Semantic_Discovery`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-ARCH-0013_Introspection`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-REGISTRY-0001`
- `NPSPEC-ALGORITHM-DESCRIPTOR-0001`
- `NPSPEC-ALGORITHM-REGISTRATION-0001`
- `NPSPEC-ALGORITHM-DISCOVERY-0001`
- `NPSPEC-ALGORITHM-REGISTRY-LIFECYCLE-0001`

## Ergebnis

Die Algorithm Registry bildet die systemweite Quelle für verfügbare Algorithmusimplementierungen:

```text
Algorithm Providers
        ↓
Algorithm Descriptors
        ↓
Algorithm Registry
        ↓
Semantic Discovery
        ↓
Candidate Algorithms
        ↓
Execution Contract
        ↓
Algorithm Selection
        ↓
Compute Provider
        ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
Die Registry weiß,
welche Algorithmen verfügbar sind.

Der Execution Contract bestimmt,
welche davon zulässig sind.

Die Selection Engine entscheidet,
welcher davon verwendet wird.
```