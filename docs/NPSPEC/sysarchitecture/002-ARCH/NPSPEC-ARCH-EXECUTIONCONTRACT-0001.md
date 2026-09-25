# NPSPEC-ARCH-EXECUTIONCONTRACT-0001 – Nova Execution Contract

## Status

Angenommen

## Kategorie

Architecture / Execution / Contracts / Resource Management / Security

## Zweck

Der `Nova.ExecutionContract` beschreibt die Anforderungen, Grenzen und Präferenzen einer auszuführenden Operation.

Statt lediglich:

```text
Funktion ausführen
```

beschreibt der Aufrufer:

```text
Was soll ausgeführt werden?
Welche Anforderungen gelten?
Welche Ressourcen dürfen verwendet werden?
Welche Garantien werden benötigt?
```

NovaOS kann daraus einen geeigneten Ausführungsplan bestimmen.

## Grundprinzip

```text
Intent
  +
Input
  +
Execution Contract
       ↓
Execution Planning
       ↓
Provider / Algorithm / Hardware
       ↓
Execution
       ↓
Result
```

Der Execution Contract beschreibt Anforderungen und nicht zwingend die konkrete Implementierung.

## Contract-Struktur

Ein Execution Contract kann enthalten:

```text
ExecutionContract
 ├── Operation
 ├── Input Types
 ├── Output Types
 ├── Hard Requirements
 ├── Soft Preferences
 ├── Latency / Deadline
 ├── Resource Budget
 ├── Determinism
 ├── Data Sovereignty
 ├── Trust Requirements
 ├── Security Context
 ├── Preferred Algorithm
 ├── Forced Algorithm
 ├── Preferred Provider
 └── Execution Location
```

Nicht jede Operation muss alle Felder verwenden.

## Semantische Ein- und Ausgaben

Ein Contract beschreibt Ein- und Ausgaben bevorzugt über semantische Typen.

Beispiel:

```text
Input:
Image.RAW

Output:
Image.PNG
```

statt ausschließlich:

```text
byte[]
→
byte[]
```

Dadurch kann NovaOS geeignete Capabilities, Algorithmen und Provider bestimmen.

## Hard Requirements

Hard Requirements müssen erfüllt sein.

Beispiele:

```text
Deadline <= 10 ms
Deterministic = Required
DataLocation = LocalOnly
Trust >= Verified
Memory <= 256 MiB
```

Kann NovaOS eine harte Anforderung nicht erfüllen:

```text
Contract
   ↓
Unsatisfied Hard Requirement
   ↓
Execution rejected
```

Das System darf sie nicht stillschweigend abschwächen.

## Soft Preferences

Soft Preferences beschreiben gewünschte, aber nicht zwingende Eigenschaften.

Beispiele:

```text
Prefer Low Energy
Prefer GPU
Prefer Local Execution
Prefer Lowest Latency
Prefer Cached Provider
```

Sie dienen der Optimierung.

```text
Hard Constraints
       ↓
gültige Möglichkeiten
       ↓
Soft Preferences
       ↓
beste geeignete Auswahl
```

## Priorität

Bei der Ausführungsplanung gilt grundsätzlich:

```text
Safety
  ↓
Security
  ↓
Data Sovereignty / Trust
  ↓
Hard Execution Requirements
  ↓
Explicit User Requirements
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine Optimierung darf keine höher priorisierte Anforderung verletzen.

## Latency

Ein Contract kann maximale Latenz definieren.

```text
MaxLatency = 20 ms
```

NovaOS kann dies bei der Auswahl von:

```text
Provider
Algorithm
CPU
GPU
Accelerator
Execution Location
Scheduling
```

berücksichtigen.

## Deadline

Zeitkritische Operationen können eine Deadline besitzen.

```text
Deadline = T
```

oder:

```text
RelativeDeadline = 5 ms
```

Eine Deadline kann als:

```text
Hard
Firm
Soft
```

klassifiziert werden.

## Resource Budget

Ein Contract kann Ressourcen begrenzen.

Beispiele:

```text
CPU Time
Memory
GPU Time
VRAM
Storage IO
Network Bandwidth
Energy
Accelerator Time
```

Beispiel:

```text
Memory <= 512 MiB
GPUTime <= 8 ms
Energy = Minimize
```

## Determinismus

Der Contract kann Anforderungen an deterministische Ausführung enthalten.

```text
Determinism:
    Required
    Preferred
    NotRequired
```

Bei:

```text
Required
```

dürfen nur Ausführungspfade verwendet werden, die die geforderte Determinismusklasse erfüllen.

## Data Sovereignty

Der Contract kann festlegen, wo Daten verarbeitet werden dürfen.

Beispiele:

```text
LocalOnly

DeviceGroupOnly

Region = EU

TrustedInfrastructureOnly
```

Diese Anforderungen sind harte Constraints, sofern der Contract sie als solche definiert.

```text
Remote Provider available
        +
