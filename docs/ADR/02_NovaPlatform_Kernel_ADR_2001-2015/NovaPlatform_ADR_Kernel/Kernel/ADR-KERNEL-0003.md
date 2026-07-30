# ADR-KERNEL-0003: Präemptiver Scheduler

* **ADR-ID:** ADR-KERNEL-0003
* **Titel:** Präemptiver Scheduler
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Scheduler, Process Manager, Thread Manager, Timer Subsystem
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0001, ADR-KERNEL-0002, NPSPEC-KERNEL-0003

---

# 1. Zusammenfassung

NovaOS verwendet einen **präemptiven, prioritätsbasierten Scheduler**.

Der Scheduler kann einen laufenden Thread jederzeit unterbrechen, wenn ein Thread mit höherer Priorität ausführbar wird oder das konfigurierte Zeitquantum abgelaufen ist. Dadurch werden kurze Reaktionszeiten, faire CPU-Verteilung und hohe Systemauslastung erreicht. Moderne allgemeine Betriebssysteme setzen aus diesen Gründen überwiegend auf präemptive Scheduler.

---

# 2. Kontext

Der Scheduler bestimmt, welcher Thread zu welchem Zeitpunkt auf welcher CPU ausgeführt wird.

Er beeinflusst unmittelbar:

* Systemreaktionszeit
* Interaktivität
* CPU-Auslastung
* Energieverbrauch
* Echtzeitverhalten
* Skalierbarkeit

---

# 3. Problemstellung

Es musste entschieden werden, ob NovaOS:

* kooperatives Multitasking,
* präemptives Multitasking,
* hybrides Scheduling,
* oder Run-to-Completion

verwenden soll.

---

# 4. Entscheidung

NovaOS verwendet einen **präemptiven Scheduler**.

Ein laufender Thread kann unterbrochen werden durch:

* Ablauf des Zeitquantums
* höher priorisierten Thread
* Interrupt
* Systemaufruf
* Blockierung
* Terminierung

Die Scheduling-Entscheidung erfolgt ausschließlich innerhalb des Kernels.

---

# 5. Motivation

Die Entscheidung basiert auf folgenden Zielen:

* kurze Reaktionszeiten
* flüssige Benutzeroberfläche
* faire CPU-Verteilung
* Unterstützung mehrerer CPUs
* Echtzeitfähigkeit
* deterministische Schedulingregeln

---

# 6. Designprinzipien

Der Scheduler basiert auf:

* Präemption
* Prioritäten
* Fairness
* Skalierbarkeit
* SMP-Unterstützung
* geringer Latenz
* deterministischem Verhalten

---

# 7. Architekturübersicht

```text
                Ready Queue
                      │
                      ▼
              Scheduler Core
                      │
        ┌─────────────┼─────────────┐
        │             │             │
        ▼             ▼             ▼
      CPU 0         CPU 1         CPU n
        ▲             ▲             ▲
        │             │             │
        └───── Context Switch ──────┘
```

---

# 8. Threadbasierter Scheduler

NovaOS plant ausschließlich **Threads**.

Prozesse dienen lediglich als Container für:

* Threads
* Adressraum
* Handles
* Ressourcen

---

# 9. Prozessmodell

Ein Prozess besitzt beliebig viele Threads.

Jeder Thread besitzt:

* Priorität
* Zustand
* CPU-Kontext
* Stack
* Schedulinginformationen

---

# 10. Präemption

Ein Thread darf jederzeit verdrängt werden.

Gründe:

* höher priorisierter Thread
* Zeitquantum abgelaufen
* Interrupt
* Kernelentscheidung

---

# 11. Zeitquantum

Jeder Thread erhält ein konfigurierbares Zeitquantum.

Nach Ablauf erfolgt eine erneute Schedulingentscheidung.

Die konkrete Länge des Quantums wird in der Scheduler-Spezifikation definiert.

---

# 12. Prioritäten

NovaOS verwendet numerische Prioritäten.

Beispiel:

```text
0      Idle

1–15   Hintergrund

16–63  Normal

64–127 Interaktiv

128–191 System

192–255 Echtzeit
```

Die exakten Werte können durch spätere NPSPECs angepasst werden.

---

# 13. Ready Queue

Ausführbare Threads befinden sich in einer Ready Queue.

Nur Threads im Zustand **READY** dürfen geplant werden.

---

# 14. Threadzustände

Mindestens:

* CREATED
* READY
* RUNNING
* WAITING
* BLOCKED
* TERMINATED

---

# 15. Schedulingereignisse

Eine Schedulingentscheidung erfolgt bei:

* Timerinterrupt
* Threadstart
* Threadende
* Blockierung
* Aufwecken
* Prioritätsänderung

---

# 16. Kontextwechsel

Ein Kontextwechsel speichert:

* Register
* Stackpointer
* Programmzähler
* CPU-Status
* Schedulinginformationen

