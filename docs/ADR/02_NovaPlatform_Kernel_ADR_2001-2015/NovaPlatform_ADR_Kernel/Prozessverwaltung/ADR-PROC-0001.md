# ADR-PROC-0001: Process Lifecycle

* **ADR-ID:** ADR-PROC-0001
* **Titel:** Process Lifecycle
* **Deutscher Titel:** Lebenszyklus von Prozessen
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-30
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Process Management
* **Gültigkeitsbereich:** Process Manager, Thread Manager, Scheduler, Kernel Object Manager, Memory Manager, Loader, Security Manager, IPC, Event Core, Service Manager, Diagnostic Core
* **Ersetzt:** Implizite und subsystemabhängige Prozesszustände
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105
* **Verwandte Dokumente:** ADR-MM-0004, ADR-MM-0005, ADR-MM-0006, ADR-MM-0008
* **Entscheidungstyp:** Architekturentscheidung
* **Sicherheitsrelevanz:** Kritisch
* **Kompatibilitätsrelevanz:** Hoch

---

# 1. Zusammenfassung

NovaOS definiert einen expliziten, typisierten und vollständig nachvollziehbaren Lebenszyklus für Prozesse.

Ein Prozess ist ein Kernelobjekt, das mindestens folgende Ressourcen und Beziehungen verwaltet:

* virtuellen Adressraum
* Threads
* Handle-Tabellen
* Capabilities
* Sicherheitskontext
* Namensraumzuordnung
* ausführbares Image
* IPC-Endpunkte
* Ressourcenlimits
* Telemetrie- und Diagnosekontext
* Eltern-, Kind- und Servicebeziehungen

Prozesse durchlaufen ausschließlich klar definierte Zustände. Jeder Zustandswechsel erfolgt über eine validierte Transition, erzeugt ein typisiertes Kernelereignis und wird durch die zuständigen Kernel-Services koordiniert.

---

# 2. Kontext

Ein Prozess ist eine der zentralen Ausführungseinheiten eines Betriebssystems.

Er verbindet:

* Programmcode
* Speicher
* Threads
* Sicherheitsrechte
* Ressourcen
* IPC
* Namensräume
* Systemdienste
* Diagnosedaten

Fehlt ein einheitliches Prozesslebenszyklusmodell, entstehen häufig:

* inkonsistente Prozesszustände
* Race Conditions beim Start und Beenden
* Ressourcenlecks
* unvollständig terminierte Threads
* verwaiste Handles
* unsichere Rechtevererbung
* unklare Eltern-Kind-Beziehungen
* schwer reproduzierbare Fehler
* unzureichende Recovery-Möglichkeiten

---

# 3. Problemstellung

Es musste entschieden werden,

* welche Zustände ein NovaOS-Prozess besitzt,
* wie Prozesse erstellt und gestartet werden,
* wann ein Prozess als sichtbar oder lauffähig gilt,
* wie Threads dem Prozess zugeordnet werden,
* wie Fehler während der Initialisierung behandelt werden,
* wie Prozesse pausiert, fortgesetzt und beendet werden,
* wie Ressourcen geordnet freigegeben werden,
* wie Eltern- und Kindprozesse behandelt werden,
* wie Prozessabstürze diagnostiziert werden,
* und wie der Lebenszyklus in Objektmodell, Event Core und Capability-System integriert wird.

---

# 4. Entscheidung

NovaOS verwendet einen expliziten Prozesszustandsautomaten.

Jeder Prozess besitzt:

* eine stabile Prozessobjekt-ID
* eine process-lokale PID
* eine Generationskennung
* einen definierten Lifecycle State
* einen Prozess-Lifecycle-Lock
* einen Sicherheitskontext
* eine Lifecycle Domain
* eine Ressourcenbeziehung im Kernel Object Graph

Zustandswechsel dürfen ausschließlich durch den Process Manager oder durch autorisierte versionierte Kernel-Services ausgeführt werden.

Direkte, unkoordinierte Änderungen des Prozesszustands durch andere Kernelkomponenten sind unzulässig.

---

# 5. Motivation

Das definierte Prozesslebenszyklusmodell ermöglicht:

* deterministische Zustandsübergänge
* vollständige Ressourcenkontrolle
* sichere Prozesserstellung
* geordnete Beendigung
* zuverlässige Diagnose
* klare Fehlergrenzen
* sichere Capability-Vererbung
* robuste IPC-Bereinigung
* kontrolliertes Self-Healing
* stabile ABI-Semantik

---

# 6. Nicht-Ziele

Diese ADR definiert nicht vollständig:

* das Scheduling einzelner Threads
* das Binärformat ausführbarer Dateien
* die konkrete System-Call-Nummerierung
* die vollständige POSIX-Prozesssemantik
* die vollständige Windows-Prozesssemantik
* die genaue Implementierung von Containern
* die konkrete Struktur des User Stacks
* das vollständige Signalmodell
* die vollständige Job-Control-Implementierung

Diese Themen werden in separaten ADRs und NPSPECs spezifiziert.

---

# 7. Designprinzipien

Der Process Lifecycle basiert auf:

* expliziten Zuständen
* validierten Übergängen
* atomaren Lebenszyklusoperationen
* Capability-basierter Autorisierung
* eindeutiger Ressourceneigentümerschaft
* geordnetem Aufräumen
* ereignisgesteuerter Koordination
* Fehlerisolation
* idempotenten Beendigungsabläufen
* lokaler Diagnose
* stabiler ABI-Semantik

---

# 8. Architekturübersicht

```text
Application / System Service
            │
            ▼
      Process Service
            │
            ▼
       Process Manager
            │
    ┌───────┼────────┬───────────┬──────────┐
    ▼       ▼        ▼           ▼          ▼
  Loader  Memory   Thread     Security      IPC
         Manager   Manager    Manager      Manager
    │       │        │           │          │
    └───────┴────────┴───────────┴──────────┘
                    │
                    ▼
             Process Object
                    │
                    ▼
          Kernel Object Graph
                    │
                    ▼
             Kernel Event Core
```

---

# 9. Prozessdefinition

Ein Prozess ist ein isolierter Ausführungskontext, der mindestens enthält:

* Process Object
* Address Space Object
* mindestens eine Handle Table
* Security Token oder Security Context
* Capability Set
* Namensraumreferenzen
* Thread-Liste
* Prozessparameter
* ausführbares Image
* Loader State
* Exit State
* Ressourcenabrechnung
* Diagnosekontext

Ein Prozess kann existieren, bevor er einen ausführbaren Thread besitzt.

---

# 10. Prozessidentität

Jeder Prozess besitzt mehrere Identifikatoren:

* systemweite Object ID
* pro Namensraum sichtbare Process ID
* Generationskennung
* optionale persistente Service Identity
* Parent Process ID
* Lifecycle Domain ID

Die Object ID ist die autoritative Objektidentität.

Die PID ist eine Namensraumreferenz und allein keine Zugriffsberechtigung.

---

# 11. PID und Generationsschutz

PIDs dürfen nach der vollständigen Freigabe eines Prozesses wiederverwendet werden.

Um Verwechslungen zu verhindern, verwendet NovaOS:

```text
Process Reference
├── PID
├── Generation
├── Namespace ID
└── Object ID
```

Ein Zugriff mit einer veralteten PID-Generation muss fehlschlagen.

---

# 12. Prozesszustände

NovaOS definiert mindestens folgende Prozesszustände:

* `Allocated`
* `Constructing`
* `Initializing`
* `Ready`
* `Starting`
* `Running`
* `Suspending`
* `Suspended`
* `Resuming`
* `Quiescing`
* `Exiting`
* `Exited`
* `Failed`
* `DestroyPending`
* `Destroyed`
* `Reclaimed`

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
    ├───────────────┐
    ▼               ▼
  Ready           Failed
    │               │
    ▼               ▼
 Starting        Exiting
    │               │
    ▼               ▼
 Running          Exited
    │               │
    ├───────────────┤
    │               ▼
    │         DestroyPending
    │               │
    ▼               ▼
Suspending       Destroyed
    │               │
    ▼               ▼
Suspended       Reclaimed
    │
    ▼
