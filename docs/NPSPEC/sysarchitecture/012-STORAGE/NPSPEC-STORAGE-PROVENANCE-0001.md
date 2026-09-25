# NPSPEC-STORAGE-PROVENANCE-0001 – Nova Storage Provenance

## Status

Angenommen

## Kategorie

Storage / Provenance / Metadata / Versioning

## Zweck

NovaOS definiert Provenance als nachvollziehbare Herkunfts- und Entstehungshistorie persistenter Storage-Objekte und ihrer Versionen.

```text
Source
  ↓
Operation
  ↓
Object Version
  ↓
Provenance
```

Damit kann nachvollzogen werden, woher Daten stammen, wodurch sie entstanden sind und welche Objekte als Grundlage dienten.

## Grundprinzip

```text
Provenance ≠ Ownership
Provenance ≠ Permission
Provenance ≠ Trust
```

Herkunftsinformation beschreibt die Entstehung eines Objekts, erzeugt aber keine automatische Vertrauens- oder Zugriffsentscheidung.

## Provenance Record

Ein Provenance-Eintrag kann mindestens enthalten:

```text
ObjectID
VersionID
Operation
Source
Timestamp
Actor
```

Optional:

```text
Source ObjectID
Source VersionID
Capability
Provider
TransactionID
Tool / Runtime
```

## Versionsbezug

Provenance muss sich auf konkrete Objektversionen beziehen können.

```text
ObjectID
├── Version 1 → Provenance A
├── Version 2 → Provenance B
└── Version 3 → Provenance C
```

Dadurch bleibt nachvollziehbar, welche Operation zu welchem Zustand geführt hat.

## Herkunftsbeziehungen

NovaOS soll typische Beziehungen ausdrücken können:

```text
CreatedBy
ImportedFrom
DerivedFrom
ConvertedFrom
GeneratedBy
ModifiedBy
CopiedFrom
```

Beziehungen zu Nova-Objekten sollen stabile `ObjectID`- und `VersionID`-Referenzen verwenden.

## Ableitung

Neue Objekte dürfen ihre Herkunft zu bestehenden Objekten dokumentieren.

```text
Object A
   ↓
Transform
   ↓
Object B
   ↓
DerivedFrom A
```

Mehrstufige Ableitungen können dadurch eine Provenance-Kette bilden.

## Externe Quellen

Importierte Daten dürfen externe Herkunftsinformation besitzen.

```text
External Source
      ↓
Import
      ↓
Nova Object
```

Externe Provenance darf beispielsweise Quelle, Importzeitpunkt und Importmechanismus dokumentieren.

Sie darf jedoch nicht automatisch als vertrauenswürdig gelten.

## Transaktionen

Provenance-Änderungen sollen gemeinsam mit der zugehörigen Storage-Änderung transaktional gespeichert werden können.

```text
Data Change
     +
Provenance
     ↓
Transaction
     ↓
Commit
```

Dadurch wird verhindert, dass Objektzustand und Herkunftsinformation auseinanderlaufen.

## Content Addressing

Provenance darf ContentIDs referenzieren.

```text
Source ContentID
      ↓
Transformation
      ↓
Result ContentID
```

Damit kann die Herkunft auch dann nachvollziehbar bleiben, wenn mehrere Objekte denselben Payload verwenden.

## Manipulationsschutz

Sicherheitsrelevante Provenance muss vor unautorisierten Änderungen geschützt werden.

Je nach Objektklasse darf NovaOS:

```text
Sign
Hash
Version
Audit
```

verwenden.

Eine nachträgliche Änderung darf nicht unbemerkt die historische Herkunft verfälschen.

## Datenschutz

Provenance kann sensible Informationen enthalten.

Beispiele:

```text
User Identity
Source Location
Application
Workflow
External Origin
```

Erfassung und Sichtbarkeit müssen daher Security-, Privacy- und Data-Sovereignty-Regeln beachten.

## Aufbewahrung

Provenance darf andere Retention-Regeln besitzen als der eigentliche Payload.

```text
Payload deleted
      ↓
Provenance retained?
```

Dies muss durch Storage-, Security- und Retention-Policy bestimmt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
VersionID
Origin
Operation
Actor
Timestamp
Source Objects
TransactionID
Integrity State
```

Die Ausgabe muss entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Provenance an konkrete Storage-Objekte und Versionen binden können.
2. Provenance MUSS logisch von Ownership, Permissions und Trust getrennt sein.
3. Beziehungen zu Nova-Objekten SOLLEN stabile `ObjectID`- und `VersionID`-Referenzen verwenden.
4. Neue Objektversionen DÜRFEN eigene Provenance-Einträge besitzen.
5. Ableitungen zwischen Objekten SOLLEN nachvollziehbar dargestellt werden können.
6. Externe Quellen MÜSSEN von internen Nova-Objekten unterscheidbar sein.
7. Externe Herkunft DARF NICHT automatisch Vertrauen erzeugen.
8. Provenance SOLL gemeinsam mit der zugehörigen Storage-Änderung transaktional aktualisiert werden.
9. Sicherheitsrelevante Provenance MUSS gegen unautorisierte Veränderung geschützt werden.
10. Provenance-Metadaten MÜSSEN Privacy- und Data-Sovereignty-Regeln beachten.
11. Retention von Provenance MUSS unabhängig vom Payload definierbar sein.
12. Provenance-Ketten und Integritätszustand SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0008`

## Ergebnis

```text
Source Object / External Source
            ↓
         Operation
            ↓
       Object Version
            ↓
        Provenance
            ↓
Traceable History
```

NovaOS erhält damit eine versionsbezogene Provenance-Schicht, mit der Herkunft und Ableitung gespeicherter Daten nachvollziehbar bleiben, ohne Provenance mit Vertrauen, Berechtigung oder Objektidentität gleichzusetzen.