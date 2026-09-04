# ADR-TRUST-0009 – Remote Attestation

## Status

Angenommen

## Kategorie

Trust Architecture / Attestation / Distributed Trust

## Kontext

NovaOS kann Anwendungen, Dienste, Capability Provider, Geräte und KI-Agenten auf entfernten Systemen ausführen oder verwenden.

Eine verifizierte Remote Identity beweist dabei lediglich, mit welcher Entität kommuniziert wird. Sie beweist nicht, dass sich deren Plattform, Software oder Ausführungsumgebung in einem erwarteten Zustand befindet.

Für sicherheitskritische Remote-Ausführung benötigt Nova.Trust deshalb einen Mechanismus, mit dem entfernte Systeme überprüfbare Aussagen über ihren aktuellen Zustand bereitstellen können.

## Entscheidung

NovaOS unterstützt **Remote Attestation als native Trust Evidence innerhalb von Nova.Trust**.

```text
Remote Identity
      +
Attestation Evidence
      ↓
Evidence Verification
      ↓
Nova.Trust
      ↓
Contextual Trust Decision
```

Remote Attestation liefert überprüfbare Aussagen über den Zustand einer entfernten Plattform oder Execution Domain.

Sie erzeugt jedoch weder automatisch Trust noch Authority.

## Grundprinzip

```text
Attested ≠ Trusted
Attested ≠ Authorized
```

Remote Attestation beantwortet primär:

```text
What state is this remote environment in?
```

Nova.Trust entscheidet anschließend:

```text
Is this state acceptable
for this purpose
under this policy?
```

## Attestation Subject

Attestation muss sich auf eine eindeutig identifizierbare Entität beziehungsweise Umgebung beziehen können.

Beispiele sind:

- physisches Gerät,
- Plattform,
- Betriebssysteminstanz,
- Execution Domain,
- Service Workload,
- Capability Provider,
- AI Agent Runtime.

```text
Identity
   ↓
Attested Environment
   ↓
Attestation Evidence
```

Identity und attestierter Zustand bleiben getrennte Eigenschaften.

## Attestation Evidence

Remote Attestation kann unter anderem Aussagen über folgende Eigenschaften transportieren:

- Boot State,
- Software Integrity,
- Runtime State,
- Platform Configuration,
- Security Configuration,
- Isolation State,
- relevante Softwareversionen,
- vertrauenswürdige Hardwarezustände.

Die konkrete Evidence-Struktur und verwendeten Attestation-Technologien werden in NPSPECs definiert.

## Hardware- und Software-Attestation

Nova.Trust darf nicht ausschließlich von einer bestimmten Hardwaretechnologie abhängig sein.

Attestation Provider können beispielsweise hardwaregestützt, softwaregestützt oder plattformspezifisch implementiert werden.

```text
Nova.Trust
    ↓
Attestation Provider
    ├── Hardware-backed
    ├── Platform-backed
    └── Software-backed
```

Die Qualität und Eigenschaften des jeweiligen Evidence Providers werden bei der Trust-Bewertung berücksichtigt.

## Freshness

Remote Attestation muss gegen die Wiederverwendung veralteter Evidence geschützt werden können.

Nova.Trust muss unterscheiden können zwischen:

```text
Current Evidence
Stale Evidence
Unknown Freshness
```

Für sicherheitskritische Entscheidungen darf veraltete Evidence nicht automatisch als aktueller Zustand interpretiert werden.

## Execution Contracts

`Nova.ExecutionContract` kann einen `Trust Requirement` enthalten, der Remote Attestation voraussetzt.

Beispiel:

```text
ExecutionContract
    ↓
Trust Requirement
    ↓
Remote Provider Candidates
    ↓
Attestation Verification
    ↓
Permitted Providers
```

Provider, welche die erforderliche Attestation nicht erfüllen, werden vor der Optimierung ausgeschlossen.

## Data Sovereignty

Erfolgreiche Remote Attestation ersetzt keine Data-Sovereignty-Prüfung.

```text
Attestation
     +
Trust
     +
Authority
     +
Sovereignty
     ↓
Remote Execution
```

Ein technisch vertrauenswürdiges System darf keine Daten erhalten, wenn deren Verarbeitung oder Übertragung dort nicht zulässig ist.

