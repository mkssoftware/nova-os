# NPSPEC-CRYPTO-SECUREERASE-0001 – Nova Cryptographic Secure Erase

## Status

Angenommen

## Kategorie

Cryptography / Key Management / Secure Erase

## Zweck

NovaOS definiert Secure Erase als kontrollierten Mechanismus zur dauerhaften Unbrauchbarmachung sensibler Daten und kryptografischer Schlüssel.

```text
Sensitive Data
      ↓
Secure Erase
      ↓
Recovery No Longer Possible
```

Bei verschlüsselten Daten soll bevorzugt kryptografisches Löschen durch sichere Vernichtung der relevanten Schlüssel verwendet werden.

## Grundprinzipien

```text
Delete ≠ Secure Erase
Free Space ≠ Erased Data
Revocation ≠ Destruction
Key Deletion ≠ Always Data Deletion
Overwrite ≠ Universally Reliable
Secure Erase ≠ Guaranteed Physical Destruction
```

## Secure-Erase-Modell

Eine Löschoperation besitzt mindestens:

```text
SecureEraseOperation
├── OperationID
├── TargetID
├── Erase Method
├── Authorization
└── State
```

Optional:

```text
KeyID
Provider
Storage Type
Verification
Security Domain
Audit Policy
```

Mögliche Zustände:

```text
Pending
Executing
Completed
Failed
Unknown
```

## Löschmethoden

NovaOS muss abhängig vom Ziel unterschiedliche Verfahren unterstützen können:

```text
Cryptographic Erase
Key Destruction
Hardware Secure Erase
Memory Zeroization
Controlled Overwrite
Provider-specific Erase
```

Die Methode muss zur tatsächlichen Speichertechnologie passen.

## Cryptographic Erase

Für verschlüsselte Daten soll kryptografisches Löschen bevorzugt werden.

```text
Encrypted Data
      +
Unique Encryption Key
      ↓
Destroy Key
      ↓
Data Cryptographically Inaccessible
```

Dies setzt voraus, dass keine nutzbare Kopie des Schlüssels verbleibt.

## Schlüsselvernichtung

Der Key Store muss Schlüssel kontrolliert zerstören können.

```text
KeyID
 ↓
Authorization
 ↓
Destroy
 ↓
Destroyed
```

Dabei müssen soweit relevant berücksichtigt werden:

```text
Key Copies
Wrapped Keys
Backups
Provider Copies
Cached Key Material
Historical Versions
```

`Destroyed` darf nur gemeldet werden, wenn die zugesicherte Löschsemantik erfüllt wurde.

## Speichertechnologien

NovaOS darf nicht davon ausgehen, dass mehrfaches Überschreiben auf jeder Speichertechnologie zuverlässig ist.

Besonders:

```text
SSD
NVMe
Flash
Wear Leveling
Copy-on-Write Storage
Snapshots
```

können physische Kopien außerhalb des direkt sichtbaren Speicherbereichs besitzen.

Hardware- oder kryptografische Löschverfahren sollen deshalb bevorzugt werden, wenn verfügbar.

## Arbeitsspeicher

Sensible temporäre Daten sollen nach Verwendung soweit technisch möglich zeroisiert werden.

```text
Keys
Passwords
Derived Secrets
Intermediate Crypto State
```

Compiler- oder Runtime-Optimierungen dürfen die vorgesehene Zeroization nicht unbeabsichtigt entfernen.

## Snapshots und Backups

Secure Erase muss bestehende Kopien berücksichtigen können:

```text
Snapshots
Backups
Replicas
Caches
Remote Copies
```

Das Löschen des aktiven Objekts bedeutet nicht automatisch, dass alle historischen Kopien gelöscht wurden.

## Verteilte Daten

Bei verteilten Daten muss NovaOS den Löschzustand einzelner Replikate verfolgen können.

```text
Erase Request
     ↓
Replica A → Completed
Replica B → Completed
Replica C → Unknown
```

`Unknown` darf nicht als erfolgreich gelöscht dargestellt werden.

## Autorisierung

Secure Erase ist eine sicherheitskritische Operation und muss explizit autorisiert werden.

```text
Delete Permission ≠ Secure Erase Permission
```

Besonders kritische Schlüssel oder Systemdaten dürfen zusätzliche Bestätigung oder Policy-Prüfung verlangen.

## Audit

Secure-Erase-Vorgänge sollen auditierbar sein, ohne die gelöschten Geheimnisse selbst zu protokollieren.

Mindestens:

```text
OperationID
TargetID
Method
Requester
Timestamp
Result
```

## Normative Anforderungen

1. NovaOS MUSS Secure Erase von normalem Löschen unterscheiden.
2. Kryptografisches Löschen SOLL für geeignet verschlüsselte Daten bevorzugt werden.
3. Schlüsselvernichtung MUSS alle kontrollierten Schlüsselkopien berücksichtigen können.
4. Secure Erase MUSS die Eigenschaften der Speichertechnologie berücksichtigen.
5. Überschreiben DARF NICHT als universell zuverlässige Löschmethode angenommen werden.
6. Sensibles temporäres Schlüsselmaterial SOLL sicher zeroisiert werden.
7. Snapshots, Backups und Replikate MÜSSEN bei Löschgarantien berücksichtigt werden.
8. `Unknown` DARF NICHT als erfolgreiche Löschung gelten.
9. Secure Erase MUSS explizit autorisiert werden.
10. Löschzustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-KEYWRAP-0001`
- `NPSPEC-CRYPTO-KEYROTATION-0001`
- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `ADR-CRYPTO-0011`

## Ergebnis

```text
Sensitive Data / Key
        ↓
Authorized Secure Erase
        ↓
Appropriate Erase Method
        ↓
Verification
        ↓
Destroyed / Failed / Unknown
```

NovaOS erhält damit einen einheitlichen Mechanismus zur sicheren Unbrauchbarmachung sensibler Daten und Schlüssel, der Kryptografie, Speichertechnologie, Backups und verteilte Kopien berücksichtigt.