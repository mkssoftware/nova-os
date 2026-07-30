# ADR-PROC-0002: Thread Lifecycle

* **ADR-ID:** ADR-PROC-0002
* **Titel:** Thread Lifecycle
* **Deutscher Titel:** Lebenszyklus von Threads
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-30
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Process Management
* **Gültigkeitsbereich:** Thread Manager, Scheduler, Process Manager, Kernel Object Manager, Memory Manager, Security Manager, Event Core, Diagnostic Core
* **Ersetzt:** Implizite Threadzustände
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-PROC-0001, ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105
* **Entscheidungstyp:** Architekturentscheidung
* **Sicherheitsrelevanz:** Kritisch
* **Kompatibilitätsrelevanz:** Hoch

---

# 1. Zusammenfassung

NovaOS definiert einen vollständig expliziten Lebenszyklus für Threads.

Ein Thread ist die kleinste planbare Ausführungseinheit des Betriebssystems und wird ausschließlich durch den **Thread Manager** verwaltet.

Ein Thread besitzt:

* eine eindeutige Thread Object ID
* eine Thread ID (TID)
* einen Ausführungskontext
* einen Kernel Stack
* optional einen User Stack
* CPU-Register
* Scheduling-Informationen
* Synchronisationsinformationen
* einen Sicherheitskontext
* Thread Local Storage
* Diagnoseinformationen

Jeder Thread durchläuft ausschließlich definierte Zustände.

Alle Zustandswechsel werden atomar durchgeführt und über den Event Core veröffentlicht.

---

# 2. Kontext

Ein Prozess beschreibt einen Ressourcencontainer.

Die eigentliche Programmausführung erfolgt jedoch ausschließlich über Threads.

Threads benötigen:

* Schedulerintegration
* Registerkontext
* Stackverwaltung
* Synchronisation
* CPU-Affinität
* Prioritäten
* Interruptintegration
* Ausnahmebehandlung
* Debugging
* Telemetrie

Fehlt ein einheitliches Threadmodell entstehen häufig:

* Race Conditions
* Zombie-Threads
* Stacklecks
* Deadlocks
* Schedulerfehler
* inkonsistente Threadzustände

---

# 3. Problemstellung

Es musste entschieden werden,

* welche Threadzustände existieren,
* wie Threads erstellt werden,
* wie Threads gestartet werden,
* wann Threads ausführbar sind,
* wie Scheduler und Threadmanager zusammenarbeiten,
* wie Threadbeendigungen erfolgen,
* wie Threadkontexte gespeichert werden,
* wie Stackverwaltung erfolgt,
* wie Debugger Threads kontrollieren,
* wie Threadereignisse veröffentlicht werden.

---

# 4. Entscheidung

NovaOS verwendet einen vollständig expliziten Threadzustandsautomaten.

Der Thread Manager ist alleiniger Eigentümer sämtlicher Lifecycle-Zustände.

Scheduler, Process Manager und andere Kernelkomponenten dürfen Threadzustände ausschließlich über definierte Kernel-Services verändern.

---

# 5. Motivation

Die Architektur ermöglicht:

* deterministische Zustandswechsel
* klare Schedulergrenzen
* sichere Kontextwechsel
* saubere Stackverwaltung
* reproduzierbare Fehleranalyse
* einfache Erweiterbarkeit
* effizientes SMP-Scheduling
* NUMA-Unterstützung
* Debugbarkeit

---

# 6. Nicht-Ziele

Diese ADR definiert nicht:

* konkrete Schedulingalgorithmen
* Prioritätsberechnung
* Echtzeitplanung
* Synchronisationsprimitive
* TLS-ABI
* Interruptarchitektur

Diese Themen werden separat beschrieben.

---

# 7. Designprinzipien

Der Thread Lifecycle basiert auf:

