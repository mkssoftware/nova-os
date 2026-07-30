# ADR-MM-0011: Memory Hotplug

* **ADR-ID:** ADR-MM-0011
* **Titel:** Memory Hotplug
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Physical Memory Manager (PMM), Virtual Memory Manager (VMM), NUMA Manager, HAL, Scheduler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-KERNEL-0010, ADR-KERNEL-0018, ADR-KERNEL-0019

---

# 1. Zusammenfassung

NovaOS unterstützt **Memory Hotplug**, um physischen Arbeitsspeicher während des laufenden Betriebs dynamisch hinzufügen oder entfernen zu können.

Neue Speicherbereiche können ohne Neustart in das System integriert werden. Ebenso können Speicherbereiche kontrolliert außer Betrieb genommen werden, sofern sich keine nicht migrierbaren Daten mehr darin befinden. Diese Architektur bildet die Grundlage für moderne Server, Virtualisierung, Cloud-Systeme sowie zukünftige CXL- und Persistent-Memory-Technologien. Memory Hotplug ermöglicht das dynamische Hinzufügen und Entfernen von Arbeitsspeicher zur Laufzeit. Das Einbinden erfolgt typischerweise in zwei Phasen: Erkennen des Speichers und anschließendes Onlining für den Page Allocator.

---

# 2. Kontext

Moderne Systeme besitzen häufig dynamisch veränderbare Speicherkonfigurationen.

Beispiele:

* virtuelle Maschinen
* Cloud-Plattformen
* NUMA-Systeme
* Blade-Server
* Embedded-Systeme
* CXL Memory
* Persistent Memory
* Hochverfügbarkeitssysteme

Ein Neustart zum Austausch oder Erweitern des Speichers wäre oftmals nicht akzeptabel.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie neuer Speicher erkannt wird,
* wie Speicher entfernt werden kann,
* wie laufende Daten migriert werden,
* wie der Page Allocator aktualisiert wird,
* und wie Anwendungen unbeeinflusst weiterlaufen.

---

# 4. Entscheidung

NovaOS implementiert einen eigenständigen **Memory Hotplug Manager (MHM)**.

Dabei gilt:

* Speicher kann online hinzugefügt werden.
* Speicher kann kontrolliert entfernt werden.
* Alle Vorgänge erfolgen transparent.
* Anwendungen müssen nicht angepasst werden.

---

# 5. Motivation

Diese Architektur bietet:

* dynamische Speichererweiterung
* Hardwarewartung ohne Neustart
* Unterstützung virtueller Maschinen
* bessere Hochverfügbarkeit
* Energieeinsparung
* Zukunftssicherheit

---

# 6. Designprinzipien

Memory Hotplug basiert auf:

* Transparenz
* Modularität
* Ereignisorientierung
* Hardwareabstraktion
* Datenmigration
* deterministischen Zustandsübergängen

---

# 7. Architekturübersicht

```text
          Hardware / Hypervisor

                   │

          Hotplug-Ereignis

                   │

                   ▼

        Memory Hotplug Manager

         │                 │

         ▼                 ▼

   Physical Memory     NUMA Manager

         │                 │

         └────────┬────────┘

                  ▼

         Page Allocator

                  ▼

      Virtual Memory Manager

                  ▼

            Anwendungen
```

---

# 8. Aufgaben des Memory Hotplug Managers

Der Memory Hotplug Manager übernimmt:

* Erkennen neuer Speicherbereiche
* Entfernen vorhandener Speicherbereiche
* Speicherinitialisierung
* Speicherfreigabe
* Migration belegter Seiten
* Ereignisverwaltung
* Statistik
* Diagnose

---

# 9. Speicherzustände

Jeder Speicherbereich besitzt einen definierten Zustand.

Mögliche Zustände:

* Detected
* Initializing
* Online
* Offline Pending
* Migrating
* Offline
* Removed
* Failed

---

# 10. Hot-Add

Beim Hinzufügen neuen Speichers erfolgt:

* Erkennung durch die HAL
* Validierung
* Initialisierung
* Integration in den Physical Memory Manager
* Registrierung beim Page Allocator

Das Hinzufügen erfolgt in zwei logischen Phasen: Einbinden des neuen Speichers in das Betriebssystem und anschließendes Freigeben für den Page Allocator.

---

# 11. Hot-Remove

Beim Entfernen eines Speicherbereichs erfolgt:

* Sperren neuer Allokationen
* Migration belegter Seiten
* Aktualisierung der Seitentabellen
* Entfernen aus dem Page Allocator
* Freigabe des Speicherbereichs

---

# 12. Datenmigration

Vor dem Entfernen eines Speicherbereichs müssen sämtliche belegten Seiten migriert werden.

Dies umfasst:

* anonyme Seiten
* Dateicache
* Shared Memory
* Kernelobjekte
* Huge Pages

Nicht migrierbare Seiten verhindern das Offlining.

---

# 13. Zusammenarbeit mit dem Page Allocator

Der Page Allocator erhält Ereignisse über:

* neuen Speicher
* entfernten Speicher
* geänderte Speicherzonen
* freie Seiten

Dadurch kann er seine Freilisten dynamisch aktualisieren.

---

# 14. Virtual Memory Manager

Der Virtual Memory Manager bleibt vollständig transparent.

Virtuelle Adressen ändern sich durch Memory Hotplug niemals.

Nur die physische Zuordnung wird angepasst.

---

# 15. NUMA-Unterstützung

Memory Hotplug unterstützt:

* neue NUMA-Knoten
* Entfernen von NUMA-Knoten
* Lastverteilung
* lokale Speicherzuweisung

---

# 16. Speicherzonen