## Dynamische Neubewertung

Attestation beschreibt einen Zustand zu einem bestimmten Zeitpunkt beziehungsweise innerhalb eines definierten Gültigkeitskontexts.

Ändert sich der relevante Zustand, kann eine erneute Attestation erforderlich werden.

Länger laufende Remote-Ausführungen müssen daher eine policyabhängige Neubewertung unterstützen können.

## Normative Anforderungen

1. Nova.Trust MUSS Remote Attestation als native Trust Evidence unterstützen.
2. Attestation und Remote Identity MÜSSEN getrennte Konzepte bleiben.
3. Attestation DARF nicht automatisch Trust erzeugen.
4. Attestation DARF keine Capability Authority erzeugen.
5. Attestation Evidence MUSS an ein eindeutig bestimmbares Attestation Subject bindbar sein.
6. NovaOS MUSS unterschiedliche Attestation Provider unterstützen können.
7. Nova.Trust DARF nicht von einer einzelnen Hardware- oder Herstellertechnologie abhängig sein.
8. Die Eigenschaften und Vertrauenswürdigkeit des Evidence Providers MÜSSEN bewertbar sein.
9. Attestation Freshness MUSS überprüfbar sein können.
10. Veraltete oder unbekannt aktuelle Evidence DARF bei entsprechenden Hard Trust Requirements nicht als gültige aktuelle Attestation behandelt werden.
11. Execution Contracts MÜSSEN Remote-Attestation-Anforderungen ausdrücken können.
12. Attestation DARF Security-, Capability- oder Data-Sovereignty-Prüfungen nicht umgehen.
13. Änderungen des relevanten Remote-Zustands MÜSSEN eine Neubewertung ermöglichen.
14. Relevante Attestation-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Remote-Ausführung kann den tatsächlichen Plattformzustand berücksichtigen,
- Capability Provider und AI Agents können vor Remote-Nutzung überprüft werden,
- Attestation integriert sich direkt in Execution Contracts,
- Hardware-Attestation kann genutzt werden, ohne NovaOS an einen Hersteller zu binden,
- Trust-Entscheidungen können dynamisch auf Zustandsänderungen reagieren.

### Negative Konsequenzen

- Remote Attestation erhöht den Aufwand verteilter Trust-Entscheidungen,
- unterschiedliche Plattformen liefern Evidence mit unterschiedlicher Qualität,
- Attestation benötigt Mechanismen für Freshness und erneute Bewertung.

## Verworfene Alternativen

### Remote Identity reicht als Vertrauensnachweis

Verworfen, da Identity keine Aussage über den aktuellen Zustand der entfernten Ausführungsumgebung trifft.

### Erfolgreiche Attestation bedeutet automatisch vertrauenswürdig

Verworfen, da Attestation lediglich Evidence für Nova.Trust bereitstellt.

### Ausschließlich TPM-basierte Remote Attestation

Verworfen, da NovaOS unterschiedliche Hardwareplattformen und zukünftige Attestation-Technologien unterstützen soll.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0004_Trust_Anchors_nach_Zweck`
- `ADR-TRUST-0005_Trust_Graph_statt_globalem_Trust_Score`
- `ADR-IDENTITY-0004_Device_Identity`
- `ADR-SECURITY-0014_Secure_und_Measured_Boot_komplementär`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-TRUST-ATTESTATION-EVIDENCE-0001`
- `NPSPEC-TRUST-ATTESTATION-PROVIDER-0001`
- `NPSPEC-TRUST-ATTESTATION-FRESHNESS-0001`
- `NPSPEC-TRUST-REMOTE-ATTESTATION-0001`
- `NPSPEC-TRUST-ATTESTATION-TEST-0001`

## Ergebnis

Remote Attestation wird als überprüfbare Evidence in Nova.Trust integriert:

```text
Remote Identity
      +
Remote State Evidence
      ↓
Attestation Verification
      ↓
Nova.Trust
      ↓
Trust Requirement
      ↓
Security + Authority + Sovereignty
      ↓
Remote Execution
```

Damit kann NovaOS nicht nur feststellen, **mit wem es kommuniziert**, sondern auch bewerten, **ob sich dessen Ausführungsumgebung in einem für die konkrete Operation akzeptablen Zustand befindet**.