# NPSPEC-IDENTITY-LIFECYCLE-0001 – Nova Identity Lifecycle

## Status

Angenommen

## Kategorie

Identity / Lifecycle / Identity Management

## Zweck

NovaOS definiert einen einheitlichen Lebenszyklus für lokale, föderierte, Geräte-, Service-, Application- und Workload-Identitäten.

```text
Create
  ↓
Activate
  ↓
Use
  ↓
Suspend / Modify
  ↓
Revoke / Retire
```

Der Lebenszyklus einer Identität wird von den Lebenszyklen ihrer Credentials, Sessions und Capabilities getrennt behandelt.

## Grundprinzipien

```text
Identity ≠ Credential
Identity ≠ Session
Identity ≠ Authority
Disabled ≠ Deleted
Credential Rotation ≠ Identity Change
Identity Retirement ≠ History Deletion
```

## Lifecycle-Modell

Eine Identity besitzt mindestens:

```text
IdentityLifecycle
├── IdentityID
├── Identity Type
├── State
└── State Version
```

Optional:

```text
Created
Activated
Suspended
Retired
Expiration
Provider
Parent Identity
Security Domain
Lifecycle Policy
Revocation Reference
```

## Zustände

NovaOS definiert mindestens:

```text
Pending
Active
Suspended
Disabled
Revoked
Retired
```

Optional können Identitätstypen zusätzliche Zustände besitzen.

### Pending

Die Identität wurde angelegt, ist aber noch nicht vollständig aktiviert.

### Active

Die Identität darf entsprechend ihrer Policies verwendet werden.

### Suspended

Die Identität ist vorübergehend deaktiviert und kann kontrolliert reaktiviert werden.

### Disabled

Normale Nutzung ist gesperrt.

### Revoked

Das Vertrauen in die Identität wurde explizit widerrufen.

### Retired

Die Identität wird nicht mehr aktiv verwendet, bleibt aber für historische Referenzen erhalten.

## Erstellung

Identitäten müssen über einen kontrollierten Vorgang erstellt werden.

```text
Identity Request
      ↓
Validation
      ↓
IdentityID Assignment
      ↓
Credential Provisioning
      ↓
Activation
```

Die Erstellung einer Identität darf nicht automatisch weitreichende Autorität erzeugen.

## Änderung

Veränderliche Eigenschaften dürfen geändert werden, ohne die stabile Identität zu ersetzen.

```text
Display Name
Attributes
Credentials
Provider Metadata
Security Context
```

```text
Mutable Attributes Change
          ≠
IdentityID Change
```

Sicherheitsrelevante Änderungen müssen autorisiert und nachvollziehbar sein.

## Credential-Lifecycle

Credentials besitzen einen eigenen Lebenszyklus.

```text
Identity
├── Credential A → Revoked
├── Credential B → Active
└── Credential C → Expired
```

Das Entfernen eines einzelnen Credentials darf die Identität nicht automatisch löschen.

## Suspension und Revocation

Eine Statusänderung muss abhängige Autorität berücksichtigen.

```text
Identity Suspended / Revoked
          ↓
Sessions
Credentials
Delegations
Capabilities
Remote Authorities
          ↓
Revoke / Revalidate / Restrict
```

Welche Auswirkungen eintreten, wird durch Lifecycle- und Security-Policies bestimmt.

## Workload- und Service-Identitäten

Kurzlebige Identitäten müssen automatische Ablaufregeln unterstützen.

```text
Create
 ↓
Active
 ↓
Expiration
 ↓
Invalid
```

Service-Identitäten können dagegen langfristig bestehen, während einzelne Workload- oder Service-Instanzen ersetzt werden.

## Federation

Bei föderierten Identitäten müssen externe und lokale Zustände getrennt behandelt werden.

```text
External Identity State
          +
Local Nova State
          ↓
Effective Identity State
```

Eine externe Reaktivierung darf eine lokale Sperre nicht automatisch aufheben.

## Retirement

Nicht mehr verwendete Identitäten sollen kontrolliert stillgelegt werden.

Historische Referenzen können erhalten bleiben für:

```text
Audit
Provenance
Ownership History
Transactions
Security Investigation
```

Retirement bedeutet daher nicht zwingend physische Löschung sämtlicher Referenzen.

## Privacy und Retention

Identity-Lifecycle-Daten müssen Privacy- und Retention-Regeln berücksichtigen.

```text
Retired Identity
      ↓
Retention Policy
      ↓
Retain / Minimize / Anonymize / Delete
```

Personenbezogene Daten dürfen nicht allein aufgrund historischer Referenzen unbegrenzt gespeichert werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Identity Type
State
Provider
Created
Expiration
Lifecycle Policy
State Version
```

Sensible Attribute und Credentials bleiben geschützt.

## Normative Anforderungen

1. NovaOS MUSS einen einheitlichen Identity Lifecycle bereitstellen.
2. Identitäten MÜSSEN einen expliziten Lifecycle-State besitzen.
3. `IdentityID` MUSS über normale Lifecycle-Änderungen stabil bleiben.
4. Identity-, Credential-, Session- und Capability-Lifecycle MÜSSEN getrennt behandelbar sein.
5. Suspension und Revocation MÜSSEN abhängige Autorität berücksichtigen.
6. Föderierte externe Zustände DÜRFEN lokale Sperren NICHT automatisch überschreiben.
7. Kurzlebige Identitäten MÜSSEN automatisch ablaufen können.
8. Retirement DARF erforderliche Audit- und Provenance-Referenzen NICHT unkontrolliert zerstören.
9. Lifecycle-Daten MÜSSEN Privacy- und Retention-Policies berücksichtigen.
10. Lifecycle-Übergänge SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-0001`
- `NPSPEC-IDENTITY-RECOVERY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-PRIVACY-EXPIRATION-0001`
- `ADR-IDENTITY-0013`

## Ergebnis

```text
Identity Creation
       ↓
Stable IdentityID
       ↓
Controlled Lifecycle
       ↓
Suspend / Revoke / Retire
       ↓
Dependent Authority Handling
       ↓
Retention / Historical References
```

NovaOS erhält damit einen einheitlichen Identity Lifecycle, der Identitäten dauerhaft und nachvollziehbar verwaltet, während Credentials, Sessions, Capabilities und historische Referenzen ihre jeweils eigenen kontrollierten Lebenszyklen behalten.