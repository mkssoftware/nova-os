# ADR-OBJECT-0007 – Immutable und Mutable Object Semantik

## Status

Angenommen

## Kategorie

Object Architecture / Mutability / Versioning

## Kontext

NovaOS-Objekte besitzen unterschiedliche Änderungsanforderungen.

Manche Objekte sollen nach ihrer Erzeugung unveränderlich bleiben, während andere einen veränderbaren logischen Zustand besitzen.

Ohne explizite Mutability-Semantik wäre unklar, ob eine Änderung ein bestehendes Objekt verändert oder einen neuen Zustand erzeugt.

## Entscheidung

NovaOS unterscheidet explizit zwischen **Immutable Objects** und **Mutable Objects**.

```text
Object
 ├── Immutable
 └── Mutable
```

Die Mutability ist Bestandteil des Object Contracts.

## Immutable Objects

Ein Immutable Object darf nach seiner Erzeugung nicht verändert werden.

```text
Object V1
    ↓
Change
    ↓
New Object / Version
```

Der bestehende Zustand bleibt unverändert.

Immutable Objects eignen sich insbesondere für:

```text
Content-addressed Data
Snapshots
Signed Artifacts
Historical States
Audit Records
```

## Mutable Objects

Ein Mutable Object besitzt eine stabile Object Identity, deren aktueller Zustand verändert werden kann.

```text
Object ID
   ↓
State V1
   ↓
Change
   ↓
State V2
```

Die Object ID bleibt erhalten.

## Versionierung

Persistente Änderungen an Mutable Objects können neue Object Versions erzeugen.

```text
Object ID
 ├── V1
 ├── V2
 └── V3
```

Damit bleibt die logische Identität stabil, während Zustandsänderungen nachvollziehbar bleiben.

## Immutable Versionen

Auch bei Mutable Objects sollen persistierte historische Versionen selbst unveränderlich behandelt werden.

```text
Mutable Object
      ↓
Current State changes

Historical Version
      ↓
Immutable
```

Eine bestehende Version darf nicht rückwirkend verändert werden.

## Wechsel der Semantik

Die Mutability eines bestehenden Objekts darf nicht stillschweigend geändert werden.

Ein Wechsel von Mutable zu Immutable oder umgekehrt benötigt eine explizite Operation beziehungsweise einen neuen Object Contract.

## Relationships

Relationships können unterscheiden, ob sie auf:

```text
Object ID
```

oder auf:

```text
Object ID + Version ID
```

zeigen.

Eine versionsgebundene Referenz bleibt dadurch auf einen unveränderlichen Zustand fixiert.

## Concurrency

Mutable Objects benötigen definierte Regeln für konkurrierende Änderungen.

```text
Current Version
      ↓
Concurrent Changes
      ↓
Conflict Detection
```

Die konkrete Konflikt- und Merge-Semantik wird separat spezifiziert.

## Normative Anforderungen

1. NovaOS MUSS zwischen Immutable und Mutable Objects unterscheiden.
2. Die Mutability MUSS Bestandteil des Object Contracts sein.
3. Immutable Objects DÜRFEN nach ihrer Erzeugung nicht in-place verändert werden.
4. Mutable Objects DÜRFEN ihre stabile Object Identity bei Zustandsänderungen behalten.
5. Persistierte historische Object Versions MÜSSEN unveränderlich sein.
6. Änderungen an Mutable Objects MÜSSEN versionierbar sein können.
7. Die Mutability eines bestehenden Objekts DARF nicht stillschweigend geändert werden.
8. Relationships MÜSSEN zwischen objekt- und versionsgebundenen Referenzen unterscheiden können.
9. Concurrent Mutations MÜSSEN kontrolliert behandelt werden können.

## Konsequenzen

### Positive Konsequenzen

- klare Änderungssemantik,
- historische Zustände bleiben zuverlässig,
- Content Addressing und Provenance werden unterstützt,
- Mutable Objects behalten eine stabile logische Identität.

### Negative Konsequenzen

- Mutable Objects benötigen Concurrency- und Versionierungslogik,
- Object Contracts müssen Mutability explizit definieren.

## Abhängigkeiten

- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-OBJECT-0004_Object_Provenance`
- `ADR-OBJECT-0006_Object_Relationships`
- `ADR-ARCH-0014_Explizite_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-MUTABILITY-0001`
- `NPSPEC-OBJECT-IMMUTABLE-0001`
- `NPSPEC-OBJECT-MUTABLE-0001`

## Ergebnis

NovaOS trennt logische Identität und Veränderbarkeit:

```text
Immutable Object
      ↓
New State = New Object / Version

Mutable Object
      ↓
Stable Object ID
      +
Versioned State
```

Der zentrale Grundsatz lautet:

```text
Unveränderliche Zustände
werden niemals überschrieben.

Veränderliche Objekte
behalten ihre Identität,
aber versionieren ihren Zustand.
```