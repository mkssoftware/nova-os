# NPSPEC-TRUST-INTROSPECTION-0001 – Nova Trust Introspection

## Status

Angenommen

## Kategorie

Trust / Introspection / Observability

## Zweck

NovaOS definiert eine einheitliche Introspection-Schnittstelle, über die aktuelle Trust-Zustände, Entscheidungsgrundlagen und Abhängigkeiten kontrolliert abgefragt werden können.

```text
Subject
   ↓
Trust Engine
   ↓
Trust State + Evidence References
   ↓
Introspection
   ↓
Authorized Consumer
```

Trust Introspection macht Vertrauensentscheidungen nachvollziehbar, ohne dadurch Trust, Autorität oder Zugriff zu erzeugen.

## Grundprinzipien

```text
Observable ≠ Mutable
Visible ≠ Authorized
Trust State ≠ Authority
Introspection ≠ Administration
Evidence Reference ≠ Evidence Access
Explainable ≠ Secret Disclosure
Current State ≠ Historical State
```

## Introspection-Modell

Eine Trust-Abfrage liefert mindestens:

```text
TrustIntrospection
├── SubjectID
├── Trust Domain
├── Trust State
├── Policy Reference
└── State Version
```

Optional:

```text
Decision Time
Expiration
Evaluator
Evidence References
Trust Anchor
Attestation State
Signature State
Provenance State
Revocation State
Decision Reason
Dependencies
```

## Abfrage

Trust-Zustände müssen über eine standardisierte Schnittstelle abfragbar sein.

```text
Query
├── Subject
├── Trust Domain
└── Context
     ↓
Trust Introspection
     ↓
Authorized View
```

Die zurückgegebenen Informationen werden entsprechend der Capability des Aufrufers gefiltert.

## Trust Graph

NovaOS soll Trust-Abhängigkeiten introspektierbar machen.

```text
Trust Anchor
    ↓
Publisher
    ↓
Software
    ↓
Service
    ↓
Agent
```

Damit kann festgestellt werden, welche Trust-Entscheidungen von bestimmten Anchors, Providern oder Evidence abhängen.

## Entscheidungsgrundlage

Eine Trust-Entscheidung soll erklärbar sein.

```text
Trust State: Restricted

Reason:
├── Identity verified
├── Signature valid
├── Provenance verified
└── Attestation expired
```

Die Erklärung soll technisch nachvollziehbar sein, ohne geheime Daten offenzulegen.

## Evidence

Introspection soll bevorzugt Evidence-Referenzen statt vollständiger Evidence liefern.

```text
EvidenceID
Evidence Type
Verification State
Timestamp
Issuer
```

Der Zugriff auf die eigentlichen Evidence-Daten benötigt separate Autorität.

## Historische Zustände

Aktueller Trust und historische Trust-Entscheidungen müssen unterscheidbar sein.

```text
Current Trust
      ↓
Trust Audit
      ↓
Historical Decisions
```

Historische Informationen werden über Trust Audit bereitgestellt und unterliegen dessen Retention-Regeln.

## Revocation

Trust Introspection muss Revocation sichtbar machen können.

```text
Subject
├── Previous State: Trusted
├── Current State: Revoked
└── Revocation Reference
```

Abhängige Trust-Neubewertungen sollen ebenfalls nachvollziehbar sein.

## Systemweite Übersicht

Autorisierte Systemkomponenten können aggregierte Informationen erhalten:

```text
Trusted Subjects
Restricted Subjects
Revoked Subjects
Expired Attestations
Broken Trust Chains
Unknown Trust States
```

Dies ermöglicht Diagnose, Security Monitoring und Self-Healing.

## Self-Healing

NovaOS darf Trust Introspection als Eingang für Self-Healing verwenden.

```text
Trust Introspection
       ↓
Anomaly Detection
       ↓
Policy Evaluation
       ↓
Repair / Isolation / Revalidation
```

Introspection selbst darf jedoch keine Änderungen durchführen.

## Privacy

Trust-Informationen können sensible Beziehungen offenlegen.

Daher gelten:

```text
Data Minimization
Capability Control
Privacy Labels
Selective Disclosure
Retention
Sovereignty
```

Insbesondere Identitäten, Geräteinformationen und Provenance dürfen nur soweit erforderlich sichtbar werden.

## Remote Introspection

Remote Trust Introspection muss explizit autorisiert sein.

```text
Remote Request
      ↓
Authentication
      ↓
Capability Check
      ↓
Filtered Trust View
```

Remote-Zugriff darf keine zusätzlichen Trust-Rechte erzeugen.

## Introspection API

Konzeptionell:

```text
trust.query(subject, domain)
trust.explain(decision)
trust.dependencies(subject)
trust.evidence(decision)
trust.revocation(subject)
```

Alle Operationen unterliegen Capability- und Privacy-Prüfungen.

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche Trust-Introspection-Schnittstelle bereitstellen.
2. Introspection MUSS von Trust-Veränderung und Administration getrennt sein.
3. Trust States MÜSSEN kontext- und domainbezogen abfragbar sein.
4. Trust-Entscheidungen SOLLEN erklärbare Entscheidungsgründe bereitstellen.
5. Evidence SOLL bevorzugt über geschützte Referenzen dargestellt werden.
6. Trust-Abhängigkeiten SOLLEN als Graph introspektierbar sein.
7. Revocation und ungültige Trust Chains MÜSSEN sichtbar gemacht werden können.
8. Introspection MUSS Capability-, Privacy- und Sovereignty-Regeln berücksichtigen.
9. Remote Introspection MUSS explizit autorisiert werden.
10. Introspection DARF NICHT automatisch Trust oder Autorität verändern.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-AGENT-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-TRUST-REVOCATION-0001`
- `NPSPEC-TRUST-AUDIT-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001`
- `ADR-TRUST-0014`

## Ergebnis

```text
Trust Engine
     ↓
Current Trust State
     +
Evidence References
     +
Decision Reason
     +
Dependency Graph
     ↓
Capability-Controlled Introspection
     ↓
Diagnosis / Monitoring / Self-Healing
```

NovaOS erhält damit eine einheitliche Trust-Introspection-Schicht, über die Vertrauenszustände und ihre Abhängigkeiten nachvollziehbar werden, ohne Beobachtbarkeit mit Änderungsrechten oder Autorität gleichzusetzen.