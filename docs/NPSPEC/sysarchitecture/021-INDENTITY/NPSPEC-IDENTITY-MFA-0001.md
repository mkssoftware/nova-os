# NPSPEC-IDENTITY-MFA-0001 – Nova Multi-Factor Authentication

## Status

Angenommen

## Kategorie

Identity / Authentication / Multi-Factor Authentication

## Zweck

NovaOS definiert Multi-Factor Authentication (MFA) als kombinierte Nutzung unabhängiger Authentifizierungsfaktoren zur stärkeren Verifikation einer Identität.

```text
Identity
   ↓
Factor A + Factor B
   ↓
Authentication Policy
   ↓
Authenticated Identity
```

MFA erhöht die Sicherheit der Authentifizierung, erzeugt jedoch keine zusätzliche Autorität.

## Grundprinzipien

```text
MFA ≠ Authorization
Two Credentials ≠ Two Factors
More Factors ≠ Automatically More Secure
Biometric Factor ≠ Identity
Device Possession ≠ User Presence
MFA Success ≠ Unlimited Authority
```

## Faktoren

NovaOS unterscheidet mindestens:

```text
Knowledge
├── Password
└── PIN

Possession
├── Passkey Device
├── Hardware Token
└── Trusted Device

Inherence
└── Biometric Verification
```

Mehrere Nachweise derselben Faktorkategorie dürfen nicht automatisch als unabhängige MFA-Faktoren gelten.

## MFA-Policy

Eine MFA-Anforderung besitzt mindestens:

```text
MFAPolicy
├── PolicyID
├── Required Factors
├── Authentication Context
└── State
```

Optional:

```text
Allowed Factor Types
Minimum Authentication Strength
Required User Presence
Required User Verification
Trusted Device Policy
Risk Context
Expiration
Recovery Policy
```

## Adaptive Anforderungen

NovaOS muss unterschiedliche MFA-Anforderungen abhängig vom Sicherheitskontext unterstützen können.

```text
Normal Operation
      ↓
Standard Authentication

Sensitive Operation
      ↓
Step-Up MFA
```

Zusätzliche Faktoren können beispielsweise erforderlich sein für:

```text
Security Settings
Key Management
Credential Changes
Administrative Operations
Sensitive Data
Recovery
Remote Access
```

## Step-Up Authentication

Eine bestehende Session darf für besonders sensitive Operationen eine erneute stärkere Authentifizierung verlangen.

```text
Existing Session
      ↓
Sensitive Request
      ↓
Step-Up MFA
      ↓
Temporary Elevated Authentication Context
```

Step-Up Authentication darf keine dauerhafte Erweiterung der Autorität erzeugen.

## Passkeys

Passkeys können als starker Authentifizierungsfaktor verwendet werden.

```text
Passkey
+
Local User Verification
↓
Strong Authentication
```

Ob dies die jeweilige MFA-Policy erfüllt, wird durch den Authentication Context bestimmt.

## Faktorunabhängigkeit

Faktoren sollen möglichst unabhängig voneinander sein.

```text
Compromise Factor A
        ↓
Factor B remains protected
```

Mehrere Faktoren, die vollständig vom selben ungeschützten Geheimnis abhängen, dürfen nicht als starke MFA-Kombination behandelt werden.

## Recovery

MFA-Recovery muss kontrolliert erfolgen.

Mögliche Recovery-Mechanismen:

```text
Recovery Credential
Backup Passkey
Hardware Token
Trusted Device
Controlled Identity Recovery
```

Recovery darf MFA nicht durch eine dauerhaft schwächere Hintertür ersetzen.

## Revocation

Einzelne Faktoren müssen unabhängig widerrufbar sein.

```text
Identity
├── Passkey → Active
├── Hardware Token → Revoked
└── Recovery Credential → Active
```

Kompromittierte Faktoren dürfen nach Revocation nicht weiter zur MFA-Erfüllung beitragen.

## Offline-Betrieb

Lokale MFA muss ohne Netzwerkverbindung möglich sein, sofern alle benötigten Faktoren lokal validierbar sind.

```text
Local Identity
      +
Local Factors
      ↓
Offline MFA
```

Cloud- oder Provider-Ausfälle dürfen nicht unnötig lokale Authentifizierung verhindern.

## Privacy

Biometrische Rohdaten sollen lokal verarbeitet werden.

```text
Biometric Input
      ↓
Local Verification
      ↓
Verification Result
```

Anwendungen und entfernte Dienste sollen nur das notwendige Authentifizierungsergebnis erhalten.

## Audit und Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Authentication Context
Factor Types
Authentication Strength
MFA Policy
State
Timestamp
```

Passwörter, private Schlüssel, biometrische Rohdaten oder andere geheime Credentials dürfen nicht protokolliert oder offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Multi-Factor Authentication unterstützen.
2. MFA-Faktoren MÜSSEN nach Faktorkategorien unterscheidbar sein.
3. Mehrere Credentials derselben Kategorie DÜRFEN NICHT automatisch als unabhängige Faktoren gelten.
4. MFA MUSS policygesteuert anforderbar sein.
5. Step-Up Authentication MUSS für sensitive Operationen unterstützt werden können.
6. MFA-Erfolg DARF NICHT automatisch zusätzliche Autorität erzeugen.
7. Einzelne Authentifizierungsfaktoren MÜSSEN widerrufbar sein.
8. MFA-Recovery DARF KEINE dauerhaft schwächere Sicherheitsumgehung erzeugen.
9. Biometrische Rohdaten SOLLEN lokal verbleiben.
10. MFA-Zustand und Authentication Strength SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-PASSKEY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-BREAKGLASS-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-RNG-0001`
- `ADR-IDENTITY-0009`

## Ergebnis

```text
Identity
   ↓
Independent Authentication Factors
   ↓
MFA Policy
   ↓
Verified Authentication Context
   ↓
Security Policy + Capabilities
```

NovaOS erhält damit eine einheitliche MFA-Architektur für lokale, entfernte und sensitive Authentifizierungsvorgänge, bei der Faktorstärke, Step-Up Authentication, Recovery und Revocation kontrolliert werden, ohne Authentifizierung mit Autorität gleichzusetzen.