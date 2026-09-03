# ADR-TRUST-0004 – Trust Anchors nach Zweck

## Status

Angenommen

## Kategorie

Trust Architecture / Trust Anchors / Trust Domains

## Kontext

Nova.Trust benötigt vertrauenswürdige Ausgangspunkte, anhand derer Signaturen, Attestations, Identities und Provenance bewertet werden können.

Ein einziger globaler Trust Store wäre dafür ungeeignet. Ein Trust Anchor, der beispielsweise für Betriebssystemkomponenten akzeptiert wird, muss nicht automatisch für Anwendungen, Geräte, Dokumente oder Remote Services vertrauenswürdig sein.

NovaOS benötigt deshalb zweckgebundene Trust Anchors.

## Entscheidung

Nova.Trust ordnet **Trust Anchors explizit einem oder mehreren definierten Zwecken und Trust Domains zu**.

```text
Trust Anchor
     ↓
Allowed Purpose / Trust Domain
     ↓
Trust Evaluation
```

Ein Trust Anchor gilt nur innerhalb des für ihn autorisierten Kontexts.

## Grundprinzip

```text
Trusted for X
≠
Trusted for everything
```

Trust Anchors erhalten keine universelle systemweite Vertrauensstellung.

## Zweckgebundene Trust Domains

NovaOS kann unterschiedliche Trust Domains definieren, beispielsweise:

```text
Boot Trust
System Software Trust
Application Trust
Driver Trust
Device Trust
Service Trust
Document Trust
Media Trust
AI Trust
```

Ein Trust Anchor kann für mehrere Domains zugelassen sein, wenn dies explizit definiert wurde.

## Trust-Anchor-Hierarchie

Trust Anchors können hierarchisch organisiert werden.

```text
Root Trust Anchor
      ↓
Delegated Trust Anchor
      ↓
Purpose-Limited Trust
```

Delegation darf den zulässigen Trust Scope nicht unkontrolliert erweitern.

## System-, Organisations- und Benutzer-Trust

NovaOS kann Trust Anchors aus unterschiedlichen administrativen Ebenen berücksichtigen:

```text
NovaOS System Trust
Organization Trust
Device / Local Trust
User Trust
```

Welche Ebene für einen bestimmten Zweck zulässig ist, wird durch Trust Policy bestimmt.

Ein vom Benutzer hinzugefügter Trust Anchor wird dadurch nicht automatisch zu einem System- oder Boot-Trust-Anchor.

## Revocation und Lifecycle

Trust Anchors müssen einen expliziten Lifecycle besitzen und widerrufbar sein.

Eine bekannte Revocation muss bei zukünftigen Trust-Entscheidungen berücksichtigt werden.

Bestehende Artefakte können anschließend entsprechend ihrer Policy neu bewertet werden.

## Trust und Authority

Auch ein erfolgreich bis zu einem gültigen Trust Anchor verifizierter Trust Path erzeugt keine Capability Authority.

```text
Valid Trust Path ≠ Authority
```

Authorization bleibt Aufgabe des Capability- und Policy-Systems.

## Normative Anforderungen

1. Nova.Trust MUSS Trust Anchors zweckgebunden verwalten.
2. Trust Anchors DÜRFEN NICHT automatisch für alle Trust Domains gelten.
3. Trust Policies MÜSSEN den zulässigen Zweck eines Trust Anchors berücksichtigen.
4. NovaOS MUSS mehrere unabhängige Trust Domains unterstützen können.
5. Trust Anchors MÜSSEN hierarchisch delegierbar sein können.
6. Delegierte Trust Anchors DÜRFEN ihren autorisierten Trust Scope nicht eigenständig erweitern.
7. System-, Organisations-, lokale und Benutzer-Trust-Anchors MÜSSEN unterscheidbar sein.
8. Benutzer-Trust DARF nicht automatisch System- oder Boot-Trust erzeugen.
9. Trust Anchors MÜSSEN widerrufbar sein.
10. Revocation MUSS bei zukünftigen Trust-Entscheidungen berücksichtigt werden.
11. Ein gültiger Trust Path DARF keine automatische Capability Authority erzeugen.
12. Trust Anchor, Trust Domain und Bewertungsweg MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- kein universeller globaler Vertrauensanker,
- kompromittierter Trust kann auf bestimmte Zwecke begrenzt bleiben,
- unterschiedliche Sicherheitsanforderungen für Boot, Software, Geräte, Medien und KI,
- Organisationen und Benutzer können eigene Trust Domains ergänzen,
- Trust Delegation bleibt kontrollierbar.

### Negative Konsequenzen

- Trust-Anchor-Verwaltung wird komplexer,
- Trust Policies müssen den jeweiligen Zweck explizit berücksichtigen.

## Verworfene Alternativen

### Ein globaler Trust Store für alle Zwecke

Verworfen, da ein Trust Anchor dadurch weitreichendere Vertrauenswirkung erhalten könnte als vorgesehen.

### Jeder gültige Root Anchor ist systemweit vertrauenswürdig

Verworfen, da kryptographische Vertrauenskette und zulässiger Verwendungszweck getrennt bewertet werden müssen.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0002_Identity_Signature_Provenance_Permission_Capability_Audit`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-SECURITY-0014_Secure_und_Measured_Boot_komplementär`
- `ADR-CRYPTO-0001_Crypto_Agility`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-ANCHOR-0001`
- `NPSPEC-TRUST-DOMAIN-0001`
- `NPSPEC-TRUST-ANCHOR-PURPOSE-0001`
- `NPSPEC-TRUST-ANCHOR-DELEGATION-0001`
- `NPSPEC-TRUST-ANCHOR-REVOCATION-0001`
- `NPSPEC-TRUST-ANCHOR-TEST-0001`

## Ergebnis

Nova.Trust verwendet keine universellen Trust Anchors, sondern zweckgebundene Vertrauensanker:

```text
Trust Anchor
     ↓
Purpose / Trust Domain
     ↓
Trust Path
     ↓
Contextual Trust Decision
```

Damit bedeutet Vertrauen in einen Anchor für einen bestimmten Zweck niemals automatisch Vertrauen für alle anderen Bereiche von NovaOS.