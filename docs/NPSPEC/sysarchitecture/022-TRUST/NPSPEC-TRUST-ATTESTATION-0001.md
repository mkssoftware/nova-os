# NPSPEC-TRUST-ATTESTATION-0001 – Nova Attestation Trust

## Status

Angenommen

## Kategorie

Trust / Attestation / Security

## Zweck

NovaOS definiert Attestation als überprüfbaren Nachweis über Identität, Integrität oder Zustand einer Komponente.

```text
Subject
   ↓
Measured State
   ↓
Attestation Evidence
   ↓
Verification
   ↓
Trust Policy
   ↓
Trust Decision
```

Attestation liefert Evidence für eine Trust-Entscheidung, erzeugt aber weder selbst Vertrauen noch Autorität.

## Grundprinzipien

```text
Attestation ≠ Trust
Attestation ≠ Authorization
Measurement ≠ Permission
Valid Evidence ≠ Acceptable State
Trusted Hardware ≠ Trusted Workload
Remote Attestation ≠ Remote Authority
Stale Attestation ≠ Current State
```

## Attestation-Modell

Eine Attestation besitzt mindestens:

```text
Attestation
├── SubjectID
├── Evidence Type
├── Measurement
├── Issuer / Attester
├── Verification State
└── Freshness
```

Optional:

```text
Nonce
Timestamp
Platform Identity
Boot Measurements
Code Identity
Configuration State
Security Domain
Hardware Root
Signature
Policy Reference
```

## Attestation-Typen

NovaOS muss verschiedene Attestation-Quellen unterstützen können:

```text
Device Attestation
Platform Attestation
Boot Attestation
Application Attestation
Service Attestation
Workload Attestation
Remote Attestation
Hardware-backed Attestation
```

Das Modell bleibt erweiterbar für zukünftige Verfahren.

## Evidence

Attestation Evidence kann beispielsweise enthalten:

```text
Firmware Measurements
Bootloader Measurement
Kernel Measurement
Module Measurement
Application Measurement
Configuration State
Security State
Hardware Identity
```

Evidence beschreibt einen beobachteten oder gemessenen Zustand.

Die Trust Policy entscheidet, ob dieser Zustand akzeptabel ist.

## Verification

Die Verifikation erfolgt vor der Trust-Bewertung.

```text
Attestation Evidence
       ↓
Signature Verification
       ↓
Trust Anchor Validation
       ↓
Freshness Validation
       ↓
Evidence Validation
       ↓
Trust Policy
```

Ungültige Evidence darf nicht als vertrauenswürdig behandelt werden.

## Freshness und Replay-Schutz

Attestations müssen gegen Wiederverwendung alter Nachweise geschützt werden können.

Geeignete Mechanismen sind:

```text
Nonce
Challenge
Timestamp
Sequence
Short-lived Evidence
Session Binding
```

```text
Previously Valid Attestation
            ≠
Current Trusted State
```

## Hardware-backed Attestation

TPM oder vergleichbare sichere Hardware kann kryptografisch geschützte Evidence bereitstellen.

```text
Hardware Root
     ↓
Measured Boot
     ↓
Platform State
     ↓
Signed Attestation
```

Hardwarebasierte Attestation ist bevorzugte Evidence für besonders kritische Plattformzustände, erzeugt aber keine automatische Autorität.

## Remote Attestation

Entfernte Systeme können Attestation Evidence austauschen.

```text
Remote Subject
      ↓
Challenge
      ↓
Attestation Evidence
      ↓
Local Verification
      ↓
Local Trust Policy
```

Die empfangende NovaOS-Instanz trifft ihre Trust-Entscheidung selbst.

## Workload Attestation

Services und Workloads können ihren aktuellen Ausführungszustand nachweisen.

Berücksichtigt werden können:

```text
Application Identity
Code Measurement
Service Identity
Runtime Environment
Host Trust
Security Domain
Configuration
```

Eine Migration auf einen anderen Host muss eine erneute Attestation auslösen können.

## Trust Integration

Attestation ist eine Evidence-Quelle der Trust Architecture.

```text
Identity Trust
      +
Signature Trust
      +
Provenance Trust
      +
Attestation
      ↓
Trust Policy
      ↓
Effective Trust State
```

Attestation darf andere notwendige Trust-Anforderungen nicht ersetzen.

## Änderung des Zustands

Ändert sich ein attestierter Zustand, muss bestehender Trust neu bewertet werden können.

```text
Trusted State
     ↓
Code / Configuration Change
     ↓
New Measurement
     ↓
Re-Attestation
     ↓
Trust Re-Evaluation
```

## Privacy

Attestation darf nicht unnötig detaillierte Hardware-, Nutzer- oder Geräteinformationen offenlegen.

Wo möglich sollen verwendet werden:

```text
Minimal Evidence
Scoped Identifiers
Pseudonymous Attestation
Selective Disclosure
```

Attestation muss Privacy- und Sovereignty-Regeln berücksichtigen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
SubjectID
Evidence Type
Attester
Verification State
Freshness
Trust Anchor
Trust Domain
Result
```

Geheime Schlüssel und geschützte Attestation-Daten dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Attestation als Trust Evidence unterstützen.
2. Attestation MUSS von Trust und Autorität getrennt behandelt werden.
3. Evidence MUSS kryptografisch verifizierbar sein können.
4. Attestation MUSS Freshness- und Replay-Schutz unterstützen.
5. Hardware-backed Attestation SOLL für kritische Plattformzustände unterstützt werden.
6. Remote Attestation MUSS lokal gegen Nova Trust Policies ausgewertet werden.
7. Zustandsänderungen MÜSSEN eine erneute Attestation auslösen können.
8. Attestation DARF NICHT automatisch Capabilities oder Autorität erzeugen.
9. Attestation MUSS Privacy- und Sovereignty-Anforderungen berücksichtigen.
10. Attestation-Ergebnisse SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-0001`
- `ADR-TRUST-0007`

## Ergebnis

```text
Subject State
     ↓
Measurement
     ↓
Protected Attestation Evidence
     ↓
Verification + Freshness
     ↓
Trust Policy
     ↓
Contextual Trust State
```

NovaOS erhält damit eine einheitliche Attestation-Architektur, mit der lokale und entfernte Komponenten ihren aktuellen Zustand überprüfbar nachweisen können, während die eigentliche Vertrauens- und Autorisierungsentscheidung weiterhin unabhängig und policygesteuert erfolgt.