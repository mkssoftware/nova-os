# NPSPEC-SEMANTIC-RESOURCE-0001 – Nova Semantic Resource Model

## Status

Angenommen

## Kategorie

Architecture / Semantic / Resource Model

## Zweck

NovaOS definiert Ressourcen nach ihrer semantischen Bedeutung und ihren Eigenschaften, unabhängig von konkretem Gerät, Speicherort, Provider oder Implementierung.

```text
Resource
   ↓
Semantic Resource Description
   ↓
Requirements
   ↓
Discovery
   ↓
Provider / Physical Resource
```

Dadurch können Komponenten ausdrücken, **welche Ressource sie benötigen**, ohne vorher wissen zu müssen, wo oder wie diese bereitgestellt wird.

## Grundprinzipien

```text
Semantic Resource ≠ Physical Resource
Resource Identity ≠ Resource Location
Resource Type ≠ Provider
Resource Availability ≠ Authority
Resource Discovery ≠ Resource Access
Resource Compatibility ≠ Resource Ownership
```

## Semantic Resource

Eine semantische Ressource besitzt mindestens:

```text
SemanticResource
├── ResourceID
├── ResourceTypeID
├── Properties
├── Constraints
└── State
```

Optional:

```text
SemanticType
Provider
Location
Trust Properties
Sovereignty Properties
Performance Properties
Resource Capacity
Security Domain
Capability Requirements
```

## Resource-Typen

Beispiele:

```text
Compute.CPU
Compute.GPU
Compute.Accelerator

Storage.Persistent
Storage.Temporary

Memory.Shared
Memory.Private

Network.Connection

Display.Output
Audio.Input
Audio.Output

Device.Camera
Device.Sensor
Device.Printer
```

Die Typen beschreiben die Bedeutung der Ressource und nicht deren konkrete Hardware.

## Anforderungen

Komponenten können Ressourcen deklarativ anfordern.

```text
Resource Requirement
├── Resource Type
├── Capacity
├── Performance
├── Latency
├── Locality
├── Trust
├── Sovereignty
└── Security Constraints
```

Beispiel:

```text
Type: Compute.GPU
Memory: >= 2 GiB
Locality: Local
Trust: Trusted
```

## Resource Resolution

NovaOS löst semantische Anforderungen gegen verfügbare Ressourcen auf.

```text
Semantic Requirement
        ↓
Resource Discovery
        ↓
Candidate Resources
        ↓
Hard Constraints
        ↓
Policy
        ↓
Selection
```

Die konkrete Ressource kann sich ändern, solange die Anforderungen weiterhin erfüllt werden.

## Capability Integration

Ressourcenverfügbarkeit erzeugt keine Zugriffsrechte.

```text
Resource Found
     ↓
Capability Validation
     ↓
Authorized Resource Access
```

Es gilt:

```text
ResourceID ≠ CapabilityID
```

Der Zugriff erfolgt ausschließlich über geeignete Capabilities.

## ExecutionContract Integration

`Nova.ExecutionContract` kann semantische Ressourcenanforderungen enthalten.

```text
ExecutionContract
├── Operation
├── Semantic Input / Output
├── Resource Requirements
├── Resource Budget
├── Deadline
├── Determinism
└── Security / Trust Constraints
```

NovaOS kann dadurch geeignete Ressourcen automatisch auswählen.

## Resource Economy

Semantic Resources integrieren sich in die NovaOS Resource Economy.

```text
Demand
  ↓
Budget
  ↓
Resource Selection
  ↓
Allocation
  ↓
Usage
  ↓
Release
```

Eine Ressource darf nur innerhalb des zugewiesenen Resource Budgets verwendet werden.

## Location Transparency

Ressourcen können lokal oder entfernt bereitgestellt werden.

```text
Compute Requirement
├── Local CPU
├── Local GPU
├── Remote Compute
└── Specialized Accelerator
```

Dabei gilt:

```text
Identity ≠ Location
Transparent Location ≠ Transparent Authority
```

Sovereignty-, Trust- und Security-Anforderungen bleiben verbindlich.

## Dynamische Ressourcen

NovaOS muss Änderungen der verfügbaren Ressourcen berücksichtigen.

```text
Available
   ↓
Allocated
   ↓
Degraded
   ↓
Unavailable
```

Hotplug, Ausfall, Migration oder Ressourcenknappheit können eine erneute Auflösung auslösen.

## Re-Resolution

Kann eine Ressource die Anforderungen nicht mehr erfüllen:

```text
Resource Failure
      ↓
Requirement Re-Evaluation
      ↓
Alternative Resource
      ↓
Validation
      ↓
Continue / Degrade / Fail
```

Ein Wechsel darf Hard Constraints nicht verletzen.

## Semantic Type Integration

Semantic Types beschreiben Daten.

Semantic Resources beschreiben benötigte oder verfügbare Ressourcen.

```text
Semantic Type
     ↓
Operation
     ↓
Semantic Resource Requirement
     ↓
Capability Provider
```

Beide Modelle bleiben getrennt, können aber gemeinsam im ExecutionContract verwendet werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ResourceID
ResourceTypeID
Properties
Capacity
State
Provider
Location Class
Allocation
Resource Budget
Trust State
Capability Requirements
```

## Normative Anforderungen

1. NovaOS MUSS Ressourcen semantisch beschreiben können.
2. Semantic Resource Identity MUSS von physischer Location und Provider getrennt bleiben.
3. Ressourcenanforderungen SOLLEN deklarativ ausdrückbar sein.
4. Resource Discovery DARF keine Autorität erzeugen.
5. Ressourcenzugriff MUSS über Capabilities kontrolliert werden.
6. ExecutionContracts SOLLEN Semantic Resource Requirements enthalten können.
7. Resource Selection MUSS Hard Constraints berücksichtigen.
8. Resource Budgets MÜSSEN in die Auswahl und Nutzung integrierbar sein.
9. Ressourcen MÜSSEN dynamisch neu auflösbar sein.
10. Resource-Zustände MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0009`

## Ergebnis

```text
Semantic Resource Need
        ↓
Declarative Requirements
        ↓
Discovery + Resource Economy
        ↓
Policy + Capability Validation
        ↓
Suitable Resource
        ↓
Execution
```

NovaOS erhält damit eine abstrakte Ressourcenebene, durch die Software Ressourcen nach Bedeutung und Anforderungen anfordern kann, während das System selbst entscheidet, welche konkrete Hardware, welcher Provider oder welcher Standort diese Anforderungen am besten erfüllt.