Resuming
    │
    ▼
Running

Running
    │
    ▼
Quiescing
    │
    ▼
Exiting
    │
    ▼
Exited
```

---

# 14. Zustand `Allocated`

Im Zustand `Allocated` wurde der grundlegende Speicher für das Process Object reserviert.

Vorhanden sind:

* Object Header
* Process Object ID
* Initiale Referenz
* Lifecycle Lock
* Basismetadaten

Noch nicht vorhanden sein müssen:

* Adressraum
* Sicherheitskontext
* Handle-Tabelle
* Loader State
* Threads

Ein Prozess im Zustand `Allocated` ist außerhalb des Process Managers nicht sichtbar.

---

# 15. Zustand `Constructing`

Im Zustand `Constructing` werden die grundlegenden Prozessressourcen angelegt.

Dazu gehören:

* Address Space Object
* Handle Table
* Capability Table
* Security Context
* Namespace Bindings
* Process Parameters
* Resource Accounting Context
* Event Channel
* Diagnostic Context

Fehler in diesem Zustand führen zu einem kontrollierten Rollback.

---

# 16. Zustand `Initializing`

Im Zustand `Initializing` wird das ausführbare Programm vorbereitet.

Vorgänge umfassen:

* Laden und Validieren des Executable Image
* Aufbau der virtuellen Speicherbereiche
* Laden benötigter Runtime-Komponenten
* Erstellen des User Stacks
* Anlegen des initialen Thread Objects
* Aufbau der Prozessparameter
* Capability-Übergabe
* Handle-Übergabe
* Initialisierung von IPC-Endpunkten
* Sicherheitsvalidierung

Der Prozess ist noch nicht ausführbar.

---

# 17. Zustand `Ready`

Ein Prozess im Zustand `Ready` ist vollständig initialisiert.

Es gelten:

* Executable Image ist geladen.
* Address Space ist gültig.
* initialer Thread ist vorbereitet.
* Sicherheitskontext ist aktiv.
* erforderliche Services sind verfügbar.
* verpflichtende Capabilities sind vorhanden.
* Loader hat den Entry Point bestimmt.
* Startparameter wurden validiert.

Der Prozess ist sichtbar, aber noch nicht im Userspace ausgeführt worden.

---

# 18. Zustand `Starting`

Im Zustand `Starting` wird der Prozess für die erste Ausführung vorbereitet.

Der Process Manager:

* veröffentlicht das Process Starting Event
* aktiviert initiale Handles
* finalisiert Startup Capabilities
* registriert den Prozess beim Scheduler
* setzt den initialen Thread auf `Ready`
* aktiviert Prozess-Telemetrie
* schließt den Loader-Startpfad ab

Der Zustand endet, sobald der erste Userspace-Instruktionskontext erreicht wird.

---

# 19. Zustand `Running`

Im Zustand `Running` ist der Prozess aktiv.

Ein Prozess gilt auch dann als `Running`, wenn aktuell alle seine Threads warten, solange der Prozess nicht suspendiert, beendet oder fehlgeschlagen ist.

Mögliche Threadzustände innerhalb eines laufenden Prozesses sind:

* Ready
* Running
* Waiting
* Blocked
* Sleeping
* Parked

Der Prozesszustand ist nicht identisch mit dem Zustand eines einzelnen Threads.

---

# 20. Zustand `Suspending`

`Suspending` ist ein Übergangszustand.

Der Process Manager:

* blockiert neue externe Operationen abhängig von der Suspend Policy
* stoppt oder parkt alle Threads
* friert asynchrone User-Callbacks ein
* koordiniert offene IPC-Operationen
* sichert erforderliche Prozesszustände
* wartet auf sichere Suspend Points

Der Übergang darf durch Timeout oder Fehler abgebrochen werden.

---

# 21. Zustand `Suspended`

Ein suspendierter Prozess führt keinen Userspace-Code aus.

Im Zustand `Suspended`:

* sind alle Threads gestoppt oder geparkt,
* bleiben Adressraum und Handles grundsätzlich erhalten,
* laufen keine normalen User-Callbacks,
* dürfen Kerneloperationen im Namen des Prozesses nur nach definierter Policy fortgeführt werden,
* können Diagnose- und Debugoperationen erlaubt bleiben.

Suspendierung beendet den Prozess nicht.

---

# 22. Zustand `Resuming`

Im Zustand `Resuming` wird ein suspendierter Prozess reaktiviert.

Der Process Manager:

* validiert den bestehenden Sicherheitskontext
* prüft widerrufene Capabilities
* aktualisiert Service Handles
* behandelt zwischenzeitlich entfernte Geräte
* reaktiviert Timer
* stellt Threadzustände wieder her
* veröffentlicht Resume Events

Anschließend wechselt der Prozess nach `Running`.

---

# 23. Zustand `Quiescing`

`Quiescing` dient der geordneten Stilllegung eines Prozesses.

Neue nicht notwendige Operationen werden abgewiesen.

Der Process Manager versucht:

* neue Threads zu verhindern
* neue Handle-Transfers zu blockieren
* neue IPC-Sitzungen zu verhindern
* offene Transaktionen abzuschließen
* Services über die bevorstehende Beendigung zu informieren
* Anwendungs-Exit-Handler auszuführen, sofern zulässig
* Ressourcen in einen freigabefähigen Zustand zu überführen

`Quiescing` ist zeitlich begrenzt.

---

# 24. Zustand `Exiting`

Im Zustand `Exiting` wird der Prozess zwangsweise oder regulär beendet.

Es werden:

* alle verbleibenden Threads terminiert,
* neue System Calls verhindert,
* IPC-Endpunkte geschlossen,
* Handles invalidiert oder übertragen,
* Shared-Memory-Beziehungen getrennt,
* Speicherabbildungen entfernt,
* Child Policies ausgeführt,
* Exit-Informationen gespeichert,
* Waiter benachrichtigt.

Der Übergang muss idempotent sein.

---

# 25. Zustand `Exited`

Ein Prozess im Zustand `Exited` führt keinen Code mehr aus und besitzt keine aktiven Threads.

Bestimmte Metadaten bleiben vorübergehend erhalten:

* Exit Code
* Exit Reason
* Endzeit
* Ressourcenstatistik
* Crash-Informationen
* Parent Notification State
* Audit-Referenzen
* Diagnoseinformationen

Dieser Zustand entspricht einem noch auswertbaren, aber nicht mehr ausführbaren Prozessobjekt.

---

# 26. Zustand `Failed`

Der Zustand `Failed` wird verwendet, wenn der Prozess vor oder während der Ausführung einen nicht regulär behebbaren Fehler erleidet.

Beispiele:

* Loader-Fehler
* ungültiges Executable Image
* fehlende Pflicht-Capability
* nicht unterstützte ABI
* schwerer Prozessfehler
* Sicherheitsverletzung
* irreparabler Adressraumfehler
* fehlgeschlagene Initialisierung

Ein Prozess in `Failed` muss anschließend nach `Exiting` überführt werden.

---

# 27. Zustand `DestroyPending`

Im Zustand `DestroyPending` ist der Prozess beendet, besitzt jedoch noch externe Referenzen.

Mögliche Referenzen sind:

* Wait Handles
* Debugger Handles
* Parent-Process-Referenzen
* Diagnoseobjekte
* Audit-Referenzen
* Kernel Object Graph Edges
* Service-Recovery-Referenzen

Neue operative Handles dürfen nicht mehr erzeugt werden.

---

# 28. Zustand `Destroyed`

Im Zustand `Destroyed` wurden alle prozessspezifischen Ressourcen freigegeben.

Nicht mehr vorhanden sind:

* Address Space
* Threads
* Handle Tables
* Capability Tables
* IPC Endpoints
* Loader State
* User Stacks
* Prozessprivate Kernelressourcen

Nur der minimale Object Header kann noch bis zum Ende der letzten Referenz existieren.

---

# 29. Zustand `Reclaimed`

Im Zustand `Reclaimed` wurde auch der Speicher des Process Objects freigegeben.

Die Prozessidentität darf nur noch in historischen Diagnose-, Audit- oder Telemetriedaten vorkommen.

Die PID kann danach mit neuer Generationskennung wiederverwendet werden.

---

# 30. Zulässige Zustandsübergänge

Zulässige Hauptübergänge sind:

```text
Allocated      -> Constructing
Constructing   -> Initializing
Constructing   -> Failed
Initializing   -> Ready
Initializing   -> Failed
Ready          -> Starting
Ready          -> Exiting
Starting       -> Running
Starting       -> Failed
Running        -> Suspending
Running        -> Quiescing
Running        -> Exiting
Running        -> Failed
Suspending     -> Suspended
Suspending     -> Running
Suspending     -> Exiting
Suspended      -> Resuming
Suspended      -> Exiting
Resuming       -> Running
Resuming       -> Failed
Quiescing      -> Exiting
Quiescing      -> Running
Failed         -> Exiting
Exiting        -> Exited
Exited         -> DestroyPending
DestroyPending -> Destroyed
Destroyed      -> Reclaimed
```

Andere Übergänge sind unzulässig.

---

# 31. Transition Validation

Jeder Zustandswechsel wird durch den Process Manager validiert.

Die Prüfung umfasst:

* aktuellen Zustand
* Zielzustand
* anfordernde Capability
* aktive Lifecycle Operation
* offene Threadoperationen
* Ressourcenstatus
* Sicherheitsrichtlinie
* Namensraumstatus
* Serviceabhängigkeiten
* Zeitüberschreitungen

Ein ungültiger Übergang liefert einen definierten Fehler.

---

# 32. Atomare Zustandsänderung

Der sichtbare Process State wird atomar geändert.

Der Übergang besteht logisch aus:

```text
Transition Request
        │
        ▼
