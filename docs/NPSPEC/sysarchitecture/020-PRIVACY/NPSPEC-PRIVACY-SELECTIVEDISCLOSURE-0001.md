# NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001 – Nova Selective Disclosure

## Status

Angenommen

## Kategorie

Privacy / Identity / Selective Disclosure

## Zweck

NovaOS definiert Selective Disclosure als Mechanismus, bei dem nur die für einen konkreten Zweck erforderlichen Informationen offengelegt werden.

```text
Available Attributes
        ↓
Requested Proof
        ↓
Privacy Policy
        ↓
Minimum Disclosure
```

## Grundprinzipien

```text
Identity ≠ All Attributes
Verification ≠ Full Disclosure
Possession ≠ Permission to Share
Proof ≠ Source Data
Requested ≠ Automatically Required
```

## Disclosure-Modell

Eine Offenlegung besitzt mindestens:

```text
DisclosureRequest
├── RequestID
├── Requested Claims
├── Purpose
├── Recipient
└── State
```

Optional:

```text
Privacy Policy
Expiration
Security Domain
Sovereignty Policy
Proof Requirements
User Approval
```

## Attributauswahl

NovaOS soll einzelne Attribute unabhängig offenlegen können.

```text
Identity
├── Name
├── Birth Date
├── Address
└── Age
```

Eine Anwendung, die lediglich das Alter prüfen muss, soll nicht automatisch das vollständige Geburtsdatum erhalten.

```text
Requirement:
Age ≥ 18

Disclosure:
Over18 = True
```

## Nachweise

Wo möglich sollen Aussagen nachweisbar sein, ohne zugrunde liegende Daten vollständig offenzulegen.

```text
Private Attribute
      ↓
Proof Generation
      ↓
Verified Claim
```

Beispiele:

```text
Age ≥ Required Age
Membership Valid
Credential Valid
Region = Allowed Region
```

Die Architektur muss zukünftige kryptografische Nachweisverfahren integrieren können.

## Benutzerkontrolle

Nicht zwingend systemseitig erforderliche Offenlegungen sollen dem Benutzer verständlich angezeigt werden.

```text
Requester
Purpose
Requested Claims
Recipient
Duration
```

Eine Zustimmung darf keine darüber hinausgehende Autorität erzeugen.

## Datenminimierung

Selective Disclosure muss mit Data Minimization zusammenarbeiten.

```text
Requested Information
        ↓
Purpose Evaluation
        ↓
Minimum Required Claims
        ↓
Disclosure
```

Wenn ein abgeleiteter Nachweis genügt, soll die Offenlegung des vollständigen Quelldatums vermieden werden.

## Privacy Labels

Privacy Labels können bestimmen:

```text
Disclosable
Restricted
Purpose Bound
Recipient Bound
Non-Exportable
```

Eine Offenlegung darf bestehende Information-Flow- oder DLP-Regeln nicht umgehen.

## Credentials

Digitale Credentials müssen selektive Offenlegung unterstützen können, sofern das verwendete Credential-Format dies ermöglicht.

```text
Credential
├── Claim A
├── Claim B
├── Claim C
└── Claim D

Disclosure → Claim B only
```

## Lebensdauer

Offengelegte Nachweise können zeitlich begrenzt werden.

```text
Proof
├── Created
├── Valid Until
└── Purpose
```

Eine neue Verwendung kann eine erneute Autorisierung oder einen neuen Nachweis verlangen.

## Verknüpfbarkeit

NovaOS soll unnötige Korrelation zwischen unterschiedlichen Offenlegungen vermeiden.

```text
Service A Disclosure
        ≠
Service B Disclosure
```

Globale Identifikatoren sollen nicht verwendet werden, wenn zweckgebundene oder pseudonyme Identitäten ausreichen.

## Sicherheit

Selective Disclosure ersetzt nicht:

```text
Authentication
Authorization
Capabilities
Information Flow
DLP
Data Sovereignty
```

Empfänger erhalten nur die ausdrücklich freigegebenen Claims oder Nachweise.

## Audit und Introspection

NovaOS soll mindestens darstellen können:

```text
RequestID
Requester
Recipient
Purpose
Disclosed Claims
Proof Type
Expiration
Decision
```

Nicht offengelegte Quelldaten dürfen dadurch nicht sichtbar werden.

## Normative Anforderungen

1. NovaOS MUSS selektive Offenlegung von Daten unterstützen können.
2. Anwendungen SOLLEN nur die für ihren Zweck erforderlichen Claims erhalten.
3. Abgeleitete Nachweise SOLLEN vollständigen Quelldaten vorgezogen werden, wenn sie ausreichen.
4. Offenlegung MUSS mit Data Minimization integrierbar sein.
5. Privacy Labels MÜSSEN Offenlegungsregeln definieren können.
6. Selective Disclosure DARF Information Flow oder DLP NICHT umgehen.
7. Offenlegungen MÜSSEN zweck- und empfängerspezifisch begrenzbar sein.
8. Nachweise SOLLEN zeitlich begrenzbar sein.
9. Unnötige Verknüpfbarkeit zwischen Diensten SOLL vermieden werden.
10. Offenlegungsentscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `NPSPEC-PRIVACY-DLP-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `ADR-PRIVACY-0007`

## Ergebnis

```text
Private Data
     ↓
Purpose + Request
     ↓
Minimum Necessary Claim
     ↓
Selective Proof / Disclosure
     ↓
Recipient
```

NovaOS erhält damit ein Privacy-Modell, bei dem Identität und Daten nicht vollständig offengelegt werden müssen, wenn ein begrenzter Claim oder kryptografischer Nachweis für den jeweiligen Zweck ausreicht.