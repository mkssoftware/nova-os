# NPSPEC-CRYPTO-KDF-0001 – Nova Key Derivation Function

## Status

Angenommen

## Kategorie

Cryptography / Key Management / KDF

## Zweck

NovaOS definiert eine zentrale Infrastruktur für Key Derivation Functions (KDF), um kryptografische Schlüssel sicher aus vorhandenem Schlüsselmaterial, Geheimnissen oder Passwörtern abzuleiten.

```text
Input Secret
    +
Parameters / Context
    ↓
KDF
    ↓
Derived Key
```

## Grundprinzipien

```text
Password ≠ Key
Derived Key ≠ Source Secret
KDF ≠ Hash Function
Same Secret + Different Context ≠ Same Key
Fastest KDF ≠ Best KDF
```

## KDF-Modell

Eine Ableitung besitzt mindestens:

```text
KDFContext
├── Algorithm
├── Input Secret
├── Output Purpose
├── Parameters
└── Output Key
```

Optional:

```text
Salt
Context
KeyID
Security Domain
Output Length
Resource Policy
```

## Ableitung aus Schlüsselmaterial

Vorhandenes kryptografisches Schlüsselmaterial darf zur Erzeugung separater Schlüssel verwendet werden.

```text
Master Key
   ↓
KDF + Context
   ├── Encryption Key
   ├── Authentication Key
   └── Session Key
```

Unterschiedliche Zwecke müssen kryptografisch voneinander getrennt werden können.

## Passwortbasierte Ableitung

Passwörter dürfen nicht direkt als kryptografische Schlüssel verwendet werden.

```text
Password
   +
Salt
   +
Work Parameters
   ↓
Password KDF
   ↓
Derived Key
```

Passwortbasierte KDFs sollen konfigurierbare Kostenparameter unterstützen:

```text
Memory Cost
CPU Cost
Iterations
Parallelism
```

## Salt

Wo der verwendete KDF-Typ einen Salt benötigt, muss dieser korrekt erzeugt und gespeichert werden.

```text
Salt ≠ Secret
```

Salts sollen ausreichend groß und eindeutig sein.

## Context Separation

NovaOS muss Domain Separation unterstützen.

```text
MasterKey + "storage"
MasterKey + "network"
MasterKey + "backup"
```

dürfen unterschiedliche Schlüssel erzeugen.

Kontextwerte müssen eindeutig und stabil definiert sein.

## Schlüsselobjekte

Abgeleitete Schlüssel sollen direkt als geschützte Crypto-Objekte erzeugt werden können.

```text
KDF
 ↓
Protected Key Object
 ↓
KeyID
```

Das abgeleitete Schlüsselmaterial muss nicht unnötig an Anwendungen ausgegeben werden.

## Crypto Agility

KDF-Algorithmen und Parameter müssen policygesteuert sein.

```text
KDF Requirement
      ↓
Crypto Policy
      ↓
Algorithm + Parameters
```

Veraltete KDF-Konfigurationen müssen erkennbar und migrierbar sein.

## Ressourcensteuerung

Insbesondere speicher- oder rechenintensive Passwort-KDFs müssen Resource Budgets berücksichtigen.

Unkontrollierte Parameter dürfen nicht zu:

```text
Memory Exhaustion
CPU Exhaustion
Denial of Service
```

führen.

## Fehlerverhalten

Mindestens folgende Fehler müssen unterscheidbar sein:

```text
InvalidParameters
InvalidInput
UnsupportedAlgorithm
PolicyDenied
ResourceLimit
ProviderFailure
```

Ein Fehler darf keinen automatischen Wechsel auf eine schwächere KDF verursachen.

## Introspection

NovaOS soll sicher darstellen können:

```text
Algorithm
Parameters
Purpose
Provider
Policy State
Migration State
```

Nicht dargestellt werden dürfen:

```text
Password
Source Secret
Derived Key Material
```

## Normative Anforderungen

1. NovaOS MUSS eine zentrale KDF-Infrastruktur bereitstellen.
2. Passwörter DÜRFEN NICHT direkt als kryptografische Schlüssel verwendet werden.
3. Passwort-KDFs MÜSSEN konfigurierbare Kostenparameter unterstützen können.
4. Domain Separation MUSS unterstützt werden.
5. Unterschiedliche kryptografische Zwecke SOLLEN getrennte Schlüssel erhalten.
6. Abgeleitete Schlüssel SOLLEN direkt als geschützte Crypto-Objekte erzeugbar sein.
7. KDF-Algorithmen und Parameter MÜSSEN policygesteuert sein.
8. Ressourcenintensive KDFs MÜSSEN Resource Budgets berücksichtigen.
9. Fehler DÜRFEN NICHT zu einem stillen Sicherheits-Downgrade führen.
10. KDF-Konfigurationen MÜSSEN mit Crypto Agility vereinbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-ENCRYPTION-0001`
- `NPSPEC-CRYPTO-AEAD-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-CRYPTO-0005`

## Ergebnis

```text
Secret / Password
       ↓
Policy-controlled KDF
       ↓
Purpose-separated Key
       ↓
Protected Crypto Object
```

NovaOS erhält damit ein einheitliches und crypto-agiles Verfahren zur sicheren Schlüsselableitung, ohne Passwörter oder Master Keys direkt als operative Schlüssel verwenden zu müssen.