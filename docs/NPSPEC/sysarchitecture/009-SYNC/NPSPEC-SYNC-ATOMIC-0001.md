# NPSPEC-SYNC-ATOMIC-0001 – Nova Atomic Operations

## Status

Entwurf

## Kategorie

Synchronization / Atomic Operations / Concurrency / Memory Model

## Zweck

NovaOS definiert atomare Operationen als grundlegenden Synchronisationsmechanismus für gemeinsam genutzte Zustände.

```text
Task A ─┐
        ├→ Atomic Object → Consistent State
Task B ─┘
```

Atomare Operationen ermöglichen Änderungen, die gegenüber konkurrierenden Ausführungskontexten als unteilbare Operation erscheinen.

## Grundprinzip

```text
Read
Modify
Write
```

wird atomar zu:

```text
Atomic Read-Modify-Write
```

Dabei gilt:

```text
Atomicity ≠ Mutual Exclusion
```

Atomare Operationen sind ein grundlegender Mechanismus, ersetzen aber nicht automatisch Locks, Transactions oder höhere Synchronisationsprimitive.

## Atomare Datentypen

NovaOS muss atomare Operationen für architekturabhängig unterstützte grundlegende Datentypen bereitstellen.

Mindestens vorgesehen sind:

```text
AtomicBool
AtomicInteger
AtomicPointer
AtomicFlags
```

Die tatsächlich unterstützten Größen hängen von der Hardwarearchitektur ab.

Nicht nativ unterstützte atomare Größen dürfen durch geeignete Mechanismen emuliert werden, sofern deren Semantik erhalten bleibt.

## Grundoperationen

Die atomare API soll mindestens folgende Operationen ermöglichen:

```text
Load
Store
Exchange
CompareExchange
FetchAdd
FetchSub
FetchAnd
FetchOr
FetchXor
```

Beispiel:

```text
Expected = 5
New      = 6

CompareExchange(Value, Expected, New)
```

Die Änderung erfolgt nur, wenn der aktuelle Wert dem erwarteten Wert entspricht.

## Compare-and-Exchange

Compare-and-Exchange bildet eine zentrale Grundlage für lockfreie Algorithmen.

```text
Read Current
     ↓
Current == Expected?
   ↙              ↘
 Yes              No
  ↓                ↓
Write New        Return Current
```

Prüfung und Änderung müssen atomar erfolgen.

## Memory Ordering

Atomare Operationen müssen eine explizite Memory-Ordering-Semantik unterstützen.

Mindestens erforderlich sind:

```text
Relaxed
Acquire
Release
AcquireRelease
SequentiallyConsistent
```

Dabei gilt:

```text
Atomic Operation ≠ Automatic Global Memory Ordering
```

Die stärkste Ordnung darf nicht automatisch verwendet werden, wenn eine schwächere Semantik korrekt und effizienter ist.

## Acquire / Release

Typisches Synchronisationsmuster:

```text
Producer
   ↓
Write Data
   ↓
Release Store
   ↓
Acquire Load
   ↓
Consumer
   ↓
Read Data
```

`Release` veröffentlicht vorherige relevante Speicheroperationen.

`Acquire` stellt die erforderliche Sichtbarkeit nachfolgender Operationen sicher.

## Hardware-Abstraktion

Die konkrete Implementierung darf hardwareabhängige Instruktionen verwenden.

Beispiele:

```text
x86 LOCK operations
ARM exclusive operations
RISC-V atomic instructions
```

Die öffentliche Nova-Semantik muss architekturunabhängig bleiben.

```text
Nova Atomic API
       ↓
Architecture Implementation
       ↓
Hardware Atomic Primitive
```

## Lock-Free Eigenschaften

Atomare APIs dürfen Eigenschaften ihrer Implementierung deklarieren:

```text
Lock-Free
Wait-Free
Hardware Atomic
Emulated
```

Dabei gilt:

```text
Atomic ≠ Lock-Free
Lock-Free ≠ Wait-Free
```

Eine emulierte atomare Operation darf intern Synchronisation verwenden, sofern ihre spezifizierte Semantik erhalten bleibt.

## Interrupt-Kontext

Atomare Operationen dürfen in Interrupt-Kontexten verwendet werden, sofern die jeweilige Implementierung dort sicher und nicht blockierend ist.

Blockierende Emulationen dürfen nicht in Kontexten verwendet werden, die keine Blockierung erlauben.

## SMP

Atomare Operationen müssen auf SMP-Systemen zwischen mehreren CPUs korrekt funktionieren.

```text
CPU 0 ─┐
CPU 1 ─┼→ Shared Atomic State
CPU 2 ─┘
```

CPU-lokale Atomizität allein ist für gemeinsam genutzte Daten nicht ausreichend.

## Performance

Atomare Operationen können Cache-Kohärenzverkehr und Speicherbarrieren verursachen.

```text
More Contention
      ↓
More Cache Transfers
      ↓
Higher Synchronization Cost
```

Häufig veränderte atomare Variablen sollen daher nicht unnötig zwischen vielen CPUs geteilt werden.

## Verwendung

Typische Einsatzbereiche sind:

```text
Reference Counters
State Flags
Sequence Counters
Lock Implementation
Task State
Queue Metadata
Resource Counters
```

Komplexe Invarianten über mehrere unabhängige Werte benötigen gegebenenfalls höhere Synchronisationsmechanismen.

## Normative Anforderungen

1. NovaOS MUSS atomare Load-, Store- und Read-Modify-Write-Operationen bereitstellen.
2. Compare-and-Exchange MUSS unterstützt werden.
3. Atomare Operationen MÜSSEN auf SMP-Systemen systemweit korrekt funktionieren.
4. Die API MUSS eine explizite Memory-Ordering-Semantik unterstützen.
5. Relaxed-, Acquire-, Release-, AcquireRelease- und SequentiallyConsistent-Semantik MÜSSEN darstellbar sein.
6. Hardwareabhängige Implementierungsdetails DÜRFEN NICHT Bestandteil der öffentlichen Semantik sein.
7. Nicht nativ unterstützte Atomics DÜRFEN emuliert werden, sofern die spezifizierte Semantik erhalten bleibt.
8. Eine Operation DARF nur dann als Lock-Free bezeichnet werden, wenn ihre Implementierung tatsächlich lockfrei ist.
9. Blockierende Atomic-Implementierungen DÜRFEN NICHT in nicht blockierbaren Kontexten verwendet werden.
10. Atomare Operationen DÜRFEN keine stärkere Synchronisationsgarantie vortäuschen als tatsächlich bereitgestellt wird.
11. Implementierungen SOLLEN unnötige Memory Barriers und Cache-Kohärenzkosten vermeiden.
12. Atomare Primitive MÜSSEN als Grundlage höherer Synchronisationsmechanismen verwendbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `ADR-SYNC-0001`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-VERIFY-0004_Type_Safety`

## Ergebnis

```text
Shared State
     ↓
Atomic Primitive
     ↓
Explicit Memory Ordering
     ↓
SMP-Safe State Transition
     ↓
Higher Synchronization Primitives
```

NovaOS erhält damit eine architekturunabhängige Grundlage für atomare Zustandsänderungen, Memory Ordering und den Aufbau effizienter Synchronisationsmechanismen.