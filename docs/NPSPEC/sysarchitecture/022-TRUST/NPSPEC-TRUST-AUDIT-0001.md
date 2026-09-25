# NPSPEC-TRUST-AUDIT-0001 – Nova Trust Audit

## Status

Angenommen

## Kategorie

Trust / Audit / Security

## Zweck

NovaOS definiert Trust Audit als nachvollziehbare Aufzeichnung sicherheitsrelevanter Trust-Entscheidungen, Evidence-Änderungen und Revocations.

```text
Trust Event
    ↓
Audit Record
    ↓
Protected Audit Trail
    ↓
Analysis / Verification
```

Trust Audit dokumentiert Entscheidungen, erzeugt jedoch selbst weder Vertrauen noch Autorität.

## Grundprinzipien

```text
Audit ≠ Trust
Audit ≠ Authorization
Logged ≠ Trusted
No Audit Entry ≠ Event Did Not Happen
Audit Access ≠ Audit Modification
Trust Decision Must Be Explainable
Secrets Must Not Enter Audit Logs
```

## Audit-Modell

Ein Trust-Audit-Eintrag besitzt mindestens:

```text
TrustAuditRecord
├── EventID
├── Timestamp
├── SubjectID
├── Trust Domain
├── Event Type
├── Trust State
└── Policy Reference
```

Optional:

```text
Previous Trust State
Evidence References
Evaluator Identity
Decision Reason
Trust Anchor
Attestation Reference
Provenance Reference
Revocation Reference
Security Domain
TransactionID
TraceID
```

## Audit-Ereignisse

Mindestens folgende Ereignisse müssen erfassbar sein:

```text
Trust Evaluation
Trust State Change
Trust Revocation
Trust Restoration
Policy Change
Trust Anchor Change
Attestation Result
Signature Trust Change
Provenance Trust Change
Software Trust Change
Supply Chain Trust Change
Agent Trust Change
```

## Entscheidungsnachweis

Trust-Entscheidungen sollen nachvollziehbar bleiben.

```text
Subject
   +
Evidence References
   +
Policy Version
   +
Context
   ↓
Trust Decision
   ↓
Audit Record
```

Dabei sollen bevorzugt Referenzen oder Hashes statt vollständiger sensibler Evidence gespeichert werden.

## Manipulationsschutz

Trust-Audit-Daten müssen gegen unautorisierte Veränderung geschützt werden können.

Geeignete Mechanismen umfassen:

```text
Hash Chains
Digital Signatures
Authenticated Storage
Trusted Timestamps
Append-only Records
Protected Audit Service
```

Ein kompromittierter Audit-Speicher darf nicht still vertrauenswürdige Historie vortäuschen können.

## Trust Revocation

Revocation muss nachvollziehbar mit betroffenen Trust-Entscheidungen verbunden werden können.

```text
Revocation Event
      ↓
Affected Trust Graph
      ↓
Re-Evaluations
      ↓
Audit Trail
```

Damit kann NovaOS später feststellen, warum ein Subject seinen bisherigen Trust State verloren hat.

## Policy-Versionierung

Jede relevante Trust-Entscheidung soll auf die verwendete Policy-Version verweisen.

```text
Decision
├── PolicyID
├── Policy Version
└── Evidence References
```

Spätere Policy-Änderungen dürfen historische Entscheidungen nicht rückwirkend verändern.

## Verteilte Systeme

Bei Remote- oder verteilten Trust-Entscheidungen müssen Audit-Einträge korrelierbar sein.

```text
TraceID
TransactionID
SubjectID
Remote Identity
```

Remote Audit Evidence darf nicht ungeprüft als vertrauenswürdig gelten.

## Privacy

Trust Audit kann sensible Informationen enthalten.

Daher gelten:

```text
Data Minimization
Privacy Labels
Retention
Selective Disclosure
Sovereignty
```

Credentials, private Schlüssel, Passwörter, Tokens und unnötige personenbezogene Daten dürfen nicht in Trust-Audit-Einträgen gespeichert werden.

## Retention

Audit-Daten müssen kontrollierte Aufbewahrungsregeln unterstützen.

```text
Active
  ↓
Retention
  ↓
Archive / Minimize / Anonymize / Delete
```

Gesetzliche oder sicherheitsrelevante Aufbewahrung darf nicht automatisch unbegrenzte Speicherung bedeuten.

## Introspection

Autorisierte Komponenten sollen mindestens abfragen können:

```text
SubjectID
Trust State Changes
Decision Time
Policy Version
Decision Reason
Revocation Events
Evidence References
```

Introspection darf keine geschützten Evidence-Daten offenlegen.

## Normative Anforderungen

1. NovaOS MUSS sicherheitsrelevante Trust-Entscheidungen auditierbar machen.
2. Audit MUSS von Trust und Autorität getrennt bleiben.
3. Trust-State-Änderungen und Revocations MÜSSEN nachvollziehbar sein.
4. Audit-Einträge SOLLEN Policy-Version und relevante Evidence-Referenzen enthalten.
5. Trust-Audit-Daten MÜSSEN gegen unautorisierte Manipulation geschützt werden.
6. Historische Trust-Entscheidungen DÜRFEN durch spätere Policy-Änderungen NICHT rückwirkend verändert werden.
7. Verteilte Trust-Ereignisse MÜSSEN korrelierbar sein können.
8. Geheimnisse DÜRFEN NICHT in Trust-Audit-Daten geschrieben werden.
9. Trust Audit MUSS Privacy-, Retention- und Sovereignty-Regeln berücksichtigen.
10. Audit-Ausfälle DÜRFEN NICHT automatisch Trust erzeugen.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-AGENT-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-TRUST-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-TRUST-0013`

## Ergebnis

```text
Trust Decision / Event
         ↓
Protected Audit Record
         ↓
Correlated Trust History
         ↓
Verification + Analysis
         ↓
Explainable Trust State
```

NovaOS erhält damit eine manipulationsgeschützte und datensparsame Trust-Audit-Schicht, über die Trust-Entscheidungen, Policy-Grundlagen, Zustandsänderungen und Revocations nachvollziehbar bleiben.