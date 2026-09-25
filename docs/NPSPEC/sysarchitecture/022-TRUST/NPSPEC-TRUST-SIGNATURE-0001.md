# NPSPEC-TRUST-SIGNATURE-0001 – Nova Signature Trust

## Status

Angenommen

## Kategorie

Trust / Digital Signatures / Security

## Zweck

NovaOS definiert Signature Trust als kontextabhängige Bewertung digitaler Signaturen und ihrer Vertrauenskette.

```text
Signed Object
     ↓
Signature Verification
     ↓
Signer Identity + Trust Evidence
     ↓
Signature Trust Evaluation
     ↓
Trust State
```

Eine kryptografisch gültige Signatur beweist, dass eine Signatur zum angegebenen Schlüssel passt. Sie bedeutet nicht automatisch, dass Signierer, Inhalt oder ausgeführte Aktion vertrauenswürdig sind.

## Grundprinzipien

```text
Valid Signature ≠ Trusted Signer
Trusted Signer ≠ Trusted Content
Signature ≠ Authorization
Certificate ≠ Permission
Signed Code ≠ Safe Code
Expired Certificate ≠ Automatically Invalid Historical Signature
Unknown Signer ≠ Trusted Signer
```

## Trust-Modell

Eine Signature-Trust-Bewertung besitzt mindestens:

```text
SignatureTrust
├── ObjectID
├── Signer Identity
├── Signature Status
├── Trust Domain
└── Trust State
```

Optional:

```text
Algorithm
Certificate Chain
Timestamp
Purpose
Security Domain
Provider
Revocation State
Attestation
Policy
```

## Signaturprüfung

Die kryptografische Prüfung erfolgt getrennt von der Trust-Bewertung.

```text
Object
  +
Signature
  +
Public Key
   ↓
Cryptographic Verification
   ↓
Valid / Invalid
```

Erst danach erfolgt:

```text
Signer
+
Trust Policy
+
Context
+
Revocation State
↓
Trust Decision
```

## Signer Identity

Signaturen müssen soweit möglich einer überprüfbaren Identity zugeordnet werden.

```text
Signature
   ↓
Credential
   ↓
Signer Identity
```

Dies kann beispielsweise sein:

```text
Application Identity
Service Identity
Device Identity
Organization Identity
Publisher Identity
```

Die Signer Identity erzeugt keine automatische Autorität.

## Trust Chains

NovaOS muss hierarchische Vertrauensketten unterstützen können.

```text
Root
 ↓
Intermediate
 ↓
Signer
 ↓
Signature
```

Jede Stufe muss entsprechend der jeweiligen Trust Policy überprüft werden.

Eine technisch gültige Zertifikatskette bedeutet nicht automatisch, dass sie für den konkreten Zweck akzeptiert wird.

## Trust Domains

Signaturvertrauen muss zweckgebunden sein können.

```text
Signer A
├── Application Signing → Trusted
├── Document Signing → Trusted
└── Kernel Signing → Not Trusted
```

Trust darf daher nicht global aus einer einzigen Signaturentscheidung abgeleitet werden.

## Zeitbezug

Signaturbewertungen müssen Zeitinformationen berücksichtigen können.

```text
Signing Time
Certificate Validity
Trusted Timestamp
Revocation Time
Verification Time
```

Eine heute abgelaufene Signierer-Credential macht eine nachweislich während ihrer Gültigkeit erzeugte historische Signatur nicht automatisch ungültig.

## Revocation

NovaOS muss Widerruf von Signer Credentials und Trust Anchors berücksichtigen können.

```text
Valid Signature
      +
Revoked Signer
      ↓
Policy Evaluation
```

Je nach Zeitpunkt, Zweck und Policy kann das Ergebnis unterschiedlich ausfallen.

## Code Signing

Bei ausführbarem Code muss Signature Trust mit Code Integrity kombiniert werden.

```text
Code
 ↓
Integrity Verification
 ↓
Signature Verification
 ↓
Signer Trust
 ↓
Execution Policy
```

Eine gültige Signatur darf weder Malware-Erkennung noch Sandbox-, Capability- oder Code-Integrity-Regeln ersetzen.

## Dokumente und Daten

Signature Trust muss auch für signierte Datenobjekte verwendbar sein:

```text
Documents
Packages
Updates
NovaFiles
Configuration
Manifests
Audit Records
```

Dabei kann NovaOS Herkunft und Integrität prüfen, ohne dem Inhalt automatisch zu vertrauen.

## Crypto Agility

Signaturvertrauen muss kryptografische Agilität unterstützen.

Algorithmen können abhängig von Policy und Zeitpunkt als:

```text
Preferred
Allowed
Legacy
Deprecated
Forbidden
```

klassifiziert werden.

Post-Quantum- und hybride Signaturen müssen integrierbar sein.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Signer Identity
Signature Status
Trust State
Trust Domain
Algorithm
Timestamp
Revocation State
Trust Chain
Decision Reason
```

Private Schlüssel oder andere geheime Credential-Daten dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS kryptografische Signaturgültigkeit und Signature Trust getrennt behandeln.
2. Eine gültige Signatur DARF NICHT automatisch Vertrauen erzeugen.
3. Signature Trust MUSS kontext- und zweckabhängig sein können.
4. Signaturen SOLLEN überprüfbaren Signer Identities zugeordnet werden.
5. Trust Chains MÜSSEN policygesteuert validierbar sein.
6. Revocation und zeitliche Gültigkeit MÜSSEN berücksichtigt werden können.
7. Signature Trust DARF NICHT automatisch Autorität erzeugen.
8. Code Signing MUSS mit Code Integrity und Execution Policies kombinierbar sein.
9. Crypto Agility sowie zukünftige Post-Quantum-Signaturen MÜSSEN integrierbar sein.
10. Signature-Trust-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-PQ-0001`
- `ADR-TRUST-0003`

## Ergebnis

```text
Signed Object
     ↓
Cryptographic Verification
     ↓
Signer Identity
     ↓
Trust Chain + Context + Policy
     ↓
Signature Trust State
     ↓
Security Decision
```

NovaOS erhält damit eine einheitliche Signature-Trust-Architektur, die kryptografische Gültigkeit, Signiereridentität und tatsächliches Vertrauen konsequent trennt und Signaturen abhängig von Zweck, Zeitpunkt und Sicherheitskontext bewertet.