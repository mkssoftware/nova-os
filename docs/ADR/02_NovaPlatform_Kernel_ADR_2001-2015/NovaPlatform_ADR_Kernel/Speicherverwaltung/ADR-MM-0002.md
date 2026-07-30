# ADR-MM-0002: Slab Allocator

* **ADR-ID:** ADR-MM-0002
* **Titel:** Slab Allocator
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Kernel Heap, Object Manager, Process Manager, VFS, Netzwerk, Gerätetreiber, IPC, Scheduler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-KERNEL-0011, ADR-KERNEL-0012, ADR-KERNEL-0018

---

# 1. Zusammenfassung

NovaOS verwendet einen **objektorientierten Slab Allocator** zur effizienten Verwaltung häufig verwendeter Kernelobjekte.

Während der Page Allocator physische Speicherseiten verwaltet, dient der Slab Allocator der schnellen Allokation kleiner Kernelobjekte mit fester Größe. Objekte werden in typisierten Caches organisiert und nach der Freigabe wiederverwendet, wodurch Speicherfragmentierung reduziert und wiederholte Initialisierungskosten vermieden werden. Slab-Allocator arbeiten mit objektbezogenen Caches und beschleunigen häufige Allokationen durch Wiederverwendung bereits initialisierter Objekte.

---

# 2. Kontext

Der Kernel erzeugt ständig kleine Objekte.

Beispiele:

* Prozesse
* Threads
* Handles
* Capabilities
* Timer
* Ereignisse
* IPC-Nachrichten
* Dateideskriptoren
* VFS-Inodes
* Dentries
* Netzwerkpuffer

Eine direkte Seitenallokation wäre hierfür ineffizient.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie kleine Kernelobjekte effizient verwaltet werden,
* wie Fragmentierung reduziert wird,
* wie Objektinitialisierung beschleunigt wird,
* wie Mehrkernsysteme unterstützt werden,
* und wie Objektcaches organisiert werden.

---

# 4. Entscheidung

NovaOS implementiert einen typisierten Slab Allocator.

Dabei gilt:

* jeder Objekttyp besitzt einen eigenen Cache,
* Slabs bestehen aus einer oder mehreren Seiten des Page Allocators,
* Objekte werden wiederverwendet,
* Konstruktoren und Destruktoren werden unterstützt,
* CPU-lokale Caches minimieren Synchronisation.

---

# 5. Motivation

Diese Architektur bietet:

* extrem schnelle Objektallokationen
* geringe Fragmentierung
* Wiederverwendung initialisierter Objekte
* hohe Mehrkernskalierung
* bessere Cache-Lokalität
* deterministische Laufzeiten

---

# 6. Designprinzipien

Der Slab Allocator basiert auf:

* objektorientierter Speicherverwaltung
* Wiederverwendung
* Typisierung
* Cache-Lokalität
* Modularität
* Skalierbarkeit

---

# 7. Architekturübersicht

```text
            Page Allocator

                  │

                  ▼

          Slab Allocator

                  │

      +-----------+-----------+

      ▼                       ▼

 Objektcache A          Objektcache B

      ▼                       ▼

 [Obj][Obj][Obj]       [Obj][Obj][Obj]

      ▼                       ▼

      Kernelobjekte verschiedener Typen
```

---

# 8. Objektcaches

Jeder Kernelobjekttyp besitzt einen eigenen Cache.

Beispiele:

* Process Cache
* Thread Cache
* Handle Cache
* Capability Cache
* Event Cache
* Timer Cache
* Mutex Cache
* Socket Cache
* VFS Cache

Jeder Cache verwaltet ausschließlich Objekte identischer Größe.

---

# 9. Slabs

Ein Slab besteht aus einer oder mehreren zusammenhängenden Speicherseiten.

Ein Slab enthält:

* Verwaltungsinformationen
* Bitmap oder Freiliste
* Objektspeicher

Mehrere Slabs bilden gemeinsam einen Objektcache. Ein Cache verwaltet mehrere Slabs, die wiederum in viele gleich große Objekte unterteilt sind.

---

# 10. Objektzustände

Objekte befinden sich in genau einem Zustand:

* Free
* Allocated
* Partial
* Full
* Destroying

---

# 11. Slab-Zustände

Ein Slab besitzt einen der folgenden Zustände:

* Empty
* Partial
* Full

Dadurch können freie Objekte schnell gefunden werden.

---

# 12. Speicherallokation

Bei einer Allokation erfolgt:

1. CPU-lokalen Cache prüfen
2. Partial Slab auswählen
3. freies Objekt entnehmen
4. Objekt markieren
5. Objekt zurückgeben

Falls kein freies Objekt vorhanden ist, wird ein neuer Slab vom Page Allocator angefordert.

---

# 13. Speicherfreigabe

Beim Freigeben:

* Objekt validieren
* Freiliste aktualisieren
* Slab-Status prüfen
* leere Slabs gegebenenfalls freigeben

---

# 14. Konstruktoren

Ein Objektcache kann einen Konstruktor besitzen.

Dieser wird einmalig beim Erzeugen eines neuen Objekts ausgeführt.

Beispiele:

* Initialisierung von Listen
* Spinlocks
* Referenzzählern
* Standardwerten

---

# 15. Destruktoren

Optional kann ein Destruktor definiert werden.

Dieser dient beispielsweise zum:

* Freigeben interner Ressourcen
* Zurücksetzen temporärer Zustände
* Validieren des Objekts

---

# 16. Objektwiederverwendung

Freigegebene Objekte verbleiben zunächst im Cache.

Dadurch entfallen:

* erneute Speicherreservierung
* vollständige Neuinitialisierung
* unnötige Seitenallokationen

Das Wiederverwenden bereits initialisierter Objekte ist einer der Hauptvorteile des Slab-Konzepts.

---

# 17. CPU-lokale Caches

Jeder Prozessor besitzt lokale Objektcaches.

Vorteile:

* weniger globale Sperren
* bessere Cache-Lokalität
* höhere Parallelität
* geringere Latenzen

---

# 18. Alignment

Objekte werden hardwaregerecht ausgerichtet.

Unterstützt werden unter anderem:

* Wortgrenzen
* Cache-Line-Alignment
* SIMD-Alignment
* Architekturabhängige Spezialanforderungen

---

# 19. Cache Coloring

NovaOS unterstützt optional Cache Coloring.

Dadurch können:

* Cache-Konflikte reduziert,
* Speicherzugriffe gleichmäßiger verteilt,
* Cache-Hits erhöht

werden. Objekt-Coloring kann die Ausnutzung von CPU-Caches verbessern.

---

# 20. Fragmentierungsmanagement

Der Slab Allocator reduziert Fragmentierung durch:

* feste Objektgrößen
* typisierte Caches
* Wiederverwendung
* Freigabe leerer Slabs

---

# 21. Synchronisation

Der Slab Allocator verwendet:

* Spinlocks
* atomare Operationen
* CPU-lokale Freilisten
* lockarme Datenstrukturen

Globale Sperren sollen möglichst vermieden werden.

---

# 22. Sicherheit

Der Allocator schützt vor:

* Double Free
* Use-after-Free
* Speicherkorruption
* ungültigen Objekten
* Cache-Beschädigungen

Optional können Debug-Modi Objekte beim Freigeben mit bekannten Mustern überschreiben.

---

# 23. Performance

Der Slab Allocator soll:

* O(1)-Allokationen
* O(1)-Freigaben
* minimale Sperrzeiten
* hohe Mehrkernskalierung
* geringe Speicherfragmentierung

ermöglichen.

---

# 24. Debug-Unterstützung

Optional unterstützt NovaOS:

* Objektstatistiken
* Cache-Dumps
* Leak-Erkennung
* Double-Free-Erkennung
* Poisoning
* Red Zones
* Objektverfolgung

Viele Slab-Implementierungen bieten Debugfunktionen wie Poisoning oder Red-Zones zur Erkennung von Speicherfehlern.

---

# 25. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* NUMA-spezifische Objektcaches
* Memory Hotplug
* Security Tags
* Hardware-Memory-Tagging
* Persistente Objektcaches
* adaptive Cachegrößen

---

# 26. Auswirkungen

Positive Auswirkungen:

* sehr schnelle Objektallokation
* geringe Fragmentierung
* hohe Cache-Effizienz
* geringe Initialisierungskosten
* hervorragende Mehrkernskalierung
* bewährte Architektur

Negative Auswirkungen:

* zusätzlicher Verwaltungsaufwand
* höherer Metadatenspeicher
* ungenutzte Objekte können Speicher belegen

---

# 27. Verworfene Alternativen

## Direkte Page Allocation

Vorteile:

* einfache Architektur

Nachteile:

* enorme Speicherverschwendung
* hohe Allokationskosten
* schlechte Cache-Lokalität

---

## Allgemeiner Heap Allocator

Vorteile:

* flexibel

Nachteile:

* höhere Fragmentierung
* langsamere Allokationen
* keine Objektwiederverwendung

---

## Pool Allocator ohne Typisierung

Vorteile:

* einfache Implementierung

Nachteile:

* schlechtere Speicherlokalität
* geringere Wiederverwendbarkeit
* schwierigeres Debugging

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Kernelobjekte ausschließlich über den Slab Allocator allokieren,
* für jeden Objekttyp einen eigenen Cache verwenden,
* Konstruktoren für dauerhafte Initialisierung nutzen,
* CPU-lokale Caches bevorzugen,
* Objekte ausschließlich an ihren ursprünglichen Cache zurückgeben.

---

# 29. Teststrategie

Der Slab Allocator wird geprüft durch:

* Unit-Tests
* Fragmentierungstests
* Mehrkerntests
* Leak-Tests
* Stresstests
* Langzeittests
* Performancebenchmarks
* Debugmodus-Tests

---

# 30. Konsequenzen

Der Slab Allocator bildet die zentrale Infrastruktur für sämtliche kleinen Kernelobjekte innerhalb von NovaOS.

Alle zukünftigen Kernelkomponenten – insbesondere Object Manager, Process Manager, Scheduler, VFS, Netzwerkstack und IPC – müssen ihre Objekte über typisierte Slab-Caches verwalten. Dadurch entsteht eine hochperformante, skalierbare und wartbare Objektverwaltung, die optimal mit dem Page Allocator zusammenarbeitet.

---

# 31. Zusammenfassung

NovaOS implementiert einen **objektorientierten Slab Allocator**, der kleine Kernelobjekte über typisierte Caches verwaltet. Durch die Wiederverwendung bereits initialisierter Objekte, CPU-lokale Caches und eine enge Integration mit dem Page Allocator werden sehr schnelle Allokationen, geringe Fragmentierung und eine hohe Skalierbarkeit erreicht. Der Slab Allocator bildet damit die Grundlage der effizienten Objektverwaltung innerhalb der gesamten Kernelarchitektur.
