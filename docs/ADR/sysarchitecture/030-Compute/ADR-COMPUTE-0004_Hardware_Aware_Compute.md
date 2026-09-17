# ADR-COMPUTE-0004 – Hardware-Aware Compute

## Status

Angenommen

## Kategorie

Compute Architecture / Hardware Awareness

## Kontext

Unterschiedliche Compute Provider besitzen unterschiedliche Hardwareeigenschaften.

Beispiele:

```text
CPU:
    Core Count
    SIMD Extensions
    Cache Topology

GPU:
    Compute Units
    VRAM
    Memory Bandwidth

NPU:
    Supported Tensor Types
    Operations

FPGA:
    Available Logic
    Configurations
```

Eine vollständig hardwareblinde Planung würde diese Unterschiede ignorieren und könnte ineffiziente oder ungeeignete Ausführungsentscheidungen erzeugen.

## Entscheidung

NovaOS verwendet **Hardware-Aware Compute**.

Die Unified Compute Runtime kennt relevante Hardwareeigenschaften der verfügbaren Compute Provider und berücksichtigt sie bei der Ausführungsplanung.

```text
Compute Request
      ↓
Compute Contract
      ↓
Hardware Requirements
      ↓
Provider Capabilities
      ↓
Execution Plan
```

Der Consumer bleibt dabei möglichst hardwareunabhängig.

## Hardware Description

Compute Provider stellen eine strukturierte Beschreibung ihrer relevanten Hardwareeigenschaften bereit.

Beispiele:

```text
Architecture
Instruction Sets
Core / Compute Unit Count
Memory
Cache
Bandwidth
Precision Support
Accelerator Features
Topology
```

Nur für die Planung relevante Eigenschaften müssen exponiert werden.

## Capability Mapping

Hardwareeigenschaften werden bevorzugt auf Compute Capabilities abgebildet.

```text
Hardware Feature
      ↓
Provider Capability
      ↓
Compute Runtime
```

Consumer sollen daher nicht direkt nach konkreten CPU- oder GPU-Modellen fragen müssen.

## Hardware Requirements

Ein Compute Contract kann notwendige Hardwareeigenschaften verlangen.

```text
Required:
    FP64
    SIMD
    Tensor Operations
    Minimum Memory
```

Provider, die diese Anforderungen nicht erfüllen, werden ausgeschlossen.

## Hardware Preferences

Hardwareeigenschaften können auch als Präferenz definiert werden.

```text
Preferred:
    GPU
    Large Cache
    Low Power Accelerator
```

Präferenzen dürfen Hard Constraints nicht überschreiben.

## Topologie

Die Compute Runtime darf die physische Hardwaretopologie berücksichtigen.

```text
CPU
 ├── NUMA Node A
 │    └── GPU A
 └── NUMA Node B
      └── GPU B
```

Dadurch können unnötige Speicher- und Interconnect-Transfers vermieden werden.

## Speichereigenschaften

Unterschiedliche Provider besitzen unterschiedliche Speicherarchitekturen.

```text
Shared Memory
Dedicated Memory
Unified Memory
NUMA Memory
Device Memory
```

Diese Eigenschaften müssen bei Datenplatzierung und Zero-Copy berücksichtigt werden können.

## Portabilität

Hardware Awareness darf nicht zu unnötiger Hardwarebindung führen.

```text
Compute Capability
       ↓
Hardware-Aware Planning
       ↓
Suitable Provider
```

Spezifische Hardware darf nur zwingend vorausgesetzt werden, wenn der Compute Contract dies verlangt.

## Dynamische Eigenschaften

Neben statischen Hardwaremerkmalen können dynamische Eigenschaften relevant sein:

```text
Current Load
Available Memory
Temperature / Throttling
Power State
Availability
```

Diese gehören zum beobachteten Provider State und können die Auswahl beeinflussen.

## Normative Anforderungen

1. NovaOS MUSS relevante Hardwareeigenschaften von Compute Providern erfassen können.
2. Compute Provider MÜSSEN ihre planungsrelevanten Hardwareeigenschaften deklarieren können.
3. Hardwareeigenschaften SOLLEN auf Compute Capabilities abgebildet werden.
4. Compute Contracts MÜSSEN Hardware Requirements ausdrücken können.
5. Hard Hardware Requirements MÜSSEN bei der Provider-Auswahl eingehalten werden.
6. Hardware Preferences DÜRFEN als Optimierung verwendet werden.
7. Hardwaretopologie SOLL bei Daten- und Compute-Platzierung berücksichtigt werden.
8. Speicherarchitektur und Transferkosten MÜSSEN berücksichtigt werden können.
9. Hardware Awareness DARF nicht zu unnötiger Hardwarebindung von Consumern führen.
10. Dynamische Hardwarezustände SOLLEN bei der Ausführungsplanung berücksichtigt werden.

## Konsequenzen

### Positive Konsequenzen

- vorhandene Hardware kann effizienter genutzt werden,
- Topologie und Speicherarchitektur fließen in die Planung ein,
- spezialisierte Hardwarefunktionen werden automatisch nutzbar,
- Consumer bleiben weitgehend hardwareunabhängig.

### Negative Konsequenzen

- Hardwarebeschreibungen müssen aktuell und zuverlässig sein,
- die Compute-Planung wird komplexer.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_CPU_GPU_NPU_DSP_FPGA_als_Compute_Provider`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-HARDWARE-DESCRIPTION-0001`
- `NPSPEC-COMPUTE-HARDWARE-REQUIREMENTS-0001`
- `NPSPEC-COMPUTE-HARDWARE-TOPOLOGY-0001`

## Ergebnis

NovaOS bleibt auf API-Ebene hardwareunabhängig, berücksichtigt aber die tatsächlichen Eigenschaften der Hardware bei der Ausführung:

```text
Compute Requirement
       ↓
Hardware Capabilities
       ↓
Topology + Resources
       ↓
Constraint Solving
       ↓
Suitable Provider
       ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
Der Consumer muss die Hardware
nicht selbst verwalten.

NovaOS kennt die Hardware
und nutzt ihre Eigenschaften
für die optimale Ausführungsplanung.
```