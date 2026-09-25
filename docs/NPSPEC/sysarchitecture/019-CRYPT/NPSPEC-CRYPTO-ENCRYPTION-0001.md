# NPSPEC-CRYPTO-ENCRYPTION-0001 – Nova Encryption

## Status

Angenommen

## Kategorie

Cryptography / Encryption / Data Protection

## Zweck

NovaOS definiert ein einheitliches Verschlüsselungsmodell zum Schutz vertraulicher Daten während Speicherung, Übertragung und Verarbeitung.

```text
Plaintext
   ↓
Encryption
   ↓
Ciphertext
   ↓
Decryption
   ↓
Plaintext
```

Verschlüsselung schützt Vertraulichkeit, ersetzt jedoch keine Autorisierung oder Integritätsprüfung.

## Grundprinzipien

```text
Encryption ≠ Authorization
Encryption ≠ Integrity
Encryption ≠ Identity
Encrypted ≠ Trusted
Key Possession ≠ Unlimited Authority
```

## Verschlüsselungsmodell

Eine Verschlüsselungsoperation besitzt mindestens:

```text
EncryptionContext
├── Algorithm
├── KeyID
├── Parameters
├── Input
└── Output
```

Optional:

```text
Associated Data
Security Domain
Purpose
Data Classification
Sovereignty Policy
```

## Symmetrische Verschlüsselung

NovaOS muss symmetrische Verschlüsselung für große Datenmengen unterstützen.

Bevorzugt werden authentifizierte Verfahren:

```text
Plaintext
   ↓
AEAD
   ↓
Ciphertext + Authentication Tag
```

Damit können Vertraulichkeit und kryptografische Integrität gemeinsam geschützt werden.

## Asymmetrische Verschlüsselung

Asymmetrische Verfahren dürfen für geeignete Anwendungsfälle eingesetzt werden:

```text
Public Key
    ↓
Encrypt / Establish Secret
    ↓
Private Key
```

Für große Datenmengen soll bevorzugt hybride Verschlüsselung verwendet werden.

```text
Asymmetric Crypto
      ↓
Session/Data Key
      ↓
Symmetric Encryption
```

## Schlüssel

Schlüssel werden über die zentrale Nova-Crypto-Architektur verwaltet.

```text
KeyID → Protected Key Object
```

Anwendungen sollen Schlüsselmaterial nicht direkt speichern oder kopieren müssen.

Schlüssel dürfen auf:

```text
Purpose
Resource
Security Domain
Lifetime
Export Policy
```

begrenzt werden.

## Daten im Speicher

NovaOS soll Verschlüsselung unterstützen für:

```text
Files
NovaFile Payloads
Volumes
Backups
Snapshots
Configuration
Sensitive Metadata
```

Verschlüsselung muss mit Storage-Transaktionen, Versionierung und Recovery vereinbar sein.

## Datenübertragung

Transportverschlüsselung wird über geeignete Protokolle wie TLS oder QUIC bereitgestellt.

```text
Application Data
      ↓
Secure Transport
      ↓
Encrypted Network Traffic
```

Transportverschlüsselung ersetzt keine Ende-zu-Ende-Verschlüsselung, wenn diese erforderlich ist.

## Nonce und IV

Verfahren, die Nonces oder Initialisierungsvektoren benötigen, müssen deren korrekte Erzeugung und Verwendung sicherstellen.

```text
Nonce Reuse ≠ Allowed
```

Anwendungen sollen diese Werte möglichst nicht selbst verwalten müssen.

## Crypto Agility

Konkrete Algorithmen dürfen nicht unnötig fest in Anwendungen eingebaut werden.

```text
Encryption Requirement
        ↓
Crypto Policy
        ↓
Allowed Algorithm
```

Veraltete Verfahren müssen kontrolliert migrierbar sein.

## Fehlerverhalten

Fehler müssen explizit behandelt werden:

```text
InvalidKey
AuthenticationFailed
UnsupportedAlgorithm
PolicyDenied
InvalidParameters
ProviderFailure
```

Ein Fehler darf keinen stillen Wechsel zu unverschlüsselter Verarbeitung verursachen.

```text
Encryption Failure ≠ Plaintext Fallback
```

## Introspection

NovaOS soll mindestens darstellen können:

```text
Algorithm
KeyID
Provider
Policy State
Encryption State
Migration State
```

Schlüsselmaterial und Klartext dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS eine zentrale Verschlüsselungsinfrastruktur bereitstellen.
2. Authentifizierte Verschlüsselung SOLL standardmäßig bevorzugt werden.
3. Schlüssel MÜSSEN über geschützte Crypto-Objekte verwaltbar sein.
4. Große Datenmengen SOLLEN symmetrisch oder hybrid verschlüsselt werden.
5. Nonces und IVs MÜSSEN kryptografisch korrekt verwaltet werden.
6. Verschlüsselung MUSS mit Crypto Agility vereinbar sein.
7. Unsichere Algorithmen MÜSSEN policygesteuert deaktivierbar sein.
8. Verschlüsselungsfehler DÜRFEN NICHT zu stillem Plaintext-Fallback führen.
9. Verschlüsselung DARF Autorisierung und Zugriffskontrolle NICHT ersetzen.
10. Verschlüsselungszustände SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `ADR-CRYPTO-0003`

## Ergebnis

```text
Data
 ↓
Encryption Policy
 ↓
Key + Algorithm + Provider
 ↓
Protected Ciphertext
```

NovaOS erhält damit ein einheitliches Verschlüsselungsmodell für lokale und übertragene Daten, ohne Anwendungen dauerhaft an einzelne Algorithmen oder Schlüsselimplementierungen zu binden.