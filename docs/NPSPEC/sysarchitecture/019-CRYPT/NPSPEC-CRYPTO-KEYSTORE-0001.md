# NPSPEC-CRYPTO-KEYSTORE-0001 – Nova Cryptographic Key Store

## Status

Angenommen

## Kategorie

Cryptography / Key Management / Key Store

## Zweck

NovaOS definiert einen zentralen Key Store zur sicheren Speicherung und Verwendung kryptografischer Schlüssel.

```text
Application / Service
        ↓
      KeyID
        ↓
Nova Crypto Key Store
        ↓
Software / TPM / HSM
```

Anwendungen sollen kryptografisches Schlüsselmaterial normalerweise nicht direkt besitzen müssen.

## Grundprinzipien

```text
KeyID ≠ Key Material
Key Access ≠ Key Export
Key Possession ≠ Unlimited Use
Stored ≠ Exportable
Administrator ≠ Automatic Key Access
```

## Schlüsselobjekt

Ein gespeicherter Schlüssel besitzt mindestens:

```text
CryptoKey
├── KeyID
├── Key Type
├── Allowed Usage
├── Provider
└── State
```

Optional:

```text
Owner
Security Domain
Algorithm
Expiration
Export Policy
Hardware Binding
Sovereignty Policy
Rotation Policy
Metadata
```

Mögliche Zustände:

```text
Active
Disabled
Expired
Revoked
Destroyed
Unavailable
```

## Speicherung

Schlüssel dürfen abhängig von Policy und Hardware gespeichert werden in:

```text
Protected System Storage
TPM
HSM
Secure Hardware
External Security Provider
```

Die physische Position eines Schlüssels darf von seiner logischen `KeyID` getrennt sein.

```text
Key Identity ≠ Key Location
```

## Schlüsselzugriff

Operationen sollen bevorzugt über Handles oder `KeyID` erfolgen.

```text
KeyID
  +
Operation
  ↓
Authorization
  ↓
Crypto Provider
  ↓
Result
```

Der Key Store darf kryptografische Operationen ausführen lassen, ohne das Schlüsselmaterial an den Aufrufer auszugeben.

## Berechtigungen

Schlüsselzugriffe müssen über NovaOS-Sicherheitsmechanismen kontrolliert werden.

Beispielrechte:

```text
Use
Sign
Verify
Encrypt
Decrypt
Derive
Export
Rotate
Revoke
Destroy
```

`Use` oder `Sign` darf nicht automatisch `Export` erlauben.

## Export

Schlüssel können eine Export Policy besitzen:

```text
NonExportable
WrappedExportOnly
AuthorizedExport
```

Hardwaregebundene Schlüssel dürfen vollständig nicht exportierbar sein.

## Import und Erzeugung

Schlüssel müssen sicher erzeugt oder importiert werden können.

```text
Generate → Validate → Store → Activate
Import   → Validate → Store → Activate
```

Importiertes Schlüsselmaterial muss nach Übergabe möglichst früh aus temporären Speicherbereichen entfernt werden.

## Rotation

Schlüssel müssen kontrolliert rotierbar sein.

```text
Key Version N
     ↓
Rotation
     ↓
Key Version N+1
```

Bestehende Daten dürfen während einer Migration weiterhin eindeutig dem benötigten Schlüssel zugeordnet werden.

## Revocation und Zerstörung

```text
Active
  ↓
Revoke
  ↓
No New Operations
```

Zerstörung entfernt die Möglichkeit zur weiteren Verwendung, soweit dies durch den jeweiligen Provider technisch gewährleistet werden kann.

```text
Revocation ≠ Destruction
```

## Backup und Recovery

Backup von Schlüsseln darf nur erfolgen, wenn die jeweilige Policy dies erlaubt.

Recovery darf Sicherheits- und Exportregeln nicht automatisch umgehen.

Nicht exportierbare Schlüssel benötigen gegebenenfalls separate Recovery-Strategien.

## Isolation

Schlüssel unterschiedlicher:

```text
Users
Applications
Services
Security Domains
```

müssen logisch voneinander isolierbar sein.

Die Kenntnis einer `KeyID` erzeugt keine Zugriffsberechtigung.

## Introspection

NovaOS soll sicher darstellen können:

```text
KeyID
Type
Algorithm
Provider
State
Allowed Usage
Export Policy
Expiration
Rotation State
```

Geheimes Schlüsselmaterial darf nicht über Introspection offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS einen zentralen kryptografischen Key Store bereitstellen.
2. Schlüssel MÜSSEN über stabile `KeyID`s referenzierbar sein.
3. Schlüsselidentität MUSS von physischem Speicherort und Provider getrennt sein können.
4. Schlüsselverwendung MUSS explizit autorisiert werden.
5. Schlüsseloperation und Schlüsselexport MÜSSEN getrennte Rechte sein.
6. Nicht exportierbare Schlüssel MÜSSEN unterstützt werden.
7. TPM-, HSM- und andere Hardware-Provider MÜSSEN integrierbar sein.
8. Schlüssel MÜSSEN rotierbar, widerrufbar und zerstörbar sein können.
9. Backup und Recovery MÜSSEN die jeweilige Key Policy respektieren.
10. Geheimes Schlüsselmaterial DARF NICHT über normale Logs oder Introspection offengelegt werden.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-KDF-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-BOOT-TPM-0001`
- `ADR-CRYPTO-0006`

## Ergebnis

```text
KeyID
 ↓
Authorization
 ↓
Nova Key Store
 ↓
Protected Key
 ↓
Authorized Crypto Operation
```

NovaOS erhält damit eine zentrale und providerunabhängige Schlüsselverwaltung, bei der kryptografische Schlüssel sicher verwendet werden können, ohne ihr geheimes Schlüsselmaterial unnötig an Anwendungen oder Dienste weiterzugeben.