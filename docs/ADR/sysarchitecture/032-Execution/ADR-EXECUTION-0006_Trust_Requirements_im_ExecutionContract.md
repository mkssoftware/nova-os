# ADR-EXECUTION-0006 – Trust Requirements im ExecutionContract

## Status

Angenommen

## Kategorie

Execution Architecture / Trust

## Kontext

NovaOS kann eine Ausführung durch unterschiedliche lokale oder entfernte Provider durchführen.

Ein Provider kann technisch geeignet und autorisiert sein, ohne das für eine bestimmte Ausführung erforderliche Vertrauensniveau zu besitzen.

```text
Technically Compatible
        ≠
Authorized
        ≠
Trusted
```

Trust muss deshalb als explizite Anforderung im `Nova.ExecutionContract` modelliert werden.

## Entscheidung

`Nova.ExecutionContract` unterstützt explizite **Trust Requirements**.

```text
Execution Request
       ↓
Trust Requirements
       ↓
Trust Evidence
       ↓
Trust Evaluation
       ↓
Eligible Providers
       ↓
Execution Planning
```

Trust Requirements sind Hard Constraints, wenn der Contract ein bestimmtes Vertrauensniveau verlangt.

## Evidence-basierter Trust

NovaOS behandelt Trust nicht als pauschale Eigenschaft eines Providers.

Trust basiert auf überprüfbarer Evidence.

Beispiele:

```text
Identity
Signature
Measured Boot
Software Provenance
Hardware Attestation
Configuration State
Security State
Administrative Domain
Execution Environment
```

Die benötigte Evidence hängt vom jeweiligen Execution Contract ab.

## Trust Requirement

Ein Contract kann Anforderungen an unterschiedliche Vertrauenseigenschaften definieren.

Beispiele:

```text
Verified Identity
Signed Software
Known Provenance
Measured Environment
Trusted Hardware
Approved Organization
Required Security State
Required Trust Domain
```

Mehrere Anforderungen können kombiniert werden.

## Trust Evaluation

Vor der Auswahl eines Providers werden dessen Trust-Eigenschaften gegen den Contract geprüft.

```text
Provider
   +
Trust Evidence
   ↓
Trust Evaluation
   ↓
Satisfied / Unsatisfied / Unknown
```

Für Hard Trust Requirements gilt:

```text
Unknown ≠ Satisfied
```

Fehlende Evidence darf nicht als positives Vertrauenssignal interpretiert werden.

## Trust, Identity und Authorization

Trust bleibt von Identity und Authorization getrennt.

```text
Identity
   ↓
Trust Evidence
   ↓
Trust Evaluation
   ↓
Authorization
   ↓
Execution Eligibility
```

Eine bekannte Identität bedeutet nicht automatisch Vertrauen.

Ein vertrauenswürdiger Provider besitzt nicht automatisch die Berechtigung zur Verarbeitung bestimmter Daten.

## Provider Filtering

Trust Requirements werden vor Performance-Optimierungen ausgewertet.

```text
Candidate Providers
       ↓
Security Constraints
       ↓
Data Sovereignty
       ↓
Trust Requirements
       ↓
Authorization
       ↓
Valid Providers
       ↓
Performance / Energy Optimization
```

Ein schnellerer Provider darf einen nicht erfüllten Trust Requirement niemals kompensieren.

## Remote Execution

Remote Provider benötigen ausreichende Trust Evidence, bevor sie als zulässiger Ausführungsort gelten.

```text
Remote Provider
      ↓
Identity Verification
      ↓
Trust Evidence
      ↓
Sovereignty Check
      ↓
Authorization
      ↓
Eligible
```

Netzwerkerreichbarkeit oder bekannte Adresse sind kein Trust-Nachweis.

## Dynamischer Trust

Trust kann sich während der Systemlaufzeit ändern.

Beispiele:

```text
Certificate Revoked
Measured State Changed
Software Updated
Security Incident
Provider Compromised
Trust Policy Changed
```

NovaOS muss Trust deshalb bei relevanten Ausführungen erneut bewerten können.

## Laufende Ausführungen

Verliert ein Provider während einer Ausführung notwendige Trust-Eigenschaften, muss dies als Contract-relevantes Ereignis behandelt werden.

Mögliche Reaktionen:

```text
Continue
Restrict
Checkpoint
Migrate
Abort
Quarantine
```

Die zulässige Reaktion hängt von Operationssemantik, Sicherheitsanforderungen und Execution Contract ab.

## Algorithmus und Implementation Trust

Trust Requirements können sich auch auf die verwendete Algorithmusimplementierung beziehen.

