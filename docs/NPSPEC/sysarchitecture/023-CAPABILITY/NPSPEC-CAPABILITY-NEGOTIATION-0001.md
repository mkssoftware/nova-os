# NPSPEC-CAPABILITY-NEGOTIATION-0001 – Nova Capability Negotiation

## Status

Angenommen

## Kategorie

Capability / Negotiation / Security

## Zweck

NovaOS definiert Capability Negotiation als kontrollierten Mechanismus, mit dem zwei oder mehr Komponenten die für eine Operation benötigte Autorität, Rechte und Constraints aushandeln können.

```text
Requester Requirements
        +
Provider Requirements
        ↓
Capability Negotiation
        ↓
Compatible Authority
        ↓
Capability
```

Negotiation darf keine Autorität erzeugen, die keine beteiligte Policy oder Capability erlaubt.

## Grundprinzipien

```text
Negotiation ≠ Authorization
Negotiation ≠ Authority Creation
Request ≠ Permission
Provider Offer ≠ Capability Grant
Compatibility ≠ Trust
Downgrade ≠ Acceptable by Default
Negotiation Failure ≠ Security Bypass
```

## Negotiation-Modell

Eine Verhandlung besitzt mindestens:

```text
CapabilityNegotiation
├── NegotiationID
├── Requester Identity
├── Provider Identity
├── Required Operations
├── Offered Authority
└── State
```

Optional:

```text
Required Rights
Constraints
Trust Requirements
Security Domain
Purpose
Expiration
Resource Budget
Sovereignty
ExecutionContract
```

## Ablauf

```text
Capability Request
       ↓
Requirement Validation
       ↓
Provider Offer
       ↓
Policy Intersection
       ↓
Agreement
       ↓
Capability Creation
```

Die resultierende Autorität entspricht ausschließlich der erlaubten Schnittmenge.

```text
Result =
Requested Authority
∩ Provider Authority
∩ Security Policy
∩ Trust Policy
∩ Constraints
```

## Rechteaushandlung

Beispiel:

```text
Requester:
Read + Write + Delete

Provider:
Read + Write

Policy:
Read only

Result:
Read
```

Eine nicht verfügbare Berechtigung darf nicht still ersetzt oder erweitert werden.

## Constraints

Neben Rechten können Constraints ausgehandelt werden:

```text
Expiration
Purpose
Resource Scope
Location
Security Domain
Delegation
Data Sovereignty
Trust Level
Resource Budget
```

Das Ergebnis muss alle verpflichtenden Constraints erfüllen.

## Hard und Soft Requirements

Negotiation muss zwischen verpflichtenden und bevorzugten Anforderungen unterscheiden können.

```text
Hard Requirement
→ MUSS erfüllt sein

Soft Preference
→ DARF angepasst werden
```

Kann ein Hard Requirement nicht erfüllt werden:

```text
Negotiation Failed
```

Es darf kein stiller Sicherheits-Downgrade erfolgen.

## Trust

Requester und Provider können Trust-Anforderungen definieren.

```text
Requester
   ↓
Requires Provider Trust ≥ Policy Requirement

Provider
   ↓
Requires Requester Trust ≥ Policy Requirement
```

Trust-Prüfung bleibt von der eigentlichen Capability-Autorität getrennt.

## ExecutionContract

Capability Negotiation soll direkt mit `Nova.ExecutionContract` integrierbar sein.

```text
ExecutionContract
├── Operation
├── Required Capabilities
├── Hard Requirements
├── Soft Preferences
├── Trust
├── Sovereignty
└── Resource Budget
```

Die Negotiation bestimmt daraus eine zulässige Capability-Konfiguration.

## Remote Negotiation

Remote Capability Negotiation muss über authentisierte und geschützte Kommunikation erfolgen.

```text
Local Requester
      ↓
Secure Channel
      ↓
Remote Provider
      ↓
Negotiation
      ↓
Remote Capability
```

Remote Negotiation darf lokale Security-, Trust- oder Sovereignty-Regeln nicht umgehen.

## Versionen und Interfaces

Negotiation kann auch kompatible Capability-Interfaces bestimmen.

```text
Requester: Interface v3
Provider:  v2, v3, v4

Result: v3
```

Ein Downgrade auf ältere oder schwächere Sicherheitssemantik muss explizit durch Policy erlaubt sein.

## Delegation

Eine ausgehandelte Capability darf nur delegierbar sein, wenn dies Bestandteil des Ergebnisses ist.

```text
Delegation Allowed: false
```

Negotiation darf nicht delegierbare Autorität nicht indirekt delegierbar machen.

## Lebenszyklus

```text
Requested
   ↓
Negotiating
   ↓
Agreed / Failed
   ↓
Capability Issued
   ↓
Expired / Revoked
```

Negotiation und resultierende Capability besitzen getrennte Lebenszyklen.

## Introspection

Autorisierte Komponenten sollen mindestens abfragen können:

```text
NegotiationID
Requester
Provider
Requested Rights
Offered Rights
Effective Rights
Constraints
Decision
Failure Reason
```

Capability Tokens dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capability Negotiation ohne Erzeugung zusätzlicher Autorität ermöglichen.
2. Das Ergebnis MUSS innerhalb der Autorität aller beteiligten Policies bleiben.
3. Hard Requirements DÜRFEN NICHT still abgeschwächt werden.
4. Soft Preferences DÜRFEN kontrolliert angepasst werden.
5. Trust- und Capability-Bewertung MÜSSEN getrennt bleiben.
6. Negotiation MUSS Constraints und Zweckbindung berücksichtigen können.
7. Remote Negotiation MUSS lokale Security-, Trust- und Sovereignty-Policies einhalten.
8. Interface- oder Security-Downgrades MÜSSEN explizit erlaubt sein.
9. Delegierbarkeit MUSS Bestandteil des ausgehandelten Ergebnisses sein.
10. Negotiation-Entscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-DISTCOMM-NEGOTIATION-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `ADR-CAPABILITY-0006`

## Ergebnis

```text
Requirements
     +
Available Authority
     +
Policies
     ↓
Capability Negotiation
     ↓
Safe Intersection
     ↓
Purpose-bound Capability
```

NovaOS erhält damit eine Capability-Negotiation-Schicht, die Rechte, Constraints, Trust-Anforderungen und Ausführungsbedingungen sicher aushandelt, ohne fehlende Autorität durch implizite Rechte oder stille Sicherheits-Downgrades zu ersetzen.