* atomaren Zustandswechseln
* eindeutiger Ownership
* vollständiger Schedulertrennung
* expliziten Übergängen
* Capability-basierter Kontrolle
* objektorientierter Verwaltung
* Ereignisorientierung
* deterministischem Verhalten
* geringer Schedulerlatenz

---

# 8. Architekturübersicht

```text
                Process
                   │
             Thread Manager
                   │
      ┌────────────┼────────────┐
      ▼            ▼            ▼
 Scheduler      Memory       Security
      │
      ▼
 Thread Object
      │
      ▼
Kernel Object Graph
      │
      ▼

Kernel Event Core
```

---

# 9. Threaddefinition

Ein Thread beschreibt genau einen Ausführungskontext.

Er besitzt mindestens:

* Registersatz
* Program Counter
* Stack Pointer
* Status Register
* Kernel Stack
* optional User Stack
* Thread Local Storage
* Schedulingparameter
* CPU-Zuordnung
* Prozessreferenz

Mehrere Threads können demselben Prozess angehören.

---

# 10. Threadidentität

Jeder Thread besitzt:

* Object ID
* Thread ID
* Generation
* Parent Process ID
* Lifecycle Domain
* Scheduler Domain

Die Object ID bleibt die autoritative Identität.

---

# 11. Threadreferenz

Eine Threadreferenz besteht logisch aus:

```text
Thread Reference
├── TID
├── Generation
├── Namespace
└── Object ID
```

Dadurch werden alte Thread-IDs zuverlässig erkannt.

---

# 12. Threadzustände

NovaOS definiert folgende Hauptzustände:

* Allocated
* Constructing
* Initializing
* Ready
* Runnable
* Running
* Waiting
* Blocked
* Sleeping
* Parked
* Suspended
* Resuming
* Terminating
* Zombie
* DestroyPending
* Destroyed
* Reclaimed

---

# 13. Zustandsdiagramm

```text
     Allocated
         │
         ▼
    Constructing
         │
         ▼
    Initializing
         │
         ▼
       Ready
         │
         ▼
      Runnable
         │
         ▼
      Running
 ┌───────┼───────┐
 ▼       ▼       ▼
Wait   Sleep   Blocked
 │       │       │
 └───────┴───────┘
         |
         ▼
      Runnable
         │
         ▼
      Running
         │
         ▼
    Terminating
         │
         ▼
       Zombie
         │
         ▼
   DestroyPending
         │
         ▼
     Destroyed
         │
         ▼
     Reclaimed
```

---

# 14. Zustand `Allocated`

Der Speicher für das Thread Object wurde reserviert.

Vorhanden:

* Object Header
* Object ID
* Lifecycle Lock
* Referenzzähler

Noch nicht vorhanden:

* Stack
* Registersatz
* Schedulerdaten

---

# 15. Zustand `Constructing`

In diesem Zustand werden erstellt:

* Kernel Stack
* TLS-Struktur
* Schedulerobjekt
* Synchronisationsobjekte
* Diagnosedaten
* Thread Context

Fehler führen zum vollständigen Rollback.

---

# 16. Zustand `Initializing`

Der Thread erhält:

* Registerinitialisierung
* Program Counter
* Stack Pointer
* Initiale Flags
* CPU-Kontext
* Exception Frame
* Prozessreferenz

Der Thread ist noch nicht planbar.

---

# 17. Zustand `Ready`

Der Thread wurde vollständig erzeugt.

Der Scheduler kennt ihn bereits.

Er wird jedoch noch nicht ausgeführt.

---

# 18. Zustand `Runnable`

Der Thread befindet sich in einer Run Queue.

Er wartet ausschließlich auf CPU-Zeit.

---

# 19. Zustand `Running`

Der Thread wird momentan auf genau einer CPU ausgeführt.

Eigenschaften:

* besitzt CPU
* besitzt Register
* besitzt aktiven Stack
* führt Userspace oder Kernelcode aus

Ein Thread kann niemals gleichzeitig auf mehreren CPUs laufen.

