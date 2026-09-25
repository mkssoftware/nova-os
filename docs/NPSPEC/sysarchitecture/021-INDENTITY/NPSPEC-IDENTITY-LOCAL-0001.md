# NPSPEC-IDENTITY-LOCAL-0001 – Nova Local Identity

## Status

Angenommen

## Kategorie

Identity / Local Identity / Account Management

## Zweck

NovaOS definiert lokale Identitäten für Benutzer und Systementitäten, deren Identitätsquelle vollständig auf dem lokalen NovaOS-System liegt.

```text
Local Entity
     ↓
Local Identity Store
     ↓
Authentication
     ↓
Security Principal
```

Eine lokale Identität benötigt keinen externen Identity Provider oder Netzwerkzugriff.

## Grundprinzipien

```text
Local Identity ≠ Local Administrator
Account Name ≠ Identity
Authentication ≠ Authorization
Offline ≠ Unprotected
Local User ≠ Unlimited Authority
```

## Identitätsmodell

Eine lokale Identität besitzt mindestens:

```text
LocalIdentity
├── IdentityID
├── Type
├── Local Provider
└── State
```

Optional:

```text
Display Name
Authentication Methods
Attributes
Security Domain
Profile Reference
Created
Last Authentication
```

Die `IdentityID` bleibt unabhängig von Benutzername, Anzeigename oder Profilpfad stabil.

## Identitätstypen

Lokale Identitäten können mindestens umfassen:

```text
User
Service
Application
System Component
Machine
```

Interaktive Benutzerkonten und technische Systemidentitäten müssen unterscheidbar sein.

## Lokaler Identity Store

NovaOS muss lokale Identitätsinformationen in einem geschützten Identity Store verwalten.

```text
Nova Local Identity Store
├── Identity Records
├── Authentication References
├── Attributes
└── State
```

Passwörter oder andere geheime Credentials dürfen nicht als Klartext gespeichert werden.

## Authentifizierung

Lokale Identitäten müssen ohne Netzwerkverbindung authentifizierbar sein.

Unterstützte Verfahren können umfassen:

```text
Password
PIN
Hardware Token
Biometric Credential
Recovery Credential
```

Die konkrete Authentifizierung wird von separaten Identity- und Authentication-Mechanismen bestimmt.

## Autorität

Nach erfolgreicher Authentifizierung entsteht ein Security Principal.

```text
Local Identity
      ↓
Authentication
      ↓
Security Principal
      ↓
Capabilities + Security Policies
```

Die lokale Identität selbst erzeugt keine unbegrenzte Autorität.

## Systemidentitäten

Dienste und Systemkomponenten sollen eigene lokale Identitäten besitzen können.

```text
Service A → Identity A
Service B → Identity B
```

Gemeinsam genutzte universelle Systemidentitäten sollen vermieden werden.

Dies unterstützt Least Privilege und Isolation.

## Lebenszyklus

Lokale Identitäten unterstützen mindestens:

```text
Create
 ↓
Active
 ↓
Suspended
 ↓
Disabled
 ↓
Retired
```

Deaktivierung muss bestehende Sessions und Autorität berücksichtigen können.

## Recovery

NovaOS muss lokale Identity Recovery ermöglichen können.

```text
Identity Recovery
      ↓
Strong Verification
      ↓
Credential Replacement
```

Recovery darf keine universelle Hintertür erzeugen.

Break-Glass-Mechanismen bleiben separat kontrolliert.

## Offline-Betrieb

Lokale Anmeldung muss auch ohne:

```text
Internet
Cloud Service
External Identity Provider
Organization Server
```

funktionieren können.

Externe Ausfälle dürfen lokale Identitäten nicht grundsätzlich unbrauchbar machen.

## Privacy

Lokale Identitätsdaten sollen standardmäßig lokal verbleiben.

Synchronisation oder Übertragung an externe Provider muss eine separate Policy-Entscheidung sein.

```text
Local Identity ≠ Cloud Identity
```

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Type
Display Name
State
Authentication Methods
Security Domain
Provider = Local
```

Credentials oder geheime Authentifizierungsdaten dürfen nicht angezeigt werden.

## Normative Anforderungen

1. NovaOS MUSS vollständig lokale Identitäten unterstützen.
2. Lokale Identitäten MÜSSEN ohne Netzwerkzugriff funktionieren können.
3. Jede lokale Identität MUSS eine stabile `IdentityID` besitzen können.
4. Benutzername und Anzeigename DÜRFEN NICHT als stabile Identität verwendet werden.
5. Credentials DÜRFEN NICHT ungeschützt gespeichert werden.
6. Lokale Identität DARF NICHT automatisch Administratorrechte erzeugen.
7. Dienste und Systemkomponenten SOLLEN eigene Identitäten verwenden.
8. Lokale Identitäten MÜSSEN deaktivierbar sein.
9. Recovery DARF KEINE universelle Sicherheitsumgehung erzeugen.
10. Lokale Identitätsdaten SOLLEN ohne explizite Policy nicht extern übertragen werden.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-BREAKGLASS-0001`
- `NPSPEC-CRYPTO-KDF-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `ADR-IDENTITY-0002`

## Ergebnis

```text
Local Identity Store
        ↓
Stable IdentityID
        ↓
Offline Authentication
        ↓
Security Principal
        ↓
Capabilities + Policies
```

NovaOS erhält damit ein vollständig lokales und offline-fähiges Identitätsmodell, das unabhängig von Cloud- oder Netzwerkdiensten funktioniert und lokale Identität klar von tatsächlicher Autorität trennt.