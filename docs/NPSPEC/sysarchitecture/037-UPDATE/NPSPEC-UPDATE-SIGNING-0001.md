# NPSPEC-UPDATE-SIGNING-0001 – Nova Update Signing

## Status

Angenommen

## Kategorie

Update / Signing / Supply Chain Security

## Zweck

NovaOS definiert ein einheitliches Signaturmodell für Update-Pakete, Repository-Metadaten und sicherheitskritische Update-Artefakte.

```text
Artifact
   ↓
Content Identity
   ↓
Digital Signature
   ↓
Signer Identity
   ↓
Trust Policy
   ↓
Authorization Decision
```

Digitale Signaturen sichern Herkunft und Integrität, erzeugen jedoch nicht automatisch Vertrauen oder Installationsberechtigung.

## Grundprinzipien

```text
Signed ≠ Trusted
Trusted Signer ≠ Authorized Update
Valid Signature ≠ Compatible Package
Signature ≠ Integrity of Running State
Repository Signature ≠ Package Signature
Transport Security ≠ Artifact Authenticity
Old Valid Signature ≠ Currently Authorized
```

## Signing Model

```text
SignedArtifact
├── ArtifactID
├── ArtifactType
├── ContentID
├── Signature
├── SigningKeyID
├── SignerIdentity
└── SignatureMetadata
```

Optional:

```text
Algorithm
Timestamp
ValidityPeriod
PublisherID
BuildID
PackageID
RepositoryID
VerificationID
ProvenanceID
```

## Signierbare Artefakte

Mindestens folgende Artefakte müssen signierbar sein:

```text
Update Package
Package Manifest
Repository Metadata
Update Index
Kernel Image
Driver
System Module
Recovery Component
Migration Component
Security Policy
```

## Content Binding

Die Signatur muss eindeutig an den signierten Inhalt gebunden sein.

```text
Artifact
   ↓
Cryptographic Hash
   ↓
ContentID
   ↓
Signature
```

Nach einer Inhaltsänderung muss die Signatur ungültig werden.

## Package Signing

Bei Update-Paketen müssen mindestens folgende Informationen kryptografisch gebunden werden:

```text
PackageID
PackageVersion
Manifest
Payload ContentIDs
Target
Publisher
```

Dadurch darf ein Payload nicht gegen einen anderen ausgetauscht werden, ohne die Signaturprüfung zu verletzen.

## Repository Signing

Repository-Metadaten werden getrennt von Paketen signiert.

```text
Repository Metadata Signature
            ↓
      Package Discovery

Package Signature
            ↓
      Package Authenticity
```

Ein kompromittierter Repository-Server darf keine gültigen neuen Pakete erzeugen können, wenn er keinen autorisierten Package-Signing-Key besitzt.

## Signer Identity

Signaturschlüssel müssen einer nachvollziehbaren Identität zugeordnet werden können.

```text
SigningKeyID
     ↓
SignerIdentity
     ↓
Trust Policy
```

Mögliche Signer:

```text
NovaOS Project
Hardware Vendor
Driver Vendor
Organization
Enterprise Administrator
Authorized Developer
Recovery Authority
```

## Trust Decision

Nach erfolgreicher kryptografischer Prüfung folgt eine separate Trust-Entscheidung.

```text
Signature Valid
      ↓
Signer Known?
      ↓
Signer Trusted?
      ↓
Authorized for Artifact Type?
      ↓
Policy Allows?
      ↓
Accepted
```

Damit kann ein Schlüssel beispielsweise für Treiber autorisiert sein, jedoch nicht für Kernel-Updates.

## Signing Scope

Schlüssel müssen auf definierte Bereiche beschränkt werden können.

Beispiele:

```text
Kernel
Driver
Repository Metadata
Recovery
Applications
Organization Packages
Specific PackageID
```

```text
Trusted Key ≠ Unlimited Signing Authority
```

## Multiple Signatures

NovaOS soll mehrere Signaturen pro Artefakt unterstützen können.

```text
Artifact
├── Signature A
├── Signature B
└── Signature C
```

Policies können beispielsweise verlangen:

```text
Any Trusted Signer
Specific Signer
Multiple Required Signers
Vendor + NovaOS
Organization Approval
```

## Key Rotation

Signing Keys müssen kontrolliert austauschbar sein.

```text
Key A
  ↓
Authorize Key B
  ↓
Transition
  ↓
Revoke Key A
```

Key Rotation muss die Update-Kette erhalten, ohne dauerhaft alte Schlüssel akzeptieren zu müssen.

## Revocation

Kompromittierte oder nicht mehr autorisierte Schlüssel müssen widerrufbar sein.

```text
SigningKey
   ↓
Revoked
   ↓
Future Acceptance Blocked
```

Bereits vorhandene Signaturen müssen gegen den aktuellen Revocation State bewertet werden können.

```text
Historically Valid ≠ Currently Authorized
```

## Expiration

Schlüssel und Signaturen dürfen Gültigkeitszeiträume besitzen.

```text
ValidFrom
ValidUntil
```

Abgelaufene Credentials dürfen nicht stillschweigend als aktuell gültig behandelt werden.

## Rollback Protection

Eine kryptografisch gültige ältere Version darf nicht automatisch installiert werden.

```text
Valid Signature
      +
Old Vulnerable Version
      ↓
Rollback Policy
      ↓
Reject
```

Signaturprüfung ersetzt keine Version-, Revocation- oder Security-State-Prüfung.

## Algorithm Agility

Das Signing-System muss kryptografische Algorithmen austauschbar halten.

```text
Signing Interface
      ↓
Algorithm Provider
```

Algorithmen dürfen nicht dauerhaft in das Paketformat eingebrannt werden.

So können zukünftige kryptografische Verfahren einschließlich Post-Quantum-Verfahren eingeführt werden.

## Offline Verification

Signaturen müssen grundsätzlich ohne aktive Verbindung zum ursprünglichen Repository überprüfbar sein, sofern die notwendigen Trust- und Revocation-Informationen verfügbar sind.

Dies ist insbesondere relevant für:

```text
Recovery
Offline Updates
Installation Media
Enterprise Deployment
Disconnected Systems
```

## Build Binding

Signaturen sollen mit Build- und Verification-Identitäten verknüpfbar sein.

```text
Source
 ↓
BuildID
 ↓
VerificationID
 ↓
Package ContentID
 ↓
Signature
```

Dadurch kann nachvollzogen werden, welcher Build tatsächlich signiert und ausgeliefert wurde.

## Secure Boot Integration

Bootkritische Update-Artefakte müssen mit der NovaOS Boot-Trust-Kette integrierbar sein.

```text
Update Signing
      ↓
Installed Boot Artifact
      ↓
Secure / Verified Boot
```

Eine erfolgreiche Update-Signaturprüfung ersetzt jedoch nicht die Boot-Verifikation.

## Key Protection

Private Signing Keys dürfen niemals Bestandteil eines Update-Pakets oder Repositorys sein.

Produktive Signing Keys sollen durch geeignete geschützte Signing-Infrastruktur verwendet werden.

```text
Private Key
    ↓
Protected Signing Environment
    ↓
Signature
```

## Provenance

Signaturentscheidungen sollen nachvollziehbar sein.

```text
ArtifactID
ContentID
SignerIdentity
SigningKeyID
Algorithm
SignatureResult
TrustDecision
RevocationState
VerificationTime
```

Private Schlüssel oder andere Secrets dürfen nicht protokolliert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ArtifactID
SignerIdentity
SigningKeyID
Algorithm
Signature State
Trust State
Revocation State
Validity
Signing Scope
Verification Time
```

## Normative Anforderungen

1. NovaOS MUSS kryptografisch signierte Update-Artefakte unterstützen.
2. Systemkritische Update-Pakete MÜSSEN signiert sein.
3. Repository-Metadaten MÜSSEN separat authentifizierbar sein.
4. Signaturen MÜSSEN eindeutig an den signierten Inhalt gebunden sein.
5. Änderungen signierter Inhalte MÜSSEN erkennbar sein.
6. Signer Identity MUSS vom Artifact Identity getrennt bleiben.
7. Eine gültige Signatur DARF NICHT automatisch Trust erzeugen.
8. Trust MUSS über die aktuelle Nova Trust Policy bestimmt werden.
9. Signing Authority MUSS auf Artifact Types oder Scopes beschränkbar sein.
10. NovaOS SOLL mehrere Signaturen pro Artefakt unterstützen.
11. Signing Keys MÜSSEN rotierbar sein.
12. Signing Keys MÜSSEN widerrufbar sein.
13. Revocation State MUSS bei sicherheitskritischen Entscheidungen berücksichtigt werden.
14. Historisch gültige Signaturen DÜRFEN NICHT automatisch als aktuell autorisiert gelten.
15. Signaturprüfung DARF Rollback Protection NICHT ersetzen.
16. Alte verwundbare Versionen MÜSSEN trotz gültiger Signatur blockierbar sein.
17. Das Signing-System MUSS kryptografische Algorithm Agility unterstützen.
18. Offline Signature Verification MUSS grundsätzlich möglich sein.
19. Package Signatures SOLLEN mit BuildID und Verification Evidence verknüpfbar sein.
20. Bootkritische Artefakte MÜSSEN mit Secure/Verified Boot integrierbar sein.
21. Update Signing DARF Boot Verification NICHT ersetzen.
22. Private Signing Keys DÜRFEN NICHT in Update-Paketen oder Repository-Daten enthalten sein.
23. Private Signing Keys MÜSSEN vor unautorisiertem Zugriff geschützt werden.
24. Signaturentscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
25. Secrets DÜRFEN NICHT in Verification Logs oder Provenance gespeichert werden.
26. Signatur-, Trust- und Revocation-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-REPOSITORY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-REVOCATION-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-KEYROTATION-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `ADR-ARCH-0167`

## Ergebnis

```text
Update Artifact
      ↓
Calculate ContentID
      ↓
Verify Signature
      ↓
Resolve Signer Identity
      ↓
Check Revocation
      ↓
Evaluate Trust + Signing Scope
      ↓
Authorized?
├── No  → Reject
└── Yes → Continue Update Validation
```

NovaOS erhält damit eine getrennte Signatur- und Vertrauenskette für Updates, bei der kryptografische Authentizität, Signer-Identität, Trust, Authority, Revocation und Rollback-Schutz unabhängig geprüft werden und eine gültige Signatur niemals allein zur Installation eines Updates berechtigt.