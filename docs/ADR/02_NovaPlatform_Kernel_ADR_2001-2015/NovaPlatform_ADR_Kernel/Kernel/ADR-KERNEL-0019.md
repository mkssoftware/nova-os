# ADR-KERNEL-0019: CPU Hotplug

* **ADR-ID:** ADR-KERNEL-0019
* **Titel:** CPU Hotplug
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Scheduler, HAL, SMP-Subsystem, Interrupt Manager, Power Manager, Device Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0018, ADR-KERNEL-0016, ADR-KERNEL-0003, NPSPEC-SMP-0002

---

# 1. Zusammenfassung

NovaOS unterstützt **CPU Hotplug**, wodurch Prozessoren zur Laufzeit kontrolliert aktiviert oder deaktiviert werden können, ohne dass ein Neustart des Systems erforderlich ist.

Die Hotplug-Architektur dient sowohl klassischen Server- und Virtualisierungsszenarien als auch Energieverwaltung, Wartung, Fehlerisolierung und zukünftigen modularen Hardwareplattformen. Vor dem Entfernen eines Prozessors werden sämtliche Threads, Interrupts, Timer und CPU-lokalen Ressourcen kontrolliert auf andere Prozessoren migriert. CPU-Hotplug wird in modernen Betriebssystemen unter anderem für Energieverwaltung, Suspend/Resume, Wartung und Fehlermanagement eingesetzt.

---

# 2. Kontext

Moderne Hardware unterstützt zunehmend:

* dynamische CPU-Aktivierung
* Energieeinsparung
* Hot-Swap-fähige Systeme
* Virtualisierung
* modulare Serverplattformen
* Hochverfügbarkeit

NovaOS soll diese Möglichkeiten vollständig unterstützen.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Prozessoren zur Laufzeit hinzugefügt oder entfernt werden,
* wie Threads migriert werden,
* wie Interrupts umgeleitet werden,
* wie CPU-lokale Ressourcen behandelt werden,
* und wie dabei die Systemstabilität gewährleistet wird.

---

# 4. Entscheidung

NovaOS implementiert vollständige CPU-Hotplug-Unterstützung.

Dabei gilt:

* Prozessoren können online oder offline geschaltet werden,
* sämtliche Kernelkomponenten unterstützen CPU-Hotplug,
* alle CPU-lokalen Ressourcen werden kontrolliert migriert,
* der Scheduler bleibt jederzeit funktionsfähig.

---

# 5. Motivation

Diese Architektur bietet:

* höhere Verfügbarkeit
* bessere Energieeffizienz
* vereinfachte Wartung
* Fehlerisolierung
* Unterstützung moderner Serverplattformen
* Zukunftssicherheit

---

# 6. Designprinzipien

Die CPU-Hotplug-Architektur basiert auf:

* Determinismus
* Ausfallsicherheit
* Transparenz
* Modularität
* Thread-Sicherheit
* vollständiger Wiederherstellbarkeit

---

# 7. Architekturübersicht

```text
              CPU Hotplug Request

                      │

                      ▼

             CPU Hotplug Manager

                      │

      ┌───────────────┼────────────────┐

      ▼               ▼                ▼

 Scheduler      Interrupt Manager   Memory Manager

      ▼               ▼                ▼

 Threadmigration   IRQ-Migration   CPU-Lokale Daten

                      │

                      ▼

             CPU Online / Offline
```

---

# 8. CPU-Zustände

Jeder Prozessor besitzt einen definierten Zustand.

Beispiele:

* Present
* Initialized
* Online
* Offline
* Removing
* Starting
* Failed

Diese Zustände bilden einen deterministischen Zustandsautomaten. CPU-Hotplug wird typischerweise als definierte Zustandsmaschine implementiert.

---

# 9. CPU Online

Beim Aktivieren eines Prozessors erfolgen unter anderem:

* Initialisierung der CPU
* Aufbau CPU-lokaler Daten
* Initialisierung der Interrupts
* Start des Idle-Threads
* Registrierung beim Scheduler
* Freigabe für Benutzerthreads

---

# 10. CPU Offline

Vor dem Abschalten eines Prozessors müssen:

* alle Threads migriert,
* alle Interrupts umgeleitet,
* alle Timer übertragen,
* alle CPU-lokalen Ressourcen bereinigt

werden.

Erst anschließend darf der Prozessor deaktiviert werden. Vor dem Offline-Schalten werden Tasks, Interrupts und Timer auf andere CPUs migriert.

---

# 11. CPU Hotplug Manager

Der CPU Hotplug Manager übernimmt:

* Zustandsverwaltung
* Reihenfolge der Hotplug-Schritte
* Fehlerbehandlung
* Synchronisation
* Ereigniserzeugung
* Diagnose

---

# 12. Threadmigration

Vor dem Entfernen einer CPU werden sämtliche ausführbaren Threads auf andere Prozessoren migriert.

Dabei berücksichtigt der Scheduler:

* Priorität
* CPU-Affinität
* Cache-Lokalität
* NUMA-Richtlinien
* Lastverteilung

---

# 13. Interruptmigration

Alle Interruptquellen der zu entfernenden CPU werden auf andere verfügbare Prozessoren umgeleitet.

Dies umfasst:

* Geräteinterrupts
* Timer
* IPIs
* Deferred Work

---

# 14. Timer-Migration

CPU-lokale Timer werden vollständig übertragen.

Kein Timer darf während eines CPU-Offlines verloren gehen.

---

# 15. CPU-Lokale Ressourcen

Folgende Ressourcen werden bereinigt oder migriert:

