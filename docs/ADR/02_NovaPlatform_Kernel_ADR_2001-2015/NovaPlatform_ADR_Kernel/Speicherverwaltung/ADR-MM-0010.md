# ADR-MM-0010: Memory Reclamation

* **ADR-ID:** ADR-MM-0010
* **Titel:** Memory Reclamation
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Page Allocator, Memory Compression Manager, Swap Manager, Scheduler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-MM-0007, ADR-MM-0008, ADR-MM-0009

---

# 1. Zusammenfassung

NovaOS implementiert **Memory Reclamation** als zentralen Mechanismus zur Rückgewinnung physischer Speicherseiten unter Speicherdruck.

Der Memory-Reclamation-Manager analysiert kontinuierlich die Speichernutzung und entscheidet, welche Seiten freigegeben, komprimiert, ausgelagert oder verworfen werden können. Ziel ist es, freien Arbeitsspeicher bereitzustellen, ohne die Systemleistung unnötig zu beeinträchtigen. Reclaim bezeichnet den Prozess, wiederverwendbare Speicherseiten freizugeben oder auszulagern, wenn freier Arbeitsspeicher knapp wird.

---

# 2. Kontext

Während der Laufzeit konkurrieren viele Komponenten um physischen Speicher.

Beispiele:

* Prozesse
* Dateicache
* Shared Memory
* Memory Mappings
* KI-Modelle
* Grafiksystem
* Netzwerkpuffer
* Kernelobjekte

Der verfügbare Speicher muss dynamisch zwischen diesen Komponenten verteilt werden.

---

# 3. Problemstellung

Es musste entschieden werden,

* wann Speicher zurückgewonnen wird,
* welche Seiten bevorzugt entfernt werden,
* wie Kompression und Swap integriert werden,
* wie Hintergrund- und Sofort-Reclaim zusammenarbeiten,
* und wie Speicherknappheit erkannt wird.

---

# 4. Entscheidung

NovaOS führt einen eigenständigen **Memory Reclamation Manager (MRM)** ein.

Dabei gilt:

* Reclamation erfolgt automatisch.
* Hintergrund- und Direkt-Reclaim werden unterstützt.
* Die Auswahl basiert auf Seitenklassen und Zugriffsmustern.
* Anwendungen müssen nicht angepasst werden.

---

# 5. Motivation

Diese Architektur bietet:

* stabile Speicherverfügbarkeit
* geringere Out-of-Memory-Situationen
* bessere Reaktionszeiten
* effiziente RAM-Nutzung
* weniger Swap-I/O
* hohe Skalierbarkeit

---

# 6. Designprinzipien

Memory Reclamation basiert auf:

* Demand Reclaim
* Hintergrundoptimierung
* Priorisierung
* Modularität
* Hardwareunabhängigkeit
* Vorhersagbarkeit

---

# 7. Architekturübersicht

```text
          Freier Speicher
                 │
Speicher unterschreitet Schwellenwert
                 │
                 ▼
      Memory Reclamation Manager
        │        │         │
        ▼        ▼         ▼
   Page Cache  Compression  Swap
        │        │         │
        └────────┴─────────┘
                 |
                 ▼
          Freie Seitenliste
```

---

# 8. Aufgaben des Memory Reclamation Managers

Der MRM übernimmt:

* Überwachung freier Seiten
* Seitenklassifizierung
* Auswahl von Kandidaten
* Freigabe von Seiten
* Komprimierung
* Auslagerung
* Statistiken
* Hintergrundoptimierung

---

# 9. Reclaim-Kandidaten

Bevorzugt zurückgewonnen werden:

* Dateicache
* ungenutzte anonyme Seiten
* komprimierbare Seiten
* Shared-Memory-Seiten ohne Referenzen
* inaktive Cacheobjekte

Nicht zurückgewonnen werden:

* MMIO
* DMA-Puffer
* Kernelcode
* gesperrte Seiten
* Echtzeitbereiche

Reclaim-fähige Seiten umfassen insbesondere Page Cache und anonyme Seiten; dauerhaft gebundene Kernel- oder DMA-Seiten gelten dagegen typischerweise als nicht reclaimbar.

---

# 10. Speicherdruck

NovaOS definiert mehrere Speicherzustände:

* Normal
* Beobachtung
* Niedriger Speicher
* Kritischer Speicher
* Notfall

Je nach Zustand werden unterschiedliche Strategien aktiviert.

---

# 11. Hintergrund-Reclaim

Bereits vor kritischem Speichermangel arbeitet ein Hintergrunddienst.

Aufgaben:

* Freigabe kalter Seiten
* Komprimierung
* Bereinigung des Dateicaches
* Vorbereiten freier Seiten

Viele Betriebssysteme starten einen Hintergrund-Reclaim bereits beim Erreichen definierter Speichergrenzen, bevor Anwendungen blockieren müssen.

---

# 12. Direkt-Reclaim

Kann eine Speicheranforderung nicht sofort erfüllt werden,

wird Direct Reclaim gestartet.

Dabei:

* sucht der Kernel unmittelbar freigebbare Seiten,
* blockiert die anfordernde Operation nur so lange wie nötig,
* beendet den Vorgang nach erfolgreicher Speichergewinnung.

---

# 13. Seitenklassifizierung

Jede Seite besitzt Metadaten.

Beispiele:

* Alter
* Zugriffshäufigkeit
* Dirty-Bit
* Referenzbit
* Seitentyp
* Priorität

Diese Informationen bilden die Grundlage für Reclaim-Entscheidungen.

---

# 14. Auswahlstrategie

Der Memory Reclamation Manager bewertet Seiten nach:

* Aktivität
* Wiederverwendungswahrscheinlichkeit
* Größe
* Kosten einer Wiederherstellung
* Priorität

Die Architektur schreibt keinen einzelnen Algorithmus fest.

---

# 15. Page Cache

Saubere Dateicache-Seiten können unmittelbar entfernt werden.

Dirty-Seiten werden zuvor:

* geschrieben,
* synchronisiert,
* anschließend freigegeben.

Saubere Dateiseiten können meist direkt verworfen werden, während geänderte Seiten zunächst zurückgeschrieben werden müssen.

---

# 16. Anonymous Memory

Anonyme Seiten können:

* komprimiert,
* ausgelagert,
* oder freigegeben werden,

abhängig von ihrer Nutzung.

---

# 17. Memory Compression

Bevor Swap verwendet wird,

prüft NovaOS,

ob eine Komprimierung wirtschaftlicher ist.

Dadurch sinkt die Anzahl physischer Schreibzugriffe.

---

# 18. Swap

Erst wenn weder Freigabe noch Kompression ausreichend sind,

werden Seiten ausgelagert.

Swap bildet die letzte Stufe der Speicherhierarchie.

---

# 19. Virtual Memory Regions

Memory Reclamation arbeitet ausschließlich auf Seitenebene.

Virtual Memory Regions bleiben unverändert bestehen.

---

# 20. Huge Pages

Huge Pages werden unterstützt.

Vor einer Rückgewinnung können sie:

* demotiert,
* teilweise freigegeben,
* oder vollständig ausgelagert werden.

---

# 21. Mehrkernbetrieb

Memory Reclamation unterstützt:

* SMP
* CPU Hotplug
* parallele Reclaim-Threads
* atomare Referenzzählung
* NUMA

---

# 22. NUMA

Auf NUMA-Systemen erfolgt Reclaim vorzugsweise

* lokal,
* knotenspezifisch,
* CPU-nah.

---

# 23. Out-of-Memory-Vermeidung

Der Memory Reclamation Manager versucht,

Out-of-Memory-Situationen durch frühzeitiges Eingreifen zu verhindern.

Nur wenn sämtliche Maßnahmen fehlschlagen,

wird der OOM-Manager aktiviert.

---

# 24. Sicherheit

Memory Reclamation schützt vor:

* doppelter Freigabe
* beschädigten Seitentabellen
* Datenverlust
* inkonsistenten Referenzen
* Race Conditions

---

# 25. Performance

Die Architektur soll ermöglichen:

* geringe Reclaim-Latenzen
* wenige Swap-Zugriffe
* stabile Systemreaktion
* hohe RAM-Auslastung
* gute Mehrkernskalierung

---

# 26. Debug-Unterstützung

Optional unterstützt NovaOS:

* Reclaim-Statistiken
* Seitenhistorie
* Speicherdruckdiagramme
* Cacheanalysen
* Reclaim-Tracing
* Performancemetriken

---

# 27. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* KI-gestützte Reclaim-Strategien
* adaptive Speicherprofile
* CXL Memory
* Persistent Memory
* Tiered Memory
* pro Prozess konfigurierbare Richtlinien

---

# 28. Auswirkungen

Positive Auswirkungen:

* stabile Speicherverwaltung
* geringere Out-of-Memory-Ereignisse
* effizientere RAM-Nutzung
* weniger Swap-I/O
* bessere Gesamtleistung
* moderne Architektur

Negative Auswirkungen:

* komplexere Verwaltungslogik
* zusätzlicher CPU-Aufwand
* höherer Implementierungsaufwand

---

# 29. Verworfene Alternativen

## Ausschließlich Swap

Vorteile:

* einfache Implementierung

Nachteile:

* hohe Latenzen
* schlechte Reaktionszeit
* unnötige SSD-Zugriffe

---

## Sofortiges Freigeben aller Cache-Seiten

Vorteile:

* schneller Speichergewinn

Nachteile:

* starke Leistungseinbußen
* viele erneute Lesezugriffe

---

## Reclaim ausschließlich bei Speichermangel

Vorteile:

* geringe Hintergrundaktivität

Nachteile:

* hohe Latenzspitzen
* häufige Blockierungen

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Speicher ausschließlich über den Memory Reclamation Manager zurückgewinnen,
* Seiten niemals direkt freigeben,
* Kompression vor Swap bevorzugen,
* Reclaim-Entscheidungen nachvollziehbar protokollieren,
* Metadaten atomar aktualisieren.

---

# 31. Teststrategie

Memory Reclamation wird geprüft durch:

* Unit-Tests
* Hintergrund-Reclaim-Tests
* Direct-Reclaim-Tests
* NUMA-Tests
* Mehrkerntests
* Langzeittests
* Lasttests
* Performancebenchmarks

---

# 32. Konsequenzen

Memory Reclamation wird zum zentralen Mechanismus für die dynamische Rückgewinnung physischer Speicherseiten in NovaOS.

Durch die Kombination aus Hintergrund-Reclaim, Direct Reclaim, Memory Compression und Swap kann der Kernel flexibel auf unterschiedliche Speicherlasten reagieren. Dadurch entsteht eine robuste und skalierbare Speicherverwaltung, die den verfügbaren Arbeitsspeicher optimal nutzt und die Systemreaktionszeit auch unter hoher Last stabil hält.

---

# 33. Zukunftsperspektive

Langfristig soll der Memory Reclamation Manager erweitert werden um:

* KI-gestützte Vorhersage zukünftiger Speicherzugriffe
* adaptive Reclaim-Strategien je Workload
* selbstoptimierende Schwellenwerte
* Energieoptimierung für mobile Systeme
* prioritätsbasierte Speicherbudgets
* Telemetriegestützte Optimierung

---

# 34. Zusammenfassung

NovaOS implementiert **Memory Reclamation** als intelligente und mehrstufige Strategie zur Rückgewinnung physischer Speicherseiten. Durch Hintergrund- und Direkt-Reclaim, Seitenklassifizierung, Memory Compression und Swap entsteht eine moderne Speicherarchitektur, die Speicherknappheit frühzeitig erkennt, Ressourcen effizient verteilt und gleichzeitig hohe Systemleistung sowie Skalierbarkeit gewährleistet. Reclaim priorisiert freigebbare Seiten wie Page Cache oder auslagerbare anonyme Seiten und kombiniert asynchrone Hintergrundarbeit mit unmittelbaren Maßnahmen unter starkem Speicherdruck.
