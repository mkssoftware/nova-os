# ADR-TRUST-0005 – Trust Graph statt globalem Trust Score

## Status

Angenommen

## Kategorie

Trust Architecture / Trust Graph / Trust Evaluation

## Kontext

Vertrauen ist in NovaOS kontextabhängig.

Eine Entität kann beispielsweise für einen bestimmten Zweck, gegenüber einer bestimmten Identity oder innerhalb einer bestimmten Trust Domain vertrauenswürdig sein, ohne allgemein vertrauenswürdig zu sein.

Ein einzelner globaler numerischer Trust Score würde diese Beziehungen zu stark vereinfachen und könnte unterschiedliche Trust-Eigenschaften miteinander vermischen.

## Entscheidung

Nova.Trust modelliert Vertrauen als **Trust Graph** und nicht als globalen Trust Score.

```text
Identity ─────── trusts-for ──────→ Service
   │                                  │
   ├── signed-by ──→ Trust Anchor     ├── provides ──→ Capability
   │                                  │
   └── owns ───────→ Device           └── runs-as ───→ Workload
```

Knoten repräsentieren relevante Entitäten.

Kanten repräsentieren überprüfbare Trust-, Identity-, Provenance- oder andere für Trust relevante Beziehungen.

## Grundprinzip

```text
Trust is relational and contextual,
not a universal number.
```

NovaOS fragt daher nicht primär:

```text
How trustworthy is X?
```

sondern:

```text
Is X sufficiently trusted
for purpose Y
in context Z
based on evidence E?
```

## Trust Graph

Der Trust Graph kann unterschiedliche Entitäten miteinander verbinden, beispielsweise:

```text
Identity
Credential
Trust Anchor
Application
Service
Device
Capability Provider
NovaFile
Document
Media
AI Agent
```

Die Beziehungen zwischen ihnen besitzen eine definierte Semantik.

Beispiele:

```text
signed-by
issued-by
derived-from
attested-by
operated-by
delegated-by
trusted-for
revoked-by
```

Die konkrete Menge der Knotentypen und Beziehungen wird in NPSPECs definiert.

## Trust Evidence

Trust-Graph-Kanten müssen auf nachvollziehbarer Trust Evidence beruhen können.

```text
Evidence
   ↓
Verified Relationship
   ↓
Trust Graph
```

Nicht verifizierte Aussagen dürfen nicht automatisch dieselbe Vertrauenswirkung wie verifizierte Beziehungen erhalten.

## Kontextabhängige Auswertung

Trust-Entscheidungen werden als Auswertung relevanter Beziehungen innerhalb des Graphen durchgeführt.

Dabei können insbesondere berücksichtigt werden:

- Purpose,
- Trust Domain,
- Trust Anchors,
- Provenance,
- Revocation,
- Identity Bindings,
- Attestation,
- aktuelle Policy.

Ein Trust Path, der für einen Zweck gültig ist, muss nicht für einen anderen Zweck gültig sein.

## Keine globale Trust-Zahl

NovaOS definiert keinen universellen Trust Score wie:

```text
Entity X = 87 % trusted
```

Interne Bewertungsmodelle dürfen Scores, Wahrscheinlichkeiten oder Gewichtungen als Evidence oder Optimierungshilfe verwenden.

Diese dürfen jedoch nicht die semantische Trust-Entscheidung ersetzen.

## Dynamischer Graph

Der Trust Graph ist veränderlich.

Ereignisse wie:

```text
Revocation
Credential Rotation
Software Update
Device State Change
New Provenance Evidence
Trust Anchor Change
```

können Beziehungen verändern oder ungültig machen.

Betroffene Trust-Entscheidungen müssen daraufhin neu bewertbar sein.

## Authority

Trust-Graph-Beziehungen erzeugen keine Capability Authority.

```text
Trust Graph → Trust Decision
Capability  → Authority
```

Beide Systeme können miteinander interagieren, bleiben aber getrennt.

## Normative Anforderungen

1. Nova.Trust MUSS Vertrauen als relationale und kontextabhängige Struktur modellieren.
2. NovaOS DARF keinen globalen Trust Score als primäres Trust-Modell verwenden.
3. Trust-relevante Entitäten MÜSSEN als eindeutig referenzierbare Graph-Knoten darstellbar sein.
4. Trust-relevante Beziehungen MÜSSEN eine definierte Semantik besitzen.
5. Trust-Beziehungen MÜSSEN auf nachvollziehbarer Evidence beruhen können.
6. Trust-Auswertung MUSS Purpose und Trust Domain berücksichtigen können.
7. Ein gültiger Trust Path DARF nicht automatisch für andere Zwecke gelten.
8. Revocation und andere Trust-Änderungen MÜSSEN den Graph beeinflussen können.
9. Betroffene Trust-Entscheidungen MÜSSEN neu bewertbar sein.
10. Numerische Scores DÜRFEN nur ergänzend verwendet werden.
11. Trust-Graph-Beziehungen DÜRFEN keine Capability Authority erzeugen.
12. Relevante Trust Paths MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Trust bleibt kontext- und zweckabhängig,
- komplexe Provenance- und Identity-Beziehungen können abgebildet werden,
- Trust-Entscheidungen werden nachvollziehbarer,
- unterschiedliche Trust Domains müssen nicht auf eine künstliche Zahl reduziert werden,
- dynamische Neubewertung wird unterstützt.

### Negative Konsequenzen

- Trust-Auswertung ist komplexer als ein einzelner Score,
- große Trust Graphs benötigen effiziente Indizierung und Auswertung.

## Verworfene Alternativen

### Globaler numerischer Trust Score

Verworfen, da unterschiedliche Arten und Zwecke von Vertrauen nicht sinnvoll auf eine universelle Zahl reduziert werden können.

### Binäres Trusted/Untrusted pro Entität

Verworfen, da dieselbe Entität je nach Zweck und Kontext unterschiedlich bewertet werden kann.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0002_Identity_Signature_Provenance_Permission_Capability_Audit`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0004_Trust_Anchors_nach_Zweck`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-GRAPH-0001`
- `NPSPEC-TRUST-GRAPH-NODE-0001`
- `NPSPEC-TRUST-GRAPH-RELATION-0001`
- `NPSPEC-TRUST-GRAPH-EVALUATION-0001`
- `NPSPEC-TRUST-GRAPH-UPDATE-0001`
- `NPSPEC-TRUST-GRAPH-TEST-0001`

## Ergebnis

Nova.Trust bildet Vertrauen als Graph aus Entitäten, Evidence und kontextabhängigen Beziehungen ab:

```text
Entities
   ↓
Trust Relationships
   ↓
Trust Graph
   ↓
Purpose + Context + Policy
   ↓
Trust Decision
```

Damit wird Vertrauen in NovaOS nicht auf einen globalen Score reduziert, sondern als nachvollziehbare, zweckgebundene und dynamisch bewertbare Beziehung modelliert.