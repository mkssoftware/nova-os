# NPSPEC-IDENTITY-STEPUP-0001 – Nova Step-Up Authentication

## Status

Angenommen

## Kategorie

Identity / Authentication / Step-Up Authentication

## Zweck

NovaOS definiert Step-Up Authentication als gezielte Erhöhung der erforderlichen Authentifizierungsstärke für sicherheitskritische Operationen, ohne eine vollständige neue Anmeldung zu verlangen.

```text
Existing Session
      ↓
Sensitive Operation
      ↓
Required Authentication Strength
      ↓
Step-Up Authentication
      ↓
Temporary Stronger Context
```

## Grundprinzipien

```text
Authenticated ≠ Sufficiently Authenticated
Step-Up ≠ New Identity
Step-Up ≠ Additional Authority
MFA ≠ Always Required
Strong Session ≠ Permanently Strong Session
Authentication Strength ≠ Permission
```

## Step-Up-Modell

Eine Step-Up-Anforderung besitzt mindestens:

```text
StepUpRequest
├── IdentityID
├── Current Authentication Context
├── Required Authentication Strength
├── Target Operation
└── State
```

Optional:

```text
Required Factors
Security Domain
Resource
Purpose
Risk Context
User Presence
Expiration
Maximum Age
```

## Authentication Context

NovaOS muss die aktuelle Authentifizierungsstärke einer Session nachvollziehen können.

```text
AuthenticationContext
├── IdentityID
├── Authentication Methods
├── Authentication Strength
├── Authentication Time
└── State
```

Dadurch kann entschieden werden, ob eine erneute Authentifizierung erforderlich ist.

## Auslösung

Step-Up kann erforderlich werden durch:

```text
Sensitive Operation
Security Policy
Expired Authentication Strength
Changed Security Context
Remote Access
Credential Management
Key Management
Privacy-sensitive Data
Administrative Action
```

Beispiel:

```text
Normal Session
     ↓
Delete Cryptographic Key
     ↓
Step-Up Required
```

## Faktorwahl

Die Policy bestimmt die erforderliche Stärke.

```text
Current:
Password

Required:
Passkey + User Verification
```

oder:

```text
Current:
Passkey

Required:
Passkey + Hardware Token
```

Es müssen nur die Faktoren verlangt werden, die zur Erfüllung der Policy erforderlich sind.

## Temporäre Gültigkeit

Eine erfolgreiche Step-Up-Authentifizierung soll zeitlich oder auf einen Operationskontext begrenzt werden können.

```text
Step-Up
  ↓
Strong Context
  ↓
Operation
  ↓
Expiration
```

Mögliche Bindungen:

```text
Single Operation
Transaction
Security Domain
Short Time Window
Specific Resource
```

Eine einmalige starke Authentifizierung darf nicht automatisch dauerhaft gelten.

## Autorisierung

Step-Up verändert nicht automatisch die vorhandenen Rechte.

```text
Authentication Strength
        +
Authorization
        ↓
Operation Allowed
```

Besitzt die Identität keine erforderliche Capability, darf auch erfolgreiche Step-Up Authentication die Operation nicht erlauben.

## Session-Bindung

Ein Step-Up-Ergebnis muss an die authentifizierte Session und Identität gebunden sein.

```text
Identity A + Session A
        ↓
Step-Up Context
```

Der stärkere Authentication Context darf nicht unkontrolliert auf andere Sessions, Prozesse oder Identitäten übertragen werden.

## Remote Operationen

Bei Remote Operations muss die Zielseite die erforderliche Authentication Strength überprüfen können.

```text
Client Authentication Context
          ↓
Protected Assertion
          ↓
Remote Policy Evaluation
```

Eine entfernte Seite darf eine behauptete Authentication Strength nicht ungeprüft übernehmen.

## Fehler und Abbruch

Fehlgeschlagene oder abgebrochene Step-Up Authentication darf die bestehende Session nicht automatisch erweitern.

```text
Step-Up Failed
      ↓
Sensitive Operation Denied
      ↓
Existing Session remains bounded
```

Policies dürfen bei wiederholten Fehlversuchen zusätzliche Schutzmaßnahmen auslösen.

## Privacy

Step-Up darf nur die zur Verifikation notwendigen Informationen offenlegen.

Biometrische Rohdaten oder private Schlüssel dürfen nicht an die anfordernde Anwendung übertragen werden.

## Audit und Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Target Operation
Previous Authentication Strength
Required Authentication Strength
Achieved Authentication Strength
Validity
Decision
```

Geheime Authentifizierungsdaten dürfen nicht protokolliert werden.

## Normative Anforderungen

1. NovaOS MUSS Step-Up Authentication unterstützen können.
2. Sessions MÜSSEN einen nachvollziehbaren Authentication Context besitzen können.
3. Policies MÜSSEN erforderliche Authentication Strength definieren können.
4. Step-Up MUSS auf Operationen, Ressourcen oder Zeiträume begrenzbar sein.
5. Erfolgreiches Step-Up DARF NICHT automatisch zusätzliche Autorität erzeugen.
6. Fehlgeschlagenes Step-Up DARF die geschützte Operation NICHT freigeben.
7. Step-Up-Ergebnisse MÜSSEN an Identität und Session bindbar sein.
8. Remote Authentication Strength MUSS verifizierbar sein.
9. Step-Up-Kontexte MÜSSEN ablaufen oder explizit ungültig werden können.
10. Step-Up-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-PASSKEY-0001`
- `NPSPEC-IDENTITY-MFA-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `ADR-IDENTITY-0010`

## Ergebnis

```text
Existing Authentication
         ↓
Sensitive Operation
         ↓
Step-Up Policy
         ↓
Stronger Authentication
         ↓
Temporary Authentication Context
         ↓
Authorization Check
         ↓
Authorized Operation
```

NovaOS erhält damit eine kontextabhängige Step-Up-Architektur, die für besonders sensible Operationen gezielt eine stärkere Identitätsprüfung verlangen kann, ohne Authentifizierungsstärke mit tatsächlicher Autorität gleichzusetzen.