* Per-CPU-Daten
* Schedulerstatus
* Performancezähler
* Statistikdaten
* Event-Queues
* Deferred-Work-Queues

---

# 16. Idle-Prozessoren

Offline-Prozessoren führen keine Benutzerthreads aus.

Sie können sich in einem stromsparenden Zustand befinden oder vollständig abgeschaltet werden.

---

# 17. Energieverwaltung

Der Power Manager kann CPU-Hotplug verwenden für:

* Energieeinsparung
* Core Parking
* thermische Regelung
* Lastoptimierung

Die Entscheidung erfolgt anhand von Systemrichtlinien.

---

# 18. Fehlerbehandlung

Fällt ein Prozessor unerwartet aus,

* werden Threads migriert,
* Interrupts umgeleitet,
* Diagnosedaten gespeichert,
* Scheduler und Kernel weiter betrieben.

---

# 19. Mehrkernintegration

CPU Hotplug ist vollständig mit der SMP-Architektur integriert.

Alle verbleibenden Prozessoren arbeiten ohne Unterbrechung weiter.

---

# 20. HAL-Integration

Architekturspezifische Unterschiede werden ausschließlich durch die HAL behandelt.

Beispiele:

* APIC
* PSCI
* ACPI
* Firmware-Schnittstellen
* Bootmechanismen

---

# 21. Schedulerintegration

Der Scheduler unterstützt:

* CPU-Hinzufügen
* CPU-Entfernen
* automatische Lastverteilung
* Rebalancing
* Affinitätsanpassung

---

# 22. Eventintegration

Jeder Hotplug-Vorgang erzeugt standardisierte Kernelereignisse.

Beispiele:

* CPUOnline
* CPUOffline
* CPUAdded
* CPURemoved
* CPUFailed

---

# 23. Sicherheit

Das CPU-Hotplug-System schützt vor:

* inkonsistenten CPU-Zuständen
* verlorenen Threads
* verlorenen Interrupts
* beschädigten Schedulerdaten
* konkurrierenden Hotplug-Vorgängen

---

# 24. Performance

Die CPU-Hotplug-Unterstützung soll:

* geringe Aktivierungszeiten
* schnelle Migration
* minimale Unterbrechungen
* geringe Synchronisationskosten

ermöglichen.

---

# 25. Debug-Unterstützung

Optional unterstützt NovaOS:

* Hotplug-Protokolle
* CPU-Zustandshistorie
* Migrationsstatistiken
* Interruptmigrationstracing
* Fehlerprotokolle

---

# 26. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* physisches CPU-Hotplug
* virtuelle CPUs
* Cluster-Hotplug
* Hybrid-Prozessoren
* dynamische CPU-Gruppen
* energieoptimierte Richtlinien

---

# 27. Auswirkungen

Positive Auswirkungen:

* hohe Verfügbarkeit
* bessere Energieeffizienz
* einfache Wartung
* flexible Ressourcenverwaltung
* vollständige SMP-Integration
* gute Skalierbarkeit

Negative Auswirkungen:

* komplexere Schedulerlogik
* zusätzlicher Synchronisationsaufwand
* höherer Testaufwand
* umfangreichere Diagnosemechanismen

---

# 28. Verworfene Alternativen

## Neustart nach jeder CPU-Änderung

Vorteile:

* einfache Implementierung

Nachteile:

* geringe Verfügbarkeit
* lange Ausfallzeiten
* ungeeignet für Server

---

## Keine CPU-Hotplug-Unterstützung

Vorteile:

* geringere Kernelkomplexität

Nachteile:

* keine dynamische Ressourcenverwaltung
* schlechtere Energieeffizienz
* eingeschränkte Zukunftsfähigkeit

---

## Plattformabhängige Hotplug-Implementierungen

Vorteile:

* hardwareoptimiert

Nachteile:

* hoher Wartungsaufwand
* geringe Portabilität
* inkonsistente Kernelarchitektur

---

# 29. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* CPU-Hotplug vollständig unterstützen,
* CPU-lokale Ressourcen korrekt migrieren,
* Hotplug-Ereignisse registrieren,
* konkurrierende Hotplug-Operationen verhindern,
* den Scheduler jederzeit konsistent halten.

---

# 30. Teststrategie

Die CPU-Hotplug-Unterstützung wird geprüft durch:

* Unit-Tests
* Online-/Offline-Tests
* Mehrkerntests
* Lasttests
* Interruptmigrationstests
* Fehlersimulationen
* Langzeittests
* Performancebenchmarks

---

# 31. Konsequenzen

CPU Hotplug wird zu einem verbindlichen Bestandteil der NovaOS-SMP-Architektur.

Alle zukünftigen Kernelmodule, Schedulerkomponenten, Gerätetreiber und Interruptsysteme müssen Prozessoren berücksichtigen, die während des laufenden Betriebs aktiviert oder deaktiviert werden können. Dadurch bleibt NovaOS auch bei dynamischen Hardwareänderungen stabil, performant und hochverfügbar.

---

# 32. Zusammenfassung

NovaOS implementiert eine **vollständige CPU-Hotplug-Architektur**, mit der Prozessoren während des laufenden Betriebs kontrolliert online oder offline geschaltet werden können. Der CPU Hotplug Manager koordiniert dabei die Migration von Threads, Interrupts, Timern und CPU-lokalen Ressourcen und arbeitet eng mit Scheduler, HAL und Power Manager zusammen. Dadurch entsteht eine skalierbare und hochverfügbare Mehrkernarchitektur, die moderne Server-, Virtualisierungs- und Energiesparszenarien unterstützt.
