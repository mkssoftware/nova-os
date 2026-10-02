# NPSPEC-UPDATE-PROVENANCE-0001 – Nova Update Provenance

## Status

Angenommen

## Kategorie

Update / Provenance / Supply Chain / Auditability

## Zweck

NovaOS definiert eine durchgängige Herkunfts- und Verarbeitungskette für Updates.

Für jedes Update muss nachvollziehbar sein, woher es stammt, welche Artefakte beteiligt waren, wie es geprüft, verändert, verteilt, installiert und aktiviert wurde und welcher Systemzustand daraus entstanden ist.

```text
Source
  ↓
Build
  ↓
Package
  ↓
Repository
  ↓
Download
  ↓
Verification
  ↓
Installation
  ↓
Activation
  ↓
Runtime State
```

## Grundprinzipien

```text
Provenance ≠ Trust
Provenance ≠ Signature
Provenance ≠ Audit Log
Known Origin ≠ Trusted Origin
Signed Artifact ≠ Known Build Process
Same Version ≠ Same Artifact
Installed Package ≠ Active Component
Provenance Missing ≠ Proven Malicious
```

## Provenance Model

```text
UpdateProvenance
├── ProvenanceID
├── UpdateID
├── PackageID
├── ContentID
├── SourceIdentity
├── BuildIdentity
├── RepositoryID
├── VerificationEvidence
└── ResultState
```

Optional:

```text
SourceRevision
BuildID
ToolchainID
SignerID
DependencySet
TransactionID
StagingID
TargetID
ActivationEvidence
RollbackReference
ParentProvenanceID
Timestamp
```

## Provenance Chain

NovaOS soll eine nachvollziehbare Kette bilden können:

```text
Source Revision
      ↓
Build Environment
      ↓
Build Artifact
      ↓
Signed Package
      ↓
Repository
      ↓
ContentID
      ↓
Update Transaction
      ↓
Installed Component
      ↓
Active Component
```

Jeder Übergang soll auf den vorherigen Zustand referenzieren können.

## Artifact Identity

Versionsnummern reichen nicht zur eindeutigen Herkunftsbestimmung.

```text
Version 5.2
   ├── Build A → ContentID X
   └── Build B → ContentID Y
```

Provenance muss deshalb bevorzugt kryptografische ContentIDs und BuildIDs verwenden.

## Source Provenance

Wenn verfügbar, können erfasst werden:

```text
Source Repository
Revision / Commit
Source ContentID
Build Configuration
Build Inputs
Dependency Versions
```

Damit kann ein Binärartefakt auf seine bekannten Build-Eingaben zurückgeführt werden.

## Build Provenance

Build-Provenance kann enthalten:

```text
BuildID
Toolchain
Compiler Version
Build Configuration
Target Architecture
Dependencies
Build Environment
Build Timestamp
Reproducibility Evidence
```

```text
Known Build ≠ Reproducible Build
```

## Repository Provenance

Beim Bezug eines Updates sollen festgehalten werden:

```text
RepositoryID
Channel
Metadata Version
PackageID
ContentID
Signer
Acquisition Time
```

Mirror oder Netzwerkadresse dürfen die logische Herkunft nicht ersetzen.

## Verification Provenance

Verifikationsergebnisse werden mit der Provenance verbunden.

```text
Artifact
   ↓
VerificationID
   ↓
Evidence
   ↓
Result
```

Dadurch bleibt nachvollziehbar, welches konkrete Artefakt tatsächlich geprüft wurde.

## Transaction Provenance

Update-Transaktionen müssen ihre Herkunftskette erhalten.

```text
UpdateID
   ↓
TransactionID
   ↓
StagingID
   ↓
Activation
   ↓
Verification
```

Rollback, Retry oder Recovery erzeugen neue Ereignisse, ohne die vorherige Historie zu überschreiben.

## Runtime Binding

NovaOS soll den aktiven Zustand auf Update-Provenance zurückführen können.

```text
Running Component
      ↓
BuildID
      ↓
ContentID
      ↓
PackageID
      ↓
UpdateID
      ↓
ProvenanceID
```

Damit kann festgestellt werden, durch welchen Update-Vorgang eine aktive Komponente entstanden ist.

## Dependency Provenance

Auch relevante Dependencies sollen nachvollziehbar bleiben.

```text
Package A
├── Dependency B → Provenance B
└── Dependency C → Provenance C
```

Die Provenance eines zusammengesetzten Systems kann dadurch als Graph dargestellt werden.

## Rollback Provenance

Rollback darf Historie nicht löschen.

```text
v1
 ↓
Update → v2
 ↓
Failure
 ↓
Rollback → v1'
```

`v1'` ist ein neuer Systemzustand mit eigener Provenance, auch wenn sein Inhalt einem früheren Zustand entspricht.

## A/B und Snapshot

Provenance muss auch bei:

```text
A/B Switching
Snapshots
Immutable Generations
Rolling Updates
Canary Updates
Live Updates
Hotpatches
```

erhalten bleiben.

## Security

Provenance-Daten können sicherheitskritisch sein und müssen gegen unautorisierte Manipulation geschützt werden.

```text
Provenance Integrity
≠
Artifact Trust
```

Trust Policy entscheidet weiterhin separat, ob eine bekannte Herkunft akzeptiert wird.

## Unvollständige Provenance

Legacy- oder externe Komponenten können unvollständige Herkunftsinformationen besitzen.

Mögliche Zustände:

```text
Complete
Partial
Unavailable
Invalid
Unknown
```

```text
Unknown Provenance
≠
Trusted Provenance
```

Sicherheitskritische Policies dürfen vollständige Provenance verlangen.

## Privacy und Minimierung

Provenance darf nicht unnötig personenbezogene oder geheime Build-Informationen offenlegen.

Es sollen nur für Nachvollziehbarkeit, Sicherheit und Reproduzierbarkeit erforderliche Daten gespeichert werden.

## Retention

Provenance darf nicht entfernt werden, solange sie für folgende Zustände benötigt wird:

```text
Active Component
Rollback
Recovery
Snapshot
A/B Slot
Audit
Security Investigation
Verification Evidence
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Installed Version
BuildID
ContentID
PackageID
RepositoryID
SignerID
UpdateID
TransactionID
Verification Evidence
Activation History
Rollback History
Provenance Completeness
```

## Normative Anforderungen

1. NovaOS MUSS Update-Provenance systemweit erfassen können.
2. Provenance MUSS eindeutig identifizierbar sein.
3. Provenance SOLL Source, Build, Package, Repository, Verification und Activation verbinden können.
4. Versionsnummern DÜRFEN NICHT als alleinige Artefaktidentität verwendet werden.
5. ContentIDs SOLLEN zur kryptografischen Artefaktbindung verwendet werden.
6. BuildIDs SOLLEN Build-Artefakte eindeutig identifizieren.
7. Repository-Herkunft MUSS unabhängig von Mirror oder Netzwerkadresse modellierbar sein.
8. Verifikationsevidence MUSS dem tatsächlich geprüften Artefakt zugeordnet werden können.
9. Update-Transaktionen MÜSSEN Provenance erhalten.
10. Der aktive Systemzustand SOLL auf seine Update-Herkunft zurückführbar sein.
11. Relevante Dependencies SOLLEN eigene Provenance besitzen.
12. Rollback DARF bestehende Provenance-Historie NICHT überschreiben.
13. Wiederhergestellte Zustände MÜSSEN neue Provenance erzeugen können.
14. A/B-, Snapshot-, Immutable-, Rolling- und Canary-Updates MÜSSEN Provenance erhalten können.
15. Live Updates und Hotpatches MÜSSEN ihre Herkunft dokumentieren können.
16. Provenance-Daten MÜSSEN gegen unautorisierte Manipulation geschützt werden.
17. Provenance DARF NICHT automatisch als Trust-Entscheidung interpretiert werden.
18. Unvollständige Provenance MUSS explizit darstellbar sein.
19. `Unknown` DARF NICHT als vollständige Provenance interpretiert werden.
20. Security Policies DÜRFEN vollständige Provenance verlangen.
21. Provenance SOLL datensparsam gespeichert werden.
22. Recovery-relevante Provenance DARF NICHT vorzeitig entfernt werden.
23. Provenance MUSS versionierbar und erweiterbar sein.
24. Unbekannte optionale Provenance-Felder SOLLEN erhalten werden können.
25. Kritische unbekannte Pflichtfelder MÜSSEN zur Ablehnung führen können.
26. Update-Provenance MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-REPOSITORY-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-CONTENTADDRESS-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-VERIFY-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `ADR-ARCH-0185`

## Ergebnis

```text
Source
  ↓
Build
  ↓
BuildID
  ↓
Package + ContentID
  ↓
Repository
  ↓
Verification
  ↓
Update Transaction
  ↓
Activation
  ↓
Runtime Component
  ↓
Provenance Chain
```

NovaOS erhält damit eine durchgängige Herkunftskette für Updates. Für aktive Systemkomponenten kann nachvollzogen werden, aus welchen Quellen und Builds sie entstanden sind, über welche Update-Pfade sie in das System gelangten, welche Prüfungen durchgeführt wurden und welche späteren Rollback-, Recovery- oder Aktivierungsereignisse ihren aktuellen Zustand erzeugt haben.