# NPSPEC-TRUST-IDENTITY-0001 – Nova Identity Trust

## Status

Angenommen

## Kategorie

Trust / Identity / Security

## Zweck

NovaOS definiert Identity Trust als kontextabhängige Bewertung, wie weit einer behaupteten oder authentifizierten Identität für einen bestimmten Zweck vertraut werden kann.

```text
Identity
   ↓
Authentication + Evidence
   ↓
Identity Trust Evaluation
   ↓
Trust State
   ↓
Security Policy
```

Eine erfolgreiche Authentifizierung bestätigt eine Identität, bedeutet jedoch nicht automatisch, dass diese Identität für jede Operation vertrauenswürdig ist.

## Grundprinzipien

```text
Identity ≠ Trust
Authentication ≠ Trust
Trust ≠ Authority
Known Identity ≠ Trusted Identity
Trusted Identity ≠ Unlimited Access
External Identity ≠ Untrusted by Definition
Trust Once ≠ Trust Forever
```

## Trust-Modell

Eine Identity-Trust-Bewertung besitzt mindestens:

```text
IdentityTrust
├── IdentityID
├── Trust Domain
├── Evidence
├── Trust State
└── State Version
```

Optional:

```text
Authentication Context
Credential Trust
Identity Provider
Federation Context
Security Domain
Purpose
Expiration
Evaluator
Attestation
```

## Trust Evidence

Zur Bewertung dürfen mehrere Nachweise kombiniert werden:

```text
Authentication Strength
Credential Validity
Credential Issuer
Identity Provider
MFA State
Device Trust
Attestation
Federation Trust
Recovery History
Revocation State
```

Kein einzelner Nachweis erzeugt automatisch vollständiges Vertrauen.

## Kontextabhängiger Trust

Identity Trust muss zweckabhängig sein können.

```text
Identity A
├── Normal Login → Trusted
├── Sensitive Data → Restricted
└── Key Administration → Insufficient Trust
```

Damit kann dieselbe Identität abhängig von Operation und Sicherheitsdomäne unterschiedlich bewertet werden.

## Lokale Identitäten

Lokale Identitäten dürfen anhand lokaler Evidence bewertet werden:

```text
Local Credential
MFA
Device Trust
Authentication Strength
Security State
```

Eine lokale Identität erhält nicht automatisch höheren Trust als eine föderierte Identität.

## Föderierte Identitäten

Bei föderierten Identitäten müssen mindestens berücksichtigt werden:

```text
Provider Trust
Assertion Validity
Authentication Strength
Accepted Claims
Federation Policy
Revocation State
```

```text
Trusted Provider ≠ Trusted Identity
```

NovaOS führt weiterhin eine eigene lokale Trust-Bewertung durch.

## Authentication Strength

Die Stärke der aktuellen Authentifizierung muss in Identity Trust einfließen können.

```text
Password
   ↓
Limited Trust

Passkey + MFA
   ↓
Higher Authentication Assurance
```

Eine höhere Authentication Strength darf jedoch nicht automatisch zusätzliche Capabilities erzeugen.

## Recovery

Nach Identity Recovery darf der Trust State temporär eingeschränkt werden.

```text
Recovery
   ↓
Identity Restored
   ↓
Restricted Trust
   ↓
Additional Verification
   ↓
Normal Trust
```

Dadurch können besonders sensitive Operationen nach einer Recovery zusätzliche Verifikation verlangen.

## Dynamische Neubewertung

Identity Trust muss neu bewertet werden können bei:

```text
Credential Revocation
Provider Trust Change
Device Compromise
Recovery
Authentication Expiration
Security Event
Policy Change
```

Bestehende Sessions können daraufhin eingeschränkt oder erneut authentifiziert werden.

## Step-Up

Reicht der aktuelle Identity Trust nicht aus, kann Step-Up Authentication angefordert werden.

```text
Current Trust
     ↓
Insufficient
     ↓
Step-Up Authentication
     ↓
Re-Evaluation
```

Step-Up erhöht den nachgewiesenen Authentication Context, nicht automatisch die Autorität.

## Autorisierung

Identity Trust bleibt eine Eingabe der Sicherheitsentscheidung.

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

Eine vertrauenswürdige Identität ohne erforderliche Capability erhält keinen Zugriff.

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Trust Domain
Trust State
Authentication Strength
Evidence
Evaluator
Expiration
State Version
```

Credentials, Schlüssel oder andere geheime Evidence-Daten dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Identity Trust getrennt von Identity und Authority behandeln.
2. Identity Trust MUSS kontext- und zweckabhängig bewertbar sein.
3. Authentication Strength MUSS als Trust Evidence verwendbar sein.
4. Lokale und föderierte Identitäten MÜSSEN nach gemeinsamen Trust-Grundsätzen bewertbar sein.
5. Externe Trust-Aussagen DÜRFEN NICHT ungeprüft übernommen werden.
6. Identity Trust MUSS nach sicherheitsrelevanten Ereignissen neu bewertet werden können.
7. Recovery MUSS eine erneute Trust-Bewertung auslösen können.
8. Unzureichender Trust SOLL Step-Up Authentication auslösen können.
9. Identity Trust DARF NICHT automatisch Capabilities oder Autorität erzeugen.
10. Identity-Trust-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-0001`
- `NPSPEC-IDENTITY-MFA-0001`
- `NPSPEC-IDENTITY-STEPUP-0001`
- `NPSPEC-IDENTITY-RECOVERY-0001`
- `NPSPEC-IDENTITY-LIFECYCLE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `ADR-TRUST-0002`

## Ergebnis

```text
Identity
   ↓
Authentication + Evidence
   ↓
Contextual Identity Trust
   ↓
Trust State
   ↓
Security Policy + Capabilities
   ↓
Authorized Operation
```

NovaOS erhält damit eine einheitliche Identity-Trust-Schicht, die Authentifizierungsstärke, Credentials, Provider, Federation und aktuelle Sicherheitszustände zu einer kontextabhängigen Trust-Bewertung zusammenführt, ohne Vertrauen mit tatsächlicher Autorität gleichzusetzen.