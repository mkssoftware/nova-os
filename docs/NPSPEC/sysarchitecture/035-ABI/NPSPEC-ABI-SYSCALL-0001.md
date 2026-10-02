# NPSPEC-ABI-SYSCALL-0001 – Nova Syscall ABI

## Status

Angenommen

## Kategorie

ABI / Syscall / Kernel Interface

## Zweck

NovaOS definiert eine stabile Syscall ABI als kontrollierte binäre Grenze zwischen Userspace und Kernel.

```text
Userspace
    ↓
Nova Syscall ABI
    ↓
Kernel Entry
    ↓
Validation
    ↓
Kernel Mechanism
    ↓
Result
```

Die Syscall ABI stellt grundlegende Kernelmechanismen bereit, ohne interne Kernelstrukturen oder höherwertige Systemdienste unnötig an die ABI zu binden.

## Grundprinzipien

```text
Syscall ABI ≠ System API
Syscall ≠ IPC
Syscall ID ≠ Function Address
Handle ≠ Pointer
Syscall Access ≠ Authority
Kernel Entry ≠ Successful Operation
Stable ABI ≠ Frozen Kernel
```

## Syscall Model

```text
NovaSyscall
├── SyscallID
├── ABIVersion
├── Arguments
├── Result
├── Error
└── Flags
```

Optional:

```text
OperationID
ExecutionContractID
Deadline
CancellationID
CapabilityHandle
BufferDescriptor
```

## Syscall Table

Syscalls besitzen stabile numerische IDs.

```text
SyscallID
   ↓
Dispatch Table
   ↓
Kernel Handler
```

Bestehende IDs dürfen innerhalb einer kompatiblen ABI-Version nicht mit anderer Semantik wiederverwendet werden.

Entfernte Syscalls bleiben reserviert.

## Syscall Kategorien

Die Syscall-Schicht soll primär grundlegende Mechanismen bereitstellen:

```text
Process / Task
Memory
IPC
Handles
Capabilities
Synchronization
Time
I/O Submission
Events
Introspection
```

Höhere Funktionen sollen bevorzugt über Services und Typed IPC bereitgestellt werden.

```text
Application
    ↓
System Service
    ↓
Typed IPC
    ↓
Primitive Syscalls
```

## Entry Mechanism

Der konkrete Eintritt in den Kernel ist architekturabhängig.

Beispiele:

```text
x86-32 → SYSENTER / INT
x86-64 → SYSCALL
ARM64  → SVC
```

Die semantische Syscall-Schnittstelle soll davon unabhängig bleiben.

## Calling Convention

Jedes Architekturprofil definiert:

```text
Syscall ID Register
Argument Registers
Return Register
Error Representation
Preserved Registers
Clobbered Registers
Stack Requirements
```

Für Argumente, die nicht effizient direkt über Register übertragen werden können, dürfen versionierte Argumentstrukturen verwendet werden.

## Argument Structures

Komplexe Syscalls sollen evolutionstaugliche Strukturen verwenden.

```text
struct nova_syscall_args {
    uint32_t size;
    uint32_t version;
    ...
};
```

Der Kernel darf niemals ungeprüft Userspace-Strukturen dereferenzieren.

## Pointer Validation

Userspace-Pointer sind grundsätzlich nicht vertrauenswürdig.

```text
Userspace Pointer
      ↓
Validate Range
      ↓
Validate Access
      ↓
Copy / Map
      ↓
Kernel Operation
```

```text
Valid Address ≠ Authorized Access
```

## Handles

Kernelobjekte werden über kontrollierte Handles referenziert.

```text
Userspace Handle
      ↓
Handle Validation
      ↓
Object Reference
```

Interne Kerneladressen dürfen nicht als öffentliche Handles verwendet werden.

## Capability Integration

Eine gültige Syscall-Nummer verleiht keine Berechtigung.

```text
Syscall
   +
Handle
   +
Capability
   ↓
Authorized Kernel Operation
```

Authority muss für die konkrete Operation geprüft werden.

## Return Model

Ein Syscall liefert eindeutig:

```text
Success
Result
Error
```

Fehler müssen maschinenlesbar sein.

Beispiele:

```text
NOVA_OK
NOVA_INVALID_ARGUMENT
NOVA_INVALID_HANDLE
NOVA_ACCESS_DENIED
NOVA_NOT_SUPPORTED
NOVA_RESOURCE_LIMIT
NOVA_TIMEOUT
NOVA_CANCELLED
NOVA_UNKNOWN_STATE
```

## Partial Results

Operationen können teilweise abgeschlossen sein.

Beispiel:

```text
Requested: 4096 Bytes
Completed: 2048 Bytes
```

Die ABI muss solche Zustände eindeutig ausdrücken können.

## Asynchronous Operations

Langlaufende Operationen sollen nicht zwingend einen Kernelthread blockieren.

```text
Syscall Submit
      ↓
OperationID
      ↓
Kernel / Driver / Service
      ↓
Completion
```

Completion kann beispielsweise über:

```text
Event
Completion Queue
IPC
Wait Object
```

signalisiert werden.

## Cancellation

Asynchrone Operationen können Cancellation unterstützen.

```text
Cancel(OperationID)
```

Dabei gilt:

```text
Cancellation Requested ≠ Cancelled
```

Die Operation kann bereits abgeschlossen oder irreversibel geworden sein.

## Deadline

Syscalls können eine Deadline aus dem Execution Contract übernehmen.

```text
Operation
   ↓
Deadline
   ↓
Complete / Timeout / Unknown
```

```text
Timeout ≠ Operation Not Executed
```

## Zero-Copy

Für große Datenmengen soll die ABI kontrollierte Shared- oder Mapped-Buffer unterstützen.