Lifecycle Lock
        │
        ▼
Precondition Validation
        │
        ▼
State = Transitional State
        │
        ▼
Lifecycle Operations
        │
        ▼
State = Target State
        │
        ▼
Event Publication
```

Lange Arbeiten dürfen nicht unter einem globalen Prozess-Lock ausgeführt werden.

---

# 33. Process Manager

Der Process Manager ist autoritativ für:

* Prozesserstellung
* Prozesszustände
* Eltern-Kind-Beziehungen
* Prozessbeendigung
* Prozesssuspendierung
* Exit-Status
* Prozessregistrierung
* Prozessenumeration
* Prozess-Reaping
* Lifecycle Events

Andere Komponenten dürfen Prozesszustände nur über definierte Process-Service-Operationen anfordern.

---

# 34. Process Service

Der versionierte Process Service stellt mindestens bereit:

```text
process_create()
process_start()
process_suspend()
process_resume()
process_terminate()
process_wait()
process_query()
process_query_exit()
process_duplicate_handle()
process_subscribe_events()
```

Die endgültigen Operations-IDs werden in einer separaten NPSPEC festgelegt.

---

# 35. Prozesserstellung

Die Prozesserstellung ist eine transaktionale Operation.

Eingaben umfassen mindestens:

* Executable Handle
* Process Manifest
* Parent Process Handle
* Security Context
* Namespace Configuration
* Startup Capabilities
* Startup Handles
* Environment Descriptor
* Resource Limits
* Start Flags

Die Erstellung liefert zunächst ein Process Handle.

---

# 36. Erstellungsablauf

```text
Process Create Request
          │
          ▼
Capability Validation
          │
          ▼
Allocate Process Object
          │
          ▼
Create Address Space
          │
          ▼
Create Security Context
          │
          ▼
Create Handle Tables
          │
          ▼
Load Executable
          │
          ▼
Create Initial Thread
          │
          ▼
Commit Process Creation
          │
          ▼
Process Ready
```

---

# 37. Erstellungstransaktion

Die Erstellung gilt erst als erfolgreich, wenn alle verpflichtenden Teilschritte abgeschlossen sind.

Bei Fehlern erfolgt Rollback in umgekehrter Reihenfolge:

```text
Initial Thread entfernen
        │
        ▼
Executable Unmap
        │
        ▼
Handles schließen
        │
        ▼
Capabilities widerrufen
        │
        ▼
Address Space zerstören
        │
        ▼
Process Object freigeben
```

Teilweise sichtbare Prozesse sind zu vermeiden.

---

# 38. Prozessmanifest

Das Process Manifest beschreibt:

* Executable Identity
* benötigte Kernel-Services
* minimale ABI-Versionen
* benötigte Capabilities
* optionale Capabilities
* Ressourcenlimits
* Sandboxprofil
* Namensräume
* Kompatibilitätsmodus
* Startverhalten
* Fehlerpolitik

Das Manifest wird vor dem Zustand `Ready` validiert.

---

# 39. Executable Image

Das Executable Image ist ein eigenständiges Objekt.

Der Prozess besitzt eine Beziehung:

```text
Process
   └── executes ──> Executable Image
```

Das Image kann enthalten:

* Code Segments
* Data Segments
* Relocation Information
* Entry Point
* ABI Metadata
* Digital Signature
* Required Services
* Runtime Metadata

---

# 40. Loader-Integration

Der Loader ist für folgende Schritte verantwortlich:

* Executable Format erkennen
* Signatur prüfen
* ABI-Profil prüfen
* Segmente validieren
* Segmente abbilden
* Relocations anwenden
* Runtime laden
* Entry Point bestimmen
* Prozessparameter vorbereiten

Der Loader darf den Prozess nicht direkt nach `Running` versetzen.

---

# 41. Address Space

Jeder normale Prozess besitzt genau einen primären virtuellen Adressraum.

Ausnahmen können durch spezielle Prozessklassen definiert werden.

Der Address Space enthält:

* Code Regions
* Data Regions
* Heap Regions
* Stack Regions
* Shared Memory Mappings
* Runtime Mappings
* Guard Pages
* Memory-Mapped Files

---

# 42. Initialer Thread

Der initiale Thread wird während `Initializing` erzeugt.

Er besitzt:

* User Entry Point
* User Stack
* Kernel Stack
* Initial Register Context
* Thread Security Context
* Prozessreferenz
* Schedulerparameter
* Startup Block

Er darf erst im Zustand `Starting` ausführbar gemacht werden.

---

# 43. Mehrere Threads

Ein Prozess kann mehrere Threads besitzen.

Neue Threads dürfen grundsätzlich nur erstellt werden, wenn der Prozess:

* `Running`
* oder ausdrücklich `Starting`

ist.

Im Zustand `Quiescing`, `Exiting`, `Exited` oder später ist die Thread-Erstellung unzulässig.

---

# 44. Thread-zu-Prozess-Beziehung

```text
Process
  ├── owns ──> Thread 1
  ├── owns ──> Thread 2
  └── owns ──> Thread 3
