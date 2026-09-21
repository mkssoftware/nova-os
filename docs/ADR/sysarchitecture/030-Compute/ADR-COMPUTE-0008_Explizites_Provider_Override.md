# ADR-COMPUTE-0008 – Explizites Provider Override

## Status

Angenommen

## Kategorie

Compute Architecture / Provider Selection

## Kontext

NovaOS wählt Compute Provider normalerweise automatisch anhand von Capability, Execution Contract, Ressourcenstatus und Optimierungszielen aus.

In bestimmten Fällen muss ein Consumer, Entwickler oder Administrator jedoch einen bestimmten Provider bevorzugen oder verbindlich vorgeben können.

Beispiele:

```text
Debugging
Benchmarking
Hardware Validation
Reproduzierbare Tests
Spezialisierte Hardware
Manuelle Optimierung
Diagnose
```

Ein Override darf dabei grundlegende Sicherheits- und Korrektheitsanforderungen nicht umgehen.

## Entscheidung

NovaOS unterstützt ein **explizites Provider Override** innerhalb der Unified Compute Runtime.

Dabei werden zwei Formen unterschieden:

```text
Preferred Provider
Required Provider
```

`Preferred` beeinflusst die automatische Auswahl.

`Required` beschränkt die Auswahl verbindlich auf den angegebenen Provider oder eine definierte Provider-Klasse.

## Preferred Provider

Ein Preferred Provider ist eine Soft Constraint.

```text
Preferred = GPU
      ↓
GPU geeignet?
 ├── Ja   → bevorzugen
 └── Nein → normale Provider-Auswahl
```

Die Runtime darf einen anderen Provider wählen, wenn der bevorzugte Provider ungeeignet oder nicht verfügbar ist.

## Required Provider

Ein Required Provider ist eine explizite Auswahlbedingung.

```text
Required = NPU
      ↓
NPU erfüllt Contract?
 ├── Ja   → ausführen
 └── Nein → Request ablehnen
```

NovaOS darf in diesem Fall nicht stillschweigend auf CPU, GPU oder einen anderen Provider ausweichen.

## Override-Ziele

Overrides können unterschiedliche Granularität besitzen.

Beispiele:

```text
Provider Class = GPU
Provider ID    = GPU-1
Device         = Local
Provider       = Remote.Node42.GPU
```

Die konkrete Provider-Identität bleibt von der logischen Compute Capability getrennt.

## Contract-Prüfung

Ein Provider Override umgeht nicht den Compute Contract.

```text
Explicit Override
       ↓
Provider Resolution
       ↓
Contract Validation
       ↓
Allowed / Rejected
```

Insbesondere bleiben folgende Anforderungen bindend:

```text
Correctness
Safety
Security
Trust
Data Sovereignty
Determinism
Required Precision
Hard Realtime
Authorization
```

## Konflikte

Steht ein Override im Konflikt mit einem Hard Constraint, gewinnt der Hard Constraint.

```text
Required Provider
       +
Hard Constraint Violation
       ↓
Rejected
```

NovaOS darf Sicherheits- oder Korrektheitsanforderungen nicht abschwächen, um ein Override zu erfüllen.

## Distributed Provider

Overrides dürfen auch Remote Provider adressieren.

Diese unterliegen weiterhin:

```text
Trust
Authorization
Data Sovereignty
Network Availability
Execution Contract
```

Ein explizit gewählter Remote Provider erhält dadurch keine zusätzlichen Rechte.

## Deterministic Mode

Provider Overrides können für reproduzierbare Tests und Benchmarks verwendet werden.

```text
Compute Request
      ↓
Fixed Provider
      ↓
Fixed Execution Constraints
      ↓
Reproducible Execution
```

Ein Override allein garantiert jedoch keinen Determinismus. Der ausgewählte Provider muss den Determinism Contract erfüllen.

## Observability

NovaOS soll sichtbar machen, wenn die normale automatische Provider-Auswahl überschrieben wurde.

Beispielsweise:

```text
Selection Mode: Required Override
Requested: GPU-1
Selected: GPU-1
Reason: Explicit Provider Override
```

Fehlgeschlagene Overrides müssen einen strukturierten Grund liefern.

## Normative Anforderungen

1. NovaOS MUSS explizite Provider Overrides unterstützen können.
2. `Preferred Provider` MUSS als Soft Constraint behandelt werden.
3. `Required Provider` MUSS als verbindliche Auswahlbedingung behandelt werden.
4. Ein Required Override DARF keinen stillen Provider-Fallback erlauben.
5. Overrides DÜRFEN Hard Constraints nicht umgehen.
6. Der ausgewählte Provider MUSS den Compute Contract erfüllen.
7. Overrides MÜSSEN sowohl Provider-Klassen als auch konkrete Provider adressieren können.
8. Remote Provider Overrides MÜSSEN weiterhin Trust-, Authorization- und Sovereignty-Prüfungen durchlaufen.
9. Ein Override DARF keine zusätzlichen Berechtigungen verleihen.
10. Provider Overrides SOLLEN über Introspection und Logging nachvollziehbar sein.
11. Fehlgeschlagene Overrides MÜSSEN einen strukturierten Ablehnungsgrund liefern.

## Konsequenzen

### Positive Konsequenzen

- gezielte Hardwareauswahl für Tests und Diagnose,
- reproduzierbare Benchmarks werden erleichtert,
- spezialisierte Hardware kann explizit erzwungen werden,
- automatische Provider-Auswahl bleibt der Standardfall.

### Negative Konsequenzen

- manuelle Overrides können suboptimale Performance oder Energieeffizienz verursachen,
- falsch gesetzte Required Overrides können Compute Requests unbrauchbar machen.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_CPU_GPU_NPU_DSP_FPGA_als_Compute_Provider`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0004_Hardware_Aware_Compute`
- `ADR-COMPUTE-0007_Distributed_Compute`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-PROVIDER-OVERRIDE-0001`
- `NPSPEC-COMPUTE-PROVIDER-PREFERENCE-0001`
- `NPSPEC-COMPUTE-PROVIDER-REQUIREMENT-0001`

## Ergebnis

NovaOS erlaubt die automatische Provider-Auswahl gezielt zu beeinflussen oder einzuschränken:

```text
Compute Request
      ↓
Provider Override
      ↓
Hard Constraint Validation
      ↓
Compute Contract
      ↓
Selected Provider
      ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
NovaOS wählt normalerweise
den geeigneten Compute Provider.

Eine explizite Auswahl ist möglich,
aber sie darf niemals die verbindlichen
System- und Sicherheitsverträge umgehen.
```