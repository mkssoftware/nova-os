# NPSPEC-ABI-NOVA-0001 – Nova Native ABI

## Status

Angenommen

## Kategorie

ABI / System Architecture / Binary Interface

## Zweck

NovaOS definiert mit der Nova Native ABI eine stabile binäre Schnittstelle zwischen Kernel, Systemkomponenten, Runtime, Services, Treibern und nativen Programmen.

```text
Application / Service / Driver
            ↓
        Nova ABI
            ↓
Kernel / Runtime / System Services
```

Die ABI definiert die binären Regeln der Kommunikation unabhängig von Programmiersprache und konkreter Compiler-Implementierung.

## Grundprinzipien

```text
ABI ≠ API
ABI ≠ Syscall Interface
ABI ≠ Programming Language
ABI ≠ IPC Protocol
ABI Stability ≠ Implementation Freeze
Binary Compatibility ≠ Semantic Compatibility
```

NovaOS darf interne Implementierungen verändern, solange die zugesagte ABI-Kompatibilität erhalten bleibt.

## ABI Model

```text
NovaABI
├── ABI_ID
├── Architecture
├── ABI_Version
├── CallingConvention
├── DataLayout
├── RegisterRules
├── StackRules
└── CompatibilityProfile
```

## ABI Identification

Jede ausführbare NovaOS-Komponente muss ihre erwartete ABI eindeutig bestimmen können.

```text
Nova ABI
├── Architecture
├── Major Version
├── Minor Version
└── Feature Set
```

Beispiel:

```text
Nova ABI 1.0
Architecture: x86-32
```

## Architecture Profiles

Die Nova ABI wird architekturspezifische Profile besitzen.

```text
Nova ABI
├── x86-32
├── x86-64
├── ARM64
└── Future Architectures
```

Die semantischen Systemkonzepte sollen architekturübergreifend möglichst identisch bleiben.

Register-, Stack- und Alignment-Regeln dürfen architekturspezifisch sein.

## Calling Convention

Die ABI definiert eindeutig:

```text
Argument Passing
Return Values
Register Usage
Caller-saved Registers
Callee-saved Registers
Stack Alignment
Stack Cleanup
Structure Passing
Error Return
```

Damit können Komponenten unterschiedlicher Compiler und Sprachen miteinander interagieren.

## Data Layout

Die ABI definiert binäre Repräsentationen grundlegender Datentypen.

```text
Integer Width
Pointer Width
Alignment
Structure Layout
Padding
Boolean Representation
Handle Representation
Identifier Representation
```

Architekturabhängige Größen müssen explizit gekennzeichnet werden.

Persistente oder IPC-relevante Formate dürfen nicht unkontrolliert von nativen Struct-Layouts abhängen.

## Stable Types

ABI-Grenzen sollen bevorzugt explizite Typgrößen verwenden.

```text
u8
u16
u32
u64

i8
i16
i32
i64
```

Unspezifizierte compilerabhängige Größen sollen an stabilen ABI-Grenzen vermieden werden.

## Handles

Kernel- und Systemobjekte werden nicht als interne Pointer exportiert.

```text
Application
    ↓
Stable Handle
    ↓
Kernel Object
```

```text
Handle ≠ Pointer
Handle ≠ Capability
Handle Knowledge ≠ Authority
```

Capability-basierte Authority bleibt ein separates Sicherheitskonzept.

## Object Identity

Stabile NovaOS-Identitäten bleiben von Speicheradressen und Handles getrennt.

```text
ObjectID ≠ Handle
ResourceID ≠ Pointer
CapabilityID ≠ Handle
```

Dadurch können Objekte ihre Identität über Prozess-, Provider- oder Standortwechsel hinweg behalten.

## ABI Structures

Öffentliche ABI-Strukturen müssen evolutionstauglich sein.

Bevorzugtes Muster:

```text
struct NovaExample {
    uint32_t size;
    uint32_t version;

    ...
};
```

Dadurch können neue Felder ergänzt werden, ohne bestehende Consumer unnötig zu brechen.

## Versioning

Nova ABI verwendet explizite Versionierung.

```text
Major Version
→ inkompatible ABI-Änderung möglich

Minor Version
→ kompatible Erweiterung
```

Neue Funktionalität soll bevorzugt über Erweiterungen statt Änderung bestehender Semantik eingeführt werden.

## Feature Discovery

Programme dürfen ABI-Funktionen nicht allein aus einer Versionsnummer ableiten müssen.

```text
Query ABI
   ↓
Supported Features
   ↓
Use Feature
```

Feature Discovery ermöglicht unterschiedliche Kernel- und Systemkonfigurationen.

## Syscall Integration

Die Nova ABI definiert die binäre Übergabe an die Syscall-Schicht.

```text
Userspace
   ↓
Nova ABI
   ↓
Syscall Boundary
   ↓
Kernel
```

Die interne Kernel-Implementierung bleibt außerhalb des ABI-Vertrags.

Syscall IDs müssen stabil und versionierbar sein.

## IPC Integration

Komplexe Systeminteraktionen sollen nicht zwangsläufig durch immer größere Syscall-Schnittstellen entstehen.

```text
Nova ABI
├── Primitive Syscalls
├── Handles
├── Shared Buffers
└── Typed IPC
```

Höhere Systemfunktionen können über typed IPC und Capabilities bereitgestellt werden.

## Error Model

ABI-Funktionen müssen Fehler eindeutig und maschinenlesbar zurückgeben.

Beispiel:

```text
NOVA_OK
NOVA_INVALID_ARGUMENT
NOVA_ACCESS_DENIED
NOVA_NOT_FOUND
NOVA_UNSUPPORTED
NOVA_RESOURCE_LIMIT
NOVA_TIMEOUT
NOVA_CANCELLED
NOVA_UNKNOWN_STATE
```

Fehlercodes dürfen nicht von lokalisierten Textmeldungen abhängen.

## Memory Ownership

ABI-Grenzen müssen Ownership explizit definieren.

```text
Caller Owned
Callee Owned
Shared
Borrowed
Transferred
```

Unklare Ownership über ABI-Grenzen ist unzulässig.

## Zero-Copy

Die ABI muss kontrollierte Zero-Copy-Datenübergabe unterstützen können.

```text
Buffer
  ↓
Mapping / Shared Handle
  ↓
Consumer
```

Bei ungeeigneten Bedingungen muss ein sicherer Copy-Pfad verfügbar bleiben.

## Asynchronität

Native ABI-Schnittstellen sollen asynchrone Operationen unterstützen können.

```text
Submit
  ↓
OperationID
  ↓
Completion / Event
```

Dadurch werden Threads nicht unnötig durch langsame IO- oder Serviceoperationen blockiert.

## Cancellation

Asynchrone ABI-Operationen sollen Cancellation unterstützen können.

```text
OperationID
    ↓
Cancel
```

```text
Cancellation Requested ≠ Operation Cancelled
```

Der endgültige Zustand muss eindeutig beobachtbar sein.

## Security

Die ABI darf Sicherheitsgrenzen nicht umgehen.

```text
ABI Call
   ↓
Handle / Capability Validation
   ↓
Authorized Operation
```

ABI-Kompatibilität darf niemals Vorrang vor Capability-, Security-, Trust- oder Isolation-Regeln erhalten.

## Kernel Isolation

Interne Kernel-Strukturen dürfen nicht direkt Teil der öffentlichen ABI werden.

```text
Public ABI Structure
        ≠
Internal Kernel Structure
```

Dadurch kann NovaOS interne Kernel-Komponenten verändern, ohne Programme neu kompilieren zu müssen.

## Driver ABI

Treiber können definierte Nova-ABI-Profile verwenden.

```text
Driver
  ↓
Driver ABI
  ↓
Kernel Driver Framework
```

Kernelmode- und Usermode-Treiber dürfen unterschiedliche ABI-Profile besitzen.

## Language Interoperability

Die Nova ABI soll sprachneutral sein.

```text
C
C++
Rust
NovaLang
Other Languages
      ↓
    Nova ABI
```

Sprachspezifische Runtime-Objekte dürfen nicht ohne stabile Repräsentation über ABI-Grenzen übertragen werden.

## Compatibility

NovaOS soll ältere unterstützte ABI-Versionen über definierte Compatibility Layer weiter ausführen können.

```text
Old Binary
    ↓
Compatibility Layer
    ↓
Current Nova ABI
```

Compatibility Layer dürfen Sicherheitsregeln nicht abschwächen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ABI Version
Architecture
Supported Features
Compatibility Profile
Calling Convention
Available Interfaces
Deprecated Interfaces
```

## Normative Anforderungen

1. NovaOS MUSS eine explizit definierte native ABI besitzen.
2. ABI und API MÜSSEN getrennte Konzepte bleiben.
3. Die ABI MUSS architekturspezifische Profile unterstützen.
4. Calling Convention MUSS pro Architektur eindeutig definiert sein.
5. Register-, Stack- und Alignment-Regeln MÜSSEN spezifiziert werden.
6. Öffentliche ABI-Datentypen MÜSSEN stabile binäre Repräsentationen besitzen.
7. Interne Kernel-Pointer DÜRFEN NICHT als öffentliche Objektidentitäten verwendet werden.
8. Object IDs, Handles und Capabilities MÜSSEN getrennte Konzepte bleiben.
9. Öffentliche ABI-Strukturen SOLLEN versionierbar und erweiterbar sein.
10. Inkompatible Änderungen MÜSSEN über Major-Versionen kenntlich gemacht werden.
11. Kompatible Erweiterungen SOLLEN bestehende Binärprogramme nicht brechen.
12. ABI Features MÜSSEN introspektierbar sein können.
13. Syscall IDs MÜSSEN stabil und versionierbar sein.
14. Memory Ownership MUSS an ABI-Grenzen eindeutig definiert sein.
15. Zero-Copy MUSS einen sicheren Fallback unterstützen können.
16. Asynchrone Operationen SOLLEN stabile Operation IDs verwenden.
17. Cancellation DARF NICHT automatisch erfolgreichen Abbruch bedeuten.
18. ABI-Kompatibilität DARF Security- und Capability-Regeln NICHT umgehen.
19. Interne Kernel-Strukturen DÜRFEN NICHT unnötig Bestandteil der öffentlichen ABI sein.
20. Die Nova ABI SOLL sprachneutral sein.
21. Compatibility Layer DÜRFEN Sicherheitsgarantien NICHT reduzieren.
22. ABI-Version und unterstützte Features MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `ADR-ARCH-0141`

## Ergebnis

```text
Native Program
      ↓
Stable Nova ABI
      ↓
Handles + Types + Calling Convention
      ↓
Syscalls / IPC / Runtime
      ↓
NovaOS Implementation
```

NovaOS erhält damit eine stabile, versionierbare und sprachneutrale native ABI, die Binärkompatibilität von der internen Implementierung entkoppelt und gleichzeitig Handles, Capabilities, Zero-Copy, asynchrone Ausführung und zukünftige Architekturen unterstützt.