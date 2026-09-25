# NPSPEC-TRUST-POLICY-0001 – Nova Trust Policy

## Status

Angenommen

## Kategorie

Trust / Policy / Security

## Zweck

NovaOS definiert Trust Policies als deklarative Regeln zur Bewertung von Vertrauen gegenüber Identitäten, Geräten, Anwendungen, Services, Workloads, Signaturen, Providern und Datenherkünften.

```text
Subject + Evidence + Context
            ↓
        Trust Policy
            ↓
      Trust Decision
```

Trust Policies entscheiden über den Vertrauenszustand, erzeugen jedoch keine eigenständige Autorität.

## Grundprinzipien

```text
Trust Policy ≠ Authorization Policy
Trust ≠ Authority
Evidence ≠ Decision
Valid Evidence ≠ Sufficient Evidence
Trusted in Context A ≠ Trusted in Context B
Unknown ≠ Trusted
Policy Failure ≠ Allow
```

## Policy-Modell

Eine Trust Policy besitzt mindestens:

```text
TrustPolicy
├── PolicyID
├── Subject Scope
├── Trust Domain
├── Requirements
└── State
```

Optional:

```text
Purpose
Required Evidence
Minimum Authentication Strength
Allowed Providers
Required Attestation
Signature Requirements
Provenance Requirements
Security Domain
Sovereignty Requirements
Expiration
Priority
```

## Policy-Auswertung

Die Trust Engine wertet verfügbare Evidence gegen die relevante Policy aus.

```text
Identity
Device State
Signature
Attestation
Provenance
Provider Trust
        ↓
Trust Policy
        ↓
Trusted / Restricted / Untrusted / Unknown
```

Eine fehlende Evidence darf nicht still als erfüllt behandelt werden.

## Kontextabhängigkeit

Policies müssen abhängig vom Operationskontext unterschiedliche Anforderungen definieren können.

```text
Normal Application
    → Standard Trust

Kernel Component
    → Strong Signature + Provenance

Key Management
    → Strong Identity + Device Trust + MFA
```

Trust ist damit keine globale Eigenschaft eines Subjects.

## Policy-Komposition

Mehrere Trust Policies müssen kombinierbar sein.

```text
Identity Trust
      +
Device Trust
      +
Signature Trust
      +
Provenance Trust
      ↓
Effective Trust Decision
```

Strengere Anforderungen dürfen durch weniger restriktive Policies nicht still aufgehoben werden.

## Priorität

Bei Konflikten gilt die NovaOS-Prioritätsordnung:

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

Performance oder Benutzerkomfort dürfen verpflichtende Trust-Anforderungen nicht umgehen.

## Dynamische Neubewertung

Trust Policies müssen während des Betriebs erneut ausgewertet werden können.

Auslöser können sein:

```text
Credential Revocation
Attestation Change
Code Modification
Provider Trust Change
Policy Update
Security Event
Provenance Change
Expiration
```

```text
Previous Trust
      ↓
Changed Evidence
      ↓
Policy Re-Evaluation
      ↓
New Trust State
```

## Policy-Versionierung

Trust Policies müssen versionierbar sein.

```text
PolicyID
├── Version 1
├── Version 2
└── Version 3
```

Sicherheitsrelevante Entscheidungen sollen nachvollziehbar der verwendeten Policy-Version zugeordnet werden können.

## Fehlerverhalten

Kann eine verpflichtende Trust Policy nicht ausgewertet werden, darf NovaOS nicht automatisch Vertrauen annehmen.

```text
Evaluation Failed
      ↓
Unknown / Restricted / Deny
```

Das konkrete Verhalten hängt von der Sicherheitsanforderung der Operation ab.

## Autorisierung

Nach der Trust-Bewertung erfolgt weiterhin eine separate Autorisierung.

```text
Trust Decision
      +
Identity
      +
Capabilities
      +
Security Policy
      ↓
Authorized Operation
```

Auch ein `Trusted` Subject benötigt die erforderliche Autorität.

## Introspection

NovaOS soll mindestens darstellen können:

```text
PolicyID
Policy Version
Subject
Trust Domain
Required Evidence
Available Evidence
Trust State
Decision Reason
```

Sensible Evidence darf dabei nicht unautorisiert offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS deklarative Trust Policies unterstützen.
2. Trust Policies MÜSSEN von Autorisierungspolicies getrennt sein.
3. Trust Policies MÜSSEN kontext- und domainabhängig definierbar sein.
4. Fehlende verpflichtende Evidence DARF NICHT automatisch als erfüllt gelten.
5. Mehrere Trust Policies MÜSSEN kontrolliert kombinierbar sein.
6. Strengere Trust-Anforderungen DÜRFEN NICHT still abgeschwächt werden.
7. Trust Policies MÜSSEN dynamisch neu ausgewertet werden können.
8. Trust Policies MÜSSEN versionierbar sein.
9. Fehler bei der Trust-Auswertung DÜRFEN NICHT automatisch Vertrauen erzeugen.
10. Trust-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-IDENTITY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-TRUST-0005`

## Ergebnis

```text
Subject
   +
Evidence
   +
Context
   ↓
Declarative Trust Policy
   ↓
Trust State
   ↓
Security Policy + Capabilities
   ↓
Authorized Operation
```

NovaOS erhält damit eine zentrale deklarative Trust-Policy-Schicht, über die Vertrauensanforderungen konsistent, kontextabhängig, versionierbar und dynamisch neu bewertbar definiert werden können.