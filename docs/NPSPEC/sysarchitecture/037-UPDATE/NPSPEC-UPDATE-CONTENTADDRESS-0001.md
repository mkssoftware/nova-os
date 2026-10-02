# NPSPEC-UPDATE-CONTENTADDRESS-0001 – Nova Content-Addressed Updates

## Status

Angenommen

## Kategorie

Update / Content Addressing / Distribution

## Zweck

NovaOS verwendet Content Addressing, um Update-Artefakte anhand ihres tatsächlichen kryptografischen Inhalts statt anhand von Dateiname, URL oder Speicherort eindeutig zu identifizieren.

```text
Content
   ↓
Cryptographic Hash
   ↓
ContentID
```

Damit können identische Artefakte erkannt, sicher gecacht, dedupliziert, verteilt und eindeutig mit Builds, Paketen und Verification Evidence verbunden werden.

## Grundprinzipien

```text
ContentID ≠ PackageID
ContentID ≠ ObjectID
ContentID ≠ Version
ContentID ≠ Filename
ContentID ≠ URL
ContentID ≠ Trust
Same ContentID → Same Content
Different Content → Different ContentID
```

## Content Identity

```text
ContentIdentity
├── Algorithm
├── Digest
└── ContentID
```

Beispiel:

```text
ContentID = Hash(ArtifactBytes)
```

Die konkrete Hash-Funktion muss über Crypto Agility austauschbar bleiben.

## Update Package Integration

Ein Update-Paket kann mehrere ContentIDs referenzieren:

```text
PackageID
├── Manifest ContentID
├── Kernel ContentID
├── Driver ContentID
├── Module ContentID
└── Resource ContentID
```

`PackageID` beschreibt die logische Paketidentität.

`ContentID` beschreibt den konkreten Inhalt.

## Repository Integration

Repositories sollen Artefakte über ContentIDs bereitstellen können.

```text
Package Metadata
      ↓
ContentID
      ↓
Repository / Mirror / Cache
      ↓
Artifact
```

Der physische Download-Ort ist dadurch von der Artefaktidentität getrennt.

## Mirror Independence

Dasselbe Artefakt darf von unterschiedlichen Quellen geladen werden:

```text
Mirror A ─┐
Mirror B ─┼→ ContentID X
Cache    ─┘
```

Nach dem Download wird der Inhalt gegen die erwartete ContentID geprüft.

```text
Transport Source ≠ Content Identity
```

## Integrity Verification

```text
Downloaded Artifact
       ↓
Calculate Hash
       ↓
Expected ContentID
       ↓
Match?
├── Yes → Accept Content
└── No  → Reject Content
```

Ein ContentID-Mismatch muss als Integritätsfehler behandelt werden.

## Trust Separation

Content Addressing bestätigt Inhalt, aber nicht dessen Vertrauenswürdigkeit.

```text
ContentID Valid
      ↓
Signature Verification
      ↓
Trust Policy
      ↓
Authorization
```

```text
Integrity ≠ Authenticity
Authenticity ≠ Authorization
```

## Deduplication

Identische Update-Artefakte müssen nicht mehrfach gespeichert werden.

```text
Package A ─┐
Package B ─┼→ ContentID X → Stored Once
Package C ─┘
```

Dies kann Repository-, Cache- und lokalen Speicherbedarf reduzieren.

## Cache

Der Update Cache darf ContentIDs als primären Schlüssel verwenden.

```text
ContentID
   ↓
Local Cache
```

Ist ein Artefakt bereits vorhanden, kann ein erneuter Download entfallen.

Vor Verwendung muss seine Integrität weiterhin überprüfbar sein.

## Delta Updates

Delta Updates referenzieren eindeutige Ausgangs- und Zielinhalte:

```text
BaseContentID
      +
Delta
      ↓
TargetContentID
```

Nach Rekonstruktion muss gelten:

```text
Hash(ReconstructedTarget)
=
TargetContentID
```

## Build Binding

ContentIDs verbinden ausgelieferte Artefakte mit konkreten Builds.

```text
Source
 ↓
BuildID
 ↓
Artifact
 ↓
ContentID
```

Dadurch kann NovaOS feststellen, ob das tatsächlich installierte Artefakt dem erwarteten Build entspricht.

## Verification Binding

Verification Evidence kann an ContentIDs gebunden werden.

```text
VerificationID
      ↓
Verified Artifact
      ↓
ContentID
```

Damit wird verhindert, dass ein Verifikationsergebnis versehentlich auf einen anderen Inhalt übertragen wird.

```text
Same Filename ≠ Same Verified Artifact
```

## Atomic und Transactional Updates

Content-addressierte Artefakte werden zunächst bereitgestellt und validiert:

