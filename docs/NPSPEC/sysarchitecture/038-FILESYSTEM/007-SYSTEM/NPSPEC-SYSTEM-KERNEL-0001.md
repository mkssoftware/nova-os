# NPSPEC-SYSTEM-KERNEL-0001 – Nova System Kernel

## Status

Angenommen

## Kategorie

System / Kernel

## Zweck

NovaOS definiert den Kernel als privilegierten Kern des Systems.

Der Kernel stellt grundlegende Mechanismen für Speicher, Prozesse, Scheduling, Interrupts, IPC, Capabilities und Hardwarezugriff bereit. Höhere Systemrichtlinien und Anwendungslogik verbleiben soweit möglich außerhalb des Kernels.

## Grundprinzipien

```text
Kernel ≠ Gesamtes Betriebssystem
Mechanism ≠ Policy
Kernel Privilege ≠ Application Authority
Kernel Object ≠ Filesystem Object
Kernel Failure → Controlled Failure Handling
```

NovaOS verwendet einen modularen monolithischen Kernel.

## Verantwortlichkeiten

Der Kernel stellt insbesondere Mechanismen bereit für:

```text
Memory Management
Process / Task Management
Scheduling
Interrupt Handling
Synchronization
IPC
Capability Enforcement
System Calls
Hardware Abstraction
Resource Accounting
Time
Low-Level I/O
```

Komplexe Benutzerfunktionen und unnötige Policy gehören nicht in den Kernel.

## Architektur

```text
Userspace
   ↓
System API / Runtime
   ↓
Syscall / IPC Boundary
   ↓
Kernel
├── Memory
├── Scheduler
├── Process
├── IPC
├── Capability Enforcement
├── Interrupts
├── I/O
└── HAL
   ↓
Hardware
```

## Modularität

Kernelkomponenten müssen klar definierte interne Schnittstellen besitzen.

```text
Kernel Core
   ├── Memory
   ├── Scheduler
   ├── IPC
   ├── Drivers
   └── HAL
```

Module dürfen nicht unkontrolliert auf interne Zustände anderer Subsysteme zugreifen.

## Mechanism / Policy

Der Kernel implementiert primär Mechanismen.

Beispiele:

```text
Kernel:
Scheduling Mechanism
Memory Mapping
Capability Validation
IPC Transport

Policy:
Scheduling Preference
Resource Priority
Application Rules
User Decisions
```

Policy darf nur dann im Kernel liegen, wenn dies aus Sicherheit, Echtzeitverhalten oder fundamentaler Systemfunktion erforderlich ist.

## Capabilities

Privilegierte Kerneloperationen werden durch den Capability-Kontext des Aufrufers kontrolliert.

```text
Request
   ↓
Capability Validation
   ↓
Kernel Mechanism
   ↓
Result
```

Ein Prozess darf keine privilegierte Operation allein aufgrund seiner Identität oder seines Pfades ausführen.

## Fehlerisolation

Fehler sollen möglichst auf die betroffene Komponente begrenzt werden.

Nicht vertrauenswürdige oder komplexe Komponenten können außerhalb des Kernels ausgeführt werden, wenn dadurch Isolation und Wiederherstellbarkeit verbessert werden.

Treiber sollen entsprechend ihrer Anforderungen im Kernel- oder Userspace betrieben werden können.

## Verified Core

Besonders sicherheitskritische Kernelbereiche sollen für formale Verifikation geeignet strukturiert werden.

Priorisiert werden insbesondere:

```text
Memory Isolation
Capability Enforcement
IPC Boundaries
Critical Kernel State
```

Nicht verifizierte Komponenten dürfen die garantierten Eigenschaften des Verified Core nicht unkontrolliert umgehen.

## Introspection

Der Kernel stellt kontrollierte Informationen über seinen Zustand bereit:

```text
Memory State
Processes / Tasks
Scheduler State
Resources
Capabilities
IPC
Drivers
Errors
Health
```

Introspection erzeugt keine zusätzliche Authority.

## Normative Anforderungen

1. NovaOS MUSS einen modularen monolithischen Kernel verwenden.
2. Kernelkomponenten MÜSSEN klar definierte Schnittstellen besitzen.
3. Mechanismus und Policy SOLLEN getrennt werden.
4. Unnötige Anwendungslogik DARF nicht in den Kernel integriert werden.
5. Privilegierte Operationen MÜSSEN capability-basiert kontrollierbar sein.
6. Prozesse DÜRFEN nicht automatisch Kernel-Authority erhalten.
7. Kritische Kernelzustände MÜSSEN gegen unkontrollierte Änderungen geschützt sein.
8. Fehler SOLLEN möglichst auf betroffene Komponenten begrenzt werden.
9. Sicherheitskritische Kernmechanismen SOLLEN formal verifizierbar gestaltet werden.
10. Nicht verifizierte Komponenten DÜRFEN Verified-Core-Garantien nicht umgehen.
11. Kernelressourcen MÜSSEN in das Resource Accounting integrierbar sein.
12. Kernelzustände MÜSSEN kontrolliert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-LAYOUT-0001`
- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaOS besitzt einen modularen monolithischen Kernel, der die fundamentalen Systemmechanismen bereitstellt und gleichzeitig Policy, Anwendungen und unnötige Komplexität aus dem privilegierten Kern heraushält. Capability-Sicherheit, Isolation, Resource Accounting und ein formal verifizierbarer sicherheitskritischer Kern bilden die Grundlage der Kernelarchitektur.