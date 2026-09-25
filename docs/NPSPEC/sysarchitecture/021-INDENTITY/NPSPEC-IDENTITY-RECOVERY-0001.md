# NPSPEC-IDENTITY-RECOVERY-0001 – Nova Identity Recovery

## Status

Angenommen

## Kategorie

Identity / Recovery / Authentication

## Zweck

NovaOS definiert Identity Recovery als kontrollierten Prozess zur Wiederherstellung des Zugangs zu einer bestehenden Identität, wenn reguläre Credentials verloren, beschädigt oder nicht mehr verfügbar sind.

```text
Identity
   ↓
Credentials Unavailable
   ↓
Recovery Verification
   ↓
Credential Replacement
   ↓
Identity Restored
```

Recovery stellt den Zugang zur bestehenden Identität wieder her und erzeugt keine neue Identität oder zusätzliche Autorität.

## Grundprinzipien

```text
Recovery ≠ Authentication Bypass
Recovery ≠ New Identity
Recovery ≠ Automatic Authority
Credential Loss ≠ Identity Loss
Recovery Credential ≠ Universal Master Key
Administrator ≠ Automatic Recovery Authority
```

## Recovery-Modell

Ein Recovery-Vorgang besitzt mindestens:

```text
RecoveryRequest
├── RecoveryID
├── IdentityID
├── Recovery Method
├── Verification State
└── State
```

Optional:

```text
Requested Credential Change
Security Domain
Recovery Authority
Required Factors
Trusted Device
Recovery Credential
Delay
Expiration
Risk Context
Audit Reference
```

## Recovery-Methoden

NovaOS muss unterschiedliche Recovery-Verfahren unterstützen können:

```text
Recovery Credential
Backup Passkey
Hardware Token
Trusted Device
Recovery Key
Federated Identity Verification
Multi-Party Approval
Controlled Administrative Recovery
```

Die zulässigen Methoden werden durch die jeweilige Recovery Policy bestimmt.

## Recovery Policy

Eine Identität kann eine explizite Recovery Policy besitzen.

```text
RecoveryPolicy
├── Allowed Methods
├── Required Verification
├── Credential Actions
└── Security Constraints
```

Hochprivilegierte Identitäten können stärkere Anforderungen besitzen als normale Benutzeridentitäten.

## Identitätskontinuität

Erfolgreiche Recovery muss die bestehende Identität erhalten.

```text
IdentityID = unchanged

Old Credential → Revoked
New Credential → Active
```

Historische Ownership-, Audit-, Capability- und Provenance-Beziehungen bleiben dadurch derselben Identität zugeordnet.

## Credential Replacement

Recovery soll bevorzugt verlorene Credentials ersetzen, statt diese wiederherzustellen.

```text
Lost Credential
      ↓
Recovery
      ↓
Revoke Old Credential
      ↓
Issue New Credential
```

Nicht mehr vertrauenswürdige Credentials dürfen nach erfolgreicher Recovery nicht weiterhin gültig bleiben.

## MFA und Step-Up

Recovery muss abhängig vom Sicherheitskontext mehrere Nachweise verlangen können.

```text
Recovery Request
      ↓
MFA / Step-Up
      ↓
Verified Recovery
```

Ein einzelner schwacher Faktor darf eine zuvor starke Authentifizierungsarchitektur nicht trivial umgehen.

## Recovery Delay

Für besonders kritische Identitäten oder Operationen kann eine Sicherheitsverzögerung vorgesehen werden.

```text
Recovery Requested
      ↓
Verification
      ↓
Security Delay
      ↓
Activation
```

Währenddessen können Benachrichtigung, Widerruf oder zusätzliche Verifikation möglich sein.

## Geräteverlust

Beim Verlust eines Geräts müssen gerätegebundene Credentials widerrufen werden können.

```text
Lost Device
├── Device Identity → Revoked
├── Passkeys → Revoked if required
└── Sessions → Revoked
```

Die Benutzeridentität selbst kann bestehen bleiben.

## Lokale Recovery

NovaOS muss Recovery für lokale Identitäten auch ohne Cloud-Abhängigkeit ermöglichen können.

Recovery kann hierfür beispielsweise auf:

```text
Recovery Key
Backup Credential
Hardware Token
NovaDOS Recovery Environment
```

basieren.

NovaDOS darf dabei keine universelle Umgehung der Identity- und Security-Policies darstellen.

## Remote und föderierte Recovery

Bei föderierten Identitäten kann ein externer Provider an der Recovery beteiligt sein.

```text
External Recovery
      ↓
Federation Verification
      ↓
Nova Recovery Policy
```

Eine erfolgreiche externe Recovery darf lokale Sperren oder Revocation nicht automatisch aufheben.

## Schlüssel und verschlüsselte Daten

Identity Recovery und Datenentschlüsselung müssen getrennt betrachtet werden.

```text
Recovered Identity
        ≠
Automatically Recovered Encryption Keys
```

Recovery von kryptografischen Schlüsseln ist nur zulässig, wenn dafür ein expliziter Key-Recovery-Mechanismus existiert.

## Audit

Sicherheitsrelevante Recovery-Ereignisse müssen nachvollziehbar sein.

```text
RecoveryID
IdentityID
Method
Decision
Credential Changes
Timestamp
```

Recovery-Secrets dürfen nicht im Audit-Log gespeichert werden.

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Identity Recovery unterstützen.
2. Recovery MUSS die bestehende `IdentityID` erhalten können.
3. Recovery DARF NICHT als universelle Authentifizierungsumgehung dienen.
4. Recovery Policies MÜSSEN mehrere Recovery-Verfahren unterstützen können.
5. Verlorene oder kompromittierte Credentials MÜSSEN widerrufbar sein.
6. Recovery SOLL Credentials ersetzen statt geheime Credentials wiederherzustellen.
7. MFA und Step-Up MÜSSEN für sensitive Recovery-Vorgänge integrierbar sein.
8. Lokale Identitäten MÜSSEN ohne zwingende Cloud-Abhängigkeit wiederherstellbar sein können.
9. Identity Recovery DARF NICHT automatisch kryptografische Schlüssel rekonstruieren.
10. Recovery-Vorgänge MÜSSEN sicher auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-IDENTITY-PASSKEY-0001`
- `NPSPEC-IDENTITY-MFA-0001`
- `NPSPEC-IDENTITY-STEPUP-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-BREAKGLASS-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `ADR-IDENTITY-0012`

## Ergebnis

```text
Lost / Unavailable Credential
          ↓
Recovery Policy
          ↓
Strong Verification
          ↓
Revoke Old Credentials
          ↓
Issue New Credentials
          ↓
Same IdentityID
```

NovaOS erhält damit eine kontrollierte Recovery-Architektur, die verlorene Credentials ersetzen und den Zugang zu einer bestehenden Identität wiederherstellen kann, ohne Recovery als universelle Hintertür oder automatische Wiederherstellung sämtlicher Autorität und Schlüssel zu behandeln.