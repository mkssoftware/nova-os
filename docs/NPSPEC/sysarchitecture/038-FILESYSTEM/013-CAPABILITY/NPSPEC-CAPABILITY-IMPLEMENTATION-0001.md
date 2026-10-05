# NPSPEC-CAPABILITY-IMPLEMENTATION-0001 – Nova Capability Implementation

## Status

Angenommen

## Kategorie

Capability / Implementation

## Zweck

NovaOS definiert die konkrete ausführbare Implementierung einer Capability getrennt von Capability-Identität, Interface und Provider.

Mehrere Implementierungen derselben Capability dürfen parallel existieren und abhängig von Hardware, Ressourcen, Trust, Policy und Execution Contract ausgewählt werden.

## Grundprinzipien

```text
CapabilityID ≠ Implementation
Interface ≠ Implementation
Provider ≠ Implementation
Implementation Version ≠ Capability Version
Implementation ≠ Authority
Installed Implementation ≠ Selected Implementation
```

## Modell

Eine Implementierung wird mindestens beschrieben durch:

```text
CapabilityImplementation
├── ImplementationID
├── CapabilityID
├── ProviderID
├── ImplementationVersion
├── InterfaceVersion
├── EntryPoint
├── Compatibility
├── Requirements
└── State
```

Optional:

```text
Architecture
Algorithms
HardwareRequirements
ResourceProfile
TrustRequirements
IsolationMode
Determinism
ExecutionLocation
```

## Beziehung

```text
CapabilityID
    ↓
Interface
    ↓
Provider
    ├── Implementation A
    ├── Implementation B
    └── Implementation C
```

Ein Provider darf mehrere Implementierungen derselben Capability bereitstellen.

Beispiel:

```text
Image Filter
├── Generic CPU
├── SIMD CPU
├── GPU
└── Accelerator
```

## Auswahl

NovaOS wählt eine geeignete Implementierung anhand des Execution Contracts.

```text
Execution Contract
       ↓
Capability Resolution
       ↓
Compatible Implementations
       ↓
Trust + Policy
       ↓
Resource / Hardware Check
       ↓
Implementation Selection
       ↓
Execution
```

Hard Requirements müssen erfüllt werden. Soft Preferences dürfen zur Optimierung der Auswahl verwendet werden.

## Kompatibilität

Eine Implementierung darf Anforderungen definieren an:

```text
Architecture
CPU Features
GPU
Accelerator
Runtime
Framework
Libraries
Devices
System Interfaces
Interface Version
```

Nicht kompatible Implementierungen dürfen nicht aktiviert werden.

## Ressourcen

Implementierungen dürfen unterschiedliche Ressourcenprofile besitzen.

Beispiel:

```text
Implementation A → CPU optimized
Implementation B → GPU optimized
Implementation C → Low Energy
```

NovaOS darf anhand von Ressourcenbudget, Systemlast, Energiebedarf und Deadline dynamisch auswählen.

## Isolation

Eine Implementierung wird entsprechend ihrer Sicherheits- und Trust-Anforderungen ausgeführt.

Mögliche Ausführungsformen:

```text
In-Process
Isolated Process
Service
Driver Domain
Sandbox
Remote Provider
```

Die Ausführungsform darf die öffentliche Capability-Schnittstelle nicht verändern.

## Authority

Eine Implementierung erhält ausschließlich die für ihre konkrete Ausführung benötigte Authority.

```text
Caller Authority
      ↓
Policy + Execution Contract
      ↓
Attenuated Authority
      ↓
Implementation
```

Die Implementierung darf keine zusätzliche Authority aus ihrer Installation, ihrem Provider oder ihrer Registrierung ableiten.

## Austauschbarkeit

Implementierungen dürfen ersetzt oder aktualisiert werden, solange der öffentliche Capability-Vertrag eingehalten wird.

Bestehende Handles und laufende Ausführungen müssen gemäß Lifecycle- und Live-Evolution-Policy behandelt werden.

## Zustände

Mindestens:

```text
Available
Selected
Active
Unavailable
Incompatible
Restricted
Disabled
Failed
```

## Normative Anforderungen

1. Jede Capability-Implementierung MUSS eine stabile `ImplementationID` besitzen.
2. CapabilityID, ProviderID und ImplementationID MÜSSEN getrennte Identitäten bleiben.
3. Mehrere Implementierungen derselben Capability MÜSSEN parallel unterstützt werden können.
4. Implementierungen MÜSSEN ihre unterstützte Interface-Version deklarieren.
5. Hardware-, Runtime- und Ressourcenanforderungen MÜSSEN deklarierbar sein.
6. Nicht kompatible Implementierungen DÜRFEN nicht ausgewählt werden.
7. Die Auswahl MUSS Hard Requirements des Execution Contracts einhalten.
8. Soft Preferences DÜRFEN zur Optimierung der Auswahl verwendet werden.
9. Trust und Policy MÜSSEN vor der Ausführung berücksichtigt werden.
10. Eine Implementierung DARF keine Authority durch Installation oder Registrierung erhalten.
11. Effektive Authority MUSS auf die konkrete Ausführung begrenzt werden.
12. Unterschiedliche Ausführungs- und Isolationsformen MÜSSEN unterstützt werden können.
13. Implementierungen MÜSSEN austauschbar sein, solange der Capability-Vertrag eingehalten wird.
14. Identität, Version, Provider, Kompatibilität, Anforderungen und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`

## Ergebnis

NovaOS trennt die abstrakte Capability konsequent von ihrer konkreten Implementierung. Dadurch können CPU-, GPU-, Hardware-, Software-, lokale oder isolierte Implementierungen derselben Capability parallel existieren und zur Laufzeit anhand von Kompatibilität, Trust, Policy, Ressourcen und Execution Contract ausgewählt werden.