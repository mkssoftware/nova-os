# NPSPEC-PROCESS-LIFECYCLE-0001 – Nova Process Lifecycle

## Status

Angenommen

## Kategorie

Process / Lifecycle / Kernel / Execution

## Zweck

NovaOS definiert den vollständigen Lebenszyklus eines Prozesses von seiner Erzeugung bis zur endgültigen Freigabe seiner Ressourcen.

```text
Create
  ↓
Start
  ↓
Run
  ↓
Terminate
  ↓
Cleanup
```

## Grundprinzip

Jeder Prozess besitzt einen expliziten und nachvollziehbaren Zustand.

```text
Created
   ↓
Starting
   ↓
Running
   ↓
Terminating
   ↓
Terminated
```

Fehler, Suspendierung und Wartezustände werden als definierte Zustandsübergänge behandelt.

## Prozesszustände

Mindestens folgende Zustände werden unterstützt:

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

`Unknown` kann verwendet werden, wenn der tatsächliche Zustand nicht sicher bestimmt werden kann.

```text
Unknown ≠ Running
Unknown ≠ Terminated
```

## Prozesserzeugung

Ein Prozess wird kontrolliert aufgebaut:

```text
Create Request
      ↓
Validate
      ↓
Create Process Object
      ↓
Create Address Space
      ↓
Assign Security Context
      ↓
Assign Capabilities
      ↓
Allocate Resources
      ↓
Create Initial Thread
      ↓
Starting
```

Bei einem Fehler während der Erzeugung müssen bereits reservierte Ressourcen wieder freigegeben werden.

## Start

Ein Prozess darf erst `Running` erreichen, wenn seine notwendigen Startbedingungen erfüllt sind.

Dazu können gehören:

```text
Executable validated
Address Space ready
Security Context valid
Required Capabilities available
Initial Thread ready
Required Resources available
```

## Laufzeit

Während `Running` kann ein Prozess zwischen ausführbaren und wartenden Zuständen wechseln.

```text
Running
   ↕
Waiting
```

Scheduling-Zustände einzelner Threads sind vom übergeordneten Prozesszustand zu unterscheiden.

## Suspendierung

Ein Prozess kann kontrolliert suspendiert werden.

```text
Running
   ↓
Suspended
   ↓
Running
```

Während `Suspended` darf keine normale Prozessausführung stattfinden.

Ressourcen und Prozesszustand bleiben grundsätzlich erhalten.

## Terminierung

Die Beendigung kann ausgelöst werden durch:

```text
Normal Exit
Explicit Termination
Cancellation
Process Failure
Security Policy
Resource Policy
Supervisor Decision
```

Der Übergang erfolgt über:

```text
Running
   ↓
Terminating
   ↓
Terminated
```

## Cleanup

Während `Terminating` werden prozessgebundene Ressourcen kontrolliert behandelt.

```text
Stop Threads
    ↓
Cancel / Resolve Tasks
    ↓
Close IPC Endpoints
    ↓
Release Handles
    ↓
Release Memory
    ↓
Release Resources
    ↓
Publish Exit State
```

Ressourcen mit anderem expliziten Besitzer dürfen nicht automatisch zerstört werden.

## Kindprozesse und Tasks

Das Ende eines Prozesses muss Auswirkungen auf abhängige Ausführungseinheiten definieren.

Mögliche Aktionen:

```text
Wait
Cancel
Terminate
Transfer Ownership
Detach to explicit Supervisor
```

Verwaiste Ausführung ohne definierten Besitzer ist zu vermeiden.

## Fehlerzustand

Ein Prozessfehler führt zu einem definierten Fehlerzustand.

```text
Running
   ↓
Failure
   ↓
Failed
   ↓
Cleanup / Restart / Recovery
```

Ein Supervisor darf abhängig von Policy einen Neustart oder andere Recovery-Maßnahmen auslösen.

## Exit-Information

Nach Prozessende müssen mindestens relevante Abschlussinformationen verfügbar sein:

```text
ProcessID
Exit Reason
Exit Status
Final State
Failure Information
Resource Cleanup Status
```

Diese Informationen dürfen nach vollständiger Auswertung kontrolliert verworfen werden.

## Normative Anforderungen

1. Jeder Prozess MUSS einen expliziten Lifecycle-Zustand besitzen.
2. Zustandsübergänge MÜSSEN kontrolliert erfolgen.
3. Ein Prozess DARF erst nach erfolgreicher Initialisierung `Running` erreichen.
4. Fehlgeschlagene Prozesserzeugung MUSS bereits reservierte Ressourcen bereinigen.
5. `Unknown` DARF NICHT als erfolgreicher oder beendeter Zustand interpretiert werden.
6. Suspendierung MUSS normale Prozessausführung stoppen.
7. Terminierung MUSS neue normale Prozessaktivität verhindern.
8. Prozessgebundene Ressourcen MÜSSEN beim Prozessende kontrolliert freigegeben werden.
9. Abhängige Threads und Tasks MÜSSEN vor Abschluss der Terminierung aufgelöst werden.
10. Verwaiste Ausführung DARF NICHT ohne expliziten neuen Besitzer entstehen.
11. Prozessfehler MÜSSEN diagnostizierbar sein.
12. Der endgültige Exit-Zustand MUSS dem zuständigen Supervisor oder Aufrufer verfügbar gemacht werden.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-PROCESS-0002`
- `ADR-PROCESS-0003`

## Ergebnis

```text
Create
  ↓
Initialize
  ↓
Run / Wait / Suspend
  ↓
Terminate / Fail
  ↓
Cleanup
  ↓
Final State
```

NovaOS erhält damit einen eindeutig definierten Prozesslebenszyklus mit kontrollierter Erzeugung, Ausführung, Terminierung und Ressourcenbereinigung.