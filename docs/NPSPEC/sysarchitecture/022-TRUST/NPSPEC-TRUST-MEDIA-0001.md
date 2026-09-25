# NPSPEC-TRUST-MEDIA-0001 – Nova Media Trust

## Status

Angenommen

## Kategorie

Trust / Media / Content Authenticity

## Zweck

NovaOS definiert Media Trust als kontextabhängige Bewertung der Herkunft, Integrität und Authentizität digitaler Medien.

```text
Media Object
     ↓
Provenance + Signature + Metadata
     ↓
Verification
     ↓
Media Trust Policy
     ↓
Trust State
```

Media Trust gilt insbesondere für:

```text
Images
Video
Audio
Documents
Generated Content
Edited Content
Captured Media
```

Eine technisch unveränderte Datei bedeutet nicht automatisch, dass ihr dargestellter Inhalt wahr oder vertrauenswürdig ist.

## Grundprinzipien

```text
Authentic File ≠ True Content
Valid Signature ≠ True Statement
Known Source ≠ Trusted Content
Metadata ≠ Proof
AI Generated ≠ Automatically Untrusted
Edited ≠ Automatically Deceptive
Missing Provenance ≠ Manipulated
Unknown ≠ Trusted
```

## Media-Trust-Modell

Eine Bewertung besitzt mindestens:

```text
MediaTrust
├── ObjectID
├── Media Type
├── Provenance State
├── Integrity State
├── Trust State
└── Trust Domain
```

Optional:

```text
Creator Identity
Capture Device
Signature
Timestamp
Transformation History
Generation Method
Source
Content Credentials
Verification Provider
Policy Reference
```

## Provenance

NovaOS soll die Herkunft eines Medienobjekts nachvollziehen können.

```text
Capture / Generation
        ↓
Original Object
        ↓
Edit
        ↓
Transformation
        ↓
Publication
        ↓
Current Object
```

Relevante Transformationen sollen als Provenance erhalten bleiben.

## Capture Trust

Direkt erzeugte Medien können zusätzliche Evidence besitzen:

```text
Device Identity
Capture Time
Capture Signature
Hardware Attestation
Location Evidence
Application Identity
```

Diese Evidence bestätigt bestimmte technische Eigenschaften der Aufnahme, nicht automatisch die Wahrheit des dargestellten Ereignisses.

## Bearbeitete Medien

Bearbeitung darf nicht automatisch als Vertrauensverlust gelten.

```text
Original
   ↓
Crop
   ↓
Color Correction
   ↓
Export
```

NovaOS soll zwischen nachvollziehbaren Transformationen und nicht nachvollziehbaren Änderungen unterscheiden können.

## Generierte Medien

Generierte Inhalte können explizit gekennzeichnet werden.

```text
Generated Media
├── Generator Identity
├── Model / Engine
├── Generation Timestamp
├── Source References
└── Provenance
```

KI-generierte Inhalte dürfen nicht allein aufgrund ihrer Erzeugungsmethode als `Untrusted` klassifiziert werden.

Entscheidend sind Kontext, Provenance und Policy.

## Content Credentials

NovaOS soll standardisierte kryptografische Herkunfts- und Content-Credential-Systeme integrieren können.

```text
Media
  +
Content Credentials
  ↓
Signature Verification
  ↓
Provenance Verification
  ↓
Media Trust
```

Das Trust-Modell bleibt dabei unabhängig vom konkreten Credential-Format.

## Trust States

Mindestens folgende Zustände müssen unterstützt werden:

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
```

Zusätzlich können separate Eigenschaften dargestellt werden:

```text
Provenance Verified
Signature Verified
Modified
Generated
Incomplete Provenance
```

Diese Eigenschaften sind keine Trust States.

## Fehlende Provenance

Fehlende Herkunftsinformationen dürfen nicht automatisch als Manipulationsbeweis interpretiert werden.

```text
No Provenance
     ↓
Unknown Provenance
```

Die jeweilige Policy entscheidet über daraus folgende Einschränkungen.

## NovaFile-Integration

NovaFile kann Media-Provenance direkt mit dem Objekt verbinden.

```text
Media.nf
├── Payload
├── Metadata
├── Provenance
├── Relationships
└── Trust References
```

Projektionen des Payloads dürfen die zugrunde liegende Trust- und Provenance-Information nicht verändern.

## Manipulation

Widersprechen Payload, Signatur oder Provenance einander, muss dies erkannt werden können.

```text
Verified Media
     ↓
Unauthorized Modification
     ↓
Integrity Failure
     ↓
Trust Re-Evaluation
```

Die ursprüngliche Provenance darf dabei nicht still überschrieben werden.

## Privacy

Media-Provenance kann sensible Informationen enthalten:

```text
Identity
Device
Location
Timestamp
Editing History
```

NovaOS muss deshalb Data Minimization, Selective Disclosure, Privacy Labels und Sovereignty berücksichtigen.

## Introspection

NovaOS soll verständlich darstellen können:

```text
Origin
Creator
Generated / Captured
Modified
Provenance State
Signature State
Integrity State
Trust State
```

Dabei muss zwischen technisch verifizierten Fakten und daraus abgeleiteten Trust-Entscheidungen unterschieden werden.

## Normative Anforderungen

1. NovaOS MUSS Media Trust von der inhaltlichen Wahrheit eines Mediums trennen.
2. Media Trust MUSS Provenance, Integrität und Signaturen berücksichtigen können.
3. Bearbeitung DARF NICHT automatisch als Manipulation oder Vertrauensverlust gelten.
4. KI-generierte Medien DÜRFEN NICHT allein aufgrund ihrer Erzeugungsmethode als untrusted gelten.
5. Fehlende Provenance DARF NICHT automatisch als Manipulationsnachweis gelten.
6. Transformationen SOLLEN nachvollziehbare Provenance erhalten.
7. Content-Credential-Systeme MÜSSEN integrierbar sein.
8. Media Trust MUSS mit NovaFile-Provenance integrierbar sein.
9. Provenance-Daten MÜSSEN Privacy- und Sovereignty-Policies berücksichtigen.
10. Media-Trust-Entscheidungen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROVENANCE-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-0001`
- `ADR-TRUST-0009`

## Ergebnis

```text
Media Object
     ↓
Origin + Transformation History
     ↓
Signature + Integrity + Provenance
     ↓
Media Trust Evaluation
     ↓
Trust State + Verifiable Properties
```

NovaOS erhält damit eine Media-Trust-Schicht, die Herkunft und technische Authentizität digitaler Medien nachvollziehbar bewertet, ohne technische Echtheit mit inhaltlicher Wahrheit gleichzusetzen.