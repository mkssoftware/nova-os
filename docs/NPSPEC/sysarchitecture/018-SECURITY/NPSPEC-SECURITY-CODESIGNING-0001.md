# NPSPEC-SECURITY-CODESIGNING-0001 – Nova Code Signing

## Status

Angenommen

## Kategorie

Security / Integrity / Code Signing

## Zweck

NovaOS definiert Code Signing zur kryptografischen Prüfung von ausführbarem Code, Systemkomponenten und Updates.

```text
Code
  ↓
Signature
  ↓
Trust Verification
  ↓
Execution Policy
```

Code Signing bestätigt Herkunft und Integrität, erzeugt jedoch keine automatische Ausführungsberechtigung.

## Grundprinzipien

```text
Signed ≠ Trusted
Trusted ≠ Authorized
Valid Signature ≠ Safe Code
Publisher Identity ≠ Capability
Signature Verification ≠ Runtime Isolation
```

Code Signing ergänzt Capabilities, MAC, Sandboxing und Verified Boot.

## Signaturmodell

Ein signiertes Artefakt besitzt mindestens:

```text
SignedArtifact
├── ArtifactID
├── Content Hash
├── Signature
├── Signer Identity
└── Signature State
```

Optional:

```text
Certificate Chain
Timestamp
Trust Domain
Version
Security Metadata
```

## Geschützte Artefakte

Code Signing muss mindestens anwendbar sein auf:

```text
Kernel Components
Drivers
System Services
Applications
Libraries
Firmware Components
Updates
Packages
```

Die Policy darf je Artefakttyp unterschiedliche Vertrauensanforderungen definieren.

## Verifikation

Vor Verwendung eines geschützten Artefakts muss geprüft werden können:

```text
Content Hash
     ↓
Signature
     ↓
Signer
     ↓
Trust Policy
     ↓
Valid / Invalid / Unknown
```

Eine gültige Signatur darf nur für exakt den signierten Inhalt gelten.

## Vertrauensmodell

NovaOS darf mehrere Trust Domains unterstützen:

```text
NovaOS
Hardware Vendor
Application Publisher
Organization
Developer
User
```

Vertrauen muss explizit durch Policy definiert sein.

Ein unbekannter Signierer darf nicht automatisch als vertrauenswürdig gelten.

## Signaturzustände

Mindestens:

```text
Valid
Invalid
Expired
Revoked
Unknown
Unsigned
```

Sicherheitskritische Komponenten dürfen bei `Invalid`, `Revoked` oder `Unknown` gemäß Policy abgelehnt werden.

## Schlüssel

Private Signaturschlüssel dürfen niemals Bestandteil des ausgelieferten Systems sein.

Vertrauenswürdige öffentliche Schlüssel dürfen geschützt gespeichert werden.

NovaOS soll Hardware-gestützten Schlüsselschutz wie TPM/HSM unterstützen können.

## Updates

Updates müssen vor Installation verifizierbar sein.

```text
Update
  ↓
Signature Verification
  ↓
Version / Policy Check
  ↓
Transactional Installation
```

Eine gültige Signatur darf Rollback-, Versions- oder Update-Policy nicht umgehen.

## Laufzeitintegration

Eine gültige Signatur erzeugt keine zusätzlichen Rechte.

```text
Signed Application
       ↓
Sandbox + Capabilities + MAC
       ↓
Controlled Execution
```

Auch signierter Code bleibt den normalen Sicherheitsgrenzen unterworfen.

## Widerruf

Kompromittierte Signierer oder Artefakte müssen widerrufbar sein.

```text
Trusted
   ↓
Revocation
   ↓
Rejected
```

Widerrufsinformationen müssen sicher aktualisierbar sein.

## Introspection

NovaOS soll mindestens darstellen können:

```text
ArtifactID
Signer
Signature State
Trust Domain
Timestamp
Revocation State
Verification Result
```

## Normative Anforderungen

1. NovaOS MUSS kryptografische Code-Signaturen prüfen können.
2. Signaturen MÜSSEN Inhalt und Signierer eindeutig binden.
3. Eine gültige Signatur DARF NICHT automatisch Autorität erzeugen.
4. Vertrauenswürdige Signierer MÜSSEN policygesteuert verwaltet werden.
5. Ungültige oder widerrufene Signaturen MÜSSEN erkennbar sein.
6. Sicherheitskritische Komponenten SOLLEN verpflichtend signierbar sein.
7. Updates MÜSSEN vor ihrer Aktivierung verifizierbar sein.
8. Private Signaturschlüssel DÜRFEN NICHT mit ausgeliefert werden.
9. Signierter Code MUSS weiterhin normalen Sicherheitsregeln unterliegen.
10. Signatur- und Vertrauenszustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-SECURITY-0009`

## Ergebnis

```text
Artifact
   ↓
Cryptographic Signature
   ↓
Trust Verification
   ↓
Security Policy
   ↓
Controlled Execution
```

NovaOS erhält damit eine einheitliche Code-Signing-Infrastruktur zur Prüfung von Herkunft und Integrität ausführbarer Komponenten, ohne Signaturen mit Vertrauen oder Autorität gleichzusetzen.