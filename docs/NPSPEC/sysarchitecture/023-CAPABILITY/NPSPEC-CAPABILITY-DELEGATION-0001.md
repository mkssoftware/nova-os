# NPSPEC-CAPABILITY-DELEGATION-0001 – Nova Capability Delegation

## Status

Angenommen

## Kategorie

Capability / Delegation / Security

## Zweck

NovaOS definiert Capability Delegation als kontrollierte Übertragung oder Ableitung von Autorität zwischen Principals, Prozessen, Workloads, Services oder Agents.

```text
Principal A
    ↓
Capability A
    ↓ Delegation
Capability B
    ↓
Principal B
```

Delegation darf Autorität ausschließlich erhalten oder einschränken, niemals erweitern.

## Grundprinzipien

```text
Delegation ≠ Authority Creation
Delegation ≠ Identity Transfer
Delegation ≠ Trust
Delegation ≠ Ownership Transfer
Possession ≠ Permission to Delegate
Delegated Authority ≠ Original Authority
```

Zentrale Invariante:

```text
Authority(B) ⊆ Authority(A)
```

## Delegationsmodell

Eine Delegation beschreibt mindestens:

```text
CapabilityDelegation
├── DelegationID
├── Source CapabilityID
├── Derived CapabilityID
├── Delegator Identity
├── Recipient Identity
├── Rights
├── Constraints
└── State
```

Optional:

```text
Purpose
Expiration
Delegation Depth
Security Domain
Trust Requirements
Location Constraints
ExecutionContract
Revocation Policy
```

## Attenuation

Delegation soll bevorzugt eingeschränkte Capabilities erzeugen.

```text
Source
├── Read
├── Write
└── Delete

      ↓ Delegate

Derived
└── Read
```

Rechte, Scope oder Laufzeit dürfen reduziert, aber nicht erweitert werden.

## Delegationskette

Mehrstufige Delegation ist möglich:

```text
Capability A
    ↓
Capability B
    ↓
Capability C
```

Dabei gilt:

```text
Authority(C)
⊆ Authority(B)
⊆ Authority(A)
```

Jede Stufe erhält eine eigene Capability Identity.

## Delegierbarkeit

Nicht jede Capability muss delegierbar sein.

Mögliche Regeln:

```text
NonDelegable
Delegable
DelegableOnce
DelegableWithAttenuation
DelegableToSpecificIdentity
DelegableWithinSecurityDomain
```

Die Delegationsregel der Quelle darf durch eine abgeleitete Capability nicht gelockert werden.

## Delegation Depth

Capabilities können eine maximale Delegationstiefe besitzen.

```text
MaxDepth = 2

A → B → C
```

Eine weitere Delegation von C wäre dann unzulässig.

## Zweckbindung

Delegierte Capabilities können auf einen bestimmten Zweck beschränkt werden.

```text
Purpose:
"Render Document X"
```

Eine zweckgebundene Capability darf nicht für davon unabhängige Operationen verwendet werden.

## Temporäre Delegation

Delegation soll zeitlich begrenzbar sein.

```text
Issued
  ↓
Active
  ↓
Expired
```

Kurzlebige Delegationen sind insbesondere für Agents, Remote Services und temporäre Workflows zu bevorzugen.

## Prozess- und Workload-Delegation

Beim Start eines Workloads werden Capabilities explizit ausgewählt.

```text
Parent
   ↓
Spawn Contract
   ↓
Selected Capabilities
   ↓
Child
```

Es darf keine automatische vollständige Vererbung der Parent-Autorität geben.

## Agent Delegation

Agents handeln ausschließlich innerhalb explizit delegierter Autorität.

```text
User
 ↓
Agent
 ↓
Sub-Agent
```

Es gilt:

```text
Authority(Sub-Agent)
⊆ Authority(Agent)
⊆ DelegatedAuthority(User)
```

## Remote Delegation

Remote Delegation erfolgt über kontrollierten Export.

```text
Local Capability
      ↓
Attenuation
      ↓
Remote Capability Token
      ↓
Secure Channel
      ↓
Remote Principal
```

Remote Delegation darf lokale Trust-, Security- und Sovereignty-Policies nicht umgehen.

## Revocation

Delegationen müssen widerrufbar sein.

```text
Source Capability
       ↓
Delegation Tree
   ├── B
   │   └── D
   └── C
```

Je nach Policy kann Revocation:

```text
Single Capability
Delegation Branch
Entire Delegation Tree
```

betreffen.

Eine widerrufene Quelle darf nicht durch bereits delegierte Nachfolger umgangen werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
DelegationID
Source CapabilityID
Derived CapabilityID
Delegator
Recipient
Rights
Constraints
Purpose
Expiration
Delegation Depth
State
```

Dadurch können Delegationsketten als Autoritätsgraph nachvollzogen werden.

## Audit

Sicherheitsrelevante Delegationen sollen protokollierbar sein.

```text
Who
delegated which Capability
to whom
with which Rights
under which Constraints
```

Capability Tokens oder geheime Autoritätsdaten dürfen nicht im Audit erscheinen.

## Normative Anforderungen

1. Delegation DARF keine zusätzliche Autorität erzeugen.
2. Abgeleitete Capabilities MÜSSEN eigenständig identifizierbar sein.
3. Rechte und Constraints der Quelle DÜRFEN NICHT erweitert werden.
4. Nicht delegierbare Capabilities DÜRFEN NICHT indirekt delegiert werden.
5. Delegation MUSS auf Identität, Zweck, Zeit und Security Domain beschränkbar sein.
6. Mehrstufige Delegation MUSS attenuiert bleiben.
7. Delegation Depth MUSS begrenzbar sein.
8. Workloads und Agents DÜRFEN Autorität nur explizit erhalten.
9. Remote Delegation MUSS Security-, Trust- und Sovereignty-Regeln einhalten.
10. Delegationsketten MÜSSEN widerrufbar, introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-TRUST-AGENT-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `ADR-CAPABILITY-0007`

## Ergebnis

```text
Source Authority
      ↓
Delegation Policy
      ↓
Attenuation
      ↓
Derived Capability
      ↓
Restricted Recipient Authority
```

NovaOS erhält damit ein durchgängiges Delegationsmodell, in dem Autorität explizit, nachvollziehbar und eingeschränkt weitergegeben werden kann, ohne Ambient Authority oder Privilege Escalation zu erzeugen.