---

# 20. Zustand `Waiting`

Der Thread wartet auf ein Ereignis.

Beispiele:

* IPC
* Completion Object
* Event
* Semaphore
* Mutex
* Process Exit
* Timer

Der Scheduler plant ihn währenddessen nicht ein.

---

# 21. Zustand `Blocked`

Der Thread kann aktuell nicht fortgesetzt werden.

Typische Ursachen:

* Lock
* I/O
* Geräteantwort
* Speicheroperation
* Seitenfehler

Nach Beseitigung der Ursache wechselt er nach `Runnable`.

---

# 22. Zustand `Sleeping`

Der Thread schläft bis zu einem definierten Zeitpunkt.

Der Scheduler aktiviert ihn nach Ablauf des Timers automatisch wieder.

---

# 23. Zustand `Parked`

Ein geparkter Thread bleibt dauerhaft angehalten.

Er wird ausschließlich durch eine explizite Park-Aufhebung wieder aktiviert.

Dieser Zustand wird häufig von Laufzeitumgebungen verwendet.

---

# 24. Zustand `Suspended`

Ein suspendierter Thread besitzt:

* vollständigen Registersatz
* gültigen Kernel Stack
* gültigen User Stack
* eingefrorenen Ausführungskontext

Er wird nicht eingeplant.

---

# 25. Zustand `Resuming`

Beim Wiederaufnehmen werden geprüft:

* Register
* Stackintegrität
* Capabilityänderungen
* CPU-Verfügbarkeit
* Schedulerparameter

Danach erfolgt der Übergang nach `Runnable`.

---

# 26. Zustand `Terminating`

Der Thread beendet sich.

Dabei erfolgen:

* Thread Exit Handler
* TLS Cleanup
* Kernel Cleanup
* Synchronisationsbereinigung
* Diagnosedatenerzeugung

Neue Systemaufrufe sind nicht mehr zulässig.

---

# 27. Zustand `Zombie`

Der Thread existiert noch als Objekt.

Vorhanden bleiben:

* Exit Code
* Diagnose
* Join Status
* Referenzen

Es wird kein Code mehr ausgeführt.

---

# 28. Zustand `DestroyPending`

Noch vorhandene Referenzen verhindern die endgültige Freigabe.

Beispiele:

* Join Handle
* Debugger
* Diagnose
* Schedulerreferenzen

---

# 29. Zustand `Destroyed`

Alle Threadressourcen wurden freigegeben.

Nicht mehr vorhanden:

* Stack
* Register
* TLS
* Schedulerobjekt

---

# 30. Zustand `Reclaimed`

Auch das Thread Object wurde freigegeben.

Die TID darf anschließend mit neuer Generation wiederverwendet werden.

---

# 31. Zulässige Übergänge

```text
Allocated -> Constructing

Constructing -> Initializing

Initializing -> Ready

Ready -> Runnable

Runnable -> Running

Running -> Waiting

Running -> Sleeping

Running -> Blocked

Running -> Terminating

Waiting -> Runnable

Sleeping -> Runnable

Blocked -> Runnable

Runnable -> Suspended

Suspended -> Resuming

Resuming -> Runnable

Terminating -> Zombie

Zombie -> DestroyPending

DestroyPending -> Destroyed

Destroyed -> Reclaimed
```

Andere Übergänge sind unzulässig.

---

# 32. Übergangsvalidierung

Jeder Zustandswechsel wird geprüft auf:

* aktuellem Zustand
* Zielzustand
* Schedulerstatus
* Prozesszustand
* Capability
* Referenzen
* aktive Kerneloperationen

Ungültige Übergänge werden verworfen.

---

# 33. Thread Manager

Der Thread Manager verwaltet:

* Threaderstellung
* Threadzerstörung
* Threadzustände
* Join
* Suspend
* Resume
* TLS
* Threadreferenzen
* Schedulerregistrierung

