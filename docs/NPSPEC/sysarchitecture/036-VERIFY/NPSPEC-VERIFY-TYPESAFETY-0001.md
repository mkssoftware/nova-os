# NPSPEC-VERIFY-TYPESAFETY-0001 – Nova Type Safety Verification

## Status

Angenommen

## Kategorie

Verification / Type Safety / Verified Core

## Zweck

NovaOS definiert Type Safety als überprüfbare Eigenschaft, dass Daten, Objekte, Handles, Capabilities und Schnittstellen ausschließlich entsprechend ihrer definierten semantischen und binären Typregeln verwendet werden.

```text
Value / Object
      ↓
Type Information
      ↓
Validate Operation
      ↓
Allowed Type Operation
      ↓
Preserve Type Invariants
```

Type Safety ergänzt Memory Safety, Capability Safety und Runtime Contracts.

## Grundprinzipien

```text
Type Safety ≠ Memory Safety
Type Safety ≠ Capability Safety
Type Safety ≠ Semantic Validation
Type Compatible ≠ Authorized
Binary Compatible ≠ Semantically Compatible
Handle Type ≠ Object Identity
Successful Cast ≠ Valid Authority
```

## Type Model

```text
NovaType
├── TypeID
├── Version
├── Representation
├── ValidOperations
├── Invariants
└── CompatibilityRules
```

Optional:

```text
SemanticTypeID
ABIType
OwnershipRules
LifetimeRules
SecurityConstraints
ConversionRules
ProvenanceID
```

## Stable Type Identity

Systemweit relevante Typen sollen stabile IDs besitzen.

```text
TypeID ≠ Type Name
TypeID ≠ Memory Address
TypeID ≠ Implementation Type
```

Dadurch können Typen unabhängig von Sprache, Prozess und Implementierung eindeutig referenziert werden.

## Static Type Safety

Wo möglich sollen Fehler bereits vor Ausführung erkannt werden.

Beispiele:

```text
Invalid Assignment
Invalid Operation
Wrong Argument Type
Invalid Return Type
Invalid Handle Use
Invalid Ownership Transfer
```

Compiler und statische Analyse dürfen hierzu verwendet werden.

## Runtime Type Safety

Nicht vollständig statisch prüfbare Grenzen benötigen Runtime Validation.

Besonders relevant:

```text
Syscalls
IPC
FFI
Drivers
Plugins
Serialization
Remote Communication
Dynamic Loading
```

```text
External Input ≠ Trusted Type
```

## Handle Type Safety

Handles müssen ihrem erwarteten Objekttyp entsprechen.

```text
FileHandle
    ↓
File Operation
```

Ein Handle eines anderen Typs darf nicht allein aufgrund eines identischen numerischen Wertes akzeptiert werden.

```text
Handle Value ≠ Handle Type
```

## Capability Type Safety

Capabilities besitzen explizite Typ- und Authority-Semantik.

```text
Capability
├── CapabilityType
├── Target
├── Rights
└── Constraints
```

Eine Capability eines inkompatiblen Typs darf nicht für eine Operation verwendet werden.

Type Compatibility erzeugt jedoch keine Authority.

## Semantic Types

Nova Semantic Types werden getrennt von ihrer physischen Repräsentation betrachtet.

```text
Semantic Type
      ↓
Representation
      ↓
Implementation Type
```

Beispiel:

```text
Nova.Image
   ↓
JPEG / PNG / RAW
```

```text
Representation Compatibility ≠ Semantic Compatibility
```

## Type Conversion

Konvertierungen müssen explizit definiert sein.

```text
Type A
  ↓
Validated Conversion
  ↓
Type B
```

Konvertierungen können klassifiziert werden als:

```text
Lossless
Lossy
Checked
Narrowing
Widening
Representation-only
Semantic
Unsafe
```

Implizite unsichere Konvertierungen sollen vermieden werden.

## ABI Type Safety

Öffentliche ABI-Grenzen müssen stabile, eindeutig definierte Typen verwenden.

Beispiele:

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

Strukturen benötigen explizite:

```text
Size
Version
Alignment
Field Layout
```

Interne Compiler-Strukturen dürfen nicht unkontrolliert zu öffentlichen ABI-Typen werden.

## IPC Type Safety

Typed IPC muss Nachrichten gegen das erwartete Schema validieren.

```text
Message
   ↓
TypeID
   ↓
Schema Validation
   ↓
Operation
```

Ungültige oder unbekannte erforderliche Felder müssen kontrolliert behandelt werden.

## FFI Type Safety

FFI-Grenzen müssen Sprachtypen explizit auf Nova-ABI-Typen abbilden.

Zu prüfen sind insbesondere:

```text
Size
Alignment
Signedness
Pointer Representation
Ownership
Lifetime
String Encoding
Calling Convention
```

```text
Language Type ≠ ABI Type
```

## Memory Safety Integration

Type Safety kann Memory Safety unterstützen, ersetzt sie jedoch nicht.

```text
Type Safe
   +
Memory Safe
   +
Ownership Safe
```

Ein korrekt typisierter Pointer kann weiterhin auf ungültigen oder nicht autorisierten Speicher zeigen.

## Type Confusion

