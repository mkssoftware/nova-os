# NPSPEC-SECURITY-RBAC-0001 – Nova Role-Based Access Control

## Status

Angenommen

## Kategorie

Security / Authorization / Role-Based Access Control

## Zweck

NovaOS definiert Role-Based Access Control (RBAC) als optionale Policy-Schicht zur Verwaltung von Berechtigungen über Rollen.

```text
Subject → Role → Permissions → Resource
```

RBAC ergänzt Capability Security, DAC und MAC und ersetzt keinen dieser Mechanismen.

## Grundprinzipien

```text
Role ≠ Identity
Role ≠ Capability
Role Membership ≠ Unlimited Authority
RBAC Allow ≠ Final Authorization
```

Rollen bündeln Berechtigungen für wiederkehrende Aufgaben und vereinfachen insbesondere Mehrbenutzer-, Organisations- und Administrationsszenarien.

## Rollenmodell

Eine Rolle besitzt mindestens:

```text
Role
├── RoleID
├── Name
├── Permissions
└── State
```

Optional:

```text
Parent Roles
Constraints
Security Domain
Expiration
Delegation Policy
```

`RoleID` ist die stabile Identität der Rolle und unabhängig von ihrem Namen.

## Rollenzuweisung

Subjects dürfen einer oder mehreren Rollen zugeordnet werden:

```text
User ─────┐
Service ──┼→ Roles
Process ──┘
```

Rollenzuweisungen müssen explizit autorisiert sein.

Ein Subject darf sich nicht selbst einer privilegierteren Rolle zuweisen.

## Berechtigungen

Rollen dürfen ressourcen- oder operationsbezogene Rechte enthalten:

```text
Operator
├── Service.Start
├── Service.Stop
└── Service.Inspect
```

Die resultierende Autorität darf die durch Capability, MAC oder andere verbindliche Sicherheitsregeln gesetzten Grenzen nicht überschreiten.

## Rollenhierarchie

Rollen dürfen andere Rollen kontrolliert erweitern:

```text
Administrator
    ↓
Operator
    ↓
Viewer
```

Vererbte Rechte müssen eindeutig bestimmbar sein.

Zyklische Rollenhierarchien müssen verhindert oder sicher behandelt werden.

## Least Privilege

Rollen sollen möglichst klein und auf konkrete Aufgaben zugeschnitten sein.

```text
BackupOperator
NetworkOperator
SecurityAuditor
DeviceManager
```

Eine universelle Administratorrolle mit unbegrenzter Systemautorität soll nicht erforderlich sein.

## Separation of Duties

NovaOS soll Rollen verwenden können, um kritische Aufgaben aufzuteilen.

Beispiel:

```text
PolicyAuthor
     ≠
PolicyApprover
```

Eine Policy darf verlangen, dass bestimmte Rechte nicht gleichzeitig demselben Subject zugewiesen werden.

## Capability-Integration

RBAC entscheidet Policy, Capabilities repräsentieren konkrete Autorität.

```text
Subject
   ↓
Role Evaluation
   ↓
Authorized Capability
   ↓
Resource
```

Eine Rolle darf nicht als frei kopierbare Capability behandelt werden.

## MAC und DAC

Die Autorisierungsentscheidung kann mehrere Ebenen kombinieren:

```text
Capability
    +
RBAC
    +
DAC
    +
MAC
    ↓
Authorization
```

Ein RBAC-Allow darf ein MAC-Deny oder eine fehlende notwendige Capability nicht umgehen.

## Änderungen und Widerruf

Änderungen an:

```text
Role Membership
Role Permissions
Role Hierarchy
Role State
```

müssen kontrolliert wirksam werden und relevante Autorisierungs-Caches invalidieren können.

## Introspection und Audit

NovaOS soll mindestens darstellen können:

```text
RoleID
SubjectID
Permissions
Role Membership
Inherited Roles
State
Decision Reason
```

Änderungen an privilegierten Rollen sollen auditierbar sein.

## Normative Anforderungen

1. NovaOS MUSS RBAC als optionale Policy-Schicht unterstützen können.
2. Rollen MÜSSEN stabile `RoleID`s besitzen.
3. Rollenzuweisungen MÜSSEN explizit autorisiert sein.
4. Subjects DÜRFEN ihre eigenen Rollen NICHT unautorisiert privilegieren.
5. Rollenhierarchien DÜRFEN keine unkontrollierte Rechteausweitung erzeugen.
6. RBAC DARF Capability Security oder MAC NICHT umgehen.
7. Least Privilege SOLL bei der Rollendefinition angewendet werden.
8. Separation of Duties SOLL unterstützt werden können.
9. Rollenänderungen MÜSSEN relevante Autorisierungszustände aktualisieren können.
10. Rollen und Zugriffsentscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SECURITY-0004`

## Ergebnis

```text
Subject
   ↓
Role
   ↓
Policy
   ↓
Capability + DAC + MAC
   ↓
Authorized Operation
```

NovaOS erhält damit eine kompakte rollenbasierte Policy-Schicht, mit der Berechtigungen für Benutzer, Dienste und Systemaufgaben verwaltet werden können, ohne das capability-basierte Sicherheitsmodell oder verbindliche MAC-Regeln zu ersetzen.