```text
Acquire Content
      ↓
Verify ContentID
      ↓
Stage
      ↓
Transactional Update
      ↓
Atomic Activation
```

Unverifizierte Inhalte dürfen nicht in den Commit übernommen werden.

## Garbage Collection

Nicht mehr referenzierte Inhalte dürfen entfernt werden.

Vor Entfernung muss NovaOS berücksichtigen:

```text
Installed Packages
Active Transactions
Snapshots
Rollback Targets
A/B Slots
Recovery State
Pinned Content
```

```text
Unreferenced ≠ Immediately Deletable
```

Ein für Recovery benötigtes Artefakt darf nicht durch normale Cache-Bereinigung entfernt werden.

## Algorithm Agility

ContentIDs müssen den verwendeten Hash-Algorithmus eindeutig bestimmen.

Konzeptionell:

```text
ContentID
├── AlgorithmID
└── Digest
```

Dadurch können Hash-Verfahren migriert werden, ohne das Content-Addressing-Modell zu verändern.

## Collision Handling

Eine erkannte Hash-Kollision oder kryptografisch nicht mehr ausreichende Hash-Funktion muss als Security-Ereignis behandelt werden.

NovaOS muss betroffene Algorithmen über Crypto Policy deaktivieren oder ersetzen können.

## Provenance

NovaOS soll Beziehungen nachvollziehen können:

```text
PackageID
   ↓
BuildID
   ↓
ContentID
   ↓
VerificationID
   ↓
Installed State
```

Downloadquelle und Content Identity bleiben getrennte Provenance-Eigenschaften.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ContentID
Algorithm
Artifact Type
Size
Package References
BuildID
VerificationID
Cache State
Integrity State
Reference Count
Recovery Relevance
```

## Normative Anforderungen

1. NovaOS MUSS Update-Artefakte content-addressiert identifizieren können.
2. ContentIDs MÜSSEN kryptografische Hash-Verfahren verwenden.
3. Der verwendete Hash-Algorithmus MUSS aus der ContentID bestimmbar sein.
4. ContentID MUSS unabhängig von Dateiname, URL und Speicherort sein.
5. PackageID und ContentID MÜSSEN getrennte Identitäten bleiben.
6. Heruntergeladene Artefakte MÜSSEN gegen ihre erwartete ContentID prüfbar sein.
7. ContentID-Mismatch MUSS zur Ablehnung des Artefakts führen.
8. Content Addressing DARF NICHT als Trust-Entscheidung interpretiert werden.
9. Signatur-, Trust- und Authorization-Prüfungen MÜSSEN separat bleiben.
10. Identische ContentIDs SOLLEN deduplizierbar sein.
11. ContentIDs SOLLEN als Schlüssel für Update-Caches verwendbar sein.
12. Mirrors DÜRFEN denselben Inhalt unabhängig bereitstellen.
13. Die Downloadquelle DARF die erwartete ContentID NICHT verändern.
14. Delta Updates MÜSSEN BaseContentID und TargetContentID eindeutig referenzieren können.
15. Rekonstruierte Delta-Ziele MÜSSEN gegen TargetContentID geprüft werden.
16. ContentIDs SOLLEN mit BuildID verknüpfbar sein.
17. Verification Evidence SOLL mit dem tatsächlich geprüften ContentID verknüpfbar sein.
18. Unverifizierte Inhalte DÜRFEN NICHT in kritische Update-Commits übernommen werden.
19. Garbage Collection MUSS aktive Update-, Snapshot-, A/B-, Rollback- und Recovery-Referenzen berücksichtigen.
20. Recovery-relevante Inhalte DÜRFEN NICHT vorzeitig entfernt werden.
21. Content Addressing MUSS Crypto Agility unterstützen.
22. Unsichere Hash-Algorithmen MÜSSEN deaktivierbar oder migrierbar sein.
23. Erkannte Hash-Kollisionen MÜSSEN als Security-Ereignis behandelt werden.
24. Content-Provenance MUSS nachvollziehbar sein.
25. Content- und Integrity-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-REPOSITORY-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-DELTA-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0174`

## Ergebnis

```text
Update Metadata
      ↓
Expected ContentID
      ↓
Repository / Mirror / Cache
      ↓
Acquire Artifact
      ↓
Calculate ContentID
      ↓
Match?
├── No  → Reject
└── Yes
      ↓
Signature + Trust
      ↓
Stage
      ↓
Transactional Update
```

NovaOS erhält damit eine speicherort- und transportunabhängige Identität für Update-Artefakte. Pakete, Mirrors, Caches, Delta Updates, Builds und Verification Evidence können sich auf exakt denselben Inhalt beziehen, während Integrität, Authentizität, Trust und Authorization weiterhin als getrennte Sicherheitsentscheidungen behandelt werden.