# ADR-KERNEL-0018: SMP-Unterstützung

* **ADR-ID:** ADR-KERNEL-0018
* **Titel:** SMP-Unterstützung
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Scheduler, HAL, Memory Manager, Interrupt Manager, Process Manager, Device Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0002, ADR-KERNEL-0003, ADR-KERNEL-0016, ADR-KERNEL-0017, NPSPEC-SMP-0001

---

# 1. Zusammenfassung

NovaOS unterstützt **Symmetric Multiprocessing (SMP)** als primäres Ausführungsmodell für Mehrkernsysteme.

Alle verfügbaren CPU-Kerne arbeiten gleichberechtigt unter einer gemeinsamen Kernelinstanz, teilen sich den physischen Speicher und können grundsätzlich jeden Thread ausführen. Der Scheduler verteilt die Last dynamisch auf alle Prozessoren, während Synchronisationsmechanismen und Cache-Kohärenz einen konsistenten Systemzustand sicherstellen. SMP-Systeme bestehen aus mehreren gleichberechtigten Prozessoren, die sich Speicher und Betriebssystem teilen und parallel arbeiten.

---

# 2. Kontext

Moderne Prozessoren verfügen über:

* mehrere CPU-Kerne
* gemeinsame Speicherbereiche
* Cache-Hierarchien
* Hardware-Kohärenz
* Interprozessor-Kommunikation

Ein modernes Betriebssystem muss diese Ressourcen effizient nutzen.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie mehrere CPU-Kerne verwaltet werden,
* wie Threads verteilt werden,
* wie Synchronisation erfolgt,
* wie Interrupts verarbeitet werden,
* und wie zukünftige Prozessorarchitekturen unterstützt werden.

---

# 4. Entscheidung

NovaOS implementiert vollständige SMP-Unterstützung.

Dabei gilt:

* alle CPU-Kerne verwenden dieselbe Kernelinstanz,
* jeder Kern kann Kernel- und Benutzerthreads ausführen,
* der Scheduler arbeitet global mit CPU-lokalen Optimierungen,
* alle Kernelkomponenten müssen SMP-sicher implementiert werden.

---

# 5. Motivation

Diese Architektur bietet:

* maximale Parallelität
* bessere CPU-Auslastung
* hohe Skalierbarkeit
* effiziente Lastverteilung
* Unterstützung moderner Mehrkernsysteme
* langfristige Zukunftssicherheit

---

# 6. Designprinzipien

Die SMP-Architektur basiert auf:

* Symmetrie
* Parallelität
* Skalierbarkeit
* Determinismus
* Thread-Sicherheit
* Modularität

---

# 7. Architekturübersicht

```text
              Boot CPU (BSP)

                    │

          Initialisierung Kernel

                    │

          Start weiterer CPUs (APs)

                    │

        +-----------+-----------+

        ▼           ▼           ▼

      CPU 0       CPU 1       CPU n

        │           │           │

   Lokaler Scheduler pro CPU

        │           │           │

        └-----------┬-----------┘

                    ▼

          Globaler Scheduler

                    ▼

             Gemeinsamer Kernel
```

---

# 8. SMP-Modell

NovaOS verwendet **Symmetric Multiprocessing**.

Eigenschaften:

* gemeinsame Kernelinstanz
* gemeinsamer Adressraum des Kernels
* gemeinsamer physischer Speicher
* gleichberechtigte CPU-Kerne
* dynamische Threadverteilung

---

# 9. Boot-Prozessor

Beim Systemstart übernimmt ein Bootstrap Processor (BSP):

* Kernelinitialisierung
* Speicherverwaltung
* Interruptinitialisierung
* Schedulerstart
* Start weiterer Prozessoren

Nach Abschluss arbeiten alle Prozessoren gleichberechtigt.

---

# 10. Application Processor

Weitere CPU-Kerne (Application Processors) werden nach erfolgreicher Kernelinitialisierung gestartet.

Jeder Kern erhält:

