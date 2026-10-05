# NPSPEC-POLICY-CAPABILITY-0001 – Nova Capability Policy

## Status

Angenommen

## Kategorie

Policy / Capability

## Zweck

NovaOS definiert die Policy für Anforderung, Vergabe, Nutzung, Delegation und Einschränkung von Capabilities.

Die Policy entscheidet unter Berücksichtigung von Sicherheitskontext, Trust, Benutzerentscheidung und Systemzustand, ob eine angeforderte Authority zulässig ist.

## Grundprinzipien

```text
CapabilityID ≠ Authority
Discovery ≠ Permission
Request ≠ Grant
Trust ≠ Authority
Possession ≠ Delegation Right
User Approval ≠ Universal Override
Policy ≠ Capability Token
```

## Entscheidungsmodell

```text
Capability Request
       ↓
Requester Identity
       ↓
Target / Operation
       ↓
Security Context
       ↓
Trust State
       ↓
Capability Policy
       ↓
Allow / Restrict / Ask / Deny
       ↓
Capability Token / Handle
```

Nur eine positive Policy-Entscheidung darf zur Erzeugung oder Übertragung entsprechender Authority führen.

## Policy-Kontext

Eine Entscheidung kann berücksichtigen:

```text
Requester
CapabilityID
Target ObjectID
Operation
Scope
Trust State
Execution Context
User Decision
Delegation Chain
Lifetime
System State
Workspace
Solution
Program
```

## Policy-Ergebnisse

```text
Allow
AllowRestricted
AskUser
Deny
```

`AllowRestricted` kann eine abgeschwächte Capability erzeugen, beispielsweise mit:

```text
Reduced Operations
Restricted Target
Limited Scope
Expiration
Resource Limits
No Delegation
```

## Least Authority

NovaOS soll ausschließlich die minimal erforderliche Authority vergeben.

```text
Requested Authority
        ↓
Policy
        ↓
Minimum Required Authority
```

Eine Capability darf gegenüber der genehmigten Authority nicht erweitert werden.

## Delegation

Capability-Delegation muss policy-kontrolliert erfolgen.

```text
Original Capability
       ↓
Delegation Policy
       ↓
Attenuation
       ↓
Delegated Capability
```

Delegierte Authority darf niemals größer sein als die ursprüngliche Authority.

## Persistente Entscheidungen

Benutzerentscheidungen dürfen gespeichert werden, wenn die Identität und der Sicherheitskontext stabil validierbar sind.

Bei sicherheitsrelevanten Änderungen an Programmen, Solutions, Capability-Anforderungen oder Trust-Zuständen muss eine erneute Bewertung möglich sein.

## Revocation

Policy muss Capability-Revocation unterstützen.

Änderungen an Sicherheitslage, Trust, Benutzerentscheidung oder System Policy können bestehende Authority widerrufen oder einschränken.

## Policy-Priorität

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine Benutzerentscheidung darf höhere Sicherheitsgrenzen nicht automatisch überschreiben.

## Normative Anforderungen

1. Capability-Vergabe MUSS policy-basiert erfolgen.
2. Capability Discovery DARF keine Authority erzeugen.
3. Capability-Anforderung DARF nicht automatisch zur Vergabe führen.
4. NovaOS MUSS Least Authority unterstützen.
5. Policy MUSS Capabilities einschränken können.
6. Delegation DARF Authority ausschließlich erhalten oder reduzieren.
7. Persistente Entscheidungen MÜSSEN an validierbare Identitäten und Kontexte gebunden sein.
8. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Policy-Bewertung auslösen können.
9. Capability-Revocation MUSS unterstützt werden.
10. Trust DARF keine Authority automatisch erzeugen.
11. Höhere Safety- und Security-Regeln DÜRFEN nicht durch niedrigere Policy-Ebenen überschrieben werden.
12. Capability-Entscheidung, Einschränkungen und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-PROGRAM-PERMISSION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS steuert Capability-Authority über eine zentrale Policy-Schicht. Capabilities werden nur im minimal erforderlichen Umfang vergeben, können kontextabhängig eingeschränkt, delegiert oder widerrufen werden und bleiben strikt von Discovery, Trust und bloßen Benutzer- oder Programmidentitäten getrennt.