# ADR-MM-0009: Memory Compression

* **ADR-ID:** ADR-MM-0009
* **Titel:** Memory Compression
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Memory Compression Manager, Page Allocator, Swap Manager, Scheduler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-MM-0006, ADR-MM-0007, ADR-KERNEL-0009

---

# 1. Zusammenfassung

NovaOS implementiert **Memory Compression** als transparente Erweiterung der virtuellen Speicherverwaltung.

Selten genutzte Speicherseiten können verlustfrei komprimiert werden, anstatt sie sofort auf ein langsameres Speichermedium auszulagern. Dadurch bleibt mehr effektiver Arbeitsspeicher verfügbar, während gleichzeitig die Anzahl der Swap-Zugriffe deutlich reduziert wird. Moderne Betriebssysteme setzen komprimierte Speicherpools ein, um CPU-Zeit gegen geringere Swap-I/O und eine bessere Gesamtsystemleistung zu tauschen.

---

# 2. Kontext

Nicht alle Speicherseiten werden permanent benötigt.

Typische Beispiele:

* Hintergrundprozesse
* Browser-Tabs
* Office-Anwendungen
* KI-Modelle
* Caches
* Compiler
* Virtuelle Maschinen
* Multimedia-Anwendungen

Diese Seiten belegen häufig RAM, obwohl sie nur selten verwendet werden.

---

# 3. Problemstellung

Es musste entschieden werden,

* welche Seiten komprimiert werden,
* wann eine Komprimierung erfolgt,
* welche Algorithmen verwendet werden,
* wie Dekomprimierung funktioniert,
* und wie sich die Architektur in den Virtual Memory Manager integriert.

---

# 4. Entscheidung

NovaOS führt einen eigenständigen **Memory Compression Manager (MCM)** ein.

Dabei gilt:

* Komprimierung erfolgt vollständig transparent.
* Anwendungen bemerken den Vorgang nicht.
* Nur selten genutzte Seiten werden berücksichtigt.
* Die Komprimierung erfolgt verlustfrei.
* Komprimierte Seiten verbleiben im Hauptspeicher.

---

# 5. Motivation

Diese Architektur bietet:

* geringeren RAM-Verbrauch
* weniger Swap-Zugriffe
* bessere Systemreaktion
* geringeren SSD-Verschleiß
* bessere Ausnutzung des Arbeitsspeichers
* hohe Skalierbarkeit

---

# 6. Designprinzipien

Memory Compression basiert auf:

* Transparenz
* Verlustfreiheit
* Lazy Compression
* Objektorientierung
* Hardwareunabhängigkeit
* Modularität

---

# 7. Architekturübersicht

```text
               Anwendung
                   │
                   ▼
          Virtual Memory Manager
                   │
          Seitenklassifizierung
                   │
                   ▼
      Memory Compression Manager
         │                    │
         ▼                    ▼
 Komprimierter Pool      Swap Manager
         │                    │
         └──────────┬─────────┘
                    ▼
             Physischer Speicher
```

---

# 8. Memory Compression Manager

Der Memory Compression Manager übernimmt:

* Auswahl geeigneter Seiten
* Komprimierung
* Dekomprimierung
* Verwaltung komprimierter Pools
* Referenzverwaltung
* Statistiken

---

# 9. Komprimierbare Seiten

Grundsätzlich können komprimiert werden:

* anonyme Seiten
* Heap
* Stack
* Dateicache
* Shared Memory
* Copy-on-Write-Seiten

Nicht komprimiert werden standardmäßig:

* Kernelcode
* MMIO
* Geräteabbildungen
* Echtzeitbereiche
* sicherheitskritische Kernelstrukturen

---

# 10. Auswahlstrategie

Der Memory Manager bewertet Seiten anhand verschiedener Kriterien.

Beispiele:

* Zugriffsfrequenz
* Alter
* Dirty-Status
* Referenzbits
* Priorität
* Prozessklasse

---

# 11. Komprimierung

Eine Seite wird nur komprimiert,

wenn

* ausreichend CPU-Ressourcen vorhanden sind,
* eine sinnvolle Kompressionsrate erwartet wird,
* der Aufwand wirtschaftlich ist.

---

# 12. Dekomprimierung

Beim erneuten Zugriff:

* tritt ein Page Fault auf,
* die Seite wird dekomprimiert,
* die Seitentabelle aktualisiert,
* die Programmausführung fortgesetzt.

---

# 13. Komprimierte Speicherpools

NovaOS verwaltet komprimierte Seiten in separaten Speicherpools.

Jeder Pool besitzt:

* maximale Größe
* aktuelle Belegung
* Statistikdaten
* verwendeten Kompressor
* Referenzinformationen

---

# 14. Kompressionsalgorithmen

Die Architektur definiert lediglich eine Kompressor-Schnittstelle.

Mögliche Implementierungen:

* LZ4
* Zstd
* LZO
* zukünftige Hardwarekompressoren

Der verwendete Algorithmus kann je nach Workload oder Plattform variieren.

---

# 15. Adaptive Kompression

NovaOS kann dynamisch entscheiden,

* welche Seiten komprimiert werden,
* welcher Algorithmus verwendet wird,
* wann eine Seite wieder dekomprimiert wird.

---

# 16. Swap-Integration

Memory Compression ersetzt Swap nicht.

Stattdessen entsteht eine zusätzliche Speicherebene:

```text
RAM

↓

Komprimierter RAM

↓

Swap

↓

Massenspeicher
```

Komprimierte Speicherpools dienen als Zwischenschicht vor dem eigentlichen Swap und können viele Schreibzugriffe auf das Speichermedium vermeiden.

---

# 17. Virtual Memory Regions

Komprimierte Seiten bleiben Bestandteil ihrer ursprünglichen Virtual Memory Region.

Regionen ändern sich durch die Komprimierung nicht.

---

# 18. Copy-on-Write

Copy-on-Write wird vollständig unterstützt.

Beim Schreiben:

* Seite dekomprimieren
* private Kopie erzeugen
* Referenzen aktualisieren

---

# 19. Huge Pages

Huge Pages können unterstützt werden.

Je nach Architektur können Huge Pages

* vollständig,
* teilweise,
* oder nach einer Demotion komprimiert werden.

---

# 20. Mehrkernbetrieb

Memory Compression unterstützt:

* SMP
* CPU Hotplug
* atomare Referenzzählung
* parallele Komprimierung
* parallele Dekomprimierung

---

# 21. NUMA

Bei NUMA-Systemen werden komprimierte Seiten bevorzugt

* lokal,
* CPU-nah,
* NUMA-optimiert

gespeichert.

---

# 22. Sicherheit

Memory Compression schützt vor:

* Datenverlust
* fehlerhaften Dekomprimierungen
* beschädigten Metadaten
* inkonsistenten Referenzen
* Speicherkorruption

Die Kompression erfolgt ausschließlich verlustfrei.

---

# 23. Performance

Die Architektur soll ermöglichen:

* weniger Swap-I/O
* höhere RAM-Ausnutzung
* geringere Speicherkosten
* kurze Dekomprimierungszeiten
* hohe Skalierbarkeit

Der Ansatz tauscht CPU-Zeit gegen eine Reduzierung von Speicherzugriffen auf langsamere Medien und verbessert dadurch häufig die Systemreaktionszeit.

---

# 24. Debug-Unterstützung

Optional unterstützt NovaOS:

* Kompressionsstatistiken
* Pool-Auslastung
* Dekomprimierungsstatistiken
* Kompressionsraten
* Algorithmusstatistiken
* Performanceanalysen

---

# 25. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Hardware-Kompression
* KI-gestützte Seitenauswahl
* adaptive Kompressionsprofile
* GPU-Kompression
* CXL Memory
* Persistent Memory

---

# 26. Auswirkungen

Positive Auswirkungen:

* geringerer Speicherverbrauch
* weniger Swap-Zugriffe
* bessere Reaktionszeit
* geringerer SSD-Verschleiß
* höhere Skalierbarkeit
* moderne Speicherarchitektur

Negative Auswirkungen:

* zusätzlicher CPU-Aufwand
* komplexere Speicherverwaltung
* zusätzlicher Verwaltungsaufwand

---

# 27. Verworfene Alternativen

## Ausschließlich klassischer Swap

Vorteile:

* einfache Implementierung

Nachteile:

* hohe I/O-Latenzen
* langsame Reaktion
* stärkerer Massenspeicherzugriff

---

## Vollständiger Verzicht auf Kompression

Vorteile:

* geringer CPU-Aufwand

Nachteile:

* geringere effektive RAM-Kapazität
* frühzeitiges Swapping

---

## Komprimierung aller Seiten

Vorteile:

* maximaler Speichergewinn

Nachteile:

* unnötige CPU-Belastung
* erhöhte Zugriffszeiten
* ineffizient für häufig genutzte Seiten

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Memory Compression ausschließlich über den Memory Compression Manager verwenden,
* keine Seiten selbst komprimieren,
* ausschließlich verlustfreie Kompression einsetzen,
* Virtual Memory Regions unverändert lassen,
* sämtliche Metadaten atomar aktualisieren.

---

# 29. Teststrategie

Memory Compression wird geprüft durch:

* Unit-Tests
* Kompressionsbenchmarks
* Dekomprimierungstests
* Mehrkerntests
* NUMA-Tests
* Langzeittests
* Stabilitätstests
* Performancebenchmarks

---

# 30. Konsequenzen

Memory Compression wird Bestandteil der mehrstufigen Speicherhierarchie von NovaOS.

Zwischen physischem Arbeitsspeicher und klassischem Swap entsteht eine zusätzliche, transparente Kompressionsebene. Dadurch können deutlich mehr Daten im Hauptspeicher gehalten werden, ohne dass Anwendungen angepasst werden müssen. Die Architektur bleibt vollständig kompatibel mit Virtual Memory Regions, Huge Pages, Copy-on-Write und dem bestehenden Virtual Memory Manager.

---

# 31. Zukunftsperspektive

Langfristig soll der Memory Compression Manager um intelligente Optimierungsmechanismen erweitert werden.

Geplante Erweiterungen:

* KI-gestützte Vorhersage häufig benötigter Seiten
* adaptive Auswahl des Kompressionsalgorithmus
* pro Prozess konfigurierbare Kompressionsrichtlinien
* hardwarebeschleunigte Kompression
* Integration mit Tiered Memory
* automatische Optimierung anhand der Systemlast

---

# 32. Zusammenfassung

NovaOS implementiert **Memory Compression** als transparente Erweiterung der virtuellen Speicherverwaltung. Selten genutzte Seiten werden verlustfrei komprimiert und in einem dedizierten Speicherpool gehalten, bevor eine Auslagerung auf ein langsameres Speichermedium erforderlich wird. Durch die Integration mit Virtual Memory Regions, Copy-on-Write, Huge Pages und dem Virtual Memory Manager entsteht eine moderne, skalierbare und ressourceneffiziente Speicherarchitektur, die Speicherplatz optimal nutzt und die Anzahl von Swap-Zugriffen reduziert.
