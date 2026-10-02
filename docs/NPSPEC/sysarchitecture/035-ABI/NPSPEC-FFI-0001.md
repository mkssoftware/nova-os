# NPSPEC-FFI-0001 – Nova Foreign Function Interface

## Status

Angenommen

## Kategorie

FFI / Interoperability / Runtime

## Zweck

NovaOS definiert ein einheitliches Foreign Function Interface (FFI), über das Komponenten unterschiedlicher Programmiersprachen kontrolliert miteinander interagieren können.

```text
NovaLang
C
C++
Rust
Other Languages
      ↓
    Nova FFI
      ↓
   Nova ABI
```

Das FFI bildet sprachspezifische Typen, Aufrufkonventionen und Ownership-Regeln auf stabile NovaOS-Schnittstellen ab.

## Grundprinzipien

```text
FFI ≠ ABI
FFI ≠ API
FFI ≠ Serialization
FFI ≠ IPC
Language Object ≠ ABI Object
Pointer ≠ Handle
FFI Access ≠ Authority
FFI Compatible ≠ Memory Safe
```

Die Nova ABI bleibt die grundlegende binäre Basis. Das FFI definiert darauf die sichere Sprachinteroperabilität.

## FFI Model

```text
FFIContract
├── FFI_ID
├── LanguageProfile
├── ABIProfile
├── CallingConvention
├── TypeMappings
├── OwnershipRules
└── ErrorMapping
```

Optional:

```text
RuntimeRequirements
SafetyProfile
AsyncModel
CallbackModel
ExceptionPolicy
CapabilityRequirements
Version
```

## Language Profiles

NovaOS kann Profile definieren für:

```text
C
C++
Rust
NovaLang
Future Languages
```

Ein Profil beschreibt nur sprachspezifische Interoperabilität und verändert nicht die zugrunde liegende Nova ABI.

## Type Mapping

FFI-Typen müssen eindeutig auf ABI-Typen abgebildet werden.

```text
Language Type
     ↓
FFI Type
     ↓
ABI Representation
```

Beispiele:

```text
u32       → nova_u32
i64       → nova_i64
bool      → nova_bool
Handle    → nova_handle
String    → nova_string_view
Buffer    → nova_buffer
```

Compilerabhängige Datentypen dürfen nicht ungeprüft über FFI-Grenzen übertragen werden.

## Structures

Komplexe Strukturen müssen ein definiertes Layout besitzen.

```text
FFI Structure
├── Size
├── Version
├── Alignment
└── Fields
```

Sprachspezifische Objektlayouts dürfen nicht als stabile FFI-Strukturen vorausgesetzt werden.

## Strings

Strings benötigen eine explizite Repräsentation.

Beispiel:

```text
NovaStringView
├── Data
├── Length
└── Encoding
```

Nullterminierung darf nicht vorausgesetzt werden, sofern der jeweilige Contract sie nicht explizit verlangt.

## Memory Ownership

Ownership muss über FFI-Grenzen eindeutig sein:

```text
Owned
Borrowed
Shared
Transferred
```

Zusätzlich muss definiert werden, welche Seite Speicher freigeben darf.

```text
Allocator A allocates
        ↓
Defined Owner
        ↓
Defined Deallocator
```

Speicher darf nicht über einen inkompatiblen Allocator freigegeben werden.

## Handles

Systemobjekte sollen über Handles statt über interne Objektpointer übertragen werden.

```text
Language Runtime
      ↓
Nova Handle
      ↓
NovaOS Object
```

```text
Handle ≠ Raw Kernel Pointer
```

## Calling Convention

Jedes FFI-Profil muss die verwendete Calling Convention eindeutig festlegen.

Dazu gehören:

```text
Arguments
Return Values
Registers
Stack
Alignment
Structure Passing
Callbacks
```

Die Calling Convention muss mit dem verwendeten Nova-ABI-Profil kompatibel sein.

## Error Handling

Sprachspezifische Fehlermechanismen müssen auf ein stabiles FFI-Modell abgebildet werden.

```text
Nova Result
├── Status
└── Value
```

Beispiele:

```text
C      → Error Code
Rust   → Result Mapping
C++    → Error Mapping
NovaLang → Native Result
```

Exceptions dürfen ABI-/FFI-Grenzen nicht unkontrolliert überschreiten.

## Panic und Exceptions

Ein sprachspezifischer:

```text
Panic
Exception
Unwind
```

darf nicht automatisch durch fremden Code propagieren.

Das jeweilige FFI-Profil muss definieren:

```text
Catch
Translate
Terminate
or
Explicit Propagation Contract
```

## Callbacks

FFI muss kontrollierte Callbacks unterstützen können.

```text
Language A
    ↓
Function Reference
    ↓
Language B
    ↓
Callback
```

Dabei müssen mindestens:

```text
Lifetime
Thread Context
Calling Convention
Ownership
Cancellation
```

definiert sein.

## Asynchronität

Asynchrone Sprachmodelle werden nicht direkt vorausgesetzt.

```text
Async Language Operation
        ↓
FFI OperationID
        ↓
Nova Async Operation
        ↓
Completion
```

