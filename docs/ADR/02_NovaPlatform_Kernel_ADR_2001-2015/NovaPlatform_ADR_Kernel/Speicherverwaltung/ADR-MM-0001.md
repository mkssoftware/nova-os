# ADR-MM-0001: Page Allocator

* **ADR-ID:** ADR-MM-0001
* **Titel:** Page Allocator
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Physical Memory Manager (PMM), Virtual Memory Manager (VMM), Kernel Heap, Slab Allocator, DMA, NUMA
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0008, ADR-KERNEL-0009, ADR-KERNEL-0018, ADR-MM-0002

---

# 1. Zusammenfassung

NovaOS verwendet einen **Buddy-basierten Page Allocator** als zentrale Komponente zur Verwaltung physischer Speicherseiten.

Der Page Allocator stellt physische Speicherseiten in unterschiedlichen Größenordnungen (*Orders*) bereit und dient als Grundlage für sämtliche höheren Speicherverwaltungsschichten wie Kernel Heap, Slab Allocator, Virtual Memory Manager und DMA. Durch das Buddy-Verfahren werden Speicherblöcke effizient geteilt und beim Freigeben wieder zusammengeführt, wodurch externe Fragmentierung reduziert wird. Buddy-Allocator verwalten physischen Speicher in Blöcken mit Zweierpotenzen und verschmelzen benachbarte freie Blöcke automatisch wieder.

---

# 2. Kontext

Nahezu alle Kernelkomponenten benötigen physischen Speicher.

Beispiele:

* Kernel Heap
* Slab Allocator
* Seitentabellen
* Dateicache
* Netzwerkpuffer
* Gerätetreiber
* DMA
* Shared Memory

Alle diese Komponenten greifen auf den Page Allocator zurück.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie physische Seiten verwaltet werden,
* wie zusammenhängender Speicher bereitgestellt wird,
* wie Fragmentierung reduziert wird,
* wie Mehrkernsysteme unterstützt werden,
* und wie zukünftige Speichertechnologien integriert werden.

---

# 4. Entscheidung

NovaOS implementiert einen Buddy-basierten Page Allocator.

Dabei gilt:

* Verwaltung in Zweierpotenzen (*Orders*)
* automatische Blockteilung (*Split*)
* automatische Blockverschmelzung (*Merge*)
* CPU-lokale Optimierungen
* NUMA-Unterstützung
* lockarme Synchronisation

---

# 5. Motivation

Diese Architektur bietet:

* schnelle Allokationen
* schnelle Freigaben
* geringe Fragmentierung
* gute Mehrkernskalierung
* Unterstützung großer Speicherblöcke
* hohe Vorhersagbarkeit

---

# 6. Designprinzipien

Der Page Allocator basiert auf:

* Buddy-System
* deterministische Laufzeiten
* geringe Fragmentierung
* Skalierbarkeit
* Modularität
* Architekturunabhängigkeit

---

# 7. Architekturübersicht

```text
          Memory Map
               │
               ▼
    Physical Memory Manager
               │
               ▼
         Buddy Allocator
               │
    +----------+-----------+
    ▼                      ▼
Order Free Lists      Per-CPU Cache
    │                      │
    └----------+-----------┘
               ▼
      Kernel Subsysteme
```

---

# 8. Speicherorganisation

Der physische Speicher wird in Seiten organisiert.

Standardgröße:

* 4 KiB

Optional unterstützt NovaOS zusätzlich:

* 16 KiB
* 64 KiB

je nach Prozessorarchitektur.

---

# 9. Orders

Der Buddy Allocator verwaltet Speicherblöcke in Zweierpotenzen.

Beispiel:

| Order | Seiten |  Größe |
| ----: | -----: | -----: |
|     0 |      1 |  4 KiB |
|     1 |      2 |  8 KiB |
|     2 |      4 | 16 KiB |
|     3 |      8 | 32 KiB |
|     4 |     16 | 64 KiB |
|   ... |    ... |    ... |

---

# 10. Freie Listen

Für jede Order existiert eine eigene Freiliste.

Eigenschaften:

* doppelt verkettete Listen
* O(1)-Einfügen
* O(1)-Entfernen
* thread-sicher

---

# 11. Speicherallokation

Bei einer Allokation:

1. passende Order bestimmen
2. freie Liste durchsuchen
3. größeren Block wählen (falls nötig)
4. Block wiederholt teilen
5. gewünschte Order zurückgeben

Dieses rekursive Teilen großer Blöcke ist ein zentrales Merkmal des Buddy-Verfahrens.

---

# 12. Speicherfreigabe

Beim Freigeben:

* Buddy bestimmen
* Buddy prüfen
* beide Blöcke verschmelzen
* gegebenenfalls rekursiv wiederholen

Dadurch wird Fragmentierung automatisch reduziert.

---

# 13. Buddy-Berechnung

Der Buddy eines Speicherblocks wird ausschließlich anhand

* physischer Adresse
* Blockgröße

berechnet.

Es werden keine zusätzlichen Suchstrukturen benötigt.

---

# 14. Seitendeskriptoren

Jede physische Seite besitzt Metadaten.

Mindestens:

* Status
* Referenzzähler
* Order
* Flags
* NUMA-Knoten
* Zone
* Eigentümer

---

# 15. Speicherzonen

Der Allocator unterstützt getrennte Speicherzonen.

Beispiele:

* DMA
* DMA32
* Normal
* High Memory
* Device Memory
* Persistent Memory

Die Verwaltung getrennt nach Zonen erleichtert die Unterstützung unterschiedlicher Hardwareanforderungen.

---

# 16. Per-CPU Page Cache

Order-0-Seiten werden zusätzlich CPU-lokal zwischengespeichert.

Vorteile:

* weniger globale Sperren
* bessere Cache-Lokalität
* höhere Skalierung
* geringere Latenzen

Per-CPU-Seitenlisten reduzieren die Sperrkonkurrenz auf Mehrkernsystemen erheblich.

---

# 17. NUMA-Unterstützung

Bei NUMA-Systemen bevorzugt NovaOS:

* lokalen Speicher
* lokale Freilisten
* lokale CPU-Caches

Erst anschließend erfolgt eine entfernte Speicherallokation.

---

# 18. Große Speicherblöcke

Der Allocator unterstützt zusammenhängende Blöcke für:

* Huge Pages
* DMA
* Framebuffer
* Gerätetreiber
* Speicherabbilder

---

# 19. Fragmentierungsmanagement

Zur Verringerung externer Fragmentierung unterstützt NovaOS:

* Buddy-Merge
* bevorzugte lokale Allokation
* periodische Speicheranalyse
* Hintergrundkompaktierung (optional)

---

# 20. Synchronisation

Der Page Allocator verwendet:

* Spinlocks
* atomare Operationen
* CPU-lokale Freilisten
* lockarme Algorithmen

Globale Sperren sollen möglichst vermieden werden.

---

# 21. Sicherheit

Der Allocator schützt vor:

* Double Free
* ungültigen Seiten
* beschädigten Freilisten
* Speicherkorruption
* mehrfacher Allokation

---

# 22. Performance

Der Page Allocator soll:

* O(log n)-Split
* O(log n)-Merge
* sehr schnelle Order-0-Allokationen
* hohe Parallelität
* geringe Sperrzeiten

ermöglichen.

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* Speicherstatistiken
* Buddy-Dumps
* Fragmentierungsanalyse
* Double-Free-Erkennung
* Leak-Erkennung
* Seitenhistorie

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Memory Hotplug
* CXL Memory
* HBM
* Persistent Memory
* Tiered Memory
* Seitenfärbung (*Page Coloring*)

---

# 25. Auswirkungen

Positive Auswirkungen:

* geringe Fragmentierung
* schnelle Allokationen
* schnelle Freigaben
* hohe Skalierbarkeit
* Unterstützung großer Speicherbereiche
* bewährte Architektur

Negative Auswirkungen:

* interner Verschnitt bei Zweierpotenzen
* komplexere Verwaltungslogik
* zusätzlicher Metadatenspeicher

---

# 26. Verworfene Alternativen

## Bitmap Allocator

Vorteile:

* einfache Implementierung

Nachteile:

* langsame Suche
* schlechte Unterstützung großer zusammenhängender Bereiche

---

## Freiliste ohne Buddy-System

Vorteile:

* geringer Verwaltungsaufwand

Nachteile:

* starke Fragmentierung
* langsame Zusammenführung

---

## First-Fit Allocator

Vorteile:

* einfach

Nachteile:

* schlechte Langzeitfragmentierung
* geringe Vorhersagbarkeit

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* physische Seiten ausschließlich über den Page Allocator anfordern,
* Seiten ausschließlich über den Page Allocator freigeben,
* keine eigenen Seitenverwaltungen implementieren,
* CPU-lokale Caches bevorzugen,
* NUMA-Richtlinien beachten.

---

# 28. Teststrategie

Der Page Allocator wird geprüft durch:

* Unit-Tests
* Buddy-Merge-Tests
* Fragmentierungstests
* Mehrkerntests
* NUMA-Tests
* Langzeittests
* Lasttests
* Performancebenchmarks

---

# 29. Konsequenzen

Der Page Allocator bildet die zentrale Grundlage der physischen Speicherverwaltung von NovaOS.

Alle zukünftigen Speicherkomponenten – insbesondere der Slab Allocator, der Kernel Heap, der Virtual Memory Manager sowie Dateisystem- und Gerätetreiber – beziehen physische Seiten ausschließlich über diese Komponente. Dadurch entsteht eine einheitliche, skalierbare und langfristig wartbare Speicherarchitektur.

---

# 30. Zusammenfassung

NovaOS implementiert einen **Buddy-basierten Page Allocator** als primären Allokator für physischen Speicher. Durch die Verwaltung freier Speicherblöcke in Zweierpotenzen, das rekursive Teilen großer Blöcke und das automatische Verschmelzen benachbarter freier Bereiche werden schnelle Allokationen, geringe Fragmentierung und eine hohe Skalierbarkeit erreicht. CPU-lokale Seitenlisten, NUMA-Unterstützung und getrennte Speicherzonen bilden die Grundlage für eine moderne, leistungsfähige Speicherverwaltung.
