# NPSPEC-TRUST-ANCHOR-0001 – Nova Trust Anchor

## Status

Angenommen

## Kategorie

Trust / Trust Anchor / Security

## Zweck

NovaOS definiert Trust Anchors als explizit konfigurierte Ausgangspunkte für Vertrauensentscheidungen.

```text
Trust Anchor
     ↓
Verified Trust Chain
     ↓
Trust Evidence
     ↓
Trust Policy
     ↓
Trust Decision
```

Ein Trust Anchor bildet den Anfang einer Vertrauenskette, erzeugt jedoch keine universelle Autorität.

## Grundprinzipien

```text
Trust Anchor ≠ Universal Trust
Trust Anchor ≠ Authority
Installed Anchor ≠ Trusted for Everything
Root Certificate ≠ Global Permission
Hardware Root ≠ Automatically Trusted
Trust Anchor Compromise = Security-Critical Event
```

## Trust-Anchor-Modell

Ein Trust Anchor besitzt mindestens:

```text
TrustAnchor
├── AnchorID
├── Anchor Type
├── Trust Domain
├── Verification Material
└── State
```

Optional:

```text
Owner
Provider
Allowed Purposes
Security Domain
Validity
Algorithm Policy
Sovereignty Policy
Hardware Binding
Provenance
Policy Reference
```

## Anchor-Typen

NovaOS muss unterschiedliche Trust Anchors unterstützen können:

```text
Public Key
Certificate
Hardware Root
TPM-backed Root
Platform Root
Organization Root
Publisher Root
Federation Root
Local Administrative Root
```

Neue Anchor-Typen müssen erweiterbar sein.

## Trust Domains

Ein Trust Anchor muss auf definierte Trust Domains begrenzbar sein.

```text
Anchor A
├── Application Signing
└── Package Signing

Anchor B
└── Device Attestation
```

Ein für Anwendungssignaturen vertrauenswürdiger Anchor darf dadurch nicht automatisch Kernel-, Identity- oder Federation-Vertrauen erhalten.

## Trust Chains

Trust Anchors können Vertrauensketten begründen:

```text
Trust Anchor
     ↓
Intermediate
     ↓
Signer / Provider
     ↓
Evidence
```

Jede Stufe muss kryptografisch und policyseitig validiert werden.

```text
Valid Chain ≠ Automatically Trusted Result
```

## Speicherung

Trust Anchors müssen besonders geschützt gespeichert werden.

Bevorzugte Speicherorte können sein:

```text
TPM
Secure Hardware
Protected Nova Key Store
Verified System Storage
Firmware Trust Store
```

Unautorisierte Änderungen müssen verhindert oder erkannt werden.

## Provisionierung

Das Hinzufügen eines neuen Trust Anchors ist eine sicherheitskritische Operation.

```text
Anchor Candidate
      ↓
Verification
      ↓
Authorization
      ↓
Trust Domain Assignment
      ↓
Activation
```

Die bloße Installation eines Zertifikats oder Schlüssels darf keinen automatischen globalen Trust erzeugen.

## Änderung und Entfernung

Trust Anchors müssen kontrolliert:

```text
Add
Update
Restrict
Disable
Revoke
Remove
```

werden können.

Änderungen müssen bestehende Trust Chains und abhängige Entscheidungen neu bewerten können.

## Revocation

Ein kompromittierter Trust Anchor muss widerrufbar sein.

```text
Trusted Anchor
      ↓
Compromise
      ↓
Revoked
      ↓
Dependent Trust Re-Evaluation
```

Abhängige Signaturen, Provider, Credentials oder Sessions können dadurch ihren bisherigen Trust State verlieren.

## Rotation

Trust Anchors müssen kontrolliert rotierbar sein.

```text
Anchor v1
   ↓
Transition
   ↓
Anchor v2
```

Übergangsphasen dürfen mehrere Anchors unterstützen, wenn die Trust Policy dies ausdrücklich erlaubt.

Rotation darf keinen unkontrollierten Trust-Verlust oder Trust-Zuwachs erzeugen.

## Boot und Hardware Trust

Secure Boot, Measured Boot und TPM können Trust Anchors bereitstellen.

```text
Hardware / Firmware Anchor
          ↓
Boot Chain
          ↓
Kernel
          ↓
System Trust
```

Hardwarebasierter Trust bleibt trotzdem an definierte Policies und Trust Domains gebunden.

## Lokale Kontrolle

NovaOS muss eigene lokale Trust Anchors verwalten können.

Damit darf das System nicht grundsätzlich von externen oder globalen PKI-Anbietern abhängig sein.

```text
Local Trust Domain
       ↓
Local Trust Anchor
```

## Crypto Agility

Trust Anchors müssen Algorithmuswechsel unterstützen.

Wird ein Algorithmus als:

```text
Deprecated
Forbidden
```

eingestuft, müssen betroffene Anchors identifizierbar und migrierbar sein.

Post-Quantum- und hybride Trust Anchors müssen zukünftig integrierbar sein.

## Introspection

NovaOS soll mindestens darstellen können:

```text
AnchorID
Anchor Type
Trust Domain
Owner / Provider
State
Allowed Purposes
Validity
Algorithm
Hardware Binding
```

Private Schlüssel oder andere geheime Verifikationsmaterialien dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS explizite Trust Anchors unterstützen.
2. Trust Anchors MÜSSEN auf definierte Trust Domains begrenzbar sein.
3. Ein Trust Anchor DARF NICHT automatisch universelles Vertrauen oder Autorität erzeugen.
4. Trust Anchors MÜSSEN gegen unautorisierte Veränderung geschützt werden.
5. Provisionierung neuer Trust Anchors MUSS eine autorisierte Sicherheitsoperation sein.
6. Trust Anchors MÜSSEN widerrufbar und rotierbar sein.
7. Anchor-Revocation MUSS abhängige Trust-Entscheidungen neu bewerten können.
8. Lokale und hardwaregestützte Trust Anchors MÜSSEN unterstützt werden können.
9. Trust Anchors MÜSSEN Crypto Agility unterstützen.
10. Trust-Anchor-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-TRUST-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-PQ-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `NPSPEC-BOOT-TPM-0001`
- `ADR-TRUST-0006`

## Ergebnis

```text
Protected Trust Anchor
         ↓
Verified Trust Chain
         ↓
Context + Trust Policy
         ↓
Trust State
         ↓
Security Decision
```

NovaOS erhält damit eine kontrollierte Trust-Anchor-Architektur, in der Vertrauenswurzeln explizit, zweckgebunden, geschützt, widerrufbar und rotierbar sind, ohne einen universellen Root-of-Trust mit unbegrenzter Autorität zu schaffen.