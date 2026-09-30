# NPSPEC-OBJECT-PROVENANCE-0001 – Nova Object Provenance

## Status

Angenommen

## Kategorie

Object / Provenance / Architecture

## Zweck

NovaOS definiert ein einheitliches Provenance-Modell, mit dem Herkunft, Erzeugung, Veränderung, Ableitung und Verarbeitung von Objekten nachvollziehbar beschrieben werden können.

```text
Source Object
     ↓
Operation
     ↓
Result Object / Version
     ↓
Provenance Record
```

Provenance beschreibt die Geschichte eines Objekts, ohne daraus automatisch Vertrauen oder Autorität abzuleiten.

## Grundprinzipien

```text
Provenance ≠ Trust
Provenance ≠ Authority
Known Origin ≠ Trusted Origin
CreatedBy ≠ Owner
DerivedFrom ≠ Same Object
Version History ≠ Complete Provenance
Missing Provenance ≠ Invalid Object
Signed Provenance ≠ Trusted Content
```

## Provenance-Modell

Ein Provenance-Eintrag besitzt mindestens:

```text
ObjectProvenance
├── ProvenanceID
├── ObjectID
├── Operation
└── Timestamp
```

Optional:

```text
VersionID
Source ObjectIDs
Source VersionIDs
Actor Identity
CapabilityID
ProviderID
TransactionID
ExecutionID
Tool / Component
Semantic Conversion
Environment
Signature
Trust Evidence
```

## Provenance-Ereignisse

Typische Ereignisse sind:

```text
Created
Imported
Copied
Modified
Converted
Generated
Derived
Merged
Restored
Migrated
Exported
```

Objekttypen dürfen zusätzliche Ereignistypen definieren.

## Ableitungen

Neue Objekte können explizit mit ihren Quellen verbunden werden.

```text
Object B
   ↓ DerivedFrom
Object A
```

Dabei besitzen beide Objekte eigene `ObjectID`s.

```text
ObjectID A ≠ ObjectID B
```

## Versionen

Provenance kann auf konkrete Versionen verweisen.

```text
Object A:v2
     ↓ ModifiedFrom
Object A:v1
```

Dadurch kann zwischen Objektgeschichte und Versionsgeschichte unterschieden werden.

## Mehrere Quellen

Ein Objekt kann aus mehreren Quellen entstehen.

```text
Object A ─┐
          ├── Merge → Object C
Object B ─┘
```

Der Provenance Record muss mehrere Source References unterstützen können.

## Operationen

Provenance soll beschreiben, welche Operation zu einem Ergebnis geführt hat.

```text
Source
  ↓
OperationID
  ↓
Provider
  ↓
Result
```

Bei Semantic Execution können zusätzlich gespeichert werden:

```text
ExecutionID
ExecutionContract Reference
Conversion Path
ProviderID
```

## Transaktionen

Provenance muss mit Transaktionen integrierbar sein.

```text
Begin Transaction
      ↓
Object Changes
      ↓
Provenance Records
      ↓
Commit
```

Objektzustand und zugehörige Provenance dürfen bei transaktionalen Änderungen nicht widersprüchlich veröffentlicht werden.

## Beziehungen

Provenance verwendet Semantic Relationships wie:

```text
CreatedBy
DerivedFrom
GeneratedFrom
ImportedFrom
ModifiedFrom
VersionOf
MergedFrom
```

Die Provenance-Schicht ergänzt diese Beziehungen um Operations- und Kontextinformationen.

## Trust

Provenance liefert Evidence für Trust Evaluation.

```text
Provenance
    ↓
Trust Policy
    ↓
Trust Evaluation
```

NovaOS darf aus vorhandener Provenance nicht automatisch Vertrauen ableiten.

Unterbrochene oder unbekannte Provenance muss explizit darstellbar sein.

```text
Complete
Partial
Unknown
```

## Integrität

Provenance Records sollen gegen unbemerkte Manipulation geschützt werden können.

Mögliche Mechanismen:

```text
Hash
Signature
Authenticated Storage
Hash Chain
Transaction Integrity
```

Kryptografische Integrität bestätigt jedoch nicht automatisch die Vertrauenswürdigkeit des Inhalts.

## Datenschutz

Provenance kann sensible Informationen enthalten.

Beispiele:

```text
User Identity
Device Identity
Location
Provider
Timestamp
Processing History
```

Zugriff und Weitergabe müssen deshalb durch Privacy-, Security- und Capability-Regeln kontrolliert werden.

## Export

Beim Export eines Objekts muss definiert werden, welche Provenance mitgegeben wird.

```text
Preserve
Reduce
Transform
Remove
```

Sicherheitsrelevante Provenance darf nicht unkontrolliert entfernt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ProvenanceID
ObjectID
VersionID
Source Objects
Operation
Actor
Provider
Timestamp
TransactionID
ExecutionID
Provenance State
```

## Normative Anforderungen

1. NovaOS MUSS Provenance unabhängig von Objektidentität modellieren.
2. Provenance SOLL auf stabile `ObjectID`s und `VersionID`s verweisen.
3. Ableitungen aus mehreren Quellen MÜSSEN darstellbar sein.
4. Provenance MUSS Operationen und deren Ergebnisse miteinander verbinden können.
5. Provenance MUSS mit Object Versioning und Transactions integrierbar sein.
6. Provenance DARF NICHT automatisch Trust oder Authority erzeugen.
7. Unvollständige oder unbekannte Provenance MUSS explizit darstellbar sein.
8. Provenance Records SOLLEN gegen unbemerkte Manipulation geschützt werden können.
9. Zugriff auf sensible Provenance MUSS durch Capabilities und Privacy-Regeln kontrollierbar sein.
10. Provenance MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0025`

## Ergebnis

```text
Object / Version
      ↓
Provenance Records
      ↓
Sources + Operations + Actors
      ↓
Traceable Object History
      ↓
Trust / Audit / Introspection
```

NovaOS erhält damit eine systemweite Provenance-Schicht, durch die nachvollziehbar bleibt, woher ein Objekt stammt, aus welchen Quellen es entstanden ist und durch welche Operationen, Versionen und Komponenten es verändert wurde.