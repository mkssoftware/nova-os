# NPSPEC-CRYPTO-KEYWRAP-0001 – Nova Cryptographic Key Wrapping

## Status

Angenommen

## Kategorie

Cryptography / Key Management / Key Wrapping

## Zweck

NovaOS definiert Key Wrapping zur geschützten Speicherung und Übertragung kryptografischer Schlüssel.

```text
Key Material
    ↓
Key Wrapping
    ↓
Wrapped Key
    ↓
Storage / Transfer
```

Ein gewrappter Schlüssel darf nur durch autorisierte Verwendung des passenden Wrapping Keys wieder verfügbar werden.

## Grundprinzipien

```text
Wrapped Key ≠ Plain Key
Wrapping Key ≠ Wrapped Key
Key Wrapping ≠ Authorization
Wrapped ≠ Exportable
Unwrap Permission ≠ General Key Access
```

## Key-Wrap-Modell

Eine Wrapping-Operation besitzt mindestens:

```text
KeyWrapContext
├── KeyID
├── WrappingKeyID
├── Algorithm
├── WrappedKey
└── State
```

Optional:

```text
Associated Data
Target Provider
Security Domain
Purpose
Expiration
Export Policy
```

## Wrapping

```text
Protected Key
     +
Wrapping Key
     ↓
Key Wrap
     ↓
Wrapped Key Blob
```

Der erzeugte Blob muss kryptografisch gegen unerkannte Manipulation geschützt sein.

## Unwrapping

```text
Wrapped Key
     ↓
Authorization
     ↓
Key Unwrap
     ↓
Protected Key Object
```

Der entschlüsselte Schlüssel soll möglichst direkt in den Key Store oder Ziel-Provider übernommen werden.

Unnötige Klartextkopien des Schlüsselmaterials sind zu vermeiden.

## Wrapping Keys

Wrapping Keys müssen als besonders geschützte Crypto-Objekte behandelt werden.

Ihre Verwendung kann beschränkt werden auf:

```text
Wrap
Unwrap
Security Domain
Target Provider
Key Type
Lifetime
```

Ein Wrapping Key darf nicht automatisch exportierbar sein.

## Export

Key Wrapping ermöglicht kontrollierten Schlüsseltransport, erzeugt aber kein Exportrecht.

```text
Export Permission
      +
Wrap Permission
      ↓
Wrapped Export
```

Ein `NonExportable`-Schlüssel darf nicht allein durch Key Wrapping exportierbar werden.

## Provider-Transfer

Key Wrapping darf zur sicheren Migration zwischen Providern verwendet werden:

```text
Provider A
    ↓
Wrapped Transfer
    ↓
Provider B
```

Dies ist nur zulässig, wenn beide Provider und die Key Policy den Transfer erlauben.

Hardwaregebundene Schlüssel dürfen nicht automatisch migriert werden.

## Integrität und Kontextbindung

Wrapped Keys sollen an relevante Kontextinformationen gebunden werden können:

```text
KeyID
Purpose
Security Domain
Algorithm
Target Provider
```

Dadurch soll die Verwendung eines Wrapped Keys in einem unzulässigen Kontext verhindert werden.

## Rotation

Wrapping Keys müssen rotierbar sein.

```text
Old Wrapping Key
       ↓
Authorized Rewrap
       ↓
New Wrapping Key
```

Rewrapping soll ohne unnötige Offenlegung des enthaltenen Schlüsselmaterials möglich sein.

## Fehlerverhalten

Mindestens folgende Fehler müssen unterscheidbar sein:

```text
AuthenticationFailed
InvalidWrappedKey
InvalidWrappingKey
PolicyDenied
ProviderFailure
UnsupportedAlgorithm
```

Fehler dürfen keinen Fallback auf ungeschützten Schlüsselexport auslösen.

## Introspection

NovaOS soll sicher darstellen können:

```text
KeyID
WrappingKeyID
Algorithm
Provider
State
Export Policy
Target Provider
```

Schlüsselmaterial darf nicht angezeigt werden.

## Normative Anforderungen

1. NovaOS MUSS kontrolliertes Key Wrapping unterstützen.
2. Wrapped Keys MÜSSEN kryptografisch gegen Manipulation geschützt sein.
3. Wrap- und Unwrap-Rechte MÜSSEN separat kontrollierbar sein.
4. Key Wrapping DARF kein bestehendes Exportverbot umgehen.
5. Wrapping Keys MÜSSEN als geschützte Crypto-Objekte behandelt werden.
6. Unwrapped Keys SOLLEN direkt in geschützte Key-Objekte übernommen werden.
7. Provider-Transfers MÜSSEN die Key Policy respektieren.
8. Hardwarebindungen DÜRFEN NICHT automatisch durch Wrapping aufgehoben werden.
9. Wrapping Keys MÜSSEN kontrolliert rotierbar sein.
10. Fehler DÜRFEN NICHT zu ungeschütztem Schlüsselexport führen.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-AEAD-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-CRYPTO-0007`

## Ergebnis

```text
Protected Key
     ↓
Authorized Wrap
     ↓
Protected Key Blob
     ↓
Storage / Transfer
     ↓
Authorized Unwrap
     ↓
Protected Key Object
```

NovaOS erhält damit einen einheitlichen Mechanismus, kryptografische Schlüssel geschützt zu speichern, zu sichern oder zwischen autorisierten Providern zu übertragen, ohne bestehende Export- und Sicherheitsrichtlinien zu umgehen.