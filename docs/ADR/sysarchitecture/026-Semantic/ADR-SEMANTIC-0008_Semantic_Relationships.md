# ADR-SEMANTIC-0008 – Semantic Relationships

## Status

Angenommen

## Kategorie

Semantic Architecture / Relationships

## Kontext

Semantic Types beschreiben die Bedeutung einzelner Daten und Ressourcen. Viele Informationen entstehen jedoch erst durch die Beziehung zwischen mehreren Objekten.

Beispiele:

```text
Person → authored → Document
Image → derived_from → Image
Measurement → belongs_to → Experiment
Document → references → Dataset
```

Diese Beziehungen sollen nicht nur innerhalb einzelner Anwendungen existieren, sondern systemweit semantisch beschreibbar sein.

## Entscheidung

NovaOS unterstützt **Semantic Relationships** zwischen typisierten Ressourcen und Objekten.

```text
Object A
   ↓
Relationship
   ↓
Object B
```

Eine Beziehung besitzt einen eindeutig definierten semantischen Typ.

## Relationship Types

Beziehungen werden über Semantic Relationship Types beschrieben.

Beispiele:

```text
references
derived_from
contains
belongs_to
created_by
depends_on
```

Relationship Types sind namespacefähig und erweiterbar.

## Richtung

Beziehungen können gerichtet sein.

```text
Document
   ↓ references
Dataset
```

Die Gegenrichtung darf nur verwendet werden, wenn sie definiert ist.

```text
Dataset
   ↑ referenced_by
Document
```

## Typisierte Endpunkte

Relationship Types können festlegen, welche Semantic Types miteinander verbunden werden dürfen.

```text
Measurement
    ↓ belongs_to
Experiment
```

Semantisch ungültige Beziehungen können dadurch verhindert werden.

## Beziehungseigenschaften

Eine Beziehung kann eigene Semantic Metadata besitzen.

```text
Relationship
 ├── Type
 ├── Source
 ├── Target
 └── Metadata
```

Damit können beispielsweise Zeit, Herkunft oder weitere Kontextinformationen beschrieben werden.

## Objektidentität

Semantic Relationships referenzieren stabile Objektidentitäten und nicht ausschließlich Pfade.

```text
Object ID A
    ↓
Relationship
    ↓
Object ID B
```

Verschieben oder Umbenennen eines Objekts zerstört dadurch die Beziehung nicht.

## Capabilities

Capabilities können Semantic Relationships lesen, erzeugen oder verändern.

Dadurch können Anwendungen und Systemfunktionen zusammenhängende Informationen über ein gemeinsames Modell verarbeiten.

## Normative Anforderungen

1. NovaOS MUSS semantisch typisierte Beziehungen unterstützen.
2. Beziehungen MÜSSEN stabile Objektidentitäten referenzieren können.
3. Relationship Types MÜSSEN eindeutig identifizierbar und namespacefähig sein.
4. Beziehungen MÜSSEN gerichtet definiert werden können.
5. Relationship Types MÜSSEN zulässige Source- und Target-Types definieren können.
6. Beziehungen MÜSSEN eigene Semantic Metadata besitzen können.
7. Capabilities MÜSSEN Semantic Relationships verwenden können.
8. Verschieben oder Umbenennen eines Objekts DARF eine objektbasierte Beziehung nicht zerstören.

## Konsequenzen

### Positive Konsequenzen

- Zusammenhänge zwischen Daten werden systemweit verständlich.
- Ressourcen können unabhängig von ihrem Speicherort verknüpft werden.
- Anwendungen können gemeinsame Beziehungen nutzen.
- Data Linking wird Bestandteil des semantischen Systemmodells.

### Negative Konsequenzen

- Beziehungen müssen konsistent verwaltet werden.
- Änderungen und Löschungen von Objekten benötigen definierte Relationship-Regeln.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-SEMANTIC-0007_Semantic_Metadata`
- `ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-TYPE-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-METADATA-0001`

## Ergebnis

NovaOS beschreibt nicht nur die Bedeutung einzelner Objekte, sondern auch deren Beziehungen:

```text
Typed Object A
      ↓
Semantic Relationship
      ↓
Typed Object B
```

Der zentrale Grundsatz lautet:

```text
Nicht nur Daten haben Bedeutung.

Auch ihre Beziehungen
haben Bedeutung.
```