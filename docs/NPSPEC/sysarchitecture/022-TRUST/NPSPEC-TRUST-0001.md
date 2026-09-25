# NPSPEC-TRUST-0001 – Nova Trust Architecture

## Status

Angenommen

## Kategorie

Trust / Architecture / Core Security

## Zweck

NovaOS definiert ein einheitliches Trust-Modell zur Bewertung, ob Identitäten, Geräte, Anwendungen, Services, Workloads, Provider und andere Systemkomponenten für einen bestimmten Kontext ausreichend vertrauenswürdig sind.

```text
Entity
  ↓
Identity + Evidence
  ↓
Trust Evaluation
  ↓
Trust State
  ↓
Security Policy
```

Trust ist dabei eine Policy-Eingabe und erzeugt keine eigenständige Autorität.

## Grundprinzipien

```text
Trust ≠ Identity
Trust ≠ Authority
Authentication ≠ Trust
Signed ≠ Trusted
Known ≠ Trusted
Trusted Once ≠ Trusted Forever
Trust ≠ Global Property
Unknown ≠ Trusted
```

## Trust-Modell

Eine Trust-Bewertung besitzt mindestens:

```text
TrustContext
├── SubjectID
├── Trust Domain
├── Evidence
├── Trust State
└── State Version
```

Optional:

```text
Purpose
Evaluator
Security Domain
Attestation
Code Integrity
Provider
Expiration
Confidence
Sovereignty Policy
```

## Trust Domains

Trust muss kontextabhängig definiert werden können.

```text
Subject
├── Trusted for Local Execution
├── Trusted for Network Access
└── Not Trusted for Key Management
```

Eine Entität darf daher gleichzeitig unterschiedliche Trust States für verschiedene Zwecke besitzen.

## Trust States

NovaOS unterstützt mindestens:

```text
Trusted
Restricted
Untrusted
Unknown
Revoked
```

`Unknown` darf bei sicherheitskritischen Entscheidungen nicht automatisch als `Trusted` behandelt werden.

## Trust Evidence

Eine Trust-Bewertung kann unterschiedliche Nachweise berücksichtigen:

```text
Identity Verification
Code Signing
Code Integrity
Secure Boot
Measured Boot
Attestation
Device State
Provider Trust
Security Labels
Audit History
```

Ein einzelner Nachweis muss nicht automatisch ausreichendes Vertrauen erzeugen.

## Trust Evaluation

Trust wird durch Policies aus vorhandenen Nachweisen bestimmt.

```text
Identity
   +
Evidence
   +
Context
   +
Trust Policy
   ↓
Trust Decision
```

Die Entscheidung kann abhängig sein von:

```text
Operation
Resource
Security Domain
Location
Provider
Current System State
```

## Dynamischer Trust

Trust darf sich während des Betriebs ändern.

```text
Trusted
   ↓
Security Event
   ↓
Re-Evaluation
   ↓
Restricted / Untrusted
```

Auslöser können sein:

```text
Failed Attestation
Code Modification
Credential Revocation
Device Compromise
Provider Change
Security Policy Change
```

## Trust und Autorität

Trust allein darf keinen Zugriff erzeugen.

```text
Identity
   +
Trust State
   +
Security Policy
   +
Capabilities
   ↓
Authorized Operation
```

Auch eine vollständig vertrauenswürdige Komponente benötigt die erforderliche Capability.

## Trust Delegation

Vertrauen darf nicht automatisch transitiv sein.

```text
A trusts B
B trusts C

≠

A automatically trusts C
```

Trust-Beziehungen über mehrere Domains müssen explizit durch Policies erlaubt werden.

## Remote Trust

Remote-Komponenten müssen nach denselben Grundprinzipien bewertet werden.

```text
Remote Identity
      ↓
Authentication
      ↓
Attestation / Evidence
      ↓
Local Trust Evaluation
```

Behauptungen einer entfernten Gegenstelle dürfen nicht ungeprüft übernommen werden.

## Trust Cache

Trust-Entscheidungen dürfen zwischengespeichert werden, müssen jedoch:

```text
Time Bound
Context Bound
Revocable
Revalidatable
```

sein.

Ein abgelaufener oder nicht mehr verifizierbarer Trust State wird nicht automatisch verlängert.

## Revocation

Trust muss explizit widerrufbar sein.

```text
Trusted
  ↓
Revocation
  ↓
Revoked
```

Abhängige Sessions, Capabilities oder laufende Operationen können daraufhin neu bewertet oder eingeschränkt werden.

## Sovereignty

Trust und Data Sovereignty bleiben getrennte Policy-Dimensionen.

```text
Trusted Provider
       ≠
Allowed Data Location
```

Ein vertrauenswürdiger Provider darf Daten nicht entgegen geltender Sovereignty-Regeln verarbeiten.

## Introspection

NovaOS soll mindestens darstellen können:

```text
SubjectID
Trust Domain
Trust State
Evidence
Evaluator
Expiration
State Version
```

Sicherheitskritische Geheimnisse oder geschützte Evidence-Daten dürfen dadurch nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS ein systemweites Trust-Modell bereitstellen.
2. Trust MUSS von Identity, Authentication und Authority getrennt behandelt werden.
3. Trust MUSS kontext- und domainabhängig bewertbar sein.
4. `Unknown` DARF NICHT automatisch als `Trusted` behandelt werden.
5. Trust-Entscheidungen MÜSSEN auf überprüfbaren Evidence-Daten basieren können.
6. Trust MUSS während des Betriebs neu bewertet werden können.
7. Trust DARF NICHT automatisch transitiv sein.
8. Trust MUSS widerrufbar und zeitlich begrenzbar sein können.
9. Trust DARF bestehende Capability-, Security- oder Sovereignty-Regeln NICHT umgehen.
10. Trust-Zustände und ihre Entscheidungsgrundlage SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `ADR-TRUST-0001`

## Ergebnis

```text
Identity + Evidence + Context
            ↓
       Trust Policy
            ↓
       Trust State
            ↓
Security Policy + Capabilities
            ↓
    Authorized Operation
```

NovaOS erhält damit eine einheitliche Trust-Architektur, in der Vertrauen kontextabhängig, überprüfbar, widerrufbar und dynamisch neu bewertbar ist, ohne Trust mit Identität oder tatsächlicher Autorität gleichzusetzen.