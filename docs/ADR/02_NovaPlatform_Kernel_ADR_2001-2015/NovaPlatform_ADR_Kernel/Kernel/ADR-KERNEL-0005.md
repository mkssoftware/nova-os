# ADR-KERNEL-0005: Threadmodell

* **ADR-ID:** ADR-KERNEL-0005
* **Titel:** Threadmodell
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Thread Manager, Scheduler, Process Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0001, ADR-KERNEL-0003, ADR-KERNEL-0004, NPSPEC-KERNEL-0005

---

# 1. Zusammenfassung

NovaOS verwendet ein **Kernel-Thread-Modell (1:1)**, bei dem jeder Anwendungs-Thread genau einem Kernel-Thread entspricht. Threads bilden die kleinste vom Scheduler verwaltete Ausführungseinheit und besitzen einen eigenen CPU-Kontext, Stack und Ausführungszustand, während Ressourcen wie Adressraum und offene Dateien gemeinsam über den zugehörigen Prozess genutzt werden. Dieses Modell ermöglicht echte Parallelität auf Mehrkernsystemen und verhindert, dass blockierende Operationen den gesamten Prozess anhalten.

---

# 2. Kontext

Threads bilden die eigentlichen Ausführungseinheiten eines Prozesses.

Das Threadmodell beeinflusst:

* Scheduling
* Multithreading
* Parallelisierung
* Synchronisation
* IPC
* Performance
* Skalierbarkeit
* Energieverwaltung

---

# 3. Problemstellung

Es musste entschieden werden,

* welches Threadmodell verwendet wird,
* wie Threads geplant werden,
* welche Ressourcen gemeinsam genutzt werden,
* welche Informationen jedem Thread gehören,
* und wie sich Threads innerhalb eines Prozesses verhalten.

---

# 4. Entscheidung

NovaOS verwendet ein **1:1-Kernel-Thread-Modell**.

Dabei gilt:

* Jeder Benutzerthread besitzt genau einen Kernel-Thread.
* Der Scheduler plant ausschließlich Kernel-Threads.
* Jeder Thread besitzt einen eigenen CPU-Kontext.
* Threads eines Prozesses teilen gemeinsame Ressourcen.
* Blockierende Operationen betreffen nur den blockierten Thread.

---

# 5. Motivation

Diese Architektur bietet:

* echte Parallelität
* optimale Mehrkernunterstützung
* hohe Reaktionsfähigkeit
* einfache Schedulerarchitektur
* gute Debugbarkeit
* hohe Skalierbarkeit

---

# 6. Designprinzipien

Das Threadmodell basiert auf:

* Präemption
* Kernel-Scheduling
* Ressourcenteilung
* Isolation des Ausführungskontextes
* Determinismus
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
                    Prozess

+------------------------------------------------------+
| Virtueller Adressraum                                |
| Handle-Tabelle                                       |
| Heap                                                 |
| Sicherheitskontext                                   |
+----------------------+-------------------------------+
                       |
      +----------------+------------------+
      |                |                  |
      ▼                ▼                  ▼
   Thread A         Thread B          Thread C
      │                │                  │
      ▼                ▼                  ▼
 Kernel-Thread     Kernel-Thread     Kernel-Thread
      │                │                  │
      └──────────── Scheduler ────────────┘
```

---

# 8. Definition eines Threads

Ein Thread ist die kleinste vom Kernel planbare Ausführungseinheit.

Er besitzt:

* Thread-ID (TID)
* Registersatz
* Programmzähler
* Stack
* CPU-Kontext
* Schedulinginformationen
* Ausführungszustand

---

# 9. Threadidentität

Jeder Thread besitzt:

* eindeutige Thread-ID
* Besitzerprozess
* Priorität
* CPU-Affinität
* Erstellungszeitpunkt
* Status

---

# 10. Gemeinsam genutzte Ressourcen

Alle Threads eines Prozesses teilen:

* virtuellen Adressraum
* Heap
* offene Dateien
* Kernelobjekte
* IPC-Ressourcen
* Sicherheitskontext

---

# 11. Threadlokale Ressourcen

Jeder Thread besitzt ausschließlich eigene:

* Register
* Stack
* Stackpointer
* Programmzähler
* Schedulingdaten
* Thread Local Storage (TLS)

---

# 12. Threadzustände

Ein Thread kann sich mindestens in folgenden Zuständen befinden:

```text
CREATED

↓

READY

↓

RUNNING

↓

WAITING

↓

BLOCKED

↓

