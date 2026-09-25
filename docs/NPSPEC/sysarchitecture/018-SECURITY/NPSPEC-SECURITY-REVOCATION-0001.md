# NPSPEC-SECURITY-REVOCATION-0001 – Nova Security Revocation

## Status

Angenommen

## Kategorie

Security / Authorization / Revocation

## Zweck

NovaOS definiert einen gemeinsamen Mechanismus zum kontrollierten Entzug zuvor gewährter Autorität.

```text
Granted Authority
       ↓
Revocation
       ↓
Authority Invalid
```

Revocation muss systemweit für sicherheitsrelevante Berechtigungen nutzbar sein.

## Grundprinzipien

```text
Granted ≠ Permanent
Revoked ≠ Expired
Revocation ≠ Deletion
Unknown Validity ≠ Valid
Delegated Authority ≠ Independent Authority
```

## Revocation-Modell

Ein widerrufbares Sicherheitsobjekt besitzt mindestens:

```text
RevocationState
├── TargetID
├── State
├── Generation
└── Revocation Authority
```

Mögliche Zustände:

```text
Valid
Revoked
Expired
Unknown
```

Optional:

```text
Reason
Timestamp
Parent Authority
Expiration
ReplacementID
```

## Widerrufbare Autorität

Revocation muss mindestens anwendbar sein auf:

```text
Capabilities
Delegated Capabilities
Sessions
Security Tokens
Role Assignments
Temporary Permissions
Remote Capabilities
Trust Relationships
```

## Capability Revocation

Capabilities müssen kontrolliert widerrufbar sein.

```text
Capability
    ↓
Revoke
    ↓
Future Operations Denied
```

Bereits gestartete Operationen müssen entsprechend ihrer eigenen Transaktions- und Cancellation-Semantik behandelt werden.

```text
Revocation ≠ Rollback
```

## Delegation

Widerruf einer übergeordneten Autorität muss abhängige Delegationen berücksichtigen können.

```text
Capability A
├── Delegation B
└── Delegation C
```

Eine Policy darf festlegen, ob abhängige Autorität:

```text
Cascade Revoke
Revalidate
Expire
```

muss.

## Generationen

NovaOS darf Generationen verwenden, um veraltete Handles oder Capabilities sicher zu erkennen.

```text
TargetID + Generation
```

Eine neue Ressource darf dadurch nicht versehentlich über eine alte Autorisierung erreichbar werden.

## Verteilte Revocation

Remote Capabilities und verteilte Sicherheitszustände müssen Widerruf propagieren können.

```text
Authority
   ↓
Revocation
   ↓
Remote Validation / Cache Invalidation
```

Bei erforderlicher aktueller Validierung gilt:

```text
Unknown ≠ Valid
```

Netzwerkausfall darf nicht automatisch dazu führen, dass eine nicht überprüfbare Autorität unbegrenzt gültig bleibt.

## Caching

Autorisierungs- und Revocation-Caches müssen invalidierbar sein.

```text
Revocation
   ↓
Cache Invalidation
   ↓
Revalidation
```

Cache-Lebensdauer muss zur Sicherheitsanforderung passen.

## Sicherheit

Nur explizit autorisierte Komponenten dürfen Sicherheitsobjekte widerrufen.

Ein Subject darf fremde Autorität nicht allein durch Kenntnis einer ID widerrufen.

Revocation selbst muss auditierbar sein.

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
Revocation Records
Dependency Chains
Cache Entries
Propagation State
Validation Operations
```

Alte Revocation-Daten dürfen nach sicherer Ablaufzeit kontrolliert entfernt werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
TargetID
State
Generation
Revocation Time
Reason
Revocation Authority
Dependent Authority
```

## Normative Anforderungen

1. NovaOS MUSS einen gemeinsamen Revocation-Mechanismus bereitstellen.
2. Capabilities und delegierte Autorität MÜSSEN widerrufbar sein können.
3. Revocation DARF NICHT automatisch als Rollback behandelt werden.
4. Abhängige Delegationen MÜSSEN kontrolliert invalidierbar sein.
5. Veraltete Autorität MUSS sicher erkennbar sein.
6. Remote Revocation MUSS propagierbar oder erneut validierbar sein.
7. `Unknown` DARF bei erforderlicher Validierung NICHT als `Valid` gelten.
8. Revocation MUSS relevante Sicherheits-Caches invalidieren können.
9. Widerruf selbst MUSS explizit autorisiert sein.
10. Revocation-Zustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-RBAC-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `ADR-SECURITY-0015`

## Ergebnis

```text
Authority
   ↓
Delegation / Use
   ↓
Revocation
   ↓
Invalidation
   ↓
Access Denied
```

NovaOS erhält damit einen einheitlichen Mechanismus, um lokale und verteilte Autorität kontrolliert, nachvollziehbar und sicher zu entziehen.