```text
Userspace Buffer
      ↓
Validated Mapping
      ↓
Kernel / Driver
```

Wenn Zero-Copy nicht sicher oder sinnvoll möglich ist, muss ein Copy-Pfad verfügbar bleiben.

## Blocking

Syscalls müssen ihre Blocking-Semantik eindeutig definieren.

```text
Nonblocking
Potentially Blocking
Asynchronous
Realtime-safe
```

Hard-Realtime-Code darf nicht unkontrolliert auf unbeschränkte Kerneloperationen warten.

## Reentrancy

Syscall-Handler müssen mit Preemption, Interrupts und parallelen Aufrufen korrekt umgehen.

Userspace-Zustand darf nicht als vertrauenswürdiger Kernelzustand behandelt werden.

## Versioning

Die Syscall ABI ist Bestandteil der Nova Native ABI.

```text
Nova ABI Version
      ↓
Syscall ABI Profile
```

Kompatible Erweiterungen dürfen neue Syscalls hinzufügen.

Bestehende Syscall-Semantik darf innerhalb einer kompatiblen ABI nicht stillschweigend verändert werden.

## Feature Discovery

Userspace soll verfügbare Syscalls und Features abfragen können.

```text
Query Feature
     ↓
Supported?
├── Yes → Use
└── No  → Fallback
```

Programme dürfen nicht voraussetzen, dass jede mögliche NovaOS-Konfiguration alle optionalen Kernelmechanismen bereitstellt.

## Compatibility

Ältere Syscall-ABIs können über kontrollierte Compatibility Layer unterstützt werden.

```text
Legacy Binary
     ↓
Compatibility ABI
     ↓
Current Kernel Mechanism
```

Compatibility darf keine Sicherheitsprüfung umgehen.

## Security Boundary

Der Syscall Entry ist eine zentrale Trust Boundary.

Jeder Syscall muss mindestens relevante Eingaben prüfen:

```text
Syscall ID
Arguments
Pointers
Handles
Capabilities
Sizes
Ranges
Flags
Resource Limits
State
```

Unbekannte Flags müssen entsprechend der ABI-Regel abgelehnt oder explizit ignoriert werden.

## Resource Accounting

Kerneloperationen müssen dem verursachenden Resource Domain zugeordnet werden können.

```text
Syscall
   ↓
Resource Accounting
├── CPU
├── Memory
├── IO
└── Kernel Resources
```

Syscalls dürfen keine unbegrenzten Kernelressourcen erzeugen.

## Determinismus

Deterministische Modi müssen relevante Syscall-Ergebnisse kontrollieren oder aufzeichnen können.

```text
Syscall
   ↓
External / Nondeterministic Result
   ↓
Record
   ↓
Replay
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Syscall ABI Version
Supported Syscalls
Supported Features
Deprecated Syscalls
Architecture Profile
Calling Convention
```

Geheime Argumente oder Capability Tokens dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS eine stabile Syscall ABI zwischen Userspace und Kernel definieren.
2. Syscall ABI und System API MÜSSEN getrennte Konzepte bleiben.
3. Syscalls SOLLEN primär grundlegende Kernelmechanismen bereitstellen.
4. Syscall IDs MÜSSEN stabil und versionierbar sein.
5. Entfernte Syscall IDs DÜRFEN NICHT mit inkompatibler Semantik wiederverwendet werden.
6. Architekturprofile MÜSSEN Calling Convention und Entry Mechanism definieren.
7. Userspace-Pointer MÜSSEN vor Verwendung validiert werden.
8. Interne Kernel-Pointer DÜRFEN NICHT als öffentliche Handles verwendet werden.
9. Syscall Access DARF NICHT als Authority interpretiert werden.
10. Capability- und Handle-Rechte MÜSSEN vor geschützten Operationen geprüft werden.
11. Fehler MÜSSEN maschinenlesbar zurückgegeben werden.
12. Partielle Ergebnisse MÜSSEN eindeutig darstellbar sein.
13. Langlaufende Operationen SOLLEN asynchron ausführbar sein.
14. Asynchrone Operationen SOLLEN stabile Operation IDs verwenden.
15. Cancellation DARF NICHT automatisch als erfolgreicher Abbruch gelten.
16. Timeout DARF NICHT automatisch bedeuten, dass eine Operation nicht ausgeführt wurde.
17. Zero-Copy MUSS einen sicheren Copy-Fallback besitzen können.
18. Blocking-Semantik MUSS für relevante Syscalls definiert sein.
19. Syscall-Handler MÜSSEN alle Userspace-Eingaben als nicht vertrauenswürdig behandeln.
20. Kompatible ABI-Versionen DÜRFEN bestehende Syscall-Semantik NICHT stillschweigend verändern.
21. Optionale Syscall-Features MÜSSEN introspektierbar sein können.
22. Compatibility Layer DÜRFEN Security- oder Capability-Prüfungen NICHT umgehen.
23. Syscall-Ressourcennutzung MUSS dem verursachenden Kontext zurechenbar sein.
24. Syscall-Zustände und ABI-Fähigkeiten MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-0001`
- `NPSPEC-REALTIME-REPLAY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0142`

## Ergebnis

```text
Userspace
    ↓
Syscall ID + Arguments
    ↓
Kernel Entry
    ↓
Validate Input
    ↓
Validate Handle + Capability
    ↓
Account Resources
    ↓
Execute Kernel Mechanism
    ↓
Result / Async OperationID
    ↓
Userspace
```

NovaOS erhält damit eine stabile, versionierbare und sicherheitsorientierte Syscall ABI, die eine minimale kontrollierte Grenze zwischen Userspace und Kernel bildet, ohne höhere Systemdienste oder interne Kernelimplementierungen unnötig an die binäre Schnittstelle zu koppeln.