Neue Speicherbereiche werden automatisch den passenden Speicherzonen zugeordnet.

Beispiele:

* DMA
* Normal
* High Memory
* Movable
* Persistent Memory

---

# 17. Huge Pages

Huge Pages werden unterstützt.

Vor dem Offlining können sie:

* migriert,
* aufgeteilt,
* oder neu aufgebaut werden.

---

# 18. Memory Compression

Komprimierte Seiten können vor einer Migration

* dekomprimiert,
* verschoben,
* anschließend erneut komprimiert werden.

---

# 19. Memory Reclamation

Falls eine Migration scheitert,

kann Memory Reclamation

* Seiten freigeben,
* Cache bereinigen,
* oder Speicher komprimieren,

um Platz für die Migration zu schaffen.

---

# 20. Ereignismodell

Der Kernel erzeugt definierte Ereignisse:

* Memory Added
* Memory Online
* Memory Going Offline
* Memory Offline
* Memory Removed
* Memory Migration Failed

Dadurch können andere Kernelkomponenten auf Speicheränderungen reagieren. Moderne Kernel stellen Benachrichtigungsmechanismen bereit, um Subsysteme über Online- und Offline-Vorgänge zu informieren.

---

# 21. Mehrkernbetrieb

Memory Hotplug unterstützt:

* SMP
* CPU Hotplug
* parallele Migration
* atomare Statusänderungen
* TLB-Synchronisation

---

# 22. Sicherheit

Memory Hotplug schützt vor:

* Datenverlust
* unvollständiger Migration
* inkonsistenten Seitentabellen
* doppelter Freigabe
* ungültigen Speicherbereichen

---

# 23. Performance

Die Architektur soll ermöglichen:

* minimale Unterbrechung laufender Anwendungen
* schnelle Speicherintegration
* effiziente Seitenmigration
* geringe Verwaltungsaufwände
* hohe Skalierbarkeit

---

# 24. Debug-Unterstützung

Optional unterstützt NovaOS:

* Hotplug-Protokolle
* Migrationsstatistiken
* Ereignisprotokolle
* NUMA-Analysen
* Speicherkarten
* Fehlerdiagnosen

---

# 25. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* CXL Memory
* Persistent Memory
* Remote Memory
* Tiered Memory
* KI-gestützte Migrationsplanung
* Hardwarebeschleunigte Speicherkopien

---

# 26. Auswirkungen

Positive Auswirkungen:

* Speichererweiterung ohne Neustart
* Wartung im laufenden Betrieb
* bessere Virtualisierung
* höhere Verfügbarkeit
* moderne Serverarchitektur
* flexible Ressourcennutzung

Negative Auswirkungen:

* komplexere Speicherverwaltung
* zusätzlicher Migrationsaufwand
* erhöhter Implementierungsaufwand

---

# 27. Verworfene Alternativen

## Neustart bei jeder Speicheränderung

Vorteile:

* einfache Implementierung

Nachteile:

* lange Ausfallzeiten
* ungeeignet für Hochverfügbarkeit

---

## Ausschließlich Hot-Add

Vorteile:

* geringere Komplexität

Nachteile:

* Speicher kann nicht entfernt werden
* eingeschränkte Flexibilität

---

## Direkte Speicherentfernung ohne Migration

Vorteile:

* einfache Umsetzung

Nachteile:

* Datenverlust
* Instabilität
* ungeeignet für produktive Systeme

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Speicheränderungen ausschließlich über den Memory Hotplug Manager durchführen,
* keine direkten Änderungen am Physical Memory Manager vornehmen,
* Seiten vor dem Offlining vollständig migrieren,
* Statusübergänge atomar durchführen,
* Ereignisse an registrierte Subsysteme weiterleiten.

---

# 29. Teststrategie

Memory Hotplug wird geprüft durch:

* Unit-Tests
* Hot-Add-Tests
* Hot-Remove-Tests
* NUMA-Tests
* Mehrkerntests
* Migrationstests
* Langzeittests
* Performancebenchmarks

---

# 30. Konsequenzen

Memory Hotplug wird Bestandteil der dynamischen Speicherverwaltung von NovaOS.

Das Betriebssystem kann dadurch Arbeitsspeicher während des laufenden Betriebs hinzufügen oder entfernen, ohne Anwendungen zu unterbrechen. Diese Fähigkeit bildet die Grundlage für hochverfügbare Server, Cloud-Infrastrukturen, Virtualisierung, zukünftige CXL-Systeme und modulare Speicherarchitekturen.

---

# 31. Zukunftsperspektive

Langfristig soll der Memory Hotplug Manager erweitert werden um:

* automatische Speichererweiterung abhängig von der Systemlast
* KI-gestützte Vorhersage zukünftigen Speicherbedarfs
* Integration verteilter Speicherknoten
* automatische Lastverteilung zwischen NUMA-Domänen
* Hotplug für persistenten Speicher
* Energieoptimierung durch selektives Memory-Offlining

---

# 32. Zusammenfassung

NovaOS implementiert **Memory Hotplug** als vollständig integrierten Mechanismus zur dynamischen Verwaltung physischen Arbeitsspeichers. Neue Speicherbereiche können während des laufenden Betriebs eingebunden und bestehende Bereiche nach erfolgreicher Migration sicher entfernt werden. Durch die enge Integration mit dem Physical Memory Manager, Virtual Memory Manager, NUMA-Manager und Page Allocator entsteht eine hochverfügbare, skalierbare und zukunftssichere Speicherarchitektur für moderne Server-, Cloud- und Virtualisierungssysteme. Das Architekturmodell orientiert sich an einem zweistufigen Online-/Offline-Prozess mit kontrollierter Seitenmigration und ereignisgesteuerter Integration.