* eigenen Kernelstack
* CPU-lokale Daten
* Schedulerkontext
* Interruptkontext
* Idle-Thread

---

# 11. CPU-Lokale Daten

Jeder Prozessor besitzt eigene Datenstrukturen.

Beispiele:

* aktuelle CPU-ID
* aktueller Thread
* Idle-Thread
* Schedulerstatus
* Interruptstatistik
* Performancezähler
* lokale Warteschlangen

CPU-lokale Daten reduzieren Synchronisationsaufwand erheblich.

---

# 12. Schedulerintegration

Der Scheduler unterstützt:

* globale Lastverteilung
* CPU-Affinität
* Threadmigration
* Lastbalancierung
* Idle-Ausgleich

Threads können grundsätzlich auf jedem Kern ausgeführt werden.

---

# 13. CPU-Affinität

NovaOS unterstützt optionale CPU-Affinitäten.

Ein Thread kann:

* auf beliebigen CPUs laufen,
* bevorzugte CPUs besitzen,
* oder explizit an bestimmte CPUs gebunden werden.

---

# 14. Lastbalancierung

Der Scheduler überwacht kontinuierlich:

* CPU-Auslastung
* Warteschlangen
* Threadprioritäten
* Cache-Lokalität

Bei Bedarf werden Threads zwischen Prozessoren migriert. Lastverteilung ist eine Kernaufgabe eines SMP-Betriebssystems.

---

# 15. Threadmigration

Threadmigration erfolgt ausschließlich durch den Scheduler.

Dabei werden berücksichtigt:

* Cache-Lokalität
* Priorität
* CPU-Auslastung
* NUMA-Richtlinien (optional)

Unnötige Migrationen sollen vermieden werden.

---

# 16. Synchronisation

Gemeinsam genutzte Kernelstrukturen werden geschützt durch:

* Spinlocks
* Reader-Writer-Locks
* atomare Operationen
* Memory Barriers
* lockfreie Datenstrukturen

Alle Kernelkomponenten müssen SMP-sicher implementiert sein.

---

# 17. Cache-Kohärenz

NovaOS setzt hardwaregestützte Cache-Kohärenz voraus.

Der Kernel berücksichtigt:

* Cache-Lines
* False Sharing
* Memory Ordering
* Cache-Invalidierung

Die Software vermeidet unnötige Cache-Konflikte.

---

# 18. Speicherverwaltung

Der Memory Manager unterstützt:

* parallele Speicherallokation
* CPU-lokale Caches
* parallele Freigaben
* TLB-Synchronisation

---

# 19. Interprozessor-Interrupts

Interprozessor-Interrupts (IPI) dienen unter anderem für:

* Schedulersteuerung
* TLB-Flush
* CPU-Wakeup
* Synchronisation
* Kernelkoordination

---

# 20. Interruptverarbeitung

Jeder Kern verarbeitet grundsätzlich seine eigenen Interrupts.

Hardware kann Interrupts:

* fest zuweisen,
* dynamisch verteilen,
* oder prioritätsabhängig routen.

---

# 21. CPU-Hotplug

Die Architektur unterstützt zukünftigen CPU-Hotplug.

Dabei können Prozessoren:

* hinzugefügt,
* deaktiviert,
* reaktiviert,
* entfernt

werden, ohne die Kernelarchitektur zu verändern.

---

# 22. Fehlerisolierung

Fällt ein CPU-Kern aus,

* bleiben andere Kerne funktionsfähig,
* laufende Threads können migriert werden,
* Diagnosedaten werden gespeichert.

Die konkrete Wiederherstellung hängt von den Fähigkeiten der Plattform ab.

---

# 23. Energieverwaltung

SMP unterstützt:

* CPU-Sleep
* Core Parking
* Taktabsenkung
* dynamisches Aufwecken

Diese Mechanismen werden durch den Power Manager gesteuert.

---

# 24. Sicherheit

Die SMP-Architektur schützt vor:

