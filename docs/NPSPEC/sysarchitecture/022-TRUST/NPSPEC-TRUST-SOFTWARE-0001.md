# NPSPEC-TRUST-SOFTWARE-0001 – Nova Software Trust

## Status

Angenommen

## Kategorie

Trust / Software / Security

## Zweck

NovaOS definiert Software Trust als kontextabhängige Bewertung, ob Software für einen bestimmten Zweck und Sicherheitskontext ausreichend vertrauenswürdig ist.

```text
Software
   ↓
Identity + Signature + Provenance + Integrity
   ↓
Trust Evidence
   ↓
Software Trust Policy
   ↓
Trust State
```

Software Trust entscheidet nicht allein über die Ausführung. Capabilities, Sandbox-, Security- und Execution-Policies bleiben unabhängig wirksam.

## Grundprinzipien

```text
Installed ≠ Trusted
Signed ≠ Trusted
Trusted Publisher ≠ Trusted Software
Valid Signature ≠ Safe Software
Trusted Software ≠ Unlimited Authority
Previous Trust ≠ Permanent Trust
Unknown ≠ Trusted
```

## Software-Trust-Modell

Eine Bewertung besitzt mindestens:

```text
SoftwareTrust
├── ApplicationID / ComponentID
├── Software Identity
├── Trust Domain
├── Evidence
├── Trust State
└── State Version
```

Optional:

```text
Version
Publisher Identity
Signature Trust
Provenance Trust
Code Integrity
Build Attestation
Distribution Source
Security Domain
Execution Context
Expiration
```

## Trust Evidence

NovaOS kann mehrere Evidence-Quellen kombinieren:

```text
Application Identity
Publisher Identity
Code Signature
Code Integrity
Provenance
Build Attestation
Package Source
Version
Security History
Trust Anchor
```

Kein einzelner Nachweis erzeugt automatisch vollständigen Software Trust.

## Trust Domains

Software Trust muss zweckabhängig sein.

```text
Software A
├── User Application → Trusted
├── System Service → Restricted
└── Kernel Component → Untrusted
```

Eine Anwendung, die für normale Benutzeraufgaben akzeptiert wird, ist damit nicht automatisch für privilegierte Systemfunktionen vertrauenswürdig.

## Herkunft und Build

NovaOS soll die Herkunft einer Software berücksichtigen können.

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
```

Evidence kann dabei umfassen:

```text
Source Provenance
Reproducible Build
Builder Identity
Build Attestation
Publisher Signature
Distribution Provenance
```

## Installation

Installation und Trust-Bewertung bleiben getrennt.

```text
Software Available
      ↓
Verification
      ↓
Trust Evaluation
      ↓
Installation Policy
```

NovaOS darf Software speichern oder analysieren, ohne sie automatisch als vertrauenswürdig oder ausführbar einzustufen.

## Ausführung

Vor sicherheitsrelevanter Ausführung kann der Trust State erneut geprüft werden.

```text
Software Identity
      +
Current Integrity
      +
Trust State
      +
Execution Policy
      +
Capabilities
      ↓
Execution Decision
```

Software Trust darf Sandbox- oder Capability-Grenzen nicht umgehen.

## Updates

Updates müssen eine erneute Bewertung ermöglichen.

```text
Trusted Version
      ↓
Update
      ↓
Signature + Provenance + Integrity
      ↓
Re-Evaluation
```

Gleiche Application Identity oder gleicher Publisher bedeutet nicht automatisch, dass jede neue Version denselben Trust State erhält.

Ein Publisher-Wechsel muss besonders behandelt werden.

## Laufende Veränderungen

Ändert sich ausführbarer Code nach der Bewertung, kann der bisherige Trust ungültig werden.

```text
Trusted Code
    ↓
Modification
    ↓
Integrity Change
    ↓
Trust Re-Evaluation
```

Dies gilt insbesondere für Kernel-Komponenten, Services, Treiber und privilegierte Anwendungen.

## Legacy Software

Software ohne moderne Trust Evidence kann abhängig von Policy eingeschränkt ausgeführt werden.

```text
Missing Evidence
      ↓
Unknown / Restricted
      ↓
Sandbox + Reduced Capabilities
```

Fehlende Evidence darf nicht durch pauschales Vertrauen ersetzt werden.

## Remote Software

Remote ausgeführte oder bereitgestellte Software unterliegt ebenfalls lokaler Trust-Bewertung.

```text
Remote Provider
      +
Workload Identity
      +
Attestation
      +
Software Provenance
      ↓
Local Trust Policy
```

Ein vertrauenswürdiger Remote Provider ersetzt keine Softwareprüfung.

## Dynamische Neubewertung

Software Trust muss neu bewertet werden können bei:

```text
Code Modification
Signature Revocation
Publisher Revocation
Provenance Change
Attestation Failure
Policy Change
Security Event
Trust Anchor Change
```

Bestehende Ausführungen können daraufhin eingeschränkt, isoliert oder beendet werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Software Identity
Version
Publisher
Trust State
Trust Domain
Signature State
Integrity State
Provenance State
Decision Reason
```

Geschützte Evidence darf nicht unautorisiert offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Software Trust getrennt von Installation und Ausführungsautorität behandeln.
2. Software Trust MUSS kontext- und domainabhängig sein.
3. Signatur, Provenance, Integrität und Identity MÜSSEN als Evidence kombinierbar sein.
4. Eine gültige Signatur DARF NICHT automatisch Software Trust erzeugen.
5. Software-Updates MÜSSEN eine erneute Trust-Bewertung ermöglichen.
6. Codeänderungen MÜSSEN bestehenden Trust invalidieren können.
7. Fehlende Evidence DARF NICHT automatisch als vertrauenswürdig gelten.
8. Software Trust DARF Capability-, Sandbox- oder Security-Policies NICHT umgehen.
9. Remote Software MUSS lokal bewertbar bleiben.
10. Software-Trust-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `ADR-TRUST-0008`

## Ergebnis

```text
Software
   ↓
Identity + Integrity + Signature + Provenance
   ↓
Software Trust Evaluation
   ↓
Contextual Trust State
   ↓
Execution Policy + Capabilities
   ↓
Controlled Execution
```

NovaOS erhält damit eine systemweite Software-Trust-Schicht, die Software anhand überprüfbarer Evidence bewertet und Vertrauen konsequent von Installation, Ausführung und tatsächlicher Autorität trennt.