# NPSPEC-CRYPTO-KEYROTATION-0001 – Nova Cryptographic Key Rotation

## Status

Angenommen

## Kategorie

Cryptography / Key Management / Key Rotation

## Zweck

NovaOS definiert einen einheitlichen Mechanismus zur kontrollierten Ersetzung kryptografischer Schlüssel.

```text
Key Version N
     ↓
Rotation
     ↓
Key Version N+1
```

Key Rotation ermöglicht den Austausch von Schlüsseln, ohne bestehende Daten oder Dienste unnötig zu unterbrechen.

## Grundprinzipien

```text
Rotation ≠ Revocation
Rotation ≠ Immediate Deletion
New Key ≠ Automatic Data Migration
KeyID ≠ Key Version
Old Key ≠ Automatically Invalid
```

## Rotationsmodell

Ein rotierbarer Schlüssel besitzt mindestens:

```text
KeyRotationState
├── KeyID
├── CurrentVersion
├── PreviousVersions
├── RotationPolicy
└── State
```

Optional:

```text
RotationInterval
Expiration
Activation Time
Migration State
Grace Period
Security Domain
```

## Schlüsselidentität

Die logische `KeyID` darf über mehrere Schlüsselversionen stabil bleiben.

```text
KeyID
├── Version 1
├── Version 2
└── Version 3
```

Neue kryptografische Operationen verwenden standardmäßig die aktuelle aktive Version.

## Rotation

Eine Rotation erfolgt kontrolliert:

```text
Generate New Key
      ↓
Validate
      ↓
Activate
      ↓
Migrate if required
      ↓
Retire Old Key
```

Der Wechsel soll transaktional erfolgen können.

## Alte Schlüsselversionen

Alte Schlüssel dürfen vorübergehend erhalten bleiben, wenn sie zum Zugriff auf bestehende Daten benötigt werden.

```text
New Data → Current Key

Old Data → Required Historical Key
```

Neue Daten dürfen nach erfolgreicher Rotation nicht standardmäßig mit einer veralteten Schlüsselversion erzeugt werden.

## Datenmigration

NovaOS soll Re-Encryption oder Re-Wrapping unterstützen:

```text
Old Key
   ↓
Decrypt / Unwrap
   ↓
New Key
   ↓
Encrypt / Rewrap
```

Migration muss kontrolliert, überprüfbar und unterbrechbar sein.

## Automatische Rotation

Policies dürfen Rotation auslösen anhand von:

```text
Time
Usage Count
Security Policy
Algorithm Migration
Compromise Response
Administrative Action
```

Automatische Rotation darf bestehende Sicherheits- oder Verfügbarkeitsanforderungen nicht umgehen.

## Rotation und Revocation

Rotation und Revocation sind getrennte Vorgänge.

```text
Rotation:
Old → New

Revocation:
Key → Invalid
```

Bei vermuteter Kompromittierung kann eine Rotation mit sofortiger Revocation der alten Version kombiniert werden.

## Provider und Hardware

Rotation muss mit unterschiedlichen Providern funktionieren können:

```text
Software
TPM
HSM
Secure Hardware
```

Providerwechsel dürfen nur erfolgen, wenn Key Policy und Exportregeln dies erlauben.

## Fehlerverhalten

Mögliche Fehler:

```text
GenerationFailed
ActivationFailed
MigrationFailed
PolicyDenied
ProviderFailure
RollbackRequired
```

Eine fehlgeschlagene Rotation darf nicht zu einem Zustand ohne gültigen Schlüssel führen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
KeyID
CurrentVersion
PreviousVersions
Rotation State
Last Rotation
Next Rotation
Migration State
```

Geheimes Schlüsselmaterial darf nicht sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Key Rotation unterstützen.
2. `KeyID` und konkrete Schlüsselversion MÜSSEN getrennt behandelbar sein.
3. Neue Operationen SOLLEN nach Rotation die aktuelle Schlüsselversion verwenden.
4. Alte Schlüsselversionen DÜRFEN für kontrollierte Migration erhalten bleiben.
5. Neue Daten DÜRFEN NICHT standardmäßig mit einer abgelösten Schlüsselversion erzeugt werden.
6. Rotation MUSS von Revocation getrennt behandelt werden.
7. Daten MÜSSEN kontrolliert auf neue Schlüssel migrierbar sein können.
8. Rotation SOLL transaktional erfolgen können.
9. Eine fehlgeschlagene Rotation DARF keinen unkontrollierten Verlust des gültigen Schlüsselzustands verursachen.
10. Rotations- und Migrationszustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-KEYWRAP-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-CRYPTO-0008`

## Ergebnis

```text
Stable KeyID
    ↓
Key Version N
    ↓
Controlled Rotation
    ↓
Key Version N+1
    ↓
Migration
    ↓
Retire Old Version
```

NovaOS erhält damit eine einheitliche Schlüsselrotation, bei der kryptografische Schlüssel erneuert werden können, ohne ihre logische Identität oder den kontrollierten Zugriff auf bestehende verschlüsselte Daten zu verlieren.