LocalOnly
        ↓
Remote Provider rejected
```

## Trust Requirements

Ein Contract kann Mindestanforderungen an den verwendeten Provider definieren.

Beispiele:

```text
Signed
Verified
TrustedProvider
HardwareAttested
KnownProvenance
```

Trust muss evidenzbasiert bewertet werden.

```text
Signed
≠
Trusted
≠
Authorized
```

## Security Context

Jede Ausführung erfolgt innerhalb eines Security Context.

```text
Identity
Permissions
Capabilities
Delegation
Policy
```

Ein Execution Contract erzeugt keine zusätzlichen Berechtigungen.

```text
Requested Operation
        ≠
Authorized Operation
```

## Algorithmusauswahl

Ein Contract kann einen bevorzugten Algorithmus angeben.

```text
PreferredAlgorithm = Algorithm.X
```

NovaOS darf einen anderen geeigneten Algorithmus wählen, wenn die Präferenz nicht erfüllbar oder eine bessere Option vorhanden ist.

## Forced Algorithm

Für spezielle Fälle kann ein Algorithmus explizit vorgeschrieben werden.

```text
ForcedAlgorithm = Algorithm.X
```

Dann darf NovaOS keinen anderen Algorithmus verwenden.

Kann dieser Algorithmus den Contract nicht erfüllen, schlägt die Ausführung fehl.

## Provider-Auswahl

Capabilities können mehrere Provider besitzen.

```text
Capability
 ├── Provider A
 ├── Provider B
 └── Provider C
```

Der Execution Planner filtert zuerst nach harten Anforderungen.

```text
Providers
    ↓
Capability Compatibility
    ↓
Security
    ↓
Trust / Sovereignty
    ↓
Hard Requirements
    ↓
Valid Providers
```

Danach werden Soft Preferences angewendet.

## Hardware-Auswahl

NovaOS kann unterschiedliche Compute Provider verwenden.

```text
CPU
GPU
NPU
DSP
FPGA
Remote Compute
```

Beispiel:

```text
Image Processing
      ↓
Execution Contract
      ↓
CPU / GPU / NPU evaluation
      ↓
GPU selected
```

Die Anwendung muss die konkrete Hardware nicht zwingend selbst auswählen.

## Execution Location

Der Contract kann Anforderungen an den Ausführungsort definieren.

```text
Local
Remote
Any
Specific Node
Specific Trust Domain
```

Ohne explizite Vorgabe darf NovaOS einen geeigneten Ausführungsort bestimmen.

Location Transparency darf keine Security- oder Sovereignty-Regel umgehen.

## Execution Planning

Die Auswahl erfolgt grundsätzlich nach:

```text
Operation
    ↓
Capability Discovery
    ↓
Candidate Providers
    ↓
Hard Constraint Filtering
    ↓
Algorithm Selection
    ↓
Resource Evaluation
    ↓
Soft Preference Optimization
    ↓
Execution Plan
```

## Execution Plan

Der erzeugte Plan beschreibt die konkrete Ausführung.

Beispiel:

```text
Capability:
Image.Resize

Provider:
Nova.Image

Algorithm:
Lanczos

Compute:
GPU0

Location:
Local

Memory Budget:
128 MiB

Deadline:
20 ms
```

Contract und Execution Plan bleiben getrennt.

```text
Contract
= Anforderungen

Execution Plan
= konkrete Umsetzung
```

## Ressourcenreservierung

Für Operationen mit harten Ressourcenanforderungen kann NovaOS Ressourcen vor der Ausführung reservieren.

```text
Plan
  ↓
Reserve
  ↓
Validate
  ↓
Execute
  ↓
Release
```

Eine erfolgreiche Planung darf keine Ressourcengarantie vortäuschen, wenn die notwendigen Ressourcen nicht tatsächlich reserviert werden können.

## Adaptive Optimierung

Adaptive Systeme dürfen historische Informationen verwenden.

Beispiele:

```text
Previous Runtime
Cache State
Provider Performance
Energy Consumption
Prediction Error
```

Damit kann NovaOS zukünftige Ausführungspläne verbessern.

Adaptive Entscheidungen bleiben jedoch unterhalb der Contract-Anforderungen.

## Replanning

Ändern sich Bedingungen vor oder während der Ausführung, kann ein neuer Plan erforderlich werden.

```text
Execution Plan
      ↓
Provider Failure
      ↓
Replanning
      ↓
Alternative Provider
```

Replanning ist nur erlaubt, wenn der neue Plan weiterhin alle Hard Requirements erfüllt.

## Contract Negotiation

Wenn ein Contract nicht vollständig erfüllbar ist, darf NovaOS bei dafür geeigneten Operationen Alternativen anbieten.

Beispiel:

```text
Requested:
4K Rendering in 10 ms

