# NPSPEC-CAPABILITY-COMPATIBILITY-0001 – Nova Capability Compatibility

## Status

Angenommen

## Kategorie

Capability / Compatibility

## Zweck

NovaOS definiert die Kompatibilitätsprüfung zwischen Capability-Anforderung, Interface, Implementierung, Provider und Ausführungsumgebung.

Kompatibilität entscheidet, ob eine konkrete Capability-Implementierung den geforderten Vertrag unter den aktuellen Systembedingungen erfüllen kann.

## Grundprinzipien

```text
Compatibility ≠ Trust
Compatibility ≠ Permission
Compatibility ≠ Authority
Compatibility ≠ Availability
Same CapabilityID ≠ Automatic Compatibility
Version Match ≠ Complete Compatibility
```

## Modell

Eine Kompatibilitätsprüfung kann berücksichtigen:

```text
CapabilityCompatibility
├── CapabilityVersion
├── InterfaceVersion
├── SemanticTypes
├── Architecture
├── Runtime
├── Framework
├── Dependencies
├── Hardware
├── SystemFeatures
└── ExecutionRequirements
```

## Zustände

Mindestens folgende Ergebnisse werden unterstützt:

```text
Compatible
CompatibleWithConstraints
CompatibleWithProvider
PartiallyCompatible
Incompatible
Unknown
```

`Unknown` darf nicht automatisch als `Compatible` behandelt werden.

## Prüfung

```text
Capability Request
      ↓
Version Requirements
      ↓
Interface Compatibility
      ↓
Semantic Type Compatibility
      ↓
Dependency Compatibility
      ↓
Runtime / Framework
      ↓
Architecture / Hardware
      ↓
Execution Contract
      ↓
Compatibility Result
```

Trust, Permission und Policy werden anschließend beziehungsweise parallel als eigenständige Prüfungen behandelt.

## Interface-Kompatibilität

Eine Implementierung muss den benötigten Interface-Vertrag unterstützen.

Dabei werden insbesondere geprüft:

```text
Operations
Inputs
Outputs
Parameters
Error Model
Execution Semantics
```

Fehlt eine zwingend benötigte Operation, ist die Implementierung für diesen Aufruf nicht vollständig kompatibel.

## Semantische Kompatibilität

Input- und Output-Typen müssen semantisch kompatibel sein.

```text
Output Capability A
        ↓
SemanticType
        ↓
Compatibility
        ↓
Input Capability B
```

Ein identisches physisches Datenformat allein reicht nicht aus.

## Plattform-Kompatibilität

Implementierungen dürfen Anforderungen besitzen an:

```text
CPU Architecture
CPU Features
GPU
Accelerator
Devices
Runtime
Framework
Libraries
System Interfaces
```

NovaOS muss diese gegen die tatsächliche Ausführungsumgebung prüfen können.

## Execution Contract

Kompatibilität wird für die konkrete Ausführung bewertet.

Beispiel:

```text
Implementation A
├── Functionally Compatible
├── Deterministic: No
└── Result: Incompatible

Execution Contract:
Deterministic = Required
```

Eine allgemein kompatible Implementierung kann somit für einen bestimmten Contract ungeeignet sein.

## Fallback

Ist eine Implementierung inkompatibel, darf NovaOS alternative Implementierungen oder Provider prüfen:

```text
Implementation A → Incompatible
Implementation B → Compatible
Implementation C → CompatibleWithConstraints
```

Ein Fallback darf Hard Requirements nicht umgehen.

## Cache

Kompatibilitätsergebnisse dürfen gecacht werden, müssen jedoch bei Änderungen relevanter Faktoren invalidiert werden:

```text
Version
Implementation
Hardware
Runtime
Dependencies
System Configuration
Execution Requirements
```

## Normative Anforderungen

1. Capability-Kompatibilität MUSS vor der Ausführung prüfbar sein.
2. CapabilityID allein DARF nicht als Kompatibilitätsnachweis gelten.
3. Capability- und Interface-Version MÜSSEN berücksichtigt werden.
4. Semantische Input- und Output-Typen MÜSSEN berücksichtigt werden.
5. Abhängigkeiten, Runtime, Framework und Systeminterfaces MÜSSEN prüfbar sein.
6. Architektur- und Hardwareanforderungen MÜSSEN berücksichtigt werden können.
7. Hard Requirements des Execution Contracts MÜSSEN erfüllt werden.
8. `Unknown` DARF nicht automatisch als `Compatible` behandelt werden.
9. Trust, Permission und Compatibility MÜSSEN getrennte Entscheidungen bleiben.
10. Alternative Implementierungen SOLLEN bei Inkompatibilität geprüft werden können.
11. Fallbacks DÜRFEN Hard Requirements nicht umgehen.
12. Kompatibilitätsergebnisse DÜRFEN gecacht werden, MÜSSEN aber invalidierbar sein.
13. Ergebnis und Ursache einer Kompatibilitätsentscheidung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-FSCAPABILITY-DEPENDENCY-0001`
- `NPSPEC-FSCAPABILITY-VERSIONING-0001`
- `NPSPEC-FSCAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Kompatibilitätsmodell für Capabilities. Dadurch kann das System feststellen, welche Implementierung einen konkreten Capability-Vertrag unter den aktuellen Software-, Hardware- und Ausführungsbedingungen tatsächlich erfüllen kann, ohne Kompatibilität mit Trust, Permission oder Authority zu vermischen.