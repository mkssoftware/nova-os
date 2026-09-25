# NPSPEC-IDENTITY-APPLICATION-0001 – Nova Application Identity

## Status

Angenommen

## Kategorie

Identity / Application Identity / Security

## Zweck

NovaOS definiert stabile Identitäten für Anwendungen unabhängig von Prozess, Installation, Version oder Ausführungsort.

```text
Application
     ↓
Application Identity
     ↓
Verification
     ↓
Security Principal
     ↓
Capabilities + Policies
```

Die Application Identity ermöglicht NovaOS, Berechtigungen und Sicherheitsentscheidungen einer Anwendung eindeutig zuzuordnen.

## Grundprinzipien

```text
Application Identity ≠ Process Identity
Application Identity ≠ Installation Path
Application Identity ≠ Package Name
Application Identity ≠ Authority
Application Version ≠ New Identity
Signed Application ≠ Trusted Application
```

## Identitätsmodell

Eine Anwendung besitzt mindestens:

```text
ApplicationIdentity
├── IdentityID
├── ApplicationID
├── Publisher
└── State
```

Optional:

```text
Display Name
Version
PackageID
Signature Identity
Security Domain
Trust State
Installation Source
Capabilities
Privacy Policy
```

`IdentityID` ist die stabile NovaOS-Identität.

`ApplicationID` bezeichnet die logische Anwendung.

## Stabilität

Die Identität darf nicht von veränderlichen Eigenschaften abhängen:

```text
Installationspfad
Dateiname
Process ID
Version
Speicheradresse
Ausführungsort
```

Updates dürfen dieselbe Application Identity behalten, sofern die Update- und Code-Identity-Regeln dies erlauben.

## Anwendung und Prozess

Eine Anwendung kann mehrere Prozesse oder Tasks besitzen.

```text
Application Identity
├── Process A
├── Process B
└── Process C
```

Diese Ausführungsinstanzen dürfen nicht automatisch eigenständige Application Identities erzeugen.

Prozesse können zusätzliche eingeschränkte Security Contexts besitzen.

## Code Identity

Application Identity muss mit Code Signing und Code Integrity verbunden werden können.

```text
Application Identity
       ↓
Signed Code
       ↓
Integrity Verification
       ↓
Execution
```

Eine gültige Signatur bestätigt jedoch nicht automatisch Vertrauen oder Autorität.

## Autorität

Anwendungen erhalten Rechte ausschließlich über kontrollierte Sicherheitsmechanismen.

```text
Application Identity
       ↓
Security Policy
       ↓
Capabilities
       ↓
Authorized Resources
```

Typische Capabilities können betreffen:

```text
Files
Devices
Network
Clipboard
Camera
Microphone
Location
IPC
System Services
```

Es darf keine implizite Vollberechtigung allein aufgrund der Application Identity geben.

## Updates

Bei einem Update muss NovaOS prüfen:

```text
Existing Application Identity
            +
Update Identity / Signature
            ↓
Identity Continuity Check
            ↓
Updated Application
```

Ein nicht autorisierter Publisher-Wechsel darf die bestehende Identität und deren Berechtigungen nicht automatisch übernehmen.

## Sandboxing

Application Identity muss mit Sandbox- und Isolation Policies kombinierbar sein.

```text
Application Identity
        ↓
Sandbox Profile
        ↓
Capabilities
        ↓
Execution Environment
```

Mehrere Anwendungen dürfen trotz gleicher technischer Runtime getrennte Identitäten besitzen.

## Installation und Portabilität

Application Identity darf nicht an einen bestimmten Installationspfad gebunden sein.

```text
System:/Apps/App
        ↓ move
User:/Apps/App
```

Die logische Identität kann erhalten bleiben, sofern Integrität und Herkunft weiterhin verifiziert werden können.

## Remote und verteilte Ausführung

Bei Remote Execution oder Migration muss die Application Identity erhalten werden können.

```text
Identity ≠ Location
```

Der Zielknoten muss die Identität und erforderliche Autorität eigenständig validieren.

## Privacy

Privacy Policies können an Application Identities gebunden werden.

Dadurch kann NovaOS nachvollziehen:

```text
Which Application
        ↓
Accessed Which Data
        ↓
For Which Purpose
```

Application Identity kann damit als Kontext für Privacy Labels, DLP und Data Sovereignty dienen.

## Lebenszyklus

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

Eine Deinstallation kann die Application Identity stilllegen, ohne notwendigerweise historische Audit- oder Provenance-Referenzen zu zerstören.

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
ApplicationID
Publisher
Version
State
Trust State
Security Domain
Granted Capabilities
```

Geheime Credentials dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Anwendungen stabile Application Identities zuweisen können.
2. Application Identity MUSS von Prozess, Pfad und Version getrennt sein.
3. Application Identity DARF NICHT automatisch Autorität erzeugen.
4. Application Identity MUSS mit Code Signing und Code Integrity integrierbar sein.
5. Updates MÜSSEN die Kontinuität der Application Identity validieren.
6. Nicht autorisierte Publisher-Wechsel DÜRFEN bestehende Autorität NICHT übernehmen.
7. Anwendungen MÜSSEN über Capabilities begrenzbar sein.
8. Application Identity MUSS mit Sandbox- und Isolation Policies integrierbar sein.
9. Migration oder Standortwechsel DÜRFEN die logische Identität erhalten.
10. Application Identity SOLL als Kontext für Privacy-, Audit- und Provenance-Systeme verwendbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-SECURITY-ISOLATION-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `NPSPEC-PRIVACY-DLP-0001`
- `ADR-IDENTITY-0005`

## Ergebnis

```text
Application
     ↓
Stable Application Identity
     ↓
Code Identity + Verification
     ↓
Sandbox + Capabilities
     ↓
Authorized Execution
```

NovaOS erhält damit stabile Anwendungsidentitäten, die unabhängig von Prozess, Version, Installationspfad und Ausführungsort bestehen und als gemeinsame Grundlage für Berechtigungen, Sandboxing, Updates, Privacy und Audit dienen.