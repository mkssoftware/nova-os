# NPSPEC-CAPABILITY-INTERFACE-0001 – Nova Capability Interface

## Status

Angenommen

## Kategorie

Capability / Interface

## Zweck

NovaOS definiert die standardisierte Schnittstelle zwischen Capability-Aufrufern und Capability-Providern.

Die Schnittstelle beschreibt den funktionalen Vertrag einer Capability unabhängig von Provider, konkreter Implementierung, Prozessgrenze oder Ausführungsort.

## Grundprinzipien

```text
Interface ≠ Implementation
Interface ≠ Provider
Interface ≠ Authority
CapabilityID ≠ InterfaceVersion
Interface Compatibility ≠ Permission
Invocation ≠ Direct Provider Access
```

## Interface-Modell

Eine Capability-Schnittstelle beschreibt mindestens:

```text
CapabilityInterface
├── CapabilityID
├── InterfaceVersion
├── Operations[]
├── SemanticInput
├── SemanticOutput
├── ErrorModel
└── ExecutionSemantics
```

Optional:

```text
Streaming
Cancellation
Deadline
Determinism
ResourceRequirements
ConcurrencyModel
```

## Operationen

Eine Capability darf eine oder mehrere klar definierte Operationen bereitstellen:

```text
Capability
├── Operation A
├── Operation B
└── Operation C
```

Jede Operation beschreibt:

```text
Operation
├── Name
├── Input
├── Output
├── Errors
└── Constraints
```

## Semantische Typen

Ein- und Ausgaben sollen über stabile semantische Typen beschrieben werden:

```text
SemanticInput
      ↓
Capability Operation
      ↓
SemanticOutput
```

Die konkreten physischen Datenformate können davon unabhängig sein.

## Providerunabhängigkeit

Mehrere Provider dürfen dieselbe Schnittstelle implementieren:

```text
CapabilityID
     ↓
Interface
├── Provider A
├── Provider B
└── Provider C
```

Aufrufer sollen nicht von einer konkreten Provider-Implementierung abhängig sein.

## Aufruf

Ein Capability-Aufruf erfolgt über autorisierte Authority:

```text
Capability Handle
      ↓
Interface Operation
      ↓
Provider Resolution
      ↓
Implementation
      ↓
Result
```

Die Kenntnis der Schnittstelle oder `CapabilityID` reicht nicht zur Ausführung.

## Versionierung

Capability- und Interface-Version bleiben getrennt:

```text
CapabilityID
CapabilityVersion
InterfaceVersion
```

Kompatible Interface-Versionen dürfen parallel unterstützt werden.

Inkompatible Änderungen müssen als neue Interface-Version veröffentlicht werden.

## Fehler

Fehler müssen strukturiert und maschinenlesbar sein.

Mindestens unterscheidbar:

```text
InvalidInput
UnsupportedOperation
PermissionDenied
Unavailable
ResourceLimit
DeadlineExceeded
Cancelled
ProviderFailure
CompatibilityError
```

Provider-spezifische interne Fehler dürfen nicht unkontrolliert Teil des öffentlichen Capability-Vertrags werden.

## Asynchrone Ausführung

Capability-Interfaces müssen asynchrone Ausführung unterstützen können.

Je nach Operation können zusätzlich verwendet werden:

```text
Cancellation
Deadline
Streaming
Progress
Backpressure
```

## Sicherheit

Das Interface beschreibt mögliche Operationen, gewährt aber keine Authority.

```text
Interface
   +
Authorized Capability Handle
   +
Policy
   ↓
Allowed Operation
```

Jeder Aufruf muss innerhalb der Authority des verwendeten Capability-Handles bleiben.

## Normative Anforderungen

1. Jede ausführbare Capability MUSS eine definierte Schnittstelle besitzen.
2. Capability Interface und Implementierung MÜSSEN getrennt bleiben.
3. Mehrere Provider MÜSSEN dieselbe Schnittstelle implementieren können.
4. Operationen MÜSSEN klar definierte Ein- und Ausgaben besitzen.
5. Semantische Typen MÜSSEN für Ein- und Ausgaben verwendbar sein.
6. Capability- und Interface-Version MÜSSEN getrennt behandelt werden.
7. Inkompatible Interface-Änderungen MÜSSEN versioniert werden.
8. Fehler MÜSSEN strukturiert und providerunabhängig darstellbar sein.
9. Asynchrone Operationen MÜSSEN unterstützt werden können.
10. Cancellation, Deadline und Streaming MÜSSEN bei geeigneten Operationen ausdrückbar sein.
11. Kenntnis einer Schnittstelle DARF keine Authority erzeugen.
12. Capability-Aufrufe DÜRFEN die Authority des verwendeten Handles nicht überschreiten.
13. Interface, Version, Operationen und semantische Typen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt einen stabilen, versionierten und providerunabhängigen Schnittstellenvertrag für Capabilities. Aufrufer können dadurch dieselbe Capability über unterschiedliche Implementierungen verwenden, während semantische Typen, Fehler, Versionierung und Ausführungssemantik einheitlich definiert und tatsächliche Authority separat kontrolliert werden.