# ADR-COMPUTE-0009 – Providerwahl unter Execution Contracts

## Status

Angenommen

## Kategorie

Compute Architecture / Execution Contracts

## Kontext

NovaOS kann für eine Compute Capability mehrere technisch kompatible Provider besitzen.

```text
CPU
GPU
NPU
DSP
FPGA
Remote Provider
```

Technische Kompatibilität allein reicht jedoch nicht aus.

Ein Provider darf nur gewählt werden, wenn er die Anforderungen des jeweiligen `Execution Contract` erfüllen kann.

## Entscheidung

Die **Providerwahl der Unified Compute Runtime erfolgt grundsätzlich unter dem vollständigen Execution Contract**.

```text
Compute Request
      ↓
Execution Contract
      ↓
Candidate Providers
      ↓
Hard Constraint Filtering
      ↓
Valid Providers
      ↓
Soft Constraint Optimization
      ↓
Selected Provider
```

Der Execution Contract bildet damit die verbindliche Grundlage jeder Compute-Providerwahl.

## Execution Contract

Für Compute können insbesondere folgende Anforderungen relevant sein:

```text
Semantic Types
Correctness
Determinism
Latency / Deadline
Resource Budget
Energy Budget
Trust Requirement
Data Sovereignty
Locality
Required Hardware
Preferred / Required Provider
```

Nicht jede Operation muss alle Felder verwenden.

## Hard Constraints

Verbindliche Anforderungen werden zuerst geprüft.

Beispiele:

```text
Safety
Security
Data Sovereignty
Hard Realtime
Determinism
Required Precision
Trust
Required Hardware
Required Provider
```

Ein Provider, der einen Hard Constraint nicht erfüllt, wird aus der Kandidatenmenge entfernt.

```text
Candidate Provider
       ↓
Contract satisfied?
 ├── Yes → Valid Candidate
 └── No  → Rejected
```

## Soft Constraints

Erst nach erfolgreicher Prüfung aller Hard Constraints dürfen Optimierungsziele berücksichtigt werden.

Beispiele:

```text
Performance
Energy
Latency Preference
Data Locality
Current Load
Transfer Cost
Preferred Hardware
Adaptive Prediction
```

Soft Constraints dürfen keinen Hard Constraint abschwächen.

## Priorisierung

Die Compute-Providerwahl folgt der systemweiten Prioritätsordnung:

```text
Safety
  ↓
Security
  ↓
Data Sovereignty
  ↓
Hard Realtime
  ↓
Determinism
  ↓
Correctness
  ↓
Explicit User Constraints
  ↓
Resource Budget
  ↓
Performance
  ↓
Energy
  ↓
Adaptive Policy
  ↓
Preference
```

## Contract Matching

Provider deklarieren ihre relevanten Fähigkeiten und Garantien.

```text
Execution Contract
        ↕
Provider Contract
        ↓
Compatible / Incompatible
```

Die Runtime darf Eigenschaften eines Providers nicht annehmen, wenn diese nicht bekannt oder verifiziert sind.

Für verpflichtende Anforderungen gilt:

```text
Unknown ≠ Satisfied
```

## Dynamischer Systemzustand

Die Providerwahl berücksichtigt neben statischen Fähigkeiten auch den aktuellen Zustand.

```text
Provider Capability
        +
Current Resources
        +
Execution Contract
        ↓
Eligibility
```

Ein grundsätzlich geeigneter Provider kann dadurch temporär ungeeignet sein.

Beispiele:

```text
Insufficient Memory
Resource Budget exhausted
Deadline no longer achievable
Provider unavailable
Trust changed
```

## Provider Override

Explizite Provider Overrides werden als Bestandteil des Execution Contract behandelt.

```text
Preferred Provider → Soft Constraint
Required Provider  → Hard Constraint
```

Auch ein Required Provider muss weiterhin alle übergeordneten Hard Constraints erfüllen.

## Distributed Compute