Dadurch können unterschiedliche Modelle wie Futures, Promises oder Tasks auf dieselbe NovaOS-Semantik abgebildet werden.

## Threading

FFI Contracts müssen relevante Threading-Regeln definieren.

```text
Thread-safe
Single-threaded
Thread-affine
Reentrant
Serialized
```

Ein Callback darf nicht automatisch auf dem ursprünglichen Thread erwartet werden.

## Zero-Copy

FFI kann Shared Buffers verwenden.

```text
Language A Buffer
       ↓
Validated Shared Buffer
       ↓
Language B
```

Ownership, Lifetime und Mutability müssen dabei eindeutig sein.

Ein sicherer Copy-Fallback muss möglich bleiben.

## Capability Security

FFI erzeugt keine Authority.

```text
Foreign Function Available
          ≠
Operation Authorized
```

Handles und Capabilities müssen entsprechend der normalen NovaOS-Regeln validiert werden.

## Unsafe Boundary

FFI-Aufrufe können eine explizite Unsafe Boundary darstellen.

```text
Safe Language
     ↓
FFI Boundary
     ↓
Foreign Code
```

Sprachen mit Memory-Safety-Garantien dürfen fremden Code nicht automatisch als sicher betrachten.

Wrapper können sichere Abstraktionen über validierten FFI-Schnittstellen bereitstellen.

## Runtime Isolation

Sprach-Runtimes dürfen ihre internen Zustände nicht unkontrolliert teilen.

Beispiele:

```text
Garbage Collector State
Exception Runtime
Thread Local Storage
Runtime Allocator
Language Metadata
```

Diese bleiben grundsätzlich innerhalb ihrer Runtime-Grenze.

## API Integration

Semantic APIs können sprachspezifische FFI Bindings erhalten.

```text
Semantic API
      ↓
API Contract
      ↓
FFI Binding
├── C
├── Rust
├── C++
└── NovaLang
```

Der FFI Wrapper darf die API-Semantik nicht verändern.

## Versioning

FFI Contracts müssen versionierbar sein.

```text
FFI_ID
+
Version
+
ABI Profile
```

Änderungen an:

```text
Calling Convention
Type Layout
Ownership
Error Semantics
Callback Semantics
```

können inkompatible Änderungen darstellen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
FFI_ID
Version
Language Profile
ABI Profile
Supported Types
Calling Convention
Safety Profile
Runtime Requirements
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches FFI-Modell unterstützen.
2. FFI und ABI MÜSSEN getrennte Konzepte bleiben.
3. FFI-Profile MÜSSEN auf definierte Nova-ABI-Profile abgebildet werden.
4. Sprachspezifische Typen MÜSSEN explizit auf stabile ABI-Typen abgebildet werden.
5. Compilerabhängige Objektlayouts DÜRFEN NICHT ungeprüft über FFI-Grenzen verwendet werden.
6. String-Repräsentation MUSS eindeutig definiert sein.
7. Memory Ownership MUSS an jeder relevanten FFI-Grenze eindeutig sein.
8. Allocator und Deallocator MÜSSEN kompatibel sein.
9. Systemobjekte SOLLEN über Handles statt interne Pointer übertragen werden.
10. Calling Convention MUSS eindeutig definiert sein.
11. Fehler MÜSSEN zwischen Sprachmodellen kontrolliert übersetzbar sein.
12. Exceptions und Panics DÜRFEN FFI-Grenzen NICHT unkontrolliert überschreiten.
13. Callbacks MÜSSEN Lifetime und Calling Convention definieren.
14. Asynchrone Operationen SOLLEN über sprachneutrale Operation IDs abbildbar sein.
15. Threading- und Reentrancy-Semantik MUSS definierbar sein.
16. Zero-Copy MUSS Ownership, Lifetime und Mutability berücksichtigen.
17. Ein sicherer Copy-Fallback MUSS verfügbar sein können.
18. FFI DARF keine Authority erzeugen oder Capability-Prüfungen umgehen.
19. Memory-safe Sprachen DÜRFEN Foreign Code NICHT automatisch als memory-safe betrachten.
20. Runtime-interne Zustände DÜRFEN NICHT ohne expliziten Contract geteilt werden.
21. FFI Bindings DÜRFEN die zugrunde liegende API-Semantik NICHT verändern.
22. FFI Contracts MÜSSEN versionierbar sein.
23. Inkompatible FFI-Änderungen MÜSSEN eindeutig erkennbar sein.
24. FFI-Profile MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ABI-STABLE-0001`
- `NPSPEC-ABI-INTERNAL-0001`
- `NPSPEC-ABI-VERSIONING-0001`
- `NPSPEC-API-CONTRACT-0001`
- `NPSPEC-API-SEMANTIC-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0152`

## Ergebnis

```text
Language A
    ↓
Language Binding
    ↓
Nova FFI Contract
    ↓
Stable Nova ABI
    ↓
Language Binding
    ↓
Language B
```

NovaOS erhält damit eine sprachneutrale FFI-Schicht, über die C, C++, Rust, NovaLang und zukünftige Sprachen kontrolliert zusammenarbeiten können, während Typen, Ownership, Fehler, Asynchronität, Memory Safety und Capability-Sicherheit an Sprachgrenzen explizit definiert bleiben.