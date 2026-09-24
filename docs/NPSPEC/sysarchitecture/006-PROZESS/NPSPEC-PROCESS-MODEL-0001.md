# NPSPEC-PROCESS-MODEL-0001 – Nova Process Model

## Status

Entwurf

## Kategorie

Process / Kernel / Execution / Isolation

## Zweck

NovaOS definiert ein einheitliches Prozessmodell für isolierte Ausführung, Ressourcenbesitz und kontrollierte Interaktion zwischen laufenden Komponenten.

```text
Executable / Capability
        ↓
Process
        ↓
Execution Context
        ↓
Threads / Tasks
```

## Grundprinzip

Ein Prozess ist primär eine isolierte Ressourcen- und Sicherheitsdomäne.

```text
Process
├── Address Space
├── Security Context
├── Capabilities
├── Resources
├── Threads / Tasks
└── Process State
```

Die eigentliche CPU-Ausführung erfolgt durch Threads oder Tasks innerhalb des Prozesses.

## Prozessidentität

Jeder Prozess besitzt eine systemweit eindeutige Prozessidentität.

```text
ProcessID
ParentID
Identity
SecurityContext
State
```

Eine wiederverwendete interne Prozessnummer darf nicht mit der dauerhaften Identität eines früheren Prozesses verwechselt werden.

## Prozesszustände

Mindestens folgende Zustände werden unterschieden:

```text
Created
Starting
Running
Waiting
Suspended
Terminating
Terminated
Failed
```

Zustandsübergänge müssen kontrolliert erfolgen.

## Adressraum

Jeder normale User-Space-Prozess erhält einen isolierten virtuellen Adressraum.

```text
Process A Address Space
        ≠
Process B Address Space
```

Gemeinsamer Speicher muss explizit eingerichtet und autorisiert werden.

Kernel-Speicher darf nicht regulär aus User Space zugänglich sein.

## Threads und Tasks

Ein Prozess kann mehrere Ausführungseinheiten enthalten.

```text
Process
 ├── Thread 1
 ├── Thread 2
 └── Thread 3
```

Threads teilen definierte Prozessressourcen, besitzen jedoch eigenen Ausführungszustand wie Register, Stack und Scheduling-Zustand.

Structured Concurrency soll für hierarchisch organisierte Tasks verwendet werden.

## Ressourcen

Ressourcen werden einem Prozess oder einem expliziten übergeordneten Besitzer zugeordnet.

Beispiele:

```text
Memory
Handles
IPC Endpoints
Files / Objects
Capabilities
Device Access
Compute Resources
```

Beim Prozessende müssen prozessgebundene Ressourcen kontrolliert freigegeben oder an einen definierten Besitzer übertragen werden.

## Sicherheit

Ein Prozess erhält nur explizit autorisierte Fähigkeiten.

```text
Process
   ↓
Security Context
   ↓
Capabilities
   ↓
Authorized Resources
```

Die Existenz eines Prozesses gewährt keine allgemeinen Systemrechte.

Kindprozesse dürfen Berechtigungen nicht automatisch erweitern.

## Prozesserzeugung

Die Erzeugung erfolgt kontrolliert:

```text
Create
  ↓
Validate Execution Request
  ↓
Create Address Space
  ↓
Assign Security Context
  ↓
Assign Resources
  ↓
Create Initial Thread
  ↓
Start
```

NovaOS muss nicht von einem klassischen POSIX-`fork()`-Modell abhängig sein.

## Prozessbeziehungen

Prozesse können Beziehungen besitzen:

```text
Parent
Child
Service
Client
Supervisor
Owner
```

Diese Beziehungen werden explizit im Systemmodell abgebildet und dürfen nicht ausschließlich aus numerischen Prozess-IDs abgeleitet werden.

## Prozessende

Ein Prozess kann enden durch:

```text
Normal Exit
Failure
Cancellation
Security Termination
Resource Enforcement
Explicit Termination
```

Beim Ende gilt:

```text
Stop Execution
    ↓
Resolve Child Tasks
    ↓
Release Resources
    ↓
Publish Exit State
```

## Fehlerisolation

Ein Fehler in einem User-Space-Prozess darf nicht unmittelbar den Kernel oder andere isolierte Prozesse beschädigen.

Fehlerzustände müssen dem zuständigen Supervisor oder Aufrufer gemeldet werden können.

## Introspection

Der Prozesszustand soll über autorisierte Introspection-Schnittstellen sichtbar sein.

Beispiele:

```text
Process State
Threads
Resource Usage
Capabilities
Relationships
Execution State
Failure Information
```

## Normative Anforderungen

1. NovaOS MUSS Prozesse als isolierte Ausführungs- und Ressourcendomänen unterstützen.
2. Jeder Prozess MUSS eindeutig identifizierbar sein.
3. User-Space-Prozesse MÜSSEN getrennte virtuelle Adressräume besitzen, sofern nicht ausdrücklich anders definiert.
4. Gemeinsamer Speicher MUSS explizit eingerichtet werden.
5. CPU-Ausführung MUSS über definierte Threads oder Tasks erfolgen.
6. Prozesse DÜRFEN nur autorisierte Capabilities und Ressourcen verwenden.
7. Prozesserzeugung DARF Berechtigungen nicht implizit erweitern.
8. Prozessressourcen MÜSSEN beim Prozessende kontrolliert behandelt werden.
9. Prozessfehler MÜSSEN soweit möglich auf die jeweilige Isolationsdomäne begrenzt bleiben.
10. Prozessbeziehungen SOLLEN explizit im Systemmodell abgebildet werden.
11. NovaOS DARF NICHT architektonisch von POSIX-`fork()` abhängig sein.
12. Prozesszustand und Ressourcenverbrauch SOLLEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-PROCESS-0001`
- `ADR-PROCESS-0002`
- `ADR-PROCESS-0003`
- `ADR-PROCESS-0004`
- `ADR-PROCESS-0005`
- `ADR-PROCESS-0006`
- `ADR-PROCESS-0007`

## Ergebnis

```text
Execution Request
       ↓
Isolated Process
       ↓
Threads / Tasks
       ↓
Controlled Resources
       ↓
Defined Lifecycle
```

NovaOS erhält damit ein natives Prozessmodell mit klarer Isolation, explizitem Ressourcenbesitz und Capability-basierter Autorisierung, ohne die Architektur an klassische POSIX-Prozesssemantik zu binden.