Er ist die einzige autoritative Komponente für den Thread Lifecycle.

---

# 34. Thread Service

Der versionierte Thread Service stellt mindestens folgende Operationen bereit:

```text
thread_create()

thread_start()

thread_suspend()

thread_resume()

thread_join()

thread_detach()

thread_cancel()

thread_query()

thread_terminate()
```

Die genaue ABI wird in einer eigenen NPSPEC definiert.

---

# 35. Threaderstellung

Die Erstellung erfolgt transaktional.

Sie umfasst:

* Thread Object
* Kernel Stack
* TLS
* Register
* Schedulerobjekt
* Startparameter
* CPU-Kontext

Fehlschläge führen zu einem vollständigen Rollback.

---

# 36. Erstellungsablauf

```text
Create Thread
      │
      ▼
Allocate Object
      │
      ▼
Allocate Stack
      │
      ▼
Initialize Registers
      │
      ▼
 Create TLS
      │
      ▼
Scheduler Registration
      │
      ▼
    Ready
```

---

# 37. Rollback

Schlägt ein Schritt fehl, werden freigegeben:

* TLS
* Stack
* Schedulerobjekt
* Thread Object

Es verbleiben keine Teilobjekte.

---

# 38. Initialer Thread

Der erste Thread eines Prozesses wird durch den Loader erzeugt.

Er beginnt am Entry Point des Programms.

Er besitzt bereits:

* User Stack
* Kernel Stack
* Register
* TLS
* Schedulerinformationen

---

# 39. Zusätzliche Threads

Weitere Threads können ausschließlich über den Thread Service erzeugt werden.

Sie übernehmen niemals automatisch Ressourcen anderer Threads.

---

# 40. Thread Ownership

Jeder Thread gehört genau einem Prozess.

Ein Thread kann niemals gleichzeitig mehreren Prozessen zugeordnet sein.

---

# 41. Scheduler-Integration

Der Scheduler ist ausschließlich für die CPU-Planung zuständig.

Der Thread Manager bleibt Eigentümer des Thread-Lifecycles.

Die Verantwortlichkeiten sind klar getrennt:

**Thread Manager**

* Thread erstellen
* Thread zerstören
* Lifecycle verwalten
* Threadobjekte verwalten
* Threadreferenzen verwalten

**Scheduler**

* CPU auswählen
* Run Queues verwalten
* Prioritäten anwenden
* Zeitscheiben vergeben
* Context Switch durchführen

Der Scheduler darf keine Threadobjekte erzeugen oder zerstören.

---

# 42. Zusammenarbeit zwischen Thread Manager und Scheduler

```text
Thread Service
      │
      ▼
Thread Manager
      │
      ▼
Scheduler Interface
      │
      ▼
Scheduler
      │
      ▼
     CPU
```

Die Kommunikation erfolgt ausschließlich über definierte Scheduler Services.

---

# 43. Schedulerzustände

Der Scheduler kennt nur planungsrelevante Zustände.

Beispiele:

* Runnable
* Running
* Waiting
* Blocked
* Sleeping

Interne Lifecyclezustände wie `Constructing` oder `DestroyPending` sind ausschließlich Sache des Thread Managers.

---

# 44. Run Queue

Runnable Threads befinden sich in einer Run Queue.

NovaOS verwendet logisch:

```text
CPU 0
 ├── High
 ├── Normal
 ├── Low

CPU 1
 ├── High
 ├── Normal
 ├── Low
```

Die konkrete Implementierung wird in der Scheduler-ADR definiert.

---

# 45. CPU-Auswahl

Der Scheduler berücksichtigt unter anderem:

* Priorität
* CPU-Affinität
* NUMA-Knoten
* CPU-Auslastung
* Cache-Lokalität
* SMT
* Energieprofil

Die Auswahl erfolgt unabhängig vom Thread Lifecycle.

---

# 46. Context Switch

