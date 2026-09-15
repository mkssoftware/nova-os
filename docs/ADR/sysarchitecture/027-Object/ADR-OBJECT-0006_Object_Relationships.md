# ADR-OBJECT-0006 – Object Relationships

## Status

Angenommen

## Kategorie

Object Architecture / Relationships

## Kontext

NovaOS-Objekte existieren nicht isoliert. Dateien, Prozesse, Geräte, Dienste, Benutzer und andere Objekte können logisch oder strukturell miteinander verbunden sein.

Diese Beziehungen müssen unabhängig von Pfaden, Namen und konkreten Speicherorten stabil darstellbar sein.

## Entscheidung

NovaOS unterstützt **Object Relationships** als Bestandteil des Unified System Object Model.

```text
Object A
    ↓
Relationship
    ↓
Object B
```

Relationships verbinden stabile Object IDs miteinander.

## Grundmodell

Eine Object Relationship besteht mindestens aus:

```text
Source Object ID
Relationship Type
Target Object ID
```

Optional können zusätzliche Metadaten mit der Beziehung verbunden werden.

## Relationship Types

Beziehungen besitzen einen definierten Typ.

Beispiele:

```text
contains
references
depends_on
derived_from
owned_by
created_by
attached_to
```

Relationship Types müssen eindeutig identifizierbar und erweiterbar sein.

## Richtung

Relationships können gerichtet sein.

```text
Object A
   ↓ depends_on
Object B
```

Die Gegenrichtung ist nicht automatisch identisch.

## Stabilität

Relationships referenzieren Object IDs statt Pfade.

```text
Object A
   ↓
Object ID B
   ↓
Object B
```

Umbenennen oder Verschieben eines Objekts zerstört die Beziehung dadurch nicht.

## Versionen

Eine Relationship kann entweder auf:

```text
Object ID
```

oder auf:

```text
Object ID + Version ID
```

verweisen.

Damit können sowohl logische Objekte als auch konkrete historische Zustände referenziert werden.

## Semantic Relationships

Object Relationships bilden die strukturelle Grundlage für Semantic Relationships.

```text
Object Relationship
        +
Semantic Relationship Type
        ↓
Semantic Relationship
```

Nicht jede Object Relationship muss semantische Bedeutung besitzen.

## Permissions

Eine Beziehung gewährt keine implizite Zugriffsberechtigung.

```text
Relationship exists
       ≠
Access allowed
```

Beim Zugriff auf das Zielobjekt müssen dessen Permissions separat geprüft werden.

## Löschen

Beim Löschen eines Objekts müssen abhängige Relationships kontrolliert behandelt werden.

Mögliche Policies sind:

```text
Remove Relationship
Preserve Tombstone
Reject Deletion
Explicit Cascade
```

Unkontrollierte implizite Cascades sind nicht zulässig.

## Normative Anforderungen

1. NovaOS MUSS Relationships zwischen Systemobjekten unterstützen.
2. Relationships MÜSSEN stabile Object IDs referenzieren können.
3. Relationship Types MÜSSEN eindeutig identifizierbar sein.
4. Gerichtete Relationships MÜSSEN unterstützt werden.
5. Relationships MÜSSEN auf konkrete Object Versions verweisen können.
6. Umbenennen oder Verschieben DARF eine objektbasierte Relationship nicht zerstören.
7. Relationships DÜRFEN keine impliziten Zugriffsrechte erzeugen.
8. Das Verhalten bei Löschung eines referenzierten Objekts MUSS explizit definiert sein.
9. Semantic Relationships MÜSSEN auf dem Object-Relationship-Modell aufbauen können.

## Konsequenzen

### Positive Konsequenzen

- stabile Verknüpfungen zwischen Systemobjekten,
- Beziehungen bleiben unabhängig von Pfaden erhalten,
- Versionsbezüge können exakt dargestellt werden,
- Semantic Relationships erhalten eine gemeinsame Objektgrundlage.

### Negative Konsequenzen

- Relationship-Lifecycle muss verwaltet werden,
- Löschung und Versionierung benötigen klare Referenzregeln.

## Abhängigkeiten

- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-OBJECT-0005_Object_Permissions`
- `ADR-SEMANTIC-0008_Semantic_Relationships`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-TYPE-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-LIFECYCLE-0001`

## Ergebnis

NovaOS verbindet Objekte über stabile, typisierte Beziehungen:

```text
Object ID A
     ↓
Relationship
     ↓
Object ID B
```

Der zentrale Grundsatz lautet:

```text
Objekte werden über ihre Identität
miteinander verbunden,

nicht über ihren aktuellen
Namen oder Speicherort.
```