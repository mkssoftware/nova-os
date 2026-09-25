# NPSPEC-CRYPTO-AEAD-0001 – Nova Authenticated Encryption with Associated Data

## Status

Angenommen

## Kategorie

Cryptography / Encryption / AEAD

## Zweck

NovaOS definiert AEAD als bevorzugtes Verfahren für Daten, die gleichzeitig Vertraulichkeit und kryptografische Integrität benötigen.

```text
Plaintext
   +
Associated Data
   +
Key + Nonce
   ↓
AEAD
   ↓
Ciphertext + Authentication Tag
```

## Grundprinzipien

```text
Encryption ≠ Integrity
AEAD ≠ Authorization
Associated Data ≠ Encrypted Data
Authentication Failure ≠ Recoverable Plaintext
Nonce Reuse ≠ Allowed
```

## AEAD-Modell

Eine Operation besitzt mindestens:

```text
AEADContext
├── Algorithm
├── KeyID
├── Nonce
├── Plaintext / Ciphertext
├── Associated Data
└── Authentication Tag
```

Optional:

```text
Security Domain
Purpose
ObjectID
Sequence Number
Protocol Context
```

## Verschlüsselung

```text
Encrypt(
    Key,
    Nonce,
    Plaintext,
    AssociatedData
)
→ Ciphertext + Tag
```

Die Associated Data wird authentifiziert, aber nicht verschlüsselt.

Geeignete Beispiele:

```text
ObjectID
Protocol Header
Version
Sequence Number
Security Metadata
```

## Entschlüsselung

```text
Ciphertext
   +
Associated Data
   +
Authentication Tag
   ↓
Verify
   ↓
Valid → Plaintext
Invalid → Error
```

Klartext darf erst als gültig behandelt werden, wenn die Authentifizierung erfolgreich abgeschlossen wurde.

## Nonce-Verwaltung

Nonce-Anforderungen des verwendeten Algorithmus müssen strikt eingehalten werden.

NovaOS soll Nonces nach Möglichkeit automatisch verwalten.

```text
Key + Nonce Combination
        ↓
Must Be Unique
```

Nonce-Wiederverwendung darf nicht durch normale API-Nutzung begünstigt werden.

## Schlüssel

AEAD-Schlüssel werden als geschützte Crypto-Objekte verwaltet.

```text
KeyID
 ↓
Nova Crypto
 ↓
AEAD Provider
```

Schlüssel dürfen durch:

```text
Purpose
Algorithm
Security Domain
Lifetime
Usage Policy
```

eingeschränkt werden.

## Streaming und große Daten

Große Daten sollen in authentifizierte Einheiten zerlegt werden können.

```text
Data
├── Chunk 0 + Tag
├── Chunk 1 + Tag
└── Chunk N + Tag
```

Reihenfolge und Zugehörigkeit der Chunks müssen kryptografisch bindbar sein.

## Fehlerverhalten

Mindestens folgende Fehler müssen unterscheidbar sein:

```text
AuthenticationFailed
InvalidNonce
InvalidKey
UnsupportedAlgorithm
PolicyDenied
ProviderFailure
```

Bei `AuthenticationFailed` darf kein nicht authentifizierter Klartext an den Aufrufer freigegeben werden.

## Crypto Agility

AEAD-Anwendungen sollen keine unnötige Abhängigkeit von einem konkreten Algorithmus besitzen.

```text
AEAD Requirement
      ↓
Crypto Policy
      ↓
Allowed Algorithm
      ↓
Provider
```

Algorithmus- und Parameterwechsel müssen kontrolliert möglich sein.

## Sicherheit

AEAD ersetzt nicht:

```text
Capabilities
Access Control
MAC
Information Flow Policy
Data Sovereignty
```

Auch korrekt entschlüsselte Daten müssen weiterhin den normalen NovaOS-Sicherheitsregeln unterliegen.

## Introspection

NovaOS soll sicher darstellen können:

```text
Algorithm
KeyID
Provider
Policy State
Operation State
```

Nicht dargestellt werden dürfen:

```text
Key Material
Plaintext
Sensitive Intermediate State
```

## Normative Anforderungen

1. NovaOS MUSS AEAD zentral unterstützen.
2. AEAD SOLL für neue authentifizierte Verschlüsselung bevorzugt werden.
3. Associated Data MUSS kryptografisch authentifiziert werden.
4. Nonces MÜSSEN entsprechend dem verwendeten Algorithmus sicher verwaltet werden.
5. Unsichere Nonce-Wiederverwendung MUSS soweit möglich verhindert werden.
6. Klartext DARF bei fehlgeschlagener Authentifizierung NICHT freigegeben werden.
7. AEAD-Schlüssel MÜSSEN als geschützte Crypto-Objekte behandelt werden.
8. Große Daten MÜSSEN sicher segmentierbar sein.
9. AEAD MUSS Crypto Agility unterstützen.
10. AEAD DARF Autorisierung und Security Policies NICHT ersetzen.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-ENCRYPTION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `ADR-CRYPTO-0004`

## Ergebnis

```text
Plaintext + Metadata
        ↓
       AEAD
        ↓
Confidentiality + Integrity
        ↓
Authenticated Ciphertext
```

NovaOS erhält damit ein einheitliches Verfahren für authentifizierte Verschlüsselung, bei dem manipulierte Daten zuverlässig erkannt und nicht als gültiger Klartext weiterverarbeitet werden.