Ein Context Switch umfasst mindestens:

* Register sichern
* Stack Pointer sichern
* Program Counter sichern
* Statusregister sichern
* TLS-Zeiger aktualisieren
* MMU-Kontext prüfen
* CPU-Lokaldaten aktualisieren
* nächsten Thread laden

---

# 47. Context Switch Ablauf

```text
Running Thread
      │
      ▼
Save Context
      │
      ▼
Scheduler Decision
      │
      ▼
Load Context
      │
      ▼
Running Thread
```

---

# 48. Thread Context

Der Thread Context enthält mindestens:

* General Purpose Register
* Instruction Pointer
* Stack Pointer
* Flags Register
* SIMD/FPU-Kontext
* Debug Register
* TLS Pointer
* Exception Context

Architekturspezifische Erweiterungen sind zulässig.

---

# 49. Kernel Stack

Jeder Thread besitzt genau einen Kernel Stack.

Eigenschaften:

* privat
* nicht teilbar
* feste Größe oder kontrollierte Erweiterung
* Guard Pages
* Kernel-only

Kernel Stacks dürfen niemals zwischen Threads geteilt werden.

---

# 50. User Stack

Ein User Thread besitzt zusätzlich einen User Stack.

Eigenschaften:

* virtueller Speicher
* Guard Pages
* automatische Erweiterung gemäß Policy
* Address Space des Prozesses

Kernel Threads besitzen normalerweise keinen User Stack.

---

# 51. Stack Guard Pages

Kernel- und User-Stacks erhalten Guard Pages.

Sie dienen:

* Stackoverflow-Erkennung
* Speicherschutz
* Diagnose
* Sicherheitsverbesserung

Ein Zugriff erzeugt eine definierte Ausnahme.

---

# 52. Thread Local Storage

Jeder Thread besitzt einen eigenen TLS-Bereich.

Er enthält beispielsweise:

* Runtime-Daten
* Fehlervariablen
* Sprachruntime
* Thread-spezifische Objekte

TLS wird beim Threadende automatisch freigegeben.

---

# 53. TLS-Lebenszyklus

TLS wird erzeugt:

* während `Constructing`

TLS existiert:

* während der gesamten Threadlaufzeit

TLS wird zerstört:

* während `Terminating`

---

# 54. CPU-Affinität

Ein Thread kann besitzen:

* keine Affinität
* bevorzugte CPUs
* verbotene CPUs
* feste CPU

Affinität beeinflusst ausschließlich den Scheduler.

---

# 55. CPU-Migration

Ein Thread darf zwischen CPUs migrieren.

Nicht erlaubt:

* gleichzeitige Ausführung auf zwei CPUs

Migration erfolgt ausschließlich über den Scheduler.

---

# 56. Prioritäten

Ein Thread besitzt mindestens:

* Basispriorität
* effektive Priorität
* Schedulingklasse

Die Berechnung erfolgt außerhalb des Lifecycles.

---

# 57. Prioritätsvererbung

Synchronisationsprimitive dürfen Prioritätsvererbung verwenden.

Der Thread Lifecycle bleibt davon unberührt.

---

# 58. Threadklassen

NovaOS unterscheidet beispielsweise:

* User Thread
* Kernel Thread
* Worker Thread
* Interrupt Thread
* Service Thread
* Idle Thread

Alle verwenden denselben Lifecycle.

---

# 59. Idle Thread

Jede CPU besitzt mindestens einen Idle Thread.

Eigenschaften:

* niemals beendet
* niedrigste Priorität
* CPU-spezifisch

Der Idle Thread besitzt keinen normalen Prozesskontext.

---

# 60. Kernel Threads

Kernel Threads besitzen:

* Kernel Stack
* Kernel Address Space
* Kernel Entry Point

Sie besitzen üblicherweise keinen User Stack.

---

# 61. Worker Threads

Worker Threads dienen:

* Hintergrundarbeiten
* IO
* Cachepflege
* Kompression
* Recovery
* Self-Healing

Sie unterscheiden sich nur durch ihre Aufgabe, nicht durch den Lifecycle.

---

# 62. Interrupt Threads

Bestimmte Interrupts können in Threadkontexte ausgelagert werden.

Dadurch:

* kürzere Interruptlatenz
* besseres Scheduling
* Prioritätssteuerung

---

# 63. Thread Start

Ein neuer Thread startet niemals direkt.

Ablauf:

```text
Create
↓
Initialize
↓
Ready
↓
Runnable
↓
Running
```

---

# 64. Startparameter

Ein Thread kann erhalten:

* Entry Point
* Parameter
* TLS Initializer
* Priorität
* CPU-Affinität
* Sicherheitskontext

---

# 65. Entry Point

Der Entry Point beschreibt:

* Startadresse
* Aufrufkonvention
* ABI
* Stacklayout

---

# 66. Thread Exit

Ein Thread beendet sich über:

* Return
* Thread Exit API
* Ausnahme
* externe Terminierung

Alle Varianten führen letztlich nach `Terminating`.

---

# 67. Exit Handler

Vor dem endgültigen Ende werden ausgeführt:

* TLS Cleanup
* Runtime Cleanup
* Sprachruntime
* Thread Hooks

---

# 68. Join

Ein anderer Thread kann auf einen Thread warten.

Join liefert:

* Exitstatus
* Diagnose
* optionale Rückgabedaten

---

# 69. Join Ablauf

```text
Thread A
↓
Join
↓
Waiting
↓
Thread B beendet sich
↓
Join beendet
↓
Runnable
```

---

# 70. Detached Threads

Ein Thread kann als Detached markiert werden.

Dann:

* kein Join erforderlich
* automatische Freigabe
* Exitstatus optional verwerfbar

---

# 71. Cancellation

Ein Thread kann eine Abbruchanforderung erhalten.

Arten:

* kooperativ
* verzögert
* sofort

Die genaue Semantik wird separat spezifiziert.

---

# 72. Cancellation Points

Kooperative Abbruchpunkte können sein:

* System Calls
* Wait
* Sleep
* IPC
* Runtime Hooks

---

# 73. Ausnahmebehandlung

Ein Thread verarbeitet:

* Hardware Exceptions
* Runtime Exceptions
* Sprachruntime
* Debug Events

Nicht behandelte Ausnahmen führen zum Thread Exit oder Prozessfehler.

---

# 74. Debugger

Debugger können:

* Thread stoppen
* Thread starten
* Register lesen
* Register schreiben
* Stack lesen
* Breakpoints setzen

Alle Operationen benötigen Capabilities.

---

# 75. Breakpoints

Breakpoints erzeugen:

* Debug Event
* Scheduler Stop
* Exception Context

Danach entscheidet der Debugger über die Fortsetzung.

---

# 76. Single Stepping

Single Stepping wird vollständig über den Debug Service gesteuert.

Der Lifecycle bleibt unverändert.

---

# 77. Thread Handles

Thread Handles enthalten Rechte wie:

* Query
* Suspend
* Resume
* Join
* Debug
* Terminate

---

# 78. Capabilityprüfung

Vor jeder Threadoperation erfolgt:

* Handleprüfung
* Generation
* Capability
* Prozesszustand
* Threadzustand

---

# 79. Thread Object Graph

```text
Thread
├── belongs-to → Process
├── owns → Kernel Stack
├── owns → TLS
├── scheduled-by → Scheduler
└── executed-on → CPU
```

---

# 80. Event Core

Threadereignisse:

* Thread Created
* Thread Started
* Thread Suspended
* Thread Resumed
* Thread Waiting
* Thread Blocked
* Thread Exit
* Thread Destroyed

Alle Ereignisse sind typisiert.

Damit endet Teil 2.