TERMINATED
```

---

# 13. Lebenszyklus

Der Lebenszyklus umfasst:

* Erstellung
* Initialisierung
* Aufnahme in die Ready Queue
* Ausführung
* Blockierung
* Wiederaufnahme
* Terminierung
* Ressourcenfreigabe

---

# 14. Threaderzeugung

Neue Threads werden ausschließlich über den Thread Manager erzeugt.

Dabei werden:

* Stack
* Threadobjekt
* CPU-Kontext
* Schedulerinformationen

initialisiert.

---

# 15. Threadbeendigung

Beim Beenden eines Threads werden freigegeben:

* Stack
* Kernelobjekt
* Thread Local Storage
* Schedulerinformationen

Gemeinsam genutzte Prozessressourcen bleiben erhalten, solange weitere Threads existieren.

---

# 16. Threadprioritäten

Jeder Thread besitzt eine eigene Priorität.

Die Priorität bestimmt seine Behandlung durch den Scheduler.

---

# 17. CPU-Affinität

Threads können:

* auf jeder CPU laufen,
* bevorzugte Prozessoren besitzen,
* oder fest an einzelne CPUs gebunden werden.

---

# 18. Synchronisation

Zur Synchronisation unterstützt NovaOS mindestens:

* Spinlocks
* Mutexe
* Semaphoren
* Reader-Writer-Locks
* Ereignisse
* Atomare Operationen

---

# 19. Thread Local Storage

Jeder Thread besitzt einen eigenen Bereich für threadlokale Daten.

Diese Daten sind ausschließlich für den jeweiligen Thread sichtbar.

---

# 20. Blockierende Operationen

Blockiert ein Thread auf:

* I/O
* Mutex
* Semaphore
* Event
* Nachricht

so bleiben alle übrigen Threads desselben Prozesses weiterhin ausführbar.

---

# 21. Kontextwechsel

Ein Threadwechsel speichert:

* Register
* Programmzähler
* Stackpointer
* CPU-Flags
* Architekturabhängige Register

Danach wird der Kontext des nächsten Threads geladen.

---

# 22. Mehrkernbetrieb

Mehrere Threads desselben Prozesses dürfen gleichzeitig auf unterschiedlichen CPU-Kernen ausgeführt werden.

---

# 23. Fehlerisolierung

Ein fehlerhafter Thread führt nicht automatisch zur Terminierung des gesamten Prozesses.

Die Reaktion erfolgt gemäß den Richtlinien des Exception Managers und des Process Managers.

---

# 24. Performance

Threadwechsel sollen deutlich günstiger sein als Prozesswechsel.

Ziel ist ein minimaler Scheduling- und Kontextwechsel-Overhead.

---

# 25. Erweiterbarkeit

Das Threadmodell ermöglicht zukünftige Erweiterungen wie:

* Echtzeitthreads
* Kernel-Worker-Threads
* CPU-Gruppen
* Prioritätsklassen
* QoS-Klassen

---

# 26. Auswirkungen

Positive Auswirkungen:

* hohe Parallelität
* optimale Mehrkernunterstützung
* schnelle Kontextwechsel
* gute Skalierbarkeit
* hohe Reaktionsfähigkeit

Negative Auswirkungen:

* höherer Synchronisationsaufwand
* Race Conditions müssen verhindert werden
* Deadlocks müssen berücksichtigt werden

---

# 27. Verworfene Alternativen

## User-Level-Threads

Vorteile:

* schnelle Threadwechsel
* geringe Kernelbeteiligung

Nachteile:

* keine echte Parallelität auf mehreren CPUs
* blockierende Systemaufrufe können den gesamten Prozess blockieren
* Scheduler besitzt keine Kenntnis einzelner Threads.

---

## M:N-Threadmodell

Vorteile:

* flexible Zuordnung
* geringerer Kernelaufwand

Nachteile:

* hohe Implementierungskomplexität
* schwierige Fehlersuche
* komplexe Synchronisation

---

## Ein Thread pro Prozess

Vorteile:

* einfache Implementierung

Nachteile:

* keine moderne Parallelisierung
* schlechte Skalierbarkeit

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Threads ausschließlich über den Thread Manager erzeugen,
* Kernel-Threads verwenden,
* den Threadzustand konsistent verwalten,
* keine architekturspezifischen Threaddaten außerhalb der HAL speichern,
* den Scheduler über Zustandsänderungen informieren.

---

# 29. Teststrategie

Das Threadmodell wird validiert durch:

* Unit-Tests
* Scheduler-Tests
* Synchronisationstests
* SMP-Tests
* Lasttests
* Langzeittests
* Deadlock-Tests
* Race-Condition-Tests

---

# 30. Konsequenzen

Diese Architekturentscheidung definiert das verbindliche Threadmodell von NovaOS.

Alle zukünftigen Komponenten des Schedulers, Process Managers, Exception Managers und der Synchronisationsmechanismen müssen mit diesem 1:1-Kernel-Thread-Modell kompatibel sein. Änderungen an dieser Architektur würden weitreichende Auswirkungen auf nahezu alle Kernelsubsysteme haben.

---

# 31. Zusammenfassung

NovaOS verwendet ein **1:1-Kernel-Thread-Modell**, bei dem jeder Thread als eigenständige, vom Kernel geplante Ausführungseinheit behandelt wird. Threads teilen sich die Ressourcen ihres Prozesses, besitzen jedoch jeweils einen eigenen Ausführungskontext, Stack und Schedulingstatus. Dieses Modell bietet eine hohe Parallelität, optimale Unterstützung für Mehrkernsysteme sowie eine klare und skalierbare Grundlage für die weitere Entwicklung des NovaOS-Kernels.
