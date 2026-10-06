# NPSPEC-FSCAPABILITY-EXECUTIONCONTRACT-0001 – Nova Filesystem Capability Execution Contract Integration

## Status

Angenommen

## Kategorie

Capability / Execution Contract

## Zweck

NovaOS definiert den Execution Contract für die kontrollierte Ausführung einer Capability.

Der Contract beschreibt die Anforderungen einer konkreten Ausführung und verbindet Capability, Eingaben, Ressourcen, Sicherheitskontext und Ausführungsziele, ohne selbst Authority zu erzeugen.

## Grundprinzipien

```text
Execution Contract ≠ Capability
Execution Contract ≠ Authority
Execution Contract ≠ Provider
Request ≠ Permission
Preferred Provider ≠ Forced Provider
Contract ≠ Implementation
```

## Modell

Ein Capability Execution Contract kann enthalten:

```text
ExecutionContract
├── CapabilityID
├── Operation
├── Inputs
├── Parameters
├── ExpectedOutputs
├── HardRequirements
├── SoftPreferences
├── Deadline
├── ResourceBudget
├── Determinism
├── Sovereignty
├── TrustRequirements
├── SecurityContext
└── ProviderConstraints
```

## Ablauf

```text
Execution Contract
       ↓
Validate
       ↓
Resolve Capability
       ↓
Evaluate Policy
       ↓
Derive Minimum Authority
       ↓
Select Provider
       ↓
Reserve Resources
       ↓
Execute
       ↓
Verify Result
```

## Hard Requirements

Harte Anforderungen müssen erfüllt werden.

Beispiele:

```text
Required Semantic Types
Required Capability Version
Deadline
Maximum Memory
Required Determinism
Required Trust Level
Required Execution Location
Security Constraints
```

Kann ein Hard Requirement nicht erfüllt werden, darf die Ausführung nicht stillschweigend davon abweichen.

## Soft Preferences

Optionale Präferenzen können beispielsweise sein:

```text
Low Latency
Low Energy
Preferred Provider
Preferred Algorithm
Preferred Device
Local Execution
High Quality
```

NovaOS darf davon abweichen, wenn Hard Requirements und Policy eingehalten werden.

## Provider-Auswahl

Mehrere Provider können denselben Contract erfüllen:

```text
CapabilityID
     ↓
Compatible Providers
     ↓
Contract Evaluation
     ↓
Selected Provider
```

Die Auswahl kann Trust, Ressourcen, Lokalität, Performance, Energiebedarf und aktuelle Systemlast berücksichtigen.

## Authority

Der Execution Contract erzeugt keine Berechtigungen.

NovaOS darf aus dem Contract ausschließlich die minimal benötigten Capabilities beziehungsweise attenuierten Handles für die konkrete Ausführung ableiten.

```text
Contract Requirements
       ↓
Existing Authority
       ∩
Policy
       ↓
Minimum Effective Authority
```

Fehlende Authority führt zu Ablehnung oder einer definierten Berechtigungsentscheidung.

## Ressourcen

Der Contract kann Ressourcenbudgets festlegen:

```text
CPU
Memory
Storage
I/O
Network
GPU
Energy
Time
```

Der Provider muss innerhalb der zugesicherten Grenzen arbeiten oder kontrolliert degradieren beziehungsweise fehlschlagen.

## Determinismus

Der Contract kann einen Determinismusgrad verlangen.

```text
BestEffort
Reproducible
Deterministic
```

Provider, die den geforderten Modus nicht unterstützen, dürfen nicht ausgewählt werden.

## Sovereignty und Location

Der Contract kann festlegen, wo Daten verarbeitet werden dürfen:

```text
LocalOnly
DeviceGroup
TrustedLocation
AllowedRegion
RemoteAllowed
```

Location Transparency darf diese Einschränkungen nicht umgehen.

## Abbruch und Deadline

Ausführungen müssen, soweit die Capability dies unterstützt, an Structured Concurrency angebunden sein:

```text
Parent Task
   ↓
Capability Execution
   ├── Deadline
   └── Cancellation
```

Abbruch muss Ressourcen und temporäre Authority kontrolliert freigeben.

## Normative Anforderungen

1. Capability-Ausführungen MÜSSEN durch einen Execution Contract beschreibbar sein.
2. Der Contract MUSS Capability und Operation eindeutig bestimmen.
3. Inputs, Parameter und erwartete Outputs MÜSSEN referenzierbar sein.
4. Hard Requirements und Soft Preferences MÜSSEN getrennt behandelt werden.
5. Nicht erfüllte Hard Requirements MÜSSEN zur Ablehnung oder definierten Degradation führen.
6. Provider-Auswahl MUSS anhand des Contracts erfolgen können.
7. Der Contract DARF keine Authority erzeugen.
8. Effektive Authority MUSS auf das für die Ausführung notwendige Minimum begrenzt werden.
9. Ressourcenbudgets MÜSSEN ausdrückbar sein.
10. Deadline, Cancellation und Determinismus MÜSSEN ausdrückbar sein.
11. Trust-, Security-, Sovereignty- und Location-Anforderungen MÜSSEN berücksichtigt werden können.
12. Temporär abgeleitete Authority MUSS nach Ende der Ausführung freigegeben oder ungültig werden können.
13. Contract, Provider-Auswahl und Ausführungsergebnis MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-INPUT-0001`
- `NPSPEC-CAPABILITY-OUTPUT-0001`
- `NPSPEC-CAPABILITY-PARAMETER-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaOS besitzt einen einheitlichen Vertrag für die konkrete Ausführung von Capabilities. Provider können anhand funktionaler, sicherheitsbezogener und ressourcenbezogener Anforderungen dynamisch ausgewählt werden, während Hard Requirements, Benutzerpräferenzen, Authority und tatsächliche Implementierung klar voneinander getrennt bleiben.