Für Remote Provider gelten zusätzliche verteilte Anforderungen.

```text
Compute Contract
      +
Trust
      +
Data Sovereignty
      +
Network Conditions
      +
Remote Availability
      ↓
Remote Provider Eligibility
```

Remote Compute wird nicht außerhalb des normalen Contract-Modells ausgewählt.

## Replanning

Ändern sich relevante Bedingungen vor oder während der Ausführung, kann eine erneute Providerwahl notwendig werden.

```text
Selected Provider
       ↓
Contract no longer satisfiable
       ↓
Replanning
       ↓
New Valid Provider
```

Migration, Retry oder erneute Ausführung sind nur zulässig, wenn die Operationssemantik dies erlaubt.

## Keine gültigen Provider

Kann kein Provider den Execution Contract erfüllen, darf NovaOS die Anforderungen nicht stillschweigend reduzieren.

```text
Candidate Providers
       ↓
Contract Filtering
       ↓
∅
       ↓
Unsatisfied Execution Contract
```

Der Grund muss strukturiert ermittelbar sein.

## Normative Anforderungen

1. Jede Compute-Providerwahl MUSS den relevanten Execution Contract berücksichtigen.
2. Hard Constraints MÜSSEN vor Soft Constraints ausgewertet werden.
3. Provider, die einen Hard Constraint nicht erfüllen, DÜRFEN nicht ausgewählt werden.
4. Unbekannte Eigenschaften DÜRFEN nicht als erfüllte Hard Constraints behandelt werden.
5. Soft Constraints DÜRFEN Hard Constraints nicht abschwächen.
6. Aktuelle Ressourcen- und Providerzustände MÜSSEN berücksichtigt werden können.
7. Preferred Provider MÜSSEN als Soft Constraint behandelt werden.
8. Required Provider MÜSSEN als Hard Constraint behandelt werden.
9. Remote Provider MÜSSEN zusätzliche Trust-, Sovereignty- und Netzwerkbedingungen erfüllen.
10. Bei Contract-Verletzungen MUSS Replanning möglich sein, sofern die Operationssemantik dies erlaubt.
11. Kann kein Provider den Contract erfüllen, MUSS die Anforderung als nicht erfüllbar gemeldet werden.
12. NovaOS DARF Execution Contracts nicht stillschweigend reduzieren, um eine Ausführung zu ermöglichen.

## Konsequenzen

### Positive Konsequenzen

- Providerwahl folgt verbindlichen Systemanforderungen,
- Performance kann Sicherheit und Korrektheit nicht überschreiben,
- lokale und Remote Provider verwenden dasselbe Auswahlmodell,
- Providerentscheidungen werden nachvollziehbar und reproduzierbar.

### Negative Konsequenzen

- Provider müssen ihre Fähigkeiten und Garantien präzise beschreiben,
- komplexe Contracts können die Provider-Auswahl aufwendiger machen.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-COMPUTE-0006_Latency_Aware_Compute`
- `ADR-COMPUTE-0007_Distributed_Compute`
- `ADR-COMPUTE-0008_Explizites_Provider_Override`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-EXECUTION-CONTRACT-0001`
- `NPSPEC-COMPUTE-PROVIDER-CONTRACT-MATCHING-0001`
- `NPSPEC-COMPUTE-PROVIDER-ELIGIBILITY-0001`
- `NPSPEC-COMPUTE-CONTRACT-REJECTION-0001`

## Ergebnis

Die Compute-Providerwahl wird vollständig durch den Execution Contract begrenzt:

```text
Compute Request
      ↓
Execution Contract
      ↓
Hard Constraints
      ↓
Valid Provider Set
      ↓
Soft Optimization
      ↓
Selected Provider
      ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
NovaOS wählt nicht den
schnellsten verfügbaren Provider.

NovaOS wählt zuerst die Provider,
die den Execution Contract erfüllen.

Erst danach wird optimiert.
```