* Race Conditions
* Deadlocks
* Datenkorruption
* inkonsistenten Speicherzuständen
* konkurrierenden Kernelzugriffen

---

# 25. Performance

Die SMP-Unterstützung soll:

* hohe Skalierung
* geringe Synchronisationskosten
* kurze Sperrzeiten
* maximale Parallelität

ermöglichen.

---

# 26. Debug-Unterstützung

Optional unterstützt NovaOS:

* CPU-Auslastungsstatistiken
* Threadmigrationstracing
* Lockanalyse
* Deadlock-Erkennung
* Cache-Statistiken
* IPI-Diagnose

---

# 27. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* NUMA
* Heterogeneous Computing
* Hybrid-CPU-Architekturen
* CPU-Isolation
* Echtzeit-Scheduler
* virtuelle Prozessoren

---

# 28. Auswirkungen

Positive Auswirkungen:

* hohe Parallelität
* optimale Nutzung moderner Mehrkernprozessoren
* bessere Skalierbarkeit
* höhere Gesamtleistung
* flexible Threadverteilung
* gute Zukunftssicherheit

Negative Auswirkungen:

* höhere Kernelkomplexität
* aufwendigere Synchronisation
* zusätzlicher Testaufwand
* schwierigere Fehlersuche bei Race Conditions

---

# 29. Verworfene Alternativen

## Uniprozessor-Kernel

Vorteile:

* einfache Implementierung
* geringe Synchronisationsanforderungen

Nachteile:

* keine Mehrkernunterstützung
* schlechte Skalierbarkeit

---

## Asymmetric Multiprocessing (AMP)

Vorteile:

* spezialisierte CPU-Aufgaben

Nachteile:

* höhere Softwarekomplexität
* getrennte Betriebssysteminstanzen
* schwierige Ressourcenverwaltung

---

## Statische CPU-Zuweisung

Vorteile:

* einfache Planung

Nachteile:

* schlechte Lastverteilung
* ungenutzte CPU-Ressourcen
* geringe Flexibilität

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* vollständig SMP-sicher implementiert sein,
* gemeinsam genutzte Daten korrekt synchronisieren,
* CPU-lokale Daten bevorzugen,
* unnötige globale Sperren vermeiden,
* Threadmigration transparent unterstützen.

---

# 31. Teststrategie

Die SMP-Unterstützung wird geprüft durch:

* Unit-Tests
* Mehrkerntests
* Stresstests
* Race-Condition-Tests
* Deadlock-Tests
* Lastverteilungstests
* Performancebenchmarks
* Langzeittests

---

# 32. Konsequenzen

Die SMP-Unterstützung bildet die Grundlage für die Parallelisierung von NovaOS.

Alle zukünftigen Kernelmodule, Gerätetreiber und Subsysteme müssen vollständig mehrkernfähig implementiert werden. Synchronisation, CPU-lokale Datenstrukturen und thread-sichere Algorithmen sind verbindliche Bestandteile der Kernelentwicklung. Dadurch kann NovaOS die Rechenleistung moderner Mehrkernsysteme effizient nutzen und gleichzeitig eine hohe Skalierbarkeit und Systemstabilität gewährleisten.

---

# 33. Zusammenfassung

NovaOS implementiert **vollständige Symmetric-Multiprocessing-Unterstützung (SMP)** mit einer gemeinsamen Kernelinstanz für alle Prozessoren. Jeder CPU-Kern kann Threads parallel ausführen, während Scheduler, Speicherverwaltung, Interruptsystem und Synchronisationsmechanismen speziell für den Mehrkernbetrieb ausgelegt sind. Durch CPU-lokale Datenstrukturen, dynamische Lastverteilung und konsequente Thread-Sicherheit entsteht eine skalierbare Kernelarchitektur, die moderne Mehrkernprozessoren effizient ausnutzt und zukünftige Erweiterungen wie NUMA oder Hybrid-Architekturen vorbereitet.
