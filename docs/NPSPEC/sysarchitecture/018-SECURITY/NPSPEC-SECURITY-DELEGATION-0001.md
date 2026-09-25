# NPSPEC-SECURITY-DELEGATION-0001 – Nova Security Delegation

## Status

Angenommen

## Kategorie

Security / Authorization / Delegation

## Zweck

NovaOS definiert einen gemeinsamen Mechanismus zur kontrollierten Weitergabe vorhandener Autorität.

```text
Holder A
   ↓
Delegation
   ↓
Holder B
```

Delegation darf Autorität weitergeben oder einschränken, aber niemals neue Autorität erzeugen.

## Grundprinzipien

```text
Delegation ≠ New Authority
Delegated Rights ⊆ Original Rights
Possession ≠ Delegation Permission
Delegation ≠ Ownership Transfer
Delegation ≠ Permanent Authority
```

## Delegationsmodell

Eine Delegation besitzt mindestens:

```text
Delegation
├── DelegationID
├── ParentAuthority
├── Delegator
├── Delegate
├── Rights
└── State
```

Optional:

```text
Constraints
Expiration
Delegation Depth
Resource Budget
Purpose
Revocation Policy
```

## Attenuation

Delegierte Autorität darf gegenüber der ursprünglichen Autorität eingeschränkt werden.

```text
Original:
Read + Write + Execute

Delegated:
Read
```

Einschränkungen dürfen betreffen:

```text
Rights
Resources
Operations
Lifetime
Usage Count
Security Domain
Network Scope
```

Eine Delegation darf ihre Parent Authority niemals überschreiten.

## Delegationsrecht

Nicht jede Capability darf weiterdelegiert werden.

```text
Capability
├── Rights
└── Delegation Policy
```

Delegation muss durch die ursprüngliche Autorität oder eine übergeordnete Policy erlaubt sein.

## Delegationskette

Mehrstufige Delegation muss nachvollziehbar bleiben.

```text
A
↓
B
↓
C
```

Dabei gilt:

```text
Authority(C) ⊆ Authority(B) ⊆ Authority(A)
```

NovaOS muss Delegationstiefe begrenzen können.

## Lebensdauer

Delegierte Autorität darf zeitlich begrenzt werden.

```text
Created
  ↓
Active
  ↓
Expired / Revoked
```

Eine Delegation darf nicht länger gültig sein als ihre zugrunde liegende Autorität.

## Revocation

Delegationen müssen widerrufbar sein können.

```text
Parent Authority
      ↓
Revocation
      ↓
Dependent Delegations
```

Je nach Policy dürfen abhängige Delegationen:

```text
Cascade Revoke
Revalidate
Expire
```

## Prozess- und IPC-Integration

Capabilities dürfen kontrolliert zwischen Prozessen delegiert werden.

```text
Process A
   ↓
IPC Transfer
   ↓
Validation
   ↓
Process B
```

Das bloße Kopieren eines Handles darf keine Delegation erzeugen.

## Remote Delegation

Delegation über Systemgrenzen muss das Remote-Capability-Modell verwenden.

Dabei müssen Security-, Sovereignty- und Revocation-Anforderungen erhalten bleiben.

## Sicherheit

Delegationen müssen gegen:

```text
Forgery
Privilege Amplification
Unauthorized Redelegation
Stale Authority
Delegation Cycles
```

geschützt sein.

Eine Komponente darf ihre Autorität durch Delegationsketten nicht erhöhen.

## Introspection und Audit

NovaOS soll mindestens darstellen können:

```text
DelegationID
ParentAuthority
Delegator
Delegate
Rights
Constraints
Depth
State
Expiration
```

Sicherheitsrelevante Delegationen sollen auditierbar sein.

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Autoritätsdelegation unterstützen.
2. Delegierte Rechte DÜRFEN die ursprüngliche Autorität NICHT überschreiten.
3. Delegation MUSS explizit erlaubt sein.
4. Attenuation MUSS unterstützt werden.
5. Delegationsketten MÜSSEN begrenzbar sein.
6. Delegierte Autorität DARF NICHT länger gültig sein als ihre Parent Authority.
7. Delegationen MÜSSEN widerrufbar sein können.
8. Handle-Kopieren DARF NICHT automatisch Autorität delegieren.
9. Remote Delegation MUSS bestehende Sicherheitsgrenzen erhalten.
10. Delegationsbeziehungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `ADR-SECURITY-0016`

## Ergebnis

```text
Original Authority
        ↓
Controlled Delegation
        ↓
Attenuated Authority
        ↓
Delegate
```

NovaOS erhält damit ein einheitliches Delegationsmodell, bei dem Autorität gezielt weitergegeben und eingeschränkt werden kann, ohne durch Delegation neue oder stärkere Rechte zu erzeugen.