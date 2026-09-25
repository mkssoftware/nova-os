# NPSPEC-TRUST-SUPPLYCHAIN-0001 – Nova Software Supply Chain Trust

## Status

Angenommen

## Kategorie

Trust / Software Supply Chain / Security

## Zweck

NovaOS definiert ein durchgängiges Trust-Modell für die Software-Lieferkette von Quellcode über Build und Paketierung bis zur Installation und Ausführung.

```text
Source
  ↓
Build
  ↓
Artifact
  ↓
Package
  ↓
Distribution
  ↓
Installation
  ↓
Execution
```

Jede Stufe erzeugt eigene Evidence und muss unabhängig überprüfbar bleiben.

## Grundprinzipien

```text
Trusted Source ≠ Trusted Binary
Trusted Build ≠ Trusted Package
Trusted Repository ≠ Trusted Package
Valid Signature ≠ Safe Software
Trusted Publisher ≠ Trusted Update
Known Provenance ≠ Automatic Trust
Supply Chain Trust ≠ Authority
```

## Supply-Chain-Modell

Eine Bewertung besitzt mindestens:

```text
SupplyChainTrust
├── Software Identity
├── Artifact Identity
├── Provenance Chain
├── Verification State
├── Trust State
└── Trust Domain
```

Optional:

```text
Source Identity
Repository
Commit Identity
Builder Identity
Build Environment
Build Attestation
Package Identity
Publisher Identity
Distribution Source
Signatures
Dependencies
SBOM
```

## Vertrauenskette

NovaOS muss relevante Stufen getrennt nachvollziehen können.

```text
Source Provenance
      ↓
Build Provenance
      ↓
Artifact Provenance
      ↓
Package Provenance
      ↓
Distribution Provenance
      ↓
Installed Software
```

Eine vertrauenswürdige vorherige Stufe darf die Prüfung nachfolgender Stufen nicht ersetzen.

## Source Trust

Quellcode kann bewertet werden anhand von:

```text
Repository Identity
Commit Signature
Author Identity
Review State
Source Provenance
Version
```

Der Quellcode selbst erhält dadurch keine automatische Ausführungsautorität.

## Build Trust

Build-Systeme sollen überprüfbare Evidence erzeugen können.

```text
Source
  +
Build Definition
  +
Builder Identity
  +
Build Environment
  ↓
Artifact + Build Attestation
```

Reproduzierbare Builds können zusätzliche Evidence liefern.

## Dependency Trust

Abhängigkeiten sind Teil der Supply Chain.

```text
Application
├── Library A
├── Library B
└── Component C
```

NovaOS muss Abhängigkeiten getrennt identifizieren und deren Trust State berücksichtigen können.

Eine vertrauenswürdige Hauptanwendung darf eine unbekannte oder widerrufene Abhängigkeit nicht automatisch vertrauenswürdig machen.

## SBOM

Software Bill of Materials muss integrierbar sein.

```text
Software
   ↓
SBOM
├── Components
├── Versions
├── Dependencies
└── Provenance References
```

Eine SBOM ist Evidence über Zusammensetzung, aber kein Vertrauensnachweis für sich allein.

## Paketierung und Distribution

NovaOS soll mindestens prüfen können:

```text
Package Identity
Package Integrity
Publisher Signature
Repository Identity
Distribution Source
Provenance Continuity
```

Ein kompromittiertes Repository darf gültige Software-Identitäten nicht still ersetzen können.

## Updates

Updates bilden eine neue Supply-Chain-Instanz.

```text
Current Version
      ↓
Update Candidate
      ↓
Supply Chain Verification
      ↓
Trust Evaluation
      ↓
Install / Reject
```

Publisher-, Signatur-, Provenance- oder Dependency-Wechsel müssen neu bewertet werden.

## Trust Anchors

Supply-Chain-Vertrauen kann auf mehreren Trust Anchors beruhen:

```text
Source Trust Anchor
Build Trust Anchor
Publisher Trust Anchor
Repository Trust Anchor
Platform Trust Anchor
```

Es darf keinen zwingenden universellen Supply-Chain-Root geben.

## Kompromittierung

Wird eine Stufe kompromittiert:

```text
Compromised Builder
        ↓
Affected Artifacts
        ↓
Trust Re-Evaluation
        ↓
Restrict / Revoke / Replace
```

NovaOS soll betroffene Software über Provenance und Dependency-Beziehungen identifizieren können.

## Execution Integration

Supply-Chain Trust ist eine Eingabe für Software Trust und Execution Policy.

```text
Supply Chain Trust
       +
Software Trust
       +
Code Integrity
       +
Capabilities
       ↓
Execution Decision
```

Supply-Chain Trust erzeugt keine zusätzlichen Capabilities.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Software Identity
Source
Builder
Publisher
Dependencies
SBOM State
Provenance State
Signature State
Supply Chain Trust State
```

## Normative Anforderungen

1. NovaOS MUSS Software Supply Chain Trust über mehrere Lieferstufen unterstützen.
2. Source, Build, Artifact, Package und Distribution MÜSSEN getrennt bewertbar sein.
3. Trust einer Stufe DARF NICHT automatisch Trust nachfolgender Stufen erzeugen.
4. Build- und Provenance-Evidence MÜSSEN integrierbar sein.
5. Abhängigkeiten MÜSSEN in die Trust-Bewertung einbezogen werden können.
6. SBOM-Daten MÜSSEN integrierbar sein.
7. Updates MÜSSEN eine erneute Supply-Chain-Bewertung ermöglichen.
8. Kompromittierte Komponenten, Builder oder Trust Anchors MÜSSEN abhängige Trust-Entscheidungen invalidieren können.
9. Supply-Chain Trust DARF NICHT automatisch Autorität erzeugen.
10. Supply-Chain-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `ADR-TRUST-0011`

## Ergebnis

```text
Source
  ↓
Verified Build
  ↓
Verified Artifact
  ↓
Verified Package
  ↓
Verified Distribution
  ↓
Supply Chain Trust
  ↓
Software Trust + Execution Policy
```

NovaOS erhält damit eine durchgängige Software-Supply-Chain-Trust-Architektur, die Herkunft, Build, Abhängigkeiten, Paketierung und Distribution überprüfbar verbindet, ohne Vertrauen einer einzelnen Stufe auf die gesamte Lieferkette zu übertragen.