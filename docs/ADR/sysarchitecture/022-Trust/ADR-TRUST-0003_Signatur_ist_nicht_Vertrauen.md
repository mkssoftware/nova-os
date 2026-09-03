# ADR-TRUST-0003 – Signatur ist nicht Vertrauen

## Status

Angenommen

## Kategorie

Trust Architecture / Signatures / Trust Evaluation

## Kontext

Digitale Signaturen sind ein wichtiger Bestandteil der NovaOS-Sicherheitsarchitektur. Sie können Integrität und die kryptographische Bindung an einen Signierer nachweisen.

Eine technisch gültige Signatur sagt jedoch nicht automatisch aus, dass:

- der Signierer vertrauenswürdig ist,
- der Inhalt sicher ist,
- die Herkunftskette vertrauenswürdig ist,
- die Nutzung zulässig ist,
- die Entität Authority für eine Operation besitzt.

NovaOS darf daher Signaturprüfung und Vertrauensbewertung nicht gleichsetzen.

## Entscheidung

Nova.Trust behandelt eine Signatur ausschließlich als **Trust Evidence**, nicht als endgültige Trust-Entscheidung.

```text
Signature Verification
        ↓
Verified Evidence
        ↓
Nova.Trust Evaluation
        ↓
Contextual Trust
```

Eine gültige Signatur kann die Vertrauensbewertung beeinflussen, erzeugt aber allein keinen vertrauenswürdigen Zustand.

## Grundprinzip

```text
Signed ≠ Trusted
```

Zusätzlich gilt:

```text
Valid Signature ≠ Safe
Valid Signature ≠ Permitted
Valid Signature ≠ Authorized
```

## Aussage einer Signatur

Eine erfolgreich verifizierte Signatur kann insbesondere bestätigen:

```text
bestimmte Daten wurden signiert
        +
Signatur gehört zu einem bestimmten Credential
        +
signierte Daten wurden seitdem nicht unbemerkt verändert
```

Ob der Signierer, das Credential oder der signierte Inhalt vertrauenswürdig ist, wird separat bewertet.

## Trust Evaluation

Nova.Trust kann neben der Signatur weitere Informationen berücksichtigen:

```text
Identity
Signature
Provenance
Revocation State
Trust Policy
Integrity State
Attestation
Context
```

Dadurch kann dieselbe gültige Signatur in unterschiedlichen Kontexten zu unterschiedlichen Trust-Ergebnissen führen.

## Signer Identity

Die kryptographische Verifikation eines Credentials und die Bewertung der dahinterstehenden Identity bleiben getrennt.

```text
Signature
   ↓
Credential
   ↓
Identity Binding
   ↓
Trust Evaluation
```

Ein kompromittiertes, widerrufenes oder nicht ausreichend vertrauenswürdiges Credential darf nicht allein wegen einer mathematisch gültigen Signatur als vertrauenswürdig gelten.

## Code und Inhalte

Das Prinzip gilt systemweit für:

- Anwendungen,
- Treiber,
- Systemkomponenten,
- Capability Provider,
- NovaFiles,
- Dokumente,
- Medien,
- KI-Artefakte,
- externe Inhalte.

Signierter Code ist daher nicht automatisch zur Ausführung autorisiert und signierte Medien sind nicht automatisch inhaltlich vertrauenswürdig.

## Authority

Signaturen erzeugen keine Capability Authority.

```text
Signature → Evidence
Capability → Authority
```

Eine signierte Anwendung benötigt weiterhin die erforderlichen Capabilities und muss alle relevanten Policies erfüllen.

## Normative Anforderungen

1. Nova.Trust MUSS Signaturen als Trust Evidence und nicht als endgültige Trust-Entscheidung behandeln.
2. Eine gültige Signatur DARF nicht automatisch Trust erzeugen.
3. Signature Verification und Trust Evaluation MÜSSEN getrennte Schritte sein.
4. Credential- und Identity-Bindings MÜSSEN bei relevanten Trust-Entscheidungen berücksichtigt werden können.
5. Revocation MUSS unabhängig von der mathematischen Gültigkeit einer Signatur berücksichtigt werden können.
6. Provenance MUSS zusätzlich zur Signatur bewertbar sein.
7. Trust MUSS kontextabhängig bestimmt werden können.
8. Signierter Code DARF nicht allein aufgrund seiner Signatur Ausführungs-Authority erhalten.
9. Signierte Daten DÜRFEN nicht automatisch als inhaltlich vertrauenswürdig gelten.
10. Eine Signatur DARF keine Capability Authority erzeugen.
11. Unbekannter Trust-Status DARF nicht als vertrauenswürdig interpretiert werden.
12. Relevante Signature- und Trust-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- kompromittierte oder ungeeignete Signierer können trotz gültiger Signatur abgelehnt werden,
- Code Signing wird sauber von Authorization getrennt,
- Provenance und Kontext können in Trust-Entscheidungen einfließen,
- dasselbe Modell gilt für Software, Daten und Medien.

### Negative Konsequenzen

- eine reine Signaturprüfung reicht für Trust-Entscheidungen nicht aus,
- zusätzliche Trust Evidence und Policies müssen ausgewertet werden.

## Verworfene Alternativen

### Gültige Signatur bedeutet vertrauenswürdig

Verworfen, da kryptographische Gültigkeit keine vollständige Vertrauensaussage darstellt.

### Signierte Software automatisch ausführen

Verworfen, da Signatur weder Permission noch Capability Authority ersetzt.

### Nur Signiererlisten verwenden

Verworfen, da Trust zusätzlich von Provenance, Revocation und Kontext abhängen kann.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0002_Identity_Signature_Provenance_Permission_Capability_Audit`
- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-SIGNER-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-REVOCATION-0001`
- `NPSPEC-TRUST-SIGNATURE-EVALUATION-0001`
- `NPSPEC-TRUST-SIGNATURE-TEST-0001`

## Ergebnis

NovaOS trennt kryptographische Signaturprüfung konsequent von Vertrauensbewertung:

```text
Valid Signature
      ↓
Trust Evidence
      ↓
Nova.Trust
      ↓
Contextual Trust Decision
```

Eine Signatur beweist damit bestimmte kryptographische Eigenschaften, aber niemals automatisch Vertrauen, Zulässigkeit oder Authority.