Available:
4K in 25 ms
1080p in 8 ms
```

NovaOS darf diese Alternativen vorschlagen.

Es darf den Contract jedoch nicht ohne Zustimmung verändern, wenn dadurch eine harte Anforderung verletzt würde.

## Unknown

Kann eine Anforderung nicht zuverlässig bewertet werden:

```text
Requirement State = Unknown
```

gilt:

```text
Unknown
≠
Satisfied
```

Bei harten Anforderungen muss `Unknown` daher grundsätzlich als nicht nachgewiesen behandelt werden.

## Observability

NovaOS soll erklären können:

```text
Warum wurde dieser Provider gewählt?

Warum wurde die GPU verwendet?

Warum wurde Remote Execution abgelehnt?

Welche Hard Requirement war nicht erfüllbar?

Welche Soft Preference bestimmte die Auswahl?
```

Dafür können Execution Decisions strukturiert protokolliert werden.

## Beispiel

```text
Operation:
Video.Transcode

Input:
Video.H264

Output:
Video.AV1

Hard Requirements:
    DataSovereignty = LocalOnly
    Memory <= 2 GiB

Soft Preferences:
    MinimizeEnergy
    PreferHardwareAcceleration

Determinism:
    NotRequired
```

Mögliche Planung:

```text
CPU Encoder
GPU Encoder
Remote Encoder
      ↓
LocalOnly removes Remote
      ↓
Resource validation
      ↓
GPU Encoder selected
```

## Fehlerverhalten

Eine Contract-Ausführung kann beispielsweise enden als:

```text
Completed
Failed
Cancelled
DeadlineMissed
ConstraintViolation
ProviderUnavailable
ResourceUnavailable
Unauthorized
Unknown
```

Fehlerzustände müssen semantisch unterscheidbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS einen gemeinsamen `Nova.ExecutionContract` für kontrollierte Ausführungsanforderungen bereitstellen.
2. Ein Execution Contract MUSS Hard Requirements und Soft Preferences unterscheiden können.
3. Hard Requirements DÜRFEN NICHT stillschweigend abgeschwächt werden.
4. Soft Preferences DÜRFEN nur innerhalb der durch Hard Requirements erlaubten Lösungsmenge optimiert werden.
5. Contracts SOLLEN semantische Input- und Output-Typen verwenden können.
6. Latency- und Deadline-Anforderungen MÜSSEN ausdrückbar sein.
7. Ressourcenbudgets MÜSSEN ausdrückbar sein.
8. Determinismus-Anforderungen MÜSSEN ausdrückbar sein.
9. Data-Sovereignty-Anforderungen MÜSSEN ausdrückbar sein.
10. Trust Requirements MÜSSEN ausdrückbar sein.
11. Ein Execution Contract DARF keine zusätzlichen Security Capabilities erzeugen.
12. Preferred und Forced Algorithm MÜSSEN semantisch unterschieden werden.
13. Ein Forced Algorithm DARF NICHT automatisch durch einen anderen Algorithmus ersetzt werden.
14. Provider-Auswahl MUSS harte Constraints vor Soft Preferences anwenden.
15. Hardware- und Execution-Location-Auswahl SOLLEN automatisch durch den Execution Planner erfolgen können.
16. Location Transparency DARF NICHT Security-, Trust- oder Sovereignty-Anforderungen umgehen.
17. Replanning DARF nur erfolgen, wenn der neue Plan weiterhin alle Hard Requirements erfüllt.
18. `Unknown` DARF bei harten Anforderungen NICHT als erfüllt interpretiert werden.
19. NovaOS SOLL Entscheidungen des Execution Planners nachvollziehbar machen können.
20. Adaptive Optimierung DARF NICHT die expliziten Anforderungen des Execution Contracts überschreiben.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0002_Semantic_Input_Output_Types`
- `ADR-EXECUTION-0003_Latency_Deadline`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-EXECUTION-0005_Data_Sovereignty`
- `ADR-EXECUTION-0006_Trust_Requirements`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-EXECUTION-0008_Preferred_Forced_Algorithm`
- `ADR-EXECUTION-0009_Hard_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatic_Execution_Planning`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-CAPABILITY-0001`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Ergebnis

Der `Nova.ExecutionContract` bildet die gemeinsame Sprache zwischen gewünschter Operation und konkreter Ausführung:

```text
Was soll passieren?
        +
Welche Grenzen gelten?
        +
Welche Eigenschaften werden bevorzugt?
        ↓
Nova.ExecutionContract
        ↓
Hard Constraint Filtering
        ↓
Execution Planning
        ↓
Provider + Algorithm + Hardware + Location
        ↓
Controlled Execution
```

Dadurch können Anwendungen beschreiben, **was sie benötigen**, während NovaOS selbst entscheiden kann, **wie und wo die Operation optimal ausgeführt wird**, ohne dabei Sicherheit, Ressourcenlimits, Determinismus, Trust oder Data Sovereignty zu verletzen.