Der Verified Core muss sicherheitskritische Type-Confusion-Fehler verhindern.

```text
Object Type A
      ↓
Interpret as Type B
      ↓
Invalid Operation
```

Insbesondere Handles, Kernelobjekte, IPC-Nachrichten und Capabilities müssen gegen Type Confusion geschützt sein.

## Formal Verification

Kritische Typregeln sollen formal ausdrückbar sein.

Beispiel:

```text
Value : Type A

Operation requires Type A

→ Operation type-valid
```

Für inkompatible Typen:

```text
Value : Type B

B !compatible A

→ Operation rejected
```

## Type Preservation

Eine zentrale Eigenschaft lautet:

```text
WellTyped(State)
AND
ValidTransition(State, Operation, State')
→
WellTyped(State')
```

Gültige Systemtransitionen dürfen den wohldefinierten Typzustand nicht verletzen.

## Progress

Für formal betrachtete Komponenten soll gelten:

```text
Well-Typed Operation
→
Valid Result
OR
Defined Error
OR
Defined Wait State
```

Ein gültiger Typzustand darf nicht aufgrund einer undefinierten Typoperation in einen unkontrollierten Zustand wechseln.

## Unsafe Boundaries

Operationen, die das Typsystem bewusst umgehen, müssen explizit markiert und begrenzt werden.

Beispiele:

```text
Raw Pointer Cast
Hardware Register Mapping
Packed Structures
FFI Cast
Low-Level Boot Structures
```

```text
Unsafe Cast ≠ Trusted Cast
```

Diese Grenzen benötigen zusätzliche Validierung.

## Versioning

Typen müssen evolvierbar sein.

```text
Type v1
   ↓
Compatible Extension
   ↓
Type v2
```

Inkompatible Änderungen benötigen explizite neue Versionen oder Konvertierungsregeln.

## Failure Handling

Type-Safety-Verletzungen müssen kontrolliert behandelt werden.

Mögliche Reaktionen:

```text
Reject Input
Return Type Error
Terminate Operation
Isolate Component
Restart Service
Enter Recovery
```

Untrusted Input darf keine Kernel-Type-Confusion verursachen.

## Verification Artifacts

Type-Safety-Verifikation soll referenzieren:

```text
SpecificationID
TypeModelVersion
Component
SourceVersion
BuildID
VerifiedProperties
Assumptions
Tool
VerificationResult
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TypeID
Type Version
Representation
Compatibility
Conversions
Invariants
Unsafe Boundaries
Verification Status
```

## Normative Anforderungen

1. NovaOS MUSS Type Safety als Eigenschaft des Verified Core behandeln.
2. Systemweit relevante Typen SOLLEN stabile TypeIDs besitzen.
3. Type Identity MUSS von Namen und Speicheradressen unabhängig sein können.
4. Kritische Typregeln MÜSSEN formal spezifizierbar sein.
5. Type Confusion MUSS im Verified Core verhindert werden.
6. Handle Types MÜSSEN vor sicherheitskritischer Verwendung validiert werden.
7. Capability Types MÜSSEN explizit überprüfbar sein.
8. Type Compatibility DARF NICHT als Authority interpretiert werden.
9. Semantic Type und Representation Type MÜSSEN getrennt behandelbar sein.
10. Unsichere Typkonvertierungen MÜSSEN explizit erkennbar sein.
11. Öffentliche ABI-Typen MÜSSEN eindeutig definiert sein.
12. Typed IPC MUSS eingehende Typinformationen validieren können.
13. FFI-Grenzen MÜSSEN explizite Type Mappings besitzen.
14. Type Safety DARF NICHT mit Memory Safety gleichgesetzt werden.
15. Kritische State Transitions SOLLEN Type Preservation gewährleisten.
16. Wohldefinierte Typoperationen SOLLEN nur definierte Resultate, Fehler oder Wartezustände erzeugen.
17. Unsafe Boundaries MÜSSEN explizit identifizierbar sein.
18. Unsafe Type Operations SOLLEN auf den kleinsten notwendigen Bereich begrenzt werden.
19. Typversionierung MUSS kontrollierte Evolution unterstützen.
20. Inkompatible Typänderungen MÜSSEN explizit behandelt werden.
21. Untrusted Input DARF keine unkontrollierte Type Confusion im Kernel verursachen.
22. Type-Safety-Verifikation MUSS mit konkreten Implementierungsversionen verknüpfbar sein.
23. Type-Safety-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-VERIFY-MEMORYSAFETY-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ABI-VERSIONING-0001`
- `NPSPEC-FFI-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `ADR-VERIFY-0004`

## Ergebnis

```text
Input / Object / Handle
          ↓
Identify Type
          ↓
Validate Type + Version
          ↓
Validate Compatibility
          ↓
Validate Authority Separately
          ↓
Execute Typed Operation
          ↓
Preserve Type Invariants
          ↓
Verified Result
```

NovaOS erhält damit ein systemweites Type-Safety-Modell, das Type Confusion verhindert, Sprach-, ABI-, IPC-, FFI- und semantische Typgrenzen kontrolliert und sicherstellt, dass kritische Systemtransitionen definierte Typinvarianten erhalten.