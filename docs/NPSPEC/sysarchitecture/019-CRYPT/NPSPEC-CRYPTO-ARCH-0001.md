# NPSPEC-CRYPTO-ARCH-0001 – Nova Cryptographic Architecture

## Status

Angenommen

## Kategorie

Cryptography / Architecture / Core

## Zweck

NovaOS definiert eine zentrale kryptografische Architektur für Verschlüsselung, Signaturen, Hashing, Schlüsselverwaltung und sichere Zufallszahlen.

```text
Application / System Service
          ↓
      Nova Crypto API
          ↓
Cryptographic Providers
          ↓
Software / CPU / TPM / HSM
```

Anwendungen sollen kryptografische Primitive nicht selbst implementieren müssen.

## Grundprinzipien

```text
Cryptography ≠ Authorization
Encryption ≠ Trust
Hash ≠ Signature
Algorithm Name ≠ Security Policy
Key Material ≠ Normal Application Data
```

Kryptografische Verfahren werden zentral bereitgestellt und policygesteuert ausgewählt.

## Architektur

Die Crypto-Infrastruktur besteht logisch aus:

```text
Nova.Crypto
├── Random
├── Hash
├── MAC
├── Symmetric
├── Asymmetric
├── Signature
├── KDF
├── Key Management
└── Provider Interface
```

Die öffentliche API soll möglichst unabhängig von konkreten Implementierungen bleiben.

## Provider

Kryptografische Operationen dürfen durch unterschiedliche Provider ausgeführt werden:

```text
Software Provider
CPU Accelerated Provider
TPM Provider
HSM Provider
Future Hardware Provider
```

Provider-Auswahl darf anhand von:

```text
Security Policy
ExecutionContract
Hardware Capability
Performance
Key Location
```

erfolgen.

Ein Providerwechsel darf die kryptografische Semantik nicht verändern.

## Algorithmen

Algorithmen müssen über stabile Algorithmus-IDs referenziert werden.

```text
AlgorithmID
├── Type
├── Parameters
├── Security State
└── Provider Support
```

Algorithmen können Zustände besitzen:

```text
Preferred
Allowed
Legacy
Deprecated
Forbidden
```

Unsichere Algorithmen müssen systemweit deaktivierbar sein.

## Schlüssel

Schlüssel müssen als geschützte Objekte behandelt werden.

```text
CryptoKey
├── KeyID
├── Type
├── Usage
├── Provider
└── State
```

Optional:

```text
Expiration
Export Policy
Security Domain
Hardware Binding
```

Anwendungen sollen Schlüssel nach Möglichkeit über `KeyID` oder geschützte Handles verwenden, statt direkt auf Schlüsselmaterial zuzugreifen.

## Schlüsselverwendung

Schlüssel müssen auf definierte Zwecke beschränkbar sein:

```text
Encrypt
Decrypt
Sign
Verify
Derive
Authenticate
```

Ein Signaturschlüssel darf nicht automatisch für beliebige andere kryptografische Operationen verwendet werden.

## Zufallszahlen

NovaOS muss einen kryptografisch sicheren Zufallszahlengenerator bereitstellen.

```text
Entropy Sources
      ↓
CSPRNG
      ↓
Crypto Operations
```

Sicherheitskritische Funktionen dürfen keine ungeeigneten Pseudozufallsquellen verwenden.

## Hardware-Unterstützung

NovaOS soll vorhandene Hardwarebeschleunigung nutzen können:

```text
AES Instructions
SHA Extensions
Vector Instructions
TPM
HSM
```

Hardwarebeschleunigung darf jedoch keine Sicherheits- oder Isolationsanforderungen umgehen.

## Agilität

Die Architektur muss Crypto Agility unterstützen.

```text
Old Algorithm
     ↓
Policy Change
     ↓
New Algorithm
```

Algorithmen und Provider müssen austauschbar sein, ohne Anwendungen neu entwerfen zu müssen.

## Fehlerverhalten

Kryptografische Fehler müssen explizit behandelt werden.

```text
VerificationFailed
InvalidKey
UnsupportedAlgorithm
ProviderFailure
PolicyDenied
EntropyUnavailable
```

Ein Fehler darf nicht still auf ein schwächeres Verfahren zurückfallen.

```text
Failure ≠ Downgrade
```

## Sicherheit

Geheimes Schlüsselmaterial darf nicht ungefiltert erscheinen in:

```text
Logs
Audit
Crash Dumps
Tracing
Introspection
```

Nicht mehr benötigtes sensibles Material soll soweit technisch möglich sicher aus Speicher entfernt werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Algorithm
Provider
KeyID
Key State
Security State
Hardware Acceleration
Policy Decision
```

Geheimes Schlüsselmaterial darf dabei nicht sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS eine zentrale kryptografische Infrastruktur bereitstellen.
2. Anwendungen SOLLEN kryptografische Primitive nicht selbst implementieren müssen.
3. Algorithmen und Provider MÜSSEN austauschbar sein.
4. Unsichere Algorithmen MÜSSEN systemweit deaktivierbar sein.
5. Schlüssel MÜSSEN als geschützte Sicherheitsobjekte behandelt werden.
6. Schlüsselverwendung MUSS auf definierte Zwecke begrenzbar sein.
7. NovaOS MUSS kryptografisch sichere Zufallszahlen bereitstellen.
8. Kryptografische Fehler DÜRFEN NICHT still zu Sicherheits-Downgrades führen.
9. Geheimes Schlüsselmaterial DARF NICHT über normale Introspection oder Logs offengelegt werden.
10. Die Architektur MUSS Crypto Agility unterstützen.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-CRYPTO-0001`

## Ergebnis

```text
Nova Crypto API
      ↓
Security Policy
      ↓
Algorithm + Provider + Key
      ↓
Software / Hardware
      ↓
Cryptographic Operation
```

NovaOS erhält damit eine zentrale, hardwareunabhängige und crypto-agile Architektur, über die kryptografische Funktionen konsistent und sicher im gesamten System bereitgestellt werden.