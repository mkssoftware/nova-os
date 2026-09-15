# ADR-SEMANTIC-0007 – Semantic Metadata

## Status

Angenommen

## Kategorie

Semantic Architecture / Metadata

## Kontext

Semantic Types beschreiben, was Daten oder Ressourcen bedeuten. Für viele Operationen reicht der Typ allein jedoch nicht aus.

Zusätzliche Informationen wie Einheit, Sprache, Zeitbezug, Koordinatensystem oder Qualität müssen ebenfalls maschinenlesbar beschrieben werden können.

## Entscheidung

NovaOS unterstützt **Semantic Metadata** als strukturierte Ergänzung zu Semantic Types.

```text
Resource
 ├── Semantic Type
 └── Semantic Metadata
```

Beispiel:

```text
Type:
    Temperature

Metadata:
    Unit: Celsius
    Precision: 0.1
```

## Grundprinzip

Semantic Metadata beschreibt Eigenschaften und Kontext eines semantisch typisierten Wertes oder einer Ressource.

```text
Semantic Type
     ↓
What is it?

Semantic Metadata
     ↓
What context does it have?
```

## Beispiele

Semantic Metadata kann unter anderem beschreiben:

```text
Unit
Language
Coordinate System
Timestamp
Precision
Quality
Encoding
Source
```

Welche Metadaten zulässig oder erforderlich sind, wird durch den jeweiligen Semantic Type definiert.

## Namespaces

Semantic Metadata verwendet eindeutige Namespaces.

```text
Nova.Semantic.Unit
Nova.Semantic.Language
Nova.Semantic.CoordinateSystem
```

Dadurch können neue Metadaten erweitert werden, ohne bestehende Definitionen zu verändern.

## Contracts

Capabilities können Anforderungen an Semantic Metadata definieren.

```text
Input:
    Location

Requires:
    CoordinateSystem = WGS84
```

Fehlende oder inkompatible Metadaten können dadurch vor der Ausführung erkannt werden.

## Speicherung

Semantic Metadata kann abhängig von der Ressource:

```text
im Objekt
im NovaFile-Container
in Dateimetadaten
oder über referenzierte Systemmetadaten
```

gespeichert werden.

Die semantische Bedeutung darf nicht vom konkreten Speichermechanismus abhängen.

## Normative Anforderungen

1. NovaOS MUSS strukturierte Semantic Metadata unterstützen.
2. Semantic Metadata MUSS einem Wert oder einer Ressource eindeutig zugeordnet werden können.
3. Semantic Types MÜSSEN zulässige oder erforderliche Metadaten definieren können.
4. Capabilities MÜSSEN Semantic Metadata in ihren Contracts prüfen können.
5. Semantic Metadata MUSS namespacefähig und erweiterbar sein.
6. Der konkrete Speicherort DARF die semantische Bedeutung der Metadaten nicht verändern.
7. Metadatenänderungen MÜSSEN von Änderungen des eigentlichen Payloads unterscheidbar sein.

## Konsequenzen

### Positive Konsequenzen

- Daten erhalten zusätzlichen maschinenlesbaren Kontext.
- Einheiten und andere Bedeutungsinformationen gehen nicht verloren.
- Capabilities können Daten präziser prüfen und kombinieren.
- automatische Verarbeitung wird zuverlässiger.

### Negative Konsequenzen

- zusätzliche Metadaten müssen verwaltet werden.
- Semantic Types benötigen Regeln für relevante Metadaten.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-SEMANTIC-0004_Typed_Files`
- `ADR-SEMANTIC-0006_Typed_Capabilities`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-METADATA-NAMESPACE-0001`
- `NPSPEC-SEMANTIC-METADATA-CONTRACT-0001`

## Ergebnis

NovaOS verbindet semantische Typen mit strukturiertem Kontext:

```text
Data
 ├── Primitive Type
 ├── Semantic Type
 └── Semantic Metadata
```

Der zentrale Grundsatz lautet:

```text
Der Semantic Type beschreibt,
was etwas ist.

Semantic Metadata beschreibt
den dazugehörigen Kontext.
```