Danach wird der Kontext des nächsten Threads geladen.

---

# 17. SMP-Unterstützung

Jede CPU besitzt eine eigene Ready Queue.

Optional kann zusätzlich ein globales Lastverteilungssystem eingesetzt werden.

---

# 18. CPU-Affinität

Threads können optional an bestimmte CPUs gebunden werden.

Beispiele:

* keine Bindung
* bevorzugte CPU
* feste CPU

---

# 19. Lastverteilung

Der Scheduler verteilt Threads möglichst gleichmäßig auf alle verfügbaren Prozessoren.

Ungleichgewichte können automatisch ausgeglichen werden.

---

# 20. Idle Thread

Jede CPU besitzt genau einen Idle Thread.

Er wird ausschließlich ausgeführt, wenn kein anderer Thread bereitsteht.

---

# 21. Echtzeitthreads

Echtzeitthreads besitzen Vorrang gegenüber normalen Threads.

Ihre Planung erfolgt nach gesonderten Regeln.

---

# 22. Prioritätsvererbung

Synchronisationsmechanismen können Prioritätsvererbung unterstützen, um Prioritätsinversionen zu vermeiden.

---

# 23. Synchronisation

Der Scheduler arbeitet mit:

* Spinlocks
* atomaren Operationen
* CPU-lokalen Datenstrukturen

Lange Sperrzeiten sind zu vermeiden.

---

# 24. Energieverwaltung

Im Leerlauf arbeitet der Scheduler mit dem Power Manager zusammen.

Dadurch können Schlafzustände der CPU genutzt werden.

---

# 25. Fairness

Normale Threads sollen langfristig einen fairen Anteil der verfügbaren CPU-Zeit erhalten.

Dabei dürfen interaktive Threads bevorzugt behandelt werden.

---

# 26. Performance

Der Scheduler muss:

* kurze Schedulinglatenzen,
* geringe Kontextwechselkosten,
* hohe Skalierbarkeit

erreichen.

---

# 27. Erweiterbarkeit

Neue Schedulingklassen können ergänzt werden.

Beispiele:

* Echtzeit
* Hintergrund
* Multimedia
* Batch

Die Kernarchitektur bleibt unverändert.

---

# 28. Auswirkungen

Positive Auswirkungen:

* hohe Reaktionsgeschwindigkeit
* flüssige Bedienung
* gute CPU-Auslastung
* SMP-Unterstützung
* Echtzeitfähigkeit

Negative Auswirkungen:

* höhere Implementierungskomplexität
* Kontextwechsel verursachen Overhead
* Synchronisation muss sorgfältig erfolgen

---

# 29. Verworfene Alternativen

## Kooperatives Scheduling

Vorteile:

* einfache Implementierung
* geringer Scheduleraufwand

Nachteile:

* ein blockierter Thread kann das gesamte System verzögern
* ungeeignet für moderne Desktopbetriebssysteme

---

## Run-to-Completion

Vorteile:

* deterministisch
* geringer Overhead

Nachteile:

* schlechte Interaktivität
* keine faire CPU-Verteilung

---

## Reiner Round-Robin-Scheduler

Vorteile:

* einfach

Nachteile:

* Prioritäten werden nicht berücksichtigt
* für komplexe Desktop-Workloads ungeeignet

---

# 30. Implementierungsrichtlinien

Der Scheduler muss:

* vollständig präemptiv arbeiten,
* Threadprioritäten berücksichtigen,
* SMP unterstützen,
* deterministisch entscheiden,
* modular aufgebaut sein,
* unabhängig von der CPU-Architektur sein.

---

# 31. Teststrategie

Der Scheduler wird geprüft durch:

* Unit-Tests
* Integrationstests
* Lasttests
* SMP-Tests
* Echtzeittests
* Langzeittests
* Stresstests

---

# 32. Konsequenzen

Diese Architekturentscheidung bildet die Grundlage sämtlicher zukünftiger Scheduler-Implementierungen in NovaOS.

Alle späteren Schedulingalgorithmen, Prioritätsklassen, Echtzeitmechanismen und Lastverteilungsstrategien müssen mit dieser ADR vereinbar sein und dürfen das präemptive Grundmodell nicht verletzen.

---

# 33. Zusammenfassung

NovaOS verwendet einen **präemptiven, prioritätsbasierten Thread-Scheduler** als zentrales Planungssystem des Kernels. Durch zeitgesteuerte Präemption, Prioritätssteuerung, Unterstützung mehrerer Prozessoren und eine faire Verteilung der CPU-Zeit bietet diese Architektur die Grundlage für ein reaktionsschnelles, skalierbares und leistungsfähiges Betriebssystem. Die konkrete Schedulingpolitik wird in den zugehörigen NPSPEC-Dokumenten spezifiziert, während diese ADR die grundlegende Architekturentscheidung festlegt.
