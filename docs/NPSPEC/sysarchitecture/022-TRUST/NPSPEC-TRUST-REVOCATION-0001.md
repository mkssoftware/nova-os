# NPSPEC-TRUST-REVOCATION-0001 – Nova Trust Revocation

## Status

Angenommen

## Kategorie

Trust / Revocation / Security

## Zweck

NovaOS definiert Trust Revocation als systemweiten Mechanismus zum Widerruf zuvor gewährten Vertrauens und zur Neubewertung davon abhängiger Trust-Entscheidungen.

```text
Trusted Subject
      ↓
Revocation Event
      ↓
Trust Invalidated
      ↓
Dependency Discovery
      ↓
Re-Evaluation
      ↓
Restrict / Revoke / Terminate
```

Trust Revocation ist von Capability-, Credential- und Identity-Revocation getrennt, kann diese jedoch auslösen oder durch sie ausgelöst werden.

## Grundprinzipien

```text
Trust ≠ Permanent
Revoked Trust ≠ Revoked Identity
Revoked Trust ≠ Deleted Identity
Revocation ≠ Deletion
Unknown Revocation State ≠ Valid Trust
Cached Trust ≠ Permanent Trust
Revocation Must Propagate
```

## Revocation-Modell

Eine Trust Revocation besitzt mindestens:

```text
TrustRevocation
├── RevocationID
├── SubjectID
├── Trust Domain
├── Reason
├── Effective Time
└── State
```

Optional:

```text
Issuer
Evidence
Affected Trust Anchors
Affected Credentials
Affected Software
Affected Provenance
Expiration
Security Domain
Policy Reference
```

## Revocation-Ziele

Widerruf muss mindestens möglich sein für:

```text
Identity Trust
Device Trust
Service Trust
Software Trust
Signature Trust
Provenance Trust
Agent Trust
Provider Trust
Trust Anchors
Attestation Trust
Supply Chain Trust
```

Revocation kann auf einzelne Trust Domains begrenzt werden.

## Revocation-Auslöser

Mögliche Ereignisse sind:

```text
Credential Compromise
Key Compromise
Invalid Attestation
Code Modification
Malicious Software
Trust Anchor Compromise
Provider Compromise
Supply Chain Compromise
Policy Violation
Administrative Decision
```

## Abhängigkeiten

Trust-Beziehungen bilden einen Abhängigkeitsgraphen.

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

Wird ein vorgelagerter Trust widerrufen, müssen abhängige Bewertungen identifiziert werden.

```text
Revocation
    ↓
Affected Trust Graph
    ↓
Re-Evaluation
```

Abhängiger Trust darf nicht automatisch bestehen bleiben.

## Laufende Operationen

Trust Revocation muss laufende Aktivitäten berücksichtigen.

```text
Trust Revoked
     ↓
Active Sessions
Running Workloads
Agent Operations
Remote Connections
Delegations
     ↓
Re-Evaluate
```

Je nach Policy können diese:

```text
Continue Restricted
Require Reauthentication
Quarantine
Suspend
Terminate
```

werden.

## Cached Trust

Zwischengespeicherte Trust-Entscheidungen müssen invalidierbar sein.

```text
Cached Trusted
     ↓
Revocation Event
     ↓
Cache Invalidated
```

Offline-Caches benötigen begrenzte Gültigkeitszeiten.

## Offline-Verhalten

Kann ein Revocation-Status nicht überprüft werden:

```text
Revocation State = Unknown
```

Für sicherheitskritische Operationen darf `Unknown` nicht automatisch als `Valid` interpretiert werden.

Policies können abhängig vom Risiko eingeschränkten Offline-Betrieb erlauben.

## Trust Anchor Revocation

Die Revocation eines Trust Anchors ist besonders kritisch.

```text
Trust Anchor Revoked
        ↓
Dependent Chains
        ↓
Signature / Identity / Software Trust
        ↓
Re-Evaluation
```

Historische Signaturen müssen dabei Zeitpunkt und gültige Timestamp-Evidence berücksichtigen können.

## Supply-Chain-Revocation

Wird ein Builder, Repository oder Dependency als kompromittiert erkannt, muss NovaOS betroffene Artefakte über Provenance-Beziehungen bestimmen können.

```text
Compromised Component
        ↓
Provenance Graph
        ↓
Affected Software
        ↓
Trust Re-Evaluation
```

## Wiederherstellung

Revocation darf nicht still rückgängig gemacht werden.

```text
Revoked
   ↓
New Evidence / Remediation
   ↓
Explicit Re-Evaluation
   ↓
New Trust Decision
```

Ein neuer Trust State ist eine neue Entscheidung und keine Aufhebung der historischen Revocation.

## Audit und Introspection

NovaOS soll mindestens darstellen können:

```text
RevocationID
SubjectID
Trust Domain
Reason
Issuer
Effective Time
Affected Dependencies
Current Trust State
```

Revocation-Ereignisse müssen nachvollziehbar bleiben, soweit Retention- und Privacy-Regeln dies erlauben.

## Normative Anforderungen

1. NovaOS MUSS Trust Revocation systemweit unterstützen.
2. Trust Revocation MUSS von Identity-, Credential- und Capability-Revocation getrennt behandelbar sein.
3. Revocation MUSS auf einzelne Trust Domains begrenzbar sein.
4. Abhängige Trust-Entscheidungen MÜSSEN nach Revocation neu bewertet werden können.
5. Cached Trust MUSS invalidierbar sein.
6. `Unknown` DARF bei sicherheitskritischen Revocation-Prüfungen NICHT automatisch als gültig gelten.
7. Laufende Sessions, Workloads und Operationen MÜSSEN nach relevanter Revocation neu bewertet werden können.
8. Trust-Anchor- und Supply-Chain-Revocation MÜSSEN abhängige Trust-Ketten erfassen können.
9. Wiederhergestellter Trust MUSS durch eine neue explizite Bewertung entstehen.
10. Trust-Revocation-Ereignisse SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-TRUST-AGENT-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `ADR-TRUST-0012`

## Ergebnis

```text
Trust Event
    ↓
Revocation
    ↓
Trust Graph Invalidation
    ↓
Dependent Re-Evaluation
    ↓
Restriction / Revocation / Termination
```

NovaOS erhält damit einen systemweiten Trust-Revocation-Mechanismus, der kompromittiertes oder nicht mehr gerechtfertigtes Vertrauen gezielt widerruft und abhängige Trust-Entscheidungen kontrolliert neu bewertet.