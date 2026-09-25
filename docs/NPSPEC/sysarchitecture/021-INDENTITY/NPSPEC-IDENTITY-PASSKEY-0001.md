# NPSPEC-IDENTITY-PASSKEY-0001 – Nova Passkey Identity Authentication

## Status

Angenommen

## Kategorie

Identity / Authentication / Passkeys

## Zweck

NovaOS definiert Passkeys als phishing-resistente, kryptografische Authentifizierungsmethode für lokale und entfernte Identitäten.

```text
Identity
   ↓
Passkey Credential
   ↓
User Verification
   ↓
Cryptographic Proof
   ↓
Authenticated Identity
```

Passkeys ersetzen dabei gemeinsam genutzte Geheimnisse durch asymmetrische Schlüsselpaare.

## Grundprinzipien

```text
Passkey ≠ Identity
Authentication ≠ Authorization
Private Key ≠ Exportable Secret
Biometric Verification ≠ Biometric Disclosure
Synced Passkey ≠ Unrestricted Key Export
Valid Passkey ≠ Unlimited Authority
```

## Passkey-Modell

Ein Passkey besitzt mindestens:

```text
PasskeyCredential
├── CredentialID
├── IdentityID
├── Public Key
├── Relying Party
└── State
```

Optional:

```text
Authenticator
Creation Time
Last Used
User Verification Policy
Backup State
Synchronization State
Hardware Binding
Attestation
Expiration
```

Der private Schlüssel darf dem authentifizierenden Dienst nicht offengelegt werden.

## Authentifizierung

Die Authentifizierung erfolgt über eine kryptografische Challenge.

```text
Relying Party
      ↓
Challenge
      ↓
Authenticator
      ↓
User Verification
      ↓
Signature
      ↓
Verification
```

Die Challenge muss an den vorgesehenen Authentifizierungskontext gebunden sein.

## User Verification

Passkeys können eine lokale Benutzerverifikation verlangen:

```text
PIN
Biometric Verification
Hardware Interaction
Device Unlock
```

Biometrische Rohdaten dürfen dabei nicht an die Relying Party übertragen werden.

```text
Biometric Data
     ↓
Local Verification
     ↓
Verification Result
```

## Authenticator

Passkeys müssen unterschiedliche Authenticator-Typen unterstützen können:

```text
Platform Authenticator
Hardware Security Key
TPM-backed Authenticator
External Authenticator
Synced Authenticator
```

Die konkrete Implementierung darf über Provider abstrahiert werden.

## Phishing-Schutz

Passkeys müssen an die jeweilige Relying Party gebunden sein.

```text
Credential
    ↓
Relying Party Binding
    ↓
Valid Origin
```

Ein Credential für Dienst A darf nicht für einen anderen Dienst B verwendbar sein.

## Schlüsselverwaltung

Private Passkey-Schlüssel müssen über geschützte kryptografische Provider verwaltet werden.

Bevorzugt:

```text
TPM
Secure Hardware
Protected Key Store
External Authenticator
```

Anwendungen dürfen keinen direkten Zugriff auf private Passkey-Schlüssel erhalten.

## Synchronisation

NovaOS darf synchronisierbare Passkeys unterstützen.

```text
Device A
   ↓
Protected Synchronization
   ↓
Device B
```

Synchronisation muss Ende-zu-Ende geschützt sein und Privacy-, Trust- sowie Sovereignty-Policies berücksichtigen.

Nicht exportierbare hardwaregebundene Passkeys müssen weiterhin unterstützt werden.

## Recovery

Der Verlust eines Geräts darf nicht zwangsläufig zum Verlust einer Identität führen.

Recovery kann über:

```text
Additional Passkey
Recovery Credential
Trusted Device
Identity Provider
Controlled Recovery Process
```

erfolgen.

Recovery darf keine schwächere universelle Hintertür erzeugen.

## Revocation

Einzelne Passkeys müssen unabhängig widerrufbar sein.

```text
Identity
├── Passkey A → Active
├── Passkey B → Revoked
└── Passkey C → Active
```

Die Revocation eines Passkeys muss nicht automatisch die zugrunde liegende Identity deaktivieren.

## Autorisierung

Nach erfolgreicher Passkey-Authentifizierung erfolgt weiterhin eine getrennte Autorisierungsentscheidung.

```text
Passkey Authentication
        ↓
Authenticated Identity
        ↓
Security Policy
        ↓
Capabilities
```

Passkeys erzeugen keine eigenständige Ressourcenautorität.

## Privacy

Passkey-Verwendung soll unnötige globale Korrelation vermeiden.

Credential-Identifier und Metadaten dürfen nicht unnötig zwischen unabhängigen Relying Parties wiederverwendet werden.

Synchronisation und Recovery müssen Data Minimization berücksichtigen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
CredentialID
IdentityID
Authenticator Type
Relying Party
State
Hardware Binding
Synchronization State
Last Used
```

Private Schlüssel und biometrische Rohdaten dürfen niemals über Introspection offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Passkey-basierte Authentifizierung unterstützen können.
2. Passkeys MÜSSEN asymmetrische kryptografische Credentials verwenden.
3. Private Passkey-Schlüssel DÜRFEN NICHT an Relying Parties offengelegt werden.
4. Passkeys MÜSSEN an ihre Relying Party gebunden sein.
5. Biometrische Rohdaten DÜRFEN NICHT an Relying Parties übertragen werden.
6. Hardwaregeschützte Passkeys SOLLEN unterstützt werden.
7. Einzelne Passkeys MÜSSEN unabhängig widerrufbar sein.
8. Passkey-Synchronisation MUSS Privacy-, Trust- und Sovereignty-Policies berücksichtigen.
9. Recovery DARF KEINE universelle Sicherheitsumgehung erzeugen.
10. Passkey-Authentifizierung DARF NICHT mit Autorisierung gleichgesetzt werden.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-RNG-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-0001`
- `ADR-IDENTITY-0008`

## Ergebnis

```text
Identity
   ↓
Passkey
   ↓
Local User Verification
   ↓
Cryptographic Authentication
   ↓
Authenticated Identity
   ↓
Capabilities + Policies
```

NovaOS erhält damit eine phishing-resistente Authentifizierungsarchitektur, bei der private Schlüssel geschützt bleiben, biometrische Daten das Gerät nicht verlassen müssen und Authentifizierung konsequent von tatsächlicher Autorität getrennt bleibt.