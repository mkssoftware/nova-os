# NPSPEC-IDENTITY-SERVICE-0001 – Nova Service Identity

## Status

Angenommen

## Kategorie

Identity / Service Identity / Security

## Zweck

NovaOS definiert ein einheitliches Identitätsmodell für Systemdienste, Hintergrunddienste und verteilte Services.

```text
Service
   ↓
Service Identity
   ↓
Authentication
   ↓
Security Principal
   ↓
Capabilities + Policies
```

Jeder sicherheitsrelevante Dienst soll eindeutig identifizierbar sein, ohne seine Identität an Prozess-ID, Netzwerkadresse oder konkrete Ausführungsinstanz zu binden.

## Grundprinzipien

```text
Service Identity ≠ Process Identity
Service Identity ≠ Network Address
Service Identity ≠ Authority
Service Identity ≠ Service Location
Authenticated Service ≠ Trusted Service
```

## Identitätsmodell

Eine Service Identity besitzt mindestens:

```text
ServiceIdentity
├── IdentityID
├── ServiceID
├── Provider
└── State
```

Optional:

```text
Service Type
Security Domain
Credentials
Trust State
Owner
Instance Identity
Version
Attestation Reference
Sovereignty Policy
```

`IdentityID` beschreibt die stabile Identität des Dienstes.

`ServiceID` beschreibt die logische Service-Schnittstelle bzw. Funktion.

## Service und Instanz

Die logische Service Identity muss von einzelnen laufenden Instanzen getrennt sein.

```text
Service Identity
├── Instance A
├── Instance B
└── Instance C
```

Eine Instanz kann beendet, ersetzt, migriert oder neu gestartet werden, ohne dass sich dadurch automatisch die logische Service Identity ändert.

## Prozessunabhängigkeit

Eine Prozess-ID darf nicht als Service Identity verwendet werden.

```text
PID 4711
   ↓ restart
PID 8124

Service Identity → unchanged
```

Dies unterstützt Restart, Supervision, Live Replacement und Migration.

## Lokale und entfernte Services

Das Modell muss sowohl lokale als auch entfernte Dienste unterstützen:

```text
Local Service
Remote Service
Distributed Service
Federated Service
```

Die Identität bleibt dabei von ihrem aktuellen Standort getrennt.

```text
Identity ≠ Location
```

## Service-Authentifizierung

Services müssen sich gegenüber anderen Systemkomponenten authentifizieren können.

Geeignete Verfahren können umfassen:

```text
Capability-bound Identity
Cryptographic Credential
Mutual Authentication
Hardware-backed Credential
Attestation
```

Die Authentifizierung erzeugt keine automatische Autorität.

## Autorität

Service-Zugriffe müssen weiterhin durch Capabilities und Security Policies kontrolliert werden.

```text
Service Identity
      ↓
Authentication
      ↓
Trust Evaluation
      ↓
Capability Check
      ↓
Authorized Operation
```

Ein Dienst darf nur die für seine Funktion erforderlichen Rechte erhalten.

## Service-to-Service-Kommunikation

Bei Kommunikation zwischen Services müssen Identität und Autorität unabhängig geprüft werden können.

```text
Service A
   ↓
Authenticated Channel
   ↓
Service B
   ↓
Capability / Policy Check
```

TLS oder ein anderer geschützter Transport ersetzt keine Autorisierung.

## Delegation

Ein Service darf Autorität nur kontrolliert an einen anderen Service delegieren.

```text
Authority(A)
     ↓ attenuation
Delegated Authority(B)
```

Delegierte Rechte dürfen die ursprüngliche Autorität nicht erweitern.

## Lifecycle

Service Identities unterstützen mindestens:

```text
Register
  ↓
Active
  ↓
Suspended
  ↓
Revoked
  ↓
Retired
```

Das Beenden einer Service-Instanz muss nicht automatisch die logische Service Identity entfernen.

## Live Replacement

Service Identity muss NovaOS Live Evolution unterstützen.

```text
Old Instance
     ↓
State Transfer
     ↓
New Instance
     ↓
Same Service Identity
```

Die neue Instanz darf Autorität erst übernehmen, nachdem ihre Integrität und Berechtigung geprüft wurden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
ServiceID
State
Provider
Security Domain
Trust State
Active Instances
```

Credentials oder geheimes Schlüsselmaterial dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Services eindeutige Identitäten zuweisen können.
2. Service Identity MUSS von Prozess-ID, Netzwerkadresse und Standort getrennt sein.
3. Mehrere Instanzen MÜSSEN derselben logischen Service Identity zugeordnet werden können.
4. Service Identity DARF NICHT automatisch Autorität erzeugen.
5. Service-to-Service-Authentifizierung MUSS unterstützt werden können.
6. Service-Autorität MUSS durch Capabilities und Policies begrenzbar sein.
7. Delegierte Autorität DARF die ursprüngliche Autorität NICHT erweitern.
8. Service Identities MÜSSEN widerrufbar sein.
9. Live Replacement und Migration DÜRFEN die stabile Service Identity erhalten.
10. Service- und Instanzzustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-PROCESS-SUPERVISION-0001`
- `ADR-IDENTITY-0004`

## Ergebnis

```text
Logical Service Identity
          ↓
Authentication + Trust
          ↓
Service Instance
          ↓
Capabilities + Policies
          ↓
Authorized Service Operation
```

NovaOS erhält damit stabile, standort- und prozessunabhängige Service-Identitäten, die Restart, Migration, verteilte Services und Live Replacement unterstützen, ohne Identität mit Autorität gleichzusetzen.