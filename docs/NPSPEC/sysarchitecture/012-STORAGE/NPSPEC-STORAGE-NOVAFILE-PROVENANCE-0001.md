# NPSPEC-STORAGE-NOVAFILE-PROVENANCE-0001 – NovaFile Provenance

## Status

Angenommen

## Kategorie

Storage / NovaFile / Provenance

## Zweck

NovaOS definiert, wie Herkunfts- und Ableitungsinformationen innerhalb eines NovaFile-Containers gespeichert und mit konkreten Objektversionen verbunden werden.

```text
NovaFile
├── Payload
├── Metadata
└── Provenance
```

Damit kann ein NovaFile seine Herkunft gemeinsam mit seinen eigentlichen Daten transportieren.

## Grundprinzip

```text
Provenance ≠ Trust
Provenance ≠ Permission
Provenance ≠ Object Identity
```

Provenance dokumentiert Herkunft und Verarbeitung, erzeugt aber weder Vertrauen noch Zugriffsrechte.

## Provenance Record

Ein NovaFile darf einen oder mehrere Provenance-Einträge enthalten.

Mindestens:

```text
Operation
Timestamp
Source
```

Optional:

```text
Actor
Capability
Provider
Source ObjectID
Source VersionID
Source ContentID
TransactionID
```

## Versionsbezug

Provenance muss einer konkreten NovaFile-Version zugeordnet werden können.

```text
ObjectID
├── Version 1 → Provenance A
├── Version 2 → Provenance B
└── Version 3 → Provenance C
```

Dadurch bleibt nachvollziehbar, wie einzelne Zustände entstanden sind.

## Herkunft

Typische Herkunftsbeziehungen sind:

```text
CreatedBy
ImportedFrom
DerivedFrom
ConvertedFrom
GeneratedBy
ModifiedBy
CopiedFrom
```

Interne NovaOS-Quellen sollen über stabile `ObjectID`, `VersionID` oder `ContentID` referenziert werden.

## Ableitungsketten

NovaFile darf mehrstufige Herkunftsketten abbilden.

```text
Object A
   ↓
Convert
   ↓
Object B
   ↓
Edit
   ↓
Object C
```

Dabei muss nicht die vollständige Historie in jedem Container dupliziert werden. Stabile Referenzen dürfen auf externe Provenance-Informationen verweisen.

## Externe Herkunft

Importierte NovaFiles dürfen externe Quellen dokumentieren.

```text
External Source
      ↓
Import
      ↓
NovaFile
```

Externe Angaben müssen eindeutig von durch NovaOS selbst bestätigten Provenance-Daten unterscheidbar sein.

## Integrität

Sicherheitsrelevante Provenance soll gegen unbemerkte Veränderung geschützt werden können.

```text
Provenance
    ↓
Hash / Signature
    ↓
Integrity State
```

Eine Signatur bestätigt dabei nur die signierten Informationen und erzeugt keine automatische Vertrauensentscheidung.

## Transaktionen

Neue Provenance-Einträge sollen gemeinsam mit der zugehörigen NovaFile-Änderung gespeichert werden.

```text
Payload / Metadata Change
          +
      Provenance
          ↓
      Transaction
          ↓
        Commit
```

Dadurch sollen Objektzustand und dokumentierte Herkunft konsistent bleiben.

## Projektion und Export

Eine normale Payload-Projektion muss Provenance nicht offenlegen.

Beim Export darf NovaOS abhängig von Policy:

```text
Preserve
Transform
Omit
```

verwenden.

Sicherheits- oder Compliance-relevante Provenance darf nicht unautorisiert entfernt werden.

## Datenschutz

Provenance kann sensible Informationen enthalten.

Beispiele:

```text
User
Application
Source
Workflow
Timestamp
External Location
```

Sichtbarkeit und Speicherung müssen deshalb Security-, Privacy- und Data-Sovereignty-Regeln beachten.

## Untrusted Provenance

Provenance aus externen NovaFiles darf nicht automatisch als verifiziert gelten.

NovaOS muss mindestens unterscheiden können:

```text
Unverified
Verified
Signed
Invalid
Unknown
```

`Unknown` darf nicht als `Verified` behandelt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
VersionID
Origin
Operation
Timestamp
Source References
Integrity State
Verification State
```

Nicht autorisierte Provenance-Daten dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaFile MUSS versionsbezogene Provenance speichern können.
2. Provenance MUSS logisch von Trust, Permissions und Objektidentität getrennt bleiben.
3. Interne Herkunftsbeziehungen SOLLEN stabile Objekt- oder Versionsidentitäten verwenden.
4. NovaFile MUSS externe und interne Provenance unterscheiden können.
5. Externe Provenance DARF NICHT automatisch als verifiziert gelten.
6. `Unknown` DARF NICHT als `Verified` behandelt werden.
7. Provenance SOLL gemeinsam mit der zugehörigen NovaFile-Änderung transaktional gespeichert werden.
8. Sicherheitsrelevante Provenance MUSS gegen unautorisierte Veränderung geschützt werden können.
9. Provenance-Ketten SOLLEN ohne unnötige Duplikation darstellbar sein.
10. Projektionen DÜRFEN geschützte Provenance NICHT unautorisiert offenlegen.
11. Provenance MUSS Privacy- und Data-Sovereignty-Regeln beachten.
12. Herkunft und Verifikationszustand SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROJECTION-0001`
- `NPSPEC-STORAGE-NOVAFILE-SECURITY-0001`
- `ADR-STORAGE-0021`

## Ergebnis

```text
Source
  ↓
Operation
  ↓
NovaFile Version
  ↓
Protected Provenance
  ↓
Traceable Origin
```

NovaFile erhält damit eine integrierte, versionsbezogene Provenance-Struktur, die Herkunft und Ableitung nachvollziehbar macht, ohne Provenance mit Vertrauen oder Berechtigung gleichzusetzen.