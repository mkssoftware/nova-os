# ADR-COMPUTE-0002 – CPU, GPU, NPU, DSP und FPGA als Compute Provider

## Status

Angenommen

## Kategorie

Compute Architecture / Provider

## Kontext

Moderne Systeme verfügen über unterschiedliche Recheneinheiten mit verschiedenen Stärken:

```text
CPU  → Allgemeine Berechnungen
GPU  → Stark parallele Berechnungen
NPU  → KI- und Tensor-Operationen
DSP  → Signalverarbeitung
FPGA → Rekonfigurierbare Hardwarebeschleunigung
```

NovaOS soll diese Hardware nicht als getrennte Ausführungswelten behandeln.

## Entscheidung

NovaOS modelliert **CPU, GPU, NPU, DSP und FPGA als Compute Provider** der Unified Compute Runtime.

```text
Compute Runtime
      ↓
 ┌────┼────┬────┬────┐
CPU  GPU  NPU  DSP  FPGA
```

Jeder Provider beschreibt seine verfügbaren Compute Capabilities und Ausführungseigenschaften.

## Provider Capabilities

Ein Compute Provider kann unterschiedliche Fähigkeiten anbieten.

```text
Provider:
    Capabilities
    Data Types
    Memory Properties
    Parallelism
    Performance Characteristics
    Determinism Properties
```

Die konkrete Hardwarearchitektur bleibt hinter dem Provider Contract gekapselt.

## CPU

Die CPU ist der universelle Compute Provider.

```text
CPU
 ├── Scalar
 ├── SIMD
 └── General Purpose
```

Sie dient auch als möglicher Fallback, wenn spezialisierte Provider nicht verfügbar sind und der Contract dies erlaubt.

## GPU

GPU Provider stellen massiv parallele Compute Capabilities bereit.

Typische Bereiche:

```text
Graphics
Image Processing
Simulation
Matrix Operations
Parallel Compute
```

Grafik- und Compute-Nutzung derselben GPU müssen durch das Resource Management koordiniert werden.

## NPU

NPU Provider stellen spezialisierte Capabilities für neuronale und tensorbasierte Berechnungen bereit.

```text
Tensor Operations
Inference
Matrix Acceleration
Neural Network Operations
```

KI-Funktionalität bleibt optional und darf keine Voraussetzung für die grundlegende Systemfunktion sein.

## DSP

DSP Provider werden für signalorientierte Berechnungen verwendet.

Beispiele:

```text
Audio Processing
Filtering
FFT
Noise Cancellation
Sensor Processing
```

DSPs können dadurch auch systemweite Medien- und Signal-Capabilities beschleunigen.

## FPGA

FPGA Provider stellen rekonfigurierbare Compute-Ressourcen bereit.

```text
Compute Requirement
       ↓
Compatible FPGA Configuration
       ↓
FPGA Provider
       ↓
Execution
```

Rekonfiguration muss explizit als Teil der Provider-Semantik berücksichtigt werden.

## Provider Selection

Die Unified Compute Runtime entscheidet anhand des Compute Contracts, welche Provider zulässig sind.

```text
Compute Request
      ↓
Required Capability
      ↓
Candidate Providers
      ↓
Contract Filtering
      ↓
CPU / GPU / NPU / DSP / FPGA
```

Die Auswahl darf nicht allein anhand theoretischer Rechenleistung erfolgen.

## Datenbewegung

Provider können unterschiedliche Speicherbereiche besitzen.

```text
System Memory
     ↓
GPU / NPU / DSP / FPGA Memory
```

Transferkosten müssen bei der Provider-Auswahl berücksichtigt werden.

Zero-Copy oder Shared Memory sollen bevorzugt werden, wenn Hardware und Sicherheitsmodell dies ermöglichen.

## Fallback

Spezialisierte Hardware darf nicht zwingend erforderlich sein, sofern der Compute Contract dies nicht verlangt.

```text
Preferred:
    NPU

Unavailable:
    ↓

Compatible GPU
    ↓
Compatible CPU
```

Fallback ist nur zulässig, wenn Semantik und Constraints erhalten bleiben.

## Normative Anforderungen

1. NovaOS MUSS CPU, GPU, NPU, DSP und FPGA als Compute Provider modellieren können.
2. Alle Compute Provider MÜSSEN über die Unified Compute Runtime erreichbar sein können.
3. Provider MÜSSEN ihre Capabilities und relevanten Ausführungseigenschaften deklarieren.
4. Consumer SOLLEN nicht direkt an eine bestimmte Hardwareklasse gekoppelt sein.
5. Provider Selection MUSS auf Compute Contracts basieren.
6. Datenbewegungs- und Speicherzugriffskosten MÜSSEN berücksichtigt werden können.
7. Zero-Copy SOLL bevorzugt werden, wenn dies technisch und sicher möglich ist.
8. Fallback zwischen Providern DARF die zugesicherte Semantik nicht verändern.
9. FPGA-Rekonfiguration MUSS als explizite Provider-Eigenschaft behandelt werden.
10. KI-spezifische Hardware DARF keine Voraussetzung für grundlegende NovaOS-Funktionen sein.

## Konsequenzen

### Positive Konsequenzen

- einheitliche Nutzung heterogener Rechenhardware,
- spezialisierte Hardware kann automatisch genutzt werden,
- Anwendungen bleiben weitgehend hardwareunabhängig,
- neue Beschleunigertypen können als weitere Provider ergänzt werden.

### Negative Konsequenzen

- Provider unterscheiden sich stark bei Speicher-, Timing- und Ausführungseigenschaften,
- die Runtime benötigt zuverlässige Hardware- und Capability-Beschreibungen.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-PROVIDER-0001`
- `NPSPEC-COMPUTE-CPU-0001`
- `NPSPEC-COMPUTE-GPU-0001`
- `NPSPEC-COMPUTE-NPU-0001`
- `NPSPEC-COMPUTE-DSP-0001`
- `NPSPEC-COMPUTE-FPGA-0001`

## Ergebnis

NovaOS behandelt unterschiedliche Recheneinheiten als spezialisierte Provider derselben Compute-Architektur:

```text
Compute Request
      ↓
Unified Compute Runtime
      ↓
Capability + Contract
      ↓
Provider Selection
      ↓
CPU / GPU / NPU / DSP / FPGA
      ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
CPU, GPU, NPU, DSP und FPGA
sind keine getrennten Rechenwelten.

Sie sind unterschiedliche Provider
derselben Compute Capabilities.
```