```

Jeder Thread gehört genau einem Prozess.

Kernel Worker ohne normalen Userspace-Prozess werden einer dedizierten Kernel Lifecycle Domain zugeordnet.

---

# 45. Prozessstart

Ein neuer Prozess wird nicht automatisch unmittelbar nach der Erstellung gestartet.

Der getrennte Start ermöglicht:

* zusätzliche Handle-Übergabe
* Debugger-Anbindung
* Policy-Prüfung
* Service-Binding
* Startreihenfolgen
* transaktionale Gruppenstarts
* initiale Suspendierung

---

# 46. Startflags

Mögliche Startflags sind:

* Start Immediately
* Start Suspended
* Debug on Start
* Isolated Startup
* Service Process
* Critical System Process
* Compatibility Mode
* Restricted Network
* Restricted Storage
* No Child Processes

Die genaue Semantik wird separat spezifiziert.

---

# 47. Prozesssuspendierung

Eine Suspend-Anforderung benötigt eine Capability wie:

* Process Suspend
* Debug
* Administer
* Lifecycle Control

Der Prozess selbst darf sich über eine definierte Operation suspendieren, sofern keine Sicherheitsrichtlinie dies verhindert.

---

# 48. Suspend Points

Threads werden bevorzugt an sicheren Suspend Points angehalten.

Geeignete Punkte sind:

* System-Call-Grenzen
* Schedulerwechsel
* explizite Safe Points
* Wait-Zustände
* kontrollierte User-Runtime-Punkte

Ein erzwungener Stop ist bei Timeout möglich, muss aber diagnostiziert werden.

---

# 49. Kritische Kerneloperationen

Ein Thread darf nicht unsicher suspendiert werden, während er:

* einen nicht unterbrechbaren Kernel-Lock hält,
* eine kritische Speicheroperation ausführt,
* eine Hardwaretransaktion kontrolliert,
* einen nicht wiederholbaren Commit durchführt,
* einen Lifecycle State aktualisiert.

In diesen Fällen wird die Suspendierung verzögert oder abgebrochen.

---

# 50. Prozessfortsetzung

Beim Fortsetzen werden alle Threadzustände validiert.

Zu prüfen sind:

* widerrufene Handles
* entfernte Shared-Memory-Objekte
* veränderte Service-Versionen
* entfernte Geräte
* abgelaufene Capabilities
* geänderte Sicherheitsrichtlinien
* überfällige Timer
* abgebrochene IPC-Operationen

---

# 51. Reguläre Prozessbeendigung

Ein Prozess kann sich selbst regulär beenden.

Er übermittelt:

* Exit Code
* optionale Exit Category
* optionale Diagnoseinformationen
* optionalen Restart Hint

Der Exit Code ist Teil des Process Exit Record.

---

# 52. Externe Prozessbeendigung

Ein anderer Prozess darf eine Beendigung nur mit entsprechender Capability anfordern.

Beispielhafte Rechte:

* Terminate
* Administer
* Debug
* Service Control

Ein Prozessname oder eine PID allein berechtigt nicht zur Beendigung.

---

# 53. Graceful Termination

Bei einer geordneten Beendigung wird zuerst `Quiescing` verwendet.

Dabei erhält die Anwendung optional:

* Exit Request Event
* definierte Grace Period
* Möglichkeit zum Speichern
* Möglichkeit zum Schließen eigener Ressourcen
* Möglichkeit zur Rückmeldung

Nach Ablauf der Frist wechselt der Prozess nach `Exiting`.

---

# 54. Forced Termination

Eine erzwungene Beendigung überspringt anwendungsseitige Cleanup-Handler.

Sie wird verwendet bei:

* Sicherheitsverletzungen
* Prozess-Deadlock
* Ressourcenmissbrauch
* Timeout
* Administratoranordnung
* System-Shutdown
* irreparablem Fehler

Kernelressourcen müssen dennoch vollständig freigegeben werden.

---

# 55. Exit Reason

NovaOS unterscheidet mindestens:

* Normal Exit
* Self Termination
* External Termination
* Unhandled Exception
* Security Violation
* Resource Limit
* Loader Failure
* Service Failure
* Watchdog Timeout
* System Shutdown
* Kernel Recovery
* Compatibility Failure

---

# 56. Exit Code

Der Exit Code besteht logisch aus:

```text
Exit Status
├── Exit Domain
├── Exit Reason
├── Application Code
├── Severity
└── Flags
```

Kompatibilitätsschichten können daraus POSIX- oder Windows-kompatible Exitcodes ableiten.

---

# 57. Exit Record

Nach Prozessende wird ein unveränderlicher Exit Record erstellt.

Er enthält:

* Process Object ID
* PID und Generation
* Startzeit
* Endzeit
* Exit Reason
* Exit Code
* CPU-Zeit
* Speichermaximum
* I/O-Statistik
* letzter Sicherheitsstatus
* Crash Report Handle
* Parent Notification State

---

# 58. Prozessabsturz

Ein Prozessabsturz entsteht durch eine nicht behandelte fatale Ausnahme oder eine irreparable Runtime-Verletzung.

Ablauf:

```text
Fatal Exception
      │
      ▼
Exception Event
      │
      ▼
Recovery / Debug Policy
      │
      ├── handled ──> Resume
      │
      └── unhandled
               │
               ▼
             Failed
               │
               ▼
             Exiting
```

---

# 59. Crash-Diagnose

Bei einem Absturz können lokal erfasst werden:

* Thread Register
* Fault Address
* Exception Type
* User Stack Trace
* relevante Kernel Call Chain
* geladene Images
* Process Object Graph View
* offene Handles
* Capability State
* letzte Ereignisse
* Speicherstatistik

Sensible Speicherinhalte werden nicht standardmäßig vollständig gespeichert.

---

# 60. Debugger-Integration

Ein autorisierter Debugger kann:

* Start Suspended verwenden
* Prozessereignisse abonnieren
* Threads stoppen und fortsetzen
* Register lesen und schreiben
* Speicher lesen und abhängig von Rechten schreiben
* Exceptions abfangen
* Exit Records lesen

Debuggerrechte werden durch Capabilities begrenzt.

---

# 61. Eltern-Kind-Beziehung

Ein neu erstellter Prozess kann eine Parent-Beziehung besitzen.

```text
Parent Process
      │
      └── created ──> Child Process
```

Die Beziehung ist eine Lifecycle- und Diagnosebeziehung, keine automatische Berechtigungsübertragung.

---

# 62. Rechte des Elternprozesses

Ein Elternprozess erhält nur explizit definierte Rechte auf das Kind.

Mögliche Rechte:

* Query
* Wait
* Observe Exit
* Terminate
* Suspend
* Transfer Handles

Die konkreten Rechte ergeben sich aus dem Create-Aufruf und der Sicherheitsrichtlinie.

---

# 63. Vererbung

NovaOS verwendet keine unkontrollierte implizite Vererbung.

Vererbbar sind nur explizit markierte:

* Handles
* Capabilities
* Namespace Bindings
* Umgebungsdaten
* Resource Limits
* Kompatibilitätsprofile

Jede Vererbung kann Rechte abschwächen.

---

# 64. Kindprozessbeendigung

Beim Beenden eines Kindprozesses:

* wird der Parent benachrichtigt,
* wird der Exit Record verfügbar,
* werden Wait Objects signalisiert,
* wird die Parent-Child-Beziehung aktualisiert,
* kann eine Restart Policy ausgelöst werden.

Der Parent muss den Prozess nicht manuell „freigeben“, um Kernelressourcen zu bereinigen.

---

# 65. Verwaiste Prozesse

Beendet sich ein Parent vor seinen Kindern, bestimmt die Lifecycle Policy das weitere Verhalten.

Mögliche Strategien:

* Kinder weiterlaufen lassen
* Kinder einer Service Domain zuordnen
* Kinder an einen System Supervisor übertragen
* Kinder beenden
* Kinder suspendieren

Die Strategie wird pro Prozess oder Lifecycle Domain definiert.

---

# 66. Prozessgruppen

Prozesse können zu Process Groups gehören.

Eine Gruppe kann verwendet werden für:

* gemeinsames Lifecycle Management
* Ressourcenlimits
* Event Broadcasting
* Terminal- oder Sitzungssteuerung
* Shutdown-Reihenfolge
* Diagnose
* Kompatibilitätsschichten

Eine Prozessgruppe ist selbst ein Kernelobjekt.

---

# 67. Lifecycle Domains

Eine Lifecycle Domain gruppiert abhängige Prozesse und Ressourcen.

Beispiele:

* Anwendung mit Hilfsprozessen
* Systemdienst mit Workerprozessen
* virtuelle Maschine mit Geräteprozessen
* UI-Sitzung
* Container
* Treiber-Servicegruppe

Eine Domain definiert:

* Startreihenfolge
* Stoppreihenfolge
* Restart Policy
* Fehlerausbreitung
* Ressourcenlimits
* Recovery-Verhalten

---

# 68. Serviceprozesse

Systemdienste sind Prozesse mit zusätzlicher Service Identity.

Sie besitzen:

* Service Object
* Health State
* Restart Policy
* Dependency Graph
* Service Endpoints
* Lifecycle Domain
* privilegierte, aber minimale Capabilities

Ein Serviceprozess ist kein Sondertyp außerhalb des Prozessmodells.

---

# 69. Kritische Systemprozesse

Bestimmte Prozesse können als systemkritisch klassifiziert werden.

Das bedeutet nicht uneingeschränkte Rechte.

Stattdessen gelten:

* besondere Restart Policy
* höhere Diagnosepriorität
* reservierte Ressourcen
* kontrollierte Shutdown-Reihenfolge
* strengere Signaturprüfung
* eingeschränkte Debugbarkeit

---

# 70. Prozesshandles

Ein Process Handle repräsentiert eine Capability auf ein Process Object.

Mögliche Rechte sind:

* Query
* Observe
* Wait
* Suspend
* Resume
* Terminate
* Debug
* Duplicate
* Transfer
* Administer

Ein Handle ist generationgesichert und widerrufbar.

---

# 71. Handle-Tabelle des Prozesses

Jeder Prozess besitzt eine oder mehrere Handle-Tabellen.

Beim Prozessende werden Handles:

* geschlossen,
* gegebenenfalls übertragen,
* widerrufen,
* oder entsprechend ihrer Policy persistent an einen Service Broker übergeben.

Implizit globale Handles sind unzulässig.

---

# 72. Capability-Lebenszyklus

Capabilities eines Prozesses können:

* beim Start gewährt,
* zur Laufzeit delegiert,
* abgeschwächt,
* zeitlich begrenzt,
* widerrufen,
* beim Exit automatisch entfernt

werden.

Beim Übergang nach `Exiting` werden neue Delegationen grundsätzlich blockiert.

---

# 73. Sicherheitskontext

Der Sicherheitskontext eines Prozesses enthält:

* Identity Reference
* Capability Set
* Sandbox Profile
* Integrity Level
* Namespace Scope
* Audit Context
* Trust State
* Executable Identity
* Policy Version

Der Sicherheitskontext ist selbst ein geschütztes Kernelobjekt.

---

# 74. Prozessisolation

Jeder normale Prozess besitzt:

* eigenen virtuellen Adressraum
* eigene Handle-Tabellen
* eigene Capability-Sicht
* eigene Fehlerdomäne
* gefilterte Objektgraphansicht
* getrennte Userspace-Ausführung

Speicher wird nur über explizite Shared-Memory-Objekte geteilt.

---

# 75. Shared Memory

Shared Memory zwischen Prozessen benötigt:

* Shared Memory Object
* Mapping Capability
* definierte Zugriffsrechte
* Lebenszykluspolicy
* Synchronisationsmodell
* Widerrufsverhalten

Beim Prozessende wird nur das Mapping des beendeten Prozesses entfernt.

---

# 76. IPC-Lebenszyklus

IPC-Endpunkte sind Prozessressourcen oder eigenständige Serviceobjekte.

Beim Prozessende:

* werden private Endpunkte geschlossen,
* werden wartende Peers benachrichtigt,
* werden Nachrichten abhängig von der Policy verworfen oder zugestellt,
* werden Shared Buffer freigegeben,
* werden Service Endpoints gegebenenfalls neu gebunden.

---

# 77. Dateisystemressourcen

Offene Dateien und Verzeichnisse werden über Handles verwaltet.

Beim Prozessende werden:

* nicht übertragene Handles geschlossen,
* ausstehende I/O-Operationen abgeschlossen oder abgebrochen,
* Dateisperren freigegeben,
* transaktionale Änderungen nach definierter Policy committed oder zurückgerollt,
* asynchrone Completion Objects signalisiert.

---

# 78. Netzwerkressourcen

Netzwerkressourcen werden beim Prozessende entsprechend ihrer Policy behandelt.

Mögliche Strategien:

* Verbindung sofort schließen
* geordnetes Shutdown
* Listener an Service Broker übertragen
* ausstehende Daten verwerfen
* Verbindungen für Recovery halten

Die Standardstrategie ist das sichere Schließen.

---

# 79. Timer

Prozessgebundene Timer werden beim Exit:

* abgebrochen,
* ausgelöst,
* übertragen,
* oder in Service-Timer umgewandelt,

abhängig von ihrem Typ.

Ein gewöhnlicher Anwendungstimer wird standardmäßig abgebrochen.

---

# 80. Ressourcenlimits

Ein Prozess kann Limits besitzen für:

* virtuellen Speicher
* physischen Speicher
* CPU-Zeit
* Threadanzahl
* Handle-Anzahl
* IPC-Queues
* offene Dateien
* Netzwerkverbindungen
* GPU-Ressourcen
* Prozesskinder

Das Überschreiten eines Limits kann zu Drosselung, Fehler oder Beendigung führen.

---

# 81. Ressourcenabrechnung

Der Process Manager erfasst mindestens:

* CPU-Zeit
* Kontextwechsel
* Speicherverbrauch
* Page Faults
* I/O-Volumen
* IPC-Volumen
* Netzwerkvolumen
* Handle-Anzahl
* Threadanzahl
* Laufzeit

Die Daten verbleiben standardmäßig lokal.

---

# 82. Scheduler-Integration

Der Scheduler verwaltet Threads, nicht Prozesse.

Der Process Manager stellt dem Scheduler jedoch bereit:

* Process Object Reference
* Scheduling Domain
* Resource Budget
* Priority Policy
* CPU Affinity Policy
* Lifecycle State

Threads eines suspendierten oder beendeten Prozesses dürfen nicht eingeplant werden.

---

# 83. Memory-Manager-Integration

Der Memory Manager erhält Prozessereignisse für:

* Address Space Creation
* Process Start
* Suspend
* Resume
* Exit
* Destruction

Beim Exit muss der Adressraum erst zerstört werden, wenn keine aktiven Threadkontexte mehr darauf zugreifen.

---

# 84. Kernel Object Graph

Ein Prozess wird im Kernel Object Graph beispielsweise wie folgt dargestellt:

```text
Process
├── owns ───────────> Address Space
├── owns ───────────> Handle Table
├── owns ───────────> Thread
├── secured-by ─────> Security Context
├── executes ───────> Executable Image
├── member-of ──────> Lifecycle Domain
├── belongs-to ─────> Namespace
├── communicates ───> IPC Endpoint
└── created-by ─────> Parent Process
```

---

# 85. Event-Core-Integration

Jeder relevante Zustandswechsel erzeugt ein Ereignis.

Mindestens vorgesehen sind:

* Process Allocated
* Process Constructing
* Process Initialized
* Process Ready
* Process Starting
* Process Started
* Process Suspending
* Process Suspended
* Process Resuming
* Process Resumed
* Process Quiescing
* Process Exiting
* Process Exited
* Process Failed
* Process Destroyed

---

# 86. Ereigniskausalität

Prozessereignisse enthalten:

* Process Object ID
* PID und Generation
* Parent Process ID
* Lifecycle Domain ID
* alte Zustandskennung
* neue Zustandskennung
* Auslöser
* Korrelations-ID
* Sicherheitskontext
* Zeitstempel
* Exit Reason, sofern vorhanden

---

# 87. Ereignisreihenfolge

Für ein einzelnes Process Object gilt kausale Reihenfolge.

Beispiel:

```text
PROCESS_READY
      │
      ▼
PROCESS_STARTING
      │
      ▼
PROCESS_STARTED
      │
      ▼
PROCESS_EXITING
      │
      ▼
PROCESS_EXITED
```

Ein späteres Lifecycle Event darf nicht vor einem kausal vorherigen Ereignis sichtbar werden.

---

# 88. Warten auf Prozesse

Process Objects implementieren das Interface `Waitable`.

Ein Wait kann ausgelöst werden bei:

* Exited
* Failed
* Suspended
* bestimmtem Lifecycle State
* Exit Record verfügbar
* Service Health Change

Standardmäßig signalisiert ein Process Wait Handle beim Zustand `Exited`.

---

# 89. Prozessenumeration

Prozessenumeration erfolgt über gefilterte Objektansichten.

Ein Client sieht nur Prozesse, für die er mindestens eine passende Capability oder Sichtbarkeitsberechtigung besitzt.

Eine globale, unbeschränkte Prozessliste ist privilegiert.

---

# 90. Prozessabfrage

Abfragbare Informationen hängen von den Rechten ab.

Mögliche Felder:

* PID
* Name
* Executable Identity
* Lifecycle State
* Startzeit
* Parent
* Threadanzahl
* Ressourcenverbrauch
* Sicherheitsklasse
* Service Identity
* Exit Record

Sensible Daten werden gefiltert.

---

# 91. Prozessname

Der Prozessname ist keine Identität und keine Autoritätsquelle.

Er dient ausschließlich:

* Darstellung
* Diagnose
* Benutzeroberfläche
* Kompatibilität

Mehrere Prozesse dürfen denselben Namen besitzen.

---

# 92. Prozessklassen

NovaOS kann unterschiedliche Prozessklassen definieren:

* User Application
* System Service
* Driver Service
* Compatibility Process
* Sandbox Process
* Container Init
* Virtual Machine Monitor
* Diagnostic Process
* Recovery Process

Alle Klassen verwenden denselben grundlegenden Lifecycle.

---

# 93. Sandbox-Prozesse

Ein Sandbox-Prozess erhält:

* minimale Startup Capabilities
* gefilterte Servicesicht
* eingeschränkte Namensräume
* definierte Ressourcenlimits
* eingeschränkte Prozesskind-Erstellung
* kontrollierte IPC-Ziele
* eingeschränkte Diagnosezugriffe

Der Lifecycle bleibt identisch.

---

# 94. Container-Prozesse

Containerprozesse besitzen:

* containerlokale PID
* systemweite Process Object ID
* Container Namespace
* Container Lifecycle Domain
* virtuelle Prozesssicht
* eigene Ressourcenlimits

Das Beenden eines Container-Init-Prozesses kann abhängig von der Policy den gesamten Container beenden.

---

# 95. Kompatibilitätsschichten

Fremde Prozessmodelle werden auf den nativen Lifecycle abgebildet.

Beispiele:

* POSIX `fork` und `exec`
* Windows `CreateProcess`
* Linux `clone`
* Android Process Spawn
* Web Runtime Worker

Die native Prozessidentität und Sicherheitssemantik bleiben erhalten.

---

# 96. POSIX-`fork`

Ein vollständiges `fork` ist kein zwingender nativer Kernmechanismus.

Eine POSIX-Schicht kann `fork` abbilden durch:

* neues Process Object
* Copy-on-Write Address Space
* duplizierte und abgeschwächte Handles
* replizierten Threadkontext
* POSIX-kompatible Parent-Child-Semantik

Der native bevorzugte Mechanismus bleibt `create` plus explizite Ressourcenübergabe.

---

# 97. `exec`-Semantik

Ein `exec`-ähnlicher Vorgang kann als Image Replacement Service implementiert werden.

Dabei werden:

* alte Threads bis auf den aufrufenden Kontext beendet,
* Address Space ersetzt,
* ausgewählte Handles beibehalten,
* Capabilities neu bewertet,
* Executable Identity geändert,
* Lifecycle Events erzeugt.

Ein Image Replacement ist nicht identisch mit einer neuen Process Object ID, sofern die Kompatibilitätsschicht dies verlangt.

---

# 98. Windows-Kompatibilität

Windows-kompatible Prozesshandles werden auf native Process Handles abgebildet.

Die Kompatibilitätsschicht übersetzt:

* Access Masks
* Process Creation Flags
* Inherited Handles
* Suspended Start
* Job Objects
* Exit Codes
* Wait Semantics

---

# 99. Prozess-Neustart

Ein beendeter Prozess wird nicht in derselben Instanz neu gestartet.

Ein Restart erzeugt grundsätzlich:

* neues Process Object
* neue Object ID
* neue PID-Generation
* neuen Adressraum
* neue Handle-Tabellen
* neuen Sicherheitskontext

Eine stabile Service Identity kann auf die neue Instanz zeigen.

---

# 100. Self-Healing

Self-Healing kann auf Prozessfehler reagieren durch:

* Neustart
* isolierten Neustart
* Capability-Reduktion
* Wechsel eines Service Backends
* Sperrung eines Executable Images
* Diagnosemodus
* Rückkehr zu einer bekannten Version
* Lifecycle-Domain-Recovery

Die Entscheidung erfolgt anhand einer definierten Recovery Policy.

---

# 101. Restart Policy

Mögliche Restart Policies sind:

* Never
* On Failure
* On Crash
* Always
* Limited Retry
* Exponential Backoff
* Manual
* Replace with Fallback

Eine Policy enthält:

* maximale Versuche
* Zeitfenster
* Backoff
* Abbruchbedingungen
* Abhängigkeitsverhalten
* Diagnoseanforderungen

---

# 102. Prozess-Watchdog

Ein autorisierter Watchdog kann einen Prozess überwachen.

Überwacht werden können:

* Heartbeat
* Event Loop Progress
* IPC Response
* CPU-Hang
* Deadlock-Indikatoren
* Speicherwachstum
* Service Health

Ein Watchdog darf nur über eine entsprechende Capability eingreifen.

---

# 103. Deadlock-Behandlung

Wird ein Prozessdeadlock erkannt, kann NovaOS:

* Diagnosedaten erfassen
* einzelne Threads untersuchen
* den Prozess suspendieren
* einen Debugger anbinden
* Ressourcenbeziehungen analysieren
* den Prozess beenden
* die Lifecycle Domain wiederherstellen

Automatische Beendigung ist policyabhängig.

---

# 104. System-Shutdown

Beim System-Shutdown werden Prozesse in definierter Reihenfolge beendet.

```text
User Applications
        │
        ▼
Background Services
        │
        ▼
Session Services
        │
        ▼
System Services
        │
        ▼
Storage / Recovery Services
```

Kritische Dienste werden zuletzt beendet.

---

# 105. Fast Shutdown

Bei einem kritischen Fast Shutdown kann NovaOS:

* Grace Periods verkürzen
* Anwendungs-Callbacks überspringen
* nur persistenzkritische Dienste quiescen
* verbleibende Prozesse zwangsweise terminieren

Dateisystem- und Integritätsgarantien müssen dennoch beachtet werden.

---

# 106. Ruhezustand

Bei einem systemweiten Ruhezustand werden Prozesse nicht beendet.

Stattdessen werden:

* Prozesszustände eingefroren
* Speicherzustände persistiert
* Geräteabhängigkeiten markiert
* Servicebindungen für Resume vorbereitet
* Capabilities beim Resume erneut geprüft

---

# 107. Live-Update

Versionierte Kernel-Services ermöglichen Prozessfortbestand über Kernel-Service-Updates.

Ein Prozess kann weiterlaufen, wenn:

* seine Service-Version weiter unterstützt wird,
* Handles neu gebunden werden können,
* Capability-Semantik erhalten bleibt,
* offene Operationen migriert oder abgeschlossen werden.

---

# 108. Sicherheit bei Prozessstart

Vor dem Start werden mindestens geprüft:

* Executable Signature
* Executable Identity
* ABI-Version
* Manifest
* Capabilities
* Resource Limits
* Namespace Policy
* Service Requirements
* Memory Permissions
* Runtime Integrity

Ein fehlgeschlagener Check verhindert den Übergang nach `Ready`.

---

# 109. Sicherheit bei Prozessende

Beim Prozessende muss verhindert werden:

* weitere Capability-Delegation
* neue Handle-Duplizierung
* neue Shared-Memory-Mappings
* neue Threads
* neue IPC-Sitzungen
* Nutzung bereits widerrufener Ressourcen
* Wiederbelebung des Prozessobjekts

---

# 110. Audit

Sicherheitsrelevante Lifecycle-Operationen können lokal protokolliert werden.

Beispiele:

* Process Create
* Process Start
* Capability Grant
* Suspend
* Resume
* External Terminate
* Security Failure
* Crash
* Restart
* Executable Replacement

---

# 111. Datenschutz

Prozessdiagnose kann sensible Informationen enthalten.

Daher gelten:

* lokale Verarbeitung
* Datenminimierung
* Capability-basierter Zugriff
* begrenzte Aufbewahrung
* keine automatische externe Übertragung
* Filterung von Speicherinhalten
* Pseudonymisierung von Benutzerreferenzen
* Schutz von Kommandozeilen- und Umgebungsdaten

---

# 112. Telemetrie

Der Process Manager erfasst lokal:

* Prozessanzahl
* Erstellungsrate
* Startlatenz
* Laufzeit
* Exitgründe
* Absturzrate
* Suspend- und Resume-Latenz
* Ressourcenverbrauch
* Restart-Vorgänge
* Lifecycle-Fehler

---

# 113. Diagnosewerkzeuge

NovaOS stellt für autorisierte Nutzer und Entwickler bereit:

* Process Explorer
* Process Object Graph View
* Handle Inspector
* Capability Inspector
* Thread Inspector
* Memory Map Viewer
* Lifecycle Trace
* Exit Record Viewer
* Crash Analyzer
* Service Dependency Viewer

---

# 114. Fehlerbehandlung

Process-Service-Operationen liefern definierte Fehler wie:

* Invalid State
* Invalid Transition
* Access Denied
* Process Gone
* Process Busy
* Timeout
* Loader Failure
* Missing Capability
* Missing Service
* Resource Limit
* Incompatible ABI
* Security Violation

---

# 115. Idempotenz

Folgende Operationen müssen soweit möglich idempotent sein:

* Terminate
* Close Process Handle
* Cancel Startup
* Revoke Process Capability
* Query Exit Record
* Destroy after Exited

Mehrfache Beendigungsanforderungen dürfen keine Ressourcen doppelt freigeben.

---

# 116. Synchronisation

Der Process Manager verwendet:

* atomare Zustandsfelder
* Lifecycle Lock
* Referenzzählung
* RCU für Prozessenumeration
* per-process Wait Queues
* geordnete Threadstop-Protokolle
* Completion Objects

Ein globaler Lock für alle Prozessoperationen ist zu vermeiden.

---

# 117. SMP-Unterstützung

Prozessoperationen müssen auf SMP-Systemen korrekt funktionieren.

Besonders zu beachten sind:

* Threads auf mehreren CPUs
* parallele System Calls
* gleichzeitige Beendigungsanforderungen
* CPU-übergreifende Suspendierung
* TLB-Shootdowns
* parallele Handleoperationen
* Prozessmigration zwischen Scheduling Domains

---

# 118. NUMA-Unterstützung

Ein Prozess kann NUMA-Policies besitzen für:

* bevorzugte Speicherknoten
* CPU-Domains
* Threadplatzierung
* Gerätezugriff
* Shared Memory
* Migration

Der Lifecycle selbst bleibt unabhängig von der NUMA-Platzierung.

---

# 119. Performanceziele

Der Prozesslebenszyklus soll:

* schnelle Prozessabfragen ermöglichen
* parallele Prozesserstellung unterstützen
* keine globale Schedulerblockierung verursachen
* kurze Startpfade ermöglichen
* geordnete, aber effiziente Beendigung erlauben
* geringe Kosten für Zustandsabfragen besitzen
* skalierbare Prozessenumeration bereitstellen

---

# 120. Optimierungen

Mögliche Optimierungen sind:

* Process Object Slab Cache
* vorinitialisierte Address-Space-Templates
* geteilte Runtime Images
* Copy-on-Write Startup Pages
* Per-CPU PID Allocation Cache
* Batch Handle Transfer
* Lazy Service Binding
* Startup Manifest Cache
* paralleles Image Mapping

---

# 121. Verbotene Muster

Nicht zulässig sind:

* direkte Änderung des Process State außerhalb des Process Managers
* Prozesszugriff allein über PID ohne Generation
* implizite Capability-Vererbung
* Start eines Prozesses vor abgeschlossener Sicherheitsprüfung
* Prozesszerstörung mit aktiven Threads
* Freigabe des Adressraums vor Threadstopp
* Wiederverwendung eines Process Objects für einen Neustart
* unkontrollierte globale Prozesslisten
* rohe Kernelpointer als Prozessreferenzen im Userspace
* doppelte Ressourcenfreigabe beim Exit

---

# 122. Verworfene Alternative: impliziter Prozesszustand

Vorteile:

* geringe anfängliche Implementierungskomplexität

Nachteile:

* schwer überprüfbare Zustände
* Race Conditions
* inkonsistente Fehlerbehandlung
* schlechte Diagnose
* unsichere Beendigung

---

# 123. Verworfene Alternative: Prozesszustand aus Threadzuständen ableiten

Vorteile:

* weniger eigene Prozessmetadaten

Nachteile:

* Prozesszustand ist nicht eindeutig
* Suspendierung schwer modellierbar
* Initialisierung ohne Threads nicht darstellbar
* Beendigung und Cleanup werden unklar
* Serviceprozesse schwer verwaltbar

---

# 124. Verworfene Alternative: sofortiger Start bei Erstellung

Vorteile:

* einfache API
* kurze konzeptionelle Abfolge

Nachteile:

* keine sichere vorbereitende Konfiguration
* erschwerte Debugger-Anbindung
* unsichere Handle-Übergabe
* keine transaktionale Gruppeninitialisierung
* unklare Fehlergrenze

---

# 125. Verworfene Alternative: ausschließlich zwangsweise Beendigung

Vorteile:

* einfache Implementierung
* kurze Exit-Pfade

Nachteile:

* Datenverlust
* unvollständige Transaktionen
* schlechte Serviceintegration
* unnötige Dateisystem- und IPC-Probleme
* ungeeignet für Systemdienste

---

# 126. Verworfene Alternative: Prozessobjekt nach Exit sofort freigeben

Vorteile:

* geringer Speicherverbrauch

Nachteile:

* Exitstatus kann verloren gehen
* Waiter verlieren Referenzen
* Diagnose wird erschwert
* Parent Notification wird unsicher
* Race Conditions beim Reaping

---

# 127. Risiken

Wesentliche Risiken sind:

* komplexer Zustandsautomat
* Deadlocks beim Suspendieren
* lange Quiescing-Phasen
* Ressourcenlecks bei Rollback
* Race Conditions zwischen Exit und IPC
* Handle- und Capability-Leaks
* zu große Exit Records
* nicht terminierbare Kerneloperationen
* fehlerhafte Parent-Child-Policies

---

# 128. Gegenmaßnahmen

Die Risiken werden reduziert durch:

* zentralen Process Manager
* formale Transition Table
* Timeouts
* idempotente Cleanup-Schritte
* Lifecycle Completion Objects
* Fehler-Injektionstests
* per-process Diagnose
* strikte Ownership-Regeln
* capability-basierte Prozesskontrolle
* automatisierte Ressourcenkontrollen

---

# 129. Implementierungsrichtlinien

Alle Prozessimplementierungen müssen:

* das definierte Zustandsmodell verwenden,
* Zustandswechsel über den Process Manager ausführen,
* Lifecycle Events erzeugen,
* PID-Generationen prüfen,
* Process Handles und Capabilities verwenden,
* Ressourcenbesitz dokumentieren,
* Exit-Abläufe idempotent gestalten,
* Rollback bei Initialisierungsfehlern unterstützen,
* Diagnosedaten lokal bereitstellen,
* Kernel Object Graph Beziehungen aktuell halten.

---

# 130. Implementierungsphasen

## Phase 1

* Process Object
* PID und Generation
* Address Space Binding
* Initial Thread
* Zustände `Allocated` bis `Exited`
* Process Handle
* Process Wait
* einfache Exit Records

## Phase 2

* Parent-Child-Beziehungen
* Suspend und Resume
* Capability-Vererbung
* Process Manifest
* Ressourcenlimits
* Lifecycle Events
* lokale Telemetrie

## Phase 3

* Lifecycle Domains
* Serviceprozesse
* Restart Policies
* Recovery
* Container-PIDs
* Kompatibilitätsschichten
* erweiterte Crash-Diagnose

## Phase 4

* Live-Update-Unterstützung
* transaktionale Prozessgruppen
* formale Lifecycle-Verifikation
* automatische Deadlockanalyse
* verteilte Serviceidentitäten

---

# 131. Teststrategie

Der Process Lifecycle wird geprüft durch:

* Unit-Tests
* Transition-Tests
* Loader-Tests
* Starttests
* Suspend- und Resume-Tests
* Exit-Tests
* Crash-Tests
* Rollback-Tests
* Capability-Tests
* Parent-Child-Tests
* SMP-Tests
* NUMA-Tests
* Langzeittests
* Fuzzing
* Performancebenchmarks

---

# 132. Zustandsübergangstests

Für jeden Zustand werden geprüft:

* alle erlaubten Übergänge
* alle verbotenen Übergänge
* parallele Übergangsanforderungen
* Timeout
* Abbruch
* Fehler während der Transition
* wiederholte Anforderung
* Ereignisreihenfolge
* Ressourcenstatus vor und nach dem Übergang

---

# 133. Erstellungs- und Rollbacktests

Geprüft werden Fehler bei:

* Process Object Allocation
* PID Allocation
* Address Space Creation
* Security Context Creation
* Handle Table Creation
* Image Loading
* Stack Creation
* Initial Thread Creation
* Service Binding
* Capability Transfer

Nach jedem Fehler dürfen keine Ressourcenlecks verbleiben.

---

# 134. Exit-Tests

Geprüft werden:

* normaler Exit
* externer Exit
* erzwungener Exit
* Exit mit mehreren Threads
* Exit während I/O
* Exit während IPC
* Exit während Suspend
* Exit während Startup
* parallele Terminate Requests
* Exit mit offenen Shared-Memory-Mappings

---

# 135. Sicherheitstests

Geprüft werden:

* gefälschte PID
* veraltete PID-Generation
* ungültige Process Handles
* fehlende Terminate Capability
* unzulässige Capability-Vererbung
* fremde Prozessabfrage
* unzulässige Debugger-Anbindung
* Handle-Transfer während Exit
* Namespace-Durchbruch
* Start unsignierter Images

---

# 136. SMP-Tests

SMP-Tests umfassen:

* Threads auf mehreren CPUs
* gleichzeitige System Calls beim Exit
* parallele Suspend Requests
* CPU Hotplug während Prozesslaufzeit
* Threadmigration während Suspend
* TLB-Shootdown beim Address-Space-Abbau
* parallele Handle-Tabellenzugriffe

---

# 137. Performanceprüfungen

Gemessen werden:

* Prozess-Erstellungslatenz
* Startlatenz
* Suspend-Latenz
* Resume-Latenz
* Exit-Latenz
* Prozessenumerationszeit
* Handle-Auflösungszeit
* Speicheraufwand pro Process Object
* Skalierung bei vielen Prozessen
* Parallelität bei Prozesserstellung

---

# 138. Akzeptanzkriterien

Die ADR gilt als umgesetzt, wenn:

* jeder Prozess ein Process Object besitzt,
* jeder Prozess einen expliziten Lifecycle State besitzt,
* nur zulässige Zustandsübergänge möglich sind,
* PID und Generation gemeinsam validiert werden,
* Prozesserstellung transaktional erfolgt,
* der initiale Thread erst nach erfolgreicher Initialisierung startet,
* Suspend und Resume alle Threads konsistent behandeln,
* Exit alle Prozessressourcen geordnet freigibt,
* Exit Records zuverlässig verfügbar bleiben,
* Process Events in korrekter Reihenfolge erzeugt werden,
* Process Handles capability-basiert autorisiert werden,
* keine aktiven Threads nach `Exited` verbleiben.

---

# 139. Konsequenzen für den Scheduler

Der Scheduler muss:

* Process Lifecycle States berücksichtigen,
* Threads suspendierter Prozesse aus dem Run Queue entfernen,
* Threads beendeter Prozesse niemals erneut einplanen,
* CPU-übergreifendes Stoppen unterstützen,
* Lifecycle Completion signalisieren,
* prozessbezogene Ressourcenbudgets beachten.

---

# 140. Konsequenzen für den Memory Manager

Der Memory Manager muss:

* Address Spaces transaktional erstellen,
* Rollback unterstützen,
* Mappings bei Exit entfernen,
* Shared Memory getrennt behandeln,
* aktive CPU-Nutzung vor dem Abbau verhindern,
* Crash-Diagnosezugriffe zeitlich begrenzt ermöglichen,
* Address Space Destruction bestätigen.

---

# 141. Konsequenzen für den Security Manager

Der Security Manager muss:

* Startup Capabilities validieren,
* Capability-Vererbung kontrollieren,
* Process Handles autorisieren,
* Executable Identity prüfen,
* Capability-Widerruf beim Exit ausführen,
* Sandbox- und Namespace-Regeln anwenden,
* sicherheitsbedingte Prozessbeendigung auslösen können.

---

# 142. Konsequenzen für IPC

Das IPC-System muss:

* Endpunkte an Process Objects binden,
* Peer Death Notifications unterstützen,
* IPC beim Exit kontrolliert abbrechen,
* Service-Rebinding ermöglichen,
* Handle-Transfers während `Exiting` blockieren,
* wartende Prozesse über Endpoint-Verlust informieren.

---

# 143. Konsequenzen für den Loader

Der Loader muss:

* vollständig rollbackfähig sein,
* den Prozess vor `Ready` nicht ausführen,
* ABI-Anforderungen prüfen,
* Manifest und Signatur validieren,
* User Stack und Entry Context definieren,
* Executable Image als Objekt referenzieren,
* Loader-Fehler in standardisierte Exit Reasons übersetzen.

---

# 144. Konsequenzen für Userspace

Userspace-Anwendungen erhalten:

* explizite Process Handles
* stabile Process-Service-Operationen
* definierte Start- und Exitsemantik
* Waitable Process Objects
* Process Events
* Exit Records
* capability-basierte Prozesskontrolle
* sichere Handle- und Capability-Übergabe

---

# 145. Konsequenzen für NovaLang

NovaLang kann Prozessoperationen als sichere Sprachabstraktionen darstellen.

Beispiel:

```text
let process = Process.create(
    executable: appImage,
    capabilities: startupCapabilities,
    startMode: ProcessStartMode.Suspended
)

process.start()

let result = await process.wait()
```

Die Runtime verwaltet dabei Process Handles und schließt sie deterministisch.

---

# 146. Dokumentationsanforderungen

Die Prozessspezifikation muss mindestens dokumentieren:

* Process Object Layout
* Lifecycle States
* Transition Table
* Process-Service-Version
* Operations-IDs
* Process Handle Rights
* Exit Reasons
* Exit Record
* Parent-Child-Policy
* Suspend- und Resume-Semantik
* Ressourcenfreigabereihenfolge
* Ereignistypen
* Sicherheitsanforderungen
* Testanforderungen

---

# 147. Zukunftsperspektive

Langfristig kann der Process Lifecycle erweitert werden um:

* formale Verifikation des Zustandsautomaten
* deterministisches Process Replay
* checkpointfähige Prozesse
* Live-Migration zwischen Systemen
* persistente Serviceprozesse
* transaktionale Prozessgruppen
* adaptive Restart Policies
* KI-gestützte Crash-Korrelation
* automatisierte Ressourcenleck-Erkennung
* hardwareunterstützte Prozessisolation
* clusterweite Lifecycle Domains

---

# 148. Konsequenz

Der Process Lifecycle wird zur verbindlichen Lebenszyklusarchitektur aller NovaOS-Prozesse.

Prozesse werden nicht als lose Kombination aus Adressraum, Threads und PID behandelt, sondern als vollständige Kernelobjekte mit:

* Identität
* Zustand
* Ressourcen
* Capabilities
* Beziehungen
* Ereignissen
* Diagnose
* kontrolliertem Lebensende

---

# 149. Zusammenfassung

NovaOS implementiert einen expliziten und vollständig kontrollierten **Process Lifecycle**.

Jeder Prozess durchläuft definierte Zustände von `Allocated` über `Running` bis `Reclaimed`. Prozesserstellung, Start, Suspendierung, Fortsetzung, Fehlerbehandlung, Beendigung und Ressourcenfreigabe werden durch den Process Manager koordiniert.

Das Modell integriert:

* Kernel Object Graph
* ereignisgesteuerten Kernel-Core
* einheitliches Objektmodell
* Capability-basierte Ressourcenverwaltung
* versionierte Kernel-Services
* Speicherverwaltung
* Threadverwaltung
* lokale Telemetrie und Diagnose
* Self-Healing und Recovery

Dadurch erhält NovaOS ein deterministisches, sicheres und erweiterbares Prozessmodell, das sowohl native Anwendungen als auch Systemdienste, Sandboxes, Container und Kompatibilitätsschichten zuverlässig unterstützt.