```text
Algorithm
    ↓
Implementation
    ↓
Signature
Provenance
Integrity
    ↓
Trust Evaluation
```

Die Registrierung eines Algorithmus in der Algorithm Registry stellt keinen Trust-Nachweis dar.

## Trust Propagation

Bei zusammengesetzten Ausführungen müssen relevante Trust Requirements an Unteroperationen weitergegeben werden.

```text
Parent Execution
      ↓
Trust Requirements
      ↓
Child A
Child B
Child C
```

Eine Unteroperation darf strengere Anforderungen des übergeordneten Contracts nicht umgehen.

## Replanning und Fallback

Bei Provider-Ausfall darf nur auf einen Provider gewechselt werden, der die Trust Requirements ebenfalls erfüllt.

```text
Provider Failure
      ↓
Replanning
      ↓
Trust Evaluation
      ↓
Valid Alternative?
 ├── Yes → Continue
 └── No  → Contract Unsatisfied
```

Trust Requirements dürfen für einen Fallback nicht automatisch abgeschwächt werden.

## Introspection

Trust-Entscheidungen müssen nachvollziehbar sein.

NovaOS soll beantworten können:

```text
Welche Trust Requirements galten?
Welche Evidence wurde verwendet?
Welche Evidence fehlte?
Warum wurde ein Provider akzeptiert?
Warum wurde ein Provider ausgeschlossen?
```

Sensible Trust Evidence darf dabei nicht unnötig offengelegt werden.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Trust Requirements ausdrücken können.
2. Verbindliche Trust Requirements MÜSSEN als Hard Constraints behandelt werden.
3. Trust MUSS auf überprüfbarer Evidence basieren.
4. Identity, Trust und Authorization MÜSSEN getrennte Konzepte bleiben.
5. `Unknown` DARF einen Hard Trust Requirement nicht erfüllen.
6. Providerwahl MUSS Trust Requirements vor Performance-Optimierungen prüfen.
7. Remote Provider MÜSSEN ausreichende Trust Evidence bereitstellen können.
8. Algorithmusimplementierungen MÜSSEN Trust- und Provenance-Anforderungen unterliegen können.
9. Registry-Einträge DÜRFEN nicht automatisch als vertrauenswürdig gelten.
10. Trust MUSS bei relevanten Zustandsänderungen neu bewertbar sein.
11. Der Verlust notwendiger Trust-Eigenschaften MUSS als Contract-relevantes Ereignis erkannt werden.
12. Unteroperationen MÜSSEN relevante Trust Requirements propagieren.
13. Replanning und Fallback DÜRFEN Trust Requirements nicht abschwächen.
14. Performance, Energie oder adaptive Optimierung DÜRFEN Hard Trust Requirements niemals überstimmen.
15. Trust-Entscheidungen MÜSSEN introspektierbar sein, ohne sensible Evidence unnötig offenzulegen.

## Konsequenzen

### Positive Konsequenzen

- Trust wird expliziter Bestandteil der Ausführungsplanung,
- lokale und entfernte Provider können nach denselben Grundprinzipien bewertet werden,
- kompromittierte oder nicht ausreichend belegte Provider können früh ausgeschlossen werden,
- Trust bleibt von bloßer Identität und Berechtigung getrennt.

### Negative Konsequenzen

- Provider benötigen geeignete Trust Evidence,
- Trust Evaluation verursacht zusätzlichen Planungsaufwand,
- dynamische Trust-Änderungen können Replanning oder Abbruch laufender Ausführungen erzwingen.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0005_Data_Sovereignty_im_ExecutionContract`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-IDENTITY-0001_Trennung_von_Identität_Anzeige_Credential_Prozess_und_Adresse`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-DISTRIBUTED-0004_Remote_Capabilities`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ALGORITHM-0008_Algorithm_Registry`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-TRUST-REQUIREMENT-0001`
- `NPSPEC-EXECUTION-TRUST-EVIDENCE-0001`
- `NPSPEC-EXECUTION-TRUST-EVALUATION-0001`
- `NPSPEC-EXECUTION-TRUST-REEVALUATION-0001`

## Ergebnis

Trust wird direkt in die Execution Planning Pipeline integriert:

```text
Execution Request
       ↓
Nova.ExecutionContract
       ↓
Trust Requirements
       ↓
Trust Evidence
       ↓
Trust Evaluation
       ↓
Sovereignty + Authorization
       ↓
Valid Algorithm + Provider
       ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
NovaOS vertraut einem Provider
nicht allein deshalb,
weil er verfügbar oder bekannt ist.

Eine Ausführung darf nur dort stattfinden,
wo das erforderliche Vertrauen
durch geeignete Evidence belegt ist.
```