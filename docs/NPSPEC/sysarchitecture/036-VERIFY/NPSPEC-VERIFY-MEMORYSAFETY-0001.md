# NPSPEC-VERIFY-MEMORYSAFETY-0001 – Nova Memory Safety Verification

## Status

Angenommen

## Kategorie

Verification / Memory Safety / Verified Core

## Zweck

NovaOS definiert formale und ergänzende technische Verfahren zur Verifikation sicherheitskritischer Speichereigenschaften.

Ziel ist insbesondere der Nachweis, dass kritische Komponenten keine unzulässigen Speicherzugriffe durchführen können.

```text
Memory Model
     ↓
Safety Properties
     ↓
Formal Verification
     ↓
Implementation Mapping
     ↓
Runtime Protection
```

## Grundprinzipien

```text
Memory Safety ≠ Memory Isolation
Memory Safety ≠ Type Safety
Memory Safety ≠ Bounds Checking Only
Valid Pointer ≠ Authorized Access
Mapped Memory ≠ Owned Memory
Memory Safe ≠ Race Free
Verified Component ≠ Verified Entire Address Space
```

## Sicherheitsziele

Der Verified Core soll mindestens folgende Fehlerklassen ausschließen oder kontrolliert erkennen können:

```text
Out-of-Bounds Read
Out-of-Bounds Write
Use-after-Free
Double Free
Invalid Free
Null Dereference
Dangling Pointer
Invalid Pointer Arithmetic
Uninitialized Memory Access
Lifetime Violation
Ownership Violation
Unauthorized Memory Access
```

## Memory Safety Model

```text
MemoryRegion
├── RegionID
├── AddressRange
├── Owner
├── Lifetime
├── AccessRights
├── State
└── Generation
```

Optional:

```text
CapabilityID
MappingID
AllocationID
SecurityLabel
SharingPolicy
ProvenanceID
```

## Pointer Validity

Ein Pointer ist nur verwendbar, wenn mindestens gilt:

```text
Pointer references valid region
AND
Region is alive
AND
Access is in bounds
AND
Requested operation is permitted
```

```text
Pointer Exists ≠ Pointer Valid
```

## Bounds Safety

Für einen Speicherbereich:

```text
Base ≤ Address
AND
Address + AccessSize ≤ End
```

muss jeder relevante Zugriff innerhalb des erlaubten Bereichs liegen.

Integer Overflow bei Adressberechnungen muss berücksichtigt werden.

## Lifetime Safety

Speicher besitzt einen definierten Lebenszyklus:

```text
Unallocated
    ↓
Allocated
    ↓
Initialized
    ↓
In Use
    ↓
Released
```

Nach `Released` darf kein bestehender Verweis den Speicher weiterhin gültig verwenden.

## Ownership

Ownership muss bei kritischen Speicherobjekten explizit modellierbar sein.

```text
Owned
Borrowed
Shared
Transferred
Pinned
```

Ownership Transfer muss den vorherigen Besitzer daran hindern können, den Speicher weiterhin als exklusiv besessen zu behandeln.

## Generation Safety

Wiederverwendete Speicherbereiche sollen durch Generationen unterscheidbar sein.

```text
Handle
├── RegionID
└── Generation
```

Dadurch können veraltete Referenzen erkannt werden.

```text
Same Address ≠ Same Allocation
```

## Initialization

Speicher darf nicht als initialisiert betrachtet werden, bevor die erforderlichen Daten definiert wurden.

```text
Allocated
   ↓
Initialize
   ↓
Readable
```

Sicherheitskritische Daten dürfen nicht unbeabsichtigt aus vorheriger Speichernutzung sichtbar werden.

## Memory Isolation

Memory Safety und Isolation ergänzen sich.

```text
Process A
   ↓
Address Space A

Process B
   ↓
Address Space B
```

Ein speichersicherer Zugriff kann trotzdem unautorisiert sein.

Daher müssen zusätzlich gelten:

```text
Memory Safety
+
Address-Space Isolation
+
Capability Authorization
```

## Shared Memory

Shared Memory benötigt explizite Regeln für:

```text
Participants
Ownership
Lifetime
Access Rights
Synchronization
Revocation
```

```text
Shared ≠ Unrestricted
```

Das Entfernen eines Teilnehmers darf keine unkontrollierten gültigen Zugriffsrechte hinterlassen.

## Zero-Copy

Zero-Copy darf Memory Safety nicht umgehen.

```text
Producer
   ↓
Shared / Mapped Buffer
   ↓
Consumer
```

Dabei müssen Bounds, Lifetime, Ownership und Zugriffsrechte weiterhin kontrolliert werden.

Falls sichere Zero-Copy-Nutzung nicht gewährleistet werden kann:

```text
Safe Copy Fallback
```

## DMA

DMA-fähige Geräte müssen in das Memory-Safety-Modell einbezogen werden.

```text
Device
   ↓
IOMMU / DMA Mapping
   ↓
Authorized Memory Region
```

Ein Treiber darf einem Gerät nicht automatisch Zugriff auf beliebigen physischen Speicher geben.

## Concurrency

Nebenläufige Speicherzugriffe müssen separat auf Synchronisationsfehler geprüft werden.

```text
Memory Safe ≠ Data-Race Free
```

Relevante Mechanismen:

```text
Atomics
Locks
RCU
Ownership Transfer
Immutable State
Versioning
```

## Formal Verification

Kritische Speicheroperationen sollen formal modelliert werden.

Beispiele:

```text
Allocate
Map
Unmap
Share
Transfer
Resize
Protect
Free
```

Für jede Operation sollen Preconditions, Postconditions und Invarianten definiert werden.

## Kerninvarianten

Beispiele:

```text
No access outside allocated region

No access after lifetime end

No unauthorized mapping

No overlapping exclusive ownership

No stale generation access

No capability amplification through memory mapping
```

## Model Checking

Nebenläufige Speicheroperationen sollen mit Model Checking untersucht werden können.

Beispiel:

```text
Task A → Acquire Reference
Task B → Free Region
Task A → Access Region
```

Der resultierende Use-after-Free-Pfad muss als Property-Verletzung erkennbar sein.

## Runtime Protection

Formale Verifikation wird durch Hardware- und Runtime-Mechanismen ergänzt:

```text
Page Protection
NX
Guard Pages
IOMMU
ASLR
Runtime Contracts
Bounds Validation
Handle Validation
```

Runtime Protection ersetzt keinen formalen Nachweis vorhandener Eigenschaften.

## Unsafe Code

Speicherunsichere Operationen müssen auf klar definierte Bereiche begrenzt werden.

```text
Safe Component
     ↓
Explicit Unsafe Boundary
     ↓
Hardware / Low-Level Operation
```

Unsafe Boundaries sollen klein, dokumentiert und besonders geprüft sein.

## FFI

FFI-Grenzen gelten als Memory-Safety-Grenzen.

Zu validieren sind insbesondere:

```text
Pointer
Length
Alignment
Lifetime
Ownership
Allocator
Mutability
```

```text
Foreign Function Call ≠ Memory Safe Call
```

## Failure Handling

Wird eine Memory-Safety-Verletzung erkannt, darf NovaOS nicht unkontrolliert weiterarbeiten.

Abhängig vom Scope:

```text
Reject Operation
Terminate Task
Isolate Process
Restart Service
Disable Driver
Enter Recovery
Kernel Panic
```

Die Reaktion richtet sich nach Failure Domain und Sicherheitsrisiko.

## Verification Artifacts

Memory-Safety-Verifikation soll referenzieren:

```text
SpecificationID
Component
Source Version
BuildID
Memory Model
Verified Properties
Assumptions
Tool
Verification Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Component
Memory-Safety Status
Verified Properties
Unsafe Boundaries
Verification Version
Runtime Protection
Known Assumptions
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS Memory Safety als Eigenschaft des Verified Core behandeln.
2. Kritische Speicheroperationen MÜSSEN formal spezifizierbar sein.
3. Out-of-Bounds-Zugriffe MÜSSEN ausgeschlossen oder kontrolliert erkannt werden können.
4. Use-after-Free MUSS für verifizierte Komponenten ausgeschlossen werden.
5. Double Free MUSS für verifizierte Komponenten ausgeschlossen werden.
6. Speicher-Lifetimes MÜSSEN modellierbar sein.
7. Ownership MÜSSEN explizit darstellbar sein können.
8. Wiederverwendete Speicherbereiche SOLLEN Generation Safety unterstützen.
9. Nicht initialisierter Speicher DARF NICHT unkontrolliert gelesen werden.
10. Memory Safety DARF NICHT mit Memory Isolation gleichgesetzt werden.
11. Speicherzugriff MUSS zusätzlich durch erforderliche Authority kontrollierbar sein.
12. Shared Memory MUSS explizite Lifetime- und Ownership-Regeln besitzen.
13. Zero-Copy DARF Memory-Safety-Garantien NICHT umgehen.
14. Ein sicherer Copy-Fallback MUSS verfügbar sein können.
15. DMA MUSS durch kontrollierte Memory Mappings begrenzbar sein.
16. Memory Safety DARF NICHT als Nachweis von Race Freedom interpretiert werden.
17. Kritische Concurrent-Memory-Operationen SOLLEN modellgeprüft werden.
18. Unsafe Boundaries MÜSSEN explizit identifizierbar sein.
19. Unsafe Code SOLL auf den kleinsten notwendigen Bereich begrenzt werden.
20. FFI-Grenzen MÜSSEN als Memory-Safety-Grenzen behandelt werden.
21. Erkannte Memory-Safety-Verletzungen MÜSSEN kontrolliert behandelt werden.
22. Verifikationsergebnisse MÜSSEN mit konkreten Implementierungsversionen verknüpfbar sein.
23. Memory-Safety-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-FFI-0001`
- `NPSPEC-SECURITY-ISOLATION-0001`
- `ADR-VERIFY-0003`

## Ergebnis

```text
Memory Operation
      ↓
Validate Region + Bounds
      ↓
Validate Lifetime + Ownership
      ↓
Validate Authority
      ↓
Execute
      ↓
Preserve Invariants
      ↓
Verify
```

NovaOS erhält damit ein formales Memory-Safety-Modell, das Bounds, Lifetime, Ownership, Isolation und Authority gemeinsam berücksichtigt und sicherheitskritische Speicheroperationen des Verified Core mathematisch überprüfbar macht.