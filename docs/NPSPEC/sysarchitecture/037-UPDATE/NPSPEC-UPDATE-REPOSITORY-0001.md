# NPSPEC-UPDATE-REPOSITORY-0001 – Nova Update Repository

## Status

Angenommen

## Kategorie

Update / Repository / Distribution

## Zweck

NovaOS definiert Update Repositories als vertrauenswürdige Quellen für Update-Pakete, Metadaten und Verification Evidence.

```text
Repository
    ↓
Metadata
    ↓
Package Discovery
    ↓
Trust + Integrity
    ↓
Nova Update Manager
```

Ein Repository stellt Updates bereit, erhält jedoch keine direkte Installations- oder Ausführungs-Authority.

## Grundprinzipien

```text
Repository ≠ Authority
Repository Reachable ≠ Repository Trusted
Package Available ≠ Package Authorized
Mirror ≠ Trust Anchor
Metadata ≠ Package
Newest ≠ Allowed
Downloaded ≠ Trusted
Repository Compromise ≠ Automatic System Compromise
```

## Repository Model

```text
Repository
├── RepositoryID
├── RepositoryType
├── Metadata
├── PackageIndex
├── TrustRequirements
└── UpdatePolicy
```

Optional:

```text
Mirrors
Channels
Architectures
Capabilities
SovereigntyPolicy
Priority
Expiration
ProvenanceID
```

## Repository Identity

Repositories besitzen eine stabile Identität.

```text
RepositoryID ≠ URL
RepositoryID ≠ DNS Name
RepositoryID ≠ Mirror
```

Der physische Standort eines Repositorys darf sich ändern, ohne seine logische Identität zu verändern.

## Repository Types

NovaOS soll unterschiedliche Quellen unterstützen:

```text
Official
Enterprise
Organization
Developer
Local
Offline
Recovery
Mirror
```

Der Repository-Typ allein bestimmt nicht dessen Authority.

## Repository Metadata

Repository-Metadaten beschreiben mindestens:

```text
RepositoryID
MetadataVersion
AvailablePackages
PackageVersions
ContentIDs
Dependencies
Signatures
Expiration
```

Metadaten müssen gegen Manipulation geschützt sein.

## Package Index

Der Package Index ermöglicht Discovery ohne vollständigen Paketdownload.

```text
PackageID
Version
Target
Architecture
Capabilities
Dependencies
Size
ContentID
```

Discovery erzeugt keine Installationsberechtigung.

## Trust

Repository Trust wird über Nova Trust Policy bestimmt.

```text
Repository
    ↓
Identity
    ↓
Metadata Signature
    ↓
Trust Policy
    ↓
Accepted / Rejected
```

Pakete müssen unabhängig von der Repository-Verbindung selbst validierbar bleiben.

```text
Trusted Repository ≠ Every Package Trusted
```

## Repository Compromise

Die Kompromittierung eines Repository-Servers darf nicht automatisch die Installation manipulierten Codes ermöglichen.

Dafür werden getrennt geprüft:

```text
Repository Metadata
Package Integrity
Package Signature
Publisher Trust
Package Policy
```

## Metadata Expiration

Signierte Metadaten können eine Gültigkeitsdauer besitzen.

```text
ValidFrom
ExpiresAt
```

Abgelaufene Metadaten dürfen für sicherheitskritische Entscheidungen nicht stillschweigend als aktuell behandelt werden.

## Rollback Protection

Ein Repository darf NovaOS nicht unbemerkt auf veraltete Metadaten oder verwundbare Paketstände zurücksetzen.

```text
Current Metadata v12
        ↓
Repository serves v8
        ↓
Rollback Detected
```

Monotone Security States und Revocation Information dürfen nicht durch ältere Repository-Daten überschrieben werden.

## Freeze Protection

NovaOS soll erkennen können, wenn ein Repository dauerhaft alte, aber formal noch gültige Informationen liefert.

Dazu können verwendet werden:

```text
Metadata Version
Expiration
Trusted Timestamp
Known Latest Generation
Repository Policy
```

## Mirrors

Repositories dürfen Mirrors verwenden.

```text
RepositoryID
├── Mirror A
├── Mirror B
└── Mirror C
```

Mirrors transportieren Inhalte, übernehmen aber nicht automatisch die Rolle eines Trust Anchors.

Pakete und Metadaten müssen unabhängig vom Mirror überprüfbar sein.

## Channels

Repositories dürfen Update Channels anbieten:

```text
Stable
Testing
Development
LTS
Security
Custom
```

Channel-Wechsel muss eine explizite Policy- oder Benutzerentscheidung sein können.

```text
Channel ≠ Trust Level
```

## Multiple Repositories

NovaOS darf mehrere Repositories gleichzeitig verwenden.

```text
Repository A
Repository B
Repository C
      ↓
Unified Discovery
      ↓
Dependency Resolution
```

Konflikte müssen explizit aufgelöst werden.

Repository-Priorität darf Security- und Trust-Regeln nicht umgehen.

## Package Selection

Ein Paket darf nur berücksichtigt werden, wenn mindestens gilt:

```text
Repository Accepted
Package Metadata Valid
Package Integrity Valid
Publisher Accepted
Compatibility Satisfied
Update Policy Allows
```

## Offline Repositories

NovaOS muss Offline-Updatequellen unterstützen können.

Beispiele:

```text
USB
Local Storage
Recovery Partition
Installation Media
Enterprise Media
```

Offline bedeutet nicht automatisch vertrauenswürdig.

Die gleichen Integrity- und Trust-Regeln gelten weiterhin.

## Repository Updates

Änderungen an Repository-Konfigurationen müssen kontrolliert erfolgen.

```text
Add
Remove
Enable
Disable
Change Channel
Change Mirror
Change Trust Association
```

Das Hinzufügen einer Quelle darf keine implizite Trust-Authority erzeugen.

## Sovereignty

Repository-Auswahl darf Sovereignty-Anforderungen berücksichtigen.

Beispiele:

```text
Allowed Region
Allowed Organization
Local-Only
Enterprise-Controlled
Offline-Only
```

Location Transparency darf diese Regeln nicht umgehen.

## Caching

Repository-Daten und Pakete dürfen lokal gecacht werden.

```text
Repository
    ↓
Cache
    ↓
Update Manager
```

Cache-Inhalte müssen weiterhin anhand ihrer Version, Integrität, Signatur und Gültigkeit geprüft werden.

```text
Cached ≠ Current
```

## Availability

Repository-Ausfall darf nicht automatisch das laufende System beeinträchtigen.

```text
Repository Unavailable
        ↓
Installed System Continues
```

Falls kein sicherer aktueller Repository-Zustand bestimmt werden kann:

```text
RepositoryState = Unknown
```

```text
Unknown ≠ Trusted Current State
```

## Provenance

NovaOS soll nachvollziehen können:

```text
RepositoryID
PackageID
PackageVersion
Publisher
ContentID
Mirror
DiscoveryTime
DownloadSource
TrustDecision
```

Die Downloadquelle bleibt von der Publisher-Identität getrennt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
RepositoryID
Repository Type
Enabled State
Channels
Mirrors
Metadata Version
Expiration
Trust State
Available Packages
Last Refresh
Last Successful Validation
Repository Health
```

## Normative Anforderungen

1. NovaOS MUSS mehrere Update Repositories unterstützen können.
2. Repository Identity MUSS vom Netzwerkstandort unabhängig sein.
3. Repository Discovery DARF keine Installations-Authority erzeugen.
4. Repository-Metadaten MÜSSEN gegen Manipulation geschützt sein.
5. Sicherheitskritische Repository-Metadaten MÜSSEN authentifizierbar sein.
6. Pakete MÜSSEN unabhängig vom Transportweg auf Integrität geprüft werden.
7. Trusted Repository DARF NICHT automatisch Trusted Package bedeuten.
8. Publisher Trust MUSS separat prüfbar bleiben.
9. Mirrors DÜRFEN NICHT automatisch Trust Anchors werden.
10. Metadata Expiration MUSS unterstützt werden.
11. Veraltete Metadata DARF NICHT stillschweigend als aktuell behandelt werden.
12. Repository Rollback MUSS erkennbar sein können.
13. Security- und Revocation-State DARF NICHT durch ältere Repository-Daten zurückgesetzt werden.
14. Freeze-Angriffe SOLLEN erkennbar sein.
15. Update Channels MÜSSEN unterstützt werden können.
16. Channel-Wechsel SOLL explizit kontrollierbar sein.
17. Repository-Prioritäten DÜRFEN Security- und Trust-Regeln NICHT umgehen.
18. Konflikte zwischen mehreren Repositories MÜSSEN explizit auflösbar sein.
19. Offline Repositories MÜSSEN unterstützt werden können.
20. Offline-Quellen MÜSSEN denselben Integrity- und Trust-Regeln unterliegen.
21. Das Hinzufügen eines Repositorys DARF NICHT automatisch Trust erzeugen.
22. Repository-Auswahl MUSS Sovereignty-Regeln berücksichtigen können.
23. Cache-Inhalte MÜSSEN vor Verwendung erneut validierbar sein.
24. Repository-Ausfall DARF das bereits installierte System NICHT unnötig beeinträchtigen.
25. `Unknown` DARF NICHT als vertrauenswürdiger aktueller Repository-Zustand interpretiert werden.
26. Repository- und Package-Provenance MÜSSEN getrennt nachvollziehbar sein.
27. Repository-Zustand MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-TRUST-REVOCATION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `ADR-ARCH-0166`

## Ergebnis

```text
Repository
    ↓
Authenticate Metadata
    ↓
Discover Packages
    ↓
Select Candidate
    ↓
Verify Package + Publisher
    ↓
Resolve Dependencies
    ↓
Check Policy + Compatibility
    ↓
Nova Update Manager
    ↓
Transactional Update
```

NovaOS erhält damit eine verteilte und transportunabhängige Repository-Architektur, bei der Repository, Mirror, Publisher, Paket und Trust getrennte Rollen besitzen und ein kompromittierter oder veralteter Repository-Server nicht automatisch die Update-Sicherheitskette brechen kann.