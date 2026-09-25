# NPSPEC-IDENTITY-CREDENTIAL-0001 – Nova Credential Architecture

## Status

Angenommen

## Kategorie

Identity / Authentication / Credentials

## Zweck

NovaOS definiert ein einheitliches Modell für Credentials, mit denen Identitäten ihre Identität oder bestimmte Eigenschaften kryptografisch nachweisen können.

```text
Identity
   ↓
Credential
   ↓
Verification
   ↓
Authentication Context
```

Credentials dienen als Nachweis einer Identität oder eines Claims, erzeugen jedoch selbst keine Autorität.

## Grundprinzipien

```text
Credential ≠ Identity
Credential ≠ Authority
Possession ≠ Authorization
Valid Credential ≠ Trusted Identity
CredentialID ≠ Secret
Revocation ≠ Identity Deletion
```

## Credential-Modell

Ein Credential besitzt mindestens:

```text
Credential
├── CredentialID
├── IdentityID
├── Credential Type
├── Issuer
└── State
```

Optional:

```text
Public Key
Authenticator
Issued At
Expiration
Allowed Usage
Security Domain
Trust Level
Hardware Binding
Revocation Reference
Attestation
Claims
```

`CredentialID` identifiziert das Credential, während `IdentityID` die zugehörige Identität bezeichnet.

## Credential-Typen

NovaOS muss unterschiedliche Credential-Typen unterstützen können:

```text
Password Credential
PIN Credential
Passkey
Public-Key Credential
Certificate
Hardware Token
Recovery Credential
Federated Credential
Device Credential
Service Credential
Workload Credential
```

Neue Credential-Typen müssen erweiterbar sein.

## Credential Provider

Credentials können durch unterschiedliche Provider verwaltet werden:

```text
Local Identity Store
Nova Key Store
TPM
Secure Hardware
External Authenticator
Federated Identity Provider
```

Anwendungen sollen Credentials über definierte Identity-Schnittstellen verwenden, statt direkt auf geheimes Schlüsselmaterial zuzugreifen.

## Credential-Bindung

Credentials müssen eindeutig an ihren vorgesehenen Kontext bindbar sein.

```text
Credential
├── Identity
├── Purpose
├── Provider
└── Security Context
```

Optional können zusätzliche Bindungen bestehen:

```text
Device
Application
Service
Workload
Relying Party
Security Domain
```

## Lebenszyklus

Credentials besitzen einen kontrollierten Lebenszyklus:

```text
Create / Issue
      ↓
Active
      ↓
Rotate / Replace
      ↓
Expired / Revoked
      ↓
Destroyed
```

Mindestens folgende Zustände müssen darstellbar sein:

```text
Active
Disabled
Expired
Revoked
Destroyed
Unavailable
```

## Rotation und Erneuerung

Credentials müssen erneuert oder ersetzt werden können, ohne die zugrunde liegende Identität zu ändern.

```text
IdentityID
├── Credential v1 → Retired
└── Credential v2 → Active
```

```text
Credential Rotation ≠ Identity Change
```

## Revocation

Ein kompromittiertes Credential muss unabhängig von der Identität widerrufbar sein.

```text
Identity
├── Credential A → Active
├── Credential B → Revoked
└── Credential C → Active
```

Widerrufene Credentials dürfen nicht weiter zur erfolgreichen Authentifizierung beitragen.

## Expiration

Credentials können zeitlich begrenzt sein.

```text
Issued
  ↓
Active
  ↓
Expiration
  ↓
Invalid
```

Kurzlebige Credentials sollen insbesondere für Sessions, Workloads und verteilte Dienste unterstützt werden.

## Credential-Schutz

Geheime Credential-Daten müssen entsprechend ihrer Sensitivität geschützt werden.

```text
Password → KDF
Private Key → Key Store / Hardware
Passkey → Authenticator
Token → Protected Storage
```

Secrets dürfen nicht unnötig kopiert, exportiert oder protokolliert werden.

## Autorisierung

Nach erfolgreicher Credential-Verifikation erfolgt weiterhin eine getrennte Autorisierungsentscheidung.

```text
Credential
    ↓
Authentication
    ↓
Identity
    ↓
Security Policy
    ↓
Capabilities
```

Ein gültiges Credential darf keine implizite Ressourcenberechtigung erzeugen.

## Federation

Externe Credentials müssen über definierte Trust Policies validiert werden.

```text
External Credential
        ↓
Issuer Verification
        ↓
Federation Trust
        ↓
Nova Identity Mapping
```

Ein gültiges externes Credential bedeutet nicht automatisch, dass NovaOS dem Issuer oder allen enthaltenen Claims vertraut.

## Recovery

Credential Recovery muss getrennt von normaler Authentifizierung behandelt werden.

Recovery darf keine universelle Hintertür erzeugen und muss bestehende MFA-, Step-Up- und Security-Policies berücksichtigen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
CredentialID
IdentityID
Credential Type
Issuer
Provider
State
Expiration
Hardware Binding
Allowed Usage
```

Nicht dargestellt werden dürfen:

```text
Passwords
PINs
Private Keys
Recovery Secrets
Raw Biometric Data
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Credential-Modell bereitstellen.
2. Credentials MÜSSEN eindeutig einer Identität zuordenbar sein.
3. Credential und Identity MÜSSEN getrennte Lebenszyklen besitzen können.
4. Credentials DÜRFEN NICHT automatisch Autorität erzeugen.
5. Credentials MÜSSEN widerrufbar und zeitlich begrenzbar sein können.
6. Credential-Rotation DARF die stabile IdentityID NICHT unnötig verändern.
7. Geheime Credential-Daten MÜSSEN geschützt gespeichert und verarbeitet werden.
8. Anwendungen SOLLEN keinen direkten Zugriff auf private Credential-Secrets erhalten.
9. Externe Credentials MÜSSEN über Trust Policies validiert werden.
10. Credential-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-IDENTITY-PASSKEY-0001`
- `NPSPEC-IDENTITY-MFA-0001`
- `NPSPEC-IDENTITY-STEPUP-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-CRYPTO-KDF-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `ADR-IDENTITY-0011`

## Ergebnis

```text
Identity
   ↓
Credential
   ↓
Protected Verification
   ↓
Authentication Context
   ↓
Security Policy
   ↓
Capabilities
```

NovaOS erhält damit eine einheitliche Credential-Architektur, die Passwörter, Passkeys, Schlüssel, Zertifikate, Hardware- und kurzlebige Credentials unter einem gemeinsamen Lebenszyklus zusammenführt und Credentials konsequent von Identität und Autorität trennt.