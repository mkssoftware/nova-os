# NPSPEC-TRUST-PROVENANCE-0001 – Nova Provenance Trust

## Status

Angenommen

## Kategorie

Trust / Provenance / Security

## Zweck

NovaOS definiert Provenance Trust als kontextabhängige Bewertung der Herkunft und Verarbeitungsgeschichte von Daten, Software und Systemobjekten.

```text
Object
  ↓
Provenance Chain
  ↓
Verification + Trust Evidence
  ↓
Provenance Trust
  ↓
Policy Decision
```

Provenance beschreibt, woher ein Objekt stammt und welche relevanten Transformationen stattgefunden haben. Sie erzeugt allein weder Vertrauen noch Autorität.

## Grundprinzipien

```text
Known Origin ≠ Trusted Origin
Provenance ≠ Trust
Trusted Source ≠ Trusted Result
Signed Provenance ≠ Correct Provenance
Transformation ≠ Loss of Provenance
Copy ≠ New Origin
Missing Provenance ≠ Trusted
```

## Provenance-Trust-Modell

Eine Bewertung besitzt mindestens:

```text
ProvenanceTrust
├── ObjectID
├── Provenance Reference
├── Trust Domain
├── Trust State
└── State Version
```

Optional:

```text
Origin Identity
Creator Identity
Transformation Chain
Signature Evidence
Timestamp
Provider
Security Domain
Verification State
Purpose
```

## Provenance-Kette

NovaOS muss Herkunft über mehrere Verarbeitungsschritte verfolgen können.

```text
Original Source
      ↓
Import
      ↓
Transformation
      ↓
Processing
      ↓
Current Object
```

Jeder relevante Schritt kann enthalten:

```text
Operation
Actor / Identity
Source Object
Result Object
Timestamp
Tool / Service
Signature
Security Context
```

## Trust-Bewertung

Provenance Trust wird aus der Herkunftskette und den verfügbaren Nachweisen bestimmt.

```text
Origin
   +
Transformation Chain
   +
Signatures
   +
Identity Trust
   +
Policy
   ↓
Provenance Trust State
```

Eine vertrauenswürdige Quelle garantiert nicht automatisch ein vertrauenswürdiges Endergebnis.

## Trust States

Mindestens folgende Zustände müssen unterstützt werden:

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
```

Fehlende oder nicht überprüfbare Provenance muss als `Unknown` behandelbar sein.

## Transformationen

Transformationen dürfen Provenance nicht still entfernen.

```text
Source Object
      ↓
Transformation
      ↓
Derived Object
      ↓
DerivedFrom → Source Object
```

Jede Transformation kann eine neue Trust-Bewertung erfordern.

## NovaFile-Integration

NovaFile-Provenance bildet eine zentrale Datenquelle für Provenance Trust.

```text
NovaFile
├── ObjectID
├── Provenance
├── CreatedBy
├── ImportedFrom
└── DerivedFrom
```

Provenance Trust bewertet diese Informationen, ohne sie mit der Provenance selbst gleichzusetzen.

## Software und Pakete

Provenance Trust muss auch auf Software anwendbar sein.

```text
Source
 ↓
Build
 ↓
Package
 ↓
Signature
 ↓
Distribution
 ↓
Installation
```

Dabei können beispielsweise berücksichtigt werden:

```text
Source Repository
Build Environment
Builder Identity
Build Attestation
Publisher
Package Signature
Distribution Source
```

Eine gültige Paketsignatur beweist nicht automatisch eine vertrauenswürdige Build-Herkunft.

## Reproduzierbarkeit

Reproduzierbare Builds können als zusätzliche Trust Evidence verwendet werden.

```text
Source + Build Definition
          ↓
Independent Build
          ↓
Matching Artifact
          ↓
Additional Evidence
```

Reproduzierbarkeit allein erzeugt jedoch keine Autorität.

## Unterbrochene Provenance

Ist ein Teil der Herkunftskette unbekannt oder ungültig:

```text
Trusted → Trusted → Unknown → Current Object
```

darf NovaOS die unbekannte Stelle nicht still als vertrauenswürdig behandeln.

Die Policy entscheidet, ob der resultierende Trust State beispielsweise `Restricted`, `Unknown` oder `Untrusted` wird.

## Manipulationsschutz

Provenance-Daten müssen gegen unautorisierte Veränderung geschützt werden können.

Geeignete Mechanismen umfassen:

```text
Signatures
Hashes
Immutable References
Content IDs
Audit Records
Trusted Timestamps
```

Provenance-Einträge dürfen nicht allein aufgrund ihrer Existenz als authentisch gelten.

## Privacy

Provenance kann personenbezogene oder sensible Informationen enthalten.

Daher müssen gelten:

```text
Data Minimization
Privacy Labels
Retention
Selective Disclosure
Sovereignty
```

Nicht jeder Empfänger eines Objekts benötigt Zugriff auf dessen vollständige Provenance.

## Introspection

NovaOS soll mindestens darstellen können:

```text
ObjectID
Origin
Provenance Chain
Verification State
Trust State
Trust Domain
Decision Reason
```

Geschützte Provenance-Daten dürfen nur bei entsprechender Autorität sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS Provenance Trust getrennt von Provenance-Daten behandeln.
2. Bekannte Herkunft DARF NICHT automatisch Vertrauen erzeugen.
3. Provenance Trust MUSS kontext- und zweckabhängig bewertbar sein.
4. Transformationen MÜSSEN Provenance-Beziehungen erhalten können.
5. Fehlende Provenance DARF NICHT automatisch als vertrauenswürdig gelten.
6. Signaturen und Identity Trust MÜSSEN als Trust Evidence verwendbar sein.
7. Provenance-Ketten MÜSSEN Manipulationsschutz unterstützen können.
8. NovaFile-Provenance MUSS in Provenance Trust integrierbar sein.
9. Provenance-Daten MÜSSEN Privacy- und Sovereignty-Policies berücksichtigen.
10. Provenance-Trust-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-IDENTITY-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROVENANCE-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `ADR-TRUST-0004`

## Ergebnis

```text
Object
  ↓
Origin + Transformation History
  ↓
Cryptographic + Identity Evidence
  ↓
Provenance Trust Evaluation
  ↓
Trust State
  ↓
Policy Decision
```

NovaOS erhält damit eine systemweite Provenance-Trust-Schicht, die Herkunft, Transformationen und kryptografische Nachweise nachvollziehbar bewertet, ohne bekannte Herkunft automatisch mit tatsächlichem